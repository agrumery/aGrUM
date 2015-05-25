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
 * @brief Inline implementation of gum::Class
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

// to ease parsing
#include <agrum/PRM/elements/class.h>

#include <agrum/PRM/elements/interface.h>
#include <agrum/PRM/elements/aggregate.h>
#include <agrum/PRM/elements/slotChain.h>

namespace gum {
  namespace prm {
    template <typename GUM_SCALAR>
    Class<GUM_SCALAR>::Class(const std::string &name)
        : ClassElementContainer<GUM_SCALAR>(name), __super(0), __implements(0) {
      GUM_CONSTRUCTOR(Class);
    }

    template <typename GUM_SCALAR>
    Class<GUM_SCALAR>::Class(const std::string &name, Class<GUM_SCALAR> &super)
        : ClassElementContainer<GUM_SCALAR>(name), __dag(super.dag()),
          __super(&super), __implements(0),
          __instantiations(super.__instantiations) {
      GUM_CONSTRUCTOR(Class);
      super.__addExtension(this);
      __inheritClass(super);
    }

    template <typename GUM_SCALAR>
    Class<GUM_SCALAR>::Class(const std::string &name,
                             const Set<Interface<GUM_SCALAR> *> &set)
        : ClassElementContainer<GUM_SCALAR>(name), __super(0),
          __implements(new Set<Interface<GUM_SCALAR> *>(set)) {
      GUM_CONSTRUCTOR(Class);

      for (const auto impl : *__implements)
        impl->__addImplementation(this);
    }

    template <typename GUM_SCALAR>
    Class<GUM_SCALAR>::Class(const std::string &name, Class<GUM_SCALAR> &super,
                             const Set<Interface<GUM_SCALAR> *> &set)
        : ClassElementContainer<GUM_SCALAR>(name), __dag(super.dag()),
          __super(&super), __implements(nullptr),
          __instantiations(super.__instantiations) {
      GUM_CONSTRUCTOR(Class);
      super.__addExtension(this);
      __inheritClass(super);

      // Adding other implementation
      if (__implements == nullptr) { // super has not created __implements
        __implements = new Set<Interface<GUM_SCALAR> *>(set);
      } else { // we just add the new implementations
        for (const auto elt : set) {
          __implements->insert(elt);
        }
      }

      for (const auto impl : *__implements)
        impl->__addImplementation(this);
    }

    template <typename GUM_SCALAR>
    Class<GUM_SCALAR>::Class(const Class<GUM_SCALAR> &source)
        : ClassElementContainer<GUM_SCALAR>(source.name()), __dag(source.dag()),
          __super(source.__super), __implements(0),
          __instantiations(source.__instantiations) {
      GUM_CONS_CPY(Class);
      GUM_ERROR(FatalError, "don't copy classes");
    }

    template <typename GUM_SCALAR> Class<GUM_SCALAR>::~Class() {
      GUM_DESTRUCTOR(Class);

      for (const auto &elt : __nodeIdMap)
        delete elt.second;

      if (__implements)
        delete __implements;
    }

