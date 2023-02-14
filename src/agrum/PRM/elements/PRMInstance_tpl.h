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
 * @brief Inline implementation of gum::PRMInstance
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/PRM/elements/PRMInstance.h>

#include <agrum/tools/multidim/implementations/multiDimSparse.h>

namespace gum {
  namespace prm {
    template < typename GUM_SCALAR >
    PRMInstance< GUM_SCALAR >::PRMInstance(const std::string& name, PRMClass< GUM_SCALAR >& type) :
        PRMObject(name), _instantiated_(false), _type_(&type) {
      GUM_CONSTRUCTOR(PRMInstance);

      // First we create attributes for each aggregate in type
      for (const auto agg: _type_->aggregates())
        _copyAggregates_(agg);

      // We add attributes in type by reference for inner ones and by copy for
      // output ones
      for (const auto attr: _type_->attributes())
        _copyAttribute_(attr);
    }

    template < typename GUM_SCALAR >
    PRMInstance< GUM_SCALAR >::~PRMInstance() {
      GUM_DESTRUCTOR(PRMInstance);

      for (const auto& elt: _nodeIdMap_)
        delete elt.second;

      for (const auto& elt: _referenceMap_)
        delete elt.second;

      for (const auto& elt: _referingAttr_)
        delete elt.second;
    }

    template < typename GUM_SCALAR >
    void PRMInstance< GUM_SCALAR >::instantiate() {
      if (!_instantiated_) {
        _instantiated_ = true;
        _doInstantiate_();
      }
    }

    template < typename GUM_SCALAR >
    void PRMInstance< GUM_SCALAR >::_doInstantiate_() {
      // First retrieving any referenced instance
      for (const auto chain: type().slotChains()) {
        _instantiateSlotChain_(chain);
      }

      // Now we need to add referred instance to each input node
      // For Attributes we first add parents, then we initialize CPF
      for (const auto attr: type().attributes()) {
        _copyAttributeCPF_(_nodeIdMap_[(*attr).id()]);
      }

      // For PRMAggregate<GUM_SCALAR> we add parents
      for (const auto agg: type().aggregates()) {
        PRMAttribute< GUM_SCALAR >& attr = get(agg->safeName());

        for (const auto node: type().containerDag().parents(agg->id())) {
          try {
            attr.addParent(get(node));
          } catch (NotFound const&) {
            auto elt = &(type().get(node));
            auto sc  = static_cast< PRMSlotChain< GUM_SCALAR >* >(elt);

            try {
              const auto& instances = getInstances(sc->id());

              for (const auto inst: instances) {
                attr.addParent(inst->get(sc->lastElt().safeName()));
              }
            } catch (NotFound const&) {   // there is no parents for this agg
            }
          }
        }
      }
    }

    template < typename GUM_SCALAR >
    void PRMInstance< GUM_SCALAR >::_instantiateSlotChain_(PRMSlotChain< GUM_SCALAR >* sc) {
      auto first_id = sc->chain()[0]->id();
      if (!_referenceMap_.exists(first_id)) { return; }
      auto set = new Set< PRMInstance< GUM_SCALAR >* >(*(_referenceMap_[first_id]));
      // We proceed with a width-first run of the slot chain
      for (Size idx = 1; idx < sc->chain().size() - 1; ++idx) {
        auto temp = new Set< PRMInstance< GUM_SCALAR >* >();
        for (auto current: *set) {
          auto& ref = current->type().get(sc->chain()[idx]->name());
          for (auto next: *(current->_referenceMap_[ref.id()])) {
            temp->insert(next);
          }
        }
        delete set;
        set = temp;
      }

      GUM_ASSERT(set->size() > 0);
      // set contains all the instances references by sc
      if (_referenceMap_.exists(sc->id())) {
        delete _referenceMap_[sc->id()];
        _referenceMap_[sc->id()] = set;
      } else {
        _referenceMap_.insert(sc->id(), set);
      }

      // Add refering instances
      for (auto i: *set) {
        _addReferingInstance_(sc, i);
      }

      // If sc is not multiple so it can be added as a parent of an attribute
      if (!sc->isMultiple()) {
        // We should have only one instance
        // Less ugly way to get the single instance in set
        for (auto instance: *set) {
          auto& attr = instance->get(sc->lastElt().safeName());
          _bijection_.insert(&(sc->type().variable()), &(attr.type().variable()));
        }
      }
    }

