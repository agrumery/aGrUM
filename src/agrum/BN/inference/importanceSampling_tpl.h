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
 * @brief Implementation of Importance Sampling for inference in Bayesian networks.
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN(_at_LIP6)
 */


#include <agrum/BN/inference/importanceSampling.h>

namespace gum {

  ///  default constructor
  template < typename GUM_SCALAR >
  ImportanceSampling< GUM_SCALAR >::ImportanceSampling(const IBayesNet< GUM_SCALAR >* bn) :
      SamplingInference< GUM_SCALAR >(bn) {
    GUM_CONSTRUCTOR(ImportanceSampling);
  }

  ///  destructor
  template < typename GUM_SCALAR >
  ImportanceSampling< GUM_SCALAR >::~ImportanceSampling() {
    GUM_DESTRUCTOR(ImportanceSampling);
  }

  /// no burn in needed for Importance sampling
  template < typename GUM_SCALAR >
  Instantiation ImportanceSampling< GUM_SCALAR >::burnIn_() {
    Instantiation I;
    return I;
  }

  template < typename GUM_SCALAR >
  Instantiation ImportanceSampling< GUM_SCALAR >::draw_(GUM_SCALAR* w, Instantiation prev) {
    GUM_SCALAR pSurQ;

    do {
      prev.clear();
      pSurQ = 1.;
      for (const auto ev: this->hardEvidenceNodes()) {
        prev.add(this->BN().variable(ev));
        prev.chgVal(this->BN().variable(ev), this->hardEvidence()[ev]);
      }

      for (const auto nod: this->BN().topologicalOrder()) {
        if (!this->hasHardEvidence(nod)) { this->addVarSample_(nod, &prev); }
        auto probaP = this->BN().cpt(nod).get(prev);
        auto probaQ = this->samplingBN().cpt(nod).get(prev);
        if ((probaP == 0) || (probaQ == 0)) {
          pSurQ = 0;
        } else {
          pSurQ = probaP / probaQ;
        }
      }
      if (pSurQ > 0.0) {
        for (const auto ev: this->hardEvidenceNodes()) {
          pSurQ *= this->samplingBN().cpt(ev).get(prev);
        }
      }
    } while (pSurQ == 0);

    *w = pSurQ;
    return prev;
  }

  template < typename GUM_SCALAR >
  void ImportanceSampling< GUM_SCALAR >::unsharpenBN_(BayesNetFragment< GUM_SCALAR >* bn,
                                                      float                           epsilon) {
    for (const auto nod: bn->nodes().asNodeSet()) {
      auto p = bn->cpt(nod).isNonZeroMap().scale(epsilon) + bn->cpt(nod);
      p.normalizeAsCPT();
      bn->installCPT(nod, p);
    }
  }

  template < typename GUM_SCALAR >
  void ImportanceSampling< GUM_SCALAR >::onContextualize_(BayesNetFragment< GUM_SCALAR >* bn) {
    for (const auto ev: this->hardEvidenceNodes()) {
      bn->uninstallCPT(ev);
      bn->installCPT(ev, *(this->evidence()[ev]));
      // we keep the variables with hard evidence but alone
      // bn->uninstallNode( sid[i] );
    }
    GUM_SCALAR minParam    = bn->minNonZeroParam();
    GUM_SCALAR minAccepted = GUM_SCALAR(this->epsilon() / bn->maxVarDomainSize());
    if (minParam < minAccepted) this->unsharpenBN_(bn, float(minAccepted));
  }
}   // namespace gum
