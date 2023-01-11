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
 * @brief Efficient functionals for projecting multiDimensional tables
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6) */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// allow projectionPatterns to be used
#  define GUM_PROJECTION_PATTERN_ALLOWED 1

#  include <agrum/tools/core/functors.h>
#  include <agrum/tools/multidim/utils/operators/projectionRegister4MultiDim.h>

// a specialized max projection function for multiDimArrays

#  define GUM_MULTI_DIM_PROJECTION_NAME projectMaxMultiDimArray
#  define GUM_MULTI_DIM_PROJECTION_EFFECTIVE_TYPE
#  define GUM_MULTI_DIM_PROJECTION(x, y) \
    if (y > x) x = y
#  define GUM_MULTI_DIM_PROJECTION_NEUTRAL std::numeric_limits< GUM_SCALAR >::min()
#  include <agrum/tools/multidim/utils/patterns/projectionPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_PROJECTION_NAME
#  undef GUM_MULTI_DIM_PROJECTION_EFFECTIVE_TYPE
#  undef GUM_MULTI_DIM_PROJECTION
#  undef GUM_MULTI_DIM_PROJECTION_NEUTRAL

#  define GUM_MULTI_DIM_PROJECTION_IMPL2ARRAY_NAME projectMaxMultiDimArray
#  define GUM_MULTI_DIM_PROJECTION_EFFECTIVE_TYPE
#  define GUM_MULTI_DIM_PROJECTION(x, y) \
    if (y > x) x = y
#  define GUM_MULTI_DIM_PROJECTION_NEUTRAL std::numeric_limits< GUM_SCALAR >::min()
#  include <agrum/tools/multidim/utils/patterns/projectionPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_PROJECTION_IMPL2ARRAY_NAME
#  undef GUM_MULTI_DIM_PROJECTION_EFFECTIVE_TYPE
#  undef GUM_MULTI_DIM_PROJECTION
#  undef GUM_MULTI_DIM_PROJECTION_NEUTRAL

#  define GUM_MULTI_DIM_PROJECTION_POINTER_NAME projectMaxMultiDimArray4Pointers
#  define GUM_MULTI_DIM_PROJECTION_EFFECTIVE_TYPE
#  define GUM_MULTI_DIM_PROJECTION(x, y) \
    if (*(y) > *(x)) *(x) = *(y)
#  define GUM_MULTI_DIM_PROJECTION_NEUTRAL std::numeric_limits< GUM_SCALAR >::min()
#  include <agrum/tools/multidim/utils/patterns/projectionPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_PROJECTION_POINTER_NAME
#  undef GUM_MULTI_DIM_PROJECTION_EFFECTIVE_TYPE
#  undef GUM_MULTI_DIM_PROJECTION
#  undef GUM_MULTI_DIM_PROJECTION_NEUTRAL

#  define GUM_MULTI_DIM_PROJECTION_POINTER_IMPL2ARRAY_NAME projectMaxMultiDimArray4Pointers
#  define GUM_MULTI_DIM_PROJECTION_EFFECTIVE_TYPE
#  define GUM_MULTI_DIM_PROJECTION(x, y) \
    if (*(y) > *(x)) *(x) = *(y)
#  define GUM_MULTI_DIM_PROJECTION_NEUTRAL std::numeric_limits< GUM_SCALAR >::min()
#  include <agrum/tools/multidim/utils/patterns/projectionPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_PROJECTION_POINTER_IMPL2ARRAY_NAME
#  undef GUM_MULTI_DIM_PROJECTION_EFFECTIVE_TYPE
#  undef GUM_MULTI_DIM_PROJECTION
#  undef GUM_MULTI_DIM_PROJECTION_NEUTRAL

// a specialized min projection function for multiDimArrays

#  define GUM_MULTI_DIM_PROJECTION_NAME projectMinMultiDimArray
#  define GUM_MULTI_DIM_PROJECTION_EFFECTIVE_TYPE
#  define GUM_MULTI_DIM_PROJECTION(x, y) \
    if (x > y) x = y
#  define GUM_MULTI_DIM_PROJECTION_NEUTRAL std::numeric_limits< GUM_SCALAR >::max()
#  include <agrum/tools/multidim/utils/patterns/projectionPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_PROJECTION_NAME
#  undef GUM_MULTI_DIM_PROJECTION_EFFECTIVE_TYPE
#  undef GUM_MULTI_DIM_PROJECTION
#  undef GUM_MULTI_DIM_PROJECTION_NEUTRAL

