/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief Implementation for the O3ClassFactory class.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 * @author Lionel TORTI
 */

#include <agrum/PRM/o3prm/O3ClassFactory.h>

namespace gum {
  namespace prm {
    namespace o3prm {

      template < typename GUM_SCALAR >
      INLINE O3ClassFactory< GUM_SCALAR >::O3ClassFactory(PRM< GUM_SCALAR >&          prm,
                                                          O3PRM&                      o3_prm,
                                                          O3NameSolver< GUM_SCALAR >& solver,
                                                          ErrorsContainer&            errors) :
          _prm_(&prm),
          _o3_prm_(&o3_prm), _solver_(&solver), _errors_(&errors) {
        GUM_CONSTRUCTOR(O3ClassFactory);
      }

      template < typename GUM_SCALAR >
      INLINE O3ClassFactory< GUM_SCALAR >::O3ClassFactory(const O3ClassFactory< GUM_SCALAR >& src) :
          _prm_(src._prm_), _o3_prm_(src._o3_prm_), _solver_(src._solver_), _errors_(src._errors_),
          _nameMap_(src._nameMap_), _classMap_(src._classMap_), _nodeMap_(src._nodeMap_),
          _dag_(src._dag_), _o3Classes_(src._o3Classes_) {
        GUM_CONS_CPY(O3ClassFactory);
      }

      template < typename GUM_SCALAR >
      INLINE O3ClassFactory< GUM_SCALAR >::O3ClassFactory(O3ClassFactory< GUM_SCALAR >&& src) :
          _prm_(std::move(src._prm_)), _o3_prm_(std::move(src._o3_prm_)),
          _solver_(std::move(src._solver_)), _errors_(std::move(src._errors_)),
          _nameMap_(std::move(src._nameMap_)), _classMap_(std::move(src._classMap_)),
          _nodeMap_(std::move(src._nodeMap_)), _dag_(std::move(src._dag_)),
          _o3Classes_(std::move(src._o3Classes_)) {
        GUM_CONS_MOV(O3ClassFactory);
      }

      template < typename GUM_SCALAR >
      INLINE O3ClassFactory< GUM_SCALAR >::~O3ClassFactory() {
        GUM_DESTRUCTOR(O3ClassFactory);
      }

      template < typename GUM_SCALAR >
      INLINE O3ClassFactory< GUM_SCALAR >&
         O3ClassFactory< GUM_SCALAR >::operator=(const O3ClassFactory< GUM_SCALAR >& src) {
        if (this == &src) { return *this; }
        _prm_       = src._prm_;
        _o3_prm_    = src._o3_prm_;
        _solver_    = src._solver_;
        _errors_    = src._errors_;
        _nameMap_   = src._nameMap_;
        _classMap_  = src._classMap_;
        _nodeMap_   = src._nodeMap_;
        _dag_       = src._dag_;
        _o3Classes_ = src._o3Classes_;
        return *this;
      }

      template < typename GUM_SCALAR >
      INLINE O3ClassFactory< GUM_SCALAR >&
         O3ClassFactory< GUM_SCALAR >::operator=(O3ClassFactory< GUM_SCALAR >&& src) {
        if (this == &src) { return *this; }
        _prm_       = std::move(src._prm_);
        _o3_prm_    = std::move(src._o3_prm_);
        _solver_    = std::move(src._solver_);
        _errors_    = std::move(src._errors_);
        _nameMap_   = std::move(src._nameMap_);
        _classMap_  = std::move(src._classMap_);
        _nodeMap_   = std::move(src._nodeMap_);
        _dag_       = std::move(src._dag_);
        _o3Classes_ = std::move(src._o3Classes_);
        return *this;
      }

      template < typename GUM_SCALAR >
      INLINE void O3ClassFactory< GUM_SCALAR >::buildClasses() {
        PRMFactory< GUM_SCALAR > factory(_prm_);

        // Class with a super class must be declared after
        if (_checkO3Classes_()) {
          _setO3ClassCreationOrder_();

          for (auto c: _o3Classes_) {
            // Soving interfaces
            auto implements = Set< std::string >();
            for (auto& i: c->interfaces()) {
              if (_solver_->resolveInterface(i)) { implements.insert(i.label()); }
            }

            // Adding the class
            if (_solver_->resolveClass(c->superLabel())) {
              factory.startClass(c->name().label(), c->superLabel().label(), &implements, true);
              factory.endClass(false);
            }
          }
        }
      }

      template < typename GUM_SCALAR >
      INLINE void O3ClassFactory< GUM_SCALAR >::_setO3ClassCreationOrder_() {
        auto topo_order = _dag_.topologicalOrder();

        for (auto id = topo_order.rbegin(); id != topo_order.rend(); --id) {
          _o3Classes_.push_back(_nodeMap_[*id]);
        }
      }

