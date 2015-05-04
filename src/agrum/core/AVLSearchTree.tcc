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
 * @brief Node class for AVL binary search trees
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// to ease IDE parsers
#include <agrum/core/AVLSearchTree.h>

namespace gum {

  /* =========================================================================== */
  /* =========================================================================== */
  /* ===                  NODES FOR AVL BINARY SEARCH TREES                  === */
  /* =========================================================================== */
  /* =========================================================================== */

  /// basic constructor: a node without parent nor children

  template <typename Val>
  INLINE BinTreeNode4AVL<Val>::BinTreeNode4AVL(const Val &val)
      : BinTreeNode<Val>(val), __height(1) {
    GUM_CONSTRUCTOR(BinTreeNode4AVL);
  }

  /** @brief copy constructor: creates a new disconnected node with the same
   * value as "from" */

  template <typename Val>
  INLINE BinTreeNode4AVL<Val>::BinTreeNode4AVL(const BinTreeNode4AVL<Val> &from)
      : BinTreeNode<Val>(from), __height(from.__height) {
    GUM_CONS_CPY(BinTreeNode4AVL);
  }

  /// destructor

  template <typename Val> INLINE BinTreeNode4AVL<Val>::~BinTreeNode4AVL() {
    GUM_DESTRUCTOR(BinTreeNode4AVL);
  }

  /** @brief copy operator: copy the value of from into this. However, this
   * does not change the current connections (parents and children) of this. */

  template <typename Val>
  INLINE BinTreeNode4AVL<Val> &BinTreeNode4AVL<Val>::
  operator=(const BinTreeNode4AVL<Val> &from) {
    GUM_OP_CPY(BinTreeNode4AVL);
    BinTreeNode<Val>::operator=(from);
    return *this;
  }

  /// alias for method value

  template <typename Val> INLINE Val &BinTreeNode4AVL<Val>::operator*() {
    return BinTreeNode<Val>::operator*();
  }

  /// returns the value stored in a node of the binary search tree

  template <typename Val> INLINE Val &BinTreeNode4AVL<Val>::value() {
    return BinTreeNode<Val>::value();
  }

  /// returns the given child of a node

  template <typename Val>
  INLINE BinTreeNode4AVL<Val> *BinTreeNode4AVL<Val>::child(BinTreeDir dir) const {
    return static_cast<BinTreeNode4AVL<Val> *>(BinTreeNode<Val>::child(dir));
  }

  /// returns the given child of a node

  template <typename Val>
  INLINE BinTreeNode4AVL<Val> *BinTreeNode4AVL<Val>::leftChild() const {
    return static_cast<BinTreeNode4AVL<Val> *>(BinTreeNode<Val>::leftChild());
  }

  /// returns the given child of a node

  template <typename Val>
  INLINE BinTreeNode4AVL<Val> *BinTreeNode4AVL<Val>::rightChild() const {
    return static_cast<BinTreeNode4AVL<Val> *>(BinTreeNode<Val>::rightChild());
  }

  /// returns the parent of a node

  template <typename Val>
  INLINE BinTreeNode4AVL<Val> *BinTreeNode4AVL<Val>::parent() const {
    return static_cast<BinTreeNode4AVL<Val> *>(BinTreeNode<Val>::parent());
  }

  /// returns the direction of the edge parent->current node, if any

  template <typename Val> INLINE BinTreeDir BinTreeNode4AVL<Val>::parentDir() const {
    return BinTreeNode<Val>::parentDir();
  }

  /// rotation droite

  template <typename Val>
  INLINE BinTreeNode4AVL<Val> *BinTreeNode4AVL<Val>::__rightRotation() {
    BinTreeNode4AVL<Val> *node_P = leftChild();
    BinTreeNode4AVL<Val> *V = node_P->rightChild();
    BinTreeNode4AVL<Val> *the_parent = parent();
    BinTreeDir parent_dir = parentDir();
    unsigned int left_height = 0;
    unsigned int right_height = rightChild() ? rightChild()->__height : 0;

    eraseLeftLink();

    if (V) {
      node_P->eraseRightLink();
      BinTreeNode<Val>::insertLeftChild(*V);
      left_height = V->__height;
    }

    __height = 1 + std::max(left_height, right_height);

    if (the_parent) {
      the_parent->eraseLink(parent_dir);
      the_parent->BinTreeNode<Val>::insertChild(*node_P, parent_dir);
    }

    node_P->BinTreeNode<Val>::insertRightChild(*this);
    left_height = node_P->leftChild() ? node_P->leftChild()->__height : 0;
    node_P->__height = 1 + std::max(left_height, __height);

    return node_P;
  }

  /// rotation gauche

