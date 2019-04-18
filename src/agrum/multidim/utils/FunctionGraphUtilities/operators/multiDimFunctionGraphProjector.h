
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
 * @brief Class used to compute the projection of a function graph
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_MULTI_DIM_FUNCTION_GRAPH_PROJECTOR_H
#define GUM_MULTI_DIM_FUNCTION_GRAPH_PROJECTOR_H

#include <functional>

#include <agrum/multidim/implementations/multiDimFunctionGraph.h>
#include <agrum/multidim/utils/FunctionGraphUtilities/terminalNodePolicies/ExactTerminalNodePolicy.h>

namespace gum {

  // clang-format off
  /**
   * @class MultiDimFunctionGraphProjector
   * @headerfile multiDimFunctionGraphProjector.h <agrum/multidim/patterns/multiDimFunctionGraphProjector.h>
   * @ingroup multidim_group
   *
   * @brief Class used to perform Function Graph projections
   */
  // clang-format on
  template < typename GUM_SCALAR,
             template < typename >
             class FUNCTOR,
             template < typename > class TerminalNodePolicy =
                ExactTerminalNodePolicy >
  class MultiDimFunctionGraphProjector {
    public:
    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /**
     * @brief Default constructor.
     */
    MultiDimFunctionGraphProjector(
       const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* src,
       const Set< const DiscreteVariable* >&                          delVars,
       const GUM_SCALAR                                               neutral);

    /**
     * @brief Default destructor.
     */
    ~MultiDimFunctionGraphProjector();

    /// @}
    // ============================================================================
    /// @name Main Method
    // ============================================================================
    /// @{

    /**
     * @brief Computes and builds the Function Graph that is the result of the
     * Projection.
     */
    MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* project();

    /// @}

    private:
    /// One of the two function graphs used for the Projection
    const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* __src;

    /// The list of variables on which the projection is performed
    const Set< const DiscreteVariable* >& __delVars;

    /// The resulting function graph
    MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* __rd;

    /// The function to be performed on the leaves
    const FUNCTOR< GUM_SCALAR > __function;

    /// The function to be performed on the leaves
    const GUM_SCALAR __neutral;
  };

#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class MultiDimFunctionGraphProjector< double, std::plus >;
#endif

}   // namespace gum

#include <agrum/multidim/utils/FunctionGraphUtilities/operators/multiDimFunctionGraphProjector_tpl.h>

#endif   // GUM_MULTI_DIM_FUNCTION_GRAPH_PROJECTOR_H
