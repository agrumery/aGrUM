/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief Inline implementation of classes for undirected edge sets
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *
 */

// to ease parsing by IDE
#include <agrum/tools/graphs/parts/edgeGraphPart.h>

namespace gum {

  INLINE bool EdgeGraphPart::emptyEdges() const { return edges__.empty(); }

  INLINE Size EdgeGraphPart::sizeEdges() const { return edges__.size(); }

  INLINE const EdgeSet& EdgeGraphPart::edges() const { return edges__; }

  INLINE bool EdgeGraphPart::existsEdge(const Edge& edge) const {
    return edges__.contains(edge);
  }

  INLINE bool EdgeGraphPart::existsEdge(const NodeId first,
                                        const NodeId second) const {
    return neighbours__.exists(first) && neighbours__[first]->exists(second);
  }

  INLINE void EdgeGraphPart::checkNeighbours__(const NodeId id) const {
    if (!neighbours__.exists(id)) { neighbours__.insert(id, new NodeSet); }
  }

  INLINE void EdgeGraphPart::addEdge(const NodeId first, const NodeId second) {
    Edge edge(first, second);
    edges__.insert(edge);
    checkNeighbours__(first);
    checkNeighbours__(second);
    neighbours__[first]->insert(second);
    neighbours__[second]->insert(first);

    GUM_EMIT2(onEdgeAdded, first, second);
  }

  INLINE void EdgeGraphPart::eraseEdge(const Edge& edge) {
    if (existsEdge(edge)) {
      // ASSUMING first and second exists in neighbours__ (if not, it is an
      // error)
      NodeId id1 = edge.first(), id2 = edge.second();

      neighbours__[id1]->erase(id2);
      neighbours__[id2]->erase(id1);
      edges__.erase(edge);
      GUM_EMIT2(onEdgeDeleted, id1, id2);
    }
  }

  INLINE const NodeSet& EdgeGraphPart::neighbours(const NodeId id) const {
    checkNeighbours__(id);
    return *(neighbours__[id]);
  }

  INLINE void EdgeGraphPart::eraseNeighbours(const NodeId id) {
    if (neighbours__.exists(id)) {
      const NodeSet& set = neighbours(id);

      for (auto iter = set.beginSafe(); iter != set.endSafe();
           ++iter) {   // safe iterator needed here
        // warning: use this erase so that you actually use the virtualized
        // edge removal function
        eraseEdge(Edge(*iter, id));
      }
    }
  }

  INLINE void EdgeGraphPart::unvirtualizedEraseNeighbours(const NodeId id) {
    if (neighbours__.exists(id)) {
      const NodeSet& set = neighbours(id);

      for (auto iter = set.beginSafe(); iter != set.endSafe();
           ++iter) {   // safe iterator needed here
        EdgeGraphPart::eraseEdge(Edge(*iter, id));
      }
    }
  }

  INLINE bool EdgeGraphPart::operator==(const EdgeGraphPart& p) const {
    return edges__ == p.edges__;
  }

  INLINE bool EdgeGraphPart::operator!=(const EdgeGraphPart& p) const {
    return edges__ != p.edges__;
  }

} /* namespace gum */
