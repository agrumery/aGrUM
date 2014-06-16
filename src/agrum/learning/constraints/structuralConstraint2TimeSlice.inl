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
    INLINE void StructuralConstraint2TimeSlice::setGraphAlone
    ( const DiGraph& graph ) {
      // check that each node has an appropriate time slice
      for ( const auto id : graph ) {
        if ( ! _2TimeSlice__time_slice.exists ( id ) ) {
          _2TimeSlice__time_slice.insert ( id, _2TimeSlice__default_slice );
        }
      }
    }

    
    /// checks whether the constraints enable to add arc (x,y)
    INLINE bool
    StructuralConstraint2TimeSlice::checkArcAdditionAlone ( NodeId x, NodeId y )
      const noexcept {
      return _2TimeSlice__time_slice[x] <= _2TimeSlice__time_slice[y];
    }

    
    /// checks whether the constraints enable to remove arc (x,y)
    INLINE bool
    StructuralConstraint2TimeSlice::checkArcDeletionAlone ( NodeId x, NodeId y )
      const noexcept {
      return true;
    }

    
    /// checks whether the constraints enable to reverse arc (x,y)
    INLINE bool
    StructuralConstraint2TimeSlice::checkArcReversalAlone ( NodeId x, NodeId y )
      const noexcept {
      return _2TimeSlice__time_slice[x] == _2TimeSlice__time_slice[y];
    }
    
    
    /// notify the constraint of a modification of the graph
    INLINE void
    StructuralConstraint2TimeSlice::modifyGraphAlone
    ( const ArcAddition& change ) {
    }

    
    /// notify the constraint of a modification of the graph
    INLINE void
    StructuralConstraint2TimeSlice::modifyGraphAlone
    ( const ArcDeletion& change ) {
    }

    
    /// notify the constraint of a modification of the graph
    INLINE void
    StructuralConstraint2TimeSlice::modifyGraphAlone
    ( const ArcReversal& change ) {
    }

    
    /// notify the constraint of a modification of the graph
    INLINE void
    StructuralConstraint2TimeSlice::modifyGraphAlone
    ( const GraphChange& change ) {
    }

    
    /// indicates whether a change will always violate the constraint
    INLINE bool
    StructuralConstraint2TimeSlice::isAlwaysInvalidAlone
    ( const GraphChange& change ) const noexcept {
      switch ( change.type () ) {
      case GraphChangeType::ARC_ADDITION:
        return ( _2TimeSlice__time_slice[change.node1 ()] >
                 _2TimeSlice__time_slice[change.node2 ()] );
        
      case GraphChangeType::ARC_DELETION:
        return false;

      case GraphChangeType::ARC_REVERSAL:
        return ( _2TimeSlice__time_slice[change.node1 ()] !=
                 _2TimeSlice__time_slice[change.node2 ()] );

      default:
        GUM_ERROR ( OperationNotAllowed, "edge modifications are not "
                    "supported by 2TimeSlice constraints" );
      }
    }


    /// checks whether the constraints enable to add an arc
    INLINE bool
    StructuralConstraint2TimeSlice::checkModificationAlone
    ( const ArcAddition& change ) const noexcept {
      return checkArcAdditionAlone ( change.node1 (), change.node2 () );
    }


    /// checks whether the constraints enable to remove an arc
    INLINE bool
    StructuralConstraint2TimeSlice::checkModificationAlone
    ( const ArcDeletion& change ) const noexcept {
      return checkArcDeletionAlone ( change.node1 (), change.node2 () );
    }


    /// checks whether the constraints enable to reverse an arc
    INLINE bool
    StructuralConstraint2TimeSlice::checkModificationAlone
    ( const ArcReversal& change ) const noexcept {
      return checkArcReversalAlone ( change.node1 (), change.node2 () );
    }

      
    /// checks whether the constraints enable to perform a graph change
    INLINE bool
    StructuralConstraint2TimeSlice::checkModificationAlone
    ( const GraphChange& change ) const noexcept {
      switch ( change.type () ) {
      case GraphChangeType::ARC_ADDITION:
        return checkArcAdditionAlone ( change.node1 (), change.node2 () );

      case GraphChangeType::ARC_DELETION:
        return checkArcDeletionAlone ( change.node1 (), change.node2 () );
        
      case GraphChangeType::ARC_REVERSAL:
        return checkArcReversalAlone ( change.node1 (), change.node2 () );
        
      default:
        GUM_ERROR ( OperationNotAllowed, "edge modifications are not "
                    "supported by StructuralConstraint2TimeSlice" );
      }
    }
   

    /// sets the time slices of all the nodes in the property 
    INLINE void StructuralConstraint2TimeSlice::setSlices
    ( const NodeProperty<bool>& time_slice ) {
      for ( const auto& slice : time_slice ) {
        _2TimeSlice__time_slice.set ( slice.first, slice.second );
      }
    }

    
    /// sets the default time slice
    INLINE void StructuralConstraint2TimeSlice::setDefaultSlice
    ( bool time_slice,
      bool update_all ) {
      if ( update_all ) {
        for ( auto& slice : _2TimeSlice__time_slice ) {
          slice.second = time_slice;
        }
      }

      _2TimeSlice__default_slice = time_slice;
    }


    // include all the methods applicable to the whole class hierarchy
    #define GUM_CONSTRAINT_CLASS_NAME StructuralConstraint2TimeSlice
    #include <agrum/learning/constraints/structuralConstraintPatternInline.h>
    #undef GUM_CONSTRAINT_CLASS_NAME

      
  } /* namespace learning */

  
} /* namespace gum */



#endif /* DOXYGEN_SHOULD_SKIP_THIS */
