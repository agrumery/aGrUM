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
 * @brief Inline implementation of the base node set class for graphs
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */

// to ease parsing by IDE
#include <agrum/graphs/nodeGraphPart.h>

namespace gum {

  INLINE void NodeGraphPartIteratorSafe::__validate ( void ) {
    __valid = false;

    if ( __pos > __nodes->bound() ) {
      __pos = __nodes->bound();
    }

    while ( __pos < __nodes->bound() ) {
      if ( ! __nodes->__inHoles ( __pos ) ) {
        __valid = true;
        return;
      }

      ++__pos;
    }
  }

  INLINE NodeGraphPart& NodeGraphPart::operator= ( const NodeGraphPart& p ) {
    // avoid self assignment
    if ( this != &p ) {
      populateNodes ( p );
    }

    return *this;
  }

  INLINE NodeId NodeGraphPart::nextNodeId() const {
    NodeId next = 0;

    //return the first hole if holes exist
    if ( __holes && ( ! __holes->empty() ) )
      next = * ( __holes->beginSafe() );
    else// in other case
      next = __bound;

    return next;
  }

  // __holes is assumed to be not nullptr and id is assumed to be in __holes
  INLINE void NodeGraphPart::__eraseHole ( NodeId id ) {
    __holes->erase ( id );

    if ( __holes->empty() ) {
      __holes->resize ( __holes_size );
    }
  }

  // warning: do not try to use function insertNode ( const NodeId id ) within
  // function insertNode(): as both functions are virtual, this may create
  // bugs within the graphs hierarchy (i.e., virtual functions calling recursively
  // each other along the hierarchy) that are not easy to debug.
  INLINE  NodeId NodeGraphPart::insertNode() {
    NodeId newNode;

    //fill the first hole if holes exist
    if ( __holes && ( ! __holes->empty() ) ) {
      newNode = * ( __holes->beginSafe() );
      __eraseHole ( newNode );
    } else {
      newNode = __bound;
      ++__bound;
      __updateEndIteratorSafe();
    }

    GUM_EMIT1 ( onNodeAdded, newNode );

    return newNode;
  }

  INLINE Size NodeGraphPart::sizeNodes( ) const {
    return ( __holes ) ? ( __bound - __holes->size() ) : __bound;
  }

  INLINE Size NodeGraphPart::size( ) const {
    return sizeNodes();
  }

  INLINE bool NodeGraphPart::existsNode ( const NodeId node ) const {
    if ( node >= __bound )
      return false;

    return ( ! __inHoles ( node ) );
  }

  INLINE bool NodeGraphPart::exists ( const NodeId node ) const {
    return existsNode ( node );
  }

  INLINE void NodeGraphPart::eraseNode ( const NodeId node ) {
    if ( ! existsNode ( node ) ) return;

    __addHole ( node );

    GUM_EMIT1 ( onNodeDeleted, node );
  }

  INLINE bool NodeGraphPart::emptyNodes() const {
    return ( sizeNodes() == 0 );
  }

  INLINE bool NodeGraphPart::empty() const {
    return emptyNodes();
  }

  INLINE NodeId NodeGraphPart::bound( ) const {
    return __bound;
  }

  INLINE void NodeGraphPart::clearNodes() {
    __clearNodes();
  }

  // warning: clear is an alias for clearNodes but it should never be the case
  // that the code of clear is just a call to clearNodes: as both methods are
  // virtual, this could induce bugs within the graphs hierarchy (i.e., virtual
  // functions calling recursively each other along the hierarchy) that are not
  // easy to debug. Hence, the code of clearNodes should be duplicated here.
  INLINE void NodeGraphPart::clear() {
    __clearNodes();
  }

  INLINE NodeGraphPartIteratorSafe NodeGraphPart::beginSafe() const {
    NodeGraphPartIteratorSafe it ( this );
    it.__validate(); // stop the iterator at the first not-in-holes
    return it;
  }

  INLINE void NodeGraphPart::__updateEndIteratorSafe() {
    __endIteratorSafe.__setPos ( __bound );
  }