    template < typename GUM_SCALAR >
    void PRMInstance< GUM_SCALAR >::add(NodeId id, PRMInstance< GUM_SCALAR >& instance) {
      PRMClassElement< GUM_SCALAR >* elt = 0;

      try {
        elt = &(type().get(id));
      } catch (NotFound const&) {
        GUM_ERROR(NotFound, "no ClassElement<GUM_SCALAR> matches the given id")
      }

      switch (elt->elt_type()) {
        case PRMClassElement< GUM_SCALAR >::prm_refslot: {
          PRMReferenceSlot< GUM_SCALAR >* ref = static_cast< PRMReferenceSlot< GUM_SCALAR >* >(elt);

          // Checking if instance's type is legal
          if (!instance.type().isSubTypeOf(ref->slotType())) {
            GUM_ERROR(PRMTypeError,
                      "given Instance type is not a proper "
                      "subclass of the ReferenceSlot<GUM_SCALAR> slot type");
          }

          // Checking the reference's size limit
          if (_referenceMap_.exists(id)
              && (!static_cast< PRMReferenceSlot< GUM_SCALAR >& >(type().get(id)).isArray())
              && (_referenceMap_[id]->size() == 1)) {
            GUM_ERROR(OutOfBounds, "ReferenceSlot<GUM_SCALAR> size limit reached")
          }

          break;
        }

        case PRMClassElement< GUM_SCALAR >::prm_slotchain: {
          PRMSlotChain< GUM_SCALAR >& sc
             = static_cast< PRMSlotChain< GUM_SCALAR >& >(type().get(id));

          // Checking if instance's type is legal
          if (!instance.type().isSubTypeOf(sc.end())) {
            GUM_ERROR(PRMTypeError,
                      "given Instance type is not a proper "
                      "subclass of the ClassElementContainer pointed"
                      " by the SlotChain<GUM_SCALAR>");
          }

          // Checking the reference's size limit
          if (_referenceMap_.exists(id)
              && (!static_cast< PRMSlotChain< GUM_SCALAR >& >(type().get(id)).isMultiple())
              && (_referenceMap_[id]->size() == 1)) {
            GUM_ERROR(OutOfBounds, "SlotChain<GUM_SCALAR> size limit reached")
          }

          break;
        }

        default: {
          if (!type().isOutputNode(*elt)) {
            GUM_ERROR(WrongClassElement, "given ClassElement<GUM_SCALAR> is not an output node")
          }
        }
      }

      if (!_referenceMap_.exists(id)) {
        _referenceMap_.insert(id, new Set< PRMInstance< GUM_SCALAR >* >());
      }

      _referenceMap_[id]->insert(&instance);
    }

    template < typename GUM_SCALAR >
    INLINE Size PRMInstance< GUM_SCALAR >::size() const {
      return _nodeIdMap_.size();
    }

    template < typename GUM_SCALAR >
    INLINE void PRMInstance< GUM_SCALAR >::_copyAggregates_(PRMAggregate< GUM_SCALAR >* source) {
      auto attr = new PRMScalarAttribute< GUM_SCALAR >(source->name(),
                                                       source->type(),
                                                       source->buildImpl());
      GUM_ASSERT(&(attr->type().variable()) != &(source->type().variable()));
      attr->setId(source->id());
      _nodeIdMap_.insert(attr->id(), attr);
      _bijection_.insert(&(source->type().variable()), &(attr->type().variable()));
    }

    template < typename GUM_SCALAR >
    INLINE void PRMInstance< GUM_SCALAR >::_copyAttribute_(PRMAttribute< GUM_SCALAR >* source) {
      auto attr = new PRMScalarAttribute< GUM_SCALAR >(source->name(), source->type());
      GUM_ASSERT(&(attr->type().variable()) != &(source->type().variable()));
      // The potential is copied when instantiate() is called
      attr->cpf().fill((GUM_SCALAR)0);
      attr->setId(source->id());
      _bijection_.insert(&(source->type().variable()), &(attr->type().variable()));
      _nodeIdMap_.insert(attr->id(), attr);
    }

    template < typename GUM_SCALAR >
    INLINE PRMInstance< GUM_SCALAR >::PRMInstance(const PRMInstance< GUM_SCALAR >& source) :
        PRMObject(source), _type_(source._type_) {
      GUM_CONS_CPY(PRMInstance);
      GUM_ERROR(FatalError, "do not copy Instance")
    }

    template < typename GUM_SCALAR >
    INLINE PRMInstance< GUM_SCALAR >& /**/
       PRMInstance< GUM_SCALAR >::operator=(const PRMClass< GUM_SCALAR >& from) {
      GUM_ERROR(FatalError, "do not copy Instance")
    }