    template <typename GUM_SCALAR>
    void Class<GUM_SCALAR>::__inheritClass(const Class<GUM_SCALAR> &c) {
      // Adding implemented interfaces of c, if any
      __implements =
          (c.__implements) ? new Set<Interface<GUM_SCALAR> *>(*(c.__implements)) : 0;
      // Copying attributes, the bijection's firsts are attributes in this and its
      // seconds are attributes
      // in c.
      Bijection<const DiscreteVariable *, const DiscreteVariable *> bij;

      for (const auto c_attr : c.__attributes) {
        // using multiDimSparse to prevent unecessary memory allocation for large
        // arrays
        Attribute<GUM_SCALAR> *attr = new Attribute<GUM_SCALAR>(
            c_attr->name(), c_attr->type(), new MultiDimSparse<GUM_SCALAR>(0.0));
        bij.insert(&(c_attr->type().variable()), &(attr->type().variable()));
        attr->setId(c_attr->id());
        __nodeIdMap.insert(attr->id(), attr);
        __attributes.insert(attr);

        if (c.__parameters.exists(const_cast<Attribute<GUM_SCALAR> *>(c_attr))) {
          __parameters.insert(attr);
          __paramValueFlags.insert(attr, c.__paramValueFlags[c_attr]);
        }

        if (c.__nameMap[c_attr->name()] == c.__nameMap[c_attr->safeName()])
          __nameMap.insert(attr->name(), attr);

        __nameMap.insert(attr->safeName(), attr);
      }

      // Copying aggregates
      for (const auto c_agg : c.__aggregates) {
        Aggregate<GUM_SCALAR> *agg = nullptr;

        try {
          agg = new Aggregate<GUM_SCALAR>(c_agg->name(), c_agg->agg_type(),
                                          c_agg->type(), c_agg->label());
        } catch (OperationNotAllowed &) {
          agg = new Aggregate<GUM_SCALAR>(c_agg->name(), c_agg->agg_type(),
                                          c_agg->type());
        }

        bij.insert(&(c_agg->type().variable()), &(agg->type().variable()));
        agg->setId(c_agg->id());
        __nodeIdMap.insert(agg->id(), agg);
        __aggregates.insert(agg);

        if (c.__nameMap[c_agg->name()] == c.__nameMap[c_agg->safeName()])
          __nameMap.insert(agg->name(), agg);

        __nameMap.insert(agg->safeName(), agg);
      }

      // Copying reference slots
      for (const auto c_refslot : c.__referenceSlots) {
        ReferenceSlot<GUM_SCALAR> *ref = new ReferenceSlot<GUM_SCALAR>(
            c_refslot->name(),
            const_cast<ClassElementContainer<GUM_SCALAR> &>(c_refslot->slotType()),
            c_refslot->isArray());

        ref->setId(c_refslot->id());
        __nodeIdMap.insert(ref->id(), ref);
        __referenceSlots.insert(ref);

        if (c.__nameMap[c_refslot->name()] == c.__nameMap[c_refslot->safeName()])
          __nameMap.insert(ref->name(), ref);

        __nameMap.insert(ref->safeName(), ref);
      }

      // Copying slot chains
      for (const auto c_slotchain : c.__slotChains) {
        // We just need to change the first ReferenceSlot<GUM_SCALAR> in the chain
        Sequence<ClassElement<GUM_SCALAR> *> chain(c_slotchain->chain());

        chain.setAtPos(0, __nameMap[c_slotchain->chain().front()->name()]);

        SlotChain<GUM_SCALAR> *sc =
            new SlotChain<GUM_SCALAR>(c_slotchain->name(), chain);
        bij.insert(&(c_slotchain->type().variable()), &(sc->type().variable()));
        sc->setId(c_slotchain->id());
        __nodeIdMap.insert(sc->id(), sc);
        __slotChains.insert(sc);

        // Slot chains do not have safe names
        __nameMap.insert(sc->name(), sc);
      }

      // Copying dependencies yield by arcs
      for (const auto &arc : c.dag().arcs()) {
        __nodeIdMap[arc.tail()]->addChild(*(__nodeIdMap[arc.head()]));
        __nodeIdMap[arc.head()]->addParent(*(__nodeIdMap[arc.tail()]));
      }

      // Copying the IO flag
      this->_copyIOFlags(c);
      // Copying content of CPF
      Attribute<GUM_SCALAR> *a = 0;

      for (const auto attr : c.__attributes) {
        a = static_cast<Attribute<GUM_SCALAR> *>(__nameMap[attr->safeName()]);
        delete a->__cpf;
        a->__cpf = copyPotential(bij, attr->cpf());
      }
    }

    template <typename GUM_SCALAR>
    bool Class<GUM_SCALAR>::isSubTypeOf(
        const ClassElementContainer<GUM_SCALAR> &cec) const {
      switch (cec.obj_type()) {
        case PRMObject::PRMType::CLASS: {
          const Class<GUM_SCALAR> *current = this;

          while (current != 0) {
            if (current == &(cec))
              return true;

            current = current->__super;
          }

          return false;
        }

        case PRMObject::PRMType::INTERFACE: {
          if (__implements != nullptr) {
            const Interface<GUM_SCALAR> &i =
                static_cast<const Interface<GUM_SCALAR> &>(cec);

            if (__implements->exists(const_cast<Interface<GUM_SCALAR> *>(&i)))
              return true;

            for (const auto impl : *__implements)
              if (impl->isSubTypeOf(i))
                return true;
          }

          return false;
        }

        default: {
          GUM_ERROR(FatalError, "unknown ClassElementContainer<GUM_SCALAR>");
        }
      }
    }

