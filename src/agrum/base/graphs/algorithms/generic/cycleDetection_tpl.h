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

#include <agrum/base/graphs/algorithms/generic/cycleDetection.h>

namespace gum::graph {

  template < GUM_DiGraphable G >
  Sequence< NodeId > topologicalOrder(const G& g) {
    if (g.empty()) return {};

    NodeProperty< Size >  indegree;
    std::vector< NodeId > border;
    border.reserve(g.size() / 2);

    for (const auto node: g.nodes()) {
      const auto& par = g.parents(node);
      indegree.insert(node, par.size());
      if (par.empty()) border.push_back(node);
    }

    if (border.empty())
      GUM_ERROR(InvalidDirectedCycle, "cycles prevent the creation of a topological ordering.")

    Sequence< NodeId > result;
    while (!border.empty()) {
      const NodeId root = border.back();
      border.pop_back();

      if (result.exists(root))
        GUM_ERROR(InvalidDirectedCycle, "cycles prevent the creation of a topological ordering.")
      result.insert(root);

      for (const auto child: g.children(root)) {
        const Size deg = indegree[child];
        if (deg == 0)
          GUM_ERROR(InvalidDirectedCycle, "cycles prevent the creation of a topological ordering.")
        if (deg == 1) border.push_back(child);
        indegree[child] = deg - 1;
      }
    }

    GUM_ASSERT(result.size() == g.size());
    return result;
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
