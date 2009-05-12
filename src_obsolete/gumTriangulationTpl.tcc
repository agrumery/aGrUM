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
 * @brief Template implementations of graph triangulations
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */


#include <sstream>
#include <limits>
#include <cmath>

#ifndef DOXYGEN_SHOULD_SKIP_THIS


/* ============================================================================== */
/* ============================================================================== */
/* ===   CLASS FOR RETRIEVING SIMPLICIAL, ALMOST AND QUASI SIMPLICIAL NODES   === */
/* ============================================================================== */
/* ============================================================================== */

// ================================================================================
/// constructor. initialize the simplicial set w.r.t. a given graph
// ================================================================================
template <typename Node>
gumSimplicialSet<Node>::gumSimplicialSet
( gumUndiGraphTpl<Node,gumEdge>& theGraph,
  const gumHashTable<gumId,float>& theModalities,
  gumHashTable<gumId,float>& theLogWeights, float theRatio, float theThreshold ) :
    graph( theGraph ), log_weights( theLogWeights ), log_modalities( theModalities ),
    simplicial_nodes( graph.getNbrNodes() ),
    almost_simplicial_nodes( std::less<float>(), false, graph.getNbrNodes() ),
    quasi_simplicial_nodes( std::less<float>(), false, graph.getNbrNodes() ),
    containing_list( graph.getNbrNodes() ),
    nb_triangles( graph.getNbrNodes() * graph.getNbrNodes() / 2 ),
    nb_adjacent_neighbours( graph.getNbrNodes() ),
    quasi_ratio( theRatio ), log_threshold( log( 1 + theThreshold ) ) {
  // for debugging purposes
  GUM_CONSTRUCTOR( gumSimplicialSet );

  // if the graph is empty, do nothing

  if ( graph.getNbrNodes() == 0 ) return;

  // set the weights of the nodes and the initial tree_width (min of the weights)
  log_tree_width = std::numeric_limits<float>::max();

  log_weights.clear();

  for ( gumNodeListIterator<Node> iterX = graph.beginNodes();
        iterX != graph.endNodes(); ++iterX ) {
    float log_weight = log_modalities[iterX.getID()];

    for ( gumEdgeListIterator<gumEdge> iterY=graph.beginneighbourEdges( iterX.getID() );
          iterY != graph.endEdges(); ++iterY )
      log_weight += log_modalities[iterY->other( iterX.getID() )];

    log_weights.insert( iterX.getID(), log_weight );

    if ( log_tree_width > log_weight )
      log_tree_width = log_weight;
  }

  // initialize the nb_triangles so that there is no need to check whether
  // nb_triangles need new insertions
  std::pair<gumId,gumId> thePair;

  for ( gumEdgeListIterator<gumEdge> iterY = graph.beginEdges();
        iterY != graph.endEdges(); ++iterY ) {
    thePair.first  = iterY->first();
    thePair.second = iterY->second();
    nb_triangles.insert( thePair,0 );
  }

  for ( gumNodeListIterator<Node> iterX = graph.beginNodes();
        iterX != graph.endNodes(); ++iterX ) {
    gumId node_id1 = iterX.getID();
    nb_adjacent_neighbours.insert( node_id1,0 );
    containing_list.insert( node_id1,GUM_NO_LIST );
    changed_status.insert( node_id1,true );
  }

  // set the nb_triangles and the nb_adjacent_neighbours: for each triangle,
  // update the nb_triangles. To count the triangles only once, parse for each
  // node X the set of its neighbours Y,Z that are adjacent to each other and
  // such that the Id of Y and Z are greater than X.
  gumEdgeListIterator<gumEdge> iterZ;

  for ( gumNodeListIterator<Node> iterX = graph.beginNodes();
        iterX != graph.endNodes(); ++iterX ) {
    gumId node_idX = iterX.getID();
    unsigned int& nb_adjacent_neighbours_idX = nb_adjacent_neighbours[node_idX];

    for ( gumEdgeListIterator<gumEdge> iterY = graph.beginneighbourEdges( node_idX );
          iterY != graph.endEdges(); ++iterY )
      if ( iterY->other( node_idX ) > node_idX ) {
        gumId node_idY = iterY->other( node_idX );
        unsigned int& nb_adjacent_neighbours_idY = nb_adjacent_neighbours[node_idY];
        iterZ = iterY;

        for ( ++iterZ; iterZ != graph.endEdges(); ++iterZ )
          if (( iterZ->other( node_idX ) > node_idX ) &&
              graph.existsEdge( node_idY,iterZ->other( node_idX ) ) ) {
            gumId node_idZ = iterZ->other( node_idX );
            ++nb_adjacent_neighbours_idX;
            ++nb_adjacent_neighbours_idY;
            ++nb_adjacent_neighbours[node_idZ];
            thePair.first  = node_idX;
            thePair.second = node_idY;
            ++nb_triangles[thePair];
            thePair.second = node_idZ;
            ++nb_triangles[thePair];
            thePair.first  = std::min( node_idY,node_idZ );
            thePair.second = std::max( node_idY,node_idZ );
            ++nb_triangles[thePair];
          }
      }
  }
}


// ================================================================================
/// copy constructor
// ================================================================================
template <typename Node>
gumSimplicialSet<Node>::gumSimplicialSet( const gumSimplicialSet<Node>& from ) {
  // #### TODO
  GUM_ERROR( gumFatalError, "not implemented yet" );
}


// ================================================================================
/// destructor
// ================================================================================
template <typename Node>
gumSimplicialSet<Node>::~gumSimplicialSet() {
  // for debugging purposes
  GUM_DESTRUCTOR( gumSimplicialSet );
}


// ================================================================================
/// adds the necessary edges so that node 'id' and its neighbours form a clique
// ================================================================================
template <typename Node>
void gumSimplicialSet<Node>::makeClique( const gumId& id ) {
  // to make id be a clique, we may have to add edges. Hence, this will create
  // new triangles and we should update the number of triangles passing through
  // the new edges. Moreover, we should also update the number of adjacent neighbours
  // for each node involved in these new triangles as well as the new weights of
  // the nodes. Finally, we should update the simplicial, almost and quasi
  // simplicial lists. However, to optimize the code, we use a lazy list update: we
  // just update a hashtable indicating whether a list update should be performed
  // for a given node. This enables performing the updates only when necessary.

  // if the node is known to be simplicial, there is nothing to do
  if ( simplicial_nodes.exists( id ) ) {
    if ( changed_status.exists( id ) )
      changed_status.erase( id );

    return;
  } else if ( almost_simplicial_nodes.exists( id ) ) {
    // get the neighbour that does not form a clique with the other neighbours
    unsigned int nb_adj = graph.getneighbourEdges( id ).size();
    unsigned int nb_almost = (( nb_adj - 1 ) * ( nb_adj - 2 ) ) / 2;
    unsigned int nb = nb_adjacent_neighbours[id];
    std::pair<gumId,gumId> thePair;
    gumId node1 = 0;

    for ( gumEdgeListIterator<gumEdge> iterEdge = graph.beginneighbourEdges( id );
          iterEdge != graph.endEdges(); ++iterEdge ) {
      node1 = iterEdge->other( id );
      thePair.first  = std::min( id,node1 );
      thePair.second = std::max( id,node1 );

      if ( nb_almost == nb - nb_triangles[thePair] ) {
        break; // we found the neighbour we were looking for
      }
    }

    float log_modal_node1 = log_modalities[node1];

    float& log_weights_node1 = log_weights[node1];

    // update the number of triangles of the edges and keep track of the
    // nodes involved.
    unsigned int nb_n1 = 0;

    for ( gumEdgeListIterator<gumEdge> iter2 = graph.beginneighbourEdges( id );
          iter2 != graph.endEdges(); ++iter2 ) {
      gumId node2 = iter2->other( id );

      if (( node2 != node1 ) && !graph.existsEdge( node1,node2 ) ) {
        // add the edge
        graph.insertEdge( gumEdge( node1,node2 ) );
	if(we_want_fill_ins) {
	  fill_ins_list.insert( gumEdge( node1,node2 ) );
	}

        if ( !changed_status.exists( node2 ) ) changed_status.insert( node2,true );

        log_weights_node1  += log_modalities[node2];

        log_weights[node2] += log_modal_node1;

        thePair.first  = std::min( node1,node2 );

        thePair.second = std::max( node1,node2 );

        nb_triangles.insert( thePair,0 );

        // for all common neighbours of node1 and node2, update the number of
        // triangles as well as the number of adjacent neighbours
        unsigned int nb_n2 = 0;

        if ( graph.getneighbourEdges( node1 ).size() <=
             graph.getneighbourEdges( node2 ).size() ) {
          for ( gumEdgeListIterator<gumEdge> iterEdge=graph.beginneighbourEdges( node1 );
                iterEdge != graph.endEdges(); ++iterEdge )
            if ( graph.existsEdge( iterEdge->other( node1 ), node2 ) ) {
              // here iter->other (node1) is a neighbour of node1 and node2
              gumId neighbour = iterEdge->other( node1 );
              ++nb_n1;
              ++nb_n2;
              ++nb_adjacent_neighbours[neighbour];

              thePair.first  = std::min( node1,neighbour );
              thePair.second = std::max( node1,neighbour );
              ++nb_triangles[thePair];
              thePair.first  = std::min( node2,neighbour );
              thePair.second = std::max( node2,neighbour );
              ++nb_triangles[thePair];

              if ( !changed_status.exists( neighbour ) )
                changed_status.insert( neighbour,true );
            }
        } else {
          for ( gumEdgeListIterator<gumEdge> iterEdge=graph.beginneighbourEdges( node2 );
                iterEdge != graph.endEdges(); ++iterEdge )
            if ( graph.existsEdge( iterEdge->other( node2 ), node1 ) ) {
              // here iter->other (node2) is a neighbour of node1 and node2
              gumId neighbour = iterEdge->other( node2 );
              ++nb_n1;
              ++nb_n2;
              ++nb_adjacent_neighbours[neighbour];

              thePair.first  = std::min( node1,neighbour );
              thePair.second = std::max( node1,neighbour );
              ++nb_triangles[thePair];
              thePair.first  = std::min( node2,neighbour );
              thePair.second = std::max( node2,neighbour );
              ++nb_triangles[thePair];

              if ( !changed_status.exists( neighbour ) )
                changed_status.insert( neighbour,true );
            }
        }

        nb_adjacent_neighbours[node2] += nb_n2;

        thePair.first  = std::min( node1,node2 );
        thePair.second = std::max( node1,node2 );
        nb_triangles[thePair] += nb_n2;
      }
    }

    if ( !changed_status.exists( node1 ) )
      changed_status.insert( node1,true );

    nb_adjacent_neighbours[node1] += nb_n1;
  } else {
    // update the number of triangles of the edges and keep track of the
    // nodes involved.
    std::pair<gumId,gumId> thePair;
    gumEdgeListIterator<gumEdge> iterEdge2;

    for ( gumEdgeListIterator<gumEdge> iter1 = graph.beginneighbourEdges( id );
          iter1 != graph.endEdges(); ++iter1 ) {
      gumId node1 = iter1->other( id );
      bool node1_status = false;
      float log_modal_node1 = log_modalities[node1];
      float& log_weights_node1 = log_weights[node1];
      unsigned int nb_n1 = 0;
      iterEdge2 = iter1;

      for ( ++iterEdge2; iterEdge2 != graph.endEdges(); ++iterEdge2 ) {
        gumId node2 = iterEdge2->other( id );

        if ( !graph.existsEdge( node1,node2 ) ) {
          // add the edge
          graph.insertEdge( gumEdge( node1,node2 ) );
	  if(we_want_fill_ins) {
	    fill_ins_list.insert( gumEdge( node1,node2 ) );
	  }
          if ( !changed_status.exists( node2 ) ) changed_status.insert( node2,true );

          node1_status = true;

          log_weights_node1  += log_modalities[node2];

          log_weights[node2] += log_modal_node1;

          thePair.first  = std::min( node1,node2 );

          thePair.second = std::max( node1,node2 );

          nb_triangles.insert( thePair,0 );

          // for all common neighbours of node1 and node2, update the number of
          // triangles as well as the number of adjacent neighbours
          unsigned int nb_n2 = 0;

          if ( graph.getneighbourEdges( node1 ).size() <=
               graph.getneighbourEdges( node2 ).size() ) {
            for ( gumEdgeListIterator<gumEdge>
                  iterEdge = graph.beginneighbourEdges( node1 );
                  iterEdge != graph.endEdges(); ++iterEdge )
              if ( graph.existsEdge( iterEdge->other( node1 ), node2 ) ) {
                // here iterEdge->other (node1) is a neighbour of
                // both node1 and node2
                gumId neighbour = iterEdge->other( node1 );
                ++nb_n1;
                ++nb_n2;
                ++nb_adjacent_neighbours[neighbour];

                thePair.first  = std::min( node1,neighbour );
                thePair.second = std::max( node1,neighbour );
                ++nb_triangles[thePair];
                thePair.first  = std::min( node2,neighbour );
                thePair.second = std::max( node2,neighbour );
                ++nb_triangles[thePair];

                if ( !changed_status.exists( neighbour ) )
                  changed_status.insert( neighbour,true );
              }
          } else {
            for ( gumEdgeListIterator<gumEdge>
                  iterEdge = graph.beginneighbourEdges( node2 );
                  iterEdge != graph.endEdges(); ++iterEdge )
              if ( graph.existsEdge( iterEdge->other( node2 ), node1 ) ) {
                // here iterEdge->other (node2) is a neighbour of
                // both node1 and node2
                gumId neighbour = iterEdge->other( node2 );
                ++nb_n1;
                ++nb_n2;
                ++nb_adjacent_neighbours[neighbour];

                thePair.first  = std::min( node1,neighbour );
                thePair.second = std::max( node1,neighbour );
                ++nb_triangles[thePair];
                thePair.first  = std::min( node2,neighbour );
                thePair.second = std::max( node2,neighbour );
                ++nb_triangles[thePair];

                if ( !changed_status.exists( neighbour ) )
                  changed_status.insert( neighbour,true );
              }
          }

          nb_adjacent_neighbours[node2] += nb_n2;

          thePair.first  = std::min( node1,node2 );
          thePair.second = std::max( node1,node2 );
          nb_triangles[thePair] += nb_n2;
        }
      }

      nb_adjacent_neighbours[node1] += nb_n1;

      if ( node1_status && !changed_status.exists( node1 ) )
        changed_status.insert( node1,true );
    }
  }

  // update the changed_status of node id as well as its containing list
  if ( !simplicial_nodes.exists( id ) ) {
    if ( changed_status.exists( id ) )
      changed_status.erase( id );

    switch ( containing_list[id] ) {
      case GUM_ALMOST_SIMPLICIAL:
        almost_simplicial_nodes.eraseByVal( id );
        break;
      case GUM_QUASI_SIMPLICIAL:
        quasi_simplicial_nodes.eraseByVal( id );
        break;
      default:
        break;
    }

    simplicial_nodes.insert( id,true );

    containing_list[id] = GUM_SIMPLICIAL;
  } else {
    if ( changed_status.exists( id ) ) {
      changed_status.erase( id );
    }
  }
}


