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


#include <algorithm>
#include <numeric>
#include <queue>
#include <set>
#include <vector>

#include <agrum/BN/learning/FCI.h>

namespace gum::learning {

    // ##########################################################################
    // Constructors / Destructors
    // ##########################################################################

    FCI::FCI()  = default;
    FCI::~FCI() = default;


    FCI::FCI(FCI&& from) noexcept :
        CIBasedLearning(std::move(from)), maxPathLength_(from.maxPathLength_) {}

    FCI& FCI::operator=(const FCI& from) {
      if (this != &from) {
        CIBasedLearning::operator=(from);
        maxPathLength_ = from.maxPathLength_;
      }
      return *this;
    }

    FCI& FCI::operator=(FCI&& from) noexcept {
      if (this != &from) {
        CIBasedLearning::operator=(std::move(from));
        maxPathLength_ = from.maxPathLength_;
      }
      return *this;
    }

    // ##########################################################################
    // Parameterisation
    // ##########################################################################

    void FCI::setMaxPathLength(Size maxLen) { maxPathLength_ = maxLen; }

    Size FCI::maxPathLength() const { return maxPathLength_; }

    std::vector< NodeId > FCI::possibleDSep(const PAG& pag, NodeId x, NodeId y) const {
      return computePossibleDSep_(pag, x, y);
    }

    // ##########################################################################
    // Conflict hook override — FCI leaves circle marks unchanged
    // ##########################################################################

    void FCI::resolveOrientConflict_(NodeId /*src*/, NodeId /*dst*/) {
      // no-op: circle marks remain; only final ↔ edges (Phase 6) become _latentCouples_
    }

    // ##########################################################################
    // Phase 2 / Phase 4 — PAG collider orientation
    // ##########################################################################

    void FCI::orientCollidersOnPAG_(PAG& pag, const MixedGraph& topology) {
      const auto triples = unshieldedTriples_(topology);

      const auto isCollider = [&](NodeId X, NodeId Y, NodeId Z) -> bool {
        if (!sepSet_.exists({X, Y})) { return false; }
        const SepSetEntry_& entry = sepSet_[{X, Y}];
        return std::ranges::find(entry.cond, Z) == entry.cond.end();
      };

      if (ucPriority_ == UCPriority::Standard) {
        for (const ThreePoints& tp: triples) {
          const NodeId X = std::get< 0 >(tp);
          const NodeId Y = std::get< 1 >(tp);
          const NodeId Z = std::get< 2 >(tp);
          if (!isCollider(X, Y, Z)) { continue; }
          if (pag.existsEdge(X, Z) && !isForbiddenArc_(X, Z)) {
            pag.setMarkAt(X, Z, EdgeMark::Arrowhead);
          }
          if (pag.existsEdge(Y, Z) && !isForbiddenArc_(Y, Z)) {
            pag.setMarkAt(Y, Z, EdgeMark::Arrowhead);
          }
        }
      } else {
        struct Candidate {
          NodeId X, Y, Z;
          double pval;
        };

        std::vector< Candidate > candidates;
        candidates.reserve(triples.size());

        for (const ThreePoints& tp: triples) {
          const NodeId X = std::get< 0 >(tp);
          const NodeId Y = std::get< 1 >(tp);
          const NodeId Z = std::get< 2 >(tp);
          if (!isCollider(X, Y, Z)) { continue; }
          candidates.push_back({.X = X, .Y = Y, .Z = Z, .pval = sepSet_[{X, Y}].pval});
        }

        std::ranges::sort(candidates, [](const Candidate& a, const Candidate& b) {
          return a.pval > b.pval;
        });

        for (const auto& [X, Y, Z, pval]: candidates) {
          if (pag.existsEdge(X, Z) && !isForbiddenArc_(X, Z)) {
            pag.setMarkAt(X, Z, EdgeMark::Arrowhead);
          }
          if (pag.existsEdge(Y, Z) && !isForbiddenArc_(Y, Z)) {
            pag.setMarkAt(Y, Z, EdgeMark::Arrowhead);
          }
        }
      }
    }

