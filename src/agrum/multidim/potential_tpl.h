/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
 * @brief Implementation of the Potential class.
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
 */
#include <cmath>


#include <agrum/config.h>
#include <agrum/multidim/potential.h>

namespace gum {

  // Default constructor: creates an empty null dimensional matrix
  // choose a MultiDimArray<> as decorated implementation
  template <typename GUM_SCALAR>
  INLINE Potential<GUM_SCALAR>::Potential()
      : MultiDimDecorator<GUM_SCALAR>( new MultiDimArray<GUM_SCALAR>(),
                                       GUM_SCALAR( 1 ) ) {
    GUM_CONSTRUCTOR( Potential );
  }

  // constructor using aContent as content
  template <typename GUM_SCALAR>
  INLINE
  Potential<GUM_SCALAR>::Potential( MultiDimImplementation<GUM_SCALAR>* aContent )
      : MultiDimDecorator<GUM_SCALAR>( aContent, GUM_SCALAR( 1 ) ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( Potential );
  }
  // copy constructor
  template <typename GUM_SCALAR>
  INLINE Potential<GUM_SCALAR>::Potential( const Potential<GUM_SCALAR>& src )
      : Potential<GUM_SCALAR>( static_cast<MultiDimImplementation<GUM_SCALAR>*>(
                                   src.content()->newFactory() ),
                               *( src.content() ) ) {
    this->_empty_value = src._empty_value;
    // todo how to optimize copy of content ?
    // GUM_CONS_CPY not here because in called Potential
    // GUM_CONS_CPY( Potential );
  }

  /// move constructor

  template <typename GUM_SCALAR>
  INLINE Potential<GUM_SCALAR>::Potential( Potential<GUM_SCALAR>&& from )
      : MultiDimDecorator<GUM_SCALAR>(
            std::forward<MultiDimDecorator<GUM_SCALAR>>( from ) ) {
    GUM_CONS_MOV( Potential );
  }

  // complex copy constructor : we choose the implementation
  template <typename GUM_SCALAR>
  Potential<GUM_SCALAR>::Potential( MultiDimImplementation<GUM_SCALAR>*  aContent,
                                    const MultiDimContainer<GUM_SCALAR>& src )
      : MultiDimDecorator<GUM_SCALAR>( aContent ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( Potential );

    if ( !src.empty() ) {
      this->beginMultipleChanges();

      for ( Idx i = 0; i < src.variablesSequence().size(); i++ ) {
        this->add( *( src.variablesSequence()[i] ) );
      }

      this->endMultipleChanges();
      this->content()->copyFrom( *src.content() );
    }
  }

  // operator = copy
  template <typename GUM_SCALAR>
  Potential<GUM_SCALAR>& Potential<GUM_SCALAR>::
  operator=( const Potential<GUM_SCALAR>& src ) {
    MultiDimDecorator<GUM_SCALAR>::operator=( src );
    GUM_OP_CPY( Potential );
    return *this;
  }

  // operator = move
  template <typename GUM_SCALAR>
  Potential<GUM_SCALAR>& Potential<GUM_SCALAR>::
  operator=( Potential<GUM_SCALAR>&& src ) {
    MultiDimDecorator<GUM_SCALAR>::operator=(
        std::forward<MultiDimDecorator<GUM_SCALAR>>( src ) );
    GUM_OP_MOV( Potential );
    return *this;
  }

  // destructor

  template <typename GUM_SCALAR>
  Potential<GUM_SCALAR>::~Potential() {
    // for debugging purposes
    GUM_DESTRUCTOR( Potential );
  }

  template <typename GUM_SCALAR>
  INLINE Potential<GUM_SCALAR>* Potential<GUM_SCALAR>::newFactory() const {
    return new Potential<GUM_SCALAR>(
        static_cast<MultiDimImplementation<GUM_SCALAR>*>(
            this->content()->newFactory() ) );
  }

  template <typename GUM_SCALAR>
  INLINE void Potential<GUM_SCALAR>::_swap( const DiscreteVariable* x,
                                            const DiscreteVariable* y ) {
    this->content()->swap( *x, *y );
  }

