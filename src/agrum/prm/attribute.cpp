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
 * @brief Source implementation of gum::Attribute
 *
 * @author Lionel TORTI
 *
 */

#include <agrum/prm/attribute.h>

#ifdef GUM_NO_INLINE
#include <agrum/prm/attribute.inl>
#endif //GUM_NO_INLINE

namespace gum {
  namespace prm {

    Attribute::Attribute( const std::string& name, const Type& type,
                          MultiDimImplementation<prm_float>* impl ):
      ClassElement( name ), __type( new Type( type ) ),
      __cpf( new Potential<prm_float>( impl ) ), __delete_type( true ) {
      GUM_CONSTRUCTOR( Attribute );
      __cpf->add( **__type );
      _safeName = ClassElement::LEFT_CAST() + __type->name() + ClassElement::RIGHT_CAST() + name;
    }

    Attribute::Attribute( const std::string& name, Type* type, Potential<prm_float>* cpf,
                          bool delete_type ):
      ClassElement( name ), __type( type ), __cpf( cpf ), __delete_type( delete_type ) {
      GUM_CONSTRUCTOR( Attribute );

      if ( not __cpf->variablesSequence().exists( &( type->variable() ) ) )
        GUM_ERROR( OperationNotAllowed, "the given Potential does not contain the type of this Attribute." );

      _safeName = ClassElement::LEFT_CAST() + __type->name() + ClassElement::RIGHT_CAST() + name;
    }

    Attribute::Attribute( const Attribute& source ):
      ClassElement( source ), __type( new Type( source.type() ) ), __cpf( 0 ) {
      GUM_CONS_CPY( Attribute );
      GUM_ERROR( FatalError, "Illegal call to the copy constructor of gum::Attribute" );
    }

    Attribute::~Attribute() {
      GUM_DESTRUCTOR( Attribute );
      delete __cpf;

      if ( __delete_type ) {
        delete __type;
      }
    }

    Attribute&
    Attribute::operator=( const Attribute& from ) {
      GUM_ERROR( FatalError, "Illegal call to the copy operator of gum::Attribute" );
    }

    Attribute*
    Attribute::getCastDescendant() const {
      Attribute* cast = 0;

      try {
        cast = new Attribute( name(), type().super() );
      } catch ( NotFound& ) {
        GUM_ERROR( OperationNotAllowed, "this Attribute can not have cast descendant" );
      }

      cast->addParent( *this );
      const DiscreteVariable& my_var = type().variable();
      DiscreteVariable& cast_var = cast->type().variable();
      Instantiation inst( cast->cpf() );

      for ( inst.setFirst(); not inst.end(); inst.inc() ) {
        if ( type().label_map()[inst.val( my_var )] == inst.val( cast_var ) ) {
          cast->cpf().set( inst, 1 );
        } else {
          cast->cpf().set( inst, 0 );
        }
      }

      return cast;
    }

    void
    Attribute::setAsCastDescendant( Attribute* cast ) {
      try {
        type().setSuper( cast->type() );
      } catch ( OperationNotAllowed& ) {
        GUM_ERROR( OperationNotAllowed, "this Attribute can not have cast descendant" );
      } catch ( WrongType& ) {
        GUM_ERROR( WrongType, "the cast descendant Type is not a direct subtype of this Attribute super Type" );
      }

      Potential<prm_float>* cpf = new Potential<prm_float>();
      cpf->add( cast->type().variable() );
      cpf->add( type().variable() );
      DiscreteVariable& my_var = type().variable();
      DiscreteVariable& cast_var = cast->type().variable();
      Instantiation inst( *cpf );

      for ( inst.setFirst(); not inst.end(); inst.inc() ) {
        if ( type().label_map()[inst.pos( my_var )] == inst.pos( cast_var ) )
          cpf->set( inst, 1 );
        else
          cpf->set( inst, 0 );
      }

      delete cast->__cpf;
      cast->__cpf = cpf;
    }


  } /* namespace prm */
} /* namespace gum */

