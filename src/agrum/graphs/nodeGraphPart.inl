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
  INLINE NodeGraphPart& NodeGraphPart::operator= ( const NodeGraphPart& p ) {
    // avoid self assignment
    if ( this != &p ) {
      populateNodes( p );
    }

    return *this;
  }

  INLINE NodeId NodeGraphPart::__nextNodeId() {
    //fill the first hole if holes exist
    if ( __holes && ( ! __holes->empty() ) ) {
      NodeId id = *( __holes->begin() );
      __eraseHole( id );
      return id;
    }

    __max += 1;

    __updateEndIterator();
    return ( __max );
  }

  INLINE NodeId NodeGraphPart::nextNodeId() const {
    //return the first hole if holes exist
    if ( __holes && ( ! __holes->empty() ) )
      return *( __holes->begin() );

    // in other case
    return 1 + __max;
  }

// warning: do not try to use function insertNode ( const NodeId id ) within
// function insertNode(): as both functions are virtual, this may create
// bugs within the graphs hierarchy (i.e., virtual functions calling recursively
// each other along the hierarchy) that are not easy to debug.
  INLINE  NodeId NodeGraphPart::insertNode() {
    // only one tmp
    NodeId newNode = __nextNodeId();
    GUM_EMIT1( onNodeAdded, newNode );

    return newNode;
  }

  INLINE void NodeGraphPart::eraseNode( const NodeId node ) {
    if ( ! existsNode( node ) ) return;

    __addHole( node );

    GUM_EMIT1( onNodeDeleted, node );

  }

  INLINE bool NodeGraphPart::emptyNodes() const {

    return ( sizeNodes() == 0 );
  }

  INLINE bool NodeGraphPart::empty() const {
    return emptyNodes();
  }

  INLINE bool NodeGraphPart::existsNode( const NodeId node ) const {
    if ( node > __max )
      return false;

    return ( ! inHoles( node ) );
  }

  INLINE bool NodeGraphPart::exists( const NodeId node ) const {
    return existsNode( node );
  }

  INLINE Size NodeGraphPart::sizeNodes( ) const {
    return ( __holes ) ? ( __max - __holes->size() ) : __max;
  }

  INLINE Size NodeGraphPart::size( ) const {
    return sizeNodes();
  }

  INLINE NodeId NodeGraphPart::maxId( ) const {
    return __max;
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

  INLINE NodeGraphPartIterator NodeGraphPart::beginNodes() const {
    NodeGraphPartIterator it( this );
    ++it; // stop the iterator at the first not-in-holes
    return it;
  }

  INLINE void NodeGraphPart::__updateEndIterator() {
    __endIterator.__setPos( __max + 1 );
  }

  INLINE const NodeGraphPartIterator& NodeGraphPart::endNodes() const {
    return __endIterator;
  }

  INLINE bool NodeGraphPart::operator== ( const NodeGraphPart& p ) const {
    if ( __max != p.__max )
      return false;

    if ( __holes )
      if ( p.__holes )
        return ( *__holes == *p.__holes );
      else
        return false;
    else
      if ( p.__holes )
        return false;

    return true;
  }

  INLINE bool NodeGraphPart::operator!= ( const NodeGraphPart& p ) const {
    return ! operator==( p );
  }


  INLINE NodeSet NodeGraphPart::nodes() const {
    NodeSet son( sizeNodes() );

    if ( ! empty() ) {
      for ( NodeId n = 1;n <= __max;n++ ) {
        if ( inHoles( n ) ) continue;

        son.insert( n );
      }
    }

    return son;
  }

  // __holes is assumed to be not NULL and id is assumed to be in __holes
  INLINE void NodeGraphPart::__eraseHole( NodeId id ) {
    __holes->erase( id );

    if ( __holes->empty() ) {
      __holes->resize( __holes_size );
    }
  }

  INLINE bool NodeGraphPart::inHoles( NodeId id ) const {
    return __holes && __holes->contains( id );
  }

  /// @return the size of __holes
  INLINE Size NodeGraphPart::sizeHoles() const {
    return __holes ? __holes->size() : ( Size )0;
  }

//=================NODEGRAPHPARTITERATOR============================
  INLINE NodeGraphPartIterator::NodeGraphPartIterator( const NodeGraphPart* nodes ) : __nodes( nodes ), __pos( 0 ) {
    GUM_CONSTRUCTOR( NodeGraphPartIterator );
  }

  INLINE NodeGraphPartIterator::NodeGraphPartIterator( const NodeGraphPartIterator& it ): __nodes( it.__nodes ), __pos( it.__pos ) {
    GUM_CONS_CPY( NodeGraphPartIterator );
  }

  INLINE NodeGraphPartIterator::~NodeGraphPartIterator() {
    GUM_DESTRUCTOR( NodeGraphPartIterator );
  }

  INLINE NodeGraphPartIterator& NodeGraphPartIterator::operator=( const NodeGraphPartIterator& it ) {
    __nodes = it.__nodes;
    __pos = it.__pos;
    GUM_OP_CPY( NodeGraphPartIterator );

    return *this;
  }

  INLINE bool NodeGraphPartIterator::operator==( const NodeGraphPartIterator& it ) const {
    return (( __pos == it.__pos ) && ( __nodes == it.__nodes ) );
  }

  INLINE bool NodeGraphPartIterator::operator!=( const NodeGraphPartIterator& it ) const {
    return !( operator==( it ) );
  }

  INLINE NodeGraphPartIterator& NodeGraphPartIterator::operator++( void ) {
    __pos++;

    while ( __nodes->inHoles( __pos ) ) __pos++; // we are sure that at least inHoles(__nodes->__max+1) is false

    return *this;
  }

  INLINE NodeId NodeGraphPartIterator::operator*( void ) const {
    return __pos;
  }

// unsafe private method
  INLINE void NodeGraphPartIterator::__setPos( NodeId id ) {
    __pos = id;
  }
} /* namespace gum */
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;
