
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
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
 * @brief A container for registering complete projection functions on
 * multiDimImplementations, i.e., projections over all variables
 *
 * @author Christophe GONZALES (@AMU) and Pierre-Henri WUILLEMIN (@LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/agrum.h>

#  include <agrum/tools/multidim/utils/operators/completeProjectionRegister4MultiDim.h>

namespace gum {

  // adds a new entry into the register
  template < typename GUM_SCALAR >
  void CompleteProjectionRegister4MultiDim< GUM_SCALAR >::insert(
     const std::string& projection_name,
     const std::string& type_multidim,
     typename CompleteProjectionRegister4MultiDim<
        GUM_SCALAR >::CompleteProjectionPtr newFunction) {
    // insert the new entry
    CompleteProjectionSet* theset;

    if (!__set.exists(projection_name)) {
      theset = __set.insert(projection_name, new CompleteProjectionSet).second;
#  ifdef GUM_DEBUG_MODE
      // for debugging purposes, we should inform the aGrUM's debugger that
      // the hashtable contained within the CompleteProjectionRegister4MultiDim
      // will be removed at the end of the program's execution.
      __debug__::__inc_deletion(
         "HashTable", __FILE__, __LINE__, "destructor of", (void*)theset);
#  endif /* GUM_DEBUG_MODE */
    } else {
      theset = __set[projection_name];
    }

    theset->insert(type_multidim, newFunction);
  }

  // removes a given entry from the register
  template < typename GUM_SCALAR >
  void CompleteProjectionRegister4MultiDim< GUM_SCALAR >::erase(
     const std::string& projection_name, const std::string& type_multidim) {
    if (!__set.exists(projection_name)) return;

    CompleteProjectionSet* theset = __set[projection_name];

    theset->erase(type_multidim);
  }

  // indicates whether a given entry exists in the register
  template < typename GUM_SCALAR >
  INLINE bool CompleteProjectionRegister4MultiDim< GUM_SCALAR >::exists(
     const std::string& projection_name, const std::string& type_multidim) const {
    if (!__set.exists(projection_name)) return false;

    return __set[projection_name].exists(type_multidim);
  }

  /** @brief returns the specialized operator assigned to a given subtype of
   * MultiDimImplementation */
  template < typename GUM_SCALAR >
  INLINE typename CompleteProjectionRegister4MultiDim<
     GUM_SCALAR >::CompleteProjectionPtr
     CompleteProjectionRegister4MultiDim< GUM_SCALAR >::get(
        const std::string& projection_name,
        const std::string& type_multidim) const {
    CompleteProjectionSet* theset = __set[projection_name];
    return theset->        operator[](type_multidim);
  }

  // a named constructor that constructs one and only one Register per data
  // type
  template < typename GUM_SCALAR >
  CompleteProjectionRegister4MultiDim< GUM_SCALAR >&
     CompleteProjectionRegister4MultiDim< GUM_SCALAR >::Register() {
    static CompleteProjectionRegister4MultiDim container;

#  ifdef GUM_DEBUG_MODE
    static bool first = true;

    if (first) {
      first = false;
      // for debugging purposes, we should inform the aGrUM's debugger that
      // the hashtable contained within the CompleteProjectionRegister4MultiDim
      // will be removed at the end of the program's execution.
      __debug__::__inc_deletion("HashTable",
                                __FILE__,
                                __LINE__,
                                "destructor of",
                                (void*)&container.__set);
    }

#  endif /* GUM_DEBUG_MODE */

    return container;
  }

  // Default constructor: creates an empty register
  template < typename GUM_SCALAR >
  CompleteProjectionRegister4MultiDim<
     GUM_SCALAR >::CompleteProjectionRegister4MultiDim() {}

  // destructor
  template < typename GUM_SCALAR >
  CompleteProjectionRegister4MultiDim<
     GUM_SCALAR >::~CompleteProjectionRegister4MultiDim() {
    // remove all the sets
    for (typename HashTable< std::string, CompleteProjectionSet* >::iterator_safe
            iter = __set.beginSafe();
         iter != __set.endSafe();
         ++iter)
      delete iter.val();
  }

  // a function to more easily register new projection functions in MultiDims
  template < typename GUM_SCALAR >
  void
     registerCompleteProjection(const std::string& projection_name,
                                const std::string& type_multidim,
                                typename CompleteProjectionRegister4MultiDim<
                                   GUM_SCALAR >::CompleteProjectionPtr function) {
    CompleteProjectionRegister4MultiDim< GUM_SCALAR >::Register().insert(
       projection_name, type_multidim, function);
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */