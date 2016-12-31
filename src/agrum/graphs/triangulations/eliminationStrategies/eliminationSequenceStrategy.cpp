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
 * @brief implementation of the base class for all elimination sequence
 *algorithms
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/config.h>
#include <agrum/graphs/triangulations/eliminationStrategies/eliminationSequenceStrategy.h>

#ifdef GUM_NO_INLINE
#include <agrum/graphs/triangulations/eliminationStrategies/eliminationSequenceStrategy_inl.h>
#endif  // GUM_NOINLINE

namespace gum {

  // an empty fill-ins set returned by default when we ask for a fill-ins set
  const EdgeSet& EliminationSequenceStrategy::__empty_fill_ins() {
#ifndef NDEBUG
    static bool first_use = true;
    if ( first_use ) {
      first_use = false;
      __debug__::__dec_creation(
          "Set", "__empty_edge_set", 0, "static variable correction", 0 );
      __debug__::__dec_creation(
          "HashTable", "__empty_edge_set", 0, "static variable correction", 0 );
    }
#endif
    static EdgeSet empty_fill_ins;
    return empty_fill_ins;
  }

  // default constructor
  EliminationSequenceStrategy::EliminationSequenceStrategy() {
    // for debugging purposes
    GUM_CONSTRUCTOR( EliminationSequenceStrategy );
  }

  // constructor for an a priori non empty graph
  EliminationSequenceStrategy::EliminationSequenceStrategy(
      UndiGraph* graph, const NodeProperty<Size>* domain_sizes ) {
    EliminationSequenceStrategy::setGraph( graph, domain_sizes );

    // for debugging purposes
    GUM_CONSTRUCTOR( EliminationSequenceStrategy );
  }

  // copy constructor
  EliminationSequenceStrategy::EliminationSequenceStrategy(
      const EliminationSequenceStrategy& from )
      : _graph( from._graph )
      , _domain_sizes( from._domain_sizes )
      , _log_domain_sizes( from._log_domain_sizes ) {
    // for debugging purposes
    GUM_CONS_CPY( EliminationSequenceStrategy );
  }

  /// move constructor
  EliminationSequenceStrategy::EliminationSequenceStrategy(
      EliminationSequenceStrategy&& from )
      : _graph( from._graph )
      , _domain_sizes( from._domain_sizes )
      , _log_domain_sizes( std::move( from._log_domain_sizes ) ) {
    // for debugging purposes
    GUM_CONS_MOV( EliminationSequenceStrategy );
  }

  // destructor
  EliminationSequenceStrategy::~EliminationSequenceStrategy() {
    // for debugging purposes
    GUM_DESTRUCTOR( EliminationSequenceStrategy );
  }

  // performs all the graph/fill-ins updates provided
  void EliminationSequenceStrategy::eliminationUpdate( const NodeId node ) {}

  /** @brief in case fill-ins are provided, this function returns the fill-ins
   * due to all the nodes eliminated so far */
  const EdgeSet& EliminationSequenceStrategy::fillIns() {
    return __empty_fill_ins();
  }

  // clears the sequence (to prepare, for instance, a new elimination sequence)
  void EliminationSequenceStrategy::clear() {
    _graph = nullptr;
    _domain_sizes = nullptr;
    _log_domain_sizes.clear();
  }

  // sets a new graph to be triangulated
  bool
  EliminationSequenceStrategy::setGraph( UndiGraph*                graph,
                                         const NodeProperty<Size>* dom_sizes ) {
    // check that both the graph and the domain sizes are different from nullptr
    // or else that both are equal to nullptr
    if ( ( ( graph != nullptr ) && ( dom_sizes == nullptr ) ) ||
         ( ( graph == nullptr ) && ( dom_sizes != nullptr ) ) ) {
      GUM_ERROR( GraphError,
                 "EliminationSequenceStrategy: one of the graph or the set of "
                 "domain sizes is a null pointer." );
    }

    // check that each node has a domain size
    if ( graph != nullptr ) {
      for ( const auto node : *graph )
        if ( !dom_sizes->exists( node ) )
          GUM_ERROR( GraphError,
                     "EliminationSequenceStrategy needs a domain size "
                     "for every node in the graph." );
    }

    // avoid empty modifications
    if ( ( graph != _graph ) || ( dom_sizes != _domain_sizes ) ) {
      // remove, if any, the current graph
      clear();

      // assign a new graph
      _graph = graph;
      _domain_sizes = dom_sizes;

      if ( _graph != nullptr ) {
        // compute the log of the modalities
        _log_domain_sizes.resize( _graph->sizeNodes() / 2 );

        for ( const auto node : *_graph )
          _log_domain_sizes.insert( node, std::log( ( *_domain_sizes )[node] ) );
      }

      return true;
    }

    return false;
  }

} /* namespace gum */