    template <typename GUM_SCALAR>
    void Class<GUM_SCALAR>::insertArc(const std::string &tail_name,
                                      const std::string &head_name) {
      addArc(tail_name, head_name);
    }

    template <typename GUM_SCALAR>
    void Class<GUM_SCALAR>::addArc(const std::string &tail_name,
                                   const std::string &head_name) {
      ClassElement<GUM_SCALAR> *tail = 0;
      ClassElement<GUM_SCALAR> *head = 0;

      try {
        tail = __nameMap[tail_name];
        head = __nameMap[head_name];
      } catch (NotFound &) {
        GUM_ERROR(NotFound, "tail and/or head of arc does not exists in this Class");
      }

      if ((tail->elt_type() == ClassElement<GUM_SCALAR>::prm_refslot) or
          (head->elt_type() == ClassElement<GUM_SCALAR>::prm_refslot)) {
        GUM_ERROR(OperationNotAllowed,
                  "a ReferenceSlot<GUM_SCALAR> can not on neither side of an arc");
      }

      if ((tail->elt_type() == ClassElement<GUM_SCALAR>::prm_slotchain) and
          (head->elt_type() == ClassElement<GUM_SCALAR>::prm_slotchain)) {
        GUM_ERROR(OperationNotAllowed,
                  "illegal insertion of an arc between two SlotChain<GUM_SCALAR>");
      }

      if (not __dag.existsArc(Arc(tail->id(), head->id()))) {
        __dag.addArc(tail->id(), head->id());
      } else {
        GUM_ERROR(DuplicateElement, "duplicate arc");
      }

      get(tail->id()).addChild(get(head->id()));
      get(head->id()).addParent(get(tail->id()));

      // Defining input / output nodes
      if (tail->elt_type() == ClassElement<GUM_SCALAR>::prm_slotchain) {
        SlotChain<GUM_SCALAR> *sc = static_cast<SlotChain<GUM_SCALAR> *>(tail);
        this->setInputNode(*head, true);
        sc->end().setOutputNode(sc->end().get(sc->lastElt().safeName()), true);
      }
    }

    template <typename GUM_SCALAR>
    NodeId Class<GUM_SCALAR>::add(ClassElement<GUM_SCALAR> *elt) {
      if (__nameMap.exists(elt->name())) {
        GUM_ERROR(DuplicateElement,
                  "name already used by another ClassElement<GUM_SCALAR>");
      }

      elt->setId(__dag.addNode());
      __nodeIdMap.insert(elt->id(), elt);
      __nameMap.insert(elt->name(), elt);

      try {
        __nameMap.insert(elt->safeName(), elt);
      } catch (DuplicateElement &) {
        // happens when elt is a slot chain
        GUM_ASSERT(elt->elt_type() == ClassElement<GUM_SCALAR>::prm_slotchain);
      }

      switch (elt->elt_type()) {
        case ClassElement<GUM_SCALAR>::prm_attribute: {
          __attributes.insert(static_cast<Attribute<GUM_SCALAR> *>(elt));
          __addCastDescendants(static_cast<Attribute<GUM_SCALAR> *>(elt));
          __addIOInterfaceFlags(elt);
          break;
        }

        case ClassElement<GUM_SCALAR>::prm_aggregate: {
          __aggregates.insert(static_cast<Aggregate<GUM_SCALAR> *>(elt));
          __addIOInterfaceFlags(elt);
          break;
        }

        case ClassElement<GUM_SCALAR>::prm_refslot: {
          ReferenceSlot<GUM_SCALAR> *ref =
              static_cast<ReferenceSlot<GUM_SCALAR> *>(elt);
          __referenceSlots.insert(ref);
          break;
        }

        case ClassElement<GUM_SCALAR>::prm_slotchain: {
          __slotChains.insert(static_cast<SlotChain<GUM_SCALAR> *>(elt));
          break;
        }

        default: { GUM_ERROR(FatalError, "unknown ClassElement<GUM_SCALAR> type"); }
      }

      return elt->id();
    }

