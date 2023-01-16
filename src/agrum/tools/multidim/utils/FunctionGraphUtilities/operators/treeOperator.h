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
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_TREE_OPERATOR_H
#define GUM_TREE_OPERATOR_H

#include <functional>

#include <agrum/tools/multidim/implementations/multiDimFunctionGraph.h>

namespace gum {

  /**
   * @class TreeOperator
   * @headerfile treeOperator.h <agrum/tools/multidim/patterns/treeOperator.h>
   * @ingroup multidim_group
   *
   * @brief Class used to perform Decision Tree Operation in the FMDP Framework
   */
  template < typename GUM_SCALAR,
             template < typename >
             class COMBINEOPERATOR,
             template < typename > class TerminalNodePolicy = ExactTerminalNodePolicy >
  class TreeOperator {
    public:
    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /**
     * @brief Default constructor.
     */
    TreeOperator(const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* dt1,
                 const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* dt2);

    /**
     * @brief Default constructor.
     */
    TreeOperator(const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* dt1,
                 const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* dt2,
                 const HashTable< const DiscreteVariable*, Idx >                givenContext);

    /**
     * @brief Default destructor.
     */
    ~TreeOperator();

    /// @}
    // ============================================================================
    /// @name Main Method
    // ============================================================================
    /// @{

    /// Computes and builds the Function Graph that is the result of the
    /// operation
    MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* compute();

    /// @}

    private:
    /// The main recursion function
    NodeId _xPloreDT1_(NodeId currentNodeId);

    /// The main recursion function
    NodeId _xPloreDT2_(NodeId currentNodeId);

    NodeId _checkRedundancy_(const DiscreteVariable*, NodeId*);

    /// The two function graphs used for the operation
    const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* _dt1_;
    const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* _dt2_;

    /// The resulting function graph
    MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* _rd_;

    /// The function to be performed on the leaves
    const COMBINEOPERATOR< GUM_SCALAR > _combine_;

    HashTable< const DiscreteVariable*, Idx > _context_;
    NodeId                                    _curDT1Leaf_;
  };

#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class TreeOperator< double, std::plus >;
#endif

}   // namespace gum

#include <agrum/tools/multidim/utils/FunctionGraphUtilities/operators/treeOperator_tpl.h>

#endif   // GUM_OPERATOR_H