// ================================================================================
/// removes a node and its adjacent edges from the underlying graph
// ================================================================================
template <typename Node>
void gumSimplicialSet<Node>::eraseClique( const gumId& id ) {
  // check if the node we wish to remove actually belongs to the graph
  if ( !graph.existsNode( id ) )
    GUM_ERROR( gumNotFound, "the node does not belong to the graph" );

  // check that node id is actually a clique
  unsigned int nb_adj = graph.getneighbourEdges( id ).size();

  if ( nb_adjacent_neighbours[id] != ( nb_adj * ( nb_adj - 1 ) ) / 2 )
    GUM_ERROR( gumNotFound, "the node is not a clique" );

  // remove the node and its adjacent edges
  float log_modal_id = log_modalities[id];

  std::pair<gumId,gumId> thePair;

  gumEdgeListIterator<gumEdge> iter2;

  for ( gumEdgeListIterator<gumEdge> iter1 = graph.beginneighbourEdges( id );
        iter1 != graph.endEdges(); ++iter1 ) {
    gumId node1 = iter1->other( id );
    nb_adjacent_neighbours[node1] -= nb_adj - 1;
    log_weights[node1] -= log_modal_id;

    if ( !changed_status.exists( node1 ) )
      changed_status.insert( node1,true );

    thePair.first  = std::min( node1,id );

    thePair.second = std::max( node1,id );

    nb_triangles.erase( thePair );

    iter2 = iter1;

    for ( ++iter2; iter2 != graph.endEdges(); ++iter2 ) {
      gumId node2 = iter2->other( id );
      thePair.first  = std::min( node1,node2 );
      thePair.second = std::max( node1,node2 );
      --nb_triangles[thePair];

      if ( !changed_status.exists( node2 ) )
        changed_status.insert( node2,true );
    }
  }

  log_tree_width = std::max( log_tree_width, log_weights[id] );

  switch ( containing_list[id] ) {
    case GUM_SIMPLICIAL:
      simplicial_nodes.erase( id );
      break;
    case GUM_ALMOST_SIMPLICIAL:
      almost_simplicial_nodes.eraseByVal( id );
      break;
    case GUM_QUASI_SIMPLICIAL:
      quasi_simplicial_nodes.eraseByVal( id );
      break;
    default:
      break;
  }

  nb_adjacent_neighbours.erase( id );

  containing_list.erase( id );
  changed_status.erase( id );
  graph.eraseNode( id );
  log_weights.erase( id );
}


// ================================================================================
/// removes a node and its adjacent edges from the underlying graph
// ================================================================================
template <typename Node>
void gumSimplicialSet<Node>::eraseNode( const gumId& id ) {
  // check if the node we wish to remove actually belongs to the graph
  if ( !graph.existsNode( id ) )
    GUM_ERROR( gumNotFound, "the node does not belong to the graph" );

  // remove the node and its adjacent edges
  for ( gumEdgeListIterator<gumEdge> iter = graph.beginneighbourEdges( id );
        iter != graph.endEdges(); ++iter )
    eraseEdge( *iter );

  switch ( containing_list[id] ) {
    case GUM_SIMPLICIAL:
      simplicial_nodes.erase( id );
      break;
    case GUM_ALMOST_SIMPLICIAL:
      almost_simplicial_nodes.eraseByVal( id );
      break;
    case GUM_QUASI_SIMPLICIAL:
      quasi_simplicial_nodes.eraseByVal( id );
      break;
    default:
      break;
  }

  nb_adjacent_neighbours.erase( id );

  containing_list.erase( id );
  changed_status.erase( id );
  graph.eraseNode( id );
  log_weights.erase( id );
}


// ================================================================================
/// removes a node and its adjacent edges from the underlying graph
// ================================================================================
template <typename Node>
void gumSimplicialSet<Node>::eraseEdge( const gumEdge& edge ) {
  // check if the edge we wish to remove actually belongs to the graph
  if ( !graph.existsEdge( edge ) )
    GUM_ERROR( gumNotFound, "the edge does not belong to the graph" );

  // get the extremal nodes of the edge
  gumId node1 = edge.first();

  gumId node2 = edge.second();

  // remove the edge
  graph.eraseEdge( edge );

  std::pair<gumId,gumId> thePair;

  thePair.first  = node1;

  thePair.second = node2;

  nb_triangles.erase( thePair );

  // update the log_weights of both nodes
  log_weights[node1] -= log_modalities[node2];

  log_weights[node2] -= log_modalities[node1];

  // udate the number of triangles and the adjacent neighbours
  unsigned int nb_neigh_n1_n2 = 0;

  for ( gumEdgeListIterator<gumEdge> iter1 = graph.beginneighbourEdges( node1 );
        iter1 != graph.endEdges(); ++iter1 ) {
    gumId othernode = iter1->other( node1 );

    if ( graph.existsEdge( node2,othernode ) ) {
      // udate the number of triangles passing through the egdes of the graph
      thePair.first  = std::min( node1,othernode );
      thePair.second = std::max( node1,othernode );
      --nb_triangles[thePair];
      thePair.first  = std::min( node2,othernode );
      thePair.second = std::max( node2,othernode );
      --nb_triangles[thePair];

      // update the neighbours
      ++nb_neigh_n1_n2;
      --nb_adjacent_neighbours[othernode];

      if ( !changed_status.exists( othernode ) )
        changed_status.insert( othernode,true );
    }
  }

  nb_adjacent_neighbours[node1] -= nb_neigh_n1_n2;

  nb_adjacent_neighbours[node2] -= nb_neigh_n1_n2;

  if ( !changed_status.exists( node1 ) )
    changed_status.insert( node1,true );

  if ( !changed_status.exists( node2 ) )
    changed_status.insert( node2,true );
}


// ================================================================================
/// adds a new edge to the graph and recomputes the simplicial set
// ================================================================================
template <typename Node>
void gumSimplicialSet<Node>::insertEdge( const gumEdge& edge ) {
  // if the edge already exists, do nothing
  if ( graph.existsEdge( edge ) )
    return;

  // get the extremal nodes of the edge
  gumId node1 = edge.first();

  gumId node2 = edge.second();

  // update the log_weights of both nodes
  log_weights[node1] += log_modalities[node2];

  log_weights[node2] += log_modalities[node1];

  // udate the number of triangles and the adjacent neighbours
  std::pair<gumId,gumId> thePair;

  unsigned int nb_triangle_in_new_edge = 0;

  unsigned int nb_neigh_n1_n2 = 0;

  for ( gumEdgeListIterator<gumEdge> iter1 = graph.beginneighbourEdges( node1 );
        iter1 != graph.endEdges(); ++iter1 ) {
    gumId othernode = iter1->other( node1 );

    if ( graph.existsEdge( node2,othernode ) ) {
      // udate the number of triangles passing through the egdes of the graph
      thePair.first  = std::min( node1,othernode );
      thePair.second = std::max( node1,othernode );
      ++nb_triangles[thePair];
      thePair.first  = std::min( node2,othernode );
      thePair.second = std::max( node2,othernode );
      ++nb_triangles[thePair];
      ++nb_triangle_in_new_edge;

      // update the neighbours
      ++nb_neigh_n1_n2;
      ++nb_adjacent_neighbours[othernode];

      if ( !changed_status.exists( othernode ) )
        changed_status.insert( othernode,true );
    }
  }

  nb_adjacent_neighbours[node1] += nb_neigh_n1_n2;

  nb_adjacent_neighbours[node2] += nb_neigh_n1_n2;

  // add the edge
  graph.insertEdge( edge );
  thePair.first  = node1;
  thePair.second = node2;
  nb_triangles.insert( thePair, nb_triangle_in_new_edge );

  if ( !changed_status.exists( node1 ) )
    changed_status.insert( node1,true );

  if ( !changed_status.exists( node2 ) )
    changed_status.insert( node2,true );
}


// ================================================================================
/// put node id in the correct simplicial/almost simplicial/quasi simplicial list
// ================================================================================
template <typename Node>
void gumSimplicialSet<Node>::updateList( gumId id ) {
  // check if the node belongs to the graph
  if ( !graph.existsNode( id ) )
    GUM_ERROR( gumNotFound, "the node could not be found" );

  // check if the status of the node has changed
  if ( !changed_status.exists( id ) ) return;

  changed_status.erase( id );

  typename gumSimplicialSet<Node>::gumBelong& belong = containing_list[id];

  // check if the node should belong to the simplicial set
  unsigned int nb_adj = graph.getneighbourEdges( id ).size();

  if ( nb_adjacent_neighbours[id] == ( nb_adj * ( nb_adj - 1 ) ) / 2 ) {
    if ( belong != GUM_SIMPLICIAL ) {
      if ( belong == GUM_ALMOST_SIMPLICIAL )
        almost_simplicial_nodes.eraseByVal( id );
      else if ( belong == GUM_QUASI_SIMPLICIAL )
        quasi_simplicial_nodes.eraseByVal( id );

      simplicial_nodes.insert( id, true );

      belong = GUM_SIMPLICIAL;
    }

    return;
  }

  // check if the node should belong to the almost simplicial set
  unsigned int nb_almost = (( nb_adj - 1 ) * ( nb_adj - 2 ) ) / 2;

  unsigned int nb = nb_adjacent_neighbours[id];

  std::pair<gumId,gumId> thePair;

  for ( gumEdgeListIterator<gumEdge> iter = graph.beginneighbourEdges( id );
        iter != graph.endEdges(); ++iter ) {
    gumId other_id = iter->other( id );
    thePair.first  = std::min( id,other_id );
    thePair.second = std::max( id,other_id );

    if ( nb_almost == nb - nb_triangles[thePair] ) {
      // the node is an almost simplicial node
      if ( belong != GUM_ALMOST_SIMPLICIAL ) {
        if ( belong == GUM_SIMPLICIAL )
          simplicial_nodes.erase( id );
        else if ( belong == GUM_QUASI_SIMPLICIAL )
          quasi_simplicial_nodes.eraseByVal( id );

        almost_simplicial_nodes.insert( log_weights[id], id );

        belong = GUM_ALMOST_SIMPLICIAL;
      } else
        almost_simplicial_nodes.setPriorityByVal( id, log_weights[id] );

      return;
    }
  }

  // check if the node should belong to the quasi simplicial nodes
  if ( nb_adjacent_neighbours[id] / (( nb_adj * ( nb_adj - 1 ) ) / 2 ) >= quasi_ratio ) {
    if ( belong != GUM_QUASI_SIMPLICIAL ) {
      if ( belong == GUM_SIMPLICIAL )
        simplicial_nodes.erase( id );
      else if ( belong == GUM_ALMOST_SIMPLICIAL )
        almost_simplicial_nodes.eraseByVal( id );

      quasi_simplicial_nodes.insert( log_weights[id], id );

      belong = GUM_QUASI_SIMPLICIAL;
    } else
      quasi_simplicial_nodes.setPriorityByVal( id, log_weights[id] );

    return;
  }

  // the node does not belong to any list, so remove the node from its current list
  if ( belong == GUM_QUASI_SIMPLICIAL )
    quasi_simplicial_nodes.eraseByVal( id );
  else if ( belong == GUM_ALMOST_SIMPLICIAL )
    almost_simplicial_nodes.eraseByVal( id );
  else if ( belong == GUM_SIMPLICIAL )
    simplicial_nodes.erase( id );
  else
    belong = GUM_NO_LIST;
}


// ================================================================================
/// indicates whether a given node is a simplicial node
// ================================================================================
template <typename Node>
const bool gumSimplicialSet<Node>::isSimplicial( const gumId& id ) {
  // check if the node belongs to the simplicial list
  if ( simplicial_nodes.exists( id ) && !changed_status( id ) )
    return true;

  // update the list to which the node belongs
  updateList( id );

  return simplicial_nodes.exists( id );
}


// ================================================================================
/// indicates whether there exists a simplicial node
// ================================================================================
template <typename Node>
const bool gumSimplicialSet<Node>::hasSimplicialNode() {
  if ( !simplicial_nodes.empty() )
    return true;
  else {
    // check if a node can enter the simplicial list
    for ( gumHashTableIterator<gumId,bool> iter = changed_status.begin();
          iter != changed_status.end(); ++iter ) {
      updateList( iter.key() );

      if ( !simplicial_nodes.empty() )
        return true;
    }
  }

  return false;
}


