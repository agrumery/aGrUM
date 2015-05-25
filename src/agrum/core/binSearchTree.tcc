/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/** @file
 * @brief basic binary tree
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <string>
#include <sstream>

// to ease IDE parsing
#include <agrum/core/binSearchTree.h>

#include <agrum/core/exceptions.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  /* =========================================================================== */
  /* =========================================================================== */
  /* ===                 GENERIC BINARY SEARCH TREE ITERATORS                === */
  /* =========================================================================== */
  /* =========================================================================== */

  /// basic constructor: returns an iterator pointing toward nothing

  template <typename Val, class Cmp, class Node>
  INLINE BinSearchTreeIterator<Val, Cmp, Node>::BinSearchTreeIterator()
      : _node(0), _next_node(0), _prev_node(0), _parent(0), _left_child(0),
        _right_child(0), _tree(0), _next_iter(0) {
    GUM_CONSTRUCTOR(BinSearchTreeIterator);
  }

  /// copy constructor: creates an iterator pointing toward the same tree

  template <typename Val, class Cmp, class Node>
  INLINE BinSearchTreeIterator<Val, Cmp, Node>::BinSearchTreeIterator(
      const BinSearchTreeIterator<Val, Cmp, Node> &from)
      : _node(from._node), _next_node(from._next_node), _prev_node(from._prev_node),
        _parent(from._parent), _left_child(from._left_child),
        _right_child(from._right_child), _tree(from._tree) {
    GUM_CONS_CPY(BinSearchTreeIterator);

    if (_tree) {
      _next_iter = _tree->_iterator_list;
      _tree->_iterator_list = this;
    } else
      _next_iter = 0;
  }

  /// a function called to initialize an iterator

  template <typename Val, class Cmp, class Node>
  INLINE void BinSearchTreeIterator<Val, Cmp, Node>::_initialize(
      const BinSearchTree<Val, Cmp, Node> *tree, const Node *current_node,
      bool add_to_iterator_list) {
    // remember: we do not check here whether the iterator already belongs to
    // a tree. We assume that it is not so.

    _tree = const_cast<BinSearchTree<Val, Cmp, Node> *>(tree);
    _node = const_cast<Node *>(current_node);

    if (add_to_iterator_list && _tree) {
      _next_iter = _tree->_iterator_list;
      _tree->_iterator_list = this;
    }
  }

  /// a method to detach the current iterator from its tree's iterator's list

  template <typename Val, class Cmp, class Node>
  INLINE void BinSearchTreeIterator<Val, Cmp, Node>::_detachFromTree() {
    if (_tree) {
      BinSearchTreeIterator<Val, Cmp, Node> *iter, *prev_iter = 0;

      for (iter = _tree->_iterator_list; iter != this && iter;
           prev_iter = iter, iter = iter->_next_iter) {
      }

      if (iter) {
        if (prev_iter)
          prev_iter->_next_iter = _next_iter;
        else
          _tree->_iterator_list = _next_iter;
      }
    }
  }

  /// destructor

  template <typename Val, class Cmp, class Node>
  INLINE BinSearchTreeIterator<Val, Cmp, Node>::~BinSearchTreeIterator() {
    GUM_DESTRUCTOR(BinSearchTreeIterator);

    // remove the iterator from its tree iterator's list
    _detachFromTree();
  }

  /// detach the iterator from its current tree (if any) and reset it

  template <typename Val, class Cmp, class Node>
  INLINE void BinSearchTreeIterator<Val, Cmp, Node>::clear() {
    // remove the iterator from its tree iterator's list
    _detachFromTree();

    // reset the iterator
    _node = 0;
    _next_node = 0;
    _prev_node = 0;
    _parent = 0;
    _left_child = 0;
    _right_child = 0;
    _tree = 0;
    _next_iter = 0;
  }

  /// copy operator

  template <typename Val, class Cmp, class Node>
  INLINE BinSearchTreeIterator<Val, Cmp, Node> &
      BinSearchTreeIterator<Val, Cmp, Node>::
      operator=(const BinSearchTreeIterator<Val, Cmp, Node> &from) {
    // avoid self assignment
    if (this != &from) {
      GUM_OP_CPY(BinSearchTreeIterator);

      // if from and this belong to different trees, detach this from its
      // current tree
      if (from._tree != _tree) {
        _detachFromTree();
        _tree = from._tree;

        if (_tree) {
          _next_iter = _tree->_iterator_list;
          _tree->_iterator_list = this;
        } else
          _next_iter = 0;
      }

      // make the iterators point to the same element
      _node = from._node;
      _next_node = from._next_node;
      _prev_node = from._prev_node;
      _parent = from._parent;
      _left_child = from._left_child;
      _right_child = from._right_child;
    }

    return *this;
  }

  /// returns the value pointed to by the iterator

  template <typename Val, class Cmp, class Node>
  INLINE const Val &BinSearchTreeIterator<Val, Cmp, Node>::operator*() const {
    if (_node)
      return _node->value();

    GUM_ERROR(UndefinedIteratorValue,
              "the iterator does not point to a node of the binary tree");
  }

  /// returns the leftmost node in the tree

  template <typename Val, class Cmp, class Node>
  INLINE Node *BinSearchTree<Val, Cmp, Node>::_minNode(Node *node) const {
    Node *prevNode = 0;

    for (; node; prevNode = node, node = node->leftChild()) {
    }

    return prevNode;
  }

  /// returns the righmost node in the tree

  template <typename Val, class Cmp, class Node>
  INLINE Node *BinSearchTree<Val, Cmp, Node>::_maxNode(Node *node) const {
    Node *prevNode = 0;

    for (; node; prevNode = node, node = node->rightChild()) {
    }

    return prevNode;
  }

  /// returns the next node according to weak ordering Cmp

  template <typename Val, class Cmp, class Node>
  INLINE Node *BinSearchTree<Val, Cmp, Node>::_succNode(Node *node) const {
    if (!node)
      return 0;

    if (node->rightChild())
      return _minNode(node->rightChild());

    Node *par = node->parent();

    while (par && (node->parentDir() == BinTreeDir::RIGHT_CHILD)) {
      node = par;
      par = par->parent();
    }

    return par;
  }

  /// returns the previous node according to weak ordering Cmp

  template <typename Val, class Cmp, class Node>
  INLINE Node *BinSearchTree<Val, Cmp, Node>::_prevNode(Node *node) const {
    if (!node)
      return 0;

    if (node->leftChild())
      return _maxNode(node->leftChild());

    Node *par = node->parent();

    while (par && (node->parentDir() == BinTreeDir::LEFT_CHILD)) {
      node = par;
      par = par->parent();
    }

    return par;
  }

  /// point the iterator to the next value in the binary search tree

  template <typename Val, class Cmp, class Node>
  INLINE BinSearchTreeIterator<Val, Cmp, Node> &
      BinSearchTreeIterator<Val, Cmp, Node>::
      operator++() {
    // if there is a current node, use it to compute the next node, else use
    // directly _next_node (this case obtains when the iterator was pointing
    // toward a node that has been deleted before we use operator++)
    _node = _node ? _tree->_succNode(_node) : _next_node;

    if (!_node) {
      _next_node = 0;
      _prev_node = 0;
      _parent = 0;
      _left_child = 0;
      _right_child = 0;
    }

    return *this;
  }

  /// point the iterator to the preceding value in the binary search tree

  template <typename Val, class Cmp, class Node>
  INLINE BinSearchTreeIterator<Val, Cmp, Node> &
      BinSearchTreeIterator<Val, Cmp, Node>::
      operator--() {
    // if there is a current node, use it to compute the preceding node, else use
    // directly _prev_node (this case obtains when the iterator was pointing
    // toward a node that has been deleted before we use operator--)
    _node = _node ? _tree->_prevNode(_node) : _prev_node;

    if (!_node) {
      _next_node = 0;
      _prev_node = 0;
      _parent = 0;
      _left_child = 0;
      _right_child = 0;
    }

    return *this;
  }

  /// checks whether two iterators are pointing toward the same element

  template <typename Val, class Cmp, class Node>
  INLINE bool BinSearchTreeIterator<Val, Cmp, Node>::
  operator==(const BinSearchTreeIterator<Val, Cmp, Node> &from) const {
    if (_node)
      return (_node == from._node);
    else
      return ((_node == from._node) && (_tree == from._tree) &&
              (_next_node == from._next_node) && (_prev_node == from._prev_node) &&
              (_parent == from._parent) && (_left_child == from._left_child) &&
              (_right_child == from._right_child));
  }

  /// checks whether two iterators are pointing toward different elements

  template <typename Val, class Cmp, class Node>
  INLINE bool BinSearchTreeIterator<Val, Cmp, Node>::
  operator!=(const BinSearchTreeIterator<Val, Cmp, Node> &from) const {
    if (_node)
      return (_node != from._node);
    else
      return ((_node != from._node) || (_tree != from._tree) ||
              (_next_node != from._next_node) || (_prev_node != from._prev_node) ||
              (_parent != from._parent) || (_left_child != from._left_child) ||
              (_right_child != from._right_child));
  }

  /// makes the iterator move to its parent node

  template <typename Val, class Cmp, class Node>
  INLINE BinSearchTreeIterator<Val, Cmp, Node> &
  BinSearchTreeIterator<Val, Cmp, Node>::up() {
    // if there is a current node, use it to compute its parent node, else use
    // directly _parent (this case obtains when the iterator was pointing
    // toward a node that has been deleted before we use operation up)
    _node = _node ? _node->parent() : _parent;

    if (!_node) {
      _next_node = 0;
      _prev_node = 0;
      _parent = 0;
      _left_child = 0;
      _right_child = 0;
    }

    return *this;
  }

  /// makes the iterator move to the left child of the node it points to

  template <typename Val, class Cmp, class Node>
  INLINE BinSearchTreeIterator<Val, Cmp, Node> &
  BinSearchTreeIterator<Val, Cmp, Node>::downLeft() {
    // if there is a current node, use it to compute its left child, else use
    // directly _left_child (this case obtains when the iterator was pointing
    // toward a node that has been deleted before we use operation downLeft)
    _node = _node ? _node->leftChild() : _left_child;

    if (!_node) {
      _next_node = 0;
      _prev_node = 0;
      _parent = 0;
      _left_child = 0;
      _right_child = 0;
    }

    return *this;
  }

  /// makes the iterator move to the right child of the node it points to

  template <typename Val, class Cmp, class Node>
  INLINE BinSearchTreeIterator<Val, Cmp, Node> &
  BinSearchTreeIterator<Val, Cmp, Node>::downRight() {
    // if there is a current node, use it to compute its right child, else use
    // directly _right_child (this case obtains when the iterator was pointing
    // toward a node that has been deleted before we use operation downRight)
    _node = _node ? _node->rightChild() : _right_child;

    if (!_node) {
      _next_node = 0;
      _prev_node = 0;
      _parent = 0;
      _left_child = 0;
      _right_child = 0;
    }

    return *this;
  }

  /* =========================================================================== */
  /* =========================================================================== */
  /* ===                      GENERIC BINARY SEARCH TREE                     === */
  /* =========================================================================== */
  /* =========================================================================== */

  /// basic constructor: returns an empty binary search tree

  template <typename Val, class Cmp, class Node>
  BinSearchTree<Val, Cmp, Node>::BinSearchTree(bool uniqueness_policy)
      : _root(0), _iterator_list(0), _uniqueness_policy(uniqueness_policy),
        _nb_elements(0) {
    GUM_CONSTRUCTOR(BinSearchTree);
    _iter_end._initialize(this, 0, false);
  }

  /// copy constructor

  template <typename Val, class Cmp, class Node>
  BinSearchTree<Val, Cmp, Node>::BinSearchTree(
      const BinSearchTree<Val, Cmp, Node> &from)
      : _root(0), _iterator_list(0), _uniqueness_policy(from._uniqueness_policy) {
    // for debugging purposes
    GUM_CONS_CPY(BinSearchTree);

    // copy the content of BinSearchTree "from"
    _root = _copy(from._root);
    _nb_elements = from._nb_elements;

    // initialize the end/rend iterator
    _iter_end._initialize(this, 0, false);
  }

  /// removes all the elements from the BinSearchTree

  template <typename Val, class Cmp, class Node>
  INLINE void BinSearchTree<Val, Cmp, Node>::clear() {
    // first we clear all the iterators, i.e., we detach them from the tree
    for (iterator *iter = _iterator_list, *next_iter = 0; iter; iter = next_iter) {
      next_iter = iter->_next_iter;
      iter->clear();
    }

    // now, delete the whole tree
    _deleteSubTree(_root);
    _root = 0;
    _nb_elements = 0;

    // note that there is no need to redefined end/rend as they do not rely
    // on the content of the tree
  }

  /// copy operator

  template <typename Val, class Cmp, class Node>
  BinSearchTree<Val, Cmp, Node> &BinSearchTree<Val, Cmp, Node>::
  operator=(const BinSearchTree<Val, Cmp, Node> &from) {
    // avoid self assignment
    if (this != &from) {
      // for debugging purposes
      GUM_OP_CPY(BinSearchTree);

      // if the tree is not currently empty, remove it
      clear();

      // copy binary tree "from"
      _uniqueness_policy = from._uniqueness_policy;
      _root = _copy(from._root); // note that we can copy from's tree structure
      // as from and this have the same ordering _cmp
      _nb_elements = from._nb_elements;

      // note that we do not need to update the end/rend iterator as, besides
      // field _tree, no other field is related to the current tree (i.e., all
      // _*_node are set to 0
    }

    return *this;
  }

  /// destructor

  template <typename Val, class Cmp, class Node>
  BinSearchTree<Val, Cmp, Node>::~BinSearchTree() {
    // for debugging purposes
    GUM_DESTRUCTOR(BinSearchTree);

    // clear all the iterators and remove all nodes
    clear();
  }

  /// a method for recursively copying the contents of a BinSearchTree

  template <typename Val, class Cmp, class Node>
  Node *BinSearchTree<Val, Cmp, Node>::_copy(Node *node, Node *parent,
                                             BinTreeDir dir) {
    // if there is no node to copy, abort
    if (!node)
      return 0;

    // create the copy of node
    Node *new_node = new Node(*node);

    if (parent)
      parent->insertChild(*new_node, dir);

    // if necessary, create the left and right subgraphs
    _copy(node->leftChild(), new_node, BinTreeDir::LEFT_CHILD);
    _copy(node->rightChild(), new_node, BinTreeDir::RIGHT_CHILD);

    return new_node;
  }

  /// a method for recursively destroying the content of a Binary search Tree

  template <typename Val, class Cmp, class Node>
  void BinSearchTree<Val, Cmp, Node>::_deleteSubTree(Node *node) {
    // if there is no node to remove, return
    if (!node)
      return;

    // delete the left and right subgraphs
    _deleteSubTree(node->leftChild());
    _deleteSubTree(node->rightChild());

    // delete the node itself
    delete node;
  }

  /** @brief creates a copy of the value, insert it in the tree and returns
   * the copy value */

  template <typename Val, class Cmp, class Node>
  INLINE Node *BinSearchTree<Val, Cmp, Node>::_insert(const Val &val) {
    // if the tree is not empty, search the binary search tree to know
    // where the node should be inserted
    if (_root) {
      Node *node = _root;

      while (true) {
        if (_cmp(val, node->value()))
          if (!node->leftChild()) {
            // here we are on a leaf => insert the new node
            ++_nb_elements;
            return node->insertLeftChild(val);
          } else {
            node = node->leftChild();
          }
        else if (_cmp(node->value(), val) || !_uniqueness_policy)
          if (!node->rightChild()) {
            // here we are on a leaf => insert the new node
            ++_nb_elements;
            return node->insertRightChild(val);
          } else {
            node = node->rightChild();
          }
        else {
          // here we found a node with the same key and the uniqueness policy
          // is set. So we should raise an exception
          GUM_ERROR(DuplicateElement, "key already defined in binary search tree");
        }
      }
    }

    // here the tree is empty, just create a new node
    _root = new Node(val);
    ++_nb_elements;
    return _root;
  }

  /** @brief creates a copy of the value, insert it in the tree and returns
   * the copy value */

  template <typename Val, class Cmp, class Node>
  INLINE const Val &BinSearchTree<Val, Cmp, Node>::insert(const Val &val) {
    return _insert(val)->value();
  }

  /// returns the value of the root of the tree

  template <typename Val, class Cmp, class Node>
  INLINE const Val &BinSearchTree<Val, Cmp, Node>::rootValue() const {
    if (_root == 0) {
      GUM_ERROR(NotFound, "no value in an empty Binary Search tree");
    }

    return _root->value();
  }

  /// returns the value of the leftmost node with the minimal key in the tree

  template <typename Val, class Cmp, class Node>
  INLINE const Val &BinSearchTree<Val, Cmp, Node>::minValue() const {
    if (_root == 0) {
      GUM_ERROR(NotFound, "no minimal value in an empty Binary Search tree");
    }

    return _minNode(_root)->value();
  }

  /// returns the value of the rightmost node with the maximal key in the tree

  template <typename Val, class Cmp, class Node>
  INLINE const Val &BinSearchTree<Val, Cmp, Node>::maxValue() const {
    if (_root == 0) {
      GUM_ERROR(NotFound, "no maximal value in an empty Binary Search tree");
    }

    return _maxNode(_root)->value();
  }

  /// returns the node containing a given value (0 if the value cannot be found)

  template <typename Val, class Cmp, class Node>
  INLINE Node *BinSearchTree<Val, Cmp, Node>::_getNode(const Val &val) const {
    // if the tree is not empty, search the binary search tree to know
    // where the node could be
    if (_root) {
      Node *node = _root;

      while (true) {
        if (_cmp(val, node->value())) {
          if (!node->leftChild())
            return 0;
          else
            node = node->leftChild();
        } else if (_cmp(node->value(), val)) {
          if (!node->rightChild())
            return 0;
          else
            node = node->rightChild();
        } else
          return node;
      }
    } else {
      return 0;
    }
  }

  /// indicates whether the BinSearchTree contains the value

  template <typename Val, class Cmp, class Node>
  INLINE bool BinSearchTree<Val, Cmp, Node>::contains(const Val &val) const {
    return (_getNode(val) != 0);
  }

  /// returns the number of elements in the binary search tree

  template <typename Val, class Cmp, class Node>
  INLINE Size BinSearchTree<Val, Cmp, Node>::size() const {
    return _nb_elements;
  }

  /// indicates whether the binary search tree is empty

  template <typename Val, class Cmp, class Node>
  INLINE bool BinSearchTree<Val, Cmp, Node>::empty() const {
    return (_nb_elements == 0);
  }

  /// displays the content of the tree (in increasing order w.r.t. Cmp)

  template <typename Val, class Cmp, class Node>
  const std::string BinSearchTree<Val, Cmp, Node>::toString() const {
    bool deja = false;
    std::stringstream stream;
    stream << "[";

    for (const_iterator iter = begin(); iter != end(); ++iter, deja = true) {
      if (deja)
        stream << " , ";

      stream << *iter;
    }

    stream << "]";

    return stream.str();
  }

  /// returns the current uniqueness policy

  template <typename Val, class Cmp, class Node>
  INLINE bool BinSearchTree<Val, Cmp, Node>::uniquenessPolicy() const {
    return _uniqueness_policy;
  }

  /** @brief enables the user to change dynamically the policy for checking
   * whether there can exist several identical elements in the binary tree */

  template <typename Val, class Cmp, class Node>
  INLINE void
  BinSearchTree<Val, Cmp, Node>::setUniquenessPolicy(const bool new_policy) {
    _uniqueness_policy = new_policy;
  }

  /// begin iterator

  template <typename Val, class Cmp, class Node>
  INLINE BinSearchTreeIterator<Val, Cmp, Node>
  BinSearchTree<Val, Cmp, Node>::begin() {
    BinSearchTreeIterator<Val, Cmp, Node> iter;
    iter._initialize(this, _minNode(_root), true);
    return iter;
  }

  /// begin iterator

  template <typename Val, class Cmp, class Node>
  INLINE BinSearchTreeIterator<Val, Cmp, Node>
  BinSearchTree<Val, Cmp, Node>::begin() const {
    BinSearchTreeIterator<Val, Cmp, Node> iter;
    iter._initialize(this, _minNode(_root), true);
    return iter;
  }

  /// rbegin iterator

  template <typename Val, class Cmp, class Node>
  INLINE BinSearchTreeIterator<Val, Cmp, Node>
  BinSearchTree<Val, Cmp, Node>::rbegin() {
    BinSearchTreeIterator<Val, Cmp, Node> iter;
    iter._initialize(this, _maxNode(_root), true);
    return iter;
  }

  /// rbegin iterator

  template <typename Val, class Cmp, class Node>
  INLINE BinSearchTreeIterator<Val, Cmp, Node>
  BinSearchTree<Val, Cmp, Node>::rbegin() const {
    BinSearchTreeIterator<Val, Cmp, Node> iter;
    iter._initialize(this, _maxNode(_root), true);
    return iter;
  }

  /// end iterator

  template <typename Val, class Cmp, class Node>
  INLINE const BinSearchTreeIterator<Val, Cmp, Node> &
  BinSearchTree<Val, Cmp, Node>::end() {
    return _iter_end;
  }

  /// end iterator

  template <typename Val, class Cmp, class Node>
  INLINE const BinSearchTreeIterator<Val, Cmp, Node> &
  BinSearchTree<Val, Cmp, Node>::end() const {
    return _iter_end;
  }

  /// rend iterator

  template <typename Val, class Cmp, class Node>
  INLINE const BinSearchTreeIterator<Val, Cmp, Node> &
  BinSearchTree<Val, Cmp, Node>::rend() {
    return _iter_end;
  }

  /// rend iterator

  template <typename Val, class Cmp, class Node>
  INLINE const BinSearchTreeIterator<Val, Cmp, Node> &
  BinSearchTree<Val, Cmp, Node>::rend() const {
    return _iter_end;
  }

  /// returns an iterator pointing to the root of the tree

  template <typename Val, class Cmp, class Node>
  INLINE BinSearchTreeIterator<Val, Cmp, Node>
  BinSearchTree<Val, Cmp, Node>::root() {
    BinSearchTreeIterator<Val, Cmp, Node> iter;
    iter._initialize(this, _root, true);
    return iter;
  }

  /// returns an iterator pointing to the root of the tree

  template <typename Val, class Cmp, class Node>
  INLINE BinSearchTreeIterator<Val, Cmp, Node>
  BinSearchTree<Val, Cmp, Node>::root() const {
    BinSearchTreeIterator<Val, Cmp, Node> iter;
    iter._initialize(this, _root, true);
    return iter;
  }

  /// erase the node passed in argument

  template <typename Val, class Cmp, class Node>
  void BinSearchTree<Val, Cmp, Node>::_erase(Node *node) {
    if (!node)
      return;

    // update all the iterators pointing to node that they should point elsewhere
    __updateEraseIterators(node);

    // update the number of elements contained in the tree
    --_nb_elements;

    // now remove the node from the tree:

    // if the node has no children, then just remove it
    if (!node->leftChild() && !node->rightChild()) {
      // if the node was the only one in the tree, then the tree becomes empty
      if (!node->parent())
        _root = 0;

      // note that, when node has a parent, there is no need to remove the
      // link between node and this parent: this will be taken care of by
      // node's destructor.
    }
    // if there is just a right child
    else if (!node->leftChild()) {
      // just relink the right child with the parent (if any)
      if (!node->parent()) {
        // in this case, no need to remove the link between "node" and its child:
        // this will be taken care of by the destructor of "node"
        _root = node->rightChild();
      } else {
        Node *parent = node->parent(), *child = node->rightChild();
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
        // in this case, no need to remove the link between "node" and its child:
        // this will be taken care of by the destructor of "node"
        _root = node->leftChild();
      } else {
        Node *parent = node->parent(), *child = node->leftChild();
        BinTreeDir dir = node->parentDir();
        parent->eraseLink(dir);
        node->eraseLeftLink();
        parent->insertChild(*child, dir);
      }
    }
    // ok, here there are two children
    else {
      // the idea is to get the successor of "node" and substitute "node" by it.
      // As "node" has two children, we are sure that the successor is one of
      // node's descendants. Moreover, by its very definition, this successor has
      // no left child. Hence, two cases can obtain:
      // 1/ the successor is precisely node's right child. In this case, we just
      //    have to make node's left child be the left child of the successor,
      //    and node's parent be the successor's parent, and the tree is again
      //    a binary search tree.
      // 2/ the successor is not node's right child. In this case, we know that
      //    the successor has a parent different from node and that the successor
      //    is a left child of this parent. We just need to put the right child
      //    of the successor (if any) as the left child of its parent, and to
      //    replace "node" by the successor.
      Node *successor = _succNode(node);

      if (successor == node->rightChild()) { // proceed to case 1:
        Node *left_child = node->leftChild();
        node->eraseLeftLink();
        node->eraseRightLink();
        successor->insertLeftChild(*left_child);

        if (!node->parent()) {
          // in this case, no need to remove the link between "node" and the
          // successor: this will be taken care of by the destructor of "node"
          _root = successor;
        } else {
          // rechain node's parent with its successor
          BinTreeDir par_dir = node->parentDir();
          Node *parent = node->parent();
          parent->eraseLink(par_dir);
          parent->insertChild(*successor, par_dir);
        }
      } else { // proceed to case 2:
        Node *parent = successor->parent();
        parent->eraseLeftLink();

        if (successor->rightChild()) {
          Node *succ_child = successor->rightChild();
          successor->eraseRightLink();
          parent->insertLeftChild(*succ_child);
        }

        Node *left = node->leftChild(), *right = node->rightChild();
        node->eraseLeftLink();
        node->eraseRightLink();
        successor->insertLeftChild(*left);
        successor->insertRightChild(*right);

        if (!node->parent()) {
          _root = successor;
        } else {
          // rechain node's parent with its successor
          BinTreeDir par_dir = node->parentDir();
          Node *parent = node->parent();
          parent->eraseLink(par_dir);
          parent->insertChild(*successor, par_dir);
        }
      }
    }

    // now we shall physically remove node from memory
    delete node;
  }

  /// erase a node with the given value

  template <typename Val, class Cmp, class Node>
  INLINE void BinSearchTree<Val, Cmp, Node>::erase(const Val &val) {
    Node *n = _getNode(val);

    if (n == nullptr)
      GUM_ERROR(gum::NotFound, "Value \"" << val << "\" not found");

    _erase(n);
  }

  /// erase the node pointed to by the iterator

  template <typename Val, class Cmp, class Node>
  INLINE void BinSearchTree<Val, Cmp, Node>::erase(const iterator &iter) {
    _erase(iter._node);
  }

  /// update all iterators when a given node is deleted

  template <typename Val, class Cmp, class Node>
  void BinSearchTree<Val, Cmp, Node>::__updateEraseIterators(Node *node) {
    for (iterator *iter = _iterator_list; iter; iter = iter->_next_iter) {
      // if the iterator points toward the node to be deleted, make its _node
      // field point to 0 and update accordingly its other fields
      if (iter->_node == node) {
        iter->_node = 0;
        iter->_next_node = _succNode(node);
        iter->_prev_node = _prevNode(node);
        iter->_parent = node->parent();
        iter->_left_child = node->leftChild();
        iter->_right_child = node->rightChild();
      } else if (!iter->_node) {
        if (iter->_next_node == node)
          iter->_next_node = _succNode(node);

        if (iter->_prev_node == node)
          iter->_prev_node = _prevNode(node);

        if (iter->_parent == node)
          iter->_parent = node->parent();

        if (iter->_left_child == node)
          iter->_left_child = node->leftChild();

        if (iter->_right_child == node)
          iter->_right_child = node->rightChild();
      }
    }
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
