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
 * @brief Template implementation of basic graphs
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */


#include <agrum/gumDebug.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS


/* ============================================================================== */
/* ============================================================================== */
/* ===                    IMPLEMENTATION OF GUM_mixedGRAPH                   === */
/* ============================================================================== */
/* ============================================================================== */

// ================================================================================
/// basic constructor: creates an empty graph
// ================================================================================
template <typename Node, typename Edge, typename Arc>
gumMixedGraphTpl<Node,Edge,Arc>::gumMixedGraphTpl
( gumSize nodes_size, bool nodes_resize_policy,
  gumSize edges_size, bool edges_resize_policy,
  gumSize arcs_size,  bool arcs_resize_policy ) :
    nodes( nodes_size,nodes_resize_policy ), edges( edges_size,edges_resize_policy ),
    arcs( arcs_size,arcs_resize_policy ) {
  // for debugging purposes
  GUM_CONSTRUCTOR( gumMixedGraphTpl );
}


// ================================================================================
/// copy constructor (creates a completely distinct copy)
// ================================================================================
template <typename Node, typename Edge, typename Arc>
gumMixedGraphTpl<Node,Edge,Arc>::gumMixedGraphTpl
( const gumMixedGraphTpl<Node,Edge,Arc>& from, gumCopyType copy_type ) :
    nodes( from.nodes, copy_type ), edges( from.edges, copy_type ),
    arcs( from.arcs, copy_type ) {
  // for debugging purposes
  GUM_CONS_CPY( gumMixedGraphTpl );
}


// ================================================================================
/// copy operator (deep copy)
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
gumMixedGraphTpl<Node,Edge,Arc>& gumMixedGraphTpl<Node,Edge,Arc>::operator=
( const gumMixedGraphTpl<Node,Edge,Arc>& from ) {
  // avoid self assignment
  if ( this != &from ) {
    // for debugging purposes
    GUM_OP_CPY( gumMixedGraphTpl );

    nodes = from.nodes;
    edges = from.edges;
    arcs  = from.arcs;
  }

  return *this;
}


// ================================================================================
/// copy operator (nodes, arcs and edges are shared by the copy)
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
gumMixedGraphTpl<Node,Edge,Arc>& gumMixedGraphTpl<Node,Edge,Arc>::shallowCopy
( const gumMixedGraphTpl<Node,Edge,Arc>& from ) {
  // avoid self assignment
  if ( this != &from ) {
    nodes.shallowCopy( from.nodes );
    edges.shallowCopy( from.edges );
    arcs.shallowCopy( from.arcs );
  }

  return *this;
}


// ================================================================================
/// adds a shallow copy of a node into the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const gumRefPtr<Node>&
gumMixedGraphTpl<Node,Edge,Arc>::shallowInsertNode( const gumRefPtr<Node>& node ) {
  return nodes.shallowInsert( node );
}


// ================================================================================
/// returns a pointer to a given node (allows shallow copies)
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const gumRefPtr<Node>&
gumMixedGraphTpl<Node,Edge,Arc>::shallowGetNode( const gumId& id )
const  {
  return nodes.shallowGet( id );
}


// ================================================================================
/// returns a pointer to a given node (allows shallow copies)
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const gumRefPtr<Node>&
gumMixedGraphTpl<Node,Edge,Arc>::shallowGetNode( const Node& node )
const  {
  return nodes.shallowGet( node );
}


// ================================================================================
/// adds a shallow copy of an edge into the edge list
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const gumRefPtr<Arc>&
gumMixedGraphTpl<Node,Edge,Arc>::shallowinsertArc( const gumRefPtr<Arc>& arc ) {
  // check that the extremal nodes already belong to the graph
  if ( !existsNode( arc->tail() ) )
    GUM_ERROR( gumInvalidEdge,
               "the graph contains no node with id" + arc->tail() );

  if ( !existsNode( arc->head() ) )
    GUM_ERROR( gumInvalidEdge,
               "the graph contains no node with id" + arc->head() );

  return arcs.shallowInsert( arc );
}


// ================================================================================
/// returns a pointer to a given arc (allows shallow copies)
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const gumRefPtr<Arc>&
gumMixedGraphTpl<Node,Edge,Arc>::shallowGetArc( const gumId& node1, const gumId& node2 )
const  {
  return arcs.shallowGet( node1, node2 );
}


// ================================================================================
/// returns a pointer to a given arc (allows shallow copies)
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const gumRefPtr<Arc>&
gumMixedGraphTpl<Node,Edge,Arc>::shallowGetArc( const Arc& arc )
const  {
  return arcs.shallowGet( arc );
}


// ================================================================================
/// adds a shallow copy of an edge into the edge list
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const gumRefPtr<Edge>&
gumMixedGraphTpl<Node,Edge,Arc>::shallowInsertEdge( const gumRefPtr<Edge>& edge ) {
  if ( !existsNode( edge->first() ) )
    GUM_ERROR( gumInvalidEdge,
               "the graph contains no node with id" + edge->first() );

  if ( !existsNode( edge->second() ) )
    GUM_ERROR( gumInvalidEdge,
               "the graph contains no node with id" + edge->second() );

  return edges.shallowInsert( edge );
}


// ================================================================================
/// returns a smart pointer on the edge the extremities of which are node1,node2
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const gumRefPtr<Edge>&
gumMixedGraphTpl<Node,Edge,Arc>::shallowGetEdge( const gumId& node1, const gumId& node2 )
const  {
  return edges.shallowGet( node1,node2 );
}


// ================================================================================
/// returns a pointer to a given edge (allows shallow copies)
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const gumRefPtr<Edge>&
gumMixedGraphTpl<Node,Edge,Arc>::shallowGetEdge( const Edge& edge )
const  {
  return edges.shallowGet( edge );
}


// ================================================================================
/// destructor
// ================================================================================
template <typename Node, typename Edge, typename Arc>
gumMixedGraphTpl<Node,Edge,Arc>::~gumMixedGraphTpl() {
  // for debugging purposes
  GUM_DESTRUCTOR( gumMixedGraphTpl );
}


// ================================================================================
/// returns the size of the nodes hashtable
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const gumSize& gumMixedGraphTpl<Node,Edge,Arc>::capacityNodes() const  {
  return nodes.capacity();
}


// ==============================================================================
/// removes all the nodes (and their adjacent edges) from the graph
// ==============================================================================
template <typename Node, typename Edge, typename Arc> INLINE
void gumMixedGraphTpl<Node,Edge,Arc>::clearNodes() {
  edges.clear();
  arcs.clear();
  nodes.clear();
}

// ================================================================================
/// returns the list of nodes of the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const gumNodeList<Node>& gumMixedGraphTpl<Node,Edge,Arc>::getNodes() const  {
  return nodes;
}


// ================================================================================
/// indicates whether the graph contains some nodes
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const bool gumMixedGraphTpl<Node,Edge,Arc>::emptyNodes() const  {
  return nodes.empty();
}


// ================================================================================
/// checks whether a given node belongs to the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const bool gumMixedGraphTpl<Node,Edge,Arc>::existsNode( const gumId& id )
const  {
  return nodes.exists( id );
}


