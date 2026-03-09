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
 * @brief Door criteria (backdoor/frontdoor) utilities for a DAG.
 */

#include <algorithm>
#include <ranges>
#include <vector>

#include <agrum/CM/doorCriteria.h>
#include <agrum/CM/tools/separation.h>

namespace gum {

  namespace {
    // Convert NodeSet to sorted std::vector<NodeId> for deterministic ops.
    std::vector< NodeId > _sortedVec(const NodeSet& s) {
      std::vector< NodeId > v;
      v.reserve(s.size());
      for (auto n: s)
        v.push_back(n);
      std::ranges::sort(v);
      return v;
    }

    // Deterministic lexicographic comparator for NodeSet in O(n).
    // Key insight: the result is determined by e = min(A △ B), the first position
    // where sorted(A) and sorted(B) diverge.
    //   - e ∈ A: A[k]=e < B[k] unless B is exhausted (B is a prefix of A -> B < A).
    //   - e ∈ B: B[k]=e < A[k] unless A is exhausted (A is a prefix of B -> A < B).
    bool _lexLess(const NodeSet& a, const NodeSet& b) {
      NodeId minDiff = std::numeric_limits< NodeId >::max();
      bool   minInA  = false;

      for (auto n: a)
        if (!b.exists(n) && n < minDiff) {
          minDiff = n;
          minInA  = true;
        }
      for (auto n: b)
        if (!a.exists(n) && n < minDiff) {
          minDiff = n;
          minInA  = false;
        }

      if (minDiff == std::numeric_limits< NodeId >::max()) return false;   // a == b

      if (minInA) {
        // a < b iff b has at least one element > minDiff (otherwise b subset a)
        for (auto n: b)
          if (n > minDiff) return true;
        return false;
      } else {
        // a < b iff a has no element > minDiff (otherwise a[k] > b[k] = minDiff)
        for (auto n: a)
          if (n > minDiff) return false;

        return true;
      }
    }
  }   // namespace

  /* ========================= Backdoor / Frontdoor ========================= */

  bool DoorCriteria::satisfiesBackdoorCriterion(const DAG&     dag,
                                                NodeId         X,
                                                NodeId         Y,
                                                const NodeSet& Z) {
    // Rule-out illegal contents up front:
    // - Z must not contain X (conditioning on X is not a valid adjustment set)
    // - Z must not contain Y (keep parity with enumeration/universe)
    if (Z.contains(X) || Z.contains(Y)) return false;

    // - Z must not contain any descendant of X
    NodeSet descX = dag.descendants(X);
    for (auto dx: descX) {
      if (Z.contains(dx)) return false;
    }

    // Backdoor blocking condition in G_{underline X}
    NodeSet Xs;
    Xs.insert(X);
    NodeSet Ys;
    Ys.insert(Y);
    return Separation::isBackdoorSeparated(dag, Xs, Ys, Z);
  }

  bool DoorCriteria::satisfiesFrontdoorCriterion(const DAG&     dag,
                                                 NodeId         X,
                                                 NodeId         Y,
                                                 const NodeSet& Z) {
    if (Z.empty()) return false;

    // (FD-1) Z must intercept all directed X->...->Y paths
    if (existsUnblockedDirectedPath(dag, X, Y, Z)) return false;

    // (FD-2) Z ∩ backdoor_reach(X) = {}
    NodeSet br = backdoorReach(dag, X);
    for (auto z: Z)
      if (br.contains(z)) return false;

    // (FD-3) check on a d-separation reduced graph with interest = Z ∪ {X,Y}
    NodeSet Xset;
    Xset.insert(X);
    NodeSet Yset;
    Yset.insert(Y);
    // interest = X ∪ Y ∪ Z  -> pass Xset, Yset, Z as-is
    DAG reduced = Separation::reduceForDSeparation(dag, Xset, Yset, Z);

    NodeSet condX;
    condX.insert(X);
    for (auto z: Z) {
      NodeSet Zs;
      Zs.insert(z);
      // backdoor_path(reduced, z, y, {x}) == !isBackdoorSeparated(reduced, {z}, {y}, {x})
      if (!Separation::isBackdoorSeparated(reduced, Zs, Yset, condX)) return false;
    }
    return true;
  }

