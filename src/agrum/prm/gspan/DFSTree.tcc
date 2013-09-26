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
 * @brief Inline implementation of the DFSTree class.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#include <agrum/prm/gspan/DFSTree.h>


namespace gum {
  namespace prm {
    namespace gspan {
      template<typename GUM_SCALAR>
      DFSTree<GUM_SCALAR>::~DFSTree() {
        GUM_DESTRUCTOR ( DFSTree );

        for ( auto iter = __data.begin(); iter != __data.end(); ++iter ) {
          delete iter.key();
          delete *iter;
        }

        delete __strategy;
      }

      template<typename GUM_SCALAR>
      void
      DFSTree<GUM_SCALAR>::addRoot ( LabelData& label ) {
        HashTable<Pattern*, std::pair<Idx, Idx> > roots;
        HashTable<Pattern*, Sequence<EdgeData<GUM_SCALAR>*>* > roots_edges;
        const Set<EdgeData<GUM_SCALAR>*>& edges = __graph->edges ( &label );

        for ( auto iter = edges.begin(); iter != edges.end(); ++iter ) {
          bool u_first = ( ( *iter )->l_u->id < ( *iter )->l_v->id ) ? true : false;
          Idx u_idx = ( u_first ) ? ( *iter )->l_u->id : ( *iter )->l_v->id;
          Idx v_idx = ( not u_first ) ? ( *iter )->l_u->id : ( *iter )->l_v->id;
          bool found = false;

          for ( auto root = roots.begin(); root != roots.end(); ++root ) {
            if ( ( root->first == u_idx ) and ( root->second == v_idx ) ) {
              roots_edges[root.key()]->insert ( *iter );
              found = true;
              break;
            }
          }

          /// Then we create a new pattern
          if ( not found ) {
            Pattern* p = new Pattern();
            roots.insert ( p, std::make_pair ( u_idx, v_idx ) );
            roots_edges.insert ( p, new Sequence<EdgeData<GUM_SCALAR>*>() );
            roots_edges[p]->insert ( *iter );
            DFSTree<GUM_SCALAR>::PatternData* data = new DFSTree<GUM_SCALAR>::PatternData ( p );
            NodeId u = p->insertNode ( ( u_first ) ? * ( ( *iter )->l_u ) : * ( ( *iter )->l_v ) );
            NodeId v = p->insertNode ( ( not u_first ) ? * ( ( *iter )->l_u ) : * ( ( *iter )->l_v ) );
            p->insertArc ( u, v, label );
            __node_map.insert ( DiGraph::insertNode(), p );
            __data.insert ( p, data );
            __roots.push_back ( __node_map.first ( p ) );
          }
        }

        // This is used to compute the max independent set of p->max_indep_set
        for ( auto root = roots_edges.begin(); root != roots_edges.end(); ++root ) {
          __initialiaze_root ( root.key(), **root );
          strategy().accept_root ( root.key() );
          delete *root;
        }
      }

