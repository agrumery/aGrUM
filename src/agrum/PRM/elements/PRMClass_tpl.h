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
 * @brief Inline implementation of gum::Class
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

// to ease parsing
#include <queue>

#include <agrum/PRM/elements/PRMClass.h>

#include <agrum/PRM/elements/PRMInterface.h>

namespace gum {
  namespace prm {
    template < typename GUM_SCALAR >
    PRMClass< GUM_SCALAR >::PRMClass(const std::string& name) :
        PRMClassElementContainer< GUM_SCALAR >(name), _superClass_(nullptr), _implements_(nullptr),
        _bijection_(nullptr) {
      GUM_CONSTRUCTOR(PRMClass);
    }

    template < typename GUM_SCALAR >
    PRMClass< GUM_SCALAR >::PRMClass(const std::string&      name,
                                     PRMClass< GUM_SCALAR >& super,
                                     bool                    delayInheritance) :
        PRMClassElementContainer< GUM_SCALAR >(name),
        _superClass_(&super), _implements_(nullptr),
        _bijection_(new Bijection< const DiscreteVariable*, const DiscreteVariable* >()) {
      GUM_CONSTRUCTOR(PRMClass);
      if (!delayInheritance) {
        _dag_ = super.containerDag();
        _inheritClass_(super);
      }
    }

    template < typename GUM_SCALAR >
    PRMClass< GUM_SCALAR >::PRMClass(const std::string&                        name,
                                     const Set< PRMInterface< GUM_SCALAR >* >& set,
                                     bool                                      delayInheritance) :
        PRMClassElementContainer< GUM_SCALAR >(name),
        _superClass_(nullptr), _implements_(new Set< PRMInterface< GUM_SCALAR >* >(set)),
        _bijection_(nullptr) {
      GUM_CONSTRUCTOR(PRMClass);

      if (!delayInheritance) { _implementInterfaces_(false); }
    }

    template < typename GUM_SCALAR >
    PRMClass< GUM_SCALAR >::PRMClass(const std::string&                        name,
                                     PRMClass< GUM_SCALAR >&                   super,
                                     const Set< PRMInterface< GUM_SCALAR >* >& set,
                                     bool                                      delayInheritance) :
        PRMClassElementContainer< GUM_SCALAR >(name),
        _superClass_(&super), _implements_(nullptr),
        _bijection_(new Bijection< const DiscreteVariable*, const DiscreteVariable* >()) {
      GUM_CONSTRUCTOR(PRMClass);
      if (!delayInheritance) {
        _dag_ = super.containerDag();
        _inheritClass_(super);
      }

      // Adding other implementation
      if (_implements_ == nullptr) {   // super has not created  _implements_
        _implements_ = new Set< PRMInterface< GUM_SCALAR >* >(set);
      } else {   // we just add the new implementations
        for (const auto elt: set) {
          _implements_->insert(elt);
        }
      }

      if (!delayInheritance) { _implementInterfaces_(false); }
    }

    template < typename GUM_SCALAR >
    void PRMClass< GUM_SCALAR >::_implementInterfaces_(bool delayedInheritance) {
      for (const auto impl: *_implements_) {
        impl->_addImplementation_(this);
        if ((!_superClass_) || (!super().isSubTypeOf(*impl)) || delayedInheritance) {
          // Reserve reference id in DAG
          for (auto ref: impl->referenceSlots()) {
            _dag_.addNodeWithId(ref->id());
          }
          // Reserve attribute id in DAG
          for (auto attr: impl->attributes()) {
            _dag_.addNodeWithId(attr->id());
          }
        }
      }
    }

    template < typename GUM_SCALAR >
    PRMClass< GUM_SCALAR >::~PRMClass() {
      GUM_DESTRUCTOR(PRMClass);

      for (const auto& elt: _nodeIdMap_) {
        delete elt.second;
      }

      if (_implements_) { delete _implements_; }

      if (_bijection_) { delete _bijection_; }
    }

    template < typename GUM_SCALAR >
    void PRMClass< GUM_SCALAR >::initializeInheritance() {
      if (_superClass_) {
        _superClass_->_addExtension_(this);
        // Adding implemented interfaces, if any
        if (_superClass_->_implements_) {
          if (!_implements_) {
            _implements_ = new Set< PRMInterface< GUM_SCALAR >* >(*(_superClass_->_implements_));
          } else {
            for (auto i: *(_superClass_->_implements_)) {
              _implements_->insert(i);
            }
          }
        }
      }
      if (_implements_) { _implementInterfaces_(true); }
    }

    template < typename GUM_SCALAR >
    void PRMClass< GUM_SCALAR >::inheritReferenceSlots() {
      if (_superClass_) {
        // Copying reference slots
        for (const auto c_refslot: _superClass_->_referenceSlots_) {
          auto ref = new PRMReferenceSlot< GUM_SCALAR >(
             c_refslot->name(),
             const_cast< PRMClassElementContainer< GUM_SCALAR >& >(c_refslot->slotType()),
             c_refslot->isArray());

          ref->setId(c_refslot->id());
          // Not reserved by an interface
          if (!_dag_.existsNode(ref->id())) { _dag_.addNodeWithId(ref->id()); }
          _nodeIdMap_.insert(ref->id(), ref);
          _referenceSlots_.insert(ref);

          if (_superClass_->_nameMap_[c_refslot->name()]
              == _superClass_->_nameMap_[c_refslot->safeName()]) {
            _nameMap_.insert(ref->name(), ref);
          }

          _nameMap_.insert(ref->safeName(), ref);
        }
      }
    }

