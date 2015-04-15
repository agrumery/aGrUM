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
 * @brief the "meta-programming" class for storing several structural constraints
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <iostream>

namespace gum {

  namespace learning {

    /// default constructor
    template <typename CONSTRAINT1, typename... OTHER_CONSTRAINTS>
    INLINE __StructuralConstraintSetStatic<
        CONSTRAINT1, OTHER_CONSTRAINTS...>::__StructuralConstraintSetStatic() {}

    /// copy constructor
    template <typename CONSTRAINT1, typename... OTHER_CONSTRAINTS>
    INLINE __StructuralConstraintSetStatic<CONSTRAINT1, OTHER_CONSTRAINTS...>::
        __StructuralConstraintSetStatic(const __StructuralConstraintSetStatic<
            CONSTRAINT1, OTHER_CONSTRAINTS...> &from)
        : CONSTRAINT1(from),
          __StructuralConstraintSetStatic<OTHER_CONSTRAINTS...>(from) {}

    /// destructor
    template <typename CONSTRAINT1, typename... OTHER_CONSTRAINTS>
    INLINE __StructuralConstraintSetStatic<
        CONSTRAINT1, OTHER_CONSTRAINTS...>::~__StructuralConstraintSetStatic() {}

    /// copy operator
    template <typename CONSTRAINT1, typename... OTHER_CONSTRAINTS>
    INLINE __StructuralConstraintSetStatic<CONSTRAINT1, OTHER_CONSTRAINTS...> &
        __StructuralConstraintSetStatic<CONSTRAINT1, OTHER_CONSTRAINTS...>::
        operator=(const __StructuralConstraintSetStatic<
            CONSTRAINT1, OTHER_CONSTRAINTS...> &from) {
      if (this != &from) {
        next_constraints::operator=(from);
        first_constraint::operator=(from);
      }

      return *this;
    }

    /// sets a new graph from which we will perform checkings
    template <typename CONSTRAINT1, typename... OTHER_CONSTRAINTS>
    INLINE void
    __StructuralConstraintSetStatic<CONSTRAINT1, OTHER_CONSTRAINTS...>::setGraph(
        const DiGraph &graph) {
      next_constraints::setGraph(graph);
      first_constraint::setGraphAlone(graph);
    }

    /// notify the constraint of a modification of the graph
    template <typename CONSTRAINT1, typename... OTHER_CONSTRAINTS>
    INLINE void
    __StructuralConstraintSetStatic<CONSTRAINT1, OTHER_CONSTRAINTS...>::modifyGraph(
        const ArcAddition &change) {
      next_constraints::modifyGraph(change);
      first_constraint::modifyGraphAlone(change);
    }

    /// notify the constraint of a modification of the graph
    template <typename CONSTRAINT1, typename... OTHER_CONSTRAINTS>
    INLINE void
    __StructuralConstraintSetStatic<CONSTRAINT1, OTHER_CONSTRAINTS...>::modifyGraph(
        const ArcDeletion &change) {
      next_constraints::modifyGraph(change);
      first_constraint::modifyGraphAlone(change);
    }

    /// notify the constraint of a modification of the graph
    template <typename CONSTRAINT1, typename... OTHER_CONSTRAINTS>
    INLINE void
    __StructuralConstraintSetStatic<CONSTRAINT1, OTHER_CONSTRAINTS...>::modifyGraph(
        const ArcReversal &change) {
      next_constraints::modifyGraph(change);
      first_constraint::modifyGraphAlone(change);
    }

    /// notify the constraint of a modification of the graph
    template <typename CONSTRAINT1, typename... OTHER_CONSTRAINTS>
    INLINE void
    __StructuralConstraintSetStatic<CONSTRAINT1, OTHER_CONSTRAINTS...>::modifyGraph(
        const GraphChange &change) {
      next_constraints::modifyGraph(change);
      first_constraint::modifyGraphAlone(change);
    }

    /// indicates whether a change will always violate the constraint
    template <typename CONSTRAINT1, typename... OTHER_CONSTRAINTS>
    INLINE bool __StructuralConstraintSetStatic<
        CONSTRAINT1,
        OTHER_CONSTRAINTS...>::isAlwaysInvalid(const GraphChange &change) const
        noexcept {
      return next_constraints::isAlwaysInvalid(change) ||
             first_constraint::isAlwaysInvalidAlone(change);
    }

