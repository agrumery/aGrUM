/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief Inline implementation of gum::PRMInstance
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(@LIP6)
 */

#include <agrum/PRM/elements/PRMInstance.h>

#include <agrum/tools/multidim/implementations/multiDimSparse.h>

namespace gum {
  namespace prm {
    template < typename GUM_SCALAR >
    PRMInstance< GUM_SCALAR >::PRMInstance(const std::string&      name,
                                           PRMClass< GUM_SCALAR >& type) :
        PRMObject(name),
        instantiated__(false), type__(&type) {
      GUM_CONSTRUCTOR(PRMInstance);

      // First we create attributes for each aggregate in type
      for (const auto agg: type__->aggregates())
        copyAggregates__(agg);

      // We add attributes in type by reference for inner ones and by copy for
      // output ones
      for (const auto attr: type__->attributes())
        copyAttribute__(attr);
    }

    template < typename GUM_SCALAR >
    PRMInstance< GUM_SCALAR >::~PRMInstance() {
      GUM_DESTRUCTOR(PRMInstance);

      for (const auto& elt: nodeIdMap__)
        delete elt.second;

      for (const auto& elt: referenceMap__)
        delete elt.second;

      for (const auto& elt: referingAttr__)
        delete elt.second;
    }

    template < typename GUM_SCALAR >
    void PRMInstance< GUM_SCALAR >::instantiate() {
      if (!instantiated__) {
        instantiated__ = true;
        doInstantiate__();
      }
    }

    template < typename GUM_SCALAR >
    void PRMInstance< GUM_SCALAR >::doInstantiate__() {
      // First retrieving any referenced instance
      for (const auto chain: type().slotChains()) {
        instantiateSlotChain__(chain);
      }

      // Now we need to add referred instance to each input node
      // For Attributes we first add parents, then we initialize CPF
      for (const auto attr: type().attributes()) {
        copyAttributeCPF__(nodeIdMap__[(*attr).id()]);
      }

      // For PRMAggregate<GUM_SCALAR> we add parents
      for (const auto agg: type().aggregates()) {
        PRMAttribute< GUM_SCALAR >& attr = get(agg->safeName());

        for (const auto node: type().containerDag().parents(agg->id())) {
          try {
            attr.addParent(get(node));
          } catch (NotFound&) {
            auto elt = &(type().get(node));
            auto sc = static_cast< PRMSlotChain< GUM_SCALAR >* >(elt);

            try {
              const auto& instances = getInstances(sc->id());

              for (const auto inst: instances) {
                attr.addParent(inst->get(sc->lastElt().safeName()));
              }
            } catch (NotFound&) {   // there is no parents for this agg
            }
          }
        }
      }
    }

    template < typename GUM_SCALAR >
    void PRMInstance< GUM_SCALAR >::instantiateSlotChain__(
       PRMSlotChain< GUM_SCALAR >* sc) {
      auto first_id = sc->chain()[0]->id();
      if (!referenceMap__.exists(first_id)) { return; }
      auto set =
         new Set< PRMInstance< GUM_SCALAR >* >(*(referenceMap__[first_id]));
      // We proceed with a width-first run of the slot chain
      for (Size idx = 1; idx < sc->chain().size() - 1; ++idx) {
        auto temp = new Set< PRMInstance< GUM_SCALAR >* >();
        for (auto current: *set) {
          auto& ref = current->type().get(sc->chain()[idx]->name());
          for (auto next: *(current->referenceMap__[ref.id()])) {
            temp->insert(next);
          }
        }
        delete set;
        set = temp;
      }

      GUM_ASSERT(set->size() > 0);
      // set contains all the instances references by sc
      if (referenceMap__.exists(sc->id())) {
        delete referenceMap__[sc->id()];
        referenceMap__[sc->id()] = set;
      } else {
        referenceMap__.insert(sc->id(), set);
      }

      // Add refering instances
      for (auto i: *set) {
        addReferingInstance__(sc, i);
      }

      // If sc is not multiple so it can be added as a parent of an attribute
      if (!sc->isMultiple()) {
        // We should have only one instance
        // Less ugly way to get the single instance in set
        for (auto instance: *set) {
          auto& attr = instance->get(sc->lastElt().safeName());
          bijection__.insert(&(sc->type().variable()), &(attr.type().variable()));
        }
      }
    }

