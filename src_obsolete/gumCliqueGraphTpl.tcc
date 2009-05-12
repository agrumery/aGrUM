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
 * @brief Template implementation of basic clique graphs
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */


#include <agrum/gumDebug.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/* ============================================================================== */
/* ============================================================================== */
/* ===                   IMPLEMENTATION OF GUM_CLIQUE_GRAPH                   === */
/* ============================================================================== */
/* ============================================================================== */

// ================================================================================
/// basic constructor: creates an empty clique graph
// ================================================================================
template <typename Clique, typename Edge, typename Node>
gumCliqueGraphTpl<Clique,Edge,Node>::gumCliqueGraphTpl( gumSize nodes_size,
    bool nodes_resize_policy,
    gumSize edges_size,
    bool edges_resize_policy ) :
    gumUndiGraphTpl<Clique,Edge> ( nodes_size, nodes_resize_policy,
                                edges_size, edges_resize_policy ) {
  // for debugging purposes
  GUM_CONSTRUCTOR( gumCliqueGraphTpl );
}


// ================================================================================
/// copy constructor (by default, deep copy of nodes and edges lists)
// ================================================================================
template <typename Clique, typename Edge, typename Node>
gumCliqueGraphTpl<Clique,Edge,Node>::gumCliqueGraphTpl
( const gumCliqueGraphTpl<Clique,Edge,Node>& from, gumCopyType copy_type ) :
    gumUndiGraphTpl<Clique,Edge> ( from, copy_type ) {
  // for debugging purposes
  GUM_CONS_CPY( gumCliqueGraphTpl );

  // fill the cliques sets

  if ( copy_type == GUM_DEEP_COPY )
    // in deep copy mode, first create an empty nodes set for each clique,
    // then fill the set with copies of the nodes
    for ( gumHashTableIterator< gumId,gumNodeList<Node> >
          iter=from.cliques.begin(); iter!=from.cliques.end(); ++iter )
      cliques.insert( iter.key(), *iter );
  else
    for ( gumHashTableIterator< gumId,gumNodeList<Node> >
          iter=from.cliques.begin(); iter!=from.cliques.end(); ++iter ) {
      gumNodeList<Node>& list = cliques.insert( iter.key(), gumNodeList<Node>() );
      list.shallowCopy( *iter );
    }

  // fill the separator sets
  if ( copy_type == GUM_DEEP_COPY )
    // in deep copy mode, first create an empty nodes set for each separator,
    // then fill the set with copies of the nodes
    for ( gumHashTableIterator< std::pair<gumId,gumId>,gumNodeList<Node> >
          iter = from.separators.begin(); iter != from.separators.end(); ++iter )
      separators.insert( iter.key(), *iter );
  else
    for ( gumHashTableIterator< std::pair<gumId,gumId>,gumNodeList<Node> >
          iter = from.separators.begin(); iter != from.separators.end(); ++iter ) {
      gumNodeList<Node>& list=separators.insert( iter.key(),gumNodeList<Node>() );
      list.shallowCopy( *iter );
    }
}


// ================================================================================
/// copy operator (deep copy)
// ================================================================================
template <typename Clique, typename Edge, typename Node>
gumCliqueGraphTpl<Clique,Edge,Node>& gumCliqueGraphTpl<Clique,Edge,Node>::operator=
( const gumCliqueGraphTpl<Clique,Edge,Node>& from ) {
  // avoid self assignment
  if ( this != &from ) {
    // for debugging purposes
    GUM_OP_CPY( gumCliqueGraphTpl );

    gumUndiGraphTpl<Clique,Edge>::operator= ( from );

    // fill the cliques sets: first create an empty nodes set for each clique,
    // then fill the set with copies of the nodes
    cliques.clear();

    for ( gumHashTableIterator< gumId,gumNodeList<Node> >
          iter=from.cliques.begin(); iter!=from.cliques.end(); ++iter )
      cliques.insert( iter.key(), *iter );

    // fill the separator sets: first create an empty nodes set for each separator,
    // then fill the set with copies of the nodes
    separators.clear();

    for ( gumHashTableIterator< std::pair<gumId,gumId>,gumNodeList<Node> >
          iter = from.separators.begin(); iter != from.separators.end(); ++iter )
      separators.insert( iter.key(), *iter );
  }

  return *this;
}


