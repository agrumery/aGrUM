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
 * @brief Inline implementation of PRMSystem.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 *
 */
#include <agrum/PRM/elements/PRMSystem.h>

#include <agrum/PRM/elements/PRMInstance.h>

#include <agrum/tools/multidim/aggregators/exists.h>
#include <agrum/tools/multidim/aggregators/forall.h>
#include <agrum/tools/multidim/aggregators/max.h>
#include <agrum/tools/multidim/aggregators/min.h>

namespace gum {
  namespace prm {
    template < typename GUM_SCALAR >
    PRMSystem< GUM_SCALAR >::PRMSystem(const std::string& name) : PRMObject(name) {
      GUM_CONSTRUCTOR(PRMSystem);
    }

    template < typename GUM_SCALAR >
    PRMSystem< GUM_SCALAR >::~PRMSystem() {
      GUM_DESTRUCTOR(PRMSystem);

      for (const auto& elt: *this)
        delete elt.second;

      for (const auto& elt: instanceMap_)
        delete elt.second;

      for (const auto& elt: arrayMap_)
        delete elt.second.second;
    }

    template < typename GUM_SCALAR >
    INLINE PRMSystem< GUM_SCALAR >::PRMSystem(const PRMSystem& from) :
        PRMObject(from), skeleton_(from.skeleton_), nodeIdMap_(from.nodeIdMap_),
        nameMap_(from.nameMap_), instanceMap_(from.instanceMap_), arrayMap_(from.arrayMap_) {
      GUM_CONS_CPY(PRMSystem);
    }

    template < typename GUM_SCALAR >
    NodeId PRMSystem< GUM_SCALAR >::add(PRMInstance< GUM_SCALAR >* i) {
      if (nameMap_.exists(i->name())) {
        GUM_ERROR(DuplicateElement,
                  "an Instance<GUM_SCALAR> with the same is already in this System");
      }

      NodeId id = skeleton_.addNode();
      nodeIdMap_.insert(id, i);
      nameMap_.insert(i->name(), i);

      try {
        instanceMap_[&(i->type())]->insert(i);
      } catch (NotFound const&) {
        instanceMap_.insert(&(i->type()), new Set< PRMInstance< GUM_SCALAR >* >());
        instanceMap_[&(i->type())]->insert(i);
      }

      return id;
    }

    template < typename GUM_SCALAR >
    void PRMSystem< GUM_SCALAR >::groundedBN(BayesNetFactory< GUM_SCALAR >& factory) const {
      factory.startNetworkDeclaration();
      factory.addNetworkProperty("name", name());
      factory.endNetworkDeclaration();

      // Adding nodes
      for (PRMSystem< GUM_SCALAR >::const_iterator iter = begin(); iter != end(); ++iter) {
        _groundAttr_(*(iter.val()), factory);
      }

      // Adding arcs and filling CPTs
      for (PRMSystem< GUM_SCALAR >::const_iterator iter = begin(); iter != end(); ++iter) {
        _groundRef_(*(iter.val()), factory);
      }
    }

    template < typename GUM_SCALAR >
    void PRMSystem< GUM_SCALAR >::_groundAttr_(const PRMInstance< GUM_SCALAR >& instance,
                                               BayesNetFactory< GUM_SCALAR >&   factory) const {
      for (const auto node: instance.type().containerDag()) {
        // Working a Class<GUM_SCALAR> level because PRMAggregate<GUM_SCALAR>
        // are
        // instantiated as PRMAttribute<GUM_SCALAR> in an
        // PRMInstance<GUM_SCALAR>
        switch (instance.type().get(node).elt_type()) {
          case PRMClassElement< GUM_SCALAR >::prm_attribute: {
            // TODO: make a special case for noisy-or
            std::stringstream elt_name;
            elt_name << instance.name() << "." << instance.type().get(node).safeName();
            DiscreteVariable* var = instance.get(node).type().variable().clone();
            var->setName(elt_name.str());
            factory.setVariable(*var);   // var is copied by the factory
            delete var;
            break;
          }

          case PRMClassElement< GUM_SCALAR >::prm_aggregate: {
            std::stringstream elt_name;
            elt_name << instance.name() << "." << instance.type().get(node).safeName();
            _groundAgg_(instance.type().get(node), elt_name.str(), factory);
            break;
          }

          default:
            break;
            /* Do nothing */;
        }
      }
    }

