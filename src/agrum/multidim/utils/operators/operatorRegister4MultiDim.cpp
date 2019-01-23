
/***************************************************************************
 *   Copyright (C) 2017 by Pierre-Henri WUILLEMIN and Christophe GONZALES   *
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
/**
 * @file
 * @brief Class to include at least once this header
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 */

#include <agrum/multidim/utils/operators/operatorRegister4MultiDim.h>


namespace gum {


  // a named constructor that constructs one and only one Register per data type
  template <>
  OperatorRegister4MultiDim< float >&
    OperatorRegister4MultiDim< float >::Register() {
    static OperatorRegister4MultiDim< float > container;
    
#  ifdef GUM_DEBUG_MODE
    static bool first = true;

    if (first) {
      first = false;
      // for debugging purposes, we should inform the aGrUM's debugger that
      // the hashtable contained within the OperatorRegister4MultiDim will be
      // removed at the end of the program's execution.
      __debug__::__inc_deletion(
        "HashTable", __FILE__, __LINE__, "destructor of", (void*)&container.__set);
    }

#  endif /* GUM_DEBUG_MODE */

    return container;
  }


  // a named constructor that constructs one and only one Register per data type
  template <>
  OperatorRegister4MultiDim< double >&
    OperatorRegister4MultiDim< double >::Register() {
    static OperatorRegister4MultiDim< double > container;
    
#  ifdef GUM_DEBUG_MODE
    static bool first = true;

    if (first) {
      first = false;
      // for debugging purposes, we should inform the aGrUM's debugger that
      // the hashtable contained within the OperatorRegister4MultiDim will be
      // removed at the end of the program's execution.
      __debug__::__inc_deletion(
        "HashTable", __FILE__, __LINE__, "destructor of", (void*)&container.__set);
    }

#  endif /* GUM_DEBUG_MODE */

    return container;
  }

} /* namespace gum */

