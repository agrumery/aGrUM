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
// files (see StructuralConstraintDegree to see how it should be included).
// This concerns only StructuralConstraints that derive from other
// StructuralConstraints. If your class does not derive from anoter constraint,
// include file StructuralConstraintPattern4UndiGraphRootInline.h instead.

#ifdef GUM_CONSTRAINT_CLASS_NAME

/// sets a new graph from which we will perform checkings
INLINE void GUM_CONSTRAINT_CLASS_NAME::setGraph(const UndiGraph& graph) {
  constraints::setGraph(graph);
  setGraphAlone(graph);
}

/// checks whether the constraints enable to add edge (x,y)
INLINE bool GUM_CONSTRAINT_CLASS_NAME::checkEdgeAddition(NodeId x, NodeId y) const {
  return constraints::checkEdgeAddition(x, y) && checkEdgeAdditionAlone(x, y);
}

/// checks whether the constraints enable to remove edge (x,y)
INLINE bool GUM_CONSTRAINT_CLASS_NAME::checkEdgeDeletion(NodeId x, NodeId y) const {
  return constraints::checkEdgeDeletion(x, y) && checkEdgeDeletionAlone(x, y);
}

/// notify the constraint of a modification of the graph
INLINE void GUM_CONSTRAINT_CLASS_NAME::modifyGraph(const EdgeAddition& change) {
  constraints::modifyGraph(change);
  modifyGraphAlone(change);
}

/// notify the constraint of a modification of the graph
INLINE void GUM_CONSTRAINT_CLASS_NAME::modifyGraph(const EdgeDeletion& change) {
  constraints::modifyGraph(change);
  modifyGraphAlone(change);
}

/// notify the constraint of a modification of the graph
INLINE void GUM_CONSTRAINT_CLASS_NAME::modifyGraph(const GraphChange& change) {
  constraints::modifyGraph(change);
  modifyGraphAlone(change);
}

/// indicates whether a change will always violate the constraint
INLINE bool GUM_CONSTRAINT_CLASS_NAME::isAlwaysInvalid(const GraphChange&) const {
  return constraints::isAlwaysInvalid(change) || isAlwaysInvalidAlone(change);
}

/// checks whether the constraints enable to add an edge
INLINE bool GUM_CONSTRAINT_CLASS_NAME::checkModification(const EdgeAddition& change) const {
  return checkEdgeAddition(change.node1(), change.node2());
}

/// checks whether the constraints enable to remove an edge
INLINE bool GUM_CONSTRAINT_CLASS_NAME::checkModification(const EdgeDeletion& change) const {
  return checkEdgeDeletion(change.node1(), change.node2());
}

/// checks whether the constraints enable to perform a graph change
INLINE bool GUM_CONSTRAINT_CLASS_NAME::checkModification(const GraphChange& change) const {
  switch (change.type()) {
    case GraphChangeType::EDGE_ADDITION: return checkEdgeAddition(change.node1(), change.node2());

    case GraphChangeType::EDGE_DELETION: return checkEdgeDeletion(change.node1(), change.node2());

    default: GUM_ERROR(OperationNotAllowed, "arc modifications are not supported by the constraint")
  }
}

#endif /* GUM_CONSTRAINT_CLASS_NAME */
