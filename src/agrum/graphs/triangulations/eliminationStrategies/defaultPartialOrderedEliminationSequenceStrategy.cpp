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
 * @brief An efficient unconstrained elimination sequence algorithm
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <cmath>

#include <agrum/config.h>

#include <agrum/graphs/triangulations/eliminationStrategies/defaultPartialOrderedEliminationSequenceStrategy.h>
#include <agrum/graphs/undiGraph.h>

namespace gum {

  /// default constructor (uses an empty graph)
  DefaultPartialOrderedEliminationSequenceStrategy::
      DefaultPartialOrderedEliminationSequenceStrategy( double theRatio,
                                                        double theThreshold )
      : __simplicial_ratio( theRatio )
      , __simplicial_threshold( theThreshold ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( DefaultPartialOrderedEliminationSequenceStrategy );
  }

  /// constructor for an a priori non empty graph
  DefaultPartialOrderedEliminationSequenceStrategy::
      DefaultPartialOrderedEliminationSequenceStrategy(
          UndiGraph*                graph,
          const NodeProperty<Size>* dom_sizes,
          const List<NodeSet>*      subsets,
          double                    ratio,
          double                    threshold )
      : __simplicial_ratio( ratio )
      , __simplicial_threshold( threshold ) {
    setGraph( graph, dom_sizes );
    setPartialOrder( subsets );

    // for debugging purposes
    GUM_CONSTRUCTOR( DefaultPartialOrderedEliminationSequenceStrategy );
  }

  /// copy constructor
  DefaultPartialOrderedEliminationSequenceStrategy::
      DefaultPartialOrderedEliminationSequenceStrategy(
          const DefaultPartialOrderedEliminationSequenceStrategy& from )
      : PartialOrderedEliminationSequenceStrategy( from )
      ,
      // no need to set __log_weights because the copy of the simplicial set
      // will set it properly
      __simplicial_set( new SimplicialSet( *from.__simplicial_set,
                                           _graph,
                                           &_log_domain_sizes,
                                           &__log_weights,
                                           false ) )
      , __simplicial_ratio( from.__simplicial_ratio )
      , __simplicial_threshold( from.__simplicial_threshold )
      , __provide_fill_ins( from.__provide_fill_ins ) {
    // for debugging purposes
    GUM_CONS_CPY( DefaultPartialOrderedEliminationSequenceStrategy );
  }

  /// move constructor
  DefaultPartialOrderedEliminationSequenceStrategy::
      DefaultPartialOrderedEliminationSequenceStrategy(
          DefaultPartialOrderedEliminationSequenceStrategy&& from )
      : PartialOrderedEliminationSequenceStrategy( std::move( from ) )
      , __log_weights( std::move( from.__log_weights ) )
      , __simplicial_set( from.__simplicial_set )
      , __simplicial_ratio( from.__simplicial_ratio )
      , __simplicial_threshold( from.__simplicial_threshold )
      , __provide_fill_ins( from.__provide_fill_ins ) {
    __simplicial_set->replaceLogWeights( &from.__log_weights, &__log_weights );
    from.__simplicial_set = nullptr;

    // for debugging purposes
    GUM_CONS_MOV( DefaultPartialOrderedEliminationSequenceStrategy );
  }

  /// destructor
  DefaultPartialOrderedEliminationSequenceStrategy::
      ~DefaultPartialOrderedEliminationSequenceStrategy() {
    // for debugging purposes
    GUM_DESTRUCTOR( DefaultPartialOrderedEliminationSequenceStrategy );

    if ( __simplicial_set ) delete __simplicial_set;
  }

  /// create a new simplicial set suited for the current graph
  void DefaultPartialOrderedEliminationSequenceStrategy::__createSimplicialSet() {
    // remove the old simplicial set, if any
    if ( __simplicial_set != nullptr ) {
      delete __simplicial_set;
      __simplicial_set = nullptr;
    }

    if ( _graph != nullptr ) {
      // create a simplicial set suited for the graph
      __simplicial_set = new SimplicialSet( _graph,
                                            &_log_domain_sizes,
                                            &__log_weights,
                                            __simplicial_ratio,
                                            __simplicial_threshold );

      __simplicial_set->setFillIns( __provide_fill_ins );
    }
  }

  /// sets a new graph to be triangulated
  bool DefaultPartialOrderedEliminationSequenceStrategy::setGraph(
      UndiGraph* graph, const NodeProperty<Size>* domain_sizes ) {
    if ( PartialOrderedEliminationSequenceStrategy::setGraph( graph,
                                                              domain_sizes ) ) {
      __createSimplicialSet();
      return true;
    }

    return false;
  }

  /// clears the sequence (to prepare, for instance, a new elimination sequence)
  void DefaultPartialOrderedEliminationSequenceStrategy::clear() {
    PartialOrderedEliminationSequenceStrategy::clear();
    __log_weights.clear();

    if ( __simplicial_set != nullptr ) {
      delete __simplicial_set;
      __simplicial_set = nullptr;
    }
  }

