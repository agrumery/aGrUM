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
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_BIN_TREE_NODE_H
#define GUM_BIN_TREE_NODE_H

#include <agrum/agrum.h>

namespace gum {

  /// The direction of a given edge in a binary tree.
  enum class BinTreeDir : char {
    LEFT_CHILD  = 0,
    RIGHT_CHILD = 1,
    NO_PARENT   = 2
  };

  // ===========================================================================
  // ===                   GENERIC NODE FOR A BINARY TREE                    ===
  // ===========================================================================

  /**
   * @class BinTreeNode
   * @headerfile binTreeNode.h <agrum/tools/core/binTreeNode.h>
   * @brief Nodes of a binary trees.
   * @ingroup basicstruct_group
   *
   * BinTreeNode is the base class for nodes of binary trees. It is equipped
   * with the material to add nodes to or remove them from the tree. The class
   * ensures that trees are always a coherent structure, that is, it is not
   * possible to have a node X having, say, a left child, and this child having
   * no parent or a parent different from X.
   *
   * @tparam Val The value's type storde in the gum::BinTreeNode.
   *
   * @par Usage example:
   *
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
   * node1.insertLeftChild(node2);
   * node1.insertRightChild(node3);
   *
   * BinTreeNode<int>* node4 = node2.insertLeftChild(3);
   * BinTreeNode<int>* node5 = node2.insertRightChild(4);
   *
   * BinTreeNode<int>* node6 = node4.insertChild(5, GUM_BIN_TREE_LEFT_CHILD);
   * BinTreeNode<int>* node7 = node4.insertChild(6, GUM_BIN_TREE_RIGHT_CHILD);
   *
   * BinTreeNode<int> node8 (55), node9(44);
   * node3.insertChild(node8, GUM_BIN_TREE_LEFT_CHILD);
   * node3.insertChild(node9, GUM_BIN_TREE_RIGHT_CHILD);
   *
   * // get the parents and children
   * BinTreeNode<int>* node8 = node.parent();
   * node8 = node1.leftChild();
   * node8 = node1.rightChild();
   * node8 = node1.child(gum::GUM_BIN_TREE_LEFT_CHILD);
   * node8 = node1.child(gum::GUM_BIN_TREE_RIGHT_CHILD);
   *
   * // remove links between parents and children
   * node1.eraseLeftLink();  // erase the connection between node1 and node2
   * node1.eraseRightLink(); // erase the connection between node1 and node3
   * node2.eraseLink(gum::GUM_BIN_TREE_LEFT_CHILD);  // erase (node2,node4)
   * node2.eraseLink(gum::GUM_BIN_TREE_RIGHT_CHILD); // erase (node2;node5)
   * @endcode
   */
  template < typename Val >
  class BinTreeNode {
    public:
    // ============================================================================
    /// @name Class constructors and destructors
    // ============================================================================
    /// @{

    /**
     * @brief Basic constructor: a node without parent nor children.
     * @param v The value stored in the node.
     */
    BinTreeNode(const Val& v);

    /**
     * @brief copy constructor: creates a new disconnected node with the same
     * value as "from".
     *
     * @warning Although the new node contains the same value as from, it has
     * no parent, nor any children, even if from has some.
     *
     * @param from The node to copy.
     */
    BinTreeNode(const BinTreeNode< Val >& from);

    /**
     * @brief Class destructor.
     *
     * In addition to removing the node, this method updates appropriately
     * its parent and children
     */
    ~BinTreeNode();

    /// @}
    // ============================================================================
    /// @name Class operators
    // ============================================================================
    /// @{

    /**
     * @brief Copy operator: copy the value of from into this.
     *
     * However, this does not change the current connections (parents and
     * children) of this.
     *
     * @param from The node to copy.
     * @return Returns this gum::BinTreeNode.
     */
    BinTreeNode< Val >& operator=(const BinTreeNode< Val >& from);

    /**
     * @brief Alias for method value.
     * @return Return the value stored in this node.
     */
    Val& operator*();

    /// @}
    // ============================================================================
    /// @name Class accessors and modifiers
    // ============================================================================
    /// @{

    /**
     * @brief Returns the value stored in a node of the binary search tree.
     * @return The value stored in this node.
     */
    Val& value();

    /**
     * @brief Returns the given child of a node.
     * @warning If the child does not exists, the method returns a 0 pointer.
     * @param dir The direction where we loog for a child.
     * @return Returns the child of a node.
     */
    BinTreeNode< Val >* child(BinTreeDir dir) const;

    /**
     * @brief Returns the given child of a node.
     * @warning If the child does not exists, the method returns a 0 pointer.
     * @return Retuns the left child of this node.
     */
    BinTreeNode< Val >* leftChild() const;

