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

#pragma once

/** @file
 * @brief the structural constraint imposing a total order over some nodes
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// sets a new graph from which we will perform checkings
    INLINE void StructuralConstraintTotalOrder::setGraphAlone(const DiGraph& graph) {}

    /// checks whether the constraints enable to add arc (x,y)
    INLINE bool StructuralConstraintTotalOrder::checkArcAdditionAlone(NodeId x, NodeId y) const {
      try {
        return _total_order_.pos(x) < _total_order_.pos(y);
      } catch (const Exception&) { return true; }
    }

    /// checks whether the constraints enable to remove arc (x,y)
    INLINE bool StructuralConstraintTotalOrder::checkArcDeletionAlone(NodeId x, NodeId y)  const{
      return true;
    }

    /// checks whether the constraints enable to reverse arc (x,y)
    INLINE bool StructuralConstraintTotalOrder::checkArcReversalAlone(NodeId x, NodeId y) const {
      try {
        return _total_order_.pos(y) < _total_order_.pos(x);
      } catch (const Exception&) { return true; }
    }

    /// checks whether the constraints enable to apply an ArcTriangleDeletion1
    INLINE bool StructuralConstraintTotalOrder::checkArcTriangleDeletion1Alone(NodeId node1,
                                                                               NodeId node2,
                                                                               NodeId node3) const {
      bool res = true;
      try {
        res = _total_order_.pos(node2) < _total_order_.pos(node1);
      } catch (const Exception&) {}
      try {
        return res && _total_order_.pos(node3) == _total_order_.pos(node1);
      } catch (const Exception&) { return true; }
      return res;
    }

    /// checks whether the constraints enable to apply an ArcTriangleDeletion2
    INLINE bool StructuralConstraintTotalOrder::checkArcTriangleDeletion2Alone(NodeId node1,
                                                                               NodeId node2,
                                                                               NodeId node3) const {
      try {
        return _total_order_.pos(node3) < _total_order_.pos(node2);
      } catch (const Exception&) { return true; }
    }

    /// notify the constraint of a modification of the graph
    INLINE void StructuralConstraintTotalOrder::modifyGraphAlone(const ArcAddition& change) {}

    /// notify the constraint of a modification of the graph
    INLINE void StructuralConstraintTotalOrder::modifyGraphAlone(const ArcDeletion& change) {}

    /// notify the constraint of a modification of the graph
    INLINE void StructuralConstraintTotalOrder::modifyGraphAlone(const ArcReversal& change) {}

    /// notify the constraint of a modification of the graph
    INLINE void
        StructuralConstraintTotalOrder::modifyGraphAlone(const ArcTriangleDeletion1& change) {}

    /// notify the constraint of a modification of the graph
    INLINE void
        StructuralConstraintTotalOrder::modifyGraphAlone(const ArcTriangleDeletion2& change) {}

    /// notify the constraint of a modification of the graph
    INLINE void StructuralConstraintTotalOrder::modifyGraphAlone(const GraphChange& change) {}

    /// indicates whether a change will always violate the constraint
    INLINE bool StructuralConstraintTotalOrder::isAlwaysInvalidAlone(const GraphChange& change) const {
      switch (change.type()) {
        case GraphChangeType::ARC_ADDITION :
          return !checkArcAdditionAlone(change.node1(), change.node2());

        case GraphChangeType::ARC_DELETION : return false;

        case GraphChangeType::ARC_REVERSAL :
          return !checkArcAdditionAlone(change.node2(), change.node1());

        case GraphChangeType::ARC_TRIANGLE_DELETION1 :
          return !checkArcTriangleDeletion1Alone(change.node1(), change.node2(), change.node3());

        case GraphChangeType::ARC_TRIANGLE_DELETION2 :
          return !checkArcTriangleDeletion2Alone(change.node1(), change.node2(), change.node3());

        default :
          GUM_ERROR(OperationNotAllowed,
                    "Graph change operation " << change.typeAsString()
                                              << " is not supported by TotalOrder constraints");
      }
    }

    /// checks whether the constraints enable to add an arc
    INLINE bool StructuralConstraintTotalOrder::checkModificationAlone(const ArcAddition& change) const {
      return checkArcAdditionAlone(change.node1(), change.node2());
    }

    /// checks whether the constraints enable to remove an arc
    INLINE bool StructuralConstraintTotalOrder::checkModificationAlone(const ArcDeletion& change) const {
      return checkArcDeletionAlone(change.node1(), change.node2());
    }

    /// checks whether the constraints enable to reverse an arc
    INLINE bool StructuralConstraintTotalOrder::checkModificationAlone(const ArcReversal& change) const {
      return checkArcReversalAlone(change.node1(), change.node2());
    }

    /// checks whether the constraints enable to apply an ArcTriangleDeletion1
    INLINE bool
        StructuralConstraintTotalOrder::checkModificationAlone(const ArcTriangleDeletion1& change) const {
      return checkArcTriangleDeletion1Alone(change.node1(), change.node2(), change.node3());
    }

    /// checks whether the constraints enable to apply an ArcTriangleDeletion2
    INLINE bool
        StructuralConstraintTotalOrder::checkModificationAlone(const ArcTriangleDeletion2& change) const {
      return checkArcTriangleDeletion2Alone(change.node1(), change.node2(), change.node3());
    }

    /// checks whether the constraints enable to perform a graph change
    INLINE bool StructuralConstraintTotalOrder::checkModificationAlone(const GraphChange& change) const {
      switch (change.type()) {
        case GraphChangeType::ARC_ADDITION :
          return checkArcAdditionAlone(change.node1(), change.node2());

        case GraphChangeType::ARC_DELETION :
          return checkArcDeletionAlone(change.node1(), change.node2());

        case GraphChangeType::ARC_REVERSAL :
          return checkArcReversalAlone(change.node1(), change.node2());

        case GraphChangeType::ARC_TRIANGLE_DELETION1 :
          return checkArcTriangleDeletion1Alone(change.node1(), change.node2(), change.node3());

        case GraphChangeType::ARC_TRIANGLE_DELETION2 :
          return checkArcTriangleDeletion2Alone(change.node1(), change.node2(), change.node3());

        default :
          GUM_ERROR(OperationNotAllowed,
                    "edge modifications are not "
                    "supported by StructuralConstraintTotalOrder");
      }
    }

    /// sets the Total order of all the nodes in the property
    INLINE void StructuralConstraintTotalOrder::setTotalOrder(const Sequence< NodeId >& order) {
      _total_order_ = order;
    }

    /// returns the current Total order
    INLINE const Sequence< NodeId >& StructuralConstraintTotalOrder::totalOrder() const {
      return _total_order_;
    }

// include all the methods applicable to the whole class hierarchy
#  define GUM_CONSTRAINT_CLASS_NAME StructuralConstraintTotalOrder
#  include <agrum/BN/learning/constraints/structuralConstraintPatternRootInline.h>
#  undef GUM_CONSTRAINT_CLASS_NAME

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