// ================================================================================
/// checks whether a given node belongs to the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const bool gumMixedGraphTpl<Node,Edge,Arc>::existsNode( const Node& node )
const  {
  return nodes.exists( node.getID() );
}

// ================================================================================

/// enables the user to change dynamically the nodes list resizing policy
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
void gumMixedGraphTpl<Node,Edge,Arc>::setResizePolicyNodes( const bool new_policy ) {
  nodes.setResizePolicy( new_policy );
}


// ================================================================================
/// returns the current list of nodes resizing policy
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const bool gumMixedGraphTpl<Node,Edge,Arc>::getResizePolicyNodes() const  {
  return nodes.getResizePolicy();
}


// ================================================================================
/// changes the size of the nodes hashtable
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
void gumMixedGraphTpl<Node,Edge,Arc>::resizeNodes( gumSize new_size ) {
  nodes.resize( new_size );
}


// ================================================================================
/// returns the number of nodes in the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const gumSize& gumMixedGraphTpl<Node,Edge,Arc>::getNbrNodes() const  {
  return nodes.size();
}


// ================================================================================
/// adds a new node in the graph (actually only a copy is added to the graph)
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
Node& gumMixedGraphTpl<Node,Edge,Arc>::insertNode( const Node& node ) {
  return nodes.insert( node );
}


// ================================================================================
/// returns the node the ID of which is passed in argument
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
Node& gumMixedGraphTpl<Node,Edge,Arc>::getNode( const gumId& id ) {
  return nodes[id];
}


// ================================================================================
/// returns the node the ID of which is passed in argument
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const Node& gumMixedGraphTpl<Node,Edge,Arc>::getNode( const gumId& id )
const  {
  return nodes[id];
}


// ================================================================================
/// removes a node specified by its ID from the list of nodes
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
void gumMixedGraphTpl<Node,Edge,Arc>::eraseNode( const gumId& id ) {
  edges.eraseneighbours( id );
  arcs.eraseneighbours( id );
  nodes.erase( id );
}


// ================================================================================
/// removes a given node from the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
void gumMixedGraphTpl<Node,Edge,Arc>::eraseNode( const Node& node ) {
  nodes.erase( node.getID() );
}


// ================================================================================
/// creates a gumNodeList of cabbage from the list of nodes of the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc>
template <typename Cabbage> INLINE
gumNodeList<Cabbage> gumMixedGraphTpl<Node,Edge,Arc>::nodeListMapNodes
( Cabbage( *f )( Node ), gumSize size ) const {
  return nodes.map( f,size );
}


// ================================================================================
/// creates a gumNodeList of cabbage from the list of nodes of the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc>
template <typename Cabbage> INLINE
gumNodeList<Cabbage> gumMixedGraphTpl<Node,Edge,Arc>::nodeListMapNodes
( Cabbage( *f )( Node& ), gumSize size ) const {
  return nodes.map( f,size );
}


// ================================================================================
/// creates a gumNodeList of cabbage from the list of nodes of the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc>
template <typename Cabbage> INLINE
gumNodeList<Cabbage> gumMixedGraphTpl<Node,Edge,Arc>::nodeListMapNodes
( Cabbage( *f )( const Node& ), gumSize size ) const {
  return nodes.map( f,size );
}


// ================================================================================
/// creates a hashtable of cabbage from the list of nodes of the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc>
template <typename Cabbage> INLINE
gumHashTable<gumId,Cabbage> gumMixedGraphTpl<Node,Edge,Arc>::hashMapNodes
( Cabbage( *f )( Node ), gumSize size ) const {
  return nodes.hashMap( f,size );
}


// ================================================================================
/// creates a hashtable of cabbage from the list of nodes of the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc>
template <typename Cabbage> INLINE
gumHashTable<gumId,Cabbage> gumMixedGraphTpl<Node,Edge,Arc>::hashMapNodes
( Cabbage( *f )( Node& ), gumSize size ) const {
  return nodes.hashMap( f,size );
}


// ================================================================================
/// creates a hashtable of cabbage from the list of nodes of the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc>
template <typename Cabbage> INLINE
gumHashTable<gumId,Cabbage> gumMixedGraphTpl<Node,Edge,Arc>::hashMapNodes
( Cabbage( *f )( const Node& ), gumSize size ) const {
  return nodes.hashMap( f,size );
}


// ================================================================================
/// creates a hashtable of cabbage from the list of nodes of the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc>
template <typename Cabbage> INLINE
gumHashTable<gumId,Cabbage> gumMixedGraphTpl<Node,Edge,Arc>::hashMapNodes
( const Cabbage& val, gumSize size ) const {
  return nodes.hashMap( val,size );
}


// ================================================================================
/// a method to create a gumList of cabbage from the list of nodes of the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc>
template <typename Cabbage> INLINE
gumList<Cabbage> gumMixedGraphTpl<Node,Edge,Arc>::listMapNodes
( Cabbage( *f )( Node ) ) const {
  return nodes.listMap( f );
}


// ================================================================================
/// a method to create a gumList of cabbage from the list of nodes of the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc>
template <typename Cabbage> INLINE
gumList<Cabbage> gumMixedGraphTpl<Node,Edge,Arc>::listMapNodes
( Cabbage( *f )( Node& ) ) const {
  return nodes.listMap( f );
}


// ================================================================================
/// a method to create a gumList of cabbage from the list of nodes of the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc>
template <typename Cabbage> INLINE
gumList<Cabbage> gumMixedGraphTpl<Node,Edge,Arc>::listMapNodes
( Cabbage( *f )( const Node& ) ) const {
  return nodes.listMap( f );
}

// ================================================================================
/// a method to create a gumList of cabbage from the list of nodes of the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc>
template <typename Cabbage> INLINE
gumList<Cabbage> gumMixedGraphTpl<Node,Edge,Arc>::listMapNodes( const Cabbage& val )
const {
  return nodes.listMap( val );
}


// ================================================================================
/// returns the size of the edges hashtable
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const gumSize& gumMixedGraphTpl<Node,Edge,Arc>::capacityEdges()
const  {
  return edges.capacity();
}


// ================================================================================
/// removes all the edges from the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
void gumMixedGraphTpl<Node,Edge,Arc>::clearEdges() {
  edges.clear();
}


// ================================================================================
/// indicates whether the graph contains some edges
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const bool gumMixedGraphTpl<Node,Edge,Arc>::emptyEdges() const  {
  return edges.empty();
}


// ================================================================================
/// returns the list of edges of the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const gumEdgeList<Edge>& gumMixedGraphTpl<Node,Edge,Arc>::getEdges() const  {
  return edges;
}


// ================================================================================
/// checks whether a given edge exists
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const bool gumMixedGraphTpl<Node,Edge,Arc>::existsEdge
( const gumId& node1, const gumId& node2 ) const  {
  return edges.exists( node1, node2 );
}


// ================================================================================
/// checks whether a given edge exists
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const bool gumMixedGraphTpl<Node,Edge,Arc>::existsEdge
( const Edge& edge ) const  {
  return ( edges.exists( edge ) );
}


// ================================================================================
/// enables the user to change dynamically the edges list resizing policy.
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
void gumMixedGraphTpl<Node,Edge,Arc>::setResizePolicyEdges( const bool new_policy ) {
  edges.setResizePolicy( new_policy );
}


