
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
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
 * @brief Inline implementation of Base classes for undirected graphs
 *
 * @author Pierre-Henri WUILLEMIN (@LIP6) and Christophe GONZALES (@AMU)
 *
 */

// to ease parser
#include <agrum/tools/graphs/undiGraph.h>

namespace gum {

  INLINE void UndiGraph::addEdge(const NodeId first, const NodeId second) {
    if (!exists(first)) { GUM_ERROR(InvalidNode, "first node"); }

    if (!exists(second)) { GUM_ERROR(InvalidNode, "second node"); }

    EdgeGraphPart::addEdge(second, first);
  }

  INLINE void UndiGraph::clear() {
    EdgeGraphPart::clearEdges();
    NodeGraphPart::clearNodes();
  }

  INLINE UndiGraph& UndiGraph::operator=(const UndiGraph& g) {
    if (this != &g) {
      UndiGraph::clear();
      NodeGraphPart::operator=(g);
      EdgeGraphPart::operator=(g);
    }

    return *this;
  }

  INLINE void UndiGraph::eraseNode(const NodeId id) {
    // warning: to remove the edges adjacent to id, use the unvirtualized
    // versions
    // of edge removals
    EdgeGraphPart::unvirtualizedEraseNeighbours(id);
    NodeGraphPart::eraseNode(id);
  }

  INLINE bool UndiGraph::operator==(const UndiGraph& p) const {
    return EdgeGraphPart::operator==(p) && NodeGraphPart::operator==(p);
  }

  INLINE bool UndiGraph::operator!=(const UndiGraph& p) const {
    return !operator==(p);
  }

} /* namespace gum */