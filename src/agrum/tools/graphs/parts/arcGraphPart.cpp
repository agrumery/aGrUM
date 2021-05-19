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
 * @brief Source implementation of classes for directed edge sets
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *
 */
#include <agrum/tools/graphs/parts/arcGraphPart.h>

#ifdef GUM_NO_INLINE
#  include <agrum/tools/graphs/parts/arcGraphPart_inl.h>
#endif   // GUM_NOINLINE
#include "agrum/tools/graphs/graphElements.h"

namespace gum {

  ///////////////////// ArcGraphPart
  ArcGraphPart::ArcGraphPart(Size arcs_size, bool arcs_resize_policy) :
      arcs__(arcs_size, arcs_resize_policy) {
    GUM_CONSTRUCTOR(ArcGraphPart);
  }

  ArcGraphPart::ArcGraphPart(const ArcGraphPart& s) : arcs__(s.arcs__) {
    GUM_CONS_CPY(ArcGraphPart);

    // copy the sets of parents
    const NodeProperty< NodeSet* >& pars = s.parents__;
    parents__.resize(pars.capacity());

    for (const auto& elt: pars) {
      NodeSet* newpar = new NodeSet(*elt.second);
      parents__.insert(elt.first, newpar);
    }

    // copy the sets of children
    const NodeProperty< NodeSet* >& children = s.children__;
    children__.resize(children.capacity());

    for (const auto& elt: children) {
      NodeSet* newchildren = new NodeSet(*elt.second);
      children__.insert(elt.first, newchildren);
    }

    // send signals to indicate that there are new arcs
    if (onArcAdded.hasListener()) {
      for (const auto& arc: arcs__) {
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
    for (const auto& elt: parents__)
      delete elt.second;

    parents__.clear();

    for (const auto& elt: children__)
      delete elt.second;

    children__.clear();

    // we need this copy only if at least one onArcDeleted listener exists
    if (onArcDeleted.hasListener()) {
      ArcSet tmp = arcs__;
      arcs__.clear();

      for (const auto& arc: tmp)
        GUM_EMIT2(onArcDeleted, arc.tail(), arc.head());
    } else {
      arcs__.clear();
    }
  }

  ArcGraphPart& ArcGraphPart::operator=(const ArcGraphPart& s) {
    // avoid self assignment
    if (this != &s) {
      // copy the arcs
      clearArcs();
      arcs__ = s.arcs__;

      // copy the sets of parents
      parents__.resize(s.parents__.capacity());

      for (const auto& elt: s.parents__) {
        NodeSet* newpar = new NodeSet(*elt.second);
        parents__.insert(elt.first, newpar);
      }

      // copy the sets of children
      children__.resize(s.children__.capacity());

      for (const auto& elt: s.children__) {
        NodeSet* newchildren = new NodeSet(*elt.second);
        children__.insert(elt.first, newchildren);
      }

      if (onArcAdded.hasListener()) {
        for (const auto& arc: arcs__) {
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

    for (const auto& arc: arcs__) {
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

  std::vector< NodeId > ArcGraphPart::directedUnorientedPath(NodeId n1,
                                                             NodeId n2) const {
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