// ================================================================================
/// returns the current list of edges resizing policy.
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const bool gumMixedGraphTpl<Node,Edge,Arc>::getResizePolicyEdges() const  {
  return edges.getResizePolicy();
}


// ================================================================================
/// changes the size of the edges hashtable
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
void gumMixedGraphTpl<Node,Edge,Arc>::resizeEdges( gumSize new_size ) {
  edges.resize( new_size );
}


// ================================================================================
/// returns the number of edges in the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const gumSize& gumMixedGraphTpl<Node,Edge,Arc>::getNbrEdges() const  {
  return edges.size();
}


// ================================================================================
/// removes a given edge from the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
void gumMixedGraphTpl<Node,Edge,Arc>::eraseEdge( const gumId& id1, const gumId& id2 ) {
  edges.erase( id1, id2 );
}


// ================================================================================
/// removes a given edge from the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
void gumMixedGraphTpl<Node,Edge,Arc>::eraseEdge( const Edge &edge ) {
  edges.erase( edge );
}


// ================================================================================
/// returns a given edge
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const Edge& gumMixedGraphTpl<Node,Edge,Arc>::getEdge
( const gumId& node1, const gumId& node2 ) const  {
  return edges.get( node1, node2 );
}


// ================================================================================
/// returns a given edge
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
Edge& gumMixedGraphTpl<Node,Edge,Arc>::getEdge
( const gumId& node1, const gumId& node2 )  {
  return edges.get( node1, node2 );
}


// ================================================================================
/// adds an undirected edge to the graph (only a copy is added to the graph)
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
Edge& gumMixedGraphTpl<Node,Edge,Arc>::insertEdge( const Edge& edge ) {
  // check that the extremal nodes already belong to the graph
  if ( !existsNode( edge.first() ) )
    GUM_ERROR( gumInvalidEdge,
               "the graph contains no node with id" + edge.first() );

  if ( !existsNode( edge.second() ) )
    GUM_ERROR( gumInvalidEdge,
               "the graph contains no node with id" + edge.second() );

  // insert a copy of the edge into the graph
  return edges.insert( edge );
}


// ================================================================================
/// returns the list of the edges adjacent to a given node
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const gumList<gumRefPtr<Edge> >& gumMixedGraphTpl<Node,Edge,Arc>::getneighbourEdges
( const gumId& node ) const  {
  if ( existsNode( node ) )
    return edges.getneighbours( node );
  else
    GUM_ERROR( gumNotFound, "an undefined node has no neighbours" );
}


// ================================================================================
/// removes all the edges adjacent to the given node
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
void gumMixedGraphTpl<Node,Edge,Arc>::eraseneighbourEdges( const gumId& node ) {
  edges.eraseneighbours( node );
}


// ================================================================================
/// creates a hashtable of cabbage from the list of edges of the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc>
template <typename Cabbage> INLINE
gumHashTable<std::pair<gumId,gumId>,Cabbage>
gumMixedGraphTpl<Node,Edge,Arc>::hashMapEdges
( Cabbage( *f )( Edge ), gumSize size ) const {
  return edges.hashMap( f,size );
}


// ================================================================================
/// creates a hashtable of cabbage from the list of edges of the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc>
template <typename Cabbage> INLINE
gumHashTable<std::pair<gumId,gumId>,Cabbage>
gumMixedGraphTpl<Node,Edge,Arc>::hashMapEdges
( Cabbage( *f )( Edge& ), gumSize size ) const {
  return edges.hashMap( f,size );
}


// ================================================================================
/// creates a hashtable of cabbage from the list of edges of the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc>
template <typename Cabbage> INLINE
gumHashTable<std::pair<gumId,gumId>,Cabbage>
gumMixedGraphTpl<Node,Edge,Arc>::hashMapEdges
( Cabbage( *f )( const Edge& ), gumSize size ) const {
  return edges.hashMap( f,size );
}


// ================================================================================
/// creates a hashtable of cabbage from the list of edges of the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc>
template <typename Cabbage> INLINE
gumHashTable<std::pair<gumId,gumId>,Cabbage>
gumMixedGraphTpl<Node,Edge,Arc>::hashMapEdges
( const Cabbage& val, gumSize size ) const {
  return edges.hashMap( val,size );
}


// ================================================================================
/// creates a list of cabbage from an edge list
// ================================================================================
template <typename Node, typename Edge, typename Arc>
template <typename Cabbage> INLINE gumList<Cabbage>
gumMixedGraphTpl<Node,Edge,Arc>::listMapEdges( Cabbage( *f )( Edge ) ) const {
  return edges.listMap( f );
}


// ================================================================================
/// creates a list of cabbage from an edge list
// ================================================================================
template <typename Node, typename Edge, typename Arc>
template <typename Cabbage> INLINE gumList<Cabbage>
gumMixedGraphTpl<Node,Edge,Arc>::listMapEdges( Cabbage( *f )( Edge& ) ) const {
  return edges.listMap( f );
}


// ================================================================================
/// creates a list of cabbage from an edge list
// ================================================================================
template <typename Node, typename Edge, typename Arc>
template <typename Cabbage> INLINE gumList<Cabbage>
gumMixedGraphTpl<Node,Edge,Arc>::listMapEdges( Cabbage( *f )( const Edge& ) ) const {
  return edges.listMap( f );
}


// ================================================================================
/// creates a list of cabbage from an edge list
// ================================================================================
template <typename Node, typename Edge, typename Arc>
template <typename Cabbage> INLINE gumList<Cabbage>
gumMixedGraphTpl<Node,Edge,Arc>::listMapEdges( const Cabbage& val ) const {
  return edges.listMap( val );
}


// ================================================================================
/// creates a gumEdgeList of Mount from the list of edges of the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc>
template <typename Mount> INLINE gumEdgeList<Mount>
gumMixedGraphTpl<Node,Edge,Arc>::edgeListMapEdges
( Mount( *f )( Edge ), gumSize size ) const {
  return edges.map( f,size );
}


// ================================================================================
/// creates a gumEdgeList of Mount from the list of edges of the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc>
template <typename Mount> INLINE gumEdgeList<Mount>
gumMixedGraphTpl<Node,Edge,Arc>::edgeListMapEdges
( Mount( *f )( Edge& ), gumSize size ) const {
  return edges.map( f,size );
}


// ================================================================================
/// creates a gumEdgeList of Mount from the list of edges of the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc>
template <typename Mount> INLINE gumEdgeList<Mount>
gumMixedGraphTpl<Node,Edge,Arc>::edgeListMapEdges
( Mount( *f )( const Edge& ), gumSize size ) const {
  return edges.map( f,size );
}


// ================================================================================
/// returns the size of the arcs hashtable
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const gumSize& gumMixedGraphTpl<Node,Edge,Arc>::capacityArcs()
const  {
  return arcs.capacity();
}


// ================================================================================
/// removes all the arcs from the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
void gumMixedGraphTpl<Node,Edge,Arc>::clearArcs() {
  arcs.clear();
}


// ================================================================================
/// returns the list of arcs of the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const gumArcList<Arc>& gumMixedGraphTpl<Node,Edge,Arc>::getArcs() const  {
  return arcs;
}


