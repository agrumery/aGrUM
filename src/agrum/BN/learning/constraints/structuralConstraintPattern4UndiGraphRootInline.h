/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


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
INLINE void GUM_CONSTRAINT_CLASS_NAME::setGraph(const UndiGraph& graph) { setGraphAlone(graph); }

/// notify the constraint of a modification of the graph
INLINE void GUM_CONSTRAINT_CLASS_NAME::modifyGraph(const EdgeAddition& change) {
  modifyGraphAlone(change);
}

/// notify the constraint of a modification of the graph
INLINE void GUM_CONSTRAINT_CLASS_NAME::modifyGraph(const EdgeDeletion& change) {
  modifyGraphAlone(change);
}

/// notify the constraint of a modification of the graph
INLINE void GUM_CONSTRAINT_CLASS_NAME::modifyGraph(const GraphChange& change) {
  modifyGraphAlone(change);
}

/// checks whether the constraints enable to add arc (x,y)
INLINE bool GUM_CONSTRAINT_CLASS_NAME::checkEdgeAddition(NodeId x, NodeId y) const {
  return checkEdgeAdditionAlone(x, y);
}

/// checks whether the constraints enable to remove arc (x,y)
INLINE bool GUM_CONSTRAINT_CLASS_NAME::checkEdgeDeletion(NodeId x, NodeId y) const {
  return checkEdgeDeletionAlone(x, y);
}

/// checks whether the constraints enable to add an arc
INLINE bool GUM_CONSTRAINT_CLASS_NAME::checkModification(const EdgeAddition& change) const {
  return checkModificationAlone(change);
}

/// checks whether the constraints enable to remove an arc
INLINE bool GUM_CONSTRAINT_CLASS_NAME::checkModification(const EdgeDeletion& change) const {
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
