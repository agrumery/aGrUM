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
      const OrderedEliminationSequenceStrategy &elimSeq,
      const JunctionTreeStrategy &JTStrategy, bool minimality)
      : StaticTriangulation(elimSeq, JTStrategy, minimality), __sequence(0) {
    // for debugging purposes
    GUM_CONSTRUCTOR(OrderedTriangulation);
  }

  /// constructor with a given graph
  OrderedTriangulation::OrderedTriangulation(
      const UndiGraph *theGraph, const NodeProperty<Size> *dom,
      const std::vector<NodeId> *sequence,
      const OrderedEliminationSequenceStrategy &elimSeq,
      const JunctionTreeStrategy &JTStrategy, bool minimality)
      : StaticTriangulation(theGraph, dom, elimSeq, JTStrategy, minimality),
        __sequence(sequence) {
    // for debugging purposes
    GUM_CONSTRUCTOR(OrderedTriangulation);
  }

  /// destructor
  OrderedTriangulation::~OrderedTriangulation() {
    // for debugging purposes
    GUM_DESTRUCTOR(OrderedTriangulation);
  }

  /// initialize the triangulation data structures for a new graph
  void OrderedTriangulation::setGraph(const UndiGraph *graph,
                                      const NodeProperty<Size> *modal,
                                      const std::vector<NodeId> *sequence) {
    // check that the graph, the modalities and the sequence seem OK
    unsigned int nb = 0;

    if (graph)
      ++nb;

    if (modal)
      ++nb;

    if (sequence)
      ++nb;

    if ((nb != 0) && (nb != 3)) {
      GUM_ERROR(GraphError, "OrderedTriangulation requires valid "
                            "graphs, modalities and elimination orderings");
    }

    StaticTriangulation::_setGraph(graph, modal);

    __sequence = sequence;
  }

  /// virtual copy constructor
  OrderedTriangulation *OrderedTriangulation::newFactory() const {
    return new OrderedTriangulation;
  }

  /// the function called to initialize the triangulation process
  void OrderedTriangulation::_initTriangulation(UndiGraph &graph) {
    OrderedEliminationSequenceStrategy *elim =
        static_cast<OrderedEliminationSequenceStrategy *>(
            _elimination_sequence_strategy);
    elim->setGraph(&graph, __sequence);
  }

} /* namespace gum */