// ================================================================================
/// indicates whether the graph contains some arcs, i.e., ordered edges
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const bool gumMixedGraphTpl<Node,Edge,Arc>::emptyArcs() const  {
  return arcs.empty();
}


// ================================================================================
/// checks whether a given arc exists
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const bool gumMixedGraphTpl<Node,Edge,Arc>::existsArc
( const gumId& node1, const gumId& node2 ) const  {
  return arcs.exists( node1, node2 );
}


// ================================================================================
/// checks whether a given arc exists
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const bool gumMixedGraphTpl<Node,Edge,Arc>::existsArc
( const Arc& arc ) const  {
  return ( arcs.exists( arc ) );
}


// ================================================================================
/// enables the user to change dynamically the arcs list resizing policy.
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
void gumMixedGraphTpl<Node,Edge,Arc>::setResizePolicyArcs( const bool new_policy ) {
  arcs.setResizePolicy( new_policy );
}


// ================================================================================
/// returns the current list of edges resizing policy.
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const bool gumMixedGraphTpl<Node,Edge,Arc>::getResizePolicyArcs() const  {
  return arcs.getResizePolicy();
}


// ================================================================================
/// changes the size of the arcs hashtable
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
void gumMixedGraphTpl<Node,Edge,Arc>::resizeArcs( gumSize new_size ) {
  arcs.resize( new_size );
}


// ================================================================================
/// returns the number of arcs in the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const gumSize& gumMixedGraphTpl<Node,Edge,Arc>::getNbrArcs() const  {
  return arcs.size();
}


// ================================================================================
/// removes a given arc from the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
void gumMixedGraphTpl<Node,Edge,Arc>::eraseArc( const gumId& id1, const gumId& id2 ) {
  arcs.erase( id1, id2 );
}


// ================================================================================
/// removes a given arc from the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
void gumMixedGraphTpl<Node,Edge,Arc>::eraseArc( const Arc &arc ) {
  arcs.erase( arc );
}


// ================================================================================
/// returns a given arc
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const Arc& gumMixedGraphTpl<Node,Edge,Arc>::getArc
( const gumId& node1, const gumId& node2 ) const  {
  return arcs.get( node1, node2 );
}


// ================================================================================
/// returns a given arc
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
Arc& gumMixedGraphTpl<Node,Edge,Arc>::getArc
( const gumId& node1, const gumId& node2 )  {
  return arcs.get( node1, node2 );
}


// ================================================================================
/// adds an directed edge to the graph (only a copy is added to the graph)
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
Arc& gumMixedGraphTpl<Node,Edge,Arc>::insertArc( const Arc& arc ) {
  // check that the extremal nodes already belong to the graph
  if ( !existsNode( arc.tail() ) )
    GUM_ERROR( gumInvalidEdge,
               "the graph contains no node with id" + arc.tail() );

  if ( !existsNode( arc.head() ) )
    GUM_ERROR( gumInvalidEdge,
               "the graph contains no node with id" + arc.head() );

  // insert a copy of the edge into the graph
  return arcs.insert( arc );
}


// ================================================================================
/// removes all the arcs adjacent to the given node
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
void gumMixedGraphTpl<Node,Edge,Arc>::eraseneighbourArcs( const gumId& node ) {
  arcs.eraseneighbours( node );
}


// ================================================================================
/// removes all the arcs outgoing from the given node
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
void gumMixedGraphTpl<Node,Edge,Arc>::eraseChildrenArcs( const gumId& node ) {
  arcs.eraseChildren( node );
}


// ================================================================================
/// removes all the arcs ingoing into the given node
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
void gumMixedGraphTpl<Node,Edge,Arc>::eraseParentArcs( const gumId& node ) {
  arcs.eraseParents( node );
}


// ================================================================================
/// returns the list of the arcs out-going from a given node
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const gumList<gumRefPtr<Arc> >& gumMixedGraphTpl<Node,Edge,Arc>::getChildrenArcs( const gumId& node ) const  {
  if ( existsNode( node ) )
    return arcs.getChildren( node );
  else
    GUM_ERROR( gumNotFound, "an undefined node has no children" );
}


// ================================================================================
/// returns the list of the arcs in-going to a given node
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const gumList<gumRefPtr<Arc> >& gumMixedGraphTpl<Node,Edge,Arc>::getParentArcs
( const gumId& node ) const  {
  if ( existsNode( node ) )
    return arcs.getParents( node );
  else
    GUM_ERROR( gumNotFound, "an undefined node has no children" );
}


// ================================================================================
/// creates a hashtable of cabbage from the arcs of the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc>
template <typename Cabbage> INLINE gumHashTable<std::pair<gumId,gumId>,Cabbage>
gumMixedGraphTpl<Node,Edge,Arc>::hashMapArcs
( Cabbage( *f )( Arc ), gumSize size ) const {
  return arcs.hashMap( f,size );
}


// ================================================================================
/// creates a hashtable of cabbage from the arcs of the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc>
template <typename Cabbage> INLINE gumHashTable<std::pair<gumId,gumId>,Cabbage>
gumMixedGraphTpl<Node,Edge,Arc>::hashMapArcs
( Cabbage( *f )( Arc& ), gumSize size ) const {
  return arcs.hashMap( f,size );
}


// ================================================================================
/// creates a hashtable of cabbage from the arcs of the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc>
template <typename Cabbage> INLINE gumHashTable<std::pair<gumId,gumId>,Cabbage>
gumMixedGraphTpl<Node,Edge,Arc>::hashMapArcs
( Cabbage( *f )( const Arc& ), gumSize size ) const {
  return arcs.hashMap( f,size );
}


// ================================================================================
/// creates a hashtable of cabbage from the arcs of the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc>
template <typename Cabbage> INLINE gumHashTable<std::pair<gumId,gumId>,Cabbage>
gumMixedGraphTpl<Node,Edge,Arc>::hashMapArcs
( const Cabbage& val, gumSize size ) const {
  return arcs.hashMap( val,size );
}


// ================================================================================
/// creates a list of cabbage from an arc list
// ================================================================================
template <typename Node, typename Edge, typename Arc>
template <typename Cabbage> INLINE gumList<Cabbage>
gumMixedGraphTpl<Node,Edge,Arc>::listMapArcs( Cabbage( *f )( Arc ) ) const {
  return arcs.listMap( f );
}


// ================================================================================
/// creates a list of cabbage from an arc list
// ================================================================================
template <typename Node, typename Edge, typename Arc>
template <typename Cabbage> INLINE gumList<Cabbage>
gumMixedGraphTpl<Node,Edge,Arc>::listMapArcs( Cabbage( *f )( Arc& ) ) const {
  return arcs.listMap( f );
}


// ================================================================================
/// creates a list of cabbage from an arc list
// ================================================================================
template <typename Node, typename Edge, typename Arc>
template <typename Cabbage> INLINE gumList<Cabbage>
gumMixedGraphTpl<Node,Edge,Arc>::listMapArcs( Cabbage( *f )( const Arc& ) ) const {
  return arcs.listMap( f );
}


// ================================================================================
/// creates a list of cabbage from an arc list
// ================================================================================
template <typename Node, typename Edge, typename Arc>
template <typename Cabbage> INLINE gumList<Cabbage>
gumMixedGraphTpl<Node,Edge,Arc>::listMapArcs( const Cabbage& val ) const {
  return arcs.listMap( val );
}


