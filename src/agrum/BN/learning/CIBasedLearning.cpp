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
#include <set>
#include <vector>

#include <agrum/base/graphs/mixedGraph.h>
#include <agrum/BN/learning/CIBasedLearning.h>

namespace gum {

  namespace learning {

    // ##########################################################################
    // Constructors / Destructors
    // ##########################################################################

    CIBasedLearning::CIBasedLearning()  = default;
    CIBasedLearning::~CIBasedLearning() = default;

    CIBasedLearning::CIBasedLearning(const CIBasedLearning& from) :
        ConstraintBasedLearning(from), test_(from.test_), alpha_(from.alpha_),
        maxCondSetSize_(from.maxCondSetSize_), stable_(from.stable_),
        exhaustiveSepSet_(from.exhaustiveSepSet_),
        ucPriority_(from.ucPriority_), sepSet_(from.sepSet_) {}

    CIBasedLearning::CIBasedLearning(CIBasedLearning&& from) noexcept :
        ConstraintBasedLearning(std::move(from)), test_(from.test_), alpha_(from.alpha_),
        maxCondSetSize_(from.maxCondSetSize_), stable_(from.stable_),
        exhaustiveSepSet_(from.exhaustiveSepSet_),
        ucPriority_(from.ucPriority_), sepSet_(std::move(from.sepSet_)) {
      from.test_ = nullptr;
    }

    CIBasedLearning& CIBasedLearning::operator=(const CIBasedLearning& from) {
      if (this != &from) {
        ConstraintBasedLearning::operator=(from);
        test_             = from.test_;
        alpha_            = from.alpha_;
        maxCondSetSize_   = from.maxCondSetSize_;
        stable_           = from.stable_;
        exhaustiveSepSet_ = from.exhaustiveSepSet_;
        ucPriority_       = from.ucPriority_;
        sepSet_           = from.sepSet_;
      }
      return *this;
    }

    CIBasedLearning& CIBasedLearning::operator=(CIBasedLearning&& from) noexcept {
      if (this != &from) {
        ConstraintBasedLearning::operator=(std::move(from));
        test_             = from.test_;
        alpha_            = from.alpha_;
        maxCondSetSize_   = from.maxCondSetSize_;
        stable_           = from.stable_;
        exhaustiveSepSet_ = from.exhaustiveSepSet_;
        ucPriority_       = from.ucPriority_;
        sepSet_           = std::move(from.sepSet_);
        from.test_        = nullptr;
      }
      return *this;
    }

    // ##########################################################################
    // Parameterisation
    // ##########################################################################

    void CIBasedLearning::setIndependenceTest(IndependenceTest& test) { test_ = &test; }

    void   CIBasedLearning::setAlpha(double alpha) { alpha_ = alpha; }
    double CIBasedLearning::alpha() const { return alpha_; }

    void CIBasedLearning::setMaxCondSetSize(Size max_k) { maxCondSetSize_ = max_k; }
    void CIBasedLearning::setStable(bool stable) { stable_ = stable; }
    void CIBasedLearning::setExhaustiveSepSet(bool exhaustive) { exhaustiveSepSet_ = exhaustive; }
    bool CIBasedLearning::exhaustiveSepSet() const { return exhaustiveSepSet_; }
    void CIBasedLearning::setUCPriority(UCPriority p) { ucPriority_ = p; }
    CIBasedLearning::UCPriority CIBasedLearning::ucPriority() const { return ucPriority_; }

    // ##########################################################################
    // Phase 1 — Skeleton
    // ##########################################################################