// ================================================================================
/// destructor
// ================================================================================
template <typename Clique, typename Edge, typename Node>
gumCliqueGraphTpl<Clique,Edge,Node>::~gumCliqueGraphTpl<Clique,Edge,Node> () {
  // for debugging purposes
  GUM_DESTRUCTOR( gumCliqueGraphTpl );
}


// ================================================================================
/// copy operator (the new graph shares its edges/nodes with \e from)
// ================================================================================
template <typename Clique, typename Edge, typename Node>
gumCliqueGraphTpl<Clique,Edge,Node>& gumCliqueGraphTpl<Clique,Edge,Node>::shallowCopy
( const gumCliqueGraphTpl<Clique,Edge,Node>& from ) {
  gumUndiGraphTpl<Clique,Edge>::shallowCopy( from );

  // avoid self assigment

  if ( this != &from ) {
    // fill the cliques sets
    cliques.clear();

    for ( gumHashTableIterator< gumId,gumNodeList<Node> >
          iter=from.cliques.begin(); iter!=from.cliques.end(); ++iter ) {
      gumNodeList<Node>& list = cliques.insert( iter.key(), gumNodeList<Node>() );
      list.shallowCopy( *iter );
    }

    // fill the separator sets
    separators.clear();

    for ( gumHashTableIterator< std::pair<gumId,gumId>,gumNodeList<Node> >
          iter = from.separators.begin(); iter != from.separators.end(); ++iter ) {
      gumNodeList<Node>& list=separators.insert( iter.key(),gumNodeList<Node>() );
      list.shallowCopy( *iter );
    }
  }

  return *this;
}


// ================================================================================
/// function used to update the separators when clique/edges are modified
// ================================================================================
template <typename Clique, typename Edge, typename Node>
void gumCliqueGraphTpl<Clique,Edge,Node>::updateSeparator( const gumId& id1,
    const gumId& id2 ) {
  // get the node sets of the two cliques
  gumNodeList<Node>& clique1 = cliques[id1];
  gumNodeList<Node>& clique2 = cliques[id2];

  // get the separator that needs be updated
  gumNodeList<Node>& separ =
    separators[std::pair<gumId,gumId>( std::min( id1,id2 ),std::max( id1,id2 ) )];

  // update the separator

  if ( clique1.size() < clique2.size() ) {
    separ.shallowCopy( clique1 );

    for ( gumNodeListIterator<Node> iter = separ.begin(); iter!=separ.end(); ++iter )
      if ( !clique2.exists( iter.getID() ) ) separ.erase( iter.getID() );
  } else {
    separ.shallowCopy( clique2 );

    for ( gumNodeListIterator<Node> iter = separ.begin(); iter!=separ.end(); ++iter )
      if ( !clique1.exists( iter.getID() ) ) separ.erase( iter.getID() );
  }
}


// ================================================================================
/// adds a copy of \e arc to the list of edges of the graph.
// ================================================================================
template <typename Clique, typename Edge, typename Node>
Edge& gumCliqueGraphTpl<Clique,Edge,Node>::insertEdge( const Edge& edge ) {
  // create the edge in the graph
  Edge& new_edge = gumUndiGraphTpl<Clique,Edge>::insertEdge( edge );

  // create the separator
  separators.insert( std::pair<gumId,gumId>( new_edge.first(),
                     new_edge.second() ),
                     gumNodeList<Node>() );
  updateSeparator( new_edge.first(),new_edge.second() );

  // return the newly created edge
  return new_edge;
}


// ================================================================================
/// removes an edge (and its separator) from the clique graph
// ================================================================================
template <typename Clique, typename Edge, typename Node>
void gumCliqueGraphTpl<Clique,Edge,Node>::eraseEdge
( const gumId &id1, const gumId &id2 ) {
  gumId min_id = std::min( id1,id2 ), max_id = std::max( id1,id2 );

  if ( gumUndiGraphTpl<Clique,Edge>::existsEdge( min_id,max_id ) ) {
    gumUndiGraphTpl<Clique,Edge>::eraseEdge( min_id,max_id );
    separators.erase( std::pair<gumId,gumId>( min_id,max_id ) );
  }
}