// ================================================================================
/// indicates whether there exists an almost simplicial node
// ================================================================================
template <typename Node>
const bool gumSimplicialSet<Node>::hasAlmostSimplicialNode() {
  // set the limit weight value
  double limit = log_tree_width + log_threshold;

  // update the elements currently in the almost simplicial list that may
  // now be contained in another list

  for ( gumHashTableIterator<gumId,bool> iter = changed_status.begin();
        iter != changed_status.end(); ++iter ) {
    if ( almost_simplicial_nodes.exists( iter.key() ) )
      updateList( iter.key() );
  }

  // check the current almost simplicial list
  if ( !almost_simplicial_nodes.empty() &&
       ( log_weights[almost_simplicial_nodes.top()] <= limit ) )
    return true;

  // if the almost simplicial list does not contain any node that has a low
  // weight, check if a node can enter the almost simplicial list
  for ( gumHashTableIterator<gumId,bool> iter = changed_status.begin();
        iter != changed_status.end(); ++iter ) {
    updateList( iter.key() );

    if ( !almost_simplicial_nodes.empty() &&
         ( log_weights[almost_simplicial_nodes.top()] <= limit ) )
      return true;
  }

  return false;
}


// ================================================================================
/// indicates whether there exists a quasi simplicial node
// ================================================================================
template <typename Node>
const bool gumSimplicialSet<Node>::hasQuasiSimplicialNode() {
  // set the limit weight value
  double limit = log_tree_width + log_threshold;

  // update the elements currently in the quasi simplicial list that may
  // now be contained in another list

  for ( gumHashTableIterator<gumId,bool> iter = changed_status.begin();
        iter != changed_status.end(); ++iter ) {
    if ( quasi_simplicial_nodes.exists( iter.key() ) )
      updateList( iter.key() );
  }

  // check the current quasi simplicial list
  if ( !quasi_simplicial_nodes.empty() &&
       ( log_weights[quasi_simplicial_nodes.top()] <= limit ) )
    return true;

  // if the quasi simplicial list does not caontain any node that has a low
  // weight, check if a node can enter the quasi simplicial list
  for ( gumHashTableIterator<gumId,bool> iter = changed_status.begin();
        iter != changed_status.end(); ++iter ) {
    updateList( iter.key() );

    if ( !quasi_simplicial_nodes.empty() &&
         ( log_weights[quasi_simplicial_nodes.top()] <= limit ) )
      return true;
  }

  return false;
}


// ================================================================================
/// gets a simplicial node
// ================================================================================
template <typename Node> INLINE
const gumId& gumSimplicialSet<Node>::getBestSimplicialNode() {
  if ( !hasSimplicialNode() )
    GUM_ERROR( gumNotFound, "no simplicial node could be found" );

  return simplicial_nodes.begin().key();
}


// ================================================================================
/// gets an almost simplicial node
// ================================================================================
template <typename Node> INLINE
const gumId& gumSimplicialSet<Node>::getBestAlmostSimplicialNode() {
  if ( !hasAlmostSimplicialNode() )
    GUM_ERROR( gumNotFound, "no almost simplicial node could be found" );

  return almost_simplicial_nodes.top();
}


// ================================================================================
/// gets a quasi simplicial node
// ================================================================================
template <typename Node> INLINE
const gumId& gumSimplicialSet<Node>::getBestQuasiSimplicialNode() {
  if ( !hasQuasiSimplicialNode() )
    GUM_ERROR( gumNotFound, "no quasi simplicial node could be found" );

  return quasi_simplicial_nodes.top();
}


// ================================================================================
/// indicates whether there exists a simplicial node
// ================================================================================
template <typename Node>
const bool gumSimplicialSet<Node>::hasAllSimplicialNodes() {
  // check if a node can enter the simplicial list
  for ( gumHashTableIterator<gumId,bool> iter = changed_status.begin();
        iter != changed_status.end(); ++iter ) {
    updateList( iter.key() );
  }

  if ( !simplicial_nodes.empty() )
    return true;
  else
    return false;
}


// ================================================================================
/// returns all the simplicial nodes
// ================================================================================
template <typename Node> INLINE
const gumHashTable<gumId,bool>& gumSimplicialSet<Node>::getAllSimplicialNodes() {
  hasAllSimplicialNodes();
  return simplicial_nodes;
}


// ================================================================================
/// returns all the almost simplicial nodes
// ================================================================================
template <typename Node> INLINE
const gumPriorityQueue<gumId,float>&
gumSimplicialSet<Node>::getAllAlmostSimplicialNodes() {
  hasAllSimplicialNodes();
  return almost_simplicial_nodes;
}


// ================================================================================
/// returns all the quasi simplicial nodes
// ================================================================================
template <typename Node> INLINE
const gumPriorityQueue<gumId,float>&
gumSimplicialSet<Node>::getAllQuasiSimplicialNodes() {
  hasAllSimplicialNodes();
  return quasi_simplicial_nodes;
}

// ==============================================================================
/// sets/unset the fill-ins storage in the standard triangulation procedure
// ==============================================================================
template <typename Node> INLINE
void gumSimplicialSet<Node>::setFillIns(bool b) {
  we_want_fill_ins = b;
}

template <typename Node> INLINE
const gumEdgeList<gumEdge>& gumSimplicialSet<Node>::getFillIns() const {
  return fill_ins_list;
}




/* ============================================================================== */
/* ============================================================================== */
/* ===              DEFAULT TRIANGULATION ALGORITHM USED BY AGRUM             === */
/* ============================================================================== */
/* ============================================================================== */

// ================================================================================
/// basic constructor. initialize the triangulation
// ================================================================================
template <typename Node>
template <typename Edge>
gumDefaultTriangulation<Node>::gumDefaultTriangulation
( const gumUndiGraphTpl<Node,Edge>& theGraph,
  const gumHashTable<gumId,unsigned int>& modal,
  bool minimality, double theRatio, double theThreshold ) :
    log_modalities( theGraph.getNbrNodes() ), elim_order( theGraph.getNbrNodes() ),
    reverse_elim_order( theGraph.getNbrNodes() ), elim_cliques( theGraph.getNbrNodes() ),
    node_2_junction_clique( theGraph.getNbrNodes() ),
    node_2_max_prime_clique( theGraph.getNbrNodes() ), has_triangulation( false ),
    has_triangulated_graph( false ), has_elimination_tree( false ),
    has_junction_tree( false ), has_max_prime_junction_tree( false ),
    quasi_ratio( theRatio ), threshold( theThreshold ),
    minimality_required( minimality ), added_fill_ins( theGraph.getNbrNodes() ),
    we_want_fill_ins(false)
{
  // for debugging purposes
  GUM_CONSTRUCTOR( gumDefaultTriangulation );

  // copy the original graph and set the log_modalities to the log of the
  // modalities of the nodes

  for ( gumNodeListIterator<Node> iter = theGraph.beginNodes();
        iter != theGraph.endNodes(); ++iter ) {
    original_graph.shallowInsertNode( iter.shallowGet() );
    log_modalities.insert( iter.getID(), log( modal[iter->getID()] ) );
  }

  // copy the edges
  for ( gumEdgeListIterator<Edge> iter = theGraph.beginEdges();
        iter != theGraph.endEdges(); ++iter )
    original_graph.insertEdge( gumEdge( iter->first(),iter->second() ) );
}


// ================================================================================
/// basic constructor. initialize the triangulation
// ================================================================================
template <typename Node>
template <typename Arc>
gumDefaultTriangulation<Node>::gumDefaultTriangulation
( const gumDiGraphTpl<Node,Arc>& theGraph,
  const gumHashTable<gumId,unsigned int>& modal,
  bool minimality, double theRatio, double theThreshold ) :
    log_modalities( theGraph.getNbrNodes() ), elim_order( theGraph.getNbrNodes() ),
    reverse_elim_order( theGraph.getNbrNodes() ), elim_cliques( theGraph.getNbrNodes() ),
    node_2_junction_clique( theGraph.getNbrNodes() ),
    node_2_max_prime_clique( theGraph.getNbrNodes() ), has_triangulation( false ),
    has_triangulated_graph( false ), has_elimination_tree( false ),
    has_junction_tree( false ), has_max_prime_junction_tree( false ),
    quasi_ratio( theRatio ), threshold( theThreshold ),
    minimality_required( minimality ), added_fill_ins( theGraph.getNbrNodes() ),
    we_want_fill_ins(false)
 {
  // for debugging purposes
  GUM_CONSTRUCTOR( gumDefaultTriangulation );

  // copy the original graph and set the log_modalities to the log of the
  // modalities of the nodes

  for ( gumNodeListIterator<Node> iter = theGraph.beginNodes();
        iter != theGraph.endNodes(); ++iter ) {
    original_graph.shallowInsertNode( iter.shallowGet() );
    log_modalities.insert( iter.getID(), log( modal[iter->getID()] ) );
  }

  // copy the arcs
  for ( gumArcListIterator<Arc> iter = theGraph.beginArcs();
        iter != theGraph.endArcs(); ++iter ) {
    gumId node1 = std::min( iter->first(),iter->second() );
    gumId node2 = std::max( iter->first(),iter->second() );
    original_graph.insertEdge( gumEdge( node1,node2 ) );
  }
}


// ================================================================================
/// basic constructor. initialize the triangulation
// ================================================================================
template <typename Node>
template <typename Edge, typename Arc>
gumDefaultTriangulation<Node>::gumDefaultTriangulation
( const gumMixedGraphTpl<Node,Edge,Arc>& theGraph,
  const gumHashTable<gumId,unsigned int>& modal,
  bool minimality, double theRatio, double theThreshold ) :
    log_modalities( theGraph.getNbrNodes() ), elim_order( theGraph.getNbrNodes() ),
    reverse_elim_order( theGraph.getNbrNodes() ), elim_cliques( theGraph.getNbrNodes() ),
    node_2_junction_clique( theGraph.getNbrNodes() ),
    node_2_max_prime_clique( theGraph.getNbrNodes() ), has_triangulation( false ),
    has_triangulated_graph( false ), has_elimination_tree( false ),
    has_junction_tree( false ), has_max_prime_junction_tree( false ),
    quasi_ratio( theRatio ), threshold( theThreshold ),
    minimality_required( minimality ), added_fill_ins( theGraph.getNbrNodes() ),
    we_want_fill_ins(false)
{
  // for debugging purposes
  GUM_CONSTRUCTOR( gumDefaultTriangulation );

  // copy the original graph and set the log_modalities to the log of the
  // modalities of the nodes

  for ( gumNodeListIterator<Node> iter = theGraph.beginNodes();
        iter != theGraph.endNodes(); ++iter ) {
    original_graph.shallowInsertNode( iter.shallowGet() );
    log_modalities.insert( iter.getID(), log( modal[iter->getID()] ) );
  }

  // copy the edges and arcs
  for ( gumEdgeListIterator<Edge> iter = theGraph.beginEdges();
        iter != theGraph.endEdges(); ++iter )
    original_graph.insertEdge( gumEdge( iter->first(),iter->second() ) );

  for ( gumArcListIterator<Arc> iter = theGraph.beginArcs();
        iter != theGraph.endArcs(); ++iter ) {
    gumId node1 = std::min( iter->first(),iter->second() );
    gumId node2 = std::max( iter->first(),iter->second() );

    try { original_graph.insertEdge( gumEdge( node1,node2 ) ); }
    catch ( gumDuplicateElement& ) { }
  }
}


// ================================================================================
/// default constructor: initialize the triangulation for an empty graph
// ================================================================================
template <typename Node>
gumDefaultTriangulation<Node>::gumDefaultTriangulation( bool minimality,
    double theRatio,
    double theThreshold ) :
    has_triangulation( true ), has_triangulated_graph( true ),
    has_elimination_tree( true ), has_junction_tree( true ),
    has_max_prime_junction_tree( true ),
    quasi_ratio( theRatio ), threshold( theThreshold ),
    minimality_required( minimality ),
    we_want_fill_ins(false)
{
  // for debugging purposes
  GUM_CONSTRUCTOR( gumDefaultTriangulation );
}


// ================================================================================
/// copy constructor
// ================================================================================
template <typename Node>
gumDefaultTriangulation<Node>::gumDefaultTriangulation
( const gumDefaultTriangulation<Node>& from ) :
    original_graph( from.original_graph ), triangulated_graph( from.triangulated_graph ),
    log_modalities( from.log_modalities ), fill_ins( from.fill_ins ),
    elim_order( from.elim_order ), reverse_elim_order( from.reverse_elim_order ),
    elim_cliques( from.elim_cliques ), elim_tree( from.elim_tree ),
    junction_tree( from.junction_tree ),
    node_2_junction_clique( from.node_2_junction_clique ),
    max_prime_junction_tree( from.max_prime_junction_tree ),
    node_2_max_prime_clique( from.node_2_max_prime_clique ),
    has_triangulation( from.has_triangulation ),
    has_triangulated_graph( from.has_triangulated_graph ),
    has_elimination_tree( from.has_elimination_tree ),
    has_junction_tree( from.has_junction_tree ),
    has_max_prime_junction_tree( from.has_max_prime_junction_tree ),
    quasi_ratio( from.quasi_ratio ), threshold( from.threshold ),
    minimality_required( from.minimality_required ),
    added_fill_ins( from.added_fill_ins ),
    we_want_fill_ins(from.we_want_fill_ins)
{
  // for debugging purposes
  GUM_CONS_CPY( gumDefaultTriangulation );
}


// ================================================================================
/// destructor
// ================================================================================
template <typename Node>
gumDefaultTriangulation<Node>::~gumDefaultTriangulation() {
  // for debugging purposes
  GUM_DESTRUCTOR( gumDefaultTriangulation );
}


// ================================================================================
/// reinitialize the graph to be triangulated to an empty graph
// ================================================================================
template <typename Node>
void gumDefaultTriangulation<Node>::clear() {
  // remove the current graphs
  original_graph.clearNodes();
  triangulated_graph.clearNodes();
  elim_tree.clearNodes();
  junction_tree.clearNodes();
  max_prime_junction_tree.clearNodes();
  elim_cliques.clear();
  node_2_junction_clique.clear();
  node_2_max_prime_clique.clear();

  // remove all fill-ins and orderings
  fill_ins.clear();
  added_fill_ins.clear();
  elim_order.clear();
  reverse_elim_order.clear();

  // clear th modalities
  log_modalities.clear();

  // indicates that the junction tree, the max prime tree, etc, are updated
  has_triangulation = true;
  has_triangulated_graph = true;
  has_elimination_tree = true;
  has_junction_tree = true;
  has_max_prime_junction_tree = true;
}


