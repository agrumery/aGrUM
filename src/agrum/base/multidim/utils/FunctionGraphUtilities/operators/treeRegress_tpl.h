/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Class used to compute the operation between two decision diagrams
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/base/multidim/utils/FunctionGraphUtilities/internalNode.h>
#include <agrum/base/multidim/utils/FunctionGraphUtilities/operators/treeOperator.h>
#include <agrum/base/multidim/utils/FunctionGraphUtilities/operators/treeRegress.h>

#define ALLOCATE(x)      SmallObjectAllocator::instance().allocate(x)
#define DEALLOCATE(x, y) SmallObjectAllocator::instance().deallocate(x, y)

namespace gum {

  template < typename GUM_SCALAR,
             template < typename > class COMBINEOPERATOR,
             template < typename > class PROJECTOPERATOR,
             template < typename > class TerminalNodePolicy >
  TreeRegress< GUM_SCALAR, COMBINEOPERATOR, PROJECTOPERATOR, TerminalNodePolicy >::TreeRegress(
      const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >*                    qAction,
      const Bijection< const DiscreteVariable*,
                       const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* > pxi) :
      _vFunc_(qAction), _pxi_(pxi) {
    GUM_CONSTRUCTOR(TreeRegress);
  }

  template < typename GUM_SCALAR,
             template < typename > class COMBINEOPERATOR,
             template < typename > class PROJECTOPERATOR,
             template < typename > class TerminalNodePolicy >
  TreeRegress< GUM_SCALAR, COMBINEOPERATOR, PROJECTOPERATOR, TerminalNodePolicy >::~TreeRegress() {
    GUM_DESTRUCTOR(TreeRegress);
  }

  // This function is the main function. To be call every time an operation
  // between the two given Function Graphs is required
  template < typename GUM_SCALAR,
             template < typename > class COMBINEOPERATOR,
             template < typename > class PROJECTOPERATOR,
             template < typename > class TerminalNodePolicy >
  MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >*
      TreeRegress< GUM_SCALAR, COMBINEOPERATOR, PROJECTOPERATOR, TerminalNodePolicy >::compute() {
    MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* ret;
    if (_vFunc_->isTerminalNode(_vFunc_->root())) {
      ret = MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >::getTreeInstance();
      ret->copy(*_vFunc_);
    } else ret = _xPloreVFunc_(_vFunc_->root());
    return ret;
  }

  template < typename GUM_SCALAR,
             template < typename > class COMBINEOPERATOR,
             template < typename > class PROJECTOPERATOR,
             template < typename > class TerminalNodePolicy >
  MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >*
      TreeRegress< GUM_SCALAR, COMBINEOPERATOR, PROJECTOPERATOR, TerminalNodePolicy >::
          _xPloreVFunc_(NodeId currentNodeId) {
    const InternalNode* currentNode = _vFunc_->node(currentNodeId);

    std::vector< MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* > varbucket;

    for (Idx moda = 0; moda < currentNode->nodeVar()->domainSize(); ++moda) {
      MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* vpxi = nullptr;
      _context_.insert(currentNode->nodeVar(), moda);
      if (_vFunc_->isTerminalNode(currentNode->son(moda))) {
        GUM_SCALAR value = _vFunc_->nodeValue(currentNode->son(moda));
        if (value) {
          vpxi = MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >::getTreeInstance();
          vpxi->manager()->setRootNode(vpxi->manager()->addTerminalNode(value));
        }
      } else {
        vpxi = _xPloreVFunc_(currentNode->son(moda));
      }

      if (vpxi != nullptr) {
        TreeOperator< GUM_SCALAR, COMBINEOPERATOR > combinope(vpxi,
                                                              _pxi_.second(currentNode->nodeVar()),
                                                              _context_);
        varbucket.push_back(combinope.compute());
      }
      delete vpxi;
      _context_.erase(currentNode->nodeVar());
    }

    if (varbucket.empty()) return nullptr;

    MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* vp = varbucket.back();
    varbucket.pop_back();
    while (!varbucket.empty()) {
      MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* temp = vp;
      TreeOperator< GUM_SCALAR, PROJECTOPERATOR >              projope(vp, varbucket.back());
      vp = projope.compute();
      delete temp;
      temp = varbucket.back();
      varbucket.pop_back();
      delete temp;
    }

    return vp;
  }

}   // namespace gum
