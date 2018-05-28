/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES et Pierre-Henri WUILLEMIN   *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/**
 * @file
 * @brief Implementation of Importance Sampling for inference in Bayesian Networks.
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN
 */


#include <agrum/BN/inference/importanceSampling.h>


namespace gum {

  ///  default constructor
  template < typename GUM_SCALAR >
  ImportanceSampling< GUM_SCALAR >::ImportanceSampling(
    const IBayesNet< GUM_SCALAR >* bn) :
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
  Instantiation ImportanceSampling< GUM_SCALAR >::_burnIn() {
    Instantiation I;
    return I;
  }

  template < typename GUM_SCALAR >
  Instantiation ImportanceSampling< GUM_SCALAR >::_draw(float*        w,
                                                        Instantiation prev) {
    GUM_SCALAR pSurQ;

    do {
      prev.clear();
      pSurQ = 1.;
      for (const auto ev : this->hardEvidenceNodes()) {
        prev.add(this->BN().variable(ev));
        prev.chgVal(this->BN().variable(ev), this->hardEvidence()[ev]);
      }

      for (const auto nod : this->BN().topologicalOrder()) {
        if (!this->hasHardEvidence(nod)) { this->_addVarSample(nod, &prev); }
        auto probaP = this->BN().cpt(nod).get(prev);
        auto probaQ = this->samplingBN().cpt(nod).get(prev);
        if ((probaP == 0) || (probaQ == 0)) {
          pSurQ = 0;
        } else {
          pSurQ = probaP / probaQ;
        }
      }
      if (pSurQ > 0.0) {
        for (const auto ev : this->hardEvidenceNodes()) {
          pSurQ *= this->samplingBN().cpt(ev).get(prev);
        }
      }
    } while (pSurQ == 0);

    *w = pSurQ;
    return prev;
  }


  template < typename GUM_SCALAR >
  void ImportanceSampling< GUM_SCALAR >::_unsharpenBN(
    BayesNetFragment< GUM_SCALAR >* bn, float epsilon) {
    for (const auto nod : bn->nodes().asNodeSet()) {
      auto p = new Potential< GUM_SCALAR >();
      *p = bn->cpt(nod).isNonZeroMap().scale(epsilon) + bn->cpt(nod);
      p->normalizeAsCPT();
      bn->installCPT(nod, p);
    }
  }

  template < typename GUM_SCALAR >
  void ImportanceSampling< GUM_SCALAR >::_onContextualize(
    BayesNetFragment< GUM_SCALAR >* bn) {
    for (const auto ev : this->hardEvidenceNodes()) {
      bn->uninstallCPT(ev);
      bn->installCPT(ev, new Potential< GUM_SCALAR >(*this->evidence()[ev]));
      // we keep the variables with hard evidence but alone
      // bn->uninstallNode( sid[i] );
    }
    auto minParam = bn->minNonZeroParam();
    auto minAccepted = this->epsilon() / bn->maxVarDomainSize();
    if (minParam < minAccepted) this->_unsharpenBN(bn, minAccepted);
  }
}   // namespace gum