// ================================================================================
/// initialize the triangulation algorithm for a new graph
// ================================================================================
template <typename Node>
template <typename Edge>
void gumDefaultTriangulation<Node>::setGraph
( const gumUndiGraphTpl<Node,Edge>& gr, const gumHashTable<gumId,unsigned int>& modal ) {
  // remove the old graph
  clear();
  log_modalities.resize( gr.getNbrNodes() );
  elim_order.resize( gr.getNbrNodes() );
  reverse_elim_order.resize( gr.getNbrNodes() );
  elim_cliques.resize( gr.getNbrNodes() );
  added_fill_ins.resize( gr.getNbrNodes() );
  node_2_junction_clique.resize( gr.getNbrNodes() );
  node_2_max_prime_clique.resize( gr.getNbrNodes() );

  // copy the graph passed in argument and set the log_modalities to
  // the log of the modalities of the nodes

  for ( gumNodeListIterator<Node> iter=gr.beginNodes(); iter!=gr.endNodes(); ++iter ) {
    original_graph.shallowInsertNode( iter.shallowGet() );
    log_modalities.insert( iter.getID(), log( modal[iter.getID()] ) );
  }

  // copy the edges
  for ( gumEdgeListIterator<Edge> iter=gr.beginEdges(); iter!=gr.endEdges(); ++iter )
    original_graph.insertEdge( gumEdge( iter->first(),iter->second() ) );

  // indicate that no triangulation was performed for this graph
  has_triangulation = false;

  has_triangulated_graph = false;

  has_elimination_tree = false;

  has_junction_tree = false;

  has_max_prime_junction_tree = false;
}

// ==============================================================================
/// sets/unset the fill-ins storage in the standard triangulation procedure
// ==============================================================================
template <typename Node> INLINE
void gumDefaultTriangulation<Node>::setFillIns(bool b) {
  we_want_fill_ins = b;
}

// ================================================================================
/// the function that performs the triangulation
// ================================================================================
template <typename Node>
void gumDefaultTriangulation<Node>::triangulate() {
  // if the graph is already triangulated, no need to triangulate it again
  if ( has_triangulation ) return;

  // copy the graph to be triangulated, so that we can modify it
  gumUndiGraphTpl<Node,gumEdge> tmp_graph( original_graph, GUM_SHALLOW_COPY );

  // create a container to store for each node the weight of the clique
  // it would form with its neighbours (it will be properly filled by the
  // gumSimplicial structure
  gumHashTable<gumId,float> log_weights( tmp_graph.getNbrNodes() );

  // create a simplicial instance that will find the simplicial,
  // almost simplicial and quasi simplicial nodes
  gumSimplicialSet<Node> simplicial( tmp_graph, log_modalities, log_weights,
                                     quasi_ratio, threshold );

  simplicial.setFillIns(we_want_fill_ins);

  // if we are to do recursive thinning, we will have to add fill-ins to the
  // triangulated graph each time we eliminate a node. Hence, we shall initialize
  // the triangulated graph by a copy of the original graph
  if ( minimality_required ) {
    triangulated_graph.shallowCopy( original_graph );
    has_triangulated_graph = true;
  }

  // perform the triangulation
  gumId removable_node = 0;

  std::pair<gumId,gumId> thePair;

  for ( unsigned int nb_elim = 0; tmp_graph.getNbrNodes() != 0; ++nb_elim ) {
    // select a node to be eliminated
    if ( simplicial.hasSimplicialNode() )
      removable_node = simplicial.getBestSimplicialNode();
    else if ( simplicial.hasAlmostSimplicialNode() )
      removable_node = simplicial.getBestAlmostSimplicialNode();
    else if ( simplicial.hasQuasiSimplicialNode() )
      removable_node = simplicial.getBestQuasiSimplicialNode();
    else {
      // removable_node = heuristic.getNode ();
      gumHashTableIterator<gumId,float> iter_heuristic = log_weights.begin();
      float min_weight = *iter_heuristic;
      removable_node = iter_heuristic.key();

      for ( ++iter_heuristic; iter_heuristic != log_weights.end(); ++iter_heuristic )
        if ( *iter_heuristic < min_weight ) {
          removable_node = iter_heuristic.key();
          min_weight = *iter_heuristic;
        }
    }

    // std::cerr << "eliminating " << removable_node << std::endl;

    // when minimality is not required, i.e., we won't apply recursive thinning,
    // the cliques sets can be computed
    if ( !minimality_required ) {
      std::vector<gumId>& cliques =
        elim_cliques.insert( removable_node, std::vector<gumId> () );
      cliques.resize( tmp_graph.getneighbourEdges( removable_node ).size() + 1 );
      cliques[0] = removable_node;
      unsigned int i = 1;

      for ( gumEdgeListIterator<gumEdge> iter_edges =
              tmp_graph.beginneighbourEdges( removable_node );
            iter_edges != tmp_graph.endEdges(); ++iter_edges, ++i ) {
        cliques[i] = iter_edges->other( removable_node );
      }
    } else {
      // here recursive thinning will be applied, hence we need store the fill-ins
      // added by the current node removal
      gumHashTable<std::pair<gumId,gumId>,bool>&
      current_fill = added_fill_ins[nb_elim];
      gumEdgeListIterator<gumEdge> iter_edges2;
      gumId node1, node2;

      for ( gumEdgeListIterator<gumEdge>
            iter_edges1 = tmp_graph.beginneighbourEdges( removable_node );
            iter_edges1 != tmp_graph.endEdges(); ++iter_edges1 ) {
        node1 = iter_edges1->other( removable_node );
        iter_edges2 = iter_edges1;

        for ( ++iter_edges2; iter_edges2 != tmp_graph.endEdges(); ++iter_edges2 ) {
          node2 = iter_edges2->other( removable_node );

          if ( !tmp_graph.existsEdge( node1,node2 ) ) {
            thePair.first  = std::min( node1,node2 );
            thePair.second = std::max( node1,node2 );
            current_fill.insert( thePair,true );
            triangulated_graph.insertEdge( gumEdge( thePair.first,thePair.second ) );
          }
        }
      }
    }

    // update the simplicial structure (this will also update the temporary
    // graph and the weights hashtable
    simplicial.makeClique( removable_node );

    simplicial.eraseClique( removable_node );

    // update the elimination order
    elim_order[nb_elim] = removable_node;

    reverse_elim_order.insert( removable_node, nb_elim );
  }

  if(we_want_fill_ins) {
    fill_ins_list_standard = simplicial.getFillIns();
  }

  // if minimality is required, remove the redondant edges
  if ( minimality_required )
    computeRecursiveThinning();

  has_triangulation = true;
}


// ================================================================================
/// removes redondant fill-ins and compute proper elimination cliques and order
// ================================================================================
template <typename Node>
void gumDefaultTriangulation<Node>::computeRecursiveThinning() {
  // apply recursive thinning (fmint, see Kjaerulff (90), "triangulation of
  // graphs - algorithms giving small total state space")
  gumId node1, node2;
  bool incomplete;
  std::vector<gumId> adj;
  gumHashTable<std::pair<gumId,gumId>,bool> T_prime;
  gumHashTable<gumId,unsigned int> R( triangulated_graph.getNbrNodes() );

  for ( gumNodeListIterator<Node> iter = triangulated_graph.beginNodes();
        iter != triangulated_graph.endNodes(); ++iter )
    R.insert( iter.getID(),0 );

  // the FMINT loop
  for ( unsigned int i = added_fill_ins.size()-1; i < added_fill_ins.size(); --i ) {
    if ( added_fill_ins[i].size() ) {
      // here apply MINT to T_i = added_fill_ins[i]
      gumHashTable<std::pair<gumId,gumId>,bool>& T = added_fill_ins[i];

      // compute R: by default, R is equal to all the nodes in the graph
      // when R[...] >= j (see the j in the loop below), this means that the node
      // belongs to an extremal edge in R

      for ( unsigned int k = 0; k < elim_order.size(); ++k )
        R[elim_order[k]] = 0;

      // apply MINT while some edges can possibly be deleted
      bool require_mint = true;

      for ( unsigned int j = 0; require_mint; ++j ) {
        // find T' (it is equal to the edges (v,w) of T such that
        // the intersection of adj(v,G) and adj(w,G) is complete and such that
        // v and/or w belongs to an extremal node in R
        for ( gumHashTableIterator<std::pair<gumId,gumId>,bool> iter_edge=T.begin();
              iter_edge != T.end(); ++iter_edge ) {
          node1 = iter_edge.key().first;
          node2 = iter_edge.key().second;

          // check if at least one extremal node belongs to R

          if (( R[node1] < j ) && ( R[node2] < j ) )
            continue;

          // check if the intersection of adj(v,G) and adj(w,G) is complete
          if ( triangulated_graph.getneighbourEdges( node2 ).size() <
               triangulated_graph.getneighbourEdges( node1 ).size() ) {
            gumId tmp = node1;
            node1 = node2;
            node2 = tmp;
          }

          incomplete = false;

          // find the nodes belonging to the intersection of adj(v,G) and adj(w,G)

          for ( gumEdgeListIterator<gumEdge> iter_adj =
                  triangulated_graph.beginneighbourEdges( node1 );
                iter_adj != triangulated_graph.endEdges(); ++iter_adj ) {
            if ( triangulated_graph.existsEdge( node2,iter_adj->other( node1 ) ) )
              adj.push_back( iter_adj->other( node1 ) );
          }

          // check if the intersection is complete
          for ( unsigned int k = 0; k < adj.size() && !incomplete; ++k ) {
            for ( unsigned int m = k+1; m < adj.size(); ++m ) {
              if ( !triangulated_graph.existsEdge( adj[k],adj[m] ) ) {
                incomplete = true;
                break;
              }
            }
          }

          adj.clear();

          if ( !incomplete ) {
            T_prime.insert( iter_edge.key(),true );
            R[node1] = j+1;
            R[node2] = j+1;
          }
        }

        // compute the new value of T (i.e. T\T') and update the triangulated graph
        for ( gumHashTableIterator<std::pair<gumId,gumId>,bool>
              iter_delete = T_prime.begin();
              iter_delete != T_prime.end(); ++iter_delete ) {
          T.erase( iter_delete.key() );
          triangulated_graph.eraseEdge( iter_delete.key().first,
                                        iter_delete.key().second );
        }

        if ( T_prime.size() == 0 )
          require_mint = false;

        T_prime.clear();
      }
    }
  }

  // here, the recursive thinning has removed all the superfluous edges.
  // Hence some cliques have been split and, as a result, the elimination order
  // has changed. We should thus compute a new perfect ordering. To do so,
  // we use a Maximal Cardinality Search: it has indeed the nice property that,
  // when the graph is already triangulated, it finds a compatible order of
  // elimination that does not require any edge addition

  // a structure storing the number of neighbours previously processed
  gumPriorityQueue< gumId,unsigned int,std::greater<unsigned int> >
  numbered_neighbours( std::greater<unsigned int>(), false,
                      triangulated_graph.getNbrNodes() );

  for ( unsigned int i = 0; i< elim_order.size(); ++i )
    numbered_neighbours.insert( 0, elim_order[i] );

  // perform the maximum cardinality search
  for ( unsigned int i = elim_order.size() - 1; i < elim_order.size(); --i ) {
    gumId node = numbered_neighbours.pop();
    elim_order[i] = node;
    reverse_elim_order[node] = i;

    for ( gumEdgeListIterator<gumEdge>
          iter = triangulated_graph.beginneighbourEdges( node );
          iter != triangulated_graph.endEdges(); ++iter ) {
      gumId neighbour = iter->other( node );

      try {
        numbered_neighbours.setPriorityByVal
        ( neighbour, 1 + numbered_neighbours.getPriorityByVal( neighbour ) );
      } catch ( gumNotFound& ) { }
    }
  }

  // here the elimination order is ok. We now need to update the elim_cliques
  for ( unsigned int i = 0; i < elim_order.size(); ++i ) {
    std::vector<gumId>& cliques =
      elim_cliques.insert( elim_order[i], std::vector<gumId> () );
    cliques.push_back( elim_order[i] );

    for ( gumEdgeListIterator<gumEdge>
          iter = triangulated_graph.beginneighbourEdges( elim_order[i] );
          iter != triangulated_graph.endEdges(); ++iter ) {
      gumId neighbour = iter->other( elim_order[i] );

      if ( reverse_elim_order[neighbour] > i ) {
        cliques.push_back( neighbour );
      }
    }
  }
}


// ================================================================================
/// returns an elimination tree from a triangulated graph
// ================================================================================
template <typename Node>
void gumDefaultTriangulation<Node>::computeEliminationTree() {
  // if there already exists an elimination tree, no need to compute it again
  if ( has_elimination_tree ) return;

  // if the graph is not triangulated, triangulate it
  if ( !has_triangulation )
    triangulate();

  // create the nodes of the elimination tree
  gumNodeList<Node> clique;

  std::ostringstream ost;

  for ( unsigned int i = 0; i < elim_order.size(); ++i ) {
    // form the elimination clique relative to the node
    clique.clear();
    std::vector<gumId>& list_of_nodes = elim_cliques[elim_order[i]];

    for ( unsigned int j = 0; j < list_of_nodes.size(); ++j )
      clique.shallowInsert( original_graph.shallowGetNode( list_of_nodes[j] ) );

    // insert the node
    ost.str( "" );

    ost << "clique " << original_graph.getNode( elim_order[i] ).label();

    elim_tree.insertNode( gumNode( elim_order[i], ost.str() ),
                          clique, GUM_SHALLOW_COPY );
  }

  // create the edges of the elimination tree: join a node to the one in its clique
  // that is eliminated first
  for ( unsigned int i = 0; i < elim_order.size(); ++i ) {
    std::vector<gumId>& list_of_nodes = elim_cliques[elim_order[i]];
    unsigned int child = original_graph.getNbrNodes() + 1;

    for ( unsigned int j = 0; j < list_of_nodes.size(); ++j )
      if (( list_of_nodes[j] != elim_order[i] ) &&
          ( child > reverse_elim_order[list_of_nodes[j]] ) )
        child = reverse_elim_order[list_of_nodes[j]];

    if ( child < original_graph.getNbrNodes() )
      elim_tree.insertEdge( gumEdge( elim_order[i], elim_order[child] ) );
  }

  has_elimination_tree = true;
}