  template <typename Val>
  INLINE BinTreeNode4AVL<Val> *BinTreeNode4AVL<Val>::__leftRotation() {
    BinTreeNode4AVL<Val> *node_Q = rightChild();
    BinTreeNode4AVL<Val> *V = node_Q->leftChild();
    BinTreeNode4AVL<Val> *the_parent = parent();
    BinTreeDir parent_dir = parentDir();
    unsigned int right_height = 0;
    unsigned int left_height = leftChild() ? leftChild()->__height : 0;

    eraseRightLink();

    if (V) {
      node_Q->eraseLeftLink();
      BinTreeNode<Val>::insertRightChild(*V);
      right_height = V->__height;
    }

    __height = 1 + std::max(left_height, right_height);

    if (the_parent) {
      the_parent->eraseLink(parent_dir);
      the_parent->BinTreeNode<Val>::insertChild(*node_Q, parent_dir);
    }

    node_Q->BinTreeNode<Val>::insertLeftChild(*this);
    right_height = node_Q->rightChild() ? node_Q->rightChild()->__height : 0;
    node_Q->__height = 1 + std::max(__height, right_height);

    return node_Q;
  }

  /// balance the whole tree, starting from the current node

  template <typename Val>
  INLINE BinTreeNode4AVL<Val> *BinTreeNode4AVL<Val>::__balance() {
    BinTreeNode4AVL<Val> *node = this, *root = this;

    while (node) {
      // compute the heights of the left and right trees
      unsigned int left_height = node->leftChild() ? node->leftChild()->__height : 0;
      unsigned int right_height =
          node->rightChild() ? node->rightChild()->__height : 0;
      unsigned int old_height = node->__height;
      node->__height = 1 + std::max(left_height, right_height);

      // check whether we need a single or a double rotation
      // see the AVL trees literature to see what need be done
      if (left_height - right_height == 2) {
        BinTreeNode4AVL<Val> *left_child = node->leftChild();
        left_height =
            left_child->leftChild() ? left_child->leftChild()->__height : 0;
        right_height =
            left_child->rightChild() ? left_child->rightChild()->__height : 0;

        if (left_height < right_height)
          left_child->__leftRotation();

        node = node->__rightRotation();
      } else if (right_height - left_height == 2) {
        BinTreeNode4AVL<Val> *right_child = node->rightChild();
        left_height =
            right_child->leftChild() ? right_child->leftChild()->__height : 0;
        right_height =
            right_child->rightChild() ? right_child->rightChild()->__height : 0;

        if (right_height < left_height)
          right_child->__rightRotation();

        node = node->__leftRotation();
      }
      // we need not rebalance the tree here. If, in addition, the height did
      // not change, then there is no need to rebalance the ancestors. Hence
      // we return 0 to indicate that the root has not changed
      else if (node->__height == old_height) {
        return 0;
      }

      // ok, here "node" is the root of a balanced tree. Now we need to check
      // whether the ancestors are well balanced
      root = node;
      node = node->parent();
    }

    // return the current root of the tree
    return root;
  }

  /// adds a new left child to the current node

  template <typename Val>
  INLINE BinTreeNode4AVL<Val> *
  BinTreeNode4AVL<Val>::insertLeftChild(const Val &val) {
    BinTreeNode4AVL<Val> *new_child = new BinTreeNode4AVL<Val>(val);
    BinTreeNode<Val>::insertLeftChild(*new_child);
    return new_child;
  }

  /// adds a new left child to the current node

  template <typename Val>
  INLINE void
  BinTreeNode4AVL<Val>::insertLeftChild(BinTreeNode4AVL<Val> &new_child) {
    BinTreeNode<Val>::insertLeftChild(new_child);
  }

  /// adds a new right child to the current node

  template <typename Val>
  INLINE BinTreeNode4AVL<Val> *
  BinTreeNode4AVL<Val>::insertRightChild(const Val &val) {
    BinTreeNode4AVL<Val> *new_child = new BinTreeNode4AVL<Val>(val);
    BinTreeNode<Val>::insertRightChild(*new_child);
    return new_child;
  }

  /// adds a new right child to the current node

  template <typename Val>
  INLINE void
  BinTreeNode4AVL<Val>::insertRightChild(BinTreeNode4AVL<Val> &new_child) {
    BinTreeNode<Val>::insertRightChild(new_child);
  }

  /// adds a new child to the current node

  template <typename Val>
  INLINE BinTreeNode4AVL<Val> *
  BinTreeNode4AVL<Val>::insertChild(const Val &val, BinTreeDir child_dir) {
    BinTreeNode4AVL<Val> *new_child = new BinTreeNode4AVL<Val>(val);
    BinTreeNode<Val>::insertChild(*new_child, child_dir);
    return new_child;
  }

  /// adds a new child to the current node

