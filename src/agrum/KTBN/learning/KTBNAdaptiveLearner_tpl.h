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


#pragma once


/** @file
 * @brief Implementation of the KTBNAdaptiveLearner class.
 *
 * @author Seth AGUILA & Anis KHACEF
 */

#include <cmath>
#include <filesystem>
#include <format>
#include <fstream>
#include <memory>

#include <agrum/base/database/CSVParser.h>
#include <agrum/base/multidim/instantiation.h>
#include <agrum/BN/learning/priors/prior.h>
#include <agrum/BN/learning/scores/scoreAIC.h>
#include <agrum/BN/learning/scores/scoreBD.h>
#include <agrum/BN/learning/scores/scoreBDeu.h>
#include <agrum/BN/learning/scores/scoreBIC.h>
#include <agrum/BN/learning/scores/scorefNML.h>
#include <agrum/BN/learning/scores/scoreLog2Likelihood.h>
#include <agrum/BN/learning/scores/scoreMDL.h>
#include <agrum/KTBN/learning/KTBNAdaptiveLearner.h>

#include <unordered_map>

namespace gum::learning {

  // =========================================================================
  // Constructors / Destructors
  // =========================================================================

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >::KTBNAdaptiveLearner(
      std::string_view                         dirPath,
      std::string_view                         csvBaseName,
      Size                                     nbSamples,
      Size                                     kMax,
      const std::unordered_set< std::string >& atemporalVars,
      const std::vector< std::string >&        missingSymbols,
      bool                                     induceTypes) :
      _dirPath_(dirPath), _csvBaseName_(csvBaseName), _nbSamples_(nbSamples), _kMax_(kMax),
      _atemporalVars_(atemporalVars), _missingSymbols_(missingSymbols), _induceTypes_(induceTypes) {
    // cheap argument checks first, so bad calls fail before touching the disk
    IKTBNLearner< GUM_SCALAR >::_checkMinimalOrder_(kMax, "kMax");
    if (nbSamples < 1) GUM_ERROR(InvalidArgument, "KTBNAdaptiveLearner requires nbSamples >= 1")

    // Load the base-variable names (the CSV columns) so the constraint setters can
    // reject unknown names eagerly. Only the header row is needed, so we parse a
    // single line instead of loading and typing the whole file. The column order
    // matches what every per-k KTBNLearner reads from the same header at
    // learnKTBN() time.
    namespace fs            = std::filesystem;
    const fs::path firstCSV = fs::path{dirPath} / (std::string{csvBaseName} + "1.csv");
    std::ifstream  is(firstCSV, std::ifstream::in);
    if (!is.is_open()) GUM_ERROR(IOError, "Cannot open " << firstCSV.string())

    CSVParser parser(is, firstCSV.string());
    if (!parser.next())
      GUM_ERROR(IOError, "Empty trajectory file (no header row): " << firstCSV.string())
    const std::vector< std::string >& header = parser.current();
    _baseNames_.insert(header.begin(), header.end());

    // every declared atemporal variable must actually be one of those columns
    for (const std::string& aname: _atemporalVars_)
      if (!_baseNames_.contains(aname))
        GUM_ERROR(InvalidArgument,
                  "atemporal variable '" << aname << "' not found in the CSV header")

    GUM_CONSTRUCTOR(KTBNAdaptiveLearner)
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >::KTBNAdaptiveLearner(
      std::string_view                  dirPath,
      std::string_view                  csvBaseName,
      Size                              nbSamples,
      Size                              kMax,
      const std::vector< std::string >& missingSymbols,
      bool                              induceTypes) :
      // _inferAtemporalVars_ checks kMax>=2 and nbSamples>=1 itself, before
      // opening anything (see its declaration) — this initialiser-list call
      // runs before the delegated-to constructor's own body, so those checks
      // cannot be left to that body the way the explicit constructor does.
      // Delegates to the explicit-atemporalVars constructor for the rest
      // (header read); _inferAtemporalVars_ needs nbSamples, unlike the
      // header-only read above, since one trajectory alone cannot show that a
      // value stays constant.
      KTBNAdaptiveLearner(
          dirPath,
          csvBaseName,
          nbSamples,
          kMax,
          _inferAtemporalVars_(dirPath, csvBaseName, nbSamples, kMax, missingSymbols),
          missingSymbols,
          induceTypes) {}

  template < GUM_Numeric GUM_SCALAR >
  std::unordered_set< std::string > KTBNAdaptiveLearner< GUM_SCALAR >::_inferAtemporalVars_(
      std::string_view                  dirPath,
      std::string_view                  csvBaseName,
      Size                              nbSamples,
      Size                              kMax,
      const std::vector< std::string >& missingSymbols) {
    IKTBNLearner< GUM_SCALAR >::_checkMinimalOrder_(kMax, "kMax");
    if (nbSamples < 1) GUM_ERROR(InvalidArgument, "KTBNAdaptiveLearner requires nbSamples >= 1")
    return IKTBNLearner< GUM_SCALAR >::_scanConstantColumns_(dirPath,
                                                             csvBaseName,
                                                             nbSamples,
                                                             missingSymbols);
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >::KTBNAdaptiveLearner(
      std::string_view                         dirPath,
      std::string_view                         csvBaseName,
      Size                                     nbSamples,
      Size                                     kMax,
      const BayesNet< GUM_SCALAR >&            bn,
      const std::unordered_set< std::string >& atemporalVars,
      const std::vector< std::string >&        missingSymbols) :
      _dirPath_(dirPath), _csvBaseName_(csvBaseName), _nbSamples_(nbSamples), _kMax_(kMax),
      _atemporalVars_(atemporalVars), _missingSymbols_(missingSymbols), _induceTypes_(false),
      _prior_bn_(std::make_unique< BayesNet< GUM_SCALAR > >(bn)) {
    IKTBNLearner< GUM_SCALAR >::_checkMinimalOrder_(kMax, "kMax");
    if (nbSamples < 1) GUM_ERROR(InvalidArgument, "KTBNAdaptiveLearner requires nbSamples >= 1")

    // the schema BN is the authoritative variable universe here (not the CSV
    // header): take the base names from it, so the constraint setters validate
    // against the same variables every per-k KTBNLearner will use. The CSV is
    // read only later, at learnKTBN() time.
    for (const NodeId node: bn.nodes())
      _baseNames_.insert(bn.variable(node).name());

    // every declared atemporal variable must be a node of the schema BN
    for (const std::string& aname: _atemporalVars_)
      if (!_baseNames_.contains(aname))
        GUM_ERROR(InvalidArgument,
                  "atemporal variable '" << aname << "' not found in the schema BN")

    GUM_CONSTRUCTOR(KTBNAdaptiveLearner)
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >::~KTBNAdaptiveLearner() {
    GUM_DESTRUCTOR(KTBNAdaptiveLearner)
  }

  // =========================================================================
  // Main learning methods
  // =========================================================================

  template < GUM_Numeric GUM_SCALAR >
  KTBN< GUM_SCALAR > KTBNAdaptiveLearner< GUM_SCALAR >::learnKTBN() {
    // Explore every candidate order _kMin_.._kMax_ and keep the k-TBN with the
    // best cross-k model-selection score (BIC by default; see useOrderScore*).
    // _kMin_ -- 2 unless raised by a constraint naming a concrete slice, see
    // _recomputeKMin_ -- skips candidates that would only drop that constraint.
    // Each candidate is a full fixed-k problem: a fresh KTBNLearner on the same
    // trajectories, replaying the recorded configuration and constraints (those
    // whose slices do not fit that k are dropped by _applyConstraints_).
    // Reset first: a re-entry, or a throw below, must not leave a stale selection
    // readable through bestK() / latentVariables().
    _bestK_ = 0;
    _bestLatentVariables_.clear();
    _scorePerCandidateK_.clear();
    KTBN< GUM_SCALAR > best;
    Size               bestK = 0;   // local; committed to _bestK_ only if the whole sweep succeeds
    double             bestScore = 0.0;                // unread until the first candidate sets it
    double             logN      = 0.0;                // log2 of the total number of observations
    std::vector< std::pair< Size, double > > scores;   // per-k scores, committed at the end
    std::vector< std::pair< std::string, std::string > >
        bestLatents;   // winner's latent pairs, committed at the end

    for (Size k = _kMin_; k <= _kMax_; ++k) {
      // build this candidate's fixed-k learner: from the schema BN when one was
      // supplied (domains fixed explicitly), else from the CSV (domains inferred).
      // KTBNLearner is non-movable, so hold it through a unique_ptr.
      std::unique_ptr< KTBNLearner< GUM_SCALAR > > learnerPtr
          = _prior_bn_ ? std::make_unique< KTBNLearner< GUM_SCALAR > >(_dirPath_,
                                                                       _csvBaseName_,
                                                                       _nbSamples_,
                                                                       k,
                                                                       *_prior_bn_,
                                                                       _atemporalVars_,
                                                                       _missingSymbols_,
                                                                       _ignoreMissingSymbols_)
                       : std::make_unique< KTBNLearner< GUM_SCALAR > >(_dirPath_,
                                                                       _csvBaseName_,
                                                                       _nbSamples_,
                                                                       k,
                                                                       _atemporalVars_,
                                                                       _missingSymbols_,
                                                                       _induceTypes_,
                                                                       _ignoreMissingSymbols_);
      KTBNLearner< GUM_SCALAR >& learner = *learnerPtr;

      // Refused here rather than deep inside structure learning: learnDag_ would
      // otherwise raise its own generic message only after this candidate's
      // databases had been built. Every candidate reads the same trajectories, so
      // the verdict on the first holds for all.
      if (!_ignoreMissingSymbols_ && learner.hasMissingValues())
        GUM_ERROR(MissingValueInDatabase,
                  "the trajectories contain missing values. Neither aGrUM's structure learning "
                  "nor the cross-k order score can evaluate an incomplete window. Call "
                  "ignoreMissingSymbols() to learn and score on the fully observed data only "
                  "(see its warning: dropping skews the selection towards larger k).")

      // sample size for the BIC penalty: the total number of time-slice rows over
      // all trajectories. Independent of k, so it is computed once on the first
      // candidate and reused — it must stay fixed for the scores to compare.
      if (logN == 0.0) {
        Size nbObs = 0;
        for (const Size len: learner.nbRows())
          nbObs += len;
        logN = std::log2(nbObs);
      }

      _applyConfig_(learner);
      _applyConstraints_(learner, k);

      KTBN< GUM_SCALAR > candidate = learner.learnKTBN();

      // Cross-k model selection by the recorded order score (BIC by default):
      // it must be comparable across k, unlike the per-k structure score, so it
      // is computed here on the whole learned k-TBN rather than read off the
      // internal learners. bestK == 0 means nothing is selected yet, so the
      // first candidate is always taken; a strict '>' afterwards keeps the
      // smallest k on ties (mild parsimony).
      const double score = _orderSelectionScore_(candidate, logN);
      scores.emplace_back(k, score);
      if (bestK == 0 || score > bestScore) {
        bestScore = score;
        best      = std::move(candidate);
        bestK     = k;
        // capture the winner's latent-variable annotations while its learner is
        // still alive (only MIIC produces them; other algos leave the set empty)
        bestLatents = (_algo_ == IBNLearner::AlgoType::MIIC)
                        ? learner.latentVariables()
                        : std::vector< std::pair< std::string, std::string > >{};
      }
    }

    // Commit only after every candidate has succeeded. An exception thrown above
    // (e.g. a trajectory too short for some candidate k) leaves the members at
    // their reset state, so bestK() / latentVariables() / scorePerCandidateK()
    // keep reporting "not learned" — the all-or-nothing guarantee promised above.
    _bestK_               = bestK;
    _scorePerCandidateK_  = std::move(scores);
    _bestLatentVariables_ = std::move(bestLatents);
    return best;
  }

  template < GUM_Numeric GUM_SCALAR >
  Size KTBNAdaptiveLearner< GUM_SCALAR >::bestK() const {
    // _bestK_ stays at its sentinel 0 (never a valid order: candidates are >= 2)
    // until learnKTBN() selects one, so 0 means no learning has run yet.
    if (_bestK_ == 0)
      GUM_ERROR(OperationNotAllowed,
                "bestK() is undefined: call learnKTBN() before querying the selected order.")
    return _bestK_;
  }

  template < GUM_Numeric GUM_SCALAR >
  const std::vector< std::pair< std::string, std::string > >&
      KTBNAdaptiveLearner< GUM_SCALAR >::latentVariables() const {
    // gated on the same sentinel as bestK(): 0 means learnKTBN() has not run.
    if (_bestK_ == 0)
      GUM_ERROR(OperationNotAllowed, "latentVariables() is undefined: call learnKTBN() first.")
    return _bestLatentVariables_;
  }

  template < GUM_Numeric GUM_SCALAR >
  const std::vector< std::pair< Size, double > >&
      KTBNAdaptiveLearner< GUM_SCALAR >::scorePerCandidateK() const {
    // same sentinel gate as bestK()/latentVariables(): 0 means learnKTBN() has not run.
    if (_bestK_ == 0)
      GUM_ERROR(
          OperationNotAllowed,
          "scorePerCandidateK() is undefined: call learnKTBN() before querying the per-k scores.")
    return _scorePerCandidateK_;
  }

  // =========================================================================
  // Diagnostics
  // =========================================================================

  template < GUM_Numeric GUM_SCALAR >
  Size KTBNAdaptiveLearner< GUM_SCALAR >::kMax() const {
    return _kMax_;
  }

  template < GUM_Numeric GUM_SCALAR >
  std::string KTBNAdaptiveLearner< GUM_SCALAR >::checkScorePriorCompatibility() const {
    // MIIC is constraint-based: it consumes a correction, not a score, so no
    // score/prior clash is possible. (Mirrors IBNLearner::isConstraintBased.)
    if (_algo_ == IBNLearner::AlgoType::MIIC) return "";

    // map the recorded prior to the internal PriorType. The adaptive learner only
    // ever records NO_prior or SMOOTHING (its sole prior setter is useSmoothingPrior).
    const PriorType prior = (_prior_ == IBNLearner::BNLearnerPriorType::SMOOTHING)
                              ? PriorType::SmoothingPriorType
                              : PriorType::NoPriorType;

    // delegate to the very same per-score static checks BNLearner uses
    switch (_score_) {
      case IBNLearner::ScoreType::AIC : return ScoreAIC::isPriorCompatible(prior, _priorWeight_);
      case IBNLearner::ScoreType::BD : return ScoreBD::isPriorCompatible(prior, _priorWeight_);
      case IBNLearner::ScoreType::BDeu : return ScoreBDeu::isPriorCompatible(prior, _priorWeight_);
      case IBNLearner::ScoreType::BIC : return ScoreBIC::isPriorCompatible(prior, _priorWeight_);
      case IBNLearner::ScoreType::fNML : return ScorefNML::isPriorCompatible(prior, _priorWeight_);
      case IBNLearner::ScoreType::LOG2LIKELIHOOD :
        return ScoreLog2Likelihood::isPriorCompatible(prior, _priorWeight_);
      case IBNLearner::ScoreType::MDL : return ScoreMDL::isPriorCompatible(prior, _priorWeight_);
      default : return "";   // K2 is never recorded by this learner's setters
    }
  }

  template < GUM_Numeric GUM_SCALAR >
  std::vector< std::tuple< std::string, std::string, std::string > >
      KTBNAdaptiveLearner< GUM_SCALAR >::state() const {
    std::vector< std::tuple< std::string, std::string, std::string > > vals;
    const auto add = [&](std::string k, std::string v, std::string c = "") {
      vals.emplace_back(std::move(k), std::move(v), std::move(c));
    };
    // render a set of engine-name pairs as "{a->b, c->d}"
    const auto arcs = [](const std::set< std::pair< std::string, std::string > >& s) {
      std::string r     = "{";
      bool        first = true;
      for (const auto& [a, b]: s) {
        if (!first) r += ", ";
        first = false;
        r += a + "->" + b;
      }
      return r + "}";
    };
    const auto names = [](const std::set< std::string >& s) {
      std::string r     = "{";
      bool        first = true;
      for (const auto& n: s) {
        if (!first) r += ", ";
        first = false;
        r += n;
      }
      return r + "}";
    };
    // render a set of (tailBase, headBase, lag) triples as "{a->b (lag 1), ...}"
    const auto kernelArcs = [](const std::set< std::tuple< std::string, std::string, int > >& s) {
      std::string r     = "{";
      bool        first = true;
      for (const auto& [a, b, lag]: s) {
        if (!first) r += ", ";
        first = false;
        r += a + "->" + b + " (lag " + std::to_string(lag) + ")";
      }
      return r + "}";
    };

    add("Candidate orders", std::to_string(_kMin_) + ".." + std::to_string(_kMax_));
    add("Selected k", _bestK_ == 0 ? "not learned yet" : std::to_string(_bestK_));
    add("Base variables", std::to_string(_baseNames_.size()));
    add("Atemporal variables", names({_atemporalVars_.begin(), _atemporalVars_.end()}));

    switch (_algo_) {
      case IBNLearner::AlgoType::MIIC : add("Algorithm", "MIIC"); break;
      case IBNLearner::AlgoType::GREEDY_HILL_CLIMBING :
        add("Algorithm", "Greedy Hill Climbing");
        break;
      case IBNLearner::AlgoType::EXTENDED_GREEDY_HILL_CLIMBING :
        add("Algorithm", "Extended Greedy Hill Climbing");
        break;
      case IBNLearner::AlgoType::LOCAL_SEARCH_WITH_TABU_LIST :
        add("Algorithm", "Local Search with Tabu List");
        add("Tabu list size", std::to_string(_tabuSize_));
        add("Tabu nb decrease", std::to_string(_nbDecrease_));
        break;
      default : add("Algorithm", "(other)"); break;
    }

    if (_algo_ == IBNLearner::AlgoType::MIIC) {
      switch (_correction_) {
        case CorrectedMutualInformation::KModeTypes::MDL : add("Correction", "MDL"); break;
        case CorrectedMutualInformation::KModeTypes::NML : add("Correction", "NML"); break;
        case CorrectedMutualInformation::KModeTypes::NoCorr :
          add("Correction", "No correction");
          break;
      }
    } else {
      switch (_score_) {
        case IBNLearner::ScoreType::AIC : add("Structure score", "AIC"); break;
        case IBNLearner::ScoreType::BD : add("Structure score", "BD"); break;
        case IBNLearner::ScoreType::BDeu : add("Structure score", "BDeu"); break;
        case IBNLearner::ScoreType::BIC : add("Structure score", "BIC"); break;
        case IBNLearner::ScoreType::fNML : add("Structure score", "fNML"); break;
        case IBNLearner::ScoreType::LOG2LIKELIHOOD :
          add("Structure score", "Log2Likelihood");
          break;
        case IBNLearner::ScoreType::MDL : add("Structure score", "MDL"); break;
        default : add("Structure score", "(other)"); break;
      }
    }

    // the outer, cross-k criterion — distinct from the per-k structure score above
    switch (_orderScore_) {
      case OrderScoreType::BIC : add("Order selection score", "BIC"); break;
      case OrderScoreType::AIC : add("Order selection score", "AIC"); break;
      case OrderScoreType::fNML : add("Order selection score", "fNML"); break;
    }

    if (_prior_ == IBNLearner::BNLearnerPriorType::SMOOTHING) {
      add("Prior", "Smoothing", checkScorePriorCompatibility());
      add("Prior weight", std::to_string(_priorWeight_));
    } else {
      add("Prior", "no prior is set", checkScorePriorCompatibility());
    }

    if (!_forbiddenArcs_.empty()) add("Forbidden arcs", arcs(_forbiddenArcs_));
    if (!_mandatoryArcs_.empty()) add("Mandatory arcs", arcs(_mandatoryArcs_));
    if (!_forbiddenKernelArcs_.empty())
      add("Forbidden kernel arcs", kernelArcs(_forbiddenKernelArcs_));
    if (!_mandatoryKernelArcs_.empty())
      add("Mandatory kernel arcs", kernelArcs(_mandatoryKernelArcs_));
    if (!_possibleEdges_.empty()) add("Possible edges", arcs(_possibleEdges_));
    if (!_forbiddenIntraSliceArcs_.empty())
      add("Forbidden intra-slice arcs", arcs(_forbiddenIntraSliceArcs_));
    if (!_forbiddenArcsAllSlices_.empty())
      add("Forbidden all-slices arcs", arcs(_forbiddenArcsAllSlices_));
    if (!_noParentNodes_.empty()) add("No-parent nodes", names(_noParentNodes_));
    if (!_noChildrenNodes_.empty()) add("No-children nodes", names(_noChildrenNodes_));
    if (_maxIndegree_ != std::numeric_limits< Size >::max())
      add("Max in-degree", std::to_string(_maxIndegree_));
    if (!_allowAdditions_) add("Arc additions", "forbidden");
    if (!_allowDeletions_) add("Arc deletions", "forbidden");
    if (!_allowReversals_) add("Arc reversals", "forbidden");

    return vals;
  }

  template < GUM_Numeric GUM_SCALAR >
  std::string KTBNAdaptiveLearner< GUM_SCALAR >::toString() const {
    // aligned "key : value  (comment)" listing, same layout as BNLearner::toString
    const auto  st     = state();
    std::size_t maxkey = 0;
    for (const auto& t: st)
      maxkey = std::max(maxkey, std::get< 0 >(t).length());

    std::string s;
    for (const auto& t: st) {
      s += std::format("{:<{}} : {}", std::get< 0 >(t), maxkey, std::get< 1 >(t));
      if (!std::get< 2 >(t).empty()) s += std::format("  ({})", std::get< 2 >(t));
      s += '\n';
    }
    return s;
  }

  // =========================================================================
  // Score selection
  // =========================================================================

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >& KTBNAdaptiveLearner< GUM_SCALAR >::useScoreAIC() {
    _score_ = IBNLearner::ScoreType::AIC;
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >& KTBNAdaptiveLearner< GUM_SCALAR >::useScoreBD() {
    _score_ = IBNLearner::ScoreType::BD;
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >& KTBNAdaptiveLearner< GUM_SCALAR >::useScoreBDeu() {
    _score_ = IBNLearner::ScoreType::BDeu;
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >& KTBNAdaptiveLearner< GUM_SCALAR >::useScoreBIC() {
    _score_ = IBNLearner::ScoreType::BIC;
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >& KTBNAdaptiveLearner< GUM_SCALAR >::useScoreLog2Likelihood() {
    _score_ = IBNLearner::ScoreType::LOG2LIKELIHOOD;
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >& KTBNAdaptiveLearner< GUM_SCALAR >::useScoreMDL() {
    _score_ = IBNLearner::ScoreType::MDL;
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBNAdaptiveLearner< GUM_SCALAR >::useScorefNML() {
    _score_ = IBNLearner::ScoreType::fNML;
  }

  // =========================================================================
  // Order selection (cross-k model selection)
  // =========================================================================

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >& KTBNAdaptiveLearner< GUM_SCALAR >::useOrderScoreBIC() {
    _orderScore_ = OrderScoreType::BIC;
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >& KTBNAdaptiveLearner< GUM_SCALAR >::useOrderScoreAIC() {
    _orderScore_ = OrderScoreType::AIC;
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >& KTBNAdaptiveLearner< GUM_SCALAR >::useOrderScorefNML() {
    _orderScore_ = OrderScoreType::fNML;
    return *this;
  }

  // =========================================================================
  // Missing values
  // =========================================================================

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >&
      KTBNAdaptiveLearner< GUM_SCALAR >::ignoreMissingSymbols(bool ignore) {
    _ignoreMissingSymbols_ = ignore;
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  INLINE bool KTBNAdaptiveLearner< GUM_SCALAR >::isIgnoringMissingSymbols() const {
    return _ignoreMissingSymbols_;
  }

  // =========================================================================
  // Algorithm selection
  // =========================================================================

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >& KTBNAdaptiveLearner< GUM_SCALAR >::useGreedyHillClimbing() {
    _algo_ = IBNLearner::AlgoType::GREEDY_HILL_CLIMBING;
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >&
      KTBNAdaptiveLearner< GUM_SCALAR >::useExtendedGreedyHillClimbing() {
    _algo_ = IBNLearner::AlgoType::EXTENDED_GREEDY_HILL_CLIMBING;
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >&
      KTBNAdaptiveLearner< GUM_SCALAR >::useLocalSearchWithTabuList(Size tabu_size,
                                                                    Size nb_decrease) {
    _algo_       = IBNLearner::AlgoType::LOCAL_SEARCH_WITH_TABU_LIST;
    _tabuSize_   = tabu_size;
    _nbDecrease_ = nb_decrease;
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >& KTBNAdaptiveLearner< GUM_SCALAR >::useMIIC() {
    _algo_ = IBNLearner::AlgoType::MIIC;
    return *this;
  }

  // =========================================================================
  // MIIC correction
  // =========================================================================

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >& KTBNAdaptiveLearner< GUM_SCALAR >::useNMLCorrection() {
    _correction_ = CorrectedMutualInformation::KModeTypes::NML;
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >& KTBNAdaptiveLearner< GUM_SCALAR >::useMDLCorrection() {
    _correction_ = CorrectedMutualInformation::KModeTypes::MDL;
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >& KTBNAdaptiveLearner< GUM_SCALAR >::useNoCorrection() {
    _correction_ = CorrectedMutualInformation::KModeTypes::NoCorr;
    return *this;
  }

  // =========================================================================
  // Prior selection
  // =========================================================================

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >&
      KTBNAdaptiveLearner< GUM_SCALAR >::useSmoothingPrior(double weight) {
    _prior_       = IBNLearner::BNLearnerPriorType::SMOOTHING;
    _priorWeight_ = weight;
    return *this;
  }

  // =========================================================================
  // Structural constraints
  // =========================================================================

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >&
      KTBNAdaptiveLearner< GUM_SCALAR >::addForbiddenArc(std::string_view tailNode,
                                                         std::string_view headNode) {
    const auto [tailBase, tailSlice] = _determineNode_(std::string{tailNode});
    const auto [headBase, headSlice] = _determineNode_(std::string{headNode});
    _verifyBase_(tailBase, tailSlice);
    _verifyBase_(headBase, headSlice);
    _forbiddenArcs_.emplace(std::string{tailNode}, std::string{headNode});
    // no _checkArcTemporallyFeasible_ call on this path (forbidding an
    // already-impossible backward arc is a harmless no-op), so head is not
    // guaranteed >= tail here: both slices must be checked.
    _raiseKMinForSlice_(tailSlice);
    _raiseKMinForSlice_(headSlice);
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >&
      KTBNAdaptiveLearner< GUM_SCALAR >::addForbiddenArc(std::string_view tailBase,
                                                         int              tailSlice,
                                                         std::string_view headBase,
                                                         int              headSlice) {
    // validate before _encode_ flattens the slice (a negative one would
    // silently read back as atemporal, losing the real error)
    _verifyBase_(tailBase, tailSlice);
    _verifyBase_(headBase, headSlice);
    return addForbiddenArc(_encode_(tailBase, tailSlice), _encode_(headBase, headSlice));
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >&
      KTBNAdaptiveLearner< GUM_SCALAR >::eraseForbiddenArc(std::string_view tailNode,
                                                           std::string_view headNode) {
    const auto [tailBase, tailSlice] = _determineNode_(std::string{tailNode});
    const auto [headBase, headSlice] = _determineNode_(std::string{headNode});
    _verifyBase_(tailBase, tailSlice);
    _verifyBase_(headBase, headSlice);
    this->_checkArcTemporallyFeasible_(tailNode, headNode, "un-forbid the arc");
    _forbiddenArcs_.erase({std::string{tailNode}, std::string{headNode}});
    _recomputeKMin_();
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >&
      KTBNAdaptiveLearner< GUM_SCALAR >::eraseForbiddenArc(std::string_view tailBase,
                                                           int              tailSlice,
                                                           std::string_view headBase,
                                                           int              headSlice) {
    // validate before _encode_ flattens the slice (a negative one would
    // silently read back as atemporal, losing the real error)
    _verifyBase_(tailBase, tailSlice);
    _verifyBase_(headBase, headSlice);
    return eraseForbiddenArc(_encode_(tailBase, tailSlice), _encode_(headBase, headSlice));
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >&
      KTBNAdaptiveLearner< GUM_SCALAR >::addMandatoryArc(std::string_view tailNode,
                                                         std::string_view headNode) {
    const auto [tailBase, tailSlice] = _determineNode_(std::string{tailNode});
    const auto [headBase, headSlice] = _determineNode_(std::string{headNode});
    _verifyBase_(tailBase, tailSlice);
    _verifyBase_(headBase, headSlice);
    // a mandatory arc must be feasible: reject up front the arcs the k-TBN can
    // never contain (same check KTBNLearner::addMandatoryArc runs)
    this->_checkArcTemporallyFeasible_(tailNode, headNode, "force the mandatory arc");
    _mandatoryArcs_.emplace(std::string{tailNode}, std::string{headNode});
    // the feasibility check above just rejected headSlice < tailSlice, so
    // headSlice >= tailSlice is guaranteed here: it alone determines _kMin_.
    _raiseKMinForSlice_(headSlice);
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >&
      KTBNAdaptiveLearner< GUM_SCALAR >::addMandatoryArc(std::string_view tailBase,
                                                         int              tailSlice,
                                                         std::string_view headBase,
                                                         int              headSlice) {
    // validate before _encode_ flattens the slice (a negative one would
    // silently read back as atemporal, losing the real error)
    _verifyBase_(tailBase, tailSlice);
    _verifyBase_(headBase, headSlice);
    return addMandatoryArc(_encode_(tailBase, tailSlice), _encode_(headBase, headSlice));
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >&
      KTBNAdaptiveLearner< GUM_SCALAR >::eraseMandatoryArc(std::string_view tailNode,
                                                           std::string_view headNode) {
    const auto [tailBase, tailSlice] = _determineNode_(std::string{tailNode});
    const auto [headBase, headSlice] = _determineNode_(std::string{headNode});
    _verifyBase_(tailBase, tailSlice);
    _verifyBase_(headBase, headSlice);
    // no feasibility check: erasing a never-forced arc is a harmless no-op
    // (mirrors KTBNLearner::eraseMandatoryArc)
    _mandatoryArcs_.erase({std::string{tailNode}, std::string{headNode}});
    _recomputeKMin_();
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >&
      KTBNAdaptiveLearner< GUM_SCALAR >::eraseMandatoryArc(std::string_view tailBase,
                                                           int              tailSlice,
                                                           std::string_view headBase,
                                                           int              headSlice) {
    // validate before _encode_ flattens the slice (a negative one would
    // silently read back as atemporal, losing the real error)
    _verifyBase_(tailBase, tailSlice);
    _verifyBase_(headBase, headSlice);
    return eraseMandatoryArc(_encode_(tailBase, tailSlice), _encode_(headBase, headSlice));
  }

  // =========================================================================
  // Kernel-relative arcs
  // =========================================================================

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >&
      KTBNAdaptiveLearner< GUM_SCALAR >::addForbiddenKernelArc(std::string_view tailBase,
                                                               int              lag,
                                                               std::string_view headBase) {
    _verifyKernelArc_(tailBase, headBase, lag);
    _forbiddenKernelArcs_.emplace(std::string{tailBase}, std::string{headBase}, lag);
    _raiseKMinForSlice_(lag);
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >&
      KTBNAdaptiveLearner< GUM_SCALAR >::eraseForbiddenKernelArc(std::string_view tailBase,
                                                                 int              lag,
                                                                 std::string_view headBase) {
    _verifyKernelArc_(tailBase, headBase, lag);
    _forbiddenKernelArcs_.erase({std::string{tailBase}, std::string{headBase}, lag});
    _recomputeKMin_();
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >&
      KTBNAdaptiveLearner< GUM_SCALAR >::addMandatoryKernelArc(std::string_view tailBase,
                                                               int              lag,
                                                               std::string_view headBase) {
    _verifyKernelArc_(tailBase, headBase, lag);
    // no separate feasibility check: lag >= 0 (just verified) already guarantees
    // headSlice = k-1 >= k-1-lag = tailSlice for every candidate, so this can
    // never be a backward-in-time arc (see addMandatoryKernelArc's doc comment)
    _mandatoryKernelArcs_.emplace(std::string{tailBase}, std::string{headBase}, lag);
    _raiseKMinForSlice_(lag);
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >&
      KTBNAdaptiveLearner< GUM_SCALAR >::eraseMandatoryKernelArc(std::string_view tailBase,
                                                                 int              lag,
                                                                 std::string_view headBase) {
    _verifyKernelArc_(tailBase, headBase, lag);
    _mandatoryKernelArcs_.erase({std::string{tailBase}, std::string{headBase}, lag});
    _recomputeKMin_();
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >&
      KTBNAdaptiveLearner< GUM_SCALAR >::addForbiddenIntraSliceArc(std::string_view tailBase,
                                                                   std::string_view headBase) {
    // these constraints carry no single slice, so no slice range is checked;
    // an atemporal base has no intra-slice position and is rejected eagerly
    // here, where KTBNLearner would throw at its own setter (expanding to "C[t]")
    _checkBaseIsTemporal_(tailBase, "an intra-slice constraint");
    _checkBaseIsTemporal_(headBase, "an intra-slice constraint");
    // record the base pair; unlike KTBNLearner it cannot be expanded per-slice
    // now (k is not fixed) — learnKTBN() expands it for each candidate k
    _forbiddenIntraSliceArcs_.emplace(std::string{tailBase}, std::string{headBase});
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >&
      KTBNAdaptiveLearner< GUM_SCALAR >::eraseForbiddenIntraSliceArc(std::string_view tailBase,
                                                                     std::string_view headBase) {
    // same eager rejection as addForbiddenIntraSliceArc: such a constraint can
    // never have been recorded
    _checkBaseIsTemporal_(tailBase, "an intra-slice constraint");
    _checkBaseIsTemporal_(headBase, "an intra-slice constraint");
    _forbiddenIntraSliceArcs_.erase({std::string{tailBase}, std::string{headBase}});
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >&
      KTBNAdaptiveLearner< GUM_SCALAR >::addForbiddenArcAllSlices(std::string_view tailBase,
                                                                  std::string_view headBase) {
    _verifyBase_(tailBase, KTBN< GUM_SCALAR >::ATEMPORAL);
    _verifyBase_(headBase, KTBN< GUM_SCALAR >::ATEMPORAL);
    // record the base pair; learnKTBN() expands it over every causally-possible
    // slice pair for each candidate k (k is not fixed here)
    _forbiddenArcsAllSlices_.emplace(std::string{tailBase}, std::string{headBase});
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >&
      KTBNAdaptiveLearner< GUM_SCALAR >::eraseForbiddenArcAllSlices(std::string_view tailBase,
                                                                    std::string_view headBase) {
    _verifyBase_(tailBase, KTBN< GUM_SCALAR >::ATEMPORAL);
    _verifyBase_(headBase, KTBN< GUM_SCALAR >::ATEMPORAL);
    _forbiddenArcsAllSlices_.erase({std::string{tailBase}, std::string{headBase}});
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >&
      KTBNAdaptiveLearner< GUM_SCALAR >::addNoParentNode(std::string_view base, int slice) {
    // validate before _encode_ flattens the slice (see the arc overloads above)
    _verifyBase_(base, slice);
    return addNoParentNode(_encode_(base, slice));
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >&
      KTBNAdaptiveLearner< GUM_SCALAR >::addNoParentNode(std::string_view name) {
    const auto [base, slice] = _determineNode_(std::string{name});
    _verifyBase_(base, slice);
    _noParentNodes_.insert(std::string{name});
    _raiseKMinForSlice_(slice);
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >&
      KTBNAdaptiveLearner< GUM_SCALAR >::eraseNoParentNode(std::string_view base, int slice) {
    // validate before _encode_ flattens the slice (see the arc overloads above)
    _verifyBase_(base, slice);
    return eraseNoParentNode(_encode_(base, slice));
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >&
      KTBNAdaptiveLearner< GUM_SCALAR >::eraseNoParentNode(std::string_view name) {
    const auto [base, slice] = _determineNode_(std::string{name});
    _verifyBase_(base, slice);
    _noParentNodes_.erase(std::string{name});
    _recomputeKMin_();
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >&
      KTBNAdaptiveLearner< GUM_SCALAR >::addNoChildrenNode(std::string_view base, int slice) {
    // validate before _encode_ flattens the slice (see the arc overloads above)
    _verifyBase_(base, slice);
    return addNoChildrenNode(_encode_(base, slice));
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >&
      KTBNAdaptiveLearner< GUM_SCALAR >::addNoChildrenNode(std::string_view name) {
    const auto [base, slice] = _determineNode_(std::string{name});
    _verifyBase_(base, slice);
    _noChildrenNodes_.insert(std::string{name});
    _raiseKMinForSlice_(slice);
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >&
      KTBNAdaptiveLearner< GUM_SCALAR >::eraseNoChildrenNode(std::string_view base, int slice) {
    // validate before _encode_ flattens the slice (see the arc overloads above)
    _verifyBase_(base, slice);
    return eraseNoChildrenNode(_encode_(base, slice));
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >&
      KTBNAdaptiveLearner< GUM_SCALAR >::eraseNoChildrenNode(std::string_view name) {
    const auto [base, slice] = _determineNode_(std::string{name});
    _verifyBase_(base, slice);
    _noChildrenNodes_.erase(std::string{name});
    _recomputeKMin_();
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >&
      KTBNAdaptiveLearner< GUM_SCALAR >::addPossibleEdge(std::string_view tailBase,
                                                         int              tailSlice,
                                                         std::string_view headBase,
                                                         int              headSlice) {
    // validate before _encode_ flattens the slice (a negative one would
    // silently read back as atemporal, losing the real error)
    _verifyBase_(tailBase, tailSlice);
    _verifyBase_(headBase, headSlice);
    return addPossibleEdge(_encode_(tailBase, tailSlice), _encode_(headBase, headSlice));
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >&
      KTBNAdaptiveLearner< GUM_SCALAR >::addPossibleEdge(std::string_view tail,
                                                         std::string_view head) {
    const auto [tailBase, tailSlice] = _determineNode_(std::string{tail});
    const auto [headBase, headSlice] = _determineNode_(std::string{head});
    _verifyBase_(tailBase, tailSlice);
    _verifyBase_(headBase, headSlice);
    // an edge is undirected (BNLearner stores it as a gum::Edge), so the pair is
    // normalized alphabetically: (A,B) and (B,A) are one and the same record
    std::string first{tail}, second{head};
    if (second < first) std::swap(first, second);
    _possibleEdges_.emplace(std::move(first), std::move(second));
    // an edge is undirected and unchecked, so neither slice dominates: both
    // must be checked (tailSlice/headSlice, decoded above the swap, are
    // unaffected by it).
    _raiseKMinForSlice_(tailSlice);
    _raiseKMinForSlice_(headSlice);
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >&
      KTBNAdaptiveLearner< GUM_SCALAR >::erasePossibleEdge(std::string_view tailBase,
                                                           int              tailSlice,
                                                           std::string_view headBase,
                                                           int              headSlice) {
    // validate before _encode_ flattens the slice (a negative one would
    // silently read back as atemporal, losing the real error)
    _verifyBase_(tailBase, tailSlice);
    _verifyBase_(headBase, headSlice);
    return erasePossibleEdge(_encode_(tailBase, tailSlice), _encode_(headBase, headSlice));
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >&
      KTBNAdaptiveLearner< GUM_SCALAR >::erasePossibleEdge(std::string_view tail,
                                                           std::string_view head) {
    const auto [tailBase, tailSlice] = _determineNode_(std::string{tail});
    const auto [headBase, headSlice] = _determineNode_(std::string{head});
    _verifyBase_(tailBase, tailSlice);
    _verifyBase_(headBase, headSlice);
    // same alphabetical normalization as addPossibleEdge, so erasing (B,A)
    // removes the edge recorded as (A,B)
    std::string first{tail}, second{head};
    if (second < first) std::swap(first, second);
    _possibleEdges_.erase({std::move(first), std::move(second)});
    _recomputeKMin_();
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >&
      KTBNAdaptiveLearner< GUM_SCALAR >::allowArcAdditions(bool allow) {
    _allowAdditions_ = allow;
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >&
      KTBNAdaptiveLearner< GUM_SCALAR >::allowArcDeletions(bool allow) {
    _allowDeletions_ = allow;
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >&
      KTBNAdaptiveLearner< GUM_SCALAR >::allowArcReversals(bool allow) {
    _allowReversals_ = allow;
    return *this;
  }

  template < GUM_Numeric GUM_SCALAR >
  KTBNAdaptiveLearner< GUM_SCALAR >&
      KTBNAdaptiveLearner< GUM_SCALAR >::setMaxIndegree(Size max_indegree) {
    _maxIndegree_ = max_indegree;
    return *this;
  }

  // =========================================================================
  // Name encoding
  // =========================================================================

  template < GUM_Numeric GUM_SCALAR >
  const std::unordered_set< std::string >&
      KTBNAdaptiveLearner< GUM_SCALAR >::_atemporalVarNames_() const {
    return _atemporalVars_;
  }

  template < GUM_Numeric GUM_SCALAR >
  bool KTBNAdaptiveLearner< GUM_SCALAR >::_isKnownBase_(std::string_view base) const {
    return _baseNames_.contains(std::string{base});
  }

  // =========================================================================
  // Validation
  // =========================================================================

  template < GUM_Numeric GUM_SCALAR >
  void KTBNAdaptiveLearner< GUM_SCALAR >::_verifyBase_(std::string_view base, int slice) const {
    if (!_baseNames_.contains(std::string{base}))
      GUM_ERROR(InvalidArgument,
                "unknown base variable '" << base << "': it is not one of the data columns")
    // an atemporal variable's only valid slice is ATEMPORAL; reject eagerly
    // rather than recording e.g. "C[1]" and failing later at learnKTBN() time
    if (_atemporalVars_.contains(std::string{base})) {
      if (slice != KTBN< GUM_SCALAR >::ATEMPORAL)
        GUM_ERROR(InvalidArgument,
                  "atemporal variable '" << base << "' cannot be given a time slice (got " << slice
                                         << "): use KTBN::ATEMPORAL")
      return;
    }
    // temporal: valid slices are [0, kMax). A negative value other than
    // ATEMPORAL must be caught here -- _encode_ would turn it into "X[-5]",
    // which reads back as atemporal and slips past the slice-fit rule.
    if (slice != KTBN< GUM_SCALAR >::ATEMPORAL && slice < 0)
      GUM_ERROR(InvalidArgument,
                "negative time slice " << slice << " for '" << base
                                       << "': slices start at 0 (use KTBN::ATEMPORAL to address an "
                                          "atemporal variable)")
    if (slice >= static_cast< int >(_kMax_))
      GUM_ERROR(InvalidArgument,
                "time slice " << slice << " for '" << base
                              << "' is out of range: it must be < kMax (" << _kMax_ << ")")
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBNAdaptiveLearner< GUM_SCALAR >::_verifyKernelArc_(std::string_view tailBase,
                                                            std::string_view headBase,
                                                            int              lag) const {
    // an atemporal variable has a single instance, not a per-slice one, so it
    // has no kernel slice to anchor a lag against: same rule as the intra-slice
    // setters, hence the same shared check
    for (const std::string_view base: {tailBase, headBase})
      _checkBaseIsTemporal_(base, "a kernel-relative arc");
    // lag < 0 would place the tail AFTER the kernel slice (the last slice by
    // definition), which can never happen
    if (lag < 0) GUM_ERROR(InvalidArgument, "negative kernel lag " << lag << ": lag must be >= 0")
    // some candidate k in [2,kMax] must be able to place the tail at slice
    // k-1-lag >= 0, i.e. lag <= k-1 <= kMax-1
    if (lag >= static_cast< int >(_kMax_))
      GUM_ERROR(InvalidArgument,
                "kernel lag " << lag << " is out of range: it must be < kMax (" << _kMax_
                              << "), so some candidate k can place the tail at slice k-1-lag >= 0")
  }

  // =========================================================================
  // Candidate-range bookkeeping
  // =========================================================================

  template < GUM_Numeric GUM_SCALAR >
  void KTBNAdaptiveLearner< GUM_SCALAR >::_raiseKMinForSlice_(int slice) {
    if (slice != KTBN< GUM_SCALAR >::ATEMPORAL)
      _kMin_ = std::max(_kMin_, static_cast< Size >(slice) + 1);
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBNAdaptiveLearner< GUM_SCALAR >::_recomputeKMin_() {
    // kMin = 1 + the largest recorded slice (fits()'s "slice < k" rule means a
    // smaller k would drop that constraint). An erase can lower kMin, unlike a
    // single _raiseKMinForSlice_ call, so this rebuilds from scratch rather
    // than adjusting in place.
    _kMin_ = 2;
    for (const auto& [tail, head]: _forbiddenArcs_) {
      _raiseKMinForSlice_(_determineNode_(tail).second);
      _raiseKMinForSlice_(_determineNode_(head).second);
    }
    for (const auto& [tail, head]: _mandatoryArcs_) {
      _raiseKMinForSlice_(_determineNode_(tail).second);
      _raiseKMinForSlice_(_determineNode_(head).second);
    }
    for (const auto& [tail, head]: _possibleEdges_) {
      _raiseKMinForSlice_(_determineNode_(tail).second);
      _raiseKMinForSlice_(_determineNode_(head).second);
    }
    for (const auto& node: _noParentNodes_)
      _raiseKMinForSlice_(_determineNode_(node).second);
    for (const auto& node: _noChildrenNodes_)
      _raiseKMinForSlice_(_determineNode_(node).second);
    // kernel-relative arcs: the lag itself is what must fit (the head's slice is
    // always the kernel k-1, which trivially fits any k), so raise on the lag
    // directly rather than decoding an engine name that doesn't exist yet.
    for (const auto& t: _forbiddenKernelArcs_)
      _raiseKMinForSlice_(std::get< 2 >(t));
    for (const auto& t: _mandatoryKernelArcs_)
      _raiseKMinForSlice_(std::get< 2 >(t));
    // _forbiddenIntraSliceArcs_ / _forbiddenArcsAllSlices_ carry no slice (they
    // are base-name pairs, expanded per-k by the fixed-k learner) and so are
    // left out: they never make a candidate drop anything, whatever k is.
  }

  // =========================================================================
  // Configuration replay (learnKTBN helpers)
  // =========================================================================

  template < GUM_Numeric GUM_SCALAR >
  void KTBNAdaptiveLearner< GUM_SCALAR >::_applyConfig_(KTBNLearner< GUM_SCALAR >& learner) const {
    // A fresh KTBNLearner already starts at the mirrored defaults (MIIC / BDeu /
    // MDL / NO_prior), so we only touch a knob when it differs from its default,
    // and only the knobs the chosen algorithm actually consumes.
    bool scoreBased = false;
    switch (_algo_) {
      case IBNLearner::AlgoType::MIIC :
        // MIIC is the default algo — nothing to set on a fresh learner. It uses a
        // correction (not a score); apply it only when it is not the default MDL.
        switch (_correction_) {
          case CorrectedMutualInformation::KModeTypes::NML : learner.useNMLCorrection(); break;
          case CorrectedMutualInformation::KModeTypes::NoCorr : learner.useNoCorrection(); break;
          case CorrectedMutualInformation::KModeTypes::MDL : break;   // default
        }
        break;
      case IBNLearner::AlgoType::GREEDY_HILL_CLIMBING :
        learner.useGreedyHillClimbing();
        scoreBased = true;
        break;
      case IBNLearner::AlgoType::EXTENDED_GREEDY_HILL_CLIMBING :
        learner.useExtendedGreedyHillClimbing();
        scoreBased = true;
        break;
      case IBNLearner::AlgoType::LOCAL_SEARCH_WITH_TABU_LIST :
        learner.useLocalSearchWithTabuList(_tabuSize_, _nbDecrease_);
        scoreBased = true;
        break;
      default : break;   // K2 / PC are never recorded by our setters
    }

    // score-based algos consume a score; apply it only when it is not the default BDeu
    if (scoreBased && _score_ != IBNLearner::ScoreType::BDeu) {
      switch (_score_) {
        case IBNLearner::ScoreType::AIC : learner.useScoreAIC(); break;
        case IBNLearner::ScoreType::BD : learner.useScoreBD(); break;
        case IBNLearner::ScoreType::BIC : learner.useScoreBIC(); break;
        case IBNLearner::ScoreType::LOG2LIKELIHOOD : learner.useScoreLog2Likelihood(); break;
        case IBNLearner::ScoreType::MDL : learner.useScoreMDL(); break;
        case IBNLearner::ScoreType::fNML : learner.useScorefNML(); break;
        default : break;   // BDeu (guarded out) and K2 (never recorded)
      }
    }

    // the prior feeds parameter estimation whatever the algorithm, so it is applied
    // independently of the score/correction split, again only when non-default
    if (_prior_ != IBNLearner::BNLearnerPriorType::NO_prior)
      learner.useSmoothingPrior(_priorWeight_);
  }

  template < GUM_Numeric GUM_SCALAR >
  void KTBNAdaptiveLearner< GUM_SCALAR >::_applyConstraints_(KTBNLearner< GUM_SCALAR >& learner,
                                                             Size                       k) const {
    const int ik = static_cast< int >(k);
    // an engine-name endpoint fits candidate k iff its slice is < k (ATEMPORAL == -1
    // always fits); a constraint touching a slice this candidate does not have is
    // skipped for this k only.
    auto fits = [&](const std::string& node) { return _determineNode_(node).second < ik; };

    for (const auto& [tail, head]: _forbiddenArcs_)
      if (fits(tail) && fits(head)) learner.addForbiddenArc(tail, head);
    for (const auto& [tail, head]: _mandatoryArcs_)
      if (fits(tail) && fits(head)) learner.addMandatoryArc(tail, head);
    for (const auto& [tail, head]: _possibleEdges_)
      if (fits(tail) && fits(head)) learner.addPossibleEdge(tail, head);
    for (const auto& node: _noParentNodes_)
      if (fits(node)) learner.addNoParentNode(node);
    for (const auto& node: _noChildrenNodes_)
      if (fits(node)) learner.addNoChildrenNode(node);

    // kernel-relative arcs: resolve the lag against this candidate's kernel
    // slice (k-1) into an explicit (base, slice) arc. Always fits once k >=
    // _kMin_ (guaranteed by _raiseKMinForSlice_/_recomputeKMin_ above), but the
    // guard mirrors fits()'s defensive style above rather than assuming it.
    for (const auto& [tailBase, headBase, lag]: _forbiddenKernelArcs_) {
      const int tailSlice = ik - 1 - lag;
      if (tailSlice >= 0) learner.addForbiddenArc(tailBase, tailSlice, headBase, ik - 1);
    }
    for (const auto& [tailBase, headBase, lag]: _mandatoryKernelArcs_) {
      const int tailSlice = ik - 1 - lag;
      if (tailSlice >= 0) learner.addMandatoryArc(tailBase, tailSlice, headBase, ik - 1);
    }

    // base-only constraints: hand them to the fixed-k learner, which expands them
    // over its own slices (no manual expansion / no slice filtering needed here).
    for (const auto& [tailBase, headBase]: _forbiddenIntraSliceArcs_)
      learner.addForbiddenIntraSliceArc(tailBase, headBase);
    for (const auto& [tailBase, headBase]: _forbiddenArcsAllSlices_)
      learner.addForbiddenArcAllSlices(tailBase, headBase);

    // structural-search knobs, applied only when the user changed them from default
    if (!_allowAdditions_) learner.allowArcAdditions(false);
    if (!_allowDeletions_) learner.allowArcDeletions(false);
    if (!_allowReversals_) learner.allowArcReversals(false);
    if (_maxIndegree_ != std::numeric_limits< Size >::max()) learner.setMaxIndegree(_maxIndegree_);
  }

  // =========================================================================
  // Model selection
  // =========================================================================

  template < GUM_Numeric GUM_SCALAR >
  template < typename PerInstance, typename PerNodeFinal >
  void KTBNAdaptiveLearner< GUM_SCALAR >::_forEachScoredNode_(const KTBN< GUM_SCALAR >& net,
                                                              PerInstance               perInstance,
                                                              PerNodeFinal perNodeFinal) const {
    namespace fs = std::filesystem;
    const int k  = static_cast< int >(net.k());

    // Precomputed scorer for one template node, reused across every row of
    // every trajectory so the inner loop never rebuilds an Instantiation
    // (~3.7x the cost of one chgVal) or re-decodes a variable name. selfDom /
    // counts carry the accumulator a reduction criterion (fNML) closes over
    // in perNodeFinal; likelihood-style reductions ignore them.
    struct NodeEval {
      const Tensor< GUM_SCALAR >*                  cpt;
      Instantiation                                inst;      // slaved to *cpt
      std::vector< std::string >                   dimName;   // cpt.variable(d).name()
      std::vector< std::pair< std::string, int > > dimNode;   // _determineNode_(name)
      Size                                         selfDom;   // |node| (CPT dim 0 is the node)
      std::unordered_map< Size, double >           counts;    // parent-config index -> #obs
    };

    auto makeEval = [&](const Tensor< GUM_SCALAR >& cpt) {
      NodeEval e;
      e.cpt     = &cpt;
      e.inst    = Instantiation(cpt);
      e.selfDom = cpt.variable(0).domainSize();   // the node varies fastest (dim 0)
      e.dimName.reserve(cpt.nbrDim());
      e.dimNode.reserve(cpt.nbrDim());
      for (Idx d = 0; d < cpt.nbrDim(); ++d) {
        std::string name = cpt.variable(d).name();
        e.dimNode.push_back(_determineNode_(name));
        e.dimName.push_back(std::move(name));
      }
      return e;
    };

    // one evaluator per (temporal base, slice), indexed by slice in 0..k-1: the
    // node scored at absolute time t uses slice min(t, k-1) (see childSlice below)
    const auto&                            temporalBases = net.temporalVarNames();
    std::vector< std::vector< NodeEval > > temporalEvals(k);
    for (int slice = 0; slice < k; ++slice) {
      temporalEvals[slice].reserve(temporalBases.size());
      for (const auto& base: temporalBases)
        temporalEvals[slice].push_back(makeEval(net.cpt(base, slice)));
    }

    // one evaluator per atemporal node (scored once per trajectory)
    std::vector< NodeEval > atemporalEvals;
    atemporalEvals.reserve(net.atemporalVarNames().size());
    for (const auto& base: net.atemporalVarNames())
      atemporalEvals.push_back(makeEval(net.cpt(base, KTBN< GUM_SCALAR >::ATEMPORAL)));

    // one circular buffer of k rows, reused across trajectories: the row for
    // absolute time t lives at slot (t % k), so the k live rows never move —
    // reading a new row overwrites the stalest slot (which held time t-k).
    std::vector< std::vector< std::string > > window(k);

    // missing-value markers, as a set for O(1) tests in the inner loop
    const std::unordered_set< std::string > missing(_missingSymbols_.begin(),
                                                    _missingSymbols_.end());

    for (Size s = 0; s < _nbSamples_; ++s) {
      const fs::path file = fs::path{_dirPath_} / (_csvBaseName_ + std::to_string(s + 1) + ".csv");

      // ---- pre-pass: column index, and the atemporal values of this trajectory ----
      // An atemporal variable is constant down the file, but the row that
      // happens to carry a missing marker teaches nothing, so its value is the
      // first *informative* one rather than row 0's. The scan stops as soon as
      // every atemporal column is resolved (row 0 in the usual case), so it
      // costs one row unless the data actually has holes. It cannot be folded
      // into the main pass: a value resolved at row 12 must already be known
      // when row 0 is scored, and buffering the rows instead would defeat the
      // width-k window that bounds this function's memory.
      std::unordered_map< std::string, std::size_t > colOf;
      std::unordered_map< std::string, std::string > atempVal;
      {
        std::ifstream pis(file, std::ifstream::in);
        if (!pis.is_open()) GUM_ERROR(IOError, "Cannot open " << file.string())
        CSVParser pre(pis, file.string());
        if (!pre.next()) GUM_ERROR(IOError, "empty trajectory file '" << file.string() << "'") {
            const auto& header = pre.current();
            for (std::size_t c = 0; c < header.size(); ++c)
              colOf[header[c]] = c;
          }
        const std::size_t nbAtemp = net.atemporalVarNames().size();
        while (atempVal.size() < nbAtemp && pre.next()) {
          const auto& row = pre.current();
          for (const auto& c: net.atemporalVarNames()) {
            if (atempVal.contains(c)) continue;
            const std::string& tok = row[colOf.at(c)];
            if (!missing.contains(tok)) atempVal[c] = tok;
          }
        }
      }

      std::ifstream is(file, std::ifstream::in);
      if (!is.is_open()) GUM_ERROR(IOError, "Cannot open " << file.string())
      CSVParser parser(is, file.string());
      parser.next();   // skip the header, already consumed by the pre-pass

      // Label observed in the data for one CPT dimension (varBase, varSlice) —
      // the scored node itself or one of its parents — while scoring the node
      // whose template slice is scoredSlice at absolute time scoredTime.
      // Returns nullptr when the value is missing (or, for an atemporal
      // variable, never observed in this trajectory); the caller then drops the
      // whole instance. A pointer rather than a value: both sources are stable
      // strings, and this is the innermost loop of the order selection.
      auto labelOf = [&](const std::string& varBase,
                         int                varSlice,
                         int                scoredTime,
                         int                scoredSlice) -> const std::string* {
        // atemporal variable: constant down the trajectory, read the captured value
        if (varSlice == KTBN< GUM_SCALAR >::ATEMPORAL) {
          const auto it = atempVal.find(varBase);
          return (it == atempVal.end()) ? nullptr : &it->second;
        }
        // temporal variable: it lags (scoredSlice - varSlice) steps behind the
        // scored node, so its value was observed at that earlier absolute time
        const int varTime = scoredTime - (scoredSlice - varSlice);
        // fetch that row from the circular buffer (slot = time modulo k)
        const std::string& tok = window[varTime % k][colOf.at(varBase)];
        return missing.contains(tok) ? nullptr : &tok;
      };

      int t = 0;   // absolute time == data-row index
      while (parser.next()) {
        const auto& cur = parser.current();
        window[t % k].assign(cur.begin(), cur.end());

        // first k-1 slices use their own initial CPT; from slice k-1 on the
        // transition kernel (template slice k-1) is reused for every step
        const int childSlice = (t < k - 1) ? t : (k - 1);
        for (NodeEval& e: temporalEvals[childSlice]) {
          // an instance counts only if its WHOLE family is observed: the node
          // and every parent, each read at its own lag. One missing value drops
          // this instance alone — the other nodes of the same row are unaffected.
          bool complete = true;
          for (std::size_t d = 0; d < e.dimName.size(); ++d) {
            const auto& [vbase, vslice] = e.dimNode[d];
            const std::string* lbl      = labelOf(vbase, vslice, t, childSlice);
            if (lbl == nullptr) {
              complete = false;
              break;
            }
            e.inst.chgVal(e.dimName[d], *lbl);
          }
          if (complete) perInstance(e);
        }
        ++t;
      }

      // atemporal nodes: scored once per trajectory (parents are atemporal too).
      // An atemporal column left unresolved by the pre-pass drops every instance
      // that reads it, here and in the temporal loop above — for THIS trajectory
      // only, the others still scoring normally.
      if (t > 0)
        for (NodeEval& e: atemporalEvals) {
          bool complete = true;
          for (std::size_t d = 0; d < e.dimName.size(); ++d) {
            const auto it = atempVal.find(e.dimNode[d].first);
            if (it == atempVal.end()) {
              complete = false;
              break;
            }
            e.inst.chgVal(e.dimName[d], it->second);
          }
          if (complete) perInstance(e);
        }
    }

    // per-node closing pass over every evaluator (all slices + atemporal), for
    // criteria that reduce the accumulated per-node state after the whole walk
    for (auto& slice: temporalEvals)
      for (NodeEval& e: slice)
        perNodeFinal(e);
    for (NodeEval& e: atemporalEvals)
      perNodeFinal(e);
  }

  template < GUM_Numeric GUM_SCALAR >
  double KTBNAdaptiveLearner< GUM_SCALAR >::_log2Likelihood_(const KTBN< GUM_SCALAR >& net) const {
    // sum log2 of the CPT entry over every visited node instance; no per-node
    // closing reduction is needed (the likelihood is fully additive per instance)
    double logL = 0.0;
    _forEachScoredNode_(
        net,
        [&logL](auto& e) { logL += std::log2((*e.cpt)[e.inst]); },
        [](auto&) {});
    return logL;
  }

  template < GUM_Numeric GUM_SCALAR >
  double KTBNAdaptiveLearner< GUM_SCALAR >::_fNMLScore_(const KTBN< GUM_SCALAR >& net) const {
    // one data walk: per instance, accumulate log2L AND bucket the observation by
    // its parent configuration; per node, close over the buckets with the
    // multinomial parametric complexity log2Cnr, exactly the per-node regret term
    // aGrUM's ScorefNML subtracts. Fused (rather than a separate _log2Likelihood_
    // call plus a separate penalty pass) so fNML order-selection reads every
    // trajectory once per candidate k, not twice.
    double logL = 0.0, penalty = 0.0;
    _forEachScoredNode_(
        net,
        [&logL](auto& e) {
          logL += std::log2((*e.cpt)[e.inst]);
          // linear index over the parent dims (1..nbrDim-1); the node is dim 0
          Size idx = 0;
          for (Idx d = 1; d < e.cpt->nbrDim(); ++d)
            idx = idx * e.cpt->variable(d).domainSize() + e.inst.val(e.cpt->variable(d));
          e.counts[idx] += 1.0;
        },
        [this, &penalty](auto& e) {
          for (const auto& [cfg, n]: e.counts)
            penalty += _ctable_.log2Cnr(e.selfDom, n);
        });
    return logL - penalty;
  }

  template < GUM_Numeric GUM_SCALAR >
  double KTBNAdaptiveLearner< GUM_SCALAR >::_countParameters_(const KTBN< GUM_SCALAR >& net) const {
    // Free parameters of the template: for each node, (|node| - 1) independent
    // entries per joint parent configuration. Summed over every template node
    // (initial slices, kernel slice, atemporal nodes), this is the k-TBN's total
    // parameter count — it rises with k, which is what the BIC penalty needs.
    double df = 0.0;
    for (const auto& [base, slice]: net.nodes()) {
      double cell = net.variable(base, slice).domainSize() - 1.0;
      for (const auto& [pbase, pslice]: net.parents(base, slice))
        cell *= net.variable(pbase, pslice).domainSize();
      df += cell;
    }
    return df;
  }

  template < GUM_Numeric GUM_SCALAR >
  double KTBNAdaptiveLearner< GUM_SCALAR >::_orderSelectionScore_(const KTBN< GUM_SCALAR >& net,
                                                                  double logN) const {
    // BIC/AIC share a log2-likelihood term plus a data-free parameter-count
    // penalty, so both read the trajectories once via _log2Likelihood_. fNML
    // needs the likelihood too, but paired with a data-dependent per-node regret
    // over the very same walk, so it is fused into a single _forEachScoredNode_
    // pass (_fNMLScore_) instead of calling _log2Likelihood_ a second time.
    switch (_orderScore_) {
      case OrderScoreType::BIC :
        // reward fit, penalise complexity: log2L − ½·df·log2(N)
        return _log2Likelihood_(net) - 0.5 * _countParameters_(net) * logN;
      case OrderScoreType::AIC :
        // lighter, sample-size-independent penalty: log2L − df
        return _log2Likelihood_(net) - _countParameters_(net);
      case OrderScoreType::fNML :
        // per-node multinomial parametric complexity (regret): log2L − Σ log2Cnr
        return _fNMLScore_(net);
    }
    // every criterion returns above; unreachable while OrderScoreType is exhausted
    return _log2Likelihood_(net) - 0.5 * _countParameters_(net) * logN;
  }

}   // namespace gum::learning