// ================================================================================
/// computes a junction tree from an elimination tree
// ================================================================================
template <typename Node>
void gumDefaultTriangulation<Node>::computeJunctionTree() {
  // if there already exists a junction tree, no need to recompute it
  if ( has_junction_tree ) return;

  // if there is no elimination tree, create it
  if ( !has_elimination_tree )
    computeEliminationTree();

  // copy the elimination tree into the junction tree
  junction_tree.shallowCopy( elim_tree );

  //mark all the edges of the junction tree to false
  gumHashTable<std::pair<gumId,gumId>,bool> mark =
    junction_tree.hashMapEdges( false );

  // create a vector indicating by which clique a given clique has been
  // substituted during the transformation from the elimination tree to the
  // junction tree. Assume that clique J the elmination tree has been substituted
  // by clique K of the elimination tree, and that clique J (resp. K) was the
  // jth (resp. kth) one created during the triangulation process. Then, in the
  // vector below, substitution[j] = k.
  std::vector<unsigned int> substitution( elim_order.size() );

  for ( unsigned int i = 0; i < substitution.size(); ++i )
    substitution[i] = i;

  // for all cliques C_i, from the last one created to the first one, check if there
  // exists a parent C_j (a neighbour created before C_i) such that |C_j| = |C_i|
  // and such that the edge is not marked. In this case, this means that C_j
  // contains C_i. Hence, we should remove C_i, and link all of its neighbours to
  // C_j. These links will be marked to true as no such neighbour can be included
  // in C_j (and conversely).
  std::pair<gumId,gumId> thePair;

  gumEdgeListIterator<gumEdge> iter;

  for ( unsigned int i=elim_order.size()-1; i < elim_order.size(); --i ) {
    gumId C_i = elim_order[i];
    unsigned int card_C_i = junction_tree.getClique( C_i ).size();

    // search for C_j such that |C_j| = [C_i| + 1
    gumId C_j = C_i;

    for ( iter = junction_tree.beginneighbourEdges( C_i );
          iter != junction_tree.endEdges(); ++iter ) {
      gumId C_jj = iter->other( C_i );
      thePair.first  = std::min( C_i,C_jj );
      thePair.second = std::max( C_i,C_jj );

      if (( reverse_elim_order[C_i] > reverse_elim_order[C_jj] ) && !mark[thePair] &&
          ( junction_tree.getClique( C_jj ).size() == card_C_i + 1 ) ) {
        // ok, here we found a parent such that |C_jj| = [C_i| + 1
        C_j = C_jj;
        junction_tree.eraseEdge( C_i,C_j );
        break;
      }
    }

    // if we found a C_j, link the neighbours of C_i to C_j
    if ( C_j != C_i ) {
      for ( iter = junction_tree.beginneighbourEdges( C_i );
            iter != junction_tree.endEdges(); ++iter ) {
        gumEdge new_edge( C_j,iter->other( C_i ) );
        junction_tree.insertEdge( new_edge );
        thePair.first  = new_edge.first();
        thePair.second = new_edge.second();
        mark.insert( thePair, true );
      }

      substitution[i] = reverse_elim_order[C_j];

      junction_tree.eraseNode( C_i );
    }
  }

  // compute the transitive closure of vector substitution
  for ( unsigned int i = 0; i < substitution.size(); ++i )
    substitution[i] = substitution[substitution[i]];

  // using the transitive closure of vector substitution, compute for each node
  // the clique of the junction tree that was created by its elimination during
  // the triangulation
  for ( unsigned int i = 0; i < substitution.size(); ++i )
    node_2_junction_clique.insert( elim_order[i], elim_order[substitution[i]] );

  has_junction_tree = true;
}


// ================================================================================
/// used for computing the junction tree of the maximal prime subgraphs
// ================================================================================
template <typename Node>
void gumDefaultTriangulation<Node>::computeMaxPrimeMergings
( const gumId& node, const gumId& from,
  std::vector< std::pair<gumId,gumId> >& merged_cliques,
  gumHashTable<gumId,bool>& mark ) const {
  mark[node] = true;
  gumNodeListIterator<Node> iter_sep2;

  for ( gumEdgeListIterator<gumEdge> iter_sep=junction_tree.beginneighbourEdges( node );
        iter_sep != junction_tree.endEdges(); ++iter_sep ) {
    gumId other_node = iter_sep->other( node );

    if ( other_node != from ) {
      const gumNodeList<Node>&
      separator = junction_tree.getSeparator( node, other_node );
      // check that the separator between node and other_node is complete
      bool complete = true;

      for ( gumNodeListIterator<Node> iter_sep1 = separator.begin();
            iter_sep1 != separator.end() && complete; ++iter_sep1 ) {
        iter_sep2 = iter_sep1;

        for ( ++iter_sep2; iter_sep2 != separator.end(); ++iter_sep2 ) {
          if ( !original_graph.existsEdge( iter_sep1->getID(),iter_sep2->getID() ) ) {
            complete = false;
            break;
          }
        }
      }

      // here complete indicates whether the separator is complete or not
      if ( !complete )
        merged_cliques.push_back( std::pair<gumId,gumId>( other_node,node ) );

      computeMaxPrimeMergings( other_node, node, merged_cliques, mark );
    }
  }
}


// ================================================================================
/// computes the junction tree of the maximal prime subgraphs
// ================================================================================
template <typename Node>
void gumDefaultTriangulation<Node>::computeMaxPrimeJunctionTree() {
  // if there already exists a max prime junction tree, no need to recompute it
  if ( has_max_prime_junction_tree ) return;

  // if there is no junction tree, create it
  if ( !has_junction_tree )
    computeJunctionTree();

  // the maximal prime subgraph join tree is created by aggregation of some
  // cliques. More precisely, when the separator between 2 cliques is not complete
  // in the original graph, then the two cliques must be merged. Create a hashtable
  // indicating which clique has been absorbed by some other clique.
  gumHashTable<gumId,gumId>
  T_mpd_cliques( junction_tree.getNbrNodes() );

  for ( gumNodeListIterator<gumNode> iter_clique = junction_tree.beginNodes();
        iter_clique != junction_tree.endNodes(); ++iter_clique )
    T_mpd_cliques.insert( iter_clique.getID(),iter_clique.getID() );

  // parse all the separators of the junction tree and test those that are not
  // complete in the orginal graph
  std::vector< std::pair<gumId,gumId> > merged_cliques;

  gumHashTable<gumId,bool> mark = junction_tree.hashMapNodes( false );

  for ( gumNodeListIterator<gumNode> iter_clique = junction_tree.beginNodes();
        iter_clique != junction_tree.endNodes(); ++iter_clique )
    if ( !mark[iter_clique.getID()] )
      computeMaxPrimeMergings( iter_clique.getID(), iter_clique.getID(),
                               merged_cliques, mark );

  // compute the transitive closure of merged_cliques. This one will contain
  // pairs (X,Y) indicating that clique X must be merged with clique Y.
  // Actually clique X will be inserted into clique Y.
  for ( unsigned int i = 0; i < merged_cliques.size(); ++i ) {
    T_mpd_cliques[merged_cliques[i].first]=T_mpd_cliques[merged_cliques[i].second];
  }

  // now we can create the max prime junction tree. First, create the cliques
  max_prime_junction_tree.clearNodes();

  for ( gumHashTableIterator<gumId,gumId> iter_clique = T_mpd_cliques.begin();
        iter_clique != T_mpd_cliques.end(); ++iter_clique ) {
    if ( iter_clique.key() == *iter_clique ) {
      max_prime_junction_tree.insertNode( junction_tree.getNode( *iter_clique ),
                                          junction_tree.getClique( *iter_clique ),
                                          GUM_SHALLOW_COPY );
    }
  }

  // add to the cliques previously created the nodes of the cliques that were
  // merged into them
  for ( gumHashTableIterator<gumId,gumId>  iter = T_mpd_cliques.begin();
        iter != T_mpd_cliques.end(); ++iter ) {
    if ( iter.key() != *iter ) {
      const gumNodeList<Node>& new_clique = junction_tree.getClique( iter.key() );

      for ( gumNodeListIterator<gumNode> iter_node = new_clique.begin();
            iter_node != new_clique.end(); ++iter_node ) {
        try {
          max_prime_junction_tree.addToClique( *iter, iter_node.shallowGet(),
                                               GUM_SHALLOW_COPY );
        } catch ( gumDuplicateElement& ) { }
      }
    }
  }

  // add the edges to the graph
  for ( gumEdgeListIterator<gumEdge> iter_edge = junction_tree.beginEdges();
        iter_edge != junction_tree.endEdges(); ++iter_edge ) {
    gumId node1 = T_mpd_cliques[iter_edge->first()];
    gumId node2 = T_mpd_cliques[iter_edge->second()];

    if ( node1 != node2 ) {
      try {
        max_prime_junction_tree.insertEdge( gumEdge( node1,node2 ) );
      } catch ( gumDuplicateElement& ) { }
    }
  }

  // compute for each node which clique of the max prime junction tree was created
  // by the elmination of the node
  for ( gumHashTableIterator<gumId,gumId>
        iter_junction = node_2_junction_clique.begin();
        iter_junction != node_2_junction_clique.end(); ++iter_junction )
    node_2_max_prime_clique.insert( iter_junction.key(),
                                    T_mpd_cliques[*iter_junction] );

  has_max_prime_junction_tree = true;
}


// ================================================================================
/// returns the fill-ins added by the triangulation algorithm
// ================================================================================
template <typename Node> INLINE
const gumEdgeList<gumEdge>& gumDefaultTriangulation<Node>::getFillIns() {
  if ( !has_triangulation )
    triangulate();
  if(minimality_required || (! we_want_fill_ins)) {
    return fill_ins;
  } else {
    return fill_ins_list_standard;
  }
}


// ================================================================================
/// returns an elimination ordering compatible with the triangulated graph
// ================================================================================
template <typename Node> INLINE
const std::vector<gumId>& gumDefaultTriangulation<Node>::eliminationOrder() {
  if ( !has_triangulation )
    triangulate();

  return elim_order;
}


// ================================================================================
/** @brief returns the number of a given node in the elimination order
 * (0 = first node eliminated) */
// ================================================================================
template <typename Node> INLINE
unsigned int gumDefaultTriangulation<Node>::eliminationOrder( const gumId& id ) {
  if ( !has_triangulation )
    triangulate();

  return reverse_elim_order[id];
}


// ================================================================================
/// returns the triangulated graph
// ================================================================================
template <typename Node> INLINE
const gumUndiGraphTpl<Node,gumEdge>&
gumDefaultTriangulation<Node>::triangulatedGraph() {
  if ( !has_triangulation )
    triangulate();

  // if we did not construct the triangulated graph during the triangulation, that
  // is, we just constructed the junction tree, we shall construct it now
  if ( !has_triangulated_graph ) {
    // just in case, be sure that the junction tree has been constructed
    if ( !has_junction_tree )
      computeJunctionTree();

    // copy the original graph
    triangulated_graph.shallowCopy( original_graph );

    // parse all the cliques of the junction tree
    gumNodeListIterator<Node> iter_clique2;

    for ( gumNodeListIterator<gumNode> iter_node = junction_tree.beginNodes();
          iter_node != junction_tree.endNodes(); ++iter_node ) {
      // for each clique, add the edges necessary to make it complete
      const gumNodeList<Node>& clique = junction_tree.getClique( *iter_node );
      std::vector<gumId> clique_nodes( clique.size() );
      unsigned int i =0;

      for ( gumNodeListIterator<Node> iter_clique = clique.begin();
            iter_clique != clique.end(); ++iter_clique, ++i )
        clique_nodes[i] = iter_clique->getID();

      for ( i = 0; i < clique_nodes.size(); ++i ) {
        for ( unsigned int j = i+1; j < clique_nodes.size(); ++j ) {
          try { triangulated_graph.insertEdge( gumEdge( i,j ) ); }
          catch ( gumDuplicateElement& ) { }
        }
      }
    }

    has_triangulated_graph = true;
  }

  return triangulated_graph;
}


// ================================================================================
/// returns the elimination tree of a compatible ordering
// ================================================================================
template <typename Node> INLINE
const gumCliqueGraphTpl<gumNode,gumEdge,Node>&
gumDefaultTriangulation<Node>::eliminationTree() {
  if ( !has_elimination_tree )
    computeEliminationTree();

  return elim_tree;
}


// ================================================================================
/// returns a compatible junction tree
// ================================================================================
template <typename Node> INLINE
const gumCliqueGraphTpl<gumNode,gumEdge,Node>&
gumDefaultTriangulation<Node>::junctionTree() {
  if ( !has_junction_tree )
    computeJunctionTree();

  return junction_tree;
}


// ================================================================================
/// returns a junction tree of maximal prime subgraphs
// ================================================================================
template <typename Node> INLINE
const gumCliqueGraphTpl<gumNode,gumEdge,Node>&
gumDefaultTriangulation<Node>::maxPrimeSubgraphTree() {
  if ( !has_max_prime_junction_tree )
    computeMaxPrimeJunctionTree();

  return max_prime_junction_tree;
}


// ================================================================================
/** @brief returns the Id of the maximal prime subgraph created by the
 * elimination of a given node during the triangulation process */
// ==============================================================================
template <typename Node> INLINE
const gumId& gumDefaultTriangulation<Node>::createdMaxPrimeSubgraph( const gumId& id ) {
  if ( !has_max_prime_junction_tree )
    computeMaxPrimeJunctionTree();

  return node_2_max_prime_clique[id];
}


// ================================================================================
/** @brief returns the Id of the clique created by the
 * elimination of a given node during the triangulation process */