    template < typename GUM_SCALAR >
    void PRMInstance< GUM_SCALAR >::add(NodeId                     id,
                                        PRMInstance< GUM_SCALAR >& instance) {
      PRMClassElement< GUM_SCALAR >* elt = 0;

      try {
        elt = &(type().get(id));
      } catch (NotFound&) {
        GUM_ERROR(NotFound, "no ClassElement<GUM_SCALAR> matches the given id");
      }

      switch (elt->elt_type()) {
        case PRMClassElement< GUM_SCALAR >::prm_refslot: {
          PRMReferenceSlot< GUM_SCALAR >* ref =
             static_cast< PRMReferenceSlot< GUM_SCALAR >* >(elt);

          // Checking if instance's type is legal
          if (!instance.type().isSubTypeOf(ref->slotType())) {
            GUM_ERROR(TypeError,
                      "given Instance type is not a proper "
                      "subclass of the ReferenceSlot<GUM_SCALAR> slot type");
          }

          // Checking the reference's size limit
          if (referenceMap__.exists(id)
              && (!static_cast< PRMReferenceSlot< GUM_SCALAR >& >(type().get(id))
                      .isArray())
              && (referenceMap__[id]->size() == 1)) {
            GUM_ERROR(OutOfUpperBound,
                      "ReferenceSlot<GUM_SCALAR> size limit reached");
          }

          break;
        }

        case PRMClassElement< GUM_SCALAR >::prm_slotchain: {
          PRMSlotChain< GUM_SCALAR >& sc =
             static_cast< PRMSlotChain< GUM_SCALAR >& >(type().get(id));

          // Checking if instance's type is legal
          if (!instance.type().isSubTypeOf(sc.end())) {
            GUM_ERROR(TypeError,
                      "given Instance type is not a proper "
                      "subclass of the ClassElementContainer pointed"
                      " by the SlotChain<GUM_SCALAR>");
          }

          // Checking the reference's size limit
          if (referenceMap__.exists(id)
              && (!static_cast< PRMSlotChain< GUM_SCALAR >& >(type().get(id))
                      .isMultiple())
              && (referenceMap__[id]->size() == 1)) {
            GUM_ERROR(OutOfUpperBound, "SlotChain<GUM_SCALAR> size limit reached");
          }

          break;
        }

        default: {
          if (!type().isOutputNode(*elt)) {
            GUM_ERROR(WrongClassElement,
                      "given ClassElement<GUM_SCALAR> is not an output node");
          }
        }
      }

      if (!referenceMap__.exists(id)) {
        referenceMap__.insert(id, new Set< PRMInstance< GUM_SCALAR >* >());
      }

      referenceMap__[id]->insert(&instance);
    }

    template < typename GUM_SCALAR >
    INLINE Size PRMInstance< GUM_SCALAR >::size() const {
      return nodeIdMap__.size();
    }

    template < typename GUM_SCALAR >
    INLINE void PRMInstance< GUM_SCALAR >::copyAggregates__(
       PRMAggregate< GUM_SCALAR >* source) {
      auto attr = new PRMScalarAttribute< GUM_SCALAR >(
         source->name(), source->type(), source->buildImpl());
      GUM_ASSERT(&(attr->type().variable()) != &(source->type().variable()));
      attr->setId(source->id());
      nodeIdMap__.insert(attr->id(), attr);
      bijection__.insert(&(source->type().variable()), &(attr->type().variable()));
    }

    template < typename GUM_SCALAR >
    INLINE void PRMInstance< GUM_SCALAR >::copyAttribute__(
       PRMAttribute< GUM_SCALAR >* source) {
      auto attr =
         new PRMScalarAttribute< GUM_SCALAR >(source->name(), source->type());
      GUM_ASSERT(&(attr->type().variable()) != &(source->type().variable()));
      // The potential is copied when instantiate() is called
      attr->cpf().fill((GUM_SCALAR)0);
      attr->setId(source->id());
      bijection__.insert(&(source->type().variable()), &(attr->type().variable()));
      nodeIdMap__.insert(attr->id(), attr);
    }

    template < typename GUM_SCALAR >
    INLINE PRMInstance< GUM_SCALAR >::PRMInstance(
       const PRMInstance< GUM_SCALAR >& source) :
        PRMObject(source),
        type__(source.type__) {
      GUM_CONS_CPY(PRMInstance);
      GUM_ERROR(FatalError, "do not copy Instance");
    }

    template < typename GUM_SCALAR >
    INLINE PRMInstance< GUM_SCALAR >& /**/
       PRMInstance< GUM_SCALAR >::operator=(const PRMClass< GUM_SCALAR >& from) {
      GUM_ERROR(FatalError, "do not copy Instance");
    }