#  define GUM_MULTI_DIM_PROJECTION_IMPL2ARRAY_NAME projectMinMultiDimArray
#  define GUM_MULTI_DIM_PROJECTION_EFFECTIVE_TYPE
#  define GUM_MULTI_DIM_PROJECTION(x, y) \
    if (x > y) x = y
#  define GUM_MULTI_DIM_PROJECTION_NEUTRAL std::numeric_limits< GUM_SCALAR >::max()
#  include <agrum/tools/multidim/utils/patterns/projectionPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_PROJECTION_IMPL2ARRAY_NAME
#  undef GUM_MULTI_DIM_PROJECTION_EFFECTIVE_TYPE
#  undef GUM_MULTI_DIM_PROJECTION
#  undef GUM_MULTI_DIM_PROJECTION_NEUTRAL

#  define GUM_MULTI_DIM_PROJECTION_POINTER_NAME projectMinMultiDimArray4Pointers
#  define GUM_MULTI_DIM_PROJECTION_EFFECTIVE_TYPE
#  define GUM_MULTI_DIM_PROJECTION(x, y) \
    if (*(x) > *(y)) *(x) = *(y)
#  define GUM_MULTI_DIM_PROJECTION_NEUTRAL std::numeric_limits< GUM_SCALAR >::max()
#  include <agrum/tools/multidim/utils/patterns/projectionPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_PROJECTION_POINTER_NAME
#  undef GUM_MULTI_DIM_PROJECTION_EFFECTIVE_TYPE
#  undef GUM_MULTI_DIM_PROJECTION
#  undef GUM_MULTI_DIM_PROJECTION_NEUTRAL

#  define GUM_MULTI_DIM_PROJECTION_POINTER_IMPL2ARRAY_NAME projectMinMultiDimArray4Pointers
#  define GUM_MULTI_DIM_PROJECTION_EFFECTIVE_TYPE
#  define GUM_MULTI_DIM_PROJECTION(x, y) \
    if (*(x) > *(y)) *(x) = *(y)
#  define GUM_MULTI_DIM_PROJECTION_NEUTRAL std::numeric_limits< GUM_SCALAR >::max()
#  include <agrum/tools/multidim/utils/patterns/projectionPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_PROJECTION_POINTER_IMPL2ARRAY_NAME
#  undef GUM_MULTI_DIM_PROJECTION_EFFECTIVE_TYPE
#  undef GUM_MULTI_DIM_PROJECTION
#  undef GUM_MULTI_DIM_PROJECTION_NEUTRAL

// a specialized sum projection function for multiDimArrays

#  define GUM_MULTI_DIM_PROJECTION_NAME projectSumMultiDimArray
#  define GUM_MULTI_DIM_PROJECTION_EFFECTIVE_TYPE
#  define GUM_MULTI_DIM_PROJECTION(x, y)   x += y
#  define GUM_MULTI_DIM_PROJECTION_NEUTRAL (GUM_SCALAR)0
#  include <agrum/tools/multidim/utils/patterns/projectionPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_PROJECTION_NAME
#  undef GUM_MULTI_DIM_PROJECTION_EFFECTIVE_TYPE
#  undef GUM_MULTI_DIM_PROJECTION
#  undef GUM_MULTI_DIM_PROJECTION_NEUTRAL

#  define GUM_MULTI_DIM_PROJECTION_IMPL2ARRAY_NAME projectSumMultiDimArray
#  define GUM_MULTI_DIM_PROJECTION_EFFECTIVE_TYPE
#  define GUM_MULTI_DIM_PROJECTION(x, y)   x += y
#  define GUM_MULTI_DIM_PROJECTION_NEUTRAL (GUM_SCALAR)0
#  include <agrum/tools/multidim/utils/patterns/projectionPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_PROJECTION_IMPL2ARRAY_NAME
#  undef GUM_MULTI_DIM_PROJECTION_EFFECTIVE_TYPE
#  undef GUM_MULTI_DIM_PROJECTION
#  undef GUM_MULTI_DIM_PROJECTION_NEUTRAL

#  define GUM_MULTI_DIM_PROJECTION_POINTER_NAME projectSumMultiDimArray4Pointers
#  define GUM_MULTI_DIM_PROJECTION_EFFECTIVE_TYPE
#  define GUM_MULTI_DIM_PROJECTION(x, y)   *(x) += *(y)
#  define GUM_MULTI_DIM_PROJECTION_NEUTRAL (GUM_SCALAR)0
#  include <agrum/tools/multidim/utils/patterns/projectionPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_PROJECTION_POINTER_NAME
#  undef GUM_MULTI_DIM_PROJECTION_EFFECTIVE_TYPE
#  undef GUM_MULTI_DIM_PROJECTION
#  undef GUM_MULTI_DIM_PROJECTION_NEUTRAL