  template <typename Val>
  INLINE void BinTreeNode4AVL<Val>::insertChild(BinTreeNode4AVL<Val> &new_child,
                                                BinTreeDir child_dir) {
    BinTreeNode<Val>::insertChild(new_child, child_dir);
  }

  /// remove the link between the current node and its left child

  template <typename Val> INLINE void BinTreeNode4AVL<Val>::eraseLeftLink() {
    BinTreeNode<Val>::eraseLeftLink();
  }

  /// remove the link between the current node and its right child

  template <typename Val> INLINE void BinTreeNode4AVL<Val>::eraseRightLink() {
    BinTreeNode<Val>::eraseRightLink();
  }

  /// remove the link between the current node and one of its children

  template <typename Val>
  INLINE void BinTreeNode4AVL<Val>::eraseLink(BinTreeDir tree_dir) {
    BinTreeNode<Val>::eraseLink(tree_dir);
  }

  /// returns the leftmost node of the current tree

  template <typename Val>
  INLINE BinTreeNode4AVL<Val> *BinTreeNode4AVL<Val>::leftmostNode() const {
    return static_cast<BinTreeNode4AVL<Val> *>(BinTreeNode<Val>::leftmostNode());
  }

  /// returns the rightmost node of the current tree

  template <typename Val>
  INLINE BinTreeNode4AVL<Val> *BinTreeNode4AVL<Val>::rightmostNode() const {
    return static_cast<BinTreeNode4AVL<Val> *>(BinTreeNode<Val>::rightmostNode());
  }

  /// returns the top ancestor of the current tree

  template <typename Val>
  INLINE BinTreeNode4AVL<Val> *BinTreeNode4AVL<Val>::root() const {
    return static_cast<BinTreeNode4AVL<Val> *>(BinTreeNode<Val>::root());
  }

  /* =========================================================================== */
  /* =========================================================================== */
  /* ===                        AVL BINARY SEARCH TREE                       === */
  /* =========================================================================== */
  /* =========================================================================== */

  /// basic constructor: returns an empty binary search tree

  template <typename Val, class Cmp>
  INLINE AVLSearchTree<Val, Cmp>::AVLSearchTree(bool uniqueness_policy)
      : BinSearchTree<Val, Cmp, Node>(uniqueness_policy) {
    GUM_CONSTRUCTOR(AVLSearchTree);
  }

  /// copy constructor

  template <typename Val, class Cmp>
  INLINE AVLSearchTree<Val, Cmp>::AVLSearchTree(const AVLSearchTree<Val, Cmp> &from)
      : BinSearchTree<Val, Cmp, Node>(from) {
    GUM_CONS_CPY(AVLSearchTree);
  }

  /// destructor

  template <typename Val, class Cmp>
  INLINE AVLSearchTree<Val, Cmp>::~AVLSearchTree() {
    GUM_DESTRUCTOR(AVLSearchTree);
  }

  /// copy operator

  template <typename Val, class Cmp>
  INLINE AVLSearchTree<Val, Cmp> &AVLSearchTree<Val, Cmp>::
  operator=(const AVLSearchTree<Val, Cmp> &from) {
    GUM_OP_CPY(AVLSearchTree);
    BinSearchTree<Val, Cmp, Node>::operator=(from);
    return *this;
  }

  /// erase the node passed in argument

