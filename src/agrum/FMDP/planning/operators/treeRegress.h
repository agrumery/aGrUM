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
*/

// =======================================================
#ifndef GUM_TREE_REGRESS_H
#define GUM_TREE_REGRESS_H
// =======================================================
#include <agrum/multidim/multiDimDecisionGraph.h>
#include <agrum/multidim/decisionGraphUtilities/terminalNodePolicies/ExactTerminalNodePolicy.h>
// =======================================================

namespace gum {

/**
 * @class TreeRegress treeRegress.h <agrum/multidim/patterns/treeRegress.h>
 * @brief Class used to perform Decision Tree Regression in the FMDP Framework
 * @ingroup multidim_group
 *
 *
 *
 */

  template <typename GUM_SCALAR,
            template <typename> class COMBINEOPERATOR,
            template <typename> class PROJECTOPERATOR,
            template <typename> class TerminalNodePolicy = ExactTerminalNodePolicy >
  class TreeRegress
  {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

      // ============================================================================
      /// Default constructor.
      // ============================================================================
      TreeRegress(const MultiDimDecisionGraph< GUM_SCALAR, TerminalNodePolicy>* qAction,
                  const Bijection<const DiscreteVariable*, const MultiDimDecisionGraph<GUM_SCALAR, TerminalNodePolicy> *> pxi,
                  const Sequence<const DiscreteVariable*> xip,
                  const GUM_SCALAR neutral );

      // ============================================================================
      /// Default destructor.
      // ============================================================================
      ~TreeRegress();

    /// @}

    // ############################################################################
    /// @name Main Method
    // ############################################################################
    /// @{

      // ============================================================================
      /// Computes and builds the Decision Graph that is the result of the operation
      // ============================================================================
      MultiDimDecisionGraph<GUM_SCALAR, TerminalNodePolicy> *compute();

    /// @}


    private :

      // ============================================================================
      /// The main recursion function
      // ============================================================================
      NodeId __xPlorePxi( const MultiDimDecisionGraph<GUM_SCALAR, TerminalNodePolicy>* xPloredTree,
                          NodeId currentNodeId,
                          Idx seqPos );

      // ============================================================================
      /// The main recursion function
      // ============================================================================
      GUM_SCALAR __xPloreVFunc( NodeId currentNodeId );

      NodeId __checkRedundancy( const DiscreteVariable*, NodeId*) ;

      // ============================================================================
      /// The decision graphs used for the operation
      // ============================================================================
      const MultiDimDecisionGraph<GUM_SCALAR, TerminalNodePolicy>* __vFunc;
      const Bijection<const DiscreteVariable*, const MultiDimDecisionGraph<GUM_SCALAR, TerminalNodePolicy>*> __pxi;

      // ============================================================================
      /// The two decision graphs used for the operation
      // ============================================================================
      const Sequence<const DiscreteVariable*> __xip;

      // ============================================================================
      /// The resulting decision graph
      // ============================================================================
      MultiDimDecisionGraph<GUM_SCALAR, TerminalNodePolicy>* __rd;

      // ============================================================================
      /// The neutral value for the projector operation
      // ============================================================================
      const GUM_SCALAR __neutral;

      // ============================================================================
      /// The functions to be performed on the leaves
      // ============================================================================
      const COMBINEOPERATOR<GUM_SCALAR> __combine;
      const PROJECTOPERATOR<GUM_SCALAR> __project;

      HashTable<const MultiDimDecisionGraph<GUM_SCALAR, TerminalNodePolicy>*, NodeId> __curLeafMap;
      HashTable<const DiscreteVariable*, Idx> __context;
  };

} // namespace gum

#include <agrum/FMDP/planning/operators/treeRegress.tcc>

#endif // GUM_REGRESS_H
