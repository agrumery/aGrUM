/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief KL divergence between BNs -- implementation using Gibbs sampling
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/tools/core/math/math_utils.h>
#include <agrum/BN/IBayesNet.h>
#include <agrum/BN/algorithms/divergence/GibbsBNdistance.h>
#include <agrum/BN/inference/tools/gibbsOperator.h>
#include <agrum/tools/core/approximations/approximationScheme.h>
#include <agrum/tools/core/hashTable.h>

#define GIBBSKL_DEFAULT_MAXITER          10000000
#define GIBBSKL_DEFAULT_EPSILON          1e-10
#define GIBBSKL_DEFAULT_MIN_EPSILON_RATE 1e-10
#define GIBBSKL_DEFAULT_PERIOD_SIZE      200
#define GIBBSKL_DEFAULT_VERBOSITY        false
#define GIBBSKL_DEFAULT_BURNIN           2000
#define GIBBSKL_DEFAULT_TIMEOUT          6000

#define GIBBSKL_POURCENT_DRAWN_SAMPLE 10   // percent drawn
#define GIBBSKL_DRAWN_AT_RANDOM       false

namespace gum {


  template < typename GUM_SCALAR >
  GibbsBNdistance< GUM_SCALAR >::GibbsBNdistance(const IBayesNet< GUM_SCALAR >& P,
                                                 const IBayesNet< GUM_SCALAR >& Q) :
      BNdistance< GUM_SCALAR >(P, Q),
      ApproximationScheme(), GibbsOperator< GUM_SCALAR >(
                                P,
                                nullptr,
                                1 + (P.size() * GIBBSKL_POURCENT_DRAWN_SAMPLE / 100),
                                GIBBSKL_DRAWN_AT_RANDOM) {
    GUM_CONSTRUCTOR(GibbsBNdistance);

    setEpsilon(GIBBSKL_DEFAULT_EPSILON);
    setMinEpsilonRate(GIBBSKL_DEFAULT_MIN_EPSILON_RATE);
    setMaxIter(GIBBSKL_DEFAULT_MAXITER);
    setVerbosity(GIBBSKL_DEFAULT_VERBOSITY);
    setBurnIn(GIBBSKL_DEFAULT_BURNIN);
    setPeriodSize(GIBBSKL_DEFAULT_PERIOD_SIZE);
    setMaxTime(GIBBSKL_DEFAULT_TIMEOUT);
  }

  template < typename GUM_SCALAR >
  GibbsBNdistance< GUM_SCALAR >::GibbsBNdistance(const BNdistance< GUM_SCALAR >& kl) :
      BNdistance< GUM_SCALAR >(kl), ApproximationScheme()
      // Gibbs operator with 10% of nodes changes at random between each samples
      ,
      GibbsOperator< GUM_SCALAR >(kl.p(),
                                  nullptr,
                                  1 + (kl.p().size() * GIBBSKL_POURCENT_DRAWN_SAMPLE / 100),
                                  true) {
    GUM_CONSTRUCTOR(GibbsBNdistance);

    setEpsilon(GIBBSKL_DEFAULT_EPSILON);
    setMinEpsilonRate(GIBBSKL_DEFAULT_MIN_EPSILON_RATE);
    setMaxIter(GIBBSKL_DEFAULT_MAXITER);
    setVerbosity(GIBBSKL_DEFAULT_VERBOSITY);
    setBurnIn(GIBBSKL_DEFAULT_BURNIN);
    setPeriodSize(GIBBSKL_DEFAULT_PERIOD_SIZE);
    setMaxTime(GIBBSKL_DEFAULT_TIMEOUT);
  }

  template < typename GUM_SCALAR >
  GibbsBNdistance< GUM_SCALAR >::~GibbsBNdistance() {
    GUM_DESTRUCTOR(GibbsBNdistance);
  }

  template < typename GUM_SCALAR >
  void GibbsBNdistance< GUM_SCALAR >::computeKL_() {
    auto Iq = q_.completeInstantiation();

    gum::Instantiation I = this->monteCarloSample();
    initApproximationScheme();

    // map between particle() variables and q_ variables (using name of vars)
    HashTable< const DiscreteVariable*, const DiscreteVariable* > map;

    for (Idx ite = 0; ite < I.nbrDim(); ++ite) {
      map.insert(&I.variable(ite), &q_.variableFromName(I.variable(ite).name()));
    }

    // BURN IN
    for (Idx i = 0; i < burnIn(); i++)
      I = this->nextSample(I);

    // SAMPLING
    klPQ_ = klQP_ = hellinger_ = jsd_ = (GUM_SCALAR)0.0;
    errorPQ_ = errorQP_ = 0;
    /// bool check_rate;
    GUM_SCALAR delta, ratio, error;
    delta = ratio = error = (GUM_SCALAR)-1;
    GUM_SCALAR oldPQ      = 0.0;
    GUM_SCALAR pp, pq, pmid;

    do {
      this->disableMinEpsilonRate();
      I = this->nextSample(I);
      updateApproximationScheme();

      //_p.synchroInstantiations( Ip,I);
      Iq.setValsFrom(map, I);

      pp   = p_.jointProbability(I);
      pq   = q_.jointProbability(Iq);
      pmid = (pp + pq) / 2.0;

      if (pp != (GUM_SCALAR)0.0) {
        hellinger_ += std::pow(std::sqrt(pp) - std::sqrt(pq), 2) / pp;

        if (pq != (GUM_SCALAR)0.0) {
          bhattacharya_ += std::sqrt(pq / pp);   // std::sqrt(pp*pq)/pp
          /// check_rate=true;
          this->enableMinEpsilonRate();   // replace check_rate=true;
          ratio = pq / pp;
          delta = (GUM_SCALAR)std::log2(ratio);
          klPQ_ += delta;

          // pmid!=0
          jsd_ -= std::log2(pp / pmid) + ratio * std::log2(pq / pmid);
        } else {
          errorPQ_++;
        }
      }

      if (pq != (GUM_SCALAR)0.0) {
        if (pp != (GUM_SCALAR)0.0) {
          // if we are here, it is certain that delta and ratio have been
          // computed
          // further lines above. (for now #112-113)
          klQP_ += (GUM_SCALAR)(-delta * ratio);
        } else {
          errorQP_++;
        }
      }

      if (this->isEnabledMinEpsilonRate()) {   // replace check_rate
        // delta is used as a temporary variable
        delta = klPQ_ / nbrIterations();
        error = (GUM_SCALAR)std::abs(delta - oldPQ);
        oldPQ = delta;
      }
    } while (continueApproximationScheme(error));   //

    klPQ_         = -klPQ_ / (nbrIterations());
    klQP_         = -klQP_ / (nbrIterations());
    jsd_          = -0.5 * jsd_ / (nbrIterations());
    hellinger_    = std::sqrt(hellinger_ / nbrIterations());
    bhattacharya_ = -std::log(bhattacharya_ / (nbrIterations()));
  }

  template < typename GUM_SCALAR >
  void GibbsBNdistance< GUM_SCALAR >::setBurnIn(Size b) {
    this->burn_in_ = b;
  }

  template < typename GUM_SCALAR >
  Size GibbsBNdistance< GUM_SCALAR >::burnIn() const {
    return this->burn_in_;
  }
}   // namespace gum