  // sum of all elements in this
  template <typename GUM_SCALAR>
  INLINE GUM_SCALAR Potential<GUM_SCALAR>::sum() const {
    if ( static_cast<MultiDimContainer<GUM_SCALAR>*>( this->_content )->empty() ) {
      return this->_empty_value;
    }
    return gum::projectSum( *this->content() );
  }
  // product of all elements in this
  template <typename GUM_SCALAR>
  INLINE GUM_SCALAR Potential<GUM_SCALAR>::product() const {
    if ( static_cast<MultiDimContainer<GUM_SCALAR>*>( this->_content )->empty() ) {
      return this->_empty_value;
    }
    return gum::projectProduct( *this->content() );
  }
  // max of all elements in this
  template <typename GUM_SCALAR>
  INLINE GUM_SCALAR Potential<GUM_SCALAR>::max() const {
    if ( static_cast<MultiDimContainer<GUM_SCALAR>*>( this->_content )->empty() ) {
      return this->_empty_value;
    }
    return gum::projectMax( *this->content() );
  }
  // min of all elements in this
  template <typename GUM_SCALAR>
  INLINE GUM_SCALAR Potential<GUM_SCALAR>::min() const {
    if ( static_cast<MultiDimContainer<GUM_SCALAR>*>( this->_content )->empty() ) {
      return this->_empty_value;
    }
    return gum::projectMin( *this->content() );
  }

  // max of all non one elements in this
  template <typename GUM_SCALAR>
  GUM_SCALAR Potential<GUM_SCALAR>::maxNonOne() const {
    GUM_SCALAR res;

    if ( static_cast<MultiDimContainer<GUM_SCALAR>*>( this->_content )->empty() ) {
      res = this->_empty_value;
    } else {
      res = this->reduce(
          []( GUM_SCALAR z, GUM_SCALAR p ) {
            return ( p == static_cast<GUM_SCALAR>( 1 ) )
                       ? z
                       : ( z == static_cast<GUM_SCALAR>( 1 ) ) ? p
                                                               : ( p > z ? p : z );
          },
          static_cast<GUM_SCALAR>( 1 ) );
    }

    if ( res == static_cast<GUM_SCALAR>( 1 ) )
      GUM_ERROR( NotFound, "No other value than 1" );

    return res;
  }

  // min of all non zero elements in this
  template <typename GUM_SCALAR>
  INLINE GUM_SCALAR Potential<GUM_SCALAR>::minNonZero() const {

    GUM_SCALAR res;

    if ( static_cast<MultiDimContainer<GUM_SCALAR>*>( this->_content )->empty() ) {
      res = this->_empty_value;
    } else {
      res = this->reduce(
          []( GUM_SCALAR z, GUM_SCALAR p ) {
            return ( p == static_cast<GUM_SCALAR>( 0 ) )
                       ? z
                       : ( z == static_cast<GUM_SCALAR>( 0 ) ) ? p
                                                               : ( p < z ? p : z );
          },
          static_cast<GUM_SCALAR>( 0 ) );
    }

    if ( res == static_cast<GUM_SCALAR>( 0 ) )
      GUM_ERROR( NotFound, "No other value than 1" );

    return res;
  }

  // entropy of this
  template <typename GUM_SCALAR>
  INLINE GUM_SCALAR Potential<GUM_SCALAR>::entropy() const {
    if ( static_cast<MultiDimContainer<GUM_SCALAR>*>( this->_content )->empty() ) {
      return static_cast<GUM_SCALAR>( 0 );
    }

    return this->reduce(
        []( GUM_SCALAR z, GUM_SCALAR p ) {
          return ( p == 0.0 ) ? z : ( z - p * log2( p ) );
        },
        0.0 );
  }

  template <typename GUM_SCALAR>
  INLINE const Potential<GUM_SCALAR>&
  Potential<GUM_SCALAR>::fillWith( const std::vector<GUM_SCALAR>& v ) const {
    this->populate( v );
    return *this;
  }


  template <typename GUM_SCALAR>
  INLINE const Potential<GUM_SCALAR>&
  Potential<GUM_SCALAR>::fillWith( std::initializer_list<GUM_SCALAR> list ) const {
    this->populate( list );
    return *this;
  }


