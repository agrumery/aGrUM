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

#pragma once

/** @file
 * @brief the base class for structural constraints imposed by DAGs
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/agrum.h>

#  include <agrum/base/graphs/algorithms/generic/pathFinding.h>

namespace gum::learning {

  ALWAYS_INLINE void StructuralConstraintDAG::_lock_() const {
    while (_lock_flag_.test_and_set(std::memory_order_acquire)) {}
  }

  ALWAYS_INLINE void StructuralConstraintDAG::_unlock_() const {
    _lock_flag_.clear(std::memory_order_release);
  }

  /// sets a new graph from which we will perform checking
  INLINE void StructuralConstraintDAG::setGraphAlone(const DiGraph& graph) {
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

  /// sets a new graph from which we will perform checking
  INLINE void StructuralConstraintDAG::setGraphAlone(Size nb_nodes) {
    for (NodeId i = 0; i < nb_nodes; ++i) {
      _graph_.addNodeWithId(i);
    }
  }

  /// checks whether the constraints enable to add arc (x,y)
  INLINE bool StructuralConstraintDAG::checkArcAdditionAlone(NodeId x, NodeId y) const {
    if (!_graph_.existsNode(x) || !_graph_.existsNode(y)) return false;
    _lock_();
    bool result = !_graph_.existsArc(x, y) && !graph::hasDirectedPath(_graph_, y, x);
    _unlock_();
    return result;
  }

  /// checks whether the constraints enable to remove arc (x,y)
  INLINE bool StructuralConstraintDAG::checkArcDeletionAlone(NodeId x, NodeId y) const {
    _lock_();
    bool result = _graph_.existsArc(x, y);
    _unlock_();
    return result;
  }

  /// checks whether the constraints enable to reverse arc (x,y)
  INLINE bool StructuralConstraintDAG::checkArcReversalAlone(NodeId x, NodeId y) const {
    bool result = false;
    _lock_();
    if (_graph_.existsArc(x, y)) {
      _graph_.eraseArc(Arc(x, y));
      result = !graph::hasDirectedPath(_graph_, x, y);
      _graph_.addArc(x, y);
    }
    _unlock_();
    return result;
  }

  /// checks whether the constraints enable to apply an ArcTriangleDeletion1
  INLINE bool StructuralConstraintDAG::checkArcTriangleDeletion1Alone(NodeId node1,
                                                                      NodeId node2,
                                                                      NodeId node3) const {
    bool result = false;
    _lock_();
    if (_graph_.existsArc(node1, node2) && _graph_.existsArc(node1, node3)
        && _graph_.existsArc(node2, node3)) {
      _graph_.eraseArc(Arc(node1, node2));
      _graph_.eraseArc(Arc(node1, node3));
      _graph_.eraseArc(Arc(node2, node3));
      if (!graph::hasDirectedPath(_graph_, node1, node2)) {
        _graph_.addArc(node2, node1);
        result = !graph::hasDirectedPath(_graph_, node1, node3);
        _graph_.eraseArc(Arc(node2, node1));
      }
      _graph_.addArc(node1, node2);
      _graph_.addArc(node1, node3);
      _graph_.addArc(node2, node3);
    }
    _unlock_();
    return result;
  }

  /// checks whether the constraints enable to apply an ArcTriangleDeletion2
  INLINE bool StructuralConstraintDAG::checkArcTriangleDeletion2Alone(NodeId node1,
                                                                      NodeId node2,
                                                                      NodeId node3) const {
    bool result = false;
    _lock_();
    if (_graph_.existsArc(node1, node2) && _graph_.existsArc(node1, node3)
        && _graph_.existsArc(node2, node3)) {
      _graph_.eraseArc(Arc(node1, node3));
      _graph_.eraseArc(Arc(node2, node3));
      result = !graph::hasDirectedPath(_graph_, node2, node3);
      _graph_.addArc(node1, node3);
      _graph_.addArc(node2, node3);
    }
    _unlock_();
    return result;
  }

  /// checks whether the constraints enable to add an arc
  INLINE bool StructuralConstraintDAG::checkModificationAlone(const ArcAddition& change) const {
    return checkArcAdditionAlone(change.node1(), change.node2());
  }

  /// checks whether the constraints enable to remove an arc
  INLINE bool StructuralConstraintDAG::checkModificationAlone(const ArcDeletion& change) const {
    return checkArcDeletionAlone(change.node1(), change.node2());
  }

  /// checks whether the constraints enable to reverse an arc
  INLINE bool StructuralConstraintDAG::checkModificationAlone(const ArcReversal& change) const {
    return checkArcReversalAlone(change.node1(), change.node2());
  }

  /// checks whether the constraints enable to apply an ArcTriangleDeletion1
  INLINE bool
      StructuralConstraintDAG::checkModificationAlone(const ArcTriangleDeletion1& change) const {
    return checkArcTriangleDeletion1Alone(change.node1(), change.node2(), change.node3());
  }

  /// checks whether the constraints enable to apply an ArcTriangleDeletion2
  INLINE bool
      StructuralConstraintDAG::checkModificationAlone(const ArcTriangleDeletion2& change) const {
    return checkArcTriangleDeletion2Alone(change.node1(), change.node2(), change.node3());
  }

  /// checks whether the constraints enable to perform a graph change
  INLINE bool StructuralConstraintDAG::checkModificationAlone(const GraphChange& change) const {
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

  /// notify the constraint of a modification of the graph
  INLINE void StructuralConstraintDAG::modifyGraphAlone(const ArcAddition& change) {
    _lock_();
    _graph_.addArc(change.node1(), change.node2());
    _unlock_();
  }

  /// notify the constraint of a modification of the graph
  INLINE void StructuralConstraintDAG::modifyGraphAlone(const ArcDeletion& change) {
    _lock_();
    _graph_.eraseArc(Arc(change.node1(), change.node2()));
    _unlock_();
  }

  /// notify the constraint of a modification of the graph
  INLINE void StructuralConstraintDAG::modifyGraphAlone(const ArcReversal& change) {
    _lock_();
    _graph_.eraseArc(Arc(change.node1(), change.node2()));
    _graph_.addArc(change.node2(), change.node1());
    _unlock_();
  }

  /// notify the constraint of a modification of the graph
  INLINE void StructuralConstraintDAG::modifyGraphAlone(const ArcTriangleDeletion1& change) {
    _lock_();
    _graph_.eraseArc(Arc(change.node1(), change.node2()));
    _graph_.eraseArc(Arc(change.node1(), change.node3()));
    _graph_.eraseArc(Arc(change.node2(), change.node3()));
    _graph_.addArc(change.node2(), change.node1());
    _graph_.addArc(change.node3(), change.node1());
    _unlock_();
  }

  /// notify the constraint of a modification of the graph
  INLINE void StructuralConstraintDAG::modifyGraphAlone(const ArcTriangleDeletion2& change) {
    _lock_();
    _graph_.eraseArc(Arc(change.node1(), change.node3()));
    _graph_.eraseArc(Arc(change.node2(), change.node3()));
    _graph_.addArc(change.node3(), change.node2());
    _unlock_();
  }

  /// notify the constraint of a modification of the graph
  INLINE void StructuralConstraintDAG::modifyGraphAlone(const GraphChange& change) {
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

  /// indicates whether a change will always violate the constraint
  INLINE bool StructuralConstraintDAG::isAlwaysInvalidAlone(const GraphChange&) const {
    return false;
  }

  /// sets a new graph from which we will perform checking
  INLINE void StructuralConstraintDAG::setGraph(const DAG& graph) { _graph_ = graph; }

  /// sets a new graph from which we will perform checkings
  INLINE void StructuralConstraintDAG::setGraph(Size nb_nodes) { setGraphAlone(nb_nodes); }

// include all the methods applicable to the whole class hierarchy
#  define GUM_CONSTRAINT_CLASS_NAME StructuralConstraintDAG
#  include <agrum/BN/learning/constraints/structuralConstraintPatternRootInline.h>
#  undef GUM_CONSTRAINT_CLASS_NAME

}   // namespace gum::learning


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
