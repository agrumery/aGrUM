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
* @brief Inline implementations for computing default triangulations of graphs
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */


#ifndef DOXYGEN_SHOULD_SKIP_THIS


/* ============================================================================== */
/* ============================================================================== */
/* ===              DEFAULT TRIANGULATION ALGORITHM USED BY AGRUM             === */
/* ============================================================================== */
/* ============================================================================== */

// ================================================================================
/// basic constructor. initialize the triangulation
// ================================================================================
template <typename gumNodeId>
template <typename Edge>
gumDefaultTriangulation<gumNodeId>::gumDefaultTriangulation
( const gumUndiGraphTpl<gumNodeId,Edge>& theGraph,
  const gumHashTable<gumNodeId,unsigned int>& modal,
  bool minimality, double theRatio, double theThreshold ) :
    __log_modalities( theGraph.getNbrNodes() ), __elim_order( theGraph.getNbrNodes() ),
    __reverse_elim_order( theGraph.getNbrNodes() ), __elim_cliques( theGraph.getNbrNodes() ),
    __node_2_junction_clique( theGraph.getNbrNodes() ),
    __node_2_max_prime_clique( theGraph.getNbrNodes() ), __has_triangulation( false ),
    __has_triangulated_graph( false ), __has_elimination_tree( false ),
    __has_junction_tree( false ), __has_max_prime_junction_tree( false ),
    __quasi_ratio( theRatio ), threshold( theThreshold ),
    __minimality_required( minimality ), __added_fill_ins( theGraph.getNbrNodes() ),
    __we_want_fill_ins( false ) {
// for debugging purposes
  GUM_CONSTRUCTOR( gumDefaultTriangulation );

// copy the original graph and set the __log_modalities to the log of the
// modalities of the nodes

  for ( gumNodeListIterator<gumNodeId> iter = theGraph.beginNodes();
        iter != theGraph.endNodes(); ++iter ) {
    __original_graph.shallowInsertNode( iter.shallowGet() );
    __log_modalities.insert( iter.getID(), log( modal[iter->getID()] ) );
  }

// copy the edges
  for ( gumEdgeListIterator<Edge> iter = theGraph.beginEdges();
        iter != theGraph.endEdges(); ++iter )
    __original_graph.insertEdge( gumEdge( iter->first(),iter->second() ) );
}


// ================================================================================
/// basic constructor. initialize the triangulation
// ================================================================================
template <typename gumNodeId>
template <typename Arc>
gumDefaultTriangulation<gumNodeId>::gumDefaultTriangulation
( const gumDiGraphTpl<gumNodeId,Arc>& theGraph,
  const gumHashTable<gumNodeId,unsigned int>& modal,
  bool minimality, double theRatio, double theThreshold ) :
    __log_modalities( theGraph.getNbrNodes() ), __elim_order( theGraph.getNbrNodes() ),
    __reverse_elim_order( theGraph.getNbrNodes() ), __elim_cliques( theGraph.getNbrNodes() ),
    __node_2_junction_clique( theGraph.getNbrNodes() ),
    __node_2_max_prime_clique( theGraph.getNbrNodes() ), __has_triangulation( false ),
    __has_triangulated_graph( false ), __has_elimination_tree( false ),
    __has_junction_tree( false ), __has_max_prime_junction_tree( false ),
    __quasi_ratio( theRatio ), threshold( theThreshold ),
    __minimality_required( minimality ), __added_fill_ins( theGraph.getNbrNodes() ),
    __we_want_fill_ins( false ) {
// for debugging purposes
  GUM_CONSTRUCTOR( gumDefaultTriangulation );

// copy the original graph and set the __log_modalities to the log of the
// modalities of the nodes

  for ( gumNodeListIterator<gumNodeId> iter = theGraph.beginNodes();
        iter != theGraph.endNodes(); ++iter ) {
    __original_graph.shallowInsertNode( iter.shallowGet() );
    __log_modalities.insert( iter.getID(), log( modal[iter->getID()] ) );
  }

// copy the arcs
  for ( gumArcListIterator<Arc> iter = theGraph.beginArcs();
        iter != theGraph.endArcs(); ++iter ) {
    gumNodeId node1 = std::min( iter->first(),iter->second() );
    gumNodeId node2 = std::max( iter->first(),iter->second() );
    __original_graph.insertEdge( gumEdge( node1,node2 ) );
  }
}


// ================================================================================
/// basic constructor. initialize the triangulation
// ================================================================================
template <typename gumNodeId>
template <typename Edge, typename Arc>
gumDefaultTriangulation<gumNodeId>::gumDefaultTriangulation
( const gumMixedGraphTpl<gumNodeId,Edge,Arc>& theGraph,
  const gumHashTable<gumNodeId,unsigned int>& modal,
  bool minimality, double theRatio, double theThreshold ) :
    __log_modalities( theGraph.getNbrNodes() ), __elim_order( theGraph.getNbrNodes() ),
    __reverse_elim_order( theGraph.getNbrNodes() ), __elim_cliques( theGraph.getNbrNodes() ),
    __node_2_junction_clique( theGraph.getNbrNodes() ),
    __node_2_max_prime_clique( theGraph.getNbrNodes() ), __has_triangulation( false ),
    __has_triangulated_graph( false ), __has_elimination_tree( false ),
    __has_junction_tree( false ), __has_max_prime_junction_tree( false ),
    __quasi_ratio( theRatio ), threshold( theThreshold ),
    __minimality_required( minimality ), __added_fill_ins( theGraph.getNbrNodes() ),
    __we_want_fill_ins( false ) {
// for debugging purposes
  GUM_CONSTRUCTOR( gumDefaultTriangulation );

// copy the original graph and set the __log_modalities to the log of the
// modalities of the nodes

  for ( gumNodeListIterator<gumNodeId> iter = theGraph.beginNodes();
        iter != theGraph.endNodes(); ++iter ) {
    __original_graph.shallowInsertNode( iter.shallowGet() );
    __log_modalities.insert( iter.getID(), log( modal[iter->getID()] ) );
  }

// copy the edges and arcs
  for ( gumEdgeListIterator<Edge> iter = theGraph.beginEdges();
        iter != theGraph.endEdges(); ++iter )
    __original_graph.insertEdge( gumEdge( iter->first(),iter->second() ) );

  for ( gumArcListIterator<Arc> iter = theGraph.beginArcs();
        iter != theGraph.endArcs(); ++iter ) {
    gumNodeId node1 = std::min( iter->first(),iter->second() );
    gumNodeId node2 = std::max( iter->first(),iter->second() );

    try { __original_graph.insertEdge( gumEdge( node1,node2 ) ); }
    catch ( gumDuplicateElement& ) { }
  }
}


// ================================================================================
/// default constructor: initialize the triangulation for an empty graph
// ================================================================================
template <typename gumNodeId>
gumDefaultTriangulation<gumNodeId>::gumDefaultTriangulation( bool minimality,
    double theRatio,
    double theThreshold ) :
    __has_triangulation( true ), __has_triangulated_graph( true ),
    __has_elimination_tree( true ), __has_junction_tree( true ),
    __has_max_prime_junction_tree( true ),
    __quasi_ratio( theRatio ), threshold( theThreshold ),
    __minimality_required( minimality ),
    __we_want_fill_ins( false ) {
// for debugging purposes
  GUM_CONSTRUCTOR( gumDefaultTriangulation );
}


// ================================================================================
/// copy constructor
// ================================================================================
template <typename gumNodeId>
gumDefaultTriangulation<gumNodeId>::gumDefaultTriangulation
( const gumDefaultTriangulation<gumNodeId>& from ) :
    __original_graph( from.__original_graph ), __triangulated_graph( from.__triangulated_graph ),
    __log_modalities( from.__log_modalities ), __fill_ins( from.__fill_ins ),
    __elim_order( from.__elim_order ), __reverse_elim_order( from.__reverse_elim_order ),
    __elim_cliques( from.__elim_cliques ), __elim_tree( from.__elim_tree ),
    __junction_tree( from.__junction_tree ),
    __node_2_junction_clique( from.__node_2_junction_clique ),
    __max_prime_junction_tree( from.__max_prime_junction_tree ),
    __node_2_max_prime_clique( from.__node_2_max_prime_clique ),
    __has_triangulation( from.__has_triangulation ),
    __has_triangulated_graph( from.has_triangulated_graph ),
    __has_elimination_tree( from.has_elimination_tree ),
    __has_junction_tree( from.has_junction_tree ),
    __has_max_prime_junction_tree( from.has_max_prime_junction_tree ),
    __quasi_ratio( from.__quasi_ratio ), threshold( from.threshold ),
    __minimality_required( from.__minimality_required ),
    __added_fill_ins( from.__added_fill_ins ),
    __we_want_fill_ins( from.__we_want_fill_ins ) {
// for debugging purposes
  GUM_CONS_CPY( gumDefaultTriangulation );
}


// ================================================================================
/// destructor
// ================================================================================
template <typename gumNodeId>
gumDefaultTriangulation<gumNodeId>::~gumDefaultTriangulation() {
// for debugging purposes
  GUM_DESTRUCTOR( gumDefaultTriangulation );
}