  template <typename GUM_SCALAR>
  INLINE const Potential<GUM_SCALAR>&
  Potential<GUM_SCALAR>::fillWith( const GUM_SCALAR& v ) const {
    this->fill( v );
    return *this;
  }


  template <typename GUM_SCALAR>
  INLINE const Potential<GUM_SCALAR>& Potential<GUM_SCALAR>::sq() const {
    this->apply( []( GUM_SCALAR x ) { return x * x; } );
    return *this;
  }

  template <typename GUM_SCALAR>
  INLINE GUM_SCALAR
  Potential<GUM_SCALAR>::KL( const Potential<GUM_SCALAR>& p ) const {
    if ( this->nbrDim() != p.nbrDim() )
      GUM_ERROR( InvalidArgument,
                 "KL between potentials with different numbers of dimensions" );
    for ( const auto var : p.variablesSequence() ) {
      if ( !this->contains( *var ) )
        GUM_ERROR(
            InvalidArgument,
            "A variable in the argument does not belong to the potential." );
    }
    for ( const auto var : this->variablesSequence() ) {
      if ( !p.contains( *var ) )
        GUM_ERROR( InvalidArgument,
                   "A variable does not belong to the argument." );
    }
    return fastKL( p );
  }

  template <typename GUM_SCALAR>
  INLINE GUM_SCALAR
  Potential<GUM_SCALAR>::fastKL( const Potential<GUM_SCALAR>& p ) const {
    Instantiation inst( *this );
    Instantiation instP( p );
    GUM_SCALAR    res = static_cast<GUM_SCALAR>( 0 );
    for ( inst.setFirst(), instP.setFirst(); !inst.end();
          inst.inc(), instP.inc() ) {
      GUM_SCALAR x = this->get( inst );
      GUM_SCALAR y = p.get( instP );
      if ( static_cast<GUM_SCALAR>( 0 ) == x ) {
        if ( static_cast<GUM_SCALAR>( 0 ) == y ) {
          continue;  // we add 0 to res
        }
        GUM_ERROR( FatalError,
                   "The Potential has a 0 while the argument has not." )
      }

      if ( static_cast<GUM_SCALAR>( 0 ) == y ) {
        // we know that x!=0;
        GUM_ERROR( FatalError,
                   "The argument has a 0 while the potential has not." )
      }
      res += x * log2( x / y );
    }
    return res;
  }

  template <typename GUM_SCALAR>
  INLINE const Potential<GUM_SCALAR>& Potential<GUM_SCALAR>::abs() const {
    this->apply( []( GUM_SCALAR x ) {
      if ( x >= 0 )
        return x;
      else
        return -x;
    } );
    return *this;
  }

  // normalisation of this
  // do nothing is sum is 0
  template <typename GUM_SCALAR>
  INLINE const Potential<GUM_SCALAR>& Potential<GUM_SCALAR>::normalize() const {

    if ( static_cast<MultiDimContainer<GUM_SCALAR>*>( this->_content )->empty() ) {
      if ( this->_empty_value != static_cast<GUM_SCALAR>( 0 ) )
        this->_empty_value = static_cast<GUM_SCALAR>( 1.0 );
    } else {
      GUM_SCALAR s = sum();

      if ( s != (GUM_SCALAR)0 ) {
        this->apply( [s]( GUM_SCALAR x ) { return x / s; } );
      }
    }
    return *this;
  }

  template <typename GUM_SCALAR>
  INLINE void Potential<GUM_SCALAR>::normalizeAsCPT() const {
    if ( static_cast<MultiDimContainer<GUM_SCALAR>*>( this->_content )->empty() ) {
      if ( this->_empty_value != static_cast<GUM_SCALAR>( 0 ) ) {
        this->_empty_value = static_cast<GUM_SCALAR>( 1.0 );
      } else {
        GUM_ERROR( FatalError,
                   "Normalization for a potential that sum to 0 in " << *this );
      }
    } else {
      Instantiation inst( *this );
      const auto&   v = this->variable( 0 );

      for ( inst.setFirst(); !inst.end(); inst.incNotVar( v ) ) {
        GUM_SCALAR s = (GUM_SCALAR)0.0;
        for ( inst.setFirstVar( v ); !inst.end(); inst.incVar( v ) )
          s += this->get( inst );
        if ( s == (GUM_SCALAR)0.0 ) {
          GUM_ERROR( FatalError,
                     "Normalization for a potential that sum to 0 in " << *this );
        }
        if ( s != (GUM_SCALAR)1.0 ) {
          for ( inst.setFirstVar( v ); !inst.end(); inst.incVar( v ) )
            this->set( inst, this->get( inst ) / s );
        }
        inst.setFirstVar( v );  // to remove inst.end()
      }
    }
  }

