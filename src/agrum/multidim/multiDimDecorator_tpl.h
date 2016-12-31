/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr   *
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
 * @brief Implementation for MultiDimDecorator.
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
 */

// include the operators that will be used by the decorators
#include <agrum/multidim/operators/completeProjections4MultiDim.h>
#include <agrum/multidim/operators/operators4MultiDim.h>
#include <agrum/multidim/operators/projections4MultiDim.h>
#include <agrum/multidim/partialInstantiation4MultiDim.h>

#include <agrum/multidim/multiDimDecorator.h>

namespace gum {
  // instrumental and non-API function
  template <typename GUM_SCALAR>
  static void ___initPotentialOperators() {
    static bool first = true;

    if ( first ) {
      first = false;

      // register the operators that will be used by the decorator
      Operators4MultiDimInitialize<GUM_SCALAR> op;
      op.init();

      // register the projectors that will be used by the decorator
      Projections4MultiDimInitialize<GUM_SCALAR> proj;
      proj.init();

      // register the projectors that will be used by the decorator
      CompleteProjections4MultiDimInitialize<GUM_SCALAR> comp_proj;
      comp_proj.init();

      // register the partial instantiators that will be used by the decorator
      PartialInstantiation4MultiDimInitialize<GUM_SCALAR> inst;
      inst.init();
    }
  }
  // constructors

  template <typename GUM_SCALAR>
  INLINE MultiDimDecorator<GUM_SCALAR>::MultiDimDecorator(
      MultiDimImplementation<GUM_SCALAR>* aContent, GUM_SCALAR empty_value )
      : _content( aContent )
      , _empty_value( empty_value ) {
    ___initPotentialOperators<GUM_SCALAR>();
    GUM_CONSTRUCTOR( MultiDimDecorator );
  }

  template <typename GUM_SCALAR>
  INLINE MultiDimDecorator<GUM_SCALAR>::MultiDimDecorator(
      const MultiDimDecorator<GUM_SCALAR>& from )
      : MultiDimContainer<GUM_SCALAR>( from ) {
    GUM_CONS_CPY( MultiDimDecorator );
    ___initPotentialOperators<GUM_SCALAR>();
    _empty_value = from._empty_value;
    content()->copy( from.content() );
  }


  template <typename GUM_SCALAR>
  INLINE MultiDimDecorator<GUM_SCALAR>& MultiDimDecorator<GUM_SCALAR>::
  operator=( MultiDimDecorator<GUM_SCALAR>&& from ) {
    MultiDimContainer<GUM_SCALAR>::operator=(
        std::forward<MultiDimContainer<GUM_SCALAR>>( from ) );
    GUM_OP_MOV( MultiDimDecorator );

    if ( this != &from ) {
      if ( _content != nullptr ) delete ( _content );  // should be the case
      _empty_value = from._empty_value;
      _content = from._content;
      from._content = nullptr;
    }

    return *this;
  }


  template <typename GUM_SCALAR>
  INLINE MultiDimDecorator<GUM_SCALAR>::MultiDimDecorator(
      MultiDimDecorator<GUM_SCALAR>&& from )
      : MultiDimContainer<GUM_SCALAR>(
            std::forward<MultiDimContainer<GUM_SCALAR>>( from ) ) {
    GUM_CONS_MOV( MultiDimDecorator );

    _empty_value = from._empty_value;
    _content = from._content;
    from._content = nullptr;
  }


  template <typename GUM_SCALAR>
  INLINE MultiDimDecorator<GUM_SCALAR>& MultiDimDecorator<GUM_SCALAR>::
  operator=( const MultiDimDecorator<GUM_SCALAR>& from ) {
    GUM_OP_CPY( MultiDimDecorator );
    ___initPotentialOperators<GUM_SCALAR>();
    MultiDimContainer<GUM_SCALAR>::operator=( from );
    _empty_value = from._empty_value;
    MultiDimDecorator<GUM_SCALAR>::content()->copy( *from.content() );
    return *this;
  }


