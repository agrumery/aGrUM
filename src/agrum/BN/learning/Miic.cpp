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


/** @file
 * @brief Implementation of gum::learning::Miic
 *
 * @author Quentin FALCAND, Marvin LASSERRE and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/base/core/hashTable.h>
#include <agrum/base/core/heap.h>
#include <agrum/base/core/math/math_utils.h>
#include <agrum/base/core/timer.h>
#include <agrum/base/graphs/mixedGraph.h>
#include <agrum/BN/learning/correctedMutualInformation.h>
#include <agrum/BN/learning/Miic.h>

namespace gum {

  namespace learning {

    // ##########################################################################
    // Constructors / Destructors
    // ##########################################################################

    Miic::Miic() : ConstraintBasedLearning() { GUM_CONSTRUCTOR(Miic); }

    Miic::Miic(int maxLog) : ConstraintBasedLearning(maxLog) { GUM_CONSTRUCTOR(Miic); }

    Miic::Miic(const Miic& from) : ConstraintBasedLearning(from) { GUM_CONS_CPY(Miic); }

    Miic::Miic(Miic&& from) : ConstraintBasedLearning(std::move(from)) { GUM_CONS_MOV(Miic); }

    Miic::~Miic() { GUM_DESTRUCTOR(Miic); }

    Miic& Miic::operator=(const Miic& from) {
      ConstraintBasedLearning::operator=(from);
      return *this;
    }

    Miic& Miic::operator=(Miic&& from) {
      ConstraintBasedLearning::operator=(std::move(from));
      return *this;
    }

    // ##########################################################################
    // Skeleton learning
    // ##########################################################################

    MixedGraph Miic::learnSkeleton(CorrectedMutualInformation& mutualInformation,
                                   MixedGraph                  graph) {
      timer_.reset();
      current_step_ = 0;

      _latentCouples_.clear();

      Heap< CondRanking, GreaterPairOn2nd >                                    rank;
      HashTable< std::pair< NodeId, NodeId >, std::vector< NodeId > > sep_set;

      initiation_(mutualInformation, graph, sep_set, rank);
      iteration_(mutualInformation, graph, sep_set, rank);

      return graph;
    }

    MixedGraph Miic::learnMixedStructure(CorrectedMutualInformation& mutualInformation,
                                         MixedGraph                  graph) {
      timer_.reset();
      current_step_ = 0;

      _latentCouples_.clear();

      Heap< CondRanking, GreaterPairOn2nd >                                    rank;
      HashTable< std::pair< NodeId, NodeId >, std::vector< NodeId > > sep_set;

      initiation_(mutualInformation, graph, sep_set, rank);
      iteration_(mutualInformation, graph, sep_set, rank);
      orientationMiic_(mutualInformation, graph, sep_set);

      return meekRules_.propagate(graph);
    }

    // ##########################################################################
    // PHASE 1 : INITIATION
    // ##########################################################################

    void Miic::initiation_(CorrectedMutualInformation& mutualInformation,
                           MixedGraph&                 graph,
                           HashTable< std::pair< NodeId, NodeId >, std::vector< NodeId > >& sepSet,
                           Heap< CondRanking, GreaterPairOn2nd >&                           rank) {
      NodeId  x, y;
      EdgeSet edges      = graph.edges();
      Size    steps_init = edges.size();

      for (const Edge& edge: edges) {
        x = edge.first();
        y = edge.second();
        if (isForbiddenEdge_(x, y)) {
          GUM_SL_EMIT(x, y, "Remove " << x << " - " << y, " Constraints : Forbidden edge")
          graph.eraseEdge(edge);
        } else {
          double Ixy = mutualInformation.score(x, y);

          if (Ixy <= 0) {
            graph.eraseEdge(edge);
            GUM_SL_EMIT(x,
                        y,
                        "Remove " << x << " - " << y,
                        "Independent based on Mutual Information :" << Ixy)
            sepSet.insert(std::make_pair(x, y), _emptySet_);
          } else {
            findBestContributor_(x, y, _emptySet_, graph, mutualInformation, rank);
            GUM_SL_EMIT(x,
                        y,
                        "Keep " << x << " - " << y,
                        "Dependent based on Mutual Information :" << Ixy)
          }

          ++current_step_;
          if (onProgress.hasListener()) {
            GUM_EMIT3(onProgress, (current_step_ * 33) / steps_init, 0., timer_.step());
          }
        }
      }
    }

    // ##########################################################################
    // PHASE 2 : ITERATION
    // ##########################################################################

    void Miic::iteration_(CorrectedMutualInformation& mutualInformation,
                          MixedGraph&                 graph,
                          HashTable< std::pair< NodeId, NodeId >, std::vector< NodeId > >& sepSet,
                          Heap< CondRanking, GreaterPairOn2nd >&                           rank) {
      CondRanking best;
      Size        steps_init = current_step_;
      Size        steps_iter = rank.size();

      try {
        while (rank.top().second > 0.5) {
          best = rank.pop();

          const NodeId          x  = std::get< 0 >(*(best.first));
          const NodeId          y  = std::get< 1 >(*(best.first));
          const NodeId          z  = std::get< 2 >(*(best.first));
          std::vector< NodeId > ui = std::move(std::get< 3 >(*(best.first)));

          ui.push_back(z);
          const double i_xy_ui = mutualInformation.score(x, y, ui);
          if (i_xy_ui < 0) {
            graph.eraseEdge(Edge(x, y));
            GUM_SL_EMIT(x,
                        y,
                        "Remove " << x << " - " << y,
                        "Independent based on MutualInformation knowing Sep "
                            << ui << "Mutual information:" << i_xy_ui)
            sepSet.insert(std::make_pair(x, y), std::move(ui));
          } else {
            findBestContributor_(x, y, ui, graph, mutualInformation, rank);
          }

          delete best.first;

          ++current_step_;
          if (onProgress.hasListener()) {
            GUM_EMIT3(onProgress,
                      (current_step_ * 66) / (steps_init + steps_iter),
                      0.,
                      timer_.step());
          }
        }
      } catch (...) {}   // heap is empty
      current_step_ = steps_init + steps_iter;
      if (onProgress.hasListener()) { GUM_EMIT3(onProgress, 66, 0., timer_.step()); }
      current_step_ = steps_init + steps_iter;
    }

    // ##########################################################################
    // PHASE 3 : ORIENTATION
    // ##########################################################################

    void Miic::orientationMiic_(
        CorrectedMutualInformation&                                            mutualInformation,
        MixedGraph&                                                            graph,
        const HashTable< std::pair< NodeId, NodeId >, std::vector< NodeId > >& sepSet) {
      HashTable< std::pair< NodeId, NodeId >, char > marks = _initialMarks_;

      for (auto& arc: _mandatoryGraph_.arcs()) {
        if (graph.existsEdge(arc.head(), arc.tail())) {
          graph.eraseEdge(Edge(arc.head(), arc.tail()));
          GUM_SL_EMIT(arc.tail(),
                      arc.head(),
                      "Add Arc" << arc.tail() << "->" << arc.head(),
                      "Mandatory")
          graph.addArc(arc.tail(), arc.head());
          marks.insert({arc.tail(), arc.head()}, '>');
          marks.insert({arc.head(), arc.tail()}, '-');
        } else {
          graph.addArc(arc.tail(), arc.head());
          marks.insert({arc.tail(), arc.head()}, '>');
          marks.insert({arc.head(), arc.tail()}, '-');
        }
      }

      for (Arc arc: _forbiddenGraph_.arcs()) {
        if (graph.existsEdge(Edge(arc.tail(), arc.head()))) {
          graph.eraseEdge(Edge(arc.tail(), arc.head()));
          graph.addArc(arc.head(), arc.tail());
          GUM_SL_EMIT(arc.head(),
                      arc.tail(),
                      "Add Arc" << arc.head() << "->" << arc.tail(),
                      "Forbidden in the other orientation")
          marks.insert({arc.tail(), arc.head()}, '-');
          marks.insert({arc.head(), arc.tail()}, '>');
        }
      }

      std::vector< ProbabilisticRanking > proba_triples
          = unshieldedTriplesMiic_(graph, mutualInformation, sepSet, marks);

      const Size steps_orient = proba_triples.size();
      Size       past_steps   = current_step_;

      ProbabilisticRanking best;
      if (steps_orient > 0) best = proba_triples[0];

      while (!proba_triples.empty() && std::max(std::get< 2 >(best), std::get< 3 >(best)) >= 0.5) {
        const NodeId x = std::get< 0 >(*std::get< 0 >(best));
        const NodeId y = std::get< 1 >(*std::get< 0 >(best));
        const NodeId z = std::get< 2 >(*std::get< 0 >(best));

        const double i3 = std::get< 1 >(best);
        const double p1 = std::get< 2 >(best);
        const double p2 = std::get< 3 >(best);

        if (i3 <= 0) {
          _orientingVstructureMiic_(graph, marks, x, y, z, p1, p2);
        } else {
          _propagatingOrientationMiic_(graph, marks, x, y, z, p1, p2);
        }

        delete std::get< 0 >(best);
        proba_triples.erase(proba_triples.begin());
        proba_triples = updateProbaTriples_(graph, proba_triples);

        if (!proba_triples.empty()) best = proba_triples[0];

        ++current_step_;
        if (onProgress.hasListener()) {
          GUM_EMIT3(onProgress,
                    (current_step_ * 100) / (steps_orient + past_steps),
                    0.,
                    timer_.step());
        }
      }

      for (auto iter = _latentCouples_.rbegin(); iter != _latentCouples_.rend(); ++iter) {
        graph.eraseArc(Arc(iter->head(), iter->tail()));
        if (_existsDirectedPath_(graph, iter->head(), iter->tail())) {
          graph.addArc(iter->head(), iter->tail());
          graph.eraseArc(Arc(iter->tail(), iter->head()));
          *iter = Arc(iter->head(), iter->tail());
        }
      }

      if (onProgress.hasListener()) { GUM_EMIT3(onProgress, 100, 0., timer_.step()); }
    }

    // ##########################################################################
    // v-structure orientation
    // ##########################################################################

    void Miic::_orientingVstructureMiic_(MixedGraph&                                     graph,
                                         HashTable< std::pair< NodeId, NodeId >, char >& marks,
                                         NodeId                                          x,
                                         NodeId                                          y,
                                         NodeId                                          z,
                                         double                                          p1,
                                         double                                          p2) {
      if (marks[{x, z}] == 'o' && marks[{y, z}] == 'o') {
        if (!_existsNonTrivialDirectedPath_(graph, z, x)) {
          if (isArcValid_(graph, x, z)) {
            graph.eraseEdge(Edge(x, z));
            graph.addArc(x, z);
            GUM_SL_EMIT(x, z, "Add Arc " << x << " -> " << z, "V-structure Orientation")
            marks[{x, z}] = '>';
            if (graph.existsArc(z, x) && _isNotLatentCouple_(z, x)) {
              _latentCouples_.emplace_back(z, x);
            }
            if (!_arcProbas_.exists(Arc(x, z))) _arcProbas_.insert(Arc(x, z), p1);
          }
        } else {
          graph.eraseEdge(Edge(x, z));
          if (!_existsNonTrivialDirectedPath_(graph, x, z)) {
            if (isArcValid_(graph, z, x)) {
              graph.addArc(z, x);
              GUM_SL_EMIT(z, x, "Add Arc " << z << " -> " << x, "V-structure Orientation")
              marks[{z, x}] = '>';
            }
          }
        }

        if (!_existsNonTrivialDirectedPath_(graph, z, y)) {
          if (isArcValid_(graph, y, z)) {
            graph.eraseEdge(Edge(y, z));
            graph.addArc(y, z);
            GUM_SL_EMIT(y, z, "Add Arc " << y << " -> " << z, "V-structure Orientation")
            marks[{y, z}] = '>';
            if (graph.existsArc(z, y) && _isNotLatentCouple_(z, y)) {
              _latentCouples_.emplace_back(z, y);
            }
            if (!_arcProbas_.exists(Arc(y, z))) _arcProbas_.insert(Arc(y, z), p2);
          }
        } else {
          graph.eraseEdge(Edge(y, z));
          if (!_existsNonTrivialDirectedPath_(graph, y, z)) {
            if (isArcValid_(graph, z, y)) {
              graph.addArc(z, y);
              GUM_SL_EMIT(z, y, "Add Arc " << z << " -> " << y, "V-structure Orientation")
              marks[{z, y}] = '>';
            }
          }
        }
      } else if (marks[{x, z}] == '>' && marks[{y, z}] == 'o') {
        if (!_existsNonTrivialDirectedPath_(graph, z, y)) {
          if (isArcValid_(graph, y, z)) {
            graph.eraseEdge(Edge(y, z));
            graph.addArc(y, z);
            GUM_SL_EMIT(y,
                        z,
                        "Add Arc " << y << " -> " << z,
                        "V-structure Orientation | existing "
                            << x << " -> " << z << ", then orienting " << y << " -> " << z)
            marks[{y, z}] = '>';
            if (graph.existsArc(z, y) && _isNotLatentCouple_(z, y)) {
              _latentCouples_.emplace_back(z, y);
            }
            if (!_arcProbas_.exists(Arc(y, z))) _arcProbas_.insert(Arc(y, z), p2);
          }
        } else {
          graph.eraseEdge(Edge(y, z));
          if (!_existsNonTrivialDirectedPath_(graph, y, z)) {
            if (isArcValid_(graph, z, y)) {
              graph.addArc(z, y);
              GUM_SL_EMIT(z,
                          y,
                          "Add Arc " << z << " -> " << y,
                          "V-structure Orientation | existing "
                              << x << " -> " << z << ", then orienting " << z << " -> " << y)
              marks[{z, y}] = '>';
            }
          }
        }
      } else if (marks[{y, z}] == '>' && marks[{x, z}] == 'o') {
        if (!_existsNonTrivialDirectedPath_(graph, z, x)) {
          if (isArcValid_(graph, x, z)) {
            graph.eraseEdge(Edge(x, z));
            graph.addArc(x, z);
            GUM_SL_EMIT(x, z, "Add Arc " << x << " -> " << z, "V-structure Orientation")
            marks[{x, z}] = '>';
            if (graph.existsArc(z, x) && _isNotLatentCouple_(z, x)) {
              _latentCouples_.emplace_back(z, x);
            }
            if (!_arcProbas_.exists(Arc(x, z))) _arcProbas_.insert(Arc(x, z), p1);
          }
        } else {
          graph.eraseEdge(Edge(x, z));
          if (!_existsNonTrivialDirectedPath_(graph, x, z)) {
            if (isArcValid_(graph, z, x)) {
              graph.addArc(z, x);
              GUM_SL_EMIT(z, x, "Add Arc " << z << " -> " << x, "V-structure Orientation")
              marks[{z, x}] = '>';
            }
          }
        }
      }
    }

    // ##########################################################################
    // Orientation propagation
    // ##########################################################################

    void Miic::_propagatingOrientationMiic_(MixedGraph&                                     graph,
                                            HashTable< std::pair< NodeId, NodeId >, char >& marks,
                                            NodeId                                          x,
                                            NodeId                                          y,
                                            NodeId                                          z,
                                            double                                          p1,
                                            double                                          p2) {
      if (marks[{x, z}] == '>' && marks[{y, z}] == 'o' && marks[{z, y}] != '-') {
        graph.eraseEdge(Edge(z, y));
        if (!_existsDirectedPath_(graph, y, z) && graph.parents(y).empty()) {
          if (isArcValid_(graph, z, y)) {
            graph.addArc(z, y);
            GUM_SL_EMIT(z,
                        y,
                        "Add Arc " << z << " -> " << y,
                        "Propagation MIIC (919) | existing x -> " << z << " and " << z << " - "
                                                                  << y)
            marks[{z, y}] = '>';
            marks[{y, z}] = '-';
            if (!_arcProbas_.exists(Arc(z, y))) _arcProbas_.insert(Arc(z, y), p2);
          }
        } else if (!_existsDirectedPath_(graph, z, y) && graph.parents(z).empty()) {
          if (isArcValid_(graph, y, z)) {
            graph.addArc(y, z);
            GUM_SL_EMIT(y, z, "Add Arc " << y << " -> " << z, "Propagation MIIC line 932 ")
            marks[{z, y}] = '-';
            marks[{y, z}] = '>';
            _latentCouples_.emplace_back(y, z);
            if (!_arcProbas_.exists(Arc(y, z))) _arcProbas_.insert(Arc(y, z), p2);
          }
        } else if (!_existsDirectedPath_(graph, y, z)) {
          if (isArcValid_(graph, z, y)) {
            graph.addArc(z, y);
            GUM_SL_EMIT(z, y, "Add Arc " << z << "->" << y, "Propagation MIIC 947 ")
            marks[{z, y}] = '>';
            marks[{y, z}] = '-';
            if (!_arcProbas_.exists(Arc(z, y))) _arcProbas_.insert(Arc(z, y), p2);
          }
        } else if (!_existsDirectedPath_(graph, z, y)) {
          if (isArcValid_(graph, y, z)) {
            graph.addArc(y, z);
            GUM_SL_EMIT(z, y, "Add Arc " << z << "->" << y, "Propagation MIIC 959")
            _latentCouples_.emplace_back(y, z);
            marks[{z, y}] = '-';
            marks[{y, z}] = '>';
            if (!_arcProbas_.exists(Arc(y, z))) _arcProbas_.insert(Arc(y, z), p2);
          }
        }
      } else if (marks[{y, z}] == '>' && marks[{x, z}] == 'o' && marks[{z, x}] != '-') {
        graph.eraseEdge(Edge(z, x));
        if (!_existsDirectedPath_(graph, x, z) && graph.parents(x).empty()) {
          if (isArcValid_(graph, z, x)) {
            graph.addArc(z, x);
            GUM_SL_EMIT(z, x, "Add Arc " << z << " -> " << x, "Propagation MIIC 977")
            marks[{z, x}] = '>';
            marks[{x, z}] = '-';
            if (!_arcProbas_.exists(Arc(z, x))) _arcProbas_.insert(Arc(z, x), p1);
          }
        } else if (!_existsDirectedPath_(graph, z, x) && graph.parents(z).empty()) {
          if (isArcValid_(graph, x, z)) {
            graph.addArc(x, z);
            GUM_SL_EMIT(x, z, "Add Arc " << x << "->" << z, "Propagation MIIC 990")
            marks[{z, x}] = '-';
            marks[{x, z}] = '>';
            _latentCouples_.emplace_back(x, z);
            if (!_arcProbas_.exists(Arc(x, z))) _arcProbas_.insert(Arc(x, z), p1);
          }
        } else if (!_existsDirectedPath_(graph, x, z)) {
          if (isArcValid_(graph, z, x)) {
            graph.addArc(z, x);
            GUM_SL_EMIT(z, x, "Add Arc " << z << " -> " << x, "Propagation MIIC 1004")
            marks[{z, x}] = '>';
            marks[{x, z}] = '-';
            if (!_arcProbas_.exists(Arc(z, x))) _arcProbas_.insert(Arc(z, x), p1);
          }
        } else if (!_existsDirectedPath_(graph, z, x)) {
          if (isArcValid_(graph, x, z)) {
            graph.addArc(x, z);
            GUM_SL_EMIT(x, z, "Add Arc " << x << " -> " << z, "Propagation MIIC 1016")
            marks[{z, x}] = '-';
            marks[{x, z}] = '>';
            _latentCouples_.emplace_back(x, z);
            if (!_arcProbas_.exists(Arc(x, z))) _arcProbas_.insert(Arc(x, z), p1);
          }
        }
      }
    }

    // ##########################################################################
    // Best contributor
    // ##########################################################################

    void Miic::findBestContributor_(NodeId                                 x,
                                    NodeId                                 y,
                                    const std::vector< NodeId >&           ui,
                                    const MixedGraph&                      graph,
                                    CorrectedMutualInformation&            mutualInformation,
                                    Heap< CondRanking, GreaterPairOn2nd >& rank) {
      double maxP = -1.0;
      NodeId maxZ = 0;

      const double Ixy_ui = mutualInformation.score(x, y, ui);

      for (const NodeId z: graph) {
        if (z != x && z != y && std::find(ui.begin(), ui.end(), z) == ui.end()) {
          double Pnv;
          double Pb;

          const double Ixyz_ui    = mutualInformation.score(x, y, z, ui);
          double       calc_expo1 = -Ixyz_ui * M_LN2;
          if (calc_expo1 > _maxLog_) {
            Pnv = 0.0;
          } else if (calc_expo1 < -_maxLog_) {
            Pnv = 1.0;
          } else {
            Pnv = 1 / (1 + std::exp(calc_expo1));
          }

          const double Ixz_ui = mutualInformation.score(x, z, ui);
          const double Iyz_ui = mutualInformation.score(y, z, ui);

          calc_expo1        = -(Ixz_ui - Ixy_ui) * M_LN2;
          double calc_expo2 = -(Iyz_ui - Ixy_ui) * M_LN2;

          if (calc_expo1 > _maxLog_ || calc_expo2 > _maxLog_) {
            Pb = 0.0;
          } else if (calc_expo1 < -_maxLog_ && calc_expo2 < -_maxLog_) {
            Pb = 1.0;
          } else {
            double expo1, expo2;
            expo1 = (calc_expo1 < -_maxLog_) ? 0.0 : std::exp(calc_expo1);
            expo2 = (calc_expo2 < -_maxLog_) ? 0.0 : std::exp(calc_expo2);
            Pb    = 1 / (1 + expo1 + expo2);
          }

          const double min_pnv_pb = std::min(Pnv, Pb);
          if (min_pnv_pb > maxP) {
            maxP = min_pnv_pb;
            maxZ = z;
          }
        }
      }
      CondRanking final;
      auto        tup  = new CondThreePoints{x, y, maxZ, ui};
      final.first      = tup;
      final.second     = maxP;
      rank.insert(final);
    }

    // ##########################################################################
    // Unshielded triples (MIIC probabilistic version)
    // ##########################################################################

    std::vector< ProbabilisticRanking > Miic::unshieldedTriplesMiic_(
        const MixedGraph&                                                      graph,
        CorrectedMutualInformation&                                            mutualInformation,
        const HashTable< std::pair< NodeId, NodeId >, std::vector< NodeId > >& sepSet,
        HashTable< std::pair< NodeId, NodeId >, char >&                        marks) {
      std::vector< ProbabilisticRanking > triples;
      for (NodeId z: graph) {
        for (NodeId x: graph.neighbours(z)) {
          for (NodeId y: graph.neighbours(z)) {
            if (y < x && !graph.existsEdge(x, y)) {
              std::vector< NodeId >       ui;
              std::pair< NodeId, NodeId > key     = {x, y};
              std::pair< NodeId, NodeId > rev_key = {y, x};
              if (sepSet.exists(key)) {
                ui = sepSet[key];
              } else if (sepSet.exists(rev_key)) {
                ui = sepSet[rev_key];
              }
              const auto iter_z_place = std::find(ui.begin(), ui.end(), z);
              if (iter_z_place != ui.end()) ui.erase(iter_z_place);

              const double         Ixyz_ui = mutualInformation.score(x, y, z, ui);
              auto                 tup     = new ThreePoints{x, y, z};
              ProbabilisticRanking triple{tup, Ixyz_ui, 0.5, 0.5};
              triples.push_back(triple);
              if (!marks.exists({x, z})) marks.insert({x, z}, 'o');
              if (!marks.exists({z, x})) marks.insert({z, x}, 'o');
              if (!marks.exists({y, z})) marks.insert({y, z}, 'o');
              if (!marks.exists({z, y})) marks.insert({z, y}, 'o');
            }
          }
        }
      }
      triples = updateProbaTriples_(graph, triples);
      std::sort(triples.begin(), triples.end(), GreaterTupleOnLast());
      return triples;
    }

    std::vector< ProbabilisticRanking >
        Miic::updateProbaTriples_(const MixedGraph&                   graph,
                                  std::vector< ProbabilisticRanking > probaTriples) {
      for (auto& triple: probaTriples) {
        NodeId x, y, z;
        x                 = std::get< 0 >(*std::get< 0 >(triple));
        y                 = std::get< 1 >(*std::get< 0 >(triple));
        z                 = std::get< 2 >(*std::get< 0 >(triple));
        const double Ixyz = std::get< 1 >(triple);
        double       Pxz  = std::get< 2 >(triple);
        double       Pyz  = std::get< 3 >(triple);

        if (Ixyz <= 0) {
          const double expo = std::exp(Ixyz);
          const double P0   = (1 + expo) / (1 + 3 * expo);
          if (Pxz == Pyz && Pyz == 0.5) {
            std::get< 2 >(triple) = P0;
            std::get< 3 >(triple) = P0;
          } else {
            if (graph.existsArc(x, z) && Pxz >= P0) {
              std::get< 3 >(triple) = Pxz * (1 / (1 + expo) - 0.5) + 0.5;
            } else if (graph.existsArc(y, z) && Pyz >= P0) {
              std::get< 2 >(triple) = Pyz * (1 / (1 + expo) - 0.5) + 0.5;
            }
          }
        } else {
          const double expo = std::exp(-Ixyz);
          if (graph.existsArc(x, z) && Pxz >= 0.5) {
            std::get< 3 >(triple) = Pxz * (1 / (1 + expo) - 0.5) + 0.5;
          } else if (graph.existsArc(y, z) && Pyz >= 0.5) {
            std::get< 2 >(triple) = Pyz * (1 / (1 + expo) - 0.5) + 0.5;
          }
        }
      }
      std::sort(probaTriples.begin(), probaTriples.end(), GreaterTupleOnLast());
      return probaTriples;
    }

    // ##########################################################################
    // Helpers
    // ##########################################################################

    bool Miic::_isNotLatentCouple_(const NodeId x, const NodeId y) {
      const auto& lbeg = _latentCouples_.begin();
      const auto& lend = _latentCouples_.end();
      return (std::find(lbeg, lend, Arc(x, y)) == lend)
          && (std::find(lbeg, lend, Arc(y, x)) == lend);
    }

  } /* namespace learning */

} /* namespace gum */
