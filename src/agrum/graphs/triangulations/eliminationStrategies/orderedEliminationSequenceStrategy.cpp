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
 * @brief An Elimination sequence algorithm that imposes a given complete
 *ordering
 * on the nodes elimination sequence
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/config.h>

#include <agrum/graphs/triangulations/eliminationStrategies/orderedEliminationSequenceStrategy.h>

#ifdef GUM_NO_INLINE
#include <agrum/graphs/triangulations/eliminationStrategies/orderedEliminationSequenceStrategy_inl.h>
#endif  // GUM_NOINLINE

namespace gum {

  /// default constructor (uses an empty graph)
  OrderedEliminationSequenceStrategy::OrderedEliminationSequenceStrategy() {
    // for debugging purposes
    GUM_CONSTRUCTOR( OrderedEliminationSequenceStrategy );
  }

  /// constructor for an a priori non empty graph
  OrderedEliminationSequenceStrategy::OrderedEliminationSequenceStrategy(
      UndiGraph*                 graph,
      const NodeProperty<Size>*  dom_sizes,
      const std::vector<NodeId>* order )
      : EliminationSequenceStrategy( graph, dom_sizes ) {
    // check that the user passed appropriate graphs and orders
    if ( ( ( graph == nullptr ) && ( order != nullptr ) ) ||
         ( ( graph != nullptr ) && ( order == nullptr ) ) ) {
      GUM_ERROR( GraphError,
                 "OrderedEliminationSequenceStrategy needs either both nullptrs "
                 "or both non-nullptrs on graph and elimination ordering" );
    }

    setOrder( order );

    // for debugging purposes
    GUM_CONSTRUCTOR( OrderedEliminationSequenceStrategy );
  }

  /// copy constructor
  OrderedEliminationSequenceStrategy::OrderedEliminationSequenceStrategy(
      const OrderedEliminationSequenceStrategy& from )
      : EliminationSequenceStrategy( from )
      , __order( from.__order )
      , __order_index( from.__order_index )
      , __order_needed( from.__order_needed ) {
    // for debugging purposes
    GUM_CONS_CPY( OrderedEliminationSequenceStrategy );
  }

  /// move constructor
  OrderedEliminationSequenceStrategy::OrderedEliminationSequenceStrategy(
      OrderedEliminationSequenceStrategy&& from )
      : EliminationSequenceStrategy( std::move( from ) )
      , __order( from.__order )
      , __order_index( from.__order_index )
      , __order_needed( from.__order_needed ) {
    // for debugging purposes
    GUM_CONS_MOV( OrderedEliminationSequenceStrategy );
  }

  /// destructor
  OrderedEliminationSequenceStrategy::~OrderedEliminationSequenceStrategy() {
    // for debugging purposes
    GUM_DESTRUCTOR( OrderedEliminationSequenceStrategy );
  }

  /** @brief creates a new elimination sequence of the same type as the current
   * object, but this sequence contains only an empty graph */
  OrderedEliminationSequenceStrategy*
  OrderedEliminationSequenceStrategy::newFactory() const {
    return new OrderedEliminationSequenceStrategy();
  }

  /// virtual copy constructor
  OrderedEliminationSequenceStrategy*
  OrderedEliminationSequenceStrategy::copyFactory() const {
    return new OrderedEliminationSequenceStrategy( *this );
  }

  /// sets a new graph to be triangulated
  bool OrderedEliminationSequenceStrategy::setGraph(
      UndiGraph* graph, const NodeProperty<Size>* domain_sizes ) {
    if ( EliminationSequenceStrategy::setGraph( graph, domain_sizes ) ) {
      setOrder( __order );
      return true;
    }

    return false;
  }

  /// indicates whether an order is compatible with the current graph
  bool OrderedEliminationSequenceStrategy::__isOrderNeeded(
      const std::vector<NodeId>* order ) const {
    if ( ( _graph == nullptr ) || ( order == nullptr ) ) return true;

    // determine the set of nodes in the order that belong to the graph
    NodeSet nodes_found( _graph->size() / 2 );
    for ( const auto node : *order ) {
      if ( _graph->existsNode( node ) ) {
        nodes_found.insert( node );
      }
    }

    // check that the size of nodes_found is equal to that of the graph
    return nodes_found.size() != _graph->size();
  }

  /// sets a new complete order
  bool OrderedEliminationSequenceStrategy::setOrder(
      const std::vector<NodeId>* order ) {
    // check that the order contains all the nodes of the graph
    __order_needed = __isOrderNeeded( order );

    if ( !__order_needed ) {
      __order = order;

      // determine the first element in order that belong to the graph
      // here, note that we have the guarantee that __order_index will be
      // lower than the size of __order since all the nodes in the graph
      // belong to vector __order
      __order_index = 0;
      while ( !_graph->existsNode( ( *__order )[__order_index] ) )
        ++__order_index;

      return true;
    }

    return false;
  }

  /// clears the order (to prepare, for instance, a new elimination sequence)
  void OrderedEliminationSequenceStrategy::clear() {
    EliminationSequenceStrategy::clear();
    __order_needed = true;
    __order_index = std::size_t( 0 );
  }

  /// returns the new node to be eliminated within the triangulation algorithm
  NodeId OrderedEliminationSequenceStrategy::nextNodeToEliminate() {
    // check that we can return a nodeId
    if ( __order_needed || ( __order->size() <= __order_index ) ) {
      GUM_ERROR( NotFound, "no node id can be returned" );
    }

    return ( *__order )[__order_index];
  }

  /** @brief if the elimination sequence is able to compute fill-ins, we
   * indicate whether we want this feature to be activated */
  void OrderedEliminationSequenceStrategy::askFillIns( bool do_it ) {
    // do nothing: we are not able to compute fill-ins
  }

  /** @brief indicates whether the fill-ins generated by the eliminated
   * nodes, if needed, will be computed by the elimination sequence, or need be
   * computed by the triangulation itself. */
  bool OrderedEliminationSequenceStrategy::providesFillIns() const {
    return false;
  }

  /** @brief indicates whether the elimination sequence updates by itself the
   * graph after a node has been eliminated */
  bool OrderedEliminationSequenceStrategy::providesGraphUpdate() const {
    return false;
  }

  /// performs all the graph/fill-ins updates provided (if any)
  void OrderedEliminationSequenceStrategy::eliminationUpdate( const NodeId node ) {
    // check whether there is something to update
    if ( !__order_needed ) {
      // check that node corresponds to the current index
      if ( ( __order_index >= __order->size() ) ||
           ( ( *__order )[__order_index] != node ) ) {
        GUM_ERROR(
            OutOfBounds,
            "update impossible because node "
                << node
                << " does not correspond to the current elimination index" );
      }

      // now perform the update: goto the next node that belongs to _graph
      ++__order_index;
      std::size_t size = __order->size();
      while ( ( __order_index < size ) &&
              !_graph->existsNode( ( *__order )[__order_index] ) )
        ++__order_index;
    }
  }

  /** @brief in case fill-ins are provided, this function returns the fill-ins
   * due to all the nodes eliminated so far */
  const EdgeSet& OrderedEliminationSequenceStrategy::fillIns() {
    return EliminationSequenceStrategy::fillIns();
  }

} /* namespace gum */