  DoorCriteria::NodeSetVec DoorCriteria::enumerateBackdoorSets(const DAG&     dag,
                                                               NodeId         X,
                                                               NodeId         Y,
                                                               const NodeSet& excluded_nodes,
                                                               std::size_t    max_cardinality,
                                                               bool           only_minimal,
                                                               bool           stopAtFirst) {
    NodeSetVec out;

    // Theoretical variant: if X has no parents, {} is a valid backdoor set.
    if (dag.parents(X).empty()) {
      out.emplace_back();
      return out;
    }

    // If Y is a parent of X, there is an open backdoor X<-Y that {} cannot block.
    if (dag.existsArc(Y, X)) return out;

    // Reduce around interest = {X, Y}; evidence = {}
    NodeSet Xset;
    Xset.insert(X);
    NodeSet Yset;
    Yset.insert(Y);
    NodeSet Zempty;
    DAG     G = Separation::reduceForDSeparation(dag, Xset, Yset, Zempty);

    // Candidate pool = nodes(G) \ (Desc(X) ∪ {X,Y} ∪ excluded_nodes)
    NodeSet descX = dag.descendants(X);
    NodeSet possible;
    for (auto n: G.nodes()) {
      if (n == X || n == Y) continue;
      if (excluded_nodes.contains(n)) continue;
      if (descX.contains(n)) continue;
      possible.insert(n);
    }
    if (possible.empty()) return out;

    auto              cand = _sortedVec(possible);
    const std::size_t N    = cand.size();
    const std::size_t Kmax = (max_cardinality == 0) ? N : std::min(max_cardinality, N);

    // Only prune supersets in only_minimal mode
    NodeSetVec chosen;
    const bool prune_supersets = only_minimal;

    NodeSet Xs;
    Xs.insert(X);
    NodeSet Ys;
    Ys.insert(Y);

    // pyagrum tries k >= 1 (never yields {} except our explicit no-parent case above)
    for (std::size_t k = 1; k <= Kmax; ++k) {
      std::vector< bool > pick(N, false);
      std::fill_n(pick.begin(), k, true);
      do {
        NodeSet Z;
        for (std::size_t i = 0; i < N; ++i)
          if (pick[i]) Z.insert(cand[i]);

        if (prune_supersets) {
          bool worth = true;
          for (const auto& s: chosen) {
            bool subset = true;
            for (auto u: s)
              if (!Z.contains(u)) {
                subset = false;
                break;
              }
            if (subset) {
              worth = false;
              break;
            }
          }
          if (!worth) continue;
        }

        if (Separation::isBackdoorSeparated(G, Xs, Ys, Z)) {
          if (only_minimal) {
            chosen.push_back(Z);
            out.push_back(Z);
          } else {
            out.push_back(Z);   // keep non-minimal supersets too (e.g., {U,W})
          }
          if (stopAtFirst) return out;
        }

      } while (std::prev_permutation(pick.begin(), pick.end()));
    }

    // Deterministic order + dedup
    std::sort(out.begin(), out.end(), _lexLess);
    out.erase(std::unique(out.begin(),
                          out.end(),
                          [](const NodeSet& a, const NodeSet& b) {
                            return _sortedVec(a) == _sortedVec(b);
                          }),
              out.end());
    return out;
  }

  DoorCriteria::NodeSetVec DoorCriteria::enumerateBackdoorSets(const DAG& dag,
                                                               NodeId     X,
                                                               NodeId     Y,
                                                               bool       stopAtFirst) {
    return enumerateBackdoorSets(dag, X, Y, NodeSet{}, 0, true, stopAtFirst);
  }