    template < typename GUM_SCALAR >
    void PRMClass< GUM_SCALAR >::inheritParameters() {
      if (_superClass_) {
        // Copying parameters
        for (const auto c_param: _superClass_->_parameters_) {
          auto param = new PRMParameter< GUM_SCALAR >(c_param->name(),
                                                      c_param->valueType(),
                                                      c_param->value());

          _parameters_.insert(param);

          param->setId(c_param->id());
          _dag_.addNodeWithId(param->id());
          _nodeIdMap_.insert(param->id(), param);
          _nameMap_.insert(param->name(), param);
        }
      }
    }

    template < typename GUM_SCALAR >
    void PRMClass< GUM_SCALAR >::inheritAttributes() {
      if (_superClass_) {
        for (const auto c_attr: _superClass_->_attributes_) {
          // using multiDimSparse to prevent unecessary memory allocation for
          // large arrays (the potentials are copied latter)
          auto attr = c_attr->newFactory(*this);

          _bijection_->insert(&(c_attr->type().variable()), &(attr->type().variable()));
          attr->setId(c_attr->id());
          try {
            _dag_.addNodeWithId(attr->id());
          } catch (gum::Exception&) {
            // Node reserved by an interface
          }
          _nodeIdMap_.insert(attr->id(), attr);
          _attributes_.insert(attr);

          if (_superClass_->_nameMap_[c_attr->name()]
              == _superClass_->_nameMap_[c_attr->safeName()]) {
            _nameMap_.insert(attr->name(), attr);
          }

          _nameMap_.insert(attr->safeName(), attr);
        }
      }
    }

    template < typename GUM_SCALAR >
    void PRMClass< GUM_SCALAR >::inheritAggregates() {
      if (_superClass_) {
        for (const auto c_agg: _superClass_->_aggregates_) {
          PRMAggregate< GUM_SCALAR >* agg = nullptr;

          try {
            agg = new PRMAggregate< GUM_SCALAR >(c_agg->name(),
                                                 c_agg->agg_type(),
                                                 c_agg->type(),
                                                 c_agg->label());
          } catch (OperationNotAllowed const&) {
            agg = new PRMAggregate< GUM_SCALAR >(c_agg->name(), c_agg->agg_type(), c_agg->type());
            agg->sharedLabel(c_agg->sharedLabel());
            agg->setLabel(c_agg->labelValue());
          }

          _bijection_->insert(&(c_agg->type().variable()), &(agg->type().variable()));
          agg->setId(c_agg->id());
          _dag_.addNodeWithId(agg->id());
          _nodeIdMap_.insert(agg->id(), agg);
          _aggregates_.insert(agg);

          if (_superClass_->_nameMap_[c_agg->name()] == _superClass_->_nameMap_[c_agg->safeName()])
            _nameMap_.insert(agg->name(), agg);

          _nameMap_.insert(agg->safeName(), agg);
        }
      }
    }

    template < typename GUM_SCALAR >
    void PRMClass< GUM_SCALAR >::inheritSlotChains() {
      if (_superClass_) {
        // Copying slot chains
        for (const auto c_sc: _superClass_->_slotChains_) {
          // Because of aggregators, some slotchains may exists already
          if (!(_nameMap_.exists(c_sc->name()) && _nameMap_.exists(c_sc->safeName()))) {
            // We just need to change the first PRMReferenceSlot<GUM_SCALAR> in
            // the
            // chain
            auto chain = c_sc->chain();

            chain.setAtPos(0, _nameMap_[c_sc->chain().front()->name()]);

            auto sc = new PRMSlotChain< GUM_SCALAR >(c_sc->name(), chain);
            _bijection_->insert(&(c_sc->type().variable()), &(sc->type().variable()));
            sc->setId(c_sc->id());
            _dag_.addNodeWithId(sc->id());
            _nodeIdMap_.insert(sc->id(), sc);
            _slotChains_.insert(sc);

            if (!_nameMap_.exists(sc->name())) { _nameMap_.insert(sc->name(), sc); }
            if (!_nameMap_.exists(sc->safeName())) { _nameMap_.insert(sc->safeName(), sc); }
          }
        }
      }
    }

    template < typename GUM_SCALAR >
    void PRMClass< GUM_SCALAR >::completeInheritance(const std::string& name) {
      if (_superClass_) {
        auto& elt = this->get(name);
        if (!(PRMClassElement< GUM_SCALAR >::isAttribute(elt)
              || PRMClassElement< GUM_SCALAR >::isAggregate(elt))) {
          GUM_ERROR(OperationNotAllowed, "you can only complete inheritance for attributes")
        }

        for (const auto& prnt: super().containerDag().parents(elt.id())) {
          this->addArc(super().get(prnt).safeName(), elt.safeName());
        }

        if (PRMClassElement< GUM_SCALAR >::isAttribute(elt)) {
          auto& attr       = static_cast< PRMAttribute< GUM_SCALAR >& >(elt);
          auto& super_attr = static_cast< const PRMAttribute< GUM_SCALAR >& >(super().get(name));
          attr.copyCpf(*_bijection_, super_attr);
        }
      }
    }

