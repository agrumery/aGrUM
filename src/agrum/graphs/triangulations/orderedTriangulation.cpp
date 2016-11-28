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
  OrderedTriangulation::OrderedTriangulation(
      const OrderedEliminationSequenceStrategy& elimSeq,
      const JunctionTreeStrategy&               JTStrategy,
      bool                                      minimality )
      : StaticTriangulation( elimSeq, JTStrategy, minimality ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( OrderedTriangulation );
  }

  /// constructor with a given graph
  OrderedTriangulation::OrderedTriangulation(
      const UndiGraph*                          theGraph,
      const NodeProperty<Size>*                 dom,
      const std::vector<NodeId>*                order,
      const OrderedEliminationSequenceStrategy& elimSeq,
      const JunctionTreeStrategy&               JTStrategy,
      bool                                      minimality )
      : StaticTriangulation( theGraph, dom, elimSeq, JTStrategy, minimality )
      , __order( order ) {
    static_cast<OrderedEliminationSequenceStrategy*>(
        _elimination_sequence_strategy )
        ->setOrder( __order );

    // for debugging purposes
    GUM_CONSTRUCTOR( OrderedTriangulation );
  }

  // copy constructor
  OrderedTriangulation::OrderedTriangulation( const OrderedTriangulation& from )
      : StaticTriangulation( from )
      , __order( from.__order ) {
    // for debugging purposes
    GUM_CONS_CPY( OrderedTriangulation );
  }

  // move constructor
  OrderedTriangulation::OrderedTriangulation( OrderedTriangulation&& from )
      : StaticTriangulation( std::move( from ) )
      , __order( from.__order ) {
    // for debugging purposes
    GUM_CONS_MOV( OrderedTriangulation );
  }

  /// virtual copy constructor
  OrderedTriangulation* OrderedTriangulation::newFactory() const {
    return new OrderedTriangulation(
        static_cast<const OrderedEliminationSequenceStrategy&>(
            *_elimination_sequence_strategy ),
        *_junction_tree_strategy );
  }

  /// virtual copy constructor
  OrderedTriangulation* OrderedTriangulation::copyFactory() const {
    return new OrderedTriangulation( *this );
  }

  /// destructor
  OrderedTriangulation::~OrderedTriangulation() {
    // for debugging purposes
    GUM_DESTRUCTOR( OrderedTriangulation );
  }

  /// initialize the triangulation data structures for a new graph
  void OrderedTriangulation::setGraph( const UndiGraph*          graph,
                                       const NodeProperty<Size>* domsizes ) {
    StaticTriangulation::setGraph( graph, domsizes );
    static_cast<OrderedEliminationSequenceStrategy*>(
        _elimination_sequence_strategy )
        ->setOrder( __order );
  }

  /// sets the sequence of elimination
  void OrderedTriangulation::setOrder( const std::vector<NodeId>* order ) {
    __order = order;
    static_cast<OrderedEliminationSequenceStrategy*>(
        _elimination_sequence_strategy )
        ->setOrder( __order );
  }

  /// the function called to initialize the triangulation process
  void OrderedTriangulation::_initTriangulation( UndiGraph& graph ) {
    OrderedEliminationSequenceStrategy* elim =
        static_cast<OrderedEliminationSequenceStrategy*>(
            _elimination_sequence_strategy );
    elim->setGraph( &graph, _domain_sizes );
    elim->setOrder( __order );
  }

} /* namespace gum */
