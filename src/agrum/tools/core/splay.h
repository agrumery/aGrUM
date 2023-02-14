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
 * @brief Splay Trees header file.
 *
 * @author Karim Tekkal
 */

#ifndef GUM_SPLAY_H
#define GUM_SPLAY_H

#include <iostream>
// #include <stdlib.h>

#include <agrum/agrum.h>
#include <agrum/tools/core/hashTable.h>

namespace gum {

  template < class Element >
  class SplayBinaryNode;
  template < class Element >
  class SplayTree;

  /// Display the node

  template < typename Element >
  INLINE std::ostream& operator<<(std::ostream& out, const SplayBinaryNode< Element >& e);

  /// Display the tree

  template < typename Element >
  INLINE std::ostream& operator<<(std::ostream& out, const SplayTree< Element >& s);

  // =========================================================================
  // ===                                   NODE                            ===
  // =========================================================================
  /**
   * @class SplayBinaryNode
   * @headerfile splay.h <agrum/tools/core/splay.h>
   * @brief the nodes of splay trees
   * @ingroup splaytree_group
   *
   * @author Karim Tekkal
   *
   * These file implements a data structure. A splay tree is a self-balancing
   * binary search tree.
   */
  template < class Element >
  class SplayBinaryNode {
    public:
    // ============================================================================
    /// @name Accessors / Modifiers
    // ============================================================================
    /// @{

    /**
     * @brief Position of the node.
     * @return The position of the node into the tree.
     */
    int position() const;

    /**
     * @brief Returns the element in the node.
     * @return Returns the element in the node.
     */
    const Element& getElement() const;

    /**
     * @brief Returns the left child.
     * @return Returns the left child.
     * @warning The returned value can be null.
     */
    const SplayBinaryNode< Element >* getFg() const;

    /**
     * @brief Returns the right child.
     * @return Returns the right child.
     * @warning The returned value can be null.
     */
    const SplayBinaryNode< Element >* getFd() const;

    /// @}

    protected:
    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /**
     * @brief Basic constructor: creates a node with a reference to the element.
     * @param e The element in the node.
     * @param addr TODO don't know what to do here.
     * @param g The left child of the node, can be nullptr.
     * @param d The right child of the node, can be nullptr.
     * @param p The father of the node, can be nullptr if the node
     * is the root of the tree.
     */
    SplayBinaryNode(const Element&                                     e,
                    HashTable< Element, SplayBinaryNode< Element >* >& addr,
                    SplayBinaryNode*                                   g = 0,
                    SplayBinaryNode*                                   d = 0,
                    SplayBinaryNode*                                   p = 0);

    /**
     * @brief Copy constructor.
     * @param from the src SplayBinaryNode
     * @param addr TODO don't know what to do here.
     */
    SplayBinaryNode(const SplayBinaryNode< Element >&                  from,
                    HashTable< Element, SplayBinaryNode< Element >* >& addr);

    /**
     * @brief A function used to perform copies.
     * @param from the src SplayBinaryNode
     * @param addr TODO don't know what to do here ..
     */
    void copy_(const SplayBinaryNode< Element >&                  from,
               HashTable< Element, SplayBinaryNode< Element >* >& addr);

    /**
     * @brief Class destructor.
     */
    ~SplayBinaryNode();

    /// @}
    // ============================================================================
    /// @name Accessors / Modifiers
    // ============================================================================
    /// @{

    /**
     * @brief A right rotation, the node must have a father.
     * @return Returns a pointer to the root of the sub-tree after rotation.
     */
    SplayBinaryNode< Element >* zig();

    /**
     * @brief A left rotation, the node must hava a father.
     * @return Returns a pointer to the root of the sub-tree after rotation.
     */
    SplayBinaryNode< Element >* zag();

    /**
     * @brief A splay rotation, the node will be the root of the tree.
     * @return Returns a pointer to the root of the sub-tree after rotation.
     */
    SplayBinaryNode< Element >* splay();

    /**
     * @brief Concatenation of two trees.
     * @param e The node to add.
     * @param addr TODO Don't know what to do here.
     * @return Returns the root of the created tree.
     */
    SplayBinaryNode< Element >* join(const SplayBinaryNode< Element >*                  e,
                                     HashTable< Element, SplayBinaryNode< Element >* >& addr);

    /// @}
    // ============================================================================
    /// @name Data Members
    // ============================================================================
    /// @{

    /// The content.
    Element elt;

    /// The size of the sub-tree.
    Size size;

    /// The left child.
    SplayBinaryNode* fg;

    /// The right child.
    SplayBinaryNode* fd;

    /// The father, nullptr for the root.
    SplayBinaryNode* pere;

    /// @}

    /// Friendly with SplayTree
    friend class SplayTree< Element >;

    /// Friendly to display
    friend std::ostream& operator<< <>(std::ostream& out, const SplayBinaryNode< Element >&);
  };