    template < typename GUM_SCALAR >
    void PRMClass< GUM_SCALAR >::_inheritClass_(const PRMClass< GUM_SCALAR >& c) {
      if (_superClass_) {
        _superClass_->_addExtension_(this);
        // Adding implemented interfaces of c, if any
        if (c._implements_) {
          if (!_implements_) {
            _implements_ = new Set< PRMInterface< GUM_SCALAR >* >(*(c._implements_));
          } else {
            for (auto i: *(c._implements_)) {
              _implements_->insert(i);
            }
          }
        }

        // Copying attributes, the bijection's firsts are attributes in this and
        // its
        // seconds are attributes
        // in c.
        Bijection< const DiscreteVariable*, const DiscreteVariable* > bij;

        // Copying parameters
        for (const auto c_param: c._parameters_) {
          auto param = new PRMParameter< GUM_SCALAR >(c_param->name(),
                                                      c_param->valueType(),
                                                      c_param->value());

          _parameters_.insert(param);

          param->setId(c_param->id());
          _nodeIdMap_.insert(param->id(), param);
          _nameMap_.insert(param->name(), param);
        }

        // Copying attributes
        for (const auto c_attr: c._attributes_) {
          // using multiDimSparse to prevent unecessary memory allocation for
          // large arrays (the potentials are copied latter)
          auto attr = c_attr->newFactory(*this);

          bij.insert(&(c_attr->type().variable()), &(attr->type().variable()));
          attr->setId(c_attr->id());
          _nodeIdMap_.insert(attr->id(), attr);
          _attributes_.insert(attr);

          if (c._nameMap_[c_attr->name()] == c._nameMap_[c_attr->safeName()]) {
            _nameMap_.insert(attr->name(), attr);
          }

          _nameMap_.insert(attr->safeName(), attr);
        }

        // Copying aggregates
        for (const auto c_agg: c._aggregates_) {
          PRMAggregate< GUM_SCALAR >* agg = nullptr;

          try {
            agg = new PRMAggregate< GUM_SCALAR >(c_agg->name(),
                                                 c_agg->agg_type(),
                                                 c_agg->type(),
                                                 c_agg->label());
          } catch (OperationNotAllowed const&) {
            agg = new PRMAggregate< GUM_SCALAR >(c_agg->name(), c_agg->agg_type(), c_agg->type());
          }

          bij.insert(&(c_agg->type().variable()), &(agg->type().variable()));
          agg->setId(c_agg->id());
          _nodeIdMap_.insert(agg->id(), agg);
          _aggregates_.insert(agg);

          if (c._nameMap_[c_agg->name()] == c._nameMap_[c_agg->safeName()])
            _nameMap_.insert(agg->name(), agg);

          _nameMap_.insert(agg->safeName(), agg);
        }

        // Copying reference slots
        for (const auto c_refslot: c._referenceSlots_) {
          PRMReferenceSlot< GUM_SCALAR >* ref = new PRMReferenceSlot< GUM_SCALAR >(
             c_refslot->name(),
             const_cast< PRMClassElementContainer< GUM_SCALAR >& >(c_refslot->slotType()),
             c_refslot->isArray());

          ref->setId(c_refslot->id());
          _nodeIdMap_.insert(ref->id(), ref);
          _referenceSlots_.insert(ref);

          if (c._nameMap_[c_refslot->name()] == c._nameMap_[c_refslot->safeName()])
            _nameMap_.insert(ref->name(), ref);

          _nameMap_.insert(ref->safeName(), ref);
        }

        // Copying slot chains
        for (const auto c_slotchain: c._slotChains_) {
          // We just need to change the first PRMReferenceSlot<GUM_SCALAR> in
          // the
          // chain
          Sequence< PRMClassElement< GUM_SCALAR >* > chain(c_slotchain->chain());

          chain.setAtPos(0, _nameMap_[c_slotchain->chain().front()->name()]);

          PRMSlotChain< GUM_SCALAR >* sc
             = new PRMSlotChain< GUM_SCALAR >(c_slotchain->name(), chain);
          bij.insert(&(c_slotchain->type().variable()), &(sc->type().variable()));
          sc->setId(c_slotchain->id());
          _nodeIdMap_.insert(sc->id(), sc);
          _slotChains_.insert(sc);

          _nameMap_.insert(sc->name(), sc);
          _nameMap_.insert(sc->safeName(), sc);
        }

        // Copying dependencies yield by arcs
        for (const auto& arc: c.containerDag().arcs()) {
          _nodeIdMap_[arc.tail()]->addChild(*(_nodeIdMap_[arc.head()]));
          _nodeIdMap_[arc.head()]->addParent(*(_nodeIdMap_[arc.tail()]));
        }

        // Copying the IO flag
        this->copyIOFlags_(c);
        // Copying content of CPF
        for (const auto attr: c._attributes_) {
          auto a = static_cast< PRMAttribute< GUM_SCALAR >* >(_nameMap_[attr->safeName()]);
          a->copyCpf(bij, *attr);
        }
      }
    }

    template < typename GUM_SCALAR >
    bool PRMClass< GUM_SCALAR >::isSubTypeOf(
       const PRMClassElementContainer< GUM_SCALAR >& cec) const {
      switch (cec.obj_type()) {
        case PRMObject::prm_type::CLASS: {
          const PRMClass< GUM_SCALAR >* current = this;

          while (current != 0) {
            if (current == &(cec)) return true;

            current = current->_superClass_;
          }

          return false;
        }

        case PRMObject::prm_type::PRM_INTERFACE: {
          if (_implements_ != nullptr) {
            const PRMInterface< GUM_SCALAR >& i
               = static_cast< const PRMInterface< GUM_SCALAR >& >(cec);

            if (_implements_->exists(const_cast< PRMInterface< GUM_SCALAR >* >(&i))) return true;

            for (const auto impl: *_implements_)
              if (impl->isSubTypeOf(i)) return true;
          }

          return false;
        }

        default: {
          GUM_ERROR(FatalError, "unknown ClassElementContainer<GUM_SCALAR>")
        }
      }
    }