// ================================================================================
/// reinitialize the graph to be triangulated to an empty graph
// ================================================================================
template <typename gumNodeId>
void gumDefaultTriangulation<gumNodeId>::clear() {
// remove the current graphs
  __original_graph.clearNodes();
  __triangulated_graph.clearNodes();
  __elim_tree.clearNodes();
  __junction_tree.clearNodes();
  __max_prime_junction_tree.clearNodes();
  __elim_cliques.clear();
  __node_2_junction_clique.clear();
  __node_2_max_prime_clique.clear();

// remove all fill-ins and orderings
  __fill_ins.clear();
  __added_fill_ins.clear();
  __elim_order.clear();
  __reverse_elim_order.clear();

// clear th modalities
  __log_modalities.clear();

// indicates that the junction tree, the max prime tree, etc, are updated
  __has_triangulation = true;
  __has_triangulated_graph = true;
  __has_elimination_tree = true;
  __has_junction_tree = true;
  __has_max_prime_junction_tree = true;
}


// ================================================================================
/// initialize the triangulation algorithm for a new graph
// ================================================================================
template <typename gumNodeId>
template <typename Edge>
void gumDefaultTriangulation<gumNodeId>::setGraph
( const gumUndiGraphTpl<gumNodeId,Edge>& gr, const gumHashTable<gumNodeId,unsigned int>& modal ) {
// remove the old graph
  clear();
  __log_modalities.resize( gr.getNbrNodes() );
  __elim_order.resize( gr.getNbrNodes() );
  __reverse_elim_order.resize( gr.getNbrNodes() );
  __elim_cliques.resize( gr.getNbrNodes() );
  __added_fill_ins.resize( gr.getNbrNodes() );
  __node_2_junction_clique.resize( gr.getNbrNodes() );
  __node_2_max_prime_clique.resize( gr.getNbrNodes() );

// copy the graph passed in argument and set the __log_modalities to
// the log of the modalities of the nodes

  for ( gumNodeListIterator<gumNodeId> iter=gr.beginNodes(); iter!=gr.endNodes(); ++iter ) {
    __original_graph.shallowInsertNode( iter.shallowGet() );
    __log_modalities.insert( iter.getID(), log( modal[iter.getID()] ) );
  }

// copy the edges
  for ( gumEdgeListIterator<Edge> iter=gr.beginEdges(); iter!=gr.endEdges(); ++iter )
    __original_graph.insertEdge( gumEdge( iter->first(),iter->second() ) );

// indicate that no triangulation was performed for this graph
  __has_triangulation = false;

  __has_triangulated_graph = false;

  __has_elimination_tree = false;

  __has_junction_tree = false;

  __has_max_prime_junction_tree = false;
}

// ==============================================================================
/// sets/unset the fill-ins storage in the standard triangulation procedure
// ==============================================================================
template <typename gumNodeId> INLINE
void gumDefaultTriangulation<gumNodeId>::setFillIns( bool b ) {
  __we_want_fill_ins = b;
}

// ================================================================================
/// the function that performs the triangulation
// ================================================================================
template <typename gumNodeId>
void gumDefaultTriangulation<gumNodeId>::__triangulate() {
// if the graph is already triangulated, no need to triangulate it again
  if ( __has_triangulation ) return;

// copy the graph to be triangulated, so that we can modify it
  gumUndiGraphTpl<gumNodeId,gumEdge> tmp_graph( __original_graph, GUM_SHALLOW_COPY );

// create a container to store for each node the weight of the clique
// it would form with its neighbours (it will be properly filled by the
// gumSimplicial structure
  gumHashTable<gumNodeId,float> log_weights( tmp_graph.getNbrNodes() );

// create a simplicial instance that will find the simplicial,
// almost simplicial and quasi simplicial nodes
  gumSimplicialSet<gumNodeId> simplicial( tmp_graph, __log_modalities, log_weights,
                                          __quasi_ratio, threshold );

  simplicial.setFillIns( __we_want_fill_ins );

// if we are to do recursive thinning, we will have to add fill-ins to the
// triangulated graph each time we eliminate a node. Hence, we shall initialize
// the triangulated graph by a copy of the original graph
  if ( __minimality_required ) {
    __triangulated_graph.shallowCopy( __original_graph );
    __has_triangulated_graph = true;
  }

// perform the triangulation
  gumNodeId removable_node = 0;

  std::pair<gumNodeId,gumNodeId> thePair;

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
      gumHashTableIterator<gumNodeId,float> iter_heuristic = log_weights.begin();
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
    if ( !__minimality_required ) {
      std::vector<gumNodeId>& cliques =
        __elim_cliques.insert( removable_node, std::vector<gumNodeId> () );
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
      gumHashTable<std::pair<gumNodeId,gumNodeId>,bool>&
      current_fill = __added_fill_ins[nb_elim];
      gumEdgeListIterator<gumEdge> iter_edges2;
      gumNodeId node1, node2;

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
            __triangulated_graph.insertEdge( gumEdge( thePair.first,thePair.second ) );
          }
        }
      }
    }

// update the simplicial structure (this will also update the temporary
// graph and the weights hashtable
    simplicial.makeClique( removable_node );

    simplicial.eraseClique( removable_node );

// update the elimination order
    __elim_order[nb_elim] = removable_node;

    __reverse_elim_order.insert( removable_node, nb_elim );
  }

  if ( __we_want_fill_ins ) {
    __fill_ins_list_standard = simplicial.getFillIns();
  }

// if minimality is required, remove the redondant edges
  if ( __minimality_required )
    __computeRecursiveThinning();

  __has_triangulation = true;
}


