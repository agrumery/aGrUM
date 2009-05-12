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
 * @brief Template implementation of lists of edges/arcs
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#include <utility>
#include <agrum/gumDebug.h>
#ifndef DOXYGEN_SHOULD_SKIP_THIS
/* ============================================================================== */
/* ============================================================================== */
/* ===                IMPLEMENTATION OF THE GUM_EDGE_LIST_BASE                === */
/* ============================================================================== */
/* ============================================================================== */
// ================================================================================
/// Default constructor
// ================================================================================
template <typename Edge>
gumEdgeListBase<Edge>::gumEdgeListBase( const gumSize nb_edges,
                                        bool resize_policy ) :
		edges( nb_edges, resize_policy ),
		iter_end( edges,GUM_EDGELIST_ITER_END ),
		iter_rend( edges,GUM_EDGELIST_ITER_REND ) {
	// for debugging purposes
	GUM_CONSTRUCTOR( gumEdgeListBase );
}

// ================================================================================
/// add a copy of \e edge to the list of edges
// ===============================================================================
template <typename Edge> INLINE
Edge& gumEdgeListBase<Edge>::insert( const Edge& edge ) {
	/* add the edge. Here we rely on hashtables to prevent adding multiple
	   edges with the same extremities. note that the gumRefPtr below, by
	   its very constructor, creates a copy of edge */
	return *( edges.insert( std::pair<gumId,gumId> ( edge.first(),
	                        edge.second() ),
	                        gumRefPtr<Edge>( new Edge( edge ) ) ) );
}

// ================================================================================
/// adds a shallow copy of an edge into the edge list
// ================================================================================
template <typename Edge> INLINE
const gumRefPtr<Edge>&
gumEdgeListBase<Edge>::shallowInsert( const gumRefPtr<Edge>& edge ) {
	return edges.insert( std::pair<gumId,gumId> ( edge->first(),
	                     edge->second() ), edge );
}

// ================================================================================
/// Copy constructor
// ================================================================================
template <typename Edge>
gumEdgeListBase<Edge>::gumEdgeListBase( const gumEdgeListBase<Edge>& from,
                                        gumCopyType copy_type ) :
		edges( 2, from.edges.getResizePolicy() ) {
	// for debugging purposes
	GUM_CONS_CPY( gumEdgeListBase );
	// checks whether we need to perform a shallow or a deep copy

	if ( copy_type == GUM_DEEP_COPY ) {
		edges.resize( from.edges.capacity() );
		// deep copy the content of from

		for ( gumHashTableIterator< std::pair<gumId,gumId>,gumRefPtr<Edge> >
		      iter = from.edges.begin(); iter != from.edges.end(); ++iter )
			edges.insert( iter.key(), gumRefPtr<Edge>( new Edge( **iter ) ) );
	} else
		edges = from.edges;

	// set properly end and rend
	iter_end.setIteratorEnd( edges,GUM_EDGELIST_ITER_END );

	iter_rend.setIteratorEnd( edges,GUM_EDGELIST_ITER_REND );
}

// ================================================================================
/// copy operator
// ================================================================================
template <typename Edge>
gumEdgeListBase<Edge>& gumEdgeListBase<Edge>::operator=
( const gumEdgeListBase<Edge>& from ) {
	// avoid self assignment
	if ( this != &from ) {
		// for debugging purposes
		GUM_OP_CPY( gumEdgeListBase );
		// remove the current content of the node list
		edges.clear();
		// deep copy the content of from

		try {
			edges.resize( from.edges.capacity() );
			edges.setResizePolicy( from.edges.getResizePolicy() );

			for ( gumHashTableIterator< std::pair<gumId,gumId>,gumRefPtr<Edge> >
			      iter = from.edges.begin(); iter != from.edges.end(); ++iter )
				edges.insert( iter.key(), gumRefPtr<Edge>( new Edge( **iter ) ) );
		} catch ( ... ) {
			edges.clear();
			iter_end.setIteratorEnd( edges, GUM_EDGELIST_ITER_END );
			iter_rend.setIteratorEnd( edges, GUM_EDGELIST_ITER_REND );
			throw;
		}

		// set properly end and rend
		iter_end.setIteratorEnd( edges, GUM_EDGELIST_ITER_END );

		iter_rend.setIteratorEnd( edges, GUM_EDGELIST_ITER_REND );
	}

	return *this;
}

// ================================================================================
/// copy operator (the new list shares its edges with \e from)
// ================================================================================
template <typename Edge>
gumEdgeListBase<Edge>& gumEdgeListBase<Edge>::shallowCopy
( const gumEdgeListBase<Edge>& from ) {
	// avoid self assignment
	if ( this != &from ) {
		// fill this with the content of from
		edges = from.edges;
		// set properly end and rend
		iter_end.setIteratorEnd( edges, GUM_EDGELIST_ITER_END );
		iter_rend.setIteratorEnd( edges, GUM_EDGELIST_ITER_REND );
	}

	return *this;
}

// ================================================================================
/// destructor
// ================================================================================
template <typename Edge>
gumEdgeListBase<Edge>::~gumEdgeListBase() {
	// for debugging purposes
	GUM_DESTRUCTOR( gumEdgeListBase );
}

// ================================================================================
/// returns an iterator pointing to the beginning of the list of edges
// ================================================================================
template <typename Edge> INLINE
gumEdgeListIterator<Edge> gumEdgeListBase<Edge>::begin() const {
	return iterator( edges, GUM_EDGELIST_ITER_BEGIN );
}

// ================================================================================
/// returns the size of the hashtables within the edge list
// ================================================================================
template <typename Edge> INLINE
const gumSize& gumEdgeListBase<Edge>::capacity() const  {
	return edges.capacity();
}

// ================================================================================
/// changes the size of the hashtables within the edge list
// ================================================================================
template <typename Edge> INLINE
void gumEdgeListBase<Edge>::resize( gumSize new_size ) {
	edges.resize( new_size );
}

// ================================================================================
/// changes the resizing policy of the hashtables contained in the edge list
// ================================================================================
template <typename Edge> INLINE
void gumEdgeListBase<Edge>::setResizePolicy( const bool new_policy )  {
	edges.setResizePolicy( new_policy );
}

// ================================================================================
/// returns the current resizing policy.
// ================================================================================
template <typename Edge> INLINE
const bool gumEdgeListBase<Edge>::getResizePolicy() const  {
	return edges.getResizePolicy();
}

// ================================================================================
/// returns an iterator pointing to the last element of the list of edges
// ================================================================================
template <typename Edge> INLINE
gumEdgeListIterator<Edge> gumEdgeListBase<Edge>::rbegin() const {
	return iterator( edges, GUM_EDGELIST_ITER_RBEGIN );
}

// ================================================================================
/// returns an iterator pointing just after the last element of the list of edges
// ================================================================================
template <typename Edge> INLINE
const gumEdgeListIterator<Edge>& gumEdgeListBase<Edge>::end() const  {
	return iter_end;
}

// ================================================================================
/// returns an iterator pointing just before the element of the list of edges
// ================================================================================
template <typename Edge> INLINE
const gumEdgeListIterator<Edge>& gumEdgeListBase<Edge>::rend() const  {
	return iter_rend;
}

// ==============================================================================
/// empty the edge list
// ==============================================================================
template <typename Edge> INLINE
void gumEdgeListBase<Edge>::clear() {
	edges.clear();
	iter_end.setIteratorEnd( edges, GUM_EDGELIST_ITER_END );
	iter_rend.setIteratorEnd( edges, GUM_EDGELIST_ITER_REND );
}

// ================================================================================
/// indicates whether the list is empty
// ================================================================================
template <typename Edge> INLINE
const bool gumEdgeListBase<Edge>::empty() const  {
	return edges.empty();
}

// ================================================================================
/// a << operator for gumEdgeListBase
// ================================================================================
template <typename Edge>
std::ostream& operator<< ( std::ostream& stream, const gumEdgeListBase<Edge>& list ) {
	for ( gumEdgeListBase<gumEdge>::iterator iter = list.begin();
	      iter != list.end(); ++iter )
		stream<< *iter<<" - ";

	return stream;
}

// ================================================================================
/// remove sa given edge from the list of edges
// ================================================================================
template <typename Edge> INLINE
void gumEdgeListBase<Edge>::erase( const Edge& edge ) {
	edges.erase( std::pair<gumId,gumId> ( edge.first(),edge.second() ) );
}

// ================================================================================
/// removes the edges adjacent to a node
// ================================================================================
template <typename Edge> INLINE
void gumEdgeListBase<Edge>::eraseneighbours( const gumId& node ) {
	for ( iterator iter = beginneighbours( node ); iter != end(); ++iter )
		erase( *iter );
}

// ================================================================================
/// returns the number of edges in the list
// ================================================================================
template <typename Edge> INLINE
const gumSize gumEdgeListBase<Edge>::size() const  {
	return edges.size();
}

// ================================================================================
/// creates a hashtable of Border from an edge list
// ================================================================================
template <typename Edge>
template <typename Border> gumHashTable<std::pair<gumId,gumId>,Border>
gumEdgeListBase<Edge>::hashMap( Border( *f )( Edge ), gumSize size ) const {
	// determine the proper size of the hashtable
	// by default, the size of the table is set so that the table does not take
	// too much space while allowing to add a few elements without resizing
	if ( size == 0 )
		size = std::max( 2UL, edges.size() / 2 );

	// create a new table
	gumHashTable<std::pair<gumId,gumId>,Border> table( size );

	// fill the new hash table
	for ( gumHashTableIterator<std::pair<gumId,gumId>,gumRefPtr<Edge> >
	      iter=edges.rbegin(); iter != edges.rend(); --iter ) {
		table.insert( iter.key(), f( **iter ) );
	}

	return table;
}

