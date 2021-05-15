/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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


/** @file
 * @brief the class imposing a N-sized tabu list as a structural constraints for
 * learning algorithms
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// sets the size of the tabu list
    INLINE
    void StructuralConstraintTabuList::setTabuListSize(Size new_size) {
      if (new_size == TabuList__changes_.size()) return;

      if (TabuList__changes_.size() > new_size) {
        // remove the oldest elements, so that only newsize elements remain
        while (TabuList__changes_.size() > new_size) {
          TabuList__changes_.eraseSecond(TabuList__offset_);
          ++TabuList__offset_;
        }
      } else {
        // add dummy elements
        while (TabuList__changes_.size() < new_size) {
          --TabuList__offset_;
          TabuList__changes_.insert(
             ArcAddition(std::numeric_limits< NodeId >::max() - TabuList__offset_,
                         std::numeric_limits< NodeId >::max()),
             TabuList__offset_);
        }
      }
    }

    /// sets a new graph from which we will perform checkings
    INLINE void StructuralConstraintTabuList::setGraphAlone(const DiGraph& graph) {
    }

    /// checks whether the constraints enable to add arc (x,y)
    INLINE bool
       StructuralConstraintTabuList::checkArcAdditionAlone(NodeId x,
                                                           NodeId y) const {
      return !TabuList__changes_.existsFirst(ArcDeletion(x, y))
          && !TabuList__changes_.existsFirst(ArcAddition(x, y));
    }

    /// checks whether the constraints enable to remove arc (x,y)
    INLINE bool
       StructuralConstraintTabuList::checkArcDeletionAlone(NodeId x,
                                                           NodeId y) const {
      return !TabuList__changes_.existsFirst(ArcAddition(x, y))
          && !TabuList__changes_.existsFirst(ArcDeletion(x, y));
    }

    /// checks whether the constraints enable to reverse arc (x,y)
    INLINE bool
       StructuralConstraintTabuList::checkArcReversalAlone(NodeId x,
                                                           NodeId y) const {
      return !TabuList__changes_.existsFirst(ArcReversal(y, x))
          && !TabuList__changes_.existsFirst(ArcReversal(x, y));
    }

    /// checks whether the constraints enable to add an arc
    INLINE bool StructuralConstraintTabuList::checkModificationAlone(
       const ArcAddition& change) const {
      return checkArcAdditionAlone(change.node1(), change.node2());
    }

    /// checks whether the constraints enable to remove an arc
    INLINE bool StructuralConstraintTabuList::checkModificationAlone(
       const ArcDeletion& change) const {
      return checkArcDeletionAlone(change.node1(), change.node2());
    }

    /// checks whether the constraints enable to reverse an arc
    INLINE bool StructuralConstraintTabuList::checkModificationAlone(
       const ArcReversal& change) const {
      return checkArcReversalAlone(change.node1(), change.node2());
    }

    /// checks whether the constraints enable to perform a graph change
    INLINE bool StructuralConstraintTabuList::checkModificationAlone(
       const GraphChange& change) const {
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
       StructuralConstraintTabuList::modifyGraphAlone(const ArcAddition& change) {
      TabuList__changes_.eraseSecond(TabuList__offset_);
      ++TabuList__offset_;
      TabuList__changes_.insert(
         change,
         NodeId(TabuList__offset_ + TabuList__changes_.size()));
    }

    /// notify the constraint of a modification of the graph
    INLINE void
       StructuralConstraintTabuList::modifyGraphAlone(const ArcDeletion& change) {
      TabuList__changes_.eraseSecond(TabuList__offset_);
      ++TabuList__offset_;
      TabuList__changes_.insert(change,
                                TabuList__offset_
                                   + NodeId(TabuList__changes_.size()));
    }

    /// notify the constraint of a modification of the graph
    INLINE void
       StructuralConstraintTabuList::modifyGraphAlone(const ArcReversal& change) {
      TabuList__changes_.eraseSecond(TabuList__offset_);
      ++TabuList__offset_;
      TabuList__changes_.insert(change,
                                TabuList__offset_
                                   + NodeId(TabuList__changes_.size()));
    }

    /// notify the constraint of a modification of the graph
    INLINE void
       StructuralConstraintTabuList::modifyGraphAlone(const GraphChange& change) {
      switch (change.type()) {
        case GraphChangeType::ARC_ADDITION:
          modifyGraphAlone(static_cast< const ArcAddition& >(change));
          break;

        case GraphChangeType::ARC_DELETION:
          modifyGraphAlone(static_cast< const ArcDeletion& >(change));
          break;

        case GraphChangeType::ARC_REVERSAL:
          modifyGraphAlone(static_cast< const ArcReversal& >(change));
          break;

        default:
          GUM_ERROR(OperationNotAllowed,
                    "edge modifications are not supported by digraph constraint")
      }
    }

    /// indicates whether a change will always violate the constraint
    INLINE bool StructuralConstraintTabuList::isAlwaysInvalidAlone(
       const GraphChange&) const {
      return false;
    }

// include all the methods applicable to the whole class hierarchy
#  define GUM_CONSTRAINT_CLASS_NAME StructuralConstraintTabuList
#  include <agrum/BN/learning/constraints/structuralConstraintPatternRootInline.h>
#  undef GUM_CONSTRAINT_CLASS_NAME

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
