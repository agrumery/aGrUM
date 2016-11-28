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
 * @brief Node class for various binary search trees
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  template <typename Val>
  INLINE BinTreeNode<Val>::BinTreeNode( const Val& v )
      : _val( v )
      , _parent( 0 )
      , _parent_dir( BinTreeDir::NO_PARENT ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( BinTreeNode );

    // set the children
    _children[0] = nullptr;
    _children[1] = nullptr;
  }

  template <typename Val>
  INLINE BinTreeNode<Val>::BinTreeNode( const BinTreeNode<Val>& from )
      : _val( from._val )
      , _parent( 0 )
      , _parent_dir( BinTreeDir::NO_PARENT ) {
    // for debugging purposes
    GUM_CONS_CPY( BinTreeNode );

    // set the children
    _children[0] = nullptr;
    _children[1] = nullptr;
  }

  template <typename Val>
  INLINE BinTreeNode<Val>::~BinTreeNode() {
    // for debugging purposes
    GUM_DESTRUCTOR( BinTreeNode );

    // update the tree accordingly to the removal of this
    if ( _parent )
      _parent->_children[static_cast<int>( _parent_dir )] =
          nullptr;  // parent_dir can not be NO_PARENT (... sure ?)

    if ( _children[0] ) {
      _children[0]->_parent = nullptr;
      _children[0]->_parent_dir = BinTreeDir::NO_PARENT;
    }

    if ( _children[1] ) {
      _children[1]->_parent = nullptr;
      _children[1]->_parent_dir = BinTreeDir::NO_PARENT;
    }
  }

  /** @brief copy operator: copy the value of from into this. However, this
   * does not change the current connections (parents and children) of this. */

  template <typename Val>
  INLINE BinTreeNode<Val>& BinTreeNode<Val>::
  operator=( const BinTreeNode<Val>& from ) {
    // avoid self assignment
    if ( this != &from ) {
      GUM_OP_CPY( BinTreeNode );
      _val = from._val;
    }

    return *this;
  }

  template <typename Val>
  INLINE Val& BinTreeNode<Val>::value() {
    return _val;
  }

  template <typename Val>
  INLINE Val& BinTreeNode<Val>::operator*() {
    return _val;
  }

  template <typename Val>
  INLINE BinTreeNode<Val>* BinTreeNode<Val>::child( BinTreeDir dir ) const {
    return _children[static_cast<int>( dir )];
  }

  template <typename Val>
  INLINE BinTreeNode<Val>* BinTreeNode<Val>::leftChild() const {
    return _children[static_cast<int>( BinTreeDir::LEFT_CHILD )];
  }

  template <typename Val>
  INLINE BinTreeNode<Val>* BinTreeNode<Val>::rightChild() const {
    return _children[static_cast<int>( BinTreeDir::RIGHT_CHILD )];
  }

  template <typename Val>
  INLINE BinTreeNode<Val>* BinTreeNode<Val>::parent() const {
    return _parent;
  }

  template <typename Val>
  INLINE BinTreeDir BinTreeNode<Val>::parentDir() const {
    return _parent_dir;
  }

  template <typename Val>
  INLINE void BinTreeNode<Val>::insertLeftChild( BinTreeNode<Val>& new_child ) {
    if ( new_child._parent ) {
      GUM_ERROR( DuplicateElement, "this child has already a parent" );
    }

    if ( _children[static_cast<int>( BinTreeDir::LEFT_CHILD )] ) {
      GUM_ERROR( DuplicateElement, "this node has already a left child" );
    }

    // proceed to the chaining
    new_child._parent = this;
    new_child._parent_dir = BinTreeDir::LEFT_CHILD;
    _children[static_cast<int>( BinTreeDir::LEFT_CHILD )] = &new_child;
  }

  template <typename Val>
  INLINE BinTreeNode<Val>* BinTreeNode<Val>::insertLeftChild( const Val& val ) {
    if ( _children[static_cast<int>( BinTreeDir::LEFT_CHILD )] ) {
      GUM_ERROR( DuplicateElement, "this node has already a left child" );
    }

    BinTreeNode<Val>* new_child = new BinTreeNode<Val>( val );

    // proceed to the chaining
    new_child->_parent = this;
    new_child->_parent_dir = BinTreeDir::LEFT_CHILD;
    _children[static_cast<int>( BinTreeDir::LEFT_CHILD )] = new_child;

    return new_child;
  }

  template <typename Val>
  INLINE void BinTreeNode<Val>::insertRightChild( BinTreeNode<Val>& new_child ) {
    if ( new_child._parent ) {
      GUM_ERROR( DuplicateElement, "this child has already a parent" );
    }

    if ( _children[static_cast<int>( BinTreeDir::RIGHT_CHILD )] ) {
      GUM_ERROR( DuplicateElement, "this node has already a right child" );
    }

    // proceed to the chaining
    new_child._parent = this;
    new_child._parent_dir = BinTreeDir::RIGHT_CHILD;
    _children[static_cast<int>( BinTreeDir::RIGHT_CHILD )] = &new_child;
  }

  template <typename Val>
  INLINE BinTreeNode<Val>* BinTreeNode<Val>::insertRightChild( const Val& val ) {
    if ( _children[static_cast<int>( BinTreeDir::RIGHT_CHILD )] ) {
      GUM_ERROR( DuplicateElement, "this node has already a right child" );
    }

    BinTreeNode<Val>* new_child = new BinTreeNode<Val>( val );

    // proceed to the chaining
    new_child->_parent = this;
    new_child->_parent_dir = BinTreeDir::RIGHT_CHILD;
    _children[static_cast<int>( BinTreeDir::RIGHT_CHILD )] = new_child;

    return new_child;
  }

  template <typename Val>
  INLINE void BinTreeNode<Val>::insertChild( BinTreeNode<Val>& new_child,
                                             BinTreeDir        child_dir ) {
    if ( new_child._parent ) {
      GUM_ERROR( DuplicateElement, "this child has already a parent" );
    }

    if ( _children[static_cast<int>( child_dir )] ) {
      GUM_ERROR( DuplicateElement, "this node has already this child" );
    }

    // proceed to the chaining
    new_child._parent = this;
    new_child._parent_dir = child_dir;
    _children[static_cast<int>( child_dir )] = &new_child;
  }

  template <typename Val>
  INLINE BinTreeNode<Val>* BinTreeNode<Val>::insertChild( const Val& val,
                                                          BinTreeDir child_dir ) {
    if ( _children[static_cast<int>( child_dir )] ) {
      GUM_ERROR( DuplicateElement, "this node has already this child" );
    }

    BinTreeNode<Val>* new_child = new BinTreeNode<Val>( val );

    // proceed to the chaining
    new_child->_parent = this;
    new_child->_parent_dir = child_dir;
    _children[static_cast<int>( child_dir )] = new_child;

    return new_child;
  }

  template <typename Val>
  INLINE void BinTreeNode<Val>::eraseLeftLink() {
    if ( _children[static_cast<int>( BinTreeDir::LEFT_CHILD )] ) {
      _children[static_cast<int>( BinTreeDir::LEFT_CHILD )]->_parent = nullptr;
      _children[static_cast<int>( BinTreeDir::LEFT_CHILD )]->_parent_dir =
          BinTreeDir::NO_PARENT;
      _children[static_cast<int>( BinTreeDir::LEFT_CHILD )] = nullptr;
    }
  }

  template <typename Val>
  INLINE void BinTreeNode<Val>::eraseRightLink() {
    if ( _children[static_cast<int>( BinTreeDir::RIGHT_CHILD )] ) {
      _children[static_cast<int>( BinTreeDir::RIGHT_CHILD )]->_parent = nullptr;
      _children[static_cast<int>( BinTreeDir::RIGHT_CHILD )]->_parent_dir =
          BinTreeDir::NO_PARENT;
      _children[static_cast<int>( BinTreeDir::RIGHT_CHILD )] = nullptr;
    }
  }

  template <typename Val>
  INLINE void BinTreeNode<Val>::eraseLink( BinTreeDir tree_dir ) {
    if ( _children[static_cast<int>( tree_dir )] ) {
      _children[static_cast<int>( tree_dir )]->_parent = nullptr;
      _children[static_cast<int>( tree_dir )]->_parent_dir = BinTreeDir::NO_PARENT;
      _children[static_cast<int>( tree_dir )] = nullptr;
    }
  }

  template <typename Val>
  INLINE BinTreeNode<Val>* BinTreeNode<Val>::leftmostNode() const {
    BinTreeNode<Val>* node = const_cast<BinTreeNode<Val>*>( this );

    while ( node->_children[static_cast<int>( BinTreeDir::LEFT_CHILD )] )
      node = node->_children[static_cast<int>( BinTreeDir::LEFT_CHILD )];

    return node;
  }

  template <typename Val>
  INLINE BinTreeNode<Val>* BinTreeNode<Val>::rightmostNode() const {
    BinTreeNode<Val>* node = const_cast<BinTreeNode<Val>*>( this );

    while ( node->_children[static_cast<int>( BinTreeDir::RIGHT_CHILD )] )
      node = node->_children[static_cast<int>( BinTreeDir::RIGHT_CHILD )];

    return node;
  }

  template <typename Val>
  INLINE BinTreeNode<Val>* BinTreeNode<Val>::root() const {
    BinTreeNode<Val>* node = const_cast<BinTreeNode<Val>*>( this );

    while ( node->_parent )
      node = node->_parent;

    return node;
  }

}  // namespace gum

#endif  // DOXYGEN_SHOULD_SKIP_THIS
