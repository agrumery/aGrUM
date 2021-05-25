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
 * @brief Source implementation of classes for undirected edge sets
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *
 */
#include <agrum/tools/graphs/parts/edgeGraphPart.h>

#ifdef GUM_NO_INLINE
#  include <agrum/tools/graphs/parts/edgeGraphPart_inl.h>
#endif   // GUM_NOINLINE
#include "agrum/tools/graphs/graphElements.h"

namespace gum {

  ///////////////////// EdgeGraphPart
  EdgeGraphPart::EdgeGraphPart(Size edges_size, bool edges_resize_policy) :
       _edges_(edges_size, edges_resize_policy) {
    GUM_CONSTRUCTOR(EdgeGraphPart);
  }

  EdgeGraphPart::EdgeGraphPart(const EdgeGraphPart& s) :  _edges_(s. _edges_) {
    GUM_CONS_CPY(EdgeGraphPart);

    // copy the set of neighbours
     _neighbours_.resize(s. _neighbours_.capacity());

    for (const auto& elt: s. _neighbours_) {
      NodeSet* newneigh = new NodeSet(*elt.second);
       _neighbours_.insert(elt.first, newneigh);
    }

    // send signals to indicate that there are new edges
    if (onEdgeAdded.hasListener())
      for (const auto& edge:  _edges_)
        GUM_EMIT2(onEdgeAdded, edge.first(), edge.second());
  }

  EdgeGraphPart::~EdgeGraphPart() {
    GUM_DESTRUCTOR(EdgeGraphPart);
    // be sure to deallocate all the neighbours sets
    clearEdges();
  }

  void EdgeGraphPart::clearEdges() {
    for (const auto& elt:  _neighbours_)
      delete elt.second;

     _neighbours_.clear();

    if (onEdgeDeleted.hasListener()) {
      EdgeSet tmp =  _edges_;
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

       _edges_ = s. _edges_;

      // copy the set of neighbours
       _neighbours_.resize(s. _neighbours_.capacity());

      for (const auto& elt: s. _neighbours_) {
        NodeSet* newneigh = new NodeSet(*elt.second);
         _neighbours_.insert(elt.first, newneigh);
      }

      if (onEdgeAdded.hasListener())
        for (const auto& edge:  _edges_)
          GUM_EMIT2(onEdgeAdded, edge.first(), edge.second());
    }

    return *this;
  }

  std::string EdgeGraphPart::toString() const {
    std::stringstream s;
    bool              first = true;
    s << "{";

    for (const auto& edge:  _edges_) {
      if (first)
        first = false;
      else
        s << ",";

      s << edge;
    }

    s << "}";

    return s.str();
  }

  const std::vector< NodeId >
     EdgeGraphPart::undirectedPath(const NodeId n1, const NodeId n2) const {
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

  bool EdgeGraphPart::hasUndirectedPath(const NodeId n1, const NodeId n2) const {
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

  bool EdgeGraphPart::hasUndirectedPath(const NodeId   n1,
                                        const NodeId   n2,
                                        const NodeSet& except) const {
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
        if (!temp.contains(next) && !visited.contains(next)
            && !except.contains(next))
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
        if (!temp.contains(next) && !visited.contains(next)
            && !except.contains(next))
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