    // ##########################################################################
    // Path helpers (anonymous namespace) — placed before possibleDSep helpers
    // so that computePossibleDSep_ can call existsSemiDirectedPath_.
    // ##########################################################################

    namespace {
      // Semi-directed path BFS: traverse edge X→W when no arrowhead at X from W.
      bool existsSemiDirectedPath_(const PAG& pag, NodeId from, NodeId to) {
        std::set< NodeId >   visited{from};
        std::queue< NodeId > q;
        q.push(from);
        while (!q.empty()) {
          const NodeId cur = q.front();
          q.pop();
          if (cur == to) { return true; }
          for (const NodeId w: pag.neighbours(cur)) {
            if (visited.count(w) > 0) { continue; }
            if (!pag.isArrowhead(w, cur)) {   // endpoint at cur from w ≠ arrowhead
              visited.insert(w);
              q.push(w);
            }
          }
        }
        return false;
      }

      // DFS backbone for existsUncoveredPdPath_.
      bool uncoveredPdDFS_(const PAG&             pag,
                           NodeId                 to,
                           std::vector< NodeId >& path,
                           std::set< NodeId >&    visited) {
        const NodeId cur = path.back();
        if (cur == to) {
          for (std::size_t i = 0; i + 2 < path.size(); ++i) {
            if (pag.existsEdge(path[i], path[i + 2])) { return false; }
          }
          return true;
        }
        for (const NodeId w: pag.neighbours(cur)) {
          if (visited.count(w) > 0) { continue; }
          if (pag.isArrowhead(w, cur)) { continue; }   // endpoint at cur from w is arrowhead
          if (pag.isTail(cur, w)) { continue; }        // endpoint at w from cur is tail
          visited.insert(w);
          path.push_back(w);
          if (uncoveredPdDFS_(pag, to, path, visited)) { return true; }
          path.pop_back();
          visited.erase(w);
        }
        return false;
      }

      // Exists uncovered possibly-directed path from→next→…→to.
      bool existsUncoveredPdPath_(const PAG& pag, NodeId from, NodeId next, NodeId to) {
        std::set< NodeId >    visited{from, next};
        std::vector< NodeId > path{from, next};
        return uncoveredPdDFS_(pag, to, path, visited);
      }

      // DFS collecting all uncovered Circle-Circle paths from path.back() to `to`.
      // excludes: nodes not allowed on path (typically {A, B}).
      // Returns all paths via `result`; each path includes the start node.
      void findUncoveredCirclePaths_(const PAG&                            pag,
                                     NodeId                                to,
                                     std::vector< NodeId >&                path,
                                     std::set< NodeId >&                   visited,
                                     const std::vector< NodeId >&          excludes,
                                     std::vector< std::vector< NodeId > >& result) {
        const NodeId cur = path.back();
        if (cur == to) {
          bool uncovered = true;
          for (std::size_t i = 0; i + 2 < path.size() && uncovered; ++i) {
            if (pag.existsEdge(path[i], path[i + 2])) { uncovered = false; }
          }
          if (uncovered) { result.push_back(path); }
          return;
        }
        for (const NodeId next: pag.neighbours(cur)) {
          if (visited.count(next) > 0) { continue; }
          if (std::ranges::find(excludes, next) != excludes.end()) { continue; }
          if (!pag.isCircle(cur, next) || !pag.isCircle(next, cur)) { continue; }
          visited.insert(next);
          path.push_back(next);
          findUncoveredCirclePaths_(pag, to, path, visited, excludes, result);
          path.pop_back();
          visited.erase(next);
        }
      }
    }   // anonymous namespace

    // ##########################################################################
    // Phase 3 — possibleDSep helpers
    // ##########################################################################

