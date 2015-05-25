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
 * @brief the base class for structural constraints imposed by DAGs
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <agrum/config.h>
#include <agrum/graphs/DAGCycleDetector.h>
#include <agrum/learning/constraints/structuralConstraintDiGraph.h>

namespace gum {

  namespace learning {

    /// sets a new graph from which we will perform checkings
    INLINE void StructuralConstraintDAG::setGraphAlone(const DiGraph &graph) {
      // check that the digraph has no cycle
      DAG g;

      for (auto node : graph)
        g.addNode(node);

      for (auto &arc : graph.arcs())
        g.addArc(arc.tail(), arc.head());

      _DAG__cycle_detector.setDAG(g);
    }

    /// sets a new graph from which we will perform checkings
    INLINE void StructuralConstraintDAG::setGraphAlone(unsigned int nb_nodes) {
      DAG g;

      for (unsigned int i = 0; i < nb_nodes; ++i) {
        g.addNode(i);
      }

      _DAG__cycle_detector.setDAG(g);
    }

    /// checks whether the constraints enable to add arc (x,y)
    INLINE bool StructuralConstraintDAG::checkArcAdditionAlone(NodeId x,
                                                               NodeId y) const
        noexcept {
      return !_DAG__cycle_detector.hasCycleFromAddition(x, y);
    }

    /// checks whether the constraints enable to remove arc (x,y)
    INLINE bool StructuralConstraintDAG::checkArcDeletionAlone(NodeId x,
                                                               NodeId y) const
        noexcept {
      return !_DAG__cycle_detector.hasCycleFromDeletion(x, y);
    }

    /// checks whether the constraints enable to reverse arc (x,y)
    INLINE bool StructuralConstraintDAG::checkArcReversalAlone(NodeId x,
                                                               NodeId y) const
        noexcept {
      return !_DAG__cycle_detector.hasCycleFromReversal(x, y);
    }

    /// checks whether the constraints enable to add an arc
    INLINE bool
    StructuralConstraintDAG::checkModificationAlone(const ArcAddition &change) const
        noexcept {
      return checkArcAdditionAlone(change.node1(), change.node2());
    }

    /// checks whether the constraints enable to remove an arc
    INLINE bool
    StructuralConstraintDAG::checkModificationAlone(const ArcDeletion &change) const
        noexcept {
      return checkArcDeletionAlone(change.node1(), change.node2());
    }

    /// checks whether the constraints enable to reverse an arc
    INLINE bool
    StructuralConstraintDAG::checkModificationAlone(const ArcReversal &change) const
        noexcept {
      return checkArcReversalAlone(change.node1(), change.node2());
    }

    /// checks whether the constraints enable to perform a graph change
    INLINE bool
    StructuralConstraintDAG::checkModificationAlone(const GraphChange &change) const
        noexcept {
      switch (change.type()) {
        case GraphChangeType::ARC_ADDITION:
          return checkArcAdditionAlone(change.node1(), change.node2());

        case GraphChangeType::ARC_DELETION:
          return checkArcDeletionAlone(change.node1(), change.node2());

        case GraphChangeType::ARC_REVERSAL:
          return checkArcReversalAlone(change.node1(), change.node2());

        default:
          GUM_ERROR(OperationNotAllowed, "edge modifications are not "
                                         "supported by StructuralConstraintDAG");
      }
    }

    /// notify the constraint of a modification of the graph
    INLINE void
    StructuralConstraintDAG::modifyGraphAlone(const ArcAddition &change) {
      _DAG__cycle_detector.addArc(change.node1(), change.node2());
    }

    /// notify the constraint of a modification of the graph
    INLINE void
    StructuralConstraintDAG::modifyGraphAlone(const ArcDeletion &change) {
      _DAG__cycle_detector.eraseArc(change.node1(), change.node2());
    }

    /// notify the constraint of a modification of the graph
    INLINE void
    StructuralConstraintDAG::modifyGraphAlone(const ArcReversal &change) {
      _DAG__cycle_detector.reverseArc(change.node1(), change.node2());
    }

    /// notify the constraint of a modification of the graph
    INLINE void
    StructuralConstraintDAG::modifyGraphAlone(const GraphChange &change) {
      switch (change.type()) {
        case GraphChangeType::ARC_ADDITION:
          modifyGraphAlone(reinterpret_cast<const ArcAddition &>(change));
          break;

        case GraphChangeType::ARC_DELETION:
          modifyGraphAlone(reinterpret_cast<const ArcDeletion &>(change));
          break;

        case GraphChangeType::ARC_REVERSAL:
          modifyGraphAlone(reinterpret_cast<const ArcReversal &>(change));
          break;

        default:
          GUM_ERROR(OperationNotAllowed,
                    "edge modifications are not supported by DAG constraints");
      }
    }

    /// indicates whether a change will always violate the constraint
    INLINE bool
    StructuralConstraintDAG::isAlwaysInvalidAlone(const GraphChange &) const
        noexcept {
      return false;
    }

    /// sets a new graph from which we will perform checkings
    INLINE void StructuralConstraintDAG::setGraph(const DAG &graph) {
      constraints::setGraph(graph);
      _DAG__cycle_detector.setDAG(graph);
    }

    /// sets a new graph from which we will perform checkings
    INLINE void StructuralConstraintDAG::setGraph(unsigned int nb_nodes) {
      StructuralConstraintDiGraph::setGraph(nb_nodes);
      setGraphAlone(nb_nodes);
    }

// include all the methods applicable to the whole class hierarchy
#define GUM_CONSTRAINT_CLASS_NAME StructuralConstraintDAG
#include <agrum/learning/constraints/structuralConstraintPatternInline.h>
#undef GUM_CONSTRAINT_CLASS_NAME

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
