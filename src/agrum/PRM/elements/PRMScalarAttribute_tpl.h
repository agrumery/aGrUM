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
 *   MERCHANTABILITY || FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/**
 * @file
 * @brief Inline implementation of gum::PRMScalarAttribute
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#include <sstream>

#include <agrum/PRM/utils_prm.h>

#include <agrum/PRM/elements/PRMAttribute.h>
#include <agrum/PRM/elements/PRMType.h>

namespace gum {
  namespace prm {

    template <typename GUM_SCALAR>
    PRMScalarAttribute<GUM_SCALAR>::PRMScalarAttribute(
        const std::string&                  name,
        const PRMType<GUM_SCALAR>&          type,
        MultiDimImplementation<GUM_SCALAR>* impl )
        : PRMAttribute<GUM_SCALAR>( name )
        , __type( new PRMType<GUM_SCALAR>( type ) )
        , __cpf( new Potential<GUM_SCALAR>( impl ) ) {
      GUM_CONSTRUCTOR( PRMScalarAttribute );
      __cpf->add( __type->variable() );

      this->_safeName =
          PRMObject::LEFT_CAST() + __type->name() + PRMObject::RIGHT_CAST() + name;
      this->__type->variable().setName( this->name() );
    }

    template <typename GUM_SCALAR>
    PRMScalarAttribute<GUM_SCALAR>::PRMScalarAttribute(
        const PRMScalarAttribute<GUM_SCALAR>& source )
        : PRMAttribute<GUM_SCALAR>( source )
        , __type( 0 )
        , __cpf( 0 ) {
      GUM_CONS_CPY( PRMScalarAttribute );
      GUM_ERROR( FatalError,
                 "Illegal call to the copy constructor of gum::ScalarAttribute" );
    }

    template <typename GUM_SCALAR>
    PRMScalarAttribute<GUM_SCALAR>::~PRMScalarAttribute() {

      GUM_DESTRUCTOR( PRMScalarAttribute );
      delete __cpf;
      delete __type;
    }

    template <typename GUM_SCALAR>
    PRMAttribute<GUM_SCALAR>* PRMScalarAttribute<GUM_SCALAR>::newFactory(
        const PRMClass<GUM_SCALAR>& c ) const {
      auto impl = static_cast<MultiDimImplementation<GUM_SCALAR>*>(
          this->cpf().content()->newFactory() );
      return new PRMScalarAttribute<GUM_SCALAR>(
          this->name(), this->type(), impl );
    }

    template <typename GUM_SCALAR>
    PRMAttribute<GUM_SCALAR>* PRMScalarAttribute<GUM_SCALAR>::copy(
        Bijection<const DiscreteVariable*, const DiscreteVariable*> bij ) const {
      auto copy = new PRMScalarAttribute<GUM_SCALAR>( this->name(), this->type() );

      if ( !bij.existsFirst( &( type().variable() ) ) ) {
        bij.insert( &( type().variable() ), &( copy->type().variable() ) );
      }

      delete copy->__cpf;
      copy->__cpf = copyPotential( bij, cpf() );

      return copy;
    }

    template <typename GUM_SCALAR>
    void PRMScalarAttribute<GUM_SCALAR>::copyCpf(
        const Bijection<const DiscreteVariable*, const DiscreteVariable*>& bij,
        const PRMAttribute<GUM_SCALAR>& source ) {

      delete __cpf;
      __cpf = new Potential<GUM_SCALAR>();

      for ( auto var : source.cpf().variablesSequence() ) {
        __cpf->add( *( bij.second( var ) ) );
      }

      Instantiation inst( *__cpf ), jnst( source.cpf() );

      for ( inst.begin(), jnst.begin(); !( inst.end() || jnst.end() );
            inst.inc(), jnst.inc() ) {
        __cpf->set( inst, source.cpf().get( jnst ) );
      }

      GUM_ASSERT( inst.end() && jnst.end() );
      GUM_ASSERT( __cpf->contains( __type->variable() ) );
      GUM_ASSERT( !__cpf->contains( source.type().variable() ) );
    }

    template <typename GUM_SCALAR>
    PRMScalarAttribute<GUM_SCALAR>& PRMScalarAttribute<GUM_SCALAR>::
    operator=( const PRMScalarAttribute<GUM_SCALAR>& from ) {
      GUM_ERROR( FatalError,
                 "Illegal call to the copy operator of gum::ScalarAttribute" );
    }

    template <typename GUM_SCALAR>
    INLINE typename PRMClassElement<GUM_SCALAR>::ClassElementType
    PRMScalarAttribute<GUM_SCALAR>::elt_type() const {
      return this->prm_attribute;
    }

    template <typename GUM_SCALAR>
    INLINE PRMType<GUM_SCALAR>& PRMScalarAttribute<GUM_SCALAR>::type() {
      return *__type;
    }

    template <typename GUM_SCALAR>
    INLINE const PRMType<GUM_SCALAR>&
                 PRMScalarAttribute<GUM_SCALAR>::type() const {
      return *__type;
    }

    template <typename GUM_SCALAR>
    INLINE const Potential<GUM_SCALAR>&
                 PRMScalarAttribute<GUM_SCALAR>::cpf() const {
      return *__cpf;
    }

    template <typename GUM_SCALAR>
    INLINE void PRMScalarAttribute<GUM_SCALAR>::addParent(
        const PRMClassElement<GUM_SCALAR>& elt ) {
      try {
        __cpf->add( elt.type().variable() );
      } catch ( DuplicateElement& ) {
        std::stringstream msg;
        msg << ": " << elt.name() << " as parent of " << this->name();
        GUM_ERROR( DuplicateElement, msg.str() );
      } catch ( OperationNotAllowed& ) {
        std::stringstream msg;
        msg << ": " << elt.name() << " of wrong type as parent of "
            << this->name();
        GUM_ERROR( OperationNotAllowed, msg.str() );
      }
    }

    // See gum::PRMClassElement<GUM_SCALAR>::_addChild().
    template <typename GUM_SCALAR>
    INLINE void PRMScalarAttribute<GUM_SCALAR>::addChild(
        const PRMClassElement<GUM_SCALAR>& elt ) {}

    template <typename GUM_SCALAR>
    PRMAttribute<GUM_SCALAR>*
    PRMScalarAttribute<GUM_SCALAR>::getCastDescendant() const {
      PRMScalarAttribute<GUM_SCALAR>* cast = 0;

      try {
        cast =
            new PRMScalarAttribute<GUM_SCALAR>( this->name(), type().superType() );
      } catch ( NotFound& ) {
        GUM_ERROR( OperationNotAllowed,
                   "this ScalarAttribute can not have cast descendant" );
      }

      cast->addParent( *this );
      const DiscreteVariable& my_var = type().variable();
      DiscreteVariable&       cast_var = cast->type().variable();
      Instantiation           inst( cast->cpf() );

      for ( inst.setFirst(); !inst.end(); inst.inc() ) {
        if ( type().label_map()[inst.val( my_var )] == inst.val( cast_var ) ) {
          cast->cpf().set( inst, 1 );
        } else {
          cast->cpf().set( inst, 0 );
        }
      }

      return cast;
    }

    template <typename GUM_SCALAR>
    void PRMScalarAttribute<GUM_SCALAR>::setAsCastDescendant(
        PRMAttribute<GUM_SCALAR>* cast ) {
      try {
        type().setSuper( cast->type() );
      } catch ( OperationNotAllowed& ) {
        GUM_ERROR( OperationNotAllowed,
                   "this ScalarAttribute can not have cast descendant" );
      } catch ( WrongType& ) {
        std::stringstream msg;
        msg << type().name() << " is not a subtype of " << cast->type().name();
        GUM_ERROR( WrongType, msg.str() );
      }
      cast->becomeCastDescendant( type() );
    }

    template <typename GUM_SCALAR>
    void PRMScalarAttribute<GUM_SCALAR>::becomeCastDescendant(
        PRMType<GUM_SCALAR>& subtype ) {
      delete __cpf;
      __cpf = new Potential<GUM_SCALAR>();
      __cpf->add( type().variable() );
      __cpf->add( subtype.variable() );

      Instantiation inst( *__cpf );

      for ( inst.setFirst(); !inst.end(); inst.inc() ) {
        auto my_pos = inst.pos( subtype.variable() );
        if ( subtype.label_map()[my_pos] == inst.pos( type().variable() ) ) {
          __cpf->set( inst, 1 );
        } else {
          __cpf->set( inst, 0 );
        }
      }
    }

    template <typename GUM_SCALAR>
    void
    PRMScalarAttribute<GUM_SCALAR>::swap( const PRMType<GUM_SCALAR>& old_type,
                                          const PRMType<GUM_SCALAR>& new_type ) {
      if ( &( old_type ) == __type ) {
        GUM_ERROR( OperationNotAllowed, "Cannot swap attribute own type" );
      }
      if ( old_type->domainSize() != new_type->domainSize() ) {
        GUM_ERROR( OperationNotAllowed,
                   "Cannot swap types with difference domain size" );
      }
      if ( !__cpf->contains( old_type.variable() ) ) {
        GUM_ERROR( NotFound, "could not find variable " + old_type.name() );
      }

      auto old = __cpf;

      __cpf = new Potential<GUM_SCALAR>();

      for ( auto var : old->variablesSequence() ) {
        if ( var != &( old_type.variable() ) ) {
          __cpf->add( *var );
        } else {
          __cpf->add( new_type.variable() );
        }
      }

      Instantiation inst( __cpf ), jnst( old );

      for ( inst.begin(), jnst.begin(); !( inst.end() || jnst.end() );
            inst.inc(), jnst.inc() ) {
        __cpf->set( inst, old->get( jnst ) );
      }

      delete old;

      GUM_ASSERT( inst.end() && jnst.end() );
      GUM_ASSERT( __cpf->contains( __type->variable() ) );
      GUM_ASSERT( __cpf->contains( new_type.variable() ) );
      GUM_ASSERT( !__cpf->contains( old_type.variable() ) );
    }

    template <typename GUM_SCALAR>
    PRMType<GUM_SCALAR>* PRMScalarAttribute<GUM_SCALAR>::_type() {
      return __type;
    }

    template <typename GUM_SCALAR>
    void PRMScalarAttribute<GUM_SCALAR>::_type( PRMType<GUM_SCALAR>* t ) {

      if ( __type->variable().domainSize() != t->variable().domainSize() ) {
        GUM_ERROR( OperationNotAllowed,
                   "Cannot swap types with difference domain size" );
      }
      auto old = __cpf;

      __cpf = new Potential<GUM_SCALAR>();

      for ( auto var : old->variablesSequence() ) {
        if ( var != &( __type->variable() ) ) {
          __cpf->add( *var );
        } else {
          __cpf->add( t->variable() );
        }
      }

      Instantiation inst( __cpf ), jnst( old );

      for ( inst.begin(), jnst.begin(); !( inst.end() || jnst.end() );
            inst.inc(), jnst.inc() ) {
        __cpf->set( inst, old->get( jnst ) );
      }

      delete old;

      __type = t;

      GUM_ASSERT( __cpf->contains( __type->variable() ) );
      GUM_ASSERT( inst.end() && jnst.end() );
    }

  } /* namespace prm */
} /* namespace gum */
