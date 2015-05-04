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
 * @brief Node class for various binary trees
 *
 * BinTreeNode is the base class for nodes of binary trees. It is equipped with
 * the material to add nodes to or remove them from the tree. The class ensures
 * that trees are always a coherent structure, that is, it is not possible to
 * have a node X having, say, a left child, and this child having no parent or a
 * parent different from X.
 * @par Usage example:
 * @code
 * // create a node containing an integer
 * BinTreeNode<int> node1 (33);
 *
 * // get the integer contained in the node
 * std::cerr << node1.value () << std::endl;
 * std::cerr << *node1 << std::endl;
 *
 * // create a disconnected node containing the same value as node1
 * BinTreeNode<int> node2 = node1;
 * BinTreeNode<int> node3 (3); node3 = node1;
 *
 * // insert left and right children
 * node1.insertLeftChild ( node2 );
 * node1.insertRightChild ( node 3 );
 *
 * BinTreeNode<int>* node4 = node2.insertLeftChild ( 3 );
 * BinTreeNode<int>* node5 = node2.insertRightChild (4 );
 *
 * BinTreeNode<int>* node6 = node4.insertChild ( 5, GUM_BIN_TREE_LEFT_CHILD );
 * BinTreeNode<int>* node7 = node4.insertChild ( 6, GUM_BIN_TREE_RIGHT_CHILD );
 *
 * BinTreeNode<int> node8 (55), node9(44);
 * node3.insertChild ( node8, GUM_BIN_TREE_LEFT_CHILD );
 * node3.insertChild ( node9, GUM_BIN_TREE_RIGHT_CHILD );
 *
 * // get the parents and children
 * BinTreeNode<int>* node8 = node.parent();
 * node8 = node1.leftChild();
 * node8 = node1.rightChild();
 * node8 = node1.child(gum::GUM_BIN_TREE_LEFT_CHILD);
 * node8 = node1.child(gum::GUM_BIN_TREE_RIGHT_CHILD);
 *
 * // remove links between parents and children
 * node1.eraseLeftLink ();  // there is no more connection between node1 and node2
 * node1.eraseRightLink (); // there is no more connection between node1 and node3
 * node2.eraseLink ( gum::GUM_BIN_TREE_LEFT_CHILD);  // erase (node2,node4)
 * node2.eraseLink ( gum::GUM_BIN_TREE_RIGHT_CHILD); // erase (node2;node5)
 * @endcode
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_BIN_TREE_NODE_H
#define GUM_BIN_TREE_NODE_H

#include <agrum/config.h>

namespace gum {

  /// the direction of a given edge in a binary tree
  enum class BinTreeDir : char { LEFT_CHILD = 0, RIGHT_CHILD = 1, NO_PARENT = 2 };

