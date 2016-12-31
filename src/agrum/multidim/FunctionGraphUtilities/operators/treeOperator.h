/****************************************************************************
 *  Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES     *
 *  {prenom.nom}_at_lip6.fr                                                 *
 *                                                                          *
 *  This program is free software; you can redistribute it and/or modify    *
 *  it under the terms of the GNU General Public License as published by    *
 *  the Free Software Foundation; either version 2 of the License, or       *
 *  (at your option) any later version.                                     *
 *                                                                          *
 *  This program is distributed in the hope that it will be useful,         *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the            *
 *  GNU General Public License for more details.                            *
 *                                                                          *
 *  You should have received a copy of the GNU General Public License       *
 *  along with this program; if not, write to the                           *
 *  Free Software Foundation, Inc.,                                         *
 *  59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.                *
 ****************************************************************************/
/**
 * @file
 * @brief Class used to compute the operation between two decision diagrams
 *
 * @author Jean-Christophe Magnan
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_TREE_OPERATOR_H
#define GUM_TREE_OPERATOR_H

#include <functional>

#include <agrum/multidim/FunctionGraphUtilities/terminalNodePolicies/ExactTerminalNodePolicy.h>
#include <agrum/multidim/multiDimFunctionGraph.h>

namespace gum {

  /**
   * @class TreeOperator treeOperator.h <agrum/multidim/patterns/treeOperator.h>
   * @ingroup multidim_group
   *
   * @brief Class used to perform Decision Tree Operation in the FMDP Framework
   */
  template <typename GUM_SCALAR,
            template <typename> class COMBINEOPERATOR,
            template <typename> class TerminalNodePolicy = ExactTerminalNodePolicy>
  class TreeOperator {
    public:
    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /**
     * @brief Default constructor.
     */
    TreeOperator(
        const MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>* dt1,
        const MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>* dt2 );

    /**
     * @brief Default constructor.
     */
    TreeOperator( const MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>* dt1,
                  const MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>* dt2,
                  const HashTable<const DiscreteVariable*, Idx> givenContext );

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
    MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>* compute();

    /// @}

    private:
    /// The main recursion function
    NodeId __xPloreDT1( NodeId currentNodeId );

    /// The main recursion function
    NodeId __xPloreDT2( NodeId currentNodeId );

    NodeId __checkRedundancy( const DiscreteVariable*, NodeId* );

    /// The two function graphs used for the operation
    const MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>* __dt1;
    const MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>* __dt2;

    /// The resulting function graph
    MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>* __rd;

    /// The function to be performed on the leaves
    const COMBINEOPERATOR<GUM_SCALAR> __combine;

    HashTable<const DiscreteVariable*, Idx> __context;
    NodeId __curDT1Leaf;
  };

  extern template class TreeOperator<double, std::plus>;

}  // namespace gum

#include <agrum/multidim/FunctionGraphUtilities/operators/treeOperator_tpl.h>

#endif  // GUM_OPERATOR_H