    template < typename GUM_SCALAR >
    void PRMSystem< GUM_SCALAR >::_groundAgg_(const PRMClassElement< GUM_SCALAR >& elt,
                                              const std::string&                   name,
                                              BayesNetFactory< GUM_SCALAR >&       factory) const {
      factory.startVariableDeclaration();
      factory.variableName(name);

      const DiscreteVariable& agg_var = elt.type().variable();

      VarType var_type = agg_var.varType();

      factory.variableType(var_type);

      switch (var_type) {
        case VarType::Labelized: {
          const auto l = static_cast< const LabelizedVariable& >(agg_var);
          for (Idx i = 0; i < l.domainSize(); ++i) {
            factory.addModality(l.label(i));
          }
          break;
        }
        case VarType::Integer: {
          const auto l = static_cast< const IntegerVariable& >(agg_var);
          for (Idx i = 0; i < l.domainSize(); ++i) {
            factory.addModality(l.label(i));
          }
          break;
        }
        case VarType::Numerical: {
          const auto l = static_cast< const NumericalDiscreteVariable& >(agg_var);
          for (Idx i = 0; i < l.domainSize(); ++i) {
            factory.addModality(l.label(i));
          }
          break;
        }
        case VarType::Discretized: {
          const auto d = static_cast< const DiscretizedVariable< GUM_SCALAR >& >(agg_var);
          for (Idx i = 0; i < d.domainSize(); ++i) {
            factory.addTick(d.tick(i));
          }
          break;
        }
        case VarType::Range: {
          const auto r = static_cast< const RangeVariable& >(agg_var);
          factory.addMin(r.minVal());
          factory.addMax(r.maxVal());
          break;
        }
        case VarType::Continuous: {
          GUM_ERROR(NotImplementedYet,
                    "PRM aggregator grounding does not support yet continuous variables");
        }
      }

      const PRMAggregate< GUM_SCALAR >& agg = static_cast< const PRMAggregate< GUM_SCALAR >& >(elt);

      switch (agg.agg_type()) {
        case PRMAggregate< GUM_SCALAR >::AggregateType::MIN: {
          factory.setVariableCPTImplementation(new aggregator::Min< GUM_SCALAR >());
          break;
        }

        case PRMAggregate< GUM_SCALAR >::AggregateType::MAX: {
          factory.setVariableCPTImplementation(new aggregator::Max< GUM_SCALAR >());
          break;
        }

        case PRMAggregate< GUM_SCALAR >::AggregateType::EXISTS: {
          factory.setVariableCPTImplementation(new aggregator::Exists< GUM_SCALAR >(agg.label()));
          break;
        }

        case PRMAggregate< GUM_SCALAR >::AggregateType::FORALL: {
          factory.setVariableCPTImplementation(new aggregator::Forall< GUM_SCALAR >(agg.label()));
          break;
        }

        case PRMAggregate< GUM_SCALAR >::AggregateType::COUNT: {
          factory.setVariableCPTImplementation(new aggregator::Count< GUM_SCALAR >(agg.label()));
          break;
        }

        case PRMAggregate< GUM_SCALAR >::AggregateType::MEDIAN: {
          factory.setVariableCPTImplementation(new aggregator::Median< GUM_SCALAR >());
          break;
        }

        case PRMAggregate< GUM_SCALAR >::AggregateType::AMPLITUDE: {
          factory.setVariableCPTImplementation(new aggregator::Amplitude< GUM_SCALAR >());
          break;
        }

        case PRMAggregate< GUM_SCALAR >::AggregateType::OR: {
          factory.setVariableCPTImplementation(new aggregator::Or< GUM_SCALAR >());
          break;
        }

        case PRMAggregate< GUM_SCALAR >::AggregateType::AND: {
          factory.setVariableCPTImplementation(new aggregator::And< GUM_SCALAR >());
          break;
        }

        case PRMAggregate< GUM_SCALAR >::AggregateType::SUM: {
          factory.setVariableCPTImplementation(new aggregator::Sum< GUM_SCALAR >());
          break;
        }

        default: GUM_ERROR(OperationNotAllowed, "Aggregator not handled yet for " << agg.name())
      }

      factory.endVariableDeclaration();
    }

