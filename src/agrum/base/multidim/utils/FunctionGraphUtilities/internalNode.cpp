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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Methods of the MultiDimFunctionGraph InternalNode class.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */

#include <agrum/base/multidim/utils/FunctionGraphUtilities/internalNode.h>

#define ALLOCATE(x)      SmallObjectAllocator::instance().allocate(x)
#define DEALLOCATE(x, y) SmallObjectAllocator::instance().deallocate(x, y)

namespace gum {

  // ============================================================================
  // Constructors
  // ============================================================================
  InternalNode::InternalNode() {
    GUM_CONSTRUCTOR(InternalNode);
    _nodeVar_ = nullptr;
  }

  InternalNode::InternalNode(const DiscreteVariable* v) {
    GUM_CONSTRUCTOR(InternalNode);
    _setNodeVar_(v);
  }

  InternalNode::InternalNode(const DiscreteVariable* v, NodeId* sons) {
    GUM_CONSTRUCTOR(InternalNode);
    _nodeVar_  = v;
    _nodeSons_ = sons;
  }

  // ============================================================================
  // Destructors
  // ============================================================================
  InternalNode::~InternalNode() {
    GUM_DESTRUCTOR(InternalNode);
    if (_nodeVar_ != nullptr) deallocateNodeSons(_nodeVar_, _nodeSons_);
  }

  // ============================================================================
  // Allocates a table of nodeid of the size given in parameter
  // ============================================================================
  NodeId* InternalNode::allocateNodeSons(const DiscreteVariable* v) {
    NodeId* sons = static_cast< NodeId* >(ALLOCATE(sizeof(NodeId) * v->domainSize()));
    for (gum::Idx i = 0; i < v->domainSize(); ++i)
      sons[i] = 0;
    return sons;
  }

  // ============================================================================
  // Deallocates a NodeSons table
  // ============================================================================
  void InternalNode::deallocateNodeSons(const DiscreteVariable* v, NodeId* s) {
    DEALLOCATE(s, sizeof(NodeId) * v->domainSize());
  }

  // ============================================================================
  // Node handlers
  // ============================================================================
  void InternalNode::setNode(const DiscreteVariable* v, NodeId* sons) {
    if (_nodeVar_ != nullptr) deallocateNodeSons(_nodeVar_, _nodeSons_);
    _nodeVar_  = v;
    _nodeSons_ = sons;
  }

  // ============================================================================
  // Var handlers
  // ============================================================================
  void InternalNode::setNodeVar(const DiscreteVariable* v) {
    if (_nodeVar_ != nullptr) deallocateNodeSons(_nodeVar_, _nodeSons_);
    _setNodeVar_(v);
  }

  void InternalNode::_setNodeVar_(const DiscreteVariable* v) {
    _nodeVar_  = v;
    _nodeSons_ = allocateNodeSons(v);
  }

  // ============================================================================
  // Parent handlers
  // ============================================================================
  void InternalNode::addParent(NodeId parent, Idx modality) {
    _nodeParents_.addLink(Parent(parent, modality));
  }

  void InternalNode::removeParent(NodeId parent, Idx modality) {
    _nodeParents_.searchAndRemoveLink(Parent(parent, modality));
  }

}   // namespace gum

#ifdef GUM_NO_INLINE
#  include <agrum/base/multidim/utils/FunctionGraphUtilities/internalNode_inl.h>
#endif /* GUM_NO_INLINE */
