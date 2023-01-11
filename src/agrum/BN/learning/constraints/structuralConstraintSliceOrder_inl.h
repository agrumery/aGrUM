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
 * @brief the structural constraint imposing a partial order over nodes
 *
 * In DBNs, it is forbidden to add arcs from nodes at time slice t to nodes at
 * time slice s, where s < t. This class allows for taking this kind of
 *constraint
 * into account by imposing a partial order over the nodes: arcs (X,Y) can then
 * only be added if X <= Y in the partial order.
 * @warning: there may exist free variables, that is variables whose order
 * w.r.t. the other variables is unspecified. In this case, arcs adjacent
 * to them can be constructed. The partial order is specified by assigning
 * numbers to nodes (through a NodeProperty). Nodes without number (i.e., that
 * do not belong to the property) are free.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// sets a new graph from which we will perform checkings
    INLINE void StructuralConstraintSliceOrder::setGraphAlone(const DiGraph& graph) {}

    /// checks whether the constraints enable to add arc (x,y)
    INLINE bool StructuralConstraintSliceOrder::checkArcAdditionAlone(NodeId x, NodeId y) const {
      try {
        return _SliceOrder_order_[x] <= _SliceOrder_order_[y];
      } catch (const Exception&) { return true; }
    }

    /// checks whether the constraints enable to remove arc (x,y)
    INLINE bool StructuralConstraintSliceOrder::checkArcDeletionAlone(NodeId x, NodeId y) const {
      return true;
    }

    /// checks whether the constraints enable to reverse arc (x,y)
    INLINE bool StructuralConstraintSliceOrder::checkArcReversalAlone(NodeId x, NodeId y) const {
      try {
        return _SliceOrder_order_[x] == _SliceOrder_order_[y];
      } catch (const Exception&) { return true; }
    }

    /// notify the constraint of a modification of the graph
    INLINE void StructuralConstraintSliceOrder::modifyGraphAlone(const ArcAddition& change) {}

    /// notify the constraint of a modification of the graph
    INLINE void StructuralConstraintSliceOrder::modifyGraphAlone(const ArcDeletion& change) {}

    /// notify the constraint of a modification of the graph
    INLINE void StructuralConstraintSliceOrder::modifyGraphAlone(const ArcReversal& change) {}

    /// notify the constraint of a modification of the graph
    INLINE void StructuralConstraintSliceOrder::modifyGraphAlone(const GraphChange& change) {}

    /// indicates whether a change will always violate the constraint
    INLINE bool
       StructuralConstraintSliceOrder::isAlwaysInvalidAlone(const GraphChange& change) const {
      switch (change.type()) {
        case GraphChangeType::ARC_ADDITION:
          try {
            return (_SliceOrder_order_[change.node1()] > _SliceOrder_order_[change.node2()]);
          } catch (const Exception&) { return false; }

        case GraphChangeType::ARC_DELETION: return false;

        case GraphChangeType::ARC_REVERSAL:
          try {
            return (_SliceOrder_order_[change.node1()] != _SliceOrder_order_[change.node2()]);
          } catch (const Exception&) { return false; }

        default:
          GUM_ERROR(OperationNotAllowed,
                    "edge modifications are not "
                    "supported by SliceOrder constraints");
      }
    }

    /// checks whether the constraints enable to add an arc
    INLINE bool
       StructuralConstraintSliceOrder::checkModificationAlone(const ArcAddition& change) const {
      return checkArcAdditionAlone(change.node1(), change.node2());
    }

    /// checks whether the constraints enable to remove an arc
    INLINE bool
       StructuralConstraintSliceOrder::checkModificationAlone(const ArcDeletion& change) const {
      return checkArcDeletionAlone(change.node1(), change.node2());
    }

    /// checks whether the constraints enable to reverse an arc
    INLINE bool
       StructuralConstraintSliceOrder::checkModificationAlone(const ArcReversal& change) const {
      return checkArcReversalAlone(change.node1(), change.node2());
    }

    /// checks whether the constraints enable to perform a graph change
    INLINE bool
       StructuralConstraintSliceOrder::checkModificationAlone(const GraphChange& change) const {
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
                    "supported by StructuralConstraintSliceOrder");
      }
    }

    /// sets the time slices of all the nodes in the property
    INLINE void StructuralConstraintSliceOrder::setSliceOrder(const NodeProperty< NodeId >& order) {
      _SliceOrder_order_ = order;
    }

    /// sets the default time slice
    INLINE void StructuralConstraintSliceOrder::setDefaultSlice(NodeId slice) {
      for (auto& node: _SliceOrder_order_) {
        node.second = slice;
      }
    }

    /// adds a new node in the slice order
    INLINE void StructuralConstraintSliceOrder::addNode(NodeId node, NodeId slice) {
      _SliceOrder_order_.set(node, slice);
    }

    /// returns the current slice order
    INLINE const NodeProperty< NodeId >& StructuralConstraintSliceOrder::sliceOrder() const {
      return _SliceOrder_order_;
    }

// include all the methods applicable to the whole class hierarchy
#  define GUM_CONSTRAINT_CLASS_NAME StructuralConstraintSliceOrder
#  include <agrum/BN/learning/constraints/structuralConstraintPatternInline.h>
#  undef GUM_CONSTRAINT_CLASS_NAME

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