    template < typename GUM_SCALAR >
    void PRMSystem< GUM_SCALAR >::_groundRef_(const PRMInstance< GUM_SCALAR >& instance,
                                              BayesNetFactory< GUM_SCALAR >&   factory) const {
      for (const auto& elt: instance) {
        std::stringstream elt_name;
        elt_name << instance.name() << "." << elt.second->safeName();
        factory.startParentsDeclaration(elt_name.str());

        for (const auto par: instance.type().containerDag().parents(elt.second->id())) {
          switch (instance.type().get(par).elt_type()) {
            case PRMClassElement< GUM_SCALAR >::prm_aggregate:
            case PRMClassElement< GUM_SCALAR >::prm_attribute: {
              std::stringstream parent_name;
              parent_name << instance.name() << "." << instance.get(par).safeName();
              factory.addParent(parent_name.str());
              break;
            }

            case PRMClassElement< GUM_SCALAR >::prm_slotchain: {
              std::string parent_name
                 = static_cast< const PRMSlotChain< GUM_SCALAR >& >(instance.type().get(par))
                      .lastElt()
                      .safeName();

              try {
                for (const auto ref: instance.getInstances(par)) {
                  std::stringstream sBuff;
                  sBuff << ref->name() << "." << parent_name;
                  factory.addParent(sBuff.str());
                }
              } catch (NotFound const&) {
                // No instances for this slotchain
              }

              break;
            }

            default:
              break;
              /* nothing to do by default */
          }
        }

        factory.endParentsDeclaration();

        // Checking if we need to ground the Potential (only for class level
        // attributes since
        // aggregates Potentials are generated)
        if (PRMClassElement< GUM_SCALAR >::isAttribute(instance.type().get(elt.second->safeName())))
          _groundPotential_(instance, *elt.second, factory);
      }
    }

    template < typename GUM_SCALAR >
    void PRMSystem< GUM_SCALAR >::_groundPotential_(const PRMInstance< GUM_SCALAR >&  instance,
                                                    const PRMAttribute< GUM_SCALAR >& attr,
                                                    BayesNetFactory< GUM_SCALAR >& factory) const {
      Bijection< const DiscreteVariable*, const DiscreteVariable* > bijection;
      std::stringstream                                             var_name;
      var_name << instance.name() << "." << attr.safeName();
      bijection.insert(&(attr.type().variable()), &(factory.variable(var_name.str())));

      for (const auto parent: instance.type().containerDag().parents(attr.id())) {
        switch (instance.type().get(parent).elt_type()) {
          case PRMClassElement< GUM_SCALAR >::prm_aggregate:
          case PRMClassElement< GUM_SCALAR >::prm_attribute: {
            std::stringstream parent_name;
            parent_name << instance.name() << "." << instance.get(parent).safeName();
            bijection.insert(&(instance.get(parent).type().variable()),
                             &(factory.variable(parent_name.str())));
            break;
          }

          case PRMClassElement< GUM_SCALAR >::prm_slotchain: {
            std::stringstream                 parent_name;
            const PRMSlotChain< GUM_SCALAR >& sc
               = static_cast< const PRMSlotChain< GUM_SCALAR >& >(instance.type().get(parent));
            parent_name << instance.getInstance(sc.id()).name() << "." << sc.lastElt().safeName();
            bijection.insert(
               &(instance.getInstance(sc.id()).get(sc.lastElt().safeName()).type().variable()),
               &(factory.variable(parent_name.str())));
            break;
          }

          default: {
            GUM_ERROR(FatalError, "invalid ClassElement<GUM_SCALAR> type as parent.")
            break;
          }
        }
      }

      // Copy Potential
      // DO NOT USE MultiDimBijArray as they will wreck havok if you delete
      // the prm befor its grounded BN (happens a lot in pyAgrum)
      Potential< GUM_SCALAR >* p = new Potential< GUM_SCALAR >();
      for (auto var: attr.cpf().variablesSequence()) {
        p->add(*(bijection.second(var)));
      }
      Instantiation inst(attr.cpf()), jnst(*p);
      for (inst.setFirst(), jnst.setFirst(); !(inst.end() || jnst.end()); inst.inc(), jnst.inc()) {
        p->set(jnst, attr.cpf().get(inst));
      }
      GUM_ASSERT(inst.end() && jnst.end());
      factory.setVariableCPT(var_name.str(), p, false);
    }