  /// returns the best possible node to be eliminated
  NodeId DefaultPartialOrderedEliminationSequenceStrategy::__nodeToEliminate(
      const PriorityQueue<NodeId, double>& possibleNodes ) {
    bool   found = false;
    double min_score = 0;
    NodeId best_node = 0;

    for ( const auto node : _nodeset ) {
      try {
        double score = possibleNodes.priority( node );

        if ( !found || ( score < min_score ) ) {
          found = true;
          min_score = score;
          best_node = node;
        }
      } catch ( NotFound& ) {
      }
    }

    if ( !found ) {
      GUM_ERROR( NotFound, "no possible node to eliminate" );
    }

    return best_node;
  }

  /// returns the new node to be eliminated within the triangulation algorithm
  NodeId DefaultPartialOrderedEliminationSequenceStrategy::nextNodeToEliminate() {
    // if there is no simplicial set, send an exception
    if ( _graph == nullptr ) GUM_ERROR( NotFound, "the graph is empty" );

    if ( _partial_order_needed )
      GUM_ERROR( NotFound,
                 "the partial order does not cover all the nodes "
                 "of the graph" );

    if ( _nodeset.empty() ) {
      GUM_ERROR( NotFound, "no node is admissible" );
    }

    // select a node to be eliminated: try simplicial nodes, then almost
    // simplicial nodes, then quasi-simplicial nodes
    // note that if _graph != nullptr, __simplicial_set has been allocated
    try {
      return __nodeToEliminate( __simplicial_set->allSimplicialNodes() );
    } catch ( NotFound& ) {
    }

    try {
      return __nodeToEliminate( __simplicial_set->allAlmostSimplicialNodes() );
    } catch ( NotFound& ) {
    }

    try {
      return __nodeToEliminate( __simplicial_set->allQuasiSimplicialNodes() );
    } catch ( NotFound& ) {
    }

    // here: select the node through Kjaerulff's heuristic
    auto   iter = _nodeset.cbegin();
    double min_score = __log_weights[*iter];
    NodeId best_node = *iter;

    for ( ++iter; iter != _nodeset.cend(); ++iter ) {
      double score = __log_weights[*iter];

      if ( score < min_score ) {
        min_score = score;
        best_node = *iter;
      }
    }

    return best_node;
  }

  /** @brief if the elimination sequence is able to compute fill-ins, we
   * indicatewhether we want this feature to be activated */
  void DefaultPartialOrderedEliminationSequenceStrategy::askFillIns( bool do_it ) {
    __provide_fill_ins = do_it;

    if ( __simplicial_set ) __simplicial_set->setFillIns( __provide_fill_ins );
  }

  /** @brief indicates whether the new fill-ins generated by a new eliminated
   * node, if needed, will be computed by the elimination sequence, or need be
   * computed by the triangulation itself. */
  bool DefaultPartialOrderedEliminationSequenceStrategy::providesFillIns() const {
    return __provide_fill_ins;
  }

  /** @brief indicates whether the elimination sequence updates by itself the
   * graph after a node has been eliminated */
  bool
  DefaultPartialOrderedEliminationSequenceStrategy::providesGraphUpdate() const {
    return true;
  }

  /// performs all the graph/fill-ins updates provided
  void DefaultPartialOrderedEliminationSequenceStrategy::eliminationUpdate(
      const NodeId id ) {
    // check whether we can do something
    if ( __simplicial_set != nullptr ) {
      __simplicial_set->makeClique( id );
      __simplicial_set->eraseClique( id );

      if ( !_partial_order_needed ) {
        // remove the node from _nodeset
        _nodeset.erase( id );

        if ( _nodeset.empty() ) {
          // go to the next non-empty subset
          for ( ++_subset_iter; _subset_iter != _subsets->cend();
                ++_subset_iter ) {
            for ( const auto node : *_subset_iter ) {
              if ( _graph->existsNode( node ) ) {
                _nodeset.insert( node );
              }
            }
            if ( !_nodeset.empty() ) break;
          }
        }
      }
    }
  }

  /** @brief in case fill-ins are provided, this function returns the fill-ins
   * generated after the last node elimination */
  const EdgeSet& DefaultPartialOrderedEliminationSequenceStrategy::fillIns() {
    if ( !__provide_fill_ins || ( __simplicial_set == nullptr ) )
      return EliminationSequenceStrategy::fillIns();
    else
      return __simplicial_set->fillIns();
  }

  /** @brief creates a new elimination sequence of the same type as the current
   * object, but this sequence contains only an empty graph */
  DefaultPartialOrderedEliminationSequenceStrategy*
  DefaultPartialOrderedEliminationSequenceStrategy::newFactory() const {
    return new DefaultPartialOrderedEliminationSequenceStrategy(
        __simplicial_ratio, __simplicial_threshold );
  }

  /// virtual copy constructor
  DefaultPartialOrderedEliminationSequenceStrategy*
  DefaultPartialOrderedEliminationSequenceStrategy::copyFactory() const {
    return new DefaultPartialOrderedEliminationSequenceStrategy( *this );
  }

} /* namespace gum */
