
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
 * @brief class for graph triangulations for which we enforce a given partial
 * ordering on the nodes eliminations.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/agrum.h>

#include <agrum/graphs/algorithms/triangulations/partialOrderedTriangulation.h>

namespace gum {

  // default constructor
  PartialOrderedTriangulation::PartialOrderedTriangulation(
     const PartialOrderedEliminationSequenceStrategy& elimSeq,
     const JunctionTreeStrategy&                      JTStrategy,
     bool                                             minimality) :
      StaticTriangulation(elimSeq, JTStrategy, minimality) {
    // for debugging purposes
    GUM_CONSTRUCTOR(PartialOrderedTriangulation);
  }

  // constructor with a given graph
  PartialOrderedTriangulation::PartialOrderedTriangulation(
     const UndiGraph*                                 theGraph,
     const NodeProperty< Size >*                      dom,
     const List< NodeSet >*                           partial_order,
     const PartialOrderedEliminationSequenceStrategy& elimSeq,
     const JunctionTreeStrategy&                      JTStrategy,
     bool                                             minimality) :
      StaticTriangulation(theGraph, dom, elimSeq, JTStrategy, minimality),
      __partial_order(partial_order) {
    static_cast< PartialOrderedEliminationSequenceStrategy* >(
       _elimination_sequence_strategy)
       ->setPartialOrder(__partial_order);

    // for debugging purposes
    GUM_CONSTRUCTOR(PartialOrderedTriangulation);
  }

  // copy constructor
  PartialOrderedTriangulation::PartialOrderedTriangulation(
     const PartialOrderedTriangulation& from) :
      StaticTriangulation(from),
      __partial_order(from.__partial_order) {
    // for debugging purposes
    GUM_CONS_CPY(PartialOrderedTriangulation);
  }

  // move constructor
  PartialOrderedTriangulation::PartialOrderedTriangulation(
     PartialOrderedTriangulation&& from) :
      StaticTriangulation(std::move(from)),
      __partial_order(from.__partial_order) {
    // for debugging purposes
    GUM_CONS_MOV(PartialOrderedTriangulation);
  }

  /// virtual copy constructor
  PartialOrderedTriangulation* PartialOrderedTriangulation::newFactory() const {
    return new PartialOrderedTriangulation(
       static_cast< const PartialOrderedEliminationSequenceStrategy& >(
          *_elimination_sequence_strategy),
       *_junction_tree_strategy);
  }

  /// virtual copy constructor
  PartialOrderedTriangulation* PartialOrderedTriangulation::copyFactory() const {
    return new PartialOrderedTriangulation(*this);
  }

  /// destructor
  PartialOrderedTriangulation::~PartialOrderedTriangulation() {
    // for debugging purposes
    GUM_DESTRUCTOR(PartialOrderedTriangulation);
  }

  /// initialize the triangulation data structures for a new graph
  void
     PartialOrderedTriangulation::setGraph(const UndiGraph*            graph,
                                           const NodeProperty< Size >* domsizes) {
    StaticTriangulation::setGraph(graph, domsizes);
    static_cast< PartialOrderedEliminationSequenceStrategy* >(
       _elimination_sequence_strategy)
       ->setPartialOrder(__partial_order);
  }

  /// sets the sequence of elimination
  void PartialOrderedTriangulation::setPartialOrder(
     const List< NodeSet >* partial_order) {
    __partial_order = partial_order;
    static_cast< PartialOrderedEliminationSequenceStrategy* >(
       _elimination_sequence_strategy)
       ->setPartialOrder(__partial_order);
  }

  /// the function called to initialize the triangulation process
  void PartialOrderedTriangulation::_initTriangulation(UndiGraph& graph) {
    PartialOrderedEliminationSequenceStrategy* elim =
       static_cast< PartialOrderedEliminationSequenceStrategy* >(
          _elimination_sequence_strategy);
    elim->setGraph(&graph, _domain_sizes);
    elim->setPartialOrder(__partial_order);
  }

} /* namespace gum */
