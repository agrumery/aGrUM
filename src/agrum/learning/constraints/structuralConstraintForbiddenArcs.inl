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
/** @file
 * @brief the structural constraint for forbidding the creation of some arcs
 * during structure learning
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// sets a new graph from which we will perform checkings
    INLINE void
    StructuralConstraintForbiddenArcs::setGraphAlone(const DiGraph &graph) {}

    /// checks whether the constraints enable to add arc (x,y)
    INLINE bool StructuralConstraintForbiddenArcs::checkArcAdditionAlone(
        NodeId x, NodeId y) const noexcept {
      return !_ForbiddenArcs__forbidden_arcs.exists(Arc(x, y));
    }

    /// checks whether the constraints enable to remove arc (x,y)
    INLINE bool StructuralConstraintForbiddenArcs::checkArcDeletionAlone(
        NodeId x, NodeId y) const noexcept {
      return true;
    }

    /// checks whether the constraints enable to reverse arc (x,y)
    INLINE bool StructuralConstraintForbiddenArcs::checkArcReversalAlone(
        NodeId x, NodeId y) const noexcept {
      return !_ForbiddenArcs__forbidden_arcs.exists(Arc(y, x));
    }

    /// checks whether the constraints enable to add an arc
    INLINE bool StructuralConstraintForbiddenArcs::checkModificationAlone(
        const ArcAddition &change) const noexcept {
      return checkArcAdditionAlone(change.node1(), change.node2());
    }

    /// checks whether the constraints enable to remove an arc
    INLINE bool StructuralConstraintForbiddenArcs::checkModificationAlone(
        const ArcDeletion &change) const noexcept {
      return checkArcDeletionAlone(change.node1(), change.node2());
    }

    /// checks whether the constraints enable to reverse an arc
    INLINE bool StructuralConstraintForbiddenArcs::checkModificationAlone(
        const ArcReversal &change) const noexcept {
      return checkArcReversalAlone(change.node1(), change.node2());
    }

    /// checks whether the constraints enable to perform a graph change
    INLINE bool StructuralConstraintForbiddenArcs::checkModificationAlone(
        const GraphChange &change) const noexcept {
      switch (change.type()) {
        case GraphChangeType::ARC_ADDITION:
          return checkArcAdditionAlone(change.node1(), change.node2());

        case GraphChangeType::ARC_DELETION:
          return checkArcDeletionAlone(change.node1(), change.node2());

        case GraphChangeType::ARC_REVERSAL:
          return checkArcReversalAlone(change.node1(), change.node2());

        default:
          GUM_ERROR(OperationNotAllowed,
                    "edge modifications are not "
                    "supported by StructuralConstraintForbiddenArcs");
      }
    }

    /// notify the constraint of a modification of the graph
    INLINE void
    StructuralConstraintForbiddenArcs::modifyGraphAlone(const ArcAddition &change) {}

    /// notify the constraint of a modification of the graph
    INLINE void
    StructuralConstraintForbiddenArcs::modifyGraphAlone(const ArcDeletion &change) {}

    /// notify the constraint of a modification of the graph
    INLINE void
    StructuralConstraintForbiddenArcs::modifyGraphAlone(const ArcReversal &change) {}

    /// notify the constraint of a modification of the graph
    INLINE void
    StructuralConstraintForbiddenArcs::modifyGraphAlone(const GraphChange &change) {}

    /// indicates whether a change will always violate the constraint
    INLINE bool StructuralConstraintForbiddenArcs::isAlwaysInvalidAlone(
        const GraphChange &change) const noexcept {
      switch (change.type()) {
        case GraphChangeType::ARC_ADDITION:
          return !checkArcAdditionAlone(change.node1(), change.node2());

        case GraphChangeType::ARC_DELETION:
          return false;

        case GraphChangeType::ARC_REVERSAL:
          return !checkArcAdditionAlone(change.node2(), change.node1());

        default:
          GUM_ERROR(OperationNotAllowed, "edge modifications are not supported "
                                         "by StructuralConstraintForbiddenArcs");
      }
    }

    /// assign a set of forbidden arcs
    INLINE void StructuralConstraintForbiddenArcs::setArcs(const ArcSet &set) {
      _ForbiddenArcs__forbidden_arcs = set;
    }

    /// assign a new forbidden arc
    INLINE void StructuralConstraintForbiddenArcs::addArc(const Arc &arc) {
      _ForbiddenArcs__forbidden_arcs.insert(arc);
    }

    /// remove a forbidden arc
    INLINE void StructuralConstraintForbiddenArcs::eraseArc(const Arc &arc) {
      _ForbiddenArcs__forbidden_arcs.erase(arc);
    }

    /// returns the set of mandatory arcs
    INLINE const ArcSet &StructuralConstraintForbiddenArcs::arcs() const noexcept {
      return _ForbiddenArcs__forbidden_arcs;
    }

// include all the methods applicable to the whole class hierarchy
#define GUM_CONSTRAINT_CLASS_NAME StructuralConstraintForbiddenArcs
#include <agrum/learning/constraints/structuralConstraintPatternRootInline.h>
#undef GUM_CONSTRAINT_CLASS_NAME

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
