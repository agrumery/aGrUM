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
 * @brief Implementation of gum::learning::ConstraintBasedLearning
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/base/core/hashTable.h>
#include <agrum/base/core/heap.h>
#include <agrum/base/graphs/mixedGraph.h>
#include <agrum/BN/learning/ConstraintBasedLearning.h>

namespace gum {

  namespace learning {

    // ##########################################################################
    // Constructors / Destructors
    // ##########################################################################

    ConstraintBasedLearning::ConstraintBasedLearning() : _maxLog_(100) {
      GUM_CONSTRUCTOR(ConstraintBasedLearning);
    }

    ConstraintBasedLearning::ConstraintBasedLearning(int maxLog) : _maxLog_(maxLog) {
      GUM_CONSTRUCTOR(ConstraintBasedLearning);
    }

    ConstraintBasedLearning::ConstraintBasedLearning(const ConstraintBasedLearning& from) :
        ApproximationScheme(from), _forbiddenGraph_(from._forbiddenGraph_),
        _mandatoryGraph_(from._mandatoryGraph_), _initialMarks_(from._initialMarks_),
        _latentCouples_(from._latentCouples_), _maxLog_(from._maxLog_),
        _maxIndegree_(from._maxIndegree_) {
      GUM_CONS_CPY(ConstraintBasedLearning);
    }

    ConstraintBasedLearning::ConstraintBasedLearning(ConstraintBasedLearning&& from) :
        ApproximationScheme(std::move(from)), _forbiddenGraph_(std::move(from._forbiddenGraph_)),
        _mandatoryGraph_(std::move(from._mandatoryGraph_)),
        _initialMarks_(std::move(from._initialMarks_)),
        _latentCouples_(std::move(from._latentCouples_)), _maxLog_(from._maxLog_),
        _maxIndegree_(from._maxIndegree_) {
      GUM_CONS_MOV(ConstraintBasedLearning);
    }

    ConstraintBasedLearning::~ConstraintBasedLearning() { GUM_DESTRUCTOR(ConstraintBasedLearning); }

    ConstraintBasedLearning&
        ConstraintBasedLearning::operator=(const ConstraintBasedLearning& from) {
      ApproximationScheme::operator=(from);
      _forbiddenGraph_ = from._forbiddenGraph_;
      _mandatoryGraph_ = from._mandatoryGraph_;
      _maxIndegree_    = from._maxIndegree_;
      _initialMarks_   = from._initialMarks_;
      _latentCouples_  = from._latentCouples_;
      _maxLog_         = from._maxLog_;
      return *this;
    }

    ConstraintBasedLearning& ConstraintBasedLearning::operator=(ConstraintBasedLearning&& from) {
      ApproximationScheme::operator=(std::move(from));
      _forbiddenGraph_ = std::move(from._forbiddenGraph_);
      _mandatoryGraph_ = std::move(from._mandatoryGraph_);
      _maxIndegree_    = from._maxIndegree_;
      _initialMarks_   = std::move(from._initialMarks_);
      _latentCouples_  = std::move(from._latentCouples_);
      _maxLog_         = from._maxLog_;
      return *this;
    }

    // ##########################################################################
    // Comparator operators
    // ##########################################################################

    bool GreaterPairOn2nd::operator()(const CondRanking& e1, const CondRanking& e2) const {
      return e1.second > e2.second;
    }

    bool GreaterAbsPairOn2nd::operator()(const Ranking& e1, const Ranking& e2) const {
      return std::abs(e1.second) > std::abs(e2.second);
    }

    bool GreaterTupleOnLast::operator()(const ProbabilisticRanking& e1,
                                        const ProbabilisticRanking& e2) const {
      double p1xz = std::get< 2 >(e1);
      double p1yz = std::get< 3 >(e1);
      double p2xz = std::get< 2 >(e2);
      double p2yz = std::get< 3 >(e2);
      double I1   = std::get< 1 >(e1);
      double I2   = std::get< 1 >(e2);
      if ((I1 < 0 && I2 < 0) || (I1 >= 0 && I2 >= 0)) {
        if (std::max(p1xz, p1yz) == std::max(p2xz, p2yz)) {
          return std::abs(I1) > std::abs(I2);
        } else {
          return std::max(p1xz, p1yz) > std::max(p2xz, p2yz);
        }
      } else {
        return I1 < I2;
      }
    }

    // ##########################################################################
    // Constraint setters
    // ##########################################################################

    void ConstraintBasedLearning::setMandatoryGraph(const gum::DAG mandaGraph) {
      _mandatoryGraph_ = mandaGraph;
    }

    void ConstraintBasedLearning::setForbiddenGraph(const gum::DiGraph forbidGraph) {
      _forbiddenGraph_ = forbidGraph;
    }

    void ConstraintBasedLearning::setMaxIndegree(gum::Size n) { _maxIndegree_ = n; }

    void ConstraintBasedLearning::addConstraints(
        HashTable< std::pair< NodeId, NodeId >, char > constraints) {
      _initialMarks_ = constraints;
    }

    const std::vector< Arc > ConstraintBasedLearning::latentVariables() const {
      return _latentCouples_;
    }

