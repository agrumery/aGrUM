/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief Implementation of the non pure virtual methods of class
 * ApproximateInference.
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN(@LIP6)
 */

#include <agrum/BN/BayesNetFragment.h>
#include <agrum/BN/algorithms/barrenNodesFinder.h>
#include <agrum/BN/algorithms/dSeparation.h>
#include <agrum/BN/inference/tools/samplingInference.h>


#define DEFAULT_MAXITER          10000000
#define DEFAULT_PERIOD_SIZE      100
#define DEFAULT_VERBOSITY        false
#define DEFAULT_TIMEOUT          6000
#define DEFAULT_EPSILON          1e-2
#define DEFAULT_MIN_EPSILON_RATE 1e-5


namespace gum {

  template < typename GUM_SCALAR >
  SamplingInference< GUM_SCALAR >::SamplingInference(
     const IBayesNet< GUM_SCALAR >* bn) :
      ApproximateInference< GUM_SCALAR >(bn),
      estimator__(), samplingBN__(nullptr) {
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
    if (samplingBN__ != nullptr) {
      if (isContextualized) {   // otherwise samplingBN__==&BN()
        delete samplingBN__;
      }
    }
  }

  template < typename GUM_SCALAR >
  INLINE const IBayesNet< GUM_SCALAR >&
               SamplingInference< GUM_SCALAR >::samplingBN() {
    this->prepareInference();
    if (samplingBN__ == nullptr)
      return this->BN();
    else
      return *samplingBN__;
  }
  template < typename GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::setEstimatorFromBN_() {
    estimator__.setFromBN(&samplingBN(), this->hardEvidenceNodes());
    this->isSetEstimator = true;
  }

  template < typename GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::setEstimatorFromLBP_(
     LoopyBeliefPropagation< GUM_SCALAR >* lbp, GUM_SCALAR virtualLBPSize) {
    estimator__.setFromLBP(lbp, this->hardEvidenceNodes(), virtualLBPSize);
    this->isSetEstimator = true;
  }


  template < typename GUM_SCALAR >
  const Potential< GUM_SCALAR >&
     SamplingInference< GUM_SCALAR >::currentPosterior(NodeId id) {
    return estimator__.posterior(this->BN().variable(id));
  }

  template < typename GUM_SCALAR >
  const Potential< GUM_SCALAR >&
     SamplingInference< GUM_SCALAR >::currentPosterior(const std::string& name) {
    return currentPosterior(this->BN().idFromName(name));
  }

  template < typename GUM_SCALAR >
  const Potential< GUM_SCALAR >&
     SamplingInference< GUM_SCALAR >::posterior_(NodeId id) {
    return estimator__.posterior(this->BN().variable(id));
  }

  template < typename GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::contextualize() {
    // Finding Barren nodes

    BarrenNodesFinder barr_nodes = BarrenNodesFinder(&this->BN().dag());
    barr_nodes.setTargets(&this->targets());
    barr_nodes.setEvidence(&this->hardEvidenceNodes());
    const NodeSet& barren = barr_nodes.barrenNodes();

    // creating BN fragment
    samplingBN__ = new BayesNetFragment< GUM_SCALAR >(this->BN());
    for (const auto elmt: this->BN().dag().asNodeSet() - barren)
      samplingBN__->installNode(elmt);

    // D-separated nodes

    dSeparation dsep = gum::dSeparation();
    NodeSet     requisite;
    dsep.requisiteNodes(
       this->BN().dag(),
       this->BN().nodes().asNodeSet(),   // no target for approximateInference
       this->hardEvidenceNodes(),
       this->softEvidenceNodes(),   // should be empty
       requisite);
    requisite += this->hardEvidenceNodes();

    auto nonRequisite = this->BN().dag().asNodeSet() - requisite;

    for (const auto elmt: nonRequisite)
      samplingBN__->uninstallNode(elmt);
    for (const auto hard: this->hardEvidenceNodes()) {
      gum::Instantiation I;
      I.add(this->BN().variable(hard));
      I.chgVal(this->BN().variable(hard), this->hardEvidence()[hard]);

      for (const auto& child: this->BN().children(hard)) {
        samplingBN__->installCPT(child, this->BN().cpt(child).extract(I));
      }
    }

    this->isContextualized = true;
    this->onContextualize_(samplingBN__);
  }


  template < typename GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::makeInference_() {
    if (!isSetEstimator) this->setEstimatorFromBN_();
    loopApproxInference_();
  }

  template < typename GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::loopApproxInference_() {
    //@todo This should be in prepareInference__
    if (!isContextualized) { this->contextualize(); }

    this->initApproximationScheme();
    gum::Instantiation Ip;
    GUM_SCALAR         w = .0;   //

    // Burn in
    Ip = this->burnIn_();
    do {
      Ip = this->draw_(&w, Ip);
      estimator__.update(Ip, w);
      this->updateApproximationScheme();
    } while (this->continueApproximationScheme(estimator__.confidence()));

    this->isSetEstimator = false;
  }


  template < typename GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::addVarSample_(NodeId         nod,
                                                      Instantiation* I) {
    gum::Instantiation Itop = gum::Instantiation(*I);

    I->add(samplingBN().variable(nod));
    I->chgVal(samplingBN().variable(nod),
              samplingBN().cpt(nod).extract(Itop).draw());
  }

  template < typename GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::onContextualize_(
     BayesNetFragment< GUM_SCALAR >* bn) {}


  template < typename GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::onEvidenceAdded_(const NodeId id,
                                                         bool isHardEvidence) {
    if (!isHardEvidence) {
      GUM_ERROR(FatalError, "Approximated inference only accept hard evidence");
    }
  }

  template < typename GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::onEvidenceErased_(const NodeId id,
                                                          bool isHardEvidence) {}

  template < typename GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::onAllEvidenceErased_(
     bool contains_hard_evidence) {}

  template < typename GUM_SCALAR >
  void
     SamplingInference< GUM_SCALAR >::onEvidenceChanged_(const NodeId id,
                                                         bool hasChangedSoftHard) {
    if (hasChangedSoftHard) {
      GUM_ERROR(FatalError, "Approximated inference only accept hard evidence");
    }
  }

  template < typename GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::onModelChanged_(const GraphicalModel* bn) {
  }

  template < typename GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::updateOutdatedStructure_() {}

  template < typename GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::updateOutdatedPotentials_() {}

  template < typename GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::onMarginalTargetAdded_(const NodeId id) {}

  template < typename GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::onMarginalTargetErased_(const NodeId id) {}

  template < typename GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::onAllMarginalTargetsAdded_() {}

  template < typename GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::onAllMarginalTargetsErased_() {}

  template < typename GUM_SCALAR >
  void SamplingInference< GUM_SCALAR >::onStateChanged_() {
    if (this->isInferenceReady()) {
      estimator__.clear();
      this->initApproximationScheme();
    }
  }
}   // namespace gum