    std::vector< NodeId > FCI::computePossibleDSep_(const PAG& pag, NodeId x, NodeId y) const {
      std::set< NodeId >                        result;
      std::queue< std::pair< NodeId, NodeId > > bfsQueue;
      std::set< std::pair< NodeId, NodeId > >   visited;

      for (const NodeId b: pag.neighbours(x)) {
        if (b == y) { continue; }
        if (visited.insert({x, b}).second) {
          bfsQueue.emplace(x, b);
          result.insert(b);
        }
      }

      while (!bfsQueue.empty()) {
        const auto [a, b] = bfsQueue.front();
        bfsQueue.pop();

        for (const NodeId c: pag.neighbours(b)) {
          if (c == x || c == y || c == a) { continue; }
          // expand if B is a definite collider on (A, B, C) or A and C are adjacent
          // (Zhang 2008 / Colombo et al. criterion)
          if (pag.isDefCollider(a, b, c) || pag.existsEdge(a, c)) {
            if (visited.insert({b, c}).second) {
              bfsQueue.emplace(b, c);
              // FIX #3: only add c to PossibleDSep if it has a semi-directed path
              // back toward x or b (Zhang 2008 §3.2 filter).
              if (existsSemiDirectedPath_(pag, c, x) || existsSemiDirectedPath_(pag, c, b)) {
                result.insert(c);
              }
            }
          }
        }
      }

      result.erase(x);
      result.erase(y);
      return {result.begin(), result.end()};
    }

    void FCI::possibleDSepPhase_(PAG& pag) {
      // snapshot edges to avoid invalidation during removal
      const std::vector< Edge > edgeSnap(pag.edges().begin(), pag.edges().end());

      const auto tryRemove = [&](NodeId X, NodeId Y, const std::vector< NodeId >& dsep) {
        const Size maxK = (maxCondSetSize_ == Size(-1))
                            ? static_cast< Size >(dsep.size())
                            : std::min(maxCondSetSize_, static_cast< Size >(dsep.size()));

        // FIX #4: start at k=2; sizes 0 and 1 were already exhaustively tested
        // in Phase 1 (FAS skeleton discovery).
        for (Size k = 2; k <= maxK; ++k) {
          if (dsep.size() < k) { break; }

          std::vector< std::size_t > idx(k);
          std::iota(idx.begin(), idx.end(), std::size_t(0));

          for (;;) {
            std::vector< NodeId > cond(k);
            for (Size i = 0; i < k; ++i) {
              cond[i] = dsep[idx[i]];
            }

            const double pval = test_->statistics(X, Y, cond).second;
            if (pval > alpha_) {
              pag.eraseEdge(Edge(X, Y));
              sepSet_.set({X, Y}, SepSetEntry_{.cond = cond, .pval = pval});
              sepSet_.set({Y, X}, SepSetEntry_{.cond = cond, .pval = pval});
              return true;
            }

            if (k == 0) { break; }
            int i = static_cast< int >(k) - 1;
            while (i >= 0
                   && idx[static_cast< std::size_t >(i)]
                          == dsep.size() - k + static_cast< std::size_t >(i)) {
              --i;
            }
            if (i < 0) { break; }
            ++idx[static_cast< std::size_t >(i)];
            for (int j = i + 1; j < static_cast< int >(k); ++j) {
              idx[static_cast< std::size_t >(j)] = idx[static_cast< std::size_t >(j - 1)] + 1;
            }
          }
        }
        return false;
      };

      for (const Edge& edge: edgeSnap) {
        if (!pag.existsEdge(edge)) { continue; }
        const NodeId X = edge.first();
        const NodeId Y = edge.second();

        const auto dsepXY = computePossibleDSep_(pag, X, Y);
        if (tryRemove(X, Y, dsepXY)) { continue; }

        if (!pag.existsEdge(edge)) { continue; }
        const auto dsepYX = computePossibleDSep_(pag, Y, X);
        tryRemove(X, Y, dsepYX);
      }
    }

    // ##########################################################################
    // Phase 5 — orientation rules
    // ##########################################################################

    // R1: Away from Collider.  A *→ B o-* C, A not adj C  →  B → C
    bool FCI::ruleR1_(PAG& pag) const {
      bool changed = false;
      for (const NodeId b: pag.nodes()) {
        for (const NodeId a: pag.neighbours(b)) {
          if (!pag.isArrowhead(a, b)) { continue; }
          for (const NodeId c: pag.neighbours(b)) {
            if (c == a) { continue; }
            if (!pag.isCircle(c, b)) { continue; }
            if (pag.existsEdge(a, c)) { continue; }
            if (isForbiddenArc_(b, c)) { continue; }
            // FIX #1: do not overwrite an existing Tail at C (from B) with Arrowhead.
            // marks_[Arc(B,C)] == Tail means B *— C is already established;
            // upgrading it to B *→ C would contradict that orientation.
            if (pag.isTail(b, c)) { continue; }
            pag.setMarkAt(c, b, EdgeMark::Tail);
            pag.setMarkAt(b, c, EdgeMark::Arrowhead);
            changed = true;
          }
        }
      }
      return changed;
    }

