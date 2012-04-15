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
#include<agrum/multidim/CIModels/multiDimNoisyORCompound.h>

namespace gum {
  // ==============================================================================
  /// Default constructor
  // ==============================================================================
  template<typename T_DATA> INLINE
  MultiDimNoisyORCompound<T_DATA>::MultiDimNoisyORCompound( T_DATA external_weight, T_DATA default_weight ): MultiDimCIModel<T_DATA>( external_weight , default_weight ) {
    GUM_CONSTRUCTOR( MultiDimNoisyORCompound ) ;
  }

// ==============================================================================
/// Default constructor
// ==============================================================================
  template<typename T_DATA> INLINE
  MultiDimNoisyORCompound<T_DATA>::MultiDimNoisyORCompound( const MultiDimNoisyORCompound<T_DATA>& from ) : MultiDimCIModel<T_DATA>( from ) {
    GUM_CONS_CPY( MultiDimNoisyORCompound );
  }

  // ============================================================================
  /// Copy constructor using a bijection to swap variables from source.
  // ============================================================================
  template<typename T_DATA> INLINE
  MultiDimNoisyORCompound<T_DATA>::MultiDimNoisyORCompound( const Bijection<const DiscreteVariable*, const DiscreteVariable*>& bij,
      const MultiDimNoisyORCompound<T_DATA>& from ):
      MultiDimCIModel<T_DATA>(bij,from) {
    GUM_CONSTRUCTOR( MultiDimNoisyORCompound );
  }

// ==============================================================================
/// destructor
// ==============================================================================
  template<typename T_DATA> INLINE
  MultiDimNoisyORCompound<T_DATA>::~MultiDimNoisyORCompound() {
    GUM_DESTRUCTOR( MultiDimNoisyORCompound );
  }

  template<typename T_DATA>
  T_DATA MultiDimNoisyORCompound<T_DATA>::get( const Instantiation& i ) const {
    if ( this->nbrDim() < 1 ) {
      GUM_ERROR( OperationNotAllowed, "Not enough variable for a NoisyOr " );
    }

    const DiscreteVariable& C = this->variable(( Idx )0 );

    if ( i.val( C ) > 1 ) return ( T_DATA )0.0;

    T_DATA ratio = ( T_DATA )1.0 - this->externalWeight();

    T_DATA fact = ( T_DATA )ratio;

    if ( fact != ( T_DATA )0 ) {
      for ( Idx j = 1;j < this->nbrDim();j++ ) {
        const DiscreteVariable& v = this->variable( j );

        if ( i.val( v ) == 1 ) {
          T_DATA pr = ( 1-this->causalWeight( v ) )/ratio;

          if ( pr == ( T_DATA )0.0 ) {
            fact = ( T_DATA )0.0;
            break;
          } else {
            fact *= pr;
          }
        }
      }
    }

    return ( i.val( C ) != 1 ) ? fact : ( T_DATA )1.0 - fact;
  }

  template<typename T_DATA>
  const std::string MultiDimNoisyORCompound<T_DATA>::toString() const {
    std::stringstream s;
    s << MultiDimImplementation<T_DATA>::variable( 0 ) << "=noisyORCompound([" << this->externalWeight() << "],";

    for ( Idx i = 1;i < MultiDimImplementation<T_DATA>::nbrDim();i++ ) {
      s << MultiDimImplementation<T_DATA>::variable( i ) << "[" << this->causalWeight( MultiDimImplementation<T_DATA>::variable( i ) ) << "]";
    }
    
    s << ")";

    std::string res;
    s >> res;
    return res;
  }


  // ============================================================================
  // For friendly displaying the content of the variable.
  // ============================================================================
  template<typename T_DATA> INLINE
  std::ostream& operator<<( std::ostream& s, const MultiDimNoisyORCompound<T_DATA>& ag ) {
    return s << ag.toString();
  }

  template<typename T_DATA> INLINE
  MultiDimContainer<T_DATA>* MultiDimNoisyORCompound<T_DATA>::newFactory() const {
    return new MultiDimNoisyORCompound<T_DATA>( this->__external_weight, this->__default_weight );
  }

  // returns the name of the implementation
  template<typename T_DATA> INLINE
  const std::string& MultiDimNoisyORCompound<T_DATA>::name() const {
    static const std::string str = "MultiDimNoisyORCompound";
    return str;
  }

// ==================================================
} /* namespace gum */
