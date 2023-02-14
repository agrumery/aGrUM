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
 * @brief Inline implementation of gum::prm::PRMInterface
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <agrum/PRM/elements/PRMInterface.h>

#include <agrum/PRM/elements/PRMClass.h>

namespace gum {
  namespace prm {

    template < typename GUM_SCALAR >
    PRMInterface< GUM_SCALAR >::PRMInterface(const std::string& name) :
        PRMClassElementContainer< GUM_SCALAR >(name), _superInterface_(0) {
      GUM_CONSTRUCTOR(PRMInterface);
    }

    template < typename GUM_SCALAR >
    PRMInterface< GUM_SCALAR >::PRMInterface(const std::string&          name,
                                             PRMInterface< GUM_SCALAR >& super,
                                             bool                        delayInheritance) :
        PRMClassElementContainer< GUM_SCALAR >(name),
        _superInterface_(&super) {
      GUM_CONSTRUCTOR(PRMInterface);
      if (!delayInheritance) { _inheritInterface_(super); }
    }

    template < typename GUM_SCALAR >
    PRMInterface< GUM_SCALAR >::PRMInterface(const PRMInterface< GUM_SCALAR >& source) :
        PRMClassElementContainer< GUM_SCALAR >(source.name()), _dag_(source._dag_),
        _superInterface_(source._superInterface_) {
      GUM_CONS_CPY(PRMInterface);
      GUM_ERROR(FatalError, "don't copy an interface")
    }

    template < typename GUM_SCALAR >
    PRMInterface< GUM_SCALAR >::~PRMInterface() {
      GUM_DESTRUCTOR(PRMInterface);

      for (const auto& elt: _nodeIdMap_) {
        delete elt.second;
      }
    }

    template < typename GUM_SCALAR >
    void PRMInterface< GUM_SCALAR >::inheritInterface() {
      if (_superInterface_ != nullptr) { _inheritInterface_(*_superInterface_); }
    }

    template < typename GUM_SCALAR >
    void PRMInterface< GUM_SCALAR >::_inheritInterface_(const PRMInterface< GUM_SCALAR >& i) {
      // Copying attributes
      for (const auto i_attr: i._attributes_) {
        auto attr = new PRMScalarAttribute< GUM_SCALAR >(i_attr->name(), i_attr->type());
        attr->setId(i_attr->id());
        _nodeIdMap_.insert(attr->id(), attr);
        _attributes_.insert(attr);

        if (i._nameMap_[i_attr->name()] == i._nameMap_[i_attr->safeName()]) {
          _nameMap_.insert(attr->name(), attr);
        }

        _nameMap_.insert(attr->safeName(), attr);
        _dag_.addNodeWithId(attr->id());
      }

      // Copying reference slots
      for (const auto i_ref: i._referenceSlots_) {
        auto ref = new PRMReferenceSlot< GUM_SCALAR >(
           i_ref->name(),
           const_cast< PRMClassElementContainer< GUM_SCALAR >& >(i_ref->slotType()),
           i_ref->isArray());

        ref->setId(i_ref->id());
        _nodeIdMap_.insert(ref->id(), ref);
        _referenceSlots_.insert(ref);

        if (i._nameMap_.exists(ref->name())) { _nameMap_.insert(ref->name(), ref); }

        _nameMap_.insert(ref->safeName(), ref);
        _dag_.addNodeWithId(ref->id());
      }
    }

    template < typename GUM_SCALAR >
    NodeId PRMInterface< GUM_SCALAR >::add(PRMClassElement< GUM_SCALAR >* elt) {
      if (_nameMap_.exists(elt->name())) {
        GUM_ERROR(DuplicateElement,
                  "name '" << elt->name() << "' is already used by another ClassElement");
      }

      if (PRMClassElement< GUM_SCALAR >::isAttribute(*elt)) {
        PRMAttribute< GUM_SCALAR >* attr = static_cast< PRMAttribute< GUM_SCALAR >* >(elt);
        _nameMap_.insert(attr->name(), attr);

        while (true) {
          attr->setId(nextNodeId());
          _dag_.addNodeWithId(attr->id());
          _nodeIdMap_.insert(attr->id(), attr);
          _nameMap_.insert(attr->safeName(), attr);
          _attributes_.insert(attr);

          if (attr->type().isSubType()) {
            attr = attr->getCastDescendant();
          } else {
            break;
          }
        }
      } else if (PRMClassElement< GUM_SCALAR >::isReferenceSlot(*elt)) {
        elt->setId(nextNodeId());
        _dag_.addNodeWithId(elt->id());
        _nodeIdMap_.insert(elt->id(), elt);
        _referenceSlots_.insert(static_cast< PRMReferenceSlot< GUM_SCALAR >* >(elt));
        _nameMap_.insert(elt->name(), elt);
        _nameMap_.insert(elt->safeName(), elt);
      } else {
        GUM_ERROR(WrongClassElement, "illegal ClassElement<GUM_SCALAR> for an Interface")
      }

      return elt->id();
    }

