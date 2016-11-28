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
 * @brief Inline implementation of gum::PRMFormAttribute
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */
#include <iostream>

#include <agrum/core/math/formula.h>

#include <agrum/PRM/elements/PRMScalarAttribute.h>
#include <agrum/PRM/elements/PRMType.h>

// to ease IDE parser
#include <agrum/PRM/elements/PRMFormAttribute.h>

namespace gum {
  namespace prm {

    template <typename GUM_SCALAR>
    PRMFormAttribute<GUM_SCALAR>::PRMFormAttribute(
        const PRMClass<GUM_SCALAR>&          c,
        const std::string&                   name,
        const PRMType<GUM_SCALAR>&           type,
        MultiDimImplementation<std::string>* impl )
        : PRMAttribute<GUM_SCALAR>( name )
        , __type( new PRMType<GUM_SCALAR>( type ) )
        , __cpf( 0 )
        , __formulas( impl )
        , __class( &c ) {
      GUM_CONSTRUCTOR( PRMFormAttribute );
      __formulas->add( __type->variable() );
      this->_safeName =
          PRMObject::LEFT_CAST() + __type->name() + PRMObject::RIGHT_CAST() + name;
      this->__type->variable().setName( this->name() );
    }

    template <typename GUM_SCALAR>
    PRMFormAttribute<GUM_SCALAR>::~PRMFormAttribute() {
      GUM_DESTRUCTOR( PRMFormAttribute );
      delete __type;
      delete __cpf;
      delete __formulas;
    }

    template <typename GUM_SCALAR>
    PRMAttribute<GUM_SCALAR>* PRMFormAttribute<GUM_SCALAR>::newFactory(
        const PRMClass<GUM_SCALAR>& c ) const {
      auto impl = static_cast<MultiDimImplementation<std::string>*>(
          this->__formulas->newFactory() );
      return new PRMFormAttribute( c, this->name(), this->type(), impl );
    }

    template <typename GUM_SCALAR>
    PRMAttribute<GUM_SCALAR>* PRMFormAttribute<GUM_SCALAR>::copy(
        Bijection<const DiscreteVariable*, const DiscreteVariable*> bij ) const {
      auto copy =
          new PRMFormAttribute<GUM_SCALAR>( *__class, this->name(), this->type() );
      for ( auto var : __formulas->variablesSequence() ) {
        if ( var != &( __type->variable() ) ) {
          copy->__formulas->add( *var );
        }
      }

      Instantiation inst( *( copy->__formulas ) ), jnst( *__formulas );
      for ( inst.begin(), jnst.begin(); !( inst.end() || jnst.end() );
            inst.inc(), jnst.inc() ) {
        copy->__formulas->set( inst, __formulas->get( jnst ) );
      }

      GUM_ASSERT( copy->__formulas->contains( copy->__type->variable() ) );
      return copy;
    }

    template <typename GUM_SCALAR>
    void PRMFormAttribute<GUM_SCALAR>::copyCpf(
        const Bijection<const DiscreteVariable*, const DiscreteVariable*>& bij,
        const PRMAttribute<GUM_SCALAR>& source ) {

      delete __formulas;
      __formulas = new MultiDimArray<std::string>();

      for ( const auto& var : source.cpf().variablesSequence() ) {
        __formulas->add( *( bij.second( var ) ) );
      }

      if ( dynamic_cast<const PRMFormAttribute*>( &source ) ) {

        const auto& src = static_cast<const PRMFormAttribute&>( source );

        Instantiation inst( __formulas ), jnst( src.__formulas );

        for ( inst.begin(), jnst.begin(); !( inst.end() || jnst.end() );
              inst.inc(), jnst.inc() ) {
          __formulas->set( inst, src.__formulas->get( jnst ) );
        }

        GUM_ASSERT( inst.end() && jnst.end() );

      } else {

        Instantiation inst( __formulas ), jnst( source.cpf() );

        for ( inst.begin(), jnst.begin(); !( inst.end() || jnst.end() );
              inst.inc(), jnst.inc() ) {

          auto val = std::to_string( source.cpf().get( jnst ) );
          __formulas->set( inst, val );
        }

        GUM_ASSERT( inst.end() && jnst.end() );
      }

      if ( __cpf ) {
        delete __cpf;
        __cpf = 0;
      }

      GUM_ASSERT( __formulas->contains( __type->variable() ) );
      GUM_ASSERT( !__formulas->contains( source.type().variable() ) );
    }