// ================================================================================
/// removes redondant fill-ins and compute proper elimination cliques and order
// ================================================================================
template <typename gumNodeId>
void gumDefaultTriangulation<gumNodeId>::__computeRecursiveThinning() {
// apply recursive thinning (fmint, see Kjaerulff (90), "triangulation of
// graphs - algorithms giving small total state space")
  gumNodeId node1, node2;
  bool incomplete;
  std::vector<gumNodeId> adj;
  gumHashTable<std::pair<gumNodeId,gumNodeId>,bool> T_prime;
  gumHashTable<gumNodeId,unsigned int> R( __triangulated_graph.getNbrNodes() );

  for ( gumNodeListIterator<gumNodeId> iter = __triangulated_graph.beginNodes();
        iter != __triangulated_graph.endNodes(); ++iter )
    R.insert( iter.getID(),0 );

// the FMINT loop
  for ( unsigned int i = __added_fill_ins.size()-1; i < __added_fill_ins.size(); --i ) {
    if ( __added_fill_ins[i].size() ) {
// here apply MINT to T_i = __added_fill_ins[i]
      gumHashTable<std::pair<gumNodeId,gumNodeId>,bool>& T = __added_fill_ins[i];

// compute R: by default, R is equal to all the nodes in the graph
// when R[...] >= j (see the j in the loop below), this means that the node
// belongs to an extremal edge in R

      for ( unsigned int k = 0; k < __elim_order.size(); ++k )
        R[__elim_order[k]] = 0;

// apply MINT while some edges can possibly be deleted
      bool require_mint = true;

      for ( unsigned int j = 0; require_mint; ++j ) {
// find T' (it is equal to the edges (v,w) of T such that
// the intersection of adj(v,G) and adj(w,G) is complete and such that
// v and/or w belongs to an extremal node in R
        for ( gumHashTableIterator<std::pair<gumNodeId,gumNodeId>,bool> iter_edge=T.begin();
              iter_edge != T.end(); ++iter_edge ) {
          node1 = iter_edge.key().first;
          node2 = iter_edge.key().second;

// check if at least one extremal node belongs to R

          if (( R[node1] < j ) && ( R[node2] < j ) )
            continue;

// check if the intersection of adj(v,G) and adj(w,G) is complete
          if ( __triangulated_graph.getneighbourEdges( node2 ).size() <
               __triangulated_graph.getneighbourEdges( node1 ).size() ) {
            gumNodeId tmp = node1;
            node1 = node2;
            node2 = tmp;
          }

          incomplete = false;

// find the nodes belonging to the intersection of adj(v,G) and adj(w,G)

          for ( gumEdgeListIterator<gumEdge> iter_adj =
                  __triangulated_graph.beginneighbourEdges( node1 );
                iter_adj != __triangulated_graph.endEdges(); ++iter_adj ) {
            if ( __triangulated_graph.existsEdge( node2,iter_adj->other( node1 ) ) )
              adj.push_back( iter_adj->other( node1 ) );
          }

// check if the intersection is complete
          for ( unsigned int k = 0; k < adj.size() && !incomplete; ++k ) {
            for ( unsigned int m = k+1; m < adj.size(); ++m ) {
              if ( !__triangulated_graph.existsEdge( adj[k],adj[m] ) ) {
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
        for ( gumHashTableIterator<std::pair<gumNodeId,gumNodeId>,bool>
              iter_delete = T_prime.begin();
              iter_delete != T_prime.end(); ++iter_delete ) {
          T.erase( iter_delete.key() );
          __triangulated_graph.eraseEdge( iter_delete.key().first,
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
  gumPriorityQueue< gumNodeId,unsigned int,std::greater<unsigned int> >
  numbered_neighbours( std::greater<unsigned int>(), false,
                       __triangulated_graph.getNbrNodes() );

  for ( unsigned int i = 0; i< __elim_order.size(); ++i )
    numbered_neighbours.insert( 0, __elim_order[i] );

// perform the maximum cardinality search
  for ( unsigned int i = __elim_order.size() - 1; i < __elim_order.size(); --i ) {
    gumNodeId node = numbered_neighbours.pop();
    __elim_order[i] = node;
    __reverse_elim_order[node] = i;

    for ( gumEdgeListIterator<gumEdge>
          iter = __triangulated_graph.beginneighbourEdges( node );
          iter != __triangulated_graph.endEdges(); ++iter ) {
      gumNodeId neighbour = iter->other( node );

      try {
        numbered_neighbours.setPriorityByVal
        ( neighbour, 1 + numbered_neighbours.getPriorityByVal( neighbour ) );
      } catch ( gumNotFound& ) { }
    }
  }

// here the elimination order is ok. We now need to update the __elim_cliques
  for ( unsigned int i = 0; i < __elim_order.size(); ++i ) {
    std::vector<gumNodeId>& cliques =
      __elim_cliques.insert( __elim_order[i], std::vector<gumNodeId> () );
    cliques.push_back( __elim_order[i] );

    for ( gumEdgeListIterator<gumEdge>
          iter = __triangulated_graph.beginneighbourEdges( __elim_order[i] );
          iter != __triangulated_graph.endEdges(); ++iter ) {
      gumNodeId neighbour = iter->other( __elim_order[i] );

      if ( __reverse_elim_order[neighbour] > i ) {
        cliques.push_back( neighbour );
      }
    }
  }
}


// ================================================================================
/// returns an elimination tree from a triangulated graph
// ================================================================================
template <typename gumNodeId>
void gumDefaultTriangulation<gumNodeId>::__computeEliminationTree() {
// if there already exists an elimination tree, no need to compute it again
  if ( __has_elimination_tree ) return;

// if the graph is not triangulated, triangulate it
  if ( !__has_triangulation )
    __triangulate();

// create the nodes of the elimination tree
  gumNodeList<gumNodeId> clique;

  std::ostringstream ost;

  for ( unsigned int i = 0; i < __elim_order.size(); ++i ) {
// form the elimination clique relative to the node
    clique.clear();
    std::vector<gumNodeId>& list_of_nodes = __elim_cliques[__elim_order[i]];

    for ( unsigned int j = 0; j < list_of_nodes.size(); ++j )
      clique.shallowInsert( __original_graph.shallowGetNode( list_of_nodes[j] ) );

// insert the node
    ost.str( "" );

    ost << "clique " << __original_graph.getNode( __elim_order[i] ).label();

    __elim_tree.insertNode( gumNode( __elim_order[i], ost.str() ),
                            clique, GUM_SHALLOW_COPY );
  }

// create the edges of the elimination tree: join a node to the one in its clique
// that is eliminated first
  for ( unsigned int i = 0; i < __elim_order.size(); ++i ) {
    std::vector<gumNodeId>& list_of_nodes = __elim_cliques[__elim_order[i]];
    unsigned int child = __original_graph.getNbrNodes() + 1;

    for ( unsigned int j = 0; j < list_of_nodes.size(); ++j )
      if (( list_of_nodes[j] != __elim_order[i] ) &&
          ( child > __reverse_elim_order[list_of_nodes[j]] ) )
        child = __reverse_elim_order[list_of_nodes[j]];

    if ( child < __original_graph.getNbrNodes() )
      __elim_tree.insertEdge( gumEdge( __elim_order[i], __elim_order[child] ) );
  }

  __has_elimination_tree = true;
}


// ================================================================================
/// computes a junction tree from an elimination tree
// ================================================================================
template <typename gumNodeId>
void gumDefaultTriangulation<gumNodeId>::__computeJunctionTree() {
// if there already exists a junction tree, no need to recompute it
  if ( __has_junction_tree ) return;

// if there is no elimination tree, create it
  if ( !has_elimination_tree )
    __computeEliminationTree();

// copy the elimination tree into the junction tree
  __junction_tree.shallowCopy( __elim_tree );

//mark all the edges of the junction tree to false
  gumHashTable<std::pair<gumNodeId,gumNodeId>,bool> mark =
    __junction_tree.hashMapEdges( false );

// create a vector indicating by which clique a given clique has been
// substituted during the transformation from the elimination tree to the
// junction tree. Assume that clique J the elmination tree has been substituted
// by clique K of the elimination tree, and that clique J (resp. K) was the
// jth (resp. kth) one created during the triangulation process. Then, in the
// vector below, substitution[j] = k.
  std::vector<unsigned int> substitution( __elim_order.size() );

  for ( unsigned int i = 0; i < substitution.size(); ++i )
    substitution[i] = i;

// for all cliques C_i, from the last one created to the first one, check if there
// exists a parent C_j (a neighbour created before C_i) such that |C_j| = |C_i|
// and such that the edge is not marked. In this case, this means that C_j
// contains C_i. Hence, we should remove C_i, and link all of its neighbours to
// C_j. These links will be marked to true as no such neighbour can be included
// in C_j (and conversely).
  std::pair<gumNodeId,gumNodeId> thePair;

  gumEdgeListIterator<gumEdge> iter;

  for ( unsigned int i=__elim_order.size()-1; i < __elim_order.size(); --i ) {
    gumNodeId C_i = __elim_order[i];
    unsigned int card_C_i = __junction_tree.getClique( C_i ).size();

// search for C_j such that |C_j| = [C_i| + 1
    gumNodeId C_j = C_i;

    for ( iter = __junction_tree.beginneighbourEdges( C_i );
          iter != __junction_tree.endEdges(); ++iter ) {
      gumNodeId C_jj = iter->other( C_i );
      thePair.first  = std::min( C_i,C_jj );
      thePair.second = std::max( C_i,C_jj );

      if (( __reverse_elim_order[C_i] > __reverse_elim_order[C_jj] ) && !mark[thePair] &&
          ( __junction_tree.getClique( C_jj ).size() == card_C_i + 1 ) ) {
// ok, here we found a parent such that |C_jj| = [C_i| + 1
        C_j = C_jj;
        __junction_tree.eraseEdge( C_i,C_j );
        break;
      }
    }

// if we found a C_j, link the neighbours of C_i to C_j
    if ( C_j != C_i ) {
      for ( iter = __junction_tree.beginneighbourEdges( C_i );
            iter != __junction_tree.endEdges(); ++iter ) {
        gumEdge new_edge( C_j,iter->other( C_i ) );
        __junction_tree.insertEdge( new_edge );
        thePair.first  = new_edge.first();
        thePair.second = new_edge.second();
        mark.insert( thePair, true );
      }

      substitution[i] = __reverse_elim_order[C_j];

      __junction_tree.eraseNode( C_i );
    }
  }

// compute the transitive closure of vector substitution
  for ( unsigned int i = 0; i < substitution.size(); ++i )
    substitution[i] = substitution[substitution[i]];

// using the transitive closure of vector substitution, compute for each node
// the clique of the junction tree that was created by its elimination during
// the triangulation
  for ( unsigned int i = 0; i < substitution.size(); ++i )
    __node_2_junction_clique.insert( __elim_order[i], __elim_order[substitution[i]] );

  __has_junction_tree = true;
}


// ================================================================================
/// used for computing the junction tree of the maximal prime subgraphs
// ================================================================================
template <typename gumNodeId>
void gumDefaultTriangulation<gumNodeId>::__computeMaxPrimeMergings
( const gumNodeId& node, const gumNodeId& from,
  std::vector< std::pair<gumNodeId,gumNodeId> >& merged_cliques,
  gumHashTable<gumNodeId,bool>& mark ) const {
  mark[node] = true;
  gumNodeListIterator<gumNodeId> iter_sep2;

  for ( gumEdgeListIterator<gumEdge> iter_sep=__junction_tree.beginneighbourEdges( node );
        iter_sep != __junction_tree.endEdges(); ++iter_sep ) {
    gumNodeId other_node = iter_sep->other( node );

    if ( other_node != from ) {
      const gumNodeList<gumNodeId>&
      separator = __junction_tree.getSeparator( node, other_node );
// check that the separator between node and other_node is complete
      bool complete = true;

      for ( gumNodeListIterator<gumNodeId> iter_sep1 = separator.begin();
            iter_sep1 != separator.end() && complete; ++iter_sep1 ) {
        iter_sep2 = iter_sep1;

        for ( ++iter_sep2; iter_sep2 != separator.end(); ++iter_sep2 ) {
          if ( !__original_graph.existsEdge( iter_sep1->getID(),iter_sep2->getID() ) ) {
            complete = false;
            break;
          }
        }
      }

// here complete indicates whether the separator is complete or not
      if ( !complete )
        merged_cliques.push_back( std::pair<gumNodeId,gumNodeId>( other_node,node ) );

      __computeMaxPrimeMergings( other_node, node, merged_cliques, mark );
    }
  }
}


// ================================================================================
/// computes the junction tree of the maximal prime subgraphs
// ================================================================================
template <typename gumNodeId>
void gumDefaultTriangulation<gumNodeId>::__computeMaxPrimeJunctionTree() {
// if there already exists a max prime junction tree, no need to recompute it
  if ( __has_max_prime_junction_tree ) return;

// if there is no junction tree, create it
  if ( !has_junction_tree )
    __computeJunctionTree();

// the maximal prime subgraph join tree is created by aggregation of some
// cliques. More precisely, when the separator between 2 cliques is not complete
// in the original graph, then the two cliques must be merged. Create a hashtable
// indicating which clique has been absorbed by some other clique.
  gumHashTable<gumNodeId,gumNodeId>
  T_mpd_cliques( __junction_tree.getNbrNodes() );

  for ( gumNodeListIterator<gumNode> iter_clique = __junction_tree.beginNodes();
        iter_clique != __junction_tree.endNodes(); ++iter_clique )
    T_mpd_cliques.insert( iter_clique.getID(),iter_clique.getID() );

// parse all the separators of the junction tree and test those that are not
// complete in the orginal graph
  std::vector< std::pair<gumNodeId,gumNodeId> > merged_cliques;

  gumHashTable<gumNodeId,bool> mark = __junction_tree.hashMapNodes( false );

  for ( gumNodeListIterator<gumNode> iter_clique = __junction_tree.beginNodes();
        iter_clique != __junction_tree.endNodes(); ++iter_clique )
    if ( !mark[iter_clique.getID()] )
      __computeMaxPrimeMergings( iter_clique.getID(), iter_clique.getID(),
                                 merged_cliques, mark );

// compute the transitive closure of merged_cliques. This one will contain
// pairs (X,Y) indicating that clique X must be merged with clique Y.
// Actually clique X will be inserted into clique Y.
  for ( unsigned int i = 0; i < merged_cliques.size(); ++i ) {
    T_mpd_cliques[merged_cliques[i].first]=T_mpd_cliques[merged_cliques[i].second];
  }

// now we can create the max prime junction tree. First, create the cliques
  __max_prime_junction_tree.clearNodes();

  for ( gumHashTableIterator<gumNodeId,gumNodeId> iter_clique = T_mpd_cliques.begin();
        iter_clique != T_mpd_cliques.end(); ++iter_clique ) {
    if ( iter_clique.key() == *iter_clique ) {
      __max_prime_junction_tree.insertNode( __junction_tree.getNode( *iter_clique ),
                                            __junction_tree.getClique( *iter_clique ),
                                            GUM_SHALLOW_COPY );
    }
  }

// add to the cliques previously created the nodes of the cliques that were
// merged into them
  for ( gumHashTableIterator<gumNodeId,gumNodeId>  iter = T_mpd_cliques.begin();
        iter != T_mpd_cliques.end(); ++iter ) {
    if ( iter.key() != *iter ) {
      const gumNodeList<gumNodeId>& new_clique = __junction_tree.getClique( iter.key() );

      for ( gumNodeListIterator<gumNode> iter_node = new_clique.begin();
            iter_node != new_clique.end(); ++iter_node ) {
        try {
          __max_prime_junction_tree.addToClique( *iter, iter_node.shallowGet(),
                                                 GUM_SHALLOW_COPY );
        } catch ( gumDuplicateElement& ) { }
      }
    }
  }

// add the edges to the graph
  for ( gumEdgeListIterator<gumEdge> iter_edge = __junction_tree.beginEdges();
        iter_edge != __junction_tree.endEdges(); ++iter_edge ) {
    gumNodeId node1 = T_mpd_cliques[iter_edge->first()];
    gumNodeId node2 = T_mpd_cliques[iter_edge->second()];

    if ( node1 != node2 ) {
      try {
        __max_prime_junction_tree.insertEdge( gumEdge( node1,node2 ) );
      } catch ( gumDuplicateElement& ) { }
    }
  }

// compute for each node which clique of the max prime junction tree was created
// by the elmination of the node
  for ( gumHashTableIterator<gumNodeId,gumNodeId>
        iter_junction = __node_2_junction_clique.begin();
        iter_junction != __node_2_junction_clique.end(); ++iter_junction )
    __node_2_max_prime_clique.insert( iter_junction.key(),
                                      T_mpd_cliques[*iter_junction] );

  __has_max_prime_junction_tree = true;
}


// ================================================================================
/// returns the fill-ins added by the triangulation algorithm
// ================================================================================
template <typename gumNodeId> INLINE
const gumEdgeList<gumEdge>& gumDefaultTriangulation<gumNodeId>::getFillIns() {
  if ( !__has_triangulation )
    __triangulate();

  if ( __minimality_required || ( ! __we_want_fill_ins ) ) {
    return __fill_ins;
  } else {
    return __fill_ins_list_standard;
  }
}


// ================================================================================
/// returns an elimination ordering compatible with the triangulated graph
// ================================================================================
template <typename gumNodeId> INLINE
const std::vector<gumNodeId>& gumDefaultTriangulation<gumNodeId>::eliminationOrder() {
  if ( !__has_triangulation )
    __triangulate();

  return __elim_order;
}


// ================================================================================
/** @brief returns the number of a given node in the elimination order
* (0 = first node eliminated) */
// ================================================================================
template <typename gumNodeId> INLINE
unsigned int gumDefaultTriangulation<gumNodeId>::eliminationOrder( const gumNodeId& id ) {
  if ( !__has_triangulation )
    __triangulate();

  return __reverse_elim_order[id];
}


// ================================================================================
/// returns the triangulated graph
// ================================================================================
template <typename gumNodeId> INLINE
const gumUndiGraphTpl<gumNodeId,gumEdge>&
gumDefaultTriangulation<gumNodeId>::triangulatedGraph() {
  if ( !__has_triangulation )
    __triangulate();

// if we did not construct the triangulated graph during the triangulation, that
// is, we just constructed the junction tree, we shall construct it now
  if ( !has_triangulated_graph ) {
// just in case, be sure that the junction tree has been constructed
    if ( !has_junction_tree )
      __computeJunctionTree();

// copy the original graph
    __triangulated_graph.shallowCopy( __original_graph );

// parse all the cliques of the junction tree
    gumNodeListIterator<gumNodeId> iter_clique2;

    for ( gumNodeListIterator<gumNode> iter_node = __junction_tree.beginNodes();
          iter_node != __junction_tree.endNodes(); ++iter_node ) {
// for each clique, add the edges necessary to make it complete
      const gumNodeList<gumNodeId>& clique = __junction_tree.getClique( *iter_node );
      std::vector<gumNodeId> clique_nodes( clique.size() );
      unsigned int i =0;

      for ( gumNodeListIterator<gumNodeId> iter_clique = clique.begin();
            iter_clique != clique.end(); ++iter_clique, ++i )
        clique_nodes[i] = iter_clique->getID();

      for ( i = 0; i < clique_nodes.size(); ++i ) {
        for ( unsigned int j = i+1; j < clique_nodes.size(); ++j ) {
          try { __triangulated_graph.insertEdge( gumEdge( i,j ) ); }
          catch ( gumDuplicateElement& ) { }
        }
      }
    }

    __has_triangulated_graph = true;
  }

  return __triangulated_graph;
}


// ================================================================================
/// returns the elimination tree of a compatible ordering
// ================================================================================
template <typename gumNodeId> INLINE
const gumCliqueGraphTpl<gumNode,gumEdge,gumNodeId>&
gumDefaultTriangulation<gumNodeId>::eliminationTree() {
  if ( !has_elimination_tree )
    __computeEliminationTree();

  return __elim_tree;
}


// ================================================================================
/// returns a compatible junction tree
// ================================================================================
template <typename gumNodeId> INLINE
const gumCliqueGraphTpl<gumNode,gumEdge,gumNodeId>&
gumDefaultTriangulation<gumNodeId>::junctionTree() {
  if ( !has_junction_tree )
    __computeJunctionTree();

  return __junction_tree;
}


// ================================================================================
/// returns a junction tree of maximal prime subgraphs
// ================================================================================
template <typename gumNodeId> INLINE
const gumCliqueGraphTpl<gumNode,gumEdge,gumNodeId>&
gumDefaultTriangulation<gumNodeId>::maxPrimeSubgraphTree() {
  if ( !has_max_prime_junction_tree )
    __computeMaxPrimeJunctionTree();

  return __max_prime_junction_tree;
}


// ================================================================================
/** @brief returns the Id of the maximal prime subgraph created by the
* elimination of a given node during the triangulation process */
// ==============================================================================
template <typename gumNodeId> INLINE
const gumNodeId& gumDefaultTriangulation<gumNodeId>::createdMaxPrimeSubgraph( const gumNodeId& id ) {
  if ( !has_max_prime_junction_tree )
    __computeMaxPrimeJunctionTree();

  return __node_2_max_prime_clique[id];
}


// ================================================================================
/** @brief returns the Id of the clique created by the
* elimination of a given node during the triangulation process */
// ==============================================================================
template <typename gumNodeId> INLINE
const gumNodeId& gumDefaultTriangulation<gumNodeId>::createdClique( const gumNodeId& id ) {
  if ( !has_junction_tree )
    __computeJunctionTree();

  return __node_2_junction_clique[id];
}









/* ============================================================================== */
/* ============================================================================== */
/* ===             CLASS FOR COMPUTING INCREMENTAL TRIANGULATIONS             === */
/* ============================================================================== */
/* ============================================================================== */

// ================================================================================
/// constructor
// ================================================================================
template <typename gumNodeId, template <typename> class Triangulation>
template <typename Edge>
gumIncrementalTriangulation<gumNodeId,Triangulation>::gumIncrementalTriangulation
( const gumUndiGraphTpl<gumNodeId,Edge>& theGraph,
  const gumHashTable<gumNodeId,unsigned int>& modal ) : require_update( false ) {
// for debugging purposes
  GUM_CONSTRUCTOR( gumIncrementalTriangulation );

// shallow copy the graph passed in argument and update the structures
// containing the informations useful for the triangulation

  for ( gumNodeListIterator<gumNodeId> iter_node = theGraph.beginNodes();
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
template <typename gumNodeId, template <typename> class Triangulation>
gumIncrementalTriangulation<gumNodeId,Triangulation>::gumIncrementalTriangulation() :
    require_update( false ) {
// for debugging purposes
  GUM_CONSTRUCTOR( gumIncrementalTriangulation );
}


// ================================================================================
/// copy operator
// ================================================================================
template <typename gumNodeId, template <typename> class Triangulation>
gumIncrementalTriangulation<gumNodeId,Triangulation>::gumIncrementalTriangulation
( const gumIncrementalTriangulation<gumNodeId,Triangulation>& from ) :
    modalities( from.modalities ), __junction_tree( from.__junction_tree ), T_mpd( from.T_mpd ),
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
template <typename gumNodeId, template <typename> class Triangulation>
gumIncrementalTriangulation<gumNodeId,Triangulation>::~gumIncrementalTriangulation() {
// for debugging purposes
  GUM_DESTRUCTOR( gumIncrementalTriangulation );
}


// ================================================================================
/// copy operator
// ================================================================================
template <typename gumNodeId, template <typename> class Triangulation>
gumIncrementalTriangulation<gumNodeId,Triangulation>&
gumIncrementalTriangulation<gumNodeId,Triangulation>::operator=
( const gumIncrementalTriangulation<gumNodeId,Triangulation>& from ) {
// avoid self assignment
  if ( this != &from ) {
// for debugging purposes
    GUM_OP_CPY( gumIncrementalTriangulation );

// copy all the structures stored in "from"
    graph.shallowCopy( from.graph );
    modalities = from.modalities;
    __junction_tree = from.__junction_tree;
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
template <typename gumNodeId, template <typename> class Triangulation>
void gumIncrementalTriangulation<gumNodeId,Triangulation>::insertNode
( const gumRefPtr<gumNodeId>& node, unsigned int modal ) {
// add the new node to the graph
  graph.shallowInsertNode( node );
  modalities.insert( node->getID(), modal );

// add a new clique to T_mpd and the junction tree
  gumNodeList<gumNodeId> clique_nodes( 2 );
  clique_nodes.shallowInsert( node );
  gumNode clique = gumNode( idFactory.getNewId(),"" );
  gumNode& MPS = T_mpd.insertNode( clique, clique_nodes, GUM_SHALLOW_COPY );
  gumNode& new_clique =
    __junction_tree.insertNode( clique, clique_nodes, GUM_SHALLOW_COPY );

// indicate in which MPS node belongs
  gumList<gumNodeId>& list_of_mps = mps_of_node.insert( node->getID(),gumList<gumNodeId>() );
  list_of_mps.insert( MPS.getID() );

// indicate in which MPS the clique added to the junction tree belongs
  std::vector<gumNodeId>& cliquesMPS =
    cliques_of_mps.insert( MPS.getID(), std::vector<gumNodeId>() );
  cliquesMPS.push_back( new_clique.getID() );
  mps_of_clique.insert( new_clique.getID(),MPS.getID() );

// indicate that the new MPS should not be affected by a triangulation
  mps_affected.insert( MPS.getID(),false );
}


// ================================================================================
/// mark the mps affected by the deletion of a given edge
// ================================================================================
template <typename gumNodeId, template <typename> class Triangulation>
void gumIncrementalTriangulation<gumNodeId,Triangulation>::markAffectedMPSsByRemoveLink
( const gumNodeId& My, const gumNodeId& Mz, const std::pair<gumNodeId,gumNodeId>& edge ) {
// mark the MPS so that it will be retriangulated
  mps_affected[My] = true;

// mark all the neighbour MPS that contain edge

  for ( gumEdgeListIterator<gumEdge> iter = T_mpd.beginneighbourEdges( My );
        iter != T_mpd.endEdges(); ++iter ) {
    if ( iter->other( My ) != Mz ) {
      const gumNodeList<gumNodeId>& Syk = T_mpd.getSeparator( *iter );

      if ( Syk.exists( edge.first ) && Syk.exists( edge.second ) )
        markAffectedMPSsByRemoveLink( iter->other( My ), My, edge );
    }
  }
}


// ================================================================================
/// removes an edge from the graph (the join tree may need a retriangulation)
// ================================================================================
template <typename gumNodeId, template <typename> class Triangulation>
void gumIncrementalTriangulation<gumNodeId,Triangulation>::eraseEdge
( const gumNodeId& X, const gumNodeId& Y ) {
// check that the edge exist
  if ( !graph.existsEdge( X,Y ) ) return;

// find a MPS containing the edge (X,Y)
  const gumList<gumNodeId>& mps1 = mps_of_node[X];

  const gumList<gumNodeId>& mps2 = mps_of_node[Y];

  gumNodeId Mx = mps1[0];

  if ( mps1.size() <= mps2.size() ) {
    for ( gumListIterator<gumNodeId> iter = mps1.begin(); iter != mps1.end(); ++iter ) {
      if ( T_mpd.getClique( *iter ).exists( Y ) ) {
        Mx = *iter;
        break;
      }
    }
  } else {
    for ( gumListIterator<gumNodeId> iter = mps2.begin(); iter != mps2.end(); ++iter ) {
      if ( T_mpd.getClique( *iter ).exists( X ) ) {
        Mx = *iter;
        break;
      }
    }
  }

// mark the MPS that need be updated
  markAffectedMPSsByRemoveLink
  ( Mx, Mx, std::pair<gumNodeId,gumNodeId> ( std::min( X,Y ), std::max( X,Y ) ) );

  require_update = true;

// remove the edge (X,Y) from the graph
  graph.eraseEdge( X,Y );
}


// ================================================================================
/// removes a node from the graph (the join tree may need a triangulation update)
// ================================================================================
template <typename gumNodeId, template <typename> class Triangulation>
void gumIncrementalTriangulation<gumNodeId,Triangulation>::eraseNode( const gumNodeId& X ) {
// remove all the edges adjacent to the node
  for ( gumEdgeListIterator<gumEdge> neighbour_edge = graph.beginneighbourEdges( X );
        neighbour_edge != graph.endEdges(); ++neighbour_edge )
    eraseEdge( neighbour_edge->first(), neighbour_edge->second() );

// remove X from the MPS containing X
  gumList<gumNodeId>& MPS_of_X = mps_of_node[X];

  for ( gumListIterator<gumNodeId> iter=MPS_of_X.begin(); iter!=MPS_of_X.end(); ++iter ) {
    T_mpd.eraseFromClique( *iter, X );
// if the intersection between *iter and one of its neighbour is empty,
// remove the edge linking them

    for ( gumEdgeListIterator<gumEdge> neighbour = T_mpd.beginneighbourEdges( *iter );
          neighbour != T_mpd.endEdges(); ++neighbour )
      if ( T_mpd.getSeparator( *iter,neighbour->other( *iter ) ).size()==0 )
        T_mpd.eraseEdge( *iter,neighbour->other( *iter ) );
  }

// remove X from the cliques containing X
  for ( gumListIterator<gumNodeId> iter=MPS_of_X.begin(); iter!=MPS_of_X.end(); ++iter ) {
    const std::vector<gumNodeId>& cliques_of_X = cliques_of_mps[*iter];

    for ( unsigned int i = 0; i < cliques_of_X.size(); ++i ) {
      __junction_tree.eraseFromClique( cliques_of_X[i], X );
// if the intersection between *iter and one of its neighbour is empty,
// remove the edge linking them

      for ( gumEdgeListIterator<gumEdge> neighbour =
              __junction_tree.beginneighbourEdges( cliques_of_X[i] );
            neighbour != __junction_tree.endEdges(); ++neighbour )
        if ( __junction_tree.getSeparator( cliques_of_X[i],
                                           neighbour->other( cliques_of_X[i] ) ).size()==0 )
          __junction_tree.eraseEdge( cliques_of_X[i],
                                     neighbour->other( cliques_of_X[i] ) );
    }
  }

// if the MPS containing X is empty, then remove it, as well as the corresponding
// clique in the junction tree (which also only contain X)
  if (( MPS_of_X.size()==1 ) && ( T_mpd.getClique( MPS_of_X[0] ).size()==0 ) ) {
    __junction_tree.eraseNode( cliques_of_mps[MPS_of_X[0]][0] );
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
template <typename gumNodeId, template <typename> class Triangulation> INLINE
void gumIncrementalTriangulation<gumNodeId,Triangulation>::eraseNode( const gumNodeId& X ) {
  eraseNode( X.getID() );
}


// ================================================================================
/// add a new edge to the graph
// ================================================================================
template <typename gumNodeId, template <typename> class Triangulation>
int gumIncrementalTriangulation<gumNodeId,Triangulation>::markAffectedMPSsByAddLink
( const gumNodeId& Mx, const gumNodeId& Mz, const gumNodeId& X, const gumNodeId& Y ) {
// check if Mx contains Y. In this case, mark Mx and return 1 to indicate that
// Y has been found or 2 to indicate that Y has been found and that the nearest
// MPS containing X has been marked
  const gumNodeList<gumNodeId>& cliqueMX = T_mpd.getClique( Mx );

  if ( cliqueMX.exists( Y ) ) {
    mps_affected[Mx] = true;

    if ( cliqueMX.exists( X ) ) return 2;

    return 1;
  }

// parse Mx's neighbours until we find Y
  for ( gumEdgeListIterator<gumEdge> neighbour = T_mpd.beginneighbourEdges( Mx );
        neighbour != T_mpd.endEdges(); ++neighbour ) {
    gumNodeId other_node = neighbour->other( Mx );

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
template <typename gumNodeId, template <typename> class Triangulation>
void gumIncrementalTriangulation<gumNodeId,Triangulation>::insertEdge
( const gumNodeId& X, const gumNodeId& Y ) {
// add the edge to the graph
  graph.insertEdge( gumEdge( X,Y ) );

// take any MPS containing X and search its tree to find Y
  gumNodeId& mps_X = mps_of_node[X][0];
  int found = markAffectedMPSsByAddLink( mps_X, mps_X, X, Y );

  if ( found == 0 ) {
// the mps containing X do not belong to the same tree as those containing Y
    gumNodeId& mps_Y = mps_of_node[Y][0];

// find a clique in mps_X containing X and another in mps_Y containing Y
// and add a clique XY to the junction tree linked to the cliques found
// in mps_X and mps_Y
    const std::vector<gumNodeId>& cliques_X = cliques_of_mps[mps_X];
    const std::vector<gumNodeId>& cliques_Y = cliques_of_mps[mps_Y];
    gumNodeId c_X = 0, c_Y = 0;

    for ( unsigned int i = 0; i < cliques_X.size(); ++i ) {
      if ( __junction_tree.getClique( cliques_X[i] ).exists( X ) ) {
        c_X = cliques_X[i];
        break;
      }
    }

    for ( unsigned int i = 0; i < cliques_Y.size(); ++i ) {
      if ( __junction_tree.getClique( cliques_Y[i] ).exists( Y ) ) {
        c_Y = cliques_Y[i];
        break;
      }
    }

// link c_Y and c_X through a new node containing XY
    gumNodeId newID = idFactory.getNewId();

    gumNode newNode = gumNode( newID,"" );

    gumNodeList<gumNodeId> nodes( 2 );

    nodes.shallowInsert( graph.shallowGetNode( X ) );

    nodes.shallowInsert( graph.shallowGetNode( Y ) );

    __junction_tree.insertNode( newNode, nodes, GUM_SHALLOW_COPY );

    __junction_tree.insertEdge( gumEdge( newID, c_X ) );

    __junction_tree.insertEdge( gumEdge( newID, c_Y ) );

    T_mpd.insertNode( newNode, nodes, GUM_SHALLOW_COPY );

    T_mpd.insertEdge( gumEdge( newID,mps_X ) );

    T_mpd.insertEdge( gumEdge( newID,mps_Y ) );

// check that the maximal prime subgraph containing X is not X alone
// in this case, remove this max prime subgraph, as well as the corresponding
// clique in the junction tree
    if ( T_mpd.getClique( mps_X ).size() == 1 ) {
      __junction_tree.eraseNode( c_X );
      T_mpd.eraseNode( mps_X );
      mps_affected.erase( mps_X );
      mps_of_clique.erase( c_X );
      cliques_of_mps.erase( mps_X );
      mps_X = newID;
    } else
      mps_of_node[X].insert( newID );

// do the same thing as above for node Y
    if ( T_mpd.getClique( mps_Y ).size() == 1 ) {
      __junction_tree.eraseNode( c_Y );
      T_mpd.eraseNode( mps_Y );
      mps_affected.erase( mps_Y );
      mps_of_clique.erase( c_Y );
      cliques_of_mps.erase( mps_Y );
      mps_Y = newID;
    } else
      mps_of_node[Y].insert( newID );

    std::vector<gumNodeId>& cl = cliques_of_mps.insert( newID,std::vector<gumNodeId>() );

    cl.push_back( newID );

    mps_of_clique.insert( newID,newID );

    mps_affected.insert( newID,false );
  } else
    require_update = true;
}


// ================================================================================
/// set up a connected subgraph to be triangulated
// ================================================================================
template <typename gumNodeId, template <typename> class Triangulation>
void gumIncrementalTriangulation<gumNodeId,Triangulation>::setUpConnectedTriangulation
( gumNodeId Mx, gumNodeId Mfrom, gumUndiGraphTpl<gumNodeId,gumEdge>& theGraph,
  std::vector<gumEdge>& notAffectedneighbourCliques,
  gumHashTable<gumNodeId,bool>& cliques_affected ) {
// mark the clique so that we won't try to update it several times
  cliques_affected[Mx] = false;

// get the nodes that are concerned by the triangulation update
  const gumNodeList<gumNodeId>& clique = __junction_tree.getClique( Mx );

  for ( gumNodeListIterator<gumNodeId> iter_node = clique.begin();
        iter_node != clique.end(); ++iter_node )
    if ( !theGraph.existsNode( iter_node->getID() ) )
      theGraph.shallowInsertNode( iter_node.shallowGet() );

// go on with the neighbour cliques in the junction tree
  for ( gumEdgeListIterator<gumEdge> neighbour = __junction_tree.beginneighbourEdges( Mx );
        neighbour != __junction_tree.endEdges(); ++neighbour ) {
    gumNodeId othernode = neighbour->other( Mx );

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
template <typename gumNodeId, template <typename> class Triangulation>
void gumIncrementalTriangulation<gumNodeId,Triangulation>::__computeMaxPrimeMergings
( const gumNodeId& node, const gumNodeId& from,
  std::vector< std::pair<gumNodeId,gumNodeId> >& merged_cliques,
  gumHashTable<gumNodeId,bool>& mark, const gumNodeId& max_old_id ) const {
  mark[node] = true;
  gumNodeListIterator<gumNodeId> iter_sep2;
// check the separators on all the adjacent edges of Mx

  for ( gumEdgeListIterator<gumEdge> iter_sep=__junction_tree.beginneighbourEdges( node );
        iter_sep != __junction_tree.endEdges(); ++iter_sep ) {
    gumNodeId other_node = iter_sep->other( node );

    if ( other_node != from ) {
      const gumNodeList<gumNodeId>&
      separator = __junction_tree.getSeparator( node, other_node );
// check that the separator between node and other_node is complete
      bool complete = true;

      for ( gumNodeListIterator<gumNodeId> iter_sep1 = separator.begin();
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
        merged_cliques.push_back( std::pair<gumNodeId,gumNodeId>( other_node,node ) );

      if ( other_node > max_old_id )
        __computeMaxPrimeMergings( other_node, node, merged_cliques,
                                   mark, max_old_id );
    }
  }
}

// ================================================================================
// ================================================================================
template <typename gumNodeId, template <typename> class Triangulation>
void gumIncrementalTriangulation<gumNodeId,Triangulation>::check() {
// check que les noeuds du graphe appartiennent a l'arbre de jonction
  {
    gumHashTable<gumNodeId,bool> nodes = graph.hashMapNodes( false );

    for ( gumNodeListIterator<gumNode> iter = __junction_tree.beginNodes();
          iter != __junction_tree.endNodes(); ++iter ) {
      const gumNodeList<gumNodeId>& clique = __junction_tree.getClique( iter.getID() );

      for ( gumNodeListIterator<gumNodeId> iter2 = clique.begin();
            iter2 != clique.end(); ++iter2 )
        nodes[iter2.getID()] = true;
    }

    for ( gumHashTableIterator<gumNodeId,bool> iter = nodes.begin();
          iter != nodes.end(); ++iter )
      if ( !*iter ) {
        std::cerr << "check nodes" << std::endl
        << graph << std::endl << __junction_tree << std::endl;
      }
  }

// check que les arcs du graphe appartiennent a l'arbre de jonction
  {
    gumNodeListIterator<gumNodeId> iter3;
    std::pair<gumNodeId,gumNodeId> thePair;
    gumHashTable<std::pair<gumNodeId,gumNodeId>,bool> edges = graph.hashMapEdges( false );

    for ( gumNodeListIterator<gumNode> iter = __junction_tree.beginNodes();
          iter != __junction_tree.endNodes(); ++iter ) {
      const gumNodeList<gumNodeId>& clique = __junction_tree.getClique( iter.getID() );

      for ( gumNodeListIterator<gumNodeId> iter2 = clique.begin();
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

    for ( gumHashTableIterator<std::pair<gumNodeId,gumNodeId>,bool> iter = edges.begin();
          iter != edges.end(); ++iter )
      if ( !*iter ) {
        std::cerr << "check edges" << std::endl
        << graph << std::endl << __junction_tree << std::endl;
      }
  }

// check que les noeuds du graphe appartiennent a l'arbre de MPS
  {
    gumHashTable<gumNodeId,bool> nodes = graph.hashMapNodes( false );

    for ( gumNodeListIterator<gumNode> iter = T_mpd.beginNodes();
          iter != T_mpd.endNodes(); ++iter ) {
      const gumNodeList<gumNodeId>& clique = T_mpd.getClique( iter.getID() );

      for ( gumNodeListIterator<gumNodeId> iter2 = clique.begin();
            iter2 != clique.end(); ++iter2 )
        nodes[iter2.getID()] = true;
    }

    for ( gumHashTableIterator<gumNodeId,bool> iter = nodes.begin();
          iter != nodes.end(); ++iter )
      if ( !*iter ) {
        std::cerr << "check nodes" << std::endl
        << graph << std::endl << T_mpd << std::endl;
      }
  }

// check que les arcs du graphe appartiennent a l'arbre de MPS
  {
    gumNodeListIterator<gumNodeId> iter3;
    std::pair<gumNodeId,gumNodeId> thePair;
    gumHashTable<std::pair<gumNodeId,gumNodeId>,bool> edges = graph.hashMapEdges( false );

    for ( gumNodeListIterator<gumNode> iter = T_mpd.beginNodes();
          iter != T_mpd.endNodes(); ++iter ) {
      const gumNodeList<gumNodeId>& clique = T_mpd.getClique( iter.getID() );

      for ( gumNodeListIterator<gumNodeId> iter2 = clique.begin();
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

    for ( gumHashTableIterator<std::pair<gumNodeId,gumNodeId>,bool> iter = edges.begin();
          iter != edges.end(); ++iter )
      if ( !*iter ) {
        std::cerr << "check edges" << std::endl
        << graph << std::endl << T_mpd << std::endl;
      }
  }

// check le MPS of node
  {
    gumHashTable< gumNodeId,gumHashTable<gumNodeId,bool> > chk;

    for ( gumNodeListIterator<gumNodeId> iter = graph.beginNodes();
          iter != graph.endNodes(); ++iter )
      chk.insert( iter.getID(), gumHashTable<gumNodeId,bool>() );

    for ( gumNodeListIterator<gumNode> iter = T_mpd.beginNodes();
          iter != T_mpd.endNodes(); ++iter ) {
      const gumNodeList<gumNodeId>& clique = T_mpd.getClique( iter.getID() );

      for ( gumNodeListIterator<gumNodeId> iter2 = clique.begin();
            iter2 != clique.end(); ++iter2 )
        chk[iter2.getID()].insert( iter.getID(),false );
    }

    for ( gumHashTableIterator< gumNodeId,gumList<gumNodeId> > iter =  mps_of_node.begin();
          iter !=  mps_of_node.end(); ++iter ) {
      const gumList<gumNodeId>& liste = *iter;
      gumHashTable<gumNodeId,bool>& hash = chk[iter.key()];

      for ( gumListIterator<gumNodeId> iter2 = liste.begin();
            iter2 != liste.end(); ++iter2 ) {
        if ( !hash.exists( *iter2 ) )
          std::cerr << "check mps of nodes" << std::endl
          << T_mpd << std::endl << mps_of_node << std::endl;

        hash[*iter2] = true;
      }
    }

    for ( gumHashTableIterator< gumNodeId,gumHashTable<gumNodeId,bool> > iter=chk.begin();
          iter != chk.end(); ++iter ) {
      const gumHashTable<gumNodeId,bool>& hash = *iter;

      for ( gumHashTableIterator<gumNodeId,bool> iter2 = hash.begin();
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
template <typename gumNodeId, template <typename> class Triangulation>
void gumIncrementalTriangulation<gumNodeId,Triangulation>::updateTriangulation() {
  if ( !require_update ) return;

// the last ID generated for the junction tree and the max prime subgraph tree
// this enables to discriminate between "old" nodes and the new ones created
// by the triangulation
  gumNodeId max_old_id = idFactory.getMaxId();

// a temporary subgraph in which we actually perform the triangulation
  gumUndiGraphTpl<gumNodeId,gumEdge> tmp_graph;

// the set of all the cliques that should be affected by the different
// triangulations we will perform (one by connected component)
  gumHashTable<gumNodeId,bool> all_cliques_affected( __junction_tree.getNbrNodes() );

// for each triangulation, we will keep track of the cliques of the
// junction tree that are not affected by the triangulation but that are
// adjacent to cliques affected. This will enable us to connect easily the
// newly created cliques with the old ones.
  std::vector<gumEdge> notAffectedneighbourCliques;

// parse all the affected MPS and get the corresponding cliques
  for ( gumHashTableIterator<gumNodeId,bool> iter_mps = mps_affected.begin();
        iter_mps != mps_affected.end(); ++iter_mps ) {
    if ( *iter_mps ) {
// get the cliques contained in this MPS
      const std::vector<gumNodeId>& cliques = cliques_of_mps[iter_mps.key()];

      for ( unsigned int i = 0; i < cliques.size(); ++i )
        all_cliques_affected.insert( cliques[i],true );
    }
  }

// for each connected set of cliques involved in the triangulations
// perform a new triangulation and update the max prime subgraph tree
  for ( gumHashTableIterator<gumNodeId,bool> iter_clique = all_cliques_affected.begin();
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
      for ( gumNodeListIterator<gumNodeId> iter_node = tmp_graph.beginNodes();
            iter_node != tmp_graph.endNodes(); ++iter_node ) {
        gumList<gumNodeId>& mps = mps_of_node[iter_node->getID()];

        for ( gumHashTableIterator<gumNodeId,bool> iter_mps = mps_affected.begin();
              iter_mps != mps_affected.end(); ++iter_mps ) {
          if ( *iter_mps )
            mps.eraseByVal( iter_mps.key() );
        }
      }

// now tmp_graph contains the graph that should be triangulated.
// so triangulate it and get its junction tree
      triangulation.setGraph( tmp_graph,modalities );

      const gumCliqueGraphTpl<gumNode,gumEdge,gumNodeId>& tmp_junction_tree =
        triangulation.junctionTree();

// now, update the global junction tree
// first add the nodes of tmp_junction_tree to __junction_tree
      gumHashTable<gumNodeId,gumNodeId>
      tmp2global_junction_tree( tmp_junction_tree.getNbrNodes() );

      for ( gumNodeListIterator<gumNode> iter_jt = tmp_junction_tree.beginNodes();
            iter_jt != tmp_junction_tree.endNodes(); ++iter_jt ) {
        gumNodeId new_id = idFactory.getNewId();
        __junction_tree.insertNode( gumNode( new_id,"" ),
                                    tmp_junction_tree.getClique( iter_jt->getID() ),
                                    GUM_SHALLOW_COPY );
// translate the id of the temprary JT into an id of the global JT
        tmp2global_junction_tree.insert( iter_jt->getID(),new_id );
      }

// and add the edges of tmp_junction_tree to __junction_tree
      for ( gumEdgeListIterator<gumEdge> iter_edge = tmp_junction_tree.beginEdges();
            iter_edge != tmp_junction_tree.endEdges(); ++iter_edge )
        __junction_tree.insertEdge
        ( gumEdge( tmp2global_junction_tree[iter_edge->first()],
                   tmp2global_junction_tree[iter_edge->second()] ) );

// second get the edges in __junction_tree that have an extremal clique R in the
// affected clique set and the other one S not in the affected set and see
// which new node V in the __junction_tree should be connected to S. The running
// intersection property guarrantees that the clique in the tmp_junction_tree
// that results from the elimination (during the triangulation process) of
// the first eliminated node in the separator between R and S is an
// admissible candidate
      for ( unsigned int i = 0; i < notAffectedneighbourCliques.size(); ++i ) {
// now find the first eliminated node in the separator
        const gumNodeList<gumNodeId>& sep =
          __junction_tree.getSeparator( notAffectedneighbourCliques[i] );
        unsigned int __elim_order = tmp_graph.getNbrNodes() + 1;
        gumNodeId elim_node = 0;

        for ( gumNodeListIterator<gumNodeId> iter_sep = sep.begin();
              iter_sep != sep.end(); ++iter_sep ) {
          gumNodeId id = iter_sep->getID();
          unsigned int new_order = triangulation.eliminationOrder( id );

          if ( new_order < __elim_order ) {
            __elim_order = new_order;
            elim_node = id;
          }
        }

// find the __junction_tree clique corresponding to the elimination of
// elim_node and insert an edge between this clique and that which was not
// affected
        gumNodeId& to_connect =
          tmp2global_junction_tree[triangulation.createdClique( elim_node )];

        gumNodeId not_affected =
          all_cliques_affected.exists( notAffectedneighbourCliques[i].first() )?
          notAffectedneighbourCliques[i].second() :
          notAffectedneighbourCliques[i].first();

        __junction_tree.insertEdge( gumEdge( not_affected,to_connect ) );

        if ( to_connect <= max_old_id )
          T_mpd.insertEdge( gumEdge( mps_of_clique[to_connect],
                                     mps_of_clique[not_affected] ) );

// check that the separator created by the new edge is not equal to
// to_connect. If this is the case, then to_connect is included in
// not_affected and, hence, should be removed from the graph
        if ( __junction_tree.getSeparator( not_affected,to_connect ).size() ==
             __junction_tree.getClique( to_connect ).size() ) {
          __junction_tree.eraseEdge( not_affected,to_connect );

          for ( gumEdgeListIterator<gumEdge>
                iter_neighbour = __junction_tree.beginneighbourEdges( to_connect );
                iter_neighbour != __junction_tree.endEdges(); ++iter_neighbour ) {
            __junction_tree.insertEdge
            ( gumEdge( iter_neighbour->other( to_connect ), not_affected ) );

            if ( iter_neighbour->other( to_connect ) <= max_old_id )
              T_mpd.insertEdge
              ( gumEdge( mps_of_clique[iter_neighbour->other( to_connect )],
                         mps_of_clique[not_affected] ) );
          }

          __junction_tree.eraseNode( to_connect );

          to_connect = not_affected;
        }
      }
    }
  }

// remove the mps that were affected and update the cliques_of_mps table
  for ( gumHashTableIterator<gumNodeId,bool> iter_clique = all_cliques_affected.begin();
        iter_clique != all_cliques_affected.end(); ++iter_clique ) {
    mps_of_clique.erase( iter_clique.key() );
    __junction_tree.eraseNode( iter_clique.key() );
  }

  for ( gumHashTableIterator<gumNodeId,bool> iter_MPS = mps_affected.begin();
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
  gumHashTable<gumNodeId,gumNodeId>
  T_mpd_cliques( all_cliques_affected.size() );

  for ( gumNodeListIterator<gumNode> iter_clique = __junction_tree.beginNodes();
        iter_clique != __junction_tree.endNodes(); ++iter_clique )
    if ( iter_clique.getID() > max_old_id )
      T_mpd_cliques.insert( iter_clique.getID(),iter_clique.getID() );

// parse all the separators of the junction tree and test those that are not
// complete in the original graph
  std::vector< std::pair<gumNodeId,gumNodeId> > merged_cliques;

  gumHashTable<gumNodeId,bool> mark = T_mpd_cliques.map( false );

  for ( gumHashTableIterator<gumNodeId,bool> iter_clique = mark.begin();
        iter_clique != mark.end(); ++iter_clique )
    if ( !mark[iter_clique.key()] )
      __computeMaxPrimeMergings( iter_clique.key(), iter_clique.key(),
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
  for ( gumHashTableIterator<gumNodeId,gumNodeId> iter_clique = T_mpd_cliques.begin();
        iter_clique != T_mpd_cliques.end(); ++iter_clique ) {
    if ( iter_clique.key() == *iter_clique ) {
      T_mpd.insertNode( __junction_tree.getNode( *iter_clique ),
                        __junction_tree.getClique( *iter_clique ), GUM_SHALLOW_COPY );
      std::vector<gumNodeId>& vect_of_cliques =
        cliques_of_mps.insert( *iter_clique, std::vector<gumNodeId>() );
      vect_of_cliques.push_back( *iter_clique );
    }
  }

// add to the cliques previously created the nodes of the cliques that were
// merged into them and update the cliques_of_mps
  for ( gumHashTableIterator<gumNodeId,gumNodeId> iter = T_mpd_cliques.begin();
        iter != T_mpd_cliques.end(); ++iter ) {
    if (( iter.key() != *iter ) && ( *iter > max_old_id ) ) {
      const gumNodeList<gumNodeId>& new_clique = __junction_tree.getClique( iter.key() );

      for ( gumNodeListIterator<gumNode> iter_node = new_clique.begin();
            iter_node != new_clique.end(); ++iter_node ) {
        try { T_mpd.addToClique( *iter, iter_node.shallowGet(), GUM_SHALLOW_COPY ); }
        catch ( gumDuplicateElement& ) { }
      }

      cliques_of_mps[*iter].push_back( iter.key() );
    }
  }

// update the mps_of_node and the mps_of_clique
  for ( gumHashTableIterator<gumNodeId,gumNodeId> iter = T_mpd_cliques.begin();
        iter != T_mpd_cliques.end(); ++iter ) {
    mps_of_clique.insert( iter.key(), *iter );

    if ( iter.key() == *iter ) {
      const gumNodeList<gumNodeId>& nodes = T_mpd.getClique( *iter );

      for ( gumNodeListIterator<gumNodeId> iter_node = nodes.begin();
            iter_node != nodes.end(); ++iter_node )
        mps_of_node[iter_node.getID()].insert( *iter );
    }
  }

// add the edges to the max prime subgraph tree
  for ( gumHashTableIterator<gumNodeId,gumNodeId> iter = T_mpd_cliques.begin();
        iter != T_mpd_cliques.end(); ++iter ) {
    gumNodeId clique = *iter;

    for ( gumEdgeListIterator<gumEdge>
          iter_neighbour = __junction_tree.beginneighbourEdges( iter.key() );
          iter_neighbour != __junction_tree.endEdges(); ++iter_neighbour ) {
      gumNodeId othernode = iter_neighbour->other( iter.key() );

      if ( T_mpd_cliques.exists( othernode ) ) {
// here iter is linked to another node that has been created during
// the triangulation
        gumNodeId otherClique = T_mpd_cliques[othernode];
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
template <typename gumNodeId, template <typename> class Triangulation> INLINE
const gumCliqueGraphTpl<gumNode,gumEdge,gumNodeId>&
gumIncrementalTriangulation<gumNodeId,Triangulation>::junctionTree() {
  updateTriangulation();
  return __junction_tree;
}


// ================================================================================
/// returns the junction tree of the maximal prime subgraphs
// ================================================================================
template <typename gumNodeId, template <typename> class Triangulation> INLINE
const gumCliqueGraphTpl<gumNode,gumEdge,gumNodeId>&
gumIncrementalTriangulation<gumNodeId,Triangulation>::maxPrimeSubgraphTree() {
  updateTriangulation();
  return T_mpd;
}


// ================================================================================
/// sets the graph to the empty graph
// ================================================================================
template <typename gumNodeId, template <typename> class Triangulation>
void gumIncrementalTriangulation<gumNodeId,Triangulation>::clear() {
  graph.clearNodes();
  modalities.clear();
  __junction_tree.clearNodes();
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
template <typename gumNodeId, template <typename> class Triangulation>
template <typename Edge>
void gumIncrementalTriangulation<gumNodeId,Triangulation>::setGraph
( const gumUndiGraphTpl<gumNodeId,Edge>& theGraph,
  const gumHashTable<gumNodeId,unsigned int>& modal ) {
// remove the current graph
  clear();

// shallow copy the graph passed in argument and update the structures
// containing the informations useful for the triangulation

  for ( gumNodeListIterator<gumNodeId> iter_node = theGraph.beginNodes();
        iter_node != theGraph.endNodes(); ++iter_node )
    insertNode( iter_node.shallowGet(), modal[iter_node.getID()] );

  for ( gumEdgeListIterator<Edge> iter_edge = theGraph.beginEdges();
        iter_edge != theGraph.endEdges(); ++iter_edge )
    insertEdge( iter_edge->first(), iter_edge->second() );
}


// ================================================================================
/// returns the triangulation algorithm (useful for fine tuning it)
// ================================================================================
template <typename gumNodeId, template <typename> class Triangulation> INLINE
Triangulation<gumNodeId>&
gumIncrementalTriangulation<gumNodeId,Triangulation>::getTriangulationAlgo()
const  {
  return triangulation;
}


// ================================================================================
/// returns a clique containing a given node of the triangulated graph
// ================================================================================
template <typename gumNodeId, template <typename> class Triangulation> INLINE
gumNodeId gumIncrementalTriangulation<gumNodeId,Triangulation>::getContainerClique
( const gumNodeId& id ) const {
  const std::vector<gumNodeId>& contain_cliques = cliques_of_mps[mps_of_node[id][0]];

  for ( unsigned int i = 0; i < contain_cliques.size(); ++i )
    if ( __junction_tree.getClique( contain_cliques[i] ).exists( id ) )
      return contain_cliques[i];

  GUM_ERROR( gumNotFound, "no container clique" );
}


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
