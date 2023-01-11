/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief the class for structural constraints limiting the number of parents
 * of nodes in a directed graph
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// sets a new graph from which we will perform checkings
    INLINE void StructuralConstraintIndegree::setGraphAlone(const DiGraph& graph) {
      // check that the max_indegree corresponds to the graph
      for (const auto id: graph) {
        if (!_Indegree_max_parents_.exists(id)) {
          _Indegree_max_parents_.insert(id, _Indegree_max_indegree_);
        }
      }
    }

    /// checks whether the constraints enable to add arc (x,y)
    INLINE bool StructuralConstraintIndegree::checkArcAdditionAlone(NodeId x, NodeId y) const {
      return (_Indegree_max_parents_[y] > _DiGraph_graph_.parents(y).size());
    }

    /// checks whether the constraints enable to remove arc (x,y)
    INLINE bool StructuralConstraintIndegree::checkArcDeletionAlone(NodeId x, NodeId y) const {
      return true;
    }

    /// checks whether the constraints enable to reverse arc (x,y)
    INLINE bool StructuralConstraintIndegree::checkArcReversalAlone(NodeId x, NodeId y) const {
      return (_Indegree_max_parents_[x] > _DiGraph_graph_.parents(x).size());
    }

    /// checks whether the constraints enable to add an arc
    INLINE bool
       StructuralConstraintIndegree::checkModificationAlone(const ArcAddition& change) const {
      return checkArcAdditionAlone(change.node1(), change.node2());
    }

    /// checks whether the constraints enable to remove an arc
    INLINE bool
       StructuralConstraintIndegree::checkModificationAlone(const ArcDeletion& change) const {
      return checkArcDeletionAlone(change.node1(), change.node2());
    }

    /// checks whether the constraints enable to reverse an arc
    INLINE bool
       StructuralConstraintIndegree::checkModificationAlone(const ArcReversal& change) const {
      return checkArcReversalAlone(change.node1(), change.node2());
    }

    /// checks whether the constraints enable to perform a graph change
    INLINE bool
       StructuralConstraintIndegree::checkModificationAlone(const GraphChange& change) const {
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
                    "supported by StructuralConstraintIndegree");
      }
    }

    /// notify the constraint of a modification of the graph
    INLINE void StructuralConstraintIndegree::modifyGraphAlone(const ArcAddition& change) {}

    /// notify the constraint of a modification of the graph
    INLINE void StructuralConstraintIndegree::modifyGraphAlone(const ArcDeletion& change) {}

    /// notify the constraint of a modification of the graph
    INLINE void StructuralConstraintIndegree::modifyGraphAlone(const ArcReversal& change) {}

    /// notify the constraint of a modification of the graph
    INLINE void StructuralConstraintIndegree::modifyGraphAlone(const GraphChange& change) {}

    /// indicates whether a change will always violate the constraint
    INLINE bool
       StructuralConstraintIndegree::isAlwaysInvalidAlone(const GraphChange& change) const {
      if ((change.type() == GraphChangeType::ARC_ADDITION)
          && (_Indegree_max_parents_[change.node2()] == 0)) {
        return true;
      } else if ((change.type() == GraphChangeType::ARC_REVERSAL)
                 && (_Indegree_max_parents_[change.node1()] == 0)) {
        return true;
      } else {
        return false;
      }
    }

    /// sets the indegree for a given set of nodes
    INLINE void
       StructuralConstraintIndegree::setIndegree(const NodeProperty< Size >& max_indegree) {
      for (const auto& degree: max_indegree) {
        _Indegree_max_parents_.set(degree.first, degree.second);
      }
    }

    /// resets the max indegree
    INLINE void StructuralConstraintIndegree::setMaxIndegree(Size max_indegree, bool update_all) {
      if (update_all) {
        for (auto& degree: _Indegree_max_parents_) {
          degree.second = max_indegree;
        }
      }

      _Indegree_max_indegree_ = max_indegree;
    }

// include all the methods applicable to the whole class hierarchy
#  define GUM_CONSTRAINT_CLASS_NAME StructuralConstraintIndegree
#  include <agrum/BN/learning/constraints/structuralConstraintPatternInline.h>
#  undef GUM_CONSTRAINT_CLASS_NAME

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