#  define GUM_MULTI_DIM_PROJECTION_POINTER_IMPL2ARRAY_NAME projectSumMultiDimArray4Pointers
#  define GUM_MULTI_DIM_PROJECTION_EFFECTIVE_TYPE
#  define GUM_MULTI_DIM_PROJECTION(x, y)   *(x) += *(y)
#  define GUM_MULTI_DIM_PROJECTION_NEUTRAL (GUM_SCALAR)0
#  include <agrum/tools/multidim/utils/patterns/projectionPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_PROJECTION_POINTER_IMPL2ARRAY_NAME
#  undef GUM_MULTI_DIM_PROJECTION_EFFECTIVE_TYPE
#  undef GUM_MULTI_DIM_PROJECTION
#  undef GUM_MULTI_DIM_PROJECTION_NEUTRAL

// a specialized product projection function for multiDimArrays

#  define GUM_MULTI_DIM_PROJECTION_NAME projectProductMultiDimArray
#  define GUM_MULTI_DIM_PROJECTION_EFFECTIVE_TYPE
#  define GUM_MULTI_DIM_PROJECTION(x, y)   (x) *= (y)
#  define GUM_MULTI_DIM_PROJECTION_NEUTRAL (GUM_SCALAR)1
#  include <agrum/tools/multidim/utils/patterns/projectionPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_PROJECTION_NAME
#  undef GUM_MULTI_DIM_PROJECTION_EFFECTIVE_TYPE
#  undef GUM_MULTI_DIM_PROJECTION
#  undef GUM_MULTI_DIM_PROJECTION_NEUTRAL

#  define GUM_MULTI_DIM_PROJECTION_IMPL2ARRAY_NAME projectProductMultiDimArray
#  define GUM_MULTI_DIM_PROJECTION_EFFECTIVE_TYPE
#  define GUM_MULTI_DIM_PROJECTION(x, y)   (x) *= (y)
#  define GUM_MULTI_DIM_PROJECTION_NEUTRAL (GUM_SCALAR)1
#  include <agrum/tools/multidim/utils/patterns/projectionPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_PROJECTION_IMPL2ARRAY_NAME
#  undef GUM_MULTI_DIM_PROJECTION_EFFECTIVE_TYPE
#  undef GUM_MULTI_DIM_PROJECTION
#  undef GUM_MULTI_DIM_PROJECTION_NEUTRAL

#  define GUM_MULTI_DIM_PROJECTION_POINTER_NAME projectProductMultiDimArray4Pointers
#  define GUM_MULTI_DIM_PROJECTION_EFFECTIVE_TYPE
#  define GUM_MULTI_DIM_PROJECTION(x, y)   (*x) *= (*y)
#  define GUM_MULTI_DIM_PROJECTION_NEUTRAL (GUM_SCALAR)1
#  include <agrum/tools/multidim/utils/patterns/projectionPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_PROJECTION_POINTER_NAME
#  undef GUM_MULTI_DIM_PROJECTION_EFFECTIVE_TYPE
#  undef GUM_MULTI_DIM_PROJECTION
#  undef GUM_MULTI_DIM_PROJECTION_NEUTRAL

#  define GUM_MULTI_DIM_PROJECTION_POINTER_IMPL2ARRAY_NAME projectProductMultiDimArray4Pointers
#  define GUM_MULTI_DIM_PROJECTION_EFFECTIVE_TYPE
#  define GUM_MULTI_DIM_PROJECTION(x, y)   (*x) *= (*y)
#  define GUM_MULTI_DIM_PROJECTION_NEUTRAL (GUM_SCALAR)1
#  include <agrum/tools/multidim/utils/patterns/projectionPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_PROJECTION_POINTER_IMPL2ARRAY_NAME
#  undef GUM_MULTI_DIM_PROJECTION_EFFECTIVE_TYPE
#  undef GUM_MULTI_DIM_PROJECTION
#  undef GUM_MULTI_DIM_PROJECTION_NEUTRAL

// a specialized max projection function for multiDimFunctionGraphs

