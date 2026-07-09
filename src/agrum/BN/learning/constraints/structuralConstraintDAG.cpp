/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/** @file
 * @brief the base class for structural constraints imposed by DAGs
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <agrum/BN/learning/constraints/structuralConstraintDAG.h>

/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#  include <agrum/BN/learning/constraints/structuralConstraintDAG_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor
    StructuralConstraintDAG::StructuralConstraintDAG() { GUM_CONSTRUCTOR(StructuralConstraintDAG); }

    /// constructor starting with an empty graph with a given number of nodes
    StructuralConstraintDAG::StructuralConstraintDAG(Size nb_nodes) {
      setGraph(nb_nodes);
      GUM_CONSTRUCTOR(StructuralConstraintDAG);
    }

    /// constructor starting with a given graph
    StructuralConstraintDAG::StructuralConstraintDAG(const DAG& graph) {
      setGraph(graph);
      GUM_CONSTRUCTOR(StructuralConstraintDAG);
    }

    /// copy constructor
    StructuralConstraintDAG::StructuralConstraintDAG(const StructuralConstraintDAG& from) :
        _graph_(from._graph_) {
      GUM_CONS_CPY(StructuralConstraintDAG);
    }

    /// move constructor
    StructuralConstraintDAG::StructuralConstraintDAG(StructuralConstraintDAG&& from) noexcept :
        _graph_(std::move(from._graph_)) {
      GUM_CONS_MOV(StructuralConstraintDAG);
    }

    /// destructor
    StructuralConstraintDAG::~StructuralConstraintDAG() { GUM_DESTRUCTOR(StructuralConstraintDAG); }

    /// copy operator
    StructuralConstraintDAG&
        StructuralConstraintDAG::operator=(const StructuralConstraintDAG& from) {
      if (this != &from) { _graph_ = from._graph_; }
      return *this;
    }

    /// move operator
    StructuralConstraintDAG& StructuralConstraintDAG::operator=(StructuralConstraintDAG&& from) {
      if (this != &from) { _graph_ = std::move(from._graph_); }
      return *this;
    }


  void StructuralConstraintDAG::setGraphAlone(const DiGraph& graph) {
    // check that the digraph contains no directed cycle
    DiGraph g;

    for (auto node: graph)
      g.addNodeWithId(node);

    for (auto& arc: graph.arcs()) {
      if (graph::hasDirectedPath(g, arc.head(), arc.tail())) {
        GUM_ERROR(InvalidDirectedCycle,
                  "Graphs with directed cycles cannot be passed to StructuralConstraintDAG");
      }
      g.addArc(arc.tail(), arc.head());
    }

    // ok, here, there is no directed cycle
    _graph_ = std::move(g);
  }

  bool StructuralConstraintDAG::checkModificationAlone(const GraphChange& change) const {
    switch (change.type()) {
      case GraphChangeType::ARC_ADDITION :
        return checkArcAdditionAlone(change.node1(), change.node2());

      case GraphChangeType::ARC_DELETION :
        return checkArcDeletionAlone(change.node1(), change.node2());

      case GraphChangeType::ARC_REVERSAL :
        return checkArcReversalAlone(change.node1(), change.node2());

      case GraphChangeType::ARC_TRIANGLE_DELETION1 :
        return checkArcTriangleDeletion1Alone(change.node1(), change.node2(), change.node3());

      case GraphChangeType::ARC_TRIANGLE_DELETION2 :
        return checkArcTriangleDeletion2Alone(change.node1(), change.node2(), change.node3());

      default :
        GUM_ERROR(OperationNotAllowed,
                  "Graph change operation " << change.typeAsString()
                                            << "is not supported by the DAG structural constraint");
    }
  }

  void StructuralConstraintDAG::modifyGraphAlone(const GraphChange& change) {
    switch (change.type()) {
      case GraphChangeType::ARC_ADDITION :
        modifyGraphAlone(reinterpret_cast< const ArcAddition& >(change));
        break;

      case GraphChangeType::ARC_DELETION :
        modifyGraphAlone(reinterpret_cast< const ArcDeletion& >(change));
        break;

      case GraphChangeType::ARC_REVERSAL :
        modifyGraphAlone(reinterpret_cast< const ArcReversal& >(change));
        break;

      case GraphChangeType::ARC_TRIANGLE_DELETION1 :
        modifyGraphAlone(reinterpret_cast< const ArcTriangleDeletion1& >(change));
        break;

      case GraphChangeType::ARC_TRIANGLE_DELETION2 :
        modifyGraphAlone(reinterpret_cast< const ArcTriangleDeletion2& >(change));
        break;

      default :
        GUM_ERROR(OperationNotAllowed,
                  "Graph change operation "
                      << change.typeAsString()
                      << " is not supported by the DAG structural constraints")
    }
  }
  } /* namespace learning */

} /* namespace gum */