    template < typename GUM_SCALAR >
    void PRMClass< GUM_SCALAR >::addArc(const std::string& tail_name,
                                        const std::string& head_name) {
      PRMClassElement< GUM_SCALAR >* tail = 0;
      PRMClassElement< GUM_SCALAR >* head = 0;

      try {
        tail = _nameMap_[tail_name];
        head = _nameMap_[head_name];
      } catch (NotFound const&) {
        GUM_ERROR(NotFound, "tail and/or head of arc does not exists in this Class")
      }

      if ((tail->elt_type() == PRMClassElement< GUM_SCALAR >::prm_refslot)
          || (head->elt_type() == PRMClassElement< GUM_SCALAR >::prm_refslot)) {
        GUM_ERROR(OperationNotAllowed,
                  "a PRMReferenceSlot<GUM_SCALAR> can "
                  "not on neither side of an arc");
      }

      if ((tail->elt_type() == PRMClassElement< GUM_SCALAR >::prm_slotchain)
          && (head->elt_type() == PRMClassElement< GUM_SCALAR >::prm_slotchain)) {
        GUM_ERROR(OperationNotAllowed,
                  "illegal insertion of an arc between two SlotChain<GUM_SCALAR>")
      }

      if (!_dag_.existsArc(Arc(tail->id(), head->id()))) {
        _dag_.addArc(tail->id(), head->id());
      } else {
        GUM_ERROR(DuplicateElement, "duplicate arc " << tail_name << "->" << head_name)
      }

      get(tail->id()).addChild(get(head->id()));
      get(head->id()).addParent(get(tail->id()));

      // Defining input / output nodes
      if (tail->elt_type() == PRMClassElement< GUM_SCALAR >::prm_slotchain) {
        PRMSlotChain< GUM_SCALAR >* sc = static_cast< PRMSlotChain< GUM_SCALAR >* >(tail);
        this->setInputNode(*head, true);
        sc->end().setOutputNode(sc->end().get(sc->lastElt().safeName()), true);
      }
    }

    template < typename GUM_SCALAR >
    void PRMClass< GUM_SCALAR >::_checkInterfaces_(PRMClassElement< GUM_SCALAR >* elt) {
      try {
        for (auto i: implements()) {
          if (i->exists(elt->name())) { _checkInterface_(elt, i); }
        }
      } catch (NotFound const&) {
        // No interface
      }
    }

    template < typename GUM_SCALAR >
    void PRMClass< GUM_SCALAR >::_checkInterface_(PRMClassElement< GUM_SCALAR >* elt,
                                                  PRMInterface< GUM_SCALAR >*    i) {
      const auto& i_elt   = i->get(elt->name());
      bool        is_attr = PRMClassElement< GUM_SCALAR >::isAttribute(i_elt);
      bool        is_agg  = PRMClassElement< GUM_SCALAR >::isAggregate(i_elt);

      if (!(is_attr || is_agg)) {
        GUM_ERROR(OperationNotAllowed, "Class does not respect it's interface")
      }

      if (!elt->type().isSubTypeOf(i_elt.type())) {
        GUM_ERROR(OperationNotAllowed, "Attribute type does not respect class interface")
      }

      if (elt->type() != i_elt.type()) {
        if (!this->exists(i_elt.safeName())) {
          GUM_ERROR(OperationNotAllowed, "Attribute type does not respect class interface")
        }
        elt = &(this->get(i_elt.safeName()));
      }

      // Node must be reserved by constructor
      if (!_dag_.existsNode(i_elt.id())) {
        GUM_ERROR(FatalError, "Class does not reserved implemented nodes")
      }

      // Removing unused node and changing to proper node
      if (elt->id() != i_elt.id()) {
        // Update cast descendants
        for (auto child: _dag_.children(elt->id())) {
          _dag_.addArc(i_elt.id(), child);
        }
        _dag_.eraseNode(elt->id());
      }
      _nodeIdMap_.erase(elt->id());
      elt->setId(i_elt.id());
      _nodeIdMap_.insert(elt->id(), elt);
    }

    template < typename GUM_SCALAR >
    void PRMClass< GUM_SCALAR >::_checkRefInterfaces_(PRMReferenceSlot< GUM_SCALAR >* ref) {
      try {
        for (auto i: implements()) {
          if (i->exists(ref->name())) { _checkRefInterface_(ref, i); }
        }
      } catch (NotFound const&) {
        // No interface to check
      }
    }

    template < typename GUM_SCALAR >
    void PRMClass< GUM_SCALAR >::_checkRefInterface_(PRMReferenceSlot< GUM_SCALAR >* ref,
                                                     PRMInterface< GUM_SCALAR >*     i) {
      auto& i_elt = i->get(ref->name());
      if (i_elt.elt_type() != ref->elt_type()) {
        GUM_ERROR(OperationNotAllowed, "Class does not respect it's interface")
      }
      auto& i_ref = static_cast< PRMReferenceSlot< GUM_SCALAR >& >(i_elt);
      if (!ref->slotType().isSubTypeOf(i_ref.slotType())) {
        GUM_ERROR(OperationNotAllowed, "ReferenceSlot type does not respect class interface")
      }
      // Node must be reserved by constructor
      if (!_dag_.exists(i_ref.id())) {
        GUM_ERROR(FatalError,
                  "class " << this->name() << " does not respect interface " << i->name()
                           << " implementation");
      }
      // Removing unused node and changin to propoer node
      if (ref->id() != i_ref.id()) { _dag_.eraseNode(ref->id()); }
      _nodeIdMap_.erase(ref->id());
      ref->setId(i_ref.id());
      _nodeIdMap_.insert(ref->id(), ref);
    }