    /// checks whether the constraints enable to add arc (x,y)
    template <typename CONSTRAINT1, typename... OTHER_CONSTRAINTS>
    INLINE bool __StructuralConstraintSetStatic<
        CONSTRAINT1, OTHER_CONSTRAINTS...>::checkArcAddition(NodeId x,
                                                             NodeId y) const
        noexcept {
      return next_constraints::checkArcAddition(x, y) &&
             first_constraint::checkArcAdditionAlone(x, y);
    }

    /// checks whether the constraints enable to remove arc (x,y)
    template <typename CONSTRAINT1, typename... OTHER_CONSTRAINTS>
    INLINE bool __StructuralConstraintSetStatic<
        CONSTRAINT1, OTHER_CONSTRAINTS...>::checkArcDeletion(NodeId x,
                                                             NodeId y) const
        noexcept {
      return next_constraints::checkArcDeletion(x, y) &&
             first_constraint::checkArcDeletionAlone(x, y);
    }

    /// checks whether the constraints enable to reverse arc (x,y)
    template <typename CONSTRAINT1, typename... OTHER_CONSTRAINTS>
    INLINE bool __StructuralConstraintSetStatic<
        CONSTRAINT1, OTHER_CONSTRAINTS...>::checkArcReversal(NodeId x,
                                                             NodeId y) const
        noexcept {
      return next_constraints::checkArcReversal(x, y) &&
             first_constraint::checkArcReversalAlone(x, y);
    }

    /// checks whether the constraints enable to add an arc
    template <typename CONSTRAINT1, typename... OTHER_CONSTRAINTS>
    INLINE bool __StructuralConstraintSetStatic<
        CONSTRAINT1,
        OTHER_CONSTRAINTS...>::checkModification(const ArcAddition &change) const
        noexcept {
      return next_constraints::checkModification(change) &&
             first_constraint::checkModificationAlone(change);
    }

    /// checks whether the constraints enable to remove an arc
    template <typename CONSTRAINT1, typename... OTHER_CONSTRAINTS>
    INLINE bool __StructuralConstraintSetStatic<
        CONSTRAINT1,
        OTHER_CONSTRAINTS...>::checkModification(const ArcDeletion &change) const
        noexcept {
      return next_constraints::checkModification(change) &&
             first_constraint::checkModificationAlone(change);
    }

    /// checks whether the constraints enable to reverse an arc
    template <typename CONSTRAINT1, typename... OTHER_CONSTRAINTS>
    INLINE bool __StructuralConstraintSetStatic<
        CONSTRAINT1,
        OTHER_CONSTRAINTS...>::checkModification(const ArcReversal &change) const
        noexcept {
      return next_constraints::checkModification(change) &&
             first_constraint::checkModificationAlone(change);
    }

    /// checks whether the constraints enable to perform a graph change
    template <typename CONSTRAINT1, typename... OTHER_CONSTRAINTS>
    INLINE bool __StructuralConstraintSetStatic<
        CONSTRAINT1,
        OTHER_CONSTRAINTS...>::checkModification(const GraphChange &change) const
        noexcept {
      return next_constraints::checkModification(change) &&
             first_constraint::checkModificationAlone(change);
    }

    // ===========================================================================

    /// default constructor
    template <typename CONSTRAINT>
    INLINE
    __StructuralConstraintSetStatic<CONSTRAINT>::__StructuralConstraintSetStatic() {}

    /// copy constructor
    template <typename CONSTRAINT>
    INLINE
    __StructuralConstraintSetStatic<CONSTRAINT>::__StructuralConstraintSetStatic(
        const __StructuralConstraintSetStatic<CONSTRAINT> &from)
        : CONSTRAINT(from) {}

    /// destructor
    template <typename CONSTRAINT>
    INLINE __StructuralConstraintSetStatic<
        CONSTRAINT>::~__StructuralConstraintSetStatic() {}

    /// copy operator
    template <typename CONSTRAINT>
    INLINE __StructuralConstraintSetStatic<CONSTRAINT> &
        __StructuralConstraintSetStatic<CONSTRAINT>::
        operator=(const __StructuralConstraintSetStatic<CONSTRAINT> &from) {
      if (this != &from) {
        CONSTRAINT::operator=(from);
      }

      return *this;
    }

