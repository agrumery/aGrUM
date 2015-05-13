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
 * @brief KL divergence between BNs brute force implementation
 *
 * @author Pierre-Henri WUILLEMIN
 */

#include <math.h>

#include <agrum/core/hashTable.h>
#include <agrum/BN/IBayesNet.h>

#include <agrum/BN/algorithms/divergence/KL.h>
#include <agrum/BN/algorithms/divergence/GibbsKL.h>
#include <agrum/BN/particles/Gibbs.h>
#include <agrum/core/algorithms/approximationScheme/approximationScheme.h>

#define KL_DEFAULT_MAXITER 10000000
#define KL_DEFAULT_EPSILON 1e-10
#define KL_DEFAULT_MIN_EPSILON_RATE 1e-10
#define KL_DEFAULT_PERIOD_SIZE 500
#define KL_DEFAULT_VERBOSITY false
#define KL_DEFAULT_BURNIN 3000

namespace gum {

  template <typename GUM_SCALAR>
  GibbsKL<GUM_SCALAR>::GibbsKL(const IBayesNet<GUM_SCALAR> &P,
                               const IBayesNet<GUM_SCALAR> &Q)
      : KL<GUM_SCALAR>(P, Q), ApproximationScheme(), particle::Gibbs<GUM_SCALAR>(P) {
    GUM_CONSTRUCTOR(GibbsKL);

    setEpsilon(KL_DEFAULT_EPSILON);
    setMinEpsilonRate(KL_DEFAULT_MIN_EPSILON_RATE);
    setMaxIter(KL_DEFAULT_MAXITER);
    setVerbosity(KL_DEFAULT_VERBOSITY);
    setBurnIn(KL_DEFAULT_BURNIN);
    setPeriodSize(KL_DEFAULT_PERIOD_SIZE);
  }

  template <typename GUM_SCALAR>
  GibbsKL<GUM_SCALAR>::GibbsKL(const KL<GUM_SCALAR> &kl)
      : KL<GUM_SCALAR>(kl), ApproximationScheme(),
        particle::Gibbs<GUM_SCALAR>(kl.p()) {
    GUM_CONSTRUCTOR(GibbsKL);

    setEpsilon(KL_DEFAULT_EPSILON);
    setMinEpsilonRate(KL_DEFAULT_MIN_EPSILON_RATE);
    setMaxIter(KL_DEFAULT_MAXITER);
    setVerbosity(KL_DEFAULT_VERBOSITY);
    setBurnIn(KL_DEFAULT_BURNIN);
    setPeriodSize(KL_DEFAULT_PERIOD_SIZE);
  }

  template <typename GUM_SCALAR> GibbsKL<GUM_SCALAR>::~GibbsKL() {
    GUM_DESTRUCTOR(GibbsKL);
  }

  template <typename GUM_SCALAR> void GibbsKL<GUM_SCALAR>::_computeKL() {

    gum::Instantiation Iq;
    _q.completeInstantiation(Iq);

    initParticle();
    initApproximationScheme();

    // map between particle() variables and _q variables (using name of vars)
    HashTable<const DiscreteVariable *, const DiscreteVariable *> map;

    for (Idx ite = 0; ite < particle().nbrDim(); ++ite) {
      map.insert(&particle().variable(ite),
                 &_q.variableFromName(particle().variable(ite).name()));
    }

    // BURN IN
    for (Idx i = 0; i < burnIn(); i++)
      nextParticle();

    // SAMPLING
    _klPQ = _klQP = _hellinger = (GUM_SCALAR)0.0;
    _errorPQ = _errorQP = 0;
    /// bool check_rate;
    GUM_SCALAR delta, ratio, error;
    delta = ratio = error = (GUM_SCALAR)-1;
    GUM_SCALAR oldPQ = 0.0;
    GUM_SCALAR pp, pq;

    do {
      /// check_rate=false;
      this->disableMinEpsilonRate(); // replace check_rate = false
      nextParticle();
      updateApproximationScheme();

      //_p.synchroInstantiations( Ip,particle() );
      Iq.setValsFrom(map, particle());

      pp = _p.jointProbability(particle());
      pq = _q.jointProbability(Iq);

      if (pp != (GUM_SCALAR)0.0) {
        _hellinger += std::pow(std::sqrt(pp) - std::sqrt(pq), 2) / pp;

        if (pq != (GUM_SCALAR)0.0) {
          _bhattacharya += std::sqrt(pq / pp); // std::sqrt(pp*pq)/pp
          /// check_rate=true;
          this->enableMinEpsilonRate(); // replace check_rate=true;
          ratio = pq / pp;
          delta = (GUM_SCALAR)log2(ratio);
          _klPQ += delta;
        } else {
          _errorPQ++;
        }
      }

      if (pq != (GUM_SCALAR)0.0) {
        if (pp != (GUM_SCALAR)0.0) {
          // if we are here, it is certain that delta and ratio have been computed
          // further lines above. (for now #112-113)
          _klQP += (GUM_SCALAR)(-delta * ratio);
        } else {
          _errorQP++;
        }
      }

      if (this->isEnabledMinEpsilonRate() /* replace check_rate */) {
        // delta is used as a temporary variable
        delta = _klPQ / nbrIterations();
        error = (double) std::fabs(delta - oldPQ);
        oldPQ = delta;
      }
    } while (continueApproximationScheme(error /*,check_rate*/));

    _klPQ = -_klPQ / (nbrIterations());
    _klQP = -_klQP / (nbrIterations());
    _hellinger = std::sqrt(_hellinger / nbrIterations());
    _bhattacharya = -std::log(_bhattacharya);
  }

} // namespace gum
// kate: indent-mode cstyle; indent-width 2; replace-tabs on;
