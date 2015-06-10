/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                                  *
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
 * @brief class for storing trees (as data structures, not graphs)
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_INDEXED_TREE_H
#define GUM_INDEXED_TREE_H

#include <iostream>

#include <agrum/config.h>

#include <agrum/core/hashTable.h>

namespace gum {

  /* ============================================================================ */
  /* ============================================================================ */
  /* ===                NODES FOR GENERIC TREE (DATA) STRUCTURE               === */
  /* ============================================================================ */
  /* ============================================================================ */
  /** @class IndexedTree
   * @brief The class for storing the nodes of the Arborescence
   * @ingroup basicstruct_group
   */
  /* ============================================================================ */

  template <typename Key, typename Data> class IndexedTree {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// creates a tree with one node without data

    IndexedTree(Data *data = 0);

    /// creates a tree with one node (with or without data)
    /** the parameters are inserted directly into the tree, i.e., no copy is
     * performed. For copies of key and data to occur, use the constructor
     * with const& parameters. */

    IndexedTree(const Key &theKey, Data *data = 0);

    /// creates a tree with one node with data
    /** the key and data are copied into the tree. If you do not want any copy,
     * use the constructor with the pointers parameters. */

    IndexedTree(const Key &theKey, const Data &data);

    /// copy constructor

    IndexedTree(const IndexedTree<Key, Data> &from);

    /// copy operator

    IndexedTree<Key, Data> &operator=(const IndexedTree<Key, Data> &from);

    /// destructor

    ~IndexedTree();

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// adds a new node into the tree
    /** @throw DuplicateElement exception is thrown if a node with an identical
     * index has already been entered into the tree. If, in this case, you would like
     * the value of the to be updated, use function setNode instead. */

    void insertNode(const std::vector<Key> &index, const Data *data);

    /// adds a new node into the tree
    /** @throw DuplicateElement exception is thrown if a node with an identical
     * index has already been entered into the tree. If, in this case, you would like
     * the value of the to be updated, use function setNode instead. */

    void insertNode(const std::vector<Key> &index, const Data &data);

    /// updates the value of a node (or adds it if it does not already exist)

    void setNode(const std::vector<Key> &index, Data *data);

    /// updates the value of a node (or adds it if it does not already exist)

    void setNode(const std::vector<Key> &index, const Data &data);

    /// returns the value of a given node of the tree
    /** @throw NotFound exception is thrown if the so-called value
     * cannot be found in the tree. */

    Data &getData(const std::vector<Key> &index) const;

    /// returns a given node of the tree
    /** @throw NotFound exception is thrown if the node we look for cannot
     * be found in the tree. */

    IndexedTree<Key, Data> &getNode(const std::vector<Key> &index);

    /// @}

    private:
    /// the key of the current node
    Key key;

    /// the data stored into the node
    Data *data;

    /// the parent of the node
    IndexedTree<Key, Data> *parent;

    /// the list of children nodes of the current node
    HashTable<Key, IndexedTree<Key, Data> *> children;
  };

  /// necessary for the hashtable operator <<
  template <typename Key, typename Data>
  std::ostream &operator<<(std::ostream &, const IndexedTree<Key, Data> &);

} /* namespace gum */

/// always include the templated implementations
#include <agrum/core/indexedTree.tcc>

#endif /* GUM_INDEXED_TREE_H */
