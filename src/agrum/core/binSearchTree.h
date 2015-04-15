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
 * @brief basic binary search trees
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_BIN_SEARCH_TREE_H
#define GUM_BIN_SEARCH_TREE_H

#include <agrum/config.h>

#include <agrum/core/binTreeNode.h>

namespace gum {

  // classes provided/used by this header

  template <typename Val, class Cmp, class Node> class BinSearchTree;

  template <typename Val, class Cmp, class Node> class BinSearchTreeIterator;

  template <typename Val, class Cmp> class AVLSearchTree;

  /* =========================================================================== */
  /* ===                      GENERIC BINARY SEARCH TREE                     === */
  /* =========================================================================== */
  /** @class BinSearchTree
   * @brief generic binary search trees */
  /* =========================================================================== */
  template <typename Val,

            class Cmp = std::less<Val>,

            class Node = BinTreeNode<Val>>
  class BinSearchTree {
    public:
    typedef BinSearchTreeIterator<Val, Cmp, Node> iterator;
    typedef BinSearchTreeIterator<Val, Cmp, Node> const_iterator;

    // ############################################################################
    /// @name constructors / destructors
    // ############################################################################
    ///@{

    /// basic constructor: returns an empty binary search tree
    /** @param uniqueness_policy allows (false) or disables (true) the possibility
     * for the binary tree to have multiple instances of the same value within
     * the tree. */

    explicit BinSearchTree(bool uniqueness_policy = false);

    /// copy constructor

    BinSearchTree(const BinSearchTree<Val, Cmp, Node> &from);

    /// destructor

    virtual ~BinSearchTree();

    ///@}

    // ############################################################################
    /// @name operators
    // ############################################################################
    ///@{

    /// copy operator

    BinSearchTree<Val, Cmp, Node> &
    operator=(const BinSearchTree<Val, Cmp, Node> &from);

    ///@}

    // ############################################################################
    /// @name iterators
    // ############################################################################
    ///@{

    /// begin iterator

    iterator begin();
    const_iterator begin() const;

    /// rbegin iterator

    iterator rbegin();
    const_iterator rbegin() const;

    /// end iterator

    const iterator &end();
    const const_iterator &end() const;

    /// rend iterator

    const iterator &rend();
    const const_iterator &rend() const;

    /// returns an iterator pointing to the root of the tree

    iterator root();
    const_iterator root() const;

    ///@}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// returns the value of the root of the tree
    /** @throw NotFound exception is raised if the tree is empty */

    const Val &rootValue() const;

    /// returns the value of the leftmost node with the minimal key in the tree
    /** @throw NotFound exception is raised if the tree is empty */

    const Val &minValue() const;

    /// returns the value of the rightmost node with the maximal key in the tree
    /** @throw NotFound exception is raised if the tree is empty */

    const Val &maxValue() const;

    /** @brief creates a copy of the value, insert it in the tree and returns
     * the copy value
     *
     * When elements are inserted into binary search trees, this is actually copies
     * that are inserted. Thus, the method returns the newly created copy, so that
     * the user may reference it.
     * @throw DuplicateElement exception is raised if the binary tree already
     * contains the value and the uniqueness property is set to true */

    const Val &insert(const Val &val);

    /// erase the leftmost node with the given (key,val) pair
    /**
     * @throws NotFound if we could not find the node
     */

    void erase(const Val &val);

    /// erase the node pointed to by the iterator
    /** if we could not find the node, then nothing happens. In particular, no
     * exception is raised. */

    void erase(const iterator &iter);

    /// indicates whether the BinSearchTree contains the value

    bool contains(const Val &) const;

    /// removes all the elements from the BinSearchTree

    void clear();

    /// returns the number of elements in the binary search tree

    Size size() const;

    /// indicates whether the binary search tree is empty

    bool empty() const;

    /// displays the content of the tree (in increasing order w.r.t. Cmp)

