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
 * @brief the base class for structural constraints used by learning algorithms
 * that learn a directed graph structure
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {

  
  namespace learning {


    /// sets a new graph from which we will perform checkings
    INLINE void StructuralConstraintDiGraph::setGraph ( const DiGraph& graph ) {
      _graph = graph;
    }
    

    /// sets a new graph from which we will perform checkings
    INLINE void StructuralConstraintDiGraph::setGraph ( unsigned int nb_nodes ) {
      _graph.clear ();
      for ( unsigned int i = 0; i < nb_nodes; ++i ) {
        _graph.insertNode ( i );
      }
    }


    /// adds a new arc into the graph (without checking the constraints)
    INLINE void
    StructuralConstraintDiGraph::modifyGraph (  const GraphChange& change ) {
      switch ( change.type () ) {
      case GraphChangeType::ARC_ADDITION:
        _graph.insertArc ( change.node1(), change.node2 () );
        break;

      case GraphChangeType::ARC_DELETION:
        _graph.eraseArc ( Arc ( change.node1(), change.node2 () ) );
        break;

      case GraphChangeType::ARC_REVERSAL:
        _graph.eraseArc ( Arc ( change.node1(), change.node2 () ) );
        _graph.insertArc ( change.node2(), change.node1 () );
        break;

      default:
        GUM_ERROR ( OperationNotAllowed,
                    "edge modifications are not supported by digraph constraint" );
      }
    }
      
    
    /// checks whether the constraints enable to add arc (x,y)
    INLINE bool
    StructuralConstraintDiGraph::checkArcAddition ( NodeId x, NodeId y ) {
      return _graph.existsNode ( x ) && _graph.existsNode ( y ) &&
        ! _graph.existsArc ( x, y );
    }


    /// checks whether the constraints enable to remove arc (x,y)
    INLINE bool
    StructuralConstraintDiGraph::checkArcDeletion ( NodeId x, NodeId y ) {
      return _graph.existsArc ( x, y );
    }

    
    /// checks whether the constraints enable to reverse arc (x,y)
    INLINE bool
    StructuralConstraintDiGraph::checkArcReversal ( NodeId x, NodeId y ) {
      return _graph.existsArc ( x, y ) && ! _graph.existsArc ( y, x );
    }

    
    /// checks whether the constraints enable to add an arc
    INLINE bool
    StructuralConstraintDiGraph::checkModification ( ArcAddition& change ) {
      return StructuralConstraintDiGraph::checkArcAddition
        ( change.node1 (), change.node2 () );
    }

    
    /// checks whether the constraints enable to remove an arc
    INLINE bool
    StructuralConstraintDiGraph::checkModification ( ArcDeletion& change ) {
       return StructuralConstraintDiGraph::checkArcDeletion
         ( change.node1 (), change.node2 () );
    }


    /// checks whether the constraints enable to reverse an arc
    INLINE bool
    StructuralConstraintDiGraph::checkModification ( ArcReversal& change ) {
      return StructuralConstraintDiGraph::checkArcReversal
         ( change.node1 (), change.node2 () );
    }


    /// checks whether the constraints enable to perform a graph change
    INLINE bool
    StructuralConstraintDiGraph::checkModification ( GraphChange& change ) {
      switch ( change.type () ) {
      case GraphChangeType::ARC_ADDITION:
        return StructuralConstraintDiGraph::checkArcAddition
          ( change.node1 (), change.node2 () );

      case GraphChangeType::ARC_DELETION:
        return StructuralConstraintDiGraph::checkArcDeletion
          ( change.node1 (), change.node2 () );
        
      case GraphChangeType::ARC_REVERSAL:
        return StructuralConstraintDiGraph::checkArcReversal
          ( change.node1 (), change.node2 () );
        
      default:
        GUM_ERROR ( OperationNotAllowed, "edge modifications are not "
                    "supported by StructuralConstraintDiGraph" );
      }
    }

    
    /// indicates whether a change will always violate the constraint
    INLINE bool
    StructuralConstraintDiGraph::isAlwaysInvalid ( const GraphChange& ) {
      return false;
    }


  } /* namespace learning */

  
} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
