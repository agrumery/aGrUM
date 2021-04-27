/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @author Pierre-Henri WUILLEMIN(@LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(@AMU)
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#include <agrum/tools/multidim/utils/FunctionGraphUtilities/internalNode.h>
#include <agrum/tools/multidim/utils/FunctionGraphUtilities/operators/treeOperator.h>
#include <agrum/tools/multidim/utils/FunctionGraphUtilities/operators/treeRegress.h>

#define ALLOCATE(x)      SmallObjectAllocator::instance().allocate(x)
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
      vFunc__(qAction),
      pxi__(pxi) {
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
    if (vFunc__->isTerminalNode(vFunc__->root())) {
      ret = MultiDimFunctionGraph< GUM_SCALAR,
                                   TerminalNodePolicy >::getTreeInstance();
      ret->copy(*vFunc__);
    } else
      ret = xPloreVFunc__(vFunc__->root());
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
                  TerminalNodePolicy >::xPloreVFunc__(NodeId currentNodeId) {
    const InternalNode* currentNode = vFunc__->node(currentNodeId);

    std::vector< MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* >
       varbucket;

    for (Idx moda = 0; moda < currentNode->nodeVar()->domainSize(); ++moda) {
      MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* vpxi = nullptr;
      context__.insert(currentNode->nodeVar(), moda);
      if (vFunc__->isTerminalNode(currentNode->son(moda))) {
        GUM_SCALAR value = vFunc__->nodeValue(currentNode->son(moda));
        if (value) {
          vpxi = MultiDimFunctionGraph< GUM_SCALAR,
                                        TerminalNodePolicy >::getTreeInstance();
          vpxi->manager()->setRootNode(vpxi->manager()->addTerminalNode(value));
        }
      } else {
        vpxi = xPloreVFunc__(currentNode->son(moda));
      }

      if (vpxi != nullptr) {
        TreeOperator< GUM_SCALAR, COMBINEOPERATOR > combinope(
           vpxi,
           pxi__.second(currentNode->nodeVar()),
           context__);
        varbucket.push_back(combinope.compute());
      }
      delete vpxi;
      context__.erase(currentNode->nodeVar());
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
