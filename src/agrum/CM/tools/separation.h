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

namespace gum {

  /**
   * @class Separation
   * @brief Consolidated d-separation utilities (DAG-centric).
   *
   * High-level helpers to test (conditional) independence in directed acyclic
   * graphs (DAGs) using the d-separation criterion. Wherever possible,
   * this class delegates to aGrUM-provided primitives and mirrors pyAgrum
   * semantics.
   *
   * **C++ ↔ pyAgrum correspondence**
   *  - `isDSeparated`              ⇨ `isDSep` / `isDSep_tech2`
   *  - `isBackdoorSeparated`       ⇨ `isDSep_parents` / `_isDSep_tech2_parents`
   *  - `isForwardSeparated`        ⇨ `_isDSep_tech2_children`
   *  - `reduceForDSeparation`      ⇨ `dSep_reduce` (uses `_barren_nodes`)
   *  - `findBarrenNodes`           ⇨ `_barren_nodes`
   *  - `isAncestorOf`              ⇨ `_is_ascendant`
   *  - `isDescendantOf`            ⇨ `_is_descendant`
   *  - `anyUndirectedConnection`   ⇨ `_is_path_x_y` (internal helper)
   *
   */
  class Separation {
    public:
    using NodeId  = gum::NodeId;
    using NodeSet = gum::NodeSet;

    // =======================================================================
    // Core public API
    // =======================================================================

    /**
     * @brief Test \( X \perp\!\!\!\perp Y \mid Z \) (general d-separation).
     *
     * Delegates to `DAG::dSeparation`, which:
     *  1. Builds the moralized ancestral graph of \(X \cup Y \cup Z\).
     *  2. Removes nodes in \(Z\).
     *  3. Checks undirected connectivity between \(X\) and \(Y\).
     *
     * @param dag The causal DAG.
     * @param X Set of source nodes.
     * @param Y Set of target nodes.
     * @param Z Conditioning set.
     * @return `true` iff \(X\) is d-separated from \(Y\) given \(Z\) in `dag`.
     */
    static bool isDSeparated(const DAG& dag, const NodeSet& X, const NodeSet& Y, const NodeSet& Z);

    /**
     * @brief Backdoor-style restriction: only paths whose first edge points *into* X.
     *
     * Semantics: tests whether Z blocks all paths from Y to X that enter X via an
     * incoming edge (the “backdoor graph” \(G_{\underline{X}}\)).
     *
     * Implementation:
     *  1. Copy `dag` to `g`.
     *  2. For each \(x \in X\), erase every outgoing arc \(x \to c\) in `g`.
     *  3. Return `g.dSeparation(X, Y, Z)`.
     *
     * @param dag The causal DAG.
     * @param X Set of “treatment” nodes (incoming-edge focus).
     * @param Y Set of outcome nodes.
     * @param Z Conditioning set.
     * @return `true` iff Z d-separates X and Y in the backdoor graph \(G_{\underline{X}}\).
     */
    static bool
        isBackdoorSeparated(const DAG& dag, const NodeSet& X, const NodeSet& Y, const NodeSet& Z);

    /**
     * @brief Forward-style restriction: only paths whose first edge *leaves* X.
     *
     * Semantics: tests whether Z blocks all paths starting from X via an
     * outgoing edge (children-first restriction used in frontdoor checks).
     *
     * Outline:
     *  1. Let `interest = Anc(Y ∪ Z)`. Build the moralized ancestral graph:
     *     `UndiGraph ug = dag.moralizedAncestralGraph(interest)`.
     *  2. Ensure each \(x \in X\) exists in `ug` (add isolated node if missing),
     *     but do **not** add \(x\)–parent links.
     *  3. For each \(x \in X\), add an undirected edge \(x\)—\(c\) for every child
     *     \(c\) of \(x\) present in `ug`.
     *  4. Remove all nodes in \(Z\) from `ug`.
     *  5. Return `true` iff there is **no** undirected path between any node in X and any node in
     * Y.
     *
     * @param dag The causal DAG.
     * @param X Set of start nodes (outgoing-edge focus).
     * @param Y Set of target nodes.
     * @param Z Conditioning set.
     * @return `true` iff Z blocks all forward/children paths from X to Y.
     */
    static bool
        isForwardSeparated(const DAG& dag, const NodeSet& X, const NodeSet& Y, const NodeSet& Z);

    // =======================================================================
    // Optional reduction for speed
    // =======================================================================

    /**
     * @brief Barren-node pruning relative to the interest set \(X \cup Y \cup Z\).
     *
     * Uses `BarrenNodesFinder` with `evidence = Z` and `targets = X ∪ Y` to
     * compute barren nodes, then removes them from a copy of `dag`.
     *
     * @param dag The causal DAG to prune.
     * @param X Source nodes of interest.
     * @param Y Target nodes of interest.
     * @param Z Evidence/conditioning set.
     * @return A reduced DAG with barren nodes removed (safe for d-sep queries).
     */
    static DAG
        reduceForDSeparation(const DAG& dag, const NodeSet& X, const NodeSet& Y, const NodeSet& Z);

    /**
     * @brief Compute barren nodes given evidence and targets.
     *
     * Thin wrapper over `BarrenNodesFinder::barrenNodes()`.
     *
     * @param dag The causal DAG.
     * @param evidenceZ Evidence/conditioning set Z.
     * @param targetsXY Union of target nodes (typically \(X \cup Y\)).
     * @return Set of barren nodes that can be pruned without affecting d-sep.
     */
    static NodeSet
        findBarrenNodes(const DAG& dag, const NodeSet& evidenceZ, const NodeSet& targetsXY);

    // =======================================================================
    // Minimal structural helpers (delegating to DAG where possible)
    // =======================================================================

    /**
     * @brief Is x an ancestor of y?
     *
     * @param dag The causal DAG.
     * @param x Candidate ancestor.
     * @param y Candidate descendant.
     * @return `true` iff \(x \in \mathrm{Anc}(y)\).
     */
    static bool isAncestorOf(const DAG& dag, NodeId x, NodeId y);

    /**
     * @brief Is x a descendant of y?
     *
     * @param dag The causal DAG.
     * @param x Candidate descendant.
     * @param y Candidate ancestor.
     * @return `true` iff \(x \in \mathrm{Desc}(y)\).
     */
    static bool isDescendantOf(const DAG& dag, NodeId x, NodeId y);

    private:
    // =======================================================================
    // Internal helper
    // =======================================================================

    /**
     * @brief Check if any node in A is undirected-connected to any node in B.
     *
     * @param ug An undirected graph (usually a moralized ancestral graph).
     * @param A First node set.
     * @param B Second node set.
     * @return `true` iff there exists an undirected path between some `a ∈ A`
     *         and some `b ∈ B` in `ug`.
     */
    static bool anyUndirectedConnection(const UndiGraph& ug, const NodeSet& A, const NodeSet& B);
  };

}   // namespace gum

#endif   // GUM_DSEPARATION_H