      template < typename GUM_SCALAR >
      INLINE bool O3ClassFactory< GUM_SCALAR >::_checkO3Classes_() {
        return _checkAndAddNodesToDag_() && _checkAndAddArcsToDag_();
      }

      template < typename GUM_SCALAR >
      INLINE bool O3ClassFactory< GUM_SCALAR >::_checkAndAddNodesToDag_() {
        for (auto& c: _o3_prm_->classes()) {
          auto id = _dag_.addNode();

          try {
            _nameMap_.insert(c->name().label(), id);
            _classMap_.insert(c->name().label(), c.get());
            _nodeMap_.insert(id, c.get());

          } catch (DuplicateElement const&) {
            O3PRM_CLASS_DUPLICATE(c->name(), *_errors_);
            return false;
          }
        }

        return true;
      }

      template < typename GUM_SCALAR >
      INLINE bool O3ClassFactory< GUM_SCALAR >::_checkAndAddArcsToDag_() {
        for (auto& c: _o3_prm_->classes()) {
          if (c->superLabel().label() != "") {
            if (!_solver_->resolveClass(c->superLabel())) { return false; }

            auto head = _nameMap_[c->superLabel().label()];
            auto tail = _nameMap_[c->name().label()];

            try {
              _dag_.addArc(tail, head);
            } catch (InvalidDirectedCycle const&) {
              // Cyclic inheritance
              O3PRM_CLASS_CYLIC_INHERITANCE(c->name(), c->superLabel(), *_errors_);
              return false;
            }
          }
        }

        return true;
      }

      template < typename GUM_SCALAR >
      INLINE void O3ClassFactory< GUM_SCALAR >::buildImplementations() {
        for (auto& c: _o3_prm_->classes()) {
          if (_checkImplementation_(*c)) {
            _prm_->getClass(c->name().label()).initializeInheritance();
          }
        }
      }

      using AttrMap = HashTable< std::string, O3Attribute* >;
      using RefMap  = HashTable< std::string, O3ReferenceSlot* >;
      using AggMap  = HashTable< std::string, O3Aggregate* >;

      template < typename GUM_SCALAR >
      INLINE bool O3ClassFactory< GUM_SCALAR >::_checkImplementation_(O3Class& c) {
        // Saving attributes names for fast lookup
        auto attr_map = AttrMap();
        for (auto& a: c.attributes()) {
          attr_map.insert(a->name().label(), a.get());
        }

        // Saving aggregates names for fast lookup
        auto agg_map = AggMap();
        for (auto& agg: c.aggregates()) {
          agg_map.insert(agg.name().label(), &agg);
        }

        auto ref_map = RefMap();
        for (auto& ref: c.referenceSlots()) {
          ref_map.insert(ref.name().label(), &ref);
        }

        // Cheking interface implementation
        for (auto& i: c.interfaces()) {
          if (_solver_->resolveInterface(i)) {
            if (!_checkImplementation_(c, i, attr_map, agg_map, ref_map)) { return false; }
          }
        }

        return true;
      }

      template < typename GUM_SCALAR >
      INLINE bool O3ClassFactory< GUM_SCALAR >::_checkImplementation_(O3Class& c,
                                                                      O3Label& i,
                                                                      AttrMap& attr_map,
                                                                      AggMap&  agg_map,
                                                                      RefMap&  ref_map) {
        const auto& real_i = _prm_->getInterface(i.label());

        auto counter = (Size)0;
        for (const auto& a: real_i.attributes()) {
          if (attr_map.exists(a->name())) {
            ++counter;

            if (!_checkImplementation_(attr_map[a->name()]->type(), a->type())) {
              O3PRM_CLASS_ATTR_IMPLEMENTATION(c.name(), i, attr_map[a->name()]->name(), *_errors_);
              return false;
            }
          }

          if (agg_map.exists(a->name())) {
            ++counter;

            if (!_checkImplementation_(agg_map[a->name()]->variableType(), a->type())) {
              O3PRM_CLASS_AGG_IMPLEMENTATION(c.name(), i, agg_map[a->name()]->name(), *_errors_);
              return false;
            }
          }
        }

        if (counter != real_i.attributes().size()) {
          O3PRM_CLASS_MISSING_ATTRIBUTES(c.name(), i, *_errors_);
          return false;
        }

        counter = 0;
        for (const auto& r: real_i.referenceSlots()) {
          if (ref_map.exists(r->name())) {
            ++counter;

            if (!_checkImplementation_(ref_map[r->name()]->type(), r->slotType())) {
              O3PRM_CLASS_REF_IMPLEMENTATION(c.name(), i, ref_map[r->name()]->name(), *_errors_);
              return false;
            }
          }
        }
        return true;
      }

