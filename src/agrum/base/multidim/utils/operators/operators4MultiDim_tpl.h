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
 * @brief Efficient functionals for combining multiDims
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// allow operatorsPatterns to be used
#  define GUM_OPERATOR_PATTERN_ALLOWED 1

#  include <agrum/base/multidim/utils/operators/operatorRegister4MultiDim.h>

// ==========================================================================
//                              MultiDimArrays functions
// ==========================================================================

// a specialized function for summing two multiDimArrays

// addition taking in argument 2 multiDimArrays
#  define GUM_MULTI_DIM_OPERATOR_NAME  add2MultiDimArrays
#  define GUM_MULTI_DIM_OPERATOR(x, y) (x) + (y)
#  include <agrum/base/multidim/utils/patterns/operatorPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_OPERATOR_NAME
#  undef GUM_MULTI_DIM_OPERATOR

#  define GUM_MULTI_DIM_OPERATOR_IMPL2ARRAY_NAME add2MultiDimArrays
#  define GUM_MULTI_DIM_OPERATOR(x, y)           (x) + (y)
#  include <agrum/base/multidim/utils/patterns/operatorPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_OPERATOR_IMPL2ARRAY_NAME
#  undef GUM_MULTI_DIM_OPERATOR

#  define GUM_MULTI_DIM_OPERATOR_POINTER_NAME add2MultiDimArrays4Pointers
#  define GUM_MULTI_DIM_OPERATOR(x, y)        new T((*x) + (*y))
#  include <agrum/base/multidim/utils/patterns/operatorPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_OPERATOR_POINTER_NAME
#  undef GUM_MULTI_DIM_OPERATOR

#  define GUM_MULTI_DIM_OPERATOR_POINTER_IMPL2ARRAY_NAME add2MultiDimArrays4Pointers
#  define GUM_MULTI_DIM_OPERATOR(x, y)                   new T((*x) + (*y))
#  include <agrum/base/multidim/utils/patterns/operatorPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_OPERATOR_POINTER_IMPL2ARRAY_NAME
#  undef GUM_MULTI_DIM_OPERATOR

// a specialized function for subtracting two multiDimArrays

#  define GUM_MULTI_DIM_OPERATOR_NAME  subtract2MultiDimArrays
#  define GUM_MULTI_DIM_OPERATOR(x, y) (x) - (y)
#  include <agrum/base/multidim/utils/patterns/operatorPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_OPERATOR_NAME
#  undef GUM_MULTI_DIM_OPERATOR

#  define GUM_MULTI_DIM_OPERATOR_IMPL2ARRAY_NAME subtract2MultiDimArrays
#  define GUM_MULTI_DIM_OPERATOR(x, y)           (x) - (y)
#  include <agrum/base/multidim/utils/patterns/operatorPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_OPERATOR_IMPL2ARRAY_NAME
#  undef GUM_MULTI_DIM_OPERATOR

#  define GUM_MULTI_DIM_OPERATOR_POINTER_NAME subtract2MultiDimArrays4Pointers
#  define GUM_MULTI_DIM_OPERATOR(x, y)        new T((*x) - (*y))
#  include <agrum/base/multidim/utils/patterns/operatorPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_OPERATOR_POINTER_NAME
#  undef GUM_MULTI_DIM_OPERATOR

#  define GUM_MULTI_DIM_OPERATOR_POINTER_IMPL2ARRAY_NAME subtract2MultiDimArrays4Pointers
#  define GUM_MULTI_DIM_OPERATOR(x, y)                   new T((*x) - (*y))
#  include <agrum/base/multidim/utils/patterns/operatorPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_OPERATOR_POINTER_IMPL2ARRAY_NAME
#  undef GUM_MULTI_DIM_OPERATOR

// a specialized function for multiplying two multiDimArrays

#  define GUM_MULTI_DIM_OPERATOR_NAME  multiply2MultiDimArrays
#  define GUM_MULTI_DIM_OPERATOR(x, y) (x) * (y)
#  include <agrum/base/multidim/utils/patterns/operatorPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_OPERATOR_NAME
#  undef GUM_MULTI_DIM_OPERATOR

