/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
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
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
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
 * @brief Source implementation of classes for directed edge sets
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */
#include <agrum/base/graphs/parts/arcGraphPart.h>

#ifdef GUM_NO_INLINE
#  include <agrum/base/graphs/parts/arcGraphPart_inl.h>
#endif   // GUM_NOINLINE
#include "agrum/base/graphs/graphElements.h"

namespace gum {

  ///////////////////// ArcGraphPart
  ArcGraphPart::ArcGraphPart(Size arcs_size, bool arcs_resize_policy) :
      _arcs_(arcs_size, arcs_resize_policy) {
    GUM_CONSTRUCTOR(ArcGraphPart);
  }

  ArcGraphPart::ArcGraphPart(const ArcGraphPart& s) : _arcs_(s._arcs_) {
    GUM_CONS_CPY(ArcGraphPart);

    // copy the sets of parents
    const NodeProperty< NodeSet* >& pars = s._parents_;
    _parents_.resize(pars.capacity());

    for (const auto& [key, nodeset]: pars) {
      NodeSet* newpar = new NodeSet(*nodeset);
      _parents_.insert(key, newpar);
    }

    // copy the sets of children
    const NodeProperty< NodeSet* >& children = s._children_;
    _children_.resize(children.capacity());

    for (const auto& [key, nodeset]: children) {
      NodeSet* newchildren = new NodeSet(*nodeset);
      _children_.insert(key, newchildren);
    }

    // send signals to indicate that there are new arcs
    if (onArcAdded.hasListener()) {
      for (const auto& arc: _arcs_) {
        GUM_EMIT2(onArcAdded, arc.tail(), arc.head());
      }
    }
  }

  ArcGraphPart::~ArcGraphPart() {
    GUM_DESTRUCTOR(ArcGraphPart);
    // be sure to deallocate all the parents and children sets
    clearArcs();
  }

  void ArcGraphPart::clearArcs() {
    for (const auto& elt: _parents_)
      delete elt.second;

    _parents_.clear();

    for (const auto& elt: _children_)
      delete elt.second;

    _children_.clear();

    // we need this copy only if at least one onArcDeleted listener exists
    if (onArcDeleted.hasListener()) {
      ArcSet tmp = _arcs_;
      _arcs_.clear();

      for (const auto& arc: tmp)
        GUM_EMIT2(onArcDeleted, arc.tail(), arc.head());
    } else {
      _arcs_.clear();
    }
  }

  ArcGraphPart& ArcGraphPart::operator=(const ArcGraphPart& s) {
    // avoid self assignment
    if (this != &s) {
      // copy the arcs
      clearArcs();
      _arcs_ = s._arcs_;

      // copy the sets of parents
      _parents_.resize(s._parents_.capacity());

      for (const auto& [key, nodeset]: s._parents_) {
        NodeSet* newpar = new NodeSet(*nodeset);
        _parents_.insert(key, newpar);
      }

      // copy the sets of children
      _children_.resize(s._children_.capacity());

      for (const auto& [key, nodeset]: s._children_) {
        NodeSet* newchildren = new NodeSet(*nodeset);
        _children_.insert(key, newchildren);
      }

      if (onArcAdded.hasListener()) {
        for (const auto& arc: _arcs_) {
          GUM_EMIT2(onArcAdded, arc.tail(), arc.head());
        }
      }
    }

    return *this;
  }

  std::string ArcGraphPart::toString() const {
    std::stringstream s;
    bool              first = true;
    s << "{";

    for (const auto& arc: _arcs_) {
      if (first) {
        first = false;
      } else {
        s << ",";
      }

      s << arc;
    }

    s << "}";

    return s.str();
  }

  NodeSet ArcGraphPart::descendants(NodeId id) const {
    NodeSet res;
    NodeSet tmp;
    for (auto next: children(id))
      tmp.insert(next);

    while (!tmp.empty()) {
      auto current = *(tmp.begin());
      tmp.erase(current);
      res.insert(current);
      for (auto next: children(current)) {
        if (!tmp.contains(next) && !res.contains(next)) { tmp.insert(next); }
      }
    }
    return res;
  }

  NodeSet ArcGraphPart::ancestors(NodeId id) const {
    NodeSet res;
    NodeSet tmp;
    for (auto next: parents(id))
      tmp.insert(next);

    while (!tmp.empty()) {
      auto current = *(tmp.begin());
      tmp.erase(current);
      res.insert(current);
      for (auto next: parents(current)) {
        if (!tmp.contains(next) && !res.contains(next)) { tmp.insert(next); }
      }
    }
    return res;
  }

  std::vector< NodeId > ArcGraphPart::directedPath(NodeId n1, NodeId n2) const {
    // not recursive version => use a FIFO for simulating the recursion
    List< NodeId > nodeFIFO;
    nodeFIFO.pushBack(n2);

    // mark[node] = successor if visited, else mark[node] does not exist
    NodeProperty< NodeId > mark;
    mark.insert(n2, n2);

    NodeId current;

    while (!nodeFIFO.empty()) {
      current = nodeFIFO.front();
      nodeFIFO.popFront();

      // check the parents

      for (const auto new_one: parents(current)) {
        if (mark.exists(new_one))   // if this node is already marked, do not
          continue;                 // check it again

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

  std::vector< NodeId > ArcGraphPart::directedUnorientedPath(NodeId n1, NodeId n2) const {
    // not recursive version => use a FIFO for simulating the recursion
    List< NodeId > nodeFIFO;
    nodeFIFO.pushBack(n2);

    // mark[node] = successor if visited, else mark[node] does not exist
    NodeProperty< NodeId > mark;
    mark.insert(n2, n2);

    NodeId current;

    while (!nodeFIFO.empty()) {
      current = nodeFIFO.front();
      nodeFIFO.popFront();

      // check the parents
      for (const auto new_one: parents(current)) {
        if (mark.exists(new_one))   // the node has already been visited
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

      // check the children
      for (const auto new_one: children(current)) {
        if (mark.exists(new_one))   // the node has already been visited
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

  std::ostream& operator<<(std::ostream& stream, const ArcGraphPart& set) {
    stream << set.toString();
    return stream;
  }

} /* namespace gum */
