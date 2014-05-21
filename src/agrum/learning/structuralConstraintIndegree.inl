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
    ( const DiGraph& graph ) {
      // check that the max_indegree corresponds to the graph
      for ( const auto id : graph ) {
        if ( ! _max_parents.exists ( id ) ) {
          GUM_ERROR ( InvalidNode,
                      "there exists a node in the graph without max indegree" );
        }
      }
    }
    
    
    /// sets a new graph from which we will perform checkings
    INLINE void StructuralConstraintIndegree::setGraph
    ( const DiGraph& graph ) {
      StructuralConstraintIndegree::_setGraph ( graph );
      StructuralConstraintDiGraph::setGraph ( graph );
    }


    /// sets the indegree for each node
    INLINE void StructuralConstraintIndegree::setIndegree
    ( const NodeProperty<unsigned int>& max_indegree ) {
      for ( const auto id : _graph ) {
        if ( ! _max_parents.exists ( id ) ) {
          GUM_ERROR ( InvalidNode,
                      "there exists a node in the graph without max indegree" );
        }
      }

      _max_parents = max_indegree;
    }


    /// resets the max indegree
    INLINE void StructuralConstraintIndegree::setIndegree
    ( unsigned int max_indegree ) {
      _max_parents.clear ();
      for ( const auto id : _graph ) {
        _max_parents.insert ( id, max_indegree );
      }
    }
    

    /// checks whether the constraints enable to add arc (x,y)
    INLINE bool StructuralConstraintIndegree::_checkArcAddition
    ( NodeId x, NodeId y ) const noexcept {
      return ( _max_parents[y] > _graph.parents (y).size () );
    }


    /// checks whether the constraints enable to add arc (x,y)
    INLINE bool StructuralConstraintIndegree::checkArcAddition
    ( NodeId x, NodeId y ) const noexcept {
      return StructuralConstraintDiGraph::checkArcAddition ( x,y ) &&
        StructuralConstraintIndegree::_checkArcAddition ( x,y );
    }

    
    /// checks whether the constraints enable to remove arc (x,y)
    INLINE bool StructuralConstraintIndegree::_checkArcDeletion
    ( NodeId x, NodeId y ) const noexcept {
      return true;
    }

    
    /// checks whether the constraints enable to remove arc (x,y)
    INLINE bool StructuralConstraintIndegree::checkArcDeletion
    ( NodeId x, NodeId y ) const noexcept {
      return  StructuralConstraintIndegree::_checkArcDeletion ( x,y ) &&
        StructuralConstraintDiGraph::checkArcDeletion ( x,y );
    }

    
    /// checks whether the constraints enable to reverse arc (x,y)
    INLINE bool StructuralConstraintIndegree::_checkArcReversal
    ( NodeId x, NodeId y ) const noexcept {   
      return ( _max_parents[x] > _graph.parents (x).size () );
    }

    
    /// checks whether the constraints enable to reverse arc (x,y)
    INLINE bool StructuralConstraintIndegree::checkArcReversal
    ( NodeId x, NodeId y ) const noexcept {   
      return StructuralConstraintDiGraph::checkArcReversal ( x,y ) &&
        StructuralConstraintIndegree::_checkArcReversal ( x,y );
    }
    

    /// notify the constraint of a modification of the graph
    INLINE void
    StructuralConstraintIndegree::_modifyGraph ( const ArcAddition& change ) {
      // check that the max indegree is not already reached
      if ( _graph.exists ( change.node2 () ) &&
           _max_parents[change.node2 ()] ==
           _graph.parents ( change.node2 () ).size () ) {
        GUM_ERROR ( OutOfUpperBound, "there are already too many parents" );
      }
    }


    /// notify the constraint of a modification of the graph
    INLINE void
    StructuralConstraintIndegree::_modifyGraph ( const ArcDeletion& change ) {
    }


    /// notify the constraint of a modification of the graph
    INLINE void
    StructuralConstraintIndegree::_modifyGraph ( const ArcReversal& change ) {
      // check that the max indegree is not already reached
      if ( _graph.exists ( change.node1 () ) &&
           _max_parents[change.node1 ()] ==
           _graph.parents ( change.node1 () ).size () ) {
        GUM_ERROR ( OutOfUpperBound, "there are already too many parents" );
      }
    }
    

    /// notify the constraint of a modification of the graph
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
    StructuralConstraintIndegree::modifyGraph ( const ArcAddition& change ) {
      StructuralConstraintIndegree::_modifyGraph ( change );
      StructuralConstraintDiGraph::modifyGraph ( change );
    }

    /// adds a new arc into the graph
    INLINE void
    StructuralConstraintIndegree::modifyGraph ( const ArcDeletion& change ) {
      StructuralConstraintIndegree::_modifyGraph ( change );
      StructuralConstraintDiGraph::modifyGraph ( change );
    }

    /// adds a new arc into the graph
    INLINE void
    StructuralConstraintIndegree::modifyGraph ( const ArcReversal& change ) {
      StructuralConstraintIndegree::_modifyGraph ( change );
      StructuralConstraintDiGraph::modifyGraph ( change );
    }

    
    /// adds a new arc into the graph
    INLINE void
    StructuralConstraintIndegree::modifyGraph ( const GraphChange& change ) {
      StructuralConstraintIndegree::_modifyGraph ( change );
      StructuralConstraintDiGraph::modifyGraph ( change );
    }

    
    /// indicates whether a change will always violate the constraint
    INLINE bool
    StructuralConstraintIndegree::isAlwaysInvalid ( const GraphChange& )
      const noexcept {
      return false;
    }


    /// checks whether the constraints enable to add an arc
    INLINE bool
    StructuralConstraintIndegree::_checkModification ( const ArcAddition& change )
      const noexcept {
      return StructuralConstraintIndegree::_checkArcAddition
        ( change.node1 (), change.node2 () );
    }


    /// checks whether the constraints enable to remove an arc
    INLINE bool
    StructuralConstraintIndegree::_checkModification ( const ArcDeletion& change )
      const noexcept {
      return StructuralConstraintIndegree::_checkArcDeletion
        ( change.node1 (), change.node2 () );
    }


    /// checks whether the constraints enable to reverse an arc
    INLINE bool
    StructuralConstraintIndegree::_checkModification ( const ArcReversal& change )
      const noexcept {
      return StructuralConstraintIndegree::_checkArcReversal
        ( change.node1 (), change.node2 () );
    }

      
    /// checks whether the constraints enable to perform a graph change
    INLINE bool
    StructuralConstraintIndegree::_checkModification ( const GraphChange& change )
      const noexcept {
      switch ( change.type () ) {
      case GraphChangeType::ARC_ADDITION:
        return StructuralConstraintIndegree::_checkArcAddition
          ( change.node1 (), change.node2 () );

      case GraphChangeType::ARC_DELETION:
        return StructuralConstraintIndegree::_checkArcDeletion
          ( change.node1 (), change.node2 () );
        
      case GraphChangeType::ARC_REVERSAL:
        return StructuralConstraintIndegree::_checkArcReversal
          ( change.node1 (), change.node2 () );
        
      default:
        GUM_ERROR ( OperationNotAllowed, "edge modifications are not "
                    "supported by StructuralConstraintIndegree" );
      }
    }
   

    /// checks whether the constraints enable to add an arc
    INLINE bool
    StructuralConstraintIndegree::checkModification ( const ArcAddition& change )
      const noexcept {
      return StructuralConstraintIndegree::checkArcAddition
        ( change.node1 (), change.node2 () );
    }
    

    /// checks whether the constraints enable to remove an arc
    INLINE bool
    StructuralConstraintIndegree::checkModification ( const ArcDeletion& change )
      const noexcept {
      return StructuralConstraintIndegree::checkArcDeletion
        ( change.node1 (), change.node2 () );
    }

    
    /// checks whether the constraints enable to reverse an arc
    INLINE bool
    StructuralConstraintIndegree::checkModification ( const ArcReversal& change )
      const noexcept {
      return StructuralConstraintIndegree::checkArcReversal
        ( change.node1 (), change.node2 () );
    }

    
    /// checks whether the constraints enable to perform a graph change
    INLINE bool
    StructuralConstraintIndegree::checkModification ( const GraphChange& change )
      const noexcept {
      switch ( change.type () ) {
      case GraphChangeType::ARC_ADDITION:
        return StructuralConstraintIndegree::checkArcAddition
          ( change.node1 (), change.node2 () );
        
      case GraphChangeType::ARC_DELETION:
        return StructuralConstraintIndegree::checkArcDeletion
          ( change.node1 (), change.node2 () );
        
      case GraphChangeType::ARC_REVERSAL:
        return StructuralConstraintIndegree::checkArcReversal
          ( change.node1 (), change.node2 () );
        
      default:
        GUM_ERROR ( OperationNotAllowed, "edge modifications are not "
                    "supported by StructuralConstraintIndegree" );
      }
    }
    

    
    
  } /* namespace learning */

  
} /* namespace gum */



#endif /* DOXYGEN_SHOULD_SKIP_THIS */
