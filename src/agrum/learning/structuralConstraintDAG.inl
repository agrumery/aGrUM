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
#ifndef DOXYGEN_SHOULD_SKIP_THIS


#include <agrum/config.h>
#include <agrum/graphs/DAGCycleDetector.h>
#include <agrum/learning/structuralConstraintDiGraph.h>


namespace gum {

  
  namespace learning {

    
    /// sets a new graph from which we will perform checkings
    INLINE void StructuralConstraintDAG::setGraph ( const DiGraph& graph ) {
      DAG g;
      g.populateNodes ( graph.nodes () );
      for ( const auto& arc : graph.arcs() )
        g.insertArc ( arc.tail (), arc.head () );
      _cycle_detector.setDAG ( g );
      StructuralConstraintDiGraph::setGraph ( graph );
    }

    
    /// sets a new graph from which we will perform checkings
    INLINE void StructuralConstraintDAG::setGraph ( const DAG& graph ) {
      _cycle_detector.setDAG ( graph );
      StructuralConstraintDiGraph::setGraph ( graph );
    }


    /// adds a new arc into the graph (without checking the constraints)
    INLINE void StructuralConstraintDAG::insertArc ( NodeId x, NodeId y ) {
      _cycle_detector.insertArc ( x, y );
      StructuralConstraintDiGraph::insertArc ( x, y );
    }


    /// removes an arc from the graph (without checking the constraints)
    INLINE void StructuralConstraintDAG::eraseArc ( NodeId x, NodeId y ) {
      _cycle_detector.eraseArc ( x, y );
      StructuralConstraintDiGraph::eraseArc ( x, y );
    }

    
    /// checks whether the constraints enable to add arc (x,y)
    INLINE bool StructuralConstraintDAG::checkArcAddition ( NodeId x, NodeId y ) {
      return ! _cycle_detector.hasCycleFromAddition ( x, y );
    }


    /// checks whether the constraints enable to remove arc (x,y)
    INLINE bool StructuralConstraintDAG::checkArcDeletion ( NodeId x, NodeId y ) {
     return ! _cycle_detector.hasCycleFromDeletion ( x, y );
    }
      

    /// checks whether the constraints enable to reverse arc (x,y)
    INLINE bool StructuralConstraintDAG::checkArcReversal ( NodeId x, NodeId y ) {
      return ! _cycle_detector.hasCycleFromReversal ( x, y );
    }

    
  } /* namespace learning */

  
} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
