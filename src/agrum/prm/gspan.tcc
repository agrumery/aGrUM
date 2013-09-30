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
 * @brief Inline implementation of gspan.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */
#include <agrum/prm/gspan.h>
#include <agrum/prm/gspan/edgeGrowth.h>

namespace gum {
  namespace prm {

    template<typename GUM_SCALAR>
    void
    GSpan<GUM_SCALAR>::discoverPatterns() {
      Timer t;
      __sortNodesAndEdges();
      gspan::InterfaceGraph<GUM_SCALAR> graph ( *__graph );

      for ( auto root = __tree.roots().begin(); root != __tree.roots().end(); ++root ) {
        if ( __tree.strategy().accept_root ( & ( __tree.pattern ( *root ) ) ) ) {
          gspan::Pattern& p = __tree.pattern ( *root );
          __subgraph_mining ( graph, p );

          for ( const auto node : __tree.iso_graph ( p ).nodes() ) {
            Instance<GUM_SCALAR>* u = __tree.iso_map ( p, node ).atPos ( 0 );
            Instance<GUM_SCALAR>* v = __tree.iso_map ( p, node ).atPos ( 1 );
            graph.graph().eraseEdge ( Edge ( graph.id ( u ), graph.id ( v ) ) );
          }
        }
      }

      __sortPatterns();
    }

    template<typename GUM_SCALAR>
    void
    GSpan<GUM_SCALAR>::__sortNodesAndEdges() {
      for ( auto iter = __graph->labels().begin(); iter != __graph->labels().end(); ++iter ) {
        try {
          if ( __graph->nodes ( iter.second() ).size() >= 2 ) {
            __cost.insert ( iter.second(), __cost_func ( iter.second()->tree_width, __graph->nodes ( iter.second() ).size() ) );
            __nodes.push_back ( const_cast<gspan::LabelData*> ( iter.second() ) );
          }
        } catch ( NotFound& ) {
          // It's a label over edges
          if ( __isEdgeEligible ( * ( __graph->edges ( iter.second() ).begin() ) ) ) {
            __cost.insert ( iter.second(), __cost_func ( iter.second()->tree_width, __graph->edges ( iter.second() ).size() ) );
            __edges.push_back ( iter.second() );
          }
        }
      }

      Bijection<Idx, gspan::LabelData*>* new_labels = new Bijection<Idx, gspan::LabelData*>();
      GSpan<GUM_SCALAR>::LabelSort my_sort ( this );
      std::sort ( __nodes.begin(), __nodes.end(), my_sort );
      std::sort ( __edges.begin(), __edges.end(), my_sort );
      Size idx = 0;

      for ( auto iter = __nodes.begin(); iter != __nodes.end(); ++iter ) {
        ( *iter )->id = ++idx;
        new_labels->insert ( idx, *iter );
      }

      for ( auto iter = __edges.begin(); iter != __edges.end(); ++iter ) {
        ( *iter )->id = ++idx;
        new_labels->insert ( idx, *iter );
        __tree.addRoot ( **iter );
      }

      delete __graph->__labels;
      __graph->__labels = new_labels;
    }

