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
 * @brief Basic binary search trees.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_BIN_SEARCH_TREE_H
#define GUM_BIN_SEARCH_TREE_H

#include <agrum/agrum.h>

#include <agrum/tools/core/binTreeNode.h>

namespace gum {

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  // classes provided/used by this header
  template < typename Val, class Cmp, class Node >
  class BinSearchTree;

  template < typename Val, class Cmp, class Node >
  class BinSearchTreeIterator;
#endif   // DOXYGEN_SHOULD_SKIP_THIS

  // ===========================================================================
  // ===                      GENERIC BINARY SEARCH TREE                     ===
  // ===========================================================================

  /**
   * @class BinSearchTree
   * @headerfile binSearchTree.h <agrum/tools/core/binSearchTree.h>
   * @brief A generic binary search tree.
   * @ingroup basicstruct_group
   *
   * @tparam Val The values type to store in the binary search tree.
   * @tparam Cmp The comparator for sorting the binary search tree.
   * @tparam Node The nodes type used to store values in the binary search
   * tree.
   *
   */
  template < typename Val, class Cmp = std::less< Val >, class Node = BinTreeNode< Val > >
  class BinSearchTree {
    public:
    /// @brief Alias for gum::BinSearchTree iterators
    /// @{
    using iterator       = BinSearchTreeIterator< Val, Cmp, Node >;
    using const_iterator = BinSearchTreeIterator< Val, Cmp, Node >;
    /// @}

    // ============================================================================
    /// @name Class constructors and destructors
    // ============================================================================
    /// @{

    /**
     * @brief Basic constructor: returns an empty binary search tree.
     * @param uniqueness_policy Allows (false) or disables (true) the binary
     * tree uniqueness.
     *
     * It is possible for the binary tree to have multiple instances of the
     * same value within the tree.
     */
    explicit BinSearchTree(bool uniqueness_policy = false);

    /**
     * @brief Copy constructor.
     * @param from The gum::BinSearchTree to copy.
     */
    BinSearchTree(const BinSearchTree< Val, Cmp, Node >& from);

    /**
     * @brief Class destructor.
     */
    virtual ~BinSearchTree();

    /// @}
    // ============================================================================
    /// @name Class operators
    // ============================================================================
    /// @{

    /**
     * @brief Copy operator.
     * @param from The gum::BinSearchTree to copy.
     * @return Returns this gum::BinSearchTree.
     */
    BinSearchTree< Val, Cmp, Node >& operator=(const BinSearchTree< Val, Cmp, Node >& from);

    /// @}
    // ============================================================================
    /// @name Class iterators
    // ============================================================================
    /// @{

    /**
     * @brief Begin iterator.
     * @return Returns an iterator at the beginning of the gum::BinSearchTree.
     */
    /// @{
    iterator       begin();
    const_iterator begin() const;
    /// @}

    /**
     * @brief Reverse iterator.
     * @return Returns an iterator at the beginning of the gum::BinSearchTree for
     * reverse iteration.
     */
    /// @{
    iterator       rbegin();
    const_iterator rbegin() const;
    /// @}

    /**
     * @brief End iterator.
     * @return Returns an iterator at the end of the gum::BinSearchTree.
     */
    /// @{
    const iterator&       end();
    const const_iterator& end() const;
    /// @}

    /**
     * @brief Reverse end iterator.
     * @return Returns an iterator at the end of the gum::BinSearchTree for
     * reverse iteration.
     */
    /// @{
    const iterator&       rend();
    const const_iterator& rend() const;
    /// @}

    /**
     * @brief Returns an iterator at the root of the tree.
     * @return Returns an iterator at the root of the tree.
     */
    /// @{
    iterator       root();
    const_iterator root() const;
    /// @}

    /// @}
    // ============================================================================
    /// @name Class Accessors and Modifiers
    // ============================================================================
    /// @{

    /**
     * @brief Returns the value of the root of the tree.
     * @return Returns the value of the root of the tree.
     * @throw NotFound Raised if the tree is empty.
     */
    const Val& rootValue() const;

    /**
     * @brief Returns the value of the leftmost node with the minimal key in
     * the tree.
     * @return Returns the value of the leftmost node with the minimal key in
     * the tree.
     * @throw NotFound Raised if the tree is empty.
     */
    const Val& minValue() const;

    /**
     * @brief Returns the value of the rightmost node with the maximal key in
     * the tree.
     * @return Returns the value of the rightmost node with the maximal key in
     * the tree.
     * @throw NotFound Raised if the tree is empty/
     */
    const Val& maxValue() const;