    // R2: Away from Cycle.  A *→ B *→ C, A *-o C, A→B or B→C  →  A *→ C
    bool FCI::ruleR2_(PAG& pag) const {
      bool changed = false;
      for (const NodeId b: pag.nodes()) {
        for (const NodeId a: pag.neighbours(b)) {
          for (const NodeId c: pag.neighbours(b)) {
            if (a == c) { continue; }
            if (!pag.existsEdge(a, c)) { continue; }
            if (!pag.isCircle(a, c)) { continue; }      // circle at c from a
            if (!pag.isArrowhead(a, b)) { continue; }   // a *→ b
            if (!pag.isArrowhead(b, c)) { continue; }   // b *→ c
            if (!pag.isTail(b, a) && !pag.isTail(c, b)) { continue; }
            if (isForbiddenArc_(a, c)) { continue; }
            // FIX #1 (defensive): isCircle(a,c) already ensures the mark at C
            // from A is Circle, so isTail cannot be true here. Added for symmetry
            // with R1 and to make the guard explicit.
            if (pag.isTail(a, c)) { continue; }
            pag.setMarkAt(a, c, EdgeMark::Arrowhead);
            changed = true;
          }
        }
      }
      return changed;
    }

    // R3: Double Triangle.
    // A *→ B ←* C (A,C non-adj), D o-* B, A adj D, C adj D, D non-collider(A,D,C),
    // circle at D from A and C  →  D *→ B
    bool FCI::ruleR3_(PAG& pag) const {
      bool changed = false;
      for (const NodeId b: pag.nodes()) {
        std::vector< NodeId > into_arrows, into_circles;
        for (const NodeId n: pag.neighbours(b)) {
          if (pag.isArrowhead(n, b)) { into_arrows.push_back(n); }
          if (pag.isCircle(n, b)) { into_circles.push_back(n); }
        }
        if (into_arrows.size() < 2) { continue; }

        for (const NodeId d: into_circles) {
          for (std::size_t i = 0; i < into_arrows.size(); ++i) {
            for (std::size_t j = i + 1; j < into_arrows.size(); ++j) {
              const NodeId a = into_arrows[i];
              const NodeId c = into_arrows[j];
              if (pag.existsEdge(a, c)) { continue; }   // A, C not adjacent
              if (!pag.existsEdge(a, d)) { continue; }
              if (!pag.existsEdge(c, d)) { continue; }
              if (!pag.isCircle(a, d)) { continue; }    // circle at d from a
              if (!pag.isCircle(c, d)) { continue; }    // circle at d from c
              // D is non-collider on A-D-C: D ∈ sepSet(A,C)
              bool nonCollider = false;
              for (const auto& key: {std::make_pair(a, c), std::make_pair(c, a)}) {
                if (sepSet_.exists(key)) {
                  const auto& cond = sepSet_[key].cond;
                  if (std::ranges::find(cond, d) != cond.end()) {
                    nonCollider = true;
                    break;
                  }
                }
              }
              if (!nonCollider) { continue; }
              pag.setMarkAt(d, b, EdgeMark::Arrowhead);
              changed = true;
            }
          }
        }
      }
      return changed;
    }