    template < typename GUM_SCALAR >
    NodeId PRMInterface< GUM_SCALAR >::overload(PRMClassElement< GUM_SCALAR >* overloader) {
      try {
        if (!super().exists(overloader->name()))
          GUM_ERROR(OperationNotAllowed, "found no ClassElement<GUM_SCALAR> to overload")

      } catch (NotFound const&) {
        GUM_ERROR(OperationNotAllowed, "overload is possible only with sub interfaces")
      }

      PRMClassElement< GUM_SCALAR >* overloaded = _nameMap_[overloader->name()];
      if (overloaded == overloader)
        GUM_ERROR(DuplicateElement, "duplicate ClassElement '" << overloader->name() << "'")

      if (!_checkOverloadLegality_(overloaded, overloader))
        GUM_ERROR(OperationNotAllowed, "illegal overload")

      switch (overloader->elt_type()) {
        case PRMClassElement< GUM_SCALAR >::prm_attribute: {
          auto attr_overloader = static_cast< PRMAttribute< GUM_SCALAR >* >(overloader);
          auto attr_overloaded = static_cast< PRMAttribute< GUM_SCALAR >* >(overloaded);
          _overloadAttribute_(attr_overloader, attr_overloaded);
          break;
        }

        case PRMClassElement< GUM_SCALAR >::prm_refslot: {
          auto ref_overloader = static_cast< PRMReferenceSlot< GUM_SCALAR >* >(overloader);
          auto ref_overloaded = static_cast< PRMReferenceSlot< GUM_SCALAR >* >(overloaded);
          _overloadReferenceSlot_(ref_overloader, ref_overloaded);
          break;
        }

        case PRMClassElement< GUM_SCALAR >::prm_aggregate:
        case PRMClassElement< GUM_SCALAR >::prm_slotchain:
          GUM_ERROR(OperationNotAllowed,
                    "Element " << overloader->name() << " can not be overloaded")
        default:
          GUM_ERROR(FatalError, "Unknown ClassElement<GUM_SCALAR> type for " << overloader->name())
      }

      return overloader->id();
    }

    template < typename GUM_SCALAR >
    void PRMInterface< GUM_SCALAR >::_overloadAttribute_(PRMAttribute< GUM_SCALAR >* overloader,
                                                         PRMAttribute< GUM_SCALAR >* overloaded) {
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
        // Swapping types, ugly but necessary to preserve the
        // PRMType<GUM_SCALAR>
        // pointer of overloaded
        overloader->overload(overloaded);
        delete overloaded;
      }
    }

    template < typename GUM_SCALAR >
    void PRMInterface< GUM_SCALAR >::_overloadReferenceSlot_(
       PRMReferenceSlot< GUM_SCALAR >* overloader,
       PRMReferenceSlot< GUM_SCALAR >* overloaded) {
      // Adding overloading reference
      overloader->setId(overloaded->id());
      _nodeIdMap_[overloader->id()] = overloader;
      _nameMap_[overloader->name()] = overloader;
      _nameMap_.insert(overloader->safeName(), overloader);
      _referenceSlots_.insert(overloader);
      // Removing overloaded PRMReferenceSlot<GUM_SCALAR>
      _referenceSlots_.erase(overloaded);
      _nameMap_.erase(overloaded->safeName());
      delete overloaded;
    }

    template < typename GUM_SCALAR >
    void PRMInterface< GUM_SCALAR >::_addCastDescendants_(PRMAttribute< GUM_SCALAR >* start,
                                                          PRMAttribute< GUM_SCALAR >* end) {
      PRMAttribute< GUM_SCALAR >* parent = start;
      PRMAttribute< GUM_SCALAR >* child  = nullptr;

      while (parent->type().superType() != end->type()) {
        child = parent->getCastDescendant();
        child->setId(nextNodeId());
        _dag_.addNodeWithId(child->id());
        _nodeIdMap_.insert(child->id(), child);
        // Only use child's safe name when adding to the name map!
        _nameMap_.insert(child->safeName(), child);
        _attributes_.insert(child);
        // Do ! use Class<GUM_SCALAR>::insertArc(), child's CPF is already
        // initialized properly
        parent = child;
      }

      parent->setAsCastDescendant(end);
    }

