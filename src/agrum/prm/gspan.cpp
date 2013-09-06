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
/**
 * @file
 * @brief Implementation of gspan.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#include <agrum/prm/gspan.h>

#ifdef GUM_NO_INLINE
#include <agrum/prm/gspan.inl>
#endif // GUM_NO_INLINE

namespace gum {
  namespace prm {

    void
    GSpan::discoverPatterns() {
      Timer t;
      __sortNodesAndEdges();
      gspan::InterfaceGraph graph( *__graph );

      for ( std::list<NodeId>::iterator root = __tree.roots().begin(); root != __tree.roots().end(); ++root ) {
        if ( __tree.strategy().accept_root( &( __tree.pattern( *root ) ) ) ) {
          gspan::Pattern& p = __tree.pattern( *root );
          __subgraph_mining( graph, p );

          for ( UndiGraph::NodeIterator node = __tree.iso_graph( p ).beginNodes(); node != __tree.iso_graph( p ).endNodes(); ++node ) {
            Instance* u = __tree.iso_map( p, *node ).atPos( 0 );
            Instance* v = __tree.iso_map( p, *node ).atPos( 1 );
            graph.graph().eraseEdge( Edge( graph.id( u ), graph.id( v ) ) );
          }
        }
      }

      __sortPatterns();
    }

    void
    GSpan::__sortNodesAndEdges() {
      typedef Bijection<Idx, gspan::LabelData*>::iterator BijIterator;

      for ( BijIterator iter = __graph->labels().begin(); iter != __graph->labels().end(); ++iter ) {
        try {
          if ( __graph->nodes( iter.second() ).size() >= 2 ) {
            __cost.insert( iter.second(), __cost_func( iter.second()->tree_width, __graph->nodes( iter.second() ).size() ) );
            __nodes.push_back( const_cast<gspan::LabelData*>( iter.second() ) );
          }
        } catch ( NotFound& ) {
          // It's a label over edges
          if ( __isEdgeEligible( *( __graph->edges( iter.second() ).begin() ) ) ) {
            __cost.insert( iter.second(), __cost_func( iter.second()->tree_width, __graph->edges( iter.second() ).size() ) );
            __edges.push_back( iter.second() );
          }
        }
      }

      Bijection<Idx, gspan::LabelData*>* new_labels = new Bijection<Idx, gspan::LabelData*>();
      GSpan::LabelSort my_sort( this );
      std::sort( __nodes.begin(), __nodes.end(), my_sort );
      std::sort( __edges.begin(), __edges.end(), my_sort );
      Size idx = 0;

      for ( std::vector<gspan::LabelData*>::iterator iter = __nodes.begin(); iter != __nodes.end(); ++iter ) {
        ( *iter )->id = ++idx;
        new_labels->insert( idx, *iter );
      }

      for ( std::vector<gspan::LabelData*>::iterator iter = __edges.begin(); iter != __edges.end(); ++iter ) {
        ( *iter )->id = ++idx;
        new_labels->insert( idx, *iter );
        __tree.addRoot( **iter );
      }

      delete __graph->__labels;
      __graph->__labels = new_labels;
    }

    void
    GSpan::__subgraph_mining( gspan::InterfaceGraph& ig, gspan::Pattern& pat ) {
      std::vector<gspan::Pattern*> stack;
      stack.push_back( &pat );
      // Pointers used in the following while
      gspan::Pattern* p = 0;
      HashTable<std::string, gspan::DFSTree::EdgeGrowth*>* edge_count = 0;
      gspan::DFSTree::EdgeGrowth* edge_growth = 0;
      Sequence<Instance*>* seq = 0; Instance* current = 0; Instance* neighbor = 0;
      // Neighbor_id is the neighbor's id in the interface graph and neighbor_node
      // is its id in the rightmost path in the case of a backward edge growth
      NodeId current_id = 0; NodeId neighbor_id = 0; NodeId neighbor_node =0;
      gspan::LabelData* neighbor_label = 0; gspan::EdgeData* edge_data = 0;
      size_t idx;
      const std::list<NodeId>* children = 0;

      while ( not stack.empty() ) {
        // Getting next pattern
        p = stack.back();
        stack.pop_back();

        if ( p->code().codes.size() < __depth_stop ) {
          // We need the rightmost path of p
          std::list<NodeId> r_path;
          p->rightmostPath( r_path );
          // Mapping used to count each possible child of p, the position in the vector
          // matches the one in the rightmost path
          std::vector< HashTable<std::string, gspan::DFSTree::EdgeGrowth*>* > count_vector;

          for ( size_t i = 0; i < r_path.size(); ++i )
            count_vector.push_back( new HashTable<std::string, gspan::DFSTree::EdgeGrowth*>() );

          // For each subgraph represented by p, we look for a valid edge growth for each instance match of p in its isomorphism graph.
          for ( UndiGraph::NodeIterator iso_node = __tree.iso_graph( *p ).beginNodes(); iso_node != __tree.iso_graph( *p ).endNodes(); ++iso_node ) {
            seq = &( __tree.iso_map( *p, *iso_node ) );
            idx = 0;

            for ( std::list<NodeId>::iterator node = r_path.begin(); node != r_path.end(); ++node, ++idx ) {
              edge_count = count_vector[idx];
              // Retrieving the equivalent instance in the current match
              current = seq->atPos( ( Idx )( ( *node ) - 1 ) );
              current_id = ig.id( current );
              // Checking for edges not in p
              const NodeSet& neighbours = ig.graph().neighbours( current_id );

              for ( NodeSet::iterator edge = neighbours.begin(); edge != neighbours.end(); ++edge ) {
                neighbor_id = *edge;
                neighbor = ig.node( neighbor_id ).n;

                // We want a forward edge in any case or a backward edge if current is the rightmost vertex
                if ( ( not seq->exists( neighbor ) ) or ( ( *node ) == r_path.back() ) ) {
                  // Things we need to know: the LabelData data of the neighbour and, if it's a backward edge, its node id in the rightmost path
                  edge_data = &( ig.edge( current_id, neighbor_id ) );
                  neighbor_label = ( neighbor == edge_data->u )?edge_data->l_u:edge_data->l_v;
                  neighbor_node = ( seq->exists( neighbor ) )?seq->pos( neighbor ) + 1:0;
                  // Adding the edge growth to the edge_growth hashtable
                  gspan::DFSTree::EdgeGrowth temp_growth( *node, edge_data->l, neighbor_label, neighbor_node );

                  try {
                    edge_growth = ( *edge_count )[temp_growth.toString()];
                    edge_growth->insert( current, neighbor );
                  } catch ( NotFound& ) {
                    edge_growth = new gspan::DFSTree::EdgeGrowth( *node, edge_data->l, neighbor_label, neighbor_node );
                    edge_growth->insert( current, neighbor );
                    edge_count->insert( edge_growth->toString(), edge_growth );
                  }
                }
              }
            }
          }

          // Removing any infrequent child
          typedef HashTable<std::string, gspan::DFSTree::EdgeGrowth*>::iterator EdgeCountIter;

          for ( size_t node = 0; node < count_vector.size(); ++node ) {
            edge_count = count_vector[node];

            for ( EdgeCountIter growth = edge_count->begin(); growth != edge_count->end(); ++growth ) {
              try {
                __tree.growPattern( *p, **growth, 2 );
              } catch ( OperationNotAllowed& e ) {
                // The child was not minimal or was not worth considering
              }

              delete *growth;
            }

            delete edge_count;
          }

          // Calling __subgraph_mining over children of p
          children = &( __tree.children( *p ) );

          for ( std::list<NodeId>::const_reverse_iterator child = children->rbegin(); child != children->rend(); ++child )
            stack.push_back( &( __tree.pattern( *child ) ) );
        }
      }
    }

    void
    GSpan::__sortPatterns() {
      // First we put all the patterns in __patterns.
      std::vector<NodeId> stack;

      for ( std::list<NodeId>::reverse_iterator root = tree().roots().rbegin(); root != tree().roots().rend(); ++root ) {
        stack.push_back( *root );
      }

      NodeId id = 0;
      std::list<NodeId>* children = 0;

      while ( not stack.empty() ) {
        id = stack.back();
        stack.pop_back();
        __patterns.push_back( &( tree().pattern( id ) ) );
        children = &( tree().children( tree().pattern( id ) ) );

        for ( std::list<NodeId>::reverse_iterator child = children->rbegin(); child != children->rend(); ++child ) {
          stack.push_back( *child );
        }
      }

      if ( not __patterns.empty() ) {
        // We sort __patterns.
        GSpan::PatternSort my_sort( this );
        std::sort( __patterns.begin(), __patterns.end(), my_sort );
        // Now we need to find all the matches we can, using __patterns.
        // We start by the best Pattern and add it's maximal independent set to __chosen
        GSpan::MatchedInstances* matches = new GSpan::MatchedInstances();
        Set<NodeId>& max_indep_set = tree().max_indep_set( *( __patterns.front() ) );
        Sequence<Instance*>* match = 0;

        for ( Set<NodeId>::iterator node = max_indep_set.begin(); node != max_indep_set.end(); ++node ) {
          match = &( tree().iso_map( *( __patterns.front() ), *node ) );

          for ( Sequence<Instance*>::iterator i = match->begin(); i != match->end(); ++i ) {
            __chosen.insert( *i );
          }

          matches->insert( match );
        }

        __matched_instances.insert( __patterns.front(), matches );
        // Now we see what kind of pattern we can still use
        bool found;
        UndiGraph* iso_graph = 0;
        const NodeSet* neighbours = 0;

        for ( std::vector<gspan::Pattern*>::iterator p = __patterns.begin() + 1; p != __patterns.end(); ++p ) {
          UndiGraph reduced_iso_graph;
          std::vector<NodeId> degree_list;
          iso_graph = &( tree().iso_graph( **p ) );

          for ( UndiGraph::NodeIterator node = iso_graph->beginNodes(); node != iso_graph->endNodes(); ++node ) {
            found = false;
            match = &( tree().iso_map( **p, *node ) );

            for ( Sequence<Instance*>::iterator i = match->begin(); i != match->end(); ++i ) {
              if ( __chosen.exists( *i ) ) {
                found = true;
                break;
              }
            }

            if ( not found ) {
              // We add the pattern to the reduced isomorphism graph to compute the max independent set
              // over the remaining matches
              reduced_iso_graph.insertNode( *node );

              for ( UndiGraph::NodeIterator iso = reduced_iso_graph.beginNodes(); iso != reduced_iso_graph.endNodes(); ++iso ) {
                if ( iso_graph->existsEdge( *node, *iso ) ) {
                  reduced_iso_graph.insertEdge( *node, *iso );
                }
              }

              degree_list.push_back( *node );
            }
          }

          // We create a new set to hold all the chosen matches of p
          matches = new GSpan::MatchedInstances();
          // We can compute the max independent set and the matches belonging to it
          gspan::DFSTree::NeighborDegreeSort my_sort( reduced_iso_graph );
          std::sort( degree_list.begin(), degree_list.end(), my_sort );
          Set<NodeId> removed;

          for ( std::vector<NodeId>::iterator node = degree_list.begin(); node != degree_list.end(); ++node ) {
            if ( not removed.exists( *node ) ) {
              // First we update removed to follow the max independent set algorithm
              removed.insert( *node );
              neighbours = &( reduced_iso_graph.neighbours( *node ) );

              for ( NodeSet::const_iterator neighbor = neighbours->begin(); neighbor != neighbours->end(); ++neighbor ) {
                removed.insert( *neighbor );
              }

              // Second we update match and matches to keep track of the current match
              match = &( tree().iso_map( **p, *node ) );
              matches->insert( match );

              for ( Sequence<Instance*>::iterator iter = match->begin(); iter != match->end(); ++iter ) {
                __chosen.insert( *iter );
              }
            }
          }

          __matched_instances.insert( *p, matches );
        }

        // // We remove patterns with 0 matches
        std::vector<size_t> trash;

        for ( size_t idx = 0; idx < __patterns.size(); ++idx )
          if ( __matched_instances[__patterns[idx]]->size() < 2 )
            trash.push_back( idx );

        while ( trash.size() ) {
          delete __matched_instances[__patterns[trash.back()]];
          __matched_instances.erase( __patterns[trash.back()] );
          //delete __patterns[trash.back()];
          __patterns[trash.back()] = __patterns.back();
          __patterns.pop_back();
          trash.pop_back();
        }
      }
    }

//double
//GSpan::patterns_cost() const {
//  double cost = 0;
//  typedef HashTable<gspan::Pattern*, MatchedInstances*>::const_iterator MatchIter;
//  for (MatchIter match = __matched_instances.begin(); match != __matched_instances.end(); ++match) {
//    cost += (tree().frequency(*(match.key())) * tree().cost(*(match.key())));
//  }
//  for (UndiGraph::NodeIterator node = __graph->graph().beginNodes(); node != __graph->graph().endNodes(); ++node) {
//    if (not __chosen.exists(__graph->node(*node).n)) {
//      cost += __instance_cost(__graph->node(*node).n);
//    }
//  }
//  return cost;
//}
//
//double
//GSpan::sve_cost() const {
//  unsigned long cost = 0;
//  for (UndiGraph::NodeIterator node = __graph->graph().beginNodes(); node != __graph->graph().endNodes(); ++node) {
//    cost += __instance_cost(__graph->node(*node).n);
//  }
//  return cost;
//}
//
//double
//GSpan::__instance_cost(Instance* i) const {
//  unsigned cost = 1;
//  Set<ClassElement*> inputs;
//  for (Set<SlotChain*>::iterator slot_chain = i->type().slotChains().begin(); slot_chain != i->type().slotChains().end(); ++slot_chain) {
//    for (Set<Instance*>::iterator instance = i->getInstances((*slot_chain)->id()).begin();
//         instance != i->getInstances((*slot_chain)->id()).end(); ++instance) {
//      if (not inputs.exists(&((*instance)->get((*slot_chain)->lastElt().id())))) {
//        inputs.insert(&((*instance)->get((*slot_chain)->lastElt().id())));
//        cost *= (*instance)->get((*slot_chain)->lastElt().id()).type().variable().domainSize();
//      }
//    }
//  }
//  for (DAG::NodeIterator node = i->type().dag().beginNodes(); node != i->type().dag().endNodes(); ++node) {
//    if (i->type().isOutputNode(i->type().get(*node)) and i->hasRefAttr(*node)) {
//      cost *= i->get(*node).type().variable().domainSize();
//    }
//  }
//  return cost;
//}

  } /* namespace prm */
} /* namespace gum */

