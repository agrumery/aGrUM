/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


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