    // R5: Uncovered Circle Path.
    // A o—o B, uncovered circle path A o—o C ... D o—o B (B not adj C, A not adj D)
    // → orient A—B and all path edges as Tail-Tail
    bool FCI::ruleR5_(PAG& pag) const {
      bool changed = false;
      for (const NodeId b: pag.nodes()) {
        for (const NodeId a: pag.neighbours(b)) {
          if (!pag.isCircle(a, b) || !pag.isCircle(b, a)) { continue; }
          for (const NodeId c: pag.neighbours(a)) {
            if (c == b) { continue; }
            if (pag.existsEdge(b, c)) { continue; }   // B not adj C
            if (!pag.isCircle(a, c) || !pag.isCircle(c, a)) { continue; }
            for (const NodeId d: pag.neighbours(b)) {
              if (d == a || d == c) { continue; }
              if (pag.existsEdge(a, d)) { continue; }   // A not adj D
              if (!pag.isCircle(b, d) || !pag.isCircle(d, b)) { continue; }
              const std::vector< NodeId >          excludes{a, b};
              std::vector< std::vector< NodeId > > paths;
              std::vector< NodeId >                path{c};
              std::set< NodeId >                   visited{c};
              findUncoveredCirclePaths_(pag, d, path, visited, excludes, paths);
              for (const auto& p: paths) {
                changed = true;
                pag.setMarkAt(b, a, EdgeMark::Tail);
                pag.setMarkAt(a, b, EdgeMark::Tail);
                pag.setMarkAt(c, a, EdgeMark::Tail);
                pag.setMarkAt(a, c, EdgeMark::Tail);
                pag.setMarkAt(b, p.back(), EdgeMark::Tail);
                pag.setMarkAt(p.back(), b, EdgeMark::Tail);
                for (std::size_t k = 0; k + 1 < p.size(); ++k) {
                  pag.setMarkAt(p[k + 1], p[k], EdgeMark::Tail);
                  pag.setMarkAt(p[k], p[k + 1], EdgeMark::Tail);
                }
              }
            }
          }
        }
      }
      return changed;
    }

    // R6: Tail Propagation from undirected edge.
    // A—B (Tail-Tail) exists, C o-* B → Tail at B from C
    bool FCI::ruleR6_(PAG& pag) const {
      bool changed = false;
      for (const NodeId b: pag.nodes()) {
        bool has_undirected = false;
        for (const NodeId a: pag.neighbours(b)) {
          if (pag.isTail(a, b) && pag.isTail(b, a)) {
            has_undirected = true;
            break;
          }
        }
        if (!has_undirected) { continue; }
        for (const NodeId c: pag.neighbours(b)) {
          if (pag.isCircle(c, b)) {
            pag.setMarkAt(c, b, EdgeMark::Tail);
            changed = true;
          }
        }
      }
      return changed;
    }

    // R7: Tail Propagation from A-oB.
    // A -o B (circle at B from A, tail at A from B), C o-* B (C not adj A) → Tail at B from C
    bool FCI::ruleR7_(PAG& pag) const {
      bool changed = false;
      for (const NodeId b: pag.nodes()) {
        std::vector< NodeId > a_list;
        for (const NodeId a: pag.neighbours(b)) {
          if (pag.isCircle(a, b) && pag.isTail(b, a)) { a_list.push_back(a); }
        }
        if (a_list.empty()) { continue; }
        for (const NodeId c: pag.neighbours(b)) {
          if (!pag.isCircle(c, b)) { continue; }
          for (const NodeId a: a_list) {
            if (a == c) { continue; }
            if (!pag.existsEdge(a, c)) {
              pag.setMarkAt(c, b, EdgeMark::Tail);
              changed = true;
              break;
            }
          }
        }
      }
      return changed;
    }

    // ##########################################################################
    // R4 — Discriminating Path
    // ##########################################################################