  // ============================================================================
  // ===                                SPLAY TREE ===
  // ============================================================================
  /**
   * @class SplayTree
   * @headerfile splay.h <agrum/tools/core/splay.h>
   * @brief A splay tree.
   * @ingroup splaytree_group
   * @ingroup basicstruct_group
   *
   * @warning an Element must be in just one Splay Tree, the behavior is
   * unspecified else.
   *
   * @par Usage example:
   * @code
   * // Creating empty trees
   * SplayTree<string> a;
   *
   * // Make a copy
   * SplayTree<string> b(a);
   *
   * // Add an element
   * a.insert("toto");
   * a.insert("titi");
   *
   * // Get the first element
   * a.front();
   * // And the last
   * a.back();
   *
   * // concatenate two trees
   * a.join(b);
   *
   * // divide a tree at the third position, the first part is placed into a
   * // the second into b.
   * b = a.split(3);
   *
   * // Display the splay tree
   * a.printTree();
   *
   * // Get the size
   * a.size();
   * @endcode
   *
   * @tparam Element The elements type.
   */
  template < class Element >
  class SplayTree {
    public:
    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /**
     * @brief Basic constructor, make an empty splay tree.
     */
    SplayTree();

    /**
     * @brief Basic constructor, make a splay tree with one element.
     * @param e The element of the tree.
     */
    SplayTree(const Element& e);

    /**
     * @brief Copy constructor.
     * @param from The gum::SplayTree to copy.
     */
    SplayTree(const SplayTree& from);

    /**
     * @brief Class destructor.
     */
    ~SplayTree();

    /// @}
    // ============================================================================
    /// @name Operators
    // ============================================================================
    /// @{

    /**
     * @brief Assignment operator.
     * @param from The gum::SplayTree to copy.
     * @return This gum::SplayTree.
     */
    SplayTree< Element >& operator=(const SplayTree< Element >& from);

    /// @}
    // ============================================================================
    /// @name Methods
    // ============================================================================
    /// @{

    /**
     * @brief Get the element at the position n.
     * @param i The position of the element to return.
     * @throw NotFound Raised if no element was found.
     * @return Returns the element at the position n.
     */
    Element& operator[](const unsigned int i);

    /**
     * @brief Get the element at the position n.
     * @param i The position of the element to return.
     * @throw NotFound Raised if no element was found.
     * @return Returns the element at the position n.
     */
    const Element& operator[](const unsigned int i) const;


    /**
     * @brief Get the first element.
     * @throw NotFound Raised if the splay tree is empty.
     */
    Element& front();

    /**
     * @brief Get the last element.
     * @throw NotFound Raised if the splay tree is empty.
     */
    Element& back();

    /**
     * @brief Remove the first element.
     */
    void popFront();

    /**
     * @brief Remove the last element.
     */
    void popBack();

    /**
     * @brief Add an element in the first position.
     * @param e The element to push.
     */
    void pushFront(const Element& e);

    /**
     * @brief Add an element in the last position.
     * @param e The element to push.
     */
    void pushBack(const Element& e);

    /**
     * @brief Add an element to the tree.
     * @param e The element to add.
     */
    void insert(const Element& e);

    /**
     * @brief Concatenation of two trees.
     * @param s The tree to add.
     */
    void join(const SplayTree< Element >& s);


    /**
     * @brief Divide the tree at the position.
     *
     * @warning all the elements less than e are stored into the tree and those
     * who are greater than e in the returned tree.
     *
     * @param i The position of the element (e) for split.
     * @return Returns a tree with all the elements greater than i.
     */
    SplayTree< Element > split(const int i);


    /**
     * @brief Divide the tree at the position.
     * @param e the element for split
     * @warning All the elements less than e are stored into the tree and those
     * greater than e are in the returned tree
     * @warning The element e is neither in the trees.
     * @return Returns the tree with all value greather than e.
     */
    SplayTree< Element > split_by_val(const Element& e);

    /**
     * @brief The number of elements in the tree.
     * @return the size of the tree
     */
    Size size() const;

    /**
     * @brief Test if the tree contains the element.
     * @param e The element to test if it is in the splay tree.
     * @return Returns true if e is in this splay tree.
     */
    bool contains(const Element& e) const;

    /// @}

    protected:
    // ============================================================================
    /// @name Data Menbers
    // ============================================================================
    /// @{

    /// Root of the tree
    SplayBinaryNode< Element >* root;

    /// The hash table to find quickly the position of a node
    HashTable< Element, SplayBinaryNode< Element >* > addr;

    /// @}

    /// a function used to perform copies
    void copy_(const SplayTree< Element >&);

    /// Friendly to display
    friend std::ostream& operator<< <>(std::ostream& out, const SplayTree< Element >&);
  };

} /* namespace gum */

// always include the tpl_.h as it contains only templates
#include <agrum/tools/core/splay_tpl.h>

#endif /* GUM_SPLAY_H */
