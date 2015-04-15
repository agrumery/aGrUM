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
 * @brief Inline implementation of gum::Instance
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#include <agrum/PRM/elements/instance.h>

#include <agrum/multidim/multiDimSparse.h>

namespace gum {
  namespace prm {
    template <typename GUM_SCALAR>
    Instance<GUM_SCALAR>::Instance(const std::string &name, Class<GUM_SCALAR> &type)
        : PRMObject(name), __type(&type), __params(0) {
      GUM_CONSTRUCTOR(Instance);

      // First we create attributes for each aggregate in type
      for (const auto agg : __type->aggregates())
        __copyAggregates(agg);

      // We add attributes in type by reference for inner ones and by copy for output
      // ones
      for (const auto attr : __type->attributes())
        __copyAttribute(attr);

      // Then we instantiate each parameters in type
      if (__type->parameters().size())
        __params = new Set<NodeId>();

      for (const auto param : __type->parameters())
        __copyParameter(param);
    }

    template <typename GUM_SCALAR>
    void Instance<GUM_SCALAR>::__copyParameter(Attribute<GUM_SCALAR> *source) {
      Attribute<GUM_SCALAR> *attr =
          new Attribute<GUM_SCALAR>(source->name(), source->type());
      Instantiation i(attr->cpf()), j(source->cpf());

      for (i.setFirst(), j.setFirst(); not(i.end() or j.end()); i.inc(), j.inc())
        attr->cpf().set(i, source->cpf().get(j));

      attr->setId(source->id());
      __nodeIdMap.insert(attr->id(), attr);
      __bijection.insert(&(source->type().variable()), &(attr->type().variable()));
    }

    template <typename GUM_SCALAR> Instance<GUM_SCALAR>::~Instance() {
      GUM_DESTRUCTOR(Instance);

      for (const auto &elt : __nodeIdMap)
        delete elt.second;

      for (const auto &elt : __referenceMap)
        delete elt.second;

      for (const auto &elt : __referingAttr)
        delete elt.second;

      if (__params)
        delete __params;
    }

    template <typename GUM_SCALAR> void Instance<GUM_SCALAR>::instantiate() {
      // First retrieving any referenced instance
      for (const auto chain : type().slotChains())
        __instantiateSlotChain(chain);

      // Now we need to add referred instance to each input node
      // For Attributes we first add parents, then we initialize CPF
      for (const auto attr : type().attributes())
        if (not type().isParameter(*attr))
          __copyAttributeCPF(__nodeIdMap[(*attr).id()]);

      // For Aggregate<GUM_SCALAR> we add parents
      for (const auto agg : type().aggregates()) {
        Attribute<GUM_SCALAR> &attr = get(agg->safeName());

        for (const auto node : type().dag().parents(agg->id()))
          try {
            attr.addParent(get(node));
          } catch (NotFound &) {
            SlotChain<GUM_SCALAR> &sc =
                static_cast<SlotChain<GUM_SCALAR> &>(type().get(node));
            const Set<Instance<GUM_SCALAR> *> &instances = getInstances(sc.id());

            for (const auto inst : instances) {
              attr.addParent(inst->get(sc.lastElt().safeName()));
            }
          }
      }
    }

    template <typename GUM_SCALAR>
    void Instance<GUM_SCALAR>::__instantiateSlotChain(SlotChain<GUM_SCALAR> *sc) {
      __referenceMap.insert(sc->id(), new Set<Instance<GUM_SCALAR> *>());
      // An instantiated slot chain is a tree, to find all leaves we proceed with a
      // deep run
      std::vector<std::pair<Instance<GUM_SCALAR> *, Size>> stack;
      stack.push_back(std::pair<Instance<GUM_SCALAR> *, Size>(this, 0));
      Set<Instance<GUM_SCALAR> *> visited;
      // Last element is an attribute, and we only want the instance containing it
      Size depth_stop = sc->chain().size() - 1;

      // Let's go!
      while (not stack.empty()) {
        Instance<GUM_SCALAR> *current = stack.back().first;
        Size depth = stack.back().second;
        stack.pop_back();

        if (not visited.exists(current)) {
          visited.insert(current);

          if (depth < depth_stop) {
            try {
              NodeId refId = sc->chain().atPos(depth)->id();

              for (const auto inst : *current->__referenceMap[refId])
                stack.push_back(std::make_pair(inst, depth + 1));
            } catch (NotFound &) {
              GUM_ERROR(NotFound, "found an uninstantiated reference");
            }
          } else {
            __referenceMap[sc->id()]->insert(current);
            __addReferingInstance(sc, current);

            // If slot chain is single, it could be used by an attribute so we add
            // the corresponding DiscreteVariable
            // to __bijection for CPF initialisation
            if (not sc->isMultiple()) {
              __bijection.insert(
                  &(sc->type().variable()),
                  &(current->get(sc->lastElt().safeName()).type().variable()));
            }
          }
        }
      }
    }

