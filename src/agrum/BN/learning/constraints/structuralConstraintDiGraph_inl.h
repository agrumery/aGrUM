
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
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
 * @brief the base class for structural constraints used by learning algorithms
 * that learn a directed graph structure
 *
 * @author Christophe GONZALES (@AMU) and Pierre-Henri WUILLEMIN (@LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// sets a new graph from which we will perform checkings
    INLINE void StructuralConstraintDiGraph::setGraphAlone(const DiGraph& graph) {
      _DiGraph__graph = graph;
    }

    /// checks whether the constraints enable to add arc (x,y)
    INLINE bool
       StructuralConstraintDiGraph::checkArcAdditionAlone(NodeId x,
                                                          NodeId y) const {
      return _DiGraph__graph.existsNode(x) && _DiGraph__graph.existsNode(y)
             && !_DiGraph__graph.existsArc(x, y);
    }

    /// checks whether the constraints enable to remove arc (x,y)
    INLINE bool
       StructuralConstraintDiGraph::checkArcDeletionAlone(NodeId x,
                                                          NodeId y) const {
      return _DiGraph__graph.existsArc(x, y);
    }

    /// checks whether the constraints enable to reverse arc (x,y)
    INLINE bool
       StructuralConstraintDiGraph::checkArcReversalAlone(NodeId x,
                                                          NodeId y) const {
      return _DiGraph__graph.existsArc(x, y) && !_DiGraph__graph.existsArc(y, x);
    }

    /// checks whether the constraints enable to add an arc
    INLINE bool StructuralConstraintDiGraph::checkModificationAlone(
       const ArcAddition& change) const {
      return checkArcAdditionAlone(change.node1(), change.node2());
    }

    /// checks whether the constraints enable to remove an arc
    INLINE bool StructuralConstraintDiGraph::checkModificationAlone(
       const ArcDeletion& change) const {
      return checkArcDeletionAlone(change.node1(), change.node2());
    }

    /// checks whether the constraints enable to reverse an arc
    INLINE bool StructuralConstraintDiGraph::checkModificationAlone(
       const ArcReversal& change) const {
      return checkArcReversalAlone(change.node1(), change.node2());
    }

    /// checks whether the constraints enable to perform a graph change
    INLINE bool StructuralConstraintDiGraph::checkModificationAlone(
       const GraphChange& change) const {
      switch (change.type()) {
        case GraphChangeType::ARC_ADDITION:
          return checkArcAdditionAlone(change.node1(), change.node2());

        case GraphChangeType::ARC_DELETION:
          return checkArcDeletionAlone(change.node1(), change.node2());

        case GraphChangeType::ARC_REVERSAL:
          return checkArcReversalAlone(change.node1(), change.node2());

        default:
          GUM_ERROR(OperationNotAllowed,
                    "edge modifications are not "
                    "supported by StructuralConstraintDiGraph");
      }
    }

    /// notify the constraint of a modification of the graph
    INLINE void
       StructuralConstraintDiGraph::modifyGraphAlone(const ArcAddition& change) {
      _DiGraph__graph.addArc(change.node1(), change.node2());
    }

    /// notify the constraint of a modification of the graph
    INLINE void
       StructuralConstraintDiGraph::modifyGraphAlone(const ArcDeletion& change) {
      _DiGraph__graph.eraseArc(Arc(change.node1(), change.node2()));
    }

    /// notify the constraint of a modification of the graph
    INLINE void
       StructuralConstraintDiGraph::modifyGraphAlone(const ArcReversal& change) {
      _DiGraph__graph.eraseArc(Arc(change.node1(), change.node2()));
      _DiGraph__graph.addArc(change.node2(), change.node1());
    }

    /// notify the constraint of a modification of the graph
    INLINE void
       StructuralConstraintDiGraph::modifyGraphAlone(const GraphChange& change) {
      switch (change.type()) {
        case GraphChangeType::ARC_ADDITION:
          modifyGraphAlone(reinterpret_cast< const ArcAddition& >(change));
          break;

        case GraphChangeType::ARC_DELETION:
          modifyGraphAlone(reinterpret_cast< const ArcDeletion& >(change));
          break;

        case GraphChangeType::ARC_REVERSAL:
          modifyGraphAlone(reinterpret_cast< const ArcReversal& >(change));
          break;

        default:
          GUM_ERROR(OperationNotAllowed,
                    "edge modifications are not supported by digraph constraint");
      }
    }

    /// indicates whether a change will always violate the constraint
    INLINE bool StructuralConstraintDiGraph::isAlwaysInvalidAlone(
       const GraphChange&) const {
      return false;
    }

    /// sets a new graph from which we will perform checkings
    INLINE void StructuralConstraintDiGraph::setGraph(Size nb_nodes) {
      _DiGraph__graph.clear();

      for (NodeId i = 0; i < nb_nodes; ++i) {
        _DiGraph__graph.addNodeWithId(i);
      }
    }

// include all the methods applicable to the whole class hierarchy
#  define GUM_CONSTRAINT_CLASS_NAME StructuralConstraintDiGraph
#  include <agrum/BN/learning/constraints/structuralConstraintPatternRootInline.h>
#  undef GUM_CONSTRAINT_CLASS_NAME

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */