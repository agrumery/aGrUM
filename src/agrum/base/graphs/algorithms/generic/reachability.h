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
 * @brief Generic node-reachability algorithms for aGrUM graphs.
 *
 * All functions live in namespace gum::graph and are constrained by the
 * concepts defined in graphConcepts.h :
 *
 *   GUM_DiGraphable    — ancestors, descendants, family
 *   GUM_UndiGraphable  — chainComponents
 *   GUM_NodeGraphable  — connectedComponents (Di/Undi/Mixed via if constexpr)
 *   GUM_MixedGraphable — chainComponent, boundary
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_GRAPH_REACHABILITY_H
#define GUM_GRAPH_REACHABILITY_H

#include <agrum/agrum.h>
#include <agrum/base/graphs/graphConcepts.h>
#include <agrum/base/graphs/graphElements.h>

namespace gum::graph {

  // =========================================================================
  // Directed graphs (GUM_DiGraphable)
  // =========================================================================

  /**
   * @brief Returns the set of all ancestors of @p id (nodes from which
   *        @p id is reachable following arc direction). @p id not included.
   */
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

  /**
   * @brief Returns the set of all descendants of @p id (nodes reachable
   *        from @p id following arc direction). @p id not included.
   */
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

  /// @brief Returns the family of @p id : { id } ∪ parents(id).
  template < GUM_DiGraphable G >
  NodeSet family(const G& g, NodeId id) {
    NodeSet res{id};
    return res + g.parents(id);
  }

  /// @brief Returns the union of families of all nodes in @p ids.
  template < GUM_DiGraphable G >
  NodeSet family(const G& g, const NodeSet& ids) {
    NodeSet res;
    for (const auto node: ids)
      res += family(g, node);
    return res;
  }

  // =========================================================================
  // Undirected graphs (GUM_UndiGraphable)
  // =========================================================================

  /**
   * @brief Returns a node-to-component-id mapping for the chain components
   *        of @p g (connected components of the edge-only subgraph).
   *
   * Arc orientations are ignored; only edges are traversed.
   * Component ids are assigned in order of first encounter.
   */
  template < GUM_UndiGraphable G >
  NodeProperty< NodeId > chainComponents(const G& g) {
    NodeProperty< NodeId > res;
    NodeId numCC = 0;

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

  /**
   * @brief Returns a node-to-component-id mapping for the (weakly) connected
   *        components of @p g.
   *
   * For directed graphs, arcs are treated as undirected (weak connectivity).
   * For mixed graphs, both edges and arcs are traversed.
   * Component ids are assigned in order of first encounter.
   *
   * @tparam G Must satisfy GUM_DiGraphable, GUM_UndiGraphable, or both.
   */
  template < GUM_NodeGraphable G >
  NodeProperty< NodeId > connectedComponents(const G& g) {
    NodeProperty< NodeId > res;
    NodeId numCC = 0;

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

  /**
   * @brief Returns true iff some node in @p A can reach some node in @p B
   *        via undirected edges.
   *
   * Returns false immediately if @p A or @p B is empty, or if both sets
   * share at least one node (trivially connected).
   *
   * @tparam G Any GUM_UndiGraphable graph.
   * @param g The undirected (or mixed) graph — only edges are traversed.
   * @param A First node set.
   * @param B Second node set.
   * @return `true` iff there exists an undirected path between some `a ∈ A`
   *         and some `b ∈ B` in @p g.
   */
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

  // =========================================================================
  // Mixed graphs (GUM_MixedGraphable)
  // =========================================================================

  /**
   * @brief Returns the chain component of @p node in @p g.
   *
   * The chain component is the connected component of the undirected subgraph
   * (edges only) containing @p node.  Arc orientations are ignored.
   */
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

  /// @brief Returns the boundary of @p node: neighbours ∪ parents ∪ children.
  template < GUM_MixedGraphable G >
  NodeSet boundary(const G& g, NodeId node) {
    return g.neighbours(node) + g.parents(node) + g.children(node);
  }

}   // namespace gum::graph

#endif   // GUM_GRAPH_REACHABILITY_H
