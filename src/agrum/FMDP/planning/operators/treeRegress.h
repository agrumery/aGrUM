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
#include <agrum/multidim/decisionGraphUtilities/o4DGContext.h>
#include <agrum/multidim/decisionGraphUtilities/terminalNodePolicies/ExactTerminalNodePolicy.h>
// =======================================================

namespace gum {

/**
 * @class TreeRegress treeRegress.h <agrum/multidim/patterns/treeRegress.h>
 * @brief Class used to perform Decision Graph Operations in the FMDP Framework
 * @ingroup multidim_group
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
      TreeRegress( Set<const MultiDimDecisionGraph<GUM_SCALAR, TerminalNodePolicy>*> treeSet,
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
      NodeId __compute(  );



      // ============================================================================
      /// One of the two decision graphs used for the operation
      // ============================================================================
      Set<const MultiDimDecisionGraph<GUM_SCALAR, TerminalNodePolicy>*> __treeSet;

      // ============================================================================
      /// The resulting decision graph
      // ============================================================================
      MultiDimDecisionGraph<GUM_SCALAR, TerminalNodePolicy>* __rd;

      // ============================================================================
      /// The neutral value for the projector operation
      // ============================================================================
      const GUM_SCALAR __neutral;

      // ============================================================================
      /// The total number of variable implied in the operation
      // ============================================================================
      Idx __nbVar;

      // ============================================================================
      /// The functions to be performed on the leaves
      // ============================================================================
      const COMBINEOPERATOR<GUM_SCALAR> __combine;
      const PROJECTOPERATOR<GUM_SCALAR> __project;

      // ============================================================================
      /// The hashtable used to know if two pair of nodes have already been visited
      // ============================================================================
      //HashTable<std::pair<NodeId,NodeId>, NodeId> __explorationTable;
  };

} // namespace gum

#include <agrum/FMDP/planning/treeRegress.tcc>

#endif // GUM_REGRESS_H
