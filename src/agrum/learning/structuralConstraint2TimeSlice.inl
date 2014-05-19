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

#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {

  
  namespace learning {



    /// sets a new graph from which we will perform checkings
    INLINE void StructuralConstraint2TimeSlice::_setGraph
    ( const DiGraph& graph ) {
      // check that each node has an appropriate time slice
      for ( const auto id : _graph ) {
        if ( ! _time_slice.exists ( id ) ) {
          GUM_ERROR ( InvalidNode,
                      "there exists a node in the graph without time slice" );
        }
      }
    }

    
    /// sets a new graph from which we will perform checkings
    INLINE void
    StructuralConstraint2TimeSlice::setGraph ( const DiGraph& graph ) {
      StructuralConstraint2TimeSlice::_setGraph ( graph );
      StructuralConstraintDiGraph::setGraph ( graph );
    }

    
    /// sets a new empty graph from which we will perform checkings
    INLINE void StructuralConstraint2TimeSlice::setSlices
    ( const NodeProperty<bool>& time_slice ) {
      for ( const auto id : _graph ) {
        if ( ! time_slice.exists ( id ) ) {
          GUM_ERROR ( InvalidNode,
                      "there exists a node in the graph without time slice" );
        }
      }
      _time_slice = time_slice;
    }
    

    /// checks whether the constraints enable to add arc (x,y)
    INLINE bool StructuralConstraint2TimeSlice::_checkArcAddition
    ( NodeId x, NodeId y ) {
      return _time_slice[x] <= _time_slice[y];
    }

    
    /// checks whether the constraints enable to add arc (x,y)
    INLINE bool StructuralConstraint2TimeSlice::checkArcAddition
    ( NodeId x, NodeId y ) {
      return StructuralConstraintDiGraph::checkArcAddition ( x, y ) &&
        StructuralConstraint2TimeSlice::_checkArcAddition ( x, y );
    }

    
    /// checks whether the constraints enable to remove arc (x,y)
    INLINE bool StructuralConstraint2TimeSlice::_checkArcDeletion
    ( NodeId x, NodeId y ) {
      return true;
    }


    /// checks whether the constraints enable to remove arc (x,y)
    INLINE bool StructuralConstraint2TimeSlice::checkArcDeletion
    ( NodeId x, NodeId y ) {
      return StructuralConstraintDiGraph::checkArcDeletion ( x, y ) &&
        StructuralConstraint2TimeSlice::_checkArcDeletion ( x, y );
    }

    
    /// checks whether the constraints enable to reverse arc (x,y)
    INLINE bool StructuralConstraint2TimeSlice::_checkArcReversal
    ( NodeId x, NodeId y ) {
      return _time_slice[x] == _time_slice[y];
    }
    
      
    /// checks whether the constraints enable to reverse arc (x,y)
    INLINE bool StructuralConstraint2TimeSlice::checkArcReversal
    ( NodeId x, NodeId y ) {
      return StructuralConstraintDiGraph::checkArcReversal ( x, y ) &&
        StructuralConstraint2TimeSlice::_checkArcReversal ( x, y );
    }

    
        /// adds a new arc into the graph
    INLINE void
    StructuralConstraint2TimeSlice::_modifyGraph ( const GraphChange& change ) {
      switch ( change.type () ) {
      case GraphChangeType::ARC_ADDITION:
        if ( _time_slice[change.node1 ()] > _time_slice[change.node2 ()] ) {
          GUM_ERROR ( InvalidArc, "an backward-time arc cannot be added" );
        }
        break;

      case GraphChangeType::ARC_DELETION:
        break;

      case GraphChangeType::ARC_REVERSAL:
        if ( _time_slice[change.node1 ()] != _time_slice[change.node2 ()] ) {
          GUM_ERROR ( InvalidArc, "an backward-time arc cannot be added" );
        }
        break;

      default:
        GUM_ERROR ( OperationNotAllowed, "edge modifications are not "
                    "supported by 2TimeSlice constraints" );
      }
    }

    
    /// adds a new arc into the graph
    INLINE void
    StructuralConstraint2TimeSlice::modifyGraph ( const GraphChange& change ) {
      StructuralConstraint2TimeSlice::_modifyGraph ( change );
      StructuralConstraintDiGraph::modifyGraph ( change );
    }

    
    /// indicates whether a change will always violate the constraint
    INLINE bool
    StructuralConstraint2TimeSlice::isAlwaysInvalid ( const GraphChange& change ) {
      switch ( change.type () ) {
      case GraphChangeType::ARC_ADDITION:
        return ( _time_slice[change.node1 ()] > _time_slice[change.node2 ()] );
        
      case GraphChangeType::ARC_DELETION:
        return false;

      case GraphChangeType::ARC_REVERSAL:
        return ( _time_slice[change.node1 ()] != _time_slice[change.node2 ()] );

      default:
        GUM_ERROR ( OperationNotAllowed, "edge modifications are not "
                    "supported by 2TimeSlice constraints" );
      }
    }


    /// checks whether the constraints enable to add an arc
    INLINE bool
    StructuralConstraint2TimeSlice::_checkModification ( ArcAddition& change ) {
      return StructuralConstraint2TimeSlice::_checkArcAddition
        ( change.node1 (), change.node2 () );
    }


    /// checks whether the constraints enable to remove an arc
    INLINE bool
    StructuralConstraint2TimeSlice::_checkModification ( ArcDeletion& change ) {
      return StructuralConstraint2TimeSlice::_checkArcDeletion
         ( change.node1 (), change.node2 () );
    }


    /// checks whether the constraints enable to reverse an arc
    INLINE bool
    StructuralConstraint2TimeSlice::_checkModification ( ArcReversal& change ) {
      return StructuralConstraint2TimeSlice::_checkArcReversal
        ( change.node1 (), change.node2 () );
    }

      
    /// checks whether the constraints enable to perform a graph change
    INLINE bool
    StructuralConstraint2TimeSlice::_checkModification ( GraphChange& change ) {
      switch ( change.type () ) {
      case GraphChangeType::ARC_ADDITION:
        return StructuralConstraint2TimeSlice::_checkArcAddition
          ( change.node1 (), change.node2 () );

      case GraphChangeType::ARC_DELETION:
        return StructuralConstraint2TimeSlice::_checkArcDeletion
          ( change.node1 (), change.node2 () );
        
      case GraphChangeType::ARC_REVERSAL:
        return StructuralConstraint2TimeSlice::_checkArcReversal
          ( change.node1 (), change.node2 () );
        
      default:
        GUM_ERROR ( OperationNotAllowed, "edge modifications are not "
                    "supported by StructuralConstraint2TimeSlice" );
      }
    }
   

    /// checks whether the constraints enable to add an arc
    INLINE bool
    StructuralConstraint2TimeSlice::checkModification ( ArcAddition& change ) {
      return StructuralConstraint2TimeSlice::checkArcAddition
        ( change.node1 (), change.node2 () );
    }
    

    /// checks whether the constraints enable to remove an arc
    INLINE bool
    StructuralConstraint2TimeSlice::checkModification ( ArcDeletion& change ) {
      return StructuralConstraint2TimeSlice::checkArcDeletion
        ( change.node1 (), change.node2 () );
    }

    
    /// checks whether the constraints enable to reverse an arc
    INLINE bool
    StructuralConstraint2TimeSlice::checkModification ( ArcReversal& change ) {
      return StructuralConstraint2TimeSlice::checkArcReversal
        ( change.node1 (), change.node2 () );
    }

    
    /// checks whether the constraints enable to perform a graph change
    INLINE bool
    StructuralConstraint2TimeSlice::checkModification ( GraphChange& change ) {
      switch ( change.type () ) {
      case GraphChangeType::ARC_ADDITION:
        return StructuralConstraint2TimeSlice::checkArcAddition
          ( change.node1 (), change.node2 () );
        
      case GraphChangeType::ARC_DELETION:
        return StructuralConstraint2TimeSlice::checkArcDeletion
          ( change.node1 (), change.node2 () );
        
      case GraphChangeType::ARC_REVERSAL:
        return StructuralConstraint2TimeSlice::checkArcReversal
          ( change.node1 (), change.node2 () );
        
      default:
        GUM_ERROR ( OperationNotAllowed, "edge modifications are not "
                    "supported by StructuralConstraint2TimeSlice" );
      }
    }
    
    
  } /* namespace learning */

  
} /* namespace gum */



#endif /* DOXYGEN_SHOULD_SKIP_THIS */