    template < typename GUM_SCALAR >
    bool PRMInterface< GUM_SCALAR >::_checkOverloadLegality_(
       const PRMClassElement< GUM_SCALAR >* overloaded,
       const PRMClassElement< GUM_SCALAR >* overloader) {
      if (overloaded->elt_type() != overloader->elt_type()) { return false; }

      if (overloaded->elt_type() == PRMClassElement< GUM_SCALAR >::prm_attribute) {
        if (!overloader->type().isSubTypeOf(overloaded->type())) { return false; }
      } else if (overloaded->elt_type() == PRMClassElement< GUM_SCALAR >::prm_refslot) {
        auto ref_overloader = static_cast< const PRMReferenceSlot< GUM_SCALAR >* >(overloader);
        auto ref_overloaded = static_cast< const PRMReferenceSlot< GUM_SCALAR >* >(overloaded);
        if (!ref_overloader->slotType().isSubTypeOf(ref_overloaded->slotType())) { return false; }
      } else {
        return false;
      }
      return true;
    }

    template < typename GUM_SCALAR >
    bool PRMInterface< GUM_SCALAR >::isSubTypeOf(
       const PRMClassElementContainer< GUM_SCALAR >& cec) const {
      switch (cec.obj_type()) {
        case PRMClassElement< GUM_SCALAR >::prm_type::CLASS: {
          return false;
        }

        case PRMClassElement< GUM_SCALAR >::prm_type::PRM_INTERFACE: {
          const PRMInterface* current = this;

          while (current != 0) {
            if (current == &(cec)) return true;

            current = current->_superInterface_;
          }

          return false;
        }

        default: {
          GUM_ERROR(FatalError, "unknown ClassElementContainer<GUM_SCALAR>")
        }
      }
    }

    template < typename GUM_SCALAR >
    void PRMInterface< GUM_SCALAR >::updateDescendants_(const PRMClassElement< GUM_SCALAR >& elt) {
      // for ( const auto ext :  _extensions_ )
      //  if ( !ext->isOutputNode( elt ) ) ext->setOutputNode( elt, true );

      // for ( const auto impl :  _implementations_ ) {
      //  // Because of cyclic dependencies we must use a reinterpret cast.
      //  PRMClassElementContainer<GUM_SCALAR>* c =
      //      reinterpret_cast<PRMClassElementContainer<GUM_SCALAR>*>( impl );

      //  if ( ! c->isOutputNode( elt ) ) c->setOutputNode( elt, true );
      //}
    }

    template < typename GUM_SCALAR >
    INLINE typename PRMInterface< GUM_SCALAR >::ClassEltIterator
       PRMInterface< GUM_SCALAR >::begin() {
      return _nodeIdMap_.begin();
    }

    template < typename GUM_SCALAR >
    INLINE const typename PRMInterface< GUM_SCALAR >::ClassEltIterator&
       PRMInterface< GUM_SCALAR >::end() {
      return _nodeIdMap_.end();
    }

    template < typename GUM_SCALAR >
    INLINE typename PRMInterface< GUM_SCALAR >::const_ClassEltIterator
       PRMInterface< GUM_SCALAR >::begin() const {
      return _nodeIdMap_.begin();
    }

    template < typename GUM_SCALAR >
    INLINE const typename PRMInterface< GUM_SCALAR >::const_ClassEltIterator&
       PRMInterface< GUM_SCALAR >::end() const {
      return _nodeIdMap_.end();
    }

    template < typename GUM_SCALAR >
    INLINE void PRMInterface< GUM_SCALAR >::addArc(const std::string& tail,
                                                   const std::string& head) {
      GUM_ERROR(OperationNotAllowed, "an Interface does ! have arcs")
    }

    template < typename GUM_SCALAR >
    INLINE PRMInterface< GUM_SCALAR >& PRMInterface< GUM_SCALAR >::super() {
      if (_superInterface_) return *_superInterface_;
      else GUM_ERROR(NotFound, "this Interface is ! a sub interface")
    }

    template < typename GUM_SCALAR >
    INLINE const PRMInterface< GUM_SCALAR >& PRMInterface< GUM_SCALAR >::super() const {
      if (_superInterface_) return *_superInterface_;
      else GUM_ERROR(NotFound, "this Interface is ! a sub interface")
    }

    template < typename GUM_SCALAR >
    INLINE void PRMInterface< GUM_SCALAR >::_addImplementation_(PRMClass< GUM_SCALAR >* c) {
      _implementations_.insert(c);
    }

    template < typename GUM_SCALAR >
    INLINE void PRMInterface< GUM_SCALAR >::_addExtension_(PRMInterface< GUM_SCALAR >* i) {
      _extensions_.insert(i);
    }