// ================================================================================
/// removes an edge (and its separator) from the clique graph
// ================================================================================
template <typename Clique, typename Edge, typename Node> INLINE
void gumCliqueGraphTpl<Clique,Edge,Node>::eraseEdge( const Edge &edge ) {
  eraseEdge( edge.first(), edge.second() );
}


// ================================================================================
/// adds a new clique to the graph
// ================================================================================
template <typename Clique, typename Edge, typename Node>
Clique& gumCliqueGraphTpl<Clique,Edge,Node>::insertNode
( const Clique& clique, const gumNodeList<Node>& list, gumCopyType copy_type ) {
  // create the new node in the graph
  Clique& new_node = gumUndiGraphTpl<Clique,Edge>::insertNode( clique );

  // update the set of nodes of the clique

  if ( copy_type == GUM_DEEP_COPY )
    cliques.insert( new_node.getID(), list );
  else {
    gumNodeList<Node>& new_clique = cliques.insert( new_node.getID(),
                                    gumNodeList<Node>() );
    new_clique.shallowCopy( list );
  }

  return new_node;
}


// ==============================================================================
/// removes a given clique from the clique graph
// ==============================================================================
template <typename Clique, typename Edge, typename Node>
void gumCliqueGraphTpl<Clique,Edge,Node>::eraseNode( const gumId &id ) {
  // check if the node belongs to the graph
  if ( !gumUndiGraphTpl<Clique,Edge>::existsNode( id ) ) return;

  // remove the separators
  for ( gumEdgeListIterator<Edge> iter =
          gumUndiGraphTpl<Clique,Edge>::beginneighbourEdges( id );
        iter != gumUndiGraphTpl<Clique,Edge>::endEdges(); ++iter )
    separators.erase( std::pair<gumId,gumId>( iter->first(),
                      iter->second() ) );

  // erase the clique set
  cliques.erase( id );

  // erase the node and its neighbours from the graph
  gumUndiGraphTpl<Clique,Edge>::eraseNode( id );
}


// ================================================================================
/// removes a given clique from the clique graph
// ================================================================================
template <typename Clique, typename Edge, typename Node>
void gumCliqueGraphTpl<Clique,Edge,Node>::eraseNode( const Node &node ) {
  eraseNode( node.getID() );
}


// ================================================================================
/// returns the set of nodes included into a given clique
// ================================================================================
template <typename Clique, typename Edge, typename Node> INLINE
const gumNodeList<Node>&
gumCliqueGraphTpl<Clique,Edge,Node>::getClique( const Clique& clique )
const  {
  return cliques[clique.getID()];
}


// ================================================================================
/// returns the set of nodes included into a clique defined by its ID
// ================================================================================
template <typename Clique, typename Edge, typename Node> INLINE
const gumNodeList<Node>&
gumCliqueGraphTpl<Clique,Edge,Node>::getClique( const gumId& id )
const  {
  return cliques[id];
}


// ================================================================================
/// returns the id of a clique containing the node the id of which is in argument
// ================================================================================
template <typename Clique, typename Edge, typename Node>
gumId gumCliqueGraphTpl<Clique,Edge,Node>::getContainerNode( const gumId& id )
const  {
  for ( gumHashTableIterator< gumId, gumNodeList<Node> > iter = cliques.begin();
        iter != cliques.end(); ++iter )
    if ( iter->exists( id ) )
      return iter.key();

  GUM_ERROR( gumNotFound, "" );
}


// ================================================================================
/// function used to mark the path from one clique to another
// ================================================================================
template <typename Clique, typename Edge, typename Node>
const bool gumCliqueGraphTpl<Clique,Edge,Node>::getPathMark
( gumHashTable<gumId,char>& marks, std::vector<gumId>& path,
  const gumId& node, const gumId& from, const gumId& looked ) const  {
  // mark the node as seen
  marks[node] = 1;

  if ( node == looked ) {
    path.push_back( looked );
    return true;
  }

  // mark its neighbours
  gumId neigh;

  for ( gumEdgeListIterator<Edge> iter = beginneighbourEdges( node );
        iter != endEdges(); ++iter ) {
    // get the neighbour
    neigh = iter->other( node );

    if ( neigh != from ) {
      // check if the neighbour has already been marked => no need to examine it
      if ( marks[neigh] >= 1 )
        continue;

      // okay, follow the neighbour
      if ( getPathMark( marks, path, neigh, node, looked ) ) {
        if ( path[path.size()-1] != node )
          path.push_back( node );

        return true;
      }
    }
  }

  // we checked all the subgraph and found no cycle
  marks[node] = 2;

  return false;
}



