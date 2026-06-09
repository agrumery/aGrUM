/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
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

/// checks whether the constraints enable to apply an ArcTriangleDeletion1
INLINE bool GUM_CONSTRAINT_CLASS_NAME::checkArcTriangleDeletion1(NodeId node1,
                                                                 NodeId node2,
                                                                 NodeId node3) const {
  return constraints::checkArcTriangleDeletion1(node1, node2, node3)
      && checkArcTriangleDeletion1Alone(node1, node2, node3);
}

/// checks whether the constraints enable to apply an ArcTriangleDeletion2
INLINE bool GUM_CONSTRAINT_CLASS_NAME::checkArcTriangleDeletion2(NodeId node1,
                                                                 NodeId node2,
                                                                 NodeId node3) const {
  return constraints::checkArcTriangleDeletion2(node1, node2, node3)
      && checkArcTriangleDeletion2Alone(node1, node2, node3);
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
INLINE void GUM_CONSTRAINT_CLASS_NAME::modifyGraph(const ArcTriangleDeletion1& change) {
  constraints::modifyGraph(change);
  modifyGraphAlone(change);
}

/// notify the constraint of a modification of the graph
INLINE void GUM_CONSTRAINT_CLASS_NAME::modifyGraph(const ArcTriangleDeletion2& change) {
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

/// checks whether the constraints enable to apply an ArcTriangleDeletion1
INLINE bool GUM_CONSTRAINT_CLASS_NAME::checkModification(const ArcTriangleDeletion1& change) const {
  return checkArcTriangleDeletion1(change.node1(), change.node2(), change.node3());
}

/// checks whether the constraints enable to apply an ArcTriangleDeletion2
INLINE bool GUM_CONSTRAINT_CLASS_NAME::checkModification(const ArcTriangleDeletion2& change) const {
  return checkArcTriangleDeletion2(change.node1(), change.node2(), change.node3());
}

/// checks whether the constraints enable to perform a graph change
INLINE bool GUM_CONSTRAINT_CLASS_NAME::checkModification(const GraphChange& change) const {
  switch (change.type()) {
    case GraphChangeType::ARC_ADDITION : return checkArcAddition(change.node1(), change.node2());

    case GraphChangeType::ARC_DELETION : return checkArcDeletion(change.node1(), change.node2());

    case GraphChangeType::ARC_REVERSAL : return checkArcReversal(change.node1(), change.node2());

    case GraphChangeType::ARC_TRIANGLE_DELETION1 :
      return checkArcTriangleDeletion1(change.node1(), change.node2(), change.node3());

    case GraphChangeType::ARC_TRIANGLE_DELETION2 :
      return checkArcTriangleDeletion2(change.node1(), change.node2(), change.node3());

    default :
      GUM_ERROR(OperationNotAllowed, "edge modifications are not supported by the constraint")
  }
}

#endif /* GUM_CONSTRAINT_CLASS_NAME */