    template <typename GUM_SCALAR>
    typename PRMClassElement<GUM_SCALAR>::ClassElementType
    PRMFormAttribute<GUM_SCALAR>::elt_type() const {
      return this->prm_attribute;
    }

    template <typename GUM_SCALAR>
    PRMType<GUM_SCALAR>& PRMFormAttribute<GUM_SCALAR>::type() {
      return *__type;
    }

    template <typename GUM_SCALAR>
    const PRMType<GUM_SCALAR>& PRMFormAttribute<GUM_SCALAR>::type() const {
      return *__type;
    }

    template <typename GUM_SCALAR>
    const Potential<GUM_SCALAR>& PRMFormAttribute<GUM_SCALAR>::cpf() const {
      if ( __cpf == 0 ) {
        __fillCpf();
      }
      return *__cpf;
    }

    template <typename GUM_SCALAR>
    void PRMFormAttribute<GUM_SCALAR>::addParent(
        const PRMClassElement<GUM_SCALAR>& elt ) {
      try {
        if ( __cpf ) {
          delete __cpf;
          __cpf = 0;
        }
        __formulas->add( elt.type().variable() );
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

      GUM_ASSERT( __formulas->contains( __type->variable() ) );
    }

    template <typename GUM_SCALAR>
    void PRMFormAttribute<GUM_SCALAR>::addChild(
        const PRMClassElement<GUM_SCALAR>& elt ) {}

    template <typename GUM_SCALAR>
    PRMAttribute<GUM_SCALAR>*
    PRMFormAttribute<GUM_SCALAR>::getCastDescendant() const {
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

      GUM_ASSERT( __formulas->contains( __type->variable() ) );
      return cast;
    }

    template <typename GUM_SCALAR>
    void PRMFormAttribute<GUM_SCALAR>::setAsCastDescendant(
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
    void PRMFormAttribute<GUM_SCALAR>::becomeCastDescendant(
        PRMType<GUM_SCALAR>& subtype ) {
      delete __formulas;

      __formulas = new MultiDimArray<std::string>();
      __formulas->add( type().variable() );
      __formulas->add( subtype.variable() );

      Instantiation inst( __formulas );

      for ( inst.setFirst(); !inst.end(); inst.inc() ) {
        auto my_pos = inst.pos( subtype.variable() );
        if ( subtype.label_map()[my_pos] == inst.pos( type().variable() ) ) {
          __formulas->set( inst, "1" );
        } else {
          __formulas->set( inst, "0" );
        }
      }

      if ( __cpf ) {
        delete __cpf;
        __cpf = nullptr;
      }
    }

    template <typename GUM_SCALAR>
    PRMFormAttribute<GUM_SCALAR>::PRMFormAttribute(
        const PRMFormAttribute& source )
        : PRMAttribute<GUM_SCALAR>( source.name() ) {
      GUM_CONS_CPY( PRMFormAttribute );
      GUM_ERROR( OperationNotAllowed, "Cannot copy FormAttribute" );
    }

    template <typename GUM_SCALAR>
    PRMFormAttribute<GUM_SCALAR>& PRMFormAttribute<GUM_SCALAR>::
    operator=( const PRMFormAttribute& source ) {
      GUM_ERROR( OperationNotAllowed, "Cannot copy FormAttribute" );
    }

    template <typename GUM_SCALAR>
    void PRMFormAttribute<GUM_SCALAR>::__fillCpf() const {
      try {

        if ( __cpf ) {
          delete __cpf;
        }

        __cpf = new Potential<GUM_SCALAR>();

        for ( auto var : __formulas->variablesSequence() ) {
          __cpf->add( *var );
        }

        auto params = __class->scope();

        Instantiation inst( __formulas );
        Instantiation jnst( __cpf );

        for ( inst.begin(), jnst.begin(); !( inst.end() || jnst.end() );
              inst.inc(), jnst.inc() ) {

          // With CPT defined using rules, empty values can appear
          auto val = __formulas->get( inst );
          if ( val == "" ) {
            val = "0.0";
          }

          Formula f( val );

          for ( auto item : params ) {
            f.variables().insert( item.first, item.second->value() );
          }

          __cpf->set( jnst, (GUM_SCALAR)f.result() );
        }

        GUM_ASSERT( inst.end() && jnst.end() );

      } catch ( Exception& ) {
        GUM_ERROR( NotFound, "undefined value in cpt" );
      }
      GUM_ASSERT( __formulas->contains( __type->variable() ) );
    }

    template <typename GUM_SCALAR>
    MultiDimImplementation<std::string>& PRMFormAttribute<GUM_SCALAR>::formulas() {
      if ( __cpf ) {
        delete __cpf;
        __cpf = 0;
      }
      return *__formulas;
    }

    template <typename GUM_SCALAR>
    const MultiDimImplementation<std::string>&
    PRMFormAttribute<GUM_SCALAR>::formulas() const {
      return *__formulas;
    }

    template <typename GUM_SCALAR>
    void
    PRMFormAttribute<GUM_SCALAR>::swap( const PRMType<GUM_SCALAR>& old_type,
                                        const PRMType<GUM_SCALAR>& new_type ) {
      if ( &( old_type ) == __type ) {
        GUM_ERROR( OperationNotAllowed, "Cannot swap attribute own type" );
      }
      if ( old_type->domainSize() != new_type->domainSize() ) {
        GUM_ERROR( OperationNotAllowed,
                   "Cannot swap types with difference domain size" );
      }
      if ( !__formulas->contains( old_type.variable() ) ) {
        GUM_ERROR( NotFound, "could not find variable " + old_type.name() );
      }

      auto old = __formulas;

      __formulas = new MultiDimArray<std::string>();

      for ( auto var : old->variablesSequence() ) {
        if ( var != &( old_type.variable() ) ) {
          __formulas->add( *var );
        } else {
          __formulas->add( new_type.variable() );
        }
      }

      Instantiation inst( __formulas ), jnst( old );

      for ( inst.begin(), jnst.begin(); !( inst.end() || jnst.end() );
            inst.inc(), jnst.inc() ) {
        __formulas->set( inst, old->get( jnst ) );
      }

      delete old;

      if ( __cpf ) {
        delete __cpf;
        __cpf = 0;
      }

      GUM_ASSERT( inst.end() && jnst.end() );
      GUM_ASSERT( __formulas->contains( __type->variable() ) );
      GUM_ASSERT( !__formulas->contains( new_type.variable() ) );
      GUM_ASSERT( __formulas->contains( new_type.variable() ) );
    }

    template <typename GUM_SCALAR>
    PRMType<GUM_SCALAR>* PRMFormAttribute<GUM_SCALAR>::_type() {
      return __type;
    }

    template <typename GUM_SCALAR>
    void PRMFormAttribute<GUM_SCALAR>::_type( PRMType<GUM_SCALAR>* t ) {

      if ( __type->variable().domainSize() != t->variable().domainSize() ) {
        GUM_ERROR( OperationNotAllowed,
                   "Cannot swap types with difference domain size" );
      }
      auto old = __formulas;

      __formulas = new MultiDimArray<std::string>();

      for ( auto var : old->variablesSequence() ) {
        if ( var != &( __type->variable() ) ) {
          __formulas->add( *var );
        } else {
          __formulas->add( t->variable() );
        }
      }

      Instantiation inst( __formulas ), jnst( old );

      for ( inst.begin(), jnst.begin(); !( inst.end() || jnst.end() );
            inst.inc(), jnst.inc() ) {
        __formulas->set( inst, old->get( jnst ) );
      }

      delete old;

      __type = t;

      if ( __cpf ) {
        delete __cpf;
        __cpf = 0;
      }

      GUM_ASSERT( __formulas->contains( __type->variable() ) );
      GUM_ASSERT( inst.end() && jnst.end() );
    }

  } /* namespace prm */
} /* namespace gum */
