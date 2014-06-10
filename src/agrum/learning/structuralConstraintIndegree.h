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


#include <agrum/config.h>
#include <agrum/learning/structuralConstraintSet.h>
#include <agrum/learning/structuralConstraintDiGraph.h>


namespace gum {

  
  namespace learning {

    
    /** @class StructuralConstraintIndegree
     * @brief the class for structural constraints limiting the number of parents
     * of nodes in a directed graph
     *
     * @ingroup learning_group
     */
    class StructuralConstraintIndegree :
      protected virtual StructuralConstraintSet<StructuralConstraintDiGraph> {
    public:
      
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      StructuralConstraintIndegree ();

      /// constructor starting with an empty graph with a given number of nodes
      StructuralConstraintIndegree ( unsigned int nb_nodes,
                                     unsigned int max_indegree );

      /// constructor starting with a given graph
      StructuralConstraintIndegree ( const DiGraph& graph,
                                     unsigned int max_indegree );
       
      /// destructor
      virtual ~StructuralConstraintIndegree ();
      
      /// @}



      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// sets a new graph from which we will perform checkings
      /** @throws InvalidNode if a node in the graph has no max indegree
       * already assigned */
      void setGraph ( const DiGraph& graph );

      /// sets the indegree for each node
      /** @throws InvalidNode if a node in the graph has no max indegree */
      void setIndegree ( const NodeProperty<unsigned int>& max_indegree );

      /// resets the max indegree
      void setIndegree ( unsigned int max_indegree );

      /// notify the constraint of a modification of the graph
      /** @warning If an already existing arc is added, nothing is done. In
       * particular, no exception is raised.
       * @throws InvalidNode exception is thrown if an arc (x,y) is added and x
       * or y does not belong to the graph nodes 
       * @throws OutOfUpperBound exception is thrown if the indegree constraint
       * is violated by the arc addition. */
      void modifyGraph ( const ArcAddition& change );

      /// notify the constraint of a modification of the graph
      /** @warning If a nonexisting arc is removed, nothing is done. In
       * particular, no exception is raised. */
      void modifyGraph ( const ArcDeletion& change );

      /// notify the constraint of a modification of the graph
      /** @warning If an already existing arc is added, or if a nonexisting arc
       * is removed, nothing is done. In particular, no exception is raised.
       * @throws InvalidNode exception is thrown if an arc (x,y) is added and x
       * or y does not belong to the graph nodes 
       * @throws OutOfUpperBound exception is thrown if the indegree constraint
       * is violated by the arc reversal. */
      void modifyGraph ( const ArcReversal& change );

      /// notify the constraint of a modification of the graph
      /** @warning If an already existing arc is added, or if a nonexisting arc
       * is removed, nothing is done. In particular, no exception is raised.
       * @throws InvalidNode exception is thrown if an arc (x,y) is added and x
       * or y does not belong to the graph nodes 
       * @throws OutOfUpperBound exception is thrown if the indegree constraint
       * is violated by an arc addition or reversal. */
      void modifyGraph ( const GraphChange& change );

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
      bool isAlwaysInvalid ( const GraphChange& change ) const noexcept;

      /// checks whether the constraints enable to add arc (x,y)
      /** an arc can be added if and only if its extremal nodes belong to the
       * graph and the arc does not already exist and its addition would not
       * violate the indegree constraint of y. */
      bool checkArcAddition ( NodeId x, NodeId y ) const noexcept;

      /// checks whether the constraints enable to remove arc (x,y)
      /** an arc can be removed if and only if the arc exists. */
      bool checkArcDeletion ( NodeId x, NodeId y ) const noexcept;
 
      /// checks whether the constraints enable to reverse arc (x,y)
      /** an arc can be reversed if and only if it exists and arc (y,x)
       * does not and its addition would not violate the indegree
       * constraint of x. */
      bool checkArcReversal ( NodeId x, NodeId y ) const noexcept;
 
      /// checks whether the constraints enable to add an arc
      /** an arc can be added if and only if its extremal nodes belong to the
       * graph and the arc does not already exist and its addition would not
       * violate the indegree constraint of y. */
      bool checkModification ( const ArcAddition& change ) const noexcept;

      /// checks whether the constraints enable to remove an arc
      /** an arc can be removed if and only if the arc exists. */
      bool checkModification ( const ArcDeletion& change ) const noexcept;

      /// checks whether the constraints enable to reverse an arc
      /** an arc can be reversed if and only if it exists and arc (y,x)
       * does not and its addition would not violate the indegree
       * constraint of x. */
      bool checkModification ( const ArcReversal& change ) const noexcept;
      
      /// checks whether the constraints enable to perform a graph change
      /** An arc can be added if and only if its extremal nodes belong to the
       * graph and the arc does not already exist and its addition would not
       * violate the indegree constraint of y.
       * An arc can be removed if and only if the arc exists.
       * An arc can be reversed if and only if it exists and arc (y,x)
       * does not and its addition would not violate the indegree
       * constraint of x. */
      bool checkModification ( const GraphChange& change ) const noexcept;
     
      /// @}

      
      // ##########################################################################
      /// @name Specific Accessors / Modifiers
      // ##########################################################################
      /// @{
      
      /// sets a new graph from which we will perform checkings
      void setGraphAlone ( const DiGraph& graph );

      /// notify the constraint of a modification of the graph
      /** @warning If an already existing arc is added, nothing is done. In
       * particular, no exception is raised.
       * @throws InvalidNode exception is thrown if an arc (x,y) is added and x
       * or y does not belong to the graph nodes 
       * @throws OutOfUpperBound exception is thrown if the indegree constraint
       * is violated by the arc addition. */
      void modifyGraphAlone ( const ArcAddition& change );

      /// notify the constraint of a modification of the graph
      /** @warning If a nonexisting arc is removed, nothing is done. In
       * particular, no exception is raised. */
      void modifyGraphAlone ( const ArcDeletion& change );

      /// notify the constraint of a modification of the graph
      /** @warning If an already existing arc is added, or if a nonexisting arc
       * is removed, nothing is done. In particular, no exception is raised.
       * @throws InvalidNode exception is thrown if an arc (x,y) is added and x
       * or y does not belong to the graph nodes 
       * @throws OutOfUpperBound exception is thrown if the indegree constraint
       * is violated by the arc reversal. */
      void modifyGraphAlone ( const ArcReversal& change );

      /// notify the constraint of a modification of the graph
      /** @warning If an already existing arc is added, or if a nonexisting arc
       * is removed, nothing is done. In particular, no exception is raised.
       * @throws InvalidNode exception is thrown if an arc (x,y) is added and x
       * or y does not belong to the graph nodes 
       * @throws OutOfUpperBound exception is thrown if the indegree constraint
       * is violated by an arc addition or reversal. */
      void modifyGraphAlone ( const GraphChange& change );

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
      bool isAlwaysInvalidAlone ( const GraphChange& change ) const noexcept;
      
      /// checks whether the constraints enable to add arc (x,y)
      /** an arc can be added if and only if its extremal nodes belong to the
       * graph and the arc does not already exist and its addition would not
       * violate the indegree constraint of y. */
      bool checkArcAdditionAlone ( NodeId x, NodeId y ) const noexcept;

      /// checks whether the constraints enable to remove arc (x,y)
      /** an arc can be removed if and only if the arc exists. */
      bool checkArcDeletionAlone ( NodeId x, NodeId y ) const noexcept;
 
      /// checks whether the constraints enable to reverse arc (x,y)
      /** an arc can be reversed if and only if it exists and arc (y,x)
       * does not and its addition would not violate the indegree
       * constraint of x. */
      bool checkArcReversalAlone ( NodeId x, NodeId y ) const noexcept;

      /// checks whether the constraints enable to add an arc
      /** an arc can be added if and only if its extremal nodes belong to the
       * graph and the arc does not already exist and its addition would not
       * violate the indegree constraint of y. */
      bool checkModificationAlone ( const ArcAddition& change ) const noexcept;

      /// checks whether the constraints enable to remove an arc
      /** an arc can be removed if and only if the arc exists. */
      bool checkModificationAlone ( const ArcDeletion& change ) const noexcept;

      /// checks whether the constraints enable to reverse an arc
      /** an arc can be reversed if and only if it exists and arc (y,x)
       * does not and its addition would not violate the indegree
       * constraint of x. */
      bool checkModificationAlone ( const ArcReversal& change ) const noexcept;
      
      /// checks whether the constraints enable to perform a graph change
      /** An arc can be added if and only if its extremal nodes belong to the
       * graph and the arc does not already exist and its addition would not
       * violate the indegree constraint of y.
       * An arc can be removed if and only if the arc exists.
       * An arc can be reversed if and only if it exists and arc (y,x)
       * does not and its addition would not violate the indegree
       * constraint of x. */
      bool checkModificationAlone ( const GraphChange& change ) const noexcept;
      
      /// @}
      

    protected:
      /// the max number of parents per node
      NodeProperty<unsigned int> _max_parents;


 
      
      /// copy constructor
      StructuralConstraintIndegree
      ( const StructuralConstraintIndegree& from ) = delete;

      /// move constructor
      StructuralConstraintIndegree
      ( StructuralConstraintIndegree&& from ) = delete;

      /// copy operator
      StructuralConstraintIndegree&
      operator= ( const StructuralConstraintIndegree& from ) = delete;

      /// move operator
      StructuralConstraintIndegree&
      operator= ( StructuralConstraintIndegree&& from ) = delete;

    };
    
 
  } /* namespace learning */

  
} /* namespace gum */


/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/learning/structuralConstraintIndegree.inl>
#endif /* GUM_NO_INLINE */


#endif /* GUM_LEARNING_STRUCTURAL_CONSTRAINT_INDEGREE_H */