    bool FCI::doDdpOrientation_(PAG&                               pag,
                                NodeId                             d,
                                NodeId                             a,
                                NodeId                             b,
                                NodeId                             c,
                                const HashTable< NodeId, NodeId >& previous) const {
      // Reconstruct conditioning set: follow previous from d toward b
      std::vector< NodeId > path;
      NodeId                cur = d;
      while (previous.exists(cur)) {
        cur = previous[cur];
        path.push_back(cur);
      }
      // path = [previous[d], ..., b]

      const double pval1 = test_->statistics(d, c, path).second;
      const bool   ind1  = pval1 > alpha_;

      std::vector< NodeId > path_no_b;
      for (const NodeId n: path) {
        if (n != b) { path_no_b.push_back(n); }
      }
      const double pval2 = test_->statistics(d, c, path_no_b).second;
      const bool   ind2  = pval2 > alpha_;

      bool use_b_in_sep;
      if (!ind1 && !ind2) {
        // fall back to existing sepSet
        const bool has_dc = sepSet_.exists({d, c});
        const bool has_cd = sepSet_.exists({c, d});
        if (!has_dc && !has_cd) { return false; }
        const auto& cond = has_dc ? sepSet_[{d, c}].cond : sepSet_[{c, d}].cond;
        use_b_in_sep     = (std::ranges::find(cond, b) != cond.end());
      } else {
        use_b_in_sep = ind1;
      }

      if (use_b_in_sep) {
        pag.setMarkAt(c, b, EdgeMark::Tail);   // B→C: tail at B from C
        return true;
      }
      bool any = false;
      if (!isForbiddenArc_(a, b)) {
        pag.setMarkAt(a, b, EdgeMark::Arrowhead);
        any = true;
      }
      if (!isForbiddenArc_(c, b)) {
        pag.setMarkAt(c, b, EdgeMark::Arrowhead);
        any = true;
      }
      return any;
    }

    // R4: Discriminating path rule.
    // Outer: for each B, A where B*→A, C where C o-* B (circle at B from C):
    //   if A→C and B*→C: search discriminating path D…A,B,C
    bool FCI::ruleR4_(PAG& pag) const {
      bool changed = false;

      for (const NodeId b: pag.nodes()) {
        // possA: nodes A where B *→ A
        std::vector< NodeId > poss_a;
        for (const NodeId a: pag.neighbours(b)) {
          if (pag.isArrowhead(b, a)) { poss_a.push_back(a); }
        }
        // possC: nodes C where circle at B from C, AND B *→ C
        std::vector< NodeId > poss_c;
        for (const NodeId c: pag.neighbours(b)) {
          if (pag.isCircle(c, b) && pag.isArrowhead(b, c)) { poss_c.push_back(c); }
        }

        for (const NodeId a: poss_a) {
          for (const NodeId c: poss_c) {
            if (a == c) { continue; }
            if (!pag.existsEdge(a, c)) { continue; }
            if (!pag.isArrowhead(a, c) || !pag.isTail(c, a)) { continue; }   // A→C (parent)

            // BFS backward from A to find discriminating path D…A,B,C
            HashTable< NodeId, NodeId > previous;
            previous.insert(a, b);

            // parents of C: nodes with directed edge into C
            std::set< NodeId > c_parents;
            for (const NodeId n: pag.neighbours(c)) {
              if (pag.isArrowhead(n, c) && pag.isTail(c, n)) { c_parents.insert(n); }
            }

            std::queue< NodeId > q;
            std::set< NodeId >   visited{a, b};
            q.push(a);

            // FIX #2: track BFS depth to respect maxPathLength_.
            // cur_lvl_size counts nodes remaining at the current BFS level;
            // nxt_lvl_size accumulates nodes pushed for the next level.
            // distance is incremented each time we exhaust the current level.
            int    distance     = 0;
            Size   cur_lvl_size = 1;   // node_a is the sole node at level 0
            Size   nxt_lvl_size = 0;

            bool found = false;
            while (!q.empty() && !found) {
              const NodeId t = q.front();
              q.pop();

              const NodeId prev_t = previous[t];

              // nodes D with arrowhead INTO t
              for (const NodeId d: pag.neighbours(t)) {
                if (visited.count(d) > 0) { continue; }
                if (!pag.isArrowhead(d, t)) { continue; }
                // t must be def collider on (d, t, prev_t)
                if (!pag.isArrowhead(prev_t, t)) { continue; }

                previous.set(d, t);

                if (!pag.existsEdge(d, c) && d != c) {
                  // found discriminating path
                  if (doDdpOrientation_(pag, d, a, b, c, previous)) {
                    changed = true;
                    found   = true;
                    break;
                  }
                }
                if (c_parents.count(d) > 0) {
                  q.push(d);
                  visited.insert(d);
                  ++nxt_lvl_size;
                }
              }

              if (!found) {
                --cur_lvl_size;
                if (cur_lvl_size == 0) {
                  ++distance;
                  if (maxPathLength_ != Size(-1)
                      && static_cast< Size >(distance) > maxPathLength_) {
                    break;
                  }
                  cur_lvl_size = nxt_lvl_size;
                  nxt_lvl_size = 0;
                }
              }
            }
          }
        }
      }
      return changed;
    }

