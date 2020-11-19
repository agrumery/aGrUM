/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(@LIP6)
 */
#include <agrum/PRM/elements/PRMInterface.h>

#include <agrum/PRM/elements/PRMClass.h>

namespace gum {
  namespace prm {

    template < typename GUM_SCALAR >
    PRMInterface< GUM_SCALAR >::PRMInterface(const std::string& name) :
        PRMClassElementContainer< GUM_SCALAR >(name), superInterface__(0) {
      GUM_CONSTRUCTOR(PRMInterface);
    }

    template < typename GUM_SCALAR >
    PRMInterface< GUM_SCALAR >::PRMInterface(const std::string&          name,
                                             PRMInterface< GUM_SCALAR >& super,
                                             bool delayInheritance) :
        PRMClassElementContainer< GUM_SCALAR >(name),
        superInterface__(&super) {
      GUM_CONSTRUCTOR(PRMInterface);
      if (!delayInheritance) { inheritInterface__(super); }
    }

    template < typename GUM_SCALAR >
    PRMInterface< GUM_SCALAR >::PRMInterface(
       const PRMInterface< GUM_SCALAR >& source) :
        PRMClassElementContainer< GUM_SCALAR >(source.name()),
        dag__(source.dag__), superInterface__(source.superInterface__) {
      GUM_CONS_CPY(PRMInterface);
      GUM_ERROR(FatalError, "don't copy an interface");
    }

    template < typename GUM_SCALAR >
    PRMInterface< GUM_SCALAR >::~PRMInterface() {
      GUM_DESTRUCTOR(PRMInterface);

      for (const auto& elt: nodeIdMap__) {
        delete elt.second;
      }
    }

    template < typename GUM_SCALAR >
    void PRMInterface< GUM_SCALAR >::inheritInterface() {
      if (superInterface__ != nullptr) { inheritInterface__(*superInterface__); }
    }

    template < typename GUM_SCALAR >
    void PRMInterface< GUM_SCALAR >::inheritInterface__(
       const PRMInterface< GUM_SCALAR >& i) {
      // Copying attributes
      for (const auto i_attr: i.attributes__) {
        auto attr =
           new PRMScalarAttribute< GUM_SCALAR >(i_attr->name(), i_attr->type());
        attr->setId(i_attr->id());
        nodeIdMap__.insert(attr->id(), attr);
        attributes__.insert(attr);

        if (i.nameMap__[i_attr->name()] == i.nameMap__[i_attr->safeName()]) {
          nameMap__.insert(attr->name(), attr);
        }

        nameMap__.insert(attr->safeName(), attr);
        dag__.addNodeWithId(attr->id());
      }

      // Copying reference slots
      for (const auto i_ref: i.referenceSlots__) {
        auto ref = new PRMReferenceSlot< GUM_SCALAR >(
           i_ref->name(),
           const_cast< PRMClassElementContainer< GUM_SCALAR >& >(
              i_ref->slotType()),
           i_ref->isArray());

        ref->setId(i_ref->id());
        nodeIdMap__.insert(ref->id(), ref);
        referenceSlots__.insert(ref);

        if (i.nameMap__.exists(ref->name())) {
          nameMap__.insert(ref->name(), ref);
        }

        nameMap__.insert(ref->safeName(), ref);
        dag__.addNodeWithId(ref->id());
      }
    }

    template < typename GUM_SCALAR >
    NodeId PRMInterface< GUM_SCALAR >::add(PRMClassElement< GUM_SCALAR >* elt) {
      if (nameMap__.exists(elt->name())) {
        GUM_ERROR(DuplicateElement,
                  "name '" << elt->name()
                           << "' is already used by another ClassElement");
      }

      if (PRMClassElement< GUM_SCALAR >::isAttribute(*elt)) {
        PRMAttribute< GUM_SCALAR >* attr =
           static_cast< PRMAttribute< GUM_SCALAR >* >(elt);
        nameMap__.insert(attr->name(), attr);

        while (true) {
          attr->setId(nextNodeId());
          dag__.addNodeWithId(attr->id());
          nodeIdMap__.insert(attr->id(), attr);
          nameMap__.insert(attr->safeName(), attr);
          attributes__.insert(attr);

          if (attr->type().isSubType()) {
            attr = attr->getCastDescendant();
          } else {
            break;
          }
        }
      } else if (PRMClassElement< GUM_SCALAR >::isReferenceSlot(*elt)) {
        elt->setId(nextNodeId());
        dag__.addNodeWithId(elt->id());
        nodeIdMap__.insert(elt->id(), elt);
        referenceSlots__.insert(
           static_cast< PRMReferenceSlot< GUM_SCALAR >* >(elt));
        nameMap__.insert(elt->name(), elt);
        nameMap__.insert(elt->safeName(), elt);
      } else {
        GUM_ERROR(WrongClassElement,
                  "illegal ClassElement<GUM_SCALAR> for an Interface");
      }

      return elt->id();
    }