    /// sets a new graph from which we will perform checkings
    template <typename CONSTRAINT>
    INLINE void
    __StructuralConstraintSetStatic<CONSTRAINT>::setGraph(const DiGraph &graph) {
      first_constraint::setGraphAlone(graph);
    }

    /// notify the constraint of a modification of the graph
    template <typename CONSTRAINT>
    INLINE void __StructuralConstraintSetStatic<CONSTRAINT>::modifyGraph(
        const ArcAddition &change) {
      first_constraint::modifyGraphAlone(change);
    }

    /// notify the constraint of a modification of the graph
    template <typename CONSTRAINT>
    INLINE void __StructuralConstraintSetStatic<CONSTRAINT>::modifyGraph(
        const ArcDeletion &change) {
      first_constraint::modifyGraphAlone(change);
    }

    /// notify the constraint of a modification of the graph
    template <typename CONSTRAINT>
    INLINE void __StructuralConstraintSetStatic<CONSTRAINT>::modifyGraph(
        const ArcReversal &change) {
      first_constraint::modifyGraphAlone(change);
    }

    /// notify the constraint of a modification of the graph
    template <typename CONSTRAINT>
    INLINE void __StructuralConstraintSetStatic<CONSTRAINT>::modifyGraph(
        const GraphChange &change) {
      first_constraint::modifyGraphAlone(change);
    }

    /// indicates whether a change will always violate the constraint
    template <typename CONSTRAINT>
    INLINE bool __StructuralConstraintSetStatic<CONSTRAINT>::isAlwaysInvalid(
        const GraphChange &change) const noexcept {
      return first_constraint::isAlwaysInvalidAlone(change);
    }

    /// checks whether the constraints enable to add arc (x,y)
    template <typename CONSTRAINT>
    INLINE bool __StructuralConstraintSetStatic<CONSTRAINT>::checkArcAddition(
        NodeId x, NodeId y) const noexcept {
      return first_constraint::checkArcAdditionAlone(x, y);
    }

    /// checks whether the constraints enable to remove arc (x,y)
    template <typename CONSTRAINT>
    INLINE bool __StructuralConstraintSetStatic<CONSTRAINT>::checkArcDeletion(
        NodeId x, NodeId y) const noexcept {
      return first_constraint::checkArcDeletionAlone(x, y);
    }

    /// checks whether the constraints enable to reverse arc (x,y)
    template <typename CONSTRAINT>
    INLINE bool __StructuralConstraintSetStatic<CONSTRAINT>::checkArcReversal(
        NodeId x, NodeId y) const noexcept {
      return first_constraint::checkArcReversalAlone(x, y);
    }

    /// checks whether the constraints enable to add an arc
    template <typename CONSTRAINT>
    INLINE bool __StructuralConstraintSetStatic<CONSTRAINT>::checkModification(
        const ArcAddition &change) const noexcept {
      return first_constraint::checkModificationAlone(change);
    }

    /// checks whether the constraints enable to remove an arc
    template <typename CONSTRAINT>
    INLINE bool __StructuralConstraintSetStatic<CONSTRAINT>::checkModification(
        const ArcDeletion &change) const noexcept {
      return first_constraint::checkModificationAlone(change);
    }

    /// checks whether the constraints enable to reverse an arc
    template <typename CONSTRAINT>
    INLINE bool __StructuralConstraintSetStatic<CONSTRAINT>::checkModification(
        const ArcReversal &change) const noexcept {
      return first_constraint::checkModificationAlone(change);
    }

    /// checks whether the constraints enable to perform a graph change
    template <typename CONSTRAINT>
    INLINE bool __StructuralConstraintSetStatic<CONSTRAINT>::checkModification(
        const GraphChange &change) const noexcept {
      return first_constraint::checkModificationAlone(change);
    }

    // ===========================================================================

    /// default constructor
    template <typename CONSTRAINT1, typename... OTHER_CONSTRAINTS>
    INLINE StructuralConstraintSetStatic<
        CONSTRAINT1, OTHER_CONSTRAINTS...>::StructuralConstraintSetStatic() {
      GUM_CONSTRUCTOR(StructuralConstraintSetStatic);
    }