// ================================================================================
/// creates a gumArcList of Mount from the list of arcs of the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc>
template <typename Mount> INLINE gumArcList<Mount>
gumMixedGraphTpl<Node,Edge,Arc>::arcListMapArcs
( Mount( *f )( Arc ), gumSize size ) const {
  return arcs.map( f,size );
}


// ================================================================================
/// creates a gumArcList of Mount from the list of arcs of the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc>
template <typename Mount> INLINE gumArcList<Mount>
gumMixedGraphTpl<Node,Edge,Arc>::arcListMapArcs
( Mount( *f )( Arc& ), gumSize size ) const {
  return arcs.map( f,size );
}


// ================================================================================
/// creates a gumArcList of Mount from the list of arcs of the graph
// ================================================================================
template <typename Node, typename Edge, typename Arc>
template <typename Mount> INLINE gumArcList<Mount>
gumMixedGraphTpl<Node,Edge,Arc>::arcListMapArcs
( Mount( *f )( const Arc& ), gumSize size ) const {
  return arcs.map( f,size );
}


// ================================================================================
/// returns an iterator pointing to the end of the list of nodes
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const gumNodeListIterator<Node>& gumMixedGraphTpl<Node,Edge,Arc>::endNodes()
const  {
  return nodes.end();
}


// ================================================================================
/// the iterator pointing to the rend (beginning) of the list of nodes
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const gumNodeListIterator<Node>& gumMixedGraphTpl<Node,Edge,Arc>::rendNodes()
const  {
  return nodes.rend();
}


// ================================================================================
/// the iterator pointing to the beginning of the list of nodes
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
gumNodeListIterator<Node> gumMixedGraphTpl<Node,Edge,Arc>::beginNodes() const {
  return nodes.begin();
}


// ================================================================================
/// the iterator pointing to the rbegin (the last element) of the list of nodes
// ===))===========================================================================
template <typename Node, typename Edge, typename Arc> INLINE
gumNodeListIterator<Node> gumMixedGraphTpl<Node,Edge,Arc>::rbeginNodes() const {
  return nodes.rbegin();
}


// ================================================================================
/// returns an iterator pointing to the end of the list of edges
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const gumEdgeListIterator<Edge>& gumMixedGraphTpl<Node,Edge,Arc>::endEdges()
const  {
  return edges.end();
}


// ================================================================================
/// the iterator pointing to the rend (beginning) of the list of edges
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const gumEdgeListIterator<Edge>& gumMixedGraphTpl<Node,Edge,Arc>::rendEdges()
const  {
  return edges.rend();
}


// ================================================================================
/// the iterator pointing to the beginning of the list of edges
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
gumEdgeListIterator<Edge> gumMixedGraphTpl<Node,Edge,Arc>::beginEdges() const {
  return edges.begin();
}


// ================================================================================
/// the iterator pointing to the rbegin (the last element) of the list of edges
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
gumEdgeListIterator<Edge> gumMixedGraphTpl<Node,Edge,Arc>::rbeginEdges() const {
  return edges.rbegin();
}


// ================================================================================
/// returns an iterator pointing to the first neighbour of a given node
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
gumEdgeListIterator<Edge> gumMixedGraphTpl<Node,Edge,Arc>::beginneighbourEdges
( const gumId& node ) const {
  return edges.beginneighbours( node );
}


// ================================================================================
/// returns an iterator pointing to the last neighbour of a given node */
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
gumEdgeListIterator<Edge> gumMixedGraphTpl<Node,Edge,Arc>::rbeginneighbourEdges
( const gumId& node ) const {
  return edges.rbeginneighbours( node );
}


// ================================================================================
/// returns an iterator pointing to the end of the list of arcs
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const gumArcListIterator<Arc>& gumMixedGraphTpl<Node,Edge,Arc>::endArcs()
const  {
  return arcs.end();
}


// ================================================================================
/// the iterator pointing to the rend (beginning) of the list of arcs
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const gumArcListIterator<Arc>& gumMixedGraphTpl<Node,Edge,Arc>::rendArcs()
const  {
  return arcs.rend();
}


// ================================================================================
/// the iterator pointing to the beginning of the list of arcs
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
gumArcListIterator<Arc> gumMixedGraphTpl<Node,Edge,Arc>::beginArcs() const {
  return arcs.begin();
}


// ================================================================================
/// the iterator pointing to the rbegin (the last element) of the list of arcs
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
gumArcListIterator<Arc> gumMixedGraphTpl<Node,Edge,Arc>::rbeginArcs() const {
  return arcs.rbegin();
}


// ================================================================================
/// returns an iterator pointing to the first parent of a given node
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
gumArcListIterator<Arc> gumMixedGraphTpl<Node,Edge,Arc>::beginParentArcs
( const gumId& node ) const {
  return arcs.beginParents( node );
}


// ================================================================================
/// returns an iterator pointing to the last parent of a given node
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
gumArcListIterator<Arc> gumMixedGraphTpl<Node,Edge,Arc>::rbeginParentArcs
( const gumId& node ) const {
  return arcs.rbeginParents( node );
}


// ================================================================================
/// returns an iterator pointing to the first child of a given node
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
gumArcListIterator<Arc> gumMixedGraphTpl<Node,Edge,Arc>::beginChildrenArcs
( const gumId& node ) const {
  return arcs.beginChildren( node );
}


// ================================================================================
/// returns an iterator pointing to the last parent of a given node
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
gumArcListIterator<Arc> gumMixedGraphTpl<Node,Edge,Arc>::rbeginChildrenArcs
( const gumId& node ) const {
  return arcs.rbeginChildren( node );
}


// ================================================================================
/// returns an iterator pointing to the first adjacent arc of a node
/** Actually, this corresponds to the first parent. */
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
gumArcListIterator<Arc> gumMixedGraphTpl<Node,Edge,Arc>::beginneighbourArcs
( const gumId& node ) const {
  return arcs.beginneighbours( node );
}


// ================================================================================
/// returns an iterator pointing to the last adjacent arc of a node
/** Actually, this corresponds to the last of its children */
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
gumArcListIterator<Arc> gumMixedGraphTpl<Node,Edge,Arc>::rbeginneighbourArcs
( const gumId& node ) const {
  return arcs.rbeginneighbours( node );
}


// ================================================================================
/// checks whether two graphs are identical
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const bool gumMixedGraphTpl<Node,Edge,Arc>::operator==
( const gumMixedGraphTpl<Node,Edge,Arc>& from ) const {
  return (( nodes == from.nodes ) && ( edges == from.edges ) && ( arcs == from.arcs ) );
}


// ================================================================================
/// checks whether two graphs are identical
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const bool gumMixedGraphTpl<Node,Edge,Arc>::shallowEqual
( const gumMixedGraphTpl<Node,Edge,Arc>& from ) const  {
  return ( nodes.shallowEqual( from.nodes ) &&
           edges.shallowEqual( from.edges ) &&
           arcs.shallowEqual( from.arcs ) );
}


