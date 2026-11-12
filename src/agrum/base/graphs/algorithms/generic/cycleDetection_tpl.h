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

#include <algorithm>

#include <agrum/base/graphs/algorithms/generic/cycleDetection.h>
#include <agrum/base/graphs/algorithms/generic/reachability.h>

namespace gum::graph {

  namespace detail {
    /// Kahn's algorithm: the nodes of g in a topological order, without the nodes of the directed
    /// cycles and the nodes reachable from them (all the nodes iff g has no directed cycle)
    template < GUM_DiGraphable G >
    std::vector< NodeId > kahnOrder(const G& g) {
      NodeProperty< Size >  indegree;
      std::vector< NodeId > border;
      for (const auto node: g.nodes()) {
        const Size deg = g.parents(node).size();
        indegree.insert(node, deg);
        if (deg == 0) border.push_back(node);
      }

      std::vector< NodeId > order;
      order.reserve(g.size());
      while (!border.empty()) {
        const NodeId node = border.back();
        border.pop_back();
        order.push_back(node);
        for (const auto child: g.children(node))
          if (--indegree[child] == 0) border.push_back(child);
      }
      return order;
    }
  }   // namespace detail

  template < GUM_DiGraphable G >
  Sequence< NodeId > topologicalOrder(const G& g) {
    const auto order = detail::kahnOrder(g);
    // the nodes of a cycle are never reached, even when there are nodes without parent elsewhere
    if (order.size() != g.size())
      GUM_ERROR(InvalidDirectedCycle, "cycles prevent the creation of a topological ordering.")
    Sequence< NodeId > result;
    for (const auto node: order)
      result.insert(node);
    return result;
  }

  template < GUM_DiGraphable G >
  bool hasDirectedCycle(const G& g) {
    return detail::kahnOrder(g).size() != g.size();
  }

  template < GUM_MixedGraphable G >
  bool hasPartiallyDirectedCycle(const G& g) {
    // label each node by its chain component (connected component of the undirected part), the
    // labels being 0, 1, ..., nbComponents - 1
    const auto component    = chainComponents(g);
    Size       nbComponents = 0;
    for (const auto node: g.nodes())
      nbComponents = std::max(nbComponents, Size(component[node]) + 1);

    // Kahn's algorithm on the graph of the components (an arc inside a component is a cycle)
    std::vector< Size >                indegree(nbComponents, 0);
    std::vector< std::vector< Size > > successors(nbComponents);
    for (const auto node: g.nodes())
      for (const auto child: g.children(node)) {
        const Size from = component[node], to = component[child];
        if (from == to) return true;
        successors[from].push_back(to);
        ++indegree[to];
      }

    std::vector< Size > border;
    for (Size c = 0; c < nbComponents; ++c)
      if (indegree[c] == 0) border.push_back(c);
    Size nbRemoved = 0;
    while (!border.empty()) {
      const Size c = border.back();
      border.pop_back();
      ++nbRemoved;
      for (const auto next: successors[c])
        if (--indegree[next] == 0) border.push_back(next);
    }
    return nbRemoved != nbComponents;
  }

  template < GUM_UndiGraphable G >
  bool hasUndirectedCycle(const G& g) {
    NodeProperty< bool > visited;
    for (const auto node: g.nodes())
      visited.insert(node, false);

    for (const auto node: g.nodes()) {
      if (visited[node]) continue;
      visited[node] = true;

      List< std::pair< NodeId, NodeId > > frontier;
      frontier.pushBack({node, node});

      while (!frontier.empty()) {
        const auto [current, from] = frontier.front();
        frontier.popFront();

        for (const auto next: g.neighbours(current)) {
          if (next == from) continue;
          if (visited[next]) return true;
          visited[next] = true;
          frontier.pushBack({next, current});
        }
      }
    }

    return false;
  }

}   // namespace gum::graph
