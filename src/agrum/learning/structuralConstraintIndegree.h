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
      protected virtual StructuralConstraintDiGraph {
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
      virtual void setGraph ( const DiGraph& graph,
                              const NodeProperty<unsigned int>& max_indegree );

      /// sets a new graph from which we will perform checkings
      virtual void setGraph ( const DiGraph& graph,
                              unsigned int max_indegree );

      /// adds a new arc into the graph
      /** @warning If the arc already exists, nothing is done. In particular,
       * no exception is raised.
       * @throws InvalidNode exception is thrown if x or y does not belong to the
       * graph nodes */
      virtual void insertArc ( NodeId x, NodeId y ) override;

      /// removes an arc from the graph
      /** @warning if the arc does not exist, nothing is done. In particular,
       * no exception is thrown. */
      virtual void eraseArc ( NodeId x, NodeId y ) override;

      /// checks whether the constraints enable to add arc (x,y)
      /** an arc can be added if and only if its extremal nodes belong to the
       * graph and the arc does not already exist. */
      virtual bool checkArcAddition ( NodeId x, NodeId y ) override;

      /// checks whether the constraints enable to remove arc (x,y)
      /** an arc can be removed if and only if the arc exists. */
      virtual bool checkArcDeletion ( NodeId x, NodeId y ) override;
 
      /// checks whether the constraints enable to reverse arc (x,y)
      /** an arc can be reversed if and only if it exists and arc (y,x)
       * does not. */
      virtual bool checkArcReversal ( NodeId x, NodeId y ) override;
      
      /// @}


    protected:
      /// the max number of parents per node
      NodeProperty<unsigned int> _max_parents;


      // ##########################################################################
      /// @name Specific Accessors / Modifiers
      // ##########################################################################
      /// @{
      
      /// sets a new graph from which we will perform checkings
      void _setGraph ( const DiGraph& graph,
                       const NodeProperty<unsigned int>& max_indegree );

      /// sets a new graph from which we will perform checkings
      void _setGraph ( const DiGraph& graph,
                       unsigned int max_indegree );

      /// adds a new arc into the graph
      /** @warning If the arc already exists, nothing is done. In particular,
       * no exception is raised.
       * @throws InvalidNode exception is thrown if x or y does not belong to the
       * graph nodes */
      void _insertArc ( NodeId x, NodeId y );

      /// removes an arc from the graph
      /** @warning if the arc does not exist, nothing is done. In particular,
       * no exception is thrown. */
      void _eraseArc ( NodeId x, NodeId y );

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
