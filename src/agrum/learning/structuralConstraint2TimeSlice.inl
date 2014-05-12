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
    ( const DiGraph& graph,
      const NodeProperty<bool>& time_slice ) {
      // check that each node has an appropriate time slice
      if ( time_slice.size () != graph.size () ) {
        GUM_ERROR ( SizeError,
                    "the graph and the property do not have the same size" );
      }
      for ( const auto id : graph ) {
        if ( ! time_slice.exists ( id ) ) {
          GUM_ERROR ( InvalidNode,
                      "there exists a node in the graph without time slice" );
        }
      }
      _time_slice = time_slice;
    }

    
    /// sets a new graph from which we will perform checkings
    INLINE void StructuralConstraint2TimeSlice::setGraph
    ( const DiGraph& graph,
      const NodeProperty<bool>& time_slice ) {
      StructuralConstraint2TimeSlice::_setGraph ( graph, time_slice );
      StructuralConstraintDiGraph::setGraph ( graph );
    }

    
    /// sets a new empty graph from which we will perform checkings
    INLINE void StructuralConstraint2TimeSlice::_setGraph
    ( const NodeProperty<bool>& time_slice ) {
      _time_slice = time_slice;
    }
    

    /// sets a new empty graph from which we will perform checkings
    INLINE void StructuralConstraint2TimeSlice::setGraph
    ( const NodeProperty<bool>& time_slice ) {
      StructuralConstraint2TimeSlice::_setGraph ( time_slice );
      for ( auto iter = time_slice.cbegin(); iter != time_slice.cend (); ++iter ) {
        _graph.insertNode ( iter.key () );
      }
    }


    /// adds a new arc into the graph
    INLINE void StructuralConstraint2TimeSlice::_insertArc ( NodeId x, NodeId y ) {
      if ( _time_slice[x] > _time_slice[y] ) {
        GUM_ERROR ( InvalidArc, "an backward-time arc cannot be added" );
      }
    }

    
    /// adds a new arc into the graph
    INLINE void StructuralConstraint2TimeSlice::insertArc ( NodeId x, NodeId y ) {
      StructuralConstraint2TimeSlice::_insertArc ( x, y );
      StructuralConstraintDiGraph::insertArc ( x, y );
    }

    
    /// removes an arc from the graph
    INLINE void StructuralConstraint2TimeSlice::_eraseArc ( NodeId x, NodeId y ) {
    }

    
    /// removes an arc from the graph
    INLINE void StructuralConstraint2TimeSlice::eraseArc ( NodeId x, NodeId y ) {
      StructuralConstraint2TimeSlice::_eraseArc ( x, y );
      StructuralConstraintDiGraph::eraseArc ( x, y );
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
    
    
  } /* namespace learning */

  
} /* namespace gum */



#endif /* DOXYGEN_SHOULD_SKIP_THIS */
