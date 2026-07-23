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
 * @brief KL divergence between BNs -- implementation using independent Monte
 * Carlo sampling
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/base/core/approximations/approximationScheme.h>
#include <agrum/base/core/hashTable.h>
#include <agrum/BN/algorithms/divergence/MCBNDistance.h>
#include <agrum/BN/IBayesNet.h>

#include <agrum/base/core/math/math_utils.h>

#define MCBNDISTANCE_DEFAULT_MAXITER          10000000
#define MCBNDISTANCE_DEFAULT_EPSILON          1e-10
#define MCBNDISTANCE_DEFAULT_MIN_EPSILON_RATE 1e-10
#define MCBNDISTANCE_DEFAULT_PERIOD_SIZE      200
#define MCBNDISTANCE_DEFAULT_VERBOSITY        false
#define MCBNDISTANCE_DEFAULT_TIMEOUT          6000

namespace gum {

  template < GUM_Numeric GUM_SCALAR >
  MCBNDistance< GUM_SCALAR >::MCBNDistance(const IBayesNet< GUM_SCALAR >& P,
                                           const IBayesNet< GUM_SCALAR >& Q) :
      BNdistance< GUM_SCALAR >(P, Q), ApproximationScheme() {
    GUM_CONSTRUCTOR(MCBNDistance);

    setEpsilon(MCBNDISTANCE_DEFAULT_EPSILON);
    setMinEpsilonRate(MCBNDISTANCE_DEFAULT_MIN_EPSILON_RATE);
    setMaxIter(MCBNDISTANCE_DEFAULT_MAXITER);
    setVerbosity(MCBNDISTANCE_DEFAULT_VERBOSITY);
    setPeriodSize(MCBNDISTANCE_DEFAULT_PERIOD_SIZE);
    setMaxTime(MCBNDISTANCE_DEFAULT_TIMEOUT);
  }

  template < GUM_Numeric GUM_SCALAR >
  MCBNDistance< GUM_SCALAR >::MCBNDistance(const BNdistance< GUM_SCALAR >& kl) :
      BNdistance< GUM_SCALAR >(kl), ApproximationScheme() {
    GUM_CONSTRUCTOR(MCBNDistance);

    setEpsilon(MCBNDISTANCE_DEFAULT_EPSILON);
    setMinEpsilonRate(MCBNDISTANCE_DEFAULT_MIN_EPSILON_RATE);
    setMaxIter(MCBNDISTANCE_DEFAULT_MAXITER);
    setVerbosity(MCBNDISTANCE_DEFAULT_VERBOSITY);
    setPeriodSize(MCBNDISTANCE_DEFAULT_PERIOD_SIZE);
    setMaxTime(MCBNDISTANCE_DEFAULT_TIMEOUT);
  }

  template < GUM_Numeric GUM_SCALAR >
  MCBNDistance< GUM_SCALAR >::~MCBNDistance() {
    GUM_DESTRUCTOR(MCBNDistance);
  }

  template < GUM_Numeric GUM_SCALAR >
  Instantiation MCBNDistance< GUM_SCALAR >::drawSample_() const {
    gum::Instantiation I;

    for (const auto nod: p_.topologicalOrder()) {
      I.add(p_.variable(nod));
      gum::Instantiation Itop(I);
      Itop.erase(p_.variable(nod));
      I.chgVal(p_.variable(nod), p_.cpt(nod).extract(Itop).draw());
    }
    return I;
  }

  template < GUM_Numeric GUM_SCALAR >
  void MCBNDistance< GUM_SCALAR >::computeKL_() {
    auto Iq = q_.completeInstantiation();
    initApproximationScheme();

    // map between drawSample_() variables and q_ variables (using name of vars)
    HashTable< const DiscreteVariable*, const DiscreteVariable* > map;
    {
      auto Ip = p_.completeInstantiation();
      for (Idx ite = 0; ite < Ip.nbrDim(); ++ite) {
        map.insert(&Ip.variable(ite), &q_.variableFromName(Ip.variable(ite).name()));
      }
    }

    klPQ_ = klQP_ = hellinger_ = jsd_ = (GUM_SCALAR)0.0;
    errorPQ_ = errorQP_ = 0;
    GUM_SCALAR delta, ratio, error;
    delta = ratio = error = (GUM_SCALAR)-1;
    GUM_SCALAR oldPQ      = 0.0;
    GUM_SCALAR pp, pq, pmid;

    do {
      this->disableMinEpsilonRate();
      gum::Instantiation I = drawSample_();
      updateApproximationScheme();

      Iq.setValsFrom(map, I);

      pp   = p_.jointProbability(I);
      pq   = q_.jointProbability(Iq);
      pmid = (pp + pq) / 2.0;

      if (pp != (GUM_SCALAR)0.0) {
        hellinger_ += std::pow(std::sqrt(pp) - std::sqrt(pq), 2) / pp;

        if (pq != (GUM_SCALAR)0.0) {
          bhattacharya_ += std::sqrt(pq / pp);
          this->enableMinEpsilonRate();
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
          // computed further lines above.
          klQP_ += (GUM_SCALAR)(-delta * ratio);
        } else {
          errorQP_++;
        }
      }

      if (this->isEnabledMinEpsilonRate()) {
        // delta is used as a temporary variable
        delta = klPQ_ / nbrIterations();
        error = (GUM_SCALAR)std::abs(delta - oldPQ);
        oldPQ = delta;
      }
    } while (continueApproximationScheme(error));

    klPQ_         = -klPQ_ / (nbrIterations());
    klQP_         = -klQP_ / (nbrIterations());
    jsd_          = -0.5 * jsd_ / (nbrIterations());
    hellinger_    = std::sqrt(hellinger_ / nbrIterations());
    bhattacharya_ = -std::log(bhattacharya_ / (nbrIterations()));
  }
}   // namespace gum