#  define GUM_MULTI_DIM_PROJECTION_OPERATOR Maximizes
#  define GUM_MULTI_DIM_PROJECTION_NEUTRAL  (-1 * std::numeric_limits< GUM_SCALAR >::max())

#  define GUM_MULTI_DIM_PROJECTION_NAME projectMaxMultiDimFunctionGraph
#  include <agrum/tools/multidim/utils/patterns/projectionPattern4MultiDimFunctionGraph.h>
#  undef GUM_MULTI_DIM_PROJECTION_NAME

#  define GUM_MULTI_DIM_PROJECTION_IMPL2DECISION_GRAPH_NAME projectMaxMultiDimFunctionGraph
#  include <agrum/tools/multidim/utils/patterns/projectionPattern4MultiDimFunctionGraph.h>
#  undef GUM_MULTI_DIM_PROJECTION_IMPL2DECISION_GRAPH_NAME

#  undef GUM_MULTI_DIM_PROJECTION_OPERATOR
#  undef GUM_MULTI_DIM_PROJECTION_NEUTRAL

// a specialized min projection function for multiDimFunctionGraphs

#  define GUM_MULTI_DIM_PROJECTION_OPERATOR Minimizes
#  define GUM_MULTI_DIM_PROJECTION_NEUTRAL  std::numeric_limits< GUM_SCALAR >::max()

#  define GUM_MULTI_DIM_PROJECTION_NAME projectMinMultiDimFunctionGraph
#  include <agrum/tools/multidim/utils/patterns/projectionPattern4MultiDimFunctionGraph.h>
#  undef GUM_MULTI_DIM_PROJECTION_NAME

#  define GUM_MULTI_DIM_PROJECTION_IMPL2DECISION_GRAPH_NAME projectMinMultiDimFunctionGraph
#  include <agrum/tools/multidim/utils/patterns/projectionPattern4MultiDimFunctionGraph.h>
#  undef GUM_MULTI_DIM_PROJECTION_IMPL2DECISION_GRAPH_NAME

#  undef GUM_MULTI_DIM_PROJECTION_OPERATOR
#  undef GUM_MULTI_DIM_PROJECTION_NEUTRAL

// a specialized sum projection function for multiDimFunctionGraphs

#  define GUM_MULTI_DIM_PROJECTION_OPERATOR std::plus
#  define GUM_MULTI_DIM_PROJECTION_NEUTRAL  (GUM_SCALAR)0

#  define GUM_MULTI_DIM_PROJECTION_NAME projectSumMultiDimFunctionGraph
#  include <agrum/tools/multidim/utils/patterns/projectionPattern4MultiDimFunctionGraph.h>
#  undef GUM_MULTI_DIM_PROJECTION_NAME

#  define GUM_MULTI_DIM_PROJECTION_IMPL2DECISION_GRAPH_NAME projectSumMultiDimFunctionGraph
#  include <agrum/tools/multidim/utils/patterns/projectionPattern4MultiDimFunctionGraph.h>
#  undef GUM_MULTI_DIM_PROJECTION_IMPL2DECISION_GRAPH_NAME

#  undef GUM_MULTI_DIM_PROJECTION_OPERATOR
#  undef GUM_MULTI_DIM_PROJECTION_NEUTRAL

// a specialized product projection function for multiDimFunctionGraphs

#  define GUM_MULTI_DIM_PROJECTION_OPERATOR std::multiplies
#  define GUM_MULTI_DIM_PROJECTION_NEUTRAL  (GUM_SCALAR)1

#  define GUM_MULTI_DIM_PROJECTION_NAME projectProductMultiDimFunctionGraph
#  include <agrum/tools/multidim/utils/patterns/projectionPattern4MultiDimFunctionGraph.h>
#  undef GUM_MULTI_DIM_PROJECTION_NAME

#  define GUM_MULTI_DIM_PROJECTION_IMPL2DECISION_GRAPH_NAME projectProductMultiDimFunctionGraph
#  include <agrum/tools/multidim/utils/patterns/projectionPattern4MultiDimFunctionGraph.h>
#  undef GUM_MULTI_DIM_PROJECTION_IMPL2DECISION_GRAPH_NAME

#  undef GUM_MULTI_DIM_PROJECTION_OPERATOR
#  undef GUM_MULTI_DIM_PROJECTION_NEUTRAL

// a specialized function for functionally projecting a multiDimArray

