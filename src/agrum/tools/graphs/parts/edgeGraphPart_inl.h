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
 * @brief Inline implementation of classes for undirected edge sets
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */

// to ease parsing by IDE
#include <agrum/tools/graphs/parts/edgeGraphPart.h>

namespace gum {

  INLINE bool EdgeGraphPart::emptyEdges() const { return _edges_.empty(); }

  INLINE Size EdgeGraphPart::sizeEdges() const { return _edges_.size(); }

  INLINE const EdgeSet& EdgeGraphPart::edges() const { return _edges_; }

  INLINE bool EdgeGraphPart::existsEdge(const Edge& edge) const { return _edges_.contains(edge); }

  INLINE bool EdgeGraphPart::existsEdge(NodeId first, NodeId second) const {
    return _neighbours_.exists(first) && _neighbours_[first]->exists(second);
  }

  INLINE void EdgeGraphPart::_checkNeighbours_(NodeId id) {
    if (!_neighbours_.exists(id)) { _neighbours_.insert(id, new NodeSet); }
  }

  INLINE void EdgeGraphPart::addEdge(NodeId first, NodeId second) {
    Edge edge(first, second);
    _edges_.insert(edge);
    _checkNeighbours_(first);
    _checkNeighbours_(second);
    _neighbours_[first]->insert(second);
    _neighbours_[second]->insert(first);

    GUM_EMIT2(onEdgeAdded, first, second);
  }

  INLINE void EdgeGraphPart::eraseEdge(const Edge& edge) {
    if (existsEdge(edge)) {
      // ASSUMING first and second exists in  _neighbours_ (if not, it is an
      // error)
      NodeId id1 = edge.first();
      NodeId id2 = edge.second();

      _neighbours_[id1]->erase(id2);
      _neighbours_[id2]->erase(id1);
      _edges_.erase(edge);
      GUM_EMIT2(onEdgeDeleted, id1, id2);
    }
  }

  INLINE const NodeSet& EdgeGraphPart::neighbours(NodeId id) const {
    if (_neighbours_.exists(id)) return *(_neighbours_[id]);
    else return emptyNodeSet;
  }

  INLINE void EdgeGraphPart::eraseNeighbours(NodeId id) {
    if (_neighbours_.exists(id)) {
      const NodeSet& set = *(_neighbours_[id]);

      for (auto iter = set.beginSafe(); iter != set.endSafe();
           ++iter) {   // safe iterator needed here
        // warning: use this erases so that you actually use the virtualized
        // edge removal function
        eraseEdge(Edge(*iter, id));
      }
    }
  }

  INLINE void EdgeGraphPart::unvirtualizedEraseNeighbours(NodeId id) {
    if (_neighbours_.exists(id)) {
      const NodeSet& set = *(_neighbours_[id]);

      for (auto iter = set.beginSafe(); iter != set.endSafe();
           ++iter) {   // safe iterator needed here
        EdgeGraphPart::eraseEdge(Edge(*iter, id));
      }
    }
  }

  INLINE bool EdgeGraphPart::operator==(const EdgeGraphPart& p) const {
    return _edges_ == p._edges_;
  }

} /* namespace gum */
