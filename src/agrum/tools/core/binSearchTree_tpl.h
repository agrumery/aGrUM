/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief Basic binary tree.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <sstream>
#include <string>

#include <agrum/tools/core/binSearchTree.h>
#include <agrum/tools/core/exceptions.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  // ===========================================================================
  // ===========================================================================
  // ===                 GENERIC BINARY SEARCH TREE ITERATORS                ===
  // ===========================================================================
  // ===========================================================================

  template < typename Val, class Cmp, class Node >
  INLINE BinSearchTreeIterator< Val, Cmp, Node >::BinSearchTreeIterator() :
      node_(nullptr), next_node_(nullptr), prev_node_(nullptr), parent_(nullptr),
      left_child_(nullptr), right_child_(nullptr), tree_(nullptr), next_iter_(nullptr) {
    GUM_CONSTRUCTOR(BinSearchTreeIterator);
  }

  template < typename Val, class Cmp, class Node >
  INLINE BinSearchTreeIterator< Val, Cmp, Node >::BinSearchTreeIterator(
     const BinSearchTreeIterator< Val, Cmp, Node >& from) :
      node_(from.node_),
      next_node_(from.next_node_), prev_node_(from.prev_node_), parent_(from.parent_),
      left_child_(from.left_child_), right_child_(from.right_child_), tree_(from.tree_) {
    GUM_CONS_CPY(BinSearchTreeIterator);

    if (tree_ != nullptr) {
      next_iter_            = tree_->iterator_list_;
      tree_->iterator_list_ = this;
    } else next_iter_ = nullptr;
  }

  template < typename Val, class Cmp, class Node >
  INLINE void BinSearchTreeIterator< Val, Cmp, Node >::initialize_(
     const BinSearchTree< Val, Cmp, Node >* tree,
     const Node*                            current_node,
     bool                                   add_to_iterator_list) {
    // remember: we do not check here whether the iterator already belongs to
    // a tree. We assume that it is not so.

    tree_ = const_cast< BinSearchTree< Val, Cmp, Node >* >(tree);
    node_ = const_cast< Node* >(current_node);

    if (add_to_iterator_list && (tree_ != nullptr)) {
      next_iter_            = tree_->iterator_list_;
      tree_->iterator_list_ = this;
    }
  }

  template < typename Val, class Cmp, class Node >
  INLINE void BinSearchTreeIterator< Val, Cmp, Node >::detachFromTree_() {
    if (tree_ != nullptr) {
      BinSearchTreeIterator< Val, Cmp, Node >*iter, *prev_iter = nullptr;

      for (iter      = tree_->iterator_list_; iter != this && iter != nullptr;
           prev_iter = iter, iter = iter->next_iter_) {}

      if (iter != nullptr) {
        if (prev_iter != nullptr) prev_iter->next_iter_ = next_iter_;
        else tree_->iterator_list_ = next_iter_;
      }
    }
  }

  template < typename Val, class Cmp, class Node >
  INLINE BinSearchTreeIterator< Val, Cmp, Node >::~BinSearchTreeIterator() {
    GUM_DESTRUCTOR(BinSearchTreeIterator);

    // remove the iterator from its tree iterator's list
    detachFromTree_();
  }

  template < typename Val, class Cmp, class Node >
  INLINE void BinSearchTreeIterator< Val, Cmp, Node >::clear() {
    // remove the iterator from its tree iterator's list
    detachFromTree_();

    // reset the iterator
    node_        = nullptr;
    next_node_   = nullptr;
    prev_node_   = nullptr;
    parent_      = nullptr;
    left_child_  = nullptr;
    right_child_ = nullptr;
    tree_        = nullptr;
    next_iter_   = nullptr;
  }

  template < typename Val, class Cmp, class Node >
  INLINE BinSearchTreeIterator< Val, Cmp, Node >&
     BinSearchTreeIterator< Val, Cmp, Node >::operator=(
        const BinSearchTreeIterator< Val, Cmp, Node >& from) {
    // avoid self assignment
    if (this != &from) {
      GUM_OP_CPY(BinSearchTreeIterator);

      // if from and this belong to different trees, detach this from its
      // current tree
      if (from.tree_ != tree_) {
        detachFromTree_();
        tree_ = from.tree_;

        if (tree_ != nullptr) {
          next_iter_            = tree_->iterator_list_;
          tree_->iterator_list_ = this;
        } else next_iter_ = nullptr;
      }

      // make the iterators point to the same element
      node_        = from.node_;
      next_node_   = from.next_node_;
      prev_node_   = from.prev_node_;
      parent_      = from.parent_;
      left_child_  = from.left_child_;
      right_child_ = from.right_child_;
    }

    return *this;
  }

  template < typename Val, class Cmp, class Node >
  INLINE const Val& BinSearchTreeIterator< Val, Cmp, Node >::operator*() const {
    if (node_ != nullptr) return node_->value();

    GUM_ERROR(UndefinedIteratorValue, "the iterator does not point to a node of the binary tree")
  }

  template < typename Val, class Cmp, class Node >
  INLINE Node* BinSearchTree< Val, Cmp, Node >::minNode_(Node* node) const {
    Node* prevNode = nullptr;

    for (; node != nullptr; prevNode = node, node = node->leftChild()) {}

    return prevNode;
  }

  template < typename Val, class Cmp, class Node >
  INLINE Node* BinSearchTree< Val, Cmp, Node >::maxNode_(Node* node) const {
    Node* prevNode = nullptr;

    for (; node != nullptr; prevNode = node, node = node->rightChild()) {}

    return prevNode;
  }

  template < typename Val, class Cmp, class Node >
  INLINE Node* BinSearchTree< Val, Cmp, Node >::succNode_(Node* node) const {
    if (node == nullptr) return nullptr;

    if (node->rightChild()) return minNode_(node->rightChild());

    Node* par = node->parent();

    while ((par != nullptr) && (node->parentDir() == BinTreeDir::RIGHT_CHILD)) {
      node = par;
      par  = par->parent();
    }

    return par;
  }

  template < typename Val, class Cmp, class Node >
  INLINE Node* BinSearchTree< Val, Cmp, Node >::prevNode_(Node* node) const {
    if (node == nullptr) return nullptr;

    if (node->leftChild()) return maxNode_(node->leftChild());

    Node* par = node->parent();

    while ((par != nullptr) && (node->parentDir() == BinTreeDir::LEFT_CHILD)) {
      node = par;
      par  = par->parent();
    }

    return par;
  }

  template < typename Val, class Cmp, class Node >
  INLINE BinSearchTreeIterator< Val, Cmp, Node >&
     BinSearchTreeIterator< Val, Cmp, Node >::operator++() {
    // if there is a current node, use it to compute the next node, else use
    // directly next_node_ (this case obtains when the iterator was pointing
    // toward a node that has been deleted before we use operator++)
    node_ = node_ != nullptr ? tree_->succNode_(node_) : next_node_;

    if (node_ == nullptr) {
      next_node_   = nullptr;
      prev_node_   = nullptr;
      parent_      = nullptr;
      left_child_  = nullptr;
      right_child_ = nullptr;
    }

    return *this;
  }

  template < typename Val, class Cmp, class Node >
  INLINE BinSearchTreeIterator< Val, Cmp, Node >&
     BinSearchTreeIterator< Val, Cmp, Node >::operator--() {
    // if there is a current node, use it to compute the preceding node, else
    // use
    // directly prev_node_ (this case obtains when the iterator was pointing
    // toward a node that has been deleted before we use operator--)
    node_ = node_ != nullptr ? tree_->prevNode_(node_) : prev_node_;

    if (node_ == nullptr) {
      next_node_   = nullptr;
      prev_node_   = nullptr;
      parent_      = nullptr;
      left_child_  = nullptr;
      right_child_ = nullptr;
    }

    return *this;
  }

  template < typename Val, class Cmp, class Node >
  INLINE bool BinSearchTreeIterator< Val, Cmp, Node >::operator==(
     const BinSearchTreeIterator< Val, Cmp, Node >& from) const {
    if (node_ != nullptr) return (node_ == from.node_);
    else
      return ((node_ == from.node_) && (tree_ == from.tree_) && (next_node_ == from.next_node_)
              && (prev_node_ == from.prev_node_) && (parent_ == from.parent_)
              && (left_child_ == from.left_child_) && (right_child_ == from.right_child_));
  }

  template < typename Val, class Cmp, class Node >
  INLINE bool BinSearchTreeIterator< Val, Cmp, Node >::operator!=(
     const BinSearchTreeIterator< Val, Cmp, Node >& from) const {
    if (node_ != nullptr) return (node_ != from.node_);
    else
      return ((node_ != from.node_) || (tree_ != from.tree_) || (next_node_ != from.next_node_)
              || (prev_node_ != from.prev_node_) || (parent_ != from.parent_)
              || (left_child_ != from.left_child_) || (right_child_ != from.right_child_));
  }

  template < typename Val, class Cmp, class Node >
  INLINE BinSearchTreeIterator< Val, Cmp, Node >& BinSearchTreeIterator< Val, Cmp, Node >::up() {
    // if there is a current node, use it to compute its parent node, else use
    // directly parent_ (this case obtains when the iterator was pointing
    // toward a node that has been deleted before we use operation up)
    node_ = node_ != nullptr ? node_->parent() : parent_;

    if (node_ == nullptr) {
      next_node_   = nullptr;
      prev_node_   = nullptr;
      parent_      = nullptr;
      left_child_  = nullptr;
      right_child_ = nullptr;
    }

    return *this;
  }

  template < typename Val, class Cmp, class Node >
  INLINE BinSearchTreeIterator< Val, Cmp, Node >&
         BinSearchTreeIterator< Val, Cmp, Node >::downLeft() {
    // if there is a current node, use it to compute its left child, else use
    // directly left_child_ (this case obtains when the iterator was pointing
    // toward a node that has been deleted before we use operation downLeft)
    node_ = node_ != nullptr ? node_->leftChild() : left_child_;

    if (node_ == nullptr) {
      next_node_   = nullptr;
      prev_node_   = nullptr;
      parent_      = nullptr;
      left_child_  = nullptr;
      right_child_ = nullptr;
    }

    return *this;
  }

  template < typename Val, class Cmp, class Node >
  INLINE BinSearchTreeIterator< Val, Cmp, Node >&
         BinSearchTreeIterator< Val, Cmp, Node >::downRight() {
    // if there is a current node, use it to compute its right child, else use
    // directly right_child_ (this case obtains when the iterator was pointing
    // toward a node that has been deleted before we use operation downRight)
    node_ = node_ != nullptr ? node_->rightChild() : right_child_;

    if (node_ == nullptr) {
      next_node_   = nullptr;
      prev_node_   = nullptr;
      parent_      = nullptr;
      left_child_  = nullptr;
      right_child_ = nullptr;
    }

    return *this;
  }


  // ===========================================================================
  // ===========================================================================
  // ===                      GENERIC BINARY SEARCH TREE                     ===
  // ===========================================================================
  // ===========================================================================

  template < typename Val, class Cmp, class Node >
  BinSearchTree< Val, Cmp, Node >::BinSearchTree(bool uniqueness_policy) :
      root_(nullptr), iterator_list_(nullptr), uniqueness_policy_(uniqueness_policy),
      nb_elements_(0) {
    GUM_CONSTRUCTOR(BinSearchTree);
    iter_end_.initialize_(this, nullptr, false);
  }

  template < typename Val, class Cmp, class Node >
  BinSearchTree< Val, Cmp, Node >::BinSearchTree(const BinSearchTree< Val, Cmp, Node >& from) :
      root_(nullptr), iterator_list_(nullptr), uniqueness_policy_(from.uniqueness_policy_) {
    // for debugging purposes
    GUM_CONS_CPY(BinSearchTree);

    // copy the content of BinSearchTree "from"
    root_        = copy_(from.root_);
    nb_elements_ = from.nb_elements_;

    // initialize the end/rend iterator
    iter_end_.initialize_(this, nullptr, false);
  }

  template < typename Val, class Cmp, class Node >
  INLINE void BinSearchTree< Val, Cmp, Node >::clear() {
    // first we clear all the iterators, i.e., we detach them from the tree
    for (iterator *iter = iterator_list_, *next_iter = nullptr; iter; iter = next_iter) {
      next_iter = iter->next_iter_;
      iter->clear();
    }

    // now, delete the whole tree
    deleteSubTree_(root_);
    root_        = nullptr;
    nb_elements_ = 0;

    // note that there is no need to redefined end/rend as they do not rely
    // on the content of the tree
  }

  template < typename Val, class Cmp, class Node >
  BinSearchTree< Val, Cmp, Node >&
     BinSearchTree< Val, Cmp, Node >::operator=(const BinSearchTree< Val, Cmp, Node >& from) {
    // avoid self assignment
    if (this != &from) {
      // for debugging purposes
      GUM_OP_CPY(BinSearchTree);

      // if the tree is not currently empty, remove it
      clear();

      // copy binary tree "from"
      uniqueness_policy_ = from.uniqueness_policy_;
      root_              = copy_(from.root_);   // note that we can copy from's tree structure
      // as from and this have the same ordering cmp_
      nb_elements_ = from.nb_elements_;

      // note that we do not need to update the end/rend iterator as, besides
      // field tree_, no other field is related to the current tree (i.e., all
      // _*node_ are set to nullptr
    }

    return *this;
  }

  template < typename Val, class Cmp, class Node >
  BinSearchTree< Val, Cmp, Node >::~BinSearchTree() {
    // for debugging purposes
    GUM_DESTRUCTOR(BinSearchTree);

    // clear all the iterators and remove all nodes
    clear();
  }

  template < typename Val, class Cmp, class Node >
  Node* BinSearchTree< Val, Cmp, Node >::copy_(Node* node, Node* parent, BinTreeDir dir) {
    // if there is no node to copy, abort
    if (!node) return nullptr;

    // create the copy of node
    Node* new_node = new Node(*node);

    if (parent) parent->insertChild(*new_node, dir);

    // if necessary, create the left and right subgraphs
    copy_(node->leftChild(), new_node, BinTreeDir::LEFT_CHILD);
    copy_(node->rightChild(), new_node, BinTreeDir::RIGHT_CHILD);

    return new_node;
  }

  template < typename Val, class Cmp, class Node >
  void BinSearchTree< Val, Cmp, Node >::deleteSubTree_(Node* node) {
    // if there is no node to remove, return
    if (!node) return;

    // delete the left and right subgraphs
    deleteSubTree_(node->leftChild());
    deleteSubTree_(node->rightChild());

    // delete the node itself
    delete node;
  }

  template < typename Val, class Cmp, class Node >
  INLINE Node* BinSearchTree< Val, Cmp, Node >::insert_(const Val& val) {
    // if the tree is not empty, search the binary search tree to know
    // where the node should be inserted
    if (root_) {
      Node* node = root_;

      while (true) {
        if (cmp_(val, node->value()))
          if (!node->leftChild()) {
            // here we are on a leaf => insert the new node
            ++nb_elements_;
            return node->insertLeftChild(val);
          } else {
            node = node->leftChild();
          }
        else if (cmp_(node->value(), val) || !uniqueness_policy_)
          if (!node->rightChild()) {
            // here we are on a leaf => insert the new node
            ++nb_elements_;
            return node->insertRightChild(val);
          } else {
            node = node->rightChild();
          }
        else {
          // here we found a node with the same key and the uniqueness policy
          // is set. So we should raise an exception
          GUM_ERROR(DuplicateElement, "Val " << val << " already in the binary search tree")
        }
      }
    }

    // here the tree is empty, just create a new node
    root_ = new Node(val);
    ++nb_elements_;
    return root_;
  }

  template < typename Val, class Cmp, class Node >
  INLINE const Val& BinSearchTree< Val, Cmp, Node >::insert(const Val& val) {
    return insert_(val)->value();
  }

  template < typename Val, class Cmp, class Node >
  INLINE const Val& BinSearchTree< Val, Cmp, Node >::rootValue() const {
    if (root_ == nullptr) { GUM_ERROR(NotFound, "no value in an empty Binary Search tree") }

    return root_->value();
  }

  template < typename Val, class Cmp, class Node >
  INLINE const Val& BinSearchTree< Val, Cmp, Node >::minValue() const {
    if (root_ == nullptr) { GUM_ERROR(NotFound, "no minimal value in an empty Binary Search tree") }

    return minNode_(root_)->value();
  }

  template < typename Val, class Cmp, class Node >
  INLINE const Val& BinSearchTree< Val, Cmp, Node >::maxValue() const {
    if (root_ == nullptr) { GUM_ERROR(NotFound, "no maximal value in an empty Binary Search tree") }

    return maxNode_(root_)->value();
  }

  template < typename Val, class Cmp, class Node >
  INLINE Node* BinSearchTree< Val, Cmp, Node >::getNode_(const Val& val) const {
    // if the tree is not empty, search the binary search tree to know
    // where the node could be
    if (root_) {
      Node* node = root_;

      while (true) {
        if (cmp_(val, node->value())) {
          if (!node->leftChild()) return nullptr;
          else node = node->leftChild();
        } else if (cmp_(node->value(), val)) {
          if (!node->rightChild()) return nullptr;
          else node = node->rightChild();
        } else return node;
      }
    } else {
      return nullptr;
    }
  }

  template < typename Val, class Cmp, class Node >
  INLINE bool BinSearchTree< Val, Cmp, Node >::contains(const Val& val) const {
    return (getNode_(val) != nullptr);
  }

  template < typename Val, class Cmp, class Node >
  INLINE Size BinSearchTree< Val, Cmp, Node >::size() const {
    return nb_elements_;
  }

  template < typename Val, class Cmp, class Node >
  INLINE bool BinSearchTree< Val, Cmp, Node >::empty() const {
    return (nb_elements_ == 0);
  }

  template < typename Val, class Cmp, class Node >
  std::string BinSearchTree< Val, Cmp, Node >::toString() const {
    bool              deja = false;
    std::stringstream stream;
    stream << "[";

    for (const_iterator iter = begin(); iter != end(); ++iter, deja = true) {
      if (deja) stream << " , ";

      stream << *iter;
    }

    stream << "]";

    return stream.str();
  }

  template < typename Val, class Cmp, class Node >
  INLINE bool BinSearchTree< Val, Cmp, Node >::uniquenessPolicy() const {
    return uniqueness_policy_;
  }

  template < typename Val, class Cmp, class Node >
  INLINE void BinSearchTree< Val, Cmp, Node >::setUniquenessPolicy(const bool new_policy) {
    uniqueness_policy_ = new_policy;
  }

  template < typename Val, class Cmp, class Node >
  INLINE BinSearchTreeIterator< Val, Cmp, Node > BinSearchTree< Val, Cmp, Node >::begin() {
    BinSearchTreeIterator< Val, Cmp, Node > iter;
    iter.initialize_(this, minNode_(root_), true);
    return iter;
  }

  template < typename Val, class Cmp, class Node >
  INLINE BinSearchTreeIterator< Val, Cmp, Node > BinSearchTree< Val, Cmp, Node >::begin() const {
    BinSearchTreeIterator< Val, Cmp, Node > iter;
    iter.initialize_(this, minNode_(root_), true);
    return iter;
  }

  template < typename Val, class Cmp, class Node >
  INLINE BinSearchTreeIterator< Val, Cmp, Node > BinSearchTree< Val, Cmp, Node >::rbegin() {
    BinSearchTreeIterator< Val, Cmp, Node > iter;
    iter.initialize_(this, maxNode_(root_), true);
    return iter;
  }

  template < typename Val, class Cmp, class Node >
  INLINE BinSearchTreeIterator< Val, Cmp, Node > BinSearchTree< Val, Cmp, Node >::rbegin() const {
    BinSearchTreeIterator< Val, Cmp, Node > iter;
    iter.initialize_(this, maxNode_(root_), true);
    return iter;
  }

  template < typename Val, class Cmp, class Node >
  INLINE const BinSearchTreeIterator< Val, Cmp, Node >& BinSearchTree< Val, Cmp, Node >::end() {
    return iter_end_;
  }

  template < typename Val, class Cmp, class Node >
  INLINE const BinSearchTreeIterator< Val, Cmp, Node >&
               BinSearchTree< Val, Cmp, Node >::end() const {
    return iter_end_;
  }

  template < typename Val, class Cmp, class Node >
  INLINE const BinSearchTreeIterator< Val, Cmp, Node >& BinSearchTree< Val, Cmp, Node >::rend() {
    return iter_end_;
  }

  template < typename Val, class Cmp, class Node >
  INLINE const BinSearchTreeIterator< Val, Cmp, Node >&
               BinSearchTree< Val, Cmp, Node >::rend() const {
    return iter_end_;
  }

  template < typename Val, class Cmp, class Node >
  INLINE BinSearchTreeIterator< Val, Cmp, Node > BinSearchTree< Val, Cmp, Node >::root() {
    BinSearchTreeIterator< Val, Cmp, Node > iter;
    iter.initialize_(this, root_, true);
    return iter;
  }

  template < typename Val, class Cmp, class Node >
  INLINE BinSearchTreeIterator< Val, Cmp, Node > BinSearchTree< Val, Cmp, Node >::root() const {
    BinSearchTreeIterator< Val, Cmp, Node > iter;
    iter.initialize_(this, root_, true);
    return iter;
  }

  template < typename Val, class Cmp, class Node >
  void BinSearchTree< Val, Cmp, Node >::erase_(Node* node) {
    if (!node) return;

    // update all the iterators pointing to node that they should point
    // elsewhere
    _updateEraseIterators_(node);

    // update the number of elements contained in the tree
    --nb_elements_;

    // now remove the node from the tree:

    // if the node has no children, then just remove it
    if (!node->leftChild() && !node->rightChild()) {
      // if the node was the only one in the tree, then the tree becomes empty
      if (!node->parent()) root_ = nullptr;

      // note that, when node has a parent, there is no need to remove the
      // link between node and this parent: this will be taken care of by
      // node's destructor.
    }
    // if there is just a right child
    else if (!node->leftChild()) {
      // just relink the right child with the parent (if any)
      if (!node->parent()) {
        // in this case, no need to remove the link between "node" and its
        // child:
        // this will be taken care of by the destructor of "node"
        root_ = node->rightChild();
      } else {
        Node *     parent = node->parent(), *child = node->rightChild();
        BinTreeDir dir = node->parentDir();
        parent->eraseLink(dir);
        node->eraseRightLink();
        parent->insertChild(*child, dir);
      }
    }
    // if there is just a left child
    else if (!node->rightChild()) {
      // just relink the left child with the parent (if any)
      if (!node->parent()) {
        // in this case, no need to remove the link between "node" and its
        // child:
        // this will be taken care of by the destructor of "node"
        root_ = node->leftChild();
      } else {
        Node *     parent = node->parent(), *child = node->leftChild();
        BinTreeDir dir = node->parentDir();
        parent->eraseLink(dir);
        node->eraseLeftLink();
        parent->insertChild(*child, dir);
      }
    }
    // ok, here there are two children
    else {
      _eraseWithTwoChildren_(node);
    }

    // now we shall physically remove node from memory
    delete node;
  }

  template < typename Val, class Cmp, class Node >
  INLINE void BinSearchTree< Val, Cmp, Node >::_eraseWithTwoChildren_(Node* node) {
    // the idea is to get the successor of "node" and substitute "node" by
    // it.  As "node" has two children, we are sure that the successor is one
    // of node's descendants. Moreover, by its very definition, this
    // successor has no left child. Hence, two cases can obtain:
    // 1/ the successor is precisely node's right child. In this case, we just
    //    have to make node's left child be the left child of the successor,
    //    and node's parent be the successor's parent, and the tree is again
    //    a binary search tree.
    // 2/ the successor is not node's right child. In this case, we know that
    //    the successor has a parent different from node and that the
    //    successor
    //    is a left child of this parent. We just need to put the right child
    //    of the successor (if any) as the left child of its parent, and to
    //    replace "node" by the successor.
    Node* successor = succNode_(node);

    if (successor == node->rightChild()) {   // proceed to case 1:
      Node* left_child = node->leftChild();
      node->eraseLeftLink();
      node->eraseRightLink();
      successor->insertLeftChild(*left_child);

      if (!node->parent()) {
        // in this case, no need to remove the link between "node" and the
        // successor: this will be taken care of by the destructor of "node"
        root_ = successor;
      } else {
        // rechain node's parent with its successor
        BinTreeDir par_dir = node->parentDir();
        Node*      parent  = node->parent();
        parent->eraseLink(par_dir);
        parent->insertChild(*successor, par_dir);
      }
    } else {   // proceed to case 2:
      Node* parent = successor->parent();
      parent->eraseLeftLink();

      if (successor->rightChild()) {
        Node* succ_child = successor->rightChild();
        successor->eraseRightLink();
        parent->insertLeftChild(*succ_child);
      }

      Node *left = node->leftChild(), *right = node->rightChild();
      node->eraseLeftLink();
      node->eraseRightLink();
      successor->insertLeftChild(*left);
      successor->insertRightChild(*right);

      if (!node->parent()) {
        root_ = successor;
      } else {
        // rechain node's parent with its successor
        BinTreeDir par_dir = node->parentDir();
        Node*      parent  = node->parent();
        parent->eraseLink(par_dir);
        parent->insertChild(*successor, par_dir);
      }
    }
  }

  template < typename Val, class Cmp, class Node >
  INLINE void BinSearchTree< Val, Cmp, Node >::erase(const Val& val) {
    Node* n = getNode_(val);

    if (n == nullptr) GUM_ERROR(gum::NotFound, "Value \"" << val << "\" not found")

    erase_(n);
  }

  template < typename Val, class Cmp, class Node >
  INLINE void BinSearchTree< Val, Cmp, Node >::erase(const iterator& iter) {
    erase_(iter.node_);
  }

  template < typename Val, class Cmp, class Node >
  void BinSearchTree< Val, Cmp, Node >::_updateEraseIterators_(Node* node) {
    for (iterator* iter = iterator_list_; iter; iter = iter->next_iter_) {
      // if the iterator points toward the node to be deleted, make its node_
      // field point to nullptr and update accordingly its other fields
      if (iter->node_ == node) {
        iter->node_        = nullptr;
        iter->next_node_   = succNode_(node);
        iter->prev_node_   = prevNode_(node);
        iter->parent_      = node->parent();
        iter->left_child_  = node->leftChild();
        iter->right_child_ = node->rightChild();
      } else if (!iter->node_) {
        if (iter->next_node_ == node) iter->next_node_ = succNode_(node);

        if (iter->prev_node_ == node) iter->prev_node_ = prevNode_(node);

        if (iter->parent_ == node) iter->parent_ = node->parent();

        if (iter->left_child_ == node) iter->left_child_ = node->leftChild();

        if (iter->right_child_ == node) iter->right_child_ = node->rightChild();
      }
    }
  }

}   // namespace gum

#endif   // DOXYGEN_SHOULD_SKIP_THIS
