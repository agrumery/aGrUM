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
 * @brief Door criteria (backdoor/frontdoor) utilities bound to a single DAG.
 */

#include <agrum/CM/tools/doorCriteria.h>

#include <algorithm>
#include <vector>

#include <agrum/CM/tools/separation.h>

namespace gum {

namespace {
// Convert NodeSet to sorted std::vector<NodeId> for deterministic ops.
static std::vector<NodeId> _sortedVec(const NodeSet& s) {
  std::vector<NodeId> v;
  v.reserve(s.size());
  for (auto n : s) v.push_back(n);
  std::sort(v.begin(), v.end());
  return v;
}

// Deterministic lexicographic comparator for NodeSet via their sorted vectors.
static bool _lexLess(const NodeSet& a, const NodeSet& b) {
  auto va = _sortedVec(a);
  auto vb = _sortedVec(b);
  return va < vb;
}
} // namespace

/* ========================= Backdoor / Frontdoor ========================= */

bool DoorCriteria::satisfiesBackdoorCriterion(NodeId X, NodeId Y, const NodeSet& Z) const {
  // Rule-out illegal contents up front:
  // - Z must not contain X (conditioning on X is not a valid adjustment set)
  // - Z must not contain Y (keep parity with enumeration/universe)
  if (Z.contains(X) || Z.contains(Y)) return false;

  // - Z must not contain any descendant of X
  NodeSet descX = _dag.descendants(X);
  for (auto dx : descX) {
    if (Z.contains(dx)) return false;
  }

  // Backdoor blocking condition in G_{underline X}
  NodeSet Xs; Xs.insert(X);
  NodeSet Ys; Ys.insert(Y);
  return Separation::isBackdoorSeparated(_dag, Xs, Ys, Z);
}

bool DoorCriteria::satisfiesFrontdoorCriterion(NodeId X, NodeId Y, const NodeSet& Z) const {
  if (Z.empty()) return false;

  // (FD-1) Z must intercept all directed X->...->Y paths
  if (existsUnblockedDirectedPath(X, Y, Z)) return false;

  // (FD-2) Z ∩ backdoor_reach(X) = ∅
  NodeSet br = backdoorReach(X);
  for (auto z : Z) if (br.contains(z)) return false;

  // (FD-3) check on a d-separation reduced graph with interest = Z ∪ {X,Y}
  NodeSet Xset; Xset.insert(X);
  NodeSet Yset; Yset.insert(Y);
  // interest = X ∪ Y ∪ Z  -> pass Xset, Yset, Z as-is
  DAG reduced = Separation::reduceForDSeparation(_dag, Xset, Yset, Z);

  NodeSet condX; condX.insert(X);
  for (auto z : Z) {
    NodeSet Zs; Zs.insert(z);
    // backdoor_path(reduced, z, y, {x}) == !isBackdoorSeparated(reduced, {z}, {y}, {x})
    if (!Separation::isBackdoorSeparated(reduced, Zs, Yset, condX)) return false;
  }
  return true;
}




DoorCriteria::NodeSetVec
DoorCriteria::enumerateBackdoorSets(NodeId X, NodeId Y, const EnumerationOptions& opts, bool stopAtFirst) const {
  NodeSetVec out;

  // Theoretical variant: if X has no parents, {} is a valid backdoor set.
  if (_dag.parents(X).empty()) { out.emplace_back(); return out; }

  // If Y is a parent of X, there is an open backdoor X<-Y that {} cannot block.
  if (_dag.existsArc(Y, X)) return out;

  // Reduce around interest = {X, Y}; evidence = ∅
  NodeSet Xset; Xset.insert(X);
  NodeSet Yset; Yset.insert(Y);
  NodeSet Zempty;
  DAG G = Separation::reduceForDSeparation(_dag, Xset, Yset, Zempty);

  // Candidate pool = nodes(G) \ (Desc(X) ∪ {X,Y} ∪ opts.exclude)
  NodeSet descX = _dag.descendants(X); // Python uses descendants(bn, cause)
  NodeSet possible;
  for (auto n : G.nodes()) {
    if (n == X || n == Y) continue;
    if (opts.excluded_nodes.contains(n)) continue;
    if (descX.contains(n)) continue;
    possible.insert(n);
  }
  if (possible.empty()) return out;

  auto cand = _sortedVec(possible);
  const std::size_t N    = cand.size();
  const std::size_t Kmax = (opts.max_cardinality == 0) ? N : std::min(opts.max_cardinality, N);

  // Only prune supersets in minimal_only mode
  NodeSetVec chosen;
  const bool prune_supersets = opts.only_minimal;

  NodeSet Xs; Xs.insert(X);
  NodeSet Ys; Ys.insert(Y);

  // pyagrum tries k >= 1 (never yields {} except our explicit no-parent case above)
  for (std::size_t k = 1; k <= Kmax; ++k) {
    std::vector<bool> pick(N, false);
    std::fill(pick.begin(), pick.begin() + k, true);
    do {
      NodeSet Z;
      for (std::size_t i = 0; i < N; ++i) if (pick[i]) Z.insert(cand[i]);

      if (prune_supersets) {
        bool worth = true;
        for (const auto& s : chosen) {
          bool subset = true;
          for (auto u : s) if (!Z.contains(u)) { subset = false; break; }
          if (subset) { worth = false; break; }
        }
        if (!worth) continue;
      }

      if (Separation::isBackdoorSeparated(G, Xs, Ys, Z)) {
        if (opts.only_minimal) {
          chosen.push_back(Z);
          out.push_back(Z);
        } else {
          out.push_back(Z); // keep non-minimal supersets too (e.g., {U,W})
        }
        if (stopAtFirst) return out;
      }

    } while (std::prev_permutation(pick.begin(), pick.end()));
  }

  // Deterministic order + dedup
  std::sort(out.begin(), out.end(), _lexLess);
  out.erase(std::unique(out.begin(), out.end(),
                        [](const NodeSet& a, const NodeSet& b){
                          return _sortedVec(a) == _sortedVec(b);
                        }),
            out.end());
  return out;
}



DoorCriteria::NodeSetVec
DoorCriteria::enumerateBackdoorSets(NodeId X, NodeId Y, bool stopAtFirst) const {
  EnumerationOptions opts;
  return enumerateBackdoorSets(X, Y, opts, stopAtFirst);
}

// Backward compatibility: default stopAtFirst = false
DoorCriteria::NodeSetVec
DoorCriteria::enumerateBackdoorSets(NodeId X, NodeId Y) const {
  return enumerateBackdoorSets(X, Y, false);
}


DoorCriteria::NodeSetVec
DoorCriteria::enumerateFrontdoorSets(NodeId X, NodeId Y, const EnumerationOptions& opts, bool stopAtFirst) const {
  NodeSetVec out;

  // pyagrum early exit: if X is a parent of Y, yield nothing
  for (auto p : _dag.parents(Y)) if (p == X) return out;

  // Candidate pool
  NodeSet possible = nodesOnDirectedPaths(X, Y);
  bool noDiPath = false;

  if (possible.empty()) {
    // No directed path: use weakly connected component containing both X and Y (undirected BFS)
    NodeSet cc;
    {
      std::vector<NodeId> st; st.push_back(X);
      cc.insert(X);
      while (!st.empty()) {
        NodeId u = st.back(); st.pop_back();
        for (auto v : _dag.parents(u)) if (!cc.contains(v)) { cc.insert(v); st.push_back(v); }
        for (auto v : _dag.children(u)) if (!cc.contains(v)) { cc.insert(v); st.push_back(v); }
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
  NodeSet br = backdoorReach(X);
  for (auto n : br)         possible.erase(n);
  for (auto n : opts.excluded_nodes) possible.erase(n);

  // Remove "impossible" z with a backdoor to Y given X, on a reduced graph:
  // interest = {X, Y} ∪ possible; evidence = ∅ (parity with pyagrum)
  NodeSet Xset; Xset.insert(X);
  NodeSet Yset = possible; Yset.insert(Y);
  NodeSet Zempty;

  DAG g = Separation::reduceForDSeparation(_dag, Xset, Yset, Zempty);

  NodeSet impossible;
  NodeSet condX; condX.insert(X);
  for (auto z : possible) {
    NodeSet Zs; Zs.insert(z);
    if (!Separation::isBackdoorSeparated(g, Zs, NodeSet{Y}, condX)) {
      impossible.insert(z);
    }
  }
  for (auto z : impossible) possible.erase(z);

  if (possible.empty()) return out;

  auto cand = _sortedVec(possible);

  if (noDiPath) {
    // pyagrum: yield each singleton
    for (auto n : cand) {
      NodeSet Z; Z.insert(n); out.push_back(Z);
      if (stopAtFirst) return out;
    }
  } else {
    const std::size_t N    = cand.size();
    const std::size_t Kmax = (opts.max_cardinality == 0) ? N : std::min(opts.max_cardinality, N);
    for (std::size_t k = 1; k <= Kmax; ++k) {
      std::vector<bool> pick(N, false);
      std::fill(pick.begin(), pick.begin() + k, true);
      do {
        NodeSet Z;
        for (std::size_t i = 0; i < N; ++i) if (pick[i]) Z.insert(cand[i]);
        // (FD-1): must block all directed X->Y paths
        if (!existsUnblockedDirectedPath(X, Y, Z)) {
          if (!opts.only_minimal || _isMinimalFrontdoorAdjustment(X, Y, Z)) {
            out.push_back(Z);
            if (stopAtFirst) return out;
          }
        }
      } while (std::prev_permutation(pick.begin(), pick.end()));
    }
  }

  std::sort(out.begin(), out.end(), _lexLess);
  out.erase(std::unique(out.begin(), out.end(),
                        [](const NodeSet& a, const NodeSet& b){ return _sortedVec(a) == _sortedVec(b); }),
            out.end());
  return out;
}



DoorCriteria::NodeSetVec
DoorCriteria::enumerateFrontdoorSets(NodeId X, NodeId Y, bool stopAtFirst) const {
  EnumerationOptions opts;
  return enumerateFrontdoorSets(X, Y, opts, stopAtFirst);
}

// Backward compatibility: default stopAtFirst = false
DoorCriteria::NodeSetVec
DoorCriteria::enumerateFrontdoorSets(NodeId X, NodeId Y) const {
  return enumerateFrontdoorSets(X, Y, false);
}

/* ============================== Utilities =============================== */

bool DoorCriteria::existsUnblockedDirectedPath(NodeId X, NodeId Y, const NodeSet& Z) const {
  // DFS over directed edges; internal nodes blocked by membership in Z.
  if (_dag.existsArc(X, Y)) return true;

  for (auto c : _dag.children(X)) {
    if (!Z.contains(c) && existsUnblockedDirectedPath(c, Y, Z)) return true;
  }
  return false;
}

NodeSet DoorCriteria::nodesOnDirectedPaths(NodeId X, NodeId Y) const {
  NodeSet f = _dag.descendants(X); // forward reach
  NodeSet r = _dag.ancestors(Y);  // reverse reach

  NodeSet res;
  // A node is on some X->...->Y path iff reachable from X and can reach Y.
  for (auto n : f) if (r.contains(n)) res.insert(n);

  // Include endpoints if connected (i.e., Y is forward-reachable from X).
  if (f.contains(Y)) { res.insert(X); res.insert(Y); }

  return res;
}

// pyagrum.backdoor_reach inner function:
// inner_br(bn, x, pht, reach0, reach1)
// - always explore children into reach1
// - explore parents into reach0 only if pht == false
static void _inner_br(const DAG& dag, NodeId x, bool pht,
                      NodeSet& reach0, NodeSet& reach1) {
  // children phase
  for (auto c : dag.children(x)) {
    if (!reach0.contains(c) && !reach1.contains(c)) {
      reach1.insert(c);
      _inner_br(dag, c, true, reach0, reach1);  // after child step, forbid next parent step
    }
  }
  // parents phase (only if we did not just take a parent step)
  if (!pht) {
    for (auto p : dag.parents(x)) {
      if (!reach0.contains(p)) {
        reach0.insert(p);
        _inner_br(dag, p, false, reach0, reach1);
      }
    }
  }
}



NodeSet DoorCriteria::backdoorReach(NodeId a) const {
  NodeSet reach0; // via a parent step
  NodeSet reach1; // via a child step

  // Seed with 'a' to avoid parent->a->child zig-zag artifacts
  reach0.insert(a);
  reach1.insert(a);

  // parents of X are themselves in backdoor reach (X <- parent)
  for (auto pa : _dag.parents(a)) reach0.insert(pa);

  // Start the alternating traversal from each parent
  for (auto pa : _dag.parents(a)) {
    _inner_br(_dag, pa, /*pht=*/false, reach0, reach1);
  }

  NodeSet s = reach0;
  for (auto n : reach1) s.insert(n);
  s.erase(a);
  return s;
}


bool DoorCriteria::hasBackdoorPath(NodeId X, NodeId Y, const NodeSet& Z) const {
  NodeSet Xs; Xs.insert(X);
  NodeSet Ys; Ys.insert(Y);
  return !Separation::isBackdoorSeparated(_dag, Xs, Ys, Z);
}

/* ============================= Minimality =============================== */

bool DoorCriteria::_isMinimalBackdoorAdjustment(NodeId X, NodeId Y, const NodeSet& Z) const {
  for (auto z : Z) {
    NodeSet sub = Z; sub.erase(z);
    if (satisfiesBackdoorCriterion(X, Y, sub)) return false;
  }
  return true;
}

bool DoorCriteria::_isMinimalFrontdoorAdjustment(NodeId X, NodeId Y, const NodeSet& Z) const {
  if (Z.empty()) return false; // frontdoor sets must be non-empty
  for (auto z : Z) {
    NodeSet sub = Z; sub.erase(z);
    if (satisfiesFrontdoorCriterion(X, Y, sub)) return false;
  }
  return true;
}

} // namespace gum