#  define GUM_MULTI_DIM_OPERATOR_IMPL2ARRAY_NAME multiply2MultiDimArrays
#  define GUM_MULTI_DIM_OPERATOR(x, y)           (x) * (y)
#  include <agrum/base/multidim/utils/patterns/operatorPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_OPERATOR_IMPL2ARRAY_NAME
#  undef GUM_MULTI_DIM_OPERATOR

#  define GUM_MULTI_DIM_OPERATOR_POINTER_NAME multiply2MultiDimArrays4Pointers
#  define GUM_MULTI_DIM_OPERATOR(x, y)        new T((*x) * (*y))
#  include <agrum/base/multidim/utils/patterns/operatorPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_OPERATOR_POINTER_NAME
#  undef GUM_MULTI_DIM_OPERATOR

#  define GUM_MULTI_DIM_OPERATOR_POINTER_IMPL2ARRAY_NAME multiply2MultiDimArrays4Pointers
#  define GUM_MULTI_DIM_OPERATOR(x, y)                   new T((*x) * (*y))
#  include <agrum/base/multidim/utils/patterns/operatorPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_OPERATOR_POINTER_IMPL2ARRAY_NAME
#  undef GUM_MULTI_DIM_OPERATOR

// a specialized function for dividing two multiDimArrays

#  define GUM_MULTI_DIM_OPERATOR_NAME  divide2MultiDimArrays
#  define GUM_MULTI_DIM_OPERATOR(x, y) (x) / (y)
#  include <agrum/base/multidim/utils/patterns/operatorPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_OPERATOR_NAME
#  undef GUM_MULTI_DIM_OPERATOR

#  define GUM_MULTI_DIM_OPERATOR_IMPL2ARRAY_NAME divide2MultiDimArrays
#  define GUM_MULTI_DIM_OPERATOR(x, y)           (x) / (y)
#  include <agrum/base/multidim/utils/patterns/operatorPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_OPERATOR_IMPL2ARRAY_NAME
#  undef GUM_MULTI_DIM_OPERATOR

#  define GUM_MULTI_DIM_OPERATOR_POINTER_NAME divide2MultiDimArrays4Pointers
#  define GUM_MULTI_DIM_OPERATOR(x, y)        new T((*x) / (*y))
#  include <agrum/base/multidim/utils/patterns/operatorPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_OPERATOR_POINTER_NAME
#  undef GUM_MULTI_DIM_OPERATOR

#  define GUM_MULTI_DIM_OPERATOR_POINTER_IMPL2ARRAY_NAME divide2MultiDimArrays4Pointers
#  define GUM_MULTI_DIM_OPERATOR(x, y)                   new T((*x) / (*y))
#  include <agrum/base/multidim/utils/patterns/operatorPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_OPERATOR_POINTER_IMPL2ARRAY_NAME
#  undef GUM_MULTI_DIM_OPERATOR

// ==========================================================================
//                          MultiDimFunctionGraphs functions
// ==========================================================================

// ///////////////////////////////////////////////////////
// a specialized function for summing two multiDimFunctionGraphs
// ///////////////////////////////////////////////////////

#  define GUM_MULTI_DIM_OPERATOR std::plus

#  define GUM_MULTI_DIM_OPERATOR_NAME add2MultiDimFunctionGraphs
#  include <agrum/base/multidim/utils/patterns/operatorPattern4MultiDimFunctionGraph.h>
#  undef GUM_MULTI_DIM_OPERATOR_NAME

#  define GUM_MULTI_DIM_OPERATOR_IMPL2DECISION_GRAPH_NAME add2MultiDimFunctionGraphs
#  include <agrum/base/multidim/utils/patterns/operatorPattern4MultiDimFunctionGraph.h>
#  undef GUM_MULTI_DIM_OPERATOR_IMPL2DECISION_GRAPH_NAME

#  undef GUM_MULTI_DIM_OPERATOR

// ///////////////////////////////////////////////////////
// a specialized function for subtracting two multiDimFunctionGraphs
// ///////////////////////////////////////////////////////

#  define GUM_MULTI_DIM_OPERATOR std::minus

#  define GUM_MULTI_DIM_OPERATOR_NAME subtract2MultiDimFunctionGraphs
#  include <agrum/base/multidim/utils/patterns/operatorPattern4MultiDimFunctionGraph.h>
#  undef GUM_MULTI_DIM_OPERATOR_NAME

