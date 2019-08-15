
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/** @file
 * @brief Inline implementation of Base classes for directed acylic graphs
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */

// to ease IDE parser
#include <agrum/core/exceptions.h>
#include <agrum/graphs/DAG.h>

namespace gum {

  INLINE DAG& DAG::operator=(const DAG& g) {
    // avoid self assignment
    if (this != &g) { DiGraph::operator=(g); }

    return *this;
  }

  INLINE void DAG::addArc(const NodeId tail, const NodeId head) {
    if (hasDirectedPath(head, tail)) {
      GUM_ERROR(InvalidDirectedCycle, "Add a directed cycle in a dag !");
    }

    // checking whether tail and head do belong to the graph is performed
    // within class DiGraph
    DiGraph::addArc(tail, head);
  }

} /* namespace gum */
