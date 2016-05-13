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
 * @brief Inline implementation of gum::Aggregate
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */
#include <agrum/PRM/elements/aggregate.h>
#include <agrum/PRM/elements/scalarAttribute.h>
#include <agrum/PRM/elements/type.h>

#include <agrum/multidim/aggregators/min.h>
#include <agrum/multidim/aggregators/max.h>
#include <agrum/multidim/aggregators/exists.h>
#include <agrum/multidim/aggregators/forall.h>
#include <agrum/multidim/aggregators/count.h>
#include <agrum/multidim/aggregators/or.h>
#include <agrum/multidim/aggregators/and.h>
#include <agrum/multidim/aggregators/amplitude.h>
#include <agrum/multidim/aggregators/median.h>

namespace gum {
  namespace prm {

    template <typename GUM_SCALAR>
    Aggregate<GUM_SCALAR>::Aggregate( const std::string& name,
                                      AggregateType aggType,
                                      const Type<GUM_SCALAR>& rvType )
        : ClassElement<GUM_SCALAR>( name )
        , __agg_type( aggType )
        , __type( new Type<GUM_SCALAR>( rvType ) )
        , __label( nullptr ) {
      GUM_CONSTRUCTOR( Aggregate );
      this->_safeName = PRMObject::LEFT_CAST() + __type->name() +
                        PRMObject::RIGHT_CAST() + name;
    }

    template <typename GUM_SCALAR>
    Aggregate<GUM_SCALAR>::Aggregate( const std::string& name,
                                      AggregateType aggType,
                                      const Type<GUM_SCALAR>& rvType,
                                      Idx label )
        : ClassElement<GUM_SCALAR>( name )
        , __agg_type( aggType )
        , __type( new Type<GUM_SCALAR>( rvType ) )
        , __label( new Idx( label ) ) {
      GUM_CONSTRUCTOR( Aggregate );
      this->_safeName = PRMObject::LEFT_CAST() + __type->name() +
                        PRMObject::RIGHT_CAST() + name;
    }

    template <typename GUM_SCALAR>
    Aggregate<GUM_SCALAR>::~Aggregate() {
      GUM_DESTRUCTOR( Aggregate );
      delete __type;
      if ( __label ) delete __label;
    }

    template <typename GUM_SCALAR>
    Aggregate<GUM_SCALAR>::Aggregate( const Aggregate<GUM_SCALAR>& source )
        : ClassElement<GUM_SCALAR>( source )
        , __agg_type( source.__agg_type )
        , __type( new Type<GUM_SCALAR>( source.type() ) )
        , __label( source.__label ) {
      GUM_CONS_CPY( Aggregate );
      GUM_ERROR( FatalError,
                 "illegal call to gum::Aggregate copy constructor." );
    }

    template <typename GUM_SCALAR>
    Aggregate<GUM_SCALAR>& Aggregate<GUM_SCALAR>::
    operator=( const Aggregate<GUM_SCALAR>& source ) {
      GUM_ERROR( FatalError, "illegal call to gum::Aggregate copy operator." );
    }

    template <typename GUM_SCALAR>
    INLINE typename ClassElement<GUM_SCALAR>::ClassElementType
    Aggregate<GUM_SCALAR>::elt_type() const {
      return this->prm_aggregate;
    }

    template <typename GUM_SCALAR>
    INLINE typename Aggregate<GUM_SCALAR>::AggregateType
    Aggregate<GUM_SCALAR>::agg_type() const {
      return __agg_type;
    }

    template <typename GUM_SCALAR>
    INLINE Idx Aggregate<GUM_SCALAR>::label() const {
      if ( __label ) return *__label;
      GUM_ERROR( OperationNotAllowed, "no label defined for this aggregate" );
    }

    template <typename GUM_SCALAR>
    INLINE const std::string& Aggregate<GUM_SCALAR>::labelValue() const {
      return __label_value;
    }

    template <typename GUM_SCALAR>
    INLINE void Aggregate<GUM_SCALAR>::setLabel( Idx idx ) {
      if ( __label ) {
        ( *__label ) = idx;
      } else {
        __label = new Idx( idx );
      }
    }

    template <typename GUM_SCALAR>
    INLINE void Aggregate<GUM_SCALAR>::setLabel( const std::string& value ) {
      __label_value = value;
    }

    template <typename GUM_SCALAR>
    INLINE bool Aggregate<GUM_SCALAR>::hasLabel() const {
      return __label != nullptr;
    }

    template <typename GUM_SCALAR>
    INLINE Type<GUM_SCALAR>& Aggregate<GUM_SCALAR>::type() {
      return *__type;
    }

    template <typename GUM_SCALAR>
    INLINE const Type<GUM_SCALAR>& Aggregate<GUM_SCALAR>::type() const {
      return *__type;
    }

    template <typename GUM_SCALAR>
    INLINE Potential<GUM_SCALAR>& Aggregate<GUM_SCALAR>::cpf() {
      GUM_ERROR( OperationNotAllowed, "This is an aggregate." );
    }

    template <typename GUM_SCALAR>
    INLINE const Potential<GUM_SCALAR>& Aggregate<GUM_SCALAR>::cpf() const {
      GUM_ERROR( OperationNotAllowed, "This is an aggregate." );
    }

    template <typename GUM_SCALAR>
    INLINE MultiDimImplementation<GUM_SCALAR>*
    Aggregate<GUM_SCALAR>::buildImpl() const {
      switch ( agg_type() ) {
        case AggregateType::MIN:
          return new aggregator::Min<GUM_SCALAR>();
        case AggregateType::MAX:
          return new aggregator::Max<GUM_SCALAR>();
        case AggregateType::OR:
          return new aggregator::Or<GUM_SCALAR>();
        case AggregateType::AND:
          return new aggregator::And<GUM_SCALAR>();
        case AggregateType::AMPLITUDE:
          return new aggregator::Amplitude<GUM_SCALAR>();
        case AggregateType::MEDIAN:
          return new aggregator::Median<GUM_SCALAR>();
        case AggregateType::EXISTS:
          return new aggregator::Exists<GUM_SCALAR>( label() );
        case AggregateType::FORALL:
          return new aggregator::Forall<GUM_SCALAR>( label() );
        case AggregateType::COUNT:
          return new aggregator::Count<GUM_SCALAR>( label() );
        default:
          GUM_ERROR( OperationNotAllowed, "Unknown aggregator." );
      }
      return nullptr;
    }

    // See gum::ClassElement<GUM_SCALAR>::_addParent().
    template <typename GUM_SCALAR>
    INLINE void
    Aggregate<GUM_SCALAR>::addParent( const ClassElement<GUM_SCALAR>& elt ) {}

    // See gum::ClassElement<GUM_SCALAR>::_addChild().
    template <typename GUM_SCALAR>
    INLINE void
    Aggregate<GUM_SCALAR>::addChild( const ClassElement<GUM_SCALAR>& elt ) {}

    template <typename GUM_SCALAR>
    Attribute<GUM_SCALAR>* Aggregate<GUM_SCALAR>::getCastDescendant() const {
      ScalarAttribute<GUM_SCALAR>* cast = 0;

      try {
        cast = new ScalarAttribute<GUM_SCALAR>( this->name(), type().super() );
      } catch ( NotFound& ) {
        GUM_ERROR( OperationNotAllowed,
                   "this Aggregate can not have cast descendant" );
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

  } /* namespace prm */
} /* namespace gum */
