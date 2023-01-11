/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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


// this file should be included at the end of the StructuralConstraints inline
// files (see StructuralConstraintDAG to see how it should be included).
// This concerns only StructuralConstraints that derive from other
// StructuralConstraints. If your class does not derive from anoter constraint,
// include file StructuralConstraintPatternRootInline.h instead.

#ifdef GUM_CONSTRAINT_CLASS_NAME

/// sets a new graph from which we will perform checkings
INLINE void GUM_CONSTRAINT_CLASS_NAME::setGraph(const DiGraph& graph) {
  constraints::setGraph(graph);
  setGraphAlone(graph);
}

/// checks whether the constraints enable to add arc (x,y)
INLINE bool GUM_CONSTRAINT_CLASS_NAME::checkArcAddition(NodeId x, NodeId y) const {
  return constraints::checkArcAddition(x, y) && checkArcAdditionAlone(x, y);
}

/// checks whether the constraints enable to remove arc (x,y)
INLINE bool GUM_CONSTRAINT_CLASS_NAME::checkArcDeletion(NodeId x, NodeId y) const {
  return constraints::checkArcDeletion(x, y) && checkArcDeletionAlone(x, y);
}

/// checks whether the constraints enable to reverse arc (x,y)
INLINE bool GUM_CONSTRAINT_CLASS_NAME::checkArcReversal(NodeId x, NodeId y) const {
  return constraints::checkArcReversal(x, y) && checkArcReversalAlone(x, y);
}

/// notify the constraint of a modification of the graph
INLINE void GUM_CONSTRAINT_CLASS_NAME::modifyGraph(const ArcAddition& change) {
  constraints::modifyGraph(change);
  modifyGraphAlone(change);
}

/// notify the constraint of a modification of the graph
INLINE void GUM_CONSTRAINT_CLASS_NAME::modifyGraph(const ArcDeletion& change) {
  constraints::modifyGraph(change);
  modifyGraphAlone(change);
}

/// notify the constraint of a modification of the graph
INLINE void GUM_CONSTRAINT_CLASS_NAME::modifyGraph(const ArcReversal& change) {
  constraints::modifyGraph(change);
  modifyGraphAlone(change);
}

/// notify the constraint of a modification of the graph
INLINE void GUM_CONSTRAINT_CLASS_NAME::modifyGraph(const GraphChange& change) {
  constraints::modifyGraph(change);
  modifyGraphAlone(change);
}

/// indicates whether a change will always violate the constraint
INLINE bool GUM_CONSTRAINT_CLASS_NAME::isAlwaysInvalid(const GraphChange& change) const {
  return constraints::isAlwaysInvalid(change) || isAlwaysInvalidAlone(change);
}

/// checks whether the constraints enable to add an arc
INLINE bool GUM_CONSTRAINT_CLASS_NAME::checkModification(const ArcAddition& change) const {
  return checkArcAddition(change.node1(), change.node2());
}

/// checks whether the constraints enable to remove an arc
INLINE bool GUM_CONSTRAINT_CLASS_NAME::checkModification(const ArcDeletion& change) const {
  return checkArcDeletion(change.node1(), change.node2());
}

/// checks whether the constraints enable to reverse an arc
INLINE bool GUM_CONSTRAINT_CLASS_NAME::checkModification(const ArcReversal& change) const {
  return checkArcReversal(change.node1(), change.node2());
}

/// checks whether the constraints enable to perform a graph change
INLINE bool GUM_CONSTRAINT_CLASS_NAME::checkModification(const GraphChange& change) const {
  switch (change.type()) {
    case GraphChangeType::ARC_ADDITION: return checkArcAddition(change.node1(), change.node2());

    case GraphChangeType::ARC_DELETION: return checkArcDeletion(change.node1(), change.node2());

    case GraphChangeType::ARC_REVERSAL: return checkArcReversal(change.node1(), change.node2());

    default:
      GUM_ERROR(OperationNotAllowed, "edge modifications are not supported by the constraint")
  }
}

#endif /* GUM_CONSTRAINT_CLASS_NAME */
