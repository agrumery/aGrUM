
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
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
      __order(order) {
    static_cast< OrderedEliminationSequenceStrategy* >(
       _elimination_sequence_strategy)
       ->setOrder(__order);

    // for debugging purposes
    GUM_CONSTRUCTOR(OrderedTriangulation);
  }

  // copy constructor
  OrderedTriangulation::OrderedTriangulation(const OrderedTriangulation& from) :
      StaticTriangulation(from), __order(from.__order) {
    // for debugging purposes
    GUM_CONS_CPY(OrderedTriangulation);
  }

  // move constructor
  OrderedTriangulation::OrderedTriangulation(OrderedTriangulation&& from) :
      StaticTriangulation(std::move(from)), __order(from.__order) {
    // for debugging purposes
    GUM_CONS_MOV(OrderedTriangulation);
  }

  /// virtual copy constructor
  OrderedTriangulation* OrderedTriangulation::newFactory() const {
    return new OrderedTriangulation(
       static_cast< const OrderedEliminationSequenceStrategy& >(
          *_elimination_sequence_strategy),
       *_junction_tree_strategy);
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
       _elimination_sequence_strategy)
       ->setOrder(__order);
  }

  /// sets the sequence of elimination
  void OrderedTriangulation::setOrder(const std::vector< NodeId >* order) {
    __order = order;
    static_cast< OrderedEliminationSequenceStrategy* >(
       _elimination_sequence_strategy)
       ->setOrder(__order);
  }

  /// the function called to initialize the triangulation process
  void OrderedTriangulation::_initTriangulation(UndiGraph& graph) {
    OrderedEliminationSequenceStrategy* elim =
       static_cast< OrderedEliminationSequenceStrategy* >(
          _elimination_sequence_strategy);
    elim->setGraph(&graph, _domain_sizes);
    elim->setOrder(__order);
  }

} /* namespace gum */