    /**
     * @brief Creates a copy of the value, insert it in the tree and returns
     * the copy value.
     *
     * When elements are inserted into binary search trees, this is actually
     * copies that are inserted. Thus, the method returns the newly created
     * copy, so that the user may reference it.
     *
     * @param val The value added by copy.
     * @return Returns a reference over copy added in the gum::BinSearchTree.
     * @throw DuplicateElement Raised if the binary tree already contains the
     * value and the uniqueness property is set to true.
     */
    const Val& insert(const Val& val);

    /**
     * @brief Erase the leftmost node with the given (key,val) pair.
     * @param val The value to remove.
     * @throws NotFound Raised if we could not find the node.
     */
    void erase(const Val& val);

    /**
     * @brief Erase the node pointed to by the iterator.
     *
     * If we could not find the node, then nothing happens. In particular, no
     * exception is raised.
     *
     * @param iter The iterator pointing toward the value to remove.
     */
    void erase(const iterator& iter);

    /**
     * @brief Returns true if the gum::BinSearchTree contains the value.
     * @param val The value tested for existence.
     * @return Returns true if the gum::BinSearchTree contains the value.
     */
    bool contains(const Val& val) const;

    /**
     * @brief Removes all the elements from the gum::BinSearchTree.
     */
    void clear();

    /**
     * @brief Returns the number of elements in the binary search tree.
     * @return Returns the number of elements in the binary search tree.
     */
    Size size() const;

    /**
     * @brief Indicates whether the gum::BinSearchTree search tree is empty.
     * @return Returns true if the gum::BinSearchTree is empty.
     */
    bool empty() const;

    /**
     * @brief Displays the content of the tree, in increasing order w.r.t.
     * Cmp.
     * @return Returns the content of the tree in a string.
     */
    virtual std::string toString() const;

    /// @}
    // ============================================================================
    /// @name Fine tuning
    // ============================================================================
    /// @{

    /**
     * @brief Returns the current uniqueness policy.
     * @return Returns the current uniqueness policy.
     */
    bool uniquenessPolicy() const;

    /**
     * @brief Enables the user to change dynamically the policy for checking
     * whether there can exist several identical elements in the binary tree.
     *
     * By default, trees can store several times the same element. However, for
     * some applications, we should ensure that all elements in the binary
     * search tree are distinct.
     *
     * @warning When setting the policy to "uniqueness", the function does not
     * check whether the tree already contains identical elements. It thus only
     * ensures that elements inserted from now on do not already belong to the
     * tree.
     *
     * @param new_policy Set the uniqueness policy on or off.
     */
    void setUniquenessPolicy(const bool new_policy);

    /// @}

    protected:
    /// The root node of the tree.
    Node* root_;

    /// The comparison function.
    Cmp cmp_;

    /// The list of iterators pointing to the binary search tree.
    mutable iterator* iterator_list_;

    /// The uniqueness property: whether the same value can appear multiple
    /// times.
    mutable bool uniqueness_policy_;

    /// The number of elements stored in the tree.
    Size nb_elements_;

    /**
     * @brief Pseudo static iterator.
     *
     * The end and rend iterators are constructed only once per binary search
     * tree so as to optimize for(iter = begin();iter != end(); iter++) loops:
     * this will avoid creating objects end and rend each time we pass in the
     * loop.
     */
    iterator iter_end_;

    /// @brief To speed-up accesses.
    /// @{
    friend class BinSearchTreeIterator< Val, Cmp, Node >;
    /// @}

    /**
     * @brief A method for recursively copying the contents of a BinSearchTree.
     *
     * @warning This function produces a tree with precisely the same structure
     * as that passed in argument (node). Hence, this should be used only when
     * copying binary search trees storing data w.r.t. the same weak order Cmp.
     *
     * @param root_from The root of the tree to be copied.
     * @param parent The node that should be the parent of the copy.
     * @param dir The direction of the edge parent->copy.
     */
    Node* copy_(Node* root_from, Node* parent = 0, BinTreeDir dir = BinTreeDir::LEFT_CHILD);

    /**
     * @brief Returns the smallest node w.r.t. order Cmp in the subtree rooted
     * at node.
     * @param node The root for looking for the the smallest node.
     * @return Returns the smallest node.
     */
    Node* minNode_(Node* node) const;

    /**
     * @brief Returns the greatest node w.r.t. order Cmp in the subtree rooted
     * at node.
     * @param node The root for looking for the the greatest node.
     * @return Returns the greatest node.
     */
    Node* maxNode_(Node* node) const;