  template <typename GUM_SCALAR>
  INLINE const Potential<GUM_SCALAR>&
  Potential<GUM_SCALAR>::scale( GUM_SCALAR v ) const {
    this->apply( [v]( GUM_SCALAR x ) { return x * v; } );
    return *this;
  }

  template <typename GUM_SCALAR>
  INLINE const Potential<GUM_SCALAR>&
  Potential<GUM_SCALAR>::translate( GUM_SCALAR v ) const {
    this->apply( [v]( GUM_SCALAR x ) { return x + v; } );
    return *this;
  }


  template <typename GUM_SCALAR>
  INLINE Potential<GUM_SCALAR> Potential<GUM_SCALAR>::margSumOut(
      const Set<const DiscreteVariable*>& del_vars ) const {
    if ( static_cast<MultiDimContainer<GUM_SCALAR>*>( this->_content )->empty() ) {
      return Potential<GUM_SCALAR>().fillWith( this->_empty_value );
    }
    return Potential<GUM_SCALAR>( gum::projectSum( *this->content(), del_vars ) );
  }

  template <typename GUM_SCALAR>
  INLINE Potential<GUM_SCALAR> Potential<GUM_SCALAR>::margProdOut(
      const Set<const DiscreteVariable*>& del_vars ) const {
    if ( static_cast<MultiDimContainer<GUM_SCALAR>*>( this->_content )->empty() ) {
      return Potential<GUM_SCALAR>().fillWith( this->_empty_value );
    }
    return Potential<GUM_SCALAR>(
        gum::projectProduct( *this->content(), del_vars ) );
  }

  template <typename GUM_SCALAR>
  INLINE Potential<GUM_SCALAR> Potential<GUM_SCALAR>::margMinOut(
      const Set<const DiscreteVariable*>& del_vars ) const {
    if ( static_cast<MultiDimContainer<GUM_SCALAR>*>( this->_content )->empty() ) {
      return Potential<GUM_SCALAR>().fillWith( this->_empty_value );
    }
    return Potential<GUM_SCALAR>( gum::projectMin( *this->content(), del_vars ) );
  }

  template <typename GUM_SCALAR>
  INLINE Potential<GUM_SCALAR> Potential<GUM_SCALAR>::margMaxOut(
      const Set<const DiscreteVariable*>& del_vars ) const {
    if ( static_cast<MultiDimContainer<GUM_SCALAR>*>( this->_content )->empty() ) {
      return Potential<GUM_SCALAR>().fillWith( this->_empty_value );
    }
    return Potential<GUM_SCALAR>( gum::projectMax( *this->content(), del_vars ) );
  }
  template <typename GUM_SCALAR>
  INLINE Potential<GUM_SCALAR> Potential<GUM_SCALAR>::margSumIn(
      const Set<const DiscreteVariable*>& kept_vars ) const {
    if ( static_cast<MultiDimContainer<GUM_SCALAR>*>( this->_content )->empty() ) {
      return Potential<GUM_SCALAR>().fillWith( this->_empty_value );
    }
    return Potential<GUM_SCALAR>(
        gum::projectSum( *this->content(), _complementVars( kept_vars ) ) );
  }

  template <typename GUM_SCALAR>
  INLINE Potential<GUM_SCALAR> Potential<GUM_SCALAR>::margProdIn(
      const Set<const DiscreteVariable*>& kept_vars ) const {
    if ( static_cast<MultiDimContainer<GUM_SCALAR>*>( this->_content )->empty() ) {
      return Potential<GUM_SCALAR>().fillWith( this->_empty_value );
    }
    return Potential<GUM_SCALAR>(
        gum::projectProduct( *this->content(), _complementVars( kept_vars ) ) );
  }

