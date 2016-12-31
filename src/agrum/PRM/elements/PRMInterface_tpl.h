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
 * @brief Inline implementation of gum::prm::PRMInterface
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */
#include <agrum/PRM/elements/PRMInterface.h>

#include <agrum/PRM/elements/PRMAttribute.h>
#include <agrum/PRM/elements/PRMClass.h>
#include <agrum/PRM/elements/PRMReferenceSlot.h>
#include <agrum/PRM/elements/PRMScalarAttribute.h>

namespace gum {
  namespace prm {

    template <typename GUM_SCALAR>
    PRMInterface<GUM_SCALAR>::PRMInterface( const std::string& name )
        : PRMClassElementContainer<GUM_SCALAR>( name )
        , __superInterface( 0 ) {
      GUM_CONSTRUCTOR( PRMInterface );
    }

    template <typename GUM_SCALAR>
    PRMInterface<GUM_SCALAR>::PRMInterface( const std::string&        name,
                                            PRMInterface<GUM_SCALAR>& super,
                                            bool delayInheritance )
        : PRMClassElementContainer<GUM_SCALAR>( name )
        , __superInterface( &super ) {
      GUM_CONSTRUCTOR( PRMInterface );
      if ( !delayInheritance ) {
        __inheritInterface( super );
      }
    }

    template <typename GUM_SCALAR>
    PRMInterface<GUM_SCALAR>::PRMInterface(
        const PRMInterface<GUM_SCALAR>& source )
        : PRMClassElementContainer<GUM_SCALAR>( source.name() )
        , __dag( source.__dag )
        , __superInterface( source.__superInterface ) {
      GUM_CONS_CPY( PRMInterface );
      GUM_ERROR( FatalError, "don't copy an interface" );
    }

    template <typename GUM_SCALAR>
    PRMInterface<GUM_SCALAR>::~PRMInterface() {
      GUM_DESTRUCTOR( PRMInterface );

      for ( const auto& elt : __nodeIdMap ) {
        delete elt.second;
      }
    }

    template <typename GUM_SCALAR>
    void PRMInterface<GUM_SCALAR>::inheritInterface() {
      if ( __superInterface != nullptr ) {
        __inheritInterface( *__superInterface );
      }
    }

    template <typename GUM_SCALAR>
    void PRMInterface<GUM_SCALAR>::__inheritInterface(
        const PRMInterface<GUM_SCALAR>& i ) {
      // Copying attributes
      for ( const auto i_attr : i.__attributes ) {
        auto attr =
            new PRMScalarAttribute<GUM_SCALAR>( i_attr->name(), i_attr->type() );
        attr->setId( i_attr->id() );
        __nodeIdMap.insert( attr->id(), attr );
        __attributes.insert( attr );

        if ( i.__nameMap[i_attr->name()] == i.__nameMap[i_attr->safeName()] ) {
          __nameMap.insert( attr->name(), attr );
        }

        __nameMap.insert( attr->safeName(), attr );
        __dag.addNode( attr->id() );
      }

      // Copying reference slots
      for ( const auto i_ref : i.__referenceSlots ) {
        auto ref = new PRMReferenceSlot<GUM_SCALAR>(
            i_ref->name(),
            const_cast<PRMClassElementContainer<GUM_SCALAR>&>( i_ref->slotType() ),
            i_ref->isArray() );

        ref->setId( i_ref->id() );
        __nodeIdMap.insert( ref->id(), ref );
        __referenceSlots.insert( ref );

        if ( i.__nameMap.exists( ref->name() ) ) {
          __nameMap.insert( ref->name(), ref );
        }

        __nameMap.insert( ref->safeName(), ref );
        __dag.addNode( ref->id() );
      }
    }