// ================================================================================
/// checks whether two graphs are different
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const bool gumMixedGraphTpl<Node,Edge,Arc>::operator!=
( const gumMixedGraphTpl<Node,Edge,Arc>& from ) const {
  return (( nodes != from.nodes ) || ( edges != from.edges ) || ( arcs != from.arcs ) );
}


// ================================================================================
/// checks whether two graphs are different
// ================================================================================
template <typename Node, typename Edge, typename Arc> INLINE
const bool gumMixedGraphTpl<Node,Edge,Arc>::shallowDifferent
( const gumMixedGraphTpl<Node,Edge,Arc>& from ) const  {
  return ( nodes.shallowDifferent( from.nodes ) ||
           edges.shallowDifferent( from.edges ) ||
           arcs.shallowDifferent( from.arcs ) );
}


// ==============================================================================
/// a << operator for gumMixedGraphTpl
// ==============================================================================
template <typename Node, typename Edge, typename Arc>
std::ostream& operator<< ( std::ostream& stream,
                           const gumMixedGraphTpl<Node,Edge,Arc>& graph ) {
  stream << "nodes : "<<graph.getNodes()<<std::endl;
  stream << "edges : "<<graph.getEdges()<<std::endl;
  stream << "arcs : "<<graph.getArcs()<<std::endl;
  return stream;
}


// ================================================================================
/// function used by function hasCycle ()
// ================================================================================
template <typename Node, typename Edge, typename Arc>
const bool gumMixedGraphTpl<Node,Edge,Arc>::hasCycleMark
( gumHashTable<gumId,bool>& marks, const gumId& node, const gumId& from )
const  {
  // mark the node as seen
  marks[node] = true;

  // mark its neighbours
  gumId neigh;

  for ( gumEdgeListIterator<Edge> iter = edges.beginneighbourEdges( node );
        iter != edges.end(); ++iter ) {
    // get the neighbour
    neigh = ( *iter ).other( node );

    if ( neigh != from ) {
      // check if the neighbour has already been marked => cycle
      if ( marks[neigh] == true )
        return true;

      // okay, follow the neighbour
      if ( hasCycleMark( marks, neigh, node ) )
        return true;
    }
  }

  for ( gumArcListIterator<Arc> iter = arcs.beginneighbourArcs( node );
        iter != arcs.end(); ++iter ) {
    // get the neighbour
    neigh = ( *iter ).other( node );

    if ( neigh != from ) {
      // check if the neighbour has already been marked => cycle
      if ( marks[neigh] == true )
        return true;

      // okay, follow the neighbour
      if ( hasCycleMark( marks, neigh, node ) )
        return true;
    }
  }

  // we checked all the subgraph and found no cycle
  return false;
}


// ================================================================================
/// checks whether the graph contains cycles
// ================================================================================
template <typename Node, typename Edge, typename Arc>
const bool gumMixedGraphTpl<Node,Edge,Arc>::hasCycle() const  {
  // beginning of the algorithm: mark all nodes to 'false'
  gumHashTable<gumId,bool> nodes_marked = nodes.hashMap( false );

  // for all unmarked nodes, try to mark them and see if we reach
  // an already marked node (=> there is a cycle)

  for ( typename gumHashTable<gumId,bool>::iterator iter=nodes_marked.begin();
        iter != nodes_marked.end(); ++iter )
    if ( *iter == false ) {
      if ( hasCycleMark( nodes_marked, iter.key(), iter.key() ) )
        return true;
    }

  return false;
}






/* ============================================================================== */
/* ============================================================================== */
/* ===                     IMPLEMENTATION OF GUM_DI_GRAPH                     === */
/* ============================================================================== */
/* ============================================================================== */

// ================================================================================
/// basic constructor: creates an empty graph
// ================================================================================
template <typename Node, typename Arc>
gumDiGraphTpl<Node,Arc>::gumDiGraphTpl
( gumSize nodes_size, bool nodes_resize_policy,
  gumSize arcs_size,  bool arcs_resize_policy ) :
    gumMixedGraphTpl<Node,gumEdge,Arc> ( nodes_size, nodes_resize_policy, 0, true,
                                      arcs_size, arcs_resize_policy ) {
  // for debugging purposes
  GUM_CONSTRUCTOR( gumDiGraphTpl );
}


// ================================================================================
/// copy constructor (creates a completely distinct copy)
// ================================================================================
template <typename Node, typename Arc>
gumDiGraphTpl<Node,Arc>::gumDiGraphTpl( const gumDiGraphTpl<Node,Arc>& from,
                                  gumCopyType copy_type ) :
    gumMixedGraphTpl<Node,gumEdge,Arc> ( from,copy_type ) {
  // for debugging purposes
  GUM_CONS_CPY( gumDiGraphTpl );
}


// ================================================================================
/// copy operator (nodes, arcs and edges are shared by the copy)
// ================================================================================
template <typename Node, typename Arc>
gumDiGraphTpl<Node,Arc>& gumDiGraphTpl<Node,Arc>::operator=
( const gumDiGraphTpl<Node,Arc>& from ) {
  // avoid self assignment
  if ( this != &from ) {
    // for debugging purposes
    GUM_OP_CPY( gumDiGraphTpl );

    gumMixedGraphTpl<Node,gumEdge,Arc>::operator= ( from );
  }

  return *this;
}


// ================================================================================
/// copy operator (nodes, arcs and edges are shared by the copy)
// ================================================================================
template <typename Node, typename Arc>
gumDiGraphTpl<Node,Arc>& gumDiGraphTpl<Node,Arc>::shallowCopy
( const gumDiGraphTpl<Node,Arc>& from ) {
  // avoid self assignment
  if ( this != &from ) {
    // for debugging purposes
    GUM_OP_CPY( gumDiGraphTpl );

    gumMixedGraphTpl<Node,gumEdge,Arc>::shallowCopy( from );
  }

  return *this;
}


// ================================================================================
/// destructor
// ================================================================================
template <typename Node, typename Arc>
gumDiGraphTpl<Node,Arc>::~gumDiGraphTpl() {
  // for debugging purposes
  GUM_DESTRUCTOR( gumDiGraphTpl );
}


// ==============================================================================
/// a << operator for gumDiGraphTpl
// ==============================================================================
template <typename Node, typename Arc>
std::ostream& operator<< ( std::ostream& stream,
                           const gumDiGraphTpl<Node,Arc>& graph ) {
  stream << "nodes : "<<graph.getNodes()<<std::endl;
  stream << "arcs : "<<graph.getArcs()<<std::endl;

  return stream;
}


// ================================================================================
/// checks whether two graphs are identical
// ================================================================================
template <typename Node, typename Arc> INLINE
const bool gumDiGraphTpl<Node,Arc>::operator== ( const gumDiGraphTpl<Node,Arc>& from )
const {
  return (( gumMixedGraphTpl<Node,gumEdge,Arc>::nodes ==
            from.gumMixedGraphTpl<Node,gumEdge,Arc>::nodes ) &&
          ( gumMixedGraphTpl<Node,gumEdge,Arc>::arcs ==
            from.gumMixedGraphTpl<Node,gumEdge,Arc>::arcs ) );
}