  DoorCriteria::NodeSetVec DoorCriteria::enumerateFrontdoorSets(const DAG&     dag,
                                                                NodeId         X,
                                                                NodeId         Y,
                                                                const NodeSet& excluded_nodes,
                                                                std::size_t    max_cardinality,
                                                                bool           only_minimal,
                                                                bool           stopAtFirst) {
    NodeSetVec out;

    // pyagrum early exit: if X is a parent of Y, yield nothing
    for (auto p: dag.parents(Y))
      if (p == X) return out;

    // Candidate pool
    NodeSet possible = nodesOnDirectedPaths(dag, X, Y);
    bool    noDiPath = false;

    if (possible.empty()) {
      // No directed path: use weakly connected component containing both X and Y (undirected BFS)
      NodeSet cc;
      {
        std::vector< NodeId > st;
        st.push_back(X);
        cc.insert(X);
        while (!st.empty()) {
          NodeId u = st.back();
          st.pop_back();
          for (auto v: dag.parents(u))
            if (!cc.contains(v)) {
              cc.insert(v);
              st.push_back(v);
            }
          for (auto v: dag.children(u))
            if (!cc.contains(v)) {
              cc.insert(v);
              st.push_back(v);
            }
        }
      }
      if (!cc.contains(Y)) return out;
      noDiPath = true;
      possible = cc;
      possible.erase(X);
      possible.erase(Y);
    } else {
      possible.erase(X);
      possible.erase(Y);
    }

    // Prune by backdoor reach of X and user exclusions
    NodeSet br = backdoorReach(dag, X);
    for (auto n: br)
      possible.erase(n);
    for (auto n: excluded_nodes)
      possible.erase(n);

    // Remove "impossible" z with a backdoor to Y given X, on a reduced graph:
    // interest = {X, Y} ∪ possible; evidence = {} (parity with pyagrum)
    NodeSet Xset;
    Xset.insert(X);
    NodeSet Yset = possible;
    Yset.insert(Y);
    NodeSet Zempty;

    DAG g = Separation::reduceForDSeparation(dag, Xset, Yset, Zempty);

    NodeSet impossible;
    NodeSet condX;
    condX.insert(X);
    for (auto z: possible) {
      NodeSet Zs;
      Zs.insert(z);
      if (!Separation::isBackdoorSeparated(g, Zs, NodeSet{Y}, condX)) { impossible.insert(z); }
    }
    for (auto z: impossible)
      possible.erase(z);

    if (possible.empty()) return out;

    auto cand = _sortedVec(possible);

    if (noDiPath) {
      // pyagrum: yield each singleton
      for (auto n: cand) {
        NodeSet Z;
        Z.insert(n);
        out.push_back(Z);
        if (stopAtFirst) return out;
      }
    } else {
      const std::size_t N    = cand.size();
      const std::size_t Kmax = (max_cardinality == 0) ? N : std::min(max_cardinality, N);
      for (std::size_t k = 1; k <= Kmax; ++k) {
        std::vector< bool > pick(N, false);
        std::fill_n(pick.begin(), k, true);
        do {
          NodeSet Z;
          for (std::size_t i = 0; i < N; ++i)
            if (pick[i]) Z.insert(cand[i]);
          // (FD-1): must block all directed X->Y paths
          if (!existsUnblockedDirectedPath(dag, X, Y, Z)) {
            if (!only_minimal || _isMinimalFrontdoorAdjustment(dag, X, Y, Z)) {
              out.push_back(Z);
              if (stopAtFirst) return out;
            }
          }
        } while (std::prev_permutation(pick.begin(), pick.end()));
      }
    }

    std::sort(out.begin(), out.end(), _lexLess);
    out.erase(std::unique(out.begin(),
                          out.end(),
                          [](const NodeSet& a, const NodeSet& b) {
                            return _sortedVec(a) == _sortedVec(b);
                          }),
              out.end());
    return out;
  }

  DoorCriteria::NodeSetVec DoorCriteria::enumerateFrontdoorSets(const DAG& dag,
                                                                NodeId     X,
                                                                NodeId     Y,
                                                                bool       stopAtFirst) {
    return enumerateFrontdoorSets(dag, X, Y, NodeSet{}, 0, true, stopAtFirst);
  }

  /* ============================== Utilities =============================== */