    template <typename GUM_SCALAR>
    NodeId PRMInterface<GUM_SCALAR>::add( PRMClassElement<GUM_SCALAR>* elt ) {
      if ( __nameMap.exists( elt->name() ) ) {
        GUM_ERROR( DuplicateElement,
                   "name already used by another ClassElement<GUM_SCALAR>" );
      }

      if ( PRMClassElement<GUM_SCALAR>::isAttribute( *elt ) ) {
        PRMAttribute<GUM_SCALAR>* attr =
            static_cast<PRMAttribute<GUM_SCALAR>*>( elt );
        __nameMap.insert( attr->name(), attr );

        while ( true ) {
          attr->setId( nextNodeId() );
          __dag.addNode( attr->id() );
          __nodeIdMap.insert( attr->id(), attr );
          __nameMap.insert( attr->safeName(), attr );
          __attributes.insert( attr );

          if ( attr->type().isSubType() ) {
            attr = attr->getCastDescendant();
          } else {
            break;
          }
        }
      } else if ( PRMClassElement<GUM_SCALAR>::isReferenceSlot( *elt ) ) {
        elt->setId( nextNodeId() );
        __dag.addNode( elt->id() );
        __nodeIdMap.insert( elt->id(), elt );
        __referenceSlots.insert(
            static_cast<PRMReferenceSlot<GUM_SCALAR>*>( elt ) );
        __nameMap.insert( elt->name(), elt );
        __nameMap.insert( elt->safeName(), elt );
      } else {
        GUM_ERROR( WrongClassElement,
                   "illegal ClassElement<GUM_SCALAR> for an Interface" );
      }

      return elt->id();
    }

    template <typename GUM_SCALAR>
    NodeId
    PRMInterface<GUM_SCALAR>::overload( PRMClassElement<GUM_SCALAR>* overloader ) {
      try {
        if ( !super().exists( overloader->name() ) ) {
          GUM_ERROR( OperationNotAllowed,
                     "found no ClassElement<GUM_SCALAR> to overload" );
        }
      } catch ( NotFound& ) {
        GUM_ERROR( OperationNotAllowed,
                   "overload is possible only with sub interfaces" );
      }

      PRMClassElement<GUM_SCALAR>* overloaded = __nameMap[overloader->name()];
      if ( overloaded == overloader ) {
        GUM_ERROR( DuplicateElement, "dupplicate ClassElement" );
      }
      if ( !__checkOverloadLegality( overloaded, overloader ) ) {
        GUM_ERROR( OperationNotAllowed, "illegal overload" );
      }

      switch ( overloader->elt_type() ) {
        case PRMClassElement<GUM_SCALAR>::prm_attribute: {
          auto attr_overloader =
              static_cast<PRMAttribute<GUM_SCALAR>*>( overloader );
          auto attr_overloaded =
              static_cast<PRMAttribute<GUM_SCALAR>*>( overloaded );
          __overloadAttribute( attr_overloader, attr_overloaded );
          break;
        }

        case PRMClassElement<GUM_SCALAR>::prm_refslot: {
          auto ref_overloader =
              static_cast<PRMReferenceSlot<GUM_SCALAR>*>( overloader );
          auto ref_overloaded =
              static_cast<PRMReferenceSlot<GUM_SCALAR>*>( overloaded );
          __overloadReferenceSlot( ref_overloader, ref_overloaded );
          break;
        }

        case PRMClassElement<GUM_SCALAR>::prm_aggregate:
        case PRMClassElement<GUM_SCALAR>::prm_slotchain: {
          auto msg = "element can ! be overloaded";
          GUM_ERROR( OperationNotAllowed, msg );
          break;
        }

        default: {
          GUM_ERROR( FatalError, "unknown ClassElement<GUM_SCALAR> type" );
        }
      }

      return overloader->id();
    }

