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


#pragma once

#include <list>
#include <vector>

#include <agrum/base/graphs/algorithms/generic/chordality.h>

namespace gum::graph {

  template < GUM_UndiGraphable G >
  Sequence< NodeId > lexBFS(const G& g, const std::vector< NodeId >& first) {
    for (Idx i = 0; i < first.size(); ++i) {
      if (!g.existsNode(first[i]))
        GUM_ERROR(InvalidArgument, "The node " << first[i] << " is not a node of the graph")
      for (Idx j = i + 1; j < first.size(); ++j) {
        if (first[i] == first[j])
          GUM_ERROR(InvalidArgument, "The node " << first[i] << " is given twice to visit first")
        if (!g.existsEdge(first[i], first[j]))
          GUM_ERROR(InvalidArgument, "The nodes to visit first are not a clique of the graph")
      }
    }

    // the unvisited nodes are partitioned into ordered classes (nodes with the same label); the
    // first node of the first class has the largest label
    using Class = std::list< NodeId >;
    std::list< Class > classes;
    classes.emplace_back();
    NodeProperty< typename std::list< Class >::iterator > classOf;
    NodeProperty< typename Class::iterator >              positionOf;
    NodeSet                                               visitedFirst;
    for (const auto node: first)
      visitedFirst.insert(node);
    for (const auto node: g.nodes()) {
      if (visitedFirst.contains(node)) continue;
      classes.front().push_back(node);
      classOf.insert(node, classes.begin());
      positionOf.insert(node, std::prev(classes.front().end()));
    }
    if (classes.front().empty()) classes.clear();

    Sequence< NodeId > order;
    const auto         visit = [&](NodeId node) {
      order.insert(node);
      // the unvisited neighbours of node move to a new class placed just before their own one
      HashTable< const Class*, typename std::list< Class >::iterator > split;
      for (const auto neighbour: g.neighbours(node)) {
        if (!classOf.exists(neighbour)) continue;   // already visited, or to be visited first
        const auto oldClass = classOf[neighbour];
        if (!split.exists(&*oldClass)) split.insert(&*oldClass, classes.emplace(oldClass));
        const auto newClass = split[&*oldClass];
        newClass->splice(newClass->end(), *oldClass, positionOf[neighbour]);
        classOf[neighbour] = newClass;
        if (oldClass->empty()) classes.erase(oldClass);
      }
    };

    for (const auto node: first)
      visit(node);
    while (!classes.empty()) {
      auto&        nodes = classes.front();
      const NodeId node  = nodes.front();
      nodes.pop_front();
      if (nodes.empty()) classes.pop_front();
      classOf.erase(node);
      positionOf.erase(node);
      visit(node);
    }
    return order;
  }

  template < GUM_UndiGraphable G >
  Sequence< NodeId > maximumCardinalitySearch(const G& g) {
    // buckets[k] holds the unvisited nodes with k visited neighbours
    std::vector< std::list< NodeId > >                     buckets(g.size() + 1);
    NodeProperty< Size >                                   weight;
    NodeProperty< typename std::list< NodeId >::iterator > positionOf;
    for (const auto node: g.nodes()) {
      buckets[0].push_back(node);
      weight.insert(node, 0);
      positionOf.insert(node, std::prev(buckets[0].end()));
    }

    Sequence< NodeId > order;
    Size               best = 0;
    while (order.size() < g.size()) {
      while (buckets[best].empty())
        --best;
      const NodeId node = buckets[best].front();
      buckets[best].pop_front();
      weight.erase(node);
      order.insert(node);

      for (const auto neighbour: g.neighbours(node)) {
        if (!weight.exists(neighbour)) continue;   // already visited
        Size& w = weight[neighbour];
        buckets[w + 1].splice(buckets[w + 1].end(), buckets[w], positionOf[neighbour]);
        ++w;
        if (w > best) best = w;
      }
    }
    return order;
  }

  template < GUM_UndiGraphable G >
  bool isPerfectEliminationOrdering(const G& g, const Sequence< NodeId >& order) {
    if (order.size() != g.size())
      GUM_ERROR(InvalidArgument, "The ordering does not contain all the nodes of the graph")
    for (const auto node: order)
      if (!g.existsNode(node))
        GUM_ERROR(InvalidArgument, "The ordering contains " << node << " which is not a node")

    for (const auto node: order) {
      const Idx pos = order.pos(node);
      // the first neighbour placed after node
      bool   found = false;
      NodeId first = 0;
      for (const auto neighbour: g.neighbours(node)) {
        const Idx npos = order.pos(neighbour);
        if (npos > pos && (!found || npos < order.pos(first))) {
          first = neighbour;
          found = true;
        }
      }
      if (!found) continue;
      for (const auto neighbour: g.neighbours(node))
        if (neighbour != first && order.pos(neighbour) > pos && !g.existsEdge(first, neighbour))
          return false;
    }
    return true;
  }

  template < GUM_UndiGraphable G >
  bool isChordal(const G& g) {
    const auto         visit = maximumCardinalitySearch(g);
    Sequence< NodeId > elimination;
    for (Idx i = visit.size(); i > 0; --i)
      elimination.insert(visit[i - 1]);
    return isPerfectEliminationOrdering(g, elimination);
  }

}   // namespace gum::graph
