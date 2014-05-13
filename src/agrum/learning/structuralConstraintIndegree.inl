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

#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {

  
  namespace learning {


    /// sets a new graph from which we will perform checkings
    INLINE void StructuralConstraintIndegree::_setGraph
    ( const DiGraph& graph,
      const NodeProperty<unsigned int>& max_indegree ) {
      // check that the max_indegree corresponds to the graph
      if ( max_indegree.size () != graph.size () ) {
        GUM_ERROR ( SizeError,
                    "the graph and the property do not have the same size" );
      }
      for ( const auto id : graph ) {
        if ( ! max_indegree.exists ( id ) ) {
          GUM_ERROR ( InvalidNode,
                      "there exists a node in the graph without max indegree" );
        }
      }
      _max_parents =  max_indegree;
    }
    
    
    /// sets a new graph from which we will perform checkings
    INLINE void StructuralConstraintIndegree::setGraph
    ( const DiGraph& graph,
      const NodeProperty<unsigned int>& max_indegree ) {
      StructuralConstraintIndegree::_setGraph ( graph, max_indegree );
      StructuralConstraintDiGraph::setGraph ( graph );
    }
    

    /// sets a new graph from which we will perform checkings
    INLINE void StructuralConstraintIndegree::_setGraph
    ( const DiGraph& graph,
      unsigned int max_indegree ) {
      _max_parents.clear ();
      for ( const auto id : graph ) {
        _max_parents.insert ( id, max_indegree );
      }
    }

    
    /// sets a new graph from which we will perform checkings
    INLINE void StructuralConstraintIndegree::setGraph
    ( const DiGraph& graph,
      unsigned int max_indegree ) {
      StructuralConstraintIndegree::_setGraph ( graph, max_indegree );
      StructuralConstraintDiGraph::setGraph ( graph );
    }


    /// checks whether the constraints enable to add arc (x,y)
    INLINE bool StructuralConstraintIndegree::_checkArcAddition
    ( NodeId x, NodeId y ) {
      return ( _max_parents[y] > _graph.parents (y).size () );
    }


    /// checks whether the constraints enable to add arc (x,y)
    INLINE bool StructuralConstraintIndegree::checkArcAddition
    ( NodeId x, NodeId y ) {
      return StructuralConstraintDiGraph::checkArcAddition ( x,y ) &&
        StructuralConstraintIndegree::_checkArcAddition ( x,y );
    }

    
    /// checks whether the constraints enable to remove arc (x,y)
    INLINE bool StructuralConstraintIndegree::_checkArcDeletion
    ( NodeId x, NodeId y ) {
      return true;
    }

    
    /// checks whether the constraints enable to remove arc (x,y)
    INLINE bool StructuralConstraintIndegree::checkArcDeletion
    ( NodeId x, NodeId y ) {
      return  StructuralConstraintIndegree::_checkArcDeletion ( x,y ) &&
        StructuralConstraintDiGraph::checkArcDeletion ( x,y );
    }

    
    /// checks whether the constraints enable to reverse arc (x,y)
    INLINE bool StructuralConstraintIndegree::_checkArcReversal
    ( NodeId x, NodeId y ) {   
      return ( _max_parents[x] > _graph.parents (x).size () );
    }

    
    /// checks whether the constraints enable to reverse arc (x,y)
    INLINE bool StructuralConstraintIndegree::checkArcReversal
    ( NodeId x, NodeId y ) {   
      return StructuralConstraintDiGraph::checkArcReversal ( x,y ) &&
        StructuralConstraintIndegree::_checkArcReversal ( x,y );
    }
    
    
    /// adds a new arc into the graph
    INLINE void
    StructuralConstraintIndegree::_modifyGraph ( const GraphChange& change ) {
      switch ( change.type () ) {
      case GraphChangeType::ARC_ADDITION:
        // check that the max indegree is not already reached
        if ( _graph.exists ( change.node2 () ) &&
             _max_parents[change.node2 ()] ==
             _graph.parents ( change.node2 () ).size () ) {
          GUM_ERROR ( OutOfUpperBound, "there are already too many parents" );
        }
        break;

      case GraphChangeType::ARC_DELETION:
        break;

      case GraphChangeType::ARC_REVERSAL:
        // check that the max indegree is not already reached
        if ( _graph.exists ( change.node1 () ) &&
             _max_parents[change.node1 ()] ==
             _graph.parents ( change.node1 () ).size () ) {
          GUM_ERROR ( OutOfUpperBound, "there are already too many parents" );
        }
        break;

      default:
        GUM_ERROR ( OperationNotAllowed, "edge modifications are not "
                    "supported by indegree constraints" );
      }
    }

    
    /// adds a new arc into the graph
    INLINE void
    StructuralConstraintIndegree::modifyGraph ( const GraphChange& change ) {
      StructuralConstraintIndegree::_modifyGraph ( change );
      StructuralConstraintDiGraph::modifyGraph ( change );
    }

    
  } /* namespace learning */

  
} /* namespace gum */



#endif /* DOXYGEN_SHOULD_SKIP_THIS */
