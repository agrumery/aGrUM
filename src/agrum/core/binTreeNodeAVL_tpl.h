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
 * @brief Node class for AVL binary search trees
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

// to ease IDE parsers
#include <agrum/core/binTreeNodeAVL.h>

namespace gum {

  // ===========================================================================
  // ===========================================================================
  // ===                  NODES FOR AVL BINARY SEARCH TREES                  ===
  // ===========================================================================
  // ===========================================================================

  template <typename Val>
  INLINE BinTreeNodeAVL<Val>::BinTreeNodeAVL( const Val& val )
      : BinTreeNode<Val>( val )
      , __height( 1 ) {
    GUM_CONSTRUCTOR( BinTreeNodeAVL );
  }

  template <typename Val>
  INLINE BinTreeNodeAVL<Val>::BinTreeNodeAVL( const BinTreeNodeAVL<Val>& from )
      : BinTreeNode<Val>( from )
      , __height( from.__height ) {
    GUM_CONS_CPY( BinTreeNodeAVL );
  }

  template <typename Val>
  INLINE BinTreeNodeAVL<Val>::~BinTreeNodeAVL() {
    GUM_DESTRUCTOR( BinTreeNodeAVL );
  }

  template <typename Val>
  INLINE BinTreeNodeAVL<Val>& BinTreeNodeAVL<Val>::
  operator=( const BinTreeNodeAVL<Val>& from ) {
    GUM_OP_CPY( BinTreeNodeAVL );
    BinTreeNode<Val>::operator=( from );
    return *this;
  }

  template <typename Val>
  INLINE Val& BinTreeNodeAVL<Val>::operator*() {
    return BinTreeNode<Val>::operator*();
  }

  template <typename Val>
  INLINE Val& BinTreeNodeAVL<Val>::value() {
    return BinTreeNode<Val>::value();
  }

  template <typename Val>
  INLINE BinTreeNodeAVL<Val>*
  BinTreeNodeAVL<Val>::child( BinTreeDir dir ) const {
    return static_cast<BinTreeNodeAVL<Val>*>( BinTreeNode<Val>::child( dir ) );
  }

  template <typename Val>
  INLINE BinTreeNodeAVL<Val>* BinTreeNodeAVL<Val>::leftChild() const {
    return static_cast<BinTreeNodeAVL<Val>*>( BinTreeNode<Val>::leftChild() );
  }

  template <typename Val>
  INLINE BinTreeNodeAVL<Val>* BinTreeNodeAVL<Val>::rightChild() const {
    return static_cast<BinTreeNodeAVL<Val>*>( BinTreeNode<Val>::rightChild() );
  }

  template <typename Val>
  INLINE BinTreeNodeAVL<Val>* BinTreeNodeAVL<Val>::parent() const {
    return static_cast<BinTreeNodeAVL<Val>*>( BinTreeNode<Val>::parent() );
  }

  template <typename Val>
  INLINE BinTreeDir BinTreeNodeAVL<Val>::parentDir() const {
    return BinTreeNode<Val>::parentDir();
  }

  template <typename Val>
  INLINE BinTreeNodeAVL<Val>* BinTreeNodeAVL<Val>::__rightRotation() {
    BinTreeNodeAVL<Val>* node_P = leftChild();
    BinTreeNodeAVL<Val>* V = node_P->rightChild();
    BinTreeNodeAVL<Val>* the_parent = parent();
    BinTreeDir            parent_dir = parentDir();
    unsigned int          left_height = 0;
    unsigned int          right_height = rightChild() ? rightChild()->__height : 0;

    eraseLeftLink();

    if ( V ) {
      node_P->eraseRightLink();
      BinTreeNode<Val>::insertLeftChild( *V );
      left_height = V->__height;
    }

    __height = 1 + std::max( left_height, right_height );

    if ( the_parent ) {
      the_parent->eraseLink( parent_dir );
      the_parent->BinTreeNode<Val>::insertChild( *node_P, parent_dir );
    }

    node_P->BinTreeNode<Val>::insertRightChild( *this );
    left_height = node_P->leftChild() ? node_P->leftChild()->__height : 0;
    node_P->__height = 1 + std::max( left_height, __height );

    return node_P;
  }

  template <typename Val>
  INLINE BinTreeNodeAVL<Val>* BinTreeNodeAVL<Val>::__leftRotation() {
    BinTreeNodeAVL<Val>* node_Q = rightChild();
    BinTreeNodeAVL<Val>* V = node_Q->leftChild();
    BinTreeNodeAVL<Val>* the_parent = parent();
    BinTreeDir            parent_dir = parentDir();
    unsigned int          right_height = 0;
    unsigned int          left_height = leftChild() ? leftChild()->__height : 0;

    eraseRightLink();

    if ( V ) {
      node_Q->eraseLeftLink();
      BinTreeNode<Val>::insertRightChild( *V );
      right_height = V->__height;
    }

    __height = 1 + std::max( left_height, right_height );

    if ( the_parent ) {
      the_parent->eraseLink( parent_dir );
      the_parent->BinTreeNode<Val>::insertChild( *node_Q, parent_dir );
    }

    node_Q->BinTreeNode<Val>::insertLeftChild( *this );
    right_height = node_Q->rightChild() ? node_Q->rightChild()->__height : 0;
    node_Q->__height = 1 + std::max( __height, right_height );

    return node_Q;
  }