      template < typename GUM_SCALAR >
      INLINE bool O3ClassFactory< GUM_SCALAR >::_checkImplementation_(O3Label&       o3_type,
                                                                      const PRMType& type) {
        if (!_solver_->resolveType(o3_type)) { return false; }

        return _prm_->type(o3_type.label()).isSubTypeOf(type);
      }

      template < typename GUM_SCALAR >
      INLINE bool O3ClassFactory< GUM_SCALAR >::_checkImplementation_(
         O3Label&                                      o3_type,
         const PRMClassElementContainer< GUM_SCALAR >& type) {
        if (!_solver_->resolveSlotType(o3_type)) { return false; }

        if (_prm_->isInterface(o3_type.label())) {
          return _prm_->getInterface(o3_type.label()).isSubTypeOf(type);
        } else {
          return _prm_->getClass(o3_type.label()).isSubTypeOf(type);
        }
      }

      template < typename GUM_SCALAR >
      INLINE void O3ClassFactory< GUM_SCALAR >::buildParameters() {
        PRMFactory< GUM_SCALAR > factory(_prm_);
        // Class with a super class must be declared after
        for (auto c: _o3Classes_) {
          _prm_->getClass(c->name().label()).inheritParameters();

          factory.continueClass(c->name().label());

          _addParameters_(factory, *c);

          factory.endClass(false);
        }
      }

      template < typename GUM_SCALAR >
      INLINE void O3ClassFactory< GUM_SCALAR >::_addParameters_(PRMFactory< GUM_SCALAR >& factory,
                                                                O3Class&                  c) {
        for (auto& p: c.parameters()) {
          switch (p.type()) {
            case O3Parameter::PRMType::INT: {
              factory.addParameter("int", p.name().label(), p.value().value());
              break;
            }

            case O3Parameter::PRMType::FLOAT: {
              factory.addParameter("real", p.name().label(), p.value().value());
              break;
            }

            default: {
              GUM_ERROR(FatalError, "unknown O3Parameter type")
            }
          }
        }
      }

      template < typename GUM_SCALAR >
      INLINE void O3ClassFactory< GUM_SCALAR >::buildReferenceSlots() {
        // Class with a super class must be declared after
        for (auto c: _o3Classes_) {
          _prm_->getClass(c->name().label()).inheritReferenceSlots();
          _addReferenceSlots_(*c);
        }
      }

      template < typename GUM_SCALAR >
      INLINE void O3ClassFactory< GUM_SCALAR >::_addReferenceSlots_(O3Class& c) {
        PRMFactory< GUM_SCALAR > factory(_prm_);

        factory.continueClass(c.name().label());

        // References
        for (auto& ref: c.referenceSlots()) {
          if (_checkReferenceSlot_(c, ref)) {
            factory.addReferenceSlot(ref.type().label(), ref.name().label(), ref.isArray());
          }
        }

        factory.endClass(false);
      }

      template < typename GUM_SCALAR >
      INLINE bool O3ClassFactory< GUM_SCALAR >::_checkReferenceSlot_(O3Class&         c,
                                                                     O3ReferenceSlot& ref) {
        if (!_solver_->resolveSlotType(ref.type())) { return false; }

        const auto& real_c = _prm_->getClass(c.name().label());

        // Check for dupplicates
        if (real_c.exists(ref.name().label())) {
          const auto& elt = real_c.get(ref.name().label());

          if (PRMClassElement< GUM_SCALAR >::isReferenceSlot(elt)) {
            auto slot_type = (PRMClassElementContainer< GUM_SCALAR >*)nullptr;

            if (_prm_->isInterface(ref.type().label())) {
              slot_type = &(_prm_->getInterface(ref.type().label()));

            } else {
              slot_type = &(_prm_->getClass(ref.type().label()));
            }

            auto real_ref = static_cast< const PRMReferenceSlot< GUM_SCALAR >* >(&elt);

            if (slot_type->name() == real_ref->slotType().name()) {
              O3PRM_CLASS_DUPLICATE_REFERENCE(ref.name(), *_errors_);
              return false;

            } else if (!slot_type->isSubTypeOf(real_ref->slotType())) {
              O3PRM_CLASS_ILLEGAL_OVERLOAD(ref.name(), c.name(), *_errors_);
              return false;
            }

          } else {
            O3PRM_CLASS_DUPLICATE_REFERENCE(ref.name(), *_errors_);
            return false;
          }
        }

        // If class we need to check for illegal references
        if (_prm_->isClass(ref.type().label())) {
          const auto& ref_type = _prm_->getClass(ref.type().label());

          // No recursive reference
          if ((&ref_type) == (&real_c)) {
            O3PRM_CLASS_SELF_REFERENCE(c.name(), ref.name(), *_errors_);
            return false;
          }

          // No reference to subclasses
          if (ref_type.isSubTypeOf(real_c)) {
            O3PRM_CLASS_ILLEGAL_SUB_REFERENCE(c.name(), ref.type(), *_errors_);
            return false;
          }
        }

        return true;
      }

