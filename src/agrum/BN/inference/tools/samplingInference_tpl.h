/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
 * @brief Implementation of the non pure virtual methods of class
 * ApproximateInference.
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN
 */

#include <agrum/BN/BayesNetFragment.h>
#include <agrum/BN/algorithms/barrenNodesFinder.h>
#include <agrum/BN/algorithms/dSeparation.h>
#include <agrum/BN/inference/tools/samplingInference.h>


#define DEFAULT_MAXITER 10000000
#define DEFAULT_PERIOD_SIZE 100
#define DEFAULT_VERBOSITY false
#define DEFAULT_TIMEOUT 6000
#define DEFAULT_EPSILON 1e-2
#define DEFAULT_MIN_EPSILON_RATE 1e-5


namespace gum {

  template < typename GUM_SCALAR >
  SamplingInference< GUM_SCALAR >::SamplingInference(
    const IBayesNet< GUM_SCALAR >* bn)
      : ApproximateInference< GUM_SCALAR >(bn)
      , __estimator()
      , __samplingBN(nullptr) {

    this->setEpsilon(DEFAULT_EPSILON);
    this->setMinEpsilonRate(DEFAULT_MIN_EPSILON_RATE);
    this->setMaxIter(DEFAULT_MAXITER);
    this->setVerbosity(DEFAULT_VERBOSITY);
    this->setPeriodSize(DEFAULT_PERIOD_SIZE);
    this->setMaxTime(DEFAULT_TIMEOUT);
    GUM_CONSTRUCTOR(SamplingInference);
  }


  template < typename GUM_SCALAR >
  SamplingInference< GUM_SCALAR >::~SamplingInference() {

    GUM_DESTRUCTOR(SamplingInference);
    if (__samplingBN != nullptr) {
      if (isContextualized) {  // otherwise __samplingBN==&BN()
        delete __samplingBN;
      }
    }
  }

  template < typename GUM_SCALAR >
  INLINE const IBayesNet< GUM_SCALAR >&
               SamplingInference< GUM_SCALAR >::samplingBN() {
    this->prepareInference();
    if (__samplingBN == nullptr)
      return this->BN();
    else
      return *__samplingBN;
  }
  template < typename GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::_setEstimatorFromBN() {
    __estimator.setFromBN(&samplingBN(), this->hardEvidenceNodes());
    this->isSetEstimator = true;
  }

  template < typename GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::_setEstimatorFromLBP(
    LoopyBeliefPropagation< GUM_SCALAR >* lbp, GUM_SCALAR virtualLBPSize) {

    __estimator.setFromLBP(lbp, this->hardEvidenceNodes(), virtualLBPSize);
    this->isSetEstimator = true;
  }


  template < typename GUM_SCALAR >
  const Potential< GUM_SCALAR >&
  SamplingInference< GUM_SCALAR >::_posterior(const NodeId id) {
    return __estimator.posterior(this->BN().variable(id));
  }

  template < typename GUM_SCALAR >
  const Potential< GUM_SCALAR >&
  SamplingInference< GUM_SCALAR >::_posterior(const std::string& name) {
    return _posterior(this->BN().idFromName(name));
  }

  template < typename GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::contextualize() {

    // Finding Barren nodes

    BarrenNodesFinder barr_nodes = BarrenNodesFinder(&this->BN().dag());
    barr_nodes.setTargets(&this->targets());
    barr_nodes.setEvidence(&this->hardEvidenceNodes());
    const NodeSet& barren = barr_nodes.barrenNodes();

    // creating BN fragment
    __samplingBN = new BayesNetFragment< GUM_SCALAR >(this->BN());
    for (auto elmt : this->BN().dag().asNodeSet() - barren)
      __samplingBN->installNode(elmt);

    // D-separated nodes

    dSeparation dsep = gum::dSeparation();
    NodeSet     requisite;
    dsep.requisiteNodes(
      this->BN().dag(),
      this->BN().nodes().asNodeSet(),  // no target for approximateInference
      this->hardEvidenceNodes(),
      this->softEvidenceNodes(),  // should be empty
      requisite);
    requisite += this->hardEvidenceNodes();

    auto nonRequisite = this->BN().dag().asNodeSet() - requisite;

    for (auto elmt : nonRequisite)
      __samplingBN->uninstallNode(elmt);
    for (auto hard : this->hardEvidenceNodes()) {
      gum::Instantiation I;
      I.add(this->BN().variable(hard));
      I.chgVal(this->BN().variable(hard), this->hardEvidence()[hard]);

      for (const auto& child : this->BN().dag().children(hard)) {
        gum::Potential< GUM_SCALAR >* p = new gum::Potential< GUM_SCALAR >();
        *p = this->BN().cpt(child).extract(I);
        __samplingBN->installCPT(child, p);
      }
    }

    this->isContextualized = true;
    this->_onContextualize(__samplingBN);
  }


  template < typename GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::_makeInference() {
    if (!isSetEstimator) this->_setEstimatorFromBN();
    _loopApproxInference();
  }

  template < typename GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::_loopApproxInference() {
    //@todo This should be in __prepareInference
    if (!isContextualized) {
      this->contextualize();
    }

    this->initApproximationScheme();
    gum::Instantiation Ip;
    float              w = .0;  //

    // Burn in
    Ip = this->_burnIn();
    do {

      Ip = this->_draw(&w, Ip);
      __estimator.update(Ip, w);
      this->updateApproximationScheme();
    } while (this->continueApproximationScheme(__estimator.confidence()));

    this->isSetEstimator = false;
  }


  template < typename GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::_addVarSample(NodeId         nod,
                                                         Instantiation* I) {
    gum::Instantiation Itop = gum::Instantiation(*I);

    I->add(samplingBN().variable(nod));
    I->chgVal(samplingBN().variable(nod),
              samplingBN().cpt(nod).extract(Itop).draw());
  }
}
