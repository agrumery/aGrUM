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
 * @brief Implementation of Gibbs inference methods in Bayesian networks.
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN(_at_LIP6)
 */


#include <algorithm>
#include <cstdint>

#include <agrum/base/core/configuration/configuration.h>
#include <agrum/BN/inference/tools/evidencePruning.h>
#include <agrum/BN/inference/tools/gibbsOperator.h>

#include <agrum/base/core/utils_random.h>

namespace gum {

  template < GUM_Numeric GUM_SCALAR >
  GibbsOperator< GUM_SCALAR >::GibbsOperator(const IBayesNet< GUM_SCALAR >& BN,
                                             const NodeProperty< Idx >*     hardEv,
                                             const std::string&             section) :
      GibbsOperator(
          BN,
          hardEv,
          // number of variables drawn between two samples (percentage of the variables, in [0,100]
          // according to the domain of the key)
          1
              + (BN.size()
                 * static_cast< Size >(
                     Configuration::instance().typed< std::int64_t >(section, "percentDrawnSample"))
                 / 100),
          Configuration::instance().typed< bool >(section, "drawnAtRandom")) {}

  template < GUM_Numeric GUM_SCALAR >
  GibbsOperator< GUM_SCALAR >::GibbsOperator(const IBayesNet< GUM_SCALAR >& BN,
                                             const NodeProperty< Idx >*     hardEv,
                                             Size                           nbr,
                                             bool                           atRandom) :
      counting_(0), samplingBn_(BN), hardEv_(hardEv), nbr_(nbr), atRandom_(atRandom) {
    GUM_CONSTRUCTOR(GibbsOperator);
  }

  template < GUM_Numeric GUM_SCALAR >
  GibbsOperator< GUM_SCALAR >::~GibbsOperator() {
    GUM_DESTRUCTOR(GibbsOperator);
  }

  template < GUM_Numeric GUM_SCALAR >
  void GibbsOperator< GUM_SCALAR >::updateSamplingNodes_() {
    samplingNodes_.clear();
    _deterministic_.clear();
    _detDescendants_.clear();
    _factors_.clear();
    bool unobserved = false;
    for (const auto node: samplingBn_.nodes())
      if (hardEv_ == nullptr || !hardEv_->exists(node)) {
        unobserved = true;
        if (_isDeterministic_(node)) _deterministic_.insert(node);
        else samplingNodes_.insert(node);
      }
    if (!unobserved) {
      GUM_ERROR(InvalidArgument, "No node to sample (too many nodes or too much evidence)!")
    }
    // if every non-observed node is deterministic, the chain is constant (and exact)
    if (nbr_ > samplingNodes_.size()) nbr_ = samplingNodes_.size();

    if (_deterministic_.empty()) return;
    const auto order = samplingBn_.topologicalOrder();
    for (const auto node: samplingNodes_) {
      // the deterministic nodes reached from node through deterministic nodes only
      NodeSet               reached;
      std::vector< NodeId > toVisit{node};
      while (!toVisit.empty()) {
        const auto current = toVisit.back();
        toVisit.pop_back();
        for (const auto child: samplingBn_.children(current))
          if (_deterministic_.contains(child) && !reached.contains(child)) {
            reached.insert(child);
            toVisit.push_back(child);
          }
      }
      if (reached.empty()) continue;

      std::vector< NodeId > dets(reached.begin(), reached.end());
      std::sort(dets.begin(), dets.end(), [&order](NodeId a, NodeId b) {
        return order.pos(a) < order.pos(b);
      });
      NodeSet factors{node};
      for (const auto child: samplingBn_.children(node))
        if (!reached.contains(child)) factors.insert(child);
      for (const auto det: dets)
        for (const auto child: samplingBn_.children(det))
          if (!reached.contains(child)) factors.insert(child);
      _detDescendants_.insert(node, std::move(dets));
      _factors_.insert(node, std::vector< NodeId >(factors.begin(), factors.end()));
    }
  }

  template < GUM_Numeric GUM_SCALAR >
  bool GibbsOperator< GUM_SCALAR >::_isDeterministic_(NodeId node) const {
    const auto&   cpt  = samplingBn_.cpt(node);
    const Size    rows = cpt.domainSize() / samplingBn_.variable(node).domainSize();
    Size          ones = 0;
    Instantiation i(cpt);
    for (i.setFirst(); !i.end(); i.inc())
      if (cpt[i] == (GUM_SCALAR)1.0) ++ones;
    return ones == rows;
  }

  template < GUM_Numeric GUM_SCALAR >
  void GibbsOperator< GUM_SCALAR >::_setDeterministicValue_(NodeId node, Instantiation* I) const {
    const auto& var = samplingBn_.variable(node);
    const auto& cpt = samplingBn_.cpt(node);
    for (Idx value = 0; value < var.domainSize(); ++value) {
      I->chgVal(var, value);
      if (cpt[*I] == (GUM_SCALAR)1.0) return;
    }
  }

