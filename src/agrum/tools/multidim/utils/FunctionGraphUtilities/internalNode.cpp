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
 * @brief Methods of the MultiDimFunctionGraph InternalNode class.
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 * @author Pierre-Henri WUILLEMIN(@LIP6) and Jean-Christophe MAGNAN and Christophe GONZALES(@AMU)
 */

#include <agrum/tools/multidim/utils/FunctionGraphUtilities/internalNode.h>

#define ALLOCATE(x)      SmallObjectAllocator::instance().allocate(x)
#define DEALLOCATE(x, y) SmallObjectAllocator::instance().deallocate(x, y)

namespace gum {

  // ============================================================================
  // Constructors
  // ============================================================================
  InternalNode::InternalNode() {
    GUM_CONSTRUCTOR(InternalNode);
    nodeVar__ = nullptr;
  }

  InternalNode::InternalNode(const DiscreteVariable* v) {
    GUM_CONSTRUCTOR(InternalNode);
    setNodeVar__(v);
  }

  InternalNode::InternalNode(const DiscreteVariable* v, NodeId* sons) {
    GUM_CONSTRUCTOR(InternalNode);
    nodeVar__ = v;
    nodeSons__ = sons;
  }

  // ============================================================================
  // Destructors
  // ============================================================================
  InternalNode::~InternalNode() {
    GUM_DESTRUCTOR(InternalNode);
    if (nodeVar__ != nullptr) deallocateNodeSons(nodeVar__, nodeSons__);
  }

  // ============================================================================
  // Allocates a table of nodeid of the size given in parameter
  // ============================================================================
  NodeId* InternalNode::allocateNodeSons(const DiscreteVariable* v) {
    NodeId* sons =
       static_cast< NodeId* >(ALLOCATE(sizeof(NodeId) * v->domainSize()));
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
    if (nodeVar__ != nullptr) deallocateNodeSons(nodeVar__, nodeSons__);
    nodeVar__ = v;
    nodeSons__ = sons;
  }

  // ============================================================================
  // Var handlers
  // ============================================================================
  void InternalNode::setNodeVar(const DiscreteVariable* v) {
    if (nodeVar__ != nullptr) deallocateNodeSons(nodeVar__, nodeSons__);
    setNodeVar__(v);
  }

  void InternalNode::setNodeVar__(const DiscreteVariable* v) {
    nodeVar__ = v;
    nodeSons__ = allocateNodeSons(v);
  }

  // ============================================================================
  // Parent handlers
  // ============================================================================
  void InternalNode::addParent(NodeId parent, Idx modality) {
    nodeParents__.addLink(Parent(parent, modality));
  }

  void InternalNode::removeParent(NodeId parent, Idx modality) {
    nodeParents__.searchAndRemoveLink(Parent(parent, modality));
  }

}   // namespace gum

#ifdef GUM_NO_INLINE
#  include <agrum/tools/multidim/utils/FunctionGraphUtilities/internalNode_inl.h>
#endif /* GUM_NO_INLINE */