  INLINE const NodeGraphPartIteratorSafe& NodeGraphPart::endSafe() const {
    return __endIteratorSafe;
  }

  INLINE bool NodeGraphPart::operator== ( const NodeGraphPart& p ) const {
    if ( __bound != p.__bound )
      return false;

    if ( __holes )
      if ( p.__holes )
        return ( *__holes == *p.__holes );
      else
        return false;
    else if ( p.__holes )
      return false;

    return true;
  }

  INLINE bool NodeGraphPart::operator!= ( const NodeGraphPart& p ) const {
    return ! operator== ( p );
  }


  INLINE NodeSet NodeGraphPart::asNodeSet() const {
    NodeSet son ( sizeNodes() );

    if ( ! empty() ) {
      for ( NodeId n = 0; n < __bound; ++n ) {
        if ( ! __inHoles ( n ) )
          son.insert ( n );
      }
    }

    return son;
  }

  INLINE const NodeGraphPart& NodeGraphPart::nodes() const {
    return * ( static_cast<const NodeGraphPart*> ( this ) );
  }

  INLINE bool NodeGraphPart::__inHoles ( NodeId id ) const {
    return __holes && __holes->contains ( id );
  }

  /// @return the size of __holes
  INLINE Size NodeGraphPart::__sizeHoles() const {
    return __holes ? __holes->size() : ( Size ) 0;
  }




  //=================NODEGRAPHPARTITERATOR============================



  INLINE
  NodeGraphPartIteratorSafe::NodeGraphPartIteratorSafe ( const NodeGraphPart* nodes ) :
    __nodes ( nodes ), __pos ( 0 ), __valid ( false ) {

    GUM_CONNECT ( *const_cast<NodeGraphPart*> ( nodes ),
                  onNodeDeleted, *this, NodeGraphPartIteratorSafe::whenNodeDeleted );
    GUM_CONSTRUCTOR ( NodeGraphPartIteratorSafe );
  }

  INLINE
  NodeGraphPartIteratorSafe::NodeGraphPartIteratorSafe ( const NodeGraphPartIteratorSafe& it ) :
    __nodes ( it.__nodes ), __pos ( it.__pos ), __valid ( it.__valid ) {
    GUM_CONS_CPY ( NodeGraphPartIteratorSafe );
  }

  INLINE NodeGraphPartIteratorSafe::~NodeGraphPartIteratorSafe() {
    GUM_DESTRUCTOR ( NodeGraphPartIteratorSafe );
  }

  INLINE NodeGraphPartIteratorSafe&
  NodeGraphPartIteratorSafe::operator= ( const NodeGraphPartIteratorSafe& it ) {
    //  avoid self assignment
    if ( &it != this ) {
      __nodes = it.__nodes;
      __pos = it.__pos;
      __valid = it.__valid;

      Listener::operator= ( it );
      GUM_OP_CPY ( NodeGraphPartIteratorSafe );
    }

    return *this;
  }

  INLINE
  bool NodeGraphPartIteratorSafe::operator== ( const NodeGraphPartIteratorSafe& it ) const {
    if ( __pos != it.__pos ) return false;

    if ( __valid != it.__valid ) return false;

    if ( __nodes != it.__nodes ) return false;

    return true;
  }

  INLINE
  bool NodeGraphPartIteratorSafe::operator!= ( const NodeGraphPartIteratorSafe& it ) const {
    return ! ( operator== ( it ) );
  }

  INLINE NodeGraphPartIteratorSafe& NodeGraphPartIteratorSafe::operator++ ( void ) {
    ++__pos;
    __validate();
    return *this;
  }

  INLINE NodeId NodeGraphPartIteratorSafe::operator* ( void ) const {
    if ( !__valid ) {
      GUM_ERROR ( UndefinedIteratorValue, "This iterator is not valid !" );
    }

    return __pos;
  }

  // unsafe private method
  INLINE void NodeGraphPartIteratorSafe::__setPos ( NodeId id ) {
    __pos = id;

    if ( __pos >= __nodes->bound() ) {
      __pos = __nodes->bound();
      __valid = false;
    } else {
      __valid = __nodes->exists ( __pos );
    }
  }


} /* namespace gum */
