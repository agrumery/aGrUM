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
 * @brief Splay trees
 *
 * @author Karim Tekkal
 *
 * These file implements a data structure. A splay tree is a self-balancing
 * binary search tree.
 *
 * @par Usage example:
 * @code
 *
 *
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
 *
 * @endcode
 *
 *
 */

#ifndef GUM_SPLAY_H
#define GUM_SPLAY_H

#include <iostream>

#include <agrum/config.h>

#include <agrum/core/hashTable.h>

namespace gum {

  template <class Element> class SplayBinaryNode;
  template <class Element> class SplayTree;

  /// Display the node

  template <typename Element>
  INLINE std::ostream &operator<<(std::ostream &out,
                                  const SplayBinaryNode<Element> &e);

  /// Display the tree

  template <typename Element>
  INLINE std::ostream &operator<<(std::ostream &out, const SplayTree<Element> &s);

  /* ============================================================================ */
  /* ===                                   NODE                               === */
  /* ============================================================================ */
  /** @class SplayBinaryNode
   * @brief the nodes of splay trees
   * @ingroup basicstruct_group
   */
  template <class Element> class SplayBinaryNode {
    public:
    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// Position of the node
    /**
     * @return the position of the node into the tree
     */

    int position() const;

    /// Element in the node
    /**
     * @return the element in the node
     */

    const Element &getElement() const;

    /**
     * @return the left child
     * @warning the returned value can be null
     */

    const SplayBinaryNode<Element> *getFg() const;

    /**
     * @return the right child
     * @warning the returned value can be null
     */

    const SplayBinaryNode<Element> *getFd() const;

    /// @}

    protected:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// basic constructor: creates a node with a reference to the element
    /**
    * @param e the element in the node
    * @param addr TODO don't know what to do here ..
     * @param g the left child of the node, can be nullptr
     * @param d the right child of the node, can be nullptr
     * @param p the father of the node, can be nullptr if the node
     * is the root of the tree */

    SplayBinaryNode(const Element &e,
                    HashTable<Element, SplayBinaryNode<Element> *> &addr,
                    SplayBinaryNode *g = 0, SplayBinaryNode *d = 0,
                    SplayBinaryNode *p = 0);

    /// copy constructor
    /**
    * @param from the src SplayBinaryNode
    * @param addr TODO don't know what to do here ..
    */

    SplayBinaryNode(const SplayBinaryNode<Element> &from,
                    HashTable<Element, SplayBinaryNode<Element> *> &addr);

    /// a function used to perform copies
    /**
    * @param from the src SplayBinaryNode
    * @param addr TODO don't know what to do here ..
    */

    void _copy(const SplayBinaryNode<Element> &from,
               HashTable<Element, SplayBinaryNode<Element> *> &addr);

    /// destructor

    ~SplayBinaryNode();

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// A right rotation, the node must have a father
    /**
     * @return a pointer to the root of the sub-tree after rotation
     */

    SplayBinaryNode<Element> *zig();

    /// A left rotation, the node must hava a father
    /**
     * @return a pointer to the root of the sub-tree after rotation
     */

    SplayBinaryNode<Element> *zag();

    /// A splay rotation, the node will be the root of the tree
    /**
     * @return a pointer to the root of the sub-tree after rotation
     */

    SplayBinaryNode<Element> *splay();

    /// Concatenation of two trees
    /**
     * @param e the node to add
     * @param addr TODO don't know what to do here ..
     * @return the root of the created tree
     */

    SplayBinaryNode<Element> *
    join(const SplayBinaryNode<Element> *e,
         HashTable<Element, SplayBinaryNode<Element> *> &addr);

    /// @}

    // ############################################################################
    /// @name Data Members
    // ############################################################################
    /// @{

    /// The content

    Element elt;

    /// The size of the sub-tree

    Size size;

    /// The left child

    SplayBinaryNode *fg;

    /// The right child

    SplayBinaryNode *fd;

    /// The father, nullptr for the root

    SplayBinaryNode *pere;

    /// @}

    /// Friendly with SplayTree

    friend class SplayTree<Element>;

    /// Friendly to display

    friend std::ostream &operator<<<>(std::ostream &out,
                                      const SplayBinaryNode<Element> &);
  };

  /* ============================================================================ */
  /* ===                                SPLAY TREE                            === */
  /* ============================================================================ */
  /** @class SplayTree
   * @brief the splay tree
   *
   * @warning an Element must be in just one Splay Tree,
   * the behavior is unspecified else.
   */
  template <class Element> class SplayTree {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// basic constructor, make an empty splay tree

    SplayTree();

    /// basic constructor, make a splay tree with one element
    /*
     * @param e the element of the tree
     */

    SplayTree(const Element &e);

    /// copy constructor

    SplayTree(const SplayTree &from);

    /// Assignment operator

    SplayTree<Element> &operator=(const SplayTree<Element> &from);

    /// Destructor

    ~SplayTree();

    /// @}

    // ############################################################################
    /// @name Methods
    // ############################################################################
    /// @{

    /// Get the element at the position n
    /**
     * @throw NotFound
     */

    Element &operator[](const unsigned int i);
    const Element &operator[](const unsigned int i) const;

    /// Get the first element
    /**
     * @throw NotFound
     */

    Element &front();

    /// Get the last element
    /**
     * @throw NotFound
     */

    Element &back();

    /// Remove the first element

    void popFront();

    /// Remove the last element

    void popBack();

    /// Add an element in the first position

    void pushFront(const Element &e);

    /// Add an element in the last position

    void pushBack(const Element &e);

    /// Add an element to the tree
    /*
     * @param e the element to add
     */

    void insert(const Element &e);

    /// Concatenation of two trees
    /*
     * @param s the tree to add
     */

    void join(const SplayTree<Element> &s);

    /// Divide the tree at the position
    /*
     * @param i the position of the element (e) for split
     * @warning all the elements less than e are stored into the tree and those who
     * are greater than e in the returned tree.
     */

    SplayTree<Element> split(const int i);

    /// Divide the tree at the position
    /*
     * @param e the element for split
     * @warning all the elements less than e are stored into the tree and those
     * greater than e are in the returned tree
     * @warning the element e is neither in the trees
     */

    SplayTree<Element> split_by_val(const Element &e);

    /// The number of elements in the tree
    /**
     * @return the size of the tree
     * @throw NotFound
     */

    Size size() const;

    /// Test if the tree contains the element

    bool contains(const Element &) const;

    /// @}

    protected:
    // ############################################################################
    /// @name Data Menbers
    // ############################################################################
    /// @{

    /// Root of the tree

    SplayBinaryNode<Element> *root;

    /// The hash table to find quickly the position of a node

    HashTable<Element, SplayBinaryNode<Element> *> addr;

    /// @}

    /// a function used to perform copies

    void _copy(const SplayTree<Element> &);

    /// Friendly to display

    friend std::ostream &operator<<<>(std::ostream &out, const SplayTree<Element> &);
  };

} /* namespace gum */

// always include the .tcc as it contains only templates
#include <agrum/core/splay.tcc>

#endif /* GUM_SPLAY_H */
