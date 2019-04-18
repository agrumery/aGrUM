
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @brief KL divergence between BNs brute force implementation
 *
 * @author Pierre-Henri WUILLEMIN
 */

#include <agrum/core/math/math.h>
#include <agrum/BN/IBayesNet.h>
#include <agrum/BN/algorithms/divergence/BNdistance.h>
#include <agrum/BN/algorithms/divergence/exactBNdistance.h>

namespace gum {
  template < typename GUM_SCALAR >
  ExactBNdistance< GUM_SCALAR >::ExactBNdistance(
     const IBayesNet< GUM_SCALAR >& P, const IBayesNet< GUM_SCALAR >& Q) :
      BNdistance< GUM_SCALAR >(P, Q) {
    GUM_CONSTRUCTOR(ExactBNdistance);
  }

  template < typename GUM_SCALAR >
  ExactBNdistance< GUM_SCALAR >::ExactBNdistance(
     const BNdistance< GUM_SCALAR >& kl) :
      BNdistance< GUM_SCALAR >(kl) {
    GUM_CONSTRUCTOR(ExactBNdistance);
  }

  template < typename GUM_SCALAR >
  ExactBNdistance< GUM_SCALAR >::~ExactBNdistance() {
    GUM_DESTRUCTOR(ExactBNdistance);
  }

  template < typename GUM_SCALAR >
  void ExactBNdistance< GUM_SCALAR >::_computeKL() {
    _klPQ = _klQP = _hellinger = _bhattacharya = _jsd = (GUM_SCALAR)0.0;
    _errorPQ = _errorQP = 0;

    auto Ip = _p.completeInstantiation();
    auto Iq = _q.completeInstantiation();

    // map between _p variables and _q variables (using name of vars)
    HashTable< const DiscreteVariable*, const DiscreteVariable* > map;

    for (Idx ite = 0; ite < Ip.nbrDim(); ++ite) {
      map.insert(&Ip.variable(ite), &_q.variableFromName(Ip.variable(ite).name()));
    }
    GUM_SCALAR pp, pq, pmid, lpp, lpq, lpmid;
    for (Ip.setFirst(); !Ip.end(); ++Ip) {
      Iq.setValsFrom(map, Ip);
      pp = _p.jointProbability(Ip);
      pq = _q.jointProbability(Iq);
      pmid = (pp + pq) / 2.0;
      lpmid = lpq = lpp = (GUM_SCALAR)0.0;
      if (pmid != (GUM_SCALAR)0.0) lpmid = log2(pmid);
      if (pp != (GUM_SCALAR)0.0) lpp = log2(pp);
      if (pq != (GUM_SCALAR)0.0) lpq = log2(pq);


      _hellinger += std::pow(std::sqrt(pp) - std::sqrt(pq), 2);
      _bhattacharya += std::sqrt(pp * pq);

      if (pp != (GUM_SCALAR)0.0) {
        if (pq != (GUM_SCALAR)0.0) {
          _klPQ -= pp * (lpq - lpp);   // log2(pq / pp);
        } else {
          _errorPQ++;
        }
      }

      if (pq != (GUM_SCALAR)0.0) {
        if (pp != (GUM_SCALAR)0.0) {
          _klQP -= pq * (lpp - lpq);   // log2(pp / pq);
        } else {
          _errorQP++;
        }
      }
      if (pmid != (GUM_SCALAR)0.0) {
        _jsd +=
           pp * lpp + pq * lpq
           - (pp + pq) * lpmid;   // pp* log2(pp / pmid) + pq * log2(pq / pmid);
      }
    }
    _jsd /= 2.0;
    _hellinger = std::sqrt(_hellinger);
    _bhattacharya = -std::log(_bhattacharya);
  }

}   // namespace gum
