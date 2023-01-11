/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief Inline implementation of Base classes for oriented graphs
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */

// to ease parser in IDEs
#include <agrum/tools/graphs/diGraph.h>

namespace gum {

  INLINE void DiGraph::addArc(const NodeId tail, const NodeId head) {
    if (!exists(head)) { GUM_ERROR(InvalidNode, "no head node : " << head) }

    if (!exists(tail)) { GUM_ERROR(InvalidNode, "no tail node : " << tail) }

    ArcGraphPart::addArc(tail, head);
  }

  INLINE void DiGraph::clear() {
    ArcGraphPart::clearArcs();
    NodeGraphPart::clearNodes();
  }

  INLINE DiGraph& DiGraph::operator=(const DiGraph& g) {
    // avoid self assigment
    if (this != &g) {
      DiGraph::clear();
      NodeGraphPart::operator=(g);
      ArcGraphPart:: operator=(g);
    }

    return *this;
  }

  INLINE void DiGraph::eraseNode(const NodeId id) {
    // warning: to remove the arcs adjacent to id, use the unvirtualized
    // versions
    // of arc removals
    ArcGraphPart::unvirtualizedEraseParents(id);
    ArcGraphPart::unvirtualizedEraseChildren(id);

    NodeGraphPart::eraseNode(id);
  }

  INLINE bool DiGraph::operator==(const DiGraph& p) const {
    return ArcGraphPart::operator==(p) && NodeGraphPart::operator==(p);
  }

  INLINE bool DiGraph::operator!=(const DiGraph& p) const { return !operator==(p); }

} /* namespace gum */
