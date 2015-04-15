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
 * @brief AVL binary search trees
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_AVL_SEARCH_TREE_H
#define GUM_AVL_SEARCH_TREE_H

#include <agrum/config.h>

#include <agrum/core/binTreeNode.h>
#include <agrum/core/binSearchTree.h>

namespace gum {

  template <typename Val, class Cmp> class AVLSearchTree;

  /* =========================================================================== */
  /* ===                  NODES FOR AVL BINARY SEARCH TREES                  === */
  /* =========================================================================== */
  /** @class BinTreeNode4AVL
   * @brief Binary tree nodes with additional fields for use in AVL binary
   * search trees
   * @ingroup basicstruct_group
   * BinTreeNode4AVL is the node class used in AVL binary search trees. It is
   * equipped with the material to add nodes to or remove them from the tree. The
   * class ensures that trees are always a coherent structure, that is, it is not
   * possible to have a node X having, say, a left child, and this child having no
   * parent or a parent different from X. The class also provides some functions
   * to balance the tree (balance, left and right rotations). However, the class
   * @b DOES @b NOT balances by itself the tree after insertions/deletions of
   * nodes. Balancing trees is indeed quite simple and efficient when we can
   * guarantee that new nodes inserted have no children. This property is enforced
   * within AVL search trees but cannot be enforced within the class
   * BinTreeNode4AVL. Hence, balancing is delegated to the AVLSearchTree class.
   * For the same reason, the __height field of the nodes MUST be computed by the
   * AVLSearchTree class.
   * @par Usage example:
   * @code
   * // create a node containing an integer
   * BinTreeNode4AVL<int> node1 (33);
   *
   * // get the integer contained in the node
   * std::cerr << node1.value () << std::endl;
   * std::cerr << *node1 << std::endl;
   *
   * // create a disconnected node containing the same value as node1
   * BinTreeNode4AVL<int> node2 = node1;
   * BinTreeNode4AVL<int> node3 (3); node3 = node1;
   *
   * // insert left and right children
   * node1.insertLeftChild ( node2 );
   * node1.insertRightChild ( node 3 );
   *
   * BinTreeNode4AVL<int>* node4 = node2.insertLeftChild ( 3 );
   * BinTreeNode4AVL<int>* node5 = node2.insertRightChild (4 );
   *
   * BinTreeNode4AVL<int>* node6 = node4.insertChild (5,GUM_BIN_TREE_LEFT_CHILD);
   * BinTreeNode4AVL<int>* node7 = node4.insertChild (6,GUM_BIN_TREE_RIGHT_CHILD);
   *
   * BinTreeNode4AVL<int> node8 (55), node9(44);
   * node3.insertChild ( node8, GUM_BIN_TREE_LEFT_CHILD );
   * node3.insertChild ( node9, GUM_BIN_TREE_RIGHT_CHILD );
   *
   * // get the parents and children
   * BinTreeNode4AVL<int>* node8 = node.parent();
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

  template <typename Val> class BinTreeNode4AVL : private BinTreeNode<Val> {
    public:
    // ############################################################################
    // constructors / destructors
    // ############################################################################
    /// @{

    /// basic constructor: a node without parent nor children

    BinTreeNode4AVL(const Val &v);

    /** @brief copy constructor: creates a new disconnected node with the same
     * value and height as "from"
     *
     * @warning Although the new node contains the same value as "from", it has
     * no parent, nor any children, even when "from" has some. */

    BinTreeNode4AVL(const BinTreeNode4AVL<Val> &from);

    /// destructor
    /** in addition to removing the node, this method updates appropriately
     * its parent and children */

    ~BinTreeNode4AVL();

    /// @}

    // ############################################################################
    // operators
    // ############################################################################
    /// @{

    /** @brief copy operator: copy the value of from into this. However, this
     * does not change the current connections (parents and children) of this. */

    BinTreeNode4AVL<Val> &operator=(const BinTreeNode4AVL<Val> &from);

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

    BinTreeNode4AVL<Val> *child(BinTreeDir dir) const;

    /// returns the given child of a node
    /** @warning if the child does not exists, the method returns a 0 pointer. */

    BinTreeNode4AVL<Val> *leftChild() const;

    /// returns the given child of a node
    /** @warning if the child does not exists, the method returns a 0 pointer. */

    BinTreeNode4AVL<Val> *rightChild() const;

    /// returns the parent of a node
    /** @warning if the parent does not exists, the method returns a 0 pointer. */

    BinTreeNode4AVL<Val> *parent() const;

    /// returns the direction of the edge parent->current node, if any

    BinTreeDir parentDir() const;

    /// adds a new left child to the current node
    /** @warning the new child is created on the C++ heap (i.e., using a
     * dynamic memory allocation)
     * @returns a pointer on the new created child
     * @throw DuplicateElement if the current node had already a left child */

    BinTreeNode4AVL<Val> *insertLeftChild(const Val &);

    /// adds a new left child to the current node
    /** @throw DuplicateElement if the current node had already a left child or
     * if new_child already has a parent */

    void insertLeftChild(BinTreeNode4AVL<Val> &new_child);

    /// adds a new right child to the current node
    /**  @returns a pointer on the new created child
     * @throw DuplicateElement if the current node had already a left child */

    BinTreeNode4AVL<Val> *insertRightChild(const Val &);

    /// adds a new right child to the current node
    /** @throw DuplicateElement if the current node had already a left child or
     * if new_child already has a parent */

    void insertRightChild(BinTreeNode4AVL<Val> &new_child);

    /// adds a new child to the current node
    /**  @returns a pointer on the new created child
     * @throw DuplicateElement if the current node had already a child in the
     * child_dir subtree */

