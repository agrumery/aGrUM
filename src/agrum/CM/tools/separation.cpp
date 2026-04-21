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
 * @brief d-separation utilities implementation
 */

/****************************************************************************
 *   aGrUM/pyAgrum — Separation implementation                              *
 ****************************************************************************/

#include <algorithm>
#include <vector>

#include <agrum/base/graphs/algorithms/generic/reachability.h>
#include <agrum/BN/algorithms/barrenNodesFinder.h>
#include <agrum/CM/tools/separation.h>

#include <unordered_set>

namespace gum {

  // --------------------------- small helpers -------------------------------

  namespace {
    // Remove every node of Z from the undirected graph
    inline void _eraseAll(UndiGraph& ug, const NodeSet& Z) {
      for (const auto z: Z)
        if (ug.existsNode(z)) ug.eraseNode(z);
    }

    inline void removeConditioningNodes(UndiGraph& ug, const NodeSet& Z) { _eraseAll(ug, Z); }
  }   // namespace

  // ==========================================================================
  // Core public API
  // ==========================================================================

  /**
   * Pyagrum counterpart: isDSep  (delegates to isDSep_tech2)
   */
  bool Separation::isDSeparated(const DAG&     dag,
                                const NodeSet& X,
                                const NodeSet& Y,
                                const NodeSet& Z) {
    // delegate directly to aGrUM's built-in d-separation
    return dag.dSeparation(X, Y, Z);
  }

  /**
   * Pyagrum counterpart: isDSep_parents / _isDSep_tech2_parents
   */
  bool Separation::isBackdoorSeparated(const DAG&     dag,
                                       const NodeSet& X,
                                       const NodeSet& Y,
                                       const NodeSet& Z) {
    // Build G_{\underline X}: remove all outgoing arcs from X
    DAG g = dag;

    for (auto x: X) {
      // collect children first, then erase (avoid iterator invalidation)
      std::vector< NodeId > ch;
      ch.reserve(g.children(x).size());
      for (auto c: g.children(x))
        ch.push_back(c);

      for (auto c: ch) {
        if (g.existsArc(x, c)) g.eraseArc(gum::Arc(x, c));
      }
    }

    // Now test standard d-separation in G_{\underline X}
    return g.dSeparation(X, Y, Z);
  }

  /**
   * Pyagrum counterpart: _isDSep_tech2_children
   */
  bool Separation::isForwardSeparated(const DAG&     dag,
                                      const NodeSet& X,
                                      const NodeSet& Y,
                                      const NodeSet& Z) {
    // 1) Moralize only on Anc(Y ∪ Z)
    NodeSet interest = Y;
    for (auto z: Z)
      interest.insert(z);
    UndiGraph ug = dag.moralizedAncestralGraph(interest);

    // Ensure nodes in X exist in ug (without adding X—parent edges)
    for (auto x: X)
      if (!ug.existsNode(x)) ug.addNodeWithId(x);

    // 2) Add edges X—c for children c of X that are already in ug
    for (auto x: X) {
      for (auto c: dag.children(x)) {
        if (ug.existsNode(c) && !ug.existsEdge(x, c)) ug.addEdge(x, c);
      }
    }

    // 3) Remove conditioning nodes Z
    removeConditioningNodes(ug, Z);

    // 4) Return true iff there is no undirected path between X and Y
    return !anyUndirectedConnection(ug, X, Y);
  }

  // ==========================================================================
  // Optional reduction for speed
  // ==========================================================================

  /**
   * Pyagrum counterpart: dSep_reduce (uses _barren_nodes)
   *
   */
  DAG Separation::reduceForDSeparation(const DAG&     dag,
                                       const NodeSet& X,
                                       const NodeSet& Y,
                                       const NodeSet& Z) {
    // Targets = X ∪ Y
    const NodeSet targets{X + Y};

    // Configure finder
    gum::BarrenNodesFinder finder(&dag);
    finder.setTargets(&targets);   // nodes we care about reaching
    finder.setEvidence(&Z);        // observed nodes

    // Compute barren nodes and remove them from a copy
    const NodeSet barren = finder.barrenNodes();
    DAG           reduced{dag};
    for (auto n: barren)
      if (reduced.existsNode(n)) reduced.eraseNode(n);

    return reduced;
  }

  /**
   * Pyagrum counterpart: _barren_nodes
   *
   * Implementation note:
   *  Conservative stand-in: nodes outside Anc(X∪Y∪Z) are "barren" for our purpose.
   *  If BarrenNodesFinder::barrenNodes() is available, you can implement this
   *  as a thin wrapper around it.
   */
  NodeSet Separation::findBarrenNodes(const DAG&     dag,
                                      const NodeSet& evidenceZ,
                                      const NodeSet& targetsXY) {
    gum::BarrenNodesFinder finder(&dag);
    finder.setTargets(&targetsXY);
    finder.setEvidence(&evidenceZ);
    return finder.barrenNodes();
  }

  // ==========================================================================
  // Minimal structural helpers
  // ==========================================================================

  /**
   * Pyagrum counterpart: _is_ascendant
   */
  bool Separation::isAncestorOf(const DAG& dag, NodeId x, NodeId y) {
    return dag.ancestors(y).contains(x);
  }

  /**
   * Pyagrum counterpart: _is_descendant
   */
  bool Separation::isDescendantOf(const DAG& dag, NodeId x, NodeId y) {
    return dag.descendants(y).contains(x);
  }

  // ==========================================================================
  // Internal helper
  // ==========================================================================

  /**
   * Pyagrum counterpart: _is_path_x_y
   */
  bool
      Separation::anyUndirectedConnection(const UndiGraph& ug, const NodeSet& A, const NodeSet& B) {
    return graph::areConnected(ug, A, B);
  }

}   // namespace gum
