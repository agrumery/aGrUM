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


#include <algorithm>
#include <numeric>
#include <vector>

#include <agrum/base/graphs/mixedGraph.h>
#include <agrum/BN/learning/PC.h>

namespace gum {

  namespace learning {

    // ##########################################################################
    // Constructors / Destructors
    // ##########################################################################

    PC::PC()  = default;
    PC::~PC() = default;

    PC::PC(const PC& from) :
        ConstraintBasedLearning(from), test_(from.test_), alpha_(from.alpha_),
        maxCondSetSize_(from.maxCondSetSize_), stable_(from.stable_), sepSet_(from.sepSet_) {}

    PC::PC(PC&& from) noexcept :
        ConstraintBasedLearning(std::move(from)), test_(from.test_), alpha_(from.alpha_),
        maxCondSetSize_(from.maxCondSetSize_), stable_(from.stable_),
        sepSet_(std::move(from.sepSet_)) {
      from.test_ = nullptr;
    }

    PC& PC::operator=(const PC& from) {
      if (this != &from) {
        ConstraintBasedLearning::operator=(from);
        test_           = from.test_;
        alpha_          = from.alpha_;
        maxCondSetSize_ = from.maxCondSetSize_;
        stable_         = from.stable_;
        sepSet_         = from.sepSet_;
      }
      return *this;
    }

    PC& PC::operator=(PC&& from) noexcept {
      if (this != &from) {
        ConstraintBasedLearning::operator=(std::move(from));
        test_           = from.test_;
        alpha_          = from.alpha_;
        maxCondSetSize_ = from.maxCondSetSize_;
        stable_         = from.stable_;
        sepSet_         = std::move(from.sepSet_);
        from.test_      = nullptr;
      }
      return *this;
    }

    // ##########################################################################
    // Scorer injection and parameterization
    // ##########################################################################

    void PC::setIndependenceTest(IndependenceTest& test) { test_ = &test; }

    void PC::setAlpha(double alpha) { alpha_ = alpha; }

    double PC::alpha() const { return alpha_; }

    void PC::setMaxCondSetSize(Size max_k) { maxCondSetSize_ = max_k; }

    void PC::setStable(bool stable) { stable_ = stable; }

    // ##########################################################################
    // Phase 1 — Skeleton
    // ##########################################################################

    MixedGraph PC::learnSkeleton(MixedGraph graph) {
      graph = initGraph_(graph);
      sepSet_.clear();

      Size d = 0;
      while (d <= maxCondSetSize_) {
        // snapshot edges: avoids iterator invalidation in both stable/unstable modes
        std::vector< Edge > edges(graph.edges().begin(), graph.edges().end());

        bool                anyTested = false;
        std::vector< Edge > toRemove;

        for (const Edge& edge: edges) {
          if (!graph.existsEdge(edge)) { continue; }   // already removed (unstable mode)

          const NodeId X = edge.first();
          const NodeId Y = edge.second();

          // Returns true if a separating set was found (edge removed or marked).
          // Lambda replaces goto-based early exit from nested loops.
          const bool separated = [&]() -> bool {
            if (d == 0) {
              // marginal test: symmetric, single direction suffices
              anyTested = true;
              const double pval = test_->statistics(X, Y, _emptySet_).second;
              if (pval <= alpha_) return false;
              if (stable_) toRemove.push_back(edge); else graph.eraseEdge(edge);
              sepSet_.insert({X, Y}, {});
              sepSet_.insert({Y, X}, {});
              return true;
            }

            // d > 0: try both directions (A=X,B=Y) then (A=Y,B=X)
            for (int dir = 0; dir < 2; ++dir) {
              const NodeId A = (dir == 0) ? X : Y;
              const NodeId B = (dir == 0) ? Y : X;

              std::vector< NodeId > adj;
              for (NodeId nb: graph.neighbours(A))
                if (nb != B) adj.push_back(nb);

              if (static_cast< Size >(adj.size()) < d) continue;
              anyTested = true;

              // iterate all S ⊆ adj with |S| = d
              std::vector< std::size_t > idx(d);
              std::iota(idx.begin(), idx.end(), std::size_t(0));

              for (;;) {
                std::vector< NodeId > cond(d);
                for (Size i = 0; i < d; ++i) cond[i] = adj[idx[i]];

                const double pval = test_->statistics(A, B, cond).second;
                if (pval > alpha_) {
                  if (stable_) toRemove.push_back(edge); else graph.eraseEdge(edge);
                  sepSet_.insert({X, Y}, cond);
                  sepSet_.insert({Y, X}, cond);
                  return true;
                }

                // advance combination indices
                int i = static_cast< int >(d) - 1;
                while (i >= 0 && idx[i] == adj.size() - d + static_cast< std::size_t >(i))
                  --i;
                if (i < 0) break;
                ++idx[i];
                for (int j = i + 1; j < static_cast< int >(d); ++j)
                  idx[j] = idx[j - 1] + 1;
              }
            }
            return false;
          }();
          (void)separated;
        }

        if (stable_) {
          for (const Edge& e: toRemove) {
            if (graph.existsEdge(e)) { graph.eraseEdge(e); }
          }
        }

        if (!anyTested) { break; }   // no node has enough neighbours for depth d
        ++d;
      }

      return graph;
    }

    // ##########################################################################
    // Phase 2 — V-structure orientation
    // ##########################################################################

    MixedGraph PC::orientVStructures_(MixedGraph graph) {
      const auto triples = unshieldedTriples_(graph, sepSet_);

      for (const ThreePoints& tp: triples) {
        const NodeId X = std::get< 0 >(tp);   // extremity  (y < x enforced by unshieldedTriples_)
        const NodeId Y = std::get< 1 >(tp);   // extremity
        const NodeId Z = std::get< 2 >(tp);   // centre

        if (!sepSet_.exists({X, Y})) { continue; }
        const std::vector< NodeId >& sep = sepSet_[{X, Y}];

        const bool zInSep = std::find(sep.begin(), sep.end(), Z) != sep.end();
        if (zInSep) { continue; }

        // Z is a collider: X→Z←Y
        if (graph.existsEdge(X, Z) && isArcValid_(graph, X, Z)) {
          graph.eraseEdge(Edge(X, Z));
          graph.addArc(X, Z);
          GUM_SL_EMIT(X, Z, "Orient " << X << " -> " << Z, "V-structure collider");
        }
        if (graph.existsEdge(Y, Z) && isArcValid_(graph, Y, Z)) {
          graph.eraseEdge(Edge(Y, Z));
          graph.addArc(Y, Z);
          GUM_SL_EMIT(Y, Z, "Orient " << Y << " -> " << Z, "V-structure collider");
        }
        // existsEdge false  → already oriented by a prior triple → skip (V1)
        // isArcValid_ false → forbidden arc or max-indegree violated → skip (V1)
      }

      return graph;
    }

    // ##########################################################################
    // Orchestration
    // ##########################################################################

    MixedGraph PC::learnMixedStructure(MixedGraph graph) {
      graph = learnSkeleton(graph);
      applyStructuralConstraints_(graph);
      graph = orientVStructures_(graph);
      return graph;
    }

  } /* namespace learning */

} /* namespace gum */
