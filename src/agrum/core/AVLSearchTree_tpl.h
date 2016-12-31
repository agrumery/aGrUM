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
 * @brief AVL binary search trees
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

// to ease IDE parsers
#include <agrum/core/AVLSearchTree.h>

namespace gum {

  template <typename Val, class Cmp>
  INLINE AVLSearchTree<Val, Cmp>::AVLSearchTree( bool uniqueness_policy )
      : BinSearchTree<Val, Cmp, Node>( uniqueness_policy ) {
    GUM_CONSTRUCTOR( AVLSearchTree );
  }

  template <typename Val, class Cmp>
  INLINE
  AVLSearchTree<Val, Cmp>::AVLSearchTree( const AVLSearchTree<Val, Cmp>& from )
      : BinSearchTree<Val, Cmp, Node>( from ) {
    GUM_CONS_CPY( AVLSearchTree );
  }

  template <typename Val, class Cmp>
  INLINE AVLSearchTree<Val, Cmp>::~AVLSearchTree() {
    GUM_DESTRUCTOR( AVLSearchTree );
  }

  template <typename Val, class Cmp>
  INLINE AVLSearchTree<Val, Cmp>& AVLSearchTree<Val, Cmp>::
  operator=( const AVLSearchTree<Val, Cmp>& from ) {
    GUM_OP_CPY( AVLSearchTree );
    BinSearchTree<Val, Cmp, Node>::operator=( from );
    return *this;
  }

  template <typename Val, class Cmp>
  void AVLSearchTree<Val, Cmp>::_erase( BinTreeNodeAVL<Val>* node ) {
    if ( !node ) return;

    // update all the iterators pointing to node that they should point
    // elsewhere
    BinSearchTree<Val, Cmp, Node>::__updateEraseIterators( node );

    // update the number of elements contained in the tree
    --BinSearchTree<Val, Cmp, Node>::_nb_elements;

    // now remove the node from the tree:

    // if the node has no children, then just remove it
    if ( !node->leftChild() && !node->rightChild() ) {
      // if the node was the only one in the tree, then the tree becomes empty
      if ( !node->parent() ) {
        BinSearchTree<Val, Cmp, Node>::_root = 0;
        delete node;
      } else {
        // note that, when node has a parent, there is no need to remove the
        // link between node and this parent: this will be taken care of by
        // node's destructor.
        Node* parent = node->parent();
        delete node;
        Node* new_root = parent->__balance();

        if ( new_root ) BinSearchTree<Val, Cmp, Node>::_root = new_root;
      }
    }
    // if there is just a right child
    else if ( !node->leftChild() ) {
      // just relink the right child with the parent (if any)
      if ( !node->parent() ) {
        // in this case, no need to remove the link between "node" and its
        // child:
        // this will be taken care of by the destructor of "node"
        BinSearchTree<Val, Cmp, Node>::_root = node->rightChild();
        delete node;
      } else {
        Node *     parent = node->parent(), *child = node->rightChild();
        BinTreeDir dir = node->parentDir();
        parent->eraseLink( dir );
        node->eraseRightLink();
        parent->insertChild( *child, dir );
        delete node;
        Node* new_root = parent->__balance();

        if ( new_root ) BinSearchTree<Val, Cmp, Node>::_root = new_root;
      }
    }
    // if there is just a left child
    else if ( !node->rightChild() ) {
      // just relink the left child with the parent (if any)
      if ( !node->parent() ) {
        // in this case, no need to remove the link between "node" and its
        // child:
        // this will be taken care of by the destructor of "node"
        BinSearchTree<Val, Cmp, Node>::_root = node->leftChild();
        delete node;
      } else {
        Node *     parent = node->parent(), *child = node->leftChild();
        BinTreeDir dir = node->parentDir();
        parent->eraseLink( dir );
        node->eraseLeftLink();
        parent->insertChild( *child, dir );
        delete node;
        Node* new_root = parent->__balance();

        if ( new_root ) BinSearchTree<Val, Cmp, Node>::_root = new_root;
      }
    }
    // ok, here there are two children
    else {
      // the idea is to get the successor of "node" and substitute "node" by it.
      // As "node" has two children, we are sure that the successor is one of
      // node's descendants. Moreover, by its very definition, this successor
      // has
      // no left child. Hence, two cases can obtain:
      // 1/ the successor is precisely node's right child. In this case, we just
      //    have to make node's left child be the left child of the successor,
      //    and node's parent be the successor's parent, and the tree is again
      //    a binary search tree.
      // 2/ the successor is not node's right child. In this case, we know that
      //    the successor has a parent different from node and that the
      //    successor
      //    is a left child of this parent. We just need to put the right child
      //    of the successor (if any) as the left child of its parent, and to
      //    replace "node" by the successor.
      Node* successor = this->_succNode( node );

      if ( successor == node->rightChild() ) {  // proceed to case 1:
        Node* left_child = node->leftChild();
        node->eraseLeftLink();
        node->eraseRightLink();
        successor->insertLeftChild( *left_child );

        if ( node->parent() ) {
          // rechain node's parent with its successor
          BinTreeDir par_dir = node->parentDir();
          Node*      parent = node->parent();
          parent->eraseLink( par_dir );
          parent->insertChild( *successor, par_dir );
        }

        Node* new_root = successor->__balance();

        if ( new_root )
          BinSearchTree<Val, Cmp, Node>::_root = new_root;
        else if ( BinSearchTree<Val, Cmp, Node>::_root == node )
          BinSearchTree<Val, Cmp, Node>::_root = successor;
      } else {  // proceed to case 2:
        Node* parent = successor->parent();
        parent->eraseLeftLink();

        if ( successor->rightChild() ) {
          Node* succ_child = successor->rightChild();
          successor->eraseRightLink();
          parent->insertLeftChild( *succ_child );
        }

        Node *left = node->leftChild(), *right = node->rightChild();
        successor->__height = node->__height;
        node->eraseLeftLink();
        node->eraseRightLink();
        successor->insertLeftChild( *left );
        successor->insertRightChild( *right );

        if ( node->parent() ) {
          // rechain node's parent with its successor
          BinTreeDir par_dir = node->parentDir();
          Node*      the_parent = node->parent();
          the_parent->eraseLink( par_dir );
          the_parent->insertChild( *successor, par_dir );
        }

        Node* new_root = parent->__balance();

        if ( new_root )
          BinSearchTree<Val, Cmp, Node>::_root = new_root;
        else if ( BinSearchTree<Val, Cmp, Node>::_root == node )
          BinSearchTree<Val, Cmp, Node>::_root = successor;
      }

      // now we shall physically remove node from memory
      delete node;
    }
  }

