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
 * @brief Abstract base class for all multi dimensionnal read only structure
 *
* @author Pierre-Henri WUILLEMIN et Christophe GONZALES <{prenom.nom}_at_lip6.fr>
 */


namespace gum {
  // ==============================================================================
  /// Default constructor
  // ==============================================================================
  template<typename T_DATA> INLINE
  MultiDimReadOnly<T_DATA>::MultiDimReadOnly(): MultiDimImplementation<T_DATA>() {
    GUM_CONSTRUCTOR( MultiDimReadOnly ) ;
  }

// ==============================================================================
/// Default constructor
// ==============================================================================
  template<typename T_DATA> INLINE
  MultiDimReadOnly<T_DATA>::MultiDimReadOnly( const MultiDimReadOnly<T_DATA>& from ) : MultiDimImplementation<T_DATA>( from ) {
    GUM_CONS_CPY( MultiDimReadOnly );
  }

// ==============================================================================
/// destructor
// ==============================================================================
  template<typename T_DATA> INLINE
  MultiDimReadOnly<T_DATA>::~MultiDimReadOnly() {
    GUM_DESTRUCTOR( MultiDimReadOnly );
  }

// ==================================================
} /* namespace gum */
