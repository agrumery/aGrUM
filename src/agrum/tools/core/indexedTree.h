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
 * @brief Class for storing trees (as data structures, not graphs).
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_INDEXED_TREE_H
#define GUM_INDEXED_TREE_H

#include <iostream>

#include <agrum/agrum.h>
#include <agrum/tools/core/hashTable.h>

namespace gum {

  // =========================================================================
  // ===                NODES FOR GENERIC TREE (DATA) STRUCTURE === */
  // =========================================================================

  /**
   * @class IndexedTree
   * @headerfile indexedTree.h <agrum/tools/core/indexedTree.h>
   * @brief The class for storing the nodes of the Arborescence
   * @ingroup basicstruct_group
   *
   * @tparam Key The tree's keys type.
   * @tparam Data The tree's values type.
   */
  template < typename Key, typename Data >
  class IndexedTree {
    public:
    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /**
     * @brief Creates a tree with one node with or without data.
     *
     * If data equals the nullptr, then tree is created with one node without
     * data.
     *
     * @param data Adds data as the root of the tree.
     */
    IndexedTree(Data* data = nullptr);

    /**
     * @brief Creates a tree with one node (with or without data).
     *
     * The parameters are inserted directly into the tree, i.e., no copy is
     * performed. For copies of key and data to occur, use the constructor
     * with const& parameters.
     *
     * If data equals the nullptr, then tree is created with one node without
     * data.
     *
     * @param theKey The data's key.
     * @param data The data added to the tree.
     */
    IndexedTree(const Key& theKey, Data* data = nullptr);

    /**
     * @brief Creates a tree with one node with data.
     *
     * The key and data are copied into the tree. If you do not want any copy,
     * use the constructor with the pointers parameters.
     *
     * @param theKey The data's key.
     * @param data The data added to the tree.
     */
    IndexedTree(const Key& theKey, const Data& data);

    /**
     * @brief Copy constructor.
     * @param from The gum::IndexedTree to copy.
     */
    IndexedTree(const IndexedTree< Key, Data >& from);

    /**
     * @brief Copy operator.
     * @param from The gum::IndexedTree to copy.
     * @return Returns this gum::IndexedTree.
     */
    IndexedTree< Key, Data >& operator=(const IndexedTree< Key, Data >& from);

    /**
     * @brief Class destructor.
     */
    ~IndexedTree();

    /// @}
    // ============================================================================
    /// @name Accessors / Modifiers
    // ============================================================================
    /// @{

    /**
     * @brief Adds a new node into the tree.
     *
     * @param index The node's index.
     * @param data The node's data.
     * @throw DuplicateElement exception is thrown if a node with an identical
     * index has already been entered into the tree. If, in this case, you
     * would like the value of the to be updated, use function setNode instead.
     */
    void insertNode(const std::vector< Key >& index, const Data* data);

    /**
     * @brief Adds a new node into the tree.
     *
     * @param index The node's index.
     * @param data The node's data.
     * @throw DuplicateElement exception is thrown if a node with an identical
     * index has already been entered into the tree. If, in this case, you
     * would like the value of the to be updated, use function setNode instead.
     */
    void insertNode(const std::vector< Key >& index, const Data& data);

    /**
     * @brief Updates the value of a node (or adds it if it does not already
     * exist).
     *
     * @param index The node's index.
     * @param data The node's data.
     */
    void setNode(const std::vector< Key >& index, Data* data);

    /**
     * @brief Updates the value of a node (or adds it if it does not already
     * exist).
     *
     * @param index The node's index.
     * @param data The node's data.
     */
    void setNode(const std::vector< Key >& index, const Data& data);

    /**
     * @brief Returns the value of a given node of the tree.
     *
     * @param index The node's index.
     * @return Returns the data at index.
     * @throw NotFound exception is thrown if the so-called value
     * cannot be found in the tree.
     */
    Data& getData(const std::vector< Key >& index) const;

    /**
     * @brief Returns a given node of the tree.
     * @param index The node's index.
     * @return Returns a given node of the tree.
     * @throw NotFound exception is thrown if the node we look for cannot
     * be found in the tree.
     */
    IndexedTree< Key, Data >& getNode(const std::vector< Key >& index) const;

    /// @}

    private:
    /// The key of the current node.
    Key key;

    /// The data stored into the node.
    Data* data;

    /// The parent of the node.
    IndexedTree< Key, Data >* parent;

    /// The list of children nodes of the current node.
    HashTable< Key, IndexedTree< Key, Data >* > children;
  };

  /// Necessary for the hashtable operator <<
  template < typename Key, typename Data >
  std::ostream& operator<<(std::ostream&, const IndexedTree< Key, Data >&);

} /* namespace gum */

// always include the templated implementations
#include <agrum/tools/core/indexedTree_tpl.h>

#endif /* GUM_INDEXED_TREE_H */