// ================================================================================
/// creates a hashtable of Border from an edge list
// ================================================================================
template <typename Edge>
template <typename Border> gumHashTable<std::pair<gumId,gumId>,Border>
gumEdgeListBase<Edge>::hashMap( Border( *f )( Edge& ), gumSize size ) const {
	// determine the proper size of the hashtable
	// by default, the size of the table is set so that the table does not take
	// too much space while allowing to add a few elements without resizing
	if ( size == 0 )
		size = std::max( 2UL, edges.size() / 2 );

	// create a new table
	gumHashTable<std::pair<gumId,gumId>,Border> table( size );

	// fill the new hash table
	for ( gumHashTableIterator<std::pair<gumId,gumId>,gumRefPtr<Edge> >
	      iter=edges.rbegin(); iter != edges.rend(); --iter ) {
		table.insert( iter.key(), f( **iter ) );
	}

	return table;
}

// ================================================================================
/// creates a hashtable of Border from an edge list
// ================================================================================
template <typename Edge>
template <typename Border> gumHashTable<std::pair<gumId,gumId>,Border>
gumEdgeListBase<Edge>::hashMap( Border( *f )( const Edge& ), gumSize size )
const {
	// determine the proper size of the hashtable
	// by default, the size of the table is set so that the table does not take
	// too much space while allowing to add a few elements without resizing
	if ( size == 0 )
		size = std::max( 2UL, edges.size() / 2 );

	// create a new table
	gumHashTable<std::pair<gumId,gumId>,Border> table( size );

	// fill the new hash table
	for ( gumHashTableIterator<std::pair<gumId,gumId>,gumRefPtr<Edge> >
	      iter=edges.rbegin(); iter != edges.rend(); --iter ) {
		table.insert( iter.key(), f( **iter ) );
	}

	return table;
}

// ================================================================================
/// creates a hashtable of Border from an edge list
// ================================================================================
template <typename Edge>
template <typename Border> gumHashTable<std::pair<gumId,gumId>,Border>
gumEdgeListBase<Edge>::hashMap( const Border& val, gumSize size ) const {
	// determine the proper size of the hashtable
	// by default, the size of the table is set so that the table does not take
	// too much space while allowing to add a few elements without resizing
	if ( size == 0 )
		size = std::max( 2UL, edges.size() / 2 );

	// create a new table
	gumHashTable<std::pair<gumId,gumId>,Border> table( size );

	// fill the new hash table
	for ( gumHashTableIterator<std::pair<gumId,gumId>,gumRefPtr<Edge> >
	      iter=edges.rbegin(); iter != edges.rend(); --iter ) {
		table.insert( iter.key(), val );
	}

	return table;
}

// ================================================================================
/// creates a list of Border from an edge list
// ================================================================================
template <typename Edge>
template <typename Border> gumList<Border>
gumEdgeListBase<Edge>::listMap( Border( *f )( Edge ) ) const {
	// create a new list
	gumList<Border> list;
	// fill the new list

	for ( gumHashTableIterator<std::pair<gumId,gumId>,gumRefPtr<Edge> >
	      iter = edges.begin(); iter != edges.end(); ++iter ) {
		list.pushBack( f( **iter ) );
	}

	return list;
}

// ================================================================================
/// creates a list of Border from an edge list
// ================================================================================
template <typename Edge>
template <typename Border> gumList<Border>
gumEdgeListBase<Edge>::listMap( Border( *f )( const Edge& ) ) const {
	// create a new list
	gumList<Border> list;
	// fill the new list

	for ( gumHashTableIterator<std::pair<gumId,gumId>,gumRefPtr<Edge> >
	      iter = edges.begin(); iter != edges.end(); ++iter ) {
		list.pushBack( f( **iter ) );
	}

	return list;
}

// ================================================================================
/// creates a list of Border from an edge list
// ================================================================================
template <typename Edge>
template <typename Border> gumList<Border>
gumEdgeListBase<Edge>::listMap( Border( *f )( Edge& ) ) const {
	// create a new list
	gumList<Border> list;
	// fill the new list

	for ( gumHashTableIterator<std::pair<gumId,gumId>,gumRefPtr<Edge> >
	      iter = edges.begin(); iter != edges.end(); ++iter ) {
		list.pushBack( f( **iter ) );
	}

	return list;
}

// ================================================================================
/// creates a list of Border from an edge list
// ================================================================================
template <typename Edge>
template <typename Border> gumList<Border>
gumEdgeListBase<Edge>::listMap( const Border& val ) const {
	// create a new list
	gumList<Border> list;
	// fill the new list
	unsigned int nb_elts = size();

	for ( gumSize i=0; i<nb_elts; ++i )
		list.pushBack( val );

	return list;
}

// ================================================================================
/// checks whether two lists of edges are similar
// ================================================================================
template <typename Edge>
const bool gumEdgeListBase<Edge>::operator== ( const gumEdgeListBase<Edge>& from )
const {
	// check if the two edge lists have the same number of elements
	if ( edges.size() != from.edges.size() )
		return false;

	// parse both lists to find different edges
	for ( gumHashTableIterator< std::pair<gumId,gumId>,gumRefPtr<Edge> >
	      iter = edges.begin(); iter != edges.end(); ++iter ) {

		try { if ( **iter != *( from.edges[iter.key()] ) ) return false; }
		catch ( gumNotFound& ) { return false; }
	}

	return true;
}

// ================================================================================
/// checks whether two lists of edges are similar
// ================================================================================
template <typename Edge>
const bool gumEdgeListBase<Edge>::shallowEqual( const gumEdgeListBase<Edge>& from )
const  {
	// check if the two edge lists have the same number of elements
	if ( edges.size() != from.edges.size() )
		return false;

	// parse both lists to find different edges
	for ( gumHashTableIterator< std::pair<gumId,gumId>,gumRefPtr<Edge> >
	      iter = edges.begin(); iter != edges.end(); ++iter ) {

		try { if ( *iter != from.edges[iter.key()] ) return false; }
		catch ( gumNotFound& ) { return false; }
	}

	return true;
}

// ================================================================================
/// checks whether two lists of edges contain different elements
// ================================================================================
template <typename Edge>
const bool gumEdgeListBase<Edge>::operator!= ( const gumEdgeListBase<Edge>& from )
const {
	// check if the two edge lists have the same number of elements
	if ( edges.size() != from.edges.size() )
		return true;

	// parse both lists to find different edges
	for ( gumHashTableIterator< std::pair<gumId,gumId>,gumRefPtr<Edge> >
	      iter = edges.begin(); iter != edges.end(); ++iter ) {

		try { if ( **iter != *( from.edges[iter.key()] ) ) return true; }
		catch ( gumNotFound& ) { return true; }
	}

	return false;
}

// ================================================================================
/// checks whether two lists of edges contain different elements
// ================================================================================
template <typename Edge>
const bool gumEdgeListBase<Edge>::shallowDifferent
( const gumEdgeListBase<Edge>& from ) const  {
	// check if the two edge lists have the same number of elements
	if ( edges.size() != from.edges.size() )
		return true;

	// parse both lists to find different edges
	for ( gumHashTableIterator< std::pair<gumId,gumId>,gumRefPtr<Edge> >
	      iter = edges.begin(); iter != edges.end(); ++iter ) {

		try { if ( *iter != from.edges[iter.key()] ) return true; }
		catch ( gumNotFound& ) { return true; }
	}

	return false;
}

/* ============================================================================== */
/* ============================================================================== */
/* ===                  IMPLEMENTATION OF THE GUM_EDGE_LIST                   === */
/* ============================================================================== */
/* ============================================================================== */
// ================================================================================
/// Default constructor
// ================================================================================
template <typename Edge>
gumEdgeList<Edge>::gumEdgeList( gumSize nb_edges, bool resize_policy ) :
		gumEdgeListBase<Edge>( nb_edges,resize_policy ),
		neighbours( nb_edges,resize_policy ),
		neighbours_iter_list( new gumListBase<gumListIterator< gumRefPtr<Edge> >*> ),
		empty_neighbours( neighbours_iter_list ) {
	// for debugging purposes
	GUM_CONSTRUCTOR( gumEdgeList );
}

// ================================================================================
/** @brief checks whether an undirected edge specified by its extremal
 * nodes does belong to the list of edges */
// ================================================================================
template <typename Edge> INLINE
const bool gumEdgeList<Edge>::exists( const gumId& node1, const gumId& node2 )
const  {
	// set the extremal nodes
	std::pair<gumId, gumId> edge( std::min( node1,node2 ), std::max( node1,node2 ) );
	// return whether the edge exists
	return ( gumEdgeListBase<Edge>::edges.exists( edge ) );
}

// ================================================================================
/// checks whether an undirected edge does belong to the list of edges
// ================================================================================
template <typename Edge> INLINE
const bool gumEdgeList<Edge>::exists( const Edge& edge ) const  {
	std::pair<gumId, gumId> extrems( edge.first(), edge.second() );
	return gumEdgeListBase<Edge>::edges.exists( extrems );
}

// ================================================================================
/// returns a pointer to a given edge (allows shallow copies)
// ================================================================================
template <typename Edge> INLINE
const gumRefPtr<Edge>& gumEdgeList<Edge>::shallowGet
( const gumId& node1, const gumId& node2 ) const  {
	return gumEdgeListBase<Edge>::edges[std::pair<gumId, gumId>
	                                    ( std::min( node1,node2 ),std::max( node1,node2 ) )];
}

// ================================================================================
/// returns a pointer to a given edge (allows shallow copies)
// ================================================================================
template <typename Edge> INLINE
const gumRefPtr<Edge>& gumEdgeList<Edge>::shallowGet( const Edge& edge )
const  {
	return gumEdgeListBase<Edge>::edges[std::pair<gumId, gumId>
	                                    ( edge.first(),edge.second() )];
}

// ================================================================================
/// alias for shallowGet
// ================================================================================
template <typename Edge> INLINE
const gumRefPtr<Edge>& gumEdgeList<Edge>::shallowGetEdge
( const gumId& node1, const gumId& node2 ) const  {
	return shallowGet( node1, node2 );
}

// ================================================================================
/// alias for shallowGet
// ================================================================================
template <typename Edge> INLINE
const gumRefPtr<Edge>& gumEdgeList<Edge>::shallowGetEdge( const Edge& edge )
const  {
	return shallowGet( edge );
}