// ==============================================================================
template <typename Node> INLINE
const gumId& gumDefaultTriangulation<Node>::createdClique( const gumId& id ) {
  if ( !has_junction_tree )
    computeJunctionTree();

  return node_2_junction_clique[id];
}









/* ============================================================================== */
/* ============================================================================== */
/* ===             CLASS FOR COMPUTING INCREMENTAL TRIANGULATIONS             === */
/* ============================================================================== */
/* ============================================================================== */

// ================================================================================
/// constructor
// ================================================================================
template <typename Node, template <typename> class Triangulation>
template <typename Edge>
gumIncrementalTriangulation<Node,Triangulation>::gumIncrementalTriangulation
( const gumUndiGraphTpl<Node,Edge>& theGraph,
  const gumHashTable<gumId,unsigned int>& modal ) : require_update( false ) {
  // for debugging purposes
  GUM_CONSTRUCTOR( gumIncrementalTriangulation );

  // shallow copy the graph passed in argument and update the structures
  // containing the informations useful for the triangulation

  for ( gumNodeListIterator<Node> iter_node = theGraph.beginNodes();
        iter_node != theGraph.endNodes(); ++iter_node )
    insertNode( iter_node.shallowGet(), modal[iter_node.getID()] );

  // insert all the edges of the graph into the structure. This will
  // implicitly update the "require_update" field
  for ( gumEdgeListIterator<Edge> iter_edge = theGraph.beginEdges();
        iter_edge != theGraph.endEdges(); ++iter_edge )
    insertEdge( iter_edge->first(), iter_edge->second() );
}


// ================================================================================
/// default constructor
// ================================================================================
template <typename Node, template <typename> class Triangulation>
gumIncrementalTriangulation<Node,Triangulation>::gumIncrementalTriangulation() :
    require_update( false ) {
  // for debugging purposes
  GUM_CONSTRUCTOR( gumIncrementalTriangulation );
}


// ================================================================================
/// copy operator
// ================================================================================
template <typename Node, template <typename> class Triangulation>
gumIncrementalTriangulation<Node,Triangulation>::gumIncrementalTriangulation
( const gumIncrementalTriangulation<Node,Triangulation>& from ) :
    modalities( from.modalities ), junction_tree( from.junction_tree ), T_mpd( from.T_mpd ),
    mps_of_node( from.mps_of_node ), cliques_of_mps( from.cliques_of_mps ),
    mps_of_clique( from.mps_of_clique ), idFactory( from.idFactory ),
    mps_affected( from.mps_affected ), triangulation( from.triangulation ),
    require_update( from.require_update ) {
  // for debugging purposes
  GUM_CONS_CPY( gumIncrementalTriangulation );

  // shallow copy the original graph
  graph.shallowCopy( from.graph );
}


// ================================================================================
/// destructor
// ================================================================================
template <typename Node, template <typename> class Triangulation>
gumIncrementalTriangulation<Node,Triangulation>::~gumIncrementalTriangulation() {
  // for debugging purposes
  GUM_DESTRUCTOR( gumIncrementalTriangulation );
}


// ================================================================================
/// copy operator
// ================================================================================
template <typename Node, template <typename> class Triangulation>
gumIncrementalTriangulation<Node,Triangulation>&
gumIncrementalTriangulation<Node,Triangulation>::operator=
( const gumIncrementalTriangulation<Node,Triangulation>& from ) {
  // avoid self assignment
  if ( this != &from ) {
    // for debugging purposes
    GUM_OP_CPY( gumIncrementalTriangulation );

    // copy all the structures stored in "from"
    graph.shallowCopy( from.graph );
    modalities = from.modalities;
    junction_tree = from.junction_tree;
    T_mpd = from.T_mpd;
    mps_of_node = from.mps_of_node;
    cliques_of_mps = from.cliques_of_mps;
    mps_of_clique = from.mps_of_clique;

    try { idFactory = from.idFactory; }
    catch ( gumIdError& ) { }

    mps_affected = from.mps_affected;

    triangulation = from.triangulation;
    require_update = from.require_update;
  }

  return *this;
}


// ================================================================================
/// adds a new node to the graph (and update the internal structures)
// ================================================================================
template <typename Node, template <typename> class Triangulation>
void gumIncrementalTriangulation<Node,Triangulation>::insertNode
( const gumRefPtr<Node>& node, unsigned int modal ) {
  // add the new node to the graph
  graph.shallowInsertNode( node );
  modalities.insert( node->getID(), modal );

  // add a new clique to T_mpd and the junction tree
  gumNodeList<Node> clique_nodes( 2 );
  clique_nodes.shallowInsert( node );
  gumNode clique = gumNode( idFactory.getNewId(),"" );
  gumNode& MPS = T_mpd.insertNode( clique, clique_nodes, GUM_SHALLOW_COPY );
  gumNode& new_clique =
    junction_tree.insertNode( clique, clique_nodes, GUM_SHALLOW_COPY );

  // indicate in which MPS node belongs
  gumList<gumId>& list_of_mps = mps_of_node.insert( node->getID(),gumList<gumId>() );
  list_of_mps.insert( MPS.getID() );

  // indicate in which MPS the clique added to the junction tree belongs
  std::vector<gumId>& cliquesMPS =
    cliques_of_mps.insert( MPS.getID(), std::vector<gumId>() );
  cliquesMPS.push_back( new_clique.getID() );
  mps_of_clique.insert( new_clique.getID(),MPS.getID() );

  // indicate that the new MPS should not be affected by a triangulation
  mps_affected.insert( MPS.getID(),false );
}


// ================================================================================
/// mark the mps affected by the deletion of a given edge
// ================================================================================
template <typename Node, template <typename> class Triangulation>
void gumIncrementalTriangulation<Node,Triangulation>::markAffectedMPSsByRemoveLink
( const gumId& My, const gumId& Mz, const std::pair<gumId,gumId>& edge ) {
  // mark the MPS so that it will be retriangulated
  mps_affected[My] = true;

  // mark all the neighbour MPS that contain edge

  for ( gumEdgeListIterator<gumEdge> iter = T_mpd.beginneighbourEdges( My );
        iter != T_mpd.endEdges(); ++iter ) {
    if ( iter->other( My ) != Mz ) {
      const gumNodeList<Node>& Syk = T_mpd.getSeparator( *iter );

      if ( Syk.exists( edge.first ) && Syk.exists( edge.second ) )
        markAffectedMPSsByRemoveLink( iter->other( My ), My, edge );
    }
  }
}


// ================================================================================
/// removes an edge from the graph (the join tree may need a retriangulation)
// ================================================================================
template <typename Node, template <typename> class Triangulation>
void gumIncrementalTriangulation<Node,Triangulation>::eraseEdge
( const gumId& X, const gumId& Y ) {
  // check that the edge exist
  if ( !graph.existsEdge( X,Y ) ) return;

  // find a MPS containing the edge (X,Y)
  const gumList<gumId>& mps1 = mps_of_node[X];

  const gumList<gumId>& mps2 = mps_of_node[Y];

  gumId Mx = mps1[0];

  if ( mps1.size() <= mps2.size() ) {
    for ( gumListIterator<gumId> iter = mps1.begin(); iter != mps1.end(); ++iter ) {
      if ( T_mpd.getClique( *iter ).exists( Y ) ) {
        Mx = *iter;
        break;
      }
    }
  } else {
    for ( gumListIterator<gumId> iter = mps2.begin(); iter != mps2.end(); ++iter ) {
      if ( T_mpd.getClique( *iter ).exists( X ) ) {
        Mx = *iter;
        break;
      }
    }
  }

  // mark the MPS that need be updated
  markAffectedMPSsByRemoveLink
  ( Mx, Mx, std::pair<gumId,gumId> ( std::min( X,Y ), std::max( X,Y ) ) );

  require_update = true;

  // remove the edge (X,Y) from the graph
  graph.eraseEdge( X,Y );
}


// ================================================================================
/// removes a node from the graph (the join tree may need a triangulation update)
// ================================================================================
template <typename Node, template <typename> class Triangulation>
void gumIncrementalTriangulation<Node,Triangulation>::eraseNode( const gumId& X ) {
  // remove all the edges adjacent to the node
  for ( gumEdgeListIterator<gumEdge> neighbour_edge = graph.beginneighbourEdges( X );
        neighbour_edge != graph.endEdges(); ++neighbour_edge )
    eraseEdge( neighbour_edge->first(), neighbour_edge->second() );

  // remove X from the MPS containing X
  gumList<gumId>& MPS_of_X = mps_of_node[X];

  for ( gumListIterator<gumId> iter=MPS_of_X.begin(); iter!=MPS_of_X.end(); ++iter ) {
    T_mpd.eraseFromClique( *iter, X );
    // if the intersection between *iter and one of its neighbour is empty,
    // remove the edge linking them

    for ( gumEdgeListIterator<gumEdge> neighbour = T_mpd.beginneighbourEdges( *iter );
          neighbour != T_mpd.endEdges(); ++neighbour )
      if ( T_mpd.getSeparator( *iter,neighbour->other( *iter ) ).size()==0 )
        T_mpd.eraseEdge( *iter,neighbour->other( *iter ) );
  }

  // remove X from the cliques containing X
  for ( gumListIterator<gumId> iter=MPS_of_X.begin(); iter!=MPS_of_X.end(); ++iter ) {
    const std::vector<gumId>& cliques_of_X = cliques_of_mps[*iter];

    for ( unsigned int i = 0; i < cliques_of_X.size(); ++i ) {
      junction_tree.eraseFromClique( cliques_of_X[i], X );
      // if the intersection between *iter and one of its neighbour is empty,
      // remove the edge linking them

      for ( gumEdgeListIterator<gumEdge> neighbour =
              junction_tree.beginneighbourEdges( cliques_of_X[i] );
            neighbour != junction_tree.endEdges(); ++neighbour )
        if ( junction_tree.getSeparator( cliques_of_X[i],
                                         neighbour->other( cliques_of_X[i] ) ).size()==0 )
          junction_tree.eraseEdge( cliques_of_X[i],
                                   neighbour->other( cliques_of_X[i] ) );
    }
  }

  // if the MPS containing X is empty, then remove it, as well as the corresponding
  // clique in the junction tree (which also only contain X)
  if (( MPS_of_X.size()==1 ) && ( T_mpd.getClique( MPS_of_X[0] ).size()==0 ) ) {
    junction_tree.eraseNode( cliques_of_mps[MPS_of_X[0]][0] );
    T_mpd.eraseNode( MPS_of_X[0] );
    mps_of_clique.erase( cliques_of_mps[MPS_of_X[0]][0] );
    cliques_of_mps.erase( MPS_of_X[0] );
    mps_affected.erase( MPS_of_X[0] );
  }

  mps_of_node.erase( X );

  // remove X completely from the graph
  graph.eraseNode( X );
  modalities.erase( X );
}


// ================================================================================
/// removes a node from the graph (the join tree may need a triangulation update)
// ================================================================================
template <typename Node, template <typename> class Triangulation> INLINE
void gumIncrementalTriangulation<Node,Triangulation>::eraseNode( const Node& X ) {
  eraseNode( X.getID() );
}


// ================================================================================
/// add a new edge to the graph
// ================================================================================
template <typename Node, template <typename> class Triangulation>
int gumIncrementalTriangulation<Node,Triangulation>::markAffectedMPSsByAddLink
( const gumId& Mx, const gumId& Mz, const gumId& X, const gumId& Y ) {
  // check if Mx contains Y. In this case, mark Mx and return 1 to indicate that
  // Y has been found or 2 to indicate that Y has been found and that the nearest
  // MPS containing X has been marked
  const gumNodeList<Node>& cliqueMX = T_mpd.getClique( Mx );

  if ( cliqueMX.exists( Y ) ) {
    mps_affected[Mx] = true;

    if ( cliqueMX.exists( X ) ) return 2;

    return 1;
  }

  // parse Mx's neighbours until we find Y
  for ( gumEdgeListIterator<gumEdge> neighbour = T_mpd.beginneighbourEdges( Mx );
        neighbour != T_mpd.endEdges(); ++neighbour ) {
    gumId other_node = neighbour->other( Mx );

    if ( other_node != Mz ) {
      int neighbourStatus = markAffectedMPSsByAddLink( other_node,Mx,X,Y );

      if ( neighbourStatus == 2 )
        return 2;
      else if ( neighbourStatus == 1 ) {
        mps_affected[Mx] = true;

        if ( cliqueMX.exists( X ) ) return 2;

        return 1;
      }
    }
  }

  // indicate that X was not found
  return 0;
}


