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
 *
 * @brief the pattern used by all the partial instantiations of
 * multidimensional tables
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

// check if we allowed these patterns to be used
#ifndef GUM_PARTIAL_INSTANTIATION_PATTERN_ALLOWED

// #warning To use partialIntantiationPattern4MultiDimImplementation.h, you must
// define GUM_PARTIAL_INSTANTIATION_PATTERN_ALLOWED

#else
namespace gum {

#  ifdef GUM_MULTI_DIM_PARTIAL_INSTANTIATION_NAME
  template < typename GUM_SCALAR >
  MultiDimImplementation< GUM_SCALAR >* GUM_MULTI_DIM_PARTIAL_INSTANTIATION_NAME(
     const MultiDimImplementation< GUM_SCALAR >&      ttable,
     const HashTable< const DiscreteVariable*, Idx >& inst_vars) {
#  endif

    typename PartialInstantiationRegister4MultiDim< GUM_SCALAR >::PartialInstantiationPtr func;

    // get the appropriate function to perform the operation
    try {
      // try to find func(ttable,inst_vars) in the register
      func
         = PartialInstantiationRegister4MultiDim< GUM_SCALAR >::Register().get("i", ttable.name());
    } catch (NotFound const&) {
      func = PartialInstantiationRegister4MultiDim< GUM_SCALAR >::Register().get("i",
                                                                                 ttable.basename());
    }

    // perform the partial instantiation
    return func(&ttable, inst_vars);
  }

} /* End of namespace gum */

#endif /* GUM_PARTIAL_INSTANTIATION_PATTERN_ALLOWED */