  template <typename Val>
  INLINE BinTreeNodeAVL<Val>* BinTreeNodeAVL<Val>::__balance() {
    BinTreeNodeAVL<Val> *node = this, *root = this;

    while ( node ) {
      // compute the heights of the left and right trees
      unsigned int left_height =
          node->leftChild() ? node->leftChild()->__height : 0;
      unsigned int right_height =
          node->rightChild() ? node->rightChild()->__height : 0;
      unsigned int old_height = node->__height;
      node->__height = 1 + std::max( left_height, right_height );

      // check whether we need a single or a double rotation
      // see the AVL trees literature to see what need be done
      if ( left_height - right_height == 2 ) {
        BinTreeNodeAVL<Val>* left_child = node->leftChild();
        left_height =
            left_child->leftChild() ? left_child->leftChild()->__height : 0;
        right_height =
            left_child->rightChild() ? left_child->rightChild()->__height : 0;

        if ( left_height < right_height ) left_child->__leftRotation();

        node = node->__rightRotation();
      } else if ( right_height - left_height == 2 ) {
        BinTreeNodeAVL<Val>* right_child = node->rightChild();
        left_height =
            right_child->leftChild() ? right_child->leftChild()->__height : 0;
        right_height =
            right_child->rightChild() ? right_child->rightChild()->__height : 0;

        if ( right_height < left_height ) right_child->__rightRotation();

        node = node->__leftRotation();
      }
      // we need not rebalance the tree here. If, in addition, the height did
      // not change, then there is no need to rebalance the ancestors. Hence
      // we return 0 to indicate that the root has not changed
      else if ( node->__height == old_height ) {
        return 0;
      }

      // ok, here "node" is the root of a balanced tree. Now we need to check
      // whether the ancestors are well balanced
      root = node;
      node = node->parent();
    }

    // return the current root of the tree
    return root;
  }

  template <typename Val>
  INLINE BinTreeNodeAVL<Val>*
  BinTreeNodeAVL<Val>::insertLeftChild( const Val& val ) {
    BinTreeNodeAVL<Val>* new_child = new BinTreeNodeAVL<Val>( val );
    BinTreeNode<Val>::insertLeftChild( *new_child );
    return new_child;
  }

  template <typename Val>
  INLINE void
  BinTreeNodeAVL<Val>::insertLeftChild( BinTreeNodeAVL<Val>& new_child ) {
    BinTreeNode<Val>::insertLeftChild( new_child );
  }

  template <typename Val>
  INLINE BinTreeNodeAVL<Val>*
  BinTreeNodeAVL<Val>::insertRightChild( const Val& val ) {
    BinTreeNodeAVL<Val>* new_child = new BinTreeNodeAVL<Val>( val );
    BinTreeNode<Val>::insertRightChild( *new_child );
    return new_child;
  }

  template <typename Val>
  INLINE void
  BinTreeNodeAVL<Val>::insertRightChild( BinTreeNodeAVL<Val>& new_child ) {
    BinTreeNode<Val>::insertRightChild( new_child );
  }

  template <typename Val>
  INLINE BinTreeNodeAVL<Val>*
  BinTreeNodeAVL<Val>::insertChild( const Val& val, BinTreeDir child_dir ) {
    BinTreeNodeAVL<Val>* new_child = new BinTreeNodeAVL<Val>( val );
    BinTreeNode<Val>::insertChild( *new_child, child_dir );
    return new_child;
  }

  template <typename Val>
  INLINE void BinTreeNodeAVL<Val>::insertChild( BinTreeNodeAVL<Val>& new_child,
                                                 BinTreeDir child_dir ) {
    BinTreeNode<Val>::insertChild( new_child, child_dir );
  }

  template <typename Val>
  INLINE void BinTreeNodeAVL<Val>::eraseLeftLink() {
    BinTreeNode<Val>::eraseLeftLink();
  }

  template <typename Val>
  INLINE void BinTreeNodeAVL<Val>::eraseRightLink() {
    BinTreeNode<Val>::eraseRightLink();
  }

  template <typename Val>
  INLINE void BinTreeNodeAVL<Val>::eraseLink( BinTreeDir tree_dir ) {
    BinTreeNode<Val>::eraseLink( tree_dir );
  }

  template <typename Val>
  INLINE BinTreeNodeAVL<Val>* BinTreeNodeAVL<Val>::leftmostNode() const {
    return static_cast<BinTreeNodeAVL<Val>*>( BinTreeNode<Val>::leftmostNode() );
  }

  template <typename Val>
  INLINE BinTreeNodeAVL<Val>* BinTreeNodeAVL<Val>::rightmostNode() const {
    return static_cast<BinTreeNodeAVL<Val>*>( BinTreeNode<Val>::rightmostNode() );
  }

  template <typename Val>
  INLINE BinTreeNodeAVL<Val>* BinTreeNodeAVL<Val>::root() const {
    return static_cast<BinTreeNodeAVL<Val>*>( BinTreeNode<Val>::root() );
  }

}  // namespace gum