  template <typename Val, class Cmp>
  void AVLSearchTree<Val, Cmp>::_erase(BinTreeNode4AVL<Val> *node) {
    if (!node)
      return;

    // update all the iterators pointing to node that they should point elsewhere
    BinSearchTree<Val, Cmp, Node>::__updateEraseIterators(node);

    // update the number of elements contained in the tree
    --BinSearchTree<Val, Cmp, Node>::_nb_elements;

    // now remove the node from the tree:

    // if the node has no children, then just remove it
    if (!node->leftChild() && !node->rightChild()) {
      // if the node was the only one in the tree, then the tree becomes empty
      if (!node->parent()) {
        BinSearchTree<Val, Cmp, Node>::_root = 0;
        delete node;
      } else {
        // note that, when node has a parent, there is no need to remove the
        // link between node and this parent: this will be taken care of by
        // node's destructor.
        Node *parent = node->parent();
        delete node;
        Node *new_root = parent->__balance();

        if (new_root)
          BinSearchTree<Val, Cmp, Node>::_root = new_root;
      }
    }
    // if there is just a right child
    else if (!node->leftChild()) {
      // just relink the right child with the parent (if any)
      if (!node->parent()) {
        // in this case, no need to remove the link between "node" and its child:
        // this will be taken care of by the destructor of "node"
        BinSearchTree<Val, Cmp, Node>::_root = node->rightChild();
        delete node;
      } else {
        Node *parent = node->parent(), *child = node->rightChild();
        BinTreeDir dir = node->parentDir();
        parent->eraseLink(dir);
        node->eraseRightLink();
        parent->insertChild(*child, dir);
        delete node;
        Node *new_root = parent->__balance();

        if (new_root)
          BinSearchTree<Val, Cmp, Node>::_root = new_root;
      }
    }
    // if there is just a left child
    else if (!node->rightChild()) {
      // just relink the left child with the parent (if any)
      if (!node->parent()) {
        // in this case, no need to remove the link between "node" and its child:
        // this will be taken care of by the destructor of "node"
        BinSearchTree<Val, Cmp, Node>::_root = node->leftChild();
        delete node;
      } else {
        Node *parent = node->parent(), *child = node->leftChild();
        BinTreeDir dir = node->parentDir();
        parent->eraseLink(dir);
        node->eraseLeftLink();
        parent->insertChild(*child, dir);
        delete node;
        Node *new_root = parent->__balance();

        if (new_root)
          BinSearchTree<Val, Cmp, Node>::_root = new_root;
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
      Node *successor = this->_succNode(node);

      if (successor == node->rightChild()) { // proceed to case 1:
        Node *left_child = node->leftChild();
        node->eraseLeftLink();
        node->eraseRightLink();
        successor->insertLeftChild(*left_child);

        if (node->parent()) {
          // rechain node's parent with its successor
          BinTreeDir par_dir = node->parentDir();
          Node *parent = node->parent();
          parent->eraseLink(par_dir);
          parent->insertChild(*successor, par_dir);
        }

        Node *new_root = successor->__balance();

        if (new_root)
          BinSearchTree<Val, Cmp, Node>::_root = new_root;
        else if (BinSearchTree<Val, Cmp, Node>::_root == node)
          BinSearchTree<Val, Cmp, Node>::_root = successor;
      } else { // proceed to case 2:
        Node *parent = successor->parent();
        parent->eraseLeftLink();

        if (successor->rightChild()) {
          Node *succ_child = successor->rightChild();
          successor->eraseRightLink();
          parent->insertLeftChild(*succ_child);
        }

        Node *left = node->leftChild(), *right = node->rightChild();
        successor->__height = node->__height;
        node->eraseLeftLink();
        node->eraseRightLink();
        successor->insertLeftChild(*left);
        successor->insertRightChild(*right);

        if (node->parent()) {
          // rechain node's parent with its successor
          BinTreeDir par_dir = node->parentDir();
          Node *the_parent = node->parent();
          the_parent->eraseLink(par_dir);
          the_parent->insertChild(*successor, par_dir);
        }

        Node *new_root = parent->__balance();

        if (new_root)
          BinSearchTree<Val, Cmp, Node>::_root = new_root;
        else if (BinSearchTree<Val, Cmp, Node>::_root == node)
          BinSearchTree<Val, Cmp, Node>::_root = successor;
      }

      // now we shall physically remove node from memory
      delete node;
    }
  }

  /// check the tree to determine whether it is actually AVL balanced

  template <typename Val, class Cmp>
  bool AVLSearchTree<Val, Cmp>::checkAVLStructure() const {
    for (iterator iter = begin(); iter != end(); ++iter) {
      Node *node = iter._node;

      // compute the heights of the left and right trees
      unsigned int left_height = node->leftChild() ? node->leftChild()->__height : 0;
      unsigned int right_height =
          node->rightChild() ? node->rightChild()->__height : 0;

      if (node->__height != 1 + std::max(left_height, right_height))
        return false;
    }

    return true;
  }

  /** @brief creates a copy of the value, insert it in the tree and returns
   * the copy value */

  template <typename Val, class Cmp>
  INLINE BinTreeNode4AVL<Val> *AVLSearchTree<Val, Cmp>::_insert(const Val &val) {
    // insert the new value into the tree
    Node *new_node = BinSearchTree<Val, Cmp, Node>::_insert(val);

    // balance the tree and update its root
    if (new_node->parent()) {
      Node *new_root = new_node->parent()->__balance();

      if (new_root)
        BinSearchTree<Val, Cmp, Node>::_root = new_root;
    } else {
      BinSearchTree<Val, Cmp, Node>::_root = new_node;
    }

    return new_node;
  }

  template <typename Val, class Cmp>
  const std::string AVLSearchTree<Val, Cmp>::toString() const {
    std::stringstream stream;
    stream << "{ ";
    bool first = true;

    for (iterator iter = begin(); iter != end(); ++iter) {
      if (!first)
        stream << ", ";
      else
        first = false;

      stream << *iter;
    }

    stream << " }";
    return stream.str();
  }

  /// for friendly displaying the content of AVLSearchTree

  template <typename Val, class Cmp>
  std::ostream &operator<<(std::ostream &stream,
                           const AVLSearchTree<Val, Cmp> &avl) {
    stream << avl.toString();
    return stream;
  }
} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