      template<typename GUM_SCALAR>
      void
      DFSTree<GUM_SCALAR>::__initialiaze_root ( Pattern* p, Sequence<EdgeData<GUM_SCALAR>*>& edge_seq ) {
        DFSTree<GUM_SCALAR>::PatternData* data = __data[p];
        std::vector<NodeId> degree_list;

        for ( auto edge = edge_seq.begin(); edge != edge_seq.end(); ++edge ) {
          bool u_first = ( ( *edge )->l_u->id < ( *edge )->l_v->id ) ? true : false;
          Sequence<Instance<GUM_SCALAR>*>* seq = new Sequence<Instance<GUM_SCALAR>*>();
          // Creating the multiset of instances matching p
          seq->insert ( ( u_first ) ? ( *edge )->u : ( *edge )->v );
          seq->insert ( ( not u_first ) ? ( *edge )->u : ( *edge )->v );
          NodeId an_id = data->iso_graph.insertNode();
          data->iso_map.insert ( an_id, seq );
          degree_list.push_back ( an_id );

          // Adding edges between two isomorphisms of p sharing at least one instance
          for ( auto iso = data->iso_map.begin(); iso != data->iso_map.end(); ++iso ) {
            if ( iso.key() != an_id ) {
              for ( auto inst = ( *iso )->begin(); inst != ( *iso )->end(); ++inst ) {
                if ( seq->exists ( *inst ) ) {
                  data->iso_graph.insertEdge ( an_id, iso.key() );
                  break;
                }
              }
            }
          }
        }

        // Computing p->max_indep_set using a greedy algorithm
        DFSTree<GUM_SCALAR>::NeighborDegreeSort my_operator ( data->iso_graph );
        std::sort ( degree_list.begin(), degree_list.end(), my_operator );
        Set<NodeId> removed;

        for ( std::vector<NodeId>::iterator node = degree_list.begin(); node != degree_list.end(); ++node ) {
          if ( not removed.exists ( *node ) ) {
            removed.insert ( *node );
            const NodeSet& neighbours = data->iso_graph.neighbours ( *node );

            for ( NodeSet::const_iterator neighbor = neighbours.begin();
                  neighbor != neighbours.end(); ++neighbor ) {
              removed.insert ( *neighbor );
            }

            data->max_indep_set.insert ( *node );
          }
        }
      }

      template<typename GUM_SCALAR>
      bool
      DFSTree<GUM_SCALAR>::__is_new_seq ( Sequence<Instance<GUM_SCALAR>*>& seq,
                                          NodeProperty<Sequence<Instance<GUM_SCALAR>*>*>& iso_map ) {
        for ( auto iter = iso_map.begin(); iter != iso_map.end(); ++iter ) {
          bool found = false;

          for ( auto jter = seq.begin(); jter != seq.end(); ++jter ) {
            if ( not ( *iter )->exists ( *jter ) ) {
              found = true;
              break;
            }
          }

          if ( not found ) {
            return false;
          }
        }

        return true;
      }

      template<typename GUM_SCALAR>
      void
      DFSTree<GUM_SCALAR>::__addChild ( Pattern& p, Pattern* child, EdgeGrowth& edge_growth ) {
        // Adding child to the tree
        NodeId node = DiGraph::insertNode();
        __node_map.insert ( node, child );
        // Adding child in p's children list
        std::list<NodeId>& children = __data[&p]->children;

        if ( children.empty() ) {
          children.push_back ( node );
        } else {
          size_t size = children.size();

          for ( std::list<NodeId>::iterator iter = children.begin(); iter != children.end(); ++iter ) {
            if ( child->code() < pattern ( *iter ).code() ) {
              children.insert ( iter, node );
              break;
            }
          }

          if ( size == children.size() ) {
            children.push_back ( node );
          }
        }
      }

      template<typename GUM_SCALAR>
      void
      DFSTree<GUM_SCALAR>::__checkGrowth ( Pattern& p, Pattern* child, EdgeGrowth& edge_growth ) {
        NodeId v = edge_growth.v;

        // First we check if the edge is legal
        if ( v == 0 ) {
          v = child->insertNode ( * ( edge_growth.l_v ) );
        }

        child->insertArc ( edge_growth.u, v, * ( edge_growth.edge ) );
        // Neighborhood restriction is checked by the Pattern class
        EdgeCode& edge = child->edgeCode ( edge_growth.u, v );

        // Then we check if the edge we added is valid
        if ( edge < * ( child->code().codes.front() ) ) {
          GUM_ERROR ( OperationNotAllowed, "added edge code is lesser than the first one in the pattern's DFSCode" );
        }

        if ( edge.isBackward() ) {
          typedef std::vector<EdgeCode*>::iterator EdgeIter;

          for ( EdgeIter iter = child->code().codes.begin(); ( iter + 1 ) != child->code().codes.end(); ++iter ) {
            if ( ( ( ( **iter ).i == v ) or ( ( **iter ).j == v ) ) and edge < ( **iter ) ) {
              GUM_ERROR ( OperationNotAllowed, "added backward edge is lesser than an existing edge on v" );
            }
          }
        }

        // Finally we check if child is minimal.
        if ( not child->isMinimal() ) {
          GUM_ERROR ( OperationNotAllowed, "the DFSCode for this growth is not minimal" );
        }
      }

