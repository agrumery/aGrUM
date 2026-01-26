/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#pragma once


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

#define ALLOCATE(x)      SmallObjectAllocator::instance().allocate(x)
#define DEALLOCATE(x, y) SmallObjectAllocator::instance().deallocate(x, y)

namespace gum {

  template < typename GUM_SCALAR,
             template < typename > class COMBINEOPERATOR,
             template < typename > class TerminalNodePolicy >
  INLINE TreeOperator< GUM_SCALAR, COMBINEOPERATOR, TerminalNodePolicy >::TreeOperator(
      const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* dt1,
      const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* dt2) :
      _dt1_(dt1), _dt2_(dt2), _combine_() {
    GUM_CONSTRUCTOR(TreeOperator);

    _rd_ = MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >::getTreeInstance();
  }

  template < typename GUM_SCALAR,
             template < typename > class COMBINEOPERATOR,
             template < typename > class TerminalNodePolicy >
  INLINE TreeOperator< GUM_SCALAR, COMBINEOPERATOR, TerminalNodePolicy >::TreeOperator(
      const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* dt1,
      const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* dt2,
      const HashTable< const DiscreteVariable*, Idx >                givenContext) :
      _dt1_(dt1), _dt2_(dt2), _combine_(), _context_(givenContext) {
    GUM_CONSTRUCTOR(TreeOperator);

    _rd_ = MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >::getTreeInstance();
  }

  template < typename GUM_SCALAR,
             template < typename > class COMBINEOPERATOR,
             template < typename > class TerminalNodePolicy >
  INLINE TreeOperator< GUM_SCALAR, COMBINEOPERATOR, TerminalNodePolicy >::~TreeOperator() {
    GUM_DESTRUCTOR(TreeOperator);
  }

  // This function is the main function. To be call every time an operation
  // between the two given Function Graphs is required
  template < typename GUM_SCALAR,
             template < typename > class COMBINEOPERATOR,
             template < typename > class TerminalNodePolicy >
  INLINE MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >*
         TreeOperator< GUM_SCALAR, COMBINEOPERATOR, TerminalNodePolicy >::compute() {
    _rd_->manager()->setRootNode(_xPloreDT1_(_dt1_->root()));

    return _rd_;
  }

  // Main recursion function, called every time we move on a node to determine
  // what we have to do
  template < typename GUM_SCALAR,
             template < typename > class COMBINEOPERATOR,
             template < typename > class TerminalNodePolicy >
  INLINE NodeId TreeOperator< GUM_SCALAR, COMBINEOPERATOR, TerminalNodePolicy >::_xPloreDT1_(
      NodeId currentNodeId) {
    if (_dt1_->isTerminalNode(currentNodeId)) {
      _curDT1Leaf_ = currentNodeId;
      return _xPloreDT2_(_dt2_->root());
    }

    const InternalNode* currentNode = _dt1_->node(currentNodeId);

    if (!_rd_->variablesSequence().exists(currentNode->nodeVar()))
      _rd_->add(*(currentNode->nodeVar()));

    NodeId* sonsMap
        = static_cast< NodeId* >(ALLOCATE(sizeof(NodeId) * currentNode->nodeVar()->domainSize()));
    for (Idx moda = 0; moda < currentNode->nodeVar()->domainSize(); ++moda) {
      _context_.insert(currentNode->nodeVar(), moda);
      sonsMap[moda] = _xPloreDT1_(currentNode->son(moda));
      _context_.erase(currentNode->nodeVar());
    }
    return _checkRedundancy_(currentNode->nodeVar(), sonsMap);
  }

  template < typename GUM_SCALAR,
             template < typename > class COMBINEOPERATOR,
             template < typename > class TerminalNodePolicy >
  INLINE NodeId TreeOperator< GUM_SCALAR, COMBINEOPERATOR, TerminalNodePolicy >::_xPloreDT2_(
      NodeId currentNodeId) {
    if (_dt2_->isTerminalNode(currentNodeId))
      return _rd_->manager()->addTerminalNode(
          _combine_(_dt1_->nodeValue(_curDT1Leaf_), _dt2_->nodeValue(currentNodeId)));

    const InternalNode* currentNode = _dt2_->node(currentNodeId);

    if (!_rd_->variablesSequence().exists(currentNode->nodeVar()))
      _rd_->add(*(currentNode->nodeVar()));

    if (_context_.exists(currentNode->nodeVar()))
      return _xPloreDT2_(currentNode->son(_context_[currentNode->nodeVar()]));

    NodeId* sonsMap
        = static_cast< NodeId* >(ALLOCATE(sizeof(NodeId) * currentNode->nodeVar()->domainSize()));
    for (Idx moda = 0; moda < currentNode->nodeVar()->domainSize(); ++moda) {
      _context_.insert(currentNode->nodeVar(), moda);
      sonsMap[moda] = _xPloreDT2_(currentNode->son(moda));
      _context_.erase(currentNode->nodeVar());
    }
    return _checkRedundancy_(currentNode->nodeVar(), sonsMap);
  }

  template < typename GUM_SCALAR,
             template < typename > class COMBINEOPERATOR,
             template < typename > class TerminalNodePolicy >
  INLINE NodeId TreeOperator< GUM_SCALAR, COMBINEOPERATOR, TerminalNodePolicy >::_checkRedundancy_(
      const DiscreteVariable* var,
      NodeId*                 sonsMap) {
    bool diff = false;
    for (Idx moda = 1; moda < var->domainSize() && !diff; ++moda)
      if (sonsMap[0] != sonsMap[moda]) diff = true;

    if (!diff) {
      NodeId zero = sonsMap[0];
      DEALLOCATE(sonsMap, sizeof(NodeId) * var->domainSize());
      return zero;
    }

    return _rd_->manager()->addInternalNode(var, sonsMap);
  }

}   // namespace gum
