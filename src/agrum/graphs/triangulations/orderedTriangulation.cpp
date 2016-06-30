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
 * @brief class for graph triangulations for which we enforce a given complete
 * ordering on the nodes eliminations.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/config.h>

#include <agrum/graphs/triangulations/orderedTriangulation.h>


namespace gum {

  
  /// default constructor
  OrderedTriangulation::OrderedTriangulation
  ( const OrderedEliminationSequenceStrategy& elimSeq,
    const JunctionTreeStrategy& JTStrategy,
    bool minimality ) :
    StaticTriangulation( elimSeq, JTStrategy, minimality ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( OrderedTriangulation );
  }

  
  /// constructor with a given graph
  OrderedTriangulation::OrderedTriangulation
  ( const UndiGraph& theGraph,
    const NodeProperty<Size>& dom,
    const Sequence<NodeId>& sequence,
    const OrderedEliminationSequenceStrategy& elimSeq,
    const JunctionTreeStrategy& JTStrategy,
    bool minimality ) :
    StaticTriangulation( theGraph, dom, elimSeq, JTStrategy, minimality ),
    __sequence( &sequence ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( OrderedTriangulation );

    // check whether the sequence is appropriate for performing a
    // triangulation or not
    __sequence_needed = __isSequenceNeeded ( __sequence );
  }


  // copy constructor
  OrderedTriangulation::OrderedTriangulation ( const OrderedTriangulation& from ):
    StaticTriangulation ( from ),
    __sequence ( from.__sequence ),
    __sequence_needed ( from.__sequence_needed ) {
    // for debugging purposes
    GUM_CONS_CPY ( OrderedTriangulation );
  }
  

  /// destructor
  OrderedTriangulation::~OrderedTriangulation () {
    // for debugging purposes
    GUM_DESTRUCTOR( OrderedTriangulation );
  }

  
  /// initialize the triangulation data structures for a new graph
  void OrderedTriangulation::setGraph( const UndiGraph& graph,
                                       const NodeProperty<Size>& domsizes ) {
    StaticTriangulation::setGraph( graph, domsizes );

    // check that the elimination sequence is coherent with the new graph.
    // If this is not the case, indicate that the user will have to insert
    // a new sequence before performing a triangulation
    __sequence_needed = __isSequenceNeeded ( __sequence );
  }


  /// indicate whether an elimination sequence is compatible with the current graph
  bool
  OrderedTriangulation::__isSequenceNeeded ( const Sequence<NodeId>* seq ) const {
    if ( seq == nullptr ) return true;

    for ( const auto node : *seq ) {
      if ( ! _domain_sizes.exists ( node ) ) return true;
    }

    return ( seq->size () != _domain_sizes.size () );
  }


  /// sets the sequence of elimination
  void OrderedTriangulation::setEliminationSequence
  ( const Sequence<NodeId>& sequence ) {
    __sequence = &sequence;
    __sequence_needed = __isSequenceNeeded ( __sequence );
  }

  
  /// virtual copy constructor
  OrderedTriangulation* OrderedTriangulation::newFactory() const {
    return new OrderedTriangulation ( *this );
  }

  
  /// the function called to initialize the triangulation process
  void OrderedTriangulation::_initTriangulation( UndiGraph& graph ) {
    if ( __sequence_needed ) {
      GUM_ERROR ( UndefinedElement, "A correct elimination order has not been "
                  "provided to the OrderedTriangulation." );
    }
    
    OrderedEliminationSequenceStrategy* elim =
        static_cast<OrderedEliminationSequenceStrategy*>(
            _elimination_sequence_strategy );
    elim->setGraph( &graph, __sequence );
  }
  

} /* namespace gum */
