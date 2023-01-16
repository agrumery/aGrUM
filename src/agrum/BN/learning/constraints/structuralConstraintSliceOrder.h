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
#ifndef GUM_LEARNING_STRUCTURAL_CONSTRAINT_SLICE_ORDER_H
#define GUM_LEARNING_STRUCTURAL_CONSTRAINT_SLICE_ORDER_H

#include <agrum/agrum.h>
#include <agrum/BN/learning/constraints/structuralConstraintDiGraph.h>

namespace gum {

  namespace learning {

    /** @class StructuralConstraintSliceOrder
     * @brief the structural constraint imposing a partial order over nodes
     *
     * In DBNs, it is forbidden to add arcs from nodes at time slice t to nodes
     *at
     * time slice s, where s < t. This class allows for taking this kind of
     * constraint into account by imposing a partial order over the nodes:
     * arcs (X,Y) can then only be added if X <= Y in the partial order.
     * @warning: there may exist free variables, that is variables whose order
     * w.r.t. the other variables is unspecified. In this case, arcs adjacent
     * to them can be constructed. The partial order is specified by assigning
     * numbers to nodes (through a NodeProperty). Nodes without number (i.e.,
     *that
     * do not belong to the property) are free.
     *
     * @ingroup learning_group
     */
    class StructuralConstraintSliceOrder:
        protected virtual StructuralConstraintSetStatic< StructuralConstraintDiGraph > {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      StructuralConstraintSliceOrder();

      /// constructor starting with an empty graph with a given number of nodes
      /** param order the partial order  */
      StructuralConstraintSliceOrder(const NodeProperty< NodeId >& order);

      /// constructor starting with a given graph
      StructuralConstraintSliceOrder(const DiGraph& graph, const NodeProperty< NodeId >& order);

      /// copy constructor
      StructuralConstraintSliceOrder(const StructuralConstraintSliceOrder& from);

      /// move constructor
      StructuralConstraintSliceOrder(StructuralConstraintSliceOrder&& from);

      /// destructor
      virtual ~StructuralConstraintSliceOrder();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      StructuralConstraintSliceOrder& operator=(const StructuralConstraintSliceOrder& from);

      /// move operator
      StructuralConstraintSliceOrder& operator=(StructuralConstraintSliceOrder&& from);

      /// @}

      // ##########################################################################
      /// @name Specific Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// sets the time slices of all the nodes in the property
      void setSliceOrder(const NodeProperty< NodeId >& slice);

      /// returns the current slice order
      const NodeProperty< NodeId >& sliceOrder() const;

      /// adds a new node in the slice order
      void addNode(NodeId node, NodeId slice);

      /** @brief assign a given slice to all the nodes specified in the
       * partial order */
      void setDefaultSlice(NodeId slice);

      /// sets a new graph from which we will perform checkings
      void setGraphAlone(const DiGraph& graph);

      /// notify the constraint of a modification of the graph
      /** @warning If an already existing arc is added, nothing is done. In
       * particular, no exception is raised.
       * @throws InvalidNode exception is thrown if an arc (x,y) is added and x
       * or y does not belong to the graph nodes
       * @throws InvalidArc exception is thrown if any time-backward arc
       * is created by the arc addition. */
      void modifyGraphAlone(const ArcAddition& change);

      /// notify the constraint of a modification of the graph
      /** @warning If a nonexisting arc is removed, nothing is done. In
       * particular, no exception is raised. */
      void modifyGraphAlone(const ArcDeletion& change);

      /// notify the constraint of a modification of the graph
      /** @warning If an already existing arc is added, or if a nonexisting arc
       * is removed, nothing is done. In particular, no exception is raised.
       * @throws InvalidNode exception is thrown if an arc (x,y) is added and x
       * or y does not belong to the graph nodes
       * @throws InvalidArc exception is thrown if any time-backward arc
       * is created by the arc reversal. */
      void modifyGraphAlone(const ArcReversal& change);

      /// notify the constraint of a modification of the graph
      /** @warning If an already existing arc is added, or if a nonexisting arc
       * is removed, nothing is done. In particular, no exception is raised.
       * @throws InvalidNode exception is thrown if an arc (x,y) is added or
       * reversed and x or y does not belong to the graph nodes
       * @throws InvalidArc exception is thrown if any time-backward arc
       * is created by an arc addition or reversal. */
      void modifyGraphAlone(const GraphChange& change);

      /// indicates whether a change will always violate the constraint
      /** Some learning algorithms need examine several times whether a given
       * graph change can be applied. For instance, the first time arc (X,Y)
       * addition is considered, the learning algorithm may discard this change
       * because it violates the structural constraint (e.g., if the latter
       * enforces a DAG structure, this arc addition might induce a directed
       * cycle), but, later on, other arc removal may induce that the arc
       * addition
       * is now possible. Such change is thus not always invalid. Conversely,
       * there are changes that can be discarded once and for all. For instance,
       * in a 2TBN structure, it is always impossible to add a backward-time
       * arc.
       * Such graph changes are always invalid and are therefore tagged as such
       * by the isAlwaysInvalid method. */
      bool isAlwaysInvalidAlone(const GraphChange& change) const;

      /// checks whether the constraints enable to add arc (x,y)
      /** an arc can be added if and only if its extremal nodes belong to the
       * graph and the arc does not already exist and is not a
       * backward-time arc. */
      bool checkArcAdditionAlone(NodeId x, NodeId y) const;

      /// checks whether the constraints enable to remove arc (x,y)
      /** an arc can be removed if and only if the arc exists. */
      bool checkArcDeletionAlone(NodeId x, NodeId y) const;

      /// checks whether the constraints enable to reverse arc (x,y)
      /** an arc can be reversed if and only if it exists and arc (y,x)
       * does not and is not a backward-time arc. */
      bool checkArcReversalAlone(NodeId x, NodeId y) const;

      /// checks whether the constraints enable to add an arc
      /** an arc can be added if and only if its extremal nodes belong to the
       * graph and the arc does not already exist and is not a
       * backward-time arc. */
      bool checkModificationAlone(const ArcAddition& change) const;

      /// checks whether the constraints enable to remove an arc
      /** an arc can be removed if and only if the arc exists. */
      bool checkModificationAlone(const ArcDeletion& change) const;

      /// checks whether the constraints enable to reverse an arc
      /** an arc can be reversed if and only if it exists and arc (y,x)
       * does not and is not a backward-time arc. */
      bool checkModificationAlone(const ArcReversal& change) const;

      /// checks whether the constraints enable to perform a graph change
      /** An arc can be added if and only if its extremal nodes belong to the
       * graph and the arc does not already exist and is not a
       * backward-time arc.
       * An arc can be removed if and only if the arc exists.
       * An arc (x,y) can be reversed if and only if it exists and arc (y,x)
       * does not and is not a backward-time arc. */
      bool checkModificationAlone(const GraphChange& change) const;

      /// @}

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// include the set of methods that enable the structural constraint to
// be standalone, i.e., that it needs not be included into a
// StructuralConstraintSetStatic to be used by learning algorithms
#  define GUM_CONSTRAINT_CLASS_NAME StructuralConstraintSliceOrder
#  include <agrum/BN/learning/constraints/structuralConstraintPatternHeader.h>
#  undef GUM_CONSTRAINT_CLASS_NAME
#endif   // DOXYGEN_SHOULD_SKIP_THIS

      protected:
      /// slices to which belong the nodes
      NodeProperty< NodeId > _SliceOrder_order_;
    };

  } /* namespace learning */

} /* namespace gum */

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/BN/learning/constraints/structuralConstraintSliceOrder_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_STRUCTURAL_CONSTRAINT_SLICE_ORDER_H */