    BinTreeNode4AVL<Val> *insertChild(const Val &, BinTreeDir child_dir);

    /// adds a new child to the current node
    /** @throw DuplicateElement if the current node had already a child in the
     * child_dir direction or if new_child already has a parent */

    void insertChild(BinTreeNode4AVL<Val> &new_child, BinTreeDir child_dir);

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

    BinTreeNode4AVL<Val> *leftmostNode() const;

    /// returns the rightmost node of the current tree
    /** if there is no right child, the method returns this */

    BinTreeNode4AVL<Val> *rightmostNode() const;

    /// returns the top ancestor of the current tree
    /** if the current node has no parent, the method returns this */

    BinTreeNode4AVL<Val> *root() const;

    /// @}

    private:
    /// the height of the node
    unsigned int __height;

    /// let the AVL binary search tree access the _height field

    template <typename V, typename Cmp> friend class AVLSearchTree;

    /// right rotation
    /** the rotation is performed assuming that the current node is the root
     * of the tree to be rotated */

    BinTreeNode4AVL<Val> *__rightRotation();

    /// left rotation
    /** the rotation is performed assuming that the current node is the root
     * of the tree to be rotated */

    BinTreeNode4AVL<Val> *__leftRotation();

    /// balance the whole tree, starting from the current node
    /** the balance is performed the usual way in AVL trees starting from the
     * current node and iterating the balance up to the root of the tree.
     * @warning balancing must be done after inserting new nodes or removing
     * old nodes. We assume in this function that:
     *
     * # before insertions/removals, the tree is AVL
     * # when inserting new nodes, those have no children
     *
     * As such, these restrictions hold in binary search trees.
     * @return either the new root of the whole tree if this one may have changed
     * or 0 if we are certain that the new root equals the old root. */

    BinTreeNode4AVL<Val> *__balance();
  };

  /* =========================================================================== */
  /* =========================================================================== */
  /* ===                        AVL BINARY SEARCH TREE                       === */
  /* =========================================================================== */
  /* =========================================================================== */
  /** @class AVLSearchTree
   * @brief generic binary search trees */
  /* =========================================================================== */

  template <typename Val, class Cmp = std::less<Val>>

  class AVLSearchTree : public BinSearchTree<Val, Cmp, BinTreeNode4AVL<Val>> {
    public:
    typedef BinTreeNode4AVL<Val> Node;
    typedef BinSearchTreeIterator<Val, Cmp, Node> iterator;

    // ############################################################################
    /// @name constructors / destructors
    // ############################################################################
    ///@{

    /// basic constructor: returns an empty binary search tree
    /** @param uniqueness_policy allows (false) or disables (true) the possibility
     * for the binary tree to have multiple instances of the same value within
     * the tree. */

    explicit AVLSearchTree(bool uniqueness_policy = false);

    /// copy constructor

    AVLSearchTree(const AVLSearchTree<Val, Cmp> &from);

    /// destructor

    ~AVLSearchTree();

    ///@}

    // ############################################################################
    /// @name operators
    // ############################################################################
    ///@{

    /// copy operator

    AVLSearchTree<Val, Cmp> &operator=(const AVLSearchTree<Val, Cmp> &from);

    ///@}

    // ############################################################################
    /// @name iterators
    // ############################################################################
    ///@{
    using BinSearchTree<Val, Cmp, Node>::begin;
    using BinSearchTree<Val, Cmp, Node>::rbegin;
    using BinSearchTree<Val, Cmp, Node>::end;
    using BinSearchTree<Val, Cmp, Node>::rend;
    ///@}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{
    using BinSearchTree<Val, Cmp, Node>::rootValue;
    using BinSearchTree<Val, Cmp, Node>::minValue;
    using BinSearchTree<Val, Cmp, Node>::maxValue;
    using BinSearchTree<Val, Cmp, Node>::contains;
    using BinSearchTree<Val, Cmp, Node>::empty;
    using BinSearchTree<Val, Cmp, Node>::size;
    using BinSearchTree<Val, Cmp, Node>::clear;
    using BinSearchTree<Val, Cmp, Node>::toString;
    using BinSearchTree<Val, Cmp, Node>::insert;
    using BinSearchTree<Val, Cmp, Node>::erase;

    /// check the tree to determine whether it is actually AVL balanced

    bool checkAVLStructure() const;

    /// @}

    // ############################################################################
    /// @name Fine tuning
    // ############################################################################
    /// @{
    using BinSearchTree<Val, Cmp, Node>::uniquenessPolicy;
    using BinSearchTree<Val, Cmp, Node>::setUniquenessPolicy;
    /// @}

    /// friendly displays the content of the CliqueGraph

    const std::string toString() const;

    protected:
    /// erase the node passed in argument

    void _erase(Node *node);

    /** @brief creates a copy of the value, insert it in the tree and returns
     * the copy value
     *
     * When elements are inserted into binary search trees, this is actually copies
     * that are inserted. Thus, the method returns the node containing the newly
     * created copy, so that the user may reference the new copy.
     * @warning this method is actually the implementation of method insert. It
     * is used to speed-up insertions in terminal classes such as AVL trees.
     * @throw DuplicateElement exception is raised if the binary tree already
     * contains the value and the uniqueness property is set to true */

    Node *_insert(const Val &val);
  };

  /// for friendly displaying the content of AVLSearchTree

  template <typename Val, class Cmp>
  std::ostream &operator<<(std::ostream &, const AVLSearchTree<Val, Cmp> &);

} /* namespace gum */

// always include the template implementations
#include <agrum/core/AVLSearchTree.tcc>

#endif /* GUM_AVL_SEARCH_TREE_H */