// ================================================================================
/// returns a possible path from node1 to node2 in the clique graph
// ================================================================================
template <typename Clique, typename Edge, typename Node>
std::vector<gumId>
gumCliqueGraphTpl<Clique,Edge,Node>::getPath( const gumId &node1, const gumId &node2 )
const  {
  // beginning of the algorithm: mark all nodes to 'false' and the
  // vector path is empty
  gumHashTable<gumId,char> nodes_marked = getNodes().hashMap( '\0' );
  std::vector<gumId> path;

  // call the recursive algorithm parsing the graph

  if ( getPathMark( nodes_marked, path, node2, node2, node1 ) )
    return path;

  GUM_ERROR( gumNotFound, "" );
}


// ================================================================================
/// returns a path from a clique containing node1 to a clique containing node2
// ================================================================================
template <typename Clique, typename Edge, typename Node>
std::vector<gumId>
gumCliqueGraphTpl<Clique,Edge,Node>::getContainerPath( const gumId &node1,
    const gumId &node2 )
const  {
  // get a path from a clique containing node1 to a clique containing node2
  std::vector<gumId> path = getPath( getContainerNode( node1 ),
                                     getContainerNode( node2 ) );

  // it may happen that the path contains several nodes containing node1 and
  // node2. Hence we shall remove the superfluous nodes

  while ( path.size() >= 2 )
    if ( getClique( path[path.size()-2] ).exists( node2 ) )
      path.pop_back();
    else
      break;

  while ( path.size() >= 2 )
    if ( getClique( path[1] ).exists( node1 ) )
      path.erase( path.begin() );
    else
      break;

  return path;
}


// ================================================================================
/// changes the set of nodes included into a given clique and returns the new set
// ================================================================================
template <typename Clique, typename Edge, typename Node>
const gumNodeList<Node>&
gumCliqueGraphTpl<Clique,Edge,Node>::setClique( const gumId& id,
    const gumNodeList<Node>& new_clique,
    gumCopyType copy_type ) {
  // get the current clique set
  gumNodeList<Node>& list = cliques[id];

  // modify it

  if ( copy_type == GUM_DEEP_COPY )
    list = new_clique;
  else
    list.shallowCopy( new_clique );

  // update the separators adjacent to the clique
  for ( gumEdgeListIterator<gumEdge> iter =
          gumUndiGraphTpl<Clique,Edge>::beginneighbourEdges( id );
        iter != gumUndiGraphTpl<Clique,Edge>::endEdges(); ++iter )
    updateSeparator( iter->first(), iter->second() );

  // return the list
  return list;
}


// ================================================================================
/// changes the set of nodes included into a given clique and returns the new set
// ================================================================================
template <typename Clique, typename Edge, typename Node> INLINE
const gumNodeList<Node>&
gumCliqueGraphTpl<Clique,Edge,Node>::setClique( const Clique& clique,
    const gumNodeList<Node>& new_clique,
    gumCopyType copy_type ) {
  return setClique( clique.getID(), new_clique, copy_type );
}


// ================================================================================
/// changes the set of nodes included into a given clique and returns the new set
// ================================================================================
template <typename Clique, typename Edge, typename Node>
const gumNodeList<Node>&
gumCliqueGraphTpl<Clique,Edge,Node>::addToClique( const gumId& id,
    const gumRefPtr<Node>& node,
    gumCopyType copy_type ) {
  // get the current clique set
  gumNodeList<Node>& list = cliques[id];
  gumId node_id = node->getID();

  // check if the node already exists, in which case throw an exception

  if ( list.exists( node_id ) )
    GUM_ERROR( gumDuplicateElement, "the clique set already contains the node" );

  // add the node
  if ( copy_type == GUM_DEEP_COPY )
    list.insert( *node );
  else
    list.shallowInsert( node );

  // update the separators adjacent to clique 'id'
  for ( gumEdgeListIterator<gumEdge> iter =
          gumUndiGraphTpl<Clique,Edge>::beginneighbourEdges( id );
        iter != gumUndiGraphTpl<Clique,Edge>::endEdges(); ++iter ) {
    gumId other_id = iter->other( id );

    if ( cliques[other_id].exists( node_id ) )
      separators[std::pair<gumId,gumId>
                 ( std::min( id,other_id ),std::max( id,other_id ) )].shallowInsert( node );
  }

  // return the list
  return list;
}


