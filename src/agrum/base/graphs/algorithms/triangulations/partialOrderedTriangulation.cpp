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
 * @brief class for graph triangulations for which we enforce a given partial
 * ordering on the nodes eliminations.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/agrum.h>

#include <agrum/base/graphs/algorithms/triangulations/partialOrderedTriangulation.h>

namespace gum {

  // default constructor
  PartialOrderedTriangulation::PartialOrderedTriangulation(
      const PartialOrderedEliminationSequenceStrategy& elimSeq,
      const JunctionTreeStrategy&                      JTStrategy,
      bool minimality) : StaticTriangulation(elimSeq, JTStrategy, minimality) {
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
      _partial_order_(partial_order) {
    static_cast< PartialOrderedEliminationSequenceStrategy* >(elimination_sequence_strategy_)
        ->setPartialOrder(_partial_order_);

    // for debugging purposes
    GUM_CONSTRUCTOR(PartialOrderedTriangulation);
  }

  // copy constructor
  PartialOrderedTriangulation::PartialOrderedTriangulation(
      const PartialOrderedTriangulation& from) :
      StaticTriangulation(from), _partial_order_(from._partial_order_) {
    // for debugging purposes
    GUM_CONS_CPY(PartialOrderedTriangulation);
  }

  // move constructor
  PartialOrderedTriangulation::PartialOrderedTriangulation(PartialOrderedTriangulation&& from) :
      StaticTriangulation(std::move(from)), _partial_order_(from._partial_order_) {
    // for debugging purposes
    GUM_CONS_MOV(PartialOrderedTriangulation);
  }

  /// virtual copy constructor
  PartialOrderedTriangulation* PartialOrderedTriangulation::newFactory() const {
    return new PartialOrderedTriangulation(
        static_cast< const PartialOrderedEliminationSequenceStrategy& >(
            *elimination_sequence_strategy_),
        *junction_tree_strategy_);
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
  void PartialOrderedTriangulation::setGraph(const UndiGraph*            graph,
                                             const NodeProperty< Size >* domsizes) {
    StaticTriangulation::setGraph(graph, domsizes);
    static_cast< PartialOrderedEliminationSequenceStrategy* >(elimination_sequence_strategy_)
        ->setPartialOrder(_partial_order_);
  }

  /// sets the sequence of elimination
  void PartialOrderedTriangulation::setPartialOrder(const List< NodeSet >* partial_order) {
    _partial_order_ = partial_order;
    static_cast< PartialOrderedEliminationSequenceStrategy* >(elimination_sequence_strategy_)
        ->setPartialOrder(_partial_order_);
  }

  /// the function called to initialize the triangulation process
  void PartialOrderedTriangulation::initTriangulation_(UndiGraph& graph) {
    PartialOrderedEliminationSequenceStrategy* elim
        = static_cast< PartialOrderedEliminationSequenceStrategy* >(elimination_sequence_strategy_);
    elim->setGraph(&graph, domain_sizes_);
    elim->setPartialOrder(_partial_order_);
  }

} /* namespace gum */