    template < typename GUM_SCALAR >
    INLINE PRMObject::prm_type PRMInstance< GUM_SCALAR >::obj_type() const {
      return PRMObject::prm_type::INSTANCE;
    }

    template < typename GUM_SCALAR >
    INLINE PRMClass< GUM_SCALAR >& PRMInstance< GUM_SCALAR >::type() {
      return *_type_;
    }

    template < typename GUM_SCALAR >
    INLINE const PRMClass< GUM_SCALAR >& PRMInstance< GUM_SCALAR >::type() const {
      return *_type_;
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMInstance< GUM_SCALAR >::exists(NodeId id) const {
      return _nodeIdMap_.exists(id);
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMInstance< GUM_SCALAR >::exists(const std::string& name) const {
      return _type_->exists(name) && exists(_type_->get(name).id());
    }

    template < typename GUM_SCALAR >
    INLINE PRMAttribute< GUM_SCALAR >& PRMInstance< GUM_SCALAR >::get(NodeId id) {
      try {
        return *(_nodeIdMap_[id]);
      } catch (NotFound const&) {
        GUM_ERROR(NotFound, "no PRMAttribute<GUM_SCALAR> with the given NodeId")
      }
    }

    template < typename GUM_SCALAR >
    INLINE const PRMAttribute< GUM_SCALAR >& PRMInstance< GUM_SCALAR >::get(NodeId id) const {
      try {
        return *(_nodeIdMap_[id]);
      } catch (NotFound const&) {
        GUM_ERROR(NotFound, "no PRMAttribute<GUM_SCALAR> with the given NodeId")
      }
    }

    template < typename GUM_SCALAR >
    INLINE PRMAttribute< GUM_SCALAR >& PRMInstance< GUM_SCALAR >::get(const std::string& name) {
      try {
        return *(_nodeIdMap_[type().get(name).id()]);
      } catch (NotFound const&) {
        GUM_ERROR(NotFound, "no PRMAttribute<GUM_SCALAR> with the given name")
      }
    }

    template < typename GUM_SCALAR >
    INLINE const PRMAttribute< GUM_SCALAR >&
                 PRMInstance< GUM_SCALAR >::get(const std::string& name) const {
      try {
        return *(_nodeIdMap_[type().get(name).id()]);
      } catch (NotFound const&) {
        GUM_ERROR(NotFound, "no PRMAttribute<GUM_SCALAR> with the given name")
      }
    }

    template < typename GUM_SCALAR >
    INLINE void PRMInstance< GUM_SCALAR >::_addReferingInstance_(PRMSlotChain< GUM_SCALAR >* sc,
                                                                 PRMInstance< GUM_SCALAR >*  i) {
      NodeId      id   = i->get(sc->lastElt().safeName()).id();
      std::string name = sc->lastElt().safeName();

      try {
        i->_referenceMap_[id]->insert(this);
        i->_referingAttr_[id]->push_back(std::make_pair(this, sc->lastElt().safeName()));
      } catch (NotFound const&) {
        i->_referenceMap_.insert(id, new Set< PRMInstance< GUM_SCALAR >* >());
        i->_referenceMap_[id]->insert(this);
        i->_referingAttr_.insert(id, new std::vector< pair >());
        i->_referingAttr_[id]->push_back(std::make_pair(this, sc->lastElt().safeName()));
      }
    }

    template < typename GUM_SCALAR >
    INLINE const Bijection< const DiscreteVariable*, const DiscreteVariable* >&
                 PRMInstance< GUM_SCALAR >::bijection() const {
      return _bijection_;
    }

    template < typename GUM_SCALAR >
    INLINE const PRMInstance< GUM_SCALAR >&
                 PRMInstance< GUM_SCALAR >::getInstance(NodeId id) const {
      try {
        if (_referenceMap_[id]->size() > 0) {
          return **(_referenceMap_[id]->begin());
        } else {
          GUM_ERROR(UndefinedElement, "no Instance associated with the given NodeId")
        }
      } catch (NotFound const&) {
        GUM_ERROR(NotFound,
                  "no ReferenceSlot<GUM_SCALAR> or SlotChain<GUM_SCALAR> "
                            "matches the given NodeId");
      }
    }

    template < typename GUM_SCALAR >
    INLINE const Set< PRMInstance< GUM_SCALAR >* >&
                 PRMInstance< GUM_SCALAR >::getInstances(NodeId id) const {
      try {
        return *(_referenceMap_[id]);
      } catch (NotFound const&) {
        GUM_ERROR(NotFound,
                  "no ReferenceSlot<GUM_SCALAR> or SlotChain<GUM_SCALAR> "
                            "matches the given NodeId");
      }
    }

    template < typename GUM_SCALAR >
    INLINE typename PRMInstance< GUM_SCALAR >::iterator PRMInstance< GUM_SCALAR >::begin() {
      return _nodeIdMap_.begin();
    }

    template < typename GUM_SCALAR >
    INLINE const typename PRMInstance< GUM_SCALAR >::iterator& PRMInstance< GUM_SCALAR >::end() {
      return _nodeIdMap_.end();
    }

    template < typename GUM_SCALAR >
    INLINE typename PRMInstance< GUM_SCALAR >::const_iterator
       PRMInstance< GUM_SCALAR >::begin() const {
      return _nodeIdMap_.begin();
    }

    template < typename GUM_SCALAR >
    INLINE const typename PRMInstance< GUM_SCALAR >::const_iterator&
       PRMInstance< GUM_SCALAR >::end() const {
      return _nodeIdMap_.end();
    }

    template < typename GUM_SCALAR >
    INLINE typename PRMInstance< GUM_SCALAR >::RefIterator
       PRMInstance< GUM_SCALAR >::begin(NodeId id) {
      try {
        return PRMInstance< GUM_SCALAR >::RefIterator(*(_referenceMap_[id]));
      } catch (NotFound const&) { GUM_ERROR(NotFound, "no referred instances from this NodeId") }
    }

    template < typename GUM_SCALAR >
    INLINE typename PRMInstance< GUM_SCALAR >::RefConstIterator
       PRMInstance< GUM_SCALAR >::begin(NodeId id) const {
      try {
        return PRMInstance< GUM_SCALAR >::RefConstIterator(*(_referenceMap_[id]));
      } catch (NotFound const&) { GUM_ERROR(NotFound, "no referred instances from this NodeId") }
    }

    template < typename GUM_SCALAR >
    INLINE
       PRMInstance< GUM_SCALAR >::RefIterator::RefIterator(Set< PRMInstance< GUM_SCALAR >* >& set) :
        _set_(set),
        _iter_(set.begin()) {
      GUM_CONSTRUCTOR(PRMInstance< GUM_SCALAR >::RefIterator);
    }

    template < typename GUM_SCALAR >
    INLINE PRMInstance< GUM_SCALAR >::RefIterator::RefIterator(const RefIterator& from) :
        _set_(const_cast< Set< PRMInstance< GUM_SCALAR >* >& >(from._set_)), _iter_(from._iter_) {
      GUM_CONS_CPY(PRMInstance< GUM_SCALAR >::RefIterator);
    }

    template < typename GUM_SCALAR >
    INLINE PRMInstance< GUM_SCALAR >::RefIterator::~RefIterator() {
      GUM_DESTRUCTOR(PRMInstance< GUM_SCALAR >::RefIterator);
    }

    template < typename GUM_SCALAR >
    INLINE typename PRMInstance< GUM_SCALAR >::RefIterator&
       PRMInstance< GUM_SCALAR >::RefIterator::operator=(const RefIterator& from) {
      _iter_ = from._iter_;
      return *this;
    }

    template < typename GUM_SCALAR >
    INLINE typename PRMInstance< GUM_SCALAR >::RefIterator&
       PRMInstance< GUM_SCALAR >::RefIterator::operator++() {
      ++_iter_;
      return *this;
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMInstance< GUM_SCALAR >::RefIterator::isEnd() const {
      return _iter_ == _set_.end();
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMInstance< GUM_SCALAR >::RefIterator::operator!=(const RefIterator& from) const {
      return _iter_ != from._iter_;
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMInstance< GUM_SCALAR >::RefIterator::operator==(const RefIterator& from) const {
      return _iter_ == from._iter_;
    }

    template < typename GUM_SCALAR >
    INLINE PRMInstance< GUM_SCALAR >& PRMInstance< GUM_SCALAR >::RefIterator::operator*() const {
      return **_iter_;
    }

    template < typename GUM_SCALAR >
    INLINE PRMInstance< GUM_SCALAR >* PRMInstance< GUM_SCALAR >::RefIterator::operator->() const {
      return *_iter_;
    }

    template < typename GUM_SCALAR >
    INLINE PRMInstance< GUM_SCALAR >::RefConstIterator::RefConstIterator(
       const Set< PRMInstance< GUM_SCALAR >* >& set) :
        _set_(set),
        _iter_(set.begin()) {
      GUM_CONSTRUCTOR(PRMInstance< GUM_SCALAR >::RefConstIterator);
    }

    template < typename GUM_SCALAR >
    INLINE
       PRMInstance< GUM_SCALAR >::RefConstIterator::RefConstIterator(const RefConstIterator& from) :
        _set_(from._set_),
        _iter_(from._iter_) {
      GUM_CONS_CPY(PRMInstance< GUM_SCALAR >::RefConstIterator);
    }

    template < typename GUM_SCALAR >
    INLINE PRMInstance< GUM_SCALAR >::RefConstIterator::~RefConstIterator() {
      GUM_DESTRUCTOR(PRMInstance< GUM_SCALAR >::RefConstIterator);
    }

    template < typename GUM_SCALAR >
    INLINE typename PRMInstance< GUM_SCALAR >::RefConstIterator&
       PRMInstance< GUM_SCALAR >::RefConstIterator::operator=(const RefConstIterator& from) {
      _iter_ = from._iter_;
      return *this;
    }

    template < typename GUM_SCALAR >
    INLINE typename PRMInstance< GUM_SCALAR >::RefConstIterator&
       PRMInstance< GUM_SCALAR >::RefConstIterator::operator++() {
      ++_iter_;
      return *this;
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMInstance< GUM_SCALAR >::RefConstIterator::isEnd() const {
      return _iter_ == _set_.end();
    }

    template < typename GUM_SCALAR >
    INLINE bool
       PRMInstance< GUM_SCALAR >::RefConstIterator::operator!=(const RefConstIterator& from) const {
      return _iter_ != from._iter_;
    }

    template < typename GUM_SCALAR >
    INLINE bool
       PRMInstance< GUM_SCALAR >::RefConstIterator::operator==(const RefConstIterator& from) const {
      return _iter_ == from._iter_;
    }

    template < typename GUM_SCALAR >
    INLINE const PRMInstance< GUM_SCALAR >&
       PRMInstance< GUM_SCALAR >::RefConstIterator::operator*() const {
      return **_iter_;
    }

    template < typename GUM_SCALAR >
    INLINE const PRMInstance< GUM_SCALAR >*
       PRMInstance< GUM_SCALAR >::RefConstIterator::operator->() const {
      return *_iter_;
    }

    template < typename GUM_SCALAR >
    INLINE typename PRMInstance< GUM_SCALAR >::InvRefIterator
       PRMInstance< GUM_SCALAR >::beginInvRef() {
      return _referingAttr_.begin();
    }

    template < typename GUM_SCALAR >
    INLINE const typename PRMInstance< GUM_SCALAR >::InvRefIterator&
       PRMInstance< GUM_SCALAR >::endInvRef() {
      return _referingAttr_.end();
    }

    template < typename GUM_SCALAR >
    INLINE typename PRMInstance< GUM_SCALAR >::InvRefConstIterator
       PRMInstance< GUM_SCALAR >::beginInvRef() const {
      return _referingAttr_.begin();
    }

    template < typename GUM_SCALAR >
    INLINE const typename PRMInstance< GUM_SCALAR >::InvRefConstIterator&
       PRMInstance< GUM_SCALAR >::endInvRef() const {
      return _referingAttr_.end();
    }

    template < typename GUM_SCALAR >
    INLINE std::vector< std::pair< PRMInstance< GUM_SCALAR >*, std::string > >&
           PRMInstance< GUM_SCALAR >::getRefAttr(NodeId id) {
      return *(_referingAttr_[id]);
    }

    template < typename GUM_SCALAR >
    INLINE const std::vector< std::pair< PRMInstance< GUM_SCALAR >*, std::string > >&
                 PRMInstance< GUM_SCALAR >::getRefAttr(NodeId id) const {
      return *(_referingAttr_[id]);
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMInstance< GUM_SCALAR >::hasRefAttr(NodeId id) const {
      return _referingAttr_.exists(id) && (!_referingAttr_[id]->empty());
    }

    template < typename GUM_SCALAR >
    INLINE void PRMInstance< GUM_SCALAR >::_copyAttributeCPF_(PRMAttribute< GUM_SCALAR >* attr) {
      const auto& type_attr
         = static_cast< const PRMAttribute< GUM_SCALAR >& >(type().get(attr->safeName()));
      attr->copyCpf(bijection(), type_attr);
      GUM_ASSERT(attr->cpf().contains(attr->type().variable()));
    }

  } /* namespace prm */
} /* namespace gum */
