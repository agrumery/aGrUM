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
// files (see StructuralConstraintDiGraph to see how it should be included).
// This concerns only StructuralConstraints that do not derive from other
// StructuralConstraints, i.e., that do not derive from public virtual
// StructuralConstraintSetStatic<...>. Such a class is
// StructuralConstraintDiGraph.
// If your class derives from anoter constraint, include file
// StructuralConstraintPatternInline.h instead.

#ifdef GUM_CONSTRAINT_CLASS_NAME

/// sets a new graph from which we will perform checkings
INLINE void GUM_CONSTRAINT_CLASS_NAME::setGraph(const DiGraph& graph) { setGraphAlone(graph); }

/// notify the constraint of a modification of the graph
INLINE void GUM_CONSTRAINT_CLASS_NAME::modifyGraph(const ArcAddition& change) {
  modifyGraphAlone(change);
}

/// notify the constraint of a modification of the graph
INLINE void GUM_CONSTRAINT_CLASS_NAME::modifyGraph(const ArcDeletion& change) {
  modifyGraphAlone(change);
}

/// notify the constraint of a modification of the graph
INLINE void GUM_CONSTRAINT_CLASS_NAME::modifyGraph(const ArcReversal& change) {
  modifyGraphAlone(change);
}

/// notify the constraint of a modification of the graph
INLINE void GUM_CONSTRAINT_CLASS_NAME::modifyGraph(const GraphChange& change) {
  modifyGraphAlone(change);
}

/// checks whether the constraints enable to add arc (x,y)
INLINE bool GUM_CONSTRAINT_CLASS_NAME::checkArcAddition(NodeId x, NodeId y) const {
  return checkArcAdditionAlone(x, y);
}

/// checks whether the constraints enable to remove arc (x,y)
INLINE bool GUM_CONSTRAINT_CLASS_NAME::checkArcDeletion(NodeId x, NodeId y) const {
  return checkArcDeletionAlone(x, y);
}

/// checks whether the constraints enable to reverse arc (x,y)
INLINE bool GUM_CONSTRAINT_CLASS_NAME::checkArcReversal(NodeId x, NodeId y) const {
  return checkArcReversalAlone(x, y);
}

/// checks whether the constraints enable to add an arc
INLINE bool GUM_CONSTRAINT_CLASS_NAME::checkModification(const ArcAddition& change) const {
  return checkModificationAlone(change);
}

/// checks whether the constraints enable to remove an arc
INLINE bool GUM_CONSTRAINT_CLASS_NAME::checkModification(const ArcDeletion& change) const {
  return checkModificationAlone(change);
}

/// checks whether the constraints enable to reverse an arc
INLINE bool GUM_CONSTRAINT_CLASS_NAME::checkModification(const ArcReversal& change) const {
  return checkModificationAlone(change);
}

/// checks whether the constraints enable to perform a graph change
INLINE bool GUM_CONSTRAINT_CLASS_NAME::checkModification(const GraphChange& change) const {
  return checkModificationAlone(change);
}

/// indicates whether a change will always violate the constraint
INLINE bool GUM_CONSTRAINT_CLASS_NAME::isAlwaysInvalid(const GraphChange& change) const {
  return isAlwaysInvalidAlone(change);
}

#endif /* GUM_CONSTRAINT_CLASS_NAME */