// ================================================================================
/// checks whether two graphs are identical
// ================================================================================
template <typename Node, typename Arc> INLINE
const bool gumDiGraphTpl<Node,Arc>::shallowEqual( const gumDiGraphTpl<Node,Arc>& from )
const  {
  return ( gumMixedGraphTpl<Node,gumEdge,Arc>::nodes.shallowEqual
           ( from.gumMixedGraphTpl<Node,gumEdge,Arc>::nodes ) &&
           gumMixedGraphTpl<Node,gumEdge,Arc>::arcs.shallowEqual
           ( from.gumMixedGraphTpl<Node,gumEdge,Arc>::arcs ) );
}


// ================================================================================
/// checks whether two graphs are different
// ================================================================================
template <typename Node, typename Arc> INLINE
const bool gumDiGraphTpl<Node,Arc>::operator!= ( const gumDiGraphTpl<Node,Arc>& from )
const {
  return (( gumMixedGraphTpl<Node,gumEdge,Arc>::nodes !=
            from.gumMixedGraphTpl<Node,gumEdge,Arc>::nodes ) ||
          ( gumMixedGraphTpl<Node,gumEdge,Arc>::arcs !=
            from.gumMixedGraphTpl<Node,gumEdge,Arc>::arcs ) );
}


// ================================================================================
/// checks whether two graphs are different
// ================================================================================
template <typename Node, typename Arc> INLINE
const bool gumDiGraphTpl<Node,Arc>::shallowDifferent( const gumDiGraphTpl<Node,Arc>& from )
const  {
  return ( gumMixedGraphTpl<Node,gumEdge,Arc>::nodes.shallowDifferent
           ( from.gumMixedGraphTpl<Node,gumEdge,Arc>::nodes ) ||
           gumMixedGraphTpl<Node,gumEdge,Arc>::arcs.shallowDifferent
           ( from.gumMixedGraphTpl<Node,gumEdge,Arc>::arcs ) );
}


// ================================================================================
/// function used by function hasCircuit
// ================================================================================
template <typename Node, typename Arc>
const bool gumDiGraphTpl<Node,Arc>::hasCircuitMark
( gumHashTable<gumId,char>& marks, const gumId& node, const gumId& from )
const  {
  // mark the node as seen
  marks[node] = 1;

  // mark its neighbours
  gumId neigh;

  for ( gumArcListIterator<Arc> iter = arcs.beginChildren( node );
        iter != arcs.end(); ++iter ) {
    // get the neighbour
    neigh = ( *iter ).other( node );

    if ( neigh != from ) {
      // check if the neighbour has already been marked => cycle
      if ( marks[neigh] == 1 ) {
        marks[node] = 2;
        return true;
      }

      // okay, follow the neighbour
      if (( marks[neigh] != 2 ) && hasCircuitMark( marks, neigh, node ) ) {
        marks[node] = 2;
        return true;
      }
    }
  }

  // we checked all the subgraph and found no cycle
  marks[node] = 2;

  return false;
}


// ================================================================================
/// checks whether the graph contains directed cycles
// ================================================================================
template <typename Node, typename Arc>
const bool gumDiGraphTpl<Node,Arc>::hasCircuit() const  {
  // beginning of the algorithm: mark all nodes to 'false'
  gumHashTable<gumId,char> nodes_marked = nodes.hashMap( 0 );

  // for all unmarked nodes, try to mark them and see if we reach
  // an already marked node (=> there is a cycle)

  for ( typename gumHashTable<gumId,char>::iterator iter=nodes_marked.begin();
        iter != nodes_marked.end(); ++iter )
    if ( *iter == 0 ) {
      if ( hasCircuitMark( nodes_marked, iter.key(), iter.key() ) )
        return true;
    }

  return false;
}


// ================================================================================
/// checks whether the graph directed cycles passing through arc (x,y)
// ================================================================================
template <typename Node, typename Arc>
const bool gumDiGraphTpl<Node,Arc>::hasCircuit( const gumId& x, const gumId& y ) const {
  // beginning of the algorithm: mark all nodes to 'false'
  gumHashTable<gumId,char> nodes_marked = nodes.hashMap( '\0' );

  // mark the extremities of the arc

  try {
    nodes_marked[x] = 1;
    nodes_marked[y] = 1;
  } catch ( gumNotFound& ) {
    GUM_ERROR( gumIdError, "the arc is not properly defined" );
  }

  // parse the graph to check for directed cycles
  return ( hasCircuitMark( nodes_marked, y, y ) );
}


// ================================================================================
/// returns the mixed edges/arcs graph corresponding to the current graph
// ================================================================================
template <typename Node, typename Arc> INLINE
const gumMixedGraphTpl<Node,gumEdge,Arc>&
gumDiGraphTpl<Node,Arc>::getMixedGraph() const  {
  return *this;
}






/* ============================================================================== */
/* ============================================================================== */
/* ===                    IMPLEMENTATION OF GUM_UNDI_GRAPH                    === */
/* ============================================================================== */
/* ============================================================================== */

// ================================================================================
/// basic constructor: creates an empty graph
// ================================================================================
template <typename Node, typename Edge>
gumUndiGraphTpl<Node,Edge>::gumUndiGraphTpl
( gumSize nodes_size, bool nodes_resize_policy,
  gumSize edges_size,  bool edges_resize_policy ) :
    gumMixedGraphTpl<Node,Edge,gumArc> ( nodes_size, nodes_resize_policy, 0, true,
                                      edges_size, edges_resize_policy ) {
  // for debugging purposes
  GUM_CONSTRUCTOR( gumUndiGraphTpl );
}


// ================================================================================
/// copy constructor (creates a completely distinct copy)
// ================================================================================
template <typename Node, typename Edge>
gumUndiGraphTpl<Node,Edge>::gumUndiGraphTpl( const gumUndiGraphTpl<Node,Edge>& from,
                                       gumCopyType copy_type ) :
    gumMixedGraphTpl<Node,Edge,gumArc> ( from, copy_type ) {
  // for debugging purposes
  GUM_CONS_CPY( gumUndiGraphTpl );
}


// ================================================================================
/// copy operator (deep copy)
// ================================================================================
template <typename Node, typename Edge> INLINE
gumUndiGraphTpl<Node,Edge>& gumUndiGraphTpl<Node,Edge>::operator=
( const gumUndiGraphTpl<Node,Edge>& from ) {
  // avoid self assignment
  if ( this != &from ) {
    // for debugging purposes
    GUM_OP_CPY( gumUndiGraphTpl );

    gumMixedGraphTpl<Node,Edge,gumArc>::operator= ( from );
  }

  return *this;
}


// ================================================================================
/// copy operator (nodes and edges are shared by the copy)
// ================================================================================
template <typename Node, typename Edge> INLINE
gumUndiGraphTpl<Node,Edge>& gumUndiGraphTpl<Node,Edge>::shallowCopy
( const gumUndiGraphTpl<Node,Edge>& from ) {
  // avoid self assignment
  if ( this != &from ) {
    // for debugging purposes
    GUM_OP_CPY( gumUndiGraphTpl );

    gumMixedGraphTpl<Node,Edge,gumArc>::shallowCopy( from );
  }

  return *this;
}


// ================================================================================
/// destructor
// ================================================================================
template <typename Node, typename Edge>
gumUndiGraphTpl<Node,Edge>::~gumUndiGraphTpl() {
  // for debugging purposes
  GUM_DESTRUCTOR( gumUndiGraphTpl );
}

