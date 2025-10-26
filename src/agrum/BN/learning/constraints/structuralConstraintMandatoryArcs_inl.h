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
#pragma once


/** @file
 * @brief the structural constraint indicating that some arcs shall never be
 * removed or reversed
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// sets a new graph from which we will perform checkings
    INLINE void StructuralConstraintMandatoryArcs::setGraphAlone(const DiGraph& graph) {}

    /// checks whether the constraints enable to add arc (x,y)
    INLINE bool StructuralConstraintMandatoryArcs::checkArcAdditionAlone(NodeId x, NodeId y) const {
      return true;
    }

    /// checks whether the constraints enable to remove arc (x,y)
    INLINE bool StructuralConstraintMandatoryArcs::checkArcDeletionAlone(NodeId x, NodeId y) const {
      return !_MandatoryArcs_mandatory_arcs_.exists(Arc(x, y));
    }

    /// checks whether the constraints enable to reverse arc (x,y)
    INLINE bool StructuralConstraintMandatoryArcs::checkArcReversalAlone(NodeId x, NodeId y) const {
      return !_MandatoryArcs_mandatory_arcs_.exists(Arc(x, y));
    }

    /// checks whether the constraints enable to add an arc
    INLINE bool
        StructuralConstraintMandatoryArcs::checkModificationAlone(const ArcAddition& change) const {
      return checkArcAdditionAlone(change.node1(), change.node2());
    }

    /// checks whether the constraints enable to remove an arc
    INLINE bool
        StructuralConstraintMandatoryArcs::checkModificationAlone(const ArcDeletion& change) const {
      return checkArcDeletionAlone(change.node1(), change.node2());
    }

    /// checks whether the constraints enable to reverse an arc
    INLINE bool
        StructuralConstraintMandatoryArcs::checkModificationAlone(const ArcReversal& change) const {
      return checkArcReversalAlone(change.node1(), change.node2());
    }

    /// checks whether the constraints enable to perform a graph change
    INLINE bool
        StructuralConstraintMandatoryArcs::checkModificationAlone(const GraphChange& change) const {
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
                    "supported by StructuralConstraintMandatoryArcs");
      }
    }

    /// notify the constraint of a modification of the graph
    INLINE void StructuralConstraintMandatoryArcs::modifyGraphAlone(const ArcAddition& change) {}

    /// notify the constraint of a modification of the graph
    INLINE void StructuralConstraintMandatoryArcs::modifyGraphAlone(const ArcDeletion& change) {}

    /// notify the constraint of a modification of the graph
    INLINE void StructuralConstraintMandatoryArcs::modifyGraphAlone(const ArcReversal& change) {}

    /// notify the constraint of a modification of the graph
    INLINE void StructuralConstraintMandatoryArcs::modifyGraphAlone(const GraphChange& change) {}

    /// indicates whether a change will always violate the constraint
    INLINE bool
        StructuralConstraintMandatoryArcs::isAlwaysInvalidAlone(const GraphChange& change) const {
      switch (change.type()) {
        case GraphChangeType::ARC_ADDITION : return false;

        case GraphChangeType::ARC_DELETION :
          return !checkArcDeletionAlone(change.node1(), change.node2());

        case GraphChangeType::ARC_REVERSAL :
          return !checkArcReversalAlone(change.node1(), change.node2());

        default :
          GUM_ERROR(OperationNotAllowed,
                    "edge modifications are not supported "
                    "by StructuralConstraintMandatoryArcs");
      }
    }

    /// assign a set of forbidden arcs
    INLINE void StructuralConstraintMandatoryArcs::setArcs(const ArcSet& set) {
      _MandatoryArcs_mandatory_arcs_ = set;
    }

    /// assign a new forbidden arc
    INLINE void StructuralConstraintMandatoryArcs::addArc(const Arc& arc) {
      _MandatoryArcs_mandatory_arcs_.insert(arc);
    }

    /// remove a forbidden arc
    INLINE void StructuralConstraintMandatoryArcs::eraseArc(const Arc& arc) {
      _MandatoryArcs_mandatory_arcs_.erase(arc);
    }

    /// returns the set of mandatory arcs
    INLINE const ArcSet& StructuralConstraintMandatoryArcs::arcs() const {
      return _MandatoryArcs_mandatory_arcs_;
    }

// include all the methods applicable to the whole class hierarchy
#  define GUM_CONSTRAINT_CLASS_NAME StructuralConstraintMandatoryArcs
#  include <agrum/BN/learning/constraints/structuralConstraintPatternRootInline.h>
#  undef GUM_CONSTRAINT_CLASS_NAME

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