    template<typename GUM_SCALAR>
    void
    GSpan<GUM_SCALAR>::__subgraph_mining ( gspan::InterfaceGraph<GUM_SCALAR>& ig, gspan::Pattern& pat ) {
      std::vector<gspan::Pattern*> stack;
      stack.push_back ( &pat );
      // Pointers used in the following while
      gspan::Pattern* p = 0;
      HashTable<std::string, gspan::EdgeGrowth<GUM_SCALAR>*>* edge_count = 0;
      gspan::EdgeGrowth<GUM_SCALAR>* edge_growth = 0;
      Sequence<Instance<GUM_SCALAR>*>* seq = 0; Instance<GUM_SCALAR>* current = 0; Instance<GUM_SCALAR>* neighbor = 0;
      // Neighbor_id is the neighbor's id in the interface graph and neighbor_node
      // is its id in the rightmost path in the case of a backward edge growth
      NodeId current_id = 0; NodeId neighbor_id = 0; NodeId neighbor_node = 0;
      gspan::LabelData* neighbor_label = 0; typename gspan::EdgeData<GUM_SCALAR>* edge_data = 0;
      size_t idx;
      const std::list<NodeId>* children = 0;

      while ( not stack.empty() ) {
        // Getting next pattern
        p = stack.back();
        stack.pop_back();

        if ( p->code().codes.size() < __depth_stop ) {
          // We need the rightmost path of p
          std::list<NodeId> r_path;
          p->rightmostPath ( r_path );
          // Mapping used to count each possible child of p, the position in the vector
          // matches the one in the rightmost path
          std::vector< HashTable<std::string, gspan::EdgeGrowth<GUM_SCALAR>*>* > count_vector;

          for ( size_t i = 0; i < r_path.size(); ++i )
            count_vector.push_back ( new HashTable<std::string, gspan::EdgeGrowth<GUM_SCALAR>*>() );

          // For each subgraph represented by p, we look for a valid edge growth for each instance match of p in its isomorphism graph.
          for ( const auto iso_node : __tree.iso_graph ( *p ).nodes() ) {
            seq = & ( __tree.iso_map ( *p, iso_node ) );
            idx = 0;

            for ( auto node = r_path.begin(); node != r_path.end(); ++node, ++idx ) {
              edge_count = count_vector[idx];
              // Retrieving the equivalent instance in the current match
              current = seq->atPos ( ( Idx ) ( ( *node ) - 1 ) );
              current_id = ig.id ( current );
              // Checking for edges not in p
              const NodeSet& neighbours = ig.graph().neighbours ( current_id );

              for ( auto edge = neighbours.begin(); edge != neighbours.end(); ++edge ) {
                neighbor_id = *edge;
                neighbor = ig.node ( neighbor_id ).n;

                // We want a forward edge in any case or a backward edge if current is the rightmost vertex
                if ( ( not seq->exists ( neighbor ) ) or ( ( *node ) == r_path.back() ) ) {
                  // Things we need to know: the LabelData data of the neighbour and, if it's a backward edge, its node id in the rightmost path
                  edge_data = & ( ig.edge ( current_id, neighbor_id ) );
                  neighbor_label = ( neighbor == edge_data->u ) ? edge_data->l_u : edge_data->l_v;
                  neighbor_node = ( seq->exists ( neighbor ) ) ? seq->pos ( neighbor ) + 1 : 0;
                  // Adding the edge growth to the edge_growth hashtable
                  gspan::EdgeGrowth<GUM_SCALAR> temp_growth ( *node, edge_data->l, neighbor_label, neighbor_node );

                  try {
                    edge_growth = ( *edge_count ) [temp_growth.toString()];
                    edge_growth->insert ( current, neighbor );
                  } catch ( NotFound& ) {
                    edge_growth = new gspan::EdgeGrowth<GUM_SCALAR> ( *node, edge_data->l, neighbor_label, neighbor_node );
                    edge_growth->insert ( current, neighbor );
                    edge_count->insert ( edge_growth->toString(), edge_growth );
                  }
                }
              }
            }
          }

          // Removing any infrequent child
          for ( size_t node = 0; node < count_vector.size(); ++node ) {
            edge_count = count_vector[node];

            for ( auto growth = edge_count->begin(); growth != edge_count->end(); ++growth ) {
              try {
                __tree.growPattern ( *p, **growth, 2 );
              } catch ( OperationNotAllowed& e ) {
                // The child was not minimal or was not worth considering
              }

              delete *growth;
            }

            delete edge_count;
          }

          // Calling __subgraph_mining over children of p
          children = & ( __tree.children ( *p ) );

          for ( std::list<NodeId>::const_reverse_iterator child = children->rbegin(); child != children->rend(); ++child )
            stack.push_back ( & ( __tree.pattern ( *child ) ) );
        }
      }
    }