#  define GUM_MULTI_DIM_OPERATOR_IMPL2DECISION_GRAPH_NAME subtract2MultiDimFunctionGraphs
#  include <agrum/base/multidim/utils/patterns/operatorPattern4MultiDimFunctionGraph.h>
#  undef GUM_MULTI_DIM_OPERATOR_IMPL2DECISION_GRAPH_NAME

#  undef GUM_MULTI_DIM_OPERATOR

// ///////////////////////////////////////////////////////
// a specialized function for multiplying two multiDimFunctionGraphs
// ///////////////////////////////////////////////////////

#  define GUM_MULTI_DIM_OPERATOR std::multiplies

#  define GUM_MULTI_DIM_OPERATOR_NAME multiply2MultiDimFunctionGraphs
#  include <agrum/base/multidim/utils/patterns/operatorPattern4MultiDimFunctionGraph.h>
#  undef GUM_MULTI_DIM_OPERATOR_NAME

#  define GUM_MULTI_DIM_OPERATOR_IMPL2DECISION_GRAPH_NAME multiply2MultiDimFunctionGraphs
#  include <agrum/base/multidim/utils/patterns/operatorPattern4MultiDimFunctionGraph.h>
#  undef GUM_MULTI_DIM_OPERATOR_IMPL2DECISION_GRAPH_NAME

#  undef GUM_MULTI_DIM_OPERATOR

// ///////////////////////////////////////////////////////
// a specialized function for dividing two multiDimFunctionGraphs
// ///////////////////////////////////////////////////////

#  define GUM_MULTI_DIM_OPERATOR std::divides

#  define GUM_MULTI_DIM_OPERATOR_NAME divide2MultiDimFunctionGraphs
#  include <agrum/base/multidim/utils/patterns/operatorPattern4MultiDimFunctionGraph.h>
#  undef GUM_MULTI_DIM_OPERATOR_NAME

#  define GUM_MULTI_DIM_OPERATOR_IMPL2DECISION_GRAPH_NAME divide2MultiDimFunctionGraphs
#  include <agrum/base/multidim/utils/patterns/operatorPattern4MultiDimFunctionGraph.h>
#  undef GUM_MULTI_DIM_OPERATOR_IMPL2DECISION_GRAPH_NAME

#  undef GUM_MULTI_DIM_OPERATOR

// ///////////////////////////////////////////////////////
// a specialized function for finding max of two multiDimFunctionGraphs
// ///////////////////////////////////////////////////////

#  define GUM_MULTI_DIM_OPERATOR Maximizes

#  define GUM_MULTI_DIM_OPERATOR_NAME maximize2MultiDimFunctionGraphs
#  include <agrum/base/multidim/utils/patterns/operatorPattern4MultiDimFunctionGraph.h>
#  undef GUM_MULTI_DIM_OPERATOR_NAME

#  define GUM_MULTI_DIM_OPERATOR_IMPL2DECISION_GRAPH_NAME maximize2MultiDimFunctionGraphs
#  include <agrum/base/multidim/utils/patterns/operatorPattern4MultiDimFunctionGraph.h>
#  undef GUM_MULTI_DIM_OPERATOR_IMPL2DECISION_GRAPH_NAME

#  undef GUM_MULTI_DIM_OPERATOR

// ///////////////////////////////////////////////////////
// a specialized function for finding min of two multiDimFunctionGraphs
// ///////////////////////////////////////////////////////

#  define GUM_MULTI_DIM_OPERATOR Minimizes

#  define GUM_MULTI_DIM_OPERATOR_NAME minimize2MultiDimFunctionGraphs
#  include <agrum/base/multidim/utils/patterns/operatorPattern4MultiDimFunctionGraph.h>
#  undef GUM_MULTI_DIM_OPERATOR_NAME

#  define GUM_MULTI_DIM_OPERATOR_IMPL2DECISION_GRAPH_NAME minimize2MultiDimFunctionGraphs
#  include <agrum/base/multidim/utils/patterns/operatorPattern4MultiDimFunctionGraph.h>
#  undef GUM_MULTI_DIM_OPERATOR_IMPL2DECISION_GRAPH_NAME

