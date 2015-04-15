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

// this file should be included at the end of the StructuralConstraints inline
// files (see StructuralConstraintDiGraph to see how it should be included).
// This concerns only StructuralConstraints that do not derive from other
// StructuralConstraints, i.e., that do not derive from public virtual
// StructuralConstraintSetStatic<...>. Such a class is StructuralConstraintDiGraph.
// If your class derives from anoter constraint, include file
// StructuralConstraintPatternInline.h instead.

#ifdef GUM_CONSTRAINT_CLASS_NAME

/// sets a new graph from which we will perform checkings
INLINE void GUM_CONSTRAINT_CLASS_NAME::setGraph(const DiGraph &graph) {
  setGraphAlone(graph);
}

/// notify the constraint of a modification of the graph
INLINE void GUM_CONSTRAINT_CLASS_NAME::modifyGraph(const ArcAddition &change) {
  modifyGraphAlone(change);
}

/// notify the constraint of a modification of the graph
INLINE void GUM_CONSTRAINT_CLASS_NAME::modifyGraph(const ArcDeletion &change) {
  modifyGraphAlone(change);
}

/// notify the constraint of a modification of the graph
INLINE void GUM_CONSTRAINT_CLASS_NAME::modifyGraph(const ArcReversal &change) {
  modifyGraphAlone(change);
}

/// notify the constraint of a modification of the graph
INLINE void GUM_CONSTRAINT_CLASS_NAME::modifyGraph(const GraphChange &change) {
  modifyGraphAlone(change);
}

/// checks whether the constraints enable to add arc (x,y)
INLINE bool GUM_CONSTRAINT_CLASS_NAME::checkArcAddition(NodeId x, NodeId y) const
    noexcept {
  return checkArcAdditionAlone(x, y);
}

/// checks whether the constraints enable to remove arc (x,y)
INLINE bool GUM_CONSTRAINT_CLASS_NAME::checkArcDeletion(NodeId x, NodeId y) const
    noexcept {
  return checkArcDeletionAlone(x, y);
}

/// checks whether the constraints enable to reverse arc (x,y)
INLINE bool GUM_CONSTRAINT_CLASS_NAME::checkArcReversal(NodeId x, NodeId y) const
    noexcept {
  return checkArcReversalAlone(x, y);
}

/// checks whether the constraints enable to add an arc
INLINE bool
GUM_CONSTRAINT_CLASS_NAME::checkModification(const ArcAddition &change) const
    noexcept {
  return checkModificationAlone(change);
}

/// checks whether the constraints enable to remove an arc
INLINE bool
GUM_CONSTRAINT_CLASS_NAME::checkModification(const ArcDeletion &change) const
    noexcept {
  return checkModificationAlone(change);
}

/// checks whether the constraints enable to reverse an arc
INLINE bool
GUM_CONSTRAINT_CLASS_NAME::checkModification(const ArcReversal &change) const
    noexcept {
  return checkModificationAlone(change);
}

/// checks whether the constraints enable to perform a graph change
INLINE bool
GUM_CONSTRAINT_CLASS_NAME::checkModification(const GraphChange &change) const
    noexcept {
  return checkModificationAlone(change);
}

/// indicates whether a change will always violate the constraint
INLINE bool
GUM_CONSTRAINT_CLASS_NAME::isAlwaysInvalid(const GraphChange &change) const
    noexcept {
  return isAlwaysInvalidAlone(change);
}

#endif /* GUM_CONSTRAINT_CLASS_NAME */