    template<typename GUM_SCALAR>
    void
    GSpan<GUM_SCALAR>::__sortPatterns() {
      // First we put all the patterns in __patterns.
      std::vector<NodeId> stack;

      for ( std::list<NodeId>::reverse_iterator root = tree().roots().rbegin(); root != tree().roots().rend(); ++root ) {
        stack.push_back ( *root );
      }

      NodeId id = 0;
      std::list<NodeId>* children = 0;

      while ( not stack.empty() ) {
        id = stack.back();
        stack.pop_back();
        __patterns.push_back ( & ( tree().pattern ( id ) ) );
        children = & ( tree().children ( tree().pattern ( id ) ) );

        for ( std::list<NodeId>::reverse_iterator child = children->rbegin(); child != children->rend(); ++child ) {
          stack.push_back ( *child );
        }
      }

      if ( not __patterns.empty() ) {
        // We sort __patterns.
        GSpan<GUM_SCALAR>::PatternSort my_sort ( this );
        std::sort ( __patterns.begin(), __patterns.end(), my_sort );
        // Now we need to find all the matches we can, using __patterns.
        // We start by the best Pattern and add it's maximal independent set to __chosen
        GSpan<GUM_SCALAR>::MatchedInstances* matches = new GSpan<GUM_SCALAR>::MatchedInstances();
        Set<NodeId>& max_indep_set = tree().max_indep_set ( * ( __patterns.front() ) );
        Sequence<Instance<GUM_SCALAR>*>* match = 0;

        for ( auto node = max_indep_set.begin(); node != max_indep_set.end(); ++node ) {
          match = & ( tree().iso_map ( * ( __patterns.front() ), *node ) );

          for ( auto i = match->begin(); i != match->end(); ++i ) {
            __chosen.insert ( *i );
          }

          matches->insert ( match );
        }

        __matched_instances.insert ( __patterns.front(), matches );
        // Now we see what kind of pattern we can still use
        bool found;
        UndiGraph* iso_graph = 0;
        const NodeSet* neighbours = 0;

        for ( auto p = __patterns.begin() + 1; p != __patterns.end(); ++p ) {
          UndiGraph reduced_iso_graph;
          std::vector<NodeId> degree_list;
          iso_graph = & ( tree().iso_graph ( **p ) );

          for ( const auto node : iso_graph->nodes() ) {
            found = false;
            match = & ( tree().iso_map ( **p, node ) );

            for ( auto i = match->begin(); i != match->end(); ++i ) {
              if ( __chosen.exists ( *i ) ) {
                found = true;
                break;
              }
            }

            if ( not found ) {
              // We add the pattern to the reduced isomorphism graph to compute the max independent set
              // over the remaining matches
              reduced_iso_graph.insertNode ( node );

              for ( const auto iso : reduced_iso_graph.nodes() ) {
                if ( iso_graph->existsEdge ( node, iso ) ) {
                  reduced_iso_graph.insertEdge ( node, iso );
                }
              }

              degree_list.push_back ( node );
            }
          }

          // We create a new set to hold all the chosen matches of p
          matches = new GSpan<GUM_SCALAR>::MatchedInstances();
          // We can compute the max independent set and the matches belonging to it
          typename gspan::DFSTree<GUM_SCALAR>::NeighborDegreeSort my_sort ( reduced_iso_graph );
          std::sort ( degree_list.begin(), degree_list.end(), my_sort );
          Set<NodeId> removed;

          for ( auto node = degree_list.begin(); node != degree_list.end(); ++node ) {
            if ( not removed.exists ( *node ) ) {
              // First we update removed to follow the max independent set algorithm
              removed.insert ( *node );
              neighbours = & ( reduced_iso_graph.neighbours ( *node ) );

              for ( NodeSet::const_iterator neighbor = neighbours->begin(); neighbor != neighbours->end(); ++neighbor ) {
                removed.insert ( *neighbor );
              }

              // Second we update match and matches to keep track of the current match
              match = & ( tree().iso_map ( **p, *node ) );
              matches->insert ( match );

              for ( auto iter = match->begin(); iter != match->end(); ++iter ) {
                __chosen.insert ( *iter );
              }
            }
          }

          __matched_instances.insert ( *p, matches );
        }

        // // We remove patterns with 0 matches
        std::vector<size_t> trash;

        for ( size_t idx = 0; idx < __patterns.size(); ++idx )
          if ( __matched_instances[__patterns[idx]]->size() < 2 )
            trash.push_back ( idx );

        while ( trash.size() ) {
          delete __matched_instances[__patterns[trash.back()]];
          __matched_instances.erase ( __patterns[trash.back()] );
          //delete __patterns[trash.back()];
          __patterns[trash.back()] = __patterns.back();
          __patterns.pop_back();
          trash.pop_back();
        }
      }
    }

    template<typename GUM_SCALAR> INLINE
    GSpan<GUM_SCALAR>::GSpan ( const PRM<GUM_SCALAR>& prm, const System<GUM_SCALAR>& sys, gspan::SearchStrategy<GUM_SCALAR>* strategy ) :
      __graph ( new gspan::InterfaceGraph<GUM_SCALAR> ( sys ) ), __tree ( *__graph, strategy ),
      __depth_stop ( INT_MAX ) {
      GUM_CONSTRUCTOR ( GSpan );
    }

    template<typename GUM_SCALAR> INLINE
    GSpan<GUM_SCALAR>::~GSpan() {
      GUM_DESTRUCTOR ( GSpan );
      for ( auto match = __matched_instances.begin(); match != __matched_instances.end(); ++match ) {
        delete *match;
      }

      delete __graph;
    }

    template<typename GUM_SCALAR> INLINE
    Size
    GSpan<GUM_SCALAR>::getMaxDFSDepth() const {
      return __depth_stop;
    }

    template<typename GUM_SCALAR> INLINE
    void
    GSpan<GUM_SCALAR>::setMaxDFSDepth ( Size depth ) {
      __depth_stop = depth;
    }

    template<typename GUM_SCALAR> INLINE
    gspan::DFSTree<GUM_SCALAR>&
    GSpan<GUM_SCALAR>::tree() { return __tree; }

