
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
 * @brief KL divergence between BNs implementation
 *
 * @author Pierre-Henri WUILLEMIN
 */

#include <complex>

#include <agrum/core/math/math.h>
#include <agrum/BN/IBayesNet.h>
#include <agrum/BN/algorithms/divergence/BNdistance.h>

namespace gum {
  template < typename GUM_SCALAR >
  BNdistance< GUM_SCALAR >::BNdistance(const IBayesNet< GUM_SCALAR >& P,
                                       const IBayesNet< GUM_SCALAR >& Q) :
      _p(P),
      _q(Q), _klPQ(0.0), _klQP(0.0), _errorPQ(0), _errorQP(0),
      __difficulty(Complexity::Heavy), __done(false) {
    __checkCompatibility();   // may throw OperationNotAllowed
    GUM_CONSTRUCTOR(BNdistance);

    double diff = _p.log10DomainSize();

    if (diff > GAP_COMPLEXITY_KL_HEAVY_DIFFICULT)
      __difficulty = Complexity::Heavy;
    else if (diff > GAP_COMPLEXITY_KL_DIFFICULT_CORRECT)
      __difficulty = Complexity::Difficult;
    else
      __difficulty = Complexity::Correct;
  }

  template < typename GUM_SCALAR >
  BNdistance< GUM_SCALAR >::BNdistance(const BNdistance< GUM_SCALAR >& kl) :
      _p(kl._p), _q(kl._q), _klPQ(kl._klPQ), _klQP(kl._klQP),
      _errorPQ(kl._errorPQ), _errorQP(kl._errorQP), __difficulty(kl.__difficulty),
      __done(kl.__done) {
    GUM_CONSTRUCTOR(BNdistance);
  }

  template < typename GUM_SCALAR >
  BNdistance< GUM_SCALAR >::~BNdistance() {
    GUM_DESTRUCTOR(BNdistance);
  }

  template < typename GUM_SCALAR >
  Complexity BNdistance< GUM_SCALAR >::difficulty() const {
    return __difficulty;
  }

  template < typename GUM_SCALAR >
  INLINE double BNdistance< GUM_SCALAR >::klPQ() {
    _process();
    return _klPQ;
  }

  template < typename GUM_SCALAR >
  INLINE double BNdistance< GUM_SCALAR >::klQP() {
    _process();
    return _klQP;
  }

  template < typename GUM_SCALAR >
  INLINE double BNdistance< GUM_SCALAR >::hellinger() {
    _process();
    return _hellinger;
  }

  template < typename GUM_SCALAR >
  INLINE double BNdistance< GUM_SCALAR >::bhattacharya() {
    _process();
    return _bhattacharya;
  }

  template < typename GUM_SCALAR >
  INLINE double BNdistance< GUM_SCALAR >::jsd() {
    _process();
    return _jsd;
  }

  template < typename GUM_SCALAR >
  INLINE Size BNdistance< GUM_SCALAR >::errorPQ() {
    _process();
    return _errorPQ;
  }

  template < typename GUM_SCALAR >
  INLINE Size BNdistance< GUM_SCALAR >::errorQP() {
    _process();
    return _errorQP;
  }

  template < typename GUM_SCALAR >
  INLINE const IBayesNet< GUM_SCALAR >& BNdistance< GUM_SCALAR >::p() const {
    return _p;
  }

  template < typename GUM_SCALAR >
  INLINE const IBayesNet< GUM_SCALAR >& BNdistance< GUM_SCALAR >::q() const {
    return _q;
  }

  // check if the 2 BNs are compatible
  template < typename GUM_SCALAR >
  bool BNdistance< GUM_SCALAR >::__checkCompatibility() const {
    for (auto node : _p.nodes()) {
      const DiscreteVariable& vp = _p.variable(node);

      try {
        const DiscreteVariable& vq = _q.variableFromName(vp.name());

        if (vp.domainSize() != vq.domainSize())
          GUM_ERROR(OperationNotAllowed,
                    "BNdistance : the 2 BNs are not compatible "
                    "(not the same domainSize for "
                       + vp.name() + ")");

        for (Idx i = 0; i < vp.domainSize(); i++) {
          try {
            vq[vp.label(i)];
            vp[vq.label(i)];

          } catch (OutOfBounds&) {
            GUM_ERROR(OperationNotAllowed,
                      "BNdistance : the 2 BNs are not compatible F(not the same "
                      "labels for "
                         + vp.name() + ")");
          }
        }
      } catch (NotFound&) {
        GUM_ERROR(OperationNotAllowed,
                  "BNdistance : the 2 BNs are not compatible (not the same vars : "
                     + vp.name() + ")");
      }
    }

    // should not be used
    if (_p.size() != _q.size())
      GUM_ERROR(OperationNotAllowed,
                "BNdistance : the 2 BNs are not compatible (not the same size)");

    if (std::fabs(_p.log10DomainSize() - _q.log10DomainSize()) > 1e-14) {
      GUM_ERROR(
         OperationNotAllowed,
         "BNdistance : the 2 BNs are not compatible (not the same domainSize) : p="
            << _p.log10DomainSize() << " q=" << _q.log10DomainSize() << " => "
            << _p.log10DomainSize() - _q.log10DomainSize());
    }

    return true;
  }

  // do the job if not already __done
  template < typename GUM_SCALAR >
  void BNdistance< GUM_SCALAR >::_process() {
    if (!__done) {
      _computeKL();
      __done = true;
    }
  }

  // in order to keep BNdistance instantiable
  template < typename GUM_SCALAR >
  void BNdistance< GUM_SCALAR >::_computeKL() {
    GUM_ERROR(OperationNotAllowed, "No default computations");
  }
}   // namespace gum
