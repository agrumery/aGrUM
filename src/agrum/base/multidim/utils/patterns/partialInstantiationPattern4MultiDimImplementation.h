/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
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