// ================================================================================
/// add a copy of \e edge, an undirected edge, to the list of edges
// ================================================================================
template <typename Edge> INLINE
Edge& gumEdgeList<Edge>::insert( const Edge& edge ) {
	// check if the edge already exists
	if ( exists( edge ) )
		GUM_ERROR( gumDuplicateElement, "such an edge already exists" );

	// try to add the edge to the set of all edges
	Edge& result = gumEdgeListBase<Edge>::insert( edge );

	const gumRefPtr<Edge>& ptrEdge =
	  shallowGet( edge.first(), edge.second() );

	// add it to the set of neighbours
	try { neighbours[edge.first()].insert( ptrEdge ); }
	catch ( gumNotFound& ) {
		neighbours.insert( edge.first(), empty_neighbours );
		neighbours[edge.first()].insert( ptrEdge );
	}

	if ( edge.second() != edge.first() ) {
		try { neighbours[edge.second()].insert( ptrEdge ); }
		catch ( gumNotFound& ) {
			neighbours.insert( edge.second(), empty_neighbours );
			neighbours[edge.second()].insert( ptrEdge );
		}
	}

	return result;
}

// ================================================================================
/// adds a shallow copy of an edge into the edge list
// ================================================================================
template <typename Edge> INLINE
const gumRefPtr<Edge>&
gumEdgeList<Edge>::shallowInsert( const gumRefPtr<Edge>& edge ) {
	// check if the edge already exists
	if ( exists( *edge ) )
		GUM_ERROR( gumDuplicateElement, "such an edge already exists" );

	// try to add the edge to the set of all edges
	const gumRefPtr<Edge>& ptrEdge = gumEdgeListBase<Edge>::shallowInsert( edge );

	// add it to the set of neighbours
	try { neighbours[edge->first()].insert( ptrEdge ); }
	catch ( gumNotFound& ) {
		neighbours.insert( edge->first(), empty_neighbours );
		neighbours[edge->first()].insert( ptrEdge );
	}

	if ( edge->second() != edge->first() ) {
		try { neighbours[edge->second()].insert( ptrEdge ); }
		catch ( gumNotFound& ) {
			neighbours.insert( edge->second(), empty_neighbours );
			neighbours[edge->second()].insert( ptrEdge );
		}
	}

	return ptrEdge;
}

// ================================================================================
/// adds a shallow copy of an edge into the edge list
// ================================================================================
template <typename Edge> INLINE
const gumRefPtr<Edge>&
gumEdgeList<Edge>::shallowInsertEdge( const gumRefPtr<Edge>& edge ) {
	return shallowInsert( edge );
}

// ================================================================================
/// copy constructor (the new list shares its edges with \e from)
// ================================================================================
template <typename Edge>
gumEdgeList<Edge>::gumEdgeList( const gumEdgeList<Edge>& from,
                                gumCopyType copy_type ) :
		gumEdgeListBase<Edge>( from.capacity(), from.getResizePolicy() ),
		neighbours( from.neighbours.capacity(), from.neighbours.getResizePolicy() ),
		neighbours_iter_list( new gumListBase<gumListIterator< gumRefPtr<Edge> >*> ),
		empty_neighbours( neighbours_iter_list ) {
	// for debugging purposes
	GUM_CONS_CPY( gumEdgeList );
	// perform the type of copy desired (deep or shallow)

	if ( copy_type == GUM_SHALLOW_COPY ) {
		// copy the edge list
		gumEdgeListBase<Edge>::shallowCopy( from );
		// copy into neighbours the lists contained in from.neighbours
		const gumHashTable< gumId,gumList<gumRefPtr<Edge> > >& neigh = from.neighbours;

		for ( gumHashTableIterator< gumId,gumList<gumRefPtr<Edge> > > iter=neigh.begin();
		      iter != neigh.end(); ++iter ) {
			gumList<gumRefPtr<Edge> >& list = neighbours.insert( iter.key(), *iter );
			list.setIteratorList( neighbours_iter_list );
		}
	} else {
		for ( gumHashTableIterator< std::pair<gumId,gumId>,gumRefPtr<Edge> >
		      iter = from.edges.begin(); iter != from.edges.end(); ++iter )
			insert( **iter );
	}
}

// ================================================================================
/// removes all the edges of a gumEdgeList
// ================================================================================
template <typename Edge>
void gumEdgeList<Edge>::clear() {
	// remove the edges themselves
	gumEdgeListBase<Edge>::clear();
	// remove the edges from the neighbours
	neighbours.clear();
}

// ================================================================================
/// copy operator
// ================================================================================
template <typename Edge>
gumEdgeList<Edge>& gumEdgeList<Edge>::operator= ( const gumEdgeList<Edge>& from ) {
	// avoid self assignment
	if ( this != &from ) {
		GUM_OP_CPY( gumEdgeList );
		// remove the current content of the edge list
		clear();
		gumEdgeListBase<Edge>::edges.resize( from.edges.capacity() );
		neighbours.resize( from.neighbours.capacity() );
		// copy from's content

		for ( gumHashTableIterator< std::pair<gumId,gumId>,gumRefPtr<Edge> >
		      iter = from.edges.begin(); iter != from.edges.end(); ++iter )
			insert( **iter );
	}

	return *this;
}

// ================================================================================
/// copy operator (the new list shares its edges with \e from)
// ================================================================================
template <typename Edge>
gumEdgeList<Edge>& gumEdgeList<Edge>::shallowCopy( const gumEdgeList<Edge>& from ) {
	// avoid self assignment
	if ( this != &from ) {
		GUM_OP_CPY( gumEdgeList );
		// remove the current edges and copy all the edges in 'from'
		gumEdgeListBase<Edge>::shallowCopy( from );
		// remove the edges from the neighbours
		neighbours.clear();
		neighbours.resize( from.neighbours.capacity() );
		// copy of the neighbours hashtable
		const gumHashTable< gumId,gumList<gumRefPtr<Edge> > >& neigh = from.neighbours;

		for ( gumHashTableIterator<gumId,gumList<gumRefPtr<Edge> > > iter=neigh.begin();
		      iter != neigh.end(); ++iter ) {
			gumList<gumRefPtr<Edge> >& list = neighbours.insert( iter.key(), *iter );
			list.setIteratorList( neighbours_iter_list );
		}
	}

	return *this;
}

// ================================================================================
/// destructor
// ================================================================================
template <typename Edge>
gumEdgeList<Edge>::~gumEdgeList() {
	// for debugging purposes
	GUM_DESTRUCTOR( gumEdgeList );
}

// ================================================================================
/// changes the size of the hashtables within the edge list
// ================================================================================
template <typename Edge> INLINE
void gumEdgeList<Edge>::resize( gumSize new_size ) {
	gumEdgeListBase<Edge>::resize( new_size );
	neighbours.resize( new_size );
}

// ================================================================================
/// changes the resizing policy of the hashtables contained in the edge list
// ================================================================================
template <typename Edge> INLINE
void gumEdgeList<Edge>::setResizePolicy( const bool new_policy )  {
	gumEdgeListBase<Edge>::setResizePolicy( new_policy );
	neighbours.setResizePolicy( new_policy );
}

// ================================================================================
/// returns the undirected edge in the list specified by its extremal nodes
// ================================================================================
template <typename Edge> INLINE
Edge& gumEdgeList<Edge>::get( const gumId& node1, const gumId& node2 ) {
	return *( gumEdgeListBase<Edge>::edges[std::pair<gumId, gumId>
	                                       ( std::min( node1,node2 ),std::max( node1,node2 ) )] );
}

// ================================================================================
/// returns the undirected edge in the list specified by its extremal nodes
// ================================================================================
template <typename Edge> INLINE
const Edge& gumEdgeList<Edge>::get( const gumId& node1, const gumId& node2 )
const  {
	return *( gumEdgeListBase<Edge>::edges[std::pair<gumId, gumId>
	                                       ( std::min( node1,node2 ),std::max( node1,node2 ) )] );
}

// ================================================================================
/// returns the undirected edge in the list specified by its extremal nodes
// ================================================================================
template <typename Edge> INLINE
Edge& gumEdgeList<Edge>::operator()( const gumId& node1, const gumId& node2 ) {
	return get( node1, node2 );
}

// ================================================================================
/// returns the undirected edge in the list specified by its extremal nodes
// ================================================================================
template <typename Edge> INLINE
const Edge& gumEdgeList<Edge>::operator()( const gumId& node1, const gumId& node2 )
const  {
	return get( node1, node2 );
}

// ================================================================================
/// removes an edge specified by its extremal nodes from the list of edges
// ================================================================================
template <typename Edge> INLINE
void gumEdgeList<Edge>::erase( const gumId& node1, const gumId& node2 ) {
	try {
		// try to get the edge
		const gumRefPtr<Edge>& thePtrEdge = shallowGetEdge( node1, node2 );
		// if we arrive here, the edge exists, so remove it from the neighbour's
		// sets of its extremal nodes
		neighbours[node1].eraseByVal( thePtrEdge );
		neighbours[node2].eraseByVal( thePtrEdge );
		// remove the edge from the list of all edges
		gumEdgeListBase<Edge>::erase( *thePtrEdge );
	} catch ( gumNotFound& ) { }
}

// ================================================================================
/// removes a given edge from the list of edges
// ================================================================================
template <typename Edge> INLINE
void gumEdgeList<Edge>::erase( const Edge& edge ) {
	erase( edge.first(),edge.second() );
}

// ================================================================================
/// returns the list of the edges adjacent to a given node
// ================================================================================
template <typename Edge> INLINE
const gumList<gumRefPtr<Edge> >& gumEdgeList<Edge>::getneighbours( const gumId& node )
const  {
	try {
		return neighbours[node];
	} catch ( gumNotFound& ) {
		return empty_neighbours;
	}
}

// ================================================================================
/// alias for method get
// ================================================================================
template <typename Edge> INLINE
Edge& gumEdgeList<Edge>::getEdge( const gumId& node1, const gumId& node2 ) {
	return get( node1, node2 );
}