    virtual const std::string toString() const;

    /// @}

    // ############################################################################
    /// @name Fine tuning
    // ############################################################################
    /// @{

    /// returns the current uniqueness policy

    bool uniquenessPolicy() const;

    /** @brief enables the user to change dynamically the policy for checking
     * whether there can exist several identical elements in the binary tree
     *
     * By default, trees can store several times the same element. However, for
     * some applications, we should ensure that all elements in the binary search
     * tree are distinct.
     * @warning When setting the policy to "uniqueness", the function does not
     * check whether the tree already contains identical elements. It thus only
     * ensures that elements inserted from now on do not already belong to the
     * tree. */

    void setUniquenessPolicy(const bool new_policy);

    /// @}

    protected:
    /// the root node of the tree
    Node *_root;

    /// the comparison function
    Cmp _cmp;

    /// the list of iterators pointing to the binary search tree
    mutable iterator *_iterator_list;

    /// the uniqueness property: whether the same value can appear multiple times
    mutable bool _uniqueness_policy;

    /// the number of elements stored in the tree
    Size _nb_elements;

    /** @name pseudo static iterator
     * the end and rend iterators are constructed only once per binary search tree
     * so as to optimize for(iter = begin();iter != end(); iter++) loops: this
     * will avoid creating objects end and rend each time we pass in the loop. */
    //\{
    iterator _iter_end;
    //\}

    // to speed-up accesses

    friend class BinSearchTreeIterator<Val, Cmp, Node>;

    friend class AVLSearchTree<Val, Cmp>;

    /// a method for recursively copying the contents of a BinSearchTree
    /** @warning this function produces a tree with precisely the same structure
     * as that passed in argument (node). Hence, this should be used only when
     * copying binary search trees storing data w.r.t. the same weak order Cmp.
     * @param root_from the root of the tree to be copied
     * @param parent is the node that should be the parent of the copy
     * @param dir the direction of the edge parent->copy */

    Node *_copy(Node *root_from, Node *parent = 0,
                BinTreeDir dir = BinTreeDir::LEFT_CHILD);

    /// returns the smallest node w.r.t. order Cmp in the subtree rooted at node

    Node *_minNode(Node *node) const;

    /// returns the greatest node w.r.t. order Cmp in the subtree rooted at node

    Node *_maxNode(Node *node) const;

    /// returns the next node according to weak ordering Cmp

    Node *_succNode(Node *node) const;

    /// returns the previous node according to weak ordering Cmp

    Node *_prevNode(Node *node) const;

    /// returns the node containing a given value (0 if the value cannot be found)

    Node *_getNode(const Val &) const;

    /// a method for recursively destroying a subtree of the BinSearchTree
    /** note that this method does not update the iterators pointing to nodes
     * of the subtree. These should be cleared before _deleteSubTree is called. */

    void _deleteSubTree(Node *node);

    /// erase the node passed in argument

    virtual void _erase(Node *node);

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

    virtual Node *_insert(const Val &val);

    private:
    /// update all iterators when a given node is deleted

    void __updateEraseIterators(Node *node);
  };

  /* =========================================================================== */
  /* ===                 GENERIC BINARY SEARCH TREE ITERATORS                === */
  /* =========================================================================== */
  /** @class BinSearchTreeIterator
   * @brief generic binary search trees */
  /* =========================================================================== */

