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
 * @brief forall aggregator
 *
* @author Pierre-Henri WUILLEMIN et Christophe GONZALES <{prenom.nom}_at_lip6.fr>
 */
#include <sstream>

namespace gum {
namespace aggregator {
  template<typename T_DATA> INLINE
  Forall<T_DATA>::Forall( Idx value ): MultiDimAggregator<T_DATA>(),__value( value ) {
    GUM_CONSTRUCTOR( Forall )
  }

  template<typename T_DATA> INLINE
  Forall<T_DATA>::Forall( const Forall<T_DATA>& from ) : MultiDimAggregator<T_DATA>( from ) {
    __value=from.__value;
    GUM_CONS_CPY( Forall );
  }

  template<typename T_DATA> INLINE
  Forall<T_DATA>::~Forall() {
    GUM_DESTRUCTOR( Forall );
  }

  template<typename T_DATA> INLINE
  Idx Forall<T_DATA>::_neutralElt() const { return ( Idx )1;}

  template<typename T_DATA> INLINE
  Idx Forall<T_DATA>::_folder( const DiscreteVariable& v,Idx i1,Idx i2,bool& stop_iteration ) const {
    if ( i1==__value ) {
      return ( Idx )1;
    } else {
      stop_iteration=true;
      return ( Idx )0;
    }
  }

  template<typename T_DATA> INLINE

  std::string Forall<T_DATA>::aggregatorName( void ) const {
    std::stringstream ss;
    std::string s;
    ss<<"forall["<<__value<<"]";
    ss>>s;
    return s;
  }

    template<typename T_DATA> INLINE
    MultiDimContainer<T_DATA>* Forall<T_DATA>::newFactory() const {
      GUM_ERROR( OperationNotAllowed,
		 "This class doesn't contain an empty constructor" );
      return 0;
    }

}// aggregator
} // namespace gum