    template < typename GUM_SCALAR >
    INLINE PRMObject::prm_type PRMInstance< GUM_SCALAR >::obj_type() const {
      return PRMObject::prm_type::INSTANCE;
    }

    template < typename GUM_SCALAR >
    INLINE PRMClass< GUM_SCALAR >& PRMInstance< GUM_SCALAR >::type() {
      return *type__;
    }

    template < typename GUM_SCALAR >
    INLINE const PRMClass< GUM_SCALAR >& PRMInstance< GUM_SCALAR >::type() const {
      return *type__;
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMInstance< GUM_SCALAR >::exists(NodeId id) const {
      return nodeIdMap__.exists(id);
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMInstance< GUM_SCALAR >::exists(const std::string& name) const {
      return type__->exists(name) && exists(type__->get(name).id());
    }

    template < typename GUM_SCALAR >
    INLINE PRMAttribute< GUM_SCALAR >& PRMInstance< GUM_SCALAR >::get(NodeId id) {
      try {
        return *(nodeIdMap__[id]);
      } catch (NotFound&) {
        GUM_ERROR(NotFound, "no PRMAttribute<GUM_SCALAR> with the given NodeId");
      }
    }

    template < typename GUM_SCALAR >
    INLINE const PRMAttribute< GUM_SCALAR >&
                 PRMInstance< GUM_SCALAR >::get(NodeId id) const {
      try {
        return *(nodeIdMap__[id]);
      } catch (NotFound&) {
        GUM_ERROR(NotFound, "no PRMAttribute<GUM_SCALAR> with the given NodeId");
      }
    }

    template < typename GUM_SCALAR >
    INLINE PRMAttribute< GUM_SCALAR >&
           PRMInstance< GUM_SCALAR >::get(const std::string& name) {
      try {
        return *(nodeIdMap__[type().get(name).id()]);
      } catch (NotFound&) {
        GUM_ERROR(NotFound, "no PRMAttribute<GUM_SCALAR> with the given name");
      }
    }

    template < typename GUM_SCALAR >
    INLINE const PRMAttribute< GUM_SCALAR >&
                 PRMInstance< GUM_SCALAR >::get(const std::string& name) const {
      try {
        return *(nodeIdMap__[type().get(name).id()]);
      } catch (NotFound&) {
        GUM_ERROR(NotFound, "no PRMAttribute<GUM_SCALAR> with the given name");
      }
    }

    template < typename GUM_SCALAR >
    INLINE void PRMInstance< GUM_SCALAR >::addReferingInstance__(
       PRMSlotChain< GUM_SCALAR >* sc, PRMInstance< GUM_SCALAR >* i) {
      NodeId      id = i->get(sc->lastElt().safeName()).id();
      std::string name = sc->lastElt().safeName();

      try {
        i->referenceMap__[id]->insert(this);
        i->referingAttr__[id]->push_back(
           std::make_pair(this, sc->lastElt().safeName()));
      } catch (NotFound&) {
        i->referenceMap__.insert(id, new Set< PRMInstance< GUM_SCALAR >* >());
        i->referenceMap__[id]->insert(this);
        i->referingAttr__.insert(id, new std::vector< pair >());
        i->referingAttr__[id]->push_back(
           std::make_pair(this, sc->lastElt().safeName()));
      }
    }

    template < typename GUM_SCALAR >
    INLINE const Bijection< const DiscreteVariable*, const DiscreteVariable* >&
                 PRMInstance< GUM_SCALAR >::bijection() const {
      return bijection__;
    }

    template < typename GUM_SCALAR >
    INLINE const PRMInstance< GUM_SCALAR >&
                 PRMInstance< GUM_SCALAR >::getInstance(NodeId id) const {
      try {
        if (referenceMap__[id]->size() > 0) {
          return **(referenceMap__[id]->begin());
        } else {
          GUM_ERROR(UndefinedElement,
                    "no Instance associated with the given NodeId");
        }
      } catch (NotFound&) {
        GUM_ERROR(NotFound,
                  "no ReferenceSlot<GUM_SCALAR> or SlotChain<GUM_SCALAR> "
                  "matches the given NodeId");
      }
    }

    template < typename GUM_SCALAR >
    INLINE const Set< PRMInstance< GUM_SCALAR >* >&
                 PRMInstance< GUM_SCALAR >::getInstances(NodeId id) const {
      try {
        return *(referenceMap__[id]);
      } catch (NotFound&) {
        GUM_ERROR(NotFound,
                  "no ReferenceSlot<GUM_SCALAR> or SlotChain<GUM_SCALAR> "
                  "matches the given NodeId");
      }
    }

    template < typename GUM_SCALAR >
    INLINE typename PRMInstance< GUM_SCALAR >::iterator
       PRMInstance< GUM_SCALAR >::begin() {
      return nodeIdMap__.begin();
    }

    template < typename GUM_SCALAR >
    INLINE const typename PRMInstance< GUM_SCALAR >::iterator&
       PRMInstance< GUM_SCALAR >::end() {
      return nodeIdMap__.end();
    }

    template < typename GUM_SCALAR >
    INLINE typename PRMInstance< GUM_SCALAR >::const_iterator
       PRMInstance< GUM_SCALAR >::begin() const {
      return nodeIdMap__.begin();
    }

    template < typename GUM_SCALAR >
    INLINE const typename PRMInstance< GUM_SCALAR >::const_iterator&
       PRMInstance< GUM_SCALAR >::end() const {
      return nodeIdMap__.end();
    }

    template < typename GUM_SCALAR >
    INLINE typename PRMInstance< GUM_SCALAR >::RefIterator
       PRMInstance< GUM_SCALAR >::begin(NodeId id) {
      try {
        return PRMInstance< GUM_SCALAR >::RefIterator(*(referenceMap__[id]));
      } catch (NotFound&) {
        GUM_ERROR(NotFound, "no referred instances from this NodeId");
      }
    }

    template < typename GUM_SCALAR >
    INLINE typename PRMInstance< GUM_SCALAR >::RefConstIterator
       PRMInstance< GUM_SCALAR >::begin(NodeId id) const {
      try {
        return PRMInstance< GUM_SCALAR >::RefConstIterator(*(referenceMap__[id]));
      } catch (NotFound&) {
        GUM_ERROR(NotFound, "no referred instances from this NodeId");
      }
    }

    template < typename GUM_SCALAR >
    INLINE PRMInstance< GUM_SCALAR >::RefIterator::RefIterator(
       Set< PRMInstance< GUM_SCALAR >* >& set) :
        set__(set),
        iter__(set.begin()) {
      GUM_CONSTRUCTOR(PRMInstance< GUM_SCALAR >::RefIterator);
    }

    template < typename GUM_SCALAR >
    INLINE PRMInstance< GUM_SCALAR >::RefIterator::RefIterator(
       const RefIterator& from) :
        set__(const_cast< Set< PRMInstance< GUM_SCALAR >* >& >(from.set__)),
        iter__(from.iter__) {
      GUM_CONS_CPY(PRMInstance< GUM_SCALAR >::RefIterator);
    }

    template < typename GUM_SCALAR >
    INLINE PRMInstance< GUM_SCALAR >::RefIterator::~RefIterator() {
      GUM_DESTRUCTOR(PRMInstance< GUM_SCALAR >::RefIterator);
    }

    template < typename GUM_SCALAR >
    INLINE typename PRMInstance< GUM_SCALAR >::RefIterator&
       PRMInstance< GUM_SCALAR >::RefIterator::operator=(const RefIterator& from) {
      iter__ = from.iter__;
      return *this;
    }

    template < typename GUM_SCALAR >
    INLINE typename PRMInstance< GUM_SCALAR >::RefIterator&
       PRMInstance< GUM_SCALAR >::RefIterator::operator++() {
      ++iter__;
      return *this;
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMInstance< GUM_SCALAR >::RefIterator::isEnd() const {
      return iter__ == set__.end();
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMInstance< GUM_SCALAR >::RefIterator::operator!=(
       const RefIterator& from) const {
      return iter__ != from.iter__;
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMInstance< GUM_SCALAR >::RefIterator::operator==(
       const RefIterator& from) const {
      return iter__ == from.iter__;
    }

    template < typename GUM_SCALAR >
    INLINE PRMInstance< GUM_SCALAR >&
       PRMInstance< GUM_SCALAR >::RefIterator::operator*() const {
      return **iter__;
    }

    template < typename GUM_SCALAR >
    INLINE PRMInstance< GUM_SCALAR >*
       PRMInstance< GUM_SCALAR >::RefIterator::operator->() const {
      return *iter__;
    }

    template < typename GUM_SCALAR >
    INLINE PRMInstance< GUM_SCALAR >::RefConstIterator::RefConstIterator(
       const Set< PRMInstance< GUM_SCALAR >* >& set) :
        set__(set),
        iter__(set.begin()) {
      GUM_CONSTRUCTOR(PRMInstance< GUM_SCALAR >::RefConstIterator);
    }

    template < typename GUM_SCALAR >
    INLINE PRMInstance< GUM_SCALAR >::RefConstIterator::RefConstIterator(
       const RefConstIterator& from) :
        set__(from.set__),
        iter__(from.iter__) {
      GUM_CONS_CPY(PRMInstance< GUM_SCALAR >::RefConstIterator);
    }

    template < typename GUM_SCALAR >
    INLINE PRMInstance< GUM_SCALAR >::RefConstIterator::~RefConstIterator() {
      GUM_DESTRUCTOR(PRMInstance< GUM_SCALAR >::RefConstIterator);
    }

    template < typename GUM_SCALAR >
    INLINE typename PRMInstance< GUM_SCALAR >::RefConstIterator&
       PRMInstance< GUM_SCALAR >::RefConstIterator::operator=(
          const RefConstIterator& from) {
      iter__ = from.iter__;
      return *this;
    }

    template < typename GUM_SCALAR >
    INLINE typename PRMInstance< GUM_SCALAR >::RefConstIterator&
       PRMInstance< GUM_SCALAR >::RefConstIterator::operator++() {
      ++iter__;
      return *this;
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMInstance< GUM_SCALAR >::RefConstIterator::isEnd() const {
      return iter__ == set__.end();
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMInstance< GUM_SCALAR >::RefConstIterator::operator!=(
       const RefConstIterator& from) const {
      return iter__ != from.iter__;
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMInstance< GUM_SCALAR >::RefConstIterator::operator==(
       const RefConstIterator& from) const {
      return iter__ == from.iter__;
    }

    template < typename GUM_SCALAR >
    INLINE const PRMInstance< GUM_SCALAR >&
       PRMInstance< GUM_SCALAR >::RefConstIterator::operator*() const {
      return **iter__;
    }

    template < typename GUM_SCALAR >
    INLINE const PRMInstance< GUM_SCALAR >*
       PRMInstance< GUM_SCALAR >::RefConstIterator::operator->() const {
      return *iter__;
    }

    template < typename GUM_SCALAR >
    INLINE typename PRMInstance< GUM_SCALAR >::InvRefIterator
       PRMInstance< GUM_SCALAR >::beginInvRef() {
      return referingAttr__.begin();
    }

    template < typename GUM_SCALAR >
    INLINE const typename PRMInstance< GUM_SCALAR >::InvRefIterator&
       PRMInstance< GUM_SCALAR >::endInvRef() {
      return referingAttr__.end();
    }

    template < typename GUM_SCALAR >
    INLINE typename PRMInstance< GUM_SCALAR >::InvRefConstIterator
       PRMInstance< GUM_SCALAR >::beginInvRef() const {
      return referingAttr__.begin();
    }

    template < typename GUM_SCALAR >
    INLINE const typename PRMInstance< GUM_SCALAR >::InvRefConstIterator&
       PRMInstance< GUM_SCALAR >::endInvRef() const {
      return referingAttr__.end();
    }

    template < typename GUM_SCALAR >
    INLINE std::vector< std::pair< PRMInstance< GUM_SCALAR >*, std::string > >&
           PRMInstance< GUM_SCALAR >::getRefAttr(NodeId id) {
      return *(referingAttr__[id]);
    }

    template < typename GUM_SCALAR >
    INLINE const
       std::vector< std::pair< PRMInstance< GUM_SCALAR >*, std::string > >&
       PRMInstance< GUM_SCALAR >::getRefAttr(NodeId id) const {
      return *(referingAttr__[id]);
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMInstance< GUM_SCALAR >::hasRefAttr(NodeId id) const {
      return referingAttr__.exists(id) && (!referingAttr__[id]->empty());
    }

    template < typename GUM_SCALAR >
    INLINE void PRMInstance< GUM_SCALAR >::copyAttributeCPF__(
       PRMAttribute< GUM_SCALAR >* attr) {
      const auto& type_attr = static_cast< const PRMAttribute< GUM_SCALAR >& >(
         type().get(attr->safeName()));
      attr->copyCpf(bijection(), type_attr);
      GUM_ASSERT(attr->cpf().contains(attr->type().variable()));
    }

  } /* namespace prm */
} /* namespace gum */