      template<typename GUM_SCALAR>
      Pattern&
      DFSTree<GUM_SCALAR>::growPattern ( Pattern& p, EdgeGrowth& edge_growth, Size min_freq ) {
        Pattern* child = new Pattern ( p );

        try {
          __checkGrowth ( p, child, edge_growth );
        } catch ( OperationNotAllowed& e ) {
          delete child;
          throw ;
        }

        // Now we need to build the pattern data about child
        DFSTree<GUM_SCALAR>::PatternData* data = new DFSTree<GUM_SCALAR>::PatternData ( child );
        std::vector<NodeId> degree_list;
        NodeProperty<Sequence<Instance<GUM_SCALAR>*>*>& p_iso_map = __data[&p]->iso_map;
        typename Property< std::pair<Instance<GUM_SCALAR>*, Instance<GUM_SCALAR>*> >::onNodes::iterator match;
        // Using p information to build child's isomorphism graph
        NodeId id = 0;

        for ( auto seq = p_iso_map.begin(); seq != p_iso_map.end(); ++seq ) {
          auto match = edge_growth.matches.begin();

          for ( ; match != edge_growth.matches.end(); ++match ) {
            // Adding the isomorphism in the iso_graph and building the iso_map.
            if ( child->code().codes.back()->isForward() ) {
              if ( ( *seq )->exists ( match->first ) and ( not ( *seq )->exists ( match->second ) ) ) {
                // Let's see if the new match is already matched
                Sequence<Instance<GUM_SCALAR>*>* new_seq = new Sequence<Instance<GUM_SCALAR>*> ( **seq );
                new_seq->insert ( match->second );

                if ( __is_new_seq ( *new_seq, data->iso_map ) ) {
                  id = data->iso_graph.insertNode();
                  data->iso_map.insert ( id, new_seq );
                } else {
                  delete new_seq;
                }

                break;
              }
            } else {
              if ( ( *seq )->exists ( match->first ) and ( *seq )->exists ( match->second ) ) {
                Sequence<Instance<GUM_SCALAR>*>* new_seq = new Sequence<Instance<GUM_SCALAR>*> ( **seq );

                if ( __is_new_seq ( *new_seq, data->iso_map ) ) {
                  id = data->iso_graph.insertNode();
                  data->iso_map.insert ( id, new_seq );
                } else {
                  delete new_seq;
                }

                break;
              }
            }
          }

          if ( match != edge_growth.matches.end() ) {
            // Adding edges in the iso_graph
            for ( const auto node : data->iso_graph.nodes() ) {
              if ( ( node ) != id ) {
                for ( const auto m : * ( data->iso_map[id] ) ) {
                  if ( data->iso_map[node]->exists ( m ) ) {
                    data->iso_graph.insertEdge ( node, id );
                    break;
                  }
                }
              }
            }

            degree_list.push_back ( id );
            edge_growth.matches.erase ( match.key() );
          }
        }

        if ( data->iso_graph.size() < min_freq ) {
          delete data;
          delete child;
          GUM_ERROR ( OperationNotAllowed, "child is not frequent enough" );
        }

        // Now we can compute the maximal independent set of child
        DFSTree<GUM_SCALAR>::NeighborDegreeSort my_operator ( data->iso_graph );
        std::sort ( degree_list.begin(), degree_list.end(), my_operator );
        Set<NodeId> removed;

        for ( std::vector<NodeId>::iterator node = degree_list.begin(); node != degree_list.end(); ++node ) {
          if ( not removed.exists ( *node ) ) {
            removed.insert ( *node );
            const NodeSet& neighbours =  data->iso_graph.neighbours ( *node );

            for ( NodeSet::const_iterator neighbor = neighbours.begin(); neighbor != neighbours.end(); ++neighbor )
              removed.insert ( *neighbor );

            data->max_indep_set.insert ( *node );
          }
        }

        __data.insert ( child, data );

        if ( not __strategy->accept_growth ( &p, child, edge_growth ) ) {
          __data.erase ( child );
          delete data;
          delete child;
          GUM_ERROR ( OperationNotAllowed, "child is not frequent enough" );
        }

        __addChild ( p, child, edge_growth );
        // Uncomment this if you want to debug it
        // for (UndiGraph::NodeIterator node = data->iso_graph.beginNodes(); node != data->iso_graph.endNodes(); ++node) {
        //   __find_sub_pattern(*child, *node);
        // }
        return *child;
      }

