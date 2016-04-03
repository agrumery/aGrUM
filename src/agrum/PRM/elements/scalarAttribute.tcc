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
 * @brief Inline implementation of gum::ScalarAttribute
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#include <sstream>

#include <agrum/PRM/utils_prm.h>

#include <agrum/PRM/elements/attribute.h>
#include <agrum/PRM/elements/type.h>

namespace gum {
  namespace prm {

    template <typename GUM_SCALAR>
    ScalarAttribute<GUM_SCALAR>::ScalarAttribute(
        const std::string& name,
        const Type<GUM_SCALAR>& type,
        MultiDimImplementation<GUM_SCALAR>* impl )
        : PRMAttribute<GUM_SCALAR>( name )
        , __type( new Type<GUM_SCALAR>( type ) )
        , __cpf( new Potential<GUM_SCALAR>( impl ) ) {
      GUM_CONSTRUCTOR( ScalarAttribute );
      __cpf->add( __type->variable() );

      this->_safeName = PRMObject::LEFT_CAST() + __type->name() +
                        PRMObject::RIGHT_CAST() + name;
    }

    template <typename GUM_SCALAR>
    ScalarAttribute<GUM_SCALAR>::ScalarAttribute(
        const ScalarAttribute<GUM_SCALAR>& source )
        : PRMAttribute<GUM_SCALAR>( source )
        , __type( 0 )
        , __cpf( 0 ) {
      GUM_CONS_CPY( ScalarAttribute );
      GUM_ERROR(
          FatalError,
          "Illegal call to the copy constructor of gum::ScalarAttribute" );
    }

    template <typename GUM_SCALAR>
    ScalarAttribute<GUM_SCALAR>::~ScalarAttribute() {

      GUM_DESTRUCTOR( ScalarAttribute );
      delete __cpf;
      delete __type;
    }

    template <typename GUM_SCALAR>
    PRMAttribute<GUM_SCALAR>* ScalarAttribute<GUM_SCALAR>::newFactory(
        const PRMClass<GUM_SCALAR>& c ) const {
      auto impl = static_cast<MultiDimImplementation<GUM_SCALAR>*>(
          this->cpf().content()->newFactory() );
      return new ScalarAttribute<GUM_SCALAR>(
          this->name(), this->type(), impl );
    }

    template <typename GUM_SCALAR>
    PRMAttribute<GUM_SCALAR>* ScalarAttribute<GUM_SCALAR>::copy(
        Bijection<const DiscreteVariable*, const DiscreteVariable*> bij )
        const {
      auto copy = new ScalarAttribute<GUM_SCALAR>( this->name(), this->type() );

      if ( not bij.existsFirst( &( type().variable() ) ) ) {
        bij.insert( &( type().variable() ), &( copy->type().variable() ) );
      }

      delete copy->__cpf;
      copy->__cpf = copyPotential( bij, cpf() );

      return copy;
    }

    template <typename GUM_SCALAR>
    void ScalarAttribute<GUM_SCALAR>::copyCpf(
        const Bijection<const DiscreteVariable*, const DiscreteVariable*>& bij,
        const PRMAttribute<GUM_SCALAR>& source ) {

      delete __cpf;
      __cpf = new Potential<GUM_SCALAR>();

      for ( auto var : source.cpf().variablesSequence() ) {
        __cpf->add( *( bij.second( var ) ) );
      }

      Instantiation inst( *__cpf ), jnst( source.cpf() );

      for ( inst.begin(), jnst.begin(); not( inst.end() or jnst.end() );
            inst.inc(), jnst.inc() ) {
        __cpf->set( inst, source.cpf().get( jnst ) );
      }

      GUM_ASSERT( inst.end() and jnst.end() );
      GUM_ASSERT( __cpf->contains( __type->variable() ) );
      GUM_ASSERT( not __cpf->contains( source.type().variable() ) );
    }

    template <typename GUM_SCALAR>
    ScalarAttribute<GUM_SCALAR>& ScalarAttribute<GUM_SCALAR>::
    operator=( const ScalarAttribute<GUM_SCALAR>& from ) {
      GUM_ERROR( FatalError,
                 "Illegal call to the copy operator of gum::ScalarAttribute" );
    }