    template <typename GUM_SCALAR>
    void Instance<GUM_SCALAR>::add(NodeId id, Instance<GUM_SCALAR> &instance) {
      ClassElement<GUM_SCALAR> *elt = 0;

      try {
        elt = &(type().get(id));
      } catch (NotFound &) {
        GUM_ERROR(NotFound, "no ClassElement<GUM_SCALAR> matches the given id");
      }

      switch (elt->elt_type()) {
        case ClassElement<GUM_SCALAR>::prm_refslot: {
          ReferenceSlot<GUM_SCALAR> *ref =
              static_cast<ReferenceSlot<GUM_SCALAR> *>(elt);

          // Checking if instance's type is legal
          if (not instance.type().isSubTypeOf(ref->slotType())) {
            GUM_ERROR(TypeError,
                      "given Instance type is not a proper "
                      "subclass of the ReferenceSlot<GUM_SCALAR> slot type");
          }

          // Checking the reference's size limit
          if (__referenceMap.exists(id) and
              (not static_cast<ReferenceSlot<GUM_SCALAR> &>(type().get(id))
                       .isArray()) and
              (__referenceMap[id]->size() == 1)) {
            GUM_ERROR(OutOfUpperBound,
                      "ReferenceSlot<GUM_SCALAR> size limit reached");
          }

          break;
        }

        case ClassElement<GUM_SCALAR>::prm_slotchain: {
          SlotChain<GUM_SCALAR> &sc =
              static_cast<SlotChain<GUM_SCALAR> &>(type().get(id));

          // Checking if instance's type is legal
          if (not instance.type().isSubTypeOf(sc.end())) {
            GUM_ERROR(TypeError, "given Instance type is not a proper "
                                 "subclass of the ClassElementContainer pointed"
                                 " by the SlotChain<GUM_SCALAR>");
          }

          // Checking the reference's size limit
          if (__referenceMap.exists(id) and
              (not static_cast<SlotChain<GUM_SCALAR> &>(type().get(id))
                       .isMultiple()) and
              (__referenceMap[id]->size() == 1)) {
            GUM_ERROR(OutOfUpperBound, "SlotChain<GUM_SCALAR> size limit reached");
          }

          // We need to plug this to the referred instance.
          GUM_CHECKPOINT;
          break;
        }

        default: {
          if (not type().isOutputNode(*elt)) {
            GUM_ERROR(WrongClassElement,
                      "given ClassElement<GUM_SCALAR> is not an output node");
          }
        }
      }

      if (not __referenceMap.exists(id)) {
        __referenceMap.insert(id, new Set<Instance<GUM_SCALAR> *>());
      }

      __referenceMap[id]->insert(&instance);
    }

    template <typename GUM_SCALAR> INLINE Size Instance<GUM_SCALAR>::size() const {
      return __nodeIdMap.size();
    }

    template <typename GUM_SCALAR>
    INLINE void
    Instance<GUM_SCALAR>::__copyAggregates(Aggregate<GUM_SCALAR> *source) {
      Attribute<GUM_SCALAR> *attr = new Attribute<GUM_SCALAR>(
          source->name(), source->type(), source->buildImpl());
      GUM_ASSERT(&(attr->type().variable()) != &(source->type().variable()));
      attr->setId(source->id());
      __nodeIdMap.insert(attr->id(), attr);
      __bijection.insert(&(source->type().variable()), &(attr->type().variable()));
    }