    template < typename GUM_SCALAR >
    INLINE NodeId PRMSystem< GUM_SCALAR >::add(const std::string&         array,
                                               PRMInstance< GUM_SCALAR >& i) {
      return add(array, &i);
    }

    template < typename GUM_SCALAR >
    INLINE const DiGraph& PRMSystem< GUM_SCALAR >::skeleton() const {
      return skeleton_;
    }

    template < typename GUM_SCALAR >
    INLINE PRMInstance< GUM_SCALAR >& PRMSystem< GUM_SCALAR >::get(NodeId id) {
      try {
        return *(nodeIdMap_[id]);
      } catch (NotFound const&) {
        GUM_ERROR(NotFound, "found no Instance<GUM_SCALAR> matching the given id")
      }
    }

    template < typename GUM_SCALAR >
    INLINE const PRMInstance< GUM_SCALAR >& PRMSystem< GUM_SCALAR >::get(NodeId id) const {
      try {
        return *(nodeIdMap_[id]);
      } catch (NotFound const&) {
        GUM_ERROR(NotFound, "found no Instance<GUM_SCALAR> matching the given id")
      }
    }

    template < typename GUM_SCALAR >
    INLINE NodeId PRMSystem< GUM_SCALAR >::get(const PRMInstance< GUM_SCALAR >& i) const {
      try {
        return nodeIdMap_.keyByVal(const_cast< PRMInstance< GUM_SCALAR >* >(&i));
      } catch (NotFound const&) {
        GUM_ERROR(NotFound, "found no Instance<GUM_SCALAR> matching the given id")
      }
    }

    template < typename GUM_SCALAR >
    INLINE PRMObject::prm_type PRMSystem< GUM_SCALAR >::obj_type() const {
      return PRMObject::prm_type::SYSTEM;
    }