    template < typename GUM_SCALAR >
    NodeId PRMClass< GUM_SCALAR >::add(PRMClassElement< GUM_SCALAR >* elt) {
      if (_nameMap_.exists(elt->name())) {
        GUM_ERROR(DuplicateElement,
                  "name " << elt->name() << " already used by another ClassElement");
      }

      elt->setId(nextNodeId());
      _dag_.addNodeWithId(elt->id());
      _nodeIdMap_.insert(elt->id(), elt);
      _nameMap_.insert(elt->name(), elt);

      try {
        _nameMap_.insert(elt->safeName(), elt);
      } catch (DuplicateElement& e) {
        if (!(PRMClassElement< GUM_SCALAR >::isSlotChain(*elt)
              || PRMClassElement< GUM_SCALAR >::isParameter(*elt))) {
          throw DuplicateElement(e);
        }
      }

      switch (elt->elt_type()) {
        case PRMClassElement< GUM_SCALAR >::prm_attribute: {
          _attributes_.insert(static_cast< PRMAttribute< GUM_SCALAR >* >(elt));
          _addCastDescendants_(static_cast< PRMAttribute< GUM_SCALAR >* >(elt));

          // Update attribute or cast descendant id to respect implemented
          // interface
          _checkInterfaces_(elt);

          _addIOInterfaceFlags_(elt);
          break;
        }

        case PRMClassElement< GUM_SCALAR >::prm_aggregate: {
          _aggregates_.insert(static_cast< PRMAggregate< GUM_SCALAR >* >(elt));
          _addCastDescendants_(static_cast< PRMAttribute< GUM_SCALAR >* >(elt));

          // Update attribute or cast descendant id to respect implemented
          // interface
          _checkInterfaces_(elt);

          _addIOInterfaceFlags_(elt);
          break;
        }

        case PRMClassElement< GUM_SCALAR >::prm_refslot: {
          auto ref = static_cast< PRMReferenceSlot< GUM_SCALAR >* >(elt);
          _referenceSlots_.insert(ref);

          // Updating ref's id if ref implements an interface
          _checkRefInterfaces_(ref);
          break;
        }

        case PRMClassElement< GUM_SCALAR >::prm_slotchain: {
          _slotChains_.insert(static_cast< PRMSlotChain< GUM_SCALAR >* >(elt));
          break;
        }

        case PRMClassElement< GUM_SCALAR >::prm_parameter: {
          _parameters_.insert(static_cast< PRMParameter< GUM_SCALAR >* >(elt));
          break;
        }

        default: {
          GUM_ERROR(FatalError, "unknown ClassElement<GUM_SCALAR> type")
        }
      }

      return elt->id();
    }

    template < typename GUM_SCALAR >
    void PRMClass< GUM_SCALAR >::_addCastDescendants_(PRMClassElement< GUM_SCALAR >* attr) {
      auto                        parent = attr;
      PRMAttribute< GUM_SCALAR >* child  = 0;

      while (parent->type().isSubType()) {
        child = parent->getCastDescendant();

        // Check if id was reserved by one of the class interfaces
        bool found = false;
        try {
          for (auto i: implements()) {
            if (i->exists(child->safeName())) {
              child->setId(i->get(child->safeName()).id());
              found = true;
              break;
            }
          }
        } catch (NotFound const&) {
          // No interface
        }
        if (!found) {
          child->setId(nextNodeId());
          _dag_.addNodeWithId(child->id());
        }
        _nodeIdMap_.insert(child->id(), child);
        // Only use child's safe name when adding to the name map!
        _nameMap_.insert(child->safeName(), child);
        _attributes_.insert(child);
        // Do not use Class<GUM_SCALAR>::insertArc(), child's CPF is already
        // initialized properly
        _dag_.addArc(parent->id(), child->id());

        parent = child;
      }
    }

    template < typename GUM_SCALAR >
    NodeId PRMClass< GUM_SCALAR >::overload(PRMClassElement< GUM_SCALAR >* overloader) {
      try {
        if (!super().exists(overloader->name())) {
          GUM_ERROR(OperationNotAllowed, "found no ClassElement<GUM_SCALAR> to overload")
        }
      } catch (NotFound const&) {
        GUM_ERROR(OperationNotAllowed, "overload is possible only with subclasses")
      }

      PRMClassElement< GUM_SCALAR >* overloaded = _nameMap_[overloader->name()];
      if (overloaded == overloader) {
        GUM_ERROR(DuplicateElement, "dupplicate ClassElement " << overloader->name())
      }
      // Checking overload legality
      if (!_checkOverloadLegality_(overloaded, overloader)) {
        GUM_ERROR(OperationNotAllowed, "illegal overload")
      }

      switch (overloader->elt_type()) {
        case PRMClassElement< GUM_SCALAR >::prm_attribute: {
          auto overloader_attr = static_cast< PRMAttribute< GUM_SCALAR >* >(overloader);
          auto overloaded_attr = static_cast< PRMAttribute< GUM_SCALAR >* >(overloaded);
          _overloadAttribute_(overloader_attr, overloaded_attr);
          _addIOInterfaceFlags_(overloader);
          break;
        }

        case PRMClassElement< GUM_SCALAR >::prm_aggregate: {
          _overloadAggregate_(static_cast< PRMAggregate< GUM_SCALAR >* >(overloader), overloaded);
          _addIOInterfaceFlags_(overloader);
          break;
        }

        case PRMClassElement< GUM_SCALAR >::prm_refslot: {
          //  _checkOverloadLegality_ guaranties that overloaded is a
          // PRMReferenceSlot<GUM_SCALAR>
          auto overloader_ref = static_cast< PRMReferenceSlot< GUM_SCALAR >* >(overloader);
          auto overloaded_ref = static_cast< PRMReferenceSlot< GUM_SCALAR >* >(overloaded);
          _overloadReference_(overloader_ref, overloaded_ref);
          break;
        }

        case PRMClassElement< GUM_SCALAR >::prm_slotchain: {
          GUM_ERROR(OperationNotAllowed, "SlotChain<GUM_SCALAR> can not be overloaded")
          break;
        }

        case PRMClassElement< GUM_SCALAR >::prm_parameter: {
          auto overloaded_param = static_cast< PRMParameter< GUM_SCALAR >* >(overloaded);
          auto overloader_param = static_cast< PRMParameter< GUM_SCALAR >* >(overloader);
          _overloadParameter_(overloader_param, overloaded_param);
          break;
        }
        default: {
          GUM_ERROR(OperationNotAllowed, "unknown ClassElement<GUM_SCALAR> type")
        }
      }

      return overloader->id();
    }