    template <typename GUM_SCALAR>
    void Class<GUM_SCALAR>::__addCastDescendants(Attribute<GUM_SCALAR> *attr) {
      Attribute<GUM_SCALAR> *parent = attr;
      Attribute<GUM_SCALAR> *child = 0;

      while (parent->type().isSubType()) {
        child = parent->getCastDescendant();
        child->setId(__dag.addNode());
        __nodeIdMap.insert(child->id(), child);
        // Only use child's safe name when adding to the name map!
        __nameMap.insert(child->safeName(), child);
        __attributes.insert(child);
        // Do not use Class<GUM_SCALAR>::insertArc(), child's CPF is already
        // initialized properly
        __dag.addArc(parent->id(), child->id());
        parent = child;
      }
    }

    template <typename GUM_SCALAR>
    NodeId Class<GUM_SCALAR>::overload(ClassElement<GUM_SCALAR> *overloader) {
      try {
        if (not super().exists(overloader->name())) {
          GUM_ERROR(OperationNotAllowed,
                    "found no ClassElement<GUM_SCALAR> to overload");
        }
      } catch (NotFound &) {
        GUM_ERROR(OperationNotAllowed, "overload is possible only with subclasses");
      }

      ClassElement<GUM_SCALAR> *overloaded = __nameMap[overloader->name()];
      // Checking overload legality
      __checkOverloadLegality(overloaded, overloader);

      switch (overloader->elt_type()) {
        case ClassElement<GUM_SCALAR>::prm_attribute: {
          __overloadAttribute(static_cast<Attribute<GUM_SCALAR> *>(overloader),
                              static_cast<Attribute<GUM_SCALAR> *>(overloaded));
          __addIOInterfaceFlags(overloader);
          break;
        }

        case ClassElement<GUM_SCALAR>::prm_aggregate: {
          __overloadAggregate(static_cast<Aggregate<GUM_SCALAR> *>(overloader),
                              overloaded);
          __addIOInterfaceFlags(overloader);
          break;
        }

        case ClassElement<GUM_SCALAR>::prm_refslot: {
          // __checkOverloadLegality guaranties that overloaded is a
          // ReferenceSlot<GUM_SCALAR>
          __overloadReference(static_cast<ReferenceSlot<GUM_SCALAR> *>(overloader),
                              static_cast<ReferenceSlot<GUM_SCALAR> *>(overloaded));
          break;
        }

        case ClassElement<GUM_SCALAR>::prm_slotchain: {
          GUM_ERROR(WrongClassElement,
                    "SlotChain<GUM_SCALAR> can not be overloaded");
          break;
        }

        default: { GUM_ERROR(FatalError, "unknown ClassElement<GUM_SCALAR> type"); }
      }

      return overloader->id();
    }

    template <typename GUM_SCALAR>
    void Class<GUM_SCALAR>::__overloadAttribute(Attribute<GUM_SCALAR> *overloader,
                                                Attribute<GUM_SCALAR> *overloaded) {
      __dag.eraseParents(overloaded->id());

      // Checking if we have to add cast descendant
      if (overloader->type() != overloaded->type()) {
        overloader->setId(__dag.addNode());
        __nodeIdMap.insert(overloader->id(), overloader);
        __nameMap[overloader->name()] = overloader;
        __nameMap.insert(overloader->safeName(), overloader);
        __attributes.insert(overloader);
        __addCastDescendants(overloader, overloaded);
      } else {
        overloader->setId(overloaded->id());
        __nodeIdMap[overloader->id()] = overloader;
        __nameMap[overloader->name()] = overloader;
        __nameMap[overloader->safeName()] = overloader;
        __attributes.erase(overloaded);
        __attributes.insert(overloader);
        // Swapping types, ugly but necessary to preserve the Type<GUM_SCALAR>
        // pointer of overloaded
        __swap_types(overloader, overloaded);
      }
    }

