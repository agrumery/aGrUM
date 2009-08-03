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
 * @brief min aggregator
 *
* @author Pierre-Henri WUILLEMIN et Christophe GONZALES <{prenom.nom}_at_lip6.fr>
 */


namespace gum {

  namespace aggregator {
    // ==============================================================================
    /// Default constructor
    // ==============================================================================
    template<typename T_DATA> INLINE
    Min<T_DATA>::Min(): MultiDimAggregator<T_DATA>() {
      GUM_CONSTRUCTOR( Min )
    }

// ==============================================================================
/// Default constructor
// ==============================================================================
    template<typename T_DATA> INLINE
    Min<T_DATA>::Min( const Min<T_DATA>& from ) : MultiDimAggregator<T_DATA>( from ) {
      GUM_CONS_CPY( Min );
    }

// ==============================================================================
/// destructor
// ==============================================================================
    template<typename T_DATA> INLINE
    Min<T_DATA>::~Min() {
      GUM_DESTRUCTOR( Min );
    }

    template<typename T_DATA> INLINE
    Idx Min<T_DATA>::_neutralElt() const { return ( Idx )100000;} //clearly arbitrary choosen

    template<typename T_DATA> INLINE
    Idx Min<T_DATA>::_folder( const DiscreteVariable& v,Idx i1,Idx i2,bool& stop_iteration ) const { return ( i1<i2 )?i1:i2;}

    template<typename T_DATA> INLINE
    std::string Min<T_DATA>::aggregatorName( void ) const { return "min";}

    template<typename T_DATA> INLINE
    MultiDimContainer<T_DATA>* Min<T_DATA>::newFactory() const {
      return new Min<T_DATA>;
    }

  } // aggregator
} // namespace gum
