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
    INLINE void StructuralConstraintIndegree::setGraphAlone
    ( const DiGraph& graph ) {
      // check that the max_indegree corresponds to the graph
      for ( const auto id : graph ) {
        if ( ! _Indegree__max_parents.exists ( id ) ) {
          _Indegree__max_parents.insert ( id, _Indegree__max_indegree );
        }
      }
    }
    

    /// checks whether the constraints enable to add arc (x,y)
    INLINE bool StructuralConstraintIndegree::checkArcAdditionAlone
    ( NodeId x, NodeId y ) const noexcept {
      return ( _Indegree__max_parents[y] > _DiGraph__graph.parents (y).size () );
    }

   
    /// checks whether the constraints enable to remove arc (x,y)
    INLINE bool StructuralConstraintIndegree::checkArcDeletionAlone
    ( NodeId x, NodeId y ) const noexcept {
      return true;
    }

        
    /// checks whether the constraints enable to reverse arc (x,y)
    INLINE bool StructuralConstraintIndegree::checkArcReversalAlone
    ( NodeId x, NodeId y ) const noexcept {   
      return ( _Indegree__max_parents[x] > _DiGraph__graph.parents (x).size () );
    }


    /// checks whether the constraints enable to add an arc
    INLINE bool
    StructuralConstraintIndegree::checkModificationAlone
    ( const ArcAddition& change )
      const noexcept {
      return checkArcAdditionAlone ( change.node1 (), change.node2 () );
    }


    /// checks whether the constraints enable to remove an arc
    INLINE bool
    StructuralConstraintIndegree::checkModificationAlone
    ( const ArcDeletion& change )
      const noexcept {
      return checkArcDeletionAlone ( change.node1 (), change.node2 () );
    }


    /// checks whether the constraints enable to reverse an arc
    INLINE bool
    StructuralConstraintIndegree::checkModificationAlone
    ( const ArcReversal& change )
      const noexcept {
      return checkArcReversalAlone ( change.node1 (), change.node2 () );
    }

      
    /// checks whether the constraints enable to perform a graph change
    INLINE bool
    StructuralConstraintIndegree::checkModificationAlone
    ( const GraphChange& change )
      const noexcept {
      switch ( change.type () ) {
      case GraphChangeType::ARC_ADDITION:
        return checkArcAdditionAlone ( change.node1 (), change.node2 () );

      case GraphChangeType::ARC_DELETION:
        return checkArcDeletionAlone ( change.node1 (), change.node2 () );
        
      case GraphChangeType::ARC_REVERSAL:
        return checkArcReversalAlone ( change.node1 (), change.node2 () );
        
      default:
        GUM_ERROR ( OperationNotAllowed, "edge modifications are not "
                    "supported by StructuralConstraintIndegree" );
      }
    }
   
    
    /// notify the constraint of a modification of the graph
    INLINE void
    StructuralConstraintIndegree::modifyGraphAlone ( const ArcAddition& change ) {
    }


    /// notify the constraint of a modification of the graph
    INLINE void
    StructuralConstraintIndegree::modifyGraphAlone ( const ArcDeletion& change ) {
    }


    /// notify the constraint of a modification of the graph
    INLINE void
    StructuralConstraintIndegree::modifyGraphAlone ( const ArcReversal& change ) {
    }
    

    /// notify the constraint of a modification of the graph
    INLINE void
    StructuralConstraintIndegree::modifyGraphAlone ( const GraphChange& change ) {
    }

    
    /// indicates whether a change will always violate the constraint
    INLINE bool
    StructuralConstraintIndegree::isAlwaysInvalidAlone ( const GraphChange& )
      const noexcept {
      return false;
    }




    
    /// sets a new graph from which we will perform checkings
    INLINE void StructuralConstraintIndegree::setGraph
    ( const DiGraph& graph ) {
      constraints::setGraph ( graph );
      setGraphAlone ( graph );
    }


    /// sets the indegree for a given set of nodes
    INLINE void StructuralConstraintIndegree::setIndegree
    ( const NodeProperty<unsigned int>& max_indegree ) {
      for ( const auto& degree : max_indegree ) {
        _Indegree__max_parents.set ( degree.first, degree.second );
      }
    }


    /// resets the max indegree
    INLINE void StructuralConstraintIndegree::setDefaultIndegree
    ( unsigned int max_indegree,
      bool update_all ) {
      if ( update_all ) {
        for ( auto& degree : _Indegree__max_parents ) {
          degree.second = max_indegree;
        }
      }
      
      _Indegree__max_indegree = max_indegree;
    }
    

    /// checks whether the constraints enable to add arc (x,y)
    INLINE bool StructuralConstraintIndegree::checkArcAddition
    ( NodeId x, NodeId y ) const noexcept {
      return constraints::checkArcAddition ( x, y ) &&
        checkArcAdditionAlone ( x, y );
    }

    
    /// checks whether the constraints enable to remove arc (x,y)
    INLINE bool StructuralConstraintIndegree::checkArcDeletion
    ( NodeId x, NodeId y ) const noexcept {
      return  constraints::checkArcDeletion ( x, y ) &&
        checkArcDeletionAlone ( x, y );
    }

    
    /// checks whether the constraints enable to reverse arc (x,y)
    INLINE bool StructuralConstraintIndegree::checkArcReversal
    ( NodeId x, NodeId y ) const noexcept {   
      return constraints::checkArcReversal ( x, y ) &&
        checkArcReversalAlone ( x,y );
    }


    /// adds a new arc into the graph
    INLINE void
    StructuralConstraintIndegree::modifyGraph ( const ArcAddition& change ) {
      constraints::modifyGraph ( change );
      modifyGraphAlone ( change );
    }


    /// adds a new arc into the graph
    INLINE void
    StructuralConstraintIndegree::modifyGraph ( const ArcDeletion& change ) {
      constraints::modifyGraph ( change );
      modifyGraphAlone ( change );
    }

    
    /// adds a new arc into the graph
    INLINE void
    StructuralConstraintIndegree::modifyGraph ( const ArcReversal& change ) {
      constraints::modifyGraph ( change );
      modifyGraphAlone ( change );
    }

    
    /// adds a new arc into the graph
    INLINE void
    StructuralConstraintIndegree::modifyGraph ( const GraphChange& change ) {
      constraints::modifyGraph ( change );
      modifyGraphAlone ( change );
    }

    
    /// indicates whether a change will always violate the constraint
    INLINE bool
    StructuralConstraintIndegree::isAlwaysInvalid ( const GraphChange& )
      const noexcept {
      return false;
    }


    /// checks whether the constraints enable to add an arc
    INLINE bool
    StructuralConstraintIndegree::checkModification ( const ArcAddition& change )
      const noexcept {
      return checkArcAddition ( change.node1 (), change.node2 () );
    }
    

    /// checks whether the constraints enable to remove an arc
    INLINE bool
    StructuralConstraintIndegree::checkModification ( const ArcDeletion& change )
      const noexcept {
      return checkArcDeletion ( change.node1 (), change.node2 () );
    }

    
    /// checks whether the constraints enable to reverse an arc
    INLINE bool
    StructuralConstraintIndegree::checkModification ( const ArcReversal& change )
      const noexcept {
      return checkArcReversal ( change.node1 (), change.node2 () );
    }

    
    /// checks whether the constraints enable to perform a graph change
    INLINE bool
    StructuralConstraintIndegree::checkModification ( const GraphChange& change )
      const noexcept {
      switch ( change.type () ) {
      case GraphChangeType::ARC_ADDITION:
        return checkArcAddition ( change.node1 (), change.node2 () );
        
      case GraphChangeType::ARC_DELETION:
        return checkArcDeletion ( change.node1 (), change.node2 () );
        
      case GraphChangeType::ARC_REVERSAL:
        return checkArcReversal ( change.node1 (), change.node2 () );
        
      default:
        GUM_ERROR ( OperationNotAllowed, "edge modifications are not "
                    "supported by StructuralConstraintIndegree" );
      }
    }
    

  } /* namespace learning */

  
} /* namespace gum */



#endif /* DOXYGEN_SHOULD_SKIP_THIS */
