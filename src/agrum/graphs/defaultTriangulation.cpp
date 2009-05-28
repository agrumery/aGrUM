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
 * @brief source implementations for computing default triangulations of graphs
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/graphs/defaultTriangulation.h>

#ifdef GUM_NO_INLINE
#include <agrum/graphs/defaultTriangulation.inl>
#endif //GUM_NO_INLINE


#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {


  /* ============================================================================ */
  /* ============================================================================ */
  /* ===             DEFAULT TRIANGULATION ALGORITHM USED BY AGRUM            === */
  /* ============================================================================ */
  /* ============================================================================ */

  // ==============================================================================
  /// basic constructor. initialize the triangulation
  // ==============================================================================
  DefaultTriangulation::DefaultTriangulation
  ( const UndiGraph& theGraph,
    const Property<unsigned int>::onNodes& modal,
    bool minimality, double theRatio, double theThreshold ) :
    __log_modalities( theGraph.size() ), __elim_order( theGraph.size() ),
    __reverse_elim_order( theGraph.size() ), __elim_cliques( theGraph.size() ),
    __node_2_junction_clique( theGraph.size() ),
    __node_2_max_prime_clique( theGraph.size() ), __has_triangulation( false ),
    __has_triangulated_graph( false ), __has_elimination_tree( false ),
    __has_junction_tree( false ), __has_max_prime_junction_tree( false ),
    __quasi_ratio( theRatio ), __threshold( theThreshold ),
    __minimality_required( minimality ), __added_fill_ins( theGraph.size() ),
    __we_want_fill_ins( false ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( DefaultTriangulation );
    // copy the original graph and set the __log_modalities to the log of the
    // modalities of the nodes
    __original_graph.populateNodes( theGraph );

    for ( NodeSetIterator iter = theGraph.nodes().begin();
          iter != theGraph.nodes().end(); ++iter )
      __log_modalities.insert( *iter, log( modal[*iter] ) );

    // copy the edges
    for ( EdgeSetIterator iter = theGraph.edges().begin();
          iter != theGraph.edges().end(); ++iter )
      __original_graph.insertEdge( *iter );
  }

  // ==============================================================================
  /// basic constructor. initialize the triangulation
  // ==============================================================================
  DefaultTriangulation::DefaultTriangulation
  ( const DiGraph& theGraph,
    const Property<unsigned int>::onNodes& modal,
    bool minimality, double theRatio, double theThreshold ) :
    Triangulation(),
    __log_modalities( theGraph.size() ), __elim_order( theGraph.size() ),
    __reverse_elim_order( theGraph.size() ), __elim_cliques( theGraph.size() ),
    __node_2_junction_clique( theGraph.size() ),
    __node_2_max_prime_clique( theGraph.size() ), __has_triangulation( false ),
    __has_triangulated_graph( false ), __has_elimination_tree( false ),
    __has_junction_tree( false ), __has_max_prime_junction_tree( false ),
    __quasi_ratio( theRatio ), __threshold( theThreshold ),
    __minimality_required( minimality ), __added_fill_ins( theGraph.size() ),
    __we_want_fill_ins( false ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( DefaultTriangulation );
    // copy the original graph and set the __log_modalities to the log of the
    // modalities of the nodes
    __original_graph.populateNodes( theGraph );

    for ( NodeSetIterator iter = theGraph.nodes().begin();
          iter != theGraph.nodes().end(); ++iter )
      __log_modalities.insert( *iter, log( modal[*iter] ) );

    // copy the arcs
    for ( ArcSetIterator iter = theGraph.arcs().begin();
          iter != theGraph.arcs().end(); ++iter )
      __original_graph.insertEdge( Edge( iter->first(),iter->second() ) );
  }

  // ==============================================================================
  /// basic constructor. initialize the triangulation
  // ==============================================================================
  DefaultTriangulation::DefaultTriangulation
  ( const MixedGraph& theGraph,
    const Property<unsigned int>::onNodes& modal,
    bool minimality, double theRatio, double theThreshold ) :
    __log_modalities( theGraph.size() ), __elim_order( theGraph.size() ),
    __reverse_elim_order( theGraph.size() ), __elim_cliques( theGraph.size() ),
    __node_2_junction_clique( theGraph.size() ),
    __node_2_max_prime_clique( theGraph.size() ), __has_triangulation( false ),
    __has_triangulated_graph( false ), __has_elimination_tree( false ),
    __has_junction_tree( false ), __has_max_prime_junction_tree( false ),
    __quasi_ratio( theRatio ), __threshold( theThreshold ),
    __minimality_required( minimality ), __added_fill_ins( theGraph.size() ),
    __we_want_fill_ins( false ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( DefaultTriangulation );
    // copy the original graph and set the __log_modalities to the log of the
    // modalities of the nodes
    __original_graph.populateNodes( theGraph );

    for ( NodeSetIterator iter = theGraph.nodes().begin();
          iter != theGraph.nodes().end(); ++iter )
      __log_modalities.insert( *iter, log( modal[*iter] ) );

    // copy the edges
    for ( ArcSetIterator iter = theGraph.arcs().begin();
          iter != theGraph.arcs().end(); ++iter )
      __original_graph.insertEdge( Edge( iter->first(),iter->second() ) );

    // copy the edges
    for ( EdgeSetIterator iter = theGraph.edges().begin();
          iter != theGraph.edges().end(); ++iter )
      __original_graph.insertEdge( *iter );
  }

  // ==============================================================================
  /// default constructor: initialize the triangulation for an empty graph
  // ==============================================================================
  DefaultTriangulation::DefaultTriangulation( bool minimality,
                                              double theRatio,
                                              double theThreshold ) :
    Triangulation(),
    __has_triangulation( true ), __has_triangulated_graph( true ),
    __has_elimination_tree( true ), __has_junction_tree( true ),
    __has_max_prime_junction_tree( true ),
    __quasi_ratio( theRatio ), __threshold( theThreshold ),
    __minimality_required( minimality ),
    __we_want_fill_ins( false ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( DefaultTriangulation );
  }

  // ==============================================================================
  /// copy constructor
  // ==============================================================================
  DefaultTriangulation::DefaultTriangulation( const DefaultTriangulation& from ) :
    Triangulation(),
    __original_graph( from.__original_graph ),
    __triangulated_graph( from.__triangulated_graph ),
    __log_modalities( from.__log_modalities ), __fill_ins( from.__fill_ins ),
    __elim_order( from.__elim_order ),
    __reverse_elim_order( from.__reverse_elim_order ),
    __elim_cliques( from.__elim_cliques ), __elim_tree( from.__elim_tree ),
    __junction_tree( from.__junction_tree ),
    __node_2_junction_clique( from.__node_2_junction_clique ),
    __max_prime_junction_tree( from.__max_prime_junction_tree ),
    __node_2_max_prime_clique( from.__node_2_max_prime_clique ),
    __has_triangulation( from.__has_triangulation ),
    __has_triangulated_graph( from.__has_triangulated_graph ),
    __has_elimination_tree( from.__has_elimination_tree ),
    __has_junction_tree( from.__has_junction_tree ),
    __has_max_prime_junction_tree( from.__has_max_prime_junction_tree ),
    __quasi_ratio( from.__quasi_ratio ), __threshold( from.__threshold ),
    __minimality_required( from.__minimality_required ),
    __added_fill_ins( from.__added_fill_ins ),
    __we_want_fill_ins( from.__we_want_fill_ins ) {
    // for debugging purposes
    GUM_CONS_CPY( DefaultTriangulation );
  }

  // ==============================================================================
  /// destructor
  // ==============================================================================
  DefaultTriangulation::~DefaultTriangulation() {
    // for debugging purposes
    GUM_DESTRUCTOR( DefaultTriangulation );
  }

  
  // ==============================================================================
  /// virtual copy constructor
  // ==============================================================================
  DefaultTriangulation* DefaultTriangulation::copyFactory () const {
    return new DefaultTriangulation (*this);
  }
  

  // ==============================================================================
  /// reinitialize the graph to be triangulated to an empty graph
  // ==============================================================================
  void DefaultTriangulation::clear() {
    // remove the current graphs
    __original_graph.clear();
    __triangulated_graph.clear();
    __elim_tree.clear();
    __junction_tree.clear();
    __max_prime_junction_tree.clear();
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

  // ==============================================================================
  /// initialize the triangulation algorithm for a new graph
  // ==============================================================================
  void DefaultTriangulation::setGraph
  ( const UndiGraph& gr,const Property<unsigned int>::onNodes& modal ) {
    // remove the old graph
    clear();
    __log_modalities.resize( gr.size() );
    __elim_order.resize( gr.size() );
    __reverse_elim_order.resize( gr.size() );
    __elim_cliques.resize( gr.size() );
    __added_fill_ins.resize( gr.size() );
    __node_2_junction_clique.resize( gr.size() );
    __node_2_max_prime_clique.resize( gr.size() );
    // copy the graph passed in argument and set the __log_modalities to
    // the log of the modalities of the nodes
    __original_graph.populateNodes( gr );

    for ( NodeSetIterator iter=gr.nodes().begin(); iter!=gr.nodes().end(); ++iter ) {
      __log_modalities.insert( *iter, log( modal[*iter] ) );
    }

    // copy the edges
    for ( EdgeSetIterator iter=gr.beginEdges(); iter!=gr.endEdges(); ++iter )
      __original_graph.insertEdge( Edge( iter->first(),iter->second() ) );

    // indicate that no triangulation was performed for this graph
    __has_triangulation = false;

    __has_triangulated_graph = false;

    __has_elimination_tree = false;

    __has_junction_tree = false;

    __has_max_prime_junction_tree = false;
  }

  // ==============================================================================
  /// the function that performs the triangulation
  // ==============================================================================
  void DefaultTriangulation::__triangulate() {
    // if the graph is already triangulated, no need to triangulate it again
    if ( __has_triangulation ) return;

    // copy the graph to be triangulated, so that we can modify it
    UndiGraph tmp_graph( __original_graph );

    // create a container to store for each node the weight of the clique
    // it would form with its neighbours (it will be properly filled by the
    // Simplicial structure
    Property<float>::onNodes log_weights( tmp_graph.size() );

    // create a simplicial instance that will find the simplicial,
    // almost simplicial and quasi simplicial nodes
    SimplicialSet simplicial( tmp_graph, __log_modalities, log_weights,
                              __quasi_ratio, __threshold );

    simplicial.setFillIns( __we_want_fill_ins );

    // if we are to do recursive thinning, we will have to add fill-ins to the
    // triangulated graph each time we eliminate a node. Hence, we shall initialize
    // the triangulated graph by a copy of the original graph
    if ( __minimality_required ) {
      __triangulated_graph= __original_graph ;
      __has_triangulated_graph = true;
    }

    // perform the triangulation
    NodeId removable_node = 0;

    std::pair<NodeId,NodeId> thePair;

    for ( unsigned int nb_elim = 0; tmp_graph.size() != 0; ++nb_elim ) {
      // select a node to be eliminated
      if ( simplicial.hasSimplicialNode() )
        removable_node = simplicial.bestSimplicialNode();
      else if ( simplicial.hasAlmostSimplicialNode() )
        removable_node = simplicial.bestAlmostSimplicialNode();
      else if ( simplicial.hasQuasiSimplicialNode() )
        removable_node = simplicial.bestQuasiSimplicialNode();
      else {
        // removable_node = heuristic.getNode ();
        Property< float >::onNodes::iterator iter_heuristic = log_weights.begin();
        float min_weight = *iter_heuristic;
        removable_node = iter_heuristic.key();

        for ( ++iter_heuristic; iter_heuristic != log_weights.end();
              ++iter_heuristic )
          if ( *iter_heuristic < min_weight ) {
            removable_node = iter_heuristic.key();
            min_weight = *iter_heuristic;
          }
      }

      // std::cerr << "eliminating " << removable_node << std::endl;
      // when minimality is not required, i.e., we won't apply recursive thinning,
      // the cliques sets can be computed
      if ( !__minimality_required ) {
        const EdgeSet& nei=tmp_graph.neighbours( removable_node );
        NodeSet& cliques = __elim_cliques.insert( removable_node, NodeSet() );
        cliques.resize( nei.size() + 1 );
        cliques<<removable_node;
        unsigned int i = 1;

        for ( EdgeSetIterator iter_edges =nei.begin();
              iter_edges != nei.end(); ++iter_edges, ++i ) {
          cliques<< iter_edges->other( removable_node );
        }
      }
      else {
        // here recursive thinning will be applied, hence we need store the fill-ins
        // added by the current node removal
        EdgeSet& current_fill = __added_fill_ins[nb_elim];
        EdgeSetIterator iter_edges2;
        NodeId node1, node2;
        const EdgeSet& nei=tmp_graph.neighbours( removable_node );

        for ( EdgeSetIterator iter_edges1 = nei.begin();
              iter_edges1 != nei.end(); ++iter_edges1 ) {
          node1 = iter_edges1->other( removable_node );
          iter_edges2 = iter_edges1;

          for ( ++iter_edges2; iter_edges2 != nei.end(); ++iter_edges2 ) {
            node2 = iter_edges2->other( removable_node );

            if ( !tmp_graph.existsEdge( node1,node2 ) ) {
              current_fill.insert( Edge( node1,node2 ) );
              __triangulated_graph.insertEdge( Edge( node1,node2 ) );
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
      __fill_ins_list_standard = simplicial.fillIns();
    }

    // if minimality is required, remove the redondant edges
    if ( __minimality_required )
      __computeRecursiveThinning();

    __has_triangulation = true;
  }

  // ==============================================================================
  /// removes redondant fill-ins and compute proper elimination cliques and order
  // ==============================================================================
  void DefaultTriangulation::__computeRecursiveThinning() {
    // apply recursive thinning (fmint, see Kjaerulff (90), "triangulation of
    // graphs - algorithms giving small total state space")
    NodeId node1, node2;
    bool incomplete;
    std::vector<NodeId> adj;
    EdgeSet T_prime;
    Property<unsigned int>::onNodes R( __triangulated_graph.size() );

    for ( NodeSetIterator iter = __triangulated_graph.nodes().begin();
          iter != __triangulated_graph.nodes().end(); ++iter )
      R.insert( *iter,0 );

    // the FMINT loop
    for ( unsigned int i = __added_fill_ins.size()-1;
          i < __added_fill_ins.size(); --i ) {
      if ( __added_fill_ins[i].size() ) {
        // here apply MINT to T_i = __added_fill_ins[i]
        EdgeSet& T = __added_fill_ins[i];
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
          for ( EdgeSetIterator iter_edge=T.begin();iter_edge != T.end();
                ++iter_edge ) {
            node1 = iter_edge->first();
            node2 = iter_edge->second();
            // check if at least one extremal node belongs to R

            if (( R[node1] < j ) && ( R[node2] < j ) )  continue;

            // check if the intersection of adj(v,G) and adj(w,G) is complete
            if ( __triangulated_graph.neighbours( node2 ).size() <
                 __triangulated_graph.neighbours( node1 ).size() ) {
              NodeId tmp = node1;
              node1 = node2;
              node2 = tmp;
            }

            incomplete = false;

            // find the nodes belonging to the intersection of adj(v,G) and adj(w,G)
            const EdgeSet& nei=__triangulated_graph.neighbours( node1 );

            for ( EdgeSetIterator iter_adj =nei.begin();
                  iter_adj != nei.end(); ++iter_adj ) {
              if ( __triangulated_graph.existsEdge( node2,iter_adj->other(node1)))
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
              T_prime.insert( *iter_edge );
              R[node1] = j+1;
              R[node2] = j+1;
            }
          }

          // compute the new value of T (i.e. T\T') and update the triangulated graph
          for ( EdgeSetIterator iter_delete = T_prime.begin();
                iter_delete != T_prime.end(); ++iter_delete ) {
            T.erase( *iter_delete );
            __triangulated_graph.eraseEdge( *iter_delete );
          }

          if ( T_prime.size() == 0 ) require_mint = false;

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
    PriorityQueue< NodeId,unsigned int,std::greater<unsigned int> >
      numbered_neighbours( std::greater<unsigned int>(), false,
                           __triangulated_graph.size() );

    for ( unsigned int i = 0; i< __elim_order.size(); ++i )
      numbered_neighbours.insert( 0, __elim_order[i] );

    // perform the maximum cardinality search
    for ( unsigned int i = __elim_order.size() - 1; i < __elim_order.size(); --i ) {
      NodeId node = numbered_neighbours.pop();
      __elim_order[i] = node;
      __reverse_elim_order[node] = i;
      const EdgeSet& nei=__triangulated_graph.neighbours( node );

      for ( EdgeSetIterator iter = nei.begin();iter != nei.end(); ++iter ) {
        NodeId neighbour = iter->other( node );

        try {
          numbered_neighbours.setPriorityByVal
            ( neighbour, 1 + numbered_neighbours.priorityByVal( neighbour ) );
        } catch ( NotFound& ) { }
      }
    }

    // here the elimination order is ok. We now need to update the __elim_cliques
    for ( unsigned int i = 0; i < __elim_order.size(); ++i ) {
      NodeSet& cliques = __elim_cliques.insert( __elim_order[i], NodeSet() );
      cliques<< __elim_order[i] ;
      const EdgeSet& nei=__triangulated_graph.neighbours( __elim_order[i] );

      for ( EdgeSetIterator iter = nei.begin();iter != nei.end(); ++iter ) {
        NodeId neighbour = iter->other( __elim_order[i] );

        if ( __reverse_elim_order[neighbour] > i ) {
          cliques<<neighbour ;
        }
      }
    }
  }

  // ==============================================================================
  /// returns an elimination tree from a triangulated graph
  // ==============================================================================
  void DefaultTriangulation::__computeEliminationTree() {
    // if there already exists an elimination tree, no need to compute it again
    if ( __has_elimination_tree ) return;

    // if the graph is not triangulated, triangulate it
    if ( !__has_triangulation )  __triangulate();

    /*
    // create the nodes of the elimination tree
    NodeSet clique;
    std::ostringstream ost;
    for ( unsigned int i = 0; i < __elim_order.size(); ++i ) {
    // form the elimination clique relative to the node
    clique.clear();
    NodeSet& list_of_nodes = __elim_cliques[__elim_order[i]];
    for ( unsigned int j = 0; j < list_of_nodes.size(); ++j )
    clique<< list_of_nodes[j] ;
    // insert the node
    ost.str( "" );
    ost << "clique " << __original_graph.getNode( __elim_order[i] ).label();
    __elim_tree.addNode( Node( __elim_order[i], ost.str() ),
    clique, GUM_SHALLOW_COPY );
    }*/
    __elim_tree.clear();

    for ( unsigned int i = 0; i < __elim_order.size(); ++i )
      __elim_tree.insertNode( __elim_cliques[__elim_order[i]] ); // REMEMBER :
    // i+1 is the idNode of the clique in __elim_tree for the
    // node __elim_order[i] !!!!!

    // create the edges of the elimination tree: join a node to the one in its clique
    // that is eliminated first
    for ( unsigned int i = 0; i < __elim_order.size(); ++i ) {
      const NodeSet& list_of_nodes = __elim_cliques[__elim_order[i]];
      unsigned int child = __original_graph.size() + 1;

      for ( NodeSetIterator it=list_of_nodes.begin();it!=list_of_nodes.end();++it ) {
        //   for ( unsigned int j = 0; j < list_of_nodes.size(); ++j )
        unsigned int lon_j=*it; // node number>0

        if (( lon_j != __elim_order[i] ) &&
            ( child > __reverse_elim_order[lon_j] ) )
          child = __reverse_elim_order[lon_j];
      }

      if ( child < __original_graph.size() ) {
        __elim_tree.insertEdge( Edge( i+1, child+1 ) );// node number>0
      }
    }

    __has_elimination_tree = true;
  }

  // ==============================================================================
  /// computes a junction tree from an elimination tree
  // ==============================================================================
  void DefaultTriangulation::__computeJunctionTree() {
    // if there already exists a junction tree, no need to recompute it
    if ( __has_junction_tree ) return;

    // if there is no elimination tree, create it
    if ( ! __has_elimination_tree )  __computeEliminationTree();

    // copy the elimination tree into the junction tree
    __junction_tree= __elim_tree ;

    //mark all the edges of the junction tree to false
    Property< bool >::onEdges mark = __junction_tree.edgesProperty( false );

    // create a vector indicating by which clique a given clique has been
    // substituted during the transformation from the elimination tree to the
    // junction tree. Assume that clique J the elmination tree has been substituted
    // by clique K of the elimination tree, and that clique J (resp. K) was the
    // jth (resp. kth) one created during the triangulation process. Then, in the
    // vector below, substitution[j] = k.
    // substition[0] IS NOT USED !!!
    std::vector<unsigned int> substitution( __elim_order.size() +1 );

    for ( unsigned int i = 0; i < substitution.size(); ++i )
      substitution[i] = i;

    // for all cliques C_i, from the last one created to the first one, check if
    // there exists a parent C_j (a neighbour created before C_i) such that
    // |C_j| = |C_i| and such that the edge is not marked. In this case, this
    // means that C_j contains C_i. Hence, we should remove C_i, and link all of
    // its neighbours to C_j. These links will be marked to true as no such
    // neighbour can be included in C_j (and conversely).
    EdgeSetIterator iter;

    for ( NodeId C_i=__elim_order.size(); C_i >0; --C_i ) {
      unsigned int card_C_i = __junction_tree.clique( C_i ).size();
      // search for C_j such that |C_j| = [C_i| + 1
      NodeId C_j = C_i;
      const EdgeSet nei=__junction_tree.neighbours( C_i );

      for ( iter = nei.begin();iter != nei.end(); ++iter ) {
        NodeId C_jj = iter->other( C_i );

        if (( __reverse_elim_order[__elim_order[C_i-1]] >
              __reverse_elim_order[__elim_order[C_jj-1]] ) &&
            ( !mark[Edge( C_i,C_jj )] ) &&
            ( __junction_tree.clique( C_jj ).size() == card_C_i + 1 ) ) {
          // ok, here we found a parent such that |C_jj| = [C_i| + 1
          C_j = C_jj;
          __junction_tree.eraseEdge( C_i,C_j );
          break;
        }
      }

      // if we found a C_j, link the neighbours of C_i to C_j
      if ( C_j != C_i ) {
        const EdgeSet nei=__junction_tree.neighbours( C_i );

        for ( iter = nei.begin();iter != nei.end(); ++iter ) {
          Edge new_edge( C_j,iter->other( C_i ) );
          __junction_tree.insertEdge( new_edge );
          mark.insert( new_edge, true );
        }

        substitution[C_i] = C_j; //__reverse_elim_order[__elim_order[C_i-1]]+1;

        __junction_tree.eraseNode( C_i );
      }
    }

    // compute the transitive closure of vector substitution
    for ( unsigned int i = 0; i < substitution.size(); ++i )
      substitution[i] = substitution[substitution[i]];

    // using the transitive closure of vector substitution, compute for each node
    // the clique of the junction tree that was created by its elimination during
    // the triangulation
    for ( unsigned int i = 1; i < substitution.size(); ++i ) {
      __node_2_junction_clique.insert( __elim_order[i-1], substitution[i] );
    }

    __has_junction_tree = true;
  }

  // ==============================================================================
  /// used for computing the junction tree of the maximal prime subgraphs
  // ==============================================================================
  void DefaultTriangulation::__computeMaxPrimeMergings
  ( const NodeId node, const NodeId from,
    std::vector< Edge >& merged_cliques,
    NodeSet& mark ) const {
    mark<<node;
    NodeSetIterator iter_sep2;
    const EdgeSet& nei=__junction_tree.neighbours( node );

    for ( EdgeSetIterator iter_sep=nei.begin();iter_sep != nei.end(); ++iter_sep ) {
      NodeId other_node = iter_sep->other( node );

      if ( other_node != from ) {
        const NodeSet& separator = __junction_tree.separator( node, other_node );
        // check that the separator between node and other_node is complete
        bool complete = true;

        for ( NodeSetIterator iter_sep1 = separator.begin();
              iter_sep1 != separator.end() && complete; ++iter_sep1 ) {
          iter_sep2 = iter_sep1;

          for ( ++iter_sep2; iter_sep2 != separator.end(); ++iter_sep2 ) {
            if ( !__original_graph.existsEdge( *iter_sep1,*iter_sep2 ) ) {
              complete = false;
              break;
            }
          }
        }

        // here complete indicates whether the separator is complete or not
        if ( !complete )
          merged_cliques.push_back( Edge( other_node,node ) );

        __computeMaxPrimeMergings( other_node, node, merged_cliques, mark );
      }
    }
  }

  // ==============================================================================
  /// computes the junction tree of the maximal prime subgraphs
  // ==============================================================================
  void DefaultTriangulation::__computeMaxPrimeJunctionTree() {
    // if there already exists a max prime junction tree, no need to recompute it
    if ( __has_max_prime_junction_tree ) return;

    // if there is no junction tree, create it
    if ( ! __has_junction_tree ) __computeJunctionTree();

    // the maximal prime subgraph join tree is created by aggregation of some
    // cliques. More precisely, when the separator between 2 cliques is not complete
    // in the original graph, then the two cliques must be merged. Create a hashtable
    // indicating which clique has been absorbed by some other clique.
    Property<NodeId>::onNodes T_mpd_cliques( __junction_tree.size() );

    for ( NodeSetIterator iter_clique = __junction_tree.nodes().begin();
          iter_clique != __junction_tree.nodes().end(); ++iter_clique )
      T_mpd_cliques.insert( *iter_clique,*iter_clique );

    __max_prime_junction_tree.populateNodesFromProperty<NodeId>(T_mpd_cliques );

    // parse all the separators of the junction tree and test those that are not
    // complete in the orginal graph
    std::vector<Edge> merged_cliques;

    NodeSet mark;

    for ( NodeSetIterator iter_clique = __junction_tree.nodes().begin();
          iter_clique != __junction_tree.nodes().end(); ++iter_clique )
      if ( ! mark.contains( *iter_clique ) )
        __computeMaxPrimeMergings( *iter_clique, *iter_clique,merged_cliques, mark );

    // compute the transitive closure of merged_cliques. This one will contain
    // pairs (X,Y) indicating that clique X must be merged with clique Y.
    // Actually clique X will be inserted into clique Y.
    for ( unsigned int i = 0; i < merged_cliques.size(); ++i ) {
      T_mpd_cliques[merged_cliques[i].first()] =
        T_mpd_cliques[merged_cliques[i].second()];
    }

    // now we can create the max prime junction tree. First, create the cliques
    for ( Property< NodeId >::onNodes::iterator iter_clique = T_mpd_cliques.begin();
          iter_clique != T_mpd_cliques.end(); ++iter_clique ) {
      if ( iter_clique.key() == *iter_clique ) {
        __max_prime_junction_tree.setClique
          ( *iter_clique,__junction_tree.clique( *iter_clique ) );
      }
    }

    // add to the cliques previously created the nodes of the cliques that were
    // merged into them
    for ( Property< NodeId >::onNodes::iterator  iter = T_mpd_cliques.begin();
          iter != T_mpd_cliques.end(); ++iter ) {
      if ( iter.key() != *iter ) {
        const NodeSet& new_clique = __junction_tree.clique( iter.key() );

        for ( NodeSetIterator iter_node = new_clique.begin();
              iter_node != new_clique.end(); ++iter_node ) {
          try {
            __max_prime_junction_tree.addToClique( *iter, *iter_node );
          } catch ( DuplicateElement& ) { }
        }
      }
    }

    // add the edges to the graph
    for ( EdgeSetIterator iter_edge = __junction_tree.beginEdges();
          iter_edge != __junction_tree.endEdges(); ++iter_edge ) {
      NodeId node1 = T_mpd_cliques[iter_edge->first()];
      NodeId node2 = T_mpd_cliques[iter_edge->second()];

      if ( node1 != node2 ) {
        try {
          __max_prime_junction_tree.insertEdge( *iter_edge );
        } catch ( DuplicateElement& ) { }
      }
    }

    // compute for each node which clique of the max prime junction tree was created
    // by the elmination of the node
    for ( Property< NodeId >::onNodes::iterator iter_junction =
            __node_2_junction_clique.begin();
          iter_junction != __node_2_junction_clique.end(); ++iter_junction )
      __node_2_max_prime_clique.insert( iter_junction.key(),
                                        T_mpd_cliques[*iter_junction] );

    __has_max_prime_junction_tree = true;
  }

  // ==============================================================================
  /// returns the triangulated graph
  // ==============================================================================
  const UndiGraph&
  DefaultTriangulation::triangulatedGraph() {
    if ( !__has_triangulation )  __triangulate();

    // if we did not construct the triangulated graph during the triangulation, that
    // is, we just constructed the junction tree, we shall construct it now
    if ( ! __has_triangulated_graph ) {
      // just in case, be sure that the junction tree has been constructed
      if ( ! __has_junction_tree )  __computeJunctionTree();

      // copy the original graph
      __triangulated_graph= __original_graph;

      // parse all the cliques of the junction tree
      NodeSetIterator iter_clique2;

      for ( NodeSetIterator iter_node = __junction_tree.nodes().begin();
            iter_node != __junction_tree.nodes().end(); ++iter_node ) {
        // for each clique, add the edges necessary to make it complete
        const NodeSet& clique = __junction_tree.clique( *iter_node );
        std::vector<NodeId> clique_nodes( clique.size() );
        unsigned int i =0;

        for ( NodeSetIterator iter_clique = clique.begin();
              iter_clique != clique.end(); ++iter_clique, ++i )
          clique_nodes[i] = *iter_clique;

        NodeSetIterator it2;

        for ( NodeSetIterator it1=clique.begin();it2!=clique.end();++it1 ) {
          for ( it2=++it1;it2!=clique.end();++it2 ) {
            //for ( i = 0; i < clique_nodes.size(); ++i ) {
            //  for ( unsigned int j = i+1; j < clique_nodes.size(); ++j ) {
            try { __triangulated_graph.insertEdge( Edge( *it1,*it2 ) ); }
            catch ( DuplicateElement& ) { }
          }
        }
      }

      __has_triangulated_graph = true;
    }

    return __triangulated_graph;
  }


} /* namespace gum */

  
#endif /* DOXYGEN_SHOULD_SKIP_THIS */
