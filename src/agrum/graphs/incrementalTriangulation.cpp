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


#include <agrum/graphs/incrementalTriangulation.h>


#ifdef GUM_NO_INLINE
#include <agrum/graphs/incrementalTriangulation.inl>
#endif //GUM_NO_INLINE


#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {


  /* ============================================================================ */
  /* ============================================================================ */
  /* ===            CLASS FOR COMPUTING INCREMENTAL TRIANGULATIONS            === */
  /* ============================================================================ */
  /* ============================================================================ */

  // ==============================================================================
  /// constructor
  // ==============================================================================
  IncrementalTriangulation::IncrementalTriangulation
  ( const Triangulation& triang_algo,
    const UndiGraph& theGraph,
    const Property<NodeId>::onNodes& modal ) :
    __triangulation (triang_algo.copyFactory ()), __require_update( false ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( IncrementalTriangulation );

    // reset the triangulation algorithm => it starts with an empty graph
    __triangulation->clear ();

    // shallow copy the graph passed in argument and update the structures
    // containing the informations useful for the triangulation
    for ( NodeSetIterator iter_node = theGraph.beginNodes();
          iter_node != theGraph.endNodes(); ++iter_node )
      insertNode( *iter_node, modal[*iter_node] );

    // insert all the edges of the graph into the structure. This will
    // implicitly update the "require_update" field
    for ( EdgeSetIterator iter_edge = theGraph.beginEdges();
          iter_edge != theGraph.endEdges(); ++iter_edge )
      insertEdge( iter_edge->first(), iter_edge->second() );
  }


  // ==============================================================================
  /// default constructor
  // ==============================================================================
  IncrementalTriangulation::IncrementalTriangulation
  ( const Triangulation& triang_algo ) :
    __triangulation(triang_algo.copyFactory ()), __require_update( false ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( IncrementalTriangulation );

    // reset the triangulation algorithm => it starts with an empty graph
    __triangulation->clear ();
  }


  // ==============================================================================
  /// copy operator
  // ==============================================================================
  IncrementalTriangulation::IncrementalTriangulation
  ( const IncrementalTriangulation& from ) :
    __graph ( from.__graph ), __modalities( from.__modalities ),
    __junction_tree( from.__junction_tree ),
    __T_mpd( from.__T_mpd ), __mps_of_node( from.__mps_of_node ),
    __cliques_of_mps( from.__cliques_of_mps ),
    __mps_of_clique( from.__mps_of_clique ),
    __mps_affected( from.__mps_affected ),
    __triangulation( from.__triangulation->copyFactory () ),
    __require_update( from.__require_update ) {
    // for debugging purposes
    GUM_CONS_CPY( IncrementalTriangulation );
  }


  // ==============================================================================
  /// destructor
  // ==============================================================================
  IncrementalTriangulation::~IncrementalTriangulation () {
    // for debugging purposes
    GUM_DESTRUCTOR( IncrementalTriangulation );

    // remove things that were allocated within the current class
    delete __triangulation;
  }


  // ==============================================================================
  /// copy operator
  // ==============================================================================
  IncrementalTriangulation& IncrementalTriangulation::operator=
  ( const IncrementalTriangulation& from ) {
    // avoid self assignment
    if ( this != &from ) {
      // for debugging purposes
      GUM_OP_CPY( IncrementalTriangulation );

      // copy all the structures stored in "from"
      __graph = from.__graph ;
      __modalities = from.__modalities;
      __junction_tree = from.__junction_tree;
      __T_mpd = from.__T_mpd;
      __mps_of_node = from.__mps_of_node;
      __cliques_of_mps = from.__cliques_of_mps;
      __mps_of_clique = from.__mps_of_clique;
      __mps_affected = from.__mps_affected;
      __require_update = from.__require_update;

      // just in case we change the triangulation algorithm, we remove it
      // and create it again
      delete __triangulation;
      __triangulation = from.__triangulation->copyFactory ();
    }

    return *this;
  }


  // ==============================================================================
  /// adds a new node to the graph (and update the internal structures)
  // ==============================================================================
  void IncrementalTriangulation::insertNode ( NodeId node, unsigned int modal ) {
    // add the new node to the graph
    __graph.insertNode( node );
    __modalities.insert( node, modal );

    // add a new clique to T_mpd and the junction tree
    NodeSet clique_nodes( 2 );
    clique_nodes.insert( node );
    NodeId MPS = __T_mpd.insertNode( clique_nodes );
    NodeId new_clique = __junction_tree.insertNode( clique_nodes );

    // indicate in which MPS node belongs
    List<NodeId>& list_of_mps = __mps_of_node.insert( node, List<NodeId>() );
    list_of_mps.insert( MPS );

    // indicate in which MPS the clique added to the junction tree belongs
    std::vector<NodeId>& cliquesMPS =
      __cliques_of_mps.insert( MPS, std::vector<NodeId>() );
    cliquesMPS.push_back( new_clique );
    __mps_of_clique.insert( new_clique, MPS );

    // indicate that the new MPS should not be affected by a triangulation
    __mps_affected.insert( MPS, false );
  }


  // ==============================================================================
  /// mark the mps affected by the deletion of a given edge
  // ==============================================================================
  void IncrementalTriangulation::__markAffectedMPSsByRemoveLink
  ( const NodeId& My,
    const NodeId& Mz,
    const std::pair<NodeId,NodeId>& edge ) {
    // mark the MPS so that it will be retriangulated
    __mps_affected[My] = true;

    // mark all the neighbour MPS that contain edge
    for ( EdgeSetIterator iter = __T_mpd.neighbours( My ).begin ();
          iter != __T_mpd.endEdges(); ++iter ) {
      if ( iter->other( My ) != Mz ) {
        const NodeSet& Syk = __T_mpd.separator( *iter );

        if ( Syk.contains( edge.first ) && Syk.contains( edge.second ) )
          __markAffectedMPSsByRemoveLink( iter->other( My ), My, edge );
      }
    }
  }


  // ==============================================================================
  /// removes an edge from the graph (the join tree may need a retriangulation)
  // ==============================================================================
  void IncrementalTriangulation::eraseEdge ( NodeId X, NodeId Y ) {
    // check that the edge exist
    if ( ! __graph.existsEdge( X,Y ) ) return;

    // find a MPS containing the edge (X,Y)
    const List<NodeId>& mps1 = __mps_of_node[X];
    const List<NodeId>& mps2 = __mps_of_node[Y];
    NodeId Mx = mps1[0];

    if ( mps1.size() <= mps2.size() ) {
      for ( ListConstIterator<NodeId> iter = mps1.begin();
            iter != mps1.end(); ++iter ) {
        if ( __T_mpd.clique( *iter ).contains( Y ) ) {
          Mx = *iter;
          break;
        }
      }
    }
    else {
      for ( ListConstIterator<NodeId> iter = mps2.begin();
            iter != mps2.end(); ++iter ) {
        if ( __T_mpd.clique( *iter ).contains( X ) ) {
          Mx = *iter;
          break;
        }
      }
    }

    // mark the MPS that need be updated
    __markAffectedMPSsByRemoveLink
      ( Mx, Mx, std::pair<NodeId,NodeId> ( std::min( X,Y ), std::max( X,Y ) ) );
    __require_update = true;

    // remove the edge (X,Y) from the graph
    __graph.eraseEdge( Edge (X,Y ) );
  }


  // ==============================================================================
  /// removes a node from the graph (the join tree may need a triangulation update)
  // ==============================================================================
  void IncrementalTriangulation::eraseNode( NodeId X ) {
    // remove all the edges adjacent to the node
    {
      const EdgeSet& neighbours = __graph.neighbours( X );
      for ( EdgeSetIterator neighbour_edge = neighbours.begin ();
            neighbour_edge != neighbours.end (); ++neighbour_edge )
        eraseEdge( neighbour_edge->first(), neighbour_edge->second() );
    }
    
    // remove X from the MPS containing X
    List<NodeId>& MPS_of_X = __mps_of_node[X];

    for ( ListIterator<NodeId> iter = MPS_of_X.begin();
          iter != MPS_of_X.end(); ++iter ) {
      __T_mpd.eraseFromClique( *iter, X );
      // if the intersection between *iter and one of its neighbour is empty,
      // remove the edge linking them

      const EdgeSet& neighbours = __T_mpd.neighbours( *iter );
      for ( EdgeSetIterator neighbour = neighbours.begin ();
            neighbour != neighbours.end (); ++neighbour )
        if ( __T_mpd.separator( *iter,neighbour->other( *iter ) ).size()==0 )
          __T_mpd.eraseEdge( Edge ( *iter,neighbour->other( *iter ) ) );
    }

    // remove X from the cliques containing X
    for ( ListIterator<NodeId> iter=MPS_of_X.begin();
          iter!=MPS_of_X.end(); ++iter ) {
      const std::vector<NodeId>& cliques_of_X = __cliques_of_mps[*iter];

      for ( unsigned int i = 0; i < cliques_of_X.size(); ++i ) {
        __junction_tree.eraseFromClique( cliques_of_X[i], X );
        // if the intersection between *iter and one of its neighbour is empty,
        // remove the edge linking them

        const EdgeSet& neighbours = __junction_tree.neighbours( cliques_of_X[i] );
        for ( EdgeSetIterator neighbour = neighbours.begin ();
              neighbour != neighbours.end (); ++neighbour )
          if ( __junction_tree.separator
               ( cliques_of_X[i], neighbour->other( cliques_of_X[i] ) ).size()==0 )
            __junction_tree.eraseEdge
              ( Edge ( cliques_of_X[i],neighbour->other( cliques_of_X[i] ) ) );
      }
    }

    // if the MPS containing X is empty, then remove it, as well as the
    // corresponding clique in the junction tree (which also only contain X)
    if (( MPS_of_X.size()==1 ) &&
        ( __T_mpd.clique( MPS_of_X[0] ).size()==0 ) ) {
      __junction_tree.eraseNode( __cliques_of_mps[MPS_of_X[0]][0] );
      __T_mpd.eraseNode( MPS_of_X[0] );
      __mps_of_clique.erase( __cliques_of_mps[MPS_of_X[0]][0] );
      __cliques_of_mps.erase( MPS_of_X[0] );
      __mps_affected.erase( MPS_of_X[0] );
    }

    __mps_of_node.erase( X );

    // remove X completely from the graph
    __graph.eraseNode( X );
    __modalities.erase( X );
  }


  // ==============================================================================
  /// add a new edge to the graph
  // ==============================================================================
  int IncrementalTriangulation::__markAffectedMPSsByAddLink
  ( const NodeId& Mx, const NodeId& Mz, const NodeId& X, const NodeId& Y ) {
    // check if Mx contains Y. In this case, mark Mx and return 1 to indicate that
    // Y has been found or 2 to indicate that Y has been found and that the nearest
    // MPS containing X has been marked
    const NodeSet& cliqueMX = __T_mpd.clique( Mx );

    if ( cliqueMX.contains( Y ) ) {
      __mps_affected[Mx] = true;
      if ( cliqueMX.contains( X ) ) return 2;
      return 1;
    }

    // parse Mx's neighbours until we find Y
    const EdgeSet& neighbours = __T_mpd.neighbours( Mx );
    for ( EdgeSetIterator neighbour = neighbours.begin ();
          neighbour != neighbours.end (); ++neighbour ) {
      NodeId other_node = neighbour->other( Mx );

      if ( other_node != Mz ) {
        int neighbourStatus = __markAffectedMPSsByAddLink( other_node,Mx,X,Y );
        if ( neighbourStatus == 2 )
          return 2;
        else if ( neighbourStatus == 1 ) {
          __mps_affected[Mx] = true;
          if ( cliqueMX.contains( X ) ) return 2;
          return 1;
        }
      }
    }
    
    // indicate that X was not found
    return 0;
  }
  

  // ==============================================================================
  /// adds a new edge to the graph (the join tree may need a triangulation update)
  // ==============================================================================
  void IncrementalTriangulation::insertEdge ( NodeId X, NodeId Y ) {
    // add the edge to the graph
    __graph.insertEdge( X,Y );

    // take any MPS containing X and search its tree to find Y
    NodeId& mps_X = __mps_of_node[X][0];
    int found = __markAffectedMPSsByAddLink( mps_X, mps_X, X, Y );

    if ( found == 0 ) {
      // the mps containing X do not belong to the same tree as those containing Y
      NodeId& mps_Y = __mps_of_node[Y][0];

      // find a clique in mps_X containing X and another in mps_Y containing Y
      // and add a clique XY to the junction tree linked to the cliques found
      // in mps_X and mps_Y
      const std::vector<NodeId>& cliques_X = __cliques_of_mps[mps_X];
      const std::vector<NodeId>& cliques_Y = __cliques_of_mps[mps_Y];
      NodeId c_X = 0, c_Y = 0;

      for ( unsigned int i = 0; i < cliques_X.size(); ++i ) {
        if ( __junction_tree.clique( cliques_X[i] ).contains( X ) ) {
          c_X = cliques_X[i];
          break;
        }
      }

      for ( unsigned int i = 0; i < cliques_Y.size(); ++i ) {
        if ( __junction_tree.clique( cliques_Y[i] ).contains( Y ) ) {
          c_Y = cliques_Y[i];
          break;
        }
      }

      // link c_Y and c_X through a new node containing XY
      NodeSet nodes( 2 );      
      nodes.insert( X );
      nodes.insert( Y );
      NodeId newNode = __junction_tree.insertNode( nodes );
      __junction_tree.insertEdge( newNode, c_X );
      __junction_tree.insertEdge( newNode, c_Y );
      __T_mpd.insertNode( newNode, nodes );
      __T_mpd.insertEdge( newNode, mps_X );
      __T_mpd.insertEdge( newNode, mps_Y );

      // check that the maximal prime subgraph containing X is not X alone
      // in this case, remove this max prime subgraph, as well as the corresponding
      // clique in the junction tree
      if ( __T_mpd.clique( mps_X ).size() == 1 ) {
        __junction_tree.eraseNode( c_X );
        __T_mpd.eraseNode( mps_X );
        __mps_affected.erase( mps_X );
        __mps_of_clique.erase( c_X );
        __cliques_of_mps.erase( mps_X );
        mps_X = newNode;
      }
      else
        __mps_of_node[X].insert( newNode );

      // do the same thing as above for node Y
      if ( __T_mpd.clique( mps_Y ).size() == 1 ) {
        __junction_tree.eraseNode( c_Y );
        __T_mpd.eraseNode( mps_Y );
        __mps_affected.erase( mps_Y );
        __mps_of_clique.erase( c_Y );
        __cliques_of_mps.erase( mps_Y );
        mps_Y = newNode;
      }
      else
        __mps_of_node[Y].insert( newNode );

      std::vector<NodeId>& cl =
        __cliques_of_mps.insert( newNode, std::vector<NodeId>() );
      cl.push_back( newNode );
      __mps_of_clique.insert( newNode, newNode );
      __mps_affected.insert( newNode, false );
    }
    else
      __require_update = true;
  }


  // ==============================================================================
  /// set up a connected subgraph to be triangulated
  // ==============================================================================
  void IncrementalTriangulation::__setUpConnectedTriangulation
  ( NodeId Mx, NodeId Mfrom, UndiGraph& theGraph,
    std::vector<Edge>& notAffectedneighbourCliques,
    HashTable<NodeId,bool>& cliques_affected ) {
    // mark the clique so that we won't try to update it several times
    cliques_affected[Mx] = false;

    // get the nodes that are concerned by the triangulation update
    const NodeSet& clique = __junction_tree.clique( Mx );

    for ( NodeSetIterator iter_node = clique.begin();
          iter_node != clique.end(); ++iter_node )
      if ( ! theGraph.exists ( *iter_node ) )
        theGraph.insertNode( *iter_node );

    // go on with the neighbour cliques in the junction tree
    const EdgeSet& neighbours =  __junction_tree.neighbours ( Mx );
    for ( EdgeSetIterator neighbour = neighbours.begin (); 
          neighbour != neighbours.end (); ++neighbour ) {
      NodeId othernode = neighbour->other( Mx );

      if ( othernode != Mfrom ) {
        if ( cliques_affected.exists( othernode ) ) {
          __setUpConnectedTriangulation
            ( othernode, Mx, theGraph,
              notAffectedneighbourCliques, cliques_affected );
        }
        else {
          // indicate that we have a clique not affected that is adjacent
          // to an affected one
          notAffectedneighbourCliques.push_back( *neighbour );
        }
      }
    }
  }
  

  // ==============================================================================
  /// used for computing the junction tree of the maximal prime subgraphs
  // ==============================================================================
  void IncrementalTriangulation::__computeMaxPrimeMergings
  ( const NodeId& node, const NodeId& from,
    std::vector< std::pair<NodeId,NodeId> >& merged_cliques,
    HashTable<NodeId,bool>& mark, const NodeId& max_old_id ) const {
    mark[node] = true;
    NodeSetIterator iter_sep2;

    // check the separators on all the adjacent edges of Mx
    const EdgeSet& neighbours = __junction_tree.neighbours( node );
    for ( EdgeSetIterator iter_sep = neighbours.begin ();
          iter_sep != neighbours.end (); ++iter_sep ) {
      NodeId other_node = iter_sep->other( node );

      if ( other_node != from ) {
        const NodeSet& separator = __junction_tree.separator( node, other_node );

        // check that the separator between node and other_node is complete
        bool complete = true;
        for ( NodeSetIterator iter_sep1 = separator.begin();
              iter_sep1 != separator.end() && complete; ++iter_sep1 ) {
          iter_sep2 = iter_sep1;

          for ( ++iter_sep2; iter_sep2 != separator.end(); ++iter_sep2 ) {
            if ( ! __graph.existsEdge( *iter_sep1, *iter_sep2 ) ) {
              complete = false;
              break;
            }
          }
        }

        // here complete indicates whether the separator is complete or not
        if ( !complete )
          merged_cliques.push_back( std::pair<NodeId,NodeId>( other_node,node ) );

        if ( other_node > max_old_id )
          __computeMaxPrimeMergings( other_node, node, merged_cliques,
                                     mark, max_old_id );
      }
    }
  }
  

  // ==============================================================================
  // ==============================================================================
  void IncrementalTriangulation::__check() {
    // check que les noeuds du graphe appartiennent a l'arbre de jonction
    {
      HashTable<NodeId,bool> nodes = __graph.nodes().hashMap ( false );

      for ( NodeSetIterator iter = __junction_tree.beginNodes();
            iter != __junction_tree.endNodes(); ++iter ) {
        const NodeSet& clique = __junction_tree.clique( *iter );

        for ( NodeSetIterator iter2 = clique.begin();
              iter2 != clique.end(); ++iter2 )
          nodes[*iter2] = true;
      }

      for ( HashTableConstIterator<NodeId,bool> iter = nodes.begin();
            iter != nodes.end(); ++iter )
        if ( !*iter ) {
          std::cerr << "check nodes" << std::endl
                    << __graph << std::endl << __junction_tree << std::endl;
        }
    }

    // check que les arcs du graphe appartiennent a l'arbre de jonction
    {
      NodeSetIterator iter3;
      std::pair<NodeId,NodeId> thePair;
      HashTable<Edge,bool> edges = __graph.edges().hashMap( false );

      for ( NodeSetIterator iter = __junction_tree.beginNodes();
            iter != __junction_tree.endNodes(); ++iter ) {
        const NodeSet& clique = __junction_tree.clique( *iter );

        for ( NodeSetIterator iter2 = clique.begin();
              iter2 != clique.end(); ++iter2 ) {
          iter3 = iter2;

          for ( ++iter3; iter3 != clique.end(); ++iter3 ) {
            thePair.first  = std::min( *iter2, *iter3 );
            thePair.second = std::max( *iter2, *iter3 );

            if ( __graph.existsEdge( thePair.first, thePair.second ) )
              edges[Edge (thePair.first, thePair.second )] = true;
          }
        }
      }

      for ( HashTableConstIterator<Edge,bool> iter = edges.begin();
            iter != edges.end(); ++iter )
        if ( !*iter ) {
          std::cerr << "check edges" << std::endl
                    << __graph << std::endl << __junction_tree << std::endl;
        }
    }

    // check que les noeuds du graphe appartiennent a l'arbre de MPS
    {
      HashTable<NodeId,bool> nodes = __graph.nodes().hashMap ( false );

      for ( NodeSetIterator iter = __T_mpd.beginNodes();
            iter != __T_mpd.endNodes(); ++iter ) {
        const NodeSet& clique = __T_mpd.clique( *iter );

        for ( NodeSetIterator iter2 = clique.begin();
              iter2 != clique.end(); ++iter2 )
          nodes[*iter2] = true;
      }

      for ( HashTableConstIterator<NodeId,bool> iter = nodes.begin();
            iter != nodes.end(); ++iter )
        if ( !*iter ) {
          std::cerr << "check nodes" << std::endl
                    << __graph << std::endl << __T_mpd << std::endl;
        }
    }

    // check que les arcs du graphe appartiennent a l'arbre de MPS
    {
      NodeSetIterator iter3;
      std::pair<NodeId,NodeId> thePair;
      HashTable<Edge,bool> edges = __graph.edges().hashMap( false );

      for ( NodeSetIterator iter = __T_mpd.beginNodes();
            iter != __T_mpd.endNodes(); ++iter ) {
        const NodeSet& clique = __T_mpd.clique( *iter );

        for ( NodeSetIterator iter2 = clique.begin();
              iter2 != clique.end(); ++iter2 ) {
          iter3 = iter2;

          for ( ++iter3; iter3 != clique.end(); ++iter3 ) {
            thePair.first  = std::min( *iter2, *iter3 );
            thePair.second = std::max( *iter2, *iter3 );
            
            if ( __graph.existsEdge( thePair.first, thePair.second ) )
              edges[Edge( thePair.first, thePair.second )] = true;
          }
        }
      }
      
      for ( HashTableConstIterator<Edge,bool> iter = edges.begin();
            iter != edges.end(); ++iter )
        if ( !*iter ) {
          std::cerr << "check edges" << std::endl
                    << __graph << std::endl << __T_mpd << std::endl;
        }
    }

    // check le MPS of node
    {
      HashTable< NodeId,HashTable<NodeId,bool> > chk;

      for ( NodeSetIterator iter = __graph.beginNodes();
            iter != __graph.endNodes(); ++iter )
        chk.insert( *iter, HashTable<NodeId,bool>() );

      for ( NodeSetIterator iter = __T_mpd.beginNodes();
            iter != __T_mpd.endNodes(); ++iter ) {
        const NodeSet& clique = __T_mpd.clique( *iter );

        for ( NodeSetIterator iter2 = clique.begin();
              iter2 != clique.end(); ++iter2 )
          chk[*iter2].insert( *iter, false );
      }

      for ( HashTableConstIterator< NodeId,List<NodeId> > iter =  __mps_of_node.begin();
            iter !=  __mps_of_node.end(); ++iter ) {
        const List<NodeId>& liste = *iter;
        HashTable<NodeId,bool>& hash = chk[iter.key()];

        for ( ListConstIterator<NodeId> iter2 = liste.begin();
              iter2 != liste.end(); ++iter2 ) {
          if ( !hash.exists( *iter2 ) )
            std::cerr << "check mps of nodes" << std::endl
                      << __T_mpd << std::endl << __mps_of_node << std::endl;

          hash[*iter2] = true;
        }
      }

      for ( HashTableConstIterator< NodeId,HashTable<NodeId,bool> > iter=chk.begin();
            iter != chk.end(); ++iter ) {
        const HashTable<NodeId,bool>& hash = *iter;

        for ( HashTableConstIterator<NodeId,bool> iter2 = hash.begin();
              iter2 != hash.end(); ++iter2 )
          if ( !*iter2 )
            std::cerr << "check mps of nodes2" << std::endl
                      << __T_mpd << std::endl << __mps_of_node << std::endl;
      }
    }

    // check le mps of clique
    {

    }
  }


  // ================================================================================
  /// updates the triangulated graph using the modif list
  // ================================================================================
  void IncrementalTriangulation::updateTriangulation() {
    if ( ! __require_update ) return;

    // the last ID generated for the junction tree and the max prime subgraph tree
    // this enables to discriminate between "old" nodes and the new ones created
    // by the triangulation
    NodeId max_old_id = __junction_tree.maxId ();

    // a temporary subgraph in which we actually perform the triangulation
    UndiGraph tmp_graph;

    // the set of all the cliques that should be affected by the different
    // triangulations we will perform (one by connected component)
    HashTable<NodeId,bool> all_cliques_affected( __junction_tree.nodes().size() );

    // for each triangulation, we will keep track of the cliques of the
    // junction tree that are not affected by the triangulation but that are
    // adjacent to cliques affected. This will enable us to connect easily the
    // newly created cliques with the old ones.
    std::vector<Edge> notAffectedneighbourCliques;

    // parse all the affected MPS and get the corresponding cliques
    for ( HashTableConstIterator<NodeId,bool> iter_mps = __mps_affected.begin();
          iter_mps != __mps_affected.end(); ++iter_mps ) {
      if ( *iter_mps ) {
        // get the cliques contained in this MPS
        const std::vector<NodeId>& cliques = __cliques_of_mps[iter_mps.key()];

        for ( unsigned int i = 0; i < cliques.size(); ++i )
          all_cliques_affected.insert( cliques[i],true );
      }
    }

    // for each connected set of cliques involved in the triangulations
    // perform a new triangulation and update the max prime subgraph tree
    for ( HashTableConstIterator<NodeId,bool> iter_clique = all_cliques_affected.begin();
          iter_clique != all_cliques_affected.end(); ++iter_clique ) {
      if ( *iter_clique ) {
        // set up the connected subgraph that need be retriangulated and the cliques
        // that are affected by this triangulation
        tmp_graph.clear ();
        notAffectedneighbourCliques.clear();
        __setUpConnectedTriangulation( iter_clique.key(), iter_clique.key(),
                                       tmp_graph, notAffectedneighbourCliques,
                                       all_cliques_affected );

        // insert the edges in tmp_graph
        for ( EdgeSetIterator iter_edge = __graph.beginEdges();
              iter_edge != __graph.endEdges(); ++iter_edge ) {
          try { tmp_graph.insertEdge( iter_edge->first(), iter_edge->second() ); }
          catch ( InvalidEdge& ) { } // both extremities must be in tmp_graph
        }

        // remove from the mps_of_node table the affected mps containing the node
        for ( NodeSetIterator iter_node = tmp_graph.beginNodes();
              iter_node != tmp_graph.endNodes(); ++iter_node ) {
          List<NodeId>& mps = __mps_of_node[ *iter_node ];

          for ( HashTableConstIterator<NodeId,bool> iter_mps = __mps_affected.begin();
                iter_mps != __mps_affected.end(); ++iter_mps ) {
            if ( *iter_mps )
              mps.eraseByVal( iter_mps.key() );
          }
        }

        // now tmp_graph contains the graph that should be triangulated.
        // so triangulate it and get its junction tree
        __triangulation->setGraph( tmp_graph, __modalities );
        const CliqueGraph& tmp_junction_tree = __triangulation->junctionTree();

        // now, update the global junction tree
        // first add the nodes of tmp_junction_tree to __junction_tree
        HashTable<NodeId,NodeId>
          tmp2global_junction_tree( tmp_junction_tree.nodes().size() );

        for ( NodeSetIterator iter_jt = tmp_junction_tree.beginNodes();
              iter_jt != tmp_junction_tree.endNodes(); ++iter_jt ) {
          NodeId new_id = __junction_tree.insertNode
            ( tmp_junction_tree.clique( *iter_jt ) );
          // translate the id of the temprary JT into an id of the global JT
          tmp2global_junction_tree.insert( *iter_jt, new_id );
        }

        // and add the edges of tmp_junction_tree to __junction_tree
        for ( EdgeSetIterator iter_edge = tmp_junction_tree.beginEdges();
              iter_edge != tmp_junction_tree.endEdges(); ++iter_edge )
          __junction_tree.insertEdge
            ( tmp2global_junction_tree[iter_edge->first()],
              tmp2global_junction_tree[iter_edge->second()] );

        // second get the edges in __junction_tree that have an extremal clique R
        // in the affected clique set and the other one S not in the affected set
        // and see which new node V in the __junction_tree should be connected to S.
        // The running intersection property guarrantees that the clique in the
        // tmp_junction_tree that results from the elimination (during the
        // triangulation process) of the first eliminated node in the separator
        // between R and S is an admissible candidate
        for ( unsigned int i = 0; i < notAffectedneighbourCliques.size(); ++i ) {
          // now find the first eliminated node in the separator
          const NodeSet& sep =
            __junction_tree.separator( notAffectedneighbourCliques[i] );
          unsigned int __elim_order = tmp_graph.nodes().size() + 1;
          NodeId elim_node = 0;

          for ( NodeSetIterator iter_sep = sep.begin();
                iter_sep != sep.end(); ++iter_sep ) {
            NodeId id = *iter_sep;
            unsigned int new_order = __triangulation->eliminationOrder( id );

            if ( new_order < __elim_order ) {
              __elim_order = new_order;
              elim_node = id;
            }
          }

          // find the __junction_tree clique corresponding to the elimination of
          // elim_node and insert an edge between this clique and that which was not
          // affected
          NodeId& to_connect =
            tmp2global_junction_tree[__triangulation->createdClique( elim_node )];

          NodeId not_affected =
            all_cliques_affected.exists( notAffectedneighbourCliques[i].first() )?
            notAffectedneighbourCliques[i].second() :
            notAffectedneighbourCliques[i].first();

          __junction_tree.insertEdge( not_affected,to_connect );

          if ( to_connect <= max_old_id )
            __T_mpd.insertEdge( __mps_of_clique[to_connect],
                                __mps_of_clique[not_affected] );

          // check that the separator created by the new edge is not equal to
          // to_connect. If this is the case, then to_connect is included in
          // not_affected and, hence, should be removed from the graph
          if ( __junction_tree.separator( not_affected,to_connect ).size() ==
               __junction_tree.clique( to_connect ).size() ) {
            __junction_tree.eraseEdge( Edge ( not_affected,to_connect ) );
            const EdgeSet& neighbours = __junction_tree.neighbours( to_connect );
            for ( EdgeSetIterator iter_neighbour = neighbours.begin();
                  iter_neighbour != neighbours.end(); ++iter_neighbour ) {
              __junction_tree.insertEdge
                ( iter_neighbour->other( to_connect ), not_affected );
              
              if ( iter_neighbour->other( to_connect ) <= max_old_id )
                __T_mpd.insertEdge
                  ( __mps_of_clique[iter_neighbour->other( to_connect )],
                    __mps_of_clique[not_affected] );
            }

            __junction_tree.eraseNode( to_connect );

            to_connect = not_affected;
          }
        }
      }
    }

    // remove the mps that were affected and update the cliques_of_mps table
    for ( HashTableConstIterator<NodeId,bool> iter_clique = all_cliques_affected.begin();
          iter_clique != all_cliques_affected.end(); ++iter_clique ) {
      __mps_of_clique.erase( iter_clique.key() );
      __junction_tree.eraseNode( iter_clique.key() );
    }

    for ( HashTableConstIterator<NodeId,bool> iter_MPS = __mps_affected.begin();
          iter_MPS != __mps_affected.end(); ++iter_MPS ) {
      if ( *iter_MPS ) {
        __cliques_of_mps.erase( iter_MPS.key() );
        __T_mpd.eraseNode( iter_MPS.key() );
      }
    }

    // now update the T_mpd so that it be coherent with the junction tree

    // the maximal prime subgraph join tree is created by aggregation of some
    // cliques. More precisely, when the separator between 2 cliques is not complete
    // in the original graph, then the two cliques must be merged. Create a hashtable
    // indicating which clique has been absorbed by some other clique. Keys are
    // the cliques absorbed, and values are the cliques that absorb them.
    HashTable<NodeId,NodeId>
      T_mpd_cliques( all_cliques_affected.size() );

    for ( NodeSetIterator iter_clique = __junction_tree.beginNodes();
          iter_clique != __junction_tree.endNodes(); ++iter_clique )
      if ( *iter_clique > max_old_id )
        T_mpd_cliques.insert( *iter_clique, *iter_clique );

    // parse all the separators of the junction tree and test those that are not
    // complete in the original graph
    std::vector< std::pair<NodeId,NodeId> > merged_cliques;

    HashTable<NodeId,bool> mark = T_mpd_cliques.map( false );

    for ( HashTableConstIterator<NodeId,bool> iter_clique = mark.begin();
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
          __mps_of_clique[merged_cliques[i].second];
    }

    // now we can create the max prime junction tree. First, create the new cliques
    // and create the corresponding cliques_of_mps entries
    for ( HashTableConstIterator<NodeId,NodeId> iter_clique = T_mpd_cliques.begin();
          iter_clique != T_mpd_cliques.end(); ++iter_clique ) {
      if ( iter_clique.key() == *iter_clique ) {
        __T_mpd.insertNode( *iter_clique, __junction_tree.clique( *iter_clique ) );
        std::vector<NodeId>& vect_of_cliques =
          __cliques_of_mps.insert( *iter_clique, std::vector<NodeId>() );
        vect_of_cliques.push_back( *iter_clique );
      }
    }

    // add to the cliques previously created the nodes of the cliques that were
    // merged into them and update the cliques_of_mps
    for ( HashTableConstIterator<NodeId,NodeId> iter = T_mpd_cliques.begin();
          iter != T_mpd_cliques.end(); ++iter ) {
      if (( iter.key() != *iter ) && ( *iter > max_old_id ) ) {
        const NodeSet& new_clique = __junction_tree.clique( iter.key() );

        for ( NodeSetIterator iter_node = new_clique.begin();
              iter_node != new_clique.end(); ++iter_node ) {
          try { __T_mpd.addToClique( *iter, *iter_node ); }
          catch ( DuplicateElement& ) { }
        }

        __cliques_of_mps[*iter].push_back( iter.key() );
      }
    }

    // update the mps_of_node and the mps_of_clique
    for ( HashTableConstIterator<NodeId,NodeId> iter = T_mpd_cliques.begin();
          iter != T_mpd_cliques.end(); ++iter ) {
      __mps_of_clique.insert( iter.key(), *iter );

      if ( iter.key() == *iter ) {
        const NodeSet& nodes = __T_mpd.clique( *iter );

        for ( NodeSetIterator iter_node = nodes.begin();
              iter_node != nodes.end(); ++iter_node )
          __mps_of_node[ *iter_node ].insert( *iter );
      }
    }

    // add the edges to the max prime subgraph tree
    for ( HashTableConstIterator<NodeId,NodeId> iter = T_mpd_cliques.begin();
          iter != T_mpd_cliques.end(); ++iter ) {
      NodeId clique = *iter;

      const EdgeSet& neighbours = __junction_tree.neighbours( iter.key() );
      for ( EdgeSetIterator iter_neighbour = neighbours.begin ();
            iter_neighbour != neighbours.end(); ++iter_neighbour ) {
        NodeId othernode = iter_neighbour->other( iter.key() );

        if ( T_mpd_cliques.exists( othernode ) ) {
          // here iter is linked to another node that has been created during
          // the triangulation
          NodeId otherClique = T_mpd_cliques[othernode];
          // avoid adding the same edge several times

          if ( clique > otherClique ) {
            __T_mpd.insertEdge( clique, otherClique );
          }
        }
        else {
          __T_mpd.insertEdge( clique, __mps_of_clique[othernode] );
        }
      }
    }

    // reset the MPS that are affected
    __mps_affected.clear();

    for ( NodeSetIterator iter = __T_mpd.beginNodes();
          iter != __T_mpd.endNodes(); ++iter )
      __mps_affected.insert( *iter, false );

    // remove all the structures used by the triangulation algorithm
    __triangulation->clear();

    // check();

    __require_update = false;
  }


  // ================================================================================
  /// sets the graph to the empty graph
  // ================================================================================
  void IncrementalTriangulation::clear() {
    __graph.clear ();
    __modalities.clear();
    __junction_tree.clear ();
    __T_mpd.clear ();
    __mps_of_node.clear();
    __cliques_of_mps.clear();
    __mps_affected.clear();
    __triangulation->clear();
    __require_update = false;
  }


  // ================================================================================
  /// changes the current graph
  // ================================================================================
  void IncrementalTriangulation::setGraph
  ( const UndiGraph& theGraph,
    const HashTable<NodeId,unsigned int>& modal ) {
    // remove the current graph
    clear();

    // shallow copy the graph passed in arent and update the structures
    // containing the informations useful for the triangulation

    for ( NodeSetIterator iter_node = theGraph.beginNodes();
          iter_node != theGraph.endNodes(); ++iter_node )
      insertNode( *iter_node, modal[*iter_node] );

    for ( EdgeSetIterator iter_edge = theGraph.beginEdges();
          iter_edge != theGraph.endEdges(); ++iter_edge )
      insertEdge( iter_edge->first(), iter_edge->second() );
  }  


} /* namespace gum */
  

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
