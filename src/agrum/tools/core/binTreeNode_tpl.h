/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief Node class for various binary search trees
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  template < typename Val >
  INLINE BinTreeNode< Val >::BinTreeNode(const Val& v) :
      val_(v), parent_(nullptr), parent_dir_(BinTreeDir::NO_PARENT) {
    GUM_CONSTRUCTOR(BinTreeNode);

    // set the children
    children_[0] = nullptr;
    children_[1] = nullptr;
  }

  template < typename Val >
  INLINE BinTreeNode< Val >::BinTreeNode(const BinTreeNode< Val >& from) :
      val_(from.val_), parent_(nullptr), parent_dir_(BinTreeDir::NO_PARENT) {
    GUM_CONS_CPY(BinTreeNode);

    // set the children
    children_[0] = nullptr;
    children_[1] = nullptr;
  }

  template < typename Val >
  INLINE BinTreeNode< Val >::~BinTreeNode() {
    GUM_DESTRUCTOR(BinTreeNode);

    // update the tree accordingly to the removal of this
    if (parent_ != nullptr)
      parent_->children_[static_cast< int >(parent_dir_)]
         = nullptr;   // parent_dir can not be NO_PARENT (... sure ?)

    if (children_[0] != nullptr) {
      children_[0]->parent_     = nullptr;
      children_[0]->parent_dir_ = BinTreeDir::NO_PARENT;
    }

    if (children_[1] != nullptr) {
      children_[1]->parent_     = nullptr;
      children_[1]->parent_dir_ = BinTreeDir::NO_PARENT;
    }
  }

  /** @brief copy operator: copy the value of from into this. However, this
   * does not change the current connections (parents and children) of this. */

  template < typename Val >
  INLINE BinTreeNode< Val >& BinTreeNode< Val >::operator=(const BinTreeNode< Val >& from) {
    // avoid self assignment
    if (this != &from) {
      GUM_OP_CPY(BinTreeNode);
      val_ = from.val_;
    }

    return *this;
  }

  template < typename Val >
  INLINE Val& BinTreeNode< Val >::value() {
    return val_;
  }

  template < typename Val >
  INLINE Val& BinTreeNode< Val >::operator*() {
    return val_;
  }

  template < typename Val >
  INLINE BinTreeNode< Val >* BinTreeNode< Val >::child(BinTreeDir dir) const {
    return children_[static_cast< int >(dir)];
  }

  template < typename Val >
  INLINE BinTreeNode< Val >* BinTreeNode< Val >::leftChild() const {
    return children_[static_cast< int >(BinTreeDir::LEFT_CHILD)];
  }

  template < typename Val >
  INLINE BinTreeNode< Val >* BinTreeNode< Val >::rightChild() const {
    return children_[static_cast< int >(BinTreeDir::RIGHT_CHILD)];
  }

  template < typename Val >
  INLINE BinTreeNode< Val >* BinTreeNode< Val >::parent() const {
    return parent_;
  }

  template < typename Val >
  INLINE BinTreeDir BinTreeNode< Val >::parentDir() const {
    return parent_dir_;
  }

  template < typename Val >
  INLINE void BinTreeNode< Val >::insertLeftChild(BinTreeNode< Val >& new_child) {
    if (new_child.parent_ != nullptr) {
      GUM_ERROR(DuplicateElement, "this child already has a parent in the BST")
    }

    if (children_[static_cast< int >(BinTreeDir::LEFT_CHILD)] != nullptr) {
      GUM_ERROR(DuplicateElement, "this child already has a parent in the BST")
    }

    // proceed to the chaining
    new_child.parent_                                     = this;
    new_child.parent_dir_                                 = BinTreeDir::LEFT_CHILD;
    children_[static_cast< int >(BinTreeDir::LEFT_CHILD)] = &new_child;
  }

  template < typename Val >
  INLINE BinTreeNode< Val >* BinTreeNode< Val >::insertLeftChild(const Val& val) {
    if (children_[static_cast< int >(BinTreeDir::LEFT_CHILD)] != nullptr) {
      GUM_ERROR(DuplicateElement, "this child already has a parent in the BST")
    }

    BinTreeNode< Val >* new_child = new BinTreeNode< Val >(val);

    // proceed to the chaining
    new_child->parent_                                    = this;
    new_child->parent_dir_                                = BinTreeDir::LEFT_CHILD;
    children_[static_cast< int >(BinTreeDir::LEFT_CHILD)] = new_child;

    return new_child;
  }

  template < typename Val >
  INLINE void BinTreeNode< Val >::insertRightChild(BinTreeNode< Val >& new_child) {
    if (new_child.parent_ != nullptr) {
      GUM_ERROR(DuplicateElement, "this child already has a parent in the BST")
    }

    if (children_[static_cast< int >(BinTreeDir::RIGHT_CHILD)] != nullptr) {
      GUM_ERROR(DuplicateElement, "this node already has a right child")
    }

    // proceed to the chaining
    new_child.parent_                                      = this;
    new_child.parent_dir_                                  = BinTreeDir::RIGHT_CHILD;
    children_[static_cast< int >(BinTreeDir::RIGHT_CHILD)] = &new_child;
  }

  template < typename Val >
  INLINE BinTreeNode< Val >* BinTreeNode< Val >::insertRightChild(const Val& val) {
    if (children_[static_cast< int >(BinTreeDir::RIGHT_CHILD)] != nullptr) {
      GUM_ERROR(DuplicateElement, "this node already has a right child")
    }

    BinTreeNode< Val >* new_child = new BinTreeNode< Val >(val);

    // proceed to the chaining
    new_child->parent_                                     = this;
    new_child->parent_dir_                                 = BinTreeDir::RIGHT_CHILD;
    children_[static_cast< int >(BinTreeDir::RIGHT_CHILD)] = new_child;

    return new_child;
  }

  template < typename Val >
  INLINE void BinTreeNode< Val >::insertChild(BinTreeNode< Val >& new_child, BinTreeDir child_dir) {
    if (new_child.parent_ != nullptr) {
      GUM_ERROR(DuplicateElement, "this child has already a parent")
    }

    if (children_[static_cast< int >(child_dir)] != nullptr) {
      GUM_ERROR(DuplicateElement, "this node has already this child")
    }

    // proceed to the chaining
    new_child.parent_                        = this;
    new_child.parent_dir_                    = child_dir;
    children_[static_cast< int >(child_dir)] = &new_child;
  }

  template < typename Val >
  INLINE BinTreeNode< Val >* BinTreeNode< Val >::insertChild(const Val& val, BinTreeDir child_dir) {
    if (children_[static_cast< int >(child_dir)] != nullptr) {
      GUM_ERROR(DuplicateElement, "this node has already this child")
    }

    BinTreeNode< Val >* new_child = new BinTreeNode< Val >(val);

    // proceed to the chaining
    new_child->parent_                       = this;
    new_child->parent_dir_                   = child_dir;
    children_[static_cast< int >(child_dir)] = new_child;

    return new_child;
  }

  template < typename Val >
  INLINE void BinTreeNode< Val >::eraseLeftLink() {
    if (children_[static_cast< int >(BinTreeDir::LEFT_CHILD)] != nullptr) {
      children_[static_cast< int >(BinTreeDir::LEFT_CHILD)]->parent_     = nullptr;
      children_[static_cast< int >(BinTreeDir::LEFT_CHILD)]->parent_dir_ = BinTreeDir::NO_PARENT;
      children_[static_cast< int >(BinTreeDir::LEFT_CHILD)]              = nullptr;
    }
  }

  template < typename Val >
  INLINE void BinTreeNode< Val >::eraseRightLink() {
    if (children_[static_cast< int >(BinTreeDir::RIGHT_CHILD)] != nullptr) {
      children_[static_cast< int >(BinTreeDir::RIGHT_CHILD)]->parent_     = nullptr;
      children_[static_cast< int >(BinTreeDir::RIGHT_CHILD)]->parent_dir_ = BinTreeDir::NO_PARENT;
      children_[static_cast< int >(BinTreeDir::RIGHT_CHILD)]              = nullptr;
    }
  }

  template < typename Val >
  INLINE void BinTreeNode< Val >::eraseLink(BinTreeDir tree_dir) {
    if (children_[static_cast< int >(tree_dir)] != nullptr) {
      children_[static_cast< int >(tree_dir)]->parent_     = nullptr;
      children_[static_cast< int >(tree_dir)]->parent_dir_ = BinTreeDir::NO_PARENT;
      children_[static_cast< int >(tree_dir)]              = nullptr;
    }
  }

  template < typename Val >
  INLINE BinTreeNode< Val >* BinTreeNode< Val >::leftmostNode() const {
    BinTreeNode< Val >* node = const_cast< BinTreeNode< Val >* >(this);

    while (node->children_[static_cast< int >(BinTreeDir::LEFT_CHILD)] != nullptr)
      node = node->children_[static_cast< int >(BinTreeDir::LEFT_CHILD)];

    return node;
  }

  template < typename Val >
  INLINE BinTreeNode< Val >* BinTreeNode< Val >::rightmostNode() const {
    BinTreeNode< Val >* node = const_cast< BinTreeNode< Val >* >(this);

    while (node->children_[static_cast< int >(BinTreeDir::RIGHT_CHILD)] != nullptr)
      node = node->children_[static_cast< int >(BinTreeDir::RIGHT_CHILD)];

    return node;
  }

  template < typename Val >
  INLINE BinTreeNode< Val >* BinTreeNode< Val >::root() const {
    BinTreeNode< Val >* node = const_cast< BinTreeNode< Val >* >(this);

    while (node->parent_ != nullptr)
      node = node->parent_;

    return node;
  }

}   // namespace gum

#endif   // DOXYGEN_SHOULD_SKIP_THIS