    template <typename GUM_SCALAR>
    INLINE void
    Instance<GUM_SCALAR>::__copyAttribute(Attribute<GUM_SCALAR> *source) {
      Attribute<GUM_SCALAR> *attr =
          new Attribute<GUM_SCALAR>(source->name(), source->type());
      // The potential is copied when instantiate() is called
      attr->cpf().fill((GUM_SCALAR)0);
      attr->setId(source->id());
      __bijection.insert(&(source->type().variable()), &(attr->type().variable()));
      __nodeIdMap.insert(attr->id(), attr);
    }

    template <typename GUM_SCALAR>
    INLINE Instance<GUM_SCALAR>::Instance(const Instance<GUM_SCALAR> &source)
        : PRMObject(source), __type(source.__type), __params(0) {
      GUM_CONS_CPY(Instance);
      GUM_ERROR(FatalError, "do not copy Instance");
    }

    template <typename GUM_SCALAR>
    INLINE Instance<GUM_SCALAR> & /**/
        Instance<GUM_SCALAR>::
        operator=(const Class<GUM_SCALAR> &from) {
      GUM_ERROR(FatalError, "do not copy Instance");
    }

    template <typename GUM_SCALAR>
    INLINE PRMObject::PRMType Instance<GUM_SCALAR>::obj_type() const {
      return PRMObject::PRMType::INSTANCE;
    }

    template <typename GUM_SCALAR>
    INLINE Class<GUM_SCALAR> &Instance<GUM_SCALAR>::type() {
      return *__type;
    }

    template <typename GUM_SCALAR>
    INLINE const Class<GUM_SCALAR> &Instance<GUM_SCALAR>::type() const {
      return *__type;
    }

    template <typename GUM_SCALAR>
    INLINE bool Instance<GUM_SCALAR>::exists(NodeId id) const {
      return __nodeIdMap.exists(id);
    }

    template <typename GUM_SCALAR>
    INLINE bool Instance<GUM_SCALAR>::exists(const std::string &name) const {
      return __type->exists(name) and exists(__type->get(name).id());
    }

    template <typename GUM_SCALAR>
    INLINE Attribute<GUM_SCALAR> &Instance<GUM_SCALAR>::get(NodeId id) {
      try {
        return *(__nodeIdMap[id]);
      } catch (NotFound &) {
        GUM_ERROR(NotFound, "no Attribute<GUM_SCALAR> with the given NodeId");
      }
    }

    template <typename GUM_SCALAR>
    INLINE const Attribute<GUM_SCALAR> &Instance<GUM_SCALAR>::get(NodeId id) const {
      try {
        return *(__nodeIdMap[id]);
      } catch (NotFound &) {
        GUM_ERROR(NotFound, "no Attribute<GUM_SCALAR> with the given NodeId");
      }
    }

    template <typename GUM_SCALAR>
    INLINE Attribute<GUM_SCALAR> &
    Instance<GUM_SCALAR>::get(const std::string &name) {
      try {
        return *(__nodeIdMap[type().get(name).id()]);
      } catch (NotFound &) {
        GUM_ERROR(NotFound, "no Attribute<GUM_SCALAR> with the given name");
      }
    }

    template <typename GUM_SCALAR>
    INLINE const Attribute<GUM_SCALAR> &
    Instance<GUM_SCALAR>::get(const std::string &name) const {
      try {
        return *(__nodeIdMap[type().get(name).id()]);
      } catch (NotFound &) {
        GUM_ERROR(NotFound, "no Attribute<GUM_SCALAR> with the given name");
      }
    }

    template <typename GUM_SCALAR>
    INLINE bool Instance<GUM_SCALAR>::isInitialised(NodeId id) const {
      return (__params) ? __params->exists(id) : false;
    }

