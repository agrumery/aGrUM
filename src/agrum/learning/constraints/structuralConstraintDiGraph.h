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
 * @brief the base class for structural constraints used by learning algorithms
 * that learn a directed graph structure
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_STRUCTURAL_CONSTRAINT_DIGRAPH_H
#define GUM_LEARNING_STRUCTURAL_CONSTRAINT_DIGRAPH_H

#include <agrum/config.h>
#include <agrum/graphs/diGraph.h>
#include <agrum/learning/structureUtils/graphChange.h>
#include <agrum/learning/constraints/structuralConstraint.h>

namespace gum {

  namespace learning {

    /** @class StructuralConstraintDiGraph
     * @brief The base class for structural constraints used by learning
     * algorithms that learn a directed graph structure
     *
     * This base should always be a virtual parents of the structural constraints
     * classes. This will allow to combine different constraints into a single
     * class
     * @ingroup learning_group
     */
    class StructuralConstraintDiGraph : public virtual StructuralConstraintEmpty {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      StructuralConstraintDiGraph();

      /// constructor starting with an empty graph with a given number of nodes
      StructuralConstraintDiGraph(unsigned int nb_nodes);

      /// constructor starting with a given graph
      StructuralConstraintDiGraph(const DiGraph &graph);

      /// copy constructor
      StructuralConstraintDiGraph(const StructuralConstraintDiGraph &from);

      /// move constructor
      StructuralConstraintDiGraph(StructuralConstraintDiGraph &&from);

      /// destructor
      virtual ~StructuralConstraintDiGraph();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      StructuralConstraintDiGraph &
      operator=(const StructuralConstraintDiGraph &from);

      /// move operator
      StructuralConstraintDiGraph &operator=(StructuralConstraintDiGraph &&from);

      /// @}

      // ##########################################################################
      /// @name Specific Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// sets a new empty graph from which we will perform checkings
      void setGraph(unsigned int nb_nodes);

      /// sets a new graph from which we will perform checkings
      void setGraphAlone(const DiGraph &graph);

      /// notify the constraint of a modification of the graph
      /** @warning If an already existing arc is added nothing is done. In
       * particular, no exception is raised.
       * @throws InvalidNode exception is thrown if an arc (x,y) is added and x
       * or y does not belong to the graph nodes */
      void modifyGraphAlone(const ArcAddition &change);

      /// notify the constraint of a modification of the graph
      /** @warning If a nonexisting arc is removed, nothing is done. In
       * particular, no exception is raised. */
      void modifyGraphAlone(const ArcDeletion &change);

      /// notify the constraint of a modification of the graph
      /** @warning If an already existing arc is added, or if a nonexisting arc
       * is removed, nothing is done. In particular, no exception is raised.
       * @throws InvalidNode exception is thrown if at least one extremity of
       * the arc does not belong to the graph nodes */
      void modifyGraphAlone(const ArcReversal &change);

      /// notify the constraint of a modification of the graph
      /** @warning If an already existing arc is added, or if a nonexisting arc
       * is removed, nothing is done. In particular, no exception is raised.
       * @throws InvalidNode exception is thrown if an arc (x,y) is added and x
       * or y does not belong to the graph nodes */
      void modifyGraphAlone(const GraphChange &change);

      /// indicates whether a change will always violate the constraint
      /** Some learning algorithms need examine several times whether a given
       * graph change can be applied. For instance, the first time arc (X,Y)
       * addition is considered, the learning algorithm may discard this change
       * because it violates the structural constraint (e.g., if the latter
       * enforces a DAG structure, this arc addition might induce a directed
       * cycle), but, later on, other arc removal may induce that the arc addition
       * is now possible. Such change is thus not always invalid. Conversely,
       * there are changes that can be discarded once and for all. For instance,
       * in a 2TBN structure, it is always impossible to add a backward-time arc.
       * Such graph changes are always invalid and are therefore tagged as such
       * by the isAlwaysInvalid method. */
      bool isAlwaysInvalidAlone(const GraphChange &change) const noexcept;

      /// checks whether the constraints enable to add arc (x,y)
      /** an arc can be added if and only if its extremal nodes belong to the
       * graph and the arc does not already exist. */
      bool checkArcAdditionAlone(NodeId x, NodeId y) const noexcept;

      /// checks whether the constraints enable to remove arc (x,y)
      /** an arc can be removed if and only if the arc exists. */
      bool checkArcDeletionAlone(NodeId x, NodeId y) const noexcept;

      /// checks whether the constraints enable to reverse arc (x,y)
      /** an arc can be reversed if and only if it exists and arc (y,x)
       * does not. */
      bool checkArcReversalAlone(NodeId x, NodeId y) const noexcept;

      /// checks whether the constraints enable to perform a graph change
      /** An arc can be added if and only if its extremal nodes belong to the
       * graph and the arc does not already exist.
       * An arc can be removed if and only if the arc exists.
       * An arc (x,y) can be reversed if and only if it exists and arc (y,x)
       * does not. */
      bool checkModificationAlone(const GraphChange &change) const noexcept;

      /// checks whether the constraints enable to add an arc
      /** an arc can be added if and only if its extremal nodes belong to the
       * graph and the arc does not already exist. */
      bool checkModificationAlone(const ArcAddition &change) const noexcept;

      /// checks whether the constraints enable to remove an arc
      /** an arc can be removed if and only if the arc exists. */
      bool checkModificationAlone(const ArcDeletion &change) const noexcept;

      /// checks whether the constraints enable to reverse an arc
      /** an arc (x,y) can be reversed if and only if it exists and arc (y,x)
       * does not. */
      bool checkModificationAlone(const ArcReversal &change) const noexcept;

/// @}

// include the set of methods that enable the structural constraint to
// be standalone, i.e., that it needs not be included into a
// StructuralConstraintSetStatic to be used by learning algorithms
#define GUM_CONSTRAINT_CLASS_NAME StructuralConstraintDiGraph
#include <agrum/learning/constraints/structuralConstraintPatternHeader.h>
#undef GUM_CONSTRAINT_CLASS_NAME

      protected:
      /// the DiGraph on which we perform checks
      DiGraph _DiGraph__graph;
    };

  } /* namespace learning */

} /* namespace gum */

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/learning/constraints/structuralConstraintDiGraph.inl>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_STRUCTURAL_CONSTRAINT_DIGRAPH_H */