    /**
     * @brief Returns the given child of a node.
     * @warning If the child does not exists, the method returns a 0 pointer.
     * @return Retuns the right child of this node.
     */
    BinTreeNode< Val >* rightChild() const;

    /**
     * @brief Returns the parent of a node.
     * @warning If the parent does not exists, the method returns a 0 pointer.
     * @return Returns the parent of this node.
     */
    BinTreeNode< Val >* parent() const;

    /**
     * @brief Returns the direction of the edge parent->current node, if any.
     * @return Returns the direction of the edge parent->current node, if any.
     */
    BinTreeDir parentDir() const;

    /**
     * @brief Adds a new left child to the current node.
     * @warning the new child is created on the C++ heap (i.e., using a
     * dynamic memory allocation)
     * @param val The value to store in the new child.
     * @return a pointer on the new created child
     * @throw DuplicateElement if the current node had already a left child
     */
    BinTreeNode< Val >* insertLeftChild(const Val& val);

    /**
     * @brief Adds a new left child to the current node.
     * @param new_child The child to add.
     * @throw DuplicateElement Raised if the current node had already a left
     * child or if new_child already has a parent.
     */
    void insertLeftChild(BinTreeNode< Val >& new_child);

    /**
     * @brief Adds a new left child to the current node.
     * @warning the new child is created on the C++ heap (i.e., using a
     * dynamic memory allocation)
     * @param val The value to store in the new child.
     * @return a pointer on the new created child
     * @throw DuplicateElement if the current node had already a left child
     */
    BinTreeNode< Val >* insertRightChild(const Val& val);

    /**
     * @brief Adds a new right child to the current node.
     * @param new_child The child to add.
     * @throw DuplicateElement Raised if the current node had already a right
     * child or if new_child already has a parent.
     */
    void insertRightChild(BinTreeNode< Val >& new_child);

    /**
     * @brief Adds a new child to the current node.
     * @param val The value to add.
     * @param child_dir The direction where to add the child.
     * @return Returns a pointer on the new created child
     * @throw DuplicateElement Raised if the current node had already a child
     * in the child_dir subtree.
     */
    BinTreeNode< Val >* insertChild(const Val& val, BinTreeDir child_dir);

    /**
     * @brief Adds a new child to the current node.
     * @param new_child The child to add.
     * @param child_dir The direction where to add the child.
     * @throw DuplicateElement Raised if the current node had already a child
     * in the child_dir direction or if new_child already has a parent .
     */
    void insertChild(BinTreeNode< Val >& new_child, BinTreeDir child_dir);

    /**
     * @brief Remove the link between the current node and its left child.
     *
     * Note that only the link is removed, i.e., the left child is not removed
     * itself nor, a fortiori, the left subtree of the current node. If there
     * is no left child, the method does nothing. In particular, it does not
     * raise any exception.
     */
    void eraseLeftLink();

    /**
     * @brief Remove the link between the current node and its right child.
     *
     * Note that only the link is removed, i.e., the right child is not removed
     * itself nor, a fortiori, the right subtree of the current node. If there
     * is no right child, the method does nothing. In particular, it does not
     * raise any exception.
     */
    void eraseRightLink();

    /**
     * @brief Remove the link between the current node and one of its children.
     *
     * Note that only the link is removed, i.e., the child is not removed
     * itself nor, a fortiori, its subtree. If the child does not exist, the
     * method does nothing. In particular, it does not raise any exception.
     *
     * @param tree_dir The direction where to remove the link.
     */
    void eraseLink(BinTreeDir tree_dir);

    /**
     * @brief Returns the leftmost node of the current tree.
     *
     * If there is no left child, the method returns this.
     *
     * @return The left most node in the current tree.
     */
    BinTreeNode< Val >* leftmostNode() const;

    /**
     * @brief Returns the rightmost node of the current tree.
     *
     * If there is no right child, the method returns this.
     *
     * @return The right most node in the current tree.
     */
    BinTreeNode< Val >* rightmostNode() const;

    /**
     * @brief Returns the top ancestor of the current tree.
     *
     * If the current node has no parent, the the method returns this.
     *
     * @return Returns the top ancestor of the current tree.
     */
    BinTreeNode< Val >* root() const;

    /// @}

    protected:
    /// The value stored in a node of the tree.
    Val val_;

    /// The parent of the node.
    BinTreeNode< Val >* parent_;

    /// the direction to follow from the parent to reach the current node.
    BinTreeDir parent_dir_;

    /// The children of the current node.
    BinTreeNode< Val >* children_[2];
  };

} /* namespace gum */


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
extern template class gum::BinTreeNode< int >;
#endif


// always include the implementation of the templates
#include <agrum/tools/core/binTreeNode_tpl.h>

#endif   // GUM_BIN_TREE_NODE_H