// ================================================================================
/// alias for method get
// ================================================================================
template <typename Edge> INLINE
const Edge& gumEdgeList<Edge>::getEdge( const gumId& node1, const gumId& node2 )
const  {
	return get( node1, node2 );
}

// ================================================================================
/// alias for method getneighbours
// ================================================================================
template <typename Edge> INLINE
const gumList<gumRefPtr<Edge> >&
gumEdgeList<Edge>::getneighbourEdges( const gumId& node ) const  {
	return getneighbours( node );
}

// ================================================================================
/// alias for method exists
// ================================================================================
template <typename Edge> INLINE
const bool gumEdgeList<Edge>::existsEdge( const Edge& edge ) const  {
	return exists( edge );
}

// ================================================================================
/// alias for method exists
// ================================================================================
template <typename Edge> INLINE
const bool gumEdgeList<Edge>::existsEdge( const gumId& node1, const gumId& node2 )
const  {
	return exists( node1, node2 );
}

// ================================================================================
/// alias for method insert
// ================================================================================
template <typename Edge> INLINE
Edge& gumEdgeList<Edge>::insertEdge( const Edge& edge ) {
	return insert( edge );
}

// ================================================================================
/// alias for method erase
// ================================================================================
template <typename Edge> INLINE
void gumEdgeList<Edge>::eraseEdge( const Edge& edge ) {
	erase( edge );
}

// ================================================================================
/// alias for method erase
// ================================================================================
template <typename Edge> INLINE
void gumEdgeList<Edge>::eraseEdge( const gumId& node1, const gumId& node2 ) {
	erase( node1, node2 );
}

// ================================================================================
/// returns an iterator pointing to the first neighbour of a given node
// ================================================================================
template <typename Edge> INLINE
gumEdgeListIterator<Edge> gumEdgeList<Edge>::beginneighbours( const gumId& node )
const {
	try {
		return iterator( neighbours[node], GUM_EDGELIST_ITER_BEGIN );
	} catch ( gumNotFound& ) {
		return end();
	}
}

// ================================================================================
/// returns an iterator pointing to the last neighbour of a given node */
// ================================================================================
template <typename Edge> INLINE
gumEdgeListIterator<Edge> gumEdgeList<Edge>::rbeginneighbours( const gumId& node )
const {
	try {
		return iterator( neighbours[node], GUM_EDGELIST_ITER_RBEGIN );
	} catch ( gumNotFound& ) {
		return rend();
	}
}

// ================================================================================
/// alias for iterator begin
// ================================================================================
template <typename Edge> INLINE
gumEdgeListIterator<Edge> gumEdgeList<Edge>::beginEdges() const {
	return begin();
}

// ==============================================================================
/// alias for iterator rbegin
// ==============================================================================
template <typename Edge> INLINE
gumEdgeListIterator<Edge> gumEdgeList<Edge>::rbeginEdges() const {
	return rbegin();
}

// ================================================================================
/// alias for iterator end
// ================================================================================
template <typename Edge> INLINE
const gumEdgeListIterator<Edge>& gumEdgeList<Edge>::endEdges() const {
	return end();
}

// ================================================================================
/// alias for iterator rend
// ================================================================================
template <typename Edge> INLINE
const gumEdgeListIterator<Edge>& gumEdgeList<Edge>::rendEdges() const {
	return rend();
}

// ================================================================================
/// alias for iterator beginneighbours
// ================================================================================
template <typename Edge> INLINE
gumEdgeListIterator<Edge> gumEdgeList<Edge>::beginneighbourEdges( const gumId& node )
const {
	return beginneighbours( node );
}

// ================================================================================
/// alias for iterator rbeginneighbours
// ================================================================================
template <typename Edge> INLINE
gumEdgeListIterator<Edge> gumEdgeList<Edge>::rbeginneighbourEdges( const gumId& node )
const {
	return rbeginneighbours( node );
}

// ================================================================================
/// removes the edges adjacent to a node
// ================================================================================
template <typename Edge> INLINE
void gumEdgeList<Edge>::eraseneighbours( const gumId& node ) {
	for ( iterator iter = beginneighbours( node ); iter != end(); ++iter )
		erase( *iter );
}

// ================================================================================
/// creates a gumEdgeList of Border from a gumEdgeList of Edge
// ================================================================================
template <typename Edge>
template <typename Border> gumEdgeList<Border>
gumEdgeList<Edge>::map( Border( *f )( Edge ), gumSize size ) const {
	// determine the proper size of the edgelist's embedded hashtable
	// by default, the size of the table is set so that the table does not take
	// too much space while allowing to add a few elements without resizing
	if ( size == 0 )
		size = std::max( 2UL, gumEdgeListBase<Edge>::edges.size() / 2 );

	// create a new table
	gumEdgeList<Border> list( size );

	// fill the new hash table
	for ( gumHashTableIterator<std::pair<gumId,gumId>,gumRefPtr<Edge> >
	      iter=gumEdgeListBase<Edge>::edges.rbegin();
	      iter != gumEdgeListBase<Edge>::edges.rend(); --iter ) {
		list.insert( f( **iter ) );
	}

	return list;
}

// ================================================================================
/// creates a gumEdgeList of Border from a gumEdgeList of Edge
// ================================================================================
template <typename Edge>
template <typename Border> gumEdgeList<Border>
gumEdgeList<Edge>::map( Border( *f )( Edge& ), gumSize size ) const {
	// determine the proper size of the edgelist's embedded hashtable
	// by default, the size of the table is set so that the table does not take
	// too much space while allowing to add a few elements without resizing
	if ( size == 0 )
		size = std::max( 2UL, gumEdgeListBase<Edge>::edges.size() / 2 );

	// create a new table
	gumEdgeList<Border> list( size );

	// fill the new hash table
	for ( gumHashTableIterator<std::pair<gumId,gumId>,gumRefPtr<Edge> >
	      iter=gumEdgeListBase<Edge>::edges.rbegin();
	      iter != gumEdgeListBase<Edge>::edges.rend(); --iter ) {
		list.insert( f( **iter ) );
	}

	return list;
}

// ================================================================================
/// creates a gumEdgeList of Border from a gumEdgeList of Edge
// ================================================================================
template <typename Edge>
template <typename Border> gumEdgeList<Border>
gumEdgeList<Edge>::map( Border( *f )( const Edge& ), gumSize size ) const {
	// determine the proper size of the edgelist's embedded hashtable
	// by default, the size of the table is set so that the table does not take
	// too much space while allowing to add a few elements without resizing
	if ( size == 0 )
		size = std::max( 2UL, gumEdgeListBase<Edge>::edges.size() / 2 );

	// create a new table
	gumEdgeList<Border> list( size );

	// fill the new hash table
	for ( gumHashTableIterator<std::pair<gumId,gumId>,gumRefPtr<Edge> >
	      iter=gumEdgeListBase<Edge>::edges.rbegin();
	      iter != gumEdgeListBase<Edge>::edges.rend(); --iter ) {
		list.insert( f( **iter ) );
	}

	return list;
}

// ===============================================================================
/// checks whether two lists of edges are similar
// ===============================================================================
template <typename Edge> INLINE
const bool gumEdgeList<Edge>::operator== ( const gumEdgeList<Edge>& from ) const {
	return gumEdgeListBase<Edge>::operator== ( from );
}

// ===============================================================================
/// checks whether two lists of edges are similar
// ===============================================================================
template <typename Edge> INLINE
const bool gumEdgeList<Edge>::shallowEqual( const gumEdgeList<Edge>& from )
const  {
	return gumEdgeListBase<Edge>::shallowEqual( from );
}

// ===============================================================================
/// checks whether two lists of edges contain different elements
// ===============================================================================
template <typename Edge>
const bool gumEdgeList<Edge>::operator!= ( const gumEdgeList<Edge>& from ) const {
	return gumEdgeListBase<Edge>::operator!= ( from );
}

// ===============================================================================
/// checks whether two lists of edges contain different elements
// ===============================================================================
template <typename Edge>
const bool gumEdgeList<Edge>::shallowDifferent( const gumEdgeList<Edge>& from )
const  {
	return gumEdgeListBase<Edge>::shallowDifferent( from );
}

/* ============================================================================== */
/* ============================================================================== */
/* ===                   IMPLEMENTATION OF THE GUM_ARC_LIST                   === */
/* ============================================================================== */
/* ============================================================================== */
// ================================================================================
/// Default constructor
// ================================================================================
template <typename Arc>
gumArcList<Arc>::gumArcList( gumSize nb_edges, bool resize_policy ) :
		gumEdgeListBase<Arc>( nb_edges,resize_policy ),
		parents( nb_edges,resize_policy ), children( nb_edges,resize_policy ),
		adjacent_iter_list( new gumListBase<gumListIterator< gumRefPtr<Arc> >*> ),
		empty_neighbours( adjacent_iter_list ) {
	// for debugging purposes
	GUM_CONSTRUCTOR( gumArcList );
}

// ================================================================================
/// returns a smart pointer on the edge the extremities of which are node1,node2
// ================================================================================
template <typename Arc> INLINE
const gumRefPtr<Arc>& gumArcList<Arc>::shallowGet
( const gumId& node1, const gumId& node2 ) const  {
	return gumEdgeListBase<Arc>::edges[std::pair<gumId, gumId> ( node1,node2 )];
}

// ================================================================================
/// returns a smart pointer on the edge the extremities of which are node1,node2
// ================================================================================
template <typename Arc> INLINE
const gumRefPtr<Arc>& gumArcList<Arc>::shallowGet( const Arc& arc )
const  {
	return gumEdgeListBase<Arc>::edges[std::pair<gumId, gumId>
	                                   ( arc.tail(),arc.head() )];
}

// ================================================================================
/// alias for shallowGet
// ================================================================================
template <typename Arc> INLINE
const gumRefPtr<Arc>& gumArcList<Arc>::shallowGetArc
( const gumId& node1, const gumId& node2 ) const  {
	return shallowGet( node1,node2 );
}

