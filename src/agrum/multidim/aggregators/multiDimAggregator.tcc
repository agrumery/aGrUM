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
 * @brief Abstract base class for all multi dimensionnal aggregator
 *
* @author Pierre-Henri WUILLEMIN et Christophe GONZALES <{prenom.nom}_at_lip6.fr>
 */

// to ease parser in IDEs
#include<agrum/multidim/aggregators/multiDimAggregator.h>

namespace gum {
  // ==============================================================================
  /// Default constructor
  // ==============================================================================
  template<typename T_DATA> INLINE
  MultiDimAggregator<T_DATA>::MultiDimAggregator(): MultiDimReadOnly<T_DATA>() {
    GUM_CONSTRUCTOR( MultiDimAggregator ) ;
  }

// ==============================================================================
/// Default constructor
// ==============================================================================
  template<typename T_DATA> INLINE
  MultiDimAggregator<T_DATA>::MultiDimAggregator( const MultiDimAggregator<T_DATA>& from ) : MultiDimReadOnly<T_DATA>( from ) {
    GUM_CONS_CPY( MultiDimAggregator );
  }

// ==============================================================================
/// destructor
// ==============================================================================
  template<typename T_DATA> INLINE
  MultiDimAggregator<T_DATA>::~MultiDimAggregator() {
    GUM_DESTRUCTOR( MultiDimAggregator );
  }

  template<typename T_DATA>
  T_DATA MultiDimAggregator<T_DATA>::get( const Instantiation& i ) const {
    if ( this->nbrDim() < 2 ) {
      GUM_ERROR( OperationNotAllowed, "Not enough variable for an aggregator" );
    }

    const DiscreteVariable& agg = this->variable(( Idx )0 );

    //is i equal to f(f(f(f...(j_,,neutral_elt))))
    Idx current = _neutralElt();

    bool stop_iteration = false;

    for ( Idx j = 1;j < this->nbrDim();j++ ) {
      current = _folder( this->variable( j ), i.val( this->variable( j ) ), current , stop_iteration );

      if ( stop_iteration ) break;
    }

    // truncate to fit in aggreegator domain size
    if ( current >= agg.domainSize() ) current = agg.domainSize() - 1;

    return ( i.val( agg ) == current ) ? ( T_DATA )1 : ( T_DATA )0;
  }

  template<typename T_DATA>
  const std::string MultiDimAggregator<T_DATA>::toString() const {
    std::stringstream s;
    s << MultiDimImplementation<T_DATA>::variable( 0 ) << "=" << aggregatorName() << "(";

    for ( Idx i = 1;i < MultiDimImplementation<T_DATA>::nbrDim();i++ ) {
      if ( i > 1 ) s << ",";

      s << MultiDimImplementation<T_DATA>::variable( i );
    }

    s << ")";

    return s.str();
  }

  // returns the name of the implementation
  template<typename T_DATA>
  const std::string& MultiDimAggregator<T_DATA>::name () const {
    static const std::string str = "MultiDimAggregator";
    return str;
  }
 
  // ============================================================================
  // For friendly displaying the content of the variable.
  // ============================================================================
  template<typename T_DATA> INLINE
  std::ostream& operator<<( std::ostream& s, const MultiDimAggregator<T_DATA>& ag ) {
    return s << ag.toString();
  }
 

// ==================================================
} /* namespace gum */
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