    // ##########################################################################
    // R8–R10 — Ancestor rules
    // ##########################################################################

    // R8: A→B*→C and A*→C (circle at A from C) → A→C (tail at A from C)
    // or: A-oB*→C and A*→C (circle at A from C) → A→C
    bool FCI::ruleR8_(PAG& pag) const {
      bool changed = false;
      for (const NodeId b: pag.nodes()) {
        for (const NodeId a: pag.neighbours(b)) {
          for (const NodeId c: pag.neighbours(b)) {
            if (a == c) { continue; }
            if (!pag.existsEdge(a, c)) { continue; }
            if (!pag.isArrowhead(a, c)) { continue; }   // A *→ C
            if (!pag.isCircle(c, a)) { continue; }      // circle at A from C
            if (!pag.isArrowhead(b, c)) { continue; }   // B *→ C
            if (!pag.isTail(c, b)) { continue; }        // Tail at B from C (B→C directed)
            const bool case1 = pag.isArrowhead(a, b) && pag.isTail(b, a);   // A→B
            const bool case2 = pag.isCircle(a, b) && pag.isTail(b, a);      // A-oB
            if (!case1 && !case2) { continue; }
            // FIX #1 (defensive): isCircle(c,a) already ensures this is not Arrowhead,
            // but guard explicitly for clarity.
            if (pag.isArrowhead(c, a)) { continue; }
            pag.setMarkAt(c, a, EdgeMark::Tail);                            // A→C: tail at A from C
            changed = true;
          }
        }
      }
      return changed;
    }

    // R9: A o→ C, uncovered pd-path A→B→…→C (B not adj C) → A→C
    bool FCI::ruleR9_(PAG& pag) const {
      bool changed = false;
      for (const NodeId c: pag.nodes()) {
        for (const NodeId a: pag.neighbours(c)) {
          if (!pag.isArrowhead(a, c)) { continue; }   // A *→ C
          if (!pag.isCircle(c, a)) { continue; }      // circle at A from C (A o→ C)
          bool oriented = false;
          for (const NodeId b: pag.neighbours(a)) {
            if (b == c) { continue; }
            if (pag.existsEdge(b, c)) { continue; }    // B not adjacent C
            if (pag.isArrowhead(b, a)) { continue; }   // B must be possible child of A
            if (existsUncoveredPdPath_(pag, a, b, c)) {
              pag.setMarkAt(c, a, EdgeMark::Tail);
              changed  = true;
              oriented = true;
              break;
            }
          }
          if (oriented) { continue; }
        }
      }
      return changed;
    }

    // R10: A o→ C, B→C, D→C, two paths from A's possible children to B,D → A→C
    bool FCI::ruleR10_(PAG& pag) const {
      bool changed = false;
      for (const NodeId c: pag.nodes()) {
        // all nodes with arrowhead into C
        std::vector< NodeId > into_c;
        for (const NodeId n: pag.neighbours(c)) {
          if (pag.isArrowhead(n, c)) { into_c.push_back(n); }
        }
        if (into_c.size() < 2) { continue; }

        for (const NodeId a: into_c) {
          if (!pag.isCircle(c, a)) { continue; }   // A o→ C
          // possible children of A (not C)
          std::vector< NodeId > a_children;
          for (const NodeId x: pag.neighbours(a)) {
            if (x == c) { continue; }
            if (!pag.isArrowhead(x, a)) { a_children.push_back(x); }
          }
          if (a_children.size() < 2) { continue; }

          bool oriented = false;
          for (std::size_t i = 0; i < into_c.size() && !oriented; ++i) {
            for (std::size_t j = i + 1; j < into_c.size() && !oriented; ++j) {
              const NodeId bd = into_c[i];
              const NodeId dd = into_c[j];
              if (!pag.isTail(c, bd)) { continue; }   // B→C
              if (!pag.isTail(c, dd)) { continue; }   // D→C
              for (std::size_t m = 0; m < a_children.size() && !oriented; ++m) {
                for (std::size_t n2 = m + 1; n2 < a_children.size() && !oriented; ++n2) {
                  const NodeId ch1 = a_children[m];
                  const NodeId ch2 = a_children[n2];
                  if (pag.existsEdge(ch1, ch2)) { continue; }
                  // FIX #6: check both (ch1→bd, ch2→dd) and (ch1→dd, ch2→bd).
                  // Zhang 2008 R10 requires one path to B and one to D from two
                  // non-adjacent children; either assignment of children to targets
                  // is valid.
                  const bool direct  = existsSemiDirectedPath_(pag, ch1, bd)
                                    && existsSemiDirectedPath_(pag, ch2, dd);
                  const bool crossed = existsSemiDirectedPath_(pag, ch1, dd)
                                    && existsSemiDirectedPath_(pag, ch2, bd);
                  if (!direct && !crossed) { continue; }
                  pag.setMarkAt(c, a, EdgeMark::Tail);
                  changed  = true;
                  oriented = true;
                }
              }
            }
          }
        }
      }
      return changed;
    }

