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
 * @brief the base class for structural constraints imposed by DAGs
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_STRUCTURAL_CONSTRAINT_DAG_H
#define GUM_LEARNING_STRUCTURAL_CONSTRAINT_DAG_H

#include <agrum/agrum.h>
#include <agrum/tools/graphs/algorithms/DAGCycleDetector.h>
#include <agrum/BN/learning/constraints/structuralConstraintDiGraph.h>

namespace gum {

  namespace learning {

    /** @class StructuralConstraintDAG
     * @brief The base class for structural constraints imposed by DAGs
     *
     * This base should always be a virtual parents of the structural
     *constraints
     * classes. This will allow to combine different constraints into a single
     * class
     * @ingroup learning_group
     */
    class StructuralConstraintDAG:
        public virtual StructuralConstraintSetStatic< StructuralConstraintDiGraph > {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      StructuralConstraintDAG();

      /// constructor starting with an empty graph with a given number of nodes
      StructuralConstraintDAG(Size nb_nodes);

      /// constructor starting with a given graph
      StructuralConstraintDAG(const DAG& graph);

      /// copy constructor
      StructuralConstraintDAG(const StructuralConstraintDAG& from);

      /// move constructor
      StructuralConstraintDAG(StructuralConstraintDAG&& from);

      /// destructor
      virtual ~StructuralConstraintDAG();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      StructuralConstraintDAG& operator=(const StructuralConstraintDAG& from);

      /// move operator
      StructuralConstraintDAG& operator=(StructuralConstraintDAG&& from);

      /// @}

      // ##########################################################################
      /// @name Specific Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// sets a new graph from which we will perform checkings
      void setGraphAlone(const DiGraph& graph);

      /// sets a new empty graph from which we will perform checkings
      void setGraphAlone(Size nb_nodes);

      /// notify the constraint of a modification of the graph
      /** @warning If an already existing arc is added nothing is done. In
       * particular, no exception is raised.
       * @throws InvalidNode exception is thrown if an arc (x,y) is added and x
       * or y does not belong to the graph nodes
       * @throws InvalidDirectedCycle exception is thrown if any (directed)
       * cycle
       * is created by the arc addition. */
      void modifyGraphAlone(const ArcAddition& change);

      /// notify the constraint of a modification of the graph
      /** @warning If a nonexisting arc is removed, nothing is done. In
       * particular, no exception is raised. */
      void modifyGraphAlone(const ArcDeletion& change);

      /// notify the constraint of a modification of the graph
      /** @warning If an already existing arc is added, or if a nonexisting arc
       * is removed, nothing is done. In particular, no exception is raised.
       * @throws InvalidNode exception is thrown if at least one extremity of
       * the arc does not belong to the graph nodes
       * @throws InvalidDirectedCycle exception is thrown if any (directed)
       * cycle
       * is created by the arc reversal. */
      void modifyGraphAlone(const ArcReversal& change);

      /// notify the constraint of a modification of the graph
      /** @warning If an already existing arc is added, or if a nonexisting arc
       * is removed, nothing is done. In particular, no exception is raised.
       * @throws InvalidNode exception is thrown if an arc (x,y) is added and x
       * or y does not belong to the graph nodes
       * @throws InvalidDirectedCycle exception is thrown if any (directed)
       * cycle
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
       * graph and the arc does not already exist and would not create a cycle
       */
      bool checkArcAdditionAlone(NodeId x, NodeId y) const;

      /// checks whether the constraints enable to remove arc (x,y)
      bool checkArcDeletionAlone(NodeId x, NodeId y) const;

      /// checks whether the constraints enable to reverse arc (x,y)
      /** An arc (x,y) can be reversed if and only if it exists and, after
       * deleting it, the addition of arc (y,x) does not induce a
       * directed cycle. */
      bool checkArcReversalAlone(NodeId x, NodeId y) const;

      /// checks whether the constraints enable to add an arc
      /** an arc can be added if and only if its extremal nodes belong to the
       * graph and the arc does not already exist. */
      bool checkModificationAlone(const ArcAddition& change) const;

      /// checks whether the constraints enable to remove an arc
      /** an arc can be removed if and only if the arc exists. */
      bool checkModificationAlone(const ArcDeletion& change) const;

      /// checks whether the constraints enable to reverse an arc
      /** An arc can be reversed if, after deleting arc (x,y), the addition of
       * arc (y,x) does not induce a directed cycle. */
      bool checkModificationAlone(const ArcReversal& change) const;

      /// checks whether the constraints enable to perform a graph change
      /** an arc can be added if and only if its extremal nodes belong to the
       * graph and the arc does not already exist and would not create a cycle.
       * An arc can be removed if and only if the arc exists.
       * An arc can be reversed if, after deleting arc (x,y), the addition of
       * arc (y,x) does not induce a directed cycle. */
      bool checkModificationAlone(const GraphChange& change) const;

      /// sets a new graph from which we will perform checkings
      void setGraph(const DAG& graph);

      /// sets a new empty graph from which we will perform checkings
      void setGraph(Size nb_nodes);

      /// @}

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// include the set of methods that enable the structural constraint to
// be standalone, i.e., that it needs not be included into a
// StructuralConstraintSetStatic to be used by learning algorithms
#  define GUM_CONSTRAINT_CLASS_NAME StructuralConstraintDAG
#  include <agrum/BN/learning/constraints/structuralConstraintPatternHeader.h>
#  undef GUM_CONSTRAINT_CLASS_NAME
#endif   // DOXYGEN_SHOULD_SKIP_THIS

      protected:
      /// the cycle detector used to check quickly graph modifications
      DAGCycleDetector _DAG_cycle_detector_;
    };

  } /* namespace learning */

} /* namespace gum */

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/BN/learning/constraints/structuralConstraintDAG_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_STRUCTURAL_CONSTRAINT_DAG_H */