    /// copy constructor
    template <typename CONSTRAINT1, typename... OTHER_CONSTRAINTS>
    INLINE StructuralConstraintSetStatic<CONSTRAINT1, OTHER_CONSTRAINTS...>::
        StructuralConstraintSetStatic(const StructuralConstraintSetStatic<
            CONSTRAINT1, OTHER_CONSTRAINTS...> &from)
        : constraints(from) {
      GUM_CONS_CPY(StructuralConstraintSetStatic);
    }

    /// destructor
    template <typename CONSTRAINT1, typename... OTHER_CONSTRAINTS>
    INLINE StructuralConstraintSetStatic<
        CONSTRAINT1, OTHER_CONSTRAINTS...>::~StructuralConstraintSetStatic() {
      GUM_DESTRUCTOR(StructuralConstraintSetStatic);
    }

    /// copy operator
    template <typename CONSTRAINT1, typename... OTHER_CONSTRAINTS>
    INLINE StructuralConstraintSetStatic<CONSTRAINT1, OTHER_CONSTRAINTS...> &
        StructuralConstraintSetStatic<CONSTRAINT1, OTHER_CONSTRAINTS...>::
        operator=(const StructuralConstraintSetStatic<CONSTRAINT1,
                                                      OTHER_CONSTRAINTS...> &from) {
      if (this != &from) {
        constraints::operator=(from);
      }

      return *this;
    }

    /// sets a new graph from which we will perform checkings
    template <typename CONSTRAINT1, typename... OTHER_CONSTRAINTS>
    INLINE void
    StructuralConstraintSetStatic<CONSTRAINT1, OTHER_CONSTRAINTS...>::setGraph(
        const DiGraph &graph) {
      constraints::setGraph(graph);
    }

    /// checks whether the constraints enable to add arc (x,y)
    template <typename CONSTRAINT1, typename... OTHER_CONSTRAINTS>
    INLINE bool StructuralConstraintSetStatic<
        CONSTRAINT1, OTHER_CONSTRAINTS...>::checkArcAddition(NodeId x,
                                                             NodeId y) const
        noexcept {
      return constraints::checkArcAddition(x, y);
    }

    /// checks whether the constraints enable to remove arc (x,y)
    template <typename CONSTRAINT1, typename... OTHER_CONSTRAINTS>
    INLINE bool StructuralConstraintSetStatic<
        CONSTRAINT1, OTHER_CONSTRAINTS...>::checkArcDeletion(NodeId x,
                                                             NodeId y) const
        noexcept {
      return constraints::checkArcDeletion(x, y);
    }

    /// checks whether the constraints enable to reverse arc (x,y)
    template <typename CONSTRAINT1, typename... OTHER_CONSTRAINTS>
    INLINE bool StructuralConstraintSetStatic<
        CONSTRAINT1, OTHER_CONSTRAINTS...>::checkArcReversal(NodeId x,
                                                             NodeId y) const
        noexcept {
      return constraints::checkArcReversal(x, y);
    }

    /// checks whether the constraints enable to add an arc
    template <typename CONSTRAINT1, typename... OTHER_CONSTRAINTS>
    INLINE bool StructuralConstraintSetStatic<
        CONSTRAINT1,
        OTHER_CONSTRAINTS...>::checkModification(const ArcAddition &change) const
        noexcept {
      return constraints::checkModification(change);
    }

    /// checks whether the constraints enable to remove an arc
    template <typename CONSTRAINT1, typename... OTHER_CONSTRAINTS>
    INLINE bool StructuralConstraintSetStatic<
        CONSTRAINT1,
        OTHER_CONSTRAINTS...>::checkModification(const ArcDeletion &change) const
        noexcept {
      return constraints::checkModification(change);
    }

    /// checks whether the constraints enable to reverse an arc
    template <typename CONSTRAINT1, typename... OTHER_CONSTRAINTS>
    INLINE bool StructuralConstraintSetStatic<
        CONSTRAINT1,
        OTHER_CONSTRAINTS...>::checkModification(const ArcReversal &change) const
        noexcept {
      return constraints::checkModification(change);
    }

    /// checks whether the constraints enable to perform a graph change
    template <typename CONSTRAINT1, typename... OTHER_CONSTRAINTS>
    INLINE bool StructuralConstraintSetStatic<
        CONSTRAINT1,
        OTHER_CONSTRAINTS...>::checkModification(const GraphChange &change) const
        noexcept {
      return constraints::checkModification(change);
    }

