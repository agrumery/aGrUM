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
 * @brief the structural constraint for forbidding the creation of some arcs
 * during structure learning
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// sets a new graph from which we will perform checkings
    INLINE void
       StructuralConstraintPossibleEdges::setGraphAlone(const DiGraph& graph) {}

    /// checks whether the constraints enable to add arc (x,y)
    INLINE bool
       StructuralConstraintPossibleEdges::checkArcAdditionAlone(NodeId x,
                                                                NodeId y) const {
      auto res = _PossibleEdges__possible_edges.empty()
                 || _PossibleEdges__possible_edges.exists(Edge(x, y));
      return res;
    }

    /// checks whether the constraints enable to remove arc (x,y)
    INLINE bool
       StructuralConstraintPossibleEdges::checkArcDeletionAlone(NodeId x,
                                                                NodeId y) const {
      return true;
    }

    /// checks whether the constraints enable to reverse arc (x,y)
    INLINE bool
       StructuralConstraintPossibleEdges::checkArcReversalAlone(NodeId x,
                                                                NodeId y) const {
      return _PossibleEdges__possible_edges.empty()
             || _PossibleEdges__possible_edges.exists(Edge(y, x));
    }

    /// checks whether the constraints enable to add an arc
    INLINE bool StructuralConstraintPossibleEdges::checkModificationAlone(
       const ArcAddition& change) const {
      return checkArcAdditionAlone(change.node1(), change.node2());
    }

    /// checks whether the constraints enable to remove an arc
    INLINE bool StructuralConstraintPossibleEdges::checkModificationAlone(
       const ArcDeletion& change) const {
      return checkArcDeletionAlone(change.node1(), change.node2());
    }

    /// checks whether the constraints enable to reverse an arc
    INLINE bool StructuralConstraintPossibleEdges::checkModificationAlone(
       const ArcReversal& change) const {
      return checkArcReversalAlone(change.node1(), change.node2());
    }

    /// checks whether the constraints enable to perform a graph change
    INLINE bool StructuralConstraintPossibleEdges::checkModificationAlone(
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
                    "supported by StructuralConstraintPossibleEdges");
      }
    }

    /// notify the constraint of a modification of the graph
    INLINE void StructuralConstraintPossibleEdges::modifyGraphAlone(
       const ArcAddition& change) {}

    /// notify the constraint of a modification of the graph
    INLINE void StructuralConstraintPossibleEdges::modifyGraphAlone(
       const ArcDeletion& change) {}

    /// notify the constraint of a modification of the graph
    INLINE void StructuralConstraintPossibleEdges::modifyGraphAlone(
       const ArcReversal& change) {}

    /// notify the constraint of a modification of the graph
    INLINE void StructuralConstraintPossibleEdges::modifyGraphAlone(
       const GraphChange& change) {}

    /// indicates whether a change will always violate the constraint
    INLINE bool StructuralConstraintPossibleEdges::isAlwaysInvalidAlone(
       const GraphChange& change) const {
      switch (change.type()) {
        case GraphChangeType::ARC_ADDITION:
          return !checkArcAdditionAlone(change.node1(), change.node2());

        case GraphChangeType::ARC_DELETION: return false;

        case GraphChangeType::ARC_REVERSAL:
          return !checkArcAdditionAlone(change.node2(), change.node1());

        default:
          GUM_ERROR(OperationNotAllowed,
                    "edge modifications are not supported "
                    "by StructuralConstraintPossibleEdges");
      }
    }

    /// assign a set of forbidden edge
    INLINE void StructuralConstraintPossibleEdges::setEdges(const EdgeSet& set) {
      _PossibleEdges__possible_edges = set;
    }

    /// assign a new forbidden edge
    INLINE void StructuralConstraintPossibleEdges::addEdge(const Edge& edge) {
      _PossibleEdges__possible_edges.insert(edge);
    }

    /// remove a forbidden edge
    INLINE void StructuralConstraintPossibleEdges::eraseEdge(const Edge& edge) {
      _PossibleEdges__possible_edges.erase(edge);
    }

    /// returns the set of possible edges
    INLINE const EdgeSet& StructuralConstraintPossibleEdges::edges() const {
      return _PossibleEdges__possible_edges;
    }

// include all the methods applicable to the whole class hierarchy
#  define GUM_CONSTRAINT_CLASS_NAME StructuralConstraintPossibleEdges
#  include <agrum/learning/constraints/structuralConstraintPatternRootInline.h>
#  undef GUM_CONSTRAINT_CLASS_NAME

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
