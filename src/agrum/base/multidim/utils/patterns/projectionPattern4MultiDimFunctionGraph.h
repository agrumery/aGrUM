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
 * @brief the pattern used by all the projections of multidimensional tables
 *
 * @author Jean-Christophe MAGNAN
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */


// check if we allowed these patterns to be used
#ifndef GUM_PROJECTION_PATTERN_ALLOWED

// #warning To use projectionPattern, you must define
// GUM_PROJECTION_PATTERN_ALLOWED

#else

#  include <agrum/base/multidim/utils/FunctionGraphUtilities/operators/multiDimFunctionGraphProjector.h>

namespace gum {

  // a specialized function for projecting a multiDimFunctionGraph over a subset
  // of its vars

#  ifdef GUM_MULTI_DIM_PROJECTION_NAME
#    define GUM_MULTI_DIM_PROJECTION_TYPE GUM_SCALAR

  template < typename GUM_SCALAR >
  MultiDimFunctionGraph< GUM_SCALAR >*
      GUM_MULTI_DIM_PROJECTION_NAME(const MultiDimFunctionGraph< GUM_SCALAR >* table,
                                    const gum::VariableSet&                    delVars)
#  endif


#  ifdef GUM_MULTI_DIM_PROJECTION_IMPL2DECISION_GRAPH_NAME
#    define GUM_MULTI_DIM_PROJECTION_TYPE GUM_SCALAR
          template < typename GUM_SCALAR >
          MultiDimImplementation< GUM_SCALAR >* GUM_MULTI_DIM_PROJECTION_IMPL2DECISION_GRAPH_NAME(
              const MultiDimImplementation< GUM_SCALAR >* ttable,
              const gum::VariableSet&                     delVars)
#  endif
  {

#  ifdef GUM_MULTI_DIM_PROJECTION_IMPL2DECISION_GRAPH_NAME
    const MultiDimFunctionGraph< GUM_SCALAR >* table
        = reinterpret_cast< const MultiDimFunctionGraph< GUM_SCALAR >* >(ttable);
#  endif

    MultiDimFunctionGraphProjector< GUM_MULTI_DIM_PROJECTION_TYPE,
                                    GUM_MULTI_DIM_PROJECTION_OPERATOR >
        proj(table, delVars, GUM_MULTI_DIM_PROJECTION_NEUTRAL);

    return proj.project();
  }

#  undef GUM_MULTI_DIM_PROJECTION_TYPE
}   // namespace gum
#endif /* GUM_PROJECTION_PATTERN_ALLOWED */