/*
#define GUM_MULTI_DIM_PROJECTION_NAME_F projectFuncMultiDimArray
#define GUM_MULTI_DIM_PROJECTION(x,y) f( (x),(y) )
#define GUM_MULTI_DIM_PROJECTION_NEUTRAL (GUM_SCALAR) 0
#include <agrum/tools/multidim/utils/patterns/projectionPattern4MultiDimArray.h>
#undef GUM_MULTI_DIM_PROJECTION_NAME_F
#undef GUM_MULTI_DIM_PROJECTION
*/

// the operators that should be used to select appropriately the functions
// to project multiDims

// the function to be used to project a MultiDimImplementation using a Max
#  define GUM_MULTI_DIM_PROJECTION_NAME      projectMax
#  define GUM_MULTI_DIM_PROJECTION_FUNC_NAME "max"
#  define GUM_MULTI_DIM_SYMMETRIC_PROJECTION
#  include <agrum/tools/multidim/utils/patterns/projectionPattern4MultiDimImplementation.h>
#  undef GUM_MULTI_DIM_PROJECTION_NAME
#  undef GUM_MULTI_DIM_PROJECTION_FUNC_NAME
#  undef GUM_MULTI_DIM_SYMMETRIC_PROJECTION

// the function to be used to project a MultiDimImplementation using a Min
#  define GUM_MULTI_DIM_PROJECTION_NAME      projectMin
#  define GUM_MULTI_DIM_PROJECTION_FUNC_NAME "min"
#  define GUM_MULTI_DIM_SYMMETRIC_PROJECTION
#  include <agrum/tools/multidim/utils/patterns/projectionPattern4MultiDimImplementation.h>
#  undef GUM_MULTI_DIM_PROJECTION_NAME
#  undef GUM_MULTI_DIM_PROJECTION_FUNC_NAME
#  undef GUM_MULTI_DIM_SYMMETRIC_PROJECTION

// the function to be used to project a MultiDimImplementation using a Sum
#  define GUM_MULTI_DIM_PROJECTION_NAME      projectSum
#  define GUM_MULTI_DIM_PROJECTION_FUNC_NAME "sum"
#  define GUM_MULTI_DIM_SYMMETRIC_PROJECTION
#  include <agrum/tools/multidim/utils/patterns/projectionPattern4MultiDimImplementation.h>
#  undef GUM_MULTI_DIM_PROJECTION_NAME
#  undef GUM_MULTI_DIM_PROJECTION_FUNC_NAME
#  undef GUM_MULTI_DIM_SYMMETRIC_PROJECTION

// the function to be used to project a MultiDimImplementation using a Product
#  define GUM_MULTI_DIM_PROJECTION_NAME      projectProduct
#  define GUM_MULTI_DIM_PROJECTION_FUNC_NAME "product"
#  define GUM_MULTI_DIM_SYMMETRIC_PROJECTION
#  include <agrum/tools/multidim/utils/patterns/projectionPattern4MultiDimImplementation.h>
#  undef GUM_MULTI_DIM_PROJECTION_NAME
#  undef GUM_MULTI_DIM_PROJECTION_FUNC_NAME
#  undef GUM_MULTI_DIM_SYMMETRIC_PROJECTION

// default "basename" functions for projecting MultiDimImplementations

#  define GUM_MULTI_DIM_PROJECTION_NAME    projectMaxMultiDimImplementation
#  define GUM_MULTI_DIM_PROJECTION(x, y)   ((x) > (y) ? (x) : (y))
#  define GUM_MULTI_DIM_PROJECTION_NEUTRAL std::numeric_limits< GUM_SCALAR >::min()
#  include <agrum/tools/multidim/utils/patterns/projectionPattern4BaseName.h>
#  undef GUM_MULTI_DIM_PROJECTION_NAME
#  undef GUM_MULTI_DIM_PROJECTION
#  undef GUM_MULTI_DIM_PROJECTION_NEUTRAL

#  define GUM_MULTI_DIM_PROJECTION_NAME    projectMinMultiDimImplementation
#  define GUM_MULTI_DIM_PROJECTION(x, y)   ((x) > (y) ? (y) : (x))
#  define GUM_MULTI_DIM_PROJECTION_NEUTRAL std::numeric_limits< GUM_SCALAR >::max()
#  include <agrum/tools/multidim/utils/patterns/projectionPattern4BaseName.h>
#  undef GUM_MULTI_DIM_PROJECTION_NAME
#  undef GUM_MULTI_DIM_PROJECTION
#  undef GUM_MULTI_DIM_PROJECTION_NEUTRAL