      template<typename GUM_SCALAR>
      void
      DFSTree<GUM_SCALAR>::EdgeGrowth::insert ( Instance<GUM_SCALAR>* u, Instance<GUM_SCALAR>* v ) {
        NodeId id = iso_graph.insertNode();
        degree_list->push_back ( id );

        for ( auto iter = matches.begin(); iter != matches.end(); ++iter ) {
          if ( ( iter->first == u ) or ( iter->second == u ) or
               ( iter->first == v ) or ( iter->second == v ) ) {
            iso_graph.insertEdge ( iter.key(), id );
          }
        }

        // The order between u and v is important ! DO NOT INVERSE IT !
        matches.insert ( id, std::make_pair ( u, v ) );
      }

      template<typename GUM_SCALAR>
      void
      DFSTree<GUM_SCALAR>::__find_sub_pattern ( Pattern& p, NodeId iso_map ) {
        // PatternData& data = *(__data[&p]);
        // Sequence<Instance<GUM_SCALAR>*>& seq = *(data.iso_map[iso_map]);
        // HashTable<ClassElement<GUM_SCALAR>*, Size>* elt_map = new HashTable<ClassElement<GUM_SCALAR>*, Size>();
        // ClassElement<GUM_SCALAR>* elt = 0;
        // for (Sequence<Instance<GUM_SCALAR>*>::iterator iter = seq.begin(); iter != seq.end(); ++iter) {
        //   for (Set<SlotChain*>::iterator sc = (*iter)->slotChains().begin();
        //        sc != (*iter)->slotChains().end(); ++sc) {
        //     elt = const_cast<ClassElement<GUM_SCALAR>*>(&((*sc)->lastElt()));
        //     if (not elt_map->exists(elt)) {
        //       elt_map->insert(elt, 0);
        //     }
        //     for (Set<Instance<GUM_SCALAR>*>::iterator i = (*iter)->getInstances((*sc)->id()).begin();
        //          i != (*iter)->getInstances((*sc)->id()).end(); ++i) {
        //       if (not seq.exists(*i)) {
        //         ++((*elt_map)[elt]);
        //       }
        //     }
        //     if ((*elt_map)[elt] == 0) {
        //       elt_map->erase(elt);
        //     }
        //   }
        // }
        // typedef Sequence< HashTable<ClassElement<GUM_SCALAR>*, Size>* >::iterator SubPatIter;
        // for (SubPatIter iter = data.sub_patterns.begin(); iter != data.sub_patterns.end(); ++iter) {
        //   if (__test_equality(**iter, *elt_map)) {
        //     data.sub_patterns_map.insert(iso_map, data.sub_patterns.pos(*iter));
        //     ++(data.sub_patterns_count[data.sub_patterns.pos(*iter)]);
        //     break;
        //   }
        // }
        // if (data.sub_patterns_map.exists(iso_map)) {
        //   delete elt_map;
        // } else {
        //   data.sub_patterns_map.insert(iso_map, data.sub_patterns.size());
        //   data.sub_patterns.insert(elt_map);
        //   data.sub_patterns_count.push_back((Size) 1);
        // }
      }

      template<typename GUM_SCALAR>
      bool
      DFSTree<GUM_SCALAR>::__test_equality ( HashTable<ClassElement<GUM_SCALAR>*, Size>& x, HashTable<ClassElement<GUM_SCALAR>*, Size>& y ) {
        for ( auto iter = x.begin(); iter != x.end(); ++iter ) {
          try {
            if ( y[iter.key()] != ( *iter ) )
              return false;
          } catch ( NotFound& ) {
            return false;
          }
        }

        return true;
      }


// PatternData


