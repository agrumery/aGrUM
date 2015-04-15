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
 *   GNU General Public License for more defirsts.                         *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/** @file
 * @brief Inline implementation of classes for undirected edge sets
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */

// to ease parsing by IDE
#include <agrum/graphs/edgeGraphPart.h>

namespace gum {

  INLINE bool EdgeGraphPart::emptyEdges() const { return __edges.empty(); }

  INLINE Size EdgeGraphPart::sizeEdges() const { return __edges.size(); }

  INLINE const EdgeSet &EdgeGraphPart::edges() const { return __edges; }

  INLINE bool EdgeGraphPart::existsEdge(const Edge &edge) const {
    return __edges.contains(edge);
  }

  INLINE bool EdgeGraphPart::existsEdge(const NodeId first,
                                        const NodeId second) const {
    return __neighbours.exists(first) && __neighbours[first]->exists(second);
  }

  INLINE void EdgeGraphPart::__checkNeighbours(const NodeId id) const {
    if (!__neighbours.exists(id)) {
      __neighbours.insert(id, new NodeSet);
    }
  }

  INLINE void EdgeGraphPart::insertEdge(const NodeId first, const NodeId second) {
    addEdge(first, second);
  }

  INLINE void EdgeGraphPart::addEdge(const NodeId first, const NodeId second) {
    Edge edge(first, second);
    __edges.insert(edge);
    __checkNeighbours(first);
    __checkNeighbours(second);
    __neighbours[first]->insert(second);
    __neighbours[second]->insert(first);

    GUM_EMIT2(onEdgeAdded, first, second);
  }

  INLINE void EdgeGraphPart::eraseEdge(const Edge &edge) {
    if (existsEdge(edge)) {
      // ASSUMING first and second exists in __neighbours (if not, it is an error)
      NodeId id1 = edge.first(), id2 = edge.second();

      __neighbours[id1]->erase(id2);
      __neighbours[id2]->erase(id1);
      __edges.erase(edge);
      GUM_EMIT2(onEdgeDeleted, id1, id2);
    }
  }

  INLINE const NodeSet &EdgeGraphPart::neighbours(const NodeId id) const {
    __checkNeighbours(id);
    return *(__neighbours[id]);
  }

  INLINE void EdgeGraphPart::eraseNeighbours(const NodeId id) {
    if (__neighbours.exists(id)) {
      const NodeSet &set = neighbours(id);

      for (auto iter = set.beginSafe(); iter != set.endSafe();
           ++iter) { // safe iterator needed here
        // warning: use this erase so that you actually use the virtualized
        // edge removal function
        eraseEdge(Edge(*iter, id));
      }
    }
  }

  INLINE void EdgeGraphPart::unvirtualizedEraseNeighbours(const NodeId id) {
    if (__neighbours.exists(id)) {
      const NodeSet &set = neighbours(id);

      for (auto iter = set.beginSafe(); iter != set.endSafe();
           ++iter) { // safe iterator needed here
        EdgeGraphPart::eraseEdge(Edge(*iter, id));
      }
    }
  }

  INLINE const EdgeGraphPart::EdgeIterator EdgeGraphPart::beginEdges() const {
    return __edges.begin();
  }

  INLINE const EdgeGraphPart::EdgeIterator &EdgeGraphPart::endEdges() const {
    return __edges.end();
  }

  INLINE bool EdgeGraphPart::operator==(const EdgeGraphPart &p) const {
    return __edges == p.__edges;
  }

  INLINE bool EdgeGraphPart::operator!=(const EdgeGraphPart &p) const {
    return __edges != p.__edges;
  }

} /* namespace gum */

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
