/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/





/** @file
 * @brief class for graph triangulations for which we enforce a given complete
 * ordering on the nodes eliminations.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/agrum.h>

#include <agrum/base/graphs/algorithms/triangulations/orderedTriangulation.h>

namespace gum {

  /// default constructor
  OrderedTriangulation::OrderedTriangulation(const OrderedEliminationSequenceStrategy& elimSeq,
                                             const JunctionTreeStrategy&               JTStrategy,
                                             bool                                      minimality) :
      StaticTriangulation(elimSeq, JTStrategy, minimality) {
    // for debugging purposes
    GUM_CONSTRUCTOR(OrderedTriangulation);
  }

  /// constructor with a given graph
  OrderedTriangulation::OrderedTriangulation(const UndiGraph*                          theGraph,
                                             const NodeProperty< Size >*               dom,
                                             const std::vector< NodeId >*              order,
                                             const OrderedEliminationSequenceStrategy& elimSeq,
                                             const JunctionTreeStrategy&               JTStrategy,
                                             bool                                      minimality) :
      StaticTriangulation(theGraph, dom, elimSeq, JTStrategy, minimality), _order_(order) {
    static_cast< OrderedEliminationSequenceStrategy* >(elimination_sequence_strategy_)
        ->setOrder(_order_);

    // for debugging purposes
    GUM_CONSTRUCTOR(OrderedTriangulation);
  }

  // copy constructor
  OrderedTriangulation::OrderedTriangulation(const OrderedTriangulation& from) :
      StaticTriangulation(from), _order_(from._order_) {   // for debugging purposes
    GUM_CONS_CPY(OrderedTriangulation);
  }

  // move constructor
  OrderedTriangulation::OrderedTriangulation(OrderedTriangulation&& from) :
      StaticTriangulation(std::move(from)), _order_(from._order_) {
    // for debugging purposes
    GUM_CONS_MOV(OrderedTriangulation);
  }

  /// virtual copy constructor
  OrderedTriangulation* OrderedTriangulation::newFactory() const {
    return new OrderedTriangulation(
        static_cast< const OrderedEliminationSequenceStrategy& >(*elimination_sequence_strategy_),
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
    static_cast< OrderedEliminationSequenceStrategy* >(elimination_sequence_strategy_)
        ->setOrder(_order_);
  }

  /// sets the sequence of elimination
  void OrderedTriangulation::setOrder(const std::vector< NodeId >* order) {
    _order_ = order;
    static_cast< OrderedEliminationSequenceStrategy* >(elimination_sequence_strategy_)
        ->setOrder(_order_);
  }

  /// the function called to initialize the triangulation process
  void OrderedTriangulation::initTriangulation_(UndiGraph& graph) {
    OrderedEliminationSequenceStrategy* elim
        = static_cast< OrderedEliminationSequenceStrategy* >(elimination_sequence_strategy_);
    elim->setGraph(&graph, domain_sizes_);
    elim->setOrder(_order_);
  }

} /* namespace gum */
