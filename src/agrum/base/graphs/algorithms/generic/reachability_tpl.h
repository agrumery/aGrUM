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


/**
 * @file
 * @brief Template implementations for reachability.h.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Christophe GONZALES(_at_AMU)
 */

#pragma once

// to ease IDE parser
#include <agrum/base/graphs/algorithms/generic/reachability.h>

namespace gum::graph {

  template < GUM_DiGraphable G >
  NodeSet ancestors(const G& g, NodeId id) {
    NodeSet res;
    NodeSet frontier = g.parents(id);

    while (!frontier.empty()) {
      const NodeId current = *frontier.begin();
      frontier.erase(current);
      res.insert(current);
      for (const auto p: g.parents(current))
        if (!res.contains(p)) frontier.insert(p);
    }

    return res;
  }

  template < GUM_DiGraphable G >
  NodeSet descendants(const G& g, NodeId id) {
    NodeSet res;
    NodeSet frontier = g.children(id);

    while (!frontier.empty()) {
      const NodeId current = *frontier.begin();
      frontier.erase(current);
      res.insert(current);
      for (const auto c: g.children(current))
        if (!res.contains(c)) frontier.insert(c);
    }

    return res;
  }

  template < GUM_DiGraphable G >
  NodeSet family(const G& g, NodeId id) {
    NodeSet res{id};
    return res + g.parents(id);
  }

  template < GUM_DiGraphable G >
  NodeSet family(const G& g, const NodeSet& ids) {
    NodeSet res;
    for (const auto node: ids)
      res += family(g, node);
    return res;
  }

  template < GUM_UndiGraphable G >
  NodeProperty< NodeId > chainComponents(const G& g) {
    NodeProperty< NodeId > res;
    NodeId                 numCC = 0;

    for (const auto node: g.nodes()) {
      if (res.exists(node)) continue;

      NodeSet frontier{node};
      while (!frontier.empty()) {
        const NodeId current = *frontier.begin();
        frontier.erase(current);
        res.insert(current, numCC);
        for (const auto nei: g.neighbours(current))
          if (!res.exists(nei)) frontier.insert(nei);
      }
      ++numCC;
    }

    return res;
  }

  template < GUM_NodeGraphable G >
  NodeProperty< NodeId > connectedComponents(const G& g) {
    NodeProperty< NodeId > res;
    NodeId                 numCC = 0;

    for (const auto node: g.nodes()) {
      if (res.exists(node)) continue;

      NodeSet frontier{node};
      while (!frontier.empty()) {
        const NodeId current = *frontier.begin();
        frontier.erase(current);
        res.insert(current, numCC);

        if constexpr (GUM_DiGraphable< G >) {
          for (const auto n: g.parents(current))
            if (!res.exists(n)) frontier.insert(n);
          for (const auto n: g.children(current))
            if (!res.exists(n)) frontier.insert(n);
        }
        if constexpr (GUM_UndiGraphable< G >) {
          for (const auto n: g.neighbours(current))
            if (!res.exists(n)) frontier.insert(n);
        }
      }
      ++numCC;
    }

    return res;
  }

  template < GUM_UndiGraphable G >
  bool areConnected(const G& g, const NodeSet& A, const NodeSet& B) {
    if (A.empty() || B.empty()) return false;
    if (!(A * B).empty()) return true;

    NodeSet visited;
    NodeSet frontier = A;
    for (const auto s: A)
      visited.insert(s);

    while (!frontier.empty()) {
      const NodeId u = *frontier.begin();
      frontier.erase(u);
      for (const auto v: g.neighbours(u)) {
        if (visited.exists(v)) continue;
        if (B.contains(v)) return true;
        visited.insert(v);
        frontier.insert(v);
      }
    }
    return false;
  }

  template < GUM_MixedGraphable G >
  NodeSet chainComponent(const G& g, NodeId node) {
    NodeSet res;
    NodeSet frontier{node};

    while (!frontier.empty()) {
      const NodeId n = *frontier.begin();
      frontier.erase(n);
      res.insert(n);
      for (const auto nei: g.neighbours(n))
        if (!res.contains(nei)) frontier.insert(nei);
    }

    return res;
  }

  template < GUM_MixedGraphable G >
  NodeSet boundary(const G& g, NodeId node) {
    return g.neighbours(node) + g.parents(node) + g.children(node);
  }

}   // namespace gum::graph
