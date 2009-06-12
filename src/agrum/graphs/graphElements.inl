/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN and Christophe GONZALES  *
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
 * @brief Inlined implementation of directed and undirected edges
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#include <iostream>
#include <algorithm>
#include <agrum/core/debug.h>
#include <agrum/core/exceptions.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {


  /* =========================================================================== */
  /* =========================================================================== */
  /* ===                      GENERIC LINKS IMPLEMENTATION                   === */
  /* =========================================================================== */
  /* =========================================================================== */

  // ==============================================================================
  /// basic constructor
  // ==============================================================================
  INLINE Link::Link( const NodeId& aN1,const NodeId& aN2 )  :
    n1( aN1 ), n2( aN2 ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( Link );
  }

  // ==============================================================================
  /// copy constructor
  // ==============================================================================
  INLINE Link::Link( const Link& from )  :
    n1( from.n1 ), n2( from.n2 ) {
    // for debugging purposes
    GUM_CONS_CPY( Link );
  }

  // ==============================================================================
  /// Copy operator.
  // ==============================================================================
  INLINE Link& Link::operator= ( const Link& from )  {
    // for debugging purposes
    GUM_OP_CPY( Link );
    n1 = from.n1;
    n2 = from.n2;
    return *this;
  }

  // ==============================================================================
  /// destructor
  // ==============================================================================
  INLINE Link::~Link() {
    // for debugging purposes
    GUM_DESTRUCTOR( Link );
  }

  // ============================================================================
  /// returns an extremal node of an edge given the ID of the other one
  // ============================================================================
  INLINE const NodeId& Link::other( const NodeId& id ) const {
    if ( id == n1 ) return n2;
    else if ( id == n2 ) return n1;
    else GUM_ERROR( IdError, "" );
  }

  // ==============================================================================
  /// returns one extremal node ID (whichever one it is is unspecified)
  // ==============================================================================
  INLINE const NodeId& Link::first() const  {
    return n1;
  }

  // ==============================================================================
  /// modifies one extremal node ID (whichever one it is is unspecified)
  // ==============================================================================
  INLINE void Link::__setFirst( const NodeId& id )  {
    n1 = id;
  }

  // ==============================================================================
  /// returns the second extremal node
  // ==============================================================================
  INLINE const NodeId& Link::second() const  {
    return n2;
  }

  // ==============================================================================
  /// modifies the second extremal node
  // ==============================================================================
  INLINE void Link::__setSecond( const NodeId& id )  {
    n2 = id;
  }

  // ============================================================================
  /// check if two basic edges are equal
  // ============================================================================
  INLINE bool Link::operator==( const Link& from ) const  {
    return (( isDirected() == from.isDirected() ) &&
            ( n1 == from.n1 ) && ( n2 == from.n2 ) );
  }

  // ============================================================================
  /// check if two basic edges are different
  // ============================================================================
  INLINE bool Link::operator!=( const Link& from ) const  {
    return (( isDirected() != from.isDirected() ) ||
            ( n1 != from.n1 ) || ( n2 != from.n2 ) );
  }




  
  /* =========================================================================== */
  /* =========================================================================== */
  /* ===                GENERIC UNDIRECTED EDGES IMPLEMENTATION              === */
  /* =========================================================================== */
  /* =========================================================================== */

  // ==============================================================================
  /// basic constructor
  // ==============================================================================
  INLINE Edge::Edge( const NodeId& aN1, const NodeId& aN2 )  :
    Link( std::min( aN1,aN2 ),std::max( aN1,aN2 ) ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( Edge );
  }

  // ==============================================================================
  /// copy constructor
  // ==============================================================================
  INLINE Edge::Edge( const Edge& from )  : Link( from ) {
    // for debugging purposes
    GUM_CONS_CPY( Edge );
  }

  // ==============================================================================
  /// copy operator
  // ==============================================================================
  INLINE Edge& Edge::operator= ( const Edge& from )  {
    // for debugging purposes
    GUM_OP_CPY( Edge );
    Link::operator= ( from );
    return *this;
  }

  // ==============================================================================
  /// destructor
  // ==============================================================================
  INLINE Edge::~Edge() {
    // for debugging purposes
    GUM_DESTRUCTOR( Edge );
  }

  // ==============================================================================
  /// returns a boolean indicating whether the edge is directed
  // ==============================================================================
  INLINE bool Edge::isDirected() const  {
    return false;
  }

  // ==============================================================================
  /// check if two undirected edges are equal
  // ==============================================================================
  INLINE bool Edge::operator== ( const Edge& from ) const  {
    return (( n1 == from.n1 ) && ( n2 == from.n2 ) );
  }

  // ==============================================================================
  /// check if two undirected edges are different
  // ==============================================================================
  INLINE bool Edge::operator!= ( const Edge& from ) const  {
    return (( n1 != from.n1 ) || ( n2 != from.n2 ) );
  }



  
  /* =========================================================================== */
  /* =========================================================================== */
  /* ===                 GENERIC DIRECTED EDGES IMPLEMENTATION               === */
  /* =========================================================================== */
  /* =========================================================================== */

  // ==============================================================================
  /// basic constructor.
  // ==============================================================================
  INLINE Arc::Arc( const NodeId& aN1,const NodeId& aN2 )  :
    Link( aN1,aN2 ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( Arc );
  }

  // ==============================================================================
  /// copy constructor
  // ==============================================================================
  INLINE Arc::Arc( const Arc& from )  : Link( from ) {
    // for debugging purposes
    GUM_CONS_CPY( Arc );
  }

  // ==============================================================================
  /// copy operator
  // ==============================================================================
  INLINE Arc& Arc::operator= ( const Arc& from )  {
    // for debugging purposes
    GUM_OP_CPY( Arc );
    Link::operator= ( from );
    return *this;
  }

  // ==============================================================================
  /// destructor
  // ==============================================================================
  INLINE Arc::~Arc() {
    // for debugging purposes
    GUM_DESTRUCTOR( Arc );
  }

  // ==============================================================================
  /// returns the tail of the arc
  // ==============================================================================
  INLINE const NodeId& Arc::tail() const  {
    return n1;
  }

  // ==============================================================================
  /// modifies the tail of the arc
  // ==============================================================================
  INLINE void Arc::__setTail( const NodeId& id )  {
    n1 = id;
  }

  // ==============================================================================
  /// returns the head of the arc
  // ==============================================================================
  INLINE const NodeId& Arc::head() const  {
    return n2;
  }

  // ==============================================================================
  /// modifies the head of the arc
  // ==============================================================================
  INLINE void Arc::__setHead( const NodeId& id )  {
    n2 = id;
  }

  // ==============================================================================
  /// returns a boolean indicating whether the arc is directed
  // ==============================================================================
  INLINE bool Arc::isDirected() const  {
    return true;
  }

  // ==============================================================================
  /// check if two arcs are equal
  // ==============================================================================
  INLINE bool Arc::operator== ( const Arc& from ) const  {
    return (( n1 == from.n1 ) && ( n2 == from.n2 ) );
  }

  // ==============================================================================
  /// check if two arcs are different
  // ==============================================================================
  INLINE bool Arc::operator!= ( const Arc& from ) const  {
    return (( n1 != from.n1 ) || ( n2 != from.n2 ) );
  }

  // ==============================================================================
  /// reverses the direction of the arc
  // ==============================================================================
  INLINE void Arc::operator- ()  {
    NodeId n_temp = n1;
    n1 = n2;
    n2 = n_temp;
  }





  

  ////////////////////////////////////////////////////////////////
  //we need to provide hash functions for some Edge and Arc

  // ==============================================================================
  /// returns a hashed key for hash tables the keys of which are represented
  /// by a Edge
  // ==============================================================================
  INLINE Size
  HashFunc<Edge>::operator()( const Edge& key ) const {
    pair.first  = key.first();
    pair.second = key.second();
    return real_func.operator()( pair );
  }

  // ==============================================================================
  /// update the hash function to take into account a resize of the hash table
  // ==============================================================================
  INLINE void
  HashFunc<Edge>::resize( Size new_size ) {
    real_func.resize( new_size );
  }

  // ==============================================================================
  /// returns a hashed key for hash tables the keys of which are represented
  /// by a Arc
  // ==============================================================================
  INLINE Size
  HashFunc<Arc>::operator()( const Arc& key ) const {
    pair.first  = key.first();
    pair.second = key.second();
    return real_func.operator()( pair );
  }

  // ==============================================================================
  /// update the hash function to take into account a resize of the hash table
  // ==============================================================================
  INLINE void
  HashFunc<Arc>::resize( Size new_size ) {
    real_func.resize( new_size );
  }


} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