    template <typename GUM_SCALAR>
    void PRMInterface<GUM_SCALAR>::__overloadAttribute(
        PRMAttribute<GUM_SCALAR>* overloader,
        PRMAttribute<GUM_SCALAR>* overloaded ) {
      if ( overloader->type() != overloaded->type() ) {
        overloader->setId( nextNodeId() );
        __dag.addNode( overloader->id() );
        __nodeIdMap.insert( overloader->id(), overloader );
        __nameMap[overloader->name()] = overloader;
        __nameMap.insert( overloader->safeName(), overloader );
        __attributes.insert( overloader );
        __addCastDescendants( overloader, overloaded );
      } else {
        overloader->setId( overloaded->id() );
        __nodeIdMap[overloader->id()] = overloader;
        __nameMap[overloader->name()] = overloader;
        __nameMap[overloader->safeName()] = overloader;
        __attributes.erase( overloaded );
        __attributes.insert( overloader );
        // Swapping types, ugly but necessary to preserve the
        // PRMType<GUM_SCALAR>
        // pointer of overloaded
        overloader->overload( overloaded );
        delete overloaded;
      }
    }

    template <typename GUM_SCALAR>
    void PRMInterface<GUM_SCALAR>::__overloadReferenceSlot(
        PRMReferenceSlot<GUM_SCALAR>* overloader,
        PRMReferenceSlot<GUM_SCALAR>* overloaded ) {
      // Adding overloading reference
      overloader->setId( overloaded->id() );
      __nodeIdMap[overloader->id()] = overloader;
      __nameMap[overloader->name()] = overloader;
      __nameMap.insert( overloader->safeName(), overloader );
      __referenceSlots.insert( overloader );
      // Removing overloaded PRMReferenceSlot<GUM_SCALAR>
      __referenceSlots.erase( overloaded );
      __nameMap.erase( overloaded->safeName() );
      delete overloaded;
    }

    template <typename GUM_SCALAR>
    void PRMInterface<GUM_SCALAR>::__addCastDescendants(
        PRMAttribute<GUM_SCALAR>* start, PRMAttribute<GUM_SCALAR>* end ) {
      PRMAttribute<GUM_SCALAR>* parent = start;
      PRMAttribute<GUM_SCALAR>* child = 0;

      while ( parent->type().superType() != end->type() ) {
        child = parent->getCastDescendant();
        child->setId( nextNodeId() );
        __dag.addNode( child->id() );
        __nodeIdMap.insert( child->id(), child );
        // Only use child's safe name when adding to the name map!
        __nameMap.insert( child->safeName(), child );
        __attributes.insert( child );
        // Do ! use Class<GUM_SCALAR>::insertArc(), child's CPF is already
        // initialized properly
        parent = child;
      }

      parent->setAsCastDescendant( end );
    }

    template <typename GUM_SCALAR>
    bool PRMInterface<GUM_SCALAR>::__checkOverloadLegality(
        const PRMClassElement<GUM_SCALAR>* overloaded,
        const PRMClassElement<GUM_SCALAR>* overloader ) {
      if ( overloaded->elt_type() != overloader->elt_type() ) {
        return false;
      }

      if ( overloaded->elt_type() == PRMClassElement<GUM_SCALAR>::prm_attribute ) {
        if ( !overloader->type().isSubTypeOf( overloaded->type() ) ) {
          return false;
        }
      } else if ( overloaded->elt_type() ==
                  PRMClassElement<GUM_SCALAR>::prm_refslot ) {
        auto ref_overloader =
            static_cast<const PRMReferenceSlot<GUM_SCALAR>*>( overloader );
        auto ref_overloaded =
            static_cast<const PRMReferenceSlot<GUM_SCALAR>*>( overloaded );
        if ( !ref_overloader->slotType().isSubTypeOf(
                 ref_overloaded->slotType() ) ) {
          return false;
        }
      } else {
        return false;
      }
      return true;
    }

