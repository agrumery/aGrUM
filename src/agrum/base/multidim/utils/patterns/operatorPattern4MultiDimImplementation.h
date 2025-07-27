/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief the pattern used by all binary MultiDimImplementations operators
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

// check if we allowed these patterns to be used
#ifndef GUM_OPERATOR_PATTERN_ALLOWED

// #warning To use operatorPattern4MultiDimImplementation.h, you must define
// GUM_OPERATOR_PATTERN_ALLOWED

#else

namespace gum {

#  ifdef GUM_MULTI_DIM_OPERATOR_NAME
  template < typename T >
  MultiDimImplementation< T >*
      GUM_MULTI_DIM_OPERATOR_NAME(const MultiDimImplementation< T >& impl1,
                                  const MultiDimImplementation< T >& impl2) {
#  endif

    typename OperatorRegister4MultiDim< T >::OperatorPtr func;
    bool                                                 swapped_names = false;

    // get the appropriate function to perform the operation
    try {
      // try to find +(impl1,impl2) in the register
      func = OperatorRegister4MultiDim< T >::Register().get(GUM_MULTI_DIM_OPERATOR_FUNC_NAME,
                                                            impl1.name(),
                                                            impl2.name());
    } catch (NotFound const&) {

#  ifdef GUM_MULTI_DIM_SYMMETRIC_OPERATOR

      try {
        // the function could not be found but, as additions are symmetric, it
        // may be the case that +(impl2,impl1) does exist in the register.  For
        // instance, we wrote +(MultiDimArray,MultiDimSparse) but not
        // +(MultiDimSparse,MultiDimArray) and we called
        // +(MultiDimSparse,MultiDimArray)
        func = OperatorRegister4MultiDim< T >::Register().get(GUM_MULTI_DIM_OPERATOR_FUNC_NAME,
                                                              impl2.name(),
                                                              impl1.name());
        swapped_names = true;
      } catch (NotFound const&) {
#  endif /* GUM_MULTI_DIM_SYMMETRIC_OPERATOR */

        func = OperatorRegister4MultiDim< T >::Register().get(GUM_MULTI_DIM_OPERATOR_FUNC_NAME,
                                                              impl1.basename(),
                                                              impl2.basename());

#  ifdef GUM_MULTI_DIM_SYMMETRIC_OPERATOR
      }

#  endif /* GUM_MULTI_DIM_SYMMETRIC_OPERATOR */
    }

    // perform the addition
    if (swapped_names) return func(&impl2, &impl1);
    else return func(&impl1, &impl2);
  }
}   // namespace gum
#endif   /* GUM_OPERATOR_PATTERN_ALLOWED */
