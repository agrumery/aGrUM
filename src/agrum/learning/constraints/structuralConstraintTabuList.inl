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
 * @brief the class imposing a N-sized tabu list as a structural constraints for
 * learning algorithms
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// sets the size of the tabu list
    INLINE
    void StructuralConstraintTabuList::setTabuListSize(unsigned int new_size) {
      if (new_size == _TabuList__changes.size())
        return;

      if (_TabuList__changes.size() > new_size) {
        // remove the oldest elements, so that only newsize elements remain
        while (_TabuList__changes.size() > new_size) {
          _TabuList__changes.eraseSecond(_TabuList__offset);
          ++_TabuList__offset;
        }
      } else {
        // add dummy elements
        while (_TabuList__changes.size() < new_size) {
          --_TabuList__offset;
          _TabuList__changes.insert(
              ArcAddition(std::numeric_limits<NodeId>::max() - _TabuList__offset,
                          std::numeric_limits<NodeId>::max()),
              _TabuList__offset);
        }
      }
    }

    /// sets a new graph from which we will perform checkings
    INLINE void StructuralConstraintTabuList::setGraphAlone(const DiGraph &graph) {}

    /// checks whether the constraints enable to add arc (x,y)
    INLINE bool StructuralConstraintTabuList::checkArcAdditionAlone(NodeId x,
                                                                    NodeId y) const
        noexcept {
      return !_TabuList__changes.existsFirst(ArcDeletion(x, y)) &&
             !_TabuList__changes.existsFirst(ArcAddition(x, y));
    }

    /// checks whether the constraints enable to remove arc (x,y)
    INLINE bool StructuralConstraintTabuList::checkArcDeletionAlone(NodeId x,
                                                                    NodeId y) const
        noexcept {
      return !_TabuList__changes.existsFirst(ArcAddition(x, y)) &&
             !_TabuList__changes.existsFirst(ArcDeletion(x, y));
    }

    /// checks whether the constraints enable to reverse arc (x,y)
    INLINE bool StructuralConstraintTabuList::checkArcReversalAlone(NodeId x,
                                                                    NodeId y) const
        noexcept {
      return !_TabuList__changes.existsFirst(ArcReversal(y, x)) &&
             !_TabuList__changes.existsFirst(ArcReversal(x, y));
    }

    /// checks whether the constraints enable to add an arc
    INLINE bool StructuralConstraintTabuList::checkModificationAlone(
        const ArcAddition &change) const noexcept {
      return checkArcAdditionAlone(change.node1(), change.node2());
    }

    /// checks whether the constraints enable to remove an arc
    INLINE bool StructuralConstraintTabuList::checkModificationAlone(
        const ArcDeletion &change) const noexcept {
      return checkArcDeletionAlone(change.node1(), change.node2());
    }

    /// checks whether the constraints enable to reverse an arc
    INLINE bool StructuralConstraintTabuList::checkModificationAlone(
        const ArcReversal &change) const noexcept {
      return checkArcReversalAlone(change.node1(), change.node2());
    }

    /// checks whether the constraints enable to perform a graph change
    INLINE bool StructuralConstraintTabuList::checkModificationAlone(
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
                    "supported by StructuralConstraintTabuList");
      }
    }

    /// notify the constraint of a modification of the graph
    INLINE void
    StructuralConstraintTabuList::modifyGraphAlone(const ArcAddition &change) {
      _TabuList__changes.eraseSecond(_TabuList__offset);
      ++_TabuList__offset;
      _TabuList__changes.insert(change,
                                _TabuList__offset + _TabuList__changes.size());
    }

    /// notify the constraint of a modification of the graph
    INLINE void
    StructuralConstraintTabuList::modifyGraphAlone(const ArcDeletion &change) {
      _TabuList__changes.eraseSecond(_TabuList__offset);
      ++_TabuList__offset;
      _TabuList__changes.insert(change,
                                _TabuList__offset + _TabuList__changes.size());
    }

    /// notify the constraint of a modification of the graph
    INLINE void
    StructuralConstraintTabuList::modifyGraphAlone(const ArcReversal &change) {
      _TabuList__changes.eraseSecond(_TabuList__offset);
      ++_TabuList__offset;
      _TabuList__changes.insert(change,
                                _TabuList__offset + _TabuList__changes.size());
    }

    /// notify the constraint of a modification of the graph
    INLINE void
    StructuralConstraintTabuList::modifyGraphAlone(const GraphChange &change) {
      switch (change.type()) {
        case GraphChangeType::ARC_ADDITION:
          modifyGraphAlone(static_cast<const ArcAddition &>(change));
          break;

        case GraphChangeType::ARC_DELETION:
          modifyGraphAlone(static_cast<const ArcDeletion &>(change));
          break;

        case GraphChangeType::ARC_REVERSAL:
          modifyGraphAlone(static_cast<const ArcReversal &>(change));
          break;

        default:
          GUM_ERROR(OperationNotAllowed,
                    "edge modifications are not supported by digraph constraint");
      }
    }

    /// indicates whether a change will always violate the constraint
    INLINE bool
    StructuralConstraintTabuList::isAlwaysInvalidAlone(const GraphChange &) const
        noexcept {
      return false;
    }

// include all the methods applicable to the whole class hierarchy
#define GUM_CONSTRAINT_CLASS_NAME StructuralConstraintTabuList
#include <agrum/learning/constraints/structuralConstraintPatternRootInline.h>
#undef GUM_CONSTRAINT_CLASS_NAME

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
