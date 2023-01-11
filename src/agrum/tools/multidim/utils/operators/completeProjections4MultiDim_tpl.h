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
 * @brief Efficient functionals for projecting multidim tables over all their
 * variables
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// allow projectionPatterns to be used
#  define GUM_COMPLETE_PROJECTION_PATTERN_ALLOWED 1

#  include <agrum/tools/multidim/utils/operators/completeProjectionRegister4MultiDim.h>

// a specialized max projection function for multiDimArrays

#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_NAME projectMaxMultiDimArray
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION(x, y) \
    if (y > x) x = y
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL std::numeric_limits< GUM_SCALAR >::lowest()
#  include <agrum/tools/multidim/utils/patterns/completeProjectionPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_NAME
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL

#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_IMPL2ARRAY_NAME projectMaxMultiDimArray
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION(x, y) \
    if (y > x) x = y
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL std::numeric_limits< GUM_SCALAR >::lowest()
#  include <agrum/tools/multidim/utils/patterns/completeProjectionPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_IMPL2ARRAY_NAME
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL

#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER_NAME projectMaxMultiDimArray4Pointers
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION(x, y) \
    if (*(y) > *(x)) *(x) = *(y)
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL std::numeric_limits< GUM_SCALAR >::lowest()
#  include <agrum/tools/multidim/utils/patterns/completeProjectionPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER_NAME
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL

#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER_IMPL2ARRAY_NAME projectMaxMultiDimArray4Pointers
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION(x, y) \
    if (*(y) > *(x)) *(x) = *(y)
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL std::numeric_limits< GUM_SCALAR >::lowest()
#  include <agrum/tools/multidim/utils/patterns/completeProjectionPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER_IMPL2ARRAY_NAME
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL

// a specialized min projection function for multiDimArrays

#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_NAME projectMinMultiDimArray
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION(x, y) \
    if (x > y) x = y
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL std::numeric_limits< GUM_SCALAR >::max()
#  include <agrum/tools/multidim/utils/patterns/completeProjectionPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_NAME
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL

#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_IMPL2ARRAY_NAME projectMinMultiDimArray
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION(x, y) \
    if (x > y) x = y
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL std::numeric_limits< GUM_SCALAR >::max()
#  include <agrum/tools/multidim/utils/patterns/completeProjectionPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_IMPL2ARRAY_NAME
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL

#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER_NAME projectMinMultiDimArray4Pointers
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION(x, y) \
    if (*(x) > *(y)) *(x) = *(y)
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL std::numeric_limits< GUM_SCALAR >::max()
#  include <agrum/tools/multidim/utils/patterns/completeProjectionPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER_NAME
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL

#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER_IMPL2ARRAY_NAME projectMinMultiDimArray4Pointers
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION(x, y) \
    if (*(x) > *(y)) *(x) = *(y)
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL std::numeric_limits< GUM_SCALAR >::max()
#  include <agrum/tools/multidim/utils/patterns/completeProjectionPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER_IMPL2ARRAY_NAME
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL

// a specialized sum projection function for multiDimArrays

#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_NAME    projectSumMultiDimArray
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION(x, y)   x += y
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL (GUM_SCALAR)0
#  include <agrum/tools/multidim/utils/patterns/completeProjectionPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_NAME
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL

#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_IMPL2ARRAY_NAME projectSumMultiDimArray
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION(x, y)           x += y
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL         (GUM_SCALAR)0
#  include <agrum/tools/multidim/utils/patterns/completeProjectionPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_IMPL2ARRAY_NAME
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL

#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER_NAME projectSumMultiDimArray4Pointers
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION(x, y)        *(x) += *(y)
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL      (GUM_SCALAR)0
#  include <agrum/tools/multidim/utils/patterns/completeProjectionPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER_NAME
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL

#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER_IMPL2ARRAY_NAME projectSumMultiDimArray4Pointers
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION(x, y)                   *(x) += *(y)
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL                 (GUM_SCALAR)0
#  include <agrum/tools/multidim/utils/patterns/completeProjectionPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER_IMPL2ARRAY_NAME
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL

// a specialized product projection function for multiDimArrays

#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_NAME    projectProductMultiDimArray
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION(x, y)   (x) *= (y)
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL (GUM_SCALAR)1
#  include <agrum/tools/multidim/utils/patterns/completeProjectionPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_NAME
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL

#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_IMPL2ARRAY_NAME projectProductMultiDimArray
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION(x, y)           (x) *= (y)
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL         (GUM_SCALAR)1
#  include <agrum/tools/multidim/utils/patterns/completeProjectionPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_IMPL2ARRAY_NAME
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL

#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER_NAME projectProductMultiDimArray4Pointers
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION(x, y)        (*x) *= (*y)
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL      (GUM_SCALAR)1
#  include <agrum/tools/multidim/utils/patterns/completeProjectionPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER_NAME
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL

#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER_IMPL2ARRAY_NAME \
    projectProductMultiDimArray4Pointers
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION(x, y)   (*x) *= (*y)
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL (GUM_SCALAR)1
#  include <agrum/tools/multidim/utils/patterns/completeProjectionPattern4MultiDimArray.h>
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_POINTER_IMPL2ARRAY_NAME
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL

// the operators that should be used to select appropriately the functions
// to project multiDims

// the function to be used to project a MultiDimImplementation using a Max
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_NAME      projectMax
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_FUNC_NAME "max"
#  include <agrum/tools/multidim/utils/patterns/completeProjectionPattern4MultiDimImplementation.h>
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_NAME
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_FUNC_NAME

// the function to be used to project a MultiDimImplementation using a Min
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_NAME      projectMin
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_FUNC_NAME "min"
#  include <agrum/tools/multidim/utils/patterns/completeProjectionPattern4MultiDimImplementation.h>
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_NAME
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_FUNC_NAME

// the function to be used to project a MultiDimImplementation using a Sum
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_NAME      projectSum
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_FUNC_NAME "sum"
#  include <agrum/tools/multidim/utils/patterns/completeProjectionPattern4MultiDimImplementation.h>
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_NAME
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_FUNC_NAME

// the function to be used to project a MultiDimImplementation using a Product
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_NAME      projectProduct
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_FUNC_NAME "product"
#  include <agrum/tools/multidim/utils/patterns/completeProjectionPattern4MultiDimImplementation.h>
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_NAME
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_FUNC_NAME

// default "basename" functions for projecting MultiDimImplementations

#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_NAME projectMaxMultiDimImplementation
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION(x, y) \
    if (y > x) x = y
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL std::numeric_limits< GUM_SCALAR >::lowest()
#  include <agrum/tools/multidim/utils/patterns/completeProjectionPattern4BaseName.h>
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_NAME
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL

#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_NAME projectMinMultiDimImplementation
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION(x, y) \
    if (x > y) x = y
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL std::numeric_limits< GUM_SCALAR >::max()
#  include <agrum/tools/multidim/utils/patterns/completeProjectionPattern4BaseName.h>
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_NAME
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL

#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_NAME    projectSumMultiDimImplementation
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION(x, y)   x += y
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL (GUM_SCALAR)0
#  include <agrum/tools/multidim/utils/patterns/completeProjectionPattern4BaseName.h>
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_NAME
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL

#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_NAME    projectProductMultiDimImplementation
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION(x, y)   x *= y
#  define GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL (GUM_SCALAR)1
#  include <agrum/tools/multidim/utils/patterns/completeProjectionPattern4BaseName.h>
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_NAME
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION
#  undef GUM_MULTI_DIM_COMPLETE_PROJECTION_NEUTRAL

//
// DO NOT FORGET TO REGISTER YOUR BINARY FUNCTIONS
//

namespace gum {

  // the function used to register all the above functions
  template < typename GUM_SCALAR >
  void completeProjections4MultiDimInit() {
    // ensure that only one thread will register the projections
    static std::once_flag first;
    std::call_once(first, []() {
      std::string MultiDimArrayString("MultiDimArray");
      std::string BaseNameString("MultiDimImplementation");

      // register base functions for multiDimArrays
      registerCompleteProjection< GUM_SCALAR >("max",
                                               MultiDimArrayString,
                                               &projectMaxMultiDimArray);
      registerCompleteProjection< GUM_SCALAR >("min",
                                               MultiDimArrayString,
                                               &projectMinMultiDimArray);
      registerCompleteProjection< GUM_SCALAR >("sum",
                                               MultiDimArrayString,
                                               &projectSumMultiDimArray);
      registerCompleteProjection< GUM_SCALAR >("product",
                                               MultiDimArrayString,
                                               &projectProductMultiDimArray);

      // register default basename functions
      registerCompleteProjection< GUM_SCALAR >("max",
                                               BaseNameString,
                                               &projectMaxMultiDimImplementation);
      registerCompleteProjection< GUM_SCALAR >("min",
                                               BaseNameString,
                                               &projectMinMultiDimImplementation);
      registerCompleteProjection< GUM_SCALAR >("sum",
                                               BaseNameString,
                                               &projectSumMultiDimImplementation);
      registerCompleteProjection< GUM_SCALAR >("product",
                                               BaseNameString,
                                               &projectProductMultiDimImplementation);
    });
  }

  // the function used to register all the above functions
  template < typename GUM_SCALAR >
  void pointerCompleteProjections4MultiDimInit() {
    // ensure that only one thread will register the projections
    static std::once_flag first;
    std::call_once(first, []() {
      std::string MultiDimArrayString("MultiDimArray");
      std::string BaseNameString("MultiDimImplementation");

      // register base functions for multiDimArrays
      registerCompleteProjection< GUM_SCALAR* >("max",
                                                MultiDimArrayString,
                                                &projectMaxMultiDimArray4Pointers);
      registerCompleteProjection< GUM_SCALAR* >("min",
                                                MultiDimArrayString,
                                                &projectMinMultiDimArray4Pointers);
      registerCompleteProjection< GUM_SCALAR* >("sum",
                                                MultiDimArrayString,
                                                &projectSumMultiDimArray4Pointers);
      registerCompleteProjection< GUM_SCALAR* >("product",
                                                MultiDimArrayString,
                                                &projectProductMultiDimArray4Pointers);
    });
  }

} /* namespace gum */

// remove permission to use operatorsPatterns
#  undef GUM_COMPLETE_PROJECTION_PATTERN_ALLOWED

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