    /// notify the constraint of a modification of the graph
    template <typename CONSTRAINT1, typename... OTHER_CONSTRAINTS>
    INLINE void
    StructuralConstraintSetStatic<CONSTRAINT1, OTHER_CONSTRAINTS...>::modifyGraph(
        const ArcAddition &change) {
      if (checkModification(change)) {
        constraints::modifyGraph(change);
      } else {
        GUM_ERROR(OperationNotAllowed,
                  "the constraint set does not allow this arc addition");
      }
    }

    /// notify the constraint of a modification of the graph
    template <typename CONSTRAINT1, typename... OTHER_CONSTRAINTS>
    INLINE void
    StructuralConstraintSetStatic<CONSTRAINT1, OTHER_CONSTRAINTS...>::modifyGraph(
        const ArcDeletion &change) {
      if (checkModification(change)) {
        constraints::modifyGraph(change);
      } else {
        GUM_ERROR(OperationNotAllowed,
                  "the constraint set does not allow this arc deletion");
      }
    }

    /// notify the constraint of a modification of the graph
    template <typename CONSTRAINT1, typename... OTHER_CONSTRAINTS>
    INLINE void
    StructuralConstraintSetStatic<CONSTRAINT1, OTHER_CONSTRAINTS...>::modifyGraph(
        const ArcReversal &change) {
      if (checkModification(change)) {
        constraints::modifyGraph(change);
      } else {
        GUM_ERROR(OperationNotAllowed,
                  "the constraint set does not allow this arc reversal");
      }
    }

    /// notify the constraint of a modification of the graph
    template <typename CONSTRAINT1, typename... OTHER_CONSTRAINTS>
    INLINE void
    StructuralConstraintSetStatic<CONSTRAINT1, OTHER_CONSTRAINTS...>::modifyGraph(
        const GraphChange &change) {
      switch (change.type()) {
        case GraphChangeType::ARC_ADDITION:
          modifyGraph(reinterpret_cast<const ArcAddition &>(change));
          break;

        case GraphChangeType::ARC_DELETION:
          modifyGraph(reinterpret_cast<const ArcDeletion &>(change));
          break;

        case GraphChangeType::ARC_REVERSAL:
          modifyGraph(reinterpret_cast<const ArcReversal &>(change));
          break;

        default:
          GUM_ERROR(OperationNotAllowed, "edge modifications are not "
                                         "currently supported by constraint sets");
      }
    }

    /// indicates whether a change will always violate the constraint
    template <typename CONSTRAINT1, typename... OTHER_CONSTRAINTS>
    INLINE bool StructuralConstraintSetStatic<
        CONSTRAINT1,
        OTHER_CONSTRAINTS...>::isAlwaysInvalid(const GraphChange &change) const
        noexcept {
      return constraints::isAlwaysInvalid(change);
    }

    // ===========================================================================

    /// default constructor
    template <typename CONSTRAINT>
    INLINE
    StructuralConstraintSetStatic<CONSTRAINT>::StructuralConstraintSetStatic() {
      GUM_CONSTRUCTOR(StructuralConstraintSetStatic);
    }

    /// copy constructor
    template <typename CONSTRAINT>
    INLINE StructuralConstraintSetStatic<CONSTRAINT>::StructuralConstraintSetStatic(
        const StructuralConstraintSetStatic<CONSTRAINT> &from)
        : constraints(from) {
      GUM_CONS_CPY(StructuralConstraintSetStatic);
    }

    /// destructor
    template <typename CONSTRAINT>
    INLINE
        StructuralConstraintSetStatic<CONSTRAINT>::~StructuralConstraintSetStatic() {
      GUM_DESTRUCTOR(StructuralConstraintSetStatic);
    }

    /// copy operator
    template <typename CONSTRAINT>
    INLINE StructuralConstraintSetStatic<CONSTRAINT> &
        StructuralConstraintSetStatic<CONSTRAINT>::
        operator=(const StructuralConstraintSetStatic<CONSTRAINT> &from) {
      if (this != &from) {
        constraints::operator=(from);
      }

      return *this;
    }

    /// sets a new graph from which we will perform checkings
    template <typename CONSTRAINT>
    INLINE void
    StructuralConstraintSetStatic<CONSTRAINT>::setGraph(const DiGraph &graph) {
      constraints::setGraph(graph);
    }