    template <typename GUM_SCALAR>
    bool PRMInterface<GUM_SCALAR>::isSubTypeOf(
        const PRMClassElementContainer<GUM_SCALAR>& cec ) const {
      switch ( cec.obj_type() ) {
        case PRMClassElement<GUM_SCALAR>::prm_type::CLASS: {
          return false;
        }

        case PRMClassElement<GUM_SCALAR>::prm_type::PRM_INTERFACE: {
          const PRMInterface* current = this;

          while ( current != 0 ) {
            if ( current == &( cec ) ) return true;

            current = current->__superInterface;
          }

          return false;
        }

        default: {
          GUM_ERROR( FatalError, "unknown ClassElementContainer<GUM_SCALAR>" );
        }
      }
    }

    template <typename GUM_SCALAR>
    void PRMInterface<GUM_SCALAR>::_updateDescendants(
        const PRMClassElement<GUM_SCALAR>& elt ) {
      // for ( const auto ext : __extensions )
      //  if ( !ext->isOutputNode( elt ) ) ext->setOutputNode( elt, true );

      // for ( const auto impl : __implementations ) {
      //  // Because of cyclic dependencies we must use a reinterpret cast.
      //  PRMClassElementContainer<GUM_SCALAR>* c =
      //      reinterpret_cast<PRMClassElementContainer<GUM_SCALAR>*>( impl );

      //  if ( ! c->isOutputNode( elt ) ) c->setOutputNode( elt, true );
      //}
    }

    template <typename GUM_SCALAR>
    INLINE typename PRMInterface<GUM_SCALAR>::ClassEltIterator
    PRMInterface<GUM_SCALAR>::begin() {
      return __nodeIdMap.begin();
    }

    template <typename GUM_SCALAR>
    INLINE const typename PRMInterface<GUM_SCALAR>::ClassEltIterator&
    PRMInterface<GUM_SCALAR>::end() {
      return __nodeIdMap.end();
    }

    template <typename GUM_SCALAR>
    INLINE typename PRMInterface<GUM_SCALAR>::const_ClassEltIterator
    PRMInterface<GUM_SCALAR>::begin() const {
      return __nodeIdMap.begin();
    }

    template <typename GUM_SCALAR>
    INLINE const typename PRMInterface<GUM_SCALAR>::const_ClassEltIterator&
    PRMInterface<GUM_SCALAR>::end() const {
      return __nodeIdMap.end();
    }

    template <typename GUM_SCALAR>
    INLINE void PRMInterface<GUM_SCALAR>::addArc( const std::string& tail,
                                                  const std::string& head ) {
      GUM_ERROR( OperationNotAllowed, "an Interface does ! have arcs" );
    }

    template <typename GUM_SCALAR>
    INLINE PRMInterface<GUM_SCALAR>& PRMInterface<GUM_SCALAR>::super() {
      if ( __superInterface )
        return *__superInterface;
      else
        GUM_ERROR( NotFound, "this Interface is ! a sub interface" );
    }

    template <typename GUM_SCALAR>
    INLINE const PRMInterface<GUM_SCALAR>&
                 PRMInterface<GUM_SCALAR>::super() const {
      if ( __superInterface )
        return *__superInterface;
      else
        GUM_ERROR( NotFound, "this Interface is ! a sub interface" );
    }

    template <typename GUM_SCALAR>
    INLINE void
    PRMInterface<GUM_SCALAR>::__addImplementation( PRMClass<GUM_SCALAR>* c ) {
      __implementations.insert( c );
    }

    template <typename GUM_SCALAR>
    INLINE void PRMInterface<GUM_SCALAR>::__addExtension( PRMInterface* i ) {
      __extensions.insert( i );
    }

    template <typename GUM_SCALAR>
    INLINE PRMClassElement<GUM_SCALAR>& PRMInterface<GUM_SCALAR>::
    operator[]( NodeId id ) {
      return get( id );
    }

    template <typename GUM_SCALAR>
    INLINE const PRMClassElement<GUM_SCALAR>& PRMInterface<GUM_SCALAR>::
    operator[]( NodeId id ) const {
      return get( id );
    }

