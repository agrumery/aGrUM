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
 *   useful, but WITHOUT ANY KIND, EXPRESS OR IMPLIED,          *
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
 * @brief Generic Bayes Ball algorithm (Shachter 1998) for directed graphs.
 *
 * Provides a single template function:
 *
 *   gum::graph::requisiteNodes(g, query, Zhard, Zsoft)
 *
 * which computes the set of nodes d-connected to @p query given hard
 * evidence @p Zhard and soft evidence @p Zsoft, using the Bayes Ball
 * message-passing algorithm.
 *
 * The algorithm operates purely on the graph structure (parents / children)
 * and is therefore constrained by GUM_DiGraphable.  It does not depend on
 * any Bayesian-network–specific notion.
 *
 * **Complexity**: O(n + e) in the number of nodes and arcs, versus O(n² + e)
 * for the moralization-based approach.
 *
 * **Hard vs soft evidence**
 *   - Hard evidence (Zhard): the node is observed; it blocks upward
 *     propagation (as in a chain or fork) but does not activate a collider
 *     for downstream propagation.
 *   - Soft evidence (Zsoft): the node is "virtually" observed; it activates
 *     colliders (upward propagation) but does not block downward propagation.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_GRAPH_BAYESBALL_H
#define GUM_GRAPH_BAYESBALL_H

#include <utility>

#include <agrum/agrum.h>
#include <agrum/base/graphs/graphConcepts.h>
#include <agrum/base/graphs/graphElements.h>

namespace gum::graph {

  /**
   * @brief Returns the Shachter-requisite nodes for @p query given evidence.
   *
   * Implements Shachter's Bayes Ball algorithm on any GUM_DiGraphable graph.
   * A node is *requisite* if it lies on an active path that arrives at a query
   * node from *below* (upward visit), or if it is evidence that activates a
   * v-structure (collider activation, downward visit).  This is the minimal
   * set of nodes whose distributions are needed to compute the posterior of
   * the query.
   *
   * **This is NOT the full d-connected set.**  Nodes that are d-connected to
   * the query but only reachable via a downward (non-evidence) pass are
   * included in the traversal but not in the result.  Use @ref dConnected
   * when you need the full set of d-connected nodes (e.g. for d-separation).
   *
   * @tparam G Any directed graph satisfying GUM_DiGraphable (DAG, DiGraph, …).
   * @param g        The directed graph.
   * @param query    Source nodes from which the ball is launched.
   * @param Zhard    Hard-evidence nodes: block upward propagation; do NOT
   *                 activate colliders for downward propagation.
   * @param Zsoft    Soft-evidence nodes: activate colliders (upward) but do
   *                 not block downward propagation.
   * @return NodeSet of Shachter-requisite nodes.
   */
  template < GUM_DiGraphable G >
  NodeSet requisiteNodes(const G&       g,
                         const NodeSet& query,
                         const NodeSet& Zhard = {},
                         const NodeSet& Zsoft = {}) {
    NodeSet requisite;

    NodeProperty< std::pair< bool, bool > > marks(g.size());
    const std::pair< bool, bool >           empty_mark(false, false);

    List< std::pair< NodeId, bool > > to_visit;
    for (const auto node: query)
      to_visit.insert(std::pair< NodeId, bool >(node, true));

    while (!to_visit.empty()) {
      const NodeId node       = to_visit.front().first;
      const bool   from_child = to_visit.front().second;
      to_visit.popFront();

      if (!marks.exists(node)) marks.insert(node, empty_mark);

      if (from_child) {
        requisite.insert(node);
        if (Zhard.exists(node)) continue;

        if (!marks[node].first) {
          marks[node].first = true;
          for (const auto par: g.parents(node))
            to_visit.insert(std::pair< NodeId, bool >(par, true));
        }
        if (!marks[node].second) {
          marks[node].second = true;
          for (const auto chi: g.children(node))
            to_visit.insert(std::pair< NodeId, bool >(chi, false));
        }

      } else {
        const bool is_hard = Zhard.exists(node);
        const bool is_ev   = is_hard || Zsoft.exists(node);

        if (is_ev && !marks[node].first) {
          marks[node].first = true;
          requisite.insert(node);
          for (const auto par: g.parents(node))
            to_visit.insert(std::pair< NodeId, bool >(par, true));
        }
        if (!is_hard && !marks[node].second) {
          marks[node].second = true;
          for (const auto chi: g.children(node))
            to_visit.insert(std::pair< NodeId, bool >(chi, false));
        }
      }
    }

    return requisite;
  }

  /**
   * @brief Returns all nodes d-connected to @p query given evidence.
   *
   * Runs the same Bayes Ball traversal as @ref requisiteNodes but adds a node
   * to the result whenever it is *visited* from any direction — not only on
   * upward (from-child) passes.  This yields the full set of nodes reachable
   * via active trails from the query given @p Zhard (and @p Zsoft), which is
   * exactly what is needed for d-separation testing:
   *
   *   X ⊥ Y | Z  iff  Y ∉ dConnected(g, {X}, Z)
   *
   * @tparam G Any directed graph satisfying GUM_DiGraphable (DAG, DiGraph, …).
   * @param g        The directed graph.
   * @param query    Source nodes from which the ball is launched.
   * @param Zhard    Hard-evidence nodes (conditioning set for d-separation).
   * @param Zsoft    Soft-evidence nodes: activate colliders without blocking.
   * @return NodeSet of all nodes d-connected to @p query given the evidence.
   */
  template < GUM_DiGraphable G >
  NodeSet dConnected(const G&       g,
                     const NodeSet& query,
                     const NodeSet& Zhard = {},
                     const NodeSet& Zsoft = {}) {
    NodeSet visited;

    NodeProperty< std::pair< bool, bool > > marks(g.size());
    const std::pair< bool, bool >           empty_mark(false, false);

    List< std::pair< NodeId, bool > > to_visit;
    for (const auto node: query)
      to_visit.insert(std::pair< NodeId, bool >(node, true));

    while (!to_visit.empty()) {
      const NodeId node       = to_visit.front().first;
      const bool   from_child = to_visit.front().second;
      to_visit.popFront();

      if (!marks.exists(node)) marks.insert(node, empty_mark);

      // Every visit (upward or downward) means the node is d-connected.
      visited.insert(node);

      if (from_child) {
        if (Zhard.exists(node)) continue;

        if (!marks[node].first) {
          marks[node].first = true;
          for (const auto par: g.parents(node))
            to_visit.insert(std::pair< NodeId, bool >(par, true));
        }
        if (!marks[node].second) {
          marks[node].second = true;
          for (const auto chi: g.children(node))
            to_visit.insert(std::pair< NodeId, bool >(chi, false));
        }

      } else {
        const bool is_hard = Zhard.exists(node);
        const bool is_ev   = is_hard || Zsoft.exists(node);

        if (is_ev && !marks[node].first) {
          marks[node].first = true;
          for (const auto par: g.parents(node))
            to_visit.insert(std::pair< NodeId, bool >(par, true));
        }
        if (!is_hard && !marks[node].second) {
          marks[node].second = true;
          for (const auto chi: g.children(node))
            to_visit.insert(std::pair< NodeId, bool >(chi, false));
        }
      }
    }

    return visited;
  }

}   // namespace gum::graph

#endif   // GUM_GRAPH_BAYESBALL_H
