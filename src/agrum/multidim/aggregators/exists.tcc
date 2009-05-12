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

namespace gum {

  namespace aggregator {
    template<typename T_DATA> INLINE
    Exists<T_DATA>::Exists( Idx value ): MultiDimAggregator<T_DATA>(),__value( value ) {
      GUM_CONSTRUCTOR( Exists )
    }

    template<typename T_DATA> INLINE
    Exists<T_DATA>::Exists( const Exists<T_DATA>& from ) : MultiDimAggregator<T_DATA>( from ) {
      __value=from.__value;
      GUM_CONS_CPY( Exists );
    }

    template<typename T_DATA> INLINE
    Exists<T_DATA>::~Exists() {
      GUM_DESTRUCTOR( Exists );
    }

    template<typename T_DATA> INLINE
    Idx Exists<T_DATA>::_neutralElt() const { return ( Idx )0;}

    template<typename T_DATA> INLINE
    Idx Exists<T_DATA>::_folder( const DiscreteVariable& v,Idx i1,Idx i2,bool& stop_iteration ) const {
      if ( i1!=__value ) {
        return ( Idx )0;
      } else {
        stop_iteration=true;
        return ( Idx )1;
      }
    }

    template<typename T_DATA> INLINE

    std::string Exists<T_DATA>::aggregatorName( void ) const {
      std::stringstream ss;
      std::string s;
      ss<<"exists["<<__value<<"]";
      ss>>s;
      return s;
    }
  } // namespace aggregator
} // namespace gum