  // returns a first sample with a positive probability (see the documentation in gibbsOperator.h)
  template < GUM_Numeric GUM_SCALAR >
  Instantiation GibbsOperator< GUM_SCALAR >::monteCarloSample(
      const NodeProperty< std::vector< bool > >* prunedDomains) {
    // From a sample with a null probability, every Gibbs move may have a null mass: the chain would
    // stay there forever. Hence the first sample must have a positive probability.

    // 0. the values that cannot belong to such a sample are pruned (unless already done)
    if (prunedDomains != nullptr) {
      _allowed_ = *prunedDomains;
    } else if (!pruneDomainsByEvidence(samplingBn_, hardEv_, _allowed_)) {
      GUM_ERROR(IncompatibleEvidence,
                "The hard evidence has a null probability (incompatible with the model)")
    }

    // 1. a few forward samples (in the pruned domains)
    Instantiation I;
    for (Size tries = 0; tries < _nbrRejectionTries_; ++tries) {
      I = _forwardSample_();
      if (_isCompatible_(I)) return I;
    }

    // 2. Gibbs moves in a smoothed model (null probabilities replaced by epsilon): the chain is
    // attracted by the samples with fewer null probabilities. A small epsilon gives a strong
    // attraction, a larger one helps to get through samples with more null probabilities: epsilon
    // grows only when the chain is stuck. The sampled distribution does not matter here: we stop as
    // soon as the sample is compatible. The chain may still be stuck on a plateau, depending on its
    // start: hence a few restarts from new forward samples.
    std::vector< NodeId > sampled;
    for (const auto node: samplingBn_.nodes())
      if (hardEv_ == nullptr || !hardEv_->exists(node)) sampled.push_back(node);

    // the smallest positive probability of each CPT (used by the smoothing) is computed once
    _minNonZero_.clear();
    for (const auto node: samplingBn_.nodes())
      _minNonZero_.insert(node, samplingBn_.cpt(node).minNonZero());

    for (Size restart = 0; restart < _nbrSmoothingRestarts_; ++restart) {
      if (restart > 0) I = _forwardSample_();
      for (const auto epsilon: _smoothingLevels_) {
        for (Size sweep = 0; sweep < _nbrSweepsBySmoothingLevel_; ++sweep) {
          for (const auto node: sampled)
            _smoothedGibbsSample_(node, &I, GUM_SCALAR(epsilon));
          if (_isCompatible_(I)) return I;
        }
      }
    }
    GUM_ERROR(OperationNotAllowed,
              "No sample with a positive probability found: the evidence may be incompatible "
              "with the model")
  }

  template < GUM_Numeric GUM_SCALAR >
  Instantiation GibbsOperator< GUM_SCALAR >::_forwardSample_() {
    gum::Instantiation I;

    for (const auto nod: samplingBn_.topologicalOrder()) {
      I.add(samplingBn_.variable(nod));
      if (hardEv_ != nullptr && hardEv_->exists(nod)) {
        I.chgVal(samplingBn_.variable(nod), (*hardEv_)[nod]);
      } else {
        _drawVarMonteCarlo_(nod, &I);
      }
    }
    return I;
  }

  template < GUM_Numeric GUM_SCALAR >
  bool GibbsOperator< GUM_SCALAR >::_isCompatible_(const Instantiation& I) const {
    for (const auto node: samplingBn_.nodes())
      if (samplingBn_.cpt(node).get(I) == GUM_SCALAR(0)) return false;
    return true;
  }

  template < GUM_Numeric GUM_SCALAR >
  void GibbsOperator< GUM_SCALAR >::_smoothedGibbsSample_(NodeId         id,
                                                          Instantiation* I,
                                                          GUM_SCALAR     epsilon) {
    gum::Instantiation Itop(*I);
    Itop.erase(samplingBn_.variable(id));
    // a null probability is replaced by epsilon times the smallest positive probability of its CPT:
    // a null probability always weighs less than any positive probability of the same CPT
    auto smoothed = [this, epsilon](NodeId node, Tensor< GUM_SCALAR > t) {
      const GUM_SCALAR replacement = epsilon * _minNonZero_[node];
      Instantiation    J(t);
      for (J.setFirst(); !J.end(); J.inc())
        if (t[J] == GUM_SCALAR(0)) t.set(J, replacement);
      return t;
    };
    gum::Tensor< GUM_SCALAR > p = smoothed(id, samplingBn_.cpt(id).extract(Itop));
    for (const auto nod: samplingBn_.children(id))
      p *= smoothed(nod, samplingBn_.cpt(nod).extract(Itop));
    _mask_(id, p);
    p.normalize();
    I->chgVal(samplingBn_.variable(id), p.draw());
  }

