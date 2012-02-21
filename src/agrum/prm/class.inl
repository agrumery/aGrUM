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
 * @author Lionel TORTI
 */

//to ease parsing
#include <agrum/prm/class.h>

// ============================================================================
namespace gum {
  namespace prm {
// ============================================================================

    INLINE
    Class::ClassEltIterator
    Class::begin() {
      return __nodeIdMap.begin();
    }

    INLINE
    const Class::ClassEltIterator&
    Class::end() {
      return __nodeIdMap.end();
    }

    INLINE
    Class::const_ClassEltIterator
    Class::begin() const {
      return __nodeIdMap.begin();
    }

    INLINE
    const Class::const_ClassEltIterator&
    Class::end() const {
      return __nodeIdMap.end();
    }

    INLINE
    PRMObject::ObjectType
    Class::obj_type() const {
      return PRMObject::prm_class;
    }

    INLINE
    const DAG&
    Class::_dag() const {
      return __dag;
    }

    INLINE
    DAG&
    Class::_dag() {
      return __dag;
    }

    INLINE
    ClassElement&
    Class::get( NodeId id ) {
      try {
        return *( __nodeIdMap[id] );
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound, "no ClassElement with the given NodeId" );
      }
    }

    INLINE
    const ClassElement&
    Class::get( NodeId id ) const {
      try {
        return *( __nodeIdMap[id] );
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound, "no ClassElement with the given NodeId" );
      }
    }

    INLINE
    bool
    Class::isParameter( const ClassElement& elt ) const {
      const ClassElement& mine = get( elt.safeName() );

      if ( ClassElement::isAttribute( mine ) ) {
        return __parameters.exists( static_cast<Attribute*>( const_cast<ClassElement*>( &mine ) ) );
      }

      return false;
      // if (__nodeIdMap.exists(id) and (ClassElement::isAttribute(get(id)))) {
      //   return __parameters.exists(&(static_cast<Attribute&>(const_cast<ClassElement&>(get(id)))));
      // } else if (not __nodeIdMap.exists(id)) {
      //   GUM_ERROR(NotFound, "no ClassElement with the given NodeId");
      // } else {
      //   GUM_ERROR(WrongClassElement, "given id is not a potential parameter");
      // }
    }

    INLINE
    ClassElement&
    Class::get( const std::string& name ) {
      try {	
        return *( __nameMap[name] );
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound, "no ClassElement with the given name" );
      }
    }

    INLINE
    const ClassElement&
    Class::get( const std::string& name ) const {
      try {
        return *( __nameMap[name] );
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound, "no ClassElement with the given name" );
      }
    }

    INLINE
    const Set< Attribute* >&
    Class::attributes() const {
      return __attributes;
    }

    INLINE
    const Set< Attribute* >&
    Class::parameters() const {
      return __parameters;
    }

    INLINE
    const Set< Aggregate* >&
    Class::aggregates() const {
      return __aggregates;
    }

    INLINE
    const Set< ReferenceSlot* >&
    Class::referenceSlots() const {
      return __referenceSlots;
    }

    INLINE
    const Set< SlotChain* >&
    Class::slotChains() const {
      return __slotChains;
    }

    INLINE
    const Sequence<NodeId>&
    Class::toInstantiate() const {
      return __instantiations;
    }

    INLINE
    const Class&
    Class::super() const {
      if ( __super ) {
        return *__super;
      } else {
        GUM_ERROR( NotFound, "this Class is not a subclass" );
      }
    }

    INLINE
    const Set<Interface*>&
    Class::implements() const {
      if ( __implements ) {
        return *__implements;
      } else {
        GUM_ERROR( NotFound, "this Class does not implement any Interface" );
      }
    }

    INLINE
    ClassElement&
    Class::operator[]( NodeId id ) {
      return get( id );
    }

    INLINE
    const ClassElement&
    Class::operator[]( NodeId id ) const {
      return get( id );
    }

    INLINE
    ClassElement&
    Class::operator[]( const std::string& name ) {
      return get( name );
    }

    INLINE
    const ClassElement&
    Class::operator[]( const std::string& name ) const {
      return get( name );
    }

    INLINE
    void
    Class::__overloadAggregate( Aggregate* overloader, ClassElement* overloaded ) {
      __nameMap.insert( overloader->safeName(), overloader );
      __aggregates.insert( overloader );
    }

    INLINE
    void
    Class::__checkOverloadLegality( const ClassElement* overloaded, const ClassElement* overloader ) {
      if ( overloaded->elt_type() != overloader->elt_type() )
        GUM_ERROR( TypeError, "invalid overload" );

      if ( overloaded->elt_type() == ClassElement::prm_attribute ) {
        if ( not overloader->type().isSubTypeOf( overloaded->type() ) ) {
          GUM_ERROR( TypeError, "the overloading ClassElement Type is illegal" );
        }
      } else if ( overloaded->elt_type() == ClassElement::prm_refslot ) {
        if ( not static_cast<const ReferenceSlot*>( overloader )->slotType().isSubTypeOf( static_cast<const ReferenceSlot*>( overloaded )->slotType() ) )
          GUM_ERROR( TypeError, "the overloading ReferenceSlot slot type is illegal" );
      } else {
        GUM_ERROR( TypeError, "illegal type to overload" );
      }
    }

    INLINE
    const Set<Class*>&
    Class::extensions() const { return __extensions; }

    INLINE
    void
    Class::__addExtension( Class* c ) {
      __extensions.insert( c );
    }

    INLINE
    NodeId
    Class::addParameter( Attribute* param, bool flag ) {
      if ( __nameMap.exists( param->name() ) ) {
        GUM_ERROR( DuplicateElement, "name already used by another ClassElement" );
      }

      param->setId( __dag.insertNode() );
      __nodeIdMap.insert( param->id(), param );
      __nameMap.insert( param->name(), param );
      __nameMap.insert( param->safeName(), param );
      __parameters.insert( param );
      __paramValueFlags.insert( param, flag );
      __addCastDescendants( param );
      return param->id();
    }

    INLINE
    bool
    Class::isCastDescendant( const std::string& safe_name ) const {
      const ClassElement& elt = get( safe_name );

      try {
        return elt.type().name() == get( elt.name() ).type().name();
      } catch ( OperationNotAllowed& ) {
        GUM_ERROR( NotFound, "no attribute with the given name" );
      }
    }

// ============================================================================
  } /* namespace prm */
} /* namespace gum */
// ============================================================================
