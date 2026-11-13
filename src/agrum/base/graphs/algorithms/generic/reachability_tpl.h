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

#include <vector>

// to ease IDE parser
#include <agrum/base/graphs/algorithms/generic/reachability.h>

namespace gum::graph {

  namespace detail {
    /// the nodes reachable from the nodes of ids through at least one step of forEachNext
    /// (parents or children): a node of ids belongs to the result only if it is reachable from a
    /// node of ids
    template < typename FOR_EACH_NEXT >
    NodeSet directedClosure(const NodeSet& ids, FOR_EACH_NEXT forEachNext) {
      // a node enters res when it is reached, and the stack once
      NodeSet               res;
      std::vector< NodeId > stack;
      const auto            reach = [&res, &stack](NodeId n) {
        if (!res.contains(n)) {
          res.insert(n);
          stack.push_back(n);
        }
      };
      for (const auto id: ids)
        forEachNext(id, reach);

      while (!stack.empty()) {
        const NodeId current = stack.back();
        stack.pop_back();
        forEachNext(current, reach);
      }

      return res;
    }

    template < typename FOR_EACH_PREDECESSOR >
    NodeSet pathClosure(const NodeSet& ids, FOR_EACH_PREDECESSOR forEachPredecessor) {
      NodeSet res = directedClosure(ids, forEachPredecessor);

      // true if a backward search from sources reaches target (which is not expanded); the
      // direct step from forbiddenFrom to target is not followed
      const auto reaches
          = [&forEachPredecessor](const NodeSet& sources, NodeId target, NodeId forbiddenFrom) {
              NodeSet               visited(sources);
              std::vector< NodeId > stack(sources.begin(), sources.end());
              bool                  found = false;
              while (!found && !stack.empty()) {
                const NodeId current = stack.back();
                stack.pop_back();
                forEachPredecessor(current, [&](NodeId w) {
                  if (found) return;
                  if (w == target) {
                    if (current != forbiddenFrom) found = true;
                  } else if (!visited.contains(w)) {
                    visited.insert(w);
                    stack.push_back(w);
                  }
                });
              }
              return found;
            };

      // a node of ids reached by a walk only: it must have a path to another node of ids, or lie
      // on a cycle (a walk back through the same undirected edge is not a cycle)
      for (const auto v: ids) {
        if (!res.contains(v)) continue;
        if (reaches(ids - NodeSet{v}, v, v)) continue;   // no forbidden step (v is not a source)
        bool onCycle = false;
        forEachPredecessor(v, [&](NodeId u) {
          if (!onCycle && reaches(NodeSet{u}, v, u)) onCycle = true;
        });
        if (!onCycle) res.erase(v);
      }

      return res;
    }
  }   // namespace detail

  template < GUM_DiGraphable G >
  NodeSet ancestors(const G& g, NodeId id) {
    return ancestors(g, NodeSet{id});
  }

  template < GUM_DiGraphable G >
  NodeSet ancestors(const G& g, const NodeSet& ids) {
    return detail::directedClosure(ids, [&g](NodeId n, const auto& visit) {
      for (const auto p: g.parents(n))
        visit(p);
    });
  }

  template < GUM_DiGraphable G >
  NodeSet descendants(const G& g, NodeId id) {
    return descendants(g, NodeSet{id});
  }

  template < GUM_DiGraphable G >
  NodeSet descendants(const G& g, const NodeSet& ids) {
    return detail::directedClosure(ids, [&g](NodeId n, const auto& visit) {
      for (const auto c: g.children(n))
        visit(c);
    });
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
        } else if constexpr (GUM_MarkedGraphable< G >) {
          // the adjacent nodes, whatever the marks (MAG::neighbours are the tail-tail ones only)
          if constexpr (requires { g.adjacents(current); }) {
            for (const auto n: g.adjacents(current))
              if (!res.exists(n)) frontier.insert(n);
          } else {
            for (const auto n: g.neighbours(current))
              if (!res.exists(n)) frontier.insert(n);
          }
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
  NodeSet anteriors(const G& g, const NodeSet& ids) {
    return detail::pathClosure(ids, [&g](NodeId n, const auto& visit) {
      for (const auto p: g.parents(n))
        visit(p);
      for (const auto w: g.neighbours(n))
        visit(w);
    });
  }

  template < GUM_MixedGraphable G >
  NodeSet boundary(const G& g, NodeId node) {
    return g.neighbours(node) + g.parents(node) + g.children(node);
  }

}   // namespace gum::graph
