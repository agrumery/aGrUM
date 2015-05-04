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
 * @brief the class for structural constraints limiting the number of parents
 * of nodes in a directed graph
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_STRUCTURAL_CONSTRAINT_INDEGREE_H
#define GUM_LEARNING_STRUCTURAL_CONSTRAINT_INDEGREE_H

#include <limits>

#include <agrum/config.h>
#include <agrum/learning/constraints/structuralConstraintSetStatic.h>
#include <agrum/learning/constraints/structuralConstraintDiGraph.h>

namespace gum {

  namespace learning {

    /** @class StructuralConstraintIndegree
     * @brief the class for structural constraints limiting the number of parents
     * of nodes in a directed graph
     *
     * @ingroup learning_group
     */
    class StructuralConstraintIndegree
        : protected virtual StructuralConstraintSetStatic<
              StructuralConstraintDiGraph> {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      StructuralConstraintIndegree();

      /// constructor starting with an empty graph with a given number of nodes
      StructuralConstraintIndegree(unsigned int nb_nodes, unsigned int max_indegree);

      /// constructor starting with a given graph
      StructuralConstraintIndegree(const DiGraph &graph, unsigned int max_indegree);

      /// copy constructor
      StructuralConstraintIndegree(const StructuralConstraintIndegree &from);

      /// move constructor
      StructuralConstraintIndegree(StructuralConstraintIndegree &&from);

      /// destructor
      virtual ~StructuralConstraintIndegree();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      StructuralConstraintIndegree &
      operator=(const StructuralConstraintIndegree &from);

      /// move operator
      StructuralConstraintIndegree &operator=(StructuralConstraintIndegree &&from);

      /// @}

      // ##########################################################################
      /// @name Specific Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// sets the default max indegree for all the nodes in the property
      void setIndegree(const NodeProperty<unsigned int> &max_indegree);

      /** @brief resets the default max indegree and possibly updates the
       * indegree of all nodes */
      void setMaxIndegree(unsigned int max_indegree, bool update_all_node = false);

      /// sets a new graph from which we will perform checkings
      void setGraphAlone(const DiGraph &graph);

      /// notify the constraint of a modification of the graph
      /** @warning If an already existing arc is added, nothing is done. In
       * particular, no exception is raised.
       * @throws InvalidNode exception is thrown if an arc (x,y) is added and x
       * or y does not belong to the graph nodes
       * @throws OutOfUpperBound exception is thrown if the indegree constraint
       * is violated by the arc addition. */
      void modifyGraphAlone(const ArcAddition &change);

      /// notify the constraint of a modification of the graph
      /** @warning If a nonexisting arc is removed, nothing is done. In
       * particular, no exception is raised. */
      void modifyGraphAlone(const ArcDeletion &change);

      /// notify the constraint of a modification of the graph
      /** @warning If an already existing arc is added, or if a nonexisting arc
       * is removed, nothing is done. In particular, no exception is raised.
       * @throws InvalidNode exception is thrown if an arc (x,y) is added and x
       * or y does not belong to the graph nodes
       * @throws OutOfUpperBound exception is thrown if the indegree constraint
       * is violated by the arc reversal. */
      void modifyGraphAlone(const ArcReversal &change);

      /// notify the constraint of a modification of the graph
      /** @warning If an already existing arc is added, or if a nonexisting arc
       * is removed, nothing is done. In particular, no exception is raised.
       * @throws InvalidNode exception is thrown if an arc (x,y) is added and x
       * or y does not belong to the graph nodes
       * @throws OutOfUpperBound exception is thrown if the indegree constraint
       * is violated by an arc addition or reversal. */
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
       * graph and the arc does not already exist and its addition would not
       * violate the indegree constraint of y. */
      bool checkArcAdditionAlone(NodeId x, NodeId y) const noexcept;

      /// checks whether the constraints enable to remove arc (x,y)
      /** an arc can be removed if and only if the arc exists. */
      bool checkArcDeletionAlone(NodeId x, NodeId y) const noexcept;

      /// checks whether the constraints enable to reverse arc (x,y)
      /** an arc can be reversed if and only if it exists and arc (y,x)
       * does not and its addition would not violate the indegree
       * constraint of x. */
      bool checkArcReversalAlone(NodeId x, NodeId y) const noexcept;

      /// checks whether the constraints enable to add an arc
      /** an arc can be added if and only if its extremal nodes belong to the
       * graph and the arc does not already exist and its addition would not
       * violate the indegree constraint of y. */
      bool checkModificationAlone(const ArcAddition &change) const noexcept;

      /// checks whether the constraints enable to remove an arc
      /** an arc can be removed if and only if the arc exists. */
      bool checkModificationAlone(const ArcDeletion &change) const noexcept;

      /// checks whether the constraints enable to reverse an arc
      /** an arc can be reversed if and only if it exists and arc (y,x)
       * does not and its addition would not violate the indegree
       * constraint of x. */
      bool checkModificationAlone(const ArcReversal &change) const noexcept;

      /// checks whether the constraints enable to perform a graph change
      /** An arc can be added if and only if its extremal nodes belong to the
       * graph and the arc does not already exist and its addition would not
       * violate the indegree constraint of y.
       * An arc can be removed if and only if the arc exists.
       * An arc can be reversed if and only if it exists and arc (y,x)
       * does not and its addition would not violate the indegree
       * constraint of x. */
      bool checkModificationAlone(const GraphChange &change) const noexcept;

/// @}

// include the set of methods that enable the structural constraint to
// be standalone, i.e., that it needs not be included into a
// StructuralConstraintSetStatic to be used by learning algorithms
#define GUM_CONSTRAINT_CLASS_NAME StructuralConstraintIndegree
#include <agrum/learning/constraints/structuralConstraintPatternHeader.h>
#undef GUM_CONSTRAINT_CLASS_NAME

      protected:
      /// the max number of parents per node
      NodeProperty<unsigned int> _Indegree__max_parents;

      /// a default max indegree to assign for nodes without specified indegree
      unsigned int _Indegree__max_indegree{std::numeric_limits<unsigned int>::max()};
    };

  } /* namespace learning */

} /* namespace gum */

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/learning/constraints/structuralConstraintIndegree.inl>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_STRUCTURAL_CONSTRAINT_INDEGREE_H */