      template < typename GUM_SCALAR >
      INLINE void O3ClassFactory< GUM_SCALAR >::declareAttributes() {
        // Class with a super class must be declared after
        for (auto c: _o3Classes_) {
          _prm_->getClass(c->name().label()).inheritAttributes();
          _declareAttribute_(*c);
        }
      }

      template < typename GUM_SCALAR >
      INLINE void O3ClassFactory< GUM_SCALAR >::declareAggregates() {
        // Class with a super class must be declared after
        for (auto c: _o3Classes_) {
          _prm_->getClass(c->name().label()).inheritAggregates();
          _declareAggregates_(*c);
        }
      }

      template < typename GUM_SCALAR >
      INLINE void O3ClassFactory< GUM_SCALAR >::_declareAttribute_(O3Class& c) {
        PRMFactory< GUM_SCALAR > factory(_prm_);
        factory.continueClass(c.name().label());

        for (auto& attr: c.attributes()) {
          if (_checkAttributeForDeclaration_(c, *attr)) {
            factory.startAttribute(attr->type().label(), attr->name().label());
            factory.endAttribute();
          }
        }

        factory.endClass(false);
      }

      template < typename GUM_SCALAR >
      INLINE bool O3ClassFactory< GUM_SCALAR >::_checkAttributeForDeclaration_(O3Class&     c,
                                                                               O3Attribute& attr) {
        // Check type
        if (!_solver_->resolveType(attr.type())) { return false; }

        // Checking type legality if overload
        if (c.superLabel().label() != "") {
          const auto& super = _prm_->getClass(c.superLabel().label());

          if (!super.exists(attr.name().label())) { return true; }

          const auto& super_type = super.get(attr.name().label()).type();
          const auto& type       = _prm_->type(attr.type().label());

          if (!type.isSubTypeOf(super_type)) {
            O3PRM_CLASS_ILLEGAL_OVERLOAD(attr.name(), c.superLabel(), *_errors_);
            return false;
          }
        }
        return true;
      }

      template < typename GUM_SCALAR >
      INLINE void O3ClassFactory< GUM_SCALAR >::completeAttributes() {
        PRMFactory< GUM_SCALAR > factory(_prm_);

        // Class with a super class must be declared in order
        for (auto c: _o3Classes_) {
          _prm_->getClass(c->name().label()).inheritSlotChains();
          factory.continueClass(c->name().label());

          _completeAttribute_(factory, *c);

          if (c->superLabel().label() != "") {
            auto& super       = _prm_->getClass(c->superLabel().label());
            auto  to_complete = Set< std::string >();

            for (auto a: super.attributes()) {
              to_complete.insert(a->safeName());
            }

            for (auto a: super.aggregates()) {
              to_complete.insert(a->safeName());
            }

            for (auto& a: c->attributes()) {
              to_complete.erase(
                 _prm_->getClass(c->name().label()).get(a->name().label()).safeName());
            }

            for (auto& a: c->aggregates()) {
              to_complete.erase(
                 _prm_->getClass(c->name().label()).get(a.name().label()).safeName());
            }

            for (auto a: to_complete) {
              _prm_->getClass(c->name().label()).completeInheritance(a);
            }
          }

          factory.endClass(true);
        }
      }

      template < typename GUM_SCALAR >
      INLINE void O3ClassFactory< GUM_SCALAR >::completeAggregates() {
        PRMFactory< GUM_SCALAR > factory(_prm_);

        // Class with a super class must be declared in order
        for (auto c: _o3Classes_) {
          factory.continueClass(c->name().label());

          _completeAggregates_(factory, *c);

          factory.endClass(false);
        }
      }

      template < typename GUM_SCALAR >
      INLINE void
         O3ClassFactory< GUM_SCALAR >::_completeAggregates_(PRMFactory< GUM_SCALAR >& factory,
                                                            O3Class&                  c) {
        // Attributes
        for (auto& agg: c.aggregates()) {
          if (_checkAggregateForCompletion_(c, agg)) {
            factory.continueAggregator(agg.name().label());

            for (const auto& parent: agg.parents()) {
              factory.addParent(parent.label());
            }

            factory.endAggregator();
          }
        }
      }

