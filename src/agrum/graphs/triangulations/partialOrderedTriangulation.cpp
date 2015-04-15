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
 * @brief class for graph triangulations for which we enforce a given partial
 * ordering on the nodes eliminations.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/config.h>

#include <agrum/graphs/triangulations/partialOrderedTriangulation.h>

namespace gum {

  /// default constructor
  PartialOrderedTriangulation::PartialOrderedTriangulation(
      const PartialOrderedEliminationSequenceStrategy &elimSeq,
      const JunctionTreeStrategy &JTStrategy, bool minimality)
      : StaticTriangulation(elimSeq, JTStrategy, minimality), __modalities(0),
        __partial_order(0) {
    // for debugging purposes
    GUM_CONSTRUCTOR(PartialOrderedTriangulation);
  }

  /// constructor with a given graph
  PartialOrderedTriangulation::PartialOrderedTriangulation(
      const UndiGraph *theGraph, const NodeProperty<Size> *dom,
      const List<NodeSet> *partial_order,
      const PartialOrderedEliminationSequenceStrategy &elimSeq,
      const JunctionTreeStrategy &JTStrategy, bool minimality)
      : StaticTriangulation(theGraph, dom, elimSeq, JTStrategy, minimality),
        __modalities(dom), __partial_order(partial_order) {
    // for debugging purposes
    GUM_CONSTRUCTOR(PartialOrderedTriangulation);
  }

  /// destructor
  PartialOrderedTriangulation::~PartialOrderedTriangulation() {
    // for debugging purposes
    GUM_DESTRUCTOR(PartialOrderedTriangulation);
  }

  /// initialize the triangulation data structures for a new graph
  void PartialOrderedTriangulation::setGraph(const UndiGraph *graph,
                                             const NodeProperty<Size> *modal,
                                             const List<NodeSet> *partial_order) {
    // check that the graph, the modalities and the sequence seem OK
    unsigned char nb = 0;

    if (graph)
      ++nb;

    if (modal)
      ++nb;

    if (partial_order)
      ++nb;

    if ((nb != 0) && (nb != 3)) {
      GUM_ERROR(GraphError, "PartialOrderedTriangulation requires valid "
                            "graphs, modalities and elimination orderings");
    }

    StaticTriangulation::_setGraph(graph, modal);

    __partial_order = partial_order;
    __modalities = modal;
  }

  /// virtual copy constructor
  PartialOrderedTriangulation *PartialOrderedTriangulation::newFactory() const {
    return new PartialOrderedTriangulation;
  }

  /// the function called to initialize the triangulation process
  void PartialOrderedTriangulation::_initTriangulation(UndiGraph &graph) {
    PartialOrderedEliminationSequenceStrategy *elim =
        static_cast<PartialOrderedEliminationSequenceStrategy *>(
            _elimination_sequence_strategy);
    elim->setGraph(&graph, __modalities, __partial_order);
  }

} /* namespace gum */