    template <typename GUM_SCALAR>
    void
    Class<GUM_SCALAR>::__overloadReference(ReferenceSlot<GUM_SCALAR> *overloader,
                                           ReferenceSlot<GUM_SCALAR> *overloaded) {
      // Adding overloading reference
      overloader->setId(overloaded->id());
      __nodeIdMap[overloader->id()] = overloader;
      __nameMap[overloader->name()] = overloader;
      __referenceSlots.insert(overloader);
      SlotChain<GUM_SCALAR> *sc = 0;
      ReferenceSlot<GUM_SCALAR> *ref = 0;
      ClassElement<GUM_SCALAR> *next = 0;
      std::vector<SlotChain<GUM_SCALAR> *> toRemove, toAdd;

      // Updating SlotChain<GUM_SCALAR> which started with overloaded
      for (const auto slotchain : __slotChains) {
        // If the attribute pointed by this slotchain is overloaded, we need to
        // change the slotchain
        // names to it's safename version: ref.attr is replaced by ref.(type)attr.
        if ((slotchain->chain().atPos(0) == overloaded)) {
          Sequence<ClassElement<GUM_SCALAR> *> seq;
          seq.insert(overloader);

          auto elt = ++(slotchain->chain().begin());

          while (elt != slotchain->chain().end()) {
            ref = static_cast<ReferenceSlot<GUM_SCALAR> *>(seq.back());
            next = &(ref->slotType().get((*elt)->name()));
            seq.insert(next);
            ++elt;
          }

          // If the slotchain last element type changes, we change the slotchain to
          // point towards the cast decendant
          // with the correct type
          if (seq.back()->type() != slotchain->lastElt().type()) {
            seq.erase(seq.back());
            seq.insert(&(static_cast<ReferenceSlot<GUM_SCALAR> *>(seq.back())
                             ->slotType()
                             .get(slotchain->lastElt().safeName())));
            std::string sc_name;
            std::string dot = ".";

            for (Size i = 0; i < seq.size() - 1; ++i) {
              sc_name += seq.atPos(i)->name() + dot;
            }

            sc_name += seq.back()->safeName();
            sc = new SlotChain<GUM_SCALAR>(sc_name, seq);
            sc->setId(slotchain->id());

            for (const auto child : this->dag().children(sc->id()))
              get(child).cpf().swap(slotchain->lastElt().type().variable(),
                                    sc->lastElt().type().variable());

            toAdd.push_back(sc);
            toRemove.push_back(slotchain);
          } else {
            // Types are identical, we just need to change the first reference
            slotchain->chain().setAtPos(0, overloader);
          }
        }
      }

      for (const auto torem : toRemove) {
        __nameMap.erase(torem->name());
        __slotChains.erase(torem);
        delete torem;
      }

      for (const auto toadd : toAdd) {
        __nameMap.insert(toadd->name(), toadd);
        __nodeIdMap[toadd->id()] = toadd;
        __slotChains.insert(sc);
      }

      // Removing overloaded ReferenceSlot<GUM_SCALAR>
      __referenceSlots.erase(overloaded);
      delete overloaded;
    }