  // destructor

  template <typename GUM_SCALAR>
  INLINE MultiDimDecorator<GUM_SCALAR>::~MultiDimDecorator() {
    if ( _content != nullptr ) {
      delete ( _content );
    }

    GUM_DESTRUCTOR( MultiDimDecorator );
  }

  // return a data, given a Instantiation - final method

  template <typename GUM_SCALAR>
  INLINE GUM_SCALAR&
  MultiDimDecorator<GUM_SCALAR>::_get( const Instantiation& i ) const {
    GUM_ERROR( OperationNotAllowed, "_get in the implementation !" );
  }

  template <typename GUM_SCALAR>
  INLINE GUM_SCALAR
  MultiDimDecorator<GUM_SCALAR>::get( const Instantiation& i ) const {
    if ( static_cast<MultiDimContainer<GUM_SCALAR>*>( _content )->empty() ) {
      return _empty_value;
    } else {
      return static_cast<MultiDimContainer<GUM_SCALAR>*>( _content )->get( i );
    }
  }

  template <typename GUM_SCALAR>
  INLINE void MultiDimDecorator<GUM_SCALAR>::set( const Instantiation& i,
                                                  const GUM_SCALAR& value ) const {
    if ( static_cast<MultiDimContainer<GUM_SCALAR>*>( _content )->nbrDim() == 0 ) {
      _empty_value = value;
    } else {
      static_cast<MultiDimContainer<GUM_SCALAR>*>( _content )->set( i, value );
    }
  }

  // get the size of domains - final method

  template <typename GUM_SCALAR>
  INLINE Size MultiDimDecorator<GUM_SCALAR>::domainSize() const {
    return static_cast<MultiDimContainer<GUM_SCALAR>*>( _content )->domainSize();
  }

  // add a new var to the sequence of vars - final method

  template <typename GUM_SCALAR>
  INLINE void MultiDimDecorator<GUM_SCALAR>::add( const DiscreteVariable& v ) {
    static_cast<MultiDimContainer<GUM_SCALAR>*>( _content )->add( v );
  }

  // listen to change in each recorded Instantiation. final method

  template <typename GUM_SCALAR>
  INLINE void MultiDimDecorator<GUM_SCALAR>::changeNotification(
      Instantiation&                i,
      const DiscreteVariable* const var,
      const Idx&                    oldval,
      const Idx&                    newval ) {
    static_cast<MultiDimContainer<GUM_SCALAR>*>( _content )
        ->changeNotification( i, var, oldval, newval );
  }

  // listen to an assignment of a value in a Instantiation

  template <typename GUM_SCALAR>
  INLINE void
  MultiDimDecorator<GUM_SCALAR>::setChangeNotification( Instantiation& i ) {
    static_cast<MultiDimContainer<GUM_SCALAR>*>( _content )
        ->setChangeNotification( i );
  }

  // listen to setFirst in each recorded Instantiation. final method.

  template <typename GUM_SCALAR>
  INLINE void
  MultiDimDecorator<GUM_SCALAR>::setFirstNotification( Instantiation& i ) {
    static_cast<MultiDimContainer<GUM_SCALAR>*>( _content )
        ->setFirstNotification( i );
  }

  // listen to setLast in each recorded Instantiation. final method.

  template <typename GUM_SCALAR>
  INLINE void
  MultiDimDecorator<GUM_SCALAR>::setLastNotification( Instantiation& i ) {
    static_cast<MultiDimContainer<GUM_SCALAR>*>( _content )
        ->setLastNotification( i );
  }

  // listen to increment in each recorded Instantiation. final method.

  template <typename GUM_SCALAR>
  INLINE void
  MultiDimDecorator<GUM_SCALAR>::setIncNotification( Instantiation& i ) {
    static_cast<MultiDimContainer<GUM_SCALAR>*>( _content )
        ->setIncNotification( i );
  }

  // listen to increment in each recorded Instantiation. final method.