    MixedGraph CIBasedLearning::learnSkeleton(MixedGraph graph) {
      graph = initGraph_(graph);
      sepSet_.clear();

      Size d = 0;
      while (d <= maxCondSetSize_) {
        std::vector< Edge > edges(graph.edges().begin(), graph.edges().end());

        bool                anyTested = false;
        std::vector< Edge > toRemove;

        for (const Edge& edge: edges) {
          if (!graph.existsEdge(edge)) { continue; }

          const NodeId X = edge.first();
          const NodeId Y = edge.second();

          [&]() {
            if (d == 0) {
              anyTested         = true;
              const double pval = test_->statistics(X, Y, _emptySet_).second;
              if (pval <= alpha_) { return; }
              if (stable_) { toRemove.push_back(edge); }
              else { graph.eraseEdge(edge); }
              sepSet_.insert({X, Y}, {{}, pval});
              sepSet_.insert({Y, X}, {{}, pval});
              return;
            }

            bool               exh_found = false;
            std::set< NodeId > exh_union;
            double             exh_pval  = 0.0;

            for (int dir = 0; dir < 2; ++dir) {
              const NodeId A = (dir == 0) ? X : Y;
              const NodeId B = (dir == 0) ? Y : X;

              std::vector< NodeId > adj;
              for (NodeId nb: graph.neighbours(A)) {
                if (nb != B) { adj.push_back(nb); }
              }

              if (static_cast< Size >(adj.size()) < d) { continue; }
              anyTested = true;

              std::vector< std::size_t > idx(d);
              std::iota(idx.begin(), idx.end(), std::size_t(0));

              for (;;) {
                std::vector< NodeId > cond(d);
                for (Size i = 0; i < d; ++i) {
                  cond[i] = adj[idx[i]];
                }

                const double pval = test_->statistics(A, B, cond).second;
                if (pval > alpha_) {
                  if (!exhaustiveSepSet_) {
                    if (stable_) { toRemove.push_back(edge); }
                    else { graph.eraseEdge(edge); }
                    sepSet_.insert({X, Y}, {cond, pval});
                    sepSet_.insert({Y, X}, {cond, pval});
                    return;
                  }
                  exh_found = true;
                  if (exh_pval == 0.0) { exh_pval = pval; }
                  for (const NodeId n: cond) { exh_union.insert(n); }
                }

                int i = static_cast< int >(d) - 1;
                while (i >= 0 && idx[i] == adj.size() - d + static_cast< std::size_t >(i)) {
                  --i;
                }
                if (i < 0) { break; }
                ++idx[i];
                for (int j = i + 1; j < static_cast< int >(d); ++j) {
                  idx[j] = idx[j - 1] + 1;
                }
              }
            }

            if (exh_found) {
              if (stable_) { toRemove.push_back(edge); }
              else { graph.eraseEdge(edge); }
              const std::vector< NodeId > union_vec(exh_union.begin(), exh_union.end());
              sepSet_.insert({X, Y}, {union_vec, exh_pval});
              sepSet_.insert({Y, X}, {union_vec, exh_pval});
            }
          }();
        }

        if (stable_) {
          for (const Edge& e: toRemove) {
            if (graph.existsEdge(e)) { graph.eraseEdge(e); }
          }
        }

        if (!anyTested) { break; }
        ++d;
      }

      return graph;
    }

    // ##########################################################################
    // Phase 2 — Unshielded collider orientation
    // ##########################################################################

    void CIBasedLearning::orientColliderArm_(MixedGraph& graph, NodeId src, NodeId dst) {
      if (graph.existsEdge(src, dst)) {
        if (isArcValid_(graph, src, dst) && !_existsDirectedPath_(graph, dst, src)) {
          graph.eraseEdge(Edge(src, dst));
          graph.addArc(src, dst);
          GUM_SL_EMIT(src, dst, "Orient " << src << " -> " << dst, "V-structure collider");
        } else {
          resolveOrientConflict_(src, dst);
          GUM_SL_EMIT(src, dst, "Conflict " << src << " -> " << dst, "V-structure blocked by constraint");
        }
      } else if (graph.existsArc(dst, src)) {
        resolveOrientConflict_(src, dst);
        GUM_SL_EMIT(src, dst, "Conflict " << src << " -> " << dst, "V-structure conflict: latent?");
      }
      // existsArc(src, dst) → consistent, skip
    }

    void CIBasedLearning::resolveOrientConflict_(NodeId src, NodeId dst) {
      _latentCouples_.push_back(Arc(src, dst));
    }

    MixedGraph CIBasedLearning::orientUnshieldedColliders_(MixedGraph graph) {
      _latentCouples_.clear();

      const auto triples = unshieldedTriples_(graph);

      const auto isCollider = [&](NodeId X, NodeId Y, NodeId Z) -> bool {
        if (!sepSet_.exists({X, Y})) { return false; }
        const SepSetEntry_& entry = sepSet_[{X, Y}];
        return std::find(entry.cond.begin(), entry.cond.end(), Z) == entry.cond.end();
      };

      if (ucPriority_ == UCPriority::Standard) {
        for (const ThreePoints& tp: triples) {
          const NodeId X = std::get< 0 >(tp);
          const NodeId Y = std::get< 1 >(tp);
          const NodeId Z = std::get< 2 >(tp);
          if (!isCollider(X, Y, Z)) { continue; }
          orientColliderArm_(graph, X, Z);
          orientColliderArm_(graph, Y, Z);
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
          candidates.push_back({X, Y, Z, sepSet_[{X, Y}].pval});
        }

        std::sort(candidates.begin(), candidates.end(), [](const Candidate& a, const Candidate& b) {
          return a.pval > b.pval;
        });

        for (const auto& [X, Y, Z, pval]: candidates) {
          orientColliderArm_(graph, X, Z);
          orientColliderArm_(graph, Y, Z);
        }
      }

      return graph;
    }

  } /* namespace learning */

} /* namespace gum */
