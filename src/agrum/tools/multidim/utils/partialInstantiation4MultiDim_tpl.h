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
 * @brief Implementation for partial instantiation functions.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

// allow partialInstantiationPatterns to be used
#define GUM_PARTIAL_INSTANTIATION_PATTERN_ALLOWED 1

#include <agrum/tools/multidim/utils/partialInstantiationRegister4MultiDim.h>

#define GUM_MULTI_DIM_DECORATOR_PARTIAL_INST(NAME)                        \
  namespace gum {                                                         \
    template < typename GUM_SCALAR >                                      \
    MultiDimImplementation< GUM_SCALAR >*                                 \
       NAME(const MultiDimDecorator< GUM_SCALAR >&           table,       \
            const HashTable< const DiscreteVariable*, Idx >& inst_vars) { \
      const MultiDimImplementation< GUM_SCALAR >* impl = table.content(); \
      return NAME(*impl, inst_vars);                                      \
    }                                                                     \
  }

/// a specialized partial instantiation function for multiDimArrays

#define GUM_MULTI_DIM_PARTIAL_INSTANTIATION_NAME partialInstantiationMultiDimArray
#include <agrum/tools/multidim/utils/patterns/partialInstantiationPattern4MultiDimArray.h>
#undef GUM_MULTI_DIM_PARTIAL_INSTANTIATION_NAME

#define GUM_MULTI_DIM_PARTIAL_INSTANTIATION_IMPL2ARRAY_NAME partialInstantiationMultiDimArray
#include <agrum/tools/multidim/utils/patterns/partialInstantiationPattern4MultiDimArray.h>
#undef GUM_MULTI_DIM_PARTIAL_INSTANTIATION_IMPL2ARRAY_NAME

#define GUM_MULTI_DIM_PARTIAL_INSTANTIATION_POINTER_NAME partialInstantiationMultiDimArray4Pointers
#include <agrum/tools/multidim/utils/patterns/partialInstantiationPattern4MultiDimArray.h>
#undef GUM_MULTI_DIM_PARTIAL_INSTANTIATION_POINTER_NAME

#define GUM_MULTI_DIM_PARTIAL_INSTANTIATION_POINTER_IMPL2ARRAY_NAME \
  partialInstantiationMultiDimArray4Pointers
#include <agrum/tools/multidim/utils/patterns/partialInstantiationPattern4MultiDimArray.h>
#undef GUM_MULTI_DIM_PARTIAL_INSTANTIATION_POINTER_IMPL2ARRAY_NAME

/// default "basename" functions for instantiating MultiDimImplementations

#define GUM_MULTI_DIM_PARTIAL_INSTANTIATION_NAME partialInstantiationMultiDimImplementation
#include <agrum/tools/multidim/utils/patterns/partialInstantiationPattern4BaseName.h>
#undef GUM_MULTI_DIM_PARTIAL_INSTANTIATION_NAME

#define GUM_MULTI_DIM_PARTIAL_INSTANTIATION_POINTER_NAME \
  partialInstantiationMultiDimImplementation4Pointers
#include <agrum/tools/multidim/utils/patterns/partialInstantiationPattern4BaseName.h>
#undef GUM_MULTI_DIM_PARTIAL_INSTANTIATION_NAME

// the operators that should be used to select appropriately the functions
// to partially instantiate multiDims

/// the function to be used to partially instantiate a MultiDimImplementation
#define GUM_MULTI_DIM_PARTIAL_INSTANTIATION_NAME partialInstantiation
#include <agrum/tools/multidim/utils/patterns/partialInstantiationPattern4MultiDimImplementation.h>
#undef GUM_MULTI_DIM_PARTIAL_INSTANTIATION_NAME

/// the function to be used to partially instantiate a MultiDimDecorator
GUM_MULTI_DIM_DECORATOR_PARTIAL_INST(partialInstantiation)

//
// DO NOT FORGET TO REGISTER YOUR BINARY FUNCTIONS
//

namespace gum {

  // the function used to register all the above functions
  template < typename GUM_SCALAR >
  void partialInstantiation4MultiDimInit() {
    // ensure that only one thread will register the projections
    static std::once_flag first;
    std::call_once(first, []() {
      std::string MultiDimArrayString("MultiDimArray");
      std::string MultiDimDecisionDiagramString("MultiDimDecisionDiagram");
      std::string BaseNameString("MultiDimImplementation");

      // register base functions for multiDimArrays
      registerPartialInstantiation< GUM_SCALAR >("i",
                                                 MultiDimArrayString,
                                                 &partialInstantiationMultiDimArray);

      // register default basename functions
      registerPartialInstantiation< GUM_SCALAR >("i",
                                                 BaseNameString,
                                                 &partialInstantiationMultiDimImplementation);
    });
  }

  // the function used to register all the above functions
  template < typename GUM_SCALAR >
  void pointerPartialInstantiation4MultiDimInit() {
    // ensure that only one thread will register the projections
    static std::once_flag first;
    std::call_once(first, []() {
      std::string MultiDimArrayString("MultiDimArray");
      std::string BaseNameString("MultiDimImplementation");

      // register base functions for multiDimArrays
      registerPartialInstantiation< GUM_SCALAR* >("i",
                                                  MultiDimArrayString,
                                                  &partialInstantiationMultiDimArray4Pointers);

      // register default basename functions
      registerPartialInstantiation< GUM_SCALAR* >(
         "i",
         BaseNameString,
         &partialInstantiationMultiDimImplementation4Pointers);
    });
  }

} /* namespace gum */

// remove permission to use operatorsPatterns
#undef GUM_PARTIAL_INSTANTIATION_PATTERN_ALLOWED