    template < typename GUM_SCALAR >
    void PRMClass< GUM_SCALAR >::_overloadAttribute_(PRMAttribute< GUM_SCALAR >* overloader,
                                                     PRMAttribute< GUM_SCALAR >* overloaded) {
      _dag_.eraseParents(overloaded->id());

      // Checking if we have to add cast descendant
      if (overloader->type() != overloaded->type()) {
        overloader->setId(nextNodeId());
        _dag_.addNodeWithId(overloader->id());
        _nodeIdMap_.insert(overloader->id(), overloader);
        _nameMap_[overloader->name()] = overloader;
        _nameMap_.insert(overloader->safeName(), overloader);
        _attributes_.insert(overloader);
        _addCastDescendants_(overloader, overloaded);
      } else {
        overloader->setId(overloaded->id());
        _nodeIdMap_[overloader->id()]     = overloader;
        _nameMap_[overloader->name()]     = overloader;
        _nameMap_[overloader->safeName()] = overloader;
        _attributes_.erase(overloaded);
        _attributes_.insert(overloader);
        overloader->overload(overloaded);
        delete overloaded;
      }
    }

    template < typename GUM_SCALAR >
    void PRMClass< GUM_SCALAR >::_overloadReference_(PRMReferenceSlot< GUM_SCALAR >* overloader,
                                                     PRMReferenceSlot< GUM_SCALAR >* overloaded) {
      // Adding overloading reference
      overloader->setId(overloaded->id());
      _nodeIdMap_[overloader->id()] = overloader;
      _nameMap_[overloader->name()] = overloader;
      _nameMap_.insert(overloader->safeName(), overloader);
      _referenceSlots_.insert(overloader);
      PRMSlotChain< GUM_SCALAR >*                sc   = 0;
      PRMReferenceSlot< GUM_SCALAR >*            ref  = 0;
      PRMClassElement< GUM_SCALAR >*             next = 0;
      std::vector< PRMSlotChain< GUM_SCALAR >* > toRemove, toAdd;

      // Updating PRMSlotChain<GUM_SCALAR> which started with overloaded
      for (const auto slotchain: _slotChains_) {
        // If the attribute pointed by this slotchain is overloaded, we need to
        // change the slotchain
        // names to it's safename version: ref.attr is replaced by
        // ref.(type)attr.
        if ((slotchain->chain().atPos(0) == overloaded)) {
          Sequence< PRMClassElement< GUM_SCALAR >* > seq;
          seq.insert(overloader);

          auto elt = ++(slotchain->chain().begin());

          while (elt != slotchain->chain().end()) {
            ref  = static_cast< PRMReferenceSlot< GUM_SCALAR >* >(seq.back());
            next = &(ref->slotType().get((*elt)->name()));
            seq.insert(next);
            ++elt;
          }

          // If the slotchain last element type changes, we change the slotchain
          // to
          // point towards the cast decendant
          // with the correct type
          if (seq.back()->type() != slotchain->lastElt().type()) {
            seq.erase(seq.back());
            seq.insert(&(static_cast< PRMReferenceSlot< GUM_SCALAR >* >(seq.back())
                            ->slotType()
                            .get(slotchain->lastElt().safeName())));
            std::string sc_name;
            std::string dot = ".";

            for (Size i = 0; i < seq.size() - 1; ++i) {
              sc_name += seq.atPos(i)->name() + dot;
            }

            sc_name += seq.back()->safeName();
            sc = new PRMSlotChain< GUM_SCALAR >(sc_name, seq);
            sc->setId(slotchain->id());

            for (const auto child: this->containerDag().children(sc->id())) {
              auto& elt = get(child);
              if (PRMClassElement< GUM_SCALAR >::isAttribute(elt)) {
                auto& attr     = static_cast< PRMAttribute< GUM_SCALAR >& >(elt);
                auto& old_type = slotchain->lastElt().type();
                auto& new_type = sc->lastElt().type();
                attr.swap(old_type, new_type);
              } else {
                GUM_ERROR(OperationNotAllowed, "unexpected ClassElement")
                // get( child ).cpf().replace(
                // slotchain->lastElt().type().variable(),
                // sc->lastElt().type().variable() );
              }
            }

            toAdd.push_back(sc);
            toRemove.push_back(slotchain);
          } else {
            // Types are identical, we just need to change the first reference
            slotchain->chain().setAtPos(0, overloader);
          }
        }
      }

      for (const auto torem: toRemove) {
        _nameMap_.erase(torem->name());
        _slotChains_.erase(torem);
        delete torem;
      }

      for (const auto toadd: toAdd) {
        _nameMap_.insert(toadd->name(), toadd);
        _nodeIdMap_[toadd->id()] = toadd;
        _slotChains_.insert(sc);
      }

      // Removing overloaded PRMReferenceSlot<GUM_SCALAR>
      _referenceSlots_.erase(overloaded);
      _nameMap_.erase(overloaded->safeName());
      delete overloaded;
    }

    template < typename GUM_SCALAR >
    void PRMClass< GUM_SCALAR >::_overloadParameter_(PRMParameter< GUM_SCALAR >* overloader,
                                                     PRMParameter< GUM_SCALAR >* overloaded) {
      overloader->setId(overloaded->id());
      _nodeIdMap_[overloader->id()]     = overloader;
      _nameMap_[overloader->name()]     = overloader;
      _nameMap_[overloader->safeName()] = overloader;
      _parameters_.erase(overloaded);
      _parameters_.insert(overloader);
      delete overloaded;
    }

