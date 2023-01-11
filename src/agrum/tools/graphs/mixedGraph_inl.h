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
 * @brief Inline implementation of Base classes for mixed graphs
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */

namespace gum {

  INLINE MixedGraph& MixedGraph::operator=(const MixedGraph& g) {
    // avoid self assignment
    if (this != &g) {
      // remove the old graph properly
      EdgeGraphPart::clearEdges();
      ArcGraphPart::clearArcs();
      NodeGraphPart::clearNodes();

      // fill the new graph
      NodeGraphPart::operator=(g);
      EdgeGraphPart::operator=(g);
      ArcGraphPart:: operator=(g);
    }

    return *this;
  }

  INLINE void MixedGraph::clear() {
    EdgeGraphPart::clearEdges();
    ArcGraphPart::clearArcs();
    NodeGraphPart::clearNodes();
  }

  INLINE void MixedGraph::eraseNode(const NodeId node) {
    EdgeGraphPart::unvirtualizedEraseNeighbours(node);
    ArcGraphPart::unvirtualizedEraseParents(node);
    ArcGraphPart::unvirtualizedEraseChildren(node);
    NodeGraphPart::eraseNode(node);
  }

  INLINE bool MixedGraph::operator==(const MixedGraph& g) const {
    return ArcGraphPart::operator==(g) && EdgeGraphPart::operator==(g)
        && NodeGraphPart::                               operator==(g);
  }

  INLINE NodeSet MixedGraph::boundary(NodeId node) const {
    return neighbours(node) + parents(node) + children(node);
  }
} /* namespace gum */
