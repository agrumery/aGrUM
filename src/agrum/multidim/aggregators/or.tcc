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
 * @brief exists aggregator
 *
* @author Pierre-Henri WUILLEMIN et Christophe GONZALES <{prenom.nom}_at_lip6.fr>
 */
#include <sstream>
// to ease parser in IDEs
#include<agrum/multidim/aggregators/or.h>

namespace gum {

  namespace aggregator {
    template<typename T_DATA> INLINE
    Or<T_DATA>::Or( ): MultiDimAggregator<T_DATA>() {
      GUM_CONSTRUCTOR( Or )
    }

    template<typename T_DATA> INLINE
    Or<T_DATA>::Or( const Or<T_DATA>& from ) : MultiDimAggregator<T_DATA>( from ) {
      GUM_CONS_CPY( Or );
    }

    template<typename T_DATA> INLINE
    Or<T_DATA>::~Or() {
      GUM_DESTRUCTOR( Or );
    }

    template<typename T_DATA> INLINE
    Idx Or<T_DATA>::_neutralElt() const { return ( Idx )0;}

    template<typename T_DATA> INLINE
    Idx Or<T_DATA>::_folder( const DiscreteVariable& v, Idx i1, Idx i2, bool& stop_iteration ) const {
      if( i1 != 1 ) {
        return ( Idx )0;
      } else {
        stop_iteration = true;
        return ( Idx )1;
      }
    }

    template<typename T_DATA> INLINE
    std::string Or<T_DATA>::aggregatorName( void ) const {
      return std::string( "or" );
    }

    template<typename T_DATA> INLINE
    MultiDimContainer<T_DATA>* Or<T_DATA>::newFactory() const {
      return new Or<T_DATA>();
    }

  } // namespace aggregator
} // namespace gum