  template <typename Val, class Cmp, class Node> class BinSearchTreeIterator {
    public:
    // ############################################################################
    /// constructors / destructors
    // ############################################################################
    ///@{

    /// basic constructor: returns an iterator pointing toward nothing

    BinSearchTreeIterator();

    /// copy constructor: creates an iterator pointing toward the same tree

    BinSearchTreeIterator(const BinSearchTreeIterator<Val, Cmp, Node> &from);

    /// destructor

    ~BinSearchTreeIterator();

    ///@}

    // ############################################################################
    /// operators
    // ############################################################################
    ///@{

    /// copy operator

    BinSearchTreeIterator<Val, Cmp, Node> &
    operator=(const BinSearchTreeIterator<Val, Cmp, Node> &from);

    /// returns the value pointed to by the iterator
    /** @throw UndefinedIteratorValue exception is raised if the iterator does
     * not point to a valid node of the tree */

    const Val &operator*() const;

    /// point the iterator to the next value in the binary search tree
    /** A binary search tree stores data according to a complete weak order <= .
     * A ++ operation on an iterator makes the latter point on the next value
     * in the tree w.r.t. ordering <=.
     *
     * for (iter=begin(); iter!=end(); ++iter) loops are guaranteed to
     * parse the whole binary search tree as long as no element is added to or
     * deleted from the tree while being in the loop. Deleting elements during the
     * loop is guaranteed to never produce a segmentation fault. */

    BinSearchTreeIterator<Val, Cmp, Node> &operator++();

    /// point the iterator to the preceding value in the binary search tree
    /** A binary search tree stores data according to a complete weak order <= .
     * A -- operation on an iterator makes the latter point on the preceding value
     * in the tree w.r.t. ordering <=.
     *
     * for (iter=rbegin(); iter!=rend(); --iter) loops are guaranteed to
     * parse the whole binary search tree as long as no element is added to or
     * deleted from the tree while being in the loop. Deleting elements during the
     * loop is guaranteed to never produce a segmentation fault. */

    BinSearchTreeIterator<Val, Cmp, Node> &operator--();

    /// checks whether two iterators are pointing toward different elements

    bool operator!=(const BinSearchTreeIterator<Val, Cmp, Node> &from) const;

    /// checks whether two iterators are pointing toward the same element

    bool operator==(const BinSearchTreeIterator<Val, Cmp, Node> &from) const;

    ///@}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// makes the iterator move to its parent node

    BinSearchTreeIterator<Val, Cmp, Node> &up();

    /// makes the iterator move to the left child of the node it points to

    BinSearchTreeIterator<Val, Cmp, Node> &downLeft();

    /// makes the iterator move to the right child of the node it points to

    BinSearchTreeIterator<Val, Cmp, Node> &downRight();

    /// detach the iterator from its current tree (if any) and reset it

    void clear();

    ///@}

    protected:
    /// the current node pointed to by the iterator
    Node *_node;

    /// the next node to be used when _node=0 (if a ++ operator is applied)
    Node *_next_node;

    /// the preceding node to be used when _node=0 (if a -- operator is applied)
    Node *_prev_node;

    /// the parent to be used when _node=0 (if operation up is applied)
    Node *_parent;

    /// the left child to be used when _node=0 and leftdown() is applied
    Node *_left_child;

    /// the right child to be used when _node=0 and rightdown() is applied
    Node *_right_child;

    /// the binary search tree pointed to by the iterator
    BinSearchTree<Val, Cmp, Node> *_tree;

    /// the next iterator in the list of iterators of the binSearchTree
    BinSearchTreeIterator<Val, Cmp, Node> *_next_iter;

    private:
    // to speed-up accesses

    friend class BinSearchTree<Val, Cmp, Node>;

    friend class AVLSearchTree<Val, Cmp>;

    /// a function called to initialize an iterator
    /** Assuming that the iterator has been constructed using the default
     * constructor (i.e., it points toward no binary search tree), this method
     * make it point toward one tree and, if needed, add it to the set of
     * iterators of the tree */

    void _initialize(const BinSearchTree<Val, Cmp, Node> *tree,
                     const Node *current_node, bool add_to_iterator_list);

    /// a method to detach the current iterator from its tree's iterator's list

    void _detachFromTree();
  };

} /* namespace gum */

// always include the template implementations
#include <agrum/core/binSearchTree.tcc>

#endif /* GUM_BIN_SEARCH_TREE_H */