    template<typename GUM_SCALAR> INLINE
    const gspan::DFSTree<GUM_SCALAR>&
    GSpan<GUM_SCALAR>::tree() const { return __tree; }

    template<typename GUM_SCALAR> INLINE
    unsigned long
    GSpan<GUM_SCALAR>::__cost_func ( unsigned int interface, unsigned int frequency ) {
      return interface * frequency;
    }

    template<typename GUM_SCALAR> INLINE
    std::vector<gspan::Pattern*>&
    GSpan<GUM_SCALAR>::patterns() {
      return __patterns;
    }

    template<typename GUM_SCALAR> INLINE
    const std::vector<gspan::Pattern*>&
    GSpan<GUM_SCALAR>::patterns() const {
      return __patterns;
    }

    template<typename GUM_SCALAR> INLINE
    typename GSpan<GUM_SCALAR>::MatchedInstances&
    GSpan<GUM_SCALAR>::matches ( const gspan::Pattern& p ) {
      return * ( __matched_instances[const_cast<gspan::Pattern*> ( &p )] );
    }

    template<typename GUM_SCALAR> INLINE
    const typename GSpan<GUM_SCALAR>::MatchedInstances&
    GSpan<GUM_SCALAR>::matches ( const gspan::Pattern& p ) const {
      return * ( __matched_instances[const_cast<gspan::Pattern*> ( &p )] );
    }

    template<typename GUM_SCALAR> INLINE
    gspan::InterfaceGraph<GUM_SCALAR>&
    GSpan<GUM_SCALAR>::interfaceGraph() {
      return *__graph;
    }

    template<typename GUM_SCALAR> INLINE
    const gspan::InterfaceGraph<GUM_SCALAR>&
    GSpan<GUM_SCALAR>::interfaceGraph() const {
      return *__graph;
    }

    template<typename GUM_SCALAR> INLINE
    bool
    GSpan<GUM_SCALAR>::__isEdgeEligible ( gspan::EdgeData<GUM_SCALAR>* e ) {
      return ( __graph->edges ( e->l ).size() >= 2 ) and
             ( __graph->nodes ( e->l_u ).size() >= 2 ) and
             ( __graph->nodes ( e->l_v ).size() >= 2 );
    }


// LalbeSort

    template<typename GUM_SCALAR> INLINE
    GSpan<GUM_SCALAR>::LabelSort::LabelSort ( GSpan* my_gspan ) :
      gspan ( my_gspan ) {
      GUM_CONSTRUCTOR ( GSpan<GUM_SCALAR>::LabelSort );
    }

    template<typename GUM_SCALAR> INLINE
    GSpan<GUM_SCALAR>::LabelSort::LabelSort ( const LabelSort& source ) :
      gspan ( source.gspan ) {
      GUM_CONS_CPY ( GSpan<GUM_SCALAR>::LabelSort );
    }

    template<typename GUM_SCALAR> INLINE
    GSpan<GUM_SCALAR>::LabelSort::~LabelSort() {
      GUM_DESTRUCTOR ( GSpan<GUM_SCALAR>::LabelSort );
    }

    template<typename GUM_SCALAR> INLINE
    bool
    GSpan<GUM_SCALAR>::LabelSort::operator() ( gspan::LabelData* i, gspan::LabelData* j ) {
      // We want a descending order
      //return gspan->__cost[i] > gspan->__cost[j];
      return gspan->__tree.strategy() ( i, j );
    }


// PatternSort


    template<typename GUM_SCALAR> INLINE
    GSpan<GUM_SCALAR>::PatternSort::PatternSort ( GSpan* my_gspan ) :
      gspan ( my_gspan ) {
      GUM_CONSTRUCTOR ( GSpan<GUM_SCALAR>::PatternSort );
    }

    template<typename GUM_SCALAR> INLINE
    GSpan<GUM_SCALAR>::PatternSort::PatternSort ( const PatternSort& source ) :
      gspan ( source.gspan ) {
      GUM_CONS_CPY ( GSpan<GUM_SCALAR>::PatternSort );
    }

    template<typename GUM_SCALAR> INLINE
    GSpan<GUM_SCALAR>::PatternSort::~PatternSort() {
      GUM_DESTRUCTOR ( GSpan<GUM_SCALAR>::PatternSort );
    }

    template<typename GUM_SCALAR> INLINE
    bool
    GSpan<GUM_SCALAR>::PatternSort::operator() ( gspan::Pattern* i, gspan::Pattern* j ) {
      // We want a descending order
      return gspan->tree().strategy().operator() ( i, j );
    }

  } /* namespace prm */
} /* namespace gum */

