
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
 * @brief Inline implementation of the InternalNode class.
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */

#include <agrum/multidim/utils/FunctionGraphUtilities/internalNode.h>

namespace gum {

  INLINE
  Parent::Parent(NodeId pi, Idx m) : parentId(pi), modality(m) {}


  INLINE
  Parent::Parent(const Parent& src) :
      parentId(src.parentId), modality(src.modality) {}

  INLINE
  void* Parent::operator new(size_t s) {
    return SmallObjectAllocator::instance().allocate(s);
  }

  INLINE
  void Parent::operator delete(void* p) {
    SmallObjectAllocator::instance().deallocate(p, sizeof(Parent));
  }

  INLINE
  bool Parent::operator==(const Parent& comp) const {
    if ((comp.parentId == this->parentId) && (comp.modality == this->modality))
      return true;
    return false;
  }

  INLINE
  bool Parent::operator!=(const Parent& comp) const { return !(comp == *this); }

  INLINE
  Parent& Parent::operator=(const Parent& src) {
    this->parentId = src.parentId;
    this->modality = src.modality;
    return *this;
  }

  INLINE
  void* InternalNode::operator new(size_t s) {
    return SmallObjectAllocator::instance().allocate(s);
  }

  INLINE
  void InternalNode::operator delete(void* p) {
    SmallObjectAllocator::instance().deallocate(p, sizeof(InternalNode));
  }

  INLINE
  const DiscreteVariable* InternalNode::nodeVar() const { return __nodeVar; }

  INLINE
  void InternalNode::setSon(Idx modality, NodeId son) {
    __nodeSons[modality] = son;
  }

  INLINE
  NodeId InternalNode::son(Idx modality) const { return __nodeSons[modality]; }

  INLINE
  Idx InternalNode::nbSons() const { return __nodeVar->domainSize(); }

  INLINE
  Link< Parent >* InternalNode::parents() { return __nodeParents.list(); }

  INLINE
  const Link< Parent >* InternalNode::parents() const {
    return __nodeParents.list();
  }
}   // namespace gum
