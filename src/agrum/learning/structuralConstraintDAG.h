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
 * @brief the base class for structural constraints imposed by DAGs
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_STRUCTURAL_CONSTRAINT_DAG_H
#define GUM_LEARNING_STRUCTURAL_CONSTRAINT_DAG_H


#include <agrum/config.h>
#include <agrum/graphs/DAGCycleDetector.h>
#include <agrum/learning/structuralConstraintDiGraph.h>


namespace gum {

  
  namespace learning {

    
    /** @class StructuralConstraintDAG
     * @brief The base class for structural constraints imposed by DAGs
     *
     * This base should always be a virtual parents of the structural constraints
     * classes. This will allow to combine different constraints into a single
     * class
     * @ingroup learning_group
     */
    class StructuralConstraintDAG : protected virtual StructuralConstraintDiGraph {
    public:
      
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      StructuralConstraintDAG ();

      /// constructor starting with an empty graph with a given number of nodes
      StructuralConstraintDAG ( unsigned int nb_nodes );

      /// constructor starting with a given graph
      StructuralConstraintDAG ( const DAG& graph );

      /// destructor
      virtual ~StructuralConstraintDAG ();
      
      /// @}



      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// sets a new graph from which we will perform checkings
      virtual void setGraph ( const DAG& graph );

      /// sets a new empty graph from which we will perform checkings
      virtual void setGraph ( unsigned int nb_nodes ) override;

      /// notify the constraint of a modification of the graph
      /** @warning If an already existing arc is added, or if a nonexisting arc
       * is removed, nothing is done. In particular, no exception is raised.
       * @throws InvalidNode exception is thrown if an arc (x,y) is added and x
       * or y does not belong to the graph nodes 
       * @throws InvalidDirectedCycle exception is thrown if any (directed) cycle
       * is created an arc addition or reversal. */
      virtual void modifyGraph ( const GraphChange& change ) override;

      /// checks whether the constraints enable to add arc (x,y)
      /** an arc can be added if and only if its extremal nodes belong to the
       * graph and the arc does not already exist and would not create a cycle */
      virtual bool checkArcAddition ( NodeId x, NodeId y );

      /// checks whether the constraints enable to remove arc (x,y)
      virtual bool checkArcDeletion ( NodeId x, NodeId y );

      /// checks whether the constraints enable to reverse arc (x,y)
      virtual bool checkArcReversal ( NodeId x, NodeId y );
      
      /// @}


    protected:
      /// the cycle detector used to check quickly graph modifications
      DAGCycleDetector _cycle_detector;


      // ##########################################################################
      /// @name Specific Accessors / Modifiers
      // ##########################################################################
      /// @{
      
      /// sets a new graph from which we will perform checkings
      void _setGraph ( const DAG& graph );

      /// sets a new empty graph from which we will perform checkings
      void _setGraph ( unsigned int nb_nodes );

      /// notify the constraint of a modification of the graph
      /** @warning If an already existing arc is added, or if a nonexisting arc
       * is removed, nothing is done. In particular, no exception is raised.
       * @throws InvalidNode exception is thrown if an arc (x,y) is added and x
       * or y does not belong to the graph nodes 
       * @throws InvalidDirectedCycle exception is thrown if any (directed) cycle
       * is created an arc addition or reversal. */
      void _modifyGraph ( const GraphChange& change );
      
      /// checks whether the constraints enable to add arc (x,y)
      /** an arc can be added if and only if its extremal nodes belong to the
       * graph and the arc does not already exist and would not create a cycle */
      bool _checkArcAddition ( NodeId x, NodeId y );

      /// checks whether the constraints enable to remove arc (x,y)
      bool _checkArcDeletion ( NodeId x, NodeId y );

      /// checks whether the constraints enable to reverse arc (x,y)
      bool _checkArcReversal ( NodeId x, NodeId y );
  
      /// @}

      
      
      /// copy constructor
      StructuralConstraintDAG
      ( const StructuralConstraintDAG& from ) = delete;

      /// move constructor
      StructuralConstraintDAG
      ( StructuralConstraintDAG&& from ) = delete;

      /// copy operator
      StructuralConstraintDAG&
      operator= ( const StructuralConstraintDAG& from ) = delete;

      /// move operator
      StructuralConstraintDAG&
      operator= ( StructuralConstraintDAG&& from ) = delete;

    };
    
 
  } /* namespace learning */

  
} /* namespace gum */


/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/learning/structuralConstraintDAG.inl>
#endif /* GUM_NO_INLINE */


#endif /* GUM_LEARNING_STRUCTURAL_CONSTRAINT_DAG_H */