      template < typename GUM_SCALAR >
      INLINE bool O3ClassFactory< GUM_SCALAR >::_checkAggregateForCompletion_(O3Class&     c,
                                                                              O3Aggregate& agg) {
        // Checking parents
        auto t = _checkAggParents_(c, agg);
        if (t == nullptr) { return false; }

        // Checking parameters numbers
        if (!_checkAggParameters_(c, agg, t)) { return false; }

        return true;
      }

      template < typename GUM_SCALAR >
      INLINE void
         O3ClassFactory< GUM_SCALAR >::_completeAttribute_(PRMFactory< GUM_SCALAR >& factory,
                                                           O3Class&                  c) {
        // Attributes
        for (auto& attr: c.attributes()) {
          if (_checkAttributeForCompletion_(c, *attr)) {
            factory.continueAttribute(attr->name().label());

            for (const auto& parent: attr->parents()) {
              factory.addParent(parent.label());
            }

            auto raw = dynamic_cast< const O3RawCPT* >(attr.get());

            if (raw) {
              auto values = std::vector< std::string >();
              for (const auto& val: raw->values()) {
                values.push_back(val.formula().formula());
              }
              factory.setRawCPFByColumns(values);
            }

            auto rule_cpt = dynamic_cast< const O3RuleCPT* >(attr.get());
            if (rule_cpt) {
              for (const auto& rule: rule_cpt->rules()) {
                auto labels = std::vector< std::string >();
                auto values = std::vector< std::string >();

                for (const auto& lbl: rule.first) {
                  labels.push_back(lbl.label());
                }

                for (const auto& form: rule.second) {
                  values.push_back(form.formula().formula());
                }

                factory.setCPFByRule(labels, values);
              }
            }

            factory.endAttribute();
          }
        }
      }

      template < typename GUM_SCALAR >
      INLINE bool O3ClassFactory< GUM_SCALAR >::_checkAttributeForCompletion_(const O3Class& o3_c,
                                                                              O3Attribute&   attr) {
        // Check for parents existence
        const auto& c = _prm_->getClass(o3_c.name().label());
        for (auto& prnt: attr.parents()) {
          if (!_checkParent_(c, prnt)) { return false; }
        }

        // Check that CPT sums to 1
        auto raw = dynamic_cast< O3RawCPT* >(&attr);
        if (raw) { return _checkRawCPT_(c, *raw); }

        auto rule = dynamic_cast< O3RuleCPT* >(&attr);
        if (rule) { return _checkRuleCPT_(c, *rule); }

        return true;
      }

      template < typename GUM_SCALAR >
      INLINE bool O3ClassFactory< GUM_SCALAR >::_checkParent_(const PRMClass< GUM_SCALAR >& c,
                                                              const O3Label&                prnt) {
        if (prnt.label().find('.') == std::string::npos) {
          return _checkLocalParent_(c, prnt);

        } else {
          return _checkRemoteParent_(c, prnt);
        }
      }

      template < typename GUM_SCALAR >
      INLINE bool O3ClassFactory< GUM_SCALAR >::_checkLocalParent_(const PRMClass< GUM_SCALAR >& c,
                                                                   const O3Label& prnt) {
        if (!c.exists(prnt.label())) {
          O3PRM_CLASS_PARENT_NOT_FOUND(prnt, *_errors_);
          return false;
        }

        const auto& elt = c.get(prnt.label());
        if (!(gum::prm::PRMClassElement< GUM_SCALAR >::isAttribute(elt)
              || gum::prm::PRMClassElement< GUM_SCALAR >::isSlotChain(elt)
              || gum::prm::PRMClassElement< GUM_SCALAR >::isAggregate(elt))) {
          O3PRM_CLASS_ILLEGAL_PARENT(prnt, *_errors_);
          return false;
        }

        return true;
      }

      template < typename GUM_SCALAR >
      INLINE bool O3ClassFactory< GUM_SCALAR >::_checkRemoteParent_(
         const PRMClassElementContainer< GUM_SCALAR >& c,
         const O3Label&                                prnt) {
        if (_resolveSlotChain_(c, prnt) == nullptr) { return false; }
        return true;
      }

      template < typename GUM_SCALAR >
      INLINE bool O3ClassFactory< GUM_SCALAR >::_checkLabelsNumber_(const O3RuleCPT&         attr,
                                                                    const O3RuleCPT::O3Rule& rule) {
        // Check that the number of labels is correct
        if (rule.first.size() != attr.parents().size()) {
          O3PRM_CLASS_ILLEGAL_RULE_SIZE(rule, rule.first.size(), attr.parents().size(), *_errors_);
          return false;
        }
        return true;
      }

