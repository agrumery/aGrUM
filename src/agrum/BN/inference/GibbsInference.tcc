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
 * @brief Implementation of Gibbs sampling for inference in Bayesian Networks.
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <sstream>
#include <string>
#include <algorithm>

#define INFERENCE_DEFAULT_MAXITER 10000000
#define INFERENCE_DEFAULT_EPSILON 1e-4 * std::log(2)
#define INFERENCE_DEFAULT_MIN_EPSILON_RATE 1e-4
#define INFERENCE_DEFAULT_PERIOD_SIZE 500
#define INFERENCE_DEFAULT_VERBOSITY false
#define INFERENCE_DEFAULT_BURNIN 3000

// to ease parsing for IDE
#include <agrum/BN/inference/GibbsInference.h>
#include <agrum/BN/inference/BayesNetInference.h>
#include <agrum/BN/particles/Gibbs.h>

namespace gum {
  /// default constructor
  template <typename GUM_SCALAR>
  GibbsInference<GUM_SCALAR>::GibbsInference(const IBayesNet<GUM_SCALAR> &BN)
      : ApproximationScheme(), BayesNetInference<GUM_SCALAR>(BN),
        particle::Gibbs<GUM_SCALAR>(BN) {
    // for debugging purposes
    GUM_CONSTRUCTOR(GibbsInference);

    setEpsilon(INFERENCE_DEFAULT_EPSILON);
    setMinEpsilonRate(INFERENCE_DEFAULT_MIN_EPSILON_RATE);
    setMaxIter(INFERENCE_DEFAULT_MAXITER);
    setVerbosity(INFERENCE_DEFAULT_VERBOSITY);
    setBurnIn(INFERENCE_DEFAULT_BURNIN);
    setPeriodSize(INFERENCE_DEFAULT_PERIOD_SIZE);

    for (auto node : bn().dag().nodes()) {
      const DiscreteVariable &var = bn().variable(node);
      // feed the __sampling
      Potential<GUM_SCALAR> *tmp = new Potential<GUM_SCALAR>();
      (*tmp) << var;
      __sampling_nbr.insert(node, tmp);
    }

    setRequiredInference();
  }

  /// destructor
  template <typename GUM_SCALAR>
  INLINE GibbsInference<GUM_SCALAR>::~GibbsInference() {
    GUM_DESTRUCTOR(GibbsInference);

    // remove all the created potentials and instantiations
    for (auto &elt : __sampling_nbr)
      delete (elt.second);
  }

  /// setter/getter for __inference_is_required
  template <typename GUM_SCALAR>
  INLINE void GibbsInference<GUM_SCALAR>::setRequiredInference() {
    this->_invalidatePosteriors();
    __inference_is_required = true;
  }

  /// setter/getter for __inference_is_required
  template <typename GUM_SCALAR>
  INLINE bool GibbsInference<GUM_SCALAR>::isInferenceRequired() {
    return __inference_is_required;
  }

  /// setter/getter for __inference_is_required
  template <typename GUM_SCALAR>
  INLINE void GibbsInference<GUM_SCALAR>::__unsetRequiredInference() {
    __inference_is_required = false;
  }

  template <typename GUM_SCALAR>
  INLINE void GibbsInference<GUM_SCALAR>::__initStats() {
    for (auto &elt : __sampling_nbr) {
      elt.second->fill((GUM_SCALAR)0);
    }
  }

  /**
     As a stopping criterion, we use sum of entropy.
     To compute entropy,
     instead of \sum p ln \frac{p}{q}
     We use the fact that we have Frequency : P and Q on a (nbr=nb+1) data set.
     After simplification, it remains that
     entropy=\frac{1}{nbr}ln P_i \frac{P_i}{Q_i}+ln{nbr-1}{nbr}
  */
  template <typename GUM_SCALAR>
  INLINE double GibbsInference<GUM_SCALAR>::__updateStats_with_err(Size nb) {
    Size nbr = nb + 1; // we compute the new iteration
    double sum_entropy = 0;

    for (auto &elt : __sampling_nbr) {
      GUM_SCALAR n_v = 1 + elt.second->get(particle());
      elt.second->set(particle(), n_v);

      if (n_v == (GUM_SCALAR)1)
        sum_entropy += 100;
      else
        sum_entropy += n_v * std::log(n_v / (n_v - 1));
    }

    return sum_entropy / nbr + __sampling_nbr.size() * std::log((double)nbr / nb);
  }

  /** same as __updateStats_with_err but with no entropy computation */
  template <typename GUM_SCALAR>
  INLINE void GibbsInference<GUM_SCALAR>::__updateStats_without_err() {
    for (auto &elt : __sampling_nbr) {
      elt.second->set(particle(), elt.second->get(particle()) + 1);
    }
  }

  /// remove a given evidence from the graph
  template <typename GUM_SCALAR>
  INLINE void
  GibbsInference<GUM_SCALAR>::eraseEvidence(const Potential<GUM_SCALAR> *pot) {
    particle::Gibbs<GUM_SCALAR>::eraseEvidence(pot);
    setRequiredInference();
  }

  /// remove all evidence from the graph
  template <typename GUM_SCALAR>
  INLINE void GibbsInference<GUM_SCALAR>::eraseAllEvidence() {
    particle::Gibbs<GUM_SCALAR>::eraseAllEvidence();
    setRequiredInference();
  }

  /// insert new evidence in the graph
  template <typename GUM_SCALAR>
  INLINE void GibbsInference<GUM_SCALAR>::insertEvidence(
      const List<const Potential<GUM_SCALAR> *> &pot_list) {
    this->_invalidatePosteriors();
    particle::Gibbs<GUM_SCALAR>::insertEvidence(pot_list);
    setRequiredInference();
  }

  /// Returns the probability of the variable.
  template <typename GUM_SCALAR>
  INLINE void
  GibbsInference<GUM_SCALAR>::_fillPosterior(NodeId id,
                                             Potential<GUM_SCALAR> &posterior) {
    if (isInferenceRequired())
      makeInference();

    posterior = *(__sampling_nbr[id]);
    posterior.normalize();
  }

  INLINE
  void add_and_instancie(Instantiation &I, const DiscreteVariable &v,
                         const Instantiation &__current_sample) {
    try {
      I << v;
      I.chgVal(v, __current_sample.val(v));
    } catch (DuplicateElement e) {
      // do nothing, it's OK
    }
  }

  /// Returns the probability of the variables.
  template <typename GUM_SCALAR> void GibbsInference<GUM_SCALAR>::makeInference() {
    if (!isInferenceRequired())
      return;

    __initStats();
    initParticle();
    initApproximationScheme();

    // SAMPLING
    double error = 0.0;

    do {
      nextParticle();
      updateApproximationScheme();

      if (startOfPeriod())
        error = __updateStats_with_err(nbrIterations() + burnIn());
      else
        __updateStats_without_err();
    } while (continueApproximationScheme(error));

    __unsetRequiredInference();
  }

} /* namespace gum */

#endif // DOXYGEN_SHOULD_SKIP_THIS
// kate: indent-mode cstyle; indent-width 2; replace-tabs on;