// ================================================================================
/// alias for shallowGet
// ================================================================================
template <typename Arc> INLINE
const gumRefPtr<Arc>& gumArcList<Arc>::shallowGetArc( const Arc& arc )
const  {
	return shallowGet( arc );
}

// ================================================================================
/// add a copy of \e edge to the list of edges.
// ================================================================================
template <typename Arc> INLINE
Arc& gumArcList<Arc>::insert( const Arc& edge ) {
	// try to add the edge to the set of all edges
	Arc& result = gumEdgeListBase<Arc>::insert( edge );
	const gumRefPtr<Arc>& ptrArc=shallowGet( edge.first(),edge.second() );
	// add it to the set of parents

	try { parents[edge.second()].insert( ptrArc ); }
	catch ( gumNotFound& ) {
		parents.insert( edge.second(), empty_neighbours );
		parents[edge.second()].insert( ptrArc );
	}

	// add it to the set of children
	try { children[edge.first()].insert( ptrArc ); }
	catch ( gumNotFound& ) {
		children.insert( edge.first(), empty_neighbours );
		children[edge.first()].insert( ptrArc );
	}

	return result;
}

// ================================================================================
/// adds a shallow copy of an edge into the edge list
// ================================================================================
template <typename Arc> INLINE
const gumRefPtr<Arc>& gumArcList<Arc>::shallowInsert( const gumRefPtr<Arc>& arc ) {
	// try to add the edge to the set of all edges
	const gumRefPtr<Arc>& ptrArc = gumEdgeListBase<Arc>::shallowInsert( arc );
	// add it to the set of parents

	try { parents[arc->second()].insert( ptrArc ); }
	catch ( gumNotFound& ) {
		parents.insert( arc->second(), empty_neighbours );
		parents[arc->second()].insert( ptrArc );
	}

	// add it to the set of children
	try { children[arc->first()].insert( ptrArc ); }
	catch ( gumNotFound& ) {
		children.insert( arc->first(), empty_neighbours );
		children[arc->first()].insert( ptrArc );
	}

	return ptrArc;
}

// ================================================================================
/// alias for shallowInsert
// ================================================================================
template <typename Arc> INLINE
const gumRefPtr<Arc>& gumArcList<Arc>::shallowinsertArc( const gumRefPtr<Arc>& arc ) {
	return shallowInsert( arc );
}

// ================================================================================
/// copy constructor (the new list shares its arcs with \e from)
// ================================================================================
template <typename Arc>
gumArcList<Arc>::gumArcList( const gumArcList<Arc>& from,
                             gumCopyType copy_type ) :
		gumEdgeListBase<Arc>( from.capacity(), from.getResizePolicy() ),
		parents( from.parents.capacity(), from.parents.getResizePolicy() ),
		children( from.children.capacity(), from.children.getResizePolicy() ),
		adjacent_iter_list( new gumListBase<gumListIterator< gumRefPtr<Arc> >*> ),
		empty_neighbours( adjacent_iter_list ) {
	// for debugging purposes
	GUM_CONS_CPY( gumArcList );
	// perform the type of copy desired (deep or shallow)

	if ( copy_type == GUM_SHALLOW_COPY ) {
		// copy the edge list
		gumEdgeListBase<Arc>::shallowCopy( from );
		// copy into parents the lists contained in from.parents
		const gumHashTable< gumId,gumList<gumRefPtr<Arc> > >& par = from.parents;

		for ( gumHashTableIterator< gumId,gumList<gumRefPtr<Arc> > > iter=par.begin();
		      iter != par.end(); ++iter ) {
			gumList< gumRefPtr<Arc> >& list = parents.insert( iter.key(), *iter );
			list.setIteratorList( adjacent_iter_list );
		}

		// copy into children the lists contained in from.children
		const gumHashTable< gumId,gumList<gumRefPtr<Arc> > >& child = from.children;

		for ( gumHashTableIterator< gumId,gumList<gumRefPtr<Arc> > > iter=child.begin();
		      iter != child.end(); ++iter ) {
			gumList< gumRefPtr<Arc> >& list = children.insert( iter.key(), *iter );
			list.setIteratorList( adjacent_iter_list );
		}
	} else {
		for ( gumHashTableIterator< std::pair<gumId,gumId>,gumRefPtr<Arc> >
		      iter = from.edges.begin(); iter != from.edges.end(); ++iter )
			insert( **iter );
	}
}

// ================================================================================
/// removes all the arcs from the gumArcList
// ================================================================================
template <typename Arc>
void gumArcList<Arc>::clear() {
	// remove the edges themselves
	gumEdgeListBase<Arc>::clear();
	// remove the edges from tha parents and children
	parents.clear();
	children.clear();
}

// ================================================================================
/// copy operator (the new list shares its arcs with \e from)
// ================================================================================
template <typename Arc>
gumArcList<Arc>& gumArcList<Arc>::operator= ( const gumArcList<Arc>& from ) {
	// avoid self assignment
	if ( this != &from ) {
		GUM_OP_CPY( gumArcList );
		// remove the current content of the edge list
		clear();
		gumEdgeListBase<Arc>::edges.resize( from.edges.capacity() );
		parents.resize( from.parents.capacity() );
		children.resize( from.children.capacity() );
		// copy from's content

		for ( gumHashTableIterator< std::pair<gumId,gumId>,gumRefPtr<Arc> >
		      iter = from.edges.begin(); iter != from.edges.end(); ++iter )
			insert( **iter );
	}

	return *this;
}

// ================================================================================
/// copy operator (the new list shares its arcs with \e from)
// ================================================================================
template <typename Arc>
gumArcList<Arc>& gumArcList<Arc>::shallowCopy( const gumArcList<Arc>& from ) {
	// avoid self assignment
	if ( this != &from ) {
		GUM_OP_CPY( gumArcList );
		// remove the current edges and copy all the edges in 'from'
		gumEdgeListBase<Arc>::shallowCopy( from );
		// remove the edges from the parents and children
		parents.clear();
		parents.resize( from.parents.capacity() );
		children.clear();
		children.resize( from.children.capacity() );
		// copy into parents of the lists contained in from.parents
		const gumHashTable< gumId,gumList<gumRefPtr<Arc> > >& par = from.parents;

		for ( gumHashTableIterator< gumId,gumList<gumRefPtr<Arc> > > iter=par.begin();
		      iter != par.end(); ++iter ) {
			gumList< gumRefPtr<Arc> >& list = parents.insert( iter.key(), *iter );
			list.setIteratorList( adjacent_iter_list );
		}

		// copy into children the lists contained in from.children
		const gumHashTable< gumId,gumList<gumRefPtr<Arc> > >& child = from.children;

		for ( gumHashTableIterator< gumId,gumList<gumRefPtr<Arc> > > iter=child.begin();
		      iter != child.end(); ++iter ) {
			gumList< gumRefPtr<Arc> >& list = children.insert( iter.key(), *iter );
			list.setIteratorList( adjacent_iter_list );
		}
	}

	return *this;
}

// ================================================================================
/// destructor
// ================================================================================
template <typename Arc>
gumArcList<Arc>::~gumArcList() {
	// for debugging purposes
	GUM_DESTRUCTOR( gumArcList );
}

// ================================================================================
/// changes the size of the hashtables within the arc list
// ================================================================================
template <typename Arc> INLINE
void gumArcList<Arc>::resize( gumSize new_size )  {
	gumEdgeListBase<Arc>::resize( new_size );
	parents.resize( new_size );
	children.resize( new_size );
}

// ================================================================================
/// changes the resizing policy of the hashtables contained in the edge list
// ================================================================================
template <typename Arc> INLINE
void gumArcList<Arc>::setResizePolicy( const bool new_policy )  {
	gumEdgeListBase<Arc>::setResizePolicy( new_policy );
	parents.setResizePolicy( new_policy );
	children.setResizePolicy( new_policy );
}

// ================================================================================
/// returns an iterator pointing to the first parent of a given node
// ================================================================================
template <typename Arc> INLINE
gumEdgeListIterator<Arc> gumArcList<Arc>::beginParents( const gumId& node ) const {
	try {
		return iterator( parents[node], GUM_EDGELIST_ITER_BEGIN );
	} catch ( gumNotFound& ) {
		return end();
	}
}

// ================================================================================
/// returns an iterator pointing to the last parent of a given node
// ================================================================================
template <typename Arc> INLINE
gumEdgeListIterator<Arc> gumArcList<Arc>::rbeginParents( const gumId& node ) const {
	try {
		return iterator( parents[node], GUM_EDGELIST_ITER_RBEGIN );
	} catch ( gumNotFound& ) {
		return rend();
	}
}

// ================================================================================
/// returns an iterator pointing to the first child of a given node
// ================================================================================
template <typename Arc> INLINE
gumEdgeListIterator<Arc> gumArcList<Arc>::beginChildren( const gumId& node ) const {
	try {
		return iterator( children[node], GUM_EDGELIST_ITER_BEGIN );
	} catch ( gumNotFound& ) {
		return end();
	}
}

// ================================================================================
/// returns an iterator pointing to the last parent of a given node
// ================================================================================
template <typename Arc> INLINE
gumEdgeListIterator<Arc> gumArcList<Arc>::rbeginChildren( const gumId& node ) const {
	try {
		return iterator( children[node], GUM_EDGELIST_ITER_RBEGIN );
	} catch ( gumNotFound& ) {
		return rend();
	}
}

// ================================================================================
/// returns an iterator pointing to the first adjacent edge of a node
// ================================================================================
template <typename Arc> INLINE
gumEdgeListIterator<Arc> gumArcList<Arc>::beginneighbours( const gumId& node ) const {
	const gumList<gumRefPtr<Arc> > *par = 0, *child = 0;

	try { par = &( parents[node] ); }
	catch ( gumNotFound& ) { par = 0; }

	try { child = &( children[node] ); }
	catch ( gumNotFound& ) { child = 0; }

	return iterator( par, child, node, this, GUM_EDGELIST_ITER_BEGIN );
}

