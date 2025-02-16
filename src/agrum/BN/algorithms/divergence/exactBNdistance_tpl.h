/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
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
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief KL divergence between BNs brute force implementation
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/BN/algorithms/divergence/BNdistance.h>
#include <agrum/BN/algorithms/divergence/exactBNdistance.h>
#include <agrum/BN/IBayesNet.h>

#include <agrum/base/core/math/math_utils.h>

namespace gum {
  template < typename GUM_SCALAR >
  ExactBNdistance< GUM_SCALAR >::ExactBNdistance(const IBayesNet< GUM_SCALAR >& P,
                                                 const IBayesNet< GUM_SCALAR >& Q) :
      BNdistance< GUM_SCALAR >(P, Q) {
    GUM_CONSTRUCTOR(ExactBNdistance);
  }

  template < typename GUM_SCALAR >
  ExactBNdistance< GUM_SCALAR >::ExactBNdistance(const BNdistance< GUM_SCALAR >& kl) :
      BNdistance< GUM_SCALAR >(kl) {
    GUM_CONSTRUCTOR(ExactBNdistance);
  }

  template < typename GUM_SCALAR >
  ExactBNdistance< GUM_SCALAR >::~ExactBNdistance() {
    GUM_DESTRUCTOR(ExactBNdistance);
  }

  template < typename GUM_SCALAR >
  void ExactBNdistance< GUM_SCALAR >::computeKL_() {
    klPQ_ = klQP_ = hellinger_ = bhattacharya_ = jsd_ = (GUM_SCALAR)0.0;
    errorPQ_ = errorQP_ = 0;

    auto Ip = p_.completeInstantiation();
    auto Iq = q_.completeInstantiation();

    // map between p_ variables and q_ variables (using name of vars)
    HashTable< const DiscreteVariable*, const DiscreteVariable* > map;

    for (Idx ite = 0; ite < Ip.nbrDim(); ++ite) {
      map.insert(&Ip.variable(ite), &q_.variableFromName(Ip.variable(ite).name()));
    }
    GUM_SCALAR pp, pq, pmid, lpp, lpq, lpmid;
    for (Ip.setFirst(); !Ip.end(); ++Ip) {
      Iq.setValsFrom(map, Ip);
      pp    = p_.jointProbability(Ip);
      pq    = q_.jointProbability(Iq);
      pmid  = (pp + pq) / 2.0;
      lpmid = GUM_LOG2_OR_0(pmid);
      lpp   = GUM_LOG2_OR_0(pp);
      lpq   = GUM_LOG2_OR_0(pq);


      hellinger_ += std::pow(std::sqrt(pp) - std::sqrt(pq), 2);
      bhattacharya_ += std::sqrt(pp * pq);

      if (pp != (GUM_SCALAR)0.0) {
        if (pq != (GUM_SCALAR)0.0) {
          klPQ_ -= pp * (lpq - lpp);   // log2(pq / pp);
        } else {
          errorPQ_++;
        }
      }

      if (pq != (GUM_SCALAR)0.0) {
        if (pp != (GUM_SCALAR)0.0) {
          klQP_ -= pq * (lpp - lpq);   // log2(pp / pq);
        } else {
          errorQP_++;
        }
      }
      if (pmid != (GUM_SCALAR)0.0) {
        jsd_ += pp * lpp + pq * lpq
              - (pp + pq) * lpmid;   // pp* log2(pp / pmid) + pq * log2(pq / pmid);
      }
    }
    jsd_ /= 2.0;
    hellinger_    = std::sqrt(hellinger_);
    bhattacharya_ = -std::log(bhattacharya_);
  }

}   // namespace gum