// ================================================================================
/// changes the set of nodes included into a given clique and returns the new set
// ================================================================================
template <typename Clique, typename Edge, typename Node> INLINE
const gumNodeList<Node>&
gumCliqueGraphTpl<Clique,Edge,Node>::addToClique( const Clique& clique,
    const gumRefPtr<Node>& node,
    gumCopyType copy_type ) {
  return addToClique( clique.getID(), node, copy_type );
}


// ================================================================================
/// remove a node from a clique
// ================================================================================
template <typename Clique, typename Edge, typename Node> INLINE
const gumNodeList<Node>&
gumCliqueGraphTpl<Clique,Edge,Node>::eraseFromClique( const gumId& id,
    const gumId& node ) {
  // remove the node from the clique
  try { cliques[id].erase( node ); }
  catch ( gumNotFound& ) { }

  // update the separators adjacent to clique 'id'
  for ( gumEdgeListIterator<gumEdge> iter =
          gumUndiGraphTpl<Clique,Edge>::beginneighbourEdges( id );
        iter != gumUndiGraphTpl<Clique,Edge>::endEdges(); ++iter ) {
    gumId other_id = iter->other( id );

    if ( cliques[other_id].exists( node ) )
      separators[std::pair<gumId,gumId>
                 ( std::min( id,other_id ),std::max( id,other_id ) )].erase( node );
  }

  return cliques[id];
}


// ================================================================================
/// remove a node from a clique
// ================================================================================
template <typename Clique, typename Edge, typename Node> INLINE
const gumNodeList<Node>&
gumCliqueGraphTpl<Clique,Edge,Node>::eraseFromClique( const gumId& id,
    const Node& node ) {
  return eraseFromClique( id, node.getID() );
}


// ================================================================================
/// remove a node from a clique
// ================================================================================
template <typename Clique, typename Edge, typename Node> INLINE
const gumNodeList<Node>&
gumCliqueGraphTpl<Clique,Edge,Node>::eraseFromClique( const Clique& clique,
    const Node& node ) {
  return eraseFromClique( clique.getID(), node );
}


// ================================================================================
/// returns the separator included in a given edge
// ================================================================================
template <typename Clique, typename Edge, typename Node> INLINE
const gumNodeList<Node>&
gumCliqueGraphTpl<Clique,Edge,Node>::getSeparator( const Edge& edge )
const  {
  return separators[std::pair<gumId,gumId>( edge.first(),
                    edge.second() )];
}


// ================================================================================
/// returns the separator included in an edge specified by its extremities
// ================================================================================
template <typename Clique, typename Edge, typename Node> INLINE
const gumNodeList<Node>&
gumCliqueGraphTpl<Clique,Edge,Node>::getSeparator( const gumId &node1,
    const gumId &node2 )
const  {
  return separators[std::pair<gumId,gumId>( std::min( node1,node2 ),
                    std::max( node1,node2 ) )];
}