#  undef GUM_MULTI_DIM_OPERATOR

// a specialized function for functionally combining two multiDimArrays

#  define GUM_MULTI_DIM_OPERATOR_NAME_F combine2MultiDimArrays
#  define GUM_MULTI_DIM_OPERATOR(x, y)  f((x), (y))
#  include <agrum/base/multidim/utils/patterns/operatorPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_OPERATOR_NAME_F
#  undef GUM_MULTI_DIM_OPERATOR

// the operators that should be used to select appropriately the functions
// to combine multiDims

// the function to be used to add two MultiDimImplementations
#  define GUM_MULTI_DIM_OPERATOR_NAME      operator+
#  define GUM_MULTI_DIM_OPERATOR_FUNC_NAME "+"
#  define GUM_MULTI_DIM_SYMMETRIC_OPERATOR
#  include <agrum/base/multidim/utils/patterns/operatorPattern4MultiDimImplementation.h>
#  undef GUM_MULTI_DIM_OPERATOR_NAME
#  undef GUM_MULTI_DIM_OPERATOR_FUNC_NAME
#  undef GUM_MULTI_DIM_SYMMETRIC_OPERATOR

// the function to be used to multiply two MultiDimImplementations
#  define GUM_MULTI_DIM_OPERATOR_NAME      operator*
#  define GUM_MULTI_DIM_OPERATOR_FUNC_NAME "*"
#  define GUM_MULTI_DIM_SYMMETRIC_OPERATOR
#  include <agrum/base/multidim/utils/patterns/operatorPattern4MultiDimImplementation.h>
#  undef GUM_MULTI_DIM_OPERATOR_NAME
#  undef GUM_MULTI_DIM_OPERATOR_FUNC_NAME
#  undef GUM_MULTI_DIM_SYMMETRIC_OPERATOR

// the function to be used to subtract two MultiDimImplementations
#  define GUM_MULTI_DIM_OPERATOR_NAME      operator-
#  define GUM_MULTI_DIM_OPERATOR_FUNC_NAME "-"
#  include <agrum/base/multidim/utils/patterns/operatorPattern4MultiDimImplementation.h>
#  undef GUM_MULTI_DIM_OPERATOR_NAME
#  undef GUM_MULTI_DIM_OPERATOR_FUNC_NAME

// the function to be used to divide two MultiDimImplementations
#  define GUM_MULTI_DIM_OPERATOR_NAME      operator/
#  define GUM_MULTI_DIM_OPERATOR_FUNC_NAME "/"
#  include <agrum/base/multidim/utils/patterns/operatorPattern4MultiDimImplementation.h>
#  undef GUM_MULTI_DIM_OPERATOR_NAME
#  undef GUM_MULTI_DIM_OPERATOR_FUNC_NAME

// default "basename" functions for combining two MultiDimImplementations

#  define GUM_MULTI_DIM_OPERATOR_NAME  add2MultiDimImplementations
#  define GUM_MULTI_DIM_OPERATOR(x, y) (x) + (y)
#  include <agrum/base/multidim/utils/patterns/operatorPattern4BaseName.h>
#  undef GUM_MULTI_DIM_OPERATOR_NAME
#  undef GUM_MULTI_DIM_OPERATOR

#  define GUM_MULTI_DIM_OPERATOR_NAME  subtract2MultiDimImplementations
#  define GUM_MULTI_DIM_OPERATOR(x, y) (x) - (y)
#  include <agrum/base/multidim/utils/patterns/operatorPattern4BaseName.h>
#  undef GUM_MULTI_DIM_OPERATOR_NAME
#  undef GUM_MULTI_DIM_OPERATOR

#  define GUM_MULTI_DIM_OPERATOR_NAME  multiply2MultiDimImplementations
#  define GUM_MULTI_DIM_OPERATOR(x, y) (x) * (y)
#  include <agrum/base/multidim/utils/patterns/operatorPattern4BaseName.h>
#  undef GUM_MULTI_DIM_OPERATOR_NAME
#  undef GUM_MULTI_DIM_OPERATOR