// ================================================================================
/// adds a new edge to the graph (the join tree may need a triangulation update)
// ================================================================================
template <typename Node, template <typename> class Triangulation>
void gumIncrementalTriangulation<Node,Triangulation>::insertEdge
( const gumId& X, const gumId& Y ) {
  // add the edge to the graph
  graph.insertEdge( gumEdge( X,Y ) );

  // take any MPS containing X and search its tree to find Y
  gumId& mps_X = mps_of_node[X][0];
  int found = markAffectedMPSsByAddLink( mps_X, mps_X, X, Y );

  if ( found == 0 ) {
    // the mps containing X do not belong to the same tree as those containing Y
    gumId& mps_Y = mps_of_node[Y][0];

    // find a clique in mps_X containing X and another in mps_Y containing Y
    // and add a clique XY to the junction tree linked to the cliques found
    // in mps_X and mps_Y
    const std::vector<gumId>& cliques_X = cliques_of_mps[mps_X];
    const std::vector<gumId>& cliques_Y = cliques_of_mps[mps_Y];
    gumId c_X = 0, c_Y = 0;

    for ( unsigned int i = 0; i < cliques_X.size(); ++i ) {
      if ( junction_tree.getClique( cliques_X[i] ).exists( X ) ) {
        c_X = cliques_X[i];
        break;
      }
    }

    for ( unsigned int i = 0; i < cliques_Y.size(); ++i ) {
      if ( junction_tree.getClique( cliques_Y[i] ).exists( Y ) ) {
        c_Y = cliques_Y[i];
        break;
      }
    }

    // link c_Y and c_X through a new node containing XY
    gumId newID = idFactory.getNewId();

    gumNode newNode = gumNode( newID,"" );

    gumNodeList<Node> nodes( 2 );

    nodes.shallowInsert( graph.shallowGetNode( X ) );

    nodes.shallowInsert( graph.shallowGetNode( Y ) );

    junction_tree.insertNode( newNode, nodes, GUM_SHALLOW_COPY );

    junction_tree.insertEdge( gumEdge( newID, c_X ) );

    junction_tree.insertEdge( gumEdge( newID, c_Y ) );

    T_mpd.insertNode( newNode, nodes, GUM_SHALLOW_COPY );

    T_mpd.insertEdge( gumEdge( newID,mps_X ) );

    T_mpd.insertEdge( gumEdge( newID,mps_Y ) );

    // check that the maximal prime subgraph containing X is not X alone
    // in this case, remove this max prime subgraph, as well as the corresponding
    // clique in the junction tree
    if ( T_mpd.getClique( mps_X ).size() == 1 ) {
      junction_tree.eraseNode( c_X );
      T_mpd.eraseNode( mps_X );
      mps_affected.erase( mps_X );
      mps_of_clique.erase( c_X );
      cliques_of_mps.erase( mps_X );
      mps_X = newID;
    } else
      mps_of_node[X].insert( newID );

    // do the same thing as above for node Y
    if ( T_mpd.getClique( mps_Y ).size() == 1 ) {
      junction_tree.eraseNode( c_Y );
      T_mpd.eraseNode( mps_Y );
      mps_affected.erase( mps_Y );
      mps_of_clique.erase( c_Y );
      cliques_of_mps.erase( mps_Y );
      mps_Y = newID;
    } else
      mps_of_node[Y].insert( newID );

    std::vector<gumId>& cl = cliques_of_mps.insert( newID,std::vector<gumId>() );

    cl.push_back( newID );

    mps_of_clique.insert( newID,newID );

    mps_affected.insert( newID,false );
  } else
    require_update = true;
}


// ================================================================================
/// set up a connected subgraph to be triangulated
// ================================================================================
template <typename Node, template <typename> class Triangulation>
void gumIncrementalTriangulation<Node,Triangulation>::setUpConnectedTriangulation
( gumId Mx, gumId Mfrom, gumUndiGraphTpl<Node,gumEdge>& theGraph,
  std::vector<gumEdge>& notAffectedneighbourCliques,
  gumHashTable<gumId,bool>& cliques_affected ) {
  // mark the clique so that we won't try to update it several times
  cliques_affected[Mx] = false;

  // get the nodes that are concerned by the triangulation update
  const gumNodeList<Node>& clique = junction_tree.getClique( Mx );

  for ( gumNodeListIterator<Node> iter_node = clique.begin();
        iter_node != clique.end(); ++iter_node )
    if ( !theGraph.existsNode( iter_node->getID() ) )
      theGraph.shallowInsertNode( iter_node.shallowGet() );

  // go on with the neighbour cliques in the junction tree
  for ( gumEdgeListIterator<gumEdge> neighbour = junction_tree.beginneighbourEdges( Mx );
        neighbour != junction_tree.endEdges(); ++neighbour ) {
    gumId othernode = neighbour->other( Mx );

    if ( othernode != Mfrom ) {
      if ( cliques_affected.exists( othernode ) ) {
        setUpConnectedTriangulation( othernode, Mx, theGraph,
                                     notAffectedneighbourCliques, cliques_affected );
      } else {
        // indicate that we have a clique not affected that is adjacent
        // to an affected one
        notAffectedneighbourCliques.push_back( *neighbour );
      }
    }
  }
}


// ================================================================================
/// used for computing the junction tree of the maximal prime subgraphs
// ================================================================================
template <typename Node, template <typename> class Triangulation>
void gumIncrementalTriangulation<Node,Triangulation>::computeMaxPrimeMergings
( const gumId& node, const gumId& from,
  std::vector< std::pair<gumId,gumId> >& merged_cliques,
  gumHashTable<gumId,bool>& mark, const gumId& max_old_id ) const {
  mark[node] = true;
  gumNodeListIterator<Node> iter_sep2;
  // check the separators on all the adjacent edges of Mx

  for ( gumEdgeListIterator<gumEdge> iter_sep=junction_tree.beginneighbourEdges( node );
        iter_sep != junction_tree.endEdges(); ++iter_sep ) {
    gumId other_node = iter_sep->other( node );

    if ( other_node != from ) {
      const gumNodeList<Node>&
      separator = junction_tree.getSeparator( node, other_node );
      // check that the separator between node and other_node is complete
      bool complete = true;

      for ( gumNodeListIterator<Node> iter_sep1 = separator.begin();
            iter_sep1 != separator.end() && complete; ++iter_sep1 ) {
        iter_sep2 = iter_sep1;

        for ( ++iter_sep2; iter_sep2 != separator.end(); ++iter_sep2 ) {
          if ( !graph.existsEdge( iter_sep1->getID(),iter_sep2->getID() ) ) {
            complete = false;
            break;
          }
        }
      }

      // here complete indicates whether the separator is complete or not
      if ( !complete )
        merged_cliques.push_back( std::pair<gumId,gumId>( other_node,node ) );

      if ( other_node > max_old_id )
        computeMaxPrimeMergings( other_node, node, merged_cliques,
                                 mark, max_old_id );
    }
  }
}

// ================================================================================
// ================================================================================
template <typename Node, template <typename> class Triangulation>
void gumIncrementalTriangulation<Node,Triangulation>::check() {
  // check que les noeuds du graphe appartiennent a l'arbre de jonction
  {
    gumHashTable<gumId,bool> nodes = graph.hashMapNodes( false );

    for ( gumNodeListIterator<gumNode> iter = junction_tree.beginNodes();
          iter != junction_tree.endNodes(); ++iter ) {
      const gumNodeList<Node>& clique = junction_tree.getClique( iter.getID() );

      for ( gumNodeListIterator<Node> iter2 = clique.begin();
            iter2 != clique.end(); ++iter2 )
        nodes[iter2.getID()] = true;
    }

    for ( gumHashTableIterator<gumId,bool> iter = nodes.begin();
          iter != nodes.end(); ++iter )
      if ( !*iter ) {
        std::cerr << "check nodes" << std::endl
        << graph << std::endl << junction_tree << std::endl;
      }
  }

  // check que les arcs du graphe appartiennent a l'arbre de jonction
  {
    gumNodeListIterator<Node> iter3;
    std::pair<gumId,gumId> thePair;
    gumHashTable<std::pair<gumId,gumId>,bool> edges = graph.hashMapEdges( false );

    for ( gumNodeListIterator<gumNode> iter = junction_tree.beginNodes();
          iter != junction_tree.endNodes(); ++iter ) {
      const gumNodeList<Node>& clique = junction_tree.getClique( iter.getID() );

      for ( gumNodeListIterator<Node> iter2 = clique.begin();
            iter2 != clique.end(); ++iter2 ) {
        iter3 = iter2;

        for ( ++iter3; iter3 != clique.end(); ++iter3 ) {
          thePair.first  = std::min( iter2.getID(), iter3.getID() );
          thePair.second = std::max( iter2.getID(), iter3.getID() );

          if ( graph.existsEdge( thePair.first, thePair.second ) )
            edges[thePair] = true;
        }
      }
    }

    for ( gumHashTableIterator<std::pair<gumId,gumId>,bool> iter = edges.begin();
          iter != edges.end(); ++iter )
      if ( !*iter ) {
        std::cerr << "check edges" << std::endl
        << graph << std::endl << junction_tree << std::endl;
      }
  }

  // check que les noeuds du graphe appartiennent a l'arbre de MPS
  {
    gumHashTable<gumId,bool> nodes = graph.hashMapNodes( false );

    for ( gumNodeListIterator<gumNode> iter = T_mpd.beginNodes();
          iter != T_mpd.endNodes(); ++iter ) {
      const gumNodeList<Node>& clique = T_mpd.getClique( iter.getID() );

      for ( gumNodeListIterator<Node> iter2 = clique.begin();
            iter2 != clique.end(); ++iter2 )
        nodes[iter2.getID()] = true;
    }

    for ( gumHashTableIterator<gumId,bool> iter = nodes.begin();
          iter != nodes.end(); ++iter )
      if ( !*iter ) {
        std::cerr << "check nodes" << std::endl
        << graph << std::endl << T_mpd << std::endl;
      }
  }

  // check que les arcs du graphe appartiennent a l'arbre de MPS
  {
    gumNodeListIterator<Node> iter3;
    std::pair<gumId,gumId> thePair;
    gumHashTable<std::pair<gumId,gumId>,bool> edges = graph.hashMapEdges( false );

    for ( gumNodeListIterator<gumNode> iter = T_mpd.beginNodes();
          iter != T_mpd.endNodes(); ++iter ) {
      const gumNodeList<Node>& clique = T_mpd.getClique( iter.getID() );

      for ( gumNodeListIterator<Node> iter2 = clique.begin();
            iter2 != clique.end(); ++iter2 ) {
        iter3 = iter2;

        for ( ++iter3; iter3 != clique.end(); ++iter3 ) {
          thePair.first  = std::min( iter2.getID(), iter3.getID() );
          thePair.second = std::max( iter2.getID(), iter3.getID() );

          if ( graph.existsEdge( thePair.first, thePair.second ) )
            edges[thePair] = true;
        }
      }
    }

    for ( gumHashTableIterator<std::pair<gumId,gumId>,bool> iter = edges.begin();
          iter != edges.end(); ++iter )
      if ( !*iter ) {
        std::cerr << "check edges" << std::endl
        << graph << std::endl << T_mpd << std::endl;
      }
  }

  // check le MPS of node
  {
    gumHashTable< gumId,gumHashTable<gumId,bool> > chk;

    for ( gumNodeListIterator<Node> iter = graph.beginNodes();
          iter != graph.endNodes(); ++iter )
      chk.insert( iter.getID(), gumHashTable<gumId,bool>() );

    for ( gumNodeListIterator<gumNode> iter = T_mpd.beginNodes();
          iter != T_mpd.endNodes(); ++iter ) {
      const gumNodeList<Node>& clique = T_mpd.getClique( iter.getID() );

      for ( gumNodeListIterator<Node> iter2 = clique.begin();
            iter2 != clique.end(); ++iter2 )
        chk[iter2.getID()].insert( iter.getID(),false );
    }

    for ( gumHashTableIterator< gumId,gumList<gumId> > iter =  mps_of_node.begin();
          iter !=  mps_of_node.end(); ++iter ) {
      const gumList<gumId>& liste = *iter;
      gumHashTable<gumId,bool>& hash = chk[iter.key()];

      for ( gumListIterator<gumId> iter2 = liste.begin();
            iter2 != liste.end(); ++iter2 ) {
        if ( !hash.exists( *iter2 ) )
          std::cerr << "check mps of nodes" << std::endl
          << T_mpd << std::endl << mps_of_node << std::endl;

        hash[*iter2] = true;
      }
    }

    for ( gumHashTableIterator< gumId,gumHashTable<gumId,bool> > iter=chk.begin();
          iter != chk.end(); ++iter ) {
      const gumHashTable<gumId,bool>& hash = *iter;

      for ( gumHashTableIterator<gumId,bool> iter2 = hash.begin();
            iter2 != hash.end(); ++iter2 )
        if ( !*iter2 )
          std::cerr << "check mps of nodes2" << std::endl
          << T_mpd << std::endl << mps_of_node << std::endl;
    }
  }

  // check le mps of clique
  {

  }
}


