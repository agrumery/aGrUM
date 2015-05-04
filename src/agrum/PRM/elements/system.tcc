/**************************************************************************
*   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
*   {prenom.nom}_at_lip6.fr                                               *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/
/**
 * @file
 * @brief Inline implementation of System.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 *
 */
#include <agrum/PRM/elements/system.h>

#include <agrum/PRM/elements/instance.h>

#include <agrum/multidim/aggregators/min.h>
#include <agrum/multidim/aggregators/max.h>
#include <agrum/multidim/aggregators/exists.h>
#include <agrum/multidim/aggregators/forall.h>

namespace gum {
  namespace prm {
    template <typename GUM_SCALAR>
    System<GUM_SCALAR>::System(const std::string &name)
        : PRMObject(name) {
      GUM_CONSTRUCTOR(System);
    }

    template <typename GUM_SCALAR> System<GUM_SCALAR>::~System() {
      GUM_DESTRUCTOR(System);

      for (const auto &elt : *this)
        delete elt.second;

      for (const auto &elt : __instanceMap)
        delete elt.second;

      for (const auto &elt : __arrayMap)
        delete elt.second.second;
    }

    template <typename GUM_SCALAR>
    void System<GUM_SCALAR>::insertArc(const std::string &u_name,
                                       const std::string &v_name,
                                       const std::string &ref_name) {
      addArc(u_name, v_name, ref_name);
    }
    template <typename GUM_SCALAR>
    void System<GUM_SCALAR>::addArc(const std::string &u_name,
                                    const std::string &v_name,
                                    const std::string &ref_name) {
      Instance<GUM_SCALAR> *u = nullptr;
      Instance<GUM_SCALAR> *v = nullptr;
      ReferenceSlot<GUM_SCALAR> *ref = nullptr;

      try {
        u = __nameMap[u_name];
        v = __nameMap[v_name];
      } catch (NotFound &) {
        GUM_ERROR(NotFound, "found no instance matching the given names");
      }

      try {
        ClassElement<GUM_SCALAR> &elt = u->type().get(ref_name);

        if (elt.elt_type() == ClassElement<GUM_SCALAR>::prm_refslot) {
          ref = static_cast<ReferenceSlot<GUM_SCALAR> *>(&elt);
        } else {
          GUM_ERROR(WrongClassElement, "the given name does not match a "
                                       "ReferenceSlot<GUM_SCALAR> in u's type");
        }
      } catch (NotFound &) {
        GUM_ERROR(NotFound,
                  "found no ClassElement<GUM_SCALAR> matching the given name");
      }

      u->add(ref->id(), *v);
      __skeleton.addArc(__nodeIdMap.keyByVal(u), __nodeIdMap.keyByVal(v));
    }

    template <typename GUM_SCALAR>
    NodeId System<GUM_SCALAR>::add(Instance<GUM_SCALAR> *i) {
      if (__nameMap.exists(i->name())) {
        GUM_ERROR(DuplicateElement,
                  "an Instance<GUM_SCALAR> with the same is already in this System");
      }

      NodeId id = __skeleton.addNode();
      __nodeIdMap.insert(id, i);
      __nameMap.insert(i->name(), i);

      try {
        __instanceMap[&(i->type())]->insert(i);
      } catch (NotFound &) {
        __instanceMap.insert(&(i->type()), new Set<Instance<GUM_SCALAR> *>());
        __instanceMap[&(i->type())]->insert(i);
      }

      return id;
    }

    template <typename GUM_SCALAR>
    void System<GUM_SCALAR>::groundedBN(BayesNetFactory<GUM_SCALAR> &factory) const {
      factory.startNetworkDeclaration();
      factory.addNetworkProperty("name", name());
      factory.endNetworkDeclaration();

      // Adding nodes
      for (System<GUM_SCALAR>::const_iterator iter = begin(); iter != end();
           ++iter) {
        __groundAttr(*(iter.val()), factory);
      }

      // Adding arcs and filling CPTs
      for (System<GUM_SCALAR>::const_iterator iter = begin(); iter != end();
           ++iter) {
        __groundRef(*(iter.val()), factory);
      }
    }

