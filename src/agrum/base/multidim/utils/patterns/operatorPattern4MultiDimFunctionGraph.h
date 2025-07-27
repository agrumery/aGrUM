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
 * @brief the pattern used by all binary MultiDimFunctionGraphs operators
 *
 * @author Jean-Christophe MAGNAN
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

// check if we allowed these patterns to be used
#ifndef GUM_OPERATOR_PATTERN_ALLOWED

// #warning To use operatorPattern4MultiDimFunctionGraph.h, you must define
// GUM_OPERATOR_PATTERN_ALLOWED

#else

#  include <agrum/base/core/functors.h>
#  include <agrum/base/multidim/utils/FunctionGraphUtilities/operators/multiDimFunctionGraphOperator.h>

namespace gum {

  /**
   * This specialized function combines two MultiDimFunctionGraphs.  Any
   * operation (addition, subtraction, multiplication, ...) must call this
   * methods.
   **/

#  ifdef GUM_MULTI_DIM_OPERATOR_NAME
#    define GUM_MULTI_DIM_OPERATOR_TYPE GUM_SCALAR

  template < typename GUM_SCALAR >
  MultiDimFunctionGraph< GUM_SCALAR >*
      GUM_MULTI_DIM_OPERATOR_NAME(const MultiDimFunctionGraph< GUM_SCALAR >* dg1,
                                  const MultiDimFunctionGraph< GUM_SCALAR >* dg2)
#  endif

  // clang-format off

#ifdef GUM_MULTI_DIM_OPERATOR_IMPL2DECISION_GRAPH_NAME
#define GUM_MULTI_DIM_OPERATOR_TYPE GUM_SCALAR
  template <typename GUM_SCALAR>
  MultiDimImplementation<GUM_SCALAR>*
  GUM_MULTI_DIM_OPERATOR_IMPL2DECISION_GRAPH_NAME(
      const MultiDimImplementation<GUM_SCALAR>* tt1,
      const MultiDimImplementation<GUM_SCALAR>* tt2 )
#endif

  // clang-format on

  {

#  ifdef GUM_MULTI_DIM_OPERATOR_IMPL2DECISION_GRAPH_NAME
    const MultiDimFunctionGraph< GUM_SCALAR >* dg1
        = reinterpret_cast< const MultiDimFunctionGraph< GUM_SCALAR >* >(tt1);
    const MultiDimFunctionGraph< GUM_SCALAR >* dg2
        = reinterpret_cast< const MultiDimFunctionGraph< GUM_SCALAR >* >(tt2);
#  endif
    MultiDimFunctionGraphOperator< GUM_MULTI_DIM_OPERATOR_TYPE, GUM_MULTI_DIM_OPERATOR > ope(dg1,
                                                                                             dg2);

    return ope.compute();
  }

#  undef GUM_MULTI_DIM_OPERATOR_TYPE

}   // End of namespace gum
#endif /* GUM_OPERATOR_PATTERN_ALLOWED */
