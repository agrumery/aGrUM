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
// files (see StructuralConstraintUndiGraph to see how it should be included).
// This concerns only StructuralConstraints that do not derive from other
// StructuralConstraints, i.e., that do not derive from public virtual
// StructuralConstraintSetStatic<...>. Such a class is
// StructuralConstraintUndiGraph.
// If your class derives from anoter constraint, include file
// StructuralConstraintPattern4UndiGraphInline.h instead.

#ifdef GUM_CONSTRAINT_CLASS_NAME

/// sets a new graph from which we will perform checkings
INLINE void GUM_CONSTRAINT_CLASS_NAME::setGraph(const UndiGraph &graph) {
  setGraphAlone(graph);
}

/// notify the constraint of a modification of the graph
INLINE void GUM_CONSTRAINT_CLASS_NAME::modifyGraph(const EdgeAddition &change) {
  modifyGraphAlone(change);
}

/// notify the constraint of a modification of the graph
INLINE void GUM_CONSTRAINT_CLASS_NAME::modifyGraph(const EdgeDeletion &change) {
  modifyGraphAlone(change);
}

/// notify the constraint of a modification of the graph
INLINE void GUM_CONSTRAINT_CLASS_NAME::modifyGraph(const GraphChange &change) {
  modifyGraphAlone(change);
}

/// checks whether the constraints enable to add arc (x,y)
INLINE bool GUM_CONSTRAINT_CLASS_NAME::checkEdgeAddition(NodeId x, NodeId y) const
    noexcept {
  return checkEdgeAdditionAlone(x, y);
}

/// checks whether the constraints enable to remove arc (x,y)
INLINE bool GUM_CONSTRAINT_CLASS_NAME::checkEdgeDeletion(NodeId x, NodeId y) const
    noexcept {
  return checkEdgeDeletionAlone(x, y);
}

/// checks whether the constraints enable to add an arc
INLINE bool
GUM_CONSTRAINT_CLASS_NAME::checkModification(const EdgeAddition &change) const
    noexcept {
  return checkModificationAlone(change);
}

/// checks whether the constraints enable to remove an arc
INLINE bool
GUM_CONSTRAINT_CLASS_NAME::checkModification(const EdgeDeletion &change) const
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