#  define GUM_MULTI_DIM_OPERATOR_NAME  divide2MultiDimImplementations
#  define GUM_MULTI_DIM_OPERATOR(x, y) (x) / (y)
#  include <agrum/base/multidim/utils/patterns/operatorPattern4BaseName.h>
#  undef GUM_MULTI_DIM_OPERATOR_NAME
#  undef GUM_MULTI_DIM_OPERATOR

//
// DO NOT FORGET TO REGISTER YOUR BINARY FUNCTIONS
//

namespace gum {

  // the function used to register all the above functions
  template < typename GUM_SCALAR >
  void operators4MultiDimInit() {
    // ensure that only one thread will register the projections
    static std::once_flag first;
    std::call_once(first, []() {
      std::string MultiDimArrayString("MultiDimArray");
      std::string MultiDimFunctionGraphString("MultiDimFunctionGraph");
      std::string BaseNameString("MultiDimImplementation");

      // register base functions for multiDimArrays
      registerOperator< GUM_SCALAR >("+",
                                     MultiDimArrayString,
                                     MultiDimArrayString,
                                     &add2MultiDimArrays);
      registerOperator< GUM_SCALAR >("-",
                                     MultiDimArrayString,
                                     MultiDimArrayString,
                                     &subtract2MultiDimArrays);
      registerOperator< GUM_SCALAR >("*",
                                     MultiDimArrayString,
                                     MultiDimArrayString,
                                     &multiply2MultiDimArrays);
      registerOperator< GUM_SCALAR >("/",
                                     MultiDimArrayString,
                                     MultiDimArrayString,
                                     &divide2MultiDimArrays);

      // register base functions for multiDimFunctionGraphs
      registerOperator< GUM_SCALAR >("+",
                                     MultiDimFunctionGraphString,
                                     MultiDimFunctionGraphString,
                                     &add2MultiDimFunctionGraphs);
      registerOperator< GUM_SCALAR >("-",
                                     MultiDimFunctionGraphString,
                                     MultiDimFunctionGraphString,
                                     &subtract2MultiDimFunctionGraphs);
      registerOperator< GUM_SCALAR >("*",
                                     MultiDimFunctionGraphString,
                                     MultiDimFunctionGraphString,
                                     &multiply2MultiDimFunctionGraphs);
      registerOperator< GUM_SCALAR >("/",
                                     MultiDimFunctionGraphString,
                                     MultiDimFunctionGraphString,
                                     &divide2MultiDimFunctionGraphs);

      // register default basename functions
      registerOperator< GUM_SCALAR >("+",
                                     BaseNameString,
                                     BaseNameString,
                                     &add2MultiDimImplementations);
      registerOperator< GUM_SCALAR >("-",
                                     BaseNameString,
                                     BaseNameString,
                                     &subtract2MultiDimImplementations);
      registerOperator< GUM_SCALAR >("*",
                                     BaseNameString,
                                     BaseNameString,
                                     &multiply2MultiDimImplementations);
      registerOperator< GUM_SCALAR >("/",
                                     BaseNameString,
                                     BaseNameString,
                                     &divide2MultiDimImplementations);
    });
  }

  // the function used to register all the above functions
  template < typename GUM_SCALAR >
  void pointerOperators4MultiDimInit() {
    // ensure that only one thread will register the projections
    static std::once_flag first;
    std::call_once(first, []() {
      std::string MultiDimArrayString("MultiDimArray");
      std::string BaseNameString("MultiDimImplementation");

      // register base functions for multiDimArrays
      registerOperator< GUM_SCALAR* >("+",
                                      MultiDimArrayString,
                                      MultiDimArrayString,
                                      &add2MultiDimArrays4Pointers);
      registerOperator< GUM_SCALAR* >("-",
                                      MultiDimArrayString,
                                      MultiDimArrayString,
                                      &subtract2MultiDimArrays4Pointers);
      registerOperator< GUM_SCALAR* >("*",
                                      MultiDimArrayString,
                                      MultiDimArrayString,
                                      &multiply2MultiDimArrays4Pointers);
      registerOperator< GUM_SCALAR* >("/",
                                      MultiDimArrayString,
                                      MultiDimArrayString,
                                      &divide2MultiDimArrays4Pointers);
    });
  }

} /* namespace gum */

// remove permission to use operatorsPatterns
#  undef GUM_OPERATOR_PATTERN_ALLOWED

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