      template < typename GUM_SCALAR >
      INLINE bool O3ClassFactory< GUM_SCALAR >::_checkLabelsValues_(const PRMClass< GUM_SCALAR >& c,
                                                                    const O3RuleCPT&         attr,
                                                                    const O3RuleCPT::O3Rule& rule) {
        bool errors = false;
        for (std::size_t i = 0; i < attr.parents().size(); ++i) {
          auto label = rule.first[i];
          auto prnt  = attr.parents()[i];
          try {
            auto real_labels = _resolveSlotChain_(c, prnt)->type()->labels();
            // c.get(prnt.label()).type()->labels();
            if (label.label() != "*"
                && std::find(real_labels.begin(), real_labels.end(), label.label())
                      == real_labels.end()) {
              O3PRM_CLASS_ILLEGAL_RULE_LABEL(rule, label, prnt, *_errors_);
              errors = true;
            }
          } catch (Exception const&) {
            // parent does not exists and is already reported
          }
        }
        return errors == false;
      }

      template < typename GUM_SCALAR >
      INLINE void O3ClassFactory< GUM_SCALAR >::_addParamsToForms_(
         const HashTable< std::string, const PRMParameter< GUM_SCALAR >* >& scope,
         O3RuleCPT::O3Rule&                                                 rule) {
        // Add parameters to formulas
        for (auto& f: rule.second) {
          f.formula().variables().clear();
          for (const auto& values: scope) {
            f.formula().variables().insert(values.first, values.second->value());
          }
        }
      }


      template < typename GUM_SCALAR >
      INLINE bool
         O3ClassFactory< GUM_SCALAR >::_checkRuleCPTSumsTo1_(const PRMClass< GUM_SCALAR >& c,
                                                             const O3RuleCPT&              attr,
                                                             const O3RuleCPT::O3Rule&      rule) {
        bool errors = false;
        // Check that formulas are valid and sums to 1
        GUM_SCALAR sum = 0.0;
        for (const auto& f: rule.second) {
          try {
            auto value = GUM_SCALAR(f.formula().result());
            sum += value;
            if (value < 0.0 || 1.0 < value) {
              O3PRM_CLASS_ILLEGAL_CPT_VALUE(c.name(), attr.name(), f, *_errors_);
              errors = true;
            }
          } catch (OperationNotAllowed const&) {
            O3PRM_CLASS_ILLEGAL_CPT_VALUE(c.name(), attr.name(), f, *_errors_);
            errors = true;
          }
        }

        // Check that CPT sums to 1
        if (std::abs(sum - 1.0) > 1e-3) {
          O3PRM_CLASS_CPT_DOES_NOT_SUM_TO_1(c.name(), attr.name(), float(sum), *_errors_);
          errors = true;
        } else if (std::abs(sum - 1.0f) > 1e-6) {
          O3PRM_CLASS_CPT_DOES_NOT_SUM_TO_1_WARNING(c.name(), attr.name(), float(sum), *_errors_);
        }
        return errors == false;
      }

      template < typename GUM_SCALAR >
      INLINE bool O3ClassFactory< GUM_SCALAR >::_checkRuleCPT_(const PRMClass< GUM_SCALAR >& c,
                                                               O3RuleCPT&                    attr) {
        const auto& scope  = c.scope();
        bool        errors = false;
        for (auto& rule: attr.rules()) {
          try {
            if (!_checkLabelsNumber_(attr, rule)) { errors = true; }
            if (!_checkLabelsValues_(c, attr, rule)) { errors = true; }
            _addParamsToForms_(scope, rule);
            if (!_checkRuleCPTSumsTo1_(c, attr, rule)) { errors = true; }
          } catch (Exception& e) {
            GUM_SHOWERROR(e);
            errors = true;
          }
        }

        return errors == false;
      }

