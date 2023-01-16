/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


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

#  include <agrum/tools/core/functors.h>

#  include <agrum/tools/multidim/utils/FunctionGraphUtilities/operators/multiDimFunctionGraphOperator.h>

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