// ================================================================================
/// returns an iterator pointing to the last adjacent edge of a node
// ================================================================================
template <typename Arc> INLINE
gumEdgeListIterator<Arc> gumArcList<Arc>::rbeginneighbours( const gumId& node ) const {
	const gumList<gumRefPtr<Arc> > *par = 0, *child = 0;

	try { par = &( parents[node] ); }
	catch ( gumNotFound& ) { par = 0; }

	try { child = &( children[node] ); }
	catch ( gumNotFound& ) { child = 0; }

	return iterator( par, child, node, this, GUM_EDGELIST_ITER_RBEGIN );
}

// ================================================================================
/// alias for iterator begin
// ================================================================================
template <typename Arc> INLINE
gumEdgeListIterator<Arc> gumArcList<Arc>::beginArcs() const {
	return begin();
}

// ================================================================================
/// alias for iterator rbegin
// ================================================================================
template <typename Arc> INLINE
gumEdgeListIterator<Arc> gumArcList<Arc>::rbeginArcs() const {
	return rbegin();
}

// ================================================================================
/// alias for iterator end
// ================================================================================
template <typename Arc> INLINE
const gumEdgeListIterator<Arc>& gumArcList<Arc>::endArcs() const {
	return end();
}

// ================================================================================
/// alias for iterator rend
// ================================================================================
template <typename Arc> INLINE
const gumEdgeListIterator<Arc>& gumArcList<Arc>::rendArcs() const {
	return rend();
}

// ================================================================================
/// alias for iterator beginneighbours
// ================================================================================
template <typename Arc> INLINE
gumEdgeListIterator<Arc> gumArcList<Arc>::beginneighbourArcs( const gumId& node )
const {
	return beginneighbours( node );
}

// ================================================================================
/// alias for iterator rbeginneighbours
// ================================================================================
template <typename Arc> INLINE
gumEdgeListIterator<Arc> gumArcList<Arc>::rbeginneighbourArcs( const gumId& node )
const {
	return rbeginneighbours( node );
}

// ================================================================================
/// returns the arc in the list specified by its tail and head
// ================================================================================
template <typename Arc> INLINE
Arc& gumArcList<Arc>::get( const gumId& node1, const gumId& node2 ) {
	return *( gumEdgeListBase<Arc>::edges[std::pair<gumId, gumId> ( node1,node2 )] );
}

// ================================================================================
/// returns the arc in the list specified by its tail and head
// ================================================================================
template <typename Arc> INLINE
const Arc& gumArcList<Arc>::get( const gumId& node1, const gumId& node2 ) const {
	return *( gumEdgeListBase<Arc>::edges[std::pair<gumId, gumId> ( node1,node2 )] );
}

// ================================================================================
/// returns the arc in the list specified by its tail and head
// ================================================================================
template <typename Arc> INLINE
Arc& gumArcList<Arc>::operator()( const gumId& node1, const gumId& node2 ) {
	return get( node1,node2 );
}

// ================================================================================
/// returns the arc in the list specified by its tail and head
// ================================================================================
template <typename Arc> INLINE
const Arc& gumArcList<Arc>::operator()( const gumId& node1, const gumId& node2 )
const  {
	return get( node1,node2 );
}

// ================================================================================
/// a << operator for gumArcList
// ================================================================================
template <typename Arc>
std::ostream& operator<< ( std::ostream& stream, const gumArcList<Arc>& list ) {
	for ( gumArcList<gumArc>::iterator iter = list.begin();
	      iter != list.end(); ++iter )
		stream<< *iter<<" - ";

	return stream;
}

// ================================================================================
/** @brief check whether an arc specified by its tail and head does belong
 * to the list of arcs. */
// ================================================================================
template <typename Arc> INLINE
const bool gumArcList<Arc>::exists( const gumId& node1, const gumId& node2 )
const  {
	// look for the edge
	return gumEdgeListBase<Arc>::edges.exists( std::pair<gumId, gumId> ( node1,node2 ) );
}

// ================================================================================
/// check whether an arc does belong to the list of arcs
// ================================================================================
template <typename Arc> INLINE
const bool gumArcList<Arc>::exists( const Arc& arc ) const  {
	// look for the edge
	return gumEdgeListBase<Arc>::edges.exists
	       ( std::pair<gumId, gumId> ( arc.tail(), arc.head() ) );
}

// ================================================================================
/// removes an arc specified by its tail and head from the list of edges
// ================================================================================
template <typename Arc> INLINE
void gumArcList<Arc>::erase( const gumId& node1, const gumId& node2 ) {
	try {
		// erase the edge from the set of all edges
		const gumRefPtr<Arc>& thePtrEdge = shallowGetArc( node1, node2 );
		// remove the edge from the neighbour's sets of its extremal nodes
		children[node1].eraseByVal( thePtrEdge );
		parents[node2].eraseByVal( thePtrEdge );
		gumEdgeListBase<Arc>::erase( *thePtrEdge );
	} catch ( gumNotFound& ) { }
}

// ================================================================================
/// removes a given arc from the list of edges.
// ================================================================================
template <typename Arc> INLINE
void gumArcList<Arc>::erase( const Arc& edge ) {
	erase( edge.first(),edge.second() );
}

// ================================================================================
/// removes the edges adjacent to a node
// ================================================================================
template <typename Arc> INLINE
void gumArcList<Arc>::eraseneighbours( const gumId& node ) {
	for ( iterator iter = beginneighbours( node ); iter != end(); ++iter )
		erase( *iter );
}

// ================================================================================
/// removes all the arcs outgoing from the given node
// ================================================================================
template <typename Arc> INLINE
void gumArcList<Arc>::eraseChildren( const gumId& node ) {
	for ( iterator iter = beginChildren( node ); iter != end(); ++iter )
		erase( *iter );
}

// ================================================================================
/// removes all the arcs ingoing into the given node
// ================================================================================
template <typename Arc> INLINE
void gumArcList<Arc>::eraseParents( const gumId& node ) {
	for ( iterator iter = beginParents( node ); iter != end(); ++iter )
		erase( *iter );
}

// ================================================================================
/// returns the list of the edges out-going from a given node
// ================================================================================
template <typename Arc> INLINE
const gumList<gumRefPtr<Arc> >& gumArcList<Arc>::getChildren( const gumId& node )
const  {
	try {
		return children[node];
	} catch ( gumNotFound& ) {
		return empty_neighbours;
	}
}

// ================================================================================
/// returns the list of the edges in-going to a given node
// ================================================================================
template <typename Arc> INLINE
const gumList<gumRefPtr<Arc> >& gumArcList<Arc>::getParents( const gumId& node )
const  {
	try {
		return parents[node];
	} catch ( gumNotFound& ) {
		return empty_neighbours;
	}
}

// ================================================================================
/// alias for method getEdge
// ================================================================================
template <typename Arc> INLINE
Arc& gumArcList<Arc>::getArc( const gumId& node1, const gumId& node2 ) {
	return get( node1, node2 );
}

// ================================================================================
/// alias for method getEdge
// ================================================================================
template <typename Arc> INLINE
const Arc& gumArcList<Arc>::getArc( const gumId& node1, const gumId& node2 )
const  {
	return get( node1, node2 );
}

// ==============================================================================
/// alias for method exists
// ==============================================================================
template <typename Arc> INLINE
const bool gumArcList<Arc>::existsArc( const Arc& edge ) const  {
	return exists( edge );
}

// ==============================================================================
/// alias for method exists
// ==============================================================================
template <typename Arc> INLINE
const bool gumArcList<Arc>::existsArc( const gumId& node1, const gumId& node2 )
const  {
	return exists( node1, node2 );
}

// ================================================================================
/// alias for method insert
// ================================================================================
template <typename Arc> INLINE
Arc& gumArcList<Arc>::insertArc( const Arc& edge ) {
	return insert( edge );
}

// ================================================================================
/// alias for method erase
// ================================================================================
template <typename Arc> INLINE
void gumArcList<Arc>::eraseArc( const Arc& edge ) {
	erase( edge );
}

// ================================================================================
/// alias for eraseChildren
// ================================================================================
template <typename Arc> INLINE
void gumArcList<Arc>::eraseChildrenArcs( const gumId& node ) {
	eraseChildren( node );
}

// ================================================================================
/// alias for eraseParents
// ================================================================================
template <typename Arc> INLINE
void gumArcList<Arc>::eraseParentArcs( const gumId& node ) {
	eraseParents( node );
}

// ================================================================================
/// creates a gumArcList of Curve from a gumArcList of Arc
// ================================================================================
template <typename Arc>
template <typename Curve> gumArcList<Curve>
gumArcList<Arc>::map( Curve( *f )( Arc ), gumSize size ) const {
	// determine the proper size of the arclist's embedded hashtable
	// by default, the size of the table is set so that the table does not take
	// too much space while allowing to add a few elements without resizing
	if ( size == 0 )
		size = std::max( 2UL, gumEdgeListBase<Arc>::edges.size() / 2 );

	// create a new table
	gumArcList<Curve> list( size );

	// fill the new hash table
	for ( gumHashTableIterator<std::pair<gumId,gumId>,gumRefPtr<Arc> >
	      iter=gumEdgeListBase<Arc>::edges.rbegin();
	      iter != gumEdgeListBase<Arc>::edges.rend(); --iter ) {
		list.insert( f( **iter ) );
	}

	return list;
}

// ================================================================================
/// creates a gumArcList of Curve from a gumArcList of Arc
// ================================================================================
template <typename Arc>
template <typename Curve> gumArcList<Curve>
gumArcList<Arc>::map( Curve( *f )( Arc& ), gumSize size ) const {
	// determine the proper size of the arclist's embedded hashtable
	// by default, the size of the table is set so that the table does not take
	// too much space while allowing to add a few elements without resizing
	if ( size == 0 )
		size = std::max( 2UL, gumEdgeListBase<Arc>::edges.size() / 2 );

	// create a new table
	gumArcList<Curve> list( size );

	// fill the new hash table
	for ( gumHashTableIterator<std::pair<gumId,gumId>,gumRefPtr<Arc> >
	      iter=gumEdgeListBase<Arc>::edges.rbegin();
	      iter != gumEdgeListBase<Arc>::edges.rend(); --iter ) {
		list.insert( f( **iter ) );
	}

	return list;
}