    template <typename GUM_SCALAR>
    INLINE typename ClassElement<GUM_SCALAR>::ClassElementType
    ScalarAttribute<GUM_SCALAR>::elt_type() const {
      return this->prm_attribute;
    }

    template <typename GUM_SCALAR>
    INLINE Type<GUM_SCALAR>& ScalarAttribute<GUM_SCALAR>::type() {
      return *__type;
    }

    template <typename GUM_SCALAR>
    INLINE const Type<GUM_SCALAR>& ScalarAttribute<GUM_SCALAR>::type() const {
      return *__type;
    }

    template <typename GUM_SCALAR>
    INLINE const Potential<GUM_SCALAR>&
    ScalarAttribute<GUM_SCALAR>::cpf() const {
      return *__cpf;
    }

    template <typename GUM_SCALAR>
    INLINE void ScalarAttribute<GUM_SCALAR>::addParent(
        const ClassElement<GUM_SCALAR>& elt ) {
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

    // See gum::ClassElement<GUM_SCALAR>::_addChild().
    template <typename GUM_SCALAR>
    INLINE void ScalarAttribute<GUM_SCALAR>::addChild(
        const ClassElement<GUM_SCALAR>& elt ) {}

    template <typename GUM_SCALAR>
    PRMAttribute<GUM_SCALAR>*
    ScalarAttribute<GUM_SCALAR>::getCastDescendant() const {
      ScalarAttribute<GUM_SCALAR>* cast = 0;

      try {
        cast = new ScalarAttribute<GUM_SCALAR>( this->name(), type().superType() );
      } catch ( NotFound& ) {
        GUM_ERROR( OperationNotAllowed,
                   "this ScalarAttribute can not have cast descendant" );
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

    template <typename GUM_SCALAR>
    void ScalarAttribute<GUM_SCALAR>::setAsCastDescendant(
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
    void ScalarAttribute<GUM_SCALAR>::becomeCastDescendant(
        Type<GUM_SCALAR>& subtype ) {
      delete __cpf;
      __cpf = new Potential<GUM_SCALAR>();
      __cpf->add( type().variable() );
      __cpf->add( subtype.variable() );

      Instantiation inst( *__cpf );

      for ( inst.setFirst(); not inst.end(); inst.inc() ) {
        auto my_pos = inst.pos( subtype.variable() );
        if ( subtype.label_map()[my_pos] == inst.pos( type().variable() ) ) {
          __cpf->set( inst, 1 );
        } else {
          __cpf->set( inst, 0 );
        }
      }
    }

    template <typename GUM_SCALAR>
    void ScalarAttribute<GUM_SCALAR>::swap( const Type<GUM_SCALAR>& old_type,
                                            const Type<GUM_SCALAR>& new_type ) {
      if ( &( old_type ) == __type ) {
        GUM_ERROR( OperationNotAllowed, "Cannot swap attribute own type" );
      }
      if ( old_type->domainSize() != new_type->domainSize() ) {
        GUM_ERROR( OperationNotAllowed,
                   "Cannot swap types with difference domain size" );
      }
      if ( not __cpf->contains( old_type.variable() ) ) {
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

      for ( inst.begin(), jnst.begin(); not( inst.end() or jnst.end() );
            inst.inc(), jnst.inc() ) {
        __cpf->set( inst, old->get( jnst ) );
      }

      delete old;

      GUM_ASSERT( inst.end() and jnst.end() );
      GUM_ASSERT( __cpf->contains( __type->variable() ) );
      GUM_ASSERT( __cpf->contains( new_type.variable() ) );
      GUM_ASSERT( not __cpf->contains( old_type.variable() ) );
    }

    template <typename GUM_SCALAR>
    Type<GUM_SCALAR>* ScalarAttribute<GUM_SCALAR>::_type() {
      return __type;
    }

    template <typename GUM_SCALAR>
    void ScalarAttribute<GUM_SCALAR>::_type( Type<GUM_SCALAR>* t ) {

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

      for ( inst.begin(), jnst.begin(); not( inst.end() or jnst.end() );
            inst.inc(), jnst.inc() ) {
        __cpf->set( inst, old->get( jnst ) );
      }

      delete old;

      __type = t;

      GUM_ASSERT( __cpf->contains( __type->variable() ) );
      GUM_ASSERT( inst.end() and jnst.end() );
    }

  } /* namespace prm */
} /* namespace gum */