  template <typename GUM_SCALAR>
  INLINE void
  MultiDimDecorator<GUM_SCALAR>::setDecNotification( Instantiation& i ) {
    static_cast<MultiDimContainer<GUM_SCALAR>*>( _content )
        ->setDecNotification( i );
  }

  // add a Instantiation as a slave of this

  template <typename GUM_SCALAR>
  INLINE bool MultiDimDecorator<GUM_SCALAR>::registerSlave( Instantiation& i ) {
    return static_cast<MultiDimContainer<GUM_SCALAR>*>( _content )
        ->registerSlave( i );
  }

  template <typename GUM_SCALAR>
  INLINE void MultiDimDecorator<GUM_SCALAR>::erase( const DiscreteVariable& d ) {
    static_cast<MultiDimContainer<GUM_SCALAR>*>( _content )->erase( d );
  }

  template <typename GUM_SCALAR>
  INLINE const DiscreteVariable&
  MultiDimDecorator<GUM_SCALAR>::variable( Idx i ) const {
    return static_cast<MultiDimContainer<GUM_SCALAR>*>( _content )->variable( i );
  }

  template <typename GUM_SCALAR>
  INLINE Idx
  MultiDimDecorator<GUM_SCALAR>::pos( const DiscreteVariable& d ) const {
    return static_cast<MultiDimContainer<GUM_SCALAR>*>( _content )->pos( d );
  }

  template <typename GUM_SCALAR>
  INLINE bool
  MultiDimDecorator<GUM_SCALAR>::contains( const DiscreteVariable& d ) const {
    return static_cast<MultiDimContainer<GUM_SCALAR>*>( _content )->contains( d );
  }

  template <typename GUM_SCALAR>
  INLINE bool MultiDimDecorator<GUM_SCALAR>::empty() const {
    if ( _content == nullptr ) return true;
    return static_cast<MultiDimContainer<GUM_SCALAR>*>( _content )->empty();
  }

  template <typename GUM_SCALAR>
  INLINE bool MultiDimDecorator<GUM_SCALAR>::unregisterSlave( Instantiation& i ) {
    return static_cast<MultiDimContainer<GUM_SCALAR>*>( _content )
        ->unregisterSlave( i );
  }

  template <typename GUM_SCALAR>
  INLINE void MultiDimDecorator<GUM_SCALAR>::fill( const GUM_SCALAR& d ) const {
    if ( static_cast<MultiDimContainer<GUM_SCALAR>*>( _content )->empty() ) {
      _empty_value = d;
    } else {
      static_cast<MultiDimContainer<GUM_SCALAR>*>( _content )->fill( d );
    }
  }

  // notification modification on vars to all Instantiation listeners.

  template <typename GUM_SCALAR>
  INLINE void MultiDimDecorator<GUM_SCALAR>::notifyChange() const {
    /*( (MultiDimContainer<GUM_SCALAR> *) _content)->notifyChange();*/
    GUM_ERROR( OperationNotAllowed, "Not implemented yet" );
  }

  // give a const ref to the sequence of DiscreteVariable*. final method.

  template <typename GUM_SCALAR>
  INLINE const Sequence<const DiscreteVariable*>&
               MultiDimDecorator<GUM_SCALAR>::variablesSequence() const {
    return static_cast<MultiDimContainer<GUM_SCALAR>*>( _content )
        ->variablesSequence();
  }

  // get the nbr of vars in the sequence. final method

  template <typename GUM_SCALAR>
  INLINE Idx MultiDimDecorator<GUM_SCALAR>::nbrDim() const {
    return static_cast<MultiDimContainer<GUM_SCALAR>*>( _content )->nbrDim();
  }

  template <typename GUM_SCALAR>
  void MultiDimDecorator<GUM_SCALAR>::populate(
      const std::vector<GUM_SCALAR>& v ) const {
    if ( static_cast<MultiDimContainer<GUM_SCALAR>*>( _content )->empty() ) {
      if ( v.size() == 1 ) {
        _empty_value = v[0];
      } else {
        GUM_ERROR( SizeError, "Size do not match in populate" )
      }
    } else {
      _content->populate( v );
    }
  }