  template <typename Val, class Cmp>
  bool AVLSearchTree<Val, Cmp>::checkAVLStructure() const {
    for ( iterator iter = begin(); iter != end(); ++iter ) {
      Node* node = iter._node;

      // compute the heights of the left and right trees
      unsigned int left_height =
          node->leftChild() ? node->leftChild()->__height : 0;
      unsigned int right_height =
          node->rightChild() ? node->rightChild()->__height : 0;

      if ( node->__height != 1 + std::max( left_height, right_height ) )
        return false;
    }

    return true;
  }

  template <typename Val, class Cmp>
  INLINE BinTreeNodeAVL<Val>* AVLSearchTree<Val, Cmp>::_insert( const Val& val ) {
    // insert the new value into the tree
    Node* new_node = BinSearchTree<Val, Cmp, Node>::_insert( val );

    // balance the tree and update its root
    if ( new_node->parent() ) {
      Node* new_root = new_node->parent()->__balance();

      if ( new_root ) BinSearchTree<Val, Cmp, Node>::_root = new_root;
    } else {
      BinSearchTree<Val, Cmp, Node>::_root = new_node;
    }

    return new_node;
  }

  template <typename Val, class Cmp>
  const std::string AVLSearchTree<Val, Cmp>::toString() const {
    std::stringstream stream;
    stream << "{ ";
    bool first = true;

    for ( iterator iter = begin(); iter != end(); ++iter ) {
      if ( !first )
        stream << ", ";
      else
        first = false;

      stream << *iter;
    }

    stream << " }";
    return stream.str();
  }

  template <typename Val, class Cmp>
  std::ostream& operator<<( std::ostream& stream,
                            const AVLSearchTree<Val, Cmp>& avl ) {
    stream << avl.toString();
    return stream;
  }
} /* namespace gum */