    template <typename GUM_SCALAR>
    void
    System<GUM_SCALAR>::__groundAttr(const Instance<GUM_SCALAR> &instance,
                                     BayesNetFactory<GUM_SCALAR> &factory) const {
      for (const auto node : instance.type().dag()) {
        // Working a Class<GUM_SCALAR> level because Aggregate<GUM_SCALAR> are
        // instantiated as Attribute<GUM_SCALAR> in an Instance<GUM_SCALAR>
        switch (instance.type().get(node).elt_type()) {
          case ClassElement<GUM_SCALAR>::prm_attribute: {
            // TODO: make a special case for noisy-or
            std::stringstream elt_name;
            elt_name << instance.name() << "."
                     << instance.type().get(node).safeName();
            DiscreteVariable *var = instance.get(node).type().variable().clone();
            var->setName(elt_name.str());
            factory.setVariable(*var); // var is copied by the factory
            delete var;
            break;
          }

          case ClassElement<GUM_SCALAR>::prm_aggregate: {
            std::stringstream elt_name;
            elt_name << instance.name() << "."
                     << instance.type().get(node).safeName();
            __groundAgg(instance.type().get(node), elt_name.str(), factory);
            break;
          }

          default:
            break;
            /* Do nothing */;
        }
      }
    }

    template <typename GUM_SCALAR>
    void
    System<GUM_SCALAR>::__groundAgg(const ClassElement<GUM_SCALAR> &elt,
                                    const std::string &name,
                                    BayesNetFactory<GUM_SCALAR> &factory) const {
      factory.startVariableDeclaration();
      factory.variableName(name);
      const DiscreteVariable &agg_var = elt.type().variable();

      for (Idx i = 0; i < agg_var.domainSize(); ++i) {
        factory.addModality(agg_var.label(i));
      }

      const Aggregate<GUM_SCALAR> &agg =
          static_cast<const Aggregate<GUM_SCALAR> &>(elt);

      switch (agg.agg_type()) {
        case Aggregate<GUM_SCALAR>::AggregateType::MIN:
          factory.setVariableCPTImplementation(new aggregator::Min<GUM_SCALAR>());
          break;

        case Aggregate<GUM_SCALAR>::AggregateType::MAX:
          factory.setVariableCPTImplementation(new aggregator::Max<GUM_SCALAR>());
          break;

        case Aggregate<GUM_SCALAR>::AggregateType::EXISTS:
          factory.setVariableCPTImplementation(
              new aggregator::Exists<GUM_SCALAR>(agg.label()));
          break;

        case Aggregate<GUM_SCALAR>::AggregateType::FORALL:
          factory.setVariableCPTImplementation(
              new aggregator::Forall<GUM_SCALAR>(agg.label()));
          break;

        default:
          GUM_ERROR(OperationNotAllowed, "Aggregator not handled yet");
          break;
      }

      factory.endVariableDeclaration();
    }

