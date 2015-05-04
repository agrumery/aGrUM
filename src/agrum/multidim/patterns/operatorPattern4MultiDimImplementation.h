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
 * @brief the pattern used by all binary MultiDimImplementations operators
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN */

// check if we allowed these patterns to be used
#ifndef GUM_OPERATOR_PATTERN_ALLOWED

#warning To use operatorPattern4MultiDimImplementation.h, you must define GUM_OPERATOR_PATTERN_ALLOWED

#else

namespace gum {

#ifdef GUM_MULTI_DIM_OPERATOR_NAME
  template <typename T>
  MultiDimImplementation<T> *
  GUM_MULTI_DIM_OPERATOR_NAME(const MultiDimImplementation<T> &impl1,
                              const MultiDimImplementation<T> &impl2) {
#endif

    typename OperatorRegister4MultiDim<T>::OperatorPtr func;
    bool swapped_names = false;

    // get the appropriate function to perform the operation
    try {
      // try to find +(impl1,impl2) in the register
      func = OperatorRegister4MultiDim<T>::Register().get(
          GUM_MULTI_DIM_OPERATOR_FUNC_NAME, impl1.name(), impl2.name());
    } catch (NotFound &) {

#ifdef GUM_MULTI_DIM_SYMMETRIC_OPERATOR

      try {
        // the function could not be found but, as additions are symmetric, it
        // may be the case that +(impl2,impl1) does exist in the register.
        // For instance, we wrote +(MultiDimArray,MultiDimSparse)
        // but not +(MultiDimSparse,MultiDimArray) and we called
        // +(MultiDimSparse,MultiDimArray)
        func = OperatorRegister4MultiDim<T>::Register().get(
            GUM_MULTI_DIM_OPERATOR_FUNC_NAME, impl2.name(), impl1.name());
        swapped_names = true;
      } catch (NotFound &) {
#endif /* GUM_MULTI_DIM_SYMMETRIC_OPERATOR */

        func = OperatorRegister4MultiDim<T>::Register().get(
            GUM_MULTI_DIM_OPERATOR_FUNC_NAME, impl1.basename(), impl2.basename());

#ifdef GUM_MULTI_DIM_SYMMETRIC_OPERATOR
      }

#endif /* GUM_MULTI_DIM_SYMMETRIC_OPERATOR */
    }

    // perform the addition
    if (swapped_names)
      return func(&impl2, &impl1);
    else
      return func(&impl1, &impl2);
  }
}
#endif /* GUM_OPERATOR_PATTERN_ALLOWED */
