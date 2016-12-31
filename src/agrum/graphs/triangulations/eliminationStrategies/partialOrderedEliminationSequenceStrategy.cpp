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
 * @brief Base class for all elimination sequence algorithm that impose a given
 * partial ordering on the nodes elimination sequence, that is, the set of all
 * the nodes is divided into several subsets. Within each subset, any ordering
 * can be chosen. But all the nodes of the first subset must be eliminated
 * before the nodes of the second, which must be eliminated before those of the
 * third subset, and so on.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/graphs/triangulations/eliminationStrategies/partialOrderedEliminationSequenceStrategy.h>

#ifdef GUM_NO_INLINE
#include <agrum/graphs/triangulations/eliminationStrategies/partialOrderedEliminationSequenceStrategy_inl.h>
#endif  // GUM_NOINLINE

namespace gum {

  /// default constructor
  PartialOrderedEliminationSequenceStrategy::
      PartialOrderedEliminationSequenceStrategy() {
    // for debugging purposes
    GUM_CONSTRUCTOR( PartialOrderedEliminationSequenceStrategy );
  }

  /// constructor for an a priori non empty graph
  PartialOrderedEliminationSequenceStrategy::
      PartialOrderedEliminationSequenceStrategy(
          UndiGraph*                graph,
          const NodeProperty<Size>* dom_sizes,
          const List<NodeSet>*      subsets ) {
    setGraph( graph, dom_sizes );
    setPartialOrder( subsets );

    // for debugging purposes
    GUM_CONSTRUCTOR( PartialOrderedEliminationSequenceStrategy );
  }

  /// copy constructor
  PartialOrderedEliminationSequenceStrategy::
      PartialOrderedEliminationSequenceStrategy(
          const PartialOrderedEliminationSequenceStrategy& from )
      : EliminationSequenceStrategy( from )
      , _subsets( from._subsets )
      , _subset_iter( from._subset_iter )
      , _nodeset( from._nodeset )
      , _partial_order_needed( from._partial_order_needed ) {
    // for debugging purposes
    GUM_CONS_CPY( PartialOrderedEliminationSequenceStrategy );
  }

  /// move constructor
  PartialOrderedEliminationSequenceStrategy::
      PartialOrderedEliminationSequenceStrategy(
          PartialOrderedEliminationSequenceStrategy&& from )
      : EliminationSequenceStrategy( std::move( from ) )
      , _subsets( from._subsets )
      , _subset_iter( from._subset_iter )
      , _nodeset( std::move( from._nodeset ) )
      , _partial_order_needed( from._partial_order_needed ) {
    from._partial_order_needed = true;

    // for debugging purposes
    GUM_CONS_MOV( PartialOrderedEliminationSequenceStrategy );
  }

  /// destructor
  PartialOrderedEliminationSequenceStrategy::
      ~PartialOrderedEliminationSequenceStrategy() {
    // for debugging purposes
    GUM_DESTRUCTOR( PartialOrderedEliminationSequenceStrategy );
  }

  /// sets a new graph to be triangulated
  bool PartialOrderedEliminationSequenceStrategy::setGraph(
      UndiGraph* graph, const NodeProperty<Size>* domain_sizes ) {
    if ( EliminationSequenceStrategy::setGraph( graph, domain_sizes ) ) {
      setPartialOrder( _subsets );
      return true;
    }
    return false;
  }

  /// indicate whether a partial ordering is compatible with the current graph
  bool PartialOrderedEliminationSequenceStrategy::_isPartialOrderNeeded(
      const List<NodeSet>* subsets ) const {
    if ( ( _graph == nullptr ) || ( subsets == nullptr ) ) return true;

    // determine the set of nodes in the subsets that belong to the graph
    NodeSet nodes_found( _graph->size() / 2 );
    for ( const auto& nodes : *subsets ) {
      for ( const auto node : nodes ) {
        if ( _graph->existsNode( node ) ) {
          nodes_found.insert( node );
        }
      }
    }

    // check that the size of nodes_found is equal to that of the graph
    return nodes_found.size() != _graph->size();
  }

  /// sets a new partial order
  bool PartialOrderedEliminationSequenceStrategy::setPartialOrder(
      const List<NodeSet>* subsets ) {
    // check that the partial order contains all the nodes of the graph
    _partial_order_needed = _isPartialOrderNeeded( subsets );

    if ( !_partial_order_needed ) {
      _subsets = subsets;

      // initialize properly the set of nodes that can be currently eliminated:
      // find the first subset that contains some node(s) of the graph
      _nodeset.clear();
      for ( _subset_iter = _subsets->cbegin(); _subset_iter != _subsets->cend();
            ++_subset_iter ) {
        for ( const auto node : *_subset_iter ) {
          if ( _graph->existsNode( node ) ) {
            _nodeset.insert( node );
          }
        }
        if ( !_nodeset.empty() ) return true;
      }
    }

    return false;
  }

  /// clears the sequence (to prepare, for instance, a new elimination sequence)
  void PartialOrderedEliminationSequenceStrategy::clear() {
    EliminationSequenceStrategy::clear();
    _subsets = nullptr;
    _nodeset.clear();
    _partial_order_needed = true;
  }

} /* namespace gum */
