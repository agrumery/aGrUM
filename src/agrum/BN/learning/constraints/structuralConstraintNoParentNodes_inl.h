/**
 *
 *   Copyright (c) 2005-2024  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
#include <agrum/BN/learning/constraints/structuralConstraintNoParentNodes.h>

/** @file
 * @brief the structural constraint for forbidding the creation of some arcs
 * during structure learning
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum::learning {

  /// sets a new graph from which we will perform checkings
  INLINE void StructuralConstraintNoParentNodes::setGraphAlone(const DiGraph& graph) {}

  /// checks whether the constraints enable to add arc (x,y)
  INLINE bool StructuralConstraintNoParentNodes::checkArcAdditionAlone(NodeId x, NodeId y) const {
    return !_noParentNodes_.exists(y);
  }

  /// checks whether the constraints enable to remove arc (x,y)
  INLINE bool StructuralConstraintNoParentNodes::checkArcDeletionAlone(NodeId x, NodeId y) const {
    return true;
  }

  /// checks whether the constraints enable to reverse arc (x,y)
  INLINE bool StructuralConstraintNoParentNodes::checkArcReversalAlone(NodeId x, NodeId y) const {
    return !_noParentNodes_.exists(x);
  }

  /// checks whether the constraints enable to add an arc
  INLINE bool
      StructuralConstraintNoParentNodes::checkModificationAlone(const ArcAddition& change) const {
    return checkArcAdditionAlone(change.node1(), change.node2());
  }

  /// checks whether the constraints enable to remove an arc
  INLINE bool
      StructuralConstraintNoParentNodes::checkModificationAlone(const ArcDeletion& change) const {
    return checkArcDeletionAlone(change.node1(), change.node2());
  }

  /// checks whether the constraints enable to reverse an arc
  INLINE bool
      StructuralConstraintNoParentNodes::checkModificationAlone(const ArcReversal& change) const {
    return checkArcReversalAlone(change.node1(), change.node2());
  }

  /// checks whether the constraints enable to perform a graph change
  INLINE bool
      StructuralConstraintNoParentNodes::checkModificationAlone(const GraphChange& change) const {
    switch (change.type()) {
      case GraphChangeType::ARC_ADDITION :
        return checkArcAdditionAlone(change.node1(), change.node2());

      case GraphChangeType::ARC_DELETION :
        return checkArcDeletionAlone(change.node1(), change.node2());

      case GraphChangeType::ARC_REVERSAL :
        return checkArcReversalAlone(change.node1(), change.node2());

      default :
        GUM_ERROR(OperationNotAllowed,
                  "edge modifications are not "
                  "supported by StructuralConstraintNoParentNodes");
    }
  }

  /// notify the constraint of a modification of the graph
  INLINE void StructuralConstraintNoParentNodes::modifyGraphAlone(const ArcAddition& change) {}

  /// notify the constraint of a modification of the graph
  INLINE void StructuralConstraintNoParentNodes::modifyGraphAlone(const ArcDeletion& change) {}

  /// notify the constraint of a modification of the graph
  INLINE void StructuralConstraintNoParentNodes::modifyGraphAlone(const ArcReversal& change) {}

  /// notify the constraint of a modification of the graph
  INLINE void StructuralConstraintNoParentNodes::modifyGraphAlone(const GraphChange& change) {}

  /// indicates whether a change will always violate the constraint
  INLINE bool
      StructuralConstraintNoParentNodes::isAlwaysInvalidAlone(const GraphChange& change) const {
    switch (change.type()) {
      case GraphChangeType::ARC_ADDITION :
        return !checkArcAdditionAlone(change.node1(), change.node2());

      case GraphChangeType::ARC_DELETION :
        return checkArcDeletionAlone(change.node1(), change.node2());

      case GraphChangeType::ARC_REVERSAL :
        return !checkArcAdditionAlone(change.node2(), change.node1());

      default :
        GUM_ERROR(OperationNotAllowed,
                  "edge modifications are not supported "
                  "by StructuralConstraintNoParentNodes");
    }
  }

  /// assign a set of forbidden arcs
  INLINE void StructuralConstraintNoParentNodes::setNodes(const NodeSet& set) {
    _noParentNodes_ = set;
  }

  /// assign a new forbidden arc
  INLINE void StructuralConstraintNoParentNodes::addNode(NodeId node) {
    _noParentNodes_.insert(node);
  }

  /// remove a forbidden arc
  INLINE void StructuralConstraintNoParentNodes::eraseNode(NodeId node) {
    _noParentNodes_.erase(node);
  }

  /// returns the set of mandatory arcs
  INLINE const NodeSet& StructuralConstraintNoParentNodes::nodes() const { return _noParentNodes_; }

// include all the methods applicable to the whole class hierarchy
#  define GUM_CONSTRAINT_CLASS_NAME StructuralConstraintNoParentNodes
#  include <agrum/BN/learning/constraints/structuralConstraintPatternRootInline.h>
#  undef GUM_CONSTRAINT_CLASS_NAME

}   // namespace gum::learning


#endif /* DOXYGEN_SHOULD_SKIP_THIS */