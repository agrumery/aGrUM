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
 * @brief the structural constraints imposed by DBNs
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_STRUCTURAL_CONSTRAINT_DBN_H
#define GUM_LEARNING_STRUCTURAL_CONSTRAINT_DBN_H


#include <agrum/config.h>
#include <agrum/learning/structuralConstraintDiGraph.h>


namespace gum {

  
  namespace learning {

    
    /** @class StructuralConstraintDBN
     * @brief The class for checking the structural constraints imposed by DBNs
     *
     * @ingroup learning_group
     */
    class StructuralConstraintDBN : public virtual StructuralConstraintDAG {
    public:
      
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      StructuralConstrainDBN ();

      /// destructor
      virtual ~StructuralConstraintDBN ();
      
      /// @}



      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// sets a new graph from which we will perform checkings
      virtual void setGraph ( const DiGraph& graph ) override;

      /// sets a new graph from which we will perform checkings
      virtual void setGraph ( const DAG& graph ) override;
      
      /// adds a new arc into the graph (without checking the constraints)
      /** @warning if the arc already exists, nothing is done. In particular,
       * no exception is raised.
       * @throws InvalidNode exception is thrown if x or y does not belong to the
       * graph nodes */
      virtual void insertArc ( NodeId x, NodeId y ) override;

      /// removes an arc from the graph (without checking the constraints)
      /** @warning if the arc does not exist, nothing is done. In particular,
       * no exception is thrown. */
      virtual void eraseArc ( NodeId x, NodeId y ) override;

      /// checks whether the constraints enable to add arc (x,y)
      virtual void checkArcAddition ( NodeId x, NodeId y ) override;

      /// checks whether the constraints enable to reverse arc (x,y)
      virtual void checkArcReversal ( NodeId x, NodeId y ) override;
      
      /// @}


    private:
      /// indicates whether a given node is in the first or the second time slice
      /** true = in 1st time slice, false = in the 2nd time slice */
      NodeProperty<bool> _slice_node;
      
      /// copy constructor
      StructuralConstraintDBN
      ( const StructuralConstraintDBN& from ) = delete;

      /// move constructor
      StructuralConstraintDBN
      ( StructuralConstraintDBN&& from ) = delete;

      /// copy operator
      StructuralConstraintDBN&
      operator= ( const StructuralConstraintDBN& from ) = delete;

      /// move operator
      StructuralConstraintDBN&
      operator= ( StructuralConstraintDBN&& from ) = delete;

    };
    
 
  } /* namespace learning */

  
} /* namespace gum */


/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/learning/structuralConstraintDiGraph.inl>
#endif /* GUM_NO_INLINE */


#endif /* GUM_LEARNING_STRUCTURAL_CONSTRAINT_DIGRAPH_H */