    template <typename GUM_SCALAR>
    void Class<GUM_SCALAR>::__swap_types(ClassElement<GUM_SCALAR> *overloader,
                                         ClassElement<GUM_SCALAR> *overloaded) {
      if (overloader->elt_type() == ClassElement<GUM_SCALAR>::prm_attribute) {
        Attribute<GUM_SCALAR> *loader =
            static_cast<Attribute<GUM_SCALAR> *>(overloader);

        if (overloaded->elt_type() == ClassElement<GUM_SCALAR>::prm_attribute) {
          Attribute<GUM_SCALAR> *loaded =
              static_cast<Attribute<GUM_SCALAR> *>(overloaded);
          Type<GUM_SCALAR> *tmp = loader->__type;
          loader->__type = loaded->__type;
          loaded->__type = tmp;
          loader->__cpf->erase(tmp->variable());
          loader->__cpf->add(loader->__type->variable());
        } else if (overloaded->elt_type() ==
                   ClassElement<GUM_SCALAR>::prm_aggregate) {
          Aggregate<GUM_SCALAR> *loaded =
              static_cast<Aggregate<GUM_SCALAR> *>(overloaded);
          Type<GUM_SCALAR> *tmp = loader->__type;
          loader->__type = loaded->__type;
          loaded->__type = tmp;
        } else {
          GUM_ERROR(FatalError, "swapping types impossible");
        }
      } else if (overloader->elt_type() == ClassElement<GUM_SCALAR>::prm_aggregate) {
        Aggregate<GUM_SCALAR> *loader =
            static_cast<Aggregate<GUM_SCALAR> *>(overloader);

        if (overloaded->elt_type() == ClassElement<GUM_SCALAR>::prm_attribute) {
          Attribute<GUM_SCALAR> *loaded =
              static_cast<Attribute<GUM_SCALAR> *>(overloaded);
          Type<GUM_SCALAR> *tmp = loader->__type;
          loader->__type = loaded->__type;
          loaded->__type = tmp;
        } else if (overloaded->elt_type() ==
                   ClassElement<GUM_SCALAR>::prm_aggregate) {
          Aggregate<GUM_SCALAR> *loaded =
              static_cast<Aggregate<GUM_SCALAR> *>(overloaded);
          Type<GUM_SCALAR> *tmp = loader->__type;
          loader->__type = loaded->__type;
          loaded->__type = tmp;
        } else {
          GUM_ERROR(FatalError, "swapping types impossible");
        }
      }

      delete overloaded;
    }

    template <typename GUM_SCALAR>
    void Class<GUM_SCALAR>::__addCastDescendants(Attribute<GUM_SCALAR> *start,
                                                 Attribute<GUM_SCALAR> *end) {
      Attribute<GUM_SCALAR> *parent = start;
      Attribute<GUM_SCALAR> *child = 0;

      while (parent->type().super() != end->type()) {
        child = parent->getCastDescendant();
        child->setId(__dag.addNode());
        __nodeIdMap.insert(child->id(), child);
        // Only use child's safe name when adding to the name map!
        __nameMap.insert(child->safeName(), child);
        __attributes.insert(child);
        __addIOInterfaceFlags(child);
        // Do not use Class<GUM_SCALAR>::insertArc(), child's CPF is already
        // initialized properly
        __dag.addArc(parent->id(), child->id());
        parent = child;
      }

      parent->setAsCastDescendant(end);
      __dag.addArc(parent->id(), end->id());
    }

    template <typename GUM_SCALAR>
    void Class<GUM_SCALAR>::_findAllSubtypes(
        Set<ClassElementContainer<GUM_SCALAR> *> &set) {
      for (const auto ext : __extensions) {
        set.insert(ext);
        ext->_findAllSubtypes(set);
      }
    }

    template <typename GUM_SCALAR>
    void Class<GUM_SCALAR>::__addIOInterfaceFlags(ClassElement<GUM_SCALAR> *elt) {
      if (__implements != nullptr) {
        for (const auto impl : *__implements) {
          if (impl->isOutputNode(*elt)) {
            try {
              this->_getIOFlag(*elt).second = true;
            } catch (NotFound &) {
              this->_setIOFlag(*elt, std::make_pair(false, true));
            }
          }
        }
      }
    }

    template <typename GUM_SCALAR>
    void Class<GUM_SCALAR>::_updateDescendants(const ClassElement<GUM_SCALAR> &elt) {
      for (const auto ext : __extensions) {
        // We test to prevent unnecessary recursive call from iter
        if (!ext->isOutputNode(elt)) {
          ext->setOutputNode(elt, true);
        }
      }
    }

    template <typename GUM_SCALAR>
    INLINE typename PRMObject::PRMType Class<GUM_SCALAR>::obj_type() const {
      return PRMObject::PRMType::CLASS;
    }

    template <typename GUM_SCALAR>
    INLINE const DAG &Class<GUM_SCALAR>::_dag() const {
      return __dag;
    }

    template <typename GUM_SCALAR> INLINE DAG &Class<GUM_SCALAR>::_dag() {
      return __dag;
    }

