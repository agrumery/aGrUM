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
 * @brief class for graph triangulations for which we enforce a given complete
 * ordering on the nodes eliminations.
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#include <agrum/agrum.h>

#include <agrum/tools/graphs/algorithms/triangulations/orderedTriangulation.h>

namespace gum {

  /// default constructor
  OrderedTriangulation::OrderedTriangulation(
     const OrderedEliminationSequenceStrategy& elimSeq,
     const JunctionTreeStrategy&               JTStrategy,
     bool                                      minimality) :
      StaticTriangulation(elimSeq, JTStrategy, minimality) {
    // for debugging purposes
    GUM_CONSTRUCTOR(OrderedTriangulation);
  }

  /// constructor with a given graph
  OrderedTriangulation::OrderedTriangulation(
     const UndiGraph*                          theGraph,
     const NodeProperty< Size >*               dom,
     const std::vector< NodeId >*              order,
     const OrderedEliminationSequenceStrategy& elimSeq,
     const JunctionTreeStrategy&               JTStrategy,
     bool                                      minimality) :
      StaticTriangulation(theGraph, dom, elimSeq, JTStrategy, minimality),
       _order_(order) {
    static_cast< OrderedEliminationSequenceStrategy* >(
       elimination_sequence_strategy_)
       ->setOrder( _order_);

    // for debugging purposes
    GUM_CONSTRUCTOR(OrderedTriangulation);
  }

  // copy constructor
  OrderedTriangulation::OrderedTriangulation(const OrderedTriangulation& from) :
      StaticTriangulation(from),
       _order_(from. _order_) {   // for debugging purposes
    GUM_CONS_CPY(OrderedTriangulation);
  }

  // move constructor
  OrderedTriangulation::OrderedTriangulation(OrderedTriangulation&& from) :
      StaticTriangulation(std::move(from)),  _order_(from. _order_) {
    // for debugging purposes
    GUM_CONS_MOV(OrderedTriangulation);
  }

  /// virtual copy constructor
  OrderedTriangulation* OrderedTriangulation::newFactory() const {
    return new OrderedTriangulation(
       static_cast< const OrderedEliminationSequenceStrategy& >(
          *elimination_sequence_strategy_),
       *junction_tree_strategy_);
  }

  /// virtual copy constructor
  OrderedTriangulation* OrderedTriangulation::copyFactory() const {
    return new OrderedTriangulation(*this);
  }

  /// destructor
  OrderedTriangulation::~OrderedTriangulation() {
    // for debugging purposes
    GUM_DESTRUCTOR(OrderedTriangulation);
  }

  /// initialize the triangulation data structures for a new graph
  void OrderedTriangulation::setGraph(const UndiGraph*            graph,
                                      const NodeProperty< Size >* domsizes) {
    StaticTriangulation::setGraph(graph, domsizes);
    static_cast< OrderedEliminationSequenceStrategy* >(
       elimination_sequence_strategy_)
       ->setOrder( _order_);
  }

  /// sets the sequence of elimination
  void OrderedTriangulation::setOrder(const std::vector< NodeId >* order) {
     _order_ = order;
    static_cast< OrderedEliminationSequenceStrategy* >(
       elimination_sequence_strategy_)
       ->setOrder( _order_);
  }

  /// the function called to initialize the triangulation process
  void OrderedTriangulation::initTriangulation_(UndiGraph& graph) {
    OrderedEliminationSequenceStrategy* elim
       = static_cast< OrderedEliminationSequenceStrategy* >(
          elimination_sequence_strategy_);
    elim->setGraph(&graph, domain_sizes_);
    elim->setOrder( _order_);
  }

} /* namespace gum */