    template < typename GUM_SCALAR >
    NodeId PRMInterface< GUM_SCALAR >::overload(
       PRMClassElement< GUM_SCALAR >* overloader) {
      try {
        if (!super().exists(overloader->name())) {
          GUM_ERROR(OperationNotAllowed,
                    "found no ClassElement<GUM_SCALAR> to overload");
        }
      } catch (NotFound&) {
        GUM_ERROR(OperationNotAllowed,
                  "overload is possible only with sub interfaces");
      }

      PRMClassElement< GUM_SCALAR >* overloaded = nameMap__[overloader->name()];
      if (overloaded == overloader) {
        GUM_ERROR(DuplicateElement,
                  "duplicate ClassElement '" << overloader->name() << "'");
      }
      if (!checkOverloadLegality__(overloaded, overloader)) {
        GUM_ERROR(OperationNotAllowed, "illegal overload");
      }

      switch (overloader->elt_type()) {
        case PRMClassElement< GUM_SCALAR >::prm_attribute: {
          auto attr_overloader =
             static_cast< PRMAttribute< GUM_SCALAR >* >(overloader);
          auto attr_overloaded =
             static_cast< PRMAttribute< GUM_SCALAR >* >(overloaded);
          overloadAttribute__(attr_overloader, attr_overloaded);
          break;
        }

        case PRMClassElement< GUM_SCALAR >::prm_refslot: {
          auto ref_overloader =
             static_cast< PRMReferenceSlot< GUM_SCALAR >* >(overloader);
          auto ref_overloaded =
             static_cast< PRMReferenceSlot< GUM_SCALAR >* >(overloaded);
          overloadReferenceSlot__(ref_overloader, ref_overloaded);
          break;
        }

        case PRMClassElement< GUM_SCALAR >::prm_aggregate:
        case PRMClassElement< GUM_SCALAR >::prm_slotchain: {
          auto msg = "element can ! be overloaded";
          GUM_ERROR(OperationNotAllowed, msg);
          break;
        }

        default: {
          GUM_ERROR(FatalError, "unknown ClassElement<GUM_SCALAR> type");
        }
      }

      return overloader->id();
    }

    template < typename GUM_SCALAR >
    void PRMInterface< GUM_SCALAR >::overloadAttribute__(
       PRMAttribute< GUM_SCALAR >* overloader,
       PRMAttribute< GUM_SCALAR >* overloaded) {
      if (overloader->type() != overloaded->type()) {
        overloader->setId(nextNodeId());
        dag__.addNodeWithId(overloader->id());
        nodeIdMap__.insert(overloader->id(), overloader);
        nameMap__[overloader->name()] = overloader;
        nameMap__.insert(overloader->safeName(), overloader);
        attributes__.insert(overloader);
        addCastDescendants__(overloader, overloaded);
      } else {
        overloader->setId(overloaded->id());
        nodeIdMap__[overloader->id()] = overloader;
        nameMap__[overloader->name()] = overloader;
        nameMap__[overloader->safeName()] = overloader;
        attributes__.erase(overloaded);
        attributes__.insert(overloader);
        // Swapping types, ugly but necessary to preserve the
        // PRMType<GUM_SCALAR>
        // pointer of overloaded
        overloader->overload(overloaded);
        delete overloaded;
      }
    }

    template < typename GUM_SCALAR >
    void PRMInterface< GUM_SCALAR >::overloadReferenceSlot__(
       PRMReferenceSlot< GUM_SCALAR >* overloader,
       PRMReferenceSlot< GUM_SCALAR >* overloaded) {
      // Adding overloading reference
      overloader->setId(overloaded->id());
      nodeIdMap__[overloader->id()] = overloader;
      nameMap__[overloader->name()] = overloader;
      nameMap__.insert(overloader->safeName(), overloader);
      referenceSlots__.insert(overloader);
      // Removing overloaded PRMReferenceSlot<GUM_SCALAR>
      referenceSlots__.erase(overloaded);
      nameMap__.erase(overloaded->safeName());
      delete overloaded;
    }