// ================================================================================
/// creates a gumArcList of Curve from a gumArcList of Arc
// ================================================================================
template <typename Arc>
template <typename Curve> gumArcList<Curve>
gumArcList<Arc>::map( Curve( *f )( const Arc& ), gumSize size ) const {
	// determine the proper size of the arclist's embedded hashtable
	// by default, the size of the table is set so that the table does not take
	// too much space while allowing to add a few elements without resizing
	if ( size == 0 )
		size = std::max( 2UL, gumEdgeListBase<Arc>::edges.size() / 2 );

	// create a new table
	gumArcList<Curve> list( size );

	// fill the new hash table
	for ( gumHashTableIterator<std::pair<gumId,gumId>,gumRefPtr<Arc> >
	      iter=gumEdgeListBase<Arc>::edges.rbegin();
	      iter != gumEdgeListBase<Arc>::edges.rend(); --iter ) {
		list.insert( f( **iter ) );
	}

	return list;
}

// ================================================================================
/// checks whether two lists of arcs are similar
// ================================================================================
template <typename Arc>
const bool gumArcList<Arc>::operator== ( const gumArcList<Arc>& from ) const {
	// check if the two edge lists have the same number of elements
	if ( gumEdgeListBase<Arc>::edges.size() !=
	     from.gumEdgeListBase<Arc>::edges.size() )
		return false;

	// parse both lists to find different edges
	for ( gumHashTableIterator< std::pair<gumId,gumId>,gumRefPtr<Arc> >
	      iter = gumEdgeListBase<Arc>::edges.begin();
	      iter != gumEdgeListBase<Arc>::edges.end(); ++iter ) {
		try {
			if ( **iter != *( from.gumEdgeListBase<Arc>::edges[iter.key()] ) )
				return false;
		} catch ( gumNotFound& ) { return false; }
	}

	return true;
}

// ================================================================================
/// checks whether two lists of arcs are similar
// ================================================================================
template <typename Arc>
const bool gumArcList<Arc>::shallowEqual( const gumArcList<Arc>& from )
const  {
	// check if the two edge lists have the same number of elements
	if ( gumEdgeListBase<Arc>::edges.size() !=
	     from.gumEdgeListBase<Arc>::edges.size() )
		return false;

	// parse both lists to find different edges
	for ( gumHashTableIterator< std::pair<gumId,gumId>,gumRefPtr<Arc> >
	      iter = gumEdgeListBase<Arc>::edges.begin();
	      iter != gumEdgeListBase<Arc>::edges.end(); ++iter ) {
		try {
			if ( *iter != from.gumEdgeListBase<Arc>::edges[iter.key()] )
				return false;
		} catch ( gumNotFound& ) { return false; }
	}

	return true;
}

// ================================================================================
/// checks whether two lists of edges contain different elements
// ================================================================================
template <typename Arc>
const bool gumArcList<Arc>::operator!= ( const gumArcList<Arc>& from ) const {
	// check if the two edge lists have the same number of elements
	if ( gumEdgeListBase<Arc>::edges.size() !=
	     from.gumEdgeListBase<Arc>::edges.size() )
		return true;

	// parse both lists to find different edges
	for ( gumHashTableIterator< std::pair<gumId,gumId>,gumRefPtr<Arc> >
	      iter = gumEdgeListBase<Arc>::edges.begin();
	      iter != gumEdgeListBase<Arc>::edges.end(); ++iter ) {
		try {
			if ( **iter != *( from.gumEdgeListBase<Arc>::edges[iter.key()] ) )
				return true;
		} catch ( gumNotFound& ) { return true; }
	}

	return false;
}

// ================================================================================
/// checks whether two lists of edges contain different elements
// ================================================================================
template <typename Arc>
const bool gumArcList<Arc>::shallowDifferent( const gumArcList<Arc>& from )
const  {
	// check if the two edge lists have the same number of elements
	if ( gumEdgeListBase<Arc>::edges.size() !=
	     from.gumEdgeListBase<Arc>::edges.size() )
		return true;

	// parse both lists to find different edges
	for ( gumHashTableIterator< std::pair<gumId,gumId>,gumRefPtr<Arc> >
	      iter = gumEdgeListBase<Arc>::edges.begin();
	      iter != gumEdgeListBase<Arc>::edges.end(); ++iter ) {
		try {
			if ( *iter != from.gumEdgeListBase<Arc>::edges[iter.key()] )
				return true;
		} catch ( gumNotFound& ) { return true; }
	}

	return false;
}

// ================================================================================
/// alias for method erase
// ================================================================================
template <typename Arc> INLINE
void gumArcList<Arc>::eraseArc( const gumId& node1, const gumId& node2 ) {
	erase( node1, node2 );
}

/* ============================================================================== */
/* ============================================================================== */
/* ===              IMPLEMENTATION OF THE GUM_EDGE_LIST_ITERATOR              === */
/* ============================================================================== */
/* ============================================================================== */
// ================================================================================
/// default constructor : the iterator points on nothing
// ================================================================================
template <typename Edge> INLINE
gumEdgeListIterator<Edge>::gumEdgeListIterator()  :
		iter_all( 0 ), is_end( false ), iter_adj(), mode( GUM_EDGELIST_ITER_ALL_MODE ) {
	// for debugging purposes
	GUM_CONSTRUCTOR( gumEdgeListIterator );
}

// ================================================================================
/// copy constructor
// ================================================================================
template <typename Edge> INLINE
gumEdgeListIterator<Edge>::gumEdgeListIterator
( const gumEdgeListIterator<Edge>& from ) :
		iter_all( 0 ), is_end( false ), iter_adj( from.iter_adj ), mode( from.mode ),
		parents( from.parents ), children( from.children ), neighbours( from.neighbours ),
		nodeID( from.nodeID ), arcList( from.arcList ) {
	// for debugging purposes
	GUM_CONS_CPY( gumEdgeListIterator );

	if ( from.iter_all )
		iter_all = new gumHashTableIterator< std::pair<gumId,gumId>,gumRefPtr<Edge> >
		( *from.iter_all );
}

// ================================================================================
/// copy iterator
// ================================================================================
template <typename Edge> INLINE
gumEdgeListIterator<Edge>& gumEdgeListIterator<Edge>::operator=
( const gumEdgeListIterator<Edge>& from ) {
	// avoid self assignment
	if ( this != &from ) {
		// for debugging purposes
		GUM_OP_CPY( gumEdgeListIterator );

		if ( iter_all ) delete iter_all;

		if ( from.iter_all )
			iter_all = new gumHashTableIterator< std::pair<gumId,gumId>,gumRefPtr<Edge> >
			( *from.iter_all );
		else
			iter_all = 0;

		is_end = false;

		iter_adj = from.iter_adj;

		mode = from.mode;

		parents   = from.parents;

		children  = from.children;

		neighbours = from.neighbours;

		nodeID  = from.nodeID;

		arcList = from.arcList;
	}

	return *this;
}

// ================================================================================
/// destructor
// ================================================================================
template <typename Edge> INLINE
gumEdgeListIterator<Edge>::~gumEdgeListIterator() {
	// for debugging purposes
	GUM_DESTRUCTOR( gumEdgeListIterator );

	if ( !is_end && iter_all ) delete iter_all;
}

// ================================================================================
/// makes the iterator point to the next element in the edge list
// ================================================================================
template <typename Edge> INLINE
gumEdgeListIterator<Edge>& gumEdgeListIterator<Edge>::operator++()  {
	if ( mode == GUM_EDGELIST_ITER_ALL_MODE ) {
		if ( iter_all )
			++*iter_all;
	} else {
		++iter_adj;

		if (( mode == GUM_EDGELIST_ITER_ADJ_PARENT_MODE ) && iter_adj.isEnd() ) {
			if ( !children ) {
				try {
					children = &( arcList->children[nodeID] );
				} catch ( gumNotFound& ) {
					mode = GUM_EDGELIST_ITER_END_MODE;
					return *this;
				}
			}

			iter_adj = children->begin();

			mode = GUM_EDGELIST_ITER_ADJ_CHILD_MODE;
		}
	}

	return *this;
}

// ================================================================================
/// makes the iterator point to the preceding element in the edge list
// ================================================================================
template <typename Edge> INLINE
gumEdgeListIterator<Edge>& gumEdgeListIterator<Edge>::operator--()  {
	if ( mode == GUM_EDGELIST_ITER_ALL_MODE ) {
		if ( iter_all )
			--*iter_all;
	} else {
		--iter_adj;

		if (( mode == GUM_EDGELIST_ITER_ADJ_CHILD_MODE ) && iter_adj.isEnd() ) {
			if ( !parents ) {
				try {
					parents = &( arcList->parents[nodeID] );
				} catch ( gumNotFound& ) {
					mode = GUM_EDGELIST_ITER_END_MODE;
					return *this;
				}
			}

			iter_adj = parents->rbegin();

			mode = GUM_EDGELIST_ITER_ADJ_PARENT_MODE;
		}
	}

	return *this;
}

// ================================================================================
/// checks whether two edge list iterators point to the same edge
// ================================================================================
template <typename Edge> INLINE
const bool gumEdgeListIterator<Edge>::operator==
( const gumEdgeListIterator<Edge>& from ) const  {
	if ( from.mode == GUM_EDGELIST_ITER_END_MODE ) {
		if ( mode == GUM_EDGELIST_ITER_ALL_MODE ) {
			if ( !iter_all || !from.iter_all )
				return false;
			else
				return ( *iter_all == *from.iter_all );
		} else
			return ( iter_adj == from.iter_adj );
	} else if ( mode == GUM_EDGELIST_ITER_END_MODE ) {
		if ( from.mode == GUM_EDGELIST_ITER_ALL_MODE ) {
			if ( !iter_all || !from.iter_all )
				return false;
			else
				return ( *iter_all == *from.iter_all );
		} else
			return ( iter_adj == from.iter_adj );
	} else if ( mode != from.mode )
		return false;
	else {
		if ( mode == GUM_EDGELIST_ITER_ALL_MODE ) {
			if ( !iter_all || !from.iter_all )
				return false;
			else
				return ( *iter_all == *from.iter_all );
		} else
			return ( iter_adj == from.iter_adj );
	}
}