  template < GUM_Numeric GUM_SCALAR >
  void GibbsOperator< GUM_SCALAR >::_mask_(NodeId node, Tensor< GUM_SCALAR >& p) const {
    const auto allowed = _allowed_.tryGet(node);
    if (!allowed) return;
    Instantiation J(p);
    for (J.setFirst(); !J.end(); J.inc())
      if (!(*allowed)[J.val(0)]) p.set(J, GUM_SCALAR(0));
  }

  template < GUM_Numeric GUM_SCALAR >
  void GibbsOperator< GUM_SCALAR >::_drawVarMonteCarlo_(NodeId nod, Instantiation* I) {
    gum::Instantiation Itop(*I);
    Itop.erase(samplingBn_.variable(nod));
    auto p = samplingBn_.cpt(nod).extract(Itop);
    _mask_(nod, p);
    // if no allowed value is possible, the sample is incompatible anyway
    if (p.sum() == GUM_SCALAR(0)) p = samplingBn_.cpt(nod).extract(Itop);
    else p.normalize();
    I->chgVal(samplingBn_.variable(nod), p.draw());
  }

  template < GUM_Numeric GUM_SCALAR >
  Instantiation GibbsOperator< GUM_SCALAR >::nextSample(Instantiation prev) {
    for (Idx i = 0; i < nbr_; i++) {
      auto pos
          = atRandom_ ? randomValue(samplingNodes_.size()) : (counting_ % samplingNodes_.size());
      this->_GibbsSample_(samplingNodes_[pos], &prev);
      counting_++;
    }
    return prev;
  }

  /// change in Instantiation I a new drawn value for id

  template < GUM_Numeric GUM_SCALAR >
  void GibbsOperator< GUM_SCALAR >::_GibbsSample_(NodeId id, Instantiation* I) {
    if (_detDescendants_.exists(id)) {
      // id is drawn jointly with the deterministic nodes that follow it
      const auto& var     = samplingBn_.variable(id);
      const auto& dets    = _detDescendants_[id];
      const auto& factors = _factors_[id];

      const Idx          old = I->val(var);
      std::vector< Idx > oldDets;
      for (const auto det: dets)
        oldDets.push_back(I->val(samplingBn_.variable(det)));

      Tensor< GUM_SCALAR > p;
      p.add(var);
      Instantiation                     ip(p);
      std::vector< std::vector< Idx > > detValues(var.domainSize());
      for (Idx x = 0; x < var.domainSize(); ++x) {
        I->chgVal(var, x);
        for (const auto det: dets) {
          _setDeterministicValue_(det, I);
          detValues[x].push_back(I->val(samplingBn_.variable(det)));
        }
        GUM_SCALAR weight = 1.0;
        for (const auto factor: factors)
          weight *= samplingBn_.cpt(factor)[*I];
        ip.chgVal(var, x);
        p.set(ip, weight);
      }

      Idx                       x      = old;
      const std::vector< Idx >* values = &oldDets;
      if (p.sum() != 0) {
        p.normalize();
        x      = p.draw();
        values = &detValues[x];
      }
      I->chgVal(var, x);
      for (Idx k = 0; k < dets.size(); ++k)
        I->chgVal(samplingBn_.variable(dets[k]), (*values)[k]);
      return;
    }

    gum::Instantiation Itop(*I);
    Itop.erase(samplingBn_.variable(id));
    gum::Tensor< GUM_SCALAR > p = samplingBn_.cpt(id).extract(Itop);
    for (const auto nod: samplingBn_.children(id))
      p *= samplingBn_.cpt(nod).extract(Itop);
    GUM_ASSERT(p.nbrDim() == 1);
    if (p.sum() != 0) {
      p.normalize();
      I->chgVal(samplingBn_.variable(id), p.draw());
    }
  }

  template < GUM_Numeric GUM_SCALAR >
  Size GibbsOperator< GUM_SCALAR >::nbrDrawnVar() const {
    return nbr_;
  }

  template < GUM_Numeric GUM_SCALAR >
  void GibbsOperator< GUM_SCALAR >::setNbrDrawnVar(Size nbr) {
    nbr_ = nbr;
  }

  template < GUM_Numeric GUM_SCALAR >
  bool GibbsOperator< GUM_SCALAR >::isDrawnAtRandom() const {
    return atRandom_;
  }

  template < GUM_Numeric GUM_SCALAR >
  void GibbsOperator< GUM_SCALAR >::setDrawnAtRandom(bool atRandom) {
    atRandom_ = atRandom;
  }

}   // namespace gum