// ==============================================================================
/// a << operator for gumUndiGraphTpl
// ==============================================================================
template <typename Node, typename Edge>
std::ostream& operator<< ( std::ostream& stream,
                           const gumUndiGraphTpl<Node,Edge>& graph ) {
  stream << "nodes : "<<graph.getNodes()<<std::endl;
  stream << "edges : "<<graph.getEdges()<<std::endl;

  return stream;
}


// ================================================================================
/// checks whether two graphs are identical
// ================================================================================
template <typename Node, typename Edge> INLINE
const bool gumUndiGraphTpl<Node,Edge>::operator==
( const gumUndiGraphTpl<Node,Edge>& from ) const {
  return (( gumMixedGraphTpl<Node,Edge,gumArc>::nodes ==
            from.gumMixedGraphTpl<Node,Edge,gumArc>::nodes ) &&
          ( gumMixedGraphTpl<Node,Edge,gumArc>::edges ==
            from.gumMixedGraphTpl<Node,Edge,gumArc>::edges ) );
}


// ================================================================================
/// checks whether two graphs are identical
// ================================================================================
template <typename Node, typename Edge> INLINE
const bool gumUndiGraphTpl<Node,Edge>::shallowEqual
( const gumUndiGraphTpl<Node,Edge>& from ) const  {
  return ( gumMixedGraphTpl<Node,Edge,gumArc>::nodes.shallowEqual
           ( from.gumMixedGraphTpl<Node,Edge,gumArc>::nodes ) &&
           gumMixedGraphTpl<Node,Edge,gumArc>::edges.shallowEqual
           ( from.gumMixedGraphTpl<Node,Edge,gumArc>::edges ) );
}


// ================================================================================
/// checks whether two graphs are different
// ================================================================================
template <typename Node, typename Edge> INLINE
const bool gumUndiGraphTpl<Node,Edge>::operator!=
( const gumUndiGraphTpl<Node,Edge>& from ) const {
  return (( gumMixedGraphTpl<Node,Edge,gumArc>::nodes !=
            from.gumMixedGraphTpl<Node,Edge,gumArc>::nodes ) ||
          ( gumMixedGraphTpl<Node,Edge,gumArc>::edges !=
            from.gumMixedGraphTpl<Node,Edge,gumArc>::edges ) );
}


// ================================================================================
/// checks whether two graphs are different
// ================================================================================
template <typename Node, typename Edge> INLINE
const bool gumUndiGraphTpl<Node,Edge>::shallowDifferent
( const gumUndiGraphTpl<Node,Edge>& from ) const  {
  return ( gumMixedGraphTpl<Node,Edge,gumArc>::nodes.shallowDifferent
           ( from.gumMixedGraphTpl<Node,Edge,gumArc>::nodes ) ||
           gumMixedGraphTpl<Node,Edge,gumArc>::edges.shallowDifferent
           ( from.gumMixedGraphTpl<Node,Edge,gumArc>::edges ) );
}


// ================================================================================
/// returns the mixed edges/arcs graph corresponding to the current graph
// ================================================================================
template <typename Node, typename Edge> INLINE
const gumMixedGraphTpl<Node,Edge,gumArc>&
gumUndiGraphTpl<Node,Edge>::getMixedGraph() const  {
  return *this;
}






/* ============================================================================== */
/* ============================================================================== */
/* ===                        IMPLEMENTATION OF GUM_DAG                       === */
/* ============================================================================== */
/* ============================================================================== */

// ================================================================================
/// basic constructor: creates an empty DAG
// ================================================================================
template <typename Node, typename Arc> INLINE
gumDAGTpl<Node,Arc>::gumDAGTpl( gumSize nodes_size, bool nodes_resize_policy,
                          gumSize arcs_size,  bool arcs_resize_policy ) :
    gumDiGraphTpl<Node,Arc> ( nodes_size, nodes_resize_policy,
                           arcs_size, arcs_resize_policy ) {
  // for debugging purposes
  GUM_CONSTRUCTOR( gumDAGTpl );
}


// ================================================================================
/// copy constructor (creates a completely distinct copy)
// ================================================================================
template <typename Node, typename Arc> INLINE
gumDAGTpl<Node,Arc>::gumDAGTpl( const gumDAGTpl<Node,Arc>& from, gumCopyType copy_type ) :
    gumDiGraphTpl<Node,Arc> ( from, copy_type ) {
  // for debugging purposes
  GUM_CONS_CPY( gumDAGTpl );
}


// ================================================================================
/// copy operator (deep copy)
// ================================================================================
template <typename Node, typename Arc> INLINE
gumDAGTpl<Node,Arc>& gumDAGTpl<Node,Arc>::operator= ( const gumDAGTpl<Node,Arc>& from ) {
  // avoid self assignment
  if ( this != &from ) {
    // for debugging purposes
    GUM_OP_CPY( gumDAGTpl );

    gumDiGraphTpl<Node,Arc>::operator= ( from );
  }

  return *this;
}


// ================================================================================
/// copy operator (nodes and arcs are shared by the copy)
// ================================================================================
template <typename Node, typename Arc> INLINE
gumDAGTpl<Node,Arc>& gumDAGTpl<Node,Arc>::shallowCopy( const gumDAGTpl<Node,Arc>& from ) {
  // avoid self assignment
  if ( this != &from ) {
    gumDiGraphTpl<Node,Arc>::shallowCopy( from );
  }

  return *this;
}

// ================================================================================
/// destructor
// ================================================================================
template <typename Node, typename Arc>
gumDAGTpl<Node,Arc>::~gumDAGTpl() {
  // for debugging purposes
  GUM_DESTRUCTOR( gumDAGTpl );
}


// ==============================================================================
/// a << operator for gumDAGTpl
// ==============================================================================
template <typename Node, typename Arc>
std::ostream& operator<< ( std::ostream& stream, const gumDAGTpl<Node,Arc>& graph ) {
  return stream << ( gumDiGraphTpl<Node,Arc> ) graph;
}


// ================================================================================
/// adds a copy of \e arc to the list of arcs of the graph.
// ================================================================================
template <typename Node, typename Arc> INLINE
Arc& gumDAGTpl<Node,Arc>::insertArc( const Arc& arc ) {
  // check for the existence of directed cycles
  if ( hasCircuit( arc.tail(), arc.head() ) )
    GUM_ERROR( gumCircuit, "adding this arc would create a directed cycle" );

  return gumDiGraphTpl<Node,Arc>::insertArc( arc );
}


// ================================================================================
/// adds a shallow copy of an edge into the edge list
// ================================================================================
template <typename Node, typename Arc> INLINE
const gumRefPtr<Arc>&
gumDAGTpl<Node,Arc>::shallowinsertArc( const gumRefPtr<Arc>& arc ) {
  // check for the existence of directed cycles
  if ( hasCircuit( arc->tail(), arc->head() ) )
    GUM_ERROR( gumCircuit, "adding this arc would create a directed cycle" );

  return gumDiGraphTpl<Node,Arc>::shallowinsertArc( arc );
}



#endif /* DOXYGEN_SHOULD_SKIP_THIS */