  bool DoorCriteria::existsUnblockedDirectedPath(const DAG&     dag,
                                                 NodeId         X,
                                                 NodeId         Y,
                                                 const NodeSet& Z) {
    // DFS over directed edges; internal nodes blocked by membership in Z.
    if (dag.existsArc(X, Y)) return true;

    for (auto c: dag.children(X)) {
      if (!Z.contains(c) && existsUnblockedDirectedPath(dag, c, Y, Z)) return true;
    }
    return false;
  }

  NodeSet DoorCriteria::nodesOnDirectedPaths(const DAG& dag, NodeId X, NodeId Y) {
    const NodeSet f = dag.descendants(X);   // forward reach
    const NodeSet r = dag.ancestors(Y);     // reverse reach

    NodeSet res;
    // A node is on some X->...->Y path iff reachable from X and can reach Y.
    for (auto n: f)
      if (r.contains(n)) res.insert(n);

    // Include endpoints if connected (i.e., Y is forward-reachable from X).
    if (f.contains(Y)) {
      res.insert(X);
      res.insert(Y);
    }

    return res;
  }

  // pyagrum.backdoor_reach inner function:
  // inner_br(bn, x, pht, reach0, reach1)
  // - always explore children into reach1
  // - explore parents into reach0 only if pht == false
  static void _inner_br(const DAG& dag, NodeId x, bool pht, NodeSet& reach0, NodeSet& reach1) {
    // children phase
    for (auto c: dag.children(x)) {
      if (!reach0.contains(c) && !reach1.contains(c)) {
        reach1.insert(c);
        _inner_br(dag, c, true, reach0, reach1);   // after child step, forbid next parent step
      }
    }
    // parents phase (only if we did not just take a parent step)
    if (!pht) {
      for (auto p: dag.parents(x)) {
        if (!reach0.contains(p)) {
          reach0.insert(p);
          _inner_br(dag, p, false, reach0, reach1);
        }
      }
    }
  }

  NodeSet DoorCriteria::backdoorReach(const DAG& dag, NodeId a) {
    NodeSet reach0;   // via a parent step
    NodeSet reach1;   // via a child step

    // Seed with 'a' to avoid parent->a->child zig-zag artifacts
    reach0.insert(a);
    reach1.insert(a);

    // parents of X are themselves in backdoor reach (X <- parent)
    for (auto pa: dag.parents(a))
      reach0.insert(pa);

    // Start the alternating traversal from each parent
    for (auto pa: dag.parents(a)) {
      _inner_br(dag, pa, /*pht=*/false, reach0, reach1);
    }

    NodeSet s = reach0;
    for (auto n: reach1)
      s.insert(n);
    s.erase(a);
    return s;
  }

  bool DoorCriteria::hasBackdoorPath(const DAG&     dag,
                                     NodeId         X,
                                     NodeId         Y,
                                     const NodeSet& Z) {
    NodeSet Xs;
    Xs.insert(X);
    NodeSet Ys;
    Ys.insert(Y);
    return !Separation::isBackdoorSeparated(dag, Xs, Ys, Z);
  }

  /* ============================= Minimality =============================== */

  bool DoorCriteria::_isMinimalBackdoorAdjustment(const DAG&     dag,
                                                  NodeId         X,
                                                  NodeId         Y,
                                                  const NodeSet& Z) {
    for (auto z: Z) {
      NodeSet sub = Z;
      sub.erase(z);
      if (satisfiesBackdoorCriterion(dag, X, Y, sub)) return false;
    }
    return true;
  }

  bool DoorCriteria::_isMinimalFrontdoorAdjustment(const DAG&     dag,
                                                   NodeId         X,
                                                   NodeId         Y,
                                                   const NodeSet& Z) {
    if (Z.empty()) return false;   // frontdoor sets must be non-empty
    for (auto z: Z) {
      NodeSet sub = Z;
      sub.erase(z);
      if (satisfiesFrontdoorCriterion(dag, X, Y, sub)) return false;
    }
    return true;
  }

}   // namespace gum
