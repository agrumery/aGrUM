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
 * @brief Inline implementation of gum::prm::Interface
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */
#include <agrum/PRM/elements/interface.h>

#include <agrum/PRM/elements/attribute.h>
#include <agrum/PRM/elements/scalarAttribute.h>
#include <agrum/PRM/elements/referenceSlot.h>
#include <agrum/PRM/elements/class.h>

namespace gum {
  namespace prm {

    template <typename GUM_SCALAR>
    Interface<GUM_SCALAR>::Interface( const std::string& name )
        : ClassElementContainer<GUM_SCALAR>( name )
        , __super( 0 ) {
      GUM_CONSTRUCTOR( Interface );
    }

    template <typename GUM_SCALAR>
    Interface<GUM_SCALAR>::Interface( const std::string& name,
                                      Interface<GUM_SCALAR>& super,
                                      bool delayInheritance )
        : ClassElementContainer<GUM_SCALAR>( name )
        , __super( &super ) {
      GUM_CONSTRUCTOR( Interface );
      if ( not delayInheritance ) {
        __inheritInterface( super );
      }
    }

    template <typename GUM_SCALAR>
    Interface<GUM_SCALAR>::Interface( const Interface<GUM_SCALAR>& source )
        : ClassElementContainer<GUM_SCALAR>( source.name() )
        , __dag( source.__dag )
        , __super( source.__super ) {
      GUM_CONS_CPY( Interface );
      GUM_ERROR( FatalError, "don't copy an interface" );
    }

    template <typename GUM_SCALAR>
    Interface<GUM_SCALAR>::~Interface() {
      GUM_DESTRUCTOR( Interface );

      for ( const auto& elt : __nodeIdMap ) {
        delete elt.second;
      }
    }

    template <typename GUM_SCALAR>
    void Interface<GUM_SCALAR>::inheritInterface() {
      if (__super != nullptr) {
        __inheritInterface( *__super );
      }
    }

