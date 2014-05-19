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
#include <agrum/learning/graphChange.h>


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
    class StructuralConstraintDiGraph {
    public:
      
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      StructuralConstraintDiGraph ();

      /// constructor starting with an empty graph with a given number of nodes
      StructuralConstraintDiGraph ( unsigned int nb_nodes );

      /// constructor starting with a given graph
      StructuralConstraintDiGraph ( const DiGraph& graph );

      /// destructor
      virtual ~StructuralConstraintDiGraph ();
      
      /// @}



      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// sets a new graph from which we will perform checkings
      virtual void setGraph ( const DiGraph& graph );

      /// sets a new empty graph from which we will perform checkings
      virtual void setGraph ( unsigned int nb_nodes );

      /// notify the constraint of a modification of the graph
      /** @warning If an already existing arc is added, or if a nonexisting arc
       * is removed, nothing is done. In particular, no exception is raised.
       * @throws InvalidNode exception is thrown if an arc (x,y) is added and x
       * or y does not belong to the graph nodes */
      virtual void modifyGraph ( const GraphChange& change );

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
      virtual bool isAlwaysInvalid ( const GraphChange& change );
      
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
      
      /// checks whether the constraints enable to perform a graph change
      /** An arc can be added if and only if its extremal nodes belong to the
       * graph and the arc does not already exist.
       * An arc can be removed if and only if the arc exists.
       * An arc (x,y) can be reversed if and only if it exists and arc (y,x)
       * does not. */
      virtual bool checkModification ( GraphChange& change );

      /// checks whether the constraints enable to add an arc
      /** an arc can be added if and only if its extremal nodes belong to the
       * graph and the arc does not already exist. */
      virtual bool checkModification ( ArcAddition& change );

      /// checks whether the constraints enable to remove an arc
      /** an arc can be removed if and only if the arc exists. */
      virtual bool checkModification ( ArcDeletion& change );

      /// checks whether the constraints enable to reverse an arc
      /** an arc (x,y) can be reversed if and only if it exists and arc (y,x)
       * does not. */
      virtual bool checkModification ( ArcReversal& change );

      /// @}


    protected:
      /// the DiGraph on which we perform checks
      DiGraph _graph;

      
      
      /// copy constructor
      StructuralConstraintDiGraph
      ( const StructuralConstraintDiGraph& from ) = delete;

      /// move constructor
      StructuralConstraintDiGraph
      ( StructuralConstraintDiGraph&& from ) = delete;

      /// copy operator
      StructuralConstraintDiGraph&
      operator= ( const StructuralConstraintDiGraph& from ) = delete;

      /// move operator
      StructuralConstraintDiGraph&
      operator= ( StructuralConstraintDiGraph&& from ) = delete;

    };
    
 
  } /* namespace learning */

  
} /* namespace gum */


/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/learning/structuralConstraintDiGraph.inl>
#endif /* GUM_NO_INLINE */


#endif /* GUM_LEARNING_STRUCTURAL_CONSTRAINT_DIGRAPH_H */