    template < typename GUM_SCALAR >
    void PRMInterface< GUM_SCALAR >::addCastDescendants__(
       PRMAttribute< GUM_SCALAR >* start, PRMAttribute< GUM_SCALAR >* end) {
      PRMAttribute< GUM_SCALAR >* parent = start;
      PRMAttribute< GUM_SCALAR >* child = nullptr;

      while (parent->type().superType() != end->type()) {
        child = parent->getCastDescendant();
        child->setId(nextNodeId());
        dag__.addNodeWithId(child->id());
        nodeIdMap__.insert(child->id(), child);
        // Only use child's safe name when adding to the name map!
        nameMap__.insert(child->safeName(), child);
        attributes__.insert(child);
        // Do ! use Class<GUM_SCALAR>::insertArc(), child's CPF is already
        // initialized properly
        parent = child;
      }

      parent->setAsCastDescendant(end);
    }

    template < typename GUM_SCALAR >
    bool PRMInterface< GUM_SCALAR >::checkOverloadLegality__(
       const PRMClassElement< GUM_SCALAR >* overloaded,
       const PRMClassElement< GUM_SCALAR >* overloader) {
      if (overloaded->elt_type() != overloader->elt_type()) { return false; }

      if (overloaded->elt_type() == PRMClassElement< GUM_SCALAR >::prm_attribute) {
        if (!overloader->type().isSubTypeOf(overloaded->type())) { return false; }
      } else if (overloaded->elt_type()
                 == PRMClassElement< GUM_SCALAR >::prm_refslot) {
        auto ref_overloader =
           static_cast< const PRMReferenceSlot< GUM_SCALAR >* >(overloader);
        auto ref_overloaded =
           static_cast< const PRMReferenceSlot< GUM_SCALAR >* >(overloaded);
        if (!ref_overloader->slotType().isSubTypeOf(ref_overloaded->slotType())) {
          return false;
        }
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

            current = current->superInterface__;
          }

          return false;
        }

