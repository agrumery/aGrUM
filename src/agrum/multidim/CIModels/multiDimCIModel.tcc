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
/** @file
 * @brief ANoisy-OR as described by Henrion (UAI-3, 1989, pp161-173)
 *
* @author Pierre-Henri WUILLEMIN et Christophe GONZALES <{prenom.nom}_at_lip6.fr>
 */
#include<agrum/multidim/CIModels/multiDimCIModel.h>

namespace gum {
  // ==============================================================================
  /// Default constructor
  // ==============================================================================
  template<typename T_DATA> INLINE
  MultiDimCIModel<T_DATA>::MultiDimCIModel( T_DATA external_weight, T_DATA default_weight ): MultiDimReadOnly<T_DATA>(), __external_weight( external_weight ), __default_weight( default_weight ) {
    GUM_CONSTRUCTOR( MultiDimCIModel ) ;
  }

// ==============================================================================
/// Default constructor
// ==============================================================================
  template<typename T_DATA> INLINE
  MultiDimCIModel<T_DATA>::MultiDimCIModel( const MultiDimCIModel<T_DATA>& from ) : MultiDimReadOnly<T_DATA>( from ) {
    GUM_CONS_CPY( MultiDimCIModel );
    __default_weight = from.__default_weight;
    __external_weight = from.__external_weight;
    __causal_weights = from.__causal_weights;
  }

  // ============================================================================
  /// Copy constructor using a bijection to swap variables from source.
  // ============================================================================
  template<typename T_DATA> INLINE
  MultiDimCIModel<T_DATA>::MultiDimCIModel( const Bijection<const DiscreteVariable*, const DiscreteVariable*>& bij,
      const MultiDimCIModel<T_DATA>& from ):
      MultiDimReadOnly<T_DATA>() {
    GUM_CONSTRUCTOR( MultiDimCIModel );
    __default_weight = from.__default_weight;
    __external_weight = from.__external_weight;
    for ( HashTableConstIterator< const DiscreteVariable *, T_DATA > iter = from.__causal_weights.begin(); iter != from.__causal_weights.end(); ++iter ) {
      try {
        causalWeight( *( bij.first( iter.key() ) ), *iter );
      } catch ( NotFound& ) {
        causalWeight( *( iter.key() ), *iter );
      }
    }
  }

// ==============================================================================
/// destructor
// ==============================================================================
  template<typename T_DATA> INLINE
  MultiDimCIModel<T_DATA>::~MultiDimCIModel() {
    GUM_DESTRUCTOR( MultiDimCIModel );
  }

  template<typename T_DATA> INLINE
  T_DATA MultiDimCIModel<T_DATA>::causalWeight( const DiscreteVariable& v ) const {
    return ( __causal_weights.exists( &v ) ) ? __causal_weights[&v] : __default_weight;
  }

  template<typename T_DATA> INLINE
  void MultiDimCIModel<T_DATA>::causalWeight( const DiscreteVariable& v, T_DATA w ) const {
    if ( w == ( T_DATA )0 ) {
      GUM_ERROR( OperationNotAllowed, "No 0.0 as causal weight in noisyOR" );
    }

    __causal_weights.set( &v, w );
  }

  template<typename T_DATA> INLINE
  T_DATA MultiDimCIModel<T_DATA>::externalWeight() const {
    return __external_weight;
  }

  template<typename T_DATA> INLINE
  void MultiDimCIModel<T_DATA>::externalWeight( T_DATA w ) const {
    __external_weight = w;
  }

  template<typename T_DATA>
  const std::string MultiDimCIModel<T_DATA>::toString() const {
    std::stringstream s;
    s << MultiDimImplementation<T_DATA>::variable( 0 ) << "=CIModel([" << externalWeight() << "],";

    for ( Idx i = 1;i < MultiDimImplementation<T_DATA>::nbrDim();i++ ) {
      s << MultiDimImplementation<T_DATA>::variable( i ) << "[" << causalWeight( MultiDimImplementation<T_DATA>::variable( i ) ) << "]";
    }

    s << ")";

    std::string res;
    s >> res;
    return res;
  }

  // returns the name of the implementation
  template<typename T_DATA> INLINE
  const std::string& MultiDimCIModel<T_DATA>::name() const {
    static const std::string str = "MultiDimCIModel";
    return str;
  }

  template<typename T_DATA> INLINE
  void MultiDimCIModel<T_DATA>::_swap( const DiscreteVariable* x,
      const DiscreteVariable* y ) {
    MultiDimImplementation<T_DATA>::_swap( x, y );
    __causal_weights.insert( y, __causal_weights[x] );
    __causal_weights.erase( x );
  }

// ==================================================
} /* namespace gum */
