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
 * @brief the structural constraint imposing a partial order over nodes
 *
 * In DBNs, it is forbidden to add arcs from nodes at time slice t to nodes at
 * time slice s, where s < t. This class allows for taking this kind of constraint
 * into account by imposing a partial order over the nodes: arcs (X,Y) can then
 * only be added if X <= Y in the partial order.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {

  
  namespace learning {


    /// sets a new graph from which we will perform checkings
    INLINE void StructuralConstraintPartialOrder::setGraphAlone
    ( const DiGraph& graph ) {
      // check that each node has an appropriate time slice
      for ( const auto id : graph ) {
        if ( ! _PartialOrder__order.exists ( id ) ) {
          _PartialOrder__order.insert ( id, _PartialOrder__default_slice );
        }
      }
    }

    
    /// checks whether the constraints enable to add arc (x,y)
    INLINE bool
    StructuralConstraintPartialOrder::checkArcAdditionAlone ( NodeId x, NodeId y )
      const noexcept {
      return _PartialOrder__order[x] <= _PartialOrder__order[y];
    }

    
    /// checks whether the constraints enable to remove arc (x,y)
    INLINE bool
    StructuralConstraintPartialOrder::checkArcDeletionAlone ( NodeId x, NodeId y )
      const noexcept {
      return true;
    }

    
    /// checks whether the constraints enable to reverse arc (x,y)
    INLINE bool
    StructuralConstraintPartialOrder::checkArcReversalAlone ( NodeId x, NodeId y )
      const noexcept {
      return _PartialOrder__order[x] == _PartialOrder__order[y];
    }
    
    
    /// notify the constraint of a modification of the graph
    INLINE void
    StructuralConstraintPartialOrder::modifyGraphAlone
    ( const ArcAddition& change ) {
    }

    
    /// notify the constraint of a modification of the graph
    INLINE void
    StructuralConstraintPartialOrder::modifyGraphAlone
    ( const ArcDeletion& change ) {
    }

    
    /// notify the constraint of a modification of the graph
    INLINE void
    StructuralConstraintPartialOrder::modifyGraphAlone
    ( const ArcReversal& change ) {
    }

    
    /// notify the constraint of a modification of the graph
    INLINE void
    StructuralConstraintPartialOrder::modifyGraphAlone
    ( const GraphChange& change ) {
    }

    
    /// indicates whether a change will always violate the constraint
    INLINE bool
    StructuralConstraintPartialOrder::isAlwaysInvalidAlone
    ( const GraphChange& change ) const noexcept {
      switch ( change.type () ) {
      case GraphChangeType::ARC_ADDITION:
        return ( _PartialOrder__order[change.node1 ()] >
                 _PartialOrder__order[change.node2 ()] );
        
      case GraphChangeType::ARC_DELETION:
        return false;

      case GraphChangeType::ARC_REVERSAL:
        return ( _PartialOrder__order[change.node1 ()] !=
                 _PartialOrder__order[change.node2 ()] );

      default:
        GUM_ERROR ( OperationNotAllowed, "edge modifications are not "
                    "supported by PartialOrder constraints" );
      }
    }


    /// checks whether the constraints enable to add an arc
    INLINE bool
    StructuralConstraintPartialOrder::checkModificationAlone
    ( const ArcAddition& change ) const noexcept {
      return checkArcAdditionAlone ( change.node1 (), change.node2 () );
    }


    /// checks whether the constraints enable to remove an arc
    INLINE bool
    StructuralConstraintPartialOrder::checkModificationAlone
    ( const ArcDeletion& change ) const noexcept {
      return checkArcDeletionAlone ( change.node1 (), change.node2 () );
    }


    /// checks whether the constraints enable to reverse an arc
    INLINE bool
    StructuralConstraintPartialOrder::checkModificationAlone
    ( const ArcReversal& change ) const noexcept {
      return checkArcReversalAlone ( change.node1 (), change.node2 () );
    }

      
    /// checks whether the constraints enable to perform a graph change
    INLINE bool
    StructuralConstraintPartialOrder::checkModificationAlone
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
                    "supported by StructuralConstraintPartialOrder" );
      }
    }
   

    /// sets the time slices of all the nodes in the property 
    INLINE void StructuralConstraintPartialOrder::setPartialOrder
    ( const NodeProperty<unsigned int>& order ) {
      for ( const auto& slice : order ) {
        _PartialOrder__order.set ( slice.first, slice.second );
      }
    }

    
    /// sets the default time slice
    INLINE void StructuralConstraintPartialOrder::setDefaultSlice
    ( unsigned int slice,
      bool update_all ) {
      if ( update_all ) {
        for ( auto& node : _PartialOrder__order ) {
          node.second = slice;
        }
      }

      _PartialOrder__default_slice = slice;
    }


    // include all the methods applicable to the whole class hierarchy
    #define GUM_CONSTRAINT_CLASS_NAME StructuralConstraintPartialOrder
    #include <agrum/learning/constraints/structuralConstraintPatternInline.h>
    #undef GUM_CONSTRAINT_CLASS_NAME

      
  } /* namespace learning */

  
} /* namespace gum */



#endif /* DOXYGEN_SHOULD_SKIP_THIS */