    template <typename GUM_SCALAR>
    INLINE void
    Instance<GUM_SCALAR>::setParameterValue(const std::string &name,
                                            const Potential<GUM_SCALAR> &value) {
      Attribute<GUM_SCALAR> *attr = 0;

      try {
        attr = &(get(name));
      } catch (NotFound &) {
        GUM_ERROR(NotFound,
                  "the given ClassElement<GUM_SCALAR> is not in this Instance");
      }

      if (not __type->isParameter(*attr)) {
        GUM_ERROR(WrongClassElement, "the given NodeId is not a parameter");
      }

      if (attr->cpf().nbrDim() > 1) {
        GUM_ERROR(FatalError,
                  "found a parameter's potential with more than one dimension");
      }

      if (not value.contains(attr->type().variable())) {
        GUM_ERROR(OperationNotAllowed,
                  "the given value is invalid for this parameter");
      }

      // Assigning value
      attr->cpf().copy(value);

      if (not __params) {
        __params = new Set<NodeId>();
      }

      __params->insert(attr->id());
    }

    template <typename GUM_SCALAR>
    INLINE bool Instance<GUM_SCALAR>::isInstantiated(NodeId id) const {
      return __nodeIdMap.exists(id);
    }

    template <typename GUM_SCALAR>
    INLINE void
    Instance<GUM_SCALAR>::__addReferingInstance(SlotChain<GUM_SCALAR> *sc,
                                                Instance<GUM_SCALAR> *i) {
      NodeId id = i->get(sc->lastElt().safeName()).id();
      std::string name = sc->lastElt().safeName();

      try {
        i->__referenceMap[id]->insert(this);
        i->__referingAttr[id]->push_back(
            std::make_pair(this, sc->lastElt().safeName()));
      } catch (NotFound &) {
        i->__referenceMap.insert(id, new Set<Instance<GUM_SCALAR> *>());
        i->__referenceMap[id]->insert(this);
        i->__referingAttr.insert(id, new std::vector<pair>());
        i->__referingAttr[id]->push_back(
            std::make_pair(this, sc->lastElt().safeName()));
      }
    }

    template <typename GUM_SCALAR>
    INLINE void Instance<GUM_SCALAR>::instantiate(NodeId id) {
      // Do nothing
    }

    template <typename GUM_SCALAR>
    INLINE const Bijection<const DiscreteVariable *, const DiscreteVariable *> &
    Instance<GUM_SCALAR>::bijection() const {
      return __bijection;
    }

    template <typename GUM_SCALAR>
    INLINE const Instance<GUM_SCALAR> &
    Instance<GUM_SCALAR>::getInstance(NodeId id) const {
      try {
        if (__referenceMap[id]->size() > 0) {
          return **(__referenceMap[id]->begin());
        } else {
          GUM_ERROR(UndefinedElement,
                    "no Instance associated with the given NodeId");
        }
      } catch (NotFound &) {
        GUM_ERROR(NotFound, "no ReferenceSlot<GUM_SCALAR> or SlotChain<GUM_SCALAR> "
                            "matches the given NodeId");
      }
    }

    template <typename GUM_SCALAR>
    INLINE const Set<Instance<GUM_SCALAR> *> &
    Instance<GUM_SCALAR>::getInstances(NodeId id) const {
      try {
        return *(__referenceMap[id]);
      } catch (NotFound &) {
        GUM_ERROR(NotFound, "no ReferenceSlot<GUM_SCALAR> or SlotChain<GUM_SCALAR> "
                            "matches the given NodeId");
      }
    }

    template <typename GUM_SCALAR>
    INLINE typename Instance<GUM_SCALAR>::iterator Instance<GUM_SCALAR>::begin() {
      return __nodeIdMap.begin();
    }

    template <typename GUM_SCALAR>
    INLINE const typename Instance<GUM_SCALAR>::iterator &
    Instance<GUM_SCALAR>::end() {
      return __nodeIdMap.end();
    }

    template <typename GUM_SCALAR>
    INLINE typename Instance<GUM_SCALAR>::const_iterator
    Instance<GUM_SCALAR>::begin() const {
      return __nodeIdMap.begin();
    }

