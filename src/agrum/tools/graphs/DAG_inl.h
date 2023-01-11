/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
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
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */

// to ease IDE parser
#include <agrum/tools/core/exceptions.h>
#include <agrum/tools/graphs/DAG.h>

namespace gum {

  INLINE DAG& DAG::operator=(const DAG& g) {
    // avoid self assignment
    if (this != &g) { DiGraph::operator=(g); }

    return *this;
  }

  INLINE void DAG::addArc(const NodeId tail, const NodeId head) {
    if (head == tail) { GUM_ERROR(InvalidDirectedCycle, "Add a mono-cycle in a dag !") }
    if (hasDirectedPath(head, tail)) {
      GUM_ERROR(InvalidDirectedCycle, "Add a directed cycle in a dag !")
    }

    // checking whether tail and head do belong to the graph is performed
    // within class DiGraph
    DiGraph::addArc(tail, head);
  }
} /* namespace gum */
