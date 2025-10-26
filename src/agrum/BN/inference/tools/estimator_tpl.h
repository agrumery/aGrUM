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
#pragma once

/**
 * @file
 * @brief Implementation of Estimator for approximate inference in bayesian
 * networks
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN(_at_LIP6)
 */

namespace gum {

  template < typename GUM_SCALAR >
  Estimator< GUM_SCALAR >::Estimator() {
    GUM_CONSTRUCTOR(Estimator);
    wtotal_ = (GUM_SCALAR)0.;
    ntotal_ = (Size)0;
    bn_     = nullptr;
  }

  template < typename GUM_SCALAR >
  Estimator< GUM_SCALAR >::Estimator(const IBayesNet< GUM_SCALAR >* bn) : Estimator() {
    bn_ = bn;

    for (gum::NodeGraphPartIterator iter = bn->nodes().begin(); iter != bn->nodes().end(); ++iter)
      estimator_.insert(bn->variable(*iter).name(),
                        std::vector< GUM_SCALAR >(bn->variable(*iter).domainSize(), 0.0));

    GUM_CONSTRUCTOR(Estimator);
  }

  template < typename GUM_SCALAR >
  INLINE Estimator< GUM_SCALAR >::~Estimator() {
    GUM_DESTRUCTOR(Estimator);
    // remove all the posteriors computed
    clear();
  }

  /* adds all tensor target variables from a given BN to the Estimator */

  template < typename GUM_SCALAR >
  void Estimator< GUM_SCALAR >::setFromBN(const IBayesNet< GUM_SCALAR >* bn,
                                          const NodeSet&                 hardEvidence) {
    for (gum::NodeGraphPartIterator iter = bn->nodes().begin(); iter != bn->nodes().end(); ++iter) {
      auto v = bn->variable(*iter).name();

      if (!hardEvidence.contains(*iter)) {
        if (estimator_.exists(v))
          estimator_[v]
              = std::vector< GUM_SCALAR >(bn->variable(*iter).domainSize(), (GUM_SCALAR)0.0);
        else
          estimator_.insert(
              v,
              std::vector< GUM_SCALAR >(bn->variable(*iter).domainSize(), (GUM_SCALAR)0.0));
      }
    }
  }

  // we multiply the posteriors obtained by LoopyBeliefPropagation by the it's
  // number of iterations
  template < typename GUM_SCALAR >
  void Estimator< GUM_SCALAR >::setFromLBP(LoopyBeliefPropagation< GUM_SCALAR >* lbp,
                                           const NodeSet&                        hardEvidence,
                                           GUM_SCALAR                            virtualLBPSize) {
    for (const auto& node: lbp->BN().nodes()) {
      if (!hardEvidence.contains(node)) {
        std::vector< GUM_SCALAR > v;
        auto                      p = lbp->posterior(node);
        gum::Instantiation        inst(p);

        for (inst.setFirst(); !inst.end(); ++inst) {
          v.push_back(p[inst] * virtualLBPSize);
        }

        estimator_.insert(lbp->BN().variable(node).name(), v);
      }
    }
    ntotal_ = (Size)virtualLBPSize;
    wtotal_ = virtualLBPSize;
  }

  /*update the Estimator given an instantiation I with weight bias w*/

  template < typename GUM_SCALAR >
  void Estimator< GUM_SCALAR >::update(Instantiation I, GUM_SCALAR w) {
    wtotal_ += w;
    ntotal_ += (Size)1;

    for (Idx i = 0; i < I.nbrDim(); i++) {
      if (estimator_.exists(I.variable(i).name())) estimator_[I.variable(i).name()][I.val(i)] += w;
    }
  }

  /* returns the approximation CPT of a variable */

  template < typename GUM_SCALAR >
  const Tensor< GUM_SCALAR >& Estimator< GUM_SCALAR >::posterior(const DiscreteVariable& var) {
    Tensor< GUM_SCALAR >* p = nullptr;

    if (!estimator_.exists(var.name())) GUM_ERROR(NotFound, "Target variable not found")

    // check if we have already computed the posterior
    if (_target_posteriors_.exists(var.name())) {
      p = _target_posteriors_[var.name()];
    } else {
      p = new Tensor< GUM_SCALAR >();
      *p << var;
      _target_posteriors_.insert(var.name(), p);
    }

    p->fillWith(estimator_[var.name()]);
    p->normalize();
    return *p;
  }

  /* expected value considering a Bernouilli variable with parameter val */

  template < typename GUM_SCALAR >
  GUM_SCALAR Estimator< GUM_SCALAR >::EV(std::string name, Idx val) {
    return estimator_[name][val] / wtotal_;
  }

  /* variance considering a Bernouilli variable with parameter val */

  template < typename GUM_SCALAR >
  GUM_SCALAR Estimator< GUM_SCALAR >::variance(std::string name, Idx val) {
    GUM_SCALAR p = EV(name, val);
    return p * (1 - p);
  }

  /* returns maximum length of confidence intervals for each variable, each
   * parameter */

  template < typename GUM_SCALAR >
  GUM_SCALAR Estimator< GUM_SCALAR >::confidence() {
    GUM_SCALAR ic_max = 0;

    for (auto iter = estimator_.begin(); iter != estimator_.end(); ++iter) {
      for (Idx i = 0; i < iter.val().size(); i++) {
        GUM_SCALAR ic = GUM_SCALAR(2 * 1.96 * std::sqrt(variance(iter.key(), i) / (ntotal_ - 1)));
        if (ic > ic_max) ic_max = ic;
      }
    }

    return ic_max;
  }

  template < typename GUM_SCALAR >
  void Estimator< GUM_SCALAR >::clear() {
    estimator_.clear();
    wtotal_ = (GUM_SCALAR)0;
    ntotal_ = Size(0);
    for (const auto& pot: _target_posteriors_)
      delete pot.second;
    _target_posteriors_.clear();
  }
}   // namespace gum