    template <typename GUM_SCALAR>
    INLINE const typename Instance<GUM_SCALAR>::const_iterator &
    Instance<GUM_SCALAR>::end() const {
      return __nodeIdMap.end();
    }

    template <typename GUM_SCALAR>
    INLINE typename Instance<GUM_SCALAR>::RefIterator
    Instance<GUM_SCALAR>::begin(NodeId id) {
      try {
        return Instance<GUM_SCALAR>::RefIterator(*(__referenceMap[id]));
      } catch (NotFound &) {
        GUM_ERROR(NotFound, "no referred instances from this NodeId");
      }
    }

    template <typename GUM_SCALAR>
    INLINE typename Instance<GUM_SCALAR>::RefConstIterator
    Instance<GUM_SCALAR>::begin(NodeId id) const {
      try {
        return Instance<GUM_SCALAR>::RefConstIterator(*(__referenceMap[id]));
      } catch (NotFound &) {
        GUM_ERROR(NotFound, "no referred instances from this NodeId");
      }
    }

    template <typename GUM_SCALAR>
    INLINE
    Instance<GUM_SCALAR>::RefIterator::RefIterator(Set<Instance<GUM_SCALAR> *> &set)
        : __set(set), __iter(set.begin()) {
      GUM_CONSTRUCTOR(Instance<GUM_SCALAR>::RefIterator);
    }

    template <typename GUM_SCALAR>
    INLINE Instance<GUM_SCALAR>::RefIterator::RefIterator(const RefIterator &from)
        : __set(const_cast<Set<Instance<GUM_SCALAR> *> &>(from.__set)),
          __iter(from.__iter) {
      GUM_CONS_CPY(Instance<GUM_SCALAR>::RefIterator);
    }

    template <typename GUM_SCALAR>
    INLINE Instance<GUM_SCALAR>::RefIterator::~RefIterator() {
      GUM_DESTRUCTOR(Instance<GUM_SCALAR>::RefIterator);
    }

    template <typename GUM_SCALAR>
    INLINE typename Instance<GUM_SCALAR>::RefIterator &
        Instance<GUM_SCALAR>::RefIterator::
        operator=(const RefIterator &from) {
      __iter = from.__iter;
      return *this;
    }

    template <typename GUM_SCALAR>
    INLINE typename Instance<GUM_SCALAR>::RefIterator &
        Instance<GUM_SCALAR>::RefIterator::
        operator++() {
      ++__iter;
      return *this;
    }

    template <typename GUM_SCALAR>
    INLINE bool Instance<GUM_SCALAR>::RefIterator::isEnd() const {
      return __iter == __set.end();
    }

    template <typename GUM_SCALAR>
    INLINE bool Instance<GUM_SCALAR>::RefIterator::
    operator!=(const RefIterator &from) const {
      return __iter != from.__iter;
    }

    template <typename GUM_SCALAR>
    INLINE bool Instance<GUM_SCALAR>::RefIterator::
    operator==(const RefIterator &from) const {
      return __iter == from.__iter;
    }

    template <typename GUM_SCALAR>
    INLINE Instance<GUM_SCALAR> &Instance<GUM_SCALAR>::RefIterator::
    operator*() const {
      return **__iter;
    }

    template <typename GUM_SCALAR>
    INLINE Instance<GUM_SCALAR> *Instance<GUM_SCALAR>::RefIterator::
    operator->() const {
      return *__iter;
    }

    template <typename GUM_SCALAR>
    INLINE Instance<GUM_SCALAR>::RefConstIterator::RefConstIterator(
        const Set<Instance<GUM_SCALAR> *> &set)
        : __set(set), __iter(set.begin()) {
      GUM_CONSTRUCTOR(Instance<GUM_SCALAR>::RefConstIterator);
    }

    template <typename GUM_SCALAR>
    INLINE Instance<GUM_SCALAR>::RefConstIterator::RefConstIterator(
        const RefConstIterator &from)
        : __set(from.__set), __iter(from.__iter) {
      GUM_CONS_CPY(Instance<GUM_SCALAR>::RefConstIterator);
    }