    void FCI::applyOrientationRules_(PAG& pag) const {
      bool changed = true;
      while (changed) {
        changed = false;
        changed |= ruleR1_(pag);
        changed |= ruleR2_(pag);
        changed |= ruleR3_(pag);
        changed |= ruleR4_(pag);
        changed |= ruleR5_(pag);
        changed |= ruleR6_(pag);
        changed |= ruleR7_(pag);
        changed |= ruleR8_(pag);
        changed |= ruleR9_(pag);
        changed |= ruleR10_(pag);
      }
    }

    // ##########################################################################
    // Primary output — learnPAG
    // ##########################################################################

    PAG FCI::learnPAG(MixedGraph graph) {
      // Phase 1: skeleton discovery
      MixedGraph skeleton = learnSkeleton(std::move(graph));
      applyStructuralConstraints_(skeleton);

      // Build initial PAG from skeleton
      PAG pag;
      for (const NodeId n: skeleton.nodes()) {
        pag.addNodeWithId(n);
      }
      // undirected skeleton edges → Circle-Circle
      for (const Edge& e: skeleton.edges()) {
        pag.addEdge(e.first(), e.second());
      }
      // mandatory arcs → Tail-Arrowhead
      for (const Arc& a: skeleton.arcs()) {
        pag.addEdge(a.tail(), a.head(), EdgeMark::Tail, EdgeMark::Arrowhead);
      }

      // Phase 2: orient unshielded colliders on PAG
      orientCollidersOnPAG_(pag, skeleton);

      // Phase 3: possibleDSep pruning
      possibleDSepPhase_(pag);

      // Phase 4: reset marks to Circle, restore mandatory arcs, re-orient colliders
      pag.reorientAllWith(EdgeMark::Circle);
      for (const Arc& a: skeleton.arcs()) {
        if (pag.existsEdge(a.tail(), a.head())) {
          pag.setMarkAt(a.tail(), a.head(), EdgeMark::Arrowhead);
          pag.setMarkAt(a.head(), a.tail(), EdgeMark::Tail);
        }
      }
      orientCollidersOnPAG_(pag, pag.toMixedGraph());

      // Phase 5: orientation rules R1–R10 (fixed-point)
      applyOrientationRules_(pag);

      // Phase 6: collect bidirected edges (↔) as latent couples
      _latentCouples_.clear();
      for (const Edge& e: pag.edges()) {
        if (pag.isBidirected(e.first(), e.second())) {
          _latentCouples_.emplace_back(e.first(), e.second());
        }
      }

      return pag;
    }

    // ##########################################################################
    // Bridge — learnMixedStructure
    // ##########################################################################

    MixedGraph FCI::learnMixedStructure(MixedGraph graph) {
      PAG        pag = learnPAG(std::move(graph));
      MixedGraph mg  = pag.toMixedGraph();
      orientDoubleHeadedArcs_(mg);
      return mg;
    }

}   // namespace gum::learning
