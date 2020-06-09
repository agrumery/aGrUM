
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @author Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *
 */

namespace gum {

  INLINE MixedGraph& MixedGraph::operator=(const MixedGraph& g) {
    // avoid self assigment
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

  INLINE void MixedGraph::eraseNode(const NodeId id) {
    EdgeGraphPart::unvirtualizedEraseNeighbours(id);
    ArcGraphPart::unvirtualizedEraseParents(id);
    ArcGraphPart::unvirtualizedEraseChildren(id);
    NodeGraphPart::eraseNode(id);
  }

  INLINE bool MixedGraph::operator==(const MixedGraph& p) const {
    return ArcGraphPart::operator==(p) && EdgeGraphPart::operator==(p)
           && NodeGraphPart::                            operator==(p);
  }

  INLINE bool MixedGraph::operator!=(const MixedGraph& p) const {
    return !operator==(p);
  }

} /* namespace gum */