    template < typename GUM_SCALAR >
    void PRMClass< GUM_SCALAR >::_addCastDescendants_(PRMAttribute< GUM_SCALAR >* start,
                                                      PRMAttribute< GUM_SCALAR >* end) {
      PRMAttribute< GUM_SCALAR >* parent = start;
      PRMAttribute< GUM_SCALAR >* child  = 0;

      while (parent->type().superType() != end->type()) {
        child = parent->getCastDescendant();
        child->setId(nextNodeId());
        _nodeIdMap_.insert(child->id(), child);
        _dag_.addNodeWithId(child->id());
        // Only use child's safe name when adding to the name map!
        _nameMap_.insert(child->safeName(), child);
        _attributes_.insert(child);
        _addIOInterfaceFlags_(child);
        // Do not use Class<GUM_SCALAR>::insertArc(), child's CPF is already
        // initialized properly
        _dag_.addArc(parent->id(), child->id());
        parent = child;
      }

      parent->setAsCastDescendant(end);
      _dag_.addArc(parent->id(), end->id());
    }

    template < typename GUM_SCALAR >
    void PRMClass< GUM_SCALAR >::findAllSubtypes_(
       Set< PRMClassElementContainer< GUM_SCALAR >* >& set) {
      for (auto ext: _extensions_) {
        set.insert(ext);
        ext->findAllSubtypes_(set);
      }
    }

    template < typename GUM_SCALAR >
    void PRMClass< GUM_SCALAR >::_addIOInterfaceFlags_(PRMClassElement< GUM_SCALAR >* elt) {
      // We only add IO Flags if elt matches is required by and interface
      if (_implements_ != nullptr) {
        for (const auto impl: *_implements_) {
          PRMInterface< GUM_SCALAR >* super = impl;
          while (super) {
            // If the attribute is defined in an interface, we set it as an
            // OutputNode
            if (impl->exists(elt->name())) {
              try {
                this->getIOFlag_(*elt).second = true;
              } catch (NotFound const&) { this->setIOFlag_(*elt, std::make_pair(false, true)); }
            }
            try {
              super = &(super->super());
            } catch (NotFound const&) { super = nullptr; }
          }
        }
      }
    }

    template < typename GUM_SCALAR >
    void PRMClass< GUM_SCALAR >::updateDescendants_(const PRMClassElement< GUM_SCALAR >& elt) {
      // for ( const auto ext :  _extensions_ ) {
      //  // We test to prevent unnecessary recursive call from iter
      //  if ( !ext->isOutputNode( elt ) ) {
      //    ext->setOutputNode( elt, true );
      //  }
      //}
    }

    template < typename GUM_SCALAR >
    INLINE typename PRMObject::prm_type PRMClass< GUM_SCALAR >::obj_type() const {
      return PRMObject::prm_type::CLASS;
    }

    template < typename GUM_SCALAR >
    INLINE const DAG& PRMClass< GUM_SCALAR >::dag_() const {
      return _dag_;
    }

    template < typename GUM_SCALAR >
    INLINE DAG& PRMClass< GUM_SCALAR >::dag_() {
      return _dag_;
    }

    template < typename GUM_SCALAR >
    INLINE PRMClassElement< GUM_SCALAR >& PRMClass< GUM_SCALAR >::get(NodeId id) {
      try {
        return *(_nodeIdMap_[id]);
      } catch (NotFound const&) {
        GUM_ERROR(NotFound, "no ClassElement<GUM_SCALAR> with the given NodeId")
      }
    }

    template < typename GUM_SCALAR >
    INLINE const PRMClassElement< GUM_SCALAR >& PRMClass< GUM_SCALAR >::get(NodeId id) const {
      try {
        return *(_nodeIdMap_[id]);
      } catch (NotFound const&) {
        GUM_ERROR(NotFound, "no ClassElement<GUM_SCALAR> with the given NodeId (" << id << ")");
      }
    }

    template < typename GUM_SCALAR >
    INLINE PRMClassElement< GUM_SCALAR >& PRMClass< GUM_SCALAR >::get(const std::string& name) {
      try {
        return *(_nameMap_[name]);
      } catch (NotFound const&) {
        GUM_ERROR(NotFound, "no ClassElement<GUM_SCALAR> with the given name (" << name << ")");
      }
    }

    template < typename GUM_SCALAR >
    INLINE const PRMClassElement< GUM_SCALAR >&
                 PRMClass< GUM_SCALAR >::get(const std::string& name) const {
      try {
        return *(_nameMap_[name]);
      } catch (NotFound const&) {
        GUM_ERROR(NotFound, "no ClassElement<GUM_SCALAR> with the given name (" << name << ")");
      }
    }

    template < typename GUM_SCALAR >
    INLINE const Set< PRMAttribute< GUM_SCALAR >* >& PRMClass< GUM_SCALAR >::attributes() const {
      return _attributes_;
    }

    template < typename GUM_SCALAR >
    INLINE const Set< PRMParameter< GUM_SCALAR >* >& PRMClass< GUM_SCALAR >::parameters() const {
      return _parameters_;
    }

    // Private struct for retrieving all params in scope
    template < typename GUM_SCALAR >
    struct ParamScopeData {
      std::string                   prefix;
      const PRMClass< GUM_SCALAR >* c;
      Idx                           depth;

      ParamScopeData(const std::string& s, const PRMReferenceSlot< GUM_SCALAR >& ref, Idx d) :
          prefix(s + ref.name() + "."),
          c(static_cast< const PRMClass< GUM_SCALAR >* >(&(ref.slotType()))), depth(d) {}
    };

    template < typename GUM_SCALAR >
    INLINE HashTable< std::string, const PRMParameter< GUM_SCALAR >* >
           PRMClass< GUM_SCALAR >::scope() const {
      HashTable< std::string, const PRMParameter< GUM_SCALAR >* > params;

      for (const auto p: parameters()) {
        params.insert(p->name(), p);
      }

      std::queue< ParamScopeData< GUM_SCALAR > > queue;

      for (const auto ref: referenceSlots()) {
        if (PRMObject::isClass(ref->slotType())) {
          queue.push(ParamScopeData< GUM_SCALAR >("", *ref, 1));
        }
      }

      while (!queue.empty()) {
        auto data = queue.front();
        queue.pop();

        if (data.depth < 5) {
          for (const auto p: data.c->parameters()) {
            params.insert(data.prefix + p->name(), p);
          }

          for (const auto ref: data.c->referenceSlots()) {
            if (PRMObject::isClass(ref->slotType())) {
              queue.push(ParamScopeData< GUM_SCALAR >(data.prefix, *ref, data.depth + 1));
            }
          }
        } else {
          // @todo depth>5 is a workaround. Cycle detection is needed here !
          GUM_ERROR(FatalError, "Depth limit reached when looking up parameters")
        }
      }

      return params;
    }