  /* =========================================================================== */
  /* ===                   GENERIC NODE FOR A BINARY TREE                    === */
  /* =========================================================================== */
  /** @class BinTreeNode
   * @brief Nodes of a binary trees
   * @ingroup basicstruct_group
   * BinTreeNode is the base class for nodes of binary trees. It is equipped with
   * the material to add nodes to or remove them from the tree. The class ensures
   * that trees are always a coherent structure, that is, it is not possible to
   * have a node X having, say, a left child, and this child having no parent or a
   * parent different from X.
   * @par Usage example:
   * @code
   * // create a node containing an integer
   * BinTreeNode<int> node1 (33);
   *
   * // get the integer contained in the node
   * std::cerr << node1.value () << std::endl;
   * std::cerr << *node1 << std::endl;
   *
   * // create a disconnected node containing the same value as node1
   * BinTreeNode<int> node2 = node1;
   * BinTreeNode<int> node3 (3); node3 = node1;
   *
   * // insert left and right children
   * node1.insertLeftChild ( node2 );
   * node1.insertRightChild ( node 3 );
   *
   * BinTreeNode<int>* node4 = node2.insertLeftChild ( 3 );
   * BinTreeNode<int>* node5 = node2.insertRightChild (4 );
   *
   * BinTreeNode<int>* node6 = node4.insertChild ( 5, GUM_BIN_TREE_LEFT_CHILD );
   * BinTreeNode<int>* node7 = node4.insertChild ( 6, GUM_BIN_TREE_RIGHT_CHILD );
   *
   * BinTreeNode<int> node8 (55), node9(44);
   * node3.insertChild ( node8, GUM_BIN_TREE_LEFT_CHILD );
   * node3.insertChild ( node9, GUM_BIN_TREE_RIGHT_CHILD );
   *
   * // get the parents and children
   * BinTreeNode<int>* node8 = node.parent();
   * node8 = node1.leftChild();
   * node8 = node1.rightChild();
   * node8 = node1.child(gum::GUM_BIN_TREE_LEFT_CHILD);
   * node8 = node1.child(gum::GUM_BIN_TREE_RIGHT_CHILD);
   *
   * // remove links between parents and children
   * node1.eraseLeftLink ();  // erase the connection between node1 and node2
   * node1.eraseRightLink (); // erase the connection between node1 and node3
   * node2.eraseLink ( gum::GUM_BIN_TREE_LEFT_CHILD);  // erase (node2,node4)
   * node2.eraseLink ( gum::GUM_BIN_TREE_RIGHT_CHILD); // erase (node2;node5)
   * @endcode
   */
  /* =========================================================================== */
  template <typename Val> class BinTreeNode {
    public:
    // ############################################################################
    // constructors / destructors
    // ############################################################################
    /// @{

    /// basic constructor: a node without parent nor children

    BinTreeNode(const Val &v);

    /** @brief copy constructor: creates a new disconnected node with the same
     * value as "from"
     *
     * @warning Although the new node contains the same value as from, it has
     * no parent, nor any children, even if from has some. */

    BinTreeNode(const BinTreeNode<Val> &from);

    /// destructor
    /** in addition to removing the node, this method updates appropriately
     * its parent and children */

    ~BinTreeNode();

    /// @}

    // ############################################################################
    // operators
    // ############################################################################
    /// @{

    /** @brief copy operator: copy the value of from into this. However, this
     * does not change the current connections (parents and children) of this. */

    BinTreeNode<Val> &operator=(const BinTreeNode<Val> &from);

    /// alias for method value

    Val &operator*();

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// returns the value stored in a node of the binary search tree

    Val &value();

    /// returns the given child of a node
    /** @warning if the child does not exists, the method returns a 0 pointer. */

    BinTreeNode<Val> *child(BinTreeDir dir) const;

    /// returns the given child of a node
    /** @warning if the child does not exists, the method returns a 0 pointer. */

    BinTreeNode<Val> *leftChild() const;

    /// returns the given child of a node
    /** @warning if the child does not exists, the method returns a 0 pointer. */

    BinTreeNode<Val> *rightChild() const;

    /// returns the parent of a node
    /** @warning if the parent does not exists, the method returns a 0 pointer. */

    BinTreeNode<Val> *parent() const;

    /// returns the direction of the edge parent->current node, if any

    BinTreeDir parentDir() const;

    /// adds a new left child to the current node
    /** @warning the new child is created on the C++ heap (i.e., using a
     * dynamic memory allocation)
     * @returns a pointer on the new created child
     * @throw DuplicateElement if the current node had already a left child */

    BinTreeNode<Val> *insertLeftChild(const Val &);

    /// adds a new left child to the current node
    /** @throw DuplicateElement if the current node had already a left child or
     * if new_child already has a parent */

    void insertLeftChild(BinTreeNode<Val> &new_child);

    /// adds a new right child to the current node
    /**  @returns a pointer on the new created child
     * @throw DuplicateElement if the current node had already a left child */

    BinTreeNode<Val> *insertRightChild(const Val &);

    /// adds a new right child to the current node
    /** @throw DuplicateElement if the current node had already a left child or
     * if new_child already has a parent */

    void insertRightChild(BinTreeNode<Val> &new_child);

    /// adds a new child to the current node
    /**  @returns a pointer on the new created child
     * @throw DuplicateElement if the current node had already a child in the
     * child_dir subtree */

    BinTreeNode<Val> *insertChild(const Val &, BinTreeDir child_dir);

    /// adds a new child to the current node
    /** @throw DuplicateElement if the current node had already a child in the
     * child_dir direction or if new_child already has a parent */

    void insertChild(BinTreeNode<Val> &new_child, BinTreeDir child_dir);

    /// remove the link between the current node and its left child
    /** Note that only the link is removed, i.e., the left child is not removed
     * itself nor, a fortiori, the left subtree of the current node. If there
     * is no left child, the method does nothing. In particular, it does not
     * raise any exception. */

    void eraseLeftLink();

    /// remove the link between the current node and its right child
    /** Note that only the link is removed, i.e., the right child is not removed
     * itself nor, a fortiori, the right subtree of the current node. If there
     * is no right child, the method does nothing. In particular, it does not
     * raise any exception. */

    void eraseRightLink();

    /// remove the link between the current node and one of its children
    /** Note that only the link is removed, i.e., the child is not removed
     * itself nor, a fortiori, its subtree. If the child does not exist, the
     * method does nothing. In particular, it does not raise any exception. */

    void eraseLink(BinTreeDir tree_dir);

    /// returns the leftmost node of the current tree
    /** if there is no left child, the method returns this */

    BinTreeNode<Val> *leftmostNode() const;

    /// returns the rightmost node of the current tree
    /** if there is no right child, the method returns this */

    BinTreeNode<Val> *rightmostNode() const;

    /// returns the top ancestor of the current tree
    /** if the current node has no parent, the the method returns this */

    BinTreeNode<Val> *root() const;

    /// @}

    protected:
    /// the value stored in a node of the tree
    Val _val;

    /// the parent of the node
    BinTreeNode<Val> *_parent;

    /// the direction to follow from the parent to reach the current node
    BinTreeDir _parent_dir;

    /// the children of the current node
    BinTreeNode<Val> *_children[2];
  };

} /* namespace gum */

/// always include the implementation of the templates
#include <agrum/core/binTreeNode.tcc>

#endif /* GUM_BIN_TREE_NODE_H */