  template <typename GUM_SCALAR>
  void MultiDimDecorator<GUM_SCALAR>::populate(
      std::initializer_list<GUM_SCALAR> list ) const {
    if ( static_cast<MultiDimContainer<GUM_SCALAR>*>( _content )->empty() ) {
      if ( list.size() == 1 ) {
        _empty_value = *list.begin();
      } else {
        GUM_ERROR( SizeError, "Size do not match in populate" )
      }
    } else {
      _content->populate( list );
    }
  }

  template <typename GUM_SCALAR>
  void MultiDimDecorator<GUM_SCALAR>::apply(
      std::function<GUM_SCALAR( GUM_SCALAR )> f ) const {
    if ( static_cast<MultiDimContainer<GUM_SCALAR>*>( _content )->empty() ) {
      _empty_value = f( _empty_value );
    } else {
      _content->apply( f );
    }
  }

  template <typename GUM_SCALAR>
  GUM_SCALAR MultiDimDecorator<GUM_SCALAR>::reduce(
      std::function<GUM_SCALAR( GUM_SCALAR, GUM_SCALAR )> f,
      GUM_SCALAR base ) const {
    if ( static_cast<MultiDimContainer<GUM_SCALAR>*>( _content )->empty() ) {
      return base;
    } else {
      return _content->reduce( f, base );
    }
  }

  // protected access to _content
  template <typename GUM_SCALAR>
  INLINE MultiDimImplementation<GUM_SCALAR>*
         MultiDimDecorator<GUM_SCALAR>::content() {
    return _content;
  }

  // protected access to _content
  template <typename GUM_SCALAR>
  INLINE const MultiDimImplementation<GUM_SCALAR>*
               MultiDimDecorator<GUM_SCALAR>::content() const {
    return _content;
  }

  template <typename GUM_SCALAR>
  INLINE void MultiDimDecorator<GUM_SCALAR>::beginMultipleChanges( void ) {
    static_cast<MultiDimContainer<GUM_SCALAR>*>( _content )
        ->beginMultipleChanges();
  }

  template <typename GUM_SCALAR>
  INLINE void MultiDimDecorator<GUM_SCALAR>::endMultipleChanges( void ) {
    static_cast<MultiDimContainer<GUM_SCALAR>*>( _content )->endMultipleChanges();
  }

  template <typename GUM_SCALAR>
  INLINE void
  MultiDimDecorator<GUM_SCALAR>::endMultipleChanges( const GUM_SCALAR& x ) {
    static_cast<MultiDimContainer<GUM_SCALAR>*>( _content )
        ->endMultipleChanges( x );
  }

  template <typename GUM_SCALAR>
  INLINE void MultiDimDecorator<GUM_SCALAR>::_swapContent(
      MultiDimImplementation<GUM_SCALAR>* aContent ) const {
    if ( aContent != nullptr ) {
      // TODO : frees all slave instantiations
      // TODO : control the dimensions ?
      MultiDimImplementation<GUM_SCALAR>* tmp = _content;
      _content = aContent;
      // registers all instantiations
      delete ( tmp );
    }
  }


  // string representation of internal data about i in this.
  template <typename GUM_SCALAR>
  INLINE const std::string
  MultiDimDecorator<GUM_SCALAR>::toString( const Instantiation* i ) const {
    return _content->toString( i );
  }

  template <typename GUM_SCALAR>
  INLINE const std::string MultiDimDecorator<GUM_SCALAR>::toString() const {
    if ( static_cast<MultiDimContainer<GUM_SCALAR>*>( _content )->empty() ) {
      std::stringstream ss;
      ss << "<> :: " << _empty_value;
      return ss.str();
    } else {
      return _content->toString();
    }
  }

  //@todo force GUM_SCALAR to be double-castable (to be able to use fabs,etc.)
} /* namespace gum */