      template < typename GUM_SCALAR >
      INLINE bool O3ClassFactory< GUM_SCALAR >::_checkRawCPT_(const PRMClass< GUM_SCALAR >& c,
                                                              O3RawCPT&                     attr) {
        const auto& type = _prm_->type(attr.type().label());

        auto domainSize = type->domainSize();
        for (auto& prnt: attr.parents()) {
          try {
            domainSize *= c.get(prnt.label()).type()->domainSize();
          } catch (NotFound const&) {
            // If we are here, all parents have been check so  _resolveSlotChain_
            // will not raise an error and not return a nullptr
            domainSize *= _resolveSlotChain_(c, prnt)->type()->domainSize();
          }
        }

        // Check for CPT size
        if (domainSize != attr.values().size()) {
          O3PRM_CLASS_ILLEGAL_CPT_SIZE(c.name(),
                                       attr.name(),
                                       Size(attr.values().size()),
                                       domainSize,
                                       *_errors_);
          return false;
        }

        // Add parameters to formulas
        const auto& scope = c.scope();
        for (auto& f: attr.values()) {
          f.formula().variables().clear();

          for (const auto& values: scope) {
            f.formula().variables().insert(values.first, values.second->value());
          }
        }

        // Check that CPT sums to 1
        Size parent_size = domainSize / type->domainSize();
        auto values      = std::vector< GUM_SCALAR >(parent_size, 0.0f);

        for (std::size_t i = 0; i < attr.values().size(); ++i) {
          try {
            auto idx = i % parent_size;
            auto val = (GUM_SCALAR)attr.values()[i].formula().result();
            values[idx] += val;

            if (val < 0.0 || 1.0 < val) {
              O3PRM_CLASS_ILLEGAL_CPT_VALUE(c.name(), attr.name(), attr.values()[i], *_errors_);
              return false;
            }
          } catch (Exception const&) {
            O3PRM_CLASS_ILLEGAL_CPT_VALUE(c.name(), attr.name(), attr.values()[i], *_errors_);
            return false;
          }
        }

        for (auto f: values) {
          if (std::abs(f - GUM_SCALAR(1.0)) > 1.0e-3) {
            O3PRM_CLASS_CPT_DOES_NOT_SUM_TO_1(c.name(), attr.name(), float(f), *_errors_);
            return false;
          } else if (std::abs(f - GUM_SCALAR(1.0)) > 1.0e-6) {
            O3PRM_CLASS_CPT_DOES_NOT_SUM_TO_1_WARNING(c.name(), attr.name(), float(f), *_errors_);
          }
        }
        return true;
      }

      template < typename GUM_SCALAR >
      INLINE const PRMClassElement< GUM_SCALAR >* O3ClassFactory< GUM_SCALAR >::_resolveSlotChain_(
         const PRMClassElementContainer< GUM_SCALAR >& c,
         const O3Label&                                chain) {
        auto                       s       = chain.label();
        auto                       current = &c;
        std::vector< std::string > v;

        decomposePath(chain.label(), v);

        for (size_t i = 0; i < v.size(); ++i) {
          auto link = v[i];

          if (!_checkSlotChainLink_(*current, chain, link)) { return nullptr; }

          auto elt = &(current->get(link));

          if (i == v.size() - 1) {
            // last link, should be an attribute or aggregate
            return elt;

          } else {
            // should be a reference slot

            auto ref = dynamic_cast< const PRMReferenceSlot< GUM_SCALAR >* >(elt);
            if (ref) {
              current = &(ref->slotType());
            } else {
              return nullptr;   // failsafe to prevent infinite loop
            }
          }
        }

        // Encountered only reference slots

        return nullptr;
      }

      template < typename GUM_SCALAR >
      INLINE bool O3ClassFactory< GUM_SCALAR >::_checkSlotChainLink_(
         const PRMClassElementContainer< GUM_SCALAR >& c,
         const O3Label&                                chain,
         const std::string&                            s) {
        if (!c.exists(s)) {
          O3PRM_CLASS_LINK_NOT_FOUND(chain, s, *_errors_);
          return false;
        }
        return true;
      }

      template < typename GUM_SCALAR >
      INLINE void O3ClassFactory< GUM_SCALAR >::_declareAggregates_(O3Class& c) {
        PRMFactory< GUM_SCALAR > factory(_prm_);
        factory.continueClass(c.name().label());

        for (auto& agg: c.aggregates()) {
          if (_checkAggregateForDeclaration_(c, agg)) {
            auto params = std::vector< std::string >();
            for (auto& p: agg.parameters()) {
              params.push_back(p.label());
            }

            factory.startAggregator(agg.name().label(),
                                    agg.aggregateType().label(),
                                    agg.variableType().label(),
                                    params);
            factory.endAggregator();
          }
        }

        factory.endClass(false);
      }

      template < typename GUM_SCALAR >
      INLINE bool O3ClassFactory< GUM_SCALAR >::_checkAggregateForDeclaration_(O3Class&     o3class,
                                                                               O3Aggregate& agg) {
        if (!_solver_->resolveType(agg.variableType())) { return false; }

        // Checking type legality if overload
        if (!_checkAggTypeLegality_(o3class, agg)) { return false; }

        return true;
      }

