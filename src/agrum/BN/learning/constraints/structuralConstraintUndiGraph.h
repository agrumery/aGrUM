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
 * @brief the base class for structural constraints used by learning algorithms
 * that learn an undirected graph structure
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_STRUCTURAL_CONSTRAINT_UNDIGRAPH_H
#define GUM_LEARNING_STRUCTURAL_CONSTRAINT_UNDIGRAPH_H

#include <agrum/agrum.h>
#include <agrum/tools/graphs/undiGraph.h>
#include <agrum/BN/learning/constraints/structuralConstraint.h>

namespace gum {

  namespace learning {

    /** @class StructuralConstraintUndiGraph
     * @brief The base class for structural constraints used by learning
     * algorithms that learn an undirected graph structure
     *
     * This base should always be a virtual parent of the structural constraints
     * classes. This will allow to combine different constraints into a single
     * class
     * @ingroup learning_group
     */
    class StructuralConstraintUndiGraph: public virtual StructuralConstraintEmpty {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      StructuralConstraintUndiGraph();

      /// constructor starting with an empty graph with a given number of nodes
      StructuralConstraintUndiGraph(Size nb_nodes);

      /// constructor starting with a given graph
      StructuralConstraintUndiGraph(const UndiGraph& graph);

      /// copy constructor
      StructuralConstraintUndiGraph(const StructuralConstraintUndiGraph& from);

      /// move constructor
      StructuralConstraintUndiGraph(StructuralConstraintUndiGraph&& from);

      /// destructor
      virtual ~StructuralConstraintUndiGraph();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      StructuralConstraintUndiGraph& operator=(const StructuralConstraintUndiGraph& from);

      /// move operator
      StructuralConstraintUndiGraph& operator=(StructuralConstraintUndiGraph&& from);

      /// @}

      // ##########################################################################
      /// @name Specific Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// sets a new empty graph from which we will perform checkings
      void setGraph(Size nb_nodes);

      /// sets a new graph from which we will perform checkings
      void setGraphAlone(const UndiGraph& graph);

      /// notify the constraint of a modification of the graph
      /** @warning If an already existing edge is added nothing is done. In
       * particular, no exception is raised.
       * @throws InvalidNode exception is thrown if an edge (x,y) is added and x
       * or y does not belong to the graph nodes */
      void modifyGraphAlone(const EdgeAddition& change);

      /// notify the constraint of a modification of the graph
      /** @warning If a nonexisting edge is removed, nothing is done. In
       * particular, no exception is raised. */
      void modifyGraphAlone(const EdgeDeletion& change);

      /// notify the constraint of a modification of the graph
      /** @warning If an already existing edge is added, or if a nonexisting
       * edge
       * is removed, nothing is done. In particular, no exception is raised.
       * @throws InvalidNode exception is thrown if an edge (x,y) is added and x
       * or y does not belong to the graph nodes */
      void modifyGraphAlone(const GraphChange& change);

      /// indicates whether a change will always violate the constraint
      /** Some learning algorithms need examine several times whether a given
       * graph change can be applied. For instance, the first time edge (X,Y)
       * addition is considered, the learning algorithm may discard this change
       * because it violates the structural constraint, but, later on, other
       * edge
       * removals may induce that the edge addition is now possible. Such change
       * is thus not always invalid. Conversely, there are changes that can be
       * discarded once and for all. For instance,
       * in a 2TBN structure, it is always impossible to add a backward-time
       * arc.
       * Such graph changes are always invalid and are therefore tagged as such
       * by the isAlwaysInvalid method. */
      bool isAlwaysInvalidAlone(const GraphChange& change) const;

      /// checks whether the constraints enable to add edge (x,y)
      /** an arc can be added if and only if its extremal nodes belong to the
       * graph and the edge does not already exist. */
      bool checkEdgeAdditionAlone(NodeId x, NodeId y) const;

      /// checks whether the constraints enable to remove edge (x,y)
      /** an edge can be removed if and only if the edge exists. */
      bool checkEdgeDeletionAlone(NodeId x, NodeId y) const;

      /// checks whether the constraints enable to perform a graph change
      /** An edge can be added if and only if its extremal nodes belong to the
       * graph and the edge does not already exist.
       * An edge can be removed if and only if the edge exists. */
      bool checkModificationAlone(const GraphChange& change) const;

      /// checks whether the constraints enable to add an edge
      /** an arc can be added if and only if its extremal nodes belong to the
       * graph and the edge does not already exist. */
      bool checkModificationAlone(const EdgeAddition& change) const;

      /// checks whether the constraints enable to remove an edge
      /** an edge can be removed if and only if the edge exists. */
      bool checkModificationAlone(const EdgeDeletion& change) const;

      /// @}

#ifndef DOXYGEN_SHOULD_SKIP_THIS
// include the set of methods that enable the structural constraint to
// be standalone, i.e., that it needs not be included into a
// StructuralConstraintSetStatic to be used by learning algorithms
#  define GUM_CONSTRAINT_CLASS_NAME StructuralConstraintUndiGraph
#  include <agrum/BN/learning/constraints/structuralConstraintPattern4UndiGraphHeader.h>
#  undef GUM_CONSTRAINT_CLASS_NAME
#endif   // DOXYGEN_SHOULD_SKIP_THIS

      protected:
      /// the UndiGraph on which we perform checks
      UndiGraph _UndiGraph_graph_;
    };

  } /* namespace learning */

} /* namespace gum */

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/BN/learning/constraints/structuralConstraintUndiGraph_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_STRUCTURAL_CONSTRAINT_UNDIGRAPH_H */
