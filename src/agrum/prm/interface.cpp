/***************************************************************************
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
 * @brief Headers of gum::prm::Interface
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 *
 */

#include <agrum/prm/interface.h>

#ifdef GUM_NO_INLINE
#include <agrum/prm/interface.inl>
#endif //GUM_NO_INLINE

namespace gum {
  namespace prm {


    Interface::Interface( const std::string& name ):
      ClassElementContainer( name ), __super( 0 ) {
      GUM_CONSTRUCTOR( Interface );
    }

    Interface::Interface( const std::string& name, Interface& super ):
      ClassElementContainer( name ), __dag( super.__dag ), __super( &super ) {
      GUM_CONSTRUCTOR( Interface );
      __inheritInterface( super );
    }

    Interface::Interface( const Interface& source ):
      ClassElementContainer( source.name() ), __dag( source.__dag ),
      __super( source.__super ) {
      GUM_CONS_CPY( Interface );
      GUM_ERROR( FatalError, "don't copy an interface" );
    }

    Interface::~Interface() {
      GUM_DESTRUCTOR( Interface );
      typedef Property<ClassElement*>::onNodes::iterator Iterator;

      for ( Iterator iter = __nodeIdMap.begin(); iter != __nodeIdMap.end(); ++iter ) {
        delete *iter;
      }
    }

    void
    Interface::__inheritInterface( const Interface& i ) {
      // Copying attributes
      for ( Set<Attribute*>::iterator iter = i.__attributes.begin(); iter != i.__attributes.end(); ++iter ) {
        Attribute* attr = new Attribute( ( *iter )->name(), ( *iter )->type() );
        attr->setId( ( *iter )->id() );
        __nodeIdMap.insert( attr->id(), attr );
        __attributes.insert( attr );

        if ( i.__nameMap[( *iter )->name()] == i.__nameMap[( *iter )->safeName()] )
          __nameMap.insert( attr->name(), attr );

        __nameMap.insert( attr->safeName(), attr );
      }

      // Copying reference slots
      for ( Set<ReferenceSlot*>::iterator iter = i.__referenceSlots.begin(); iter != i.__referenceSlots.end(); ++iter ) {
        ReferenceSlot* ref = new ReferenceSlot( ( *iter )->name(), const_cast<ClassElementContainer&>( ( *iter )->slotType() ), ( *iter )->isArray() );
        ref->setId( ( *iter )->id() );
        __nodeIdMap.insert( ref->id(), ref );
        __referenceSlots.insert( ref );

        if ( i.__nameMap.exists( ref->name() ) )
          __nameMap.insert( ref->name(), ref );

        __nameMap.insert( ref->safeName(), ref );
      }
    }

    NodeId
    Interface::add( ClassElement* elt ) {
      if ( __nameMap.exists( elt->name() ) ) {
        GUM_ERROR( DuplicateElement, "name already used by another ClassElement" );
      }

      if ( ClassElement::isAttribute( *elt ) ) {
        Attribute* attr = static_cast<Attribute*>( elt );
        __nameMap.insert( attr->name(), attr );

        while ( true ) {
          attr->setId( __dag.insertNode() );
          __nodeIdMap.insert( attr->id(), attr );
          __nameMap.insert( attr->safeName(), attr );
          __attributes.insert( attr );

          if ( attr->type().isSubType() ) {
            attr = attr->getCastDescendant();
          } else {
            break;
          }
        }
      } else if ( ClassElement::isReferenceSlot( *elt ) ) {
        elt->setId( __dag.insertNode() );
        __nodeIdMap.insert( elt->id(), elt );
        __referenceSlots.insert( static_cast<ReferenceSlot*>( elt ) );
        __nameMap.insert( elt->name(), elt );
        __nameMap.insert( elt->safeName(), elt );
      } else {
        GUM_ERROR( WrongClassElement, "illegal ClassElement for an Interface" );
      }

      return elt->id();
    }