// ================================================================================
/// updates the triangulated graph using the modif list
// ================================================================================
template <typename Node, template <typename> class Triangulation>
void gumIncrementalTriangulation<Node,Triangulation>::updateTriangulation() {
  if ( !require_update ) return;

  // the last ID generated for the junction tree and the max prime subgraph tree
  // this enables to discriminate between "old" nodes and the new ones created
  // by the triangulation
  gumId max_old_id = idFactory.getMaxId();

  // a temporary subgraph in which we actually perform the triangulation
  gumUndiGraphTpl<Node,gumEdge> tmp_graph;

  // the set of all the cliques that should be affected by the different
  // triangulations we will perform (one by connected component)
  gumHashTable<gumId,bool> all_cliques_affected( junction_tree.getNbrNodes() );

  // for each triangulation, we will keep track of the cliques of the
  // junction tree that are not affected by the triangulation but that are
  // adjacent to cliques affected. This will enable us to connect easily the
  // newly created cliques with the old ones.
  std::vector<gumEdge> notAffectedneighbourCliques;

  // parse all the affected MPS and get the corresponding cliques
  for ( gumHashTableIterator<gumId,bool> iter_mps = mps_affected.begin();
        iter_mps != mps_affected.end(); ++iter_mps ) {
    if ( *iter_mps ) {
      // get the cliques contained in this MPS
      const std::vector<gumId>& cliques = cliques_of_mps[iter_mps.key()];

      for ( unsigned int i = 0; i < cliques.size(); ++i )
        all_cliques_affected.insert( cliques[i],true );
    }
  }

  // for each connected set of cliques involved in the triangulations
  // perform a new triangulation and update the max prime subgraph tree
  for ( gumHashTableIterator<gumId,bool> iter_clique = all_cliques_affected.begin();
        iter_clique != all_cliques_affected.end(); ++iter_clique ) {
    if ( *iter_clique ) {
      // set up the connected subgraph that need be retriangulated and the cliques
      // that are affected by this triangulation
      tmp_graph.clearNodes();
      notAffectedneighbourCliques.clear();
      setUpConnectedTriangulation( iter_clique.key(), iter_clique.key(),
                                   tmp_graph, notAffectedneighbourCliques,
                                   all_cliques_affected );

      // insert the edges in tmp_graph

      for ( gumEdgeListIterator<gumEdge> iter_edge = graph.beginEdges();
            iter_edge != graph.endEdges(); ++iter_edge ) {
        try { tmp_graph.insertEdge( *iter_edge ); }
        catch ( gumInvalidEdge& ) { } // both extremities must be in tmp_graph
      }

      // remove from the mps_of_node table the affected mps containing the node
      for ( gumNodeListIterator<Node> iter_node = tmp_graph.beginNodes();
            iter_node != tmp_graph.endNodes(); ++iter_node ) {
        gumList<gumId>& mps = mps_of_node[iter_node->getID()];

        for ( gumHashTableIterator<gumId,bool> iter_mps = mps_affected.begin();
              iter_mps != mps_affected.end(); ++iter_mps ) {
          if ( *iter_mps )
            mps.eraseByVal( iter_mps.key() );
        }
      }

      // now tmp_graph contains the graph that should be triangulated.
      // so triangulate it and get its junction tree
      triangulation.setGraph( tmp_graph,modalities );

      const gumCliqueGraphTpl<gumNode,gumEdge,Node>& tmp_junction_tree =
        triangulation.junctionTree();

      // now, update the global junction tree
      // first add the nodes of tmp_junction_tree to junction_tree
      gumHashTable<gumId,gumId>
      tmp2global_junction_tree( tmp_junction_tree.getNbrNodes() );

      for ( gumNodeListIterator<gumNode> iter_jt = tmp_junction_tree.beginNodes();
            iter_jt != tmp_junction_tree.endNodes(); ++iter_jt ) {
        gumId new_id = idFactory.getNewId();
        junction_tree.insertNode( gumNode( new_id,"" ),
                                  tmp_junction_tree.getClique( iter_jt->getID() ),
                                  GUM_SHALLOW_COPY );
        // translate the id of the temprary JT into an id of the global JT
        tmp2global_junction_tree.insert( iter_jt->getID(),new_id );
      }

      // and add the edges of tmp_junction_tree to junction_tree
      for ( gumEdgeListIterator<gumEdge> iter_edge = tmp_junction_tree.beginEdges();
            iter_edge != tmp_junction_tree.endEdges(); ++iter_edge )
        junction_tree.insertEdge
        ( gumEdge( tmp2global_junction_tree[iter_edge->first()],
                   tmp2global_junction_tree[iter_edge->second()] ) );

      // second get the edges in junction_tree that have an extremal clique R in the
      // affected clique set and the other one S not in the affected set and see
      // which new node V in the junction_tree should be connected to S. The running
      // intersection property guarrantees that the clique in the tmp_junction_tree
      // that results from the elimination (during the triangulation process) of
      // the first eliminated node in the separator between R and S is an
      // admissible candidate
      for ( unsigned int i = 0; i < notAffectedneighbourCliques.size(); ++i ) {
        // now find the first eliminated node in the separator
        const gumNodeList<Node>& sep =
          junction_tree.getSeparator( notAffectedneighbourCliques[i] );
        unsigned int elim_order = tmp_graph.getNbrNodes() + 1;
        gumId elim_node = 0;

        for ( gumNodeListIterator<Node> iter_sep = sep.begin();
              iter_sep != sep.end(); ++iter_sep ) {
          gumId id = iter_sep->getID();
          unsigned int new_order = triangulation.eliminationOrder( id );

          if ( new_order < elim_order ) {
            elim_order = new_order;
            elim_node = id;
          }
        }

        // find the junction_tree clique corresponding to the elimination of
        // elim_node and insert an edge between this clique and that which was not
        // affected
        gumId& to_connect =
          tmp2global_junction_tree[triangulation.createdClique( elim_node )];

        gumId not_affected =
          all_cliques_affected.exists( notAffectedneighbourCliques[i].first() )?
          notAffectedneighbourCliques[i].second() :
          notAffectedneighbourCliques[i].first();

        junction_tree.insertEdge( gumEdge( not_affected,to_connect ) );

        if ( to_connect <= max_old_id )
          T_mpd.insertEdge( gumEdge( mps_of_clique[to_connect],
                                     mps_of_clique[not_affected] ) );

        // check that the separator created by the new edge is not equal to
        // to_connect. If this is the case, then to_connect is included in
        // not_affected and, hence, should be removed from the graph
        if ( junction_tree.getSeparator( not_affected,to_connect ).size() ==
             junction_tree.getClique( to_connect ).size() ) {
          junction_tree.eraseEdge( not_affected,to_connect );

          for ( gumEdgeListIterator<gumEdge>
                iter_neighbour = junction_tree.beginneighbourEdges( to_connect );
                iter_neighbour != junction_tree.endEdges(); ++iter_neighbour ) {
            junction_tree.insertEdge
            ( gumEdge( iter_neighbour->other( to_connect ), not_affected ) );

            if ( iter_neighbour->other( to_connect ) <= max_old_id )
              T_mpd.insertEdge
              ( gumEdge( mps_of_clique[iter_neighbour->other( to_connect )],
                         mps_of_clique[not_affected] ) );
          }

          junction_tree.eraseNode( to_connect );

          to_connect = not_affected;
        }
      }
    }
  }

  // remove the mps that were affected and update the cliques_of_mps table
  for ( gumHashTableIterator<gumId,bool> iter_clique = all_cliques_affected.begin();
        iter_clique != all_cliques_affected.end(); ++iter_clique ) {
    mps_of_clique.erase( iter_clique.key() );
    junction_tree.eraseNode( iter_clique.key() );
  }

  for ( gumHashTableIterator<gumId,bool> iter_MPS = mps_affected.begin();
        iter_MPS != mps_affected.end(); ++iter_MPS ) {
    if ( *iter_MPS ) {
      cliques_of_mps.erase( iter_MPS.key() );
      T_mpd.eraseNode( iter_MPS.key() );
    }
  }

  // now update the T_mpd so that it be coherent with the junction tree

  // the maximal prime subgraph join tree is created by aggregation of some
  // cliques. More precisely, when the separator between 2 cliques is not complete
  // in the original graph, then the two cliques must be merged. Create a hashtable
  // indicating which clique has been absorbed by some other clique. Keys are
  // the cliques absorbed, and values are the cliques that absorb them.
  gumHashTable<gumId,gumId>
  T_mpd_cliques( all_cliques_affected.size() );

  for ( gumNodeListIterator<gumNode> iter_clique = junction_tree.beginNodes();
        iter_clique != junction_tree.endNodes(); ++iter_clique )
    if ( iter_clique.getID() > max_old_id )
      T_mpd_cliques.insert( iter_clique.getID(),iter_clique.getID() );

  // parse all the separators of the junction tree and test those that are not
  // complete in the original graph
  std::vector< std::pair<gumId,gumId> > merged_cliques;

  gumHashTable<gumId,bool> mark = T_mpd_cliques.map( false );

  for ( gumHashTableIterator<gumId,bool> iter_clique = mark.begin();
        iter_clique != mark.end(); ++iter_clique )
    if ( !mark[iter_clique.key()] )
      computeMaxPrimeMergings( iter_clique.key(), iter_clique.key(),
                               merged_cliques, mark, max_old_id );

  // compute the transitive closure of merged_cliques. This one will contain
  // pairs (X,Y) indicating that clique X must be merged with clique Y.
  // Actually clique X will be inserted into clique Y.
  for ( unsigned int i = 0; i < merged_cliques.size(); ++i ) {
    if ( T_mpd_cliques.exists( merged_cliques[i].second ) )
      T_mpd_cliques[merged_cliques[i].first] =
        T_mpd_cliques[merged_cliques[i].second];
    else
      T_mpd_cliques[merged_cliques[i].first] =
        mps_of_clique[merged_cliques[i].second];
  }

  // now we can create the max prime junction tree. First, create the new cliques
  // and create the corresponding cliques_of_mps entries
  for ( gumHashTableIterator<gumId,gumId> iter_clique = T_mpd_cliques.begin();
        iter_clique != T_mpd_cliques.end(); ++iter_clique ) {
    if ( iter_clique.key() == *iter_clique ) {
      T_mpd.insertNode( junction_tree.getNode( *iter_clique ),
                        junction_tree.getClique( *iter_clique ), GUM_SHALLOW_COPY );
      std::vector<gumId>& vect_of_cliques =
        cliques_of_mps.insert( *iter_clique, std::vector<gumId>() );
      vect_of_cliques.push_back( *iter_clique );
    }
  }

  // add to the cliques previously created the nodes of the cliques that were
  // merged into them and update the cliques_of_mps
  for ( gumHashTableIterator<gumId,gumId> iter = T_mpd_cliques.begin();
        iter != T_mpd_cliques.end(); ++iter ) {
    if (( iter.key() != *iter ) && ( *iter > max_old_id ) ) {
      const gumNodeList<Node>& new_clique = junction_tree.getClique( iter.key() );

      for ( gumNodeListIterator<gumNode> iter_node = new_clique.begin();
            iter_node != new_clique.end(); ++iter_node ) {
        try { T_mpd.addToClique( *iter, iter_node.shallowGet(), GUM_SHALLOW_COPY ); }
        catch ( gumDuplicateElement& ) { }
      }

      cliques_of_mps[*iter].push_back( iter.key() );
    }
  }

  // update the mps_of_node and the mps_of_clique
  for ( gumHashTableIterator<gumId,gumId> iter = T_mpd_cliques.begin();
        iter != T_mpd_cliques.end(); ++iter ) {
    mps_of_clique.insert( iter.key(), *iter );

    if ( iter.key() == *iter ) {
      const gumNodeList<Node>& nodes = T_mpd.getClique( *iter );

      for ( gumNodeListIterator<Node> iter_node = nodes.begin();
            iter_node != nodes.end(); ++iter_node )
        mps_of_node[iter_node.getID()].insert( *iter );
    }
  }

  // add the edges to the max prime subgraph tree
  for ( gumHashTableIterator<gumId,gumId> iter = T_mpd_cliques.begin();
        iter != T_mpd_cliques.end(); ++iter ) {
    gumId clique = *iter;

    for ( gumEdgeListIterator<gumEdge>
          iter_neighbour = junction_tree.beginneighbourEdges( iter.key() );
          iter_neighbour != junction_tree.endEdges(); ++iter_neighbour ) {
      gumId othernode = iter_neighbour->other( iter.key() );

      if ( T_mpd_cliques.exists( othernode ) ) {
        // here iter is linked to another node that has been created during
        // the triangulation
        gumId otherClique = T_mpd_cliques[othernode];
        // avoid adding the same edge several times

        if ( clique > otherClique ) {
          T_mpd.insertEdge( gumEdge( clique, otherClique ) );
        }
      } else {
        T_mpd.insertEdge( gumEdge( clique, mps_of_clique[othernode] ) );
      }
    }
  }

  // reset the MPS that are affected
  mps_affected.clear();

  for ( gumNodeListIterator<gumNode> iter = T_mpd.beginNodes();
        iter != T_mpd.endNodes(); ++iter )
    mps_affected.insert( iter.getID(), false );

  // remove all the structures used by the triangulation algorithm
  triangulation.clear();

  // check();

  require_update = false;
}


// ================================================================================
/// returns a junction tree corresponding to the current graph
// ================================================================================
template <typename Node, template <typename> class Triangulation> INLINE
const gumCliqueGraphTpl<gumNode,gumEdge,Node>&
gumIncrementalTriangulation<Node,Triangulation>::junctionTree() {
  updateTriangulation();
  return junction_tree;
}


// ================================================================================
/// returns the junction tree of the maximal prime subgraphs
// ================================================================================
template <typename Node, template <typename> class Triangulation> INLINE
const gumCliqueGraphTpl<gumNode,gumEdge,Node>&
gumIncrementalTriangulation<Node,Triangulation>::maxPrimeSubgraphTree() {
  updateTriangulation();
  return T_mpd;
}


// ================================================================================
/// sets the graph to the empty graph
// ================================================================================
template <typename Node, template <typename> class Triangulation>
void gumIncrementalTriangulation<Node,Triangulation>::clear() {
  graph.clearNodes();
  modalities.clear();
  junction_tree.clearNodes();
  T_mpd.clearNodes();
  mps_of_node.clear();
  cliques_of_mps.clear();
  mps_affected.clear();
  triangulation.clear();
  require_update = false;
}


// ================================================================================
/// changes the current graph
// ================================================================================
template <typename Node, template <typename> class Triangulation>
template <typename Edge>
void gumIncrementalTriangulation<Node,Triangulation>::setGraph
( const gumUndiGraphTpl<Node,Edge>& theGraph,
  const gumHashTable<gumId,unsigned int>& modal ) {
  // remove the current graph
  clear();

  // shallow copy the graph passed in argument and update the structures
  // containing the informations useful for the triangulation

  for ( gumNodeListIterator<Node> iter_node = theGraph.beginNodes();
        iter_node != theGraph.endNodes(); ++iter_node )
    insertNode( iter_node.shallowGet(), modal[iter_node.getID()] );

  for ( gumEdgeListIterator<Edge> iter_edge = theGraph.beginEdges();
        iter_edge != theGraph.endEdges(); ++iter_edge )
    insertEdge( iter_edge->first(), iter_edge->second() );
}


// ================================================================================
/// returns the triangulation algorithm (useful for fine tuning it)
// ================================================================================
template <typename Node, template <typename> class Triangulation> INLINE
Triangulation<Node>&
gumIncrementalTriangulation<Node,Triangulation>::getTriangulationAlgo()
const  {
  return triangulation;
}


// ================================================================================
/// returns a clique containing a given node of the triangulated graph
// ================================================================================
template <typename Node, template <typename> class Triangulation> INLINE
gumId gumIncrementalTriangulation<Node,Triangulation>::getContainerClique
( const gumId& id ) const {
  const std::vector<gumId>& contain_cliques = cliques_of_mps[mps_of_node[id][0]];

  for ( unsigned int i = 0; i < contain_cliques.size(); ++i )
    if ( junction_tree.getClique( contain_cliques[i] ).exists( id ) )
      return contain_cliques[i];

  GUM_ERROR( gumNotFound, "no container clique" );
}



#endif /* DOXYGEN_SHOULD_SKIP_THIS */
