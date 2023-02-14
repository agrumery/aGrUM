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
 * @brief Implementation of tree data structures
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

// to ease IDE parser
#include <agrum/tools/core/indexedTree.h>

namespace gum {
  /* =========================================================================*/
  /* =========================================================================*/
  /* ===       IMPLEMENTATION OF NODES FOR GENERIC TREE (DATA) STRUCTURE === */
  /* =========================================================================*/
  /* =========================================================================*/

  // creates a tree with one node (with or without data)

  template < typename Key, typename Data >
  IndexedTree< Key, Data >::IndexedTree(const Key& theKey, Data* theData) :
      key(theKey), data(theData), parent(0) {
    // for debugging purposes
    GUM_CONSTRUCTOR(IndexedTree);
  }

  // creates a tree with one node (with or without data)

  template < typename Key, typename Data >
  IndexedTree< Key, Data >::IndexedTree(Data* theData) : data(theData), parent(0) {
    // for debugging purposes
    GUM_CONSTRUCTOR(IndexedTree);
  }

  // creates a tree with one node with data

  template < typename Key, typename Data >
  IndexedTree< Key, Data >::IndexedTree(const Key& theKey, const Data& theData) :
      key(theKey), data(new Data(theData)), parent(0) {
    // for debugging purposes
    GUM_CONSTRUCTOR(IndexedTree);
  }

  // copy constructor

  template < typename Key, typename Data >
  IndexedTree< Key, Data >::IndexedTree(const IndexedTree< Key, Data >& from) :
      key(from.key), data(0), parent(0) {
    // for debugging purposes
    GUM_CONS_CPY(IndexedTree);

    try {
      // create the data of the node
      if (from.data) data = new Data(*from.data);

      // create and link properly the children
      children = from.children;

      for (HashTableIteratorSafe< Key, IndexedTree< Key, Data > > iter = children.begin();
           iter != children.end();
           ++iter)
        iter->parent = this;
    } catch (...) {
      if (data) delete data;

      children.clear();

      throw;
    }
  }

  // copy operator

  template < typename Key, typename Data >
  IndexedTree< Key, Data >&
     IndexedTree< Key, Data >::operator=(const IndexedTree< Key, Data >& from) {
    // avoid self assignment
    if (this != &from) {
      // for debugging purposes
      GUM_OP_CPY(IndexedTree);

      try {
        key = from.key;

        if (data) delete data;

        if (from.data) data = new Data(*from.data);
        else data = 0;

        children = from.children;

        for (HashTableIteratorSafe< Key, IndexedTree< Key, Data > > iter = children.begin();
             iter != children.end();
             ++iter)
          iter->parent = this;
      } catch (...) {
        if (data) delete data;

        children.clear();

        throw;
      }
    }

    return *this;
  }

  // destructor

  template < typename Key, typename Data >
  IndexedTree< Key, Data >::~IndexedTree() {
    // for debugging purposes
    GUM_DESTRUCTOR(IndexedTree);

    if (data) delete data;
  }

  // adds a new node into the tree

  template < typename Key, typename Data >
  void IndexedTree< Key, Data >::insertNode(const std::vector< Key >& index, const Data* theData) {
    // parse the tree until we are on the proper index. Then, insert the new
    // node.
    // current_node is a pointer on the node of the tree corresponding to
    // position
    // i in vector index. When i+2 < index.size(), we need go down into the tree
    // structure before we can insert the new node
    IndexedTree< Key, Data >* current_node = this;
    unsigned int              i;

    for (i = 0; i + 1 < index.size(); ++i) {
      // if the node that should be on the path between the root of the tree and
      // the node that we wish to insert does not exist, create it
      if (!children.exists(index[i])) {
        IndexedTree< Key, Data >* new_node = new IndexedTree< Key, Data >(index[i], (Data*)0);
        current_node->children.insert(index[i], new_node);
        new_node->parent = this;
        current_node     = new_node;
      } else current_node = current_node->children[index[i]];
    }

    // here, we can insert the new node. if ind + 1 == index.size(), this means
    // that
    // the index vector was not empty, else the vector was empty and we are at
    // the
    // root of the tree
    if (i + 1 == index.size()) {
      // if the node to be inserted already exist, throw an exception
      if (current_node->children.exists(index[i])) {
        GUM_ERROR(DuplicateElement, "the indexed tree already contains the node")
      }

      // here, the node to be inserted does not exist, so we must create it
      IndexedTree< Key, Data >* new_node
         = new IndexedTree< Key, Data >(index[i], const_cast< Data* >(theData));

      current_node->children.insert(index[i], new_node);

      new_node->parent = current_node;
    } else {
      // here, the node to be inserted is the root of the tree (so it already
      // exists)
      GUM_ERROR(DuplicateElement, "the indexed tree already contains the node")
    }
  }

  // adds a new node into the tree

  template < typename Key, typename Data >
  void IndexedTree< Key, Data >::insertNode(const std::vector< Key >& index, const Data& theData) {
    // parse the tree until we are on the proper index. Then, insert the new
    // node.
    // current_node is a pointer on the node of the tree corresponding to
    // position
    // i in vector index. When i+2 < index.size(), we need go down into the tree
    // structure before we can insert the new node
    IndexedTree< Key, Data >* current_node = this;
    unsigned int              i;

    for (i = 0; i + 1 < index.size(); ++i) {
      // if the node that should be on the path between the root of the tree and
      // the node that we wish to insert does not exist, create it
      if (!children.exists(index[i])) {
        IndexedTree< Key, Data >* new_node = new IndexedTree< Key, Data >(index[i], (Data*)0);
        current_node->children.insert(index[i], new_node);
        new_node->parent = this;
        current_node     = new_node;
      } else current_node = current_node->children[index[i]];
    }

    // here, we can insert the new node. if ind + 1 == index.size(), this means
    // that
    // the index vector was not empty, else the vector was empty and we are at
    // the
    // root of the tree
    if (i + 1 == index.size()) {
      // if the node to be inserted already exist, throw an exception
      if (current_node->children.exists(index[i])) {
        GUM_ERROR(DuplicateElement, "the indexed tree already contains the node")
      }

      // here, the node to be inserted does not exist, so we must create it
      IndexedTree< Key, Data >* new_node = new IndexedTree< Key, Data >(index[i], theData);

      current_node->children.insert(index[i], new_node);

      new_node->parent = current_node;
    } else {
      // here, the node to be inserted is the root of the tree (so it already
      // exists)
      GUM_ERROR(DuplicateElement, "the indexed tree already contains the node")
    }
  }

  // updates the value of a node (or adds it if it does not already exist)

  template < typename Key, typename Data >
  void IndexedTree< Key, Data >::setNode(const std::vector< Key >& index, Data* theData) {
    // parse the tree until we are on the proper index. Then, insert the new
    // node.
    // current_node is a pointer on the node of the tree corresponding to
    // position
    // i in vector index. When i+2 < index.size(), we need go down into the tree
    // structure before we can insert the new node
    IndexedTree< Key, Data >* current_node = this;
    unsigned int              i;

    for (i = 0; i + 1 < index.size(); ++i) {
      // if the node that should be on the path between the root of the tree and
      // the node that we wish to insert does not exist, create it
      if (!children.exists(index[i])) {
        IndexedTree< Key, Data >* new_node = new IndexedTree< Key, Data >(index[i], (Data*)0);
        current_node->children.insert(index[i], new_node);
        new_node->parent = this;
        current_node     = new_node;
      } else current_node = current_node->children[index[i]];
    }

    // here, we can set the new node. if ind + 1 == index.size(), this means
    // that
    // the index vector was not empty, else the vector was empty and we are at
    // the
    // root of the tree
    if (i + 1 == index.size()) {
      // if the node to be set does not exist, create it, else modify it
      if (current_node->children.exists(index[i])) {
        IndexedTree< Key, Data >* node = current_node->children[index[i]];

        if (node->data) delete node->data;

        node->data = theData;
      } else {
        // here, the node tobe set does not exist, so we must create it
        IndexedTree< Key, Data >* new_node = new IndexedTree< Key, Data >(index[i], theData);
        current_node->children.insert(index[i], new_node);
        new_node->parent = current_node;
      }
    } else {
      // here, the node to be set is the root of the tree (so it does already
      // exist
      if (data) delete data;

      data = theData;
    }
  }

  // updates the value of a node (or adds it if it does not already exist)

  template < typename Key, typename Data >
  void IndexedTree< Key, Data >::setNode(const std::vector< Key >& index, const Data& theData) {
    // parse the tree until we are on the proper index. Then, insert the new
    // node.
    // current_node is a pointer on the node of the tree corresponding to
    // position
    // i in vector index. When i+2 < index.size(), we need go down into the tree
    // structure before we can insert the new node
    IndexedTree< Key, Data >* current_node = this;
    unsigned int              i;

    for (i = 0; i + 1 < index.size(); ++i) {
      // if the node that should be on the path between the root of the tree and
      // the node that we wish to insert does not exist, create it
      if (!children.exists(index[i])) {
        IndexedTree< Key, Data >* new_node = new IndexedTree< Key, Data >(index[i], (Data*)0);
        current_node->children.insert(index[i], new_node);
        new_node->parent = this;
        current_node     = new_node;
      } else current_node = current_node->children[index[i]];
    }

    // here, we can set the new node. if ind + 1 == index.size(), this means
    // that
    // the index vector was not empty, else the vector was empty and we are at
    // the
    // root of the tree
    if (i + 1 == index.size()) {
      // if the node to be set does not exist, create it, else modify it
      if (current_node->children.exists(index[i])) {
        IndexedTree< Key, Data >* node = current_node->children[index[i]];

        if (node->data) delete node->data;

        node->data = new Data(theData);
      } else {
        // here, the node tobe set does not exist, so we must create it
        IndexedTree< Key, Data >* new_node = new IndexedTree< Key, Data >(index[i], theData);
        current_node->children.insert(index[i], new_node);
        new_node->parent = current_node;
      }
    } else {
      // here, the node to be set is the root of the tree (so it does already
      // exist
      if (data) delete data;

      data = new Data(theData);
    }
  }

  // returns the value of a given test from the cache

  template < typename Key, typename Data >
  INLINE Data& IndexedTree< Key, Data >::getData(const std::vector< Key >& index) const {
    IndexedTree< Key, Data >* current_node = const_cast< IndexedTree< Key, Data >* >(this);

    for (unsigned int i = 0; i < index.size(); ++i)
      current_node = current_node->children[index[i]];

    if (data == 0) { GUM_ERROR(NotFound, "the datum could not be found") }

    return *(current_node->data);
  }

  // returns a given node of the tree

  template < typename Key, typename Data >
  INLINE IndexedTree< Key, Data >&
         IndexedTree< Key, Data >::getNode(const std::vector< Key >& index) const {
    IndexedTree< Key, Data >* current_node = const_cast< IndexedTree< Key, Data >* >(this);

    for (unsigned int i = 0; i < index.size(); ++i)
      current_node = current_node->children[index[i]];

    return *current_node;
  }

} /* namespace gum */
