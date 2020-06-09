
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN(@LIP6)
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#include <agrum/tools/multidim/utils/FunctionGraphUtilities/internalNode.h>
#include <agrum/tools/multidim/utils/FunctionGraphUtilities/operators/treeOperator.h>

#define ALLOCATE(x)      SmallObjectAllocator::instance().allocate(x)
#define DEALLOCATE(x, y) SmallObjectAllocator::instance().deallocate(x, y)

namespace gum {

  template < typename GUM_SCALAR,
             template < typename >
             class COMBINEOPERATOR,
             template < typename >
             class TerminalNodePolicy >
  INLINE
     TreeOperator< GUM_SCALAR, COMBINEOPERATOR, TerminalNodePolicy >::TreeOperator(
        const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* dt1,
        const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* dt2) :
      dt1__(dt1),
      dt2__(dt2), combine__() {
    GUM_CONSTRUCTOR(TreeOperator);

    rd__ =
       MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >::getTreeInstance();
  }

  template < typename GUM_SCALAR,
             template < typename >
             class COMBINEOPERATOR,
             template < typename >
             class TerminalNodePolicy >
  INLINE
     TreeOperator< GUM_SCALAR, COMBINEOPERATOR, TerminalNodePolicy >::TreeOperator(
        const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* dt1,
        const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* dt2,
        const HashTable< const DiscreteVariable*, Idx > givenContext) :
      dt1__(dt1),
      dt2__(dt2), combine__(), context__(givenContext) {
    GUM_CONSTRUCTOR(TreeOperator);

    rd__ =
       MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >::getTreeInstance();
  }

  template < typename GUM_SCALAR,
             template < typename >
             class COMBINEOPERATOR,
             template < typename >
             class TerminalNodePolicy >
  INLINE TreeOperator< GUM_SCALAR, COMBINEOPERATOR, TerminalNodePolicy >::
     ~TreeOperator() {
    GUM_DESTRUCTOR(TreeOperator);
  }

  // This function is the main function. To be call every time an operation
  // between the two given Function Graphs is required
  template < typename GUM_SCALAR,
             template < typename >
             class COMBINEOPERATOR,
             template < typename >
             class TerminalNodePolicy >
  INLINE MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >*
         TreeOperator< GUM_SCALAR, COMBINEOPERATOR, TerminalNodePolicy >::compute() {
    rd__->manager()->setRootNode(xPloreDT1__(dt1__->root()));

    return rd__;
  }

  // Main recursion function, called every time we move on a node to determine
  // what we have to do
  template < typename GUM_SCALAR,
             template < typename >
             class COMBINEOPERATOR,
             template < typename >
             class TerminalNodePolicy >
  INLINE NodeId
     TreeOperator< GUM_SCALAR, COMBINEOPERATOR, TerminalNodePolicy >::xPloreDT1__(
        NodeId currentNodeId) {
    if (dt1__->isTerminalNode(currentNodeId)) {
      curDT1Leaf__ = currentNodeId;
      return xPloreDT2__(dt2__->root());
    }

    const InternalNode* currentNode = dt1__->node(currentNodeId);

    if (!rd__->variablesSequence().exists(currentNode->nodeVar()))
      rd__->add(*(currentNode->nodeVar()));

    NodeId* sonsMap = static_cast< NodeId* >(
       ALLOCATE(sizeof(NodeId) * currentNode->nodeVar()->domainSize()));
    for (Idx moda = 0; moda < currentNode->nodeVar()->domainSize(); ++moda) {
      context__.insert(currentNode->nodeVar(), moda);
      sonsMap[moda] = xPloreDT1__(currentNode->son(moda));
      context__.erase(currentNode->nodeVar());
    }
    return checkRedundancy__(currentNode->nodeVar(), sonsMap);
  }

  template < typename GUM_SCALAR,
             template < typename >
             class COMBINEOPERATOR,
             template < typename >
             class TerminalNodePolicy >
  INLINE NodeId
     TreeOperator< GUM_SCALAR, COMBINEOPERATOR, TerminalNodePolicy >::xPloreDT2__(
        NodeId currentNodeId) {
    if (dt2__->isTerminalNode(currentNodeId))
      return rd__->manager()->addTerminalNode(combine__(
         dt1__->nodeValue(curDT1Leaf__), dt2__->nodeValue(currentNodeId)));

    const InternalNode* currentNode = dt2__->node(currentNodeId);

    if (!rd__->variablesSequence().exists(currentNode->nodeVar()))
      rd__->add(*(currentNode->nodeVar()));

    if (context__.exists(currentNode->nodeVar()))
      return xPloreDT2__(currentNode->son(context__[currentNode->nodeVar()]));

    NodeId* sonsMap = static_cast< NodeId* >(
       ALLOCATE(sizeof(NodeId) * currentNode->nodeVar()->domainSize()));
    for (Idx moda = 0; moda < currentNode->nodeVar()->domainSize(); ++moda) {
      context__.insert(currentNode->nodeVar(), moda);
      sonsMap[moda] = xPloreDT2__(currentNode->son(moda));
      context__.erase(currentNode->nodeVar());
    }
    return checkRedundancy__(currentNode->nodeVar(), sonsMap);
  }

  template < typename GUM_SCALAR,
             template < typename >
             class COMBINEOPERATOR,
             template < typename >
             class TerminalNodePolicy >
  INLINE NodeId TreeOperator< GUM_SCALAR, COMBINEOPERATOR, TerminalNodePolicy >::
     checkRedundancy__(const DiscreteVariable* var, NodeId* sonsMap) {
    bool diff = false;
    for (Idx moda = 1; moda < var->domainSize() && !diff; ++moda)
      if (sonsMap[0] != sonsMap[moda]) diff = true;

    if (!diff) {
      NodeId zero = sonsMap[0];
      DEALLOCATE(sonsMap, sizeof(NodeId) * var->domainSize());
      return zero;
    }

    return rd__->manager()->addInternalNode(var, sonsMap);
  }

}   // namespace gum