    template <typename GUM_SCALAR>
    INLINE ClassElement<GUM_SCALAR> &Class<GUM_SCALAR>::get(NodeId id) {
      try {
        return *(__nodeIdMap[id]);
      } catch (NotFound &) {
        GUM_ERROR(NotFound, "no ClassElement<GUM_SCALAR> with the given NodeId");
      }
    }

    template <typename GUM_SCALAR>
    INLINE const ClassElement<GUM_SCALAR> &Class<GUM_SCALAR>::get(NodeId id) const {
      try {
        return *(__nodeIdMap[id]);
      } catch (NotFound &) {
        GUM_ERROR(NotFound, "no ClassElement<GUM_SCALAR> with the given NodeId");
      }
    }

    template <typename GUM_SCALAR>
    INLINE bool
    Class<GUM_SCALAR>::isParameter(const ClassElement<GUM_SCALAR> &elt) const {
      const ClassElement<GUM_SCALAR> &mine = get(elt.safeName());

      if (ClassElement<GUM_SCALAR>::isAttribute(mine)) {
        return __parameters.exists(static_cast<Attribute<GUM_SCALAR> *>(
            const_cast<ClassElement<GUM_SCALAR> *>(&mine)));
      }

      return false;
      // if (__nodeIdMap.exists(id) and
      // (ClassElement<GUM_SCALAR>::isAttribute(get(id)))) {
      //   return
      //   __parameters.exists(&(static_cast<Attribute<GUM_SCALAR>&>(const_cast<ClassElement<GUM_SCALAR>&>(get(id)))));
      // } else if (not __nodeIdMap.exists(id)) {
      //   GUM_ERROR(NotFound, "no ClassElement<GUM_SCALAR> with the given NodeId");
      // } else {
      //   GUM_ERROR(WrongClassElement, "given id is not a potential parameter");
      // }
    }

    template <typename GUM_SCALAR>
    INLINE ClassElement<GUM_SCALAR> &
    Class<GUM_SCALAR>::get(const std::string &name) {
      try {
        return *(__nameMap[name]);
      } catch (NotFound &) {
        GUM_ERROR(NotFound, "no ClassElement<GUM_SCALAR> with the given name");
      }
    }

    template <typename GUM_SCALAR>
    INLINE const ClassElement<GUM_SCALAR> &
    Class<GUM_SCALAR>::get(const std::string &name) const {
      try {
        return *(__nameMap[name]);
      } catch (NotFound &) {
        GUM_ERROR(NotFound, "no ClassElement<GUM_SCALAR> with the given name");
      }
    }

    template <typename GUM_SCALAR>
    INLINE const Set<Attribute<GUM_SCALAR> *> &
    Class<GUM_SCALAR>::attributes() const {
      return __attributes;
    }

    template <typename GUM_SCALAR>
    INLINE const Set<Attribute<GUM_SCALAR> *> &
    Class<GUM_SCALAR>::parameters() const {
      return __parameters;
    }

    template <typename GUM_SCALAR>
    INLINE const Set<Aggregate<GUM_SCALAR> *> &
    Class<GUM_SCALAR>::aggregates() const {
      return __aggregates;
    }

    template <typename GUM_SCALAR>
    INLINE const Set<ReferenceSlot<GUM_SCALAR> *> &
    Class<GUM_SCALAR>::referenceSlots() const {
      return __referenceSlots;
    }

    template <typename GUM_SCALAR>
    INLINE const Set<SlotChain<GUM_SCALAR> *> &
    Class<GUM_SCALAR>::slotChains() const {
      return __slotChains;
    }

    template <typename GUM_SCALAR>
    INLINE const Sequence<NodeId> &Class<GUM_SCALAR>::toInstantiate() const {
      return __instantiations;
    }

    template <typename GUM_SCALAR>
    INLINE const Class<GUM_SCALAR> &Class<GUM_SCALAR>::super() const {
      if (__super) {
        return *__super;
      } else {
        GUM_ERROR(NotFound, "this Class is not a subclass");
      }
    }

