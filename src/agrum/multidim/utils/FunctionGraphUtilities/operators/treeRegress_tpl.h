
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
 * @brief Class used to compute the operation between two decision diagrams
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/multidim/utils/FunctionGraphUtilities/internalNode.h>
#include <agrum/multidim/utils/FunctionGraphUtilities/operators/treeOperator.h>
#include <agrum/multidim/utils/FunctionGraphUtilities/operators/treeRegress.h>

#define ALLOCATE(x) SmallObjectAllocator::instance().allocate(x)
#define DEALLOCATE(x, y) SmallObjectAllocator::instance().deallocate(x, y)

namespace gum {

  template < typename GUM_SCALAR,
             template < typename >
             class COMBINEOPERATOR,
             template < typename >
             class PROJECTOPERATOR,
             template < typename >
             class TerminalNodePolicy >
  TreeRegress< GUM_SCALAR, COMBINEOPERATOR, PROJECTOPERATOR, TerminalNodePolicy >::
     TreeRegress(
        const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* qAction,
        const Bijection<
           const DiscreteVariable*,
           const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* > pxi) :
      __vFunc(qAction),
      __pxi(pxi) {
    GUM_CONSTRUCTOR(TreeRegress);
  }

  template < typename GUM_SCALAR,
             template < typename >
             class COMBINEOPERATOR,
             template < typename >
             class PROJECTOPERATOR,
             template < typename >
             class TerminalNodePolicy >
  TreeRegress< GUM_SCALAR, COMBINEOPERATOR, PROJECTOPERATOR, TerminalNodePolicy >::
     ~TreeRegress() {
    GUM_DESTRUCTOR(TreeRegress);
  }

  // This function is the main function. To be call every time an operation
  // between the two given Function Graphs is required
  template < typename GUM_SCALAR,
             template < typename >
             class COMBINEOPERATOR,
             template < typename >
             class PROJECTOPERATOR,
             template < typename >
             class TerminalNodePolicy >
  MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >*
     TreeRegress< GUM_SCALAR,
                  COMBINEOPERATOR,
                  PROJECTOPERATOR,
                  TerminalNodePolicy >::compute() {
    MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* ret;
    if (__vFunc->isTerminalNode(__vFunc->root())) {
      ret = MultiDimFunctionGraph< GUM_SCALAR,
                                   TerminalNodePolicy >::getTreeInstance();
      ret->copy(*__vFunc);
    } else
      ret = __xPloreVFunc(__vFunc->root());
    return ret;
  }


  template < typename GUM_SCALAR,
             template < typename >
             class COMBINEOPERATOR,
             template < typename >
             class PROJECTOPERATOR,
             template < typename >
             class TerminalNodePolicy >
  MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >*
     TreeRegress< GUM_SCALAR,
                  COMBINEOPERATOR,
                  PROJECTOPERATOR,
                  TerminalNodePolicy >::__xPloreVFunc(NodeId currentNodeId) {
    const InternalNode* currentNode = __vFunc->node(currentNodeId);

    std::vector< MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* >
       varbucket;

    for (Idx moda = 0; moda < currentNode->nodeVar()->domainSize(); ++moda) {
      MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* vpxi = nullptr;
      __context.insert(currentNode->nodeVar(), moda);
      if (__vFunc->isTerminalNode(currentNode->son(moda))) {
        GUM_SCALAR value = __vFunc->nodeValue(currentNode->son(moda));
        if (value) {
          vpxi = MultiDimFunctionGraph< GUM_SCALAR,
                                        TerminalNodePolicy >::getTreeInstance();
          vpxi->manager()->setRootNode(vpxi->manager()->addTerminalNode(value));
        }
      } else {
        vpxi = __xPloreVFunc(currentNode->son(moda));
      }

      if (vpxi != nullptr) {
        TreeOperator< GUM_SCALAR, COMBINEOPERATOR > combinope(
           vpxi, __pxi.second(currentNode->nodeVar()), __context);
        varbucket.push_back(combinope.compute());
      }
      delete vpxi;
      __context.erase(currentNode->nodeVar());
    }

    if (varbucket.empty()) return nullptr;

    MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* vp = varbucket.back();
    varbucket.pop_back();
    while (!varbucket.empty()) {
      MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* temp = vp;
      TreeOperator< GUM_SCALAR, PROJECTOPERATOR > projope(vp, varbucket.back());
      vp = projope.compute();
      delete temp;
      temp = varbucket.back();
      varbucket.pop_back();
      delete temp;
    }

    return vp;
  }

}   // namespace gum