// ================================================================================
/// DFS function for computing whether the running intersection property holds
// ================================================================================
template <typename Clique, typename Edge, typename Node>
const bool gumCliqueGraphTpl<Clique,Edge,Node>::runningIntersectionDFS
( const gumId& clique, const gumId& from,
  gumCliqueGraphTpl<Clique,Edge,Node>::gumRunningIntersect& infos_DFS ) const  {
  // check that no node in the clique belongs to the set of nodes belonging to
  // other connected components of the cliqueGraph
  const gumNodeList<Node>& nodes_clique = cliques[clique];

  for ( gumNodeListIterator<Node> iter = nodes_clique.begin();
        iter != nodes_clique.end(); ++iter )
    if ( infos_DFS.nodes_other_components.exists( iter->getID() ) )
      return false;

  // update the structure that keeps track of the cliques that still require
  // chains to access some of their nodes
  for ( gumNodeListIterator<Node> iter = nodes_clique.begin();
        iter != nodes_clique.end(); ++iter ) {
    if ( !infos_DFS.nodes_DFS_forbidden.exists( iter->getID() ) )
      infos_DFS.cliques_DFS_chain[clique].erase( iter->getID() );
  }

  // if the clique had already been visited, no need to see its neighbours
  // or to update the list of visited nodes
  if ( infos_DFS.visited_cliques[clique] )
    return true;

  // update the list of nodes visited during the DFS
  for ( gumNodeListIterator<Node> iter = nodes_clique.begin();
        iter != nodes_clique.end(); ++iter ) {
    if ( !infos_DFS.nodes_DFS_seen.exists( iter->getID() ) )
      infos_DFS.nodes_DFS_seen.insert( iter->getID(), true );
  }

  // update the fact that the clique has been visited
  infos_DFS.visited_cliques[clique] = true;

  // check the neighbours that are different from "from" and that have not
  // been visited yet
  for ( gumEdgeListIterator<gumEdge> iter =
          gumUndiGraphTpl<Clique,Edge>::beginneighbourEdges( clique );
        iter != gumUndiGraphTpl<Clique,Edge>::endEdges(); ++iter ) {
    const gumId& otherID = iter->other( clique );

    if ( otherID != from ) {
      // update the list of forbidden nodes in the DFS, i.e., the nodes that
      // belong to the clique but not to the separator
      const gumNodeList<Node>& from_separ =
        separators[std::pair<gumId,gumId>( std::min( clique,otherID ),
                                           std::max( clique,otherID ) )];

      for ( gumNodeListIterator<Node> iter_clique = nodes_clique.begin();
            iter_clique != nodes_clique.end(); ++iter_clique ) {
        if ( !from_separ.exists( iter_clique->getID() ) )
          infos_DFS.nodes_DFS_forbidden.insert( iter_clique->getID(), true );
      }

      // check the neighbour
      if ( !runningIntersectionDFS( otherID, clique, infos_DFS ) )
        return false;

      // remove from the forbidden list the nodes that belong to clique
      for ( gumNodeListIterator<Node> iter_clique = nodes_clique.begin();
            iter_clique != nodes_clique.end(); ++iter_clique )
        infos_DFS.nodes_DFS_forbidden.erase( iter_clique->getID() );
    }
  }

  // when a node is terminal, i.e., it has at most one neighbour, add its nodes
  // to the nodes forbidden by the DFS. It will prevent non adjacent extremal
  // cliques to contain the same node while this one does not belong to any
  // separator
  if ( gumUndiGraphTpl<Clique,Edge>::getneighbourEdges( clique ).size() <= 1 )
    for ( gumNodeListIterator<Node> iter_clique = nodes_clique.begin();
          iter_clique != nodes_clique.end(); ++iter_clique )
      if ( !infos_DFS.nodes_DFS_forbidden.exists( iter_clique->getID() ) )
        infos_DFS.nodes_DFS_forbidden.insert( iter_clique->getID(), true );

  // here everything was OK. A priori, the running intersection property holds
  return true;
}


// ================================================================================
/// indicates whether the running intersection property holds
// ================================================================================
template <typename Clique, typename Edge, typename Node>
const bool gumCliqueGraphTpl<Clique,Edge,Node>::hasRunningIntersection()
const  {
  // create a gumRunningIntersect structure and initialize it
  gumRunningIntersect infos_DFS;
  infos_DFS.visited_cliques = gumUndiGraphTpl<Clique,Edge>::hashMapNodes( false );

  for ( gumHashTableIterator< gumId, gumNodeList<Node> > iter = cliques.begin();
        iter != cliques.end(); ++iter ) {
    infos_DFS.cliques_DFS_chain.insert( iter.key(),
                                        gumNodeList<Node>() ).shallowCopy( *iter );
  }

  // while there exist unvisited cliques, perform a DFS on them
  for ( gumHashTableIterator<gumId,bool> iter_DFS=infos_DFS.visited_cliques.begin();
        iter_DFS != infos_DFS.visited_cliques.end(); ++iter_DFS )
    if ( *iter_DFS == false ) {
      // no nodes are forbidden a priori in the DFS
      infos_DFS.nodes_DFS_forbidden.clear();

      // no node has already been seen in the DFS
      infos_DFS.nodes_DFS_seen.clear();

      // here iter_DFS points on a clique that has not been visited yet

      if ( !runningIntersectionDFS( iter_DFS.key(), iter_DFS.key(), infos_DFS ) )
        return false;

      // the nodes that were seen during the DFS belong to a connected component
      // that is different from the connected components of the subsequent DFS
      for ( gumHashTableIterator<gumId,bool> iter_seen =
              infos_DFS.nodes_DFS_seen.begin();
            iter_seen != infos_DFS.nodes_DFS_seen.end(); ++iter_seen )
        if ( !infos_DFS.nodes_other_components.exists( iter_seen.key() ) )
          infos_DFS.nodes_other_components.insert( iter_seen.key(),true );
    }

  // check that no clique requires an additional chain to guarrantee the
  // running intersection property
  for ( gumHashTableIterator< gumId,gumNodeList<Node> > iter =
          infos_DFS.cliques_DFS_chain.begin();
        iter != infos_DFS.cliques_DFS_chain.end(); ++iter )
    if ( !iter->empty() )
      return false;

  return true;
}