      template<typename GUM_SCALAR>
      DFSTree<GUM_SCALAR>::PatternData::PatternData ( const PatternData& from ) :
        pattern ( from.pattern ), children ( from.children ), iso_graph ( from.iso_graph ),
        max_indep_set ( from.max_indep_set ), cost ( from.cost ), gain ( from.gain ) {
        GUM_CONS_CPY ( DFSTree<GUM_SCALAR>::PatternData );

        for ( auto iter = from.iso_map.begin(); iter != from.iso_map.end(); ++iter ) {
          iso_map.insert ( iter.key(), new Sequence<Instance<GUM_SCALAR>*> ( **iter ) );
        }
      }

      template<typename GUM_SCALAR>
      DFSTree<GUM_SCALAR>::PatternData::~PatternData() {
        GUM_DESTRUCTOR ( DFSTree<GUM_SCALAR>::PatternData );

        for ( auto iter = iso_map.begin(); iter != iso_map.end(); ++iter ) {
          delete *iter;
        }

        // for (auto iter = sub_patterns.begin(); iter != sub_patterns.end(); ++iter) {
        //   delete *iter;
        // }
      }


      template<typename GUM_SCALAR> INLINE
      DFSTree<GUM_SCALAR>::DFSTree ( const InterfaceGraph<GUM_SCALAR>& graph, gspan::SearchStrategy<GUM_SCALAR>* strategy ) :
        __graph ( &graph ), __strategy ( strategy ) {
        GUM_CONSTRUCTOR ( DFSTree );

        if ( not __strategy )
          __strategy = new FrequenceSearch<GUM_SCALAR> ( 2 );

        __strategy->setTree ( this );
      }

      template<typename GUM_SCALAR> INLINE
      std::list<NodeId>&
      DFSTree<GUM_SCALAR>::roots() { return __roots; }

      template<typename GUM_SCALAR> INLINE
      const std::list<NodeId>&
      DFSTree<GUM_SCALAR>::roots() const { return __roots; }

      template<typename GUM_SCALAR> INLINE
      Pattern&
      DFSTree<GUM_SCALAR>::parent ( const Pattern& p ) {
        try {
          return * ( __node_map.second ( * ( DiGraph::parents ( __node_map.first ( const_cast<Pattern*> ( &p ) ) ).begin() ) ) );
        } catch ( NotFound& ) {
          if ( __node_map.existsSecond ( const_cast<Pattern*> ( &p ) ) ) {
            GUM_ERROR ( NotFound, "the given pattern is a root node" );
          } else {
            GUM_ERROR ( NotFound, "pattern not found in this DFSTree" );
          }
        }
      }

      template<typename GUM_SCALAR> INLINE
      const Pattern&
      DFSTree<GUM_SCALAR>::parent ( const Pattern& p ) const {
        try {
          return * ( __node_map.second ( * ( DiGraph::parents ( __node_map.first ( const_cast<Pattern*> ( &p ) ) ).begin() ) ) );
        } catch ( NotFound& ) {
          if ( __node_map.existsSecond ( const_cast<Pattern*> ( &p ) ) ) {
            GUM_ERROR ( NotFound, "the given pattern is a root node" );
          } else {
            GUM_ERROR ( NotFound, "pattern not found in this DFSTree" );
          }
        }
      }

      template<typename GUM_SCALAR> INLINE
      std::list<NodeId>&
      DFSTree<GUM_SCALAR>::children ( const Pattern& p ) {
        try {
          return __data[const_cast<Pattern*> ( &p )]->children;
        } catch ( NotFound& ) {
          GUM_ERROR ( NotFound, "pattern not found in this DFSTree" );
        }
      }

      template<typename GUM_SCALAR> INLINE
      const std::list<NodeId>&
      DFSTree<GUM_SCALAR>::children ( const Pattern& p ) const {
        try {
          return __data[const_cast<Pattern*> ( &p )]->children;
        } catch ( NotFound& ) {
          GUM_ERROR ( NotFound, "pattern not found in this DFSTree" );
        }
      }