    template <typename GUM_SCALAR>
    INLINE PRMClassElement<GUM_SCALAR>& PRMInterface<GUM_SCALAR>::
    operator[]( const std::string& name ) {
      return get( name );
    }

    template <typename GUM_SCALAR>
    INLINE const PRMClassElement<GUM_SCALAR>& PRMInterface<GUM_SCALAR>::
    operator[]( const std::string& name ) const {
      return get( name );
    }

    template <typename GUM_SCALAR>
    INLINE typename PRMObject::prm_type
    PRMInterface<GUM_SCALAR>::obj_type() const {
      return PRMObject::prm_type::PRM_INTERFACE;
    }

    template <typename GUM_SCALAR>
    INLINE const DAG& PRMInterface<GUM_SCALAR>::_dag() const {
      return __dag;
    }

    template <typename GUM_SCALAR>
    INLINE DAG& PRMInterface<GUM_SCALAR>::_dag() {
      return __dag;
    }

    template <typename GUM_SCALAR>
    INLINE PRMClassElement<GUM_SCALAR>&
    PRMInterface<GUM_SCALAR>::get( NodeId id ) {
      try {
        return *( __nodeIdMap[id] );
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound, "no ClassElement<GUM_SCALAR> with the given NodeId" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE const PRMClassElement<GUM_SCALAR>&
    PRMInterface<GUM_SCALAR>::get( NodeId id ) const {
      try {
        return *( __nodeIdMap[id] );
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound, "no ClassElement<GUM_SCALAR> with the given NodeId" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE PRMClassElement<GUM_SCALAR>&
    PRMInterface<GUM_SCALAR>::get( const std::string& name ) {
      try {
        return *( __nameMap[name] );
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound, "no ClassElement<GUM_SCALAR> with the given name" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE const PRMClassElement<GUM_SCALAR>&
    PRMInterface<GUM_SCALAR>::get( const std::string& name ) const {
      try {
        return *( __nameMap[name] );
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound, "no ClassElement<GUM_SCALAR> with the given name" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE const Set<PRMAttribute<GUM_SCALAR>*>&
                 PRMInterface<GUM_SCALAR>::attributes() const {
      return __attributes;
    }

    template <typename GUM_SCALAR>
    INLINE const Set<PRMReferenceSlot<GUM_SCALAR>*>&
                 PRMInterface<GUM_SCALAR>::referenceSlots() const {
      return __referenceSlots;
    }

    template <typename GUM_SCALAR>
    INLINE Set<PRMClass<GUM_SCALAR>*>&
           PRMInterface<GUM_SCALAR>::implementations() {
      return __implementations;
    }

    template <typename GUM_SCALAR>
    INLINE const Set<PRMClass<GUM_SCALAR>*>&
                 PRMInterface<GUM_SCALAR>::implementations() const {
      return __implementations;
    }

    template <typename GUM_SCALAR>
    void PRMInterface<GUM_SCALAR>::_findAllSubtypes(
        Set<PRMClassElementContainer<GUM_SCALAR>*>& set ) {
      for ( const auto impl : __implementations ) {
        set.insert( impl );
        impl->_findAllSubtypes( set );
      }

      for ( const auto ext : __extensions ) {
        set.insert( ext );
        ext->_findAllSubtypes( set );
      }
    }

    template <typename GUM_SCALAR>
    INLINE bool PRMInterface<GUM_SCALAR>::isOutputNode(
        const PRMClassElement<GUM_SCALAR>& elt ) const {
      try {
        if ( !this->_getIOFlag( elt ).second ) {

          for ( auto i : __implementations ) {
            if ( i->isOutputNode( elt ) ) {
              return true;
            }
          }

          if ( __superInterface && __superInterface->isOutputNode( elt ) ) {
            return true;
          }

        } else {
          return true;
        }
      } catch ( NotFound& ) {
      }
      return false;
    }
  } /* namespace prm */
} /* namespace gum */