      template < typename GUM_SCALAR >
      INLINE const PRMType* O3ClassFactory< GUM_SCALAR >::_checkAggParents_(O3Class&     o3class,
                                                                            O3Aggregate& agg) {
        const auto& c = _prm_->getClass(o3class.name().label());
        auto        t = (const PRMType*)nullptr;

        for (const auto& prnt: agg.parents()) {
          auto elt = _resolveSlotChain_(c, prnt);

          if (elt == nullptr) {
            O3PRM_CLASS_PARENT_NOT_FOUND(prnt, *_errors_);
            return nullptr;

          } else {
            if (t == nullptr) {
              try {
                t = &(elt->type());

              } catch (OperationNotAllowed const&) {
                O3PRM_CLASS_WRONG_PARENT(prnt, *_errors_);
                return nullptr;
              }

            } else if ((*t) != elt->type()) {
              // Wront type in chain
              O3PRM_CLASS_WRONG_PARENT_TYPE(prnt, t->name(), elt->type().name(), *_errors_);
              return nullptr;
            }
          }
        }
        return t;
      }

      template < typename GUM_SCALAR >
      INLINE bool O3ClassFactory< GUM_SCALAR >::_checkAggTypeLegality_(O3Class&     o3class,
                                                                       O3Aggregate& agg) {
        if (_prm_->isClass(o3class.superLabel().label())) {
          const auto& super    = _prm_->getClass(o3class.superLabel().label());
          const auto& agg_type = _prm_->type(agg.variableType().label());

          if (super.exists(agg.name().label())
              && !agg_type.isSubTypeOf(super.get(agg.name().label()).type())) {
            O3PRM_CLASS_ILLEGAL_OVERLOAD(agg.name(), o3class.superLabel(), *_errors_);
            return false;
          }
        }

        return true;
      }

      template < typename GUM_SCALAR >
      INLINE bool O3ClassFactory< GUM_SCALAR >::_checkAggParameters_(O3Class&       o3class,
                                                                     O3Aggregate&   agg,
                                                                     const PRMType* t) {
        bool ok = false;

        switch (gum::prm::PRMAggregate< GUM_SCALAR >::str2enum(agg.aggregateType().label())) {
          case PRMAggregate< GUM_SCALAR >::AggregateType::MIN:
          case PRMAggregate< GUM_SCALAR >::AggregateType::MAX:
          case PRMAggregate< GUM_SCALAR >::AggregateType::AMPLITUDE:
          case PRMAggregate< GUM_SCALAR >::AggregateType::MEDIAN:
          case PRMAggregate< GUM_SCALAR >::AggregateType::OR:
          case PRMAggregate< GUM_SCALAR >::AggregateType::SUM:
          case PRMAggregate< GUM_SCALAR >::AggregateType::AND: {
            ok = _checkParametersNumber_(agg, 0);
            break;
          }

          case PRMAggregate< GUM_SCALAR >::AggregateType::FORALL:
          case PRMAggregate< GUM_SCALAR >::AggregateType::EXISTS:
          case PRMAggregate< GUM_SCALAR >::AggregateType::COUNT: {
            ok = _checkParametersNumber_(agg, 1);
            break;
          }

          default: {
            GUM_ERROR(FatalError, "unknown aggregate type")
          }
        }

        if (!ok) { return false; }

        // Checking parameters type
        switch (gum::prm::PRMAggregate< GUM_SCALAR >::str2enum(agg.aggregateType().label())) {
          case PRMAggregate< GUM_SCALAR >::AggregateType::FORALL:
          case PRMAggregate< GUM_SCALAR >::AggregateType::EXISTS:
          case PRMAggregate< GUM_SCALAR >::AggregateType::COUNT: {
            ok = _checkParameterValue_(agg, *t);
            break;
          }

          default: { /* Nothing to do */
          }
        }

        return ok;
      }

      template < typename GUM_SCALAR >
      INLINE bool O3ClassFactory< GUM_SCALAR >::_checkParametersNumber_(O3Aggregate& agg, Size n) {
        if (agg.parameters().size() != n) {
          O3PRM_CLASS_AGG_PARAMETERS(agg.name(), Size(n), Size(agg.parameters().size()), *_errors_);
          return false;
        }

        return true;
      }

      template < typename GUM_SCALAR >
      INLINE bool O3ClassFactory< GUM_SCALAR >::_checkParameterValue_(O3Aggregate&             agg,
                                                                      const gum::prm::PRMType& t) {
        const auto& param = agg.parameters().front();
        bool        found = false;
        for (Size idx = 0; idx < t.variable().domainSize(); ++idx) {
          if (t.variable().label(idx) == param.label()) {
            found = true;
            break;
          }
        }

        if (!found) {
          O3PRM_CLASS_AGG_PARAMETER_NOT_FOUND(agg.name(), param, *_errors_);
          return false;
        }

        return true;
      }

    }   // namespace o3prm
  }     // namespace prm
}   // namespace gum