    /// checks whether the constraints enable to add arc (x,y)
    template <typename CONSTRAINT>
    INLINE bool StructuralConstraintSetStatic<CONSTRAINT>::checkArcAddition(
        NodeId x, NodeId y) const noexcept {
      return constraints::checkArcAddition(x, y);
    }

    /// checks whether the constraints enable to remove arc (x,y)
    template <typename CONSTRAINT>
    INLINE bool StructuralConstraintSetStatic<CONSTRAINT>::checkArcDeletion(
        NodeId x, NodeId y) const noexcept {
      return constraints::checkArcDeletion(x, y);
    }

    /// checks whether the constraints enable to reverse arc (x,y)
    template <typename CONSTRAINT>
    INLINE bool StructuralConstraintSetStatic<CONSTRAINT>::checkArcReversal(
        NodeId x, NodeId y) const noexcept {
      return constraints::checkArcReversal(x, y);
    }

    /// checks whether the constraints enable to add an arc
    template <typename CONSTRAINT>
    INLINE bool StructuralConstraintSetStatic<CONSTRAINT>::checkModification(
        const ArcAddition &change) const noexcept {
      return constraints::checkModification(change);
    }

    /// checks whether the constraints enable to remove an arc
    template <typename CONSTRAINT>
    INLINE bool StructuralConstraintSetStatic<CONSTRAINT>::checkModification(
        const ArcDeletion &change) const noexcept {
      return constraints::checkModification(change);
    }

    /// checks whether the constraints enable to reverse an arc
    template <typename CONSTRAINT>
    INLINE bool StructuralConstraintSetStatic<CONSTRAINT>::checkModification(
        const ArcReversal &change) const noexcept {
      return constraints::checkModification(change);
    }

    /// checks whether the constraints enable to perform a graph change
    template <typename CONSTRAINT>
    INLINE bool StructuralConstraintSetStatic<CONSTRAINT>::checkModification(
        const GraphChange &change) const noexcept {
      return constraints::checkModification(change);
    }

    /// notify the constraint of a modification of the graph
    template <typename CONSTRAINT>
    INLINE void StructuralConstraintSetStatic<CONSTRAINT>::modifyGraph(
        const ArcAddition &change) {
      if (checkModification(change)) {
        constraints::modifyGraph(change);
      } else {
        GUM_ERROR(OperationNotAllowed,
                  "the constraint set does not allow this arc addition");
      }
    }

    /// notify the constraint of a modification of the graph
    template <typename CONSTRAINT>
    INLINE void StructuralConstraintSetStatic<CONSTRAINT>::modifyGraph(
        const ArcDeletion &change) {
      if (checkModification(change)) {
        constraints::modifyGraph(change);
      } else {
        GUM_ERROR(OperationNotAllowed,
                  "the constraint set does not allow this arc deletion");
      }
    }

    /// notify the constraint of a modification of the graph
    template <typename CONSTRAINT>
    INLINE void StructuralConstraintSetStatic<CONSTRAINT>::modifyGraph(
        const ArcReversal &change) {
      if (checkModification(change)) {
        constraints::modifyGraph(change);
      } else {
        GUM_ERROR(OperationNotAllowed,
                  "the constraint set does not allow this arc reversal");
      }
    }

    /// notify the constraint of a modification of the graph
    template <typename CONSTRAINT>
    INLINE void StructuralConstraintSetStatic<CONSTRAINT>::modifyGraph(
        const GraphChange &change) {
      switch (change.type()) {
        case GraphChangeType::ARC_ADDITION:
          modifyGraph(reinterpret_cast<const ArcAddition &>(change));
          break;

        case GraphChangeType::ARC_DELETION:
          modifyGraph(reinterpret_cast<const ArcDeletion &>(change));
          break;

        case GraphChangeType::ARC_REVERSAL:
          modifyGraph(reinterpret_cast<const ArcReversal &>(change));
          break;

        default:
          GUM_ERROR(OperationNotAllowed, "edge modifications are not "
                                         "currently supported by constraint sets");
      }
    }

    /// indicates whether a change will always violate the constraint
    template <typename CONSTRAINT>
    INLINE bool StructuralConstraintSetStatic<CONSTRAINT>::isAlwaysInvalid(
        const GraphChange &change) const noexcept {
      return constraints::isAlwaysInvalid(change);
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