    template <typename GUM_SCALAR>
    INLINE Instance<GUM_SCALAR>::RefConstIterator::~RefConstIterator() {
      GUM_DESTRUCTOR(Instance<GUM_SCALAR>::RefConstIterator);
    }

    template <typename GUM_SCALAR>
    INLINE typename Instance<GUM_SCALAR>::RefConstIterator &
        Instance<GUM_SCALAR>::RefConstIterator::
        operator=(const RefConstIterator &from) {
      __iter = from.__iter;
      return *this;
    }

    template <typename GUM_SCALAR>
    INLINE typename Instance<GUM_SCALAR>::RefConstIterator &
        Instance<GUM_SCALAR>::RefConstIterator::
        operator++() {
      ++__iter;
      return *this;
    }

    template <typename GUM_SCALAR>
    INLINE bool Instance<GUM_SCALAR>::RefConstIterator::isEnd() const {
      return __iter == __set.end();
    }

    template <typename GUM_SCALAR>
    INLINE bool Instance<GUM_SCALAR>::RefConstIterator::
    operator!=(const RefConstIterator &from) const {
      return __iter != from.__iter;
    }

    template <typename GUM_SCALAR>
    INLINE bool Instance<GUM_SCALAR>::RefConstIterator::
    operator==(const RefConstIterator &from) const {
      return __iter == from.__iter;
    }

    template <typename GUM_SCALAR>
    INLINE const Instance<GUM_SCALAR> &Instance<GUM_SCALAR>::RefConstIterator::
    operator*() const {
      return **__iter;
    }

    template <typename GUM_SCALAR>
    INLINE const Instance<GUM_SCALAR> *Instance<GUM_SCALAR>::RefConstIterator::
    operator->() const {
      return *__iter;
    }

    template <typename GUM_SCALAR>
    INLINE typename Instance<GUM_SCALAR>::InvRefIterator
    Instance<GUM_SCALAR>::beginInvRef() {
      return __referingAttr.begin();
    }

    template <typename GUM_SCALAR>
    INLINE const typename Instance<GUM_SCALAR>::InvRefIterator &
    Instance<GUM_SCALAR>::endInvRef() {
      return __referingAttr.end();
    }

    template <typename GUM_SCALAR>
    INLINE typename Instance<GUM_SCALAR>::InvRefConstIterator
    Instance<GUM_SCALAR>::beginInvRef() const {
      return __referingAttr.begin();
    }

    template <typename GUM_SCALAR>
    INLINE const typename Instance<GUM_SCALAR>::InvRefConstIterator &
    Instance<GUM_SCALAR>::endInvRef() const {
      return __referingAttr.end();
    }

    template <typename GUM_SCALAR>
    INLINE std::vector<std::pair<Instance<GUM_SCALAR> *, std::string>> &
    Instance<GUM_SCALAR>::getRefAttr(NodeId id) {
      return *(__referingAttr[id]);
    }

    template <typename GUM_SCALAR>
    INLINE const std::vector<std::pair<Instance<GUM_SCALAR> *, std::string>> &
    Instance<GUM_SCALAR>::getRefAttr(NodeId id) const {
      return *(__referingAttr[id]);
    }

    template <typename GUM_SCALAR>
    INLINE bool Instance<GUM_SCALAR>::hasRefAttr(NodeId id) const {
      return __referingAttr.exists(id) and (not __referingAttr[id]->empty());
    }

    template <typename GUM_SCALAR>
    INLINE void
    Instance<GUM_SCALAR>::__copyAttributeCPF(Attribute<GUM_SCALAR> *attr) {
      // try {
      try {
        Potential<GUM_SCALAR> *p =
            copyPotential(bijection(), type().get(attr->safeName()).cpf());
        delete (attr->__cpf);
        attr->__cpf = p;
      } catch (Exception &e) {
#ifndef NDEBUG
        GUM_TRACE_VAR(name());
        GUM_TRACE_VAR(attr->safeName());

        for (const auto node : type().dag().parents(attr->id()))
          GUM_TRACE_VAR(type().get(node).safeName());

#endif
        throw(e);
      }
    }

  } /* namespace prm */
} /* namespace gum */