      template<typename GUM_SCALAR> INLINE
      Pattern&
      DFSTree<GUM_SCALAR>::pattern ( NodeId id ) {
        try {
          return * ( __node_map.second ( id ) );
        } catch ( NotFound& ) {
          GUM_ERROR ( NotFound, "no pattern matching the given id" );
        }
      }

      template<typename GUM_SCALAR> INLINE
      const Pattern&
      DFSTree<GUM_SCALAR>::pattern ( NodeId id ) const {
        try {
          return * ( __node_map.second ( id ) );
        } catch ( NotFound& ) {
          GUM_ERROR ( NotFound, "no pattern matching the given id" );
        }
      }

      template<typename GUM_SCALAR> INLINE
      UndiGraph&
      DFSTree<GUM_SCALAR>::iso_graph ( const Pattern& p ) {
        try {
          return __data[const_cast<Pattern*> ( &p )]->iso_graph;
        } catch ( NotFound& ) {
          GUM_ERROR ( NotFound, "pattern not found in this DFSTree" );
        }
      }

      template<typename GUM_SCALAR> INLINE
      Sequence<Instance<GUM_SCALAR>*>&
      DFSTree<GUM_SCALAR>::iso_map ( const Pattern& p, NodeId node ) {
        try {
          return * ( __data[const_cast<Pattern*> ( &p )]->iso_map[node] );
        } catch ( NotFound& ) {
          if ( __data.exists ( const_cast<Pattern*> ( &p ) ) ) {
            GUM_ERROR ( NotFound, "node not found in Pattern's isomorphism graph" );
          } else {
            GUM_ERROR ( NotFound, "pattern not found in this DFSTree" );
          }
        }
      }

      template<typename GUM_SCALAR> INLINE
      Set<NodeId>&
      DFSTree<GUM_SCALAR>::max_indep_set ( const Pattern& p ) {
        try {
          return __data[const_cast<Pattern*> ( &p )]->max_indep_set;
        } catch ( NotFound& ) {
          GUM_ERROR ( NotFound, "pattern not found in this DFSTree" );
        }
      }

      template<typename GUM_SCALAR> INLINE
      const InterfaceGraph<GUM_SCALAR>&
      DFSTree<GUM_SCALAR>::graph() const {
        return *__graph;
      }

      template<typename GUM_SCALAR> INLINE
      std::ostream&
      operator<< ( std::ostream& out, const typename DFSTree<GUM_SCALAR>::EdgeGrowth& edge ) {
        out << edge.u << ", " << * ( edge.edge ) << ", " << * ( edge.l_v ) << ", " << edge.v;
        return out;
      }

// template<typename GUM_SCALAR> INLINE
// Sequence< HashTable<ClassElement<GUM_SCALAR>*, Size>* >&
// DFSTree<GUM_SCALAR>::sub_patterns(const Pattern& p) {
//   return __data[const_cast<Pattern*>(&p)]->sub_patterns;
// }
//
// template<typename GUM_SCALAR> INLINE
// HashTable<NodeId, Idx>&
// DFSTree<GUM_SCALAR>::sub_patterns_map(const Pattern& p) {
//   return __data[const_cast<Pattern*>(&p)]->sub_patterns_map;
// }


// EdgeGrowth


      template<typename GUM_SCALAR> INLINE
      DFSTree<GUM_SCALAR>::EdgeGrowth::EdgeGrowth ( NodeId a_u, LabelData* an_edge,
          LabelData* a_l_v, NodeId a_v ) :
        u ( a_u ), edge ( an_edge ), l_v ( a_l_v ), v ( a_v ),
        degree_list ( new std::vector<NodeId>() ) {
        GUM_CONSTRUCTOR ( DFSTree<GUM_SCALAR>::EdgeGrowth );
      }

