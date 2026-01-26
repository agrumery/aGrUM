/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/** @file
 * @brief Source implementation of classes for undirected edge sets
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */
#include <agrum/base/graphs/parts/edgeGraphPart.h>

#ifdef GUM_NO_INLINE
#  include <agrum/base/graphs/parts/edgeGraphPart_inl.h>
#endif   // GUM_NO_INLINE
#include "agrum/base/graphs/graphElements.h"

namespace gum {

  ///////////////////// EdgeGraphPart
  EdgeGraphPart::EdgeGraphPart(Size edges_size, bool edges_resize_policy) :
      _edges_(edges_size, edges_resize_policy) {
    GUM_CONSTRUCTOR(EdgeGraphPart);
  }

  EdgeGraphPart::EdgeGraphPart(const EdgeGraphPart& s) : _edges_(s._edges_) {
    GUM_CONS_CPY(EdgeGraphPart)

    // copy the set of neighbours
    _neighbours_.resize(s._neighbours_.capacity());

    for (const auto& [key, nodeset]: s._neighbours_) {
      NodeSet* newneigh = new NodeSet(*nodeset);
      _neighbours_.insert(key, newneigh);
    }

    // send signals to indicate that there are new edges
    if (onEdgeAdded.hasListener())
      for (const auto& edge: _edges_)
        GUM_EMIT2(onEdgeAdded, edge.first(), edge.second());
  }

  EdgeGraphPart::~EdgeGraphPart() {
    GUM_DESTRUCTOR(EdgeGraphPart)
    // be sure to deallocate all the neighbours sets
    _clearEdges_();
  }

  void EdgeGraphPart::clearEdges() { _clearEdges_(); }

  void EdgeGraphPart::_clearEdges_() {
    for (const auto& elt: _neighbours_)
      delete elt.second;

    _neighbours_.clear();

    if (onEdgeDeleted.hasListener()) {
      EdgeSet tmp = _edges_;
      _edges_.clear();

      for (const auto& edge: tmp)
        GUM_EMIT2(onEdgeDeleted, edge.first(), edge.second());
    } else {
      _edges_.clear();
    }
  }

  EdgeGraphPart& EdgeGraphPart::operator=(const EdgeGraphPart& s) {
    // avoid self assignment
    if (this != &s) {
      clearEdges();

      _edges_ = s._edges_;

      // copy the set of neighbours
      _neighbours_.resize(s._neighbours_.capacity());

      for (const auto& [key, nodeset]: s._neighbours_) {
        NodeSet* newneigh = new NodeSet(*nodeset);
        _neighbours_.insert(key, newneigh);
      }

      if (onEdgeAdded.hasListener())
        for (const auto& edge: _edges_)
          GUM_EMIT2(onEdgeAdded, edge.first(), edge.second());
    }

    return *this;
  }

  std::string EdgeGraphPart::toString() const {
    std::stringstream s;
    bool              first = true;
    s << "{";

    for (const auto& edge: _edges_) {
      if (first) first = false;
      else s << ",";

      s << edge;
    }

    s << "}";

    return s.str();
  }

  std::vector< NodeId > EdgeGraphPart::undirectedPath(NodeId n1, NodeId n2) const {
    // not recursive version => use a FIFO for simulating the recursion
    List< NodeId > nodeFIFO;
    nodeFIFO.pushBack(n2);

    // mark[node] = predecessor if visited, else mark[node] does not exist
    NodeProperty< NodeId > mark;
    mark.insert(n2, n2);

    NodeId current;

    while (!nodeFIFO.empty()) {
      current = nodeFIFO.front();
      nodeFIFO.popFront();

      // check the neighbour
      for (const auto new_one: neighbours(current)) {
        if (mark.exists(new_one))   // if this node is already marked, stop
          continue;

        mark.insert(new_one, current);

        if (new_one == n1) {
          std::vector< NodeId > v;

          for (current = n1; current != n2; current = mark[current])
            v.push_back(current);

          v.push_back(n2);

          return v;
        }

        nodeFIFO.pushBack(new_one);
      }
    }

    GUM_ERROR(NotFound, "no path found")
  }

  bool EdgeGraphPart::hasUndirectedPath(NodeId n1, NodeId n2) const {
    NodeSet visited;
    NodeSet temp;

    temp.insert(n1);
    while (!temp.empty()) {
      NodeId current = *(temp.begin());
      if (current == n2) return true;
      temp.erase(current);
      visited.insert(current);
      for (auto next: neighbours(current)) {
        if (!temp.contains(next) && !visited.contains(next)) temp.insert(next);
      }
    }
    return false;
  }

  bool EdgeGraphPart::hasUndirectedPath(NodeId n1, const NodeId n2, const NodeSet& except) const {
    NodeSet visited;
    NodeSet temp;

    if (except.contains(n2)) return false;

    temp.insert(n1);
    while (!temp.empty()) {
      NodeId current = *(temp.begin());
      if (current == n2) return true;
      temp.erase(current);
      visited.insert(current);
      for (auto next: neighbours(current)) {
        if (!temp.contains(next) && !visited.contains(next) && !except.contains(next))
          temp.insert(next);
      }
    }
    return false;
  }

  bool EdgeGraphPart::hasUndirectedPath(const NodeSet& n1,
                                        const NodeSet& n2,
                                        const NodeSet& except) const {
    NodeSet visited;
    NodeSet temp;

    for (auto n: n1)
      temp.insert(n);

    while (!temp.empty()) {
      NodeId current = *(temp.begin());
      if (n2.contains(current)) return true;
      temp.erase(current);
      visited.insert(current);
      for (auto next: neighbours(current)) {
        if (!temp.contains(next) && !visited.contains(next) && !except.contains(next))
          temp.insert(next);
      }
    }
    return false;
  }

  std::ostream& operator<<(std::ostream& stream, const EdgeGraphPart& set) {
    stream << set.toString();
    return stream;
  }

} /* namespace gum */
