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
 * @brief max aggregator
 *
* @author Pierre-Henri WUILLEMIN et Christophe GONZALES <{prenom.nom}_at_lip6.fr>
 */


namespace gum {

  namespace aggregator {
  template<typename T_DATA> INLINE
  Max<T_DATA>::Max(): MultiDimAggregator<T_DATA>() {
    GUM_CONSTRUCTOR( Max )
  }

  template<typename T_DATA> INLINE
  Max<T_DATA>::Max( const Max<T_DATA>& from ) : MultiDimAggregator<T_DATA>( from ) {
    GUM_CONS_CPY( Max );
  }

  template<typename T_DATA> INLINE
  Max<T_DATA>::~Max() {
    GUM_DESTRUCTOR( Max );
  }

  template<typename T_DATA> INLINE
  Idx Max<T_DATA>::_neutralElt() const { return ( Idx )0;}

  template<typename T_DATA> INLINE
  Idx Max<T_DATA>::_folder( const DiscreteVariable& v,Idx i1,Idx i2,bool& stop_iteration  ) const { return ( i1>i2 )?i1:i2;}

  template<typename T_DATA> INLINE
  std::string Max<T_DATA>::aggregatorName( void ) const { return "max";}

  } // aggregator
} // namespace gum
