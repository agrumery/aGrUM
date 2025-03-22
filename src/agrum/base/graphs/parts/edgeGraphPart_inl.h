/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/** @file
 * @brief Inline implementation of classes for undirected edge sets
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */

// to ease parsing by IDE
#include <agrum/base/graphs/parts/edgeGraphPart.h>

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