// ================================================================================
/// indicates whether the graph is a join tree
// ================================================================================
template <typename Clique, typename Edge, typename Node> INLINE
const bool gumCliqueGraphTpl<Clique,Edge,Node>::isJoinTree() const  {
  return ( !gumUndiGraphTpl<Clique,Edge>::hasCycle() && hasRunningIntersection() );
}


// ================================================================================
/// removes all the nodes from the graph (as well as their adjacent edges/arcs)
// ================================================================================
template <typename Clique, typename Edge, typename Node>
void gumCliqueGraphTpl<Clique,Edge,Node>::clearNodes() {
  gumUndiGraphTpl<Clique,Edge>::clearNodes();
  cliques.clear();
  separators.clear();
}


// ================================================================================
/// removes all the edges from the clique graph
// ================================================================================
template <typename Clique, typename Edge, typename Node>
void gumCliqueGraphTpl<Clique,Edge,Node>::clearEdges() {
  gumUndiGraphTpl<Clique,Edge>::clearEdges();
  separators.clear();
}


// ================================================================================
/// checks whether two clique graphs are equal
// ================================================================================
template <typename Clique, typename Edge, typename Node>
const bool gumCliqueGraphTpl<Clique,Edge,Node>::operator==
( const gumCliqueGraphTpl<Clique,Edge,Node> &from ) const {
  // check if both graphical structures are identical
  if ( gumUndiGraphTpl<Clique,Edge>::operator!= ( from ) )
    return false;

  // check if the cliques are identical
  for ( gumHashTableIterator< gumId, gumNodeList<Node> > iter = cliques.begin();
        iter != cliques.end(); ++iter )
    if ( *iter != from.cliques[iter.key()] )
      return false;

  return true;
}


// ================================================================================
/// checks whether two clique graphs are different
// ================================================================================
template <typename Clique, typename Edge, typename Node> INLINE
const bool gumCliqueGraphTpl<Clique,Edge,Node>::operator!=
( const gumCliqueGraphTpl<Clique,Edge,Node> &from ) const {
  return ( !operator== ( from ) );
}

// ==============================================================================
/// a << operator for gumMixedGraphTpl
// ==============================================================================
template <typename Clique, typename Edge, typename Node>
std::ostream& operator<< ( std::ostream& stream,
                           const gumCliqueGraphTpl<Clique,Edge,Node>& graph ) {
  stream << "list of nodes:"<<std::endl;

  for ( gumNodeListIterator<Clique> iter_node = graph.beginNodes();
        iter_node != graph.endNodes(); ++iter_node ) {
    stream << " -- node: " << *iter_node <<std::endl
    << "    clique:";
    const gumNodeList<Node>& clique = graph.getClique( iter_node.getID() );

    for ( gumNodeListIterator<Node> iter = clique.begin();
          iter != clique.end(); ++iter )
      stream << "  " << *iter;

    stream <<std::endl;
  }

  stream << "\n\nlist of edges:\n";

  for ( gumEdgeListIterator<Edge> iter_edge = graph.beginEdges();
        iter_edge != graph.endEdges(); ++iter_edge )
    stream << *iter_edge << "  ";

  stream << std::endl;

  return stream;
}



#endif /* DOXYGEN_SHOULD_SKIP_THIS */