        default: {
          GUM_ERROR(FatalError, "unknown ClassElementContainer<GUM_SCALAR>");
        }
      }
    }

    template < typename GUM_SCALAR >
    void PRMInterface< GUM_SCALAR >::updateDescendants_(
       const PRMClassElement< GUM_SCALAR >& elt) {
      // for ( const auto ext : extensions__ )
      //  if ( !ext->isOutputNode( elt ) ) ext->setOutputNode( elt, true );

      // for ( const auto impl : implementations__ ) {
      //  // Because of cyclic dependencies we must use a reinterpret cast.
      //  PRMClassElementContainer<GUM_SCALAR>* c =
      //      reinterpret_cast<PRMClassElementContainer<GUM_SCALAR>*>( impl );

      //  if ( ! c->isOutputNode( elt ) ) c->setOutputNode( elt, true );
      //}
    }

    template < typename GUM_SCALAR >
    INLINE typename PRMInterface< GUM_SCALAR >::ClassEltIterator
       PRMInterface< GUM_SCALAR >::begin() {
      return nodeIdMap__.begin();
    }

    template < typename GUM_SCALAR >
    INLINE const typename PRMInterface< GUM_SCALAR >::ClassEltIterator&
       PRMInterface< GUM_SCALAR >::end() {
      return nodeIdMap__.end();
    }

    template < typename GUM_SCALAR >
    INLINE typename PRMInterface< GUM_SCALAR >::const_ClassEltIterator
       PRMInterface< GUM_SCALAR >::begin() const {
      return nodeIdMap__.begin();
    }

    template < typename GUM_SCALAR >
    INLINE const typename PRMInterface< GUM_SCALAR >::const_ClassEltIterator&
       PRMInterface< GUM_SCALAR >::end() const {
      return nodeIdMap__.end();
    }

    template < typename GUM_SCALAR >
    INLINE void PRMInterface< GUM_SCALAR >::addArc(const std::string& tail,
                                                   const std::string& head) {
      GUM_ERROR(OperationNotAllowed, "an Interface does ! have arcs");
    }

    template < typename GUM_SCALAR >
    INLINE PRMInterface< GUM_SCALAR >& PRMInterface< GUM_SCALAR >::super() {
      if (superInterface__)
        return *superInterface__;
      else
        GUM_ERROR(NotFound, "this Interface is ! a sub interface");
    }

    template < typename GUM_SCALAR >
    INLINE const PRMInterface< GUM_SCALAR >&
                 PRMInterface< GUM_SCALAR >::super() const {
      if (superInterface__)
        return *superInterface__;
      else
        GUM_ERROR(NotFound, "this Interface is ! a sub interface");
    }

    template < typename GUM_SCALAR >
    INLINE void
       PRMInterface< GUM_SCALAR >::addImplementation__(PRMClass< GUM_SCALAR >* c) {
      implementations__.insert(c);
    }

    template < typename GUM_SCALAR >
    INLINE void
       PRMInterface< GUM_SCALAR >::addExtension__(PRMInterface< GUM_SCALAR >* i) {
      extensions__.insert(i);
    }

    template < typename GUM_SCALAR >
    INLINE PRMClassElement< GUM_SCALAR >&
       PRMInterface< GUM_SCALAR >::operator[](NodeId id) {
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
    INLINE typename PRMObject::prm_type
       PRMInterface< GUM_SCALAR >::obj_type() const {
      return PRMObject::prm_type::PRM_INTERFACE;
    }

    template < typename GUM_SCALAR >
    INLINE const DAG& PRMInterface< GUM_SCALAR >::dag_() const {
      return dag__;
    }

    template < typename GUM_SCALAR >
    INLINE DAG& PRMInterface< GUM_SCALAR >::dag_() {
      return dag__;
    }

    template < typename GUM_SCALAR >
    INLINE PRMClassElement< GUM_SCALAR >&
           PRMInterface< GUM_SCALAR >::get(NodeId id) {
      try {
        return *(nodeIdMap__[id]);
      } catch (NotFound&) {
        GUM_ERROR(NotFound, "no ClassElement<GUM_SCALAR> with the given NodeId");
      }
    }

    template < typename GUM_SCALAR >
    INLINE const PRMClassElement< GUM_SCALAR >&
                 PRMInterface< GUM_SCALAR >::get(NodeId id) const {
      try {
        return *(nodeIdMap__[id]);
      } catch (NotFound&) {
        GUM_ERROR(NotFound, "no ClassElement<GUM_SCALAR> with the given NodeId");
      }
    }

    template < typename GUM_SCALAR >
    INLINE PRMClassElement< GUM_SCALAR >&
           PRMInterface< GUM_SCALAR >::get(const std::string& name) {
      try {
        return *(nameMap__[name]);
      } catch (NotFound&) {
        GUM_ERROR(NotFound, "no ClassElement<GUM_SCALAR> with the given name");
      }
    }

    template < typename GUM_SCALAR >
    INLINE const PRMClassElement< GUM_SCALAR >&
                 PRMInterface< GUM_SCALAR >::get(const std::string& name) const {
      try {
        return *(nameMap__[name]);
      } catch (NotFound&) {
        GUM_ERROR(NotFound, "no ClassElement<GUM_SCALAR> with the given name");
      }
    }

    template < typename GUM_SCALAR >
    INLINE const Set< PRMAttribute< GUM_SCALAR >* >&
                 PRMInterface< GUM_SCALAR >::attributes() const {
      return attributes__;
    }

    template < typename GUM_SCALAR >
    INLINE const Set< PRMReferenceSlot< GUM_SCALAR >* >&
                 PRMInterface< GUM_SCALAR >::referenceSlots() const {
      return referenceSlots__;
    }

    template < typename GUM_SCALAR >
    INLINE Set< PRMClass< GUM_SCALAR >* >&
           PRMInterface< GUM_SCALAR >::implementations() {
      return implementations__;
    }

    template < typename GUM_SCALAR >
    INLINE const Set< PRMClass< GUM_SCALAR >* >&
                 PRMInterface< GUM_SCALAR >::implementations() const {
      return implementations__;
    }

    template < typename GUM_SCALAR >
    void PRMInterface< GUM_SCALAR >::findAllSubtypes_(
       Set< PRMClassElementContainer< GUM_SCALAR >* >& set) {
      for (const auto impl: implementations__) {
        set.insert(impl);
        impl->findAllSubtypes_(set);
      }

      for (const auto ext: extensions__) {
        set.insert(ext);
        ext->findAllSubtypes_(set);
      }
    }

    template < typename GUM_SCALAR >
    INLINE bool PRMInterface< GUM_SCALAR >::isOutputNode(
       const PRMClassElement< GUM_SCALAR >& elt) const {
      try {
        if (!this->getIOFlag_(elt).second) {
          for (auto i: implementations__) {
            if (i->isOutputNode(elt)) { return true; }
          }

          if (superInterface__ && superInterface__->isOutputNode(elt)) {
            return true;
          }

        } else {
          return true;
        }
      } catch (NotFound&) {}
      return false;
    }
  } /* namespace prm */
} /* namespace gum */
