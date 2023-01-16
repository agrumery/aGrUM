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
 * @brief the structural constraint for forbidding the creation of some arcs
 * during structure learning
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_STRUCTURAL_CONSTRAINT_POSSIBLE_EDGES_H
#define GUM_LEARNING_STRUCTURAL_CONSTRAINT_POSSIBLE_EDGES_H

#include <agrum/agrum.h>
#include <agrum/BN/learning/constraints/structuralConstraint.h>

namespace gum {

  namespace learning {

    /** @class StructuralConstraintPossibleEdges
     * @brief the structural constraint for forbidding the creation of some arcs
     * except those defined in the class during structure learning
     * @ingroup learning_group
     */
    class StructuralConstraintPossibleEdges: public virtual StructuralConstraintEmpty {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      StructuralConstraintPossibleEdges();

      /// constructor starting with a given graph
      StructuralConstraintPossibleEdges(const DiGraph& graph);

      /// copy constructor
      StructuralConstraintPossibleEdges(const StructuralConstraintPossibleEdges& from);

      /// move constructor
      StructuralConstraintPossibleEdges(StructuralConstraintPossibleEdges&& from);

      /// destructor
      virtual ~StructuralConstraintPossibleEdges();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      StructuralConstraintPossibleEdges& operator=(const StructuralConstraintPossibleEdges& from);

      /// move operator
      StructuralConstraintPossibleEdges& operator=(StructuralConstraintPossibleEdges&& from);

      /// @}

      // ##########################################################################
      /// @name Specific Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// assign a set of forbidden arcs
      void setEdges(const EdgeSet& set);

      /// assign a new forbidden arc
      void addEdge(const Edge& edge);

      /// remove a forbidden arc
      void eraseEdge(const Edge& edge);

      /// returns the set of mandatory arcs
      const EdgeSet& edges() const;

      /// sets a new graph from which we will perform checkings
      void setGraphAlone(const DiGraph& graph);

      /// notify the constraint of a modification of the graph
      /** @warning If an already existing arc is added nothing is done. In
       * particular, no exception is raised.
       * @throws InvalidNode exception is thrown if an arc (x,y) is added and x
       * or y does not belong to the graph nodes */
      void modifyGraphAlone(const ArcAddition& change);

      /// notify the constraint of a modification of the graph
      /** @warning If a nonexisting arc is removed, nothing is done. In
       * particular, no exception is raised. */
      void modifyGraphAlone(const ArcDeletion& change);

      /// notify the constraint of a modification of the graph
      /** @warning If an already existing arc is added, or if a nonexisting arc
       * is removed, nothing is done. In particular, no exception is raised.
       * @throws InvalidNode exception is thrown if at least one extremity of
       * the arc does not belong to the graph nodes */
      void modifyGraphAlone(const ArcReversal& change);

      /// notify the constraint of a modification of the graph
      /** @warning If an already existing arc is added, or if a nonexisting arc
       * is removed, nothing is done. In particular, no exception is raised.
       * @throws InvalidNode exception is thrown if an arc (x,y) is added and x
       * or y does not belong to the graph nodes */
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
       * graph and the arc does not already exist. */
      bool checkArcAdditionAlone(NodeId x, NodeId y) const;

      /// checks whether the constraints enable to remove arc (x,y)
      /** an arc can be removed if and only if the arc exists. */
      bool checkArcDeletionAlone(NodeId x, NodeId y) const;

      /// checks whether the constraints enable to reverse arc (x,y)
      /** an arc can be reversed if and only if it exists and arc (y,x)
       * does not. */
      bool checkArcReversalAlone(NodeId x, NodeId y) const;

      /// checks whether the constraints enable to perform a graph change
      /** An arc can be added if and only if its extremal nodes belong to the
       * graph and the arc does not already exist.
       * An arc can be removed if and only if the arc exists.
       * An arc (x,y) can be reversed if and only if it exists and arc (y,x)
       * does not. */
      bool checkModificationAlone(const GraphChange& change) const;

      /// checks whether the constraints enable to add an arc
      /** an arc can be added if and only if its extremal nodes belong to the
       * graph and the arc does not already exist. */
      bool checkModificationAlone(const ArcAddition& change) const;

      /// checks whether the constraints enable to remove an arc
      /** an arc can be removed if and only if the arc exists. */
      bool checkModificationAlone(const ArcDeletion& change) const;

      /// checks whether the constraints enable to reverse an arc
      /** an arc (x,y) can be reversed if and only if it exists and arc (y,x)
       * does not. */
      bool checkModificationAlone(const ArcReversal& change) const;

      /// @}

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// include the set of methods that enable the structural constraint to
// be standalone, i.e., that it needs not be included into a
// StructuralConstraintSetStatic to be used by learning algorithms
#  define GUM_CONSTRAINT_CLASS_NAME StructuralConstraintPossibleEdges
#  include <agrum/BN/learning/constraints/structuralConstraintPatternHeader.h>
#  undef GUM_CONSTRAINT_CLASS_NAME
#endif   // DOXYGEN_SHOULD_SKIP_THIS

      protected:
      /// the PossibleEdges on which we perform checks
      EdgeSet _PossibleEdges_possible_edges_;
    };

  } /* namespace learning */

} /* namespace gum */

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/BN/learning/constraints/structuralConstraintPossibleEdges_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_STRUCTURAL_CONSTRAINT_POSSIBLE_EDGES_H */
