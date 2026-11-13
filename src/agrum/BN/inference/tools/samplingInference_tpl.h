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


/**
 * @file
 * @brief Implementation of the non pure virtual methods of class
 * ApproximateInference.
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <cstdint>

#include <agrum/base/core/configuration/configuration.h>
#include <agrum/BN/BayesNetFragment.h>
#include <agrum/BN/inference/tools/evidencePruning.h>
#include <agrum/BN/inference/tools/samplingInference.h>

namespace gum {

  template < GUM_Numeric GUM_SCALAR >
  SamplingInference< GUM_SCALAR >::SamplingInference(const IBayesNet< GUM_SCALAR >* bn) :
      ApproximateInference< GUM_SCALAR >(bn), _estimator_(), _samplingBN_(nullptr),
      _maxConsecutiveRejections_(static_cast< Size >(
          Configuration::instance().typed< std::int64_t >("sampling",
                                                          "maxConsecutiveRejections"))) {
    // epsilon, minimal epsilon rate, maximum number of iterations, period size, verbosity and
    // timeout
    this->setFromConfiguration_("sampling", true);
    GUM_CONSTRUCTOR(SamplingInference);
  }

  template < GUM_Numeric GUM_SCALAR >
  SamplingInference< GUM_SCALAR >::~SamplingInference() {
    GUM_DESTRUCTOR(SamplingInference);
    _discardContextualization_();
  }

  template < GUM_Numeric GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::_discardContextualization_() {
    if (isContextualized) {   // otherwise  _samplingBN_==nullptr
      delete _samplingBN_;
      _samplingBN_     = nullptr;
      isContextualized = false;
    }
  }

  template < GUM_Numeric GUM_SCALAR >
  const IBayesNet< GUM_SCALAR >& SamplingInference< GUM_SCALAR >::samplingBN() {
    this->prepareInference();
    if (_samplingBN_ == nullptr) return this->BN();
    else return *_samplingBN_;
  }

  template < GUM_Numeric GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::setEstimatorFromBN_() {
    _estimator_.setFromBN(&samplingBN(), this->hardEvidenceNodes());
    this->isSetEstimator = true;
  }

  template < GUM_Numeric GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::setEstimatorFromLBP_(
      LoopyBeliefPropagation< GUM_SCALAR >* lbp,
      GUM_SCALAR                            virtualLBPSize) {
    _estimator_.setFromLBP(lbp, this->hardEvidenceNodes(), virtualLBPSize);
    this->isSetEstimator = true;
  }

  template < GUM_Numeric GUM_SCALAR >
  const Tensor< GUM_SCALAR >& SamplingInference< GUM_SCALAR >::currentPosterior(NodeId id) {
    return _estimator_.posterior(this->BN().variable(id));
  }

  template < GUM_Numeric GUM_SCALAR >
  const Tensor< GUM_SCALAR >&
      SamplingInference< GUM_SCALAR >::currentPosterior(std::string_view name) {
    return currentPosterior(this->BN().idFromName(name));
  }

  template < GUM_Numeric GUM_SCALAR >
  const Tensor< GUM_SCALAR >& SamplingInference< GUM_SCALAR >::posterior_(NodeId id) {
    return _estimator_.posterior(this->BN().variable(id));
  }

  template < GUM_Numeric GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::contextualize() {
    _discardContextualization_();

    // every node is kept, whatever the targets: the samplers draw all the nodes of the BN
    _samplingBN_ = new BayesNetFragment< GUM_SCALAR >(this->BN());
    for (const auto node: this->BN().nodes())
      _samplingBN_->installNode(node);

    for (const auto hard: this->hardEvidenceNodes()) {
      gum::Instantiation I;
      I.add(this->BN().variable(hard));
      I.chgVal(this->BN().variable(hard), this->hardEvidence()[hard]);

      for (const auto& child: this->BN().children(hard)) {
        _samplingBN_->installCPT(child, this->BN().cpt(child).extract(I));
      }
    }

    this->isContextualized = true;
    this->onContextualize_(_samplingBN_);
  }

  template < GUM_Numeric GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::makeInference_() {
    if (!isSetEstimator) this->setEstimatorFromBN_();
    loopApproxInference_();
  }

  template < GUM_Numeric GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::loopApproxInference_() {
    //@todo This should be in  _prepareInference_
    if (!isContextualized) { this->contextualize(); }

    // the estimator is used by this inference only: the next one initializes it again (even if
    // this one throws)
    this->isSetEstimator = false;

    // an evidence proved incompatible is refused before any draw (the pruning is done on the
    // original BN: the sampling BN may have no null probability, e.g. for importance sampling)
    checkEvidenceByPruning(this->BN(), this->hardEvidence());

    this->initApproximationScheme();
    gum::Instantiation Ip;
    GUM_SCALAR         w = .0;   //

    // The estimator may start from a prior counted as virtual samples (e.g. LBP posteriors). As
    // long as the prior outweighs the samples, the estimation hardly moves: the convergence
    // criteria (epsilon and rate) would stop the sampling and return the prior. They are suspended
    // until the samples outnumber the virtual ones (the limits on time and iterations still apply).
    struct CriteriaSuspension {
      SamplingInference< GUM_SCALAR >* inference;
      const bool                       epsilon;
      const bool                       rate;
      bool                             suspended = false;

      // a suspension is not a change of the parameters: the setters would outdate the inference
      // being computed
      void suspend() {
        inference->enabled_eps_          = false;
        inference->enabled_min_rate_eps_ = false;
        suspended                        = true;
      }

      void restore() {
        if (!suspended) return;
        if (epsilon) inference->enabled_eps_ = true;
        if (rate) inference->enabled_min_rate_eps_ = true;
        suspended = false;
      }

      ~CriteriaSuspension() { restore(); }
    } criteria{this, this->isEnabledEpsilon(), this->isEnabledMinEpsilonRate()};

    if (!_estimator_.isPriorOutweighed()) criteria.suspend();

    // Burn in
    Ip                = this->burnIn_();
    _drawInterrupted_ = false;
    Size nbrSamples   = 0;
    do {
      Ip = this->draw_(&w, Ip);
      if (_drawInterrupted_) {
        // the time limit was reached while rejecting draws: the scheme records it (TimeLimit)
        this->continueApproximationScheme(_estimator_.confidence());
        break;
      }
      _estimator_.update(Ip, w);
      ++nbrSamples;
      this->updateApproximationScheme();
      if (criteria.suspended && _estimator_.isPriorOutweighed()) criteria.restore();
    } while (this->continueApproximationScheme(_estimator_.confidence()));

    if (nbrSamples == 0) {
      GUM_ERROR(IncompatibleEvidence,
                "The hard evidence has a null or too small probability: no compatible draw "
                "within the time limit")
    }
  }

  template < GUM_Numeric GUM_SCALAR >
  bool SamplingInference< GUM_SCALAR >::continueAfterRejection_(Size& nbrRejections) {
    if (++nbrRejections >= _maxConsecutiveRejections_) {
      GUM_ERROR(IncompatibleEvidence,
                "The hard evidence has a null or too small probability: "
                    << nbrRejections
                    << " consecutive draws rejected (key sampling.maxConsecutiveRejections)")
    }
    if (this->isEnabledMaxTime() && this->currentTime() > this->maxTime()) {
      _drawInterrupted_ = true;
      return false;
    }
    return true;
  }

  template < GUM_Numeric GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::addVarSample_(NodeId nod, Instantiation* I) {
    gum::Instantiation Itop = gum::Instantiation(*I);

    I->add(samplingBN().variable(nod));
    I->chgVal(samplingBN().variable(nod), samplingBN().cpt(nod).extract(Itop).draw());
  }

  template < GUM_Numeric GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::onContextualize_(BayesNetFragment< GUM_SCALAR >* bn) {}

  template < GUM_Numeric GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::onEvidenceAdded_(const NodeId id, bool isHardEvidence) {
    if (!isHardEvidence) {
      GUM_ERROR(FatalError, "Approximated inference only accept hard evidence")
    }
    _discardContextualization_();
  }

  template < GUM_Numeric GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::onEvidenceErased_(const NodeId id, bool isHardEvidence) {
    _discardContextualization_();
  }

  template < GUM_Numeric GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::onAllEvidenceErased_(bool contains_hard_evidence) {
    _discardContextualization_();
  }

  template < GUM_Numeric GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::onEvidenceChanged_(const NodeId id,
                                                           bool         hasChangedSoftHard) {
    if (hasChangedSoftHard) {
      GUM_ERROR(FatalError, "Approximated inference only accept hard evidence")
    }
    _discardContextualization_();
  }

  template < GUM_Numeric GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::onModelChanged_(const GraphicalModel* bn) {
    _discardContextualization_();
  }

  template < GUM_Numeric GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::onTensorsChanged_() {
    _discardContextualization_();
  }

  template < GUM_Numeric GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::updateOutdatedStructure_() {}

  template < GUM_Numeric GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::updateOutdatedTensors_() {}

  template < GUM_Numeric GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::onMarginalTargetAdded_(const NodeId id) {}

  template < GUM_Numeric GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::onMarginalTargetErased_(const NodeId id) {}

  template < GUM_Numeric GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::onAllMarginalTargetsAdded_() {}

  template < GUM_Numeric GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::onAllMarginalTargetsErased_() {}

  template < GUM_Numeric GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::onStateChanged_() {
    if (this->isInferenceReady()) {
      _estimator_.clear();
      this->initApproximationScheme();
    }
  }
}   // namespace gum