    template <typename GUM_SCALAR>
    void
    System<GUM_SCALAR>::__groundRef(const Instance<GUM_SCALAR> &instance,
                                    BayesNetFactory<GUM_SCALAR> &factory) const {
      for (const auto &elt : instance) {
        std::stringstream elt_name;
        elt_name << instance.name() << "." << elt.second->safeName();
        factory.startParentsDeclaration(elt_name.str());

        for (const auto par : instance.type().dag().parents(elt.second->id())) {
          switch (instance.type().get(par).elt_type()) {
            case ClassElement<GUM_SCALAR>::prm_aggregate:
            case ClassElement<GUM_SCALAR>::prm_attribute: {
              std::stringstream parent_name;
              parent_name << instance.name() << "." << instance.get(par).safeName();
              factory.addParent(parent_name.str());
              break;
            }

            case ClassElement<GUM_SCALAR>::prm_slotchain: {
              std::string parent_name = static_cast<const SlotChain<GUM_SCALAR> &>(
                                            instance.type().get(par))
                                            .lastElt()
                                            .safeName();

              for (const auto ref : instance.getInstances(par)) {
                std::stringstream sBuff;
                sBuff << ref->name() << "." << parent_name;
                factory.addParent(sBuff.str());
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
        if (ClassElement<GUM_SCALAR>::isAttribute(
                instance.type().get(elt.second->safeName())))
          __groundPotential(instance, *elt.second, factory);
      }
    }

    template <typename GUM_SCALAR>
    void System<GUM_SCALAR>::__groundPotential(
        const Instance<GUM_SCALAR> &instance, const Attribute<GUM_SCALAR> &attr,
        BayesNetFactory<GUM_SCALAR> &factory) const {
      Bijection<const DiscreteVariable *, const DiscreteVariable *> bijection;
      std::stringstream var_name;
      var_name << instance.name() << "." << attr.safeName();
      bijection.insert(&(attr.type().variable()),
                       &(factory.variable(var_name.str())));

      for (const auto parent : instance.type().dag().parents(attr.id())) {
        switch (instance.type().get(parent).elt_type()) {
          case ClassElement<GUM_SCALAR>::prm_aggregate:
          case ClassElement<GUM_SCALAR>::prm_attribute: {
            std::stringstream parent_name;
            parent_name << instance.name() << "." << instance.get(parent).safeName();
            bijection.insert(&(instance.get(parent).type().variable()),
                             &(factory.variable(parent_name.str())));
            break;
          }

          case ClassElement<GUM_SCALAR>::prm_slotchain: {
            std::stringstream parent_name;
            const SlotChain<GUM_SCALAR> &sc =
                static_cast<const SlotChain<GUM_SCALAR> &>(
                    instance.type().get(parent));
            parent_name << instance.getInstance(sc.id()).name() << "."
                        << sc.lastElt().safeName();
            bijection.insert(&(instance.getInstance(sc.id())
                                   .get(sc.lastElt().safeName())
                                   .type()
                                   .variable()),
                             &(factory.variable(parent_name.str())));
            break;
          }

          default: {
            GUM_ERROR(FatalError,
                      "invalid ClassElement<GUM_SCALAR> type as parent.");
            break;
          }
        }
      }

      // This should be optimized (using MultiDimBijArray and by handling noisy-or)
      Potential<GUM_SCALAR> *p = copyPotential(bijection, attr.cpf());
      factory.setVariableCPT(var_name.str(), p, false);
    }

    template <typename GUM_SCALAR>
    INLINE NodeId
        System<GUM_SCALAR>::add(const std::string &array, Instance<GUM_SCALAR> &i) {
      return add(array, &i);
    }

    template <typename GUM_SCALAR>
    INLINE const DiGraph &System<GUM_SCALAR>::skeleton() const {
      return __skeleton;
    }

    template <typename GUM_SCALAR>
    INLINE Instance<GUM_SCALAR> &System<GUM_SCALAR>::get(NodeId id) {
      try {
        return *(__nodeIdMap[id]);
      } catch (NotFound &) {
        GUM_ERROR(NotFound, "found no Instance<GUM_SCALAR> matching the given id");
      }
    }

    template <typename GUM_SCALAR>
    INLINE const Instance<GUM_SCALAR> &System<GUM_SCALAR>::get(NodeId id) const {
      try {
        return *(__nodeIdMap[id]);
      } catch (NotFound &) {
        GUM_ERROR(NotFound, "found no Instance<GUM_SCALAR> matching the given id");
      }
    }

    template <typename GUM_SCALAR>
    INLINE PRMObject::PRMType System<GUM_SCALAR>::obj_type() const {
      return PRMObject::PRMType::SYSTEM;
    }

    template <typename GUM_SCALAR> INLINE Size System<GUM_SCALAR>::size() const {
      return __nodeIdMap.size();
    }

    template <typename GUM_SCALAR>
    INLINE bool
    System<GUM_SCALAR>::isInstantiated(const Class<GUM_SCALAR> &c) const {
      return __instanceMap.exists(const_cast<Class<GUM_SCALAR> *>(&c));
    }

    template <typename GUM_SCALAR>
    INLINE bool System<GUM_SCALAR>::isInstance(const std::string &name) const {
      return __nameMap.exists(name);
    }

    template <typename GUM_SCALAR>
    INLINE bool System<GUM_SCALAR>::isArray(const std::string &name) const {
      return __arrayMap.exists(name);
    }

    template <typename GUM_SCALAR> INLINE void System<GUM_SCALAR>::instantiate() {
      for (auto iter = begin(); iter != end(); ++iter) {
        (*(iter.val())).instantiate();
      }
    }

    template <typename GUM_SCALAR>
    INLINE Instance<GUM_SCALAR> &System<GUM_SCALAR>::get(const std::string &name) {
      try {
        return *(__nameMap[name]);
      } catch (NotFound &) {
        GUM_ERROR(NotFound, "found no Instance<GUM_SCALAR> matching the given name");
      }
    }

    template <typename GUM_SCALAR>
    INLINE const Instance<GUM_SCALAR> &
    System<GUM_SCALAR>::get(const std::string &name) const {
      try {
        return *(__nameMap[name]);
      } catch (NotFound &) {
        GUM_ERROR(NotFound, "found no Instance<GUM_SCALAR> matching the given name");
      }
    }

    template <typename GUM_SCALAR>
    INLINE const Set<Instance<GUM_SCALAR> *> &
    System<GUM_SCALAR>::get(const Class<GUM_SCALAR> &type) const {
      try {
        return *(__instanceMap[const_cast<Class<GUM_SCALAR> *>(&type)]);
      } catch (NotFound &) {
        GUM_ERROR(NotFound,
                  "the given Class<GUM_SCALAR> has no instantiation in this System");
      }
    }

    template <typename GUM_SCALAR>
    INLINE const Sequence<Instance<GUM_SCALAR> *> &
    System<GUM_SCALAR>::getArray(const std::string &name) const {
      try {
        return *(__arrayMap[name].second);
      } catch (NotFound &) {
        GUM_ERROR(NotFound, "found no array matching the given name");
      }
    }

    template <typename GUM_SCALAR>
    INLINE ClassElementContainer<GUM_SCALAR> &
    System<GUM_SCALAR>::getArrayType(const std::string &name) {
      try {
        return *(__arrayMap[name].first);
      } catch (NotFound &) {
        GUM_ERROR(NotFound, "found no array matching the given name");
      }
    }

    template <typename GUM_SCALAR>
    INLINE const ClassElementContainer<GUM_SCALAR> &
    System<GUM_SCALAR>::getArrayType(const std::string &name) const {
      try {
        return *(__arrayMap[name].first);
      } catch (NotFound &) {
        GUM_ERROR(NotFound, "found no array matching the given name");
      }
    }

    template <typename GUM_SCALAR>
    INLINE NodeId
        System<GUM_SCALAR>::add(const std::string &array, Instance<GUM_SCALAR> *i) {
      try {
        if (i->type().isSubTypeOf(*(__arrayMap[array].first))) {
          NodeId id = add(i);
          __arrayMap[array].second->insert(i);
          return id;
        } else {
          GUM_ERROR(TypeError, "the given Instance<GUM_SCALAR> is of an incorrect "
                               "Class<GUM_SCALAR> type");
        }
      } catch (NotFound &) {
        GUM_ERROR(NotFound, "found no array matching the given name");
      }
    }

    template <typename GUM_SCALAR>
    INLINE void
    System<GUM_SCALAR>::addArray(const std::string &array,
                                 ClassElementContainer<GUM_SCALAR> &type) {
      if (__arrayMap.exists(array)) {
        GUM_ERROR(DuplicateElement,
                  "an array with the same is already in this System");
      }

      __arrayMap.insert(array, System<GUM_SCALAR>::model_pair(
                                   &type, new Sequence<Instance<GUM_SCALAR> *>()));
    }

    template <typename GUM_SCALAR>
    INLINE typename System<GUM_SCALAR>::iterator System<GUM_SCALAR>::begin() {
      return __nodeIdMap.begin();
    }

    template <typename GUM_SCALAR>
    INLINE const typename System<GUM_SCALAR>::iterator &System<GUM_SCALAR>::end() {
      return __nodeIdMap.end();
    }

    template <typename GUM_SCALAR>
    INLINE typename System<GUM_SCALAR>::const_iterator
    System<GUM_SCALAR>::begin() const {
      return __nodeIdMap.begin();
    }

    template <typename GUM_SCALAR>
    INLINE const typename System<GUM_SCALAR>::const_iterator &
    System<GUM_SCALAR>::end() const {
      return __nodeIdMap.end();
    }

    template <typename GUM_SCALAR>
    INLINE typename System<GUM_SCALAR>::array_iterator
    System<GUM_SCALAR>::begin(const std::string &a) {
      try {
        return __arrayMap[a].second->begin();
      } catch (NotFound &) {
        GUM_ERROR(NotFound, "found no array matching the given name");
      }
    }

    template <typename GUM_SCALAR>
    INLINE const typename System<GUM_SCALAR>::array_iterator &
    System<GUM_SCALAR>::end(const std::string &a) {
      try {
        return __arrayMap[a].second->end();
      } catch (NotFound &) {
        GUM_ERROR(NotFound, "found no array matching the given name");
      }
    }

    template <typename GUM_SCALAR>
    INLINE typename System<GUM_SCALAR>::const_array_iterator
    System<GUM_SCALAR>::begin(const std::string &a) const {
      try {
        return __arrayMap[a].second->begin();
      } catch (NotFound &) {
        GUM_ERROR(NotFound, "found no array matching the given name");
      }
    }

    template <typename GUM_SCALAR>
    INLINE const typename System<GUM_SCALAR>::const_array_iterator &
    System<GUM_SCALAR>::end(const std::string &a) const {
      try {
        return __arrayMap[a].second->end();
      } catch (NotFound &) {
        GUM_ERROR(NotFound, "found no array matching the given name");
      }
    }

    template <typename GUM_SCALAR>
    INLINE bool System<GUM_SCALAR>::exists(const std::string &name) const {
      return __nameMap.exists(name) or __arrayMap.exists(name);
    }

  } /* namespace prm */
} /* namespace gum */