    template < typename GUM_SCALAR >
    INLINE Size PRMSystem< GUM_SCALAR >::size() const {
      return nodeIdMap_.size();
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMSystem< GUM_SCALAR >::isInstantiated(const PRMClass< GUM_SCALAR >& c) const {
      return instanceMap_.exists(const_cast< PRMClass< GUM_SCALAR >* >(&c));
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMSystem< GUM_SCALAR >::isInstance(const std::string& name) const {
      return nameMap_.exists(name);
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMSystem< GUM_SCALAR >::isArray(const std::string& name) const {
      return arrayMap_.exists(name);
    }

    template < typename GUM_SCALAR >
    INLINE void PRMSystem< GUM_SCALAR >::instantiate() {
      for (auto iter = begin(); iter != end(); ++iter) {
        (*(iter.val())).instantiate();
      }
    }

    template < typename GUM_SCALAR >
    INLINE PRMInstance< GUM_SCALAR >& PRMSystem< GUM_SCALAR >::get(const std::string& name) {
      try {
        return *(nameMap_[name]);
      } catch (NotFound const&) {
        GUM_ERROR(NotFound, "found no Instance<GUM_SCALAR> matching the given name")
      }
    }

    template < typename GUM_SCALAR >
    INLINE const PRMInstance< GUM_SCALAR >&
                 PRMSystem< GUM_SCALAR >::get(const std::string& name) const {
      try {
        return *(nameMap_[name]);
      } catch (NotFound const&) {
        GUM_ERROR(NotFound, "found no Instance<GUM_SCALAR> matching the given name")
      }
    }

    template < typename GUM_SCALAR >
    INLINE const Set< PRMInstance< GUM_SCALAR >* >&

       PRMSystem< GUM_SCALAR >::get(const PRMClass< GUM_SCALAR >& type) const {
      try {
        return *(instanceMap_[const_cast< PRMClass< GUM_SCALAR >* >(&type)]);
      } catch (NotFound const&) {
        GUM_ERROR(NotFound, "the given Class<GUM_SCALAR> has no instantiation in this System");
      }
    }

    template < typename GUM_SCALAR >
    INLINE const Sequence< PRMInstance< GUM_SCALAR >* >&

       PRMSystem< GUM_SCALAR >::getArray(const std::string& name) const {
      try {
        return *(arrayMap_[name].second);
      } catch (NotFound const&) { GUM_ERROR(NotFound, "found no array matching the given name") }
    }

    template < typename GUM_SCALAR >
    INLINE PRMClassElementContainer< GUM_SCALAR >&
           PRMSystem< GUM_SCALAR >::getArrayType(const std::string& name) {
      try {
        return *(arrayMap_[name].first);
      } catch (NotFound const&) { GUM_ERROR(NotFound, "found no array matching the given name") }
    }

    template < typename GUM_SCALAR >
    INLINE const PRMClassElementContainer< GUM_SCALAR >&
                 PRMSystem< GUM_SCALAR >::getArrayType(const std::string& name) const {
      try {
        return *(arrayMap_[name].first);
      } catch (NotFound const&) { GUM_ERROR(NotFound, "found no array matching the given name") }
    }

    template < typename GUM_SCALAR >
    INLINE NodeId PRMSystem< GUM_SCALAR >::add(const std::string&         array,
                                               PRMInstance< GUM_SCALAR >* i) {
      try {
        if (i->type().isSubTypeOf(*(arrayMap_[array].first))) {
          NodeId id = add(i);
          arrayMap_[array].second->insert(i);
          return id;
        } else {
          GUM_ERROR(PRMTypeError,
                    "the given Instance<GUM_SCALAR> is of an incorrect "
                    "Class<GUM_SCALAR> type");
        }
      } catch (NotFound const&) { GUM_ERROR(NotFound, "found no array matching the given name") }
    }

    template < typename GUM_SCALAR >
    INLINE void PRMSystem< GUM_SCALAR >::addArray(const std::string&                      array,
                                                  PRMClassElementContainer< GUM_SCALAR >& type) {
      if (arrayMap_.exists(array)) {
        GUM_ERROR(DuplicateElement, "an array '" << array << "' is already in this System")
      }

      arrayMap_.insert(
         array,
         PRMSystem< GUM_SCALAR >::model_pair(&type, new Sequence< PRMInstance< GUM_SCALAR >* >()));
    }

    template < typename GUM_SCALAR >
    INLINE typename PRMSystem< GUM_SCALAR >::iterator PRMSystem< GUM_SCALAR >::begin() {
      return nodeIdMap_.begin();
    }

    template < typename GUM_SCALAR >
    INLINE const typename PRMSystem< GUM_SCALAR >::iterator& PRMSystem< GUM_SCALAR >::end() {
      return nodeIdMap_.end();
    }

    template < typename GUM_SCALAR >
    INLINE typename PRMSystem< GUM_SCALAR >::const_iterator PRMSystem< GUM_SCALAR >::begin() const {
      return nodeIdMap_.begin();
    }

    template < typename GUM_SCALAR >
    INLINE const typename PRMSystem< GUM_SCALAR >::const_iterator&
       PRMSystem< GUM_SCALAR >::end() const {
      return nodeIdMap_.end();
    }

    template < typename GUM_SCALAR >
    INLINE typename PRMSystem< GUM_SCALAR >::array_iterator
       PRMSystem< GUM_SCALAR >::begin(const std::string& a) {
      try {
        return arrayMap_[a].second->begin();
      } catch (NotFound const&) { GUM_ERROR(NotFound, "found no array matching the given name") }
    }

    template < typename GUM_SCALAR >
    INLINE const typename PRMSystem< GUM_SCALAR >::array_iterator&
       PRMSystem< GUM_SCALAR >::end(const std::string& a) {
      try {
        return arrayMap_[a].second->end();
      } catch (NotFound const&) { GUM_ERROR(NotFound, "found no array matching the given name") }
    }

    template < typename GUM_SCALAR >
    INLINE typename PRMSystem< GUM_SCALAR >::const_array_iterator
       PRMSystem< GUM_SCALAR >::begin(const std::string& a) const {
      try {
        return arrayMap_[a].second->begin();
      } catch (NotFound const&) { GUM_ERROR(NotFound, "found no array matching the given name") }
    }

    template < typename GUM_SCALAR >
    INLINE const typename PRMSystem< GUM_SCALAR >::const_array_iterator&
       PRMSystem< GUM_SCALAR >::end(const std::string& a) const {
      try {
        return arrayMap_[a].second->end();
      } catch (NotFound const&) { GUM_ERROR(NotFound, "found no array matching the given name") }
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMSystem< GUM_SCALAR >::exists(const std::string& name) const {
      return nameMap_.exists(name) || arrayMap_.exists(name);
    }

  } /* namespace prm */
} /* namespace gum */
