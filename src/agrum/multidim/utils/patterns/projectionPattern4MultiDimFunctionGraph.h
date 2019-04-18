
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @brief the pattern used by all the projections of multidimensional tables
 *
 * @author Jean-Christophe MAGNAN
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/multidim/instantiation.h>

// check if we allowed these patterns to be used
#ifndef GUM_PROJECTION_PATTERN_ALLOWED

// #warning To use projectionPattern, you must define
// GUM_PROJECTION_PATTERN_ALLOWED

#else

#  include <agrum/multidim/implementations/multiDimFunctionGraph.h>
#  include <agrum/multidim/implementations/multiDimImplementation.h>
#  include <agrum/multidim/utils/FunctionGraphUtilities/operators/multiDimFunctionGraphProjector.h>

namespace gum {

  // a specialized function for projecting a multiDimFunctionGraph over a subset
  // of its vars

#  ifdef GUM_MULTI_DIM_PROJECTION_NAME
#    define GUM_MULTI_DIM_PROJECTION_TYPE GUM_SCALAR
  template < typename GUM_SCALAR >
  MultiDimFunctionGraph< GUM_SCALAR >* GUM_MULTI_DIM_PROJECTION_NAME(
     const MultiDimFunctionGraph< GUM_SCALAR >* table,
     const Set< const DiscreteVariable* >&      delVars)
#  endif


#  ifdef GUM_MULTI_DIM_PROJECTION_IMPL2DECISION_GRAPH_NAME
#    define GUM_MULTI_DIM_PROJECTION_TYPE GUM_SCALAR
     template < typename GUM_SCALAR >
     MultiDimImplementation< GUM_SCALAR >* GUM_MULTI_DIM_PROJECTION_IMPL2DECISION_GRAPH_NAME(
        const MultiDimImplementation< GUM_SCALAR >* ttable,
        const Set< const DiscreteVariable* >&       delVars)
#  endif
  {

#  ifdef GUM_MULTI_DIM_PROJECTION_IMPL2DECISION_GRAPH_NAME
    const MultiDimFunctionGraph< GUM_SCALAR >* table =
       reinterpret_cast< const MultiDimFunctionGraph< GUM_SCALAR >* >(ttable);
#  endif

    MultiDimFunctionGraphProjector< GUM_MULTI_DIM_PROJECTION_TYPE,
                                    GUM_MULTI_DIM_PROJECTION_OPERATOR >
       proj(table, delVars, GUM_MULTI_DIM_PROJECTION_NEUTRAL);

    return proj.project();
  }


#  undef GUM_MULTI_DIM_PROJECTION_TYPE
}   // namespace gum
#endif /* GUM_PROJECTION_PATTERN_ALLOWED */
