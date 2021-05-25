/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/** @file
 * @brief An Elimination sequence algorithm that imposes a given complete
 *ordering
 * on the nodes elimination sequence
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#include <agrum/agrum.h>

#include <agrum/tools/graphs/algorithms/triangulations/eliminationStrategies/orderedEliminationSequenceStrategy.h>

#ifdef GUM_NO_INLINE
#  include <agrum/tools/graphs/algorithms/triangulations/eliminationStrategies/orderedEliminationSequenceStrategy_inl.h>
#endif   // GUM_NOINLINE

namespace gum {

  /// default constructor (uses an empty graph)
  OrderedEliminationSequenceStrategy::OrderedEliminationSequenceStrategy() {
    GUM_CONSTRUCTOR(OrderedEliminationSequenceStrategy);
  }

  /// constructor for an a priori non empty graph
  OrderedEliminationSequenceStrategy::OrderedEliminationSequenceStrategy(
     UndiGraph*                   graph,
     const NodeProperty< Size >*  dom_sizes,
     const std::vector< NodeId >* order) :
      EliminationSequenceStrategy(graph, dom_sizes) {
    // check that the user passed appropriate graphs and orders
    if (((graph == nullptr) && (order != nullptr)) || ((graph != nullptr) && (order == nullptr))) {
      GUM_ERROR(GraphError,
                "OrderedEliminationSequenceStrategy needs either both nullptrs "
                "or both non-nullptrs on graph and elimination ordering");
    }

    setOrder(order);

    GUM_CONSTRUCTOR(OrderedEliminationSequenceStrategy);
  }

  /// copy constructor
  OrderedEliminationSequenceStrategy::OrderedEliminationSequenceStrategy(
     const OrderedEliminationSequenceStrategy& from) :
      EliminationSequenceStrategy(from),
      _order_(from._order_), _order_index_(from._order_index_),
      _order_needed_(from._order_needed_) {
    GUM_CONS_CPY(OrderedEliminationSequenceStrategy);
  }

  /// move constructor
  OrderedEliminationSequenceStrategy::OrderedEliminationSequenceStrategy(
     OrderedEliminationSequenceStrategy&& from) :
      EliminationSequenceStrategy(std::move(from)),
      _order_(from._order_), _order_index_(from._order_index_),
      _order_needed_(from._order_needed_) {
    GUM_CONS_MOV(OrderedEliminationSequenceStrategy);
  }

  /// destructor
  OrderedEliminationSequenceStrategy::~OrderedEliminationSequenceStrategy() {
    GUM_DESTRUCTOR(OrderedEliminationSequenceStrategy);
  }

  /** @brief creates a new elimination sequence of the same type as the current
   * object, but this sequence contains only an empty graph */
  OrderedEliminationSequenceStrategy* OrderedEliminationSequenceStrategy::newFactory() const {
    return new OrderedEliminationSequenceStrategy();
  }

  /// virtual copy constructor
  OrderedEliminationSequenceStrategy* OrderedEliminationSequenceStrategy::copyFactory() const {
    return new OrderedEliminationSequenceStrategy(*this);
  }

  /// sets a new graph to be triangulated
  bool OrderedEliminationSequenceStrategy::setGraph(UndiGraph*                  graph,
                                                    const NodeProperty< Size >* domain_sizes) {
    if (EliminationSequenceStrategy::setGraph(graph, domain_sizes)) {
      setOrder(_order_);
      return true;
    }

    return false;
  }

  /// indicates whether an order is compatible with the current graph
  bool
     OrderedEliminationSequenceStrategy::_isOrderNeeded_(const std::vector< NodeId >* order) const {
    if ((graph_ == nullptr) || (order == nullptr)) return true;

    // determine the set of nodes in the order that belong to the graph
    NodeSet nodes_found(graph_->size() / 2);
    for (const auto node: *order) {
      if (graph_->existsNode(node)) { nodes_found.insert(node); }
    }

    // check that the size of nodes_found is equal to that of the graph
    return nodes_found.size() != graph_->size();
  }

  /// sets a new complete order
  bool OrderedEliminationSequenceStrategy::setOrder(const std::vector< NodeId >* order) {
    // check that the order contains all the nodes of the graph
    _order_needed_ = _isOrderNeeded_(order);

    if (!_order_needed_) {
      _order_ = order;

      // determine the first element in order that belong to the graph
      // here, note that we have the guarantee that  _order_index_ will be
      // lower than the size of  _order_ since all the nodes in the graph
      // belong to vector  _order_
      _order_index_ = 0;
      while (!graph_->existsNode((*_order_)[_order_index_]))
        ++_order_index_;

      return true;
    }

    return false;
  }

  /// clears the order (to prepare, for instance, a new elimination sequence)
  void OrderedEliminationSequenceStrategy::clear() {
    EliminationSequenceStrategy::clear();
    _order_needed_ = true;
    _order_index_  = std::size_t(0);
  }

  /// returns the new node to be eliminated within the triangulation algorithm
  NodeId OrderedEliminationSequenceStrategy::nextNodeToEliminate() {
    // check that we can return a nodeId
    if (_order_needed_ || (_order_->size() <= _order_index_)) {
      GUM_ERROR(NotFound, "no node id can be returned")
    }

    return (*_order_)[_order_index_];
  }

  /** @brief if the elimination sequence is able to compute fill-ins, we
   * indicate whether we want this feature to be activated */
  void OrderedEliminationSequenceStrategy::askFillIns(bool do_it) {
    // do nothing: we are not able to compute fill-ins
  }

  /** @brief indicates whether the fill-ins generated by the eliminated
   * nodes, if needed, will be computed by the elimination sequence, or need be
   * computed by the triangulation itself. */
  bool OrderedEliminationSequenceStrategy::providesFillIns() const { return false; }

  /** @brief indicates whether the elimination sequence updates by itself the
   * graph after a node has been eliminated */
  bool OrderedEliminationSequenceStrategy::providesGraphUpdate() const { return false; }

  /// performs all the graph/fill-ins updates provided (if any)
  void OrderedEliminationSequenceStrategy::eliminationUpdate(const NodeId node) {
    // check whether there is something to update
    if (!_order_needed_) {
      // check that node corresponds to the current index
      if ((_order_index_ >= _order_->size()) || ((*_order_)[_order_index_] != node)) {
        GUM_ERROR(OutOfBounds,
                  "update impossible because node "
                     << node << " does not correspond to the current elimination index");
      }

      // now perform the update: goto the next node that belongs to graph_
      ++_order_index_;
      std::size_t size = _order_->size();
      while ((_order_index_ < size) && !graph_->existsNode((*_order_)[_order_index_]))
        ++_order_index_;
    }
  }

  /** @brief in case fill-ins are provided, this function returns the fill-ins
   * due to all the nodes eliminated so far */
  const EdgeSet& OrderedEliminationSequenceStrategy::fillIns() {
    return EliminationSequenceStrategy::fillIns();
  }

} /* namespace gum */
