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
#include <agrum/BN/IBayesNet.h>
#include <agrum/BN/algorithms/divergence/KL.h>
#include <agrum/BN/algorithms/divergence/bruteForceKL.h>

namespace gum {
  template <typename GUM_SCALAR>
  BruteForceKL<GUM_SCALAR>::BruteForceKL(const IBayesNet<GUM_SCALAR> &P,
                                         const IBayesNet<GUM_SCALAR> &Q)
      : KL<GUM_SCALAR>(P, Q) {
    GUM_CONSTRUCTOR(BruteForceKL);
  }

  template <typename GUM_SCALAR>
  BruteForceKL<GUM_SCALAR>::BruteForceKL(const KL<GUM_SCALAR> &kl)
      : KL<GUM_SCALAR>(kl) {
    GUM_CONSTRUCTOR(BruteForceKL);
  }

  template <typename GUM_SCALAR> BruteForceKL<GUM_SCALAR>::~BruteForceKL() {
    GUM_DESTRUCTOR(BruteForceKL);
  }

  template <typename GUM_SCALAR> void BruteForceKL<GUM_SCALAR>::_computeKL() {
    _klPQ = _klQP = _hellinger = _bhattacharya = (GUM_SCALAR)0.0;
    _errorPQ = _errorQP = 0;

    gum::Instantiation Ip;
    _p.completeInstantiation(Ip);
    gum::Instantiation Iq;
    _q.completeInstantiation(Iq);

    // map between _p variables and _q variables (using name of vars)
    HashTable<const DiscreteVariable *, const DiscreteVariable *> map;

    for (Idx ite = 0; ite < Ip.nbrDim(); ++ite) {
      map.insert(&Ip.variable(ite), &_q.variableFromName(Ip.variable(ite).name()));
    }

    for (Ip.setFirst(); !Ip.end(); ++Ip) {
      Iq.setValsFrom(map, Ip);
      GUM_SCALAR pp = _p.jointProbability(Ip);
      GUM_SCALAR pq = _q.jointProbability(Iq);

      _hellinger += std::pow(std::sqrt(pp) - std::sqrt(pq), 2);
      _bhattacharya += std::sqrt(pp * pq);

      if (pp != (GUM_SCALAR)0.0) {
        if (pq != (GUM_SCALAR)0.0) {
          _klPQ -= pp * log2(pq / pp);
        } else {
          _errorPQ++;
        }
      }

      if (pq != (GUM_SCALAR)0.0) {
        if (pp != (GUM_SCALAR)0.0) {
          _klQP -= pq * log2(pp / pq);
        } else {
          _errorQP++;
        }
      }
    }

    _hellinger = std::sqrt(_hellinger);
    _bhattacharya = -std::log(_bhattacharya);
  }

} // namespace gum
