/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
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
 * @brief d-separation utilities
 */

#ifndef GUM_DSEPARATION_H
#define GUM_DSEPARATION_H

#include <agrum/agrum.h>
#include <agrum/base/graphs/DAG.h>
#include <agrum/base/graphs/undiGraph.h>

namespace gum {

/**
 * @brief Consolidated d-separation utilities (DAG-centric).
 *
 * Uses aGrUM-provided algorithms wherever possible:
 *  - General d-sep: DAG::dSeparation / DAG::moralizedAncestralGraph.  (Pyagrum: isDSep / isDSep_tech2)
 *  - Pruning: BarrenNodesFinder.                                      (Pyagrum: dSep_reduce / _barren_nodes)
 * The “parents/children only” restricted variants are built on top of the
 * moralized ancestral graph to mirror Pyagrum’s _isDSep_tech2_parents/_children.
 */
class DSeparation {
public:
  using NodeId  = gum::NodeId;
  using NodeSet = gum::NodeSet;

  // =======================================================================
  // Core public API
  // =======================================================================

  /**
   * @brief Test X ⟂ Y | Z (general d-separation).
   * Pyagrum counterpart: isDSep  (delegates to isDSep_tech2)
   *
   * Implementation note:
   *  Calls DAG::dSeparation(X,Y,Z), which builds the moralized
   *  ancestral graph of X∪Y∪Z, removes Z, and checks undirected connectivity.
   *  (See DAG::moralizedAncestralGraph / DAG::dSeparation.)
   */
  static bool isDSeparated(const DAG& dag,
                           const NodeSet& X,
                           const NodeSet& Y,
                           const NodeSet& Z);

  /**
   * @brief Backdoor-style restriction: only paths ENTERING X (incoming into X).
   * Pyagrum counterpart: isDSep_parents / _isDSep_tech2_parents
   *
   * Semantics:
   *  Z blocks all paths from Y to X whose first edge points into X.
   *
   * Implementation:
   *   1) Make a copy of dag -> g.
   *   2) For each x ∈ X, erase every arc x -> child in g (build G_{underline X}).
   *   3) Return g.dSeparation(X, Y, Z).
   */
  static bool isBackdoorSeparated(const DAG& dag,
                                  const NodeSet& X,
                                  const NodeSet& Y,
                                  const NodeSet& Z);

  /**
   * @brief Forward-style restriction: only paths LEAVING X (outgoing from X).
   * Pyagrum counterpart: _isDSep_tech2_children
   *
   * Semantics:
   *  Z blocks all directed paths starting from X via an outgoing edge.
   *
   * Implementation (matches pyAgrum):
   *   1) Let interest = Anc(Y ∪ Z). Build the moralized ancestral graph:
   *        UndiGraph ug = dag.moralizedAncestralGraph(interest);
   *   2) Ensure each x ∈ X exists as a node in ug (add if missing), but do NOT
   *      add X—parent edges.
   *   3) For each x ∈ X, add undirected edges X—c for every child c of x that is
   *      already present in ug.
   *   4) Remove all nodes in Z from ug (call _eraseAll(ug, Z)).
   *   5) Return true iff there is NO undirected path between any node in X and any node in Y.
   */
  static bool isForwardSeparated(const DAG& dag,
                                 const NodeSet& X,
                                 const NodeSet& Y,
                                 const NodeSet& Z);

  // =======================================================================
  // Optional reduction for speed
  // =======================================================================

  /**
   * @brief Barren-node pruning relative to interest set (X ∪ Y ∪ Z).
   * Pyagrum counterpart: dSep_reduce (uses _barren_nodes)
   *
   * Implementation note:
   *  Uses BarrenNodesFinder with evidence=Z and targets=X∪Y,
   *  removes the returned barren nodes from a copy of the DAG.  (No separate
   *  “filaires/linear-chain” step is exposed; see omissions below.)
   */
  static DAG reduceForDSeparation(const DAG& dag,
                                  const NodeSet& X,
                                  const NodeSet& Y,
                                  const NodeSet& Z);

  /**
   * @brief Return barren nodes given evidence and targets.
   * Pyagrum counterpart: _barren_nodes
   *
   * Implementation note:
   *  Thin wrapper over BarrenNodesFinder::barrenNodes().
   */
  static NodeSet findBarrenNodes(const DAG& dag,
                                 const NodeSet& evidenceZ,
                                 const NodeSet& targetsXY);

  // =======================================================================
  // Minimal structural helpers (delegating to DAG where possible)
  // =======================================================================

  /**
   * @brief x is ancestor of y ?
   * Pyagrum counterpart: _is_ascendant
   * (Delegates to DAG::ancestors(y).)
   */
  static bool isAncestorOf(const DAG& dag, NodeId x, NodeId y);

  /**
   * @brief x is descendant of y ?
   * Pyagrum counterpart: _is_descendant
   * (Delegates to DAG::descendants(y).)
   */
  static bool isDescendantOf(const DAG& dag, NodeId x, NodeId y);

private:
  // =======================================================================
  // Internal helper
  // =======================================================================

  /**
   * @brief Return true iff any node in A is undirected-connected to any node in B.
   * Pyagrum counterpart: _is_path_x_y
   *
   * Implementation note:
   *  Uses UndiGraph connected components (nodes2ConnectedComponent)
   *  or hasUndirectedPath under the hood.
   */
  static bool anyUndirectedConnection(const UndiGraph& ug,
                                      const NodeSet& A,
                                      const NodeSet& B);
};

} // namespace gum

#endif // GUM_DSEPARATION_H