// ================================================================================
/// checks whether two edge list iterators point to different edges
// ================================================================================
template <typename Edge> INLINE
const bool gumEdgeListIterator<Edge>::operator!=
( const gumEdgeListIterator<Edge>& from ) const  {
	if ( from.mode == GUM_EDGELIST_ITER_END_MODE ) {
		if ( mode == GUM_EDGELIST_ITER_ALL_MODE ) {
			if ( !iter_all || !from.iter_all )
				return true;
			else
				return ( *iter_all != *from.iter_all );
		} else
			return ( iter_adj != from.iter_adj );
	} else if ( mode == GUM_EDGELIST_ITER_END_MODE ) {
		if ( from.mode == GUM_EDGELIST_ITER_ALL_MODE ) {
			if ( !iter_all || !from.iter_all )
				return true;
			else
				return ( *iter_all != *from.iter_all );
		} else
			return ( iter_adj != from.iter_adj );
	} else if ( mode != from.mode ) return true;
	else {
		if ( mode == GUM_EDGELIST_ITER_ALL_MODE ) {
			if ( !iter_all || !from.iter_all )
				return true;
			else
				return ( *iter_all != *from.iter_all );
		} else
			return ( iter_adj != from.iter_adj );
	}
}

// ================================================================================
/// gives access to the edge pointed to by the iterator
// ================================================================================
template <typename Edge> INLINE
Edge* gumEdgeListIterator<Edge>::operator-> () const {
	if ( mode == GUM_EDGELIST_ITER_ALL_MODE ) {
		if ( !iter_all ) GUM_ERROR( gumUndefinedIteratorValue, "null iterator" );

		return ( *iter_all )->operator->();
	} else
		return iter_adj->operator->();
}

// ================================================================================
/// gives access to the edge pointed to by the iterator
// ================================================================================
template <typename Edge> INLINE
Edge& gumEdgeListIterator<Edge>::operator*() {
	if ( mode == GUM_EDGELIST_ITER_ALL_MODE ) {
		if ( !iter_all ) GUM_ERROR( gumUndefinedIteratorValue, "null iterator" );

		return ( ***iter_all );
	} else
		return ( **iter_adj );
}

// ================================================================================
/// gives access to the edge pointed to by the iterator
// ================================================================================
template <typename Edge> INLINE
const Edge& gumEdgeListIterator<Edge>::operator*() const {
	if ( mode == GUM_EDGELIST_ITER_ALL_MODE ) {
		if ( !iter_all ) GUM_ERROR( gumUndefinedIteratorValue, "null iterator" );

		return ( ***iter_all );
	} else
		return ( **iter_adj );
}

// ================================================================================
/// detach the iterator from its current gumEdgeList
// ================================================================================
template <typename Edge>
void gumEdgeListIterator<Edge>::clear()  {
	// if the current iterator is attached to a hashtable, we should
	// detach it before going on
	if ( iter_all && !is_end ) delete iter_all;

	iter_all = 0;

	is_end = false;

	// we should also set the iter_adj to an end_list
	iter_adj.clear();

	mode = GUM_EDGELIST_ITER_END_MODE;

	parents   = 0;

	children  = 0;

	neighbours = 0;

	nodeID  = 0;

	arcList = 0;
}

// ================================================================================
/** creates an iterator for the begin (forward=1), rbegin (forward=2),
 * end (forward = 3) and rend (forward = 4) iterators stored into the EdgeLists.
 * gumEdgeList and gumArcList instances. */
// ================================================================================
template <typename Edge>
gumEdgeListIterator<Edge>::gumEdgeListIterator
( const gumHashTable< std::pair<gumId,gumId>,gumRefPtr<Edge> >& edges,
  gumEdgeListIteratorLocation forward ) : iter_all( 0 ), iter_adj() {
	// for debugging purposes
	GUM_CONSTRUCTOR( gumEdgeListIterator );

	switch ( forward ) {
		case GUM_EDGELIST_ITER_BEGIN:
			iter_all = new gumHashTableIterator< std::pair<gumId,gumId>,gumRefPtr<Edge> >
			( edges.begin() );
			is_end = false;
			mode = GUM_EDGELIST_ITER_ALL_MODE;
			break;
		case GUM_EDGELIST_ITER_RBEGIN:
			iter_all = new gumHashTableIterator< std::pair<gumId,gumId>,gumRefPtr<Edge> >
			( edges.rbegin() );
			is_end = false;
			mode = GUM_EDGELIST_ITER_ALL_MODE;
			break;
		case GUM_EDGELIST_ITER_END:
			iter_all =
			  const_cast<gumHashTableIterator< std::pair<gumId,gumId>,gumRefPtr<Edge> >*>
			  ( &edges.end() );
			is_end = true;
			mode = GUM_EDGELIST_ITER_END_MODE;
			iter_adj.setToEnd();
			break;
		case GUM_EDGELIST_ITER_REND:
			iter_all =
			  const_cast<gumHashTableIterator< std::pair<gumId,gumId>,gumRefPtr<Edge> >*>
			  ( &edges.rend() );
			is_end = true;
			mode = GUM_EDGELIST_ITER_END_MODE;
			iter_adj.setToEnd();
			break;
		default:
			GUM_ERROR( gumFatalError, "gumEdgeListIterator constructor case undefined" );
	}
}

// ================================================================================
/// sets an iterator to end/rend (intended for iter_end/iter_rend)
// ================================================================================
template <typename Edge> void
gumEdgeListIterator<Edge>::setIteratorEnd
( const gumHashTable< std::pair<gumId,gumId>,gumRefPtr<Edge> >& edges,
  gumEdgeListIteratorLocation forward ) {
	if ( forward == GUM_EDGELIST_ITER_END )
		iter_all =
		  const_cast<gumHashTableIterator< std::pair<gumId,gumId>,gumRefPtr<Edge> >*>
		  ( &edges.end() );
	else
		iter_all =
		  const_cast<gumHashTableIterator< std::pair<gumId,gumId>,gumRefPtr<Edge> >*>
		  ( &edges.rend() );

	is_end = true;

	iter_adj.setToEnd();

	mode = GUM_EDGELIST_ITER_END_MODE;
}

// ================================================================================
/** creates an iterator for a list of adjacent undirected edges. Depending
 * on 'forward', we create a begin (forward = 1), rbegin(forward = 2),
 * end (forward = 3) or rend (forward = 4) iterator. */
// ================================================================================
template <typename Edge>
gumEdgeListIterator<Edge>::gumEdgeListIterator
( const gumList<gumRefPtr<Edge> >& edges,
  gumEdgeListIteratorLocation forward ) :
		iter_all( 0 ), is_end( false ),
		iter_adj( forward == GUM_EDGELIST_ITER_BEGIN  ? edges.begin()  :
		          forward == GUM_EDGELIST_ITER_RBEGIN ? edges.rbegin() :
		          forward == GUM_EDGELIST_ITER_END    ? edges.end() : edges.rend() ),
		mode( forward==GUM_EDGELIST_ITER_BEGIN  ? GUM_EDGELIST_ITER_ADJ_MODE :
		      forward==GUM_EDGELIST_ITER_RBEGIN ? GUM_EDGELIST_ITER_ADJ_MODE :
		      GUM_EDGELIST_ITER_END_MODE ) {
	// for debugging purposes
	GUM_CONSTRUCTOR( gumEdgeListIterator );
}

// ================================================================================
/** creates an iterator for a list of adjacent arcs. Depending on 'forward', we
 * create a begin (forward = 1), rbegin(forward = 2), end (forward = 3) or
 * rend (forward = 4) iterator. */
// ================================================================================
template <typename Edge>
gumEdgeListIterator<Edge>::gumEdgeListIterator
( const gumList<gumRefPtr<Edge> >* par, const gumList<gumRefPtr<Edge> >* child,
  const gumId& node, const gumArcList<Edge>* list,
  gumEdgeListIteratorLocation forward ) :
		iter_all( 0 ), is_end( false ), parents( par ), children( child ),
		nodeID( node ), arcList( list ) {
	// for debugging purposes
	GUM_CONSTRUCTOR( gumEdgeListIterator );
	// set properly iter_adj and mode depending on the value of forward

	switch ( forward ) {
		case GUM_EDGELIST_ITER_END:
		case GUM_EDGELIST_ITER_REND:
			iter_adj.setToEnd();
			mode = GUM_EDGELIST_ITER_END_MODE;
			break;
		case GUM_EDGELIST_ITER_BEGIN:
			// check if there are parents or not

			if ( par && par->size() ) {
				iter_adj = par->begin();
				mode = GUM_EDGELIST_ITER_ADJ_PARENT_MODE;
			}

			// if no parent, check whether there are children
			else if ( child && child->size() ) {
				iter_adj = child->begin();
				mode = GUM_EDGELIST_ITER_ADJ_CHILD_MODE;
			}

			// no parent, no child => goto end
			else {
				iter_adj.setToEnd();
				mode = GUM_EDGELIST_ITER_END_MODE;
			}

			break;

		case GUM_EDGELIST_ITER_RBEGIN:
			// check if there are children or not

			if ( child && child->size() ) {
				iter_adj = child->rbegin();
				mode = GUM_EDGELIST_ITER_ADJ_CHILD_MODE;
			}

			// if no child, check whether there are parents
			else if ( par && par->size() ) {
				iter_adj = par->rbegin();
				mode = GUM_EDGELIST_ITER_ADJ_PARENT_MODE;
			}

			// no parent, no child => goto end
			else {
				iter_adj.setToEnd();
				mode = GUM_EDGELIST_ITER_END_MODE;
			}

			break;
	}
}
#endif /* DOXYGEN_SHOULD_SKIP_THIS */