#  define GUM_MULTI_DIM_PROJECTION_NAME    projectSumMultiDimImplementation
#  define GUM_MULTI_DIM_PROJECTION(x, y)   ((x) + (y))
#  define GUM_MULTI_DIM_PROJECTION_NEUTRAL (GUM_SCALAR)0
#  include <agrum/tools/multidim/utils/patterns/projectionPattern4BaseName.h>
#  undef GUM_MULTI_DIM_PROJECTION_NAME
#  undef GUM_MULTI_DIM_PROJECTION
#  undef GUM_MULTI_DIM_PROJECTION_NEUTRAL

#  define GUM_MULTI_DIM_PROJECTION_NAME    projectProductMultiDimImplementation
#  define GUM_MULTI_DIM_PROJECTION(x, y)   ((x) * (y))
#  define GUM_MULTI_DIM_PROJECTION_NEUTRAL (GUM_SCALAR)1
#  include <agrum/tools/multidim/utils/patterns/projectionPattern4BaseName.h>
#  undef GUM_MULTI_DIM_PROJECTION_NAME
#  undef GUM_MULTI_DIM_PROJECTION
#  undef GUM_MULTI_DIM_PROJECTION_NEUTRAL

//
// DO NOT FORGET TO REGISTER YOUR BINARY FUNCTIONS
//

namespace gum {
  // the function used to register all the above functions
  template < typename GUM_SCALAR >
  void projections4MultiDimInit() {
    // ensure that only one thread will register the projections
    static std::once_flag first;
    std::call_once(first, []() {
      std::string MultiDimArrayString("MultiDimArray");
      std::string MultiDimFunctionGraphString("MultiDimFunctionGraph");
      std::string BaseNameString("MultiDimImplementation");

      // register base functions for multiDimArrays
      registerProjection< GUM_SCALAR >("max", MultiDimArrayString, &projectMaxMultiDimArray);
      registerProjection< GUM_SCALAR >("min", MultiDimArrayString, &projectMinMultiDimArray);
      registerProjection< GUM_SCALAR >("sum", MultiDimArrayString, &projectSumMultiDimArray);
      registerProjection< GUM_SCALAR >("product",
                                       MultiDimArrayString,
                                       &projectProductMultiDimArray);

      // register base functions for multiDimArrays
      registerProjection< GUM_SCALAR >("max",
                                       MultiDimFunctionGraphString,
                                       &projectMaxMultiDimFunctionGraph);
      registerProjection< GUM_SCALAR >("min",
                                       MultiDimFunctionGraphString,
                                       &projectMinMultiDimFunctionGraph);
      registerProjection< GUM_SCALAR >("sum",
                                       MultiDimFunctionGraphString,
                                       &projectSumMultiDimFunctionGraph);
      registerProjection< GUM_SCALAR >("product",
                                       MultiDimFunctionGraphString,
                                       &projectProductMultiDimFunctionGraph);

      // register default basename functions
      registerProjection< GUM_SCALAR >("max", BaseNameString, &projectMaxMultiDimImplementation);
      registerProjection< GUM_SCALAR >("min", BaseNameString, &projectMinMultiDimImplementation);
      registerProjection< GUM_SCALAR >("sum", BaseNameString, &projectSumMultiDimImplementation);
      registerProjection< GUM_SCALAR >("product",
                                       BaseNameString,
                                       &projectProductMultiDimImplementation);
    });
  }

  // the function used to register all the above functions
  template < typename GUM_SCALAR >
  void pointerProjections4MultiDimInit() {
    // ensure that only one thread will register the projections
    static std::once_flag first;
    std::call_once(first, []() {
      std::string MultiDimArrayString("MultiDimArray");
      std::string BaseNameString("MultiDimImplementation");

      // register base functions for multiDimArrays
      registerProjection< GUM_SCALAR* >("max",
                                        MultiDimArrayString,
                                        &projectMaxMultiDimArray4Pointers);
      registerProjection< GUM_SCALAR* >("min",
                                        MultiDimArrayString,
                                        &projectMinMultiDimArray4Pointers);
      registerProjection< GUM_SCALAR* >("sum",
                                        MultiDimArrayString,
                                        &projectSumMultiDimArray4Pointers);
      registerProjection< GUM_SCALAR* >("product",
                                        MultiDimArrayString,
                                        &projectProductMultiDimArray4Pointers);
    });
  }

} /* namespace gum */

// remove permission to use operatorsPatterns
#  undef GUM_PROJECTION_PATTERN_ALLOWED

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
