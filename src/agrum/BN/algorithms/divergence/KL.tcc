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
 * @brief KL divergence between BNs implementation
 *
 * @author Pierre-Henri WUILLEMIN
 */

#include <math.h>
#include <agrum/BN/algorithms/divergence/KL.h>
#include <agrum/BN/IBayesNet.h>
#include <complex>

namespace gum {
  template <typename GUM_SCALAR>
  KL<GUM_SCALAR>::KL(const IBayesNet<GUM_SCALAR> &P, const IBayesNet<GUM_SCALAR> &Q)
      : _p(P), _q(Q), _klPQ(0.0), _klQP(0.0), _errorPQ(0), _errorQP(0),
        __difficulty(Complexity::Heavy), __done(false) {
    __checkCompatibility(); // may throw OperationNotAllowed
    GUM_CONSTRUCTOR(KL);

    double diff = _p.log10DomainSize();

    if (diff > GAP_COMPLEXITY_KL_HEAVY_DIFFICULT)
      __difficulty = Complexity::Heavy;
    else if (diff > GAP_COMPLEXITY_KL_DIFFICULT_CORRECT)
      __difficulty = Complexity::Difficult;
    else
      __difficulty = Complexity::Correct;
  }

  template <typename GUM_SCALAR>
  KL<GUM_SCALAR>::KL(const KL<GUM_SCALAR> &kl)
      : _p(kl._p), _q(kl._q), _klPQ(kl._klPQ), _klQP(kl._klQP),
        _errorPQ(kl._errorPQ), _errorQP(kl._errorQP), __difficulty(kl.__difficulty),
        __done(kl.__done) {
    GUM_CONSTRUCTOR(KL);
  }

  template <typename GUM_SCALAR> KL<GUM_SCALAR>::~KL() { GUM_DESTRUCTOR(KL); }

  template <typename GUM_SCALAR> Complexity KL<GUM_SCALAR>::difficulty() const {
    return __difficulty;
  }

  template <typename GUM_SCALAR> INLINE double KL<GUM_SCALAR>::klPQ() {
    _process();
    return _klPQ;
  }

  template <typename GUM_SCALAR> INLINE double KL<GUM_SCALAR>::klQP() {
    _process();
    return _klQP;
  }

  template <typename GUM_SCALAR> INLINE double KL<GUM_SCALAR>::hellinger() {
    _process();
    return _hellinger;
  }

  template <typename GUM_SCALAR> INLINE double KL<GUM_SCALAR>::bhattacharya() {
    _process();
    return _bhattacharya;
  }

  template <typename GUM_SCALAR> INLINE Size KL<GUM_SCALAR>::errorPQ() {
    _process();
    return _errorPQ;
  }

  template <typename GUM_SCALAR> INLINE Size KL<GUM_SCALAR>::errorQP() {
    _process();
    return _errorQP;
  }

  template <typename GUM_SCALAR>
  INLINE const IBayesNet<GUM_SCALAR> &KL<GUM_SCALAR>::p(void) const {
    return _p;
  }

  template <typename GUM_SCALAR>
  INLINE const IBayesNet<GUM_SCALAR> &KL<GUM_SCALAR>::q(void) const {
    return _q;
  }

  // check if the 2 BNs are compatible
  template <typename GUM_SCALAR> bool KL<GUM_SCALAR>::__checkCompatibility() const {
    for (auto node : _p.nodes()) {
      const DiscreteVariable &vp = _p.variable(node);

      try {
        const DiscreteVariable &vq = _q.variableFromName(vp.name());

        if (vp.domainSize() != vq.domainSize())
          GUM_ERROR(
              OperationNotAllowed,
              "KL : the 2 BNs are not compatible (not the same domainSize for " +
                  vp.name() + ")");

        for (Id i = 0; i < vp.domainSize(); i++) {
          try {
            vq[vp.label(i)];
            vp[vq.label(i)];

          } catch (OutOfBounds &e) {

            GUM_ERROR(
                OperationNotAllowed,
                "KL : the 2 BNs are not compatible F(not the same labels for " +
                    vp.name() + ")");
          }
        }
      } catch (NotFound &e) {

        GUM_ERROR(OperationNotAllowed,
                  "KL : the 2 BNs are not compatible (not the same vars : " +
                      vp.name() + ")");
      }
    }

    // should not be used
    if (_p.size() != _q.size())
      GUM_ERROR(OperationNotAllowed,
                "KL : the 2 BNs are not compatible (not the same size)");

    if (std::fabs(_p.log10DomainSize() - _q.log10DomainSize()) > 1e-14) {
      GUM_ERROR(OperationNotAllowed,
                "KL : the 2 BNs are not compatible (not the same domainSize) : p="
                    << _p.log10DomainSize() << " q=" << _q.log10DomainSize()
                    << " => " << _p.log10DomainSize() - _q.log10DomainSize());
    }

    return true;
  }

  // do the job if not already __done
  template <typename GUM_SCALAR> void KL<GUM_SCALAR>::_process() {
    if (!__done) {
      _computeKL();
      __done = true;
    }
  }

  // in order to keep KL instantiable
  template <typename GUM_SCALAR> void KL<GUM_SCALAR>::_computeKL() {
    GUM_ERROR(OperationNotAllowed, "No default computations");
  }
} // namespace gum
