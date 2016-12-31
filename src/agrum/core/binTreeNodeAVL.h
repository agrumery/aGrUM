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
/**
 * @file
 * @brief AVL binary search trees.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_BIN_TREE_NODE_4_AVL_H
#define GUM_BIN_TREE_NODE_4_AVL_H

#include <agrum/config.h>

#include <agrum/core/binTreeNode.h>

namespace gum {

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  template <typename Val, class Cmp>
  class AVLSearchTree;
#endif  // DOXYGEN_SHOULD_SKIP_THIS

  // ===========================================================================
  // ===                  NODES FOR AVL BINARY SEARCH TREES                  ===
  // ===========================================================================
  /**
   * @class BinTreeNodeAVL BinTreeNodeAVL.h <agrum/core/BinTreeNodeAVL.h>
   * @brief Binary tree nodes with additional fields for use in AVL binary
   *        search trees.
   * @ingroup basicstruct_group
   *
   * BinTreeNodeAVL is the node class used in AVL binary search trees. It is
   * equipped with the material to add nodes to or remove them from the tree.
   * The class ensures that trees are always in a coherent structure, that is
   * it is not possible to have a node X with a left child not having a parent
   * or having a parent different from X. The class also provides some
   * functions to balance the tree (balance, left and right rotations).
   * However, the class @b DOES @b NOT balances the tree by itself after
   * insertions or deletions of nodes.
   *
   * Balancing trees is indeed quite simple and efficient when we can guarantee
   * that new nodes inserted have no children. This property is enforced within
   * AVL search trees but cannot be enforced within the class BinTreeNodeAVL.
   * Hence, balancing is delegated to the AVLSearchTree class. For the same
   * reason, the __height field of the nodes MUST be computed by the
   * AVLSearchTree class.
   *
   * @tparam Val The type of values stored in BinTreeNodeAVL.
   *
   * @par Usage example:
   * @code
   * // create a node containing an integer
   * BinTreeNodeAVL<int> node1 (33);
   *
   * // get the integer contained in the node
   * std::cerr << node1.value () << std::endl;
   * std::cerr << *node1 << std::endl;
   *
   * // create a disconnected node containing the same value as node1
   * BinTreeNodeAVL<int> node2 = node1;
   * BinTreeNodeAVL<int> node3 (3); node3 = node1;
   *
   * // insert left and right children
   * node1.insertLeftChild ( node2 );
   * node1.insertRightChild ( node 3 );
   *
   * auto node4 = node2.insertLeftChild ( 3 );
   * auto node5 = node2.insertRightChild (4 );
   *
   * auto node6 = node4.insertChild(5,GUM_BIN_TREE_LEFT_CHILD);
   * auto node7 = node4.insertChild(6,GUM_BIN_TREE_RIGHT_CHILD);
   *
   * BinTreeNodeAVL<int> node8 (55), node9(44);
   * node3.insertChild ( node8, GUM_BIN_TREE_LEFT_CHILD );
   * node3.insertChild ( node9, GUM_BIN_TREE_RIGHT_CHILD );
   *
   * // get the parents and children
   * BinTreeNodeAVL<int>* node8 = node.parent();
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
  template <typename Val>
  class BinTreeNodeAVL : private BinTreeNode<Val> {
    public:
    /// @name Constructors and destructor
    /// @{

    /**
     * @brief Basic constructor: a node without parent nor children
     * @param v The node's value.
     */
    BinTreeNodeAVL( const Val& v );

    /**
     * @brief Copy constructor: creates a new disconnected node with the same
     *        value and height as "from".
     * @param from The node from which the value is copied.
     *
     * @warning Although the new node contains the same value as "from", it has
     *          no parent, nor any children, even when "from" has some.
     */
    BinTreeNodeAVL( const BinTreeNodeAVL<Val>& from );

    /**
     * @brief Destructor.
     *
     * In addition to removing the node, this method updates appropriately
     * its parent and children
     */
    ~BinTreeNodeAVL();

    /// @}

    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /**
     * @brief Copy operator: copy the value of from into this.
     *
     * However, this does not change the current connections (parents and
     * children) of this.
     *
     * @param from The node from which the value is copied.
     * @return Returns this node after from's value has been copied.
     */
    BinTreeNodeAVL<Val>& operator=( const BinTreeNodeAVL<Val>& from );

    /**
     * @brief Alias for the gum::BinTreeNodeAVL::value() method.
     */
    Val& operator*();

    /// @}
    // ============================================================================
    /// @name Accessors and Modifiers
    // ============================================================================
    /// @{

    /**
     * @brief Returns the value stored in a node of the binary search tree
     * @return Returns the value stored in this node.
     */
    Val& value();

    /**
     * @brief Returns the node's child given a direction.
     * @param dir The direction from which the child is returned.
     * @return Returns nullptr if there is no child and the child otherwhise.
     * @warning If the child does not exists, the method returns nullptr.
     */
    BinTreeNodeAVL<Val>* child( BinTreeDir dir ) const;

    /**
     * @brief Returns the left child of a node.
     * @return Returns nullptr if there is no child and the child otherwhise.
     * @warning If the child does not exists, the method returns nullptr.
     */
    BinTreeNodeAVL<Val>* leftChild() const;

    /*
     * @brief Returns the given child of a node.
     * @return Returns nullptr if there is no child and the child otherwhise.
     * @warning If the child does not exists, the method returns nullptr.
     */
    BinTreeNodeAVL<Val>* rightChild() const;

    /**
     * @brief Returns the node's parent.
     * @return Returns nullptr if there is no parent and the parent otherwhise.
     * @warning If the parent does not exists, the method returns nullptr.
     */
    BinTreeNodeAVL<Val>* parent() const;

    /**
     * @brief Returns the direction of the edge parent->current node, if any.
     * @return Returns the direction of the edge parent->current node, if any.
     */
    BinTreeDir parentDir() const;

    /**
     * @brief Adds a new left child to the current node.
     * @param v The value added to the new left child.
     * @return Returns a pointer on the new created child.
     * @throw DuplicateElement Raised if the current node has a left child
     * @warning The new child is created on the C++ heap (i.e., using a dynamic
     * memory allocation)
     */
    BinTreeNodeAVL<Val>* insertLeftChild( const Val& v );

    /**
     * @brief Adds a new left child to the current node.
     * @param new_child The child to add to this node.
     * @throw DuplicateElement Raised either if the current node has a left
     * child or if new_child has a parent.
     */
    void insertLeftChild( BinTreeNodeAVL<Val>& new_child );

    /**
     * @brief Adds a new right child to the current node.
     * @param v The value added to the new right child.
     * @return Returns a pointer on the new created child.
     * @throw DuplicateElement Raised if the current node has a right child
     * @warning The new child is created on the C++ heap (i.e., using a dynamic
     * memory allocation)
     */
    BinTreeNodeAVL<Val>* insertRightChild( const Val& v );

    /**
     * @brief Adds a new right child to the current node.
     * @param new_child The child to add to this node.
     * @throw DuplicateElement Raised either if the current node has a right
     * child or if new_child has a parent.
     */
    void insertRightChild( BinTreeNodeAVL<Val>& new_child );

    /**
     * @brief Adds a new child to the current node.
     * @param v The value of the new child.
     * @param child_dir The child direction.
     * @return Returns a pointer on the new child.
     * @throw DuplicateElement Raised if the current node already has a child
     * in the given direction.
     */
    BinTreeNodeAVL<Val>* insertChild( const Val& v, BinTreeDir child_dir );

    /**
     * @brief Adds a new child to the current node.
     * @param new_child The child added to the current node.
     * @param child_dir The child direction.
     * @throw DuplicateElement Raised if the current node had already a child
     * in the child_dir direction or if new_child already has a parent.
     */
    void insertChild( BinTreeNodeAVL<Val>& new_child, BinTreeDir child_dir );

    /**
     * @brief Removes the link between the current node and its left child.
     *
     * Note that only the link is removed, i.e., the left child is not removed
     * itself nor, a fortiori, the left subtree of the current node.
     *
     * If there is no left child, the method does nothing. In particular, it
     * does not raise any exception.
     */
    void eraseLeftLink();

    /**
     * @brief Removes the link between the current node and its right child.
     *
     * Note that only the link is removed, i.e., the right child is not removed
     * itself nor, a fortiori, the right subtree of the current node.
     *
     * If there is no right child, the method does nothing. In particular, it
     * does not raise any exception.
     */
    void eraseRightLink();

    /**
     * @brief Removes the link between the current node and one of its children.
     *
     * Note that only the link is removed, i.e., the child is not removed
     * itself nor, a fortiori, its subtree.
     *
     * If the child does not exist, the method does nothing. In particular, it
     * does not raise any exception.
     *
     * @param tree_dir The direction in which the edge is removed.
     */
    void eraseLink( BinTreeDir tree_dir );

    /**
     * @brief Returns the leftmost node of the current tree.
     * @returns Returns the lefmost node or this otherwhise.
     */
    BinTreeNodeAVL<Val>* leftmostNode() const;

    /**
     * @brief Returns the rightmost node of the current tree.
     * @returns Returns the rightmost node or this otherwhise.
     */
    BinTreeNodeAVL<Val>* rightmostNode() const;

    /**
     * @brief Returns the current tree top ancestor.
     * @returns Returns the top ancestor or this otherwhise.
     */
    BinTreeNodeAVL<Val>* root() const;

    /// @}

    private:
    /// The node's height.
    unsigned int __height;

    /// Let the AVL binary search tree access the _height field.
    template <typename V, typename Cmp>
    friend class AVLSearchTree;

    /**
     * @brief Right rotation.
     *
     * The rotation is performed assuming that the current node is the root
     * of the tree to be rotated.
     *
     * @returns Returns the rotated tree.
     */
    BinTreeNodeAVL<Val>* __rightRotation();

    /**
     * @brief Left rotation.
     *
     * The rotation is performed assuming that the current node is the root
     * of the tree to be rotated
     *
     * @returns Returns the rotated tree.
     */
    BinTreeNodeAVL<Val>* __leftRotation();

    /**
     * @brief Balances the whole tree, starting from the current node.
     *
     * The balance is performed the usual way in AVL trees starting from the
     * current node and iterating the balance up to the tree's root.
     *
     * @return either the new root of the whole tree if this one may have
     * changed or nullptr if we are certain that the new root equals the old
     * root.
     *
     * @warning balancing must be done after inserting new nodes or removing
     * old nodes. We assume in this function that:
     *  - before insertions/removals, the tree is AVL,
     *  - when inserting new nodes, those have no children.
     * As such, these restrictions hold in binary search trees.
     */
    BinTreeNodeAVL<Val>* __balance();
  };

}  // namespace gum


extern template class gum::BinTreeNodeAVL<int>;


// always include the template implementations
#include <agrum/core/binTreeNodeAVL_tpl.h>

#endif  // GUM_BIN_TREE_NODE_4_AVL_H