    // ##########################################################################
    // Template methods
    // ##########################################################################

    PDAG ConstraintBasedLearning::learnPDAG(CorrectedMutualInformation& I, MixedGraph graph) {
      return meekRules_.propagateToCPDAG(learnMixedStructure(I, graph));
    }

    DAG ConstraintBasedLearning::learnStructure(CorrectedMutualInformation& I, MixedGraph graph) {
      return meekRules_.propagateToDAG(learnMixedStructure(I, graph));
    }

    // ##########################################################################
    // Constraint checks
    // ##########################################################################

    bool ConstraintBasedLearning::isForbiddenArc_(NodeId x, NodeId y) const {
      return _forbiddenGraph_.existsArc(x, y);
    }

    bool ConstraintBasedLearning::isForbiddenEdge_(NodeId x, NodeId y) {
      return _forbiddenGraph_.existsArc(x, y) && _forbiddenGraph_.existsArc(y, x);
    }

    bool ConstraintBasedLearning::isMaxIndegree_(const MixedGraph& graph, NodeId x) {
      return graph.parents(x).size() >= _maxIndegree_;
    }

    bool ConstraintBasedLearning::isArcValid_(const MixedGraph& graph, NodeId x, NodeId y) {
      return !isForbiddenArc_(x, y) && !isMaxIndegree_(graph, y);
    }

    // ##########################################################################
    // Graph utilities
    // ##########################################################################

    bool ConstraintBasedLearning::_existsNonTrivialDirectedPath_(const MixedGraph& graph,
                                                                 const NodeId      n1,
                                                                 const NodeId      n2) {
      for (const auto parent: graph.parents(n2)) {
        if (graph.existsArc(n2, parent)) continue;   // double arc — skip
        if (parent == n1) continue;                  // trivial path — not counted
        if (_existsDirectedPath_(graph, n1, parent)) return true;
      }
      return false;
    }

    bool ConstraintBasedLearning::_existsDirectedPath_(const MixedGraph& graph,
                                                       const NodeId      n1,
                                                       const NodeId      n2) {
      List< NodeId > nodeFIFO;
      Set< NodeId >  mark;

      mark.insert(n2);
      nodeFIFO.pushBack(n2);

      NodeId current;
      while (!nodeFIFO.empty()) {
        current = nodeFIFO.front();
        nodeFIFO.popFront();
        for (const auto new_one: graph.parents(current)) {
          if (graph.existsArc(current, new_one)) continue;   // double arc — skip
          if (new_one == n1) return true;
          if (mark.exists(new_one)) continue;
          mark.insert(new_one);
          nodeFIFO.pushBack(new_one);
        }
      }
      return false;
    }

    void ConstraintBasedLearning::orientDoubleHeadedArcs_(MixedGraph& mg) {
      gum::ArcSet L;
      for (gum::NodeId x: mg.nodes())
        for (NodeId y: mg.parents(x))
          if (mg.parents(y).contains(x)) {
            if (x > y) continue;
            else L.insert(gum::Arc(x, y));
          }

      if (not L.empty()) {
        while (true) {
          bool withdrawFlag_L = false;
          for (auto arc: ArcSet(L)) {
            bool tail_head        = _existsDirectedPath_(mg, arc.tail(), arc.head());
            bool head_tail        = _existsDirectedPath_(mg, arc.head(), arc.tail());
            bool withdrawFlag_arc = false;

            if (tail_head && !head_tail) {
              mg.eraseArc(Arc(arc.head(), arc.tail()));
              withdrawFlag_arc = true;
            } else if (!tail_head && head_tail) {
              mg.eraseArc(Arc(arc.tail(), arc.head()));
              withdrawFlag_arc = true;
            } else if (!tail_head && !head_tail) {
              mg.eraseArc(Arc(arc.head(), arc.tail()));
              withdrawFlag_arc = true;
            }

            if (withdrawFlag_arc) {
              L.erase(arc);
              withdrawFlag_L = true;
            }
          }
          if (L.empty()) break;

          // no arc was processed this pass — break cycle by erasing one arc arbitrarily
          if (!withdrawFlag_L) {
            auto arc = *L.begin();
            mg.eraseArc(Arc(arc.head(), arc.tail()));
            mg.eraseArc(Arc(arc.tail(), arc.head()));
            L.erase(arc);
          }
        }
      }
    }

    std::vector< Ranking > ConstraintBasedLearning::unshieldedTriples_(
        const MixedGraph&                                                      graph,
        CorrectedMutualInformation&                                            mutualInformation,
        const HashTable< std::pair< NodeId, NodeId >, std::vector< NodeId > >& sepSet) {
      std::vector< Ranking > triples;
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

              double  Ixyz_ui = mutualInformation.score(x, y, z, ui);
              Ranking triple;
              auto    tup   = new ThreePoints{x, y, z};
              triple.first  = tup;
              triple.second = Ixyz_ui;
              triples.push_back(triple);
            }
          }
        }
      }
      std::sort(triples.begin(), triples.end(), GreaterAbsPairOn2nd());
      return triples;
    }

  } /* namespace learning */

} /* namespace gum */