    template <typename GUM_SCALAR>
    INLINE const Set<Interface<GUM_SCALAR> *> &
    Class<GUM_SCALAR>::implements() const {
      if (__implements) {
        return *__implements;
      } else {
        GUM_ERROR(NotFound,
                  "this Class does not implement any Interface<GUM_SCALAR>");
      }
    }

    template <typename GUM_SCALAR>
    INLINE ClassElement<GUM_SCALAR> &Class<GUM_SCALAR>::operator[](NodeId id) {
      return get(id);
    }

    template <typename GUM_SCALAR>
    INLINE const ClassElement<GUM_SCALAR> &Class<GUM_SCALAR>::
    operator[](NodeId id) const {
      return get(id);
    }

    template <typename GUM_SCALAR>
    INLINE ClassElement<GUM_SCALAR> &Class<GUM_SCALAR>::
    operator[](const std::string &name) {
      return get(name);
    }

    template <typename GUM_SCALAR>
    INLINE const ClassElement<GUM_SCALAR> &Class<GUM_SCALAR>::
    operator[](const std::string &name) const {
      return get(name);
    }

    template <typename GUM_SCALAR>
    INLINE void
    Class<GUM_SCALAR>::__overloadAggregate(Aggregate<GUM_SCALAR> *overloader,
                                           ClassElement<GUM_SCALAR> *overloaded) {
      __nameMap.insert(overloader->safeName(), overloader);
      __aggregates.insert(overloader);
    }

    template <typename GUM_SCALAR>
    INLINE void Class<GUM_SCALAR>::__checkOverloadLegality(
        const ClassElement<GUM_SCALAR> *overloaded,
        const ClassElement<GUM_SCALAR> *overloader) {
      if (overloaded->elt_type() != overloader->elt_type())
        GUM_ERROR(TypeError, "invalid overload");

      if (overloaded->elt_type() == ClassElement<GUM_SCALAR>::prm_attribute) {
        if (not overloader->type().isSubTypeOf(overloaded->type())) {
          GUM_ERROR(TypeError, "the overloading ClassElement<GUM_SCALAR> "
                               "Type<GUM_SCALAR> is illegal");
        }
      } else if (overloaded->elt_type() == ClassElement<GUM_SCALAR>::prm_refslot) {
        if (not static_cast<const ReferenceSlot<GUM_SCALAR> *>(overloader)
                    ->slotType()
                    .isSubTypeOf(static_cast<const ReferenceSlot<GUM_SCALAR> *>(
                                     overloaded)->slotType()))
          GUM_ERROR(
              TypeError,
              "the overloading ReferenceSlot<GUM_SCALAR> slot type is illegal");
      } else {
        GUM_ERROR(TypeError, "illegal type to overload");
      }
    }

    template <typename GUM_SCALAR>
    INLINE const Set<Class<GUM_SCALAR> *> &Class<GUM_SCALAR>::extensions() const {
      return __extensions;
    }

    template <typename GUM_SCALAR>
    INLINE void Class<GUM_SCALAR>::__addExtension(Class<GUM_SCALAR> *c) {
      __extensions.insert(c);
    }

    template <typename GUM_SCALAR>
    INLINE NodeId
        Class<GUM_SCALAR>::addParameter(Attribute<GUM_SCALAR> *param, bool flag) {
      if (__nameMap.exists(param->name())) {
        GUM_ERROR(DuplicateElement,
                  "name already used by another ClassElement<GUM_SCALAR>");
      }

      param->setId(__dag.addNode());
      __nodeIdMap.insert(param->id(), param);
      __nameMap.insert(param->name(), param);
      __nameMap.insert(param->safeName(), param);
      __parameters.insert(param);
      __paramValueFlags.insert(param, flag);
      __addCastDescendants(param);
      return param->id();
    }

    template <typename GUM_SCALAR>
    INLINE bool
    Class<GUM_SCALAR>::isCastDescendant(const std::string &safe_name) const {
      const ClassElement<GUM_SCALAR> &elt = get(safe_name);

      try {
        return elt.type().name() == get(elt.name()).type().name();
      } catch (OperationNotAllowed &) {
        GUM_ERROR(NotFound, "no attribute with the given name");
      }
    }

  } /* namespace prm */
} /* namespace gum */