    /**
     * @brief Returns the next node according to the weak ordering Cmp.
     * @param node The node for which the sucessor is returned.
     * @return Returns the next node according to the weak ordering Cmp.
     */
    Node* succNode_(Node* node) const;

    /**
     * @brief Returns the previous node according to weak ordering Cmp.
     * @param node The node for which the previous node is returned.
     * @return Returns the previous node according to the weak ordering Cmp.
     */
    Node* prevNode_(Node* node) const;

    /**
     * @brief Returns the node containing a given value.
     * @param val The value of the node to return.
     * @return Returns the node containing a given value (0 if the value cannot
     * be found).
     */
    Node* getNode_(const Val& val) const;

    ///
    /**
     * @brief A method for recursively deleting a subtree of the
     * gum::BinSearchTree.
     *
     * Note that this method does not update the iterators pointing to nodes of
     * the subtree. These should be cleared before deleteSubTree_ is called.
     *
     * @param node The root of the subtree to delete.
     */
    void deleteSubTree_(Node* node);

    /**
     * @brief Erase the node passed in argument.
     * @param node The Node to erase.
     */
    virtual void erase_(Node* node);

    private:
    /**
     * @brief Erase a node with two children.
     *
     * This is used by gum::BinSearchTree::erase_(Node*).
     * @param node The node to erase.
     */
    void _eraseWithTwoChildren_(Node* node);

    protected:
    /**
     * @brief Creates a copy of the value, insert it in the gum::BinSearchTree
     * and returns the copy value.
     *
     * When elements are inserted into gum::BinSearchTree, this is actually
     * copies that are inserted. Thus, the method returns the node containing
     * the newly created copy, so that the user may reference the new copy.
     *
     * @warning this method is actually the implementation of method insert. It
     * is used to speed-up insertions in terminal classes such as AVL trees.
     * @throw DuplicateElement exception is raised if the binary tree already
     * contains the value and the uniqueness property is set to true.
     *
     * @param val The value added by copy.
     * @return Returns the node containing the newly created copy.
     */
    virtual Node* insert_(const Val& val);

    private:
    /**
     * @brief Update all iterators when a given node is deleted.
     * @param node The node that is erased.
     */
    void _updateEraseIterators_(Node* node);
  };

  // ===========================================================================
  // ===                 GENERIC BINARY SEARCH TREE ITERATORS                ===
  // ===========================================================================

  /**
   * @class BinSearchTreeIterator
   * @headerfile binSearchTree.h <agrum/tools/core/binSearchTree.h>
   * @brief A Generic binary search tree.
   * @ingroup basicstruct_group
   *
   * @tparam Val The values type to store in the binary search tree.
   * @tparam Cmp The compatator for sorting the binary search tree.
   * @tparam Node The nodes type used to store values in the binary search
   * tree.
   */
  template < typename Val, class Cmp, class Node >
  class BinSearchTreeIterator {
    public:
    // ============================================================================
    /// Class Constructors and Destructors
    // ============================================================================
    ///@{

    /**
     * @brief Basic constructor: returns an iterator pointing toward nothing.
     */
    BinSearchTreeIterator();

    /**
     * @brief Copy constructor: creates an iterator pointing toward the same
     * tree.
     * @param from The gum::BinSearchTreeIterator to copy.
     */
    BinSearchTreeIterator(const BinSearchTreeIterator< Val, Cmp, Node >& from);

    /// @brief Class destructor.
    ~BinSearchTreeIterator();

    ///@}
    // ============================================================================
    /// Class operators
    // ============================================================================
    ///@{

    /**
     * @brief Copy operator.
     * @param from the gum::BinSearchTreeIterator to copy.
     * @return Returns this gum::BinSearchTreeIterator.
     */
    BinSearchTreeIterator< Val, Cmp, Node >&
       operator=(const BinSearchTreeIterator< Val, Cmp, Node >& from);

    /**
     * @brief Returns the value pointed to by the iterator.
     * @return Returns the value pointed to by the iterator.
     * @throw UndefinedIteratorValue Raised if the iterator does not point to a
     * valid node of the tree.
     */
    const Val& operator*() const;

