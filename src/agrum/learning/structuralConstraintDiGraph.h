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

      /// destructor
      virtual ~StructuralConstraintDiGraph ();
      
      /// @}



      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// sets a new graph from which we will perform checkings
      virtual void setGraph ( const DiGraph& graph );

      /// adds a new arc into the graph (without checking the constraints)
      /** @warning Ff the arc already exists, nothing is done. In particular,
       * no exception is raised.
       * @throws InvalidNode exception is thrown if x or y does not belong to the
       * graph nodes */
      virtual void insertArc ( NodeId x, NodeId y );

      /// removes an arc from the graph (without checking the constraints)
      /** @warning if the arc does not exist, nothing is done. In particular,
       * no exception is thrown. */
      virtual void eraseArc ( NodeId x, NodeId y );

      /// checks whether the constraints enable to add arc (x,y)
      virtual bool checkArcAddition ( NodeId x, NodeId y ) = 0;

      /// checks whether the constraints enable to remove arc (x,y)
      virtual bool checkArcDeletion ( NodeId x, NodeId y ) = 0;

      /// checks whether the constraints enable to reverse arc (x,y)
      virtual bool checkArcReversal ( NodeId x, NodeId y ) = 0;
      
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