    template <typename GUM_SCALAR>
    void Interface<GUM_SCALAR>::__inheritInterface(
        const Interface<GUM_SCALAR>& i ) {
      // Copying attributes
      for ( const auto i_attr : i.__attributes ) {
        auto attr =
            new ScalarAttribute<GUM_SCALAR>( i_attr->name(), i_attr->type() );
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
        auto ref = new ReferenceSlot<GUM_SCALAR>(
            i_ref->name(),
            const_cast<ClassElementContainer<GUM_SCALAR>&>( i_ref->slotType() ),
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
    NodeId Interface<GUM_SCALAR>::add( ClassElement<GUM_SCALAR>* elt ) {
      if ( __nameMap.exists( elt->name() ) ) {
        GUM_ERROR( DuplicateElement,
                   "name already used by another ClassElement<GUM_SCALAR>" );
      }

      if ( ClassElement<GUM_SCALAR>::isAttribute( *elt ) ) {
        Attribute<GUM_SCALAR>* attr =
            static_cast<Attribute<GUM_SCALAR>*>( elt );
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
      } else if ( ClassElement<GUM_SCALAR>::isReferenceSlot( *elt ) ) {
        elt->setId( nextNodeId() );
        __dag.addNode( elt->id() );
        __nodeIdMap.insert( elt->id(), elt );
        __referenceSlots.insert(
            static_cast<ReferenceSlot<GUM_SCALAR>*>( elt ) );
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
    Interface<GUM_SCALAR>::overload( ClassElement<GUM_SCALAR>* overloader ) {
      try {
        if ( not super().exists( overloader->name() ) ) {
          GUM_ERROR( OperationNotAllowed,
                     "found no ClassElement<GUM_SCALAR> to overload" );
        }
      } catch ( NotFound& ) {
        GUM_ERROR( OperationNotAllowed,
                   "overload is possible only with sub interfaces" );
      }

      ClassElement<GUM_SCALAR>* overloaded = __nameMap[overloader->name()];
      if ( overloaded == overloader ) {
        GUM_ERROR( DuplicateElement, "dupplicate ClassElement" );
      }
      if ( not __checkOverloadLegality( overloaded, overloader ) ) {
        GUM_ERROR( OperationNotAllowed, "illegal overload" );
      }

      switch ( overloader->elt_type() ) {
        case ClassElement<GUM_SCALAR>::prm_attribute: {
          auto attr_overloader =
              static_cast<Attribute<GUM_SCALAR>*>( overloader );
          auto attr_overloaded =
              static_cast<Attribute<GUM_SCALAR>*>( overloaded );
          __overloadAttribute( attr_overloader, attr_overloaded );
          break;
        }

        case ClassElement<GUM_SCALAR>::prm_refslot: {
          auto ref_overloader =
              static_cast<ReferenceSlot<GUM_SCALAR>*>( overloader );
          auto ref_overloaded =
              static_cast<ReferenceSlot<GUM_SCALAR>*>( overloaded );
          __overloadReferenceSlot( ref_overloader, ref_overloaded );
          break;
        }

        case ClassElement<GUM_SCALAR>::prm_aggregate:
        case ClassElement<GUM_SCALAR>::prm_slotchain: {
          auto msg = "element can not be overloaded";
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
    void Interface<GUM_SCALAR>::__overloadAttribute(
        Attribute<GUM_SCALAR>* overloader, Attribute<GUM_SCALAR>* overloaded ) {
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
        // Swapping types, ugly but necessary to preserve the Type<GUM_SCALAR>
        // pointer of overloaded
        overloader->overload( overloaded );
        delete overloaded;
      }
    }

    template <typename GUM_SCALAR>
    void Interface<GUM_SCALAR>::__overloadReferenceSlot(
        ReferenceSlot<GUM_SCALAR>* overloader,
        ReferenceSlot<GUM_SCALAR>* overloaded ) {
      // Adding overloading reference
      overloader->setId( overloaded->id() );
      __nodeIdMap[overloader->id()] = overloader;
      __nameMap[overloader->name()] = overloader;
      __nameMap.insert( overloader->safeName(), overloader );
      __referenceSlots.insert( overloader );
      // Removing overloaded ReferenceSlot<GUM_SCALAR>
      __referenceSlots.erase( overloaded );
      __nameMap.erase( overloaded->safeName() );
      delete overloaded;
    }

    template <typename GUM_SCALAR>
    void
    Interface<GUM_SCALAR>::__addCastDescendants( Attribute<GUM_SCALAR>* start,
                                                 Attribute<GUM_SCALAR>* end ) {
      Attribute<GUM_SCALAR>* parent = start;
      Attribute<GUM_SCALAR>* child = 0;

      while ( parent->type().super() != end->type() ) {
        child = parent->getCastDescendant();
        child->setId( nextNodeId() );
        __dag.addNode( child->id() );
        __nodeIdMap.insert( child->id(), child );
        // Only use child's safe name when adding to the name map!
        __nameMap.insert( child->safeName(), child );
        __attributes.insert( child );
        // Do not use Class<GUM_SCALAR>::insertArc(), child's CPF is already
        // initialized properly
        parent = child;
      }

      parent->setAsCastDescendant( end );
    }

    template <typename GUM_SCALAR>
    bool Interface<GUM_SCALAR>::__checkOverloadLegality(
        const ClassElement<GUM_SCALAR>* overloaded,
        const ClassElement<GUM_SCALAR>* overloader ) {
      if ( overloaded->elt_type() != overloader->elt_type() ) {
        return false;
      }

      if ( overloaded->elt_type() == ClassElement<GUM_SCALAR>::prm_attribute ) {
        if ( not overloader->type().isSubTypeOf( overloaded->type() ) ) {
          return false;
        }
      } else if ( overloaded->elt_type() ==
                  ClassElement<GUM_SCALAR>::prm_refslot ) {
        auto ref_overloader =
            static_cast<const ReferenceSlot<GUM_SCALAR>*>( overloader );
        auto ref_overloaded =
            static_cast<const ReferenceSlot<GUM_SCALAR>*>( overloaded );
        if ( not ref_overloader->slotType().isSubTypeOf(
                 ref_overloaded->slotType() ) ) {
          return false;
        }
      } else {
        return false;
      }
      return true;
    }

    template <typename GUM_SCALAR>
    bool Interface<GUM_SCALAR>::isSubTypeOf(
        const ClassElementContainer<GUM_SCALAR>& cec ) const {
      switch ( cec.obj_type() ) {
        case ClassElement<GUM_SCALAR>::PRMType::CLASS: {
          return false;
        }

        case ClassElement<GUM_SCALAR>::PRMType::INTERFACE: {
          const Interface* current = this;

          while ( current != 0 ) {
            if ( current == &( cec ) ) return true;

            current = current->__super;
          }

          return false;
        }

        default: {
          GUM_ERROR( FatalError, "unknown ClassElementContainer<GUM_SCALAR>" );
        }
      }
    }

    template <typename GUM_SCALAR>
    void Interface<GUM_SCALAR>::_updateDescendants(
        const ClassElement<GUM_SCALAR>& elt ) {
      // for ( const auto ext : __extensions )
      //  if ( !ext->isOutputNode( elt ) ) ext->setOutputNode( elt, true );

      // for ( const auto impl : __implementations ) {
      //  // Because of cyclic dependencies we must use a reinterpret cast.
      //  ClassElementContainer<GUM_SCALAR>* c =
      //      reinterpret_cast<ClassElementContainer<GUM_SCALAR>*>( impl );

      //  if ( not c->isOutputNode( elt ) ) c->setOutputNode( elt, true );
      //}
    }

    template <typename GUM_SCALAR>
    INLINE typename Interface<GUM_SCALAR>::ClassEltIterator
    Interface<GUM_SCALAR>::begin() {
      return __nodeIdMap.begin();
    }

    template <typename GUM_SCALAR>
    INLINE const typename Interface<GUM_SCALAR>::ClassEltIterator&
    Interface<GUM_SCALAR>::end() {
      return __nodeIdMap.end();
    }

    template <typename GUM_SCALAR>
    INLINE typename Interface<GUM_SCALAR>::const_ClassEltIterator
    Interface<GUM_SCALAR>::begin() const {
      return __nodeIdMap.begin();
    }

    template <typename GUM_SCALAR>
    INLINE const typename Interface<GUM_SCALAR>::const_ClassEltIterator&
    Interface<GUM_SCALAR>::end() const {
      return __nodeIdMap.end();
    }

    template <typename GUM_SCALAR>
    INLINE void Interface<GUM_SCALAR>::addArc( const std::string& tail,
                                               const std::string& head ) {
      GUM_ERROR( OperationNotAllowed, "an Interface does not have arcs" );
    }

    template <typename GUM_SCALAR>
    INLINE Interface<GUM_SCALAR>& Interface<GUM_SCALAR>::super() {
      if ( __super )
        return *__super;
      else
        GUM_ERROR( NotFound, "this Interface is not a sub interface" );
    }

    template <typename GUM_SCALAR>
    INLINE const Interface<GUM_SCALAR>& Interface<GUM_SCALAR>::super() const {
      if ( __super )
        return *__super;
      else
        GUM_ERROR( NotFound, "this Interface is not a sub interface" );
    }

    template <typename GUM_SCALAR>
    INLINE void
    Interface<GUM_SCALAR>::__addImplementation( Class<GUM_SCALAR>* c ) {
      __implementations.insert( c );
    }

    template <typename GUM_SCALAR>
    INLINE void Interface<GUM_SCALAR>::__addExtension( Interface* i ) {
      __extensions.insert( i );
    }

    template <typename GUM_SCALAR>
    INLINE ClassElement<GUM_SCALAR>& Interface<GUM_SCALAR>::
    operator[]( NodeId id ) {
      return get( id );
    }

    template <typename GUM_SCALAR>
    INLINE const ClassElement<GUM_SCALAR>& Interface<GUM_SCALAR>::
    operator[]( NodeId id ) const {
      return get( id );
    }

    template <typename GUM_SCALAR>
    INLINE ClassElement<GUM_SCALAR>& Interface<GUM_SCALAR>::
    operator[]( const std::string& name ) {
      return get( name );
    }

    template <typename GUM_SCALAR>
    INLINE const ClassElement<GUM_SCALAR>& Interface<GUM_SCALAR>::
    operator[]( const std::string& name ) const {
      return get( name );
    }

    template <typename GUM_SCALAR>
    INLINE typename PRMObject::PRMType Interface<GUM_SCALAR>::obj_type() const {
      return PRMObject::PRMType::INTERFACE;
    }

    template <typename GUM_SCALAR>
    INLINE const DAG& Interface<GUM_SCALAR>::_dag() const {
      return __dag;
    }

    template <typename GUM_SCALAR>
    INLINE DAG& Interface<GUM_SCALAR>::_dag() {
      return __dag;
    }

    template <typename GUM_SCALAR>
    INLINE ClassElement<GUM_SCALAR>& Interface<GUM_SCALAR>::get( NodeId id ) {
      try {
        return *( __nodeIdMap[id] );
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound,
                   "no ClassElement<GUM_SCALAR> with the given NodeId" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE const ClassElement<GUM_SCALAR>&
    Interface<GUM_SCALAR>::get( NodeId id ) const {
      try {
        return *( __nodeIdMap[id] );
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound,
                   "no ClassElement<GUM_SCALAR> with the given NodeId" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE ClassElement<GUM_SCALAR>&
    Interface<GUM_SCALAR>::get( const std::string& name ) {
      try {
        return *( __nameMap[name] );
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound,
                   "no ClassElement<GUM_SCALAR> with the given name" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE const ClassElement<GUM_SCALAR>&
    Interface<GUM_SCALAR>::get( const std::string& name ) const {
      try {
        return *( __nameMap[name] );
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound,
                   "no ClassElement<GUM_SCALAR> with the given name" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE const Set<Attribute<GUM_SCALAR>*>&
    Interface<GUM_SCALAR>::attributes() const {
      return __attributes;
    }

    template <typename GUM_SCALAR>
    INLINE const Set<ReferenceSlot<GUM_SCALAR>*>&
    Interface<GUM_SCALAR>::referenceSlots() const {
      return __referenceSlots;
    }

    template <typename GUM_SCALAR>
    INLINE Set<Class<GUM_SCALAR>*>& Interface<GUM_SCALAR>::implementations() {
      return __implementations;
    }

    template <typename GUM_SCALAR>
    INLINE const Set<Class<GUM_SCALAR>*>&
    Interface<GUM_SCALAR>::implementations() const {
      return __implementations;
    }

    template <typename GUM_SCALAR>
    void Interface<GUM_SCALAR>::_findAllSubtypes(
        Set<ClassElementContainer<GUM_SCALAR>*>& set ) {
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
    INLINE bool Interface<GUM_SCALAR>::isOutputNode(
        const ClassElement<GUM_SCALAR>& elt ) const {
      try {
        if ( not this->_getIOFlag( elt ).second ) {

          for ( auto i : __implementations ) {
            if ( i->isOutputNode( elt ) ) {
              return true;
            }
          }

          if ( __super and __super->isOutputNode( elt ) ) {
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