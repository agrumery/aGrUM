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
 * @brief Class used to compute the operation between two decision diagrams
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */

#ifndef GUM_MULTI_DIM_FUNCTION_GRAPH_OPERATOR_H
#define GUM_MULTI_DIM_FUNCTION_GRAPH_OPERATOR_H

#include <functional>

#include <agrum/tools/multidim/utils/FunctionGraphUtilities/operators/o4DGContext.h>

namespace gum {

  // clang-format off
  /**
   * @class MultiDimFunctionGraphOperator
   * @headerfile multiDimFunctionGraphOperator.h <agrum/tools/multidim/patterns/multiDimFunctionGraphOperator.h>
   * @ingroup multidim_group
   *
   * @brief Class used to perform Function Graph Operations
   */
  // clang-format on
  template < typename GUM_SCALAR,
             template < typename >
             class FUNCTOR,
             template < typename > class TerminalNodePolicy = ExactTerminalNodePolicy >
  class MultiDimFunctionGraphOperator {
    public:
    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /**
     * @brief Default constructor.
     */
    MultiDimFunctionGraphOperator(
       const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* DG1,
       const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* DG2);

    /**
     * @brief Default destructor.
     */
    ~MultiDimFunctionGraphOperator();

    /// @}
    // ============================================================================
    /// @name Main Method
    // ============================================================================
    /// @{

    /**
     * @brief Computes and builds the Function Graph that is the result of the
     * operation.
     */
    MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* compute();

    /// @}

    Idx nbCall();
    Idx nbVarRetro();
    Idx sizeVarRetroDomain();

    private:
    Idx _nbCall_;
    Idx _nbVarRetro_;
    Idx _sizeVarRetro_;

    /// Computes an order for the final Decision graph that will minimize the
    /// number of re exploration
    void _establishVarOrder_();

    /// Heuristic methods to decide which of two retrograde variables should
    /// come first
    Idx _distance_(const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >*,
                   const DiscreteVariable*,
                   const DiscreteVariable*);

    /// Establish for each node in both function graph if it has retrograde
    /// variables beneath it
    void
       _findRetrogradeVariables_(const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* dg,
                                 HashTable< NodeId, short int* >& dgInstNeed);

    /// The main recursion function
    NodeId _compute_(O4DGContext& currentSituation, Idx lastInstVarPos);

    /// One of the two function graphs used for the operation
    const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* _DG1_;

    /// The other one
    const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* _DG2_;

    /// The resulting function graph
    MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* _rd_;

    /// The total number of variable implied in the operation
    Idx _nbVar_;

    /// The function to be performed on the leaves
    const FUNCTOR< GUM_SCALAR > _function_;

    /// The hashtable used to know if two pair of nodes have already been
    /// visited
    HashTable< double, NodeId > _explorationTable_;

    /// Table uses to know if a given node of first function graph has
    /// retrograde vrariables
    HashTable< NodeId, short int* > _DG1InstantiationNeeded_;

    /// Table uses to know if a given node of second function graph has
    /// retrograde vrariables
    HashTable< NodeId, short int* > _DG2InstantiationNeeded_;

    /// Just a comptuationnal trick
    short int* _default_;
  };

#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class MultiDimFunctionGraphOperator< double, std::plus >;
#endif

}   // namespace gum

#include <agrum/tools/multidim/utils/FunctionGraphUtilities/operators/multiDimFunctionGraphOperator_tpl.h>

#endif   // GUM_MULTI_DIM_FUNCTION_GRAPH_OPERATOR_H