    template < typename GUM_SCALAR >
    INLINE const Set< PRMAggregate< GUM_SCALAR >* >& PRMClass< GUM_SCALAR >::aggregates() const {
      return _aggregates_;
    }

    template < typename GUM_SCALAR >
    INLINE const Set< PRMReferenceSlot< GUM_SCALAR >* >&
                 PRMClass< GUM_SCALAR >::referenceSlots() const {
      return _referenceSlots_;
    }

    template < typename GUM_SCALAR >
    INLINE const Set< PRMSlotChain< GUM_SCALAR >* >& PRMClass< GUM_SCALAR >::slotChains() const {
      return _slotChains_;
    }

    template < typename GUM_SCALAR >
    INLINE const PRMClass< GUM_SCALAR >& PRMClass< GUM_SCALAR >::super() const {
      if (_superClass_) {
        return *_superClass_;
      } else {
        GUM_ERROR(NotFound, "this Class is not a subclass")
      }
    }

    template < typename GUM_SCALAR >
    INLINE const Set< PRMInterface< GUM_SCALAR >* >& PRMClass< GUM_SCALAR >::implements() const {
      if (_implements_) {
        return *_implements_;
      } else {
        GUM_ERROR(NotFound, "this Class does not implement any Interface<GUM_SCALAR>")
      }
    }

    template < typename GUM_SCALAR >
    INLINE PRMClassElement< GUM_SCALAR >& PRMClass< GUM_SCALAR >::operator[](NodeId id) {
      return get(id);
    }

    template < typename GUM_SCALAR >
    INLINE const PRMClassElement< GUM_SCALAR >&
       PRMClass< GUM_SCALAR >::operator[](NodeId id) const {
      return get(id);
    }

    template < typename GUM_SCALAR >
    INLINE PRMClassElement< GUM_SCALAR >&
       PRMClass< GUM_SCALAR >::operator[](const std::string& name) {
      return get(name);
    }

    template < typename GUM_SCALAR >
    INLINE const PRMClassElement< GUM_SCALAR >&
       PRMClass< GUM_SCALAR >::operator[](const std::string& name) const {
      return get(name);
    }

    template < typename GUM_SCALAR >
    INLINE void
       PRMClass< GUM_SCALAR >::_overloadAggregate_(PRMAggregate< GUM_SCALAR >*    overloader,
                                                   PRMClassElement< GUM_SCALAR >* overloaded) {
      _nameMap_.insert(overloader->safeName(), overloader);
      _aggregates_.insert(overloader);
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMClass< GUM_SCALAR >::_checkOverloadLegality_(
       const PRMClassElement< GUM_SCALAR >* overloaded,
       const PRMClassElement< GUM_SCALAR >* overloader) {
      if (overloaded->elt_type() != overloader->elt_type()) { return false; }

      switch (overloaded->elt_type()) {
        case PRMClassElement< GUM_SCALAR >::prm_attribute: {
          if (!overloader->type().isSubTypeOf(overloaded->type())) { return false; }
          break;
        }

        case PRMClassElement< GUM_SCALAR >::prm_refslot: {
          const auto& new_slot_type
             = static_cast< const PRMReferenceSlot< GUM_SCALAR >* >(overloader)->slotType();
          const auto& old_slot_type
             = static_cast< const PRMReferenceSlot< GUM_SCALAR >* >(overloaded)->slotType();

          if (!new_slot_type.isSubTypeOf(old_slot_type)) { return false; }

          break;
        }

        case PRMClassElement< GUM_SCALAR >::prm_parameter: {
          auto overloaded_param = static_cast< const PRMParameter< GUM_SCALAR >* >(overloaded);
          auto overloader_param = static_cast< const PRMParameter< GUM_SCALAR >* >(overloader);

          return overloaded_param->valueType() == overloader_param->valueType();
          break;
        }

        default: {
          return false;
        }
      }
      return true;
    }

    template < typename GUM_SCALAR >
    INLINE const Set< PRMClass< GUM_SCALAR >* >& PRMClass< GUM_SCALAR >::extensions() const {
      return _extensions_;
    }

    template < typename GUM_SCALAR >
    INLINE void PRMClass< GUM_SCALAR >::_addExtension_(PRMClass< GUM_SCALAR >* c) {
      _extensions_.insert(c);
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMClass< GUM_SCALAR >::isCastDescendant(const std::string& safe_name) const {
      const PRMClassElement< GUM_SCALAR >& elt = get(safe_name);

      try {
        return elt.type().name() == get(elt.name()).type().name();
      } catch (OperationNotAllowed const&) {
        GUM_ERROR(NotFound, "no attribute with the given name")
      }
    }

    template < typename GUM_SCALAR >
    INLINE bool
       PRMClass< GUM_SCALAR >::isOutputNode(const PRMClassElement< GUM_SCALAR >& elt) const {
      try {
        if (!this->getIOFlag_(elt).second) {
          if (_implements_) {
            for (auto i: *_implements_) {
              if (i->isOutputNode(elt)) { return true; }
            }
          }

          if (_superClass_ && (_superClass_->isOutputNode(elt))) { return true; }

        } else {
          return true;
        }
      } catch (NotFound const&) {}
      return false;
    }


  } /* namespace prm */
} /* namespace gum */
