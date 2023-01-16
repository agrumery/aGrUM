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
 * @brief Class used to compute the projection of a function graph
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_MULTI_DIM_FUNCTION_GRAPH_PROJECTOR_H
#define GUM_MULTI_DIM_FUNCTION_GRAPH_PROJECTOR_H

#include <functional>

#include <agrum/tools/multidim/implementations/multiDimFunctionGraph.h>

namespace gum {

  // clang-format off
  /**
   * @class MultiDimFunctionGraphProjector
   * @headerfile multiDimFunctionGraphProjector.h <agrum/tools/multidim/patterns/multiDimFunctionGraphProjector.h>
   * @ingroup multidim_group
   *
   * @brief Class used to perform Function Graph projections
   */
  // clang-format on
  template < typename GUM_SCALAR,
             template < typename >
             class FUNCTOR,
             template < typename > class TerminalNodePolicy = ExactTerminalNodePolicy >
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
    const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* _src_;

    /// The list of variables on which the projection is performed
    const Set< const DiscreteVariable* >& _delVars_;

    /// The resulting function graph
    MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* _rd_;

    /// The function to be performed on the leaves
    const FUNCTOR< GUM_SCALAR > _function_;

    /// The function to be performed on the leaves
    const GUM_SCALAR _neutral_;
  };

#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class MultiDimFunctionGraphProjector< double, std::plus >;
#endif

}   // namespace gum

#include <agrum/tools/multidim/utils/FunctionGraphUtilities/operators/multiDimFunctionGraphProjector_tpl.h>

#endif   // GUM_MULTI_DIM_FUNCTION_GRAPH_PROJECTOR_H