    NodeId
    Interface::overload( ClassElement* overloader ) {
      try {
        if ( not super().exists( overloader->name() ) ) {
          GUM_ERROR( OperationNotAllowed, "found no ClassElement to overload" );
        }
      } catch ( NotFound& ) {
        GUM_ERROR( OperationNotAllowed, "overload is possible only with sub interfaces" );
      }

      ClassElement* overloaded = __nameMap[overloader->name()];
      // __checkOverloadLegality garanties that overloader->type() is a subtype of overloaded->type()
      __checkOverloadLegality( overloaded, overloader );

      switch ( overloader->elt_type() ) {
        case ClassElement::prm_attribute: {
          __overloadAttribute( static_cast<Attribute*>( overloader ), static_cast<Attribute*>( overloaded ) );
          break;
        }

        case ClassElement::prm_refslot: {
          ReferenceSlot* ref = static_cast<ReferenceSlot*>( overloader );
          // Adding overloading reference
          ref->setId( overloaded->id() );
          __nodeIdMap[ref->id()] = ref;
          __nameMap[ref->name()] = ref;
          __referenceSlots.insert( ref );
          // Removing overloaded ReferenceSlot
          __referenceSlots.erase( static_cast<ReferenceSlot*>( overloaded ) );
          delete overloaded;
          break;
        }

        case ClassElement::prm_aggregate:
        case ClassElement::prm_slotchain: {
          GUM_ERROR( WrongClassElement, "wrong ClassElement to overload can not be overloaded" );
          break;
        }

        default:
        { GUM_ERROR( FatalError, "unknown ClassElement type" ); }
      }

      return overloader->id();
    }

    void
    Interface::__overloadAttribute( Attribute* overloader, Attribute* overloaded ) {
      if ( overloader->type() != overloaded->type() ) {
        overloader->setId( __dag.insertNode() );
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
        // Swapping types, ugly but necessary to preserve the Type pointer of overloaded
        __swap_types( overloader, overloaded );
      }
    }

    void
    Interface::__addCastDescendants( Attribute* start, Attribute* end ) {
      Attribute* parent = start;
      Attribute* child = 0;

      while ( parent->type().super() != end->type() ) {
        child = parent->getCastDescendant();
        child->setId( __dag.insertNode() );
        __nodeIdMap.insert( child->id(), child );
        // Only use child's safe name when adding to the name map!
        __nameMap.insert( child->safeName(), child );
        __attributes.insert( child );
        parent = child;
      }

      parent->setAsCastDescendant( end );
    }

    void
    Interface::__swap_types( Attribute* overloader, Attribute* overloaded ) {
      Type* tmp = overloader->__type;
      overloader->__type = overloaded->__type;
      overloaded->__type = tmp;
      overloader->__cpf->erase( tmp->variable() );
      overloader->__cpf->add( overloader->__type->variable() );
      delete overloaded;
    }

    void
    Interface::__checkOverloadLegality( const ClassElement* overloaded, const ClassElement* overloader ) {
      if ( overloaded->elt_type() != overloader->elt_type() )
        GUM_ERROR( TypeError, "invalid overload" );

      if ( overloaded->elt_type() == ClassElement::prm_attribute ) {
        if ( not overloader->type().isSubTypeOf( overloaded->type() ) )
          GUM_ERROR( TypeError, "the overloading ClassElement Type is illegal" );
      } else if ( overloaded->elt_type() == ClassElement::prm_refslot ) {
        if ( not static_cast<const ReferenceSlot*>( overloader )->slotType().isSubTypeOf( static_cast<const ReferenceSlot*>( overloaded )->slotType() ) )
          GUM_ERROR( TypeError, "the overloading ReferenceSlot slot type is illegal" );
      } else {
        GUM_ERROR( TypeError, "illegal type to overload" );
      }
    }

    bool
    Interface::isSubTypeOf( const ClassElementContainer& cec ) const {
      switch ( cec.obj_type() ) {
        case PRMType::CLASS: {
          return false;
        }

        case PRMType::INTERFACE: {
          const Interface* current = this;

          while ( current != 0 ) {
            if ( current == &( cec ) ) return true;

            current = current->__super;
          }

          return false;
        }

        default:
        { GUM_ERROR( FatalError, "unknown ClassElementContainer" ); }
      }
    }

    void
    Interface::_updateDescendants( const ClassElement& elt ) {
      typedef Set<Interface*>::iterator Iter;

      for ( Iter iter = __extensions.begin(); iter != __extensions.end(); ++iter ) {
        if ( not( **iter ).isOutputNode( elt ) ) {
          ( **iter ).setOutputNode( elt, true );
        }
      }

      typedef Set<Class*>::iterator Jter;

      for ( Jter iter = __implementations.begin(); iter != __implementations.end(); ++iter ) {
        // Because of cyclic dependencies we must use a reinterpret cast.
        ClassElementContainer* c = reinterpret_cast<ClassElementContainer*>( *iter );

        if ( not c->isOutputNode( elt ) ) {
          c->setOutputNode( elt, true );
        }
      }
    }



  } /* namespace prm */
} /* namespace gum */