      template<typename GUM_SCALAR> INLINE
      DFSTree<GUM_SCALAR>::EdgeGrowth::EdgeGrowth ( const EdgeGrowth& from ) :
        u ( from.u ), edge ( from.edge ), v ( from.v ), matches ( from.matches ),
        iso_graph ( from.iso_graph ), degree_list ( 0 ), max_indep_set ( from.max_indep_set ) {
        GUM_CONS_CPY ( DFSTree<GUM_SCALAR>::EdgeGrowth );

        if ( from.degree_list != 0 ) {
          degree_list = new std::vector<NodeId> ( * ( from.degree_list ) );
        }
      }

      template<typename GUM_SCALAR> INLINE
      DFSTree<GUM_SCALAR>::EdgeGrowth::~EdgeGrowth() {
        GUM_DESTRUCTOR ( DFSTree<GUM_SCALAR>::EdgeGrowth );

        if ( degree_list != 0 ) {
          delete degree_list;
        }
      }

      template<typename GUM_SCALAR> INLINE
      std::string
      DFSTree<GUM_SCALAR>::EdgeGrowth::toString() {
        std::stringstream str;
        str << u << "-" << edge << "-" << l_v << "-" << v;
        return str.str();
      }

//template<typename GUM_SCALAR> INLINE
//Size
//DFSTree<GUM_SCALAR>::EdgeGrowth::count() const {
//  return matches.size();
//}

      template<typename GUM_SCALAR> INLINE
      double
      DFSTree<GUM_SCALAR>::frequency ( const Pattern& p ) const {
        return ( double ) __data[const_cast<Pattern*> ( &p )]->max_indep_set.size();
      }

      template<typename GUM_SCALAR> INLINE
      typename DFSTree<GUM_SCALAR>::PatternData&
      DFSTree<GUM_SCALAR>::data ( const Pattern& p ) {
        return * ( __data[const_cast<Pattern*> ( &p )] );
      }

      template<typename GUM_SCALAR> INLINE
      const typename DFSTree<GUM_SCALAR>::PatternData&
      DFSTree<GUM_SCALAR>::data ( const Pattern& p ) const {
        return * ( __data[const_cast<Pattern*> ( &p )] );
      }

      template<typename GUM_SCALAR> INLINE
      SearchStrategy<GUM_SCALAR>&
      DFSTree<GUM_SCALAR>::strategy() { return *__strategy; }

      template<typename GUM_SCALAR> INLINE
      const SearchStrategy<GUM_SCALAR>&
      DFSTree<GUM_SCALAR>::strategy() const { return *__strategy; }


// NeighborDegreeSort


      template<typename GUM_SCALAR> INLINE
      DFSTree<GUM_SCALAR>::NeighborDegreeSort::NeighborDegreeSort ( UndiGraph& graph ) :
        g ( graph ) {
        GUM_CONSTRUCTOR ( DFSTree<GUM_SCALAR>::NeighborDegreeSort );
      }

      template<typename GUM_SCALAR> INLINE
      DFSTree<GUM_SCALAR>::NeighborDegreeSort::NeighborDegreeSort ( const NeighborDegreeSort& source ) :
        g ( source.g ) {
        GUM_CONS_CPY ( DFSTree<GUM_SCALAR>::NeighborDegreeSort );
      }

      template<typename GUM_SCALAR> INLINE
      DFSTree<GUM_SCALAR>::NeighborDegreeSort::~NeighborDegreeSort() {
        GUM_DESTRUCTOR ( DFSTree<GUM_SCALAR>::NeighborDegreeSort );
      }

      template<typename GUM_SCALAR> INLINE
      bool
      DFSTree<GUM_SCALAR>::NeighborDegreeSort::operator() ( NodeId i, NodeId j ) {
        return g.neighbours ( i ).size() < g.neighbours ( j ).size();
      }


// PatternData


      template<typename GUM_SCALAR> INLINE
      DFSTree<GUM_SCALAR>::PatternData::PatternData ( Pattern* p ) :
        pattern ( p ), cost ( 0 ), gain ( 0 ) {
        GUM_CONSTRUCTOR ( DFSTree<GUM_SCALAR>::PatternData );
      }

    } /* namespace gspan */
  } /* namespace prm */
} /* namespace gum */