  template <typename GUM_SCALAR>
  INLINE Potential<GUM_SCALAR> Potential<GUM_SCALAR>::margMinIn(
      const Set<const DiscreteVariable*>& kept_vars ) const {
    if ( static_cast<MultiDimContainer<GUM_SCALAR>*>( this->_content )->empty() ) {
      return Potential<GUM_SCALAR>().fillWith( this->_empty_value );
    }
    return Potential<GUM_SCALAR>(
        gum::projectMin( *this->content(), _complementVars( kept_vars ) ) );
  }

  template <typename GUM_SCALAR>
  INLINE Potential<GUM_SCALAR> Potential<GUM_SCALAR>::margMaxIn(
      const Set<const DiscreteVariable*>& kept_vars ) const {
    if ( static_cast<MultiDimContainer<GUM_SCALAR>*>( this->_content )->empty() ) {
      return Potential<GUM_SCALAR>().fillWith( this->_empty_value );
    }
    return Potential<GUM_SCALAR>(
        gum::projectMax( *this->content(), _complementVars( kept_vars ) ) );
  }

  template <typename GUM_SCALAR>
  INLINE Potential<GUM_SCALAR> Potential<GUM_SCALAR>::isNonZeroMap() const {
    auto p = Potential<GUM_SCALAR>( *this );
    p.apply( []( GUM_SCALAR x ) {
      if ( x != static_cast<GUM_SCALAR>( 0 ) )
        return static_cast<GUM_SCALAR>( 1 );
      else
        return static_cast<GUM_SCALAR>( 0 );
    } );
    return p;
  }

  template <typename GUM_SCALAR>
  Set<const DiscreteVariable*> Potential<GUM_SCALAR>::_complementVars(
      const Set<const DiscreteVariable*>& vars ) const {
    Set<const DiscreteVariable*> cplt;

    for ( auto x : this->variablesSequence() )
      if ( !vars.contains( x ) ) cplt.insert( x );

    return cplt;
  }

  template <typename GUM_SCALAR>
  Potential<GUM_SCALAR> Potential<GUM_SCALAR>::reorganize(
      const std::vector<const DiscreteVariable*>& vars ) const {
    if ( vars.size() != this->nbrDim() )
      GUM_ERROR( InvalidArgument,
                 "The vector contains " << vars.size() << " variables instead of "
                                        << this->nbrDim()
                                        << "." );
    for ( const auto var : vars ) {
      if ( !this->contains( *var ) )
        GUM_ERROR( InvalidArgument,
                   "A variable in the vector does not belong to the potential." );
    }

    Potential<GUM_SCALAR> p;
    p.beginMultipleChanges();
    for ( auto var : vars )
      p.add( *var );
    p.endMultipleChanges();
    p.copyFrom( *this, nullptr );  // copy *this in p using the same order

    return p;
  }

  template <typename GUM_SCALAR>
  Potential<GUM_SCALAR>
  Potential<GUM_SCALAR>::putFirst( const DiscreteVariable* var ) const {
    if ( !this->contains( *var ) ) {
      GUM_ERROR( InvalidArgument,
                 "The variable to put first does not belong to the potential" );
    }

    std::vector<const DiscreteVariable*> vars;
    vars.push_back( var );
    for ( Idx i = 0; i < this->nbrDim(); i++ )
      if ( &( this->variable( i ) ) != var )
        vars.push_back( &( this->variable( i ) ) );

    return this->reorganize( vars );
  }

  template <typename GUM_SCALAR>
  Potential<GUM_SCALAR>
  Potential<GUM_SCALAR>::extract( const Instantiation& inst ) const {
    Potential<GUM_SCALAR> p;
    p.extractFrom( *this, inst );

    return p;
  }

  template <typename GUM_SCALAR>
  std::ostream& operator<<( std::ostream&                out,
                            const Potential<GUM_SCALAR>& array ) {
    out << array.toString();
    return out;
  }

} /* namespace gum */
