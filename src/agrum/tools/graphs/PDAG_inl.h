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
#include <agrum/tools/graphs/PDAG.h>

namespace gum {

  INLINE PDAG& PDAG::operator=(const PDAG& g) {
    // avoid self assignment
    if (this != &g) { MixedGraph::operator=(g); }

    return *this;
  }

  INLINE void PDAG::addArc(const NodeId tail, const NodeId head) {
    if (head == tail) { GUM_ERROR(InvalidDirectedCycle, "Add a mono-cycle in a PDAG !") }
    if (this->hasMixedOrientedPath(head, tail)) {
      GUM_ERROR(InvalidPartiallyDirectedCycle, "Add a partially directed cycle in a PDAG !")
    }

    // checking whether tail and head do belong to the graph is performed
    // within class DiGraph
    MixedGraph::addArc(tail, head);
  }
  INLINE void PDAG::addEdge(const NodeId tail, const NodeId head) {
    if (head == tail) { GUM_ERROR(InvalidDirectedCycle, "Add a mono-cycle in a PDAG !") }
    if (this->hasMixedReallyOrientedPath(head, tail)) {
      GUM_ERROR(InvalidPartiallyDirectedCycle, "Add a partially directed cycle in a PDAG !")
    }
    if (this->hasMixedReallyOrientedPath(tail, head)) {
      GUM_ERROR(InvalidPartiallyDirectedCycle, "Add a partially directed cycle in a PDAG !")
    }

    // checking whether tail and head do belong to the graph is performed
    // within class DiGraph
    MixedGraph::addEdge(tail, head);
  }
} /* namespace gum */
