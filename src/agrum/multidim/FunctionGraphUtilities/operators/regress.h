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
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 * @author Jean-Christophe Magnan
 */

#ifndef GUM_REGRESS_H
#define GUM_REGRESS_H

#include <agrum/multidim/FunctionGraphUtilities/operators/o4DGContext.h>
#include <agrum/multidim/FunctionGraphUtilities/terminalNodePolicies/ExactTerminalNodePolicy.h>
#include <agrum/multidim/multiDimFunctionGraph.h>

namespace gum {

  /**
   * @class Regress regress.h <agrum/multidim/patterns/regress.h>
   * @brief Class used to perform Function Graph Operations in the FMDP
   * Framework
   * @ingroup multidim_group
   */
  template <typename GUM_SCALAR,
            template <typename> class COMBINEOPERATOR,
            template <typename> class PROJECTOPERATOR,
            template <typename> class TerminalNodePolicy = ExactTerminalNodePolicy>
  class Regress {
    public:
    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /// Default constructor.
    Regress(
        const MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>* vfunction,
        const MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>* probDist,
        const Set<const DiscreteVariable*>* primedVars,
        const DiscreteVariable*             targetVar,
        const GUM_SCALAR                    neutral );

    /// Default destructor.
    ~Regress();

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
    /// Computes an order for the final Decision graph that will minimize the
    /// number
    /// of re exploration
    void __establishVarOrder();

    /// Establish for each node in both function graph if it has retrograde
    /// variables
    /// beneath it
    void __findRetrogradeVariables(
        const MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>* dg,
        HashTable<NodeId, short int*>&                               dgInstNeed );

    /// The main recursion function
    NodeId __compute( O4DGContext& currentSituation, Idx lastInstVarPos );

    /// One of the two function graphs used for the operation
    const MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>* __DG1;

    /// The other one
    const MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>* __DG2;

    /// The resulting function graph
    MultiDimFunctionGraph<GUM_SCALAR, TerminalNodePolicy>* __rd;

    /// The set of variables we want to keep at the end
    const Set<const DiscreteVariable*>* __primedVars;

    /// The variable we work on to eleminate
    const DiscreteVariable* __targetVar;

    /// The function to be performed on the leaves
    const GUM_SCALAR __neutral;

    /// The total number of variable implied in the operation
    Idx __nbVar;

    /// The functions to be performed on the leaves
    const COMBINEOPERATOR<GUM_SCALAR> __combine;
    const PROJECTOPERATOR<GUM_SCALAR> __project;

    /// The hashtable used to know if two pair of nodes have already been
    /// visited
    HashTable<double, NodeId> __explorationTable;

    /// Table uses to know if a given node of given function graph has
    /// retrograde variables
    HashTable<NodeId, short int*> __DG1InstantiationNeeded;
    HashTable<NodeId, short int*> __DG2InstantiationNeeded;

    /// Just a computationnal trick
    short int* __default;
  };

  extern template class Regress<double, std::multiplies, std::plus>;

}  // namespace gum

#include <agrum/multidim/FunctionGraphUtilities/operators/regress_tpl.h>

#endif  // GUM_REGRESS_H
