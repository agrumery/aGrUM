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
 * @brief A generic class to combine efficiently several ScheduleMultiDims
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS


#include <agrum/core/debug.h>


namespace gum {

  
  /// constructor
  template<typename T_DATA>
  ScheduleCombination<T_DATA>::ScheduleCombination () {
    /// for debugging purposes
    GUM_CONSTRUCTOR ( ScheduleCombination );
  }

  
  /// copy constructor
  template<typename T_DATA>
  ScheduleCombination<T_DATA>::ScheduleCombination
  ( const ScheduleCombination<T_DATA>& from ) {
    /// for debugging purposes
    GUM_CONS_CPY ( ScheduleCombination );
  }

  
  /// destructor
  template<typename T_DATA>
  ScheduleCombination<T_DATA>::~ScheduleCombination () {
    /// for debugging purposes
    GUM_DESTRUCTOR ( ScheduleCombination );
  }

  
} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