    template < typename GUM_SCALAR >
    INLINE PRMClassElement< GUM_SCALAR >& PRMInterface< GUM_SCALAR >::operator[](NodeId id) {
      return get(id);
    }

    template < typename GUM_SCALAR >
    INLINE const PRMClassElement< GUM_SCALAR >&
       PRMInterface< GUM_SCALAR >::operator[](NodeId id) const {
      return get(id);
    }

    template < typename GUM_SCALAR >
    INLINE PRMClassElement< GUM_SCALAR >&
       PRMInterface< GUM_SCALAR >::operator[](const std::string& name) {
      return get(name);
    }

    template < typename GUM_SCALAR >
    INLINE const PRMClassElement< GUM_SCALAR >&
       PRMInterface< GUM_SCALAR >::operator[](const std::string& name) const {
      return get(name);
    }

    template < typename GUM_SCALAR >
    INLINE typename PRMObject::prm_type PRMInterface< GUM_SCALAR >::obj_type() const {
      return PRMObject::prm_type::PRM_INTERFACE;
    }

    template < typename GUM_SCALAR >
    INLINE const DAG& PRMInterface< GUM_SCALAR >::dag_() const {
      return _dag_;
    }

    template < typename GUM_SCALAR >
    INLINE DAG& PRMInterface< GUM_SCALAR >::dag_() {
      return _dag_;
    }

    template < typename GUM_SCALAR >
    INLINE PRMClassElement< GUM_SCALAR >& PRMInterface< GUM_SCALAR >::get(NodeId id) {
      try {
        return *(_nodeIdMap_[id]);
      } catch (NotFound const&) {
        GUM_ERROR(NotFound, "no ClassElement<GUM_SCALAR> with the given NodeId")
      }
    }

    template < typename GUM_SCALAR >
    INLINE const PRMClassElement< GUM_SCALAR >& PRMInterface< GUM_SCALAR >::get(NodeId id) const {
      try {
        return *(_nodeIdMap_[id]);
      } catch (NotFound const&) {
        GUM_ERROR(NotFound, "no ClassElement<GUM_SCALAR> with the given NodeId")
      }
    }

    template < typename GUM_SCALAR >
    INLINE PRMClassElement< GUM_SCALAR >& PRMInterface< GUM_SCALAR >::get(const std::string& name) {
      try {
        return *(_nameMap_[name]);
      } catch (NotFound const&) {
        GUM_ERROR(NotFound, "no ClassElement<GUM_SCALAR> with the given name")
      }
    }

    template < typename GUM_SCALAR >
    INLINE const PRMClassElement< GUM_SCALAR >&
                 PRMInterface< GUM_SCALAR >::get(const std::string& name) const {
      try {
        return *(_nameMap_[name]);
      } catch (NotFound const&) {
        GUM_ERROR(NotFound, "no ClassElement<GUM_SCALAR> with the given name")
      }
    }

    template < typename GUM_SCALAR >
    INLINE const Set< PRMAttribute< GUM_SCALAR >* >&
                 PRMInterface< GUM_SCALAR >::attributes() const {
      return _attributes_;
    }

    template < typename GUM_SCALAR >
    INLINE const Set< PRMReferenceSlot< GUM_SCALAR >* >&
                 PRMInterface< GUM_SCALAR >::referenceSlots() const {
      return _referenceSlots_;
    }

    template < typename GUM_SCALAR >
    INLINE Set< PRMClass< GUM_SCALAR >* >& PRMInterface< GUM_SCALAR >::implementations() {
      return _implementations_;
    }

    template < typename GUM_SCALAR >
    INLINE const Set< PRMClass< GUM_SCALAR >* >&
                 PRMInterface< GUM_SCALAR >::implementations() const {
      return _implementations_;
    }

    template < typename GUM_SCALAR >
    void PRMInterface< GUM_SCALAR >::findAllSubtypes_(
       Set< PRMClassElementContainer< GUM_SCALAR >* >& set) {
      for (const auto impl: _implementations_) {
        set.insert(impl);
        impl->findAllSubtypes_(set);
      }

      for (const auto ext: _extensions_) {
        set.insert(ext);
        ext->findAllSubtypes_(set);
      }
    }

    template < typename GUM_SCALAR >
    INLINE bool
       PRMInterface< GUM_SCALAR >::isOutputNode(const PRMClassElement< GUM_SCALAR >& elt) const {
      try {
        if (!this->getIOFlag_(elt).second) {
          for (auto i: _implementations_) {
            if (i->isOutputNode(elt)) { return true; }
          }

          if (_superInterface_ && _superInterface_->isOutputNode(elt)) { return true; }

        } else {
          return true;
        }
      } catch (NotFound const&) {}
      return false;
    }
  } /* namespace prm */
} /* namespace gum */