    /**
     * @brief Point the iterator to the next value in the binary search tree.
     *
     * A binary search tree stores data according to a complete weak order <=.
     * A ++ operation on an iterator makes the latter point on the next value
     * in the tree w.r.t. ordering <=.
     *
     * Loops are guaranteed to parse the whole binary search tree as long as no
     * element is added to or deleted from the tree while being in the loop.
     * Deleting elements during the loop is guaranteed to never produce a
     * segmentation fault.
     *
     * @code
     * for (iter = tree.begin(); iter != tree.end(); ++iter) {
     *   // some code
     * }
     * @endcode
     *
     * @return Returns this gum::BinSearchTreeIterator.
     */
    BinSearchTreeIterator< Val, Cmp, Node >& operator++();

    /**
     * @brief Point the iterator to the preceding value in the binary search
     * tree.
     *
     * A binary search tree stores data according to a complete weak order <=.
     * A -- operation on an iterator makes the latter point on the preceding
     * value in the tree w.r.t. ordering <=.
     *
     * Loops are guaranteed to parse the whole binary search tree as long as no
     * element is added to or deleted from the tree while being in the loop.
     * Deleting elements during the loop is guaranteed to never produce a
     * segmentation fault.
     *
     * @code
     * for (iter = tre.rbegin(); iter != tree.rend(); --iter) {
     *   // some code
     * }
     * @endcode
     */
    BinSearchTreeIterator< Val, Cmp, Node >& operator--();

    /**
     * @brief Checks whether two iterators are pointing toward different
     * elements.
     * @param from The gum::BinSearchTreeIterator to test for inequality.
     * @return Returns true if the two gum::BinSearchTreeIterator are
     * different.
     */
    bool operator!=(const BinSearchTreeIterator< Val, Cmp, Node >& from) const;

    /**
     * @brief Checks whether two iterators are pointing toward identical
     * elements.
     * @param from The gum::BinSearchTreeIterator to test for equality.
     * @return Returns true if the two gum::BinSearchTreeIterator are
     * equal.
     */
    bool operator==(const BinSearchTreeIterator< Val, Cmp, Node >& from) const;

    ///@}
    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /**
     * @brief Makes the iterator move to its parent node.
     * @return Return this gum::BinSearchTreeIterator.
     */
    BinSearchTreeIterator< Val, Cmp, Node >& up();

    /**
     * @brief Makes the iterator move to the left child of the node it points
     * to.
     * @return Return this gum::BinSearchTreeIterator.
     */
    BinSearchTreeIterator< Val, Cmp, Node >& downLeft();

    /**
     * @brief Makes the iterator move to the right child of the node it points
     * to.
     * @return Return this gum::BinSearchTreeIterator.
     */
    BinSearchTreeIterator< Val, Cmp, Node >& downRight();

    /**
     * @brief Detach the iterator from its current tree (if any) and reset it.
     */
    void clear();

    ///@}

    protected:
    /// The current node pointed to by the iterator.
    Node* node_;

    /// The next node to be used when node_=nullptr (if a ++ operator is applied).
    Node* next_node_;

    /// The preceding node to be used when node_=nullptr (if a -- operator is
    /// applied).
    Node* prev_node_;

    /// The parent to be used when node_=nullptr (if operation up is applied).
    Node* parent_;

    /// The left child to be used when node_=nullptr and leftdown() is applied.
    Node* left_child_;

    /// The right child to be used when node_=nullptr and rightdown() is applied.
    Node* right_child_;

    /// The binary search tree pointed to by the iterator.
    BinSearchTree< Val, Cmp, Node >* tree_;

    /// The next iterator in the list of iterators of the binSearchTree.
    BinSearchTreeIterator< Val, Cmp, Node >* next_iter_;

    private:
    /// To speed-up accesses.
    /// @{
    friend class BinSearchTree< Val, Cmp, Node >;
    /// @}

    /**
     * @brief A function called to initialize an iterator.
     *
     * Assuming that the iterator has been constructed using the default
     * constructor (i.e., it points toward no binary search tree), this method
     * make it point toward one tree and, if needed, add it to the set of
     * iterators of the tree.
     *
     * @param tree The gum::BinSearchTree to iterate on.
     * @param current_node The node pointed by this iterator.
     * @param add_to_iterator_list Add this iterator to the iterator list.
     */
    void initialize_(const BinSearchTree< Val, Cmp, Node >* tree,
                     const Node*                            current_node,
                     bool                                   add_to_iterator_list);

    /**
     * @brief a method to detach the current iterator from its tree's
     * iterator's list.
     */
    void detachFromTree_();
  };

} /* namespace gum */


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
extern template class gum::BinSearchTree< int >;
#endif


// always include the template implementations
#include <agrum/tools/core/binSearchTree_tpl.h>

#endif   // GUM_BIN_SEARCH_TREE_H
