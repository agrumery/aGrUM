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
 * @brief the class for structural constraints over nodes that belong to 2 time
 * slices
 *
 * In DBNs, it is forbidden to add arcs from nodes at time slice t to nodes at
 * time slice s, where s < t. This class allows for taking this kind of constaint
 * into account
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_STRUCTURAL_CONSTRAINT_2TIME_SLICE_H
#define GUM_LEARNING_STRUCTURAL_CONSTRAINT_2TIME_SLICE_H


#include <agrum/config.h>
#include <agrum/learning/structuralConstraintDiGraph.h>


namespace gum {

  
  namespace learning {

    
    /** @class StructuralConstraint2TimeSlice
     * @brief the class for structural constraints over nodes that belong to 2 time
     * slices
     *
     * @ingroup learning_group
     */
    class StructuralConstraint2TimeSlice :
      protected virtual StructuralConstraintDiGraph {
    public:
      
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      StructuralConstraint2TimeSlice ();

      /// constructor starting with an empty graph with a given number of nodes
      /** @param nb_nodes the number of nodes in the graph
       * @param time_slice indicates for each node in the graph whether it belongs
       * to the first time slice (false) or to the second (true) */
      StructuralConstraint2TimeSlice ( const NodeProperty<bool>& time_slice );

      /// constructor starting with a given graph
      StructuralConstraint2TimeSlice ( const DiGraph& graph,
                                       const NodeProperty<bool>& time_slice );


      /// destructor
      virtual ~StructuralConstraint2TimeSlice ();
      
      /// @}



      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// sets a new graph from which we will perform checkings
      virtual void setGraph ( const DiGraph& graph,
                              const NodeProperty<bool>& time_slice );

      /// sets a new empty graph from which we will perform checkings
      virtual void setGraph ( const NodeProperty<bool>& time_slice );

      /// notify the constraint of a modification of the graph
      /** @warning If an already existing arc is added, or if a nonexisting arc
       * is removed, nothing is done. In particular, no exception is raised.
       * @throws InvalidNode exception is thrown if an arc (x,y) is added and x
       * or y does not belong to the graph nodes 
       * @throws InvalidArc exception is thrown if any time-backward arc
       * is created by an arc addition or reversal. */
      virtual void modifyGraph ( const GraphChange& change ) override;

      /// checks whether the constraints enable to add arc (x,y)
      /** an arc can be added if and only if its extremal nodes belong to the
       * graph and the arc does not already exist. */
      virtual bool checkArcAddition ( NodeId x, NodeId y );

      /// checks whether the constraints enable to remove arc (x,y)
      /** an arc can be removed if and only if the arc exists. */
      virtual bool checkArcDeletion ( NodeId x, NodeId y );

      /// checks whether the constraints enable to reverse arc (x,y)
      /** an arc can be reversed if and only if it exists and arc (y,x)
       * does not. */
      virtual bool checkArcReversal ( NodeId x, NodeId y );
      
      /// @}


    protected:
      /// time slices to which belong the nodes ( false = 0 or true = 1 )
      NodeProperty<bool> _time_slice;


      // ##########################################################################
      /// @name Specific Accessors / Modifiers
      // ##########################################################################
      /// @{
      
      /// sets a new graph from which we will perform checkings
      void _setGraph ( const DiGraph& graph,
                       const NodeProperty<bool>& time_slice );

      /// sets a new empty graph from which we will perform checkings
      void _setGraph ( const NodeProperty<bool>& time_slice );

      /// notify the constraint of a modification of the graph
      /** @warning If an already existing arc is added, or if a nonexisting arc
       * is removed, nothing is done. In particular, no exception is raised.
       * @throws InvalidNode exception is thrown if an arc (x,y) is added or
       * reversed and x or y does not belong to the graph nodes 
       * @throws InvalidArc exception is thrown if any time-backward arc
       * is created by an arc addition or reversal. */
      void _modifyGraph ( const GraphChange& change );

      /// checks whether the constraints enable to add arc (x,y)
      /** an arc can be added if and only if its extremal nodes belong to the
       * graph and the arc does not already exist. */
      bool _checkArcAddition ( NodeId x, NodeId y );

      /// checks whether the constraints enable to remove arc (x,y)
      /** an arc can be removed if and only if the arc exists. */
      bool _checkArcDeletion ( NodeId x, NodeId y );

      /// checks whether the constraints enable to reverse arc (x,y)
      /** an arc can be reversed if and only if it exists and arc (y,x)
       * does not. */
      bool _checkArcReversal ( NodeId x, NodeId y );
 
      /// @}


      
      /// copy constructor
      StructuralConstraint2TimeSlice
      ( const StructuralConstraint2TimeSlice& from ) = delete;

      /// move constructor
      StructuralConstraint2TimeSlice
      ( StructuralConstraint2TimeSlice&& from ) = delete;

      /// copy operator
      StructuralConstraint2TimeSlice&
      operator= ( const StructuralConstraint2TimeSlice& from ) = delete;

      /// move operator
      StructuralConstraint2TimeSlice&
      operator= ( StructuralConstraint2TimeSlice&& from ) = delete;

    };
    
 
  } /* namespace learning */

  
} /* namespace gum */


/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/learning/structuralConstraint2TimeSlice.inl>
#endif /* GUM_NO_INLINE */


#endif /* GUM_LEARNING_STRUCTURAL_CONSTRAINT_2TIME_SLICE_H */
