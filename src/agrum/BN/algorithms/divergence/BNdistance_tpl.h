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
 * @brief KL divergence between BNs implementation
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6)
 */

#include <complex>

#include <agrum/tools/core/math/math_utils.h>
#include <agrum/BN/IBayesNet.h>
#include <agrum/BN/algorithms/divergence/BNdistance.h>

namespace gum {
  template < typename GUM_SCALAR >
  BNdistance< GUM_SCALAR >::BNdistance(const IBayesNet< GUM_SCALAR >& P,
                                       const IBayesNet< GUM_SCALAR >& Q) :
      p_(P),
      q_(Q), klPQ_(0.0), klQP_(0.0), errorPQ_(0), errorQP_(0),
      difficulty__(Complexity::Heavy), done__(false) {
    checkCompatibility__();   // may throw OperationNotAllowed
    GUM_CONSTRUCTOR(BNdistance);

    double diff = p_.log10DomainSize();

    if (diff > GAP_COMPLEXITY_KL_HEAVY_DIFFICULT)
      difficulty__ = Complexity::Heavy;
    else if (diff > GAP_COMPLEXITY_KL_DIFFICULT_CORRECT)
      difficulty__ = Complexity::Difficult;
    else
      difficulty__ = Complexity::Correct;
  }

  template < typename GUM_SCALAR >
  BNdistance< GUM_SCALAR >::BNdistance(const BNdistance< GUM_SCALAR >& kl) :
      p_(kl.p_), q_(kl.q_), klPQ_(kl.klPQ_), klQP_(kl.klQP_),
      errorPQ_(kl.errorPQ_), errorQP_(kl.errorQP_), difficulty__(kl.difficulty__),
      done__(kl.done__) {
    GUM_CONSTRUCTOR(BNdistance);
  }

  template < typename GUM_SCALAR >
  BNdistance< GUM_SCALAR >::~BNdistance() {
    GUM_DESTRUCTOR(BNdistance);
  }

  template < typename GUM_SCALAR >
  Complexity BNdistance< GUM_SCALAR >::difficulty() const {
    return difficulty__;
  }

  template < typename GUM_SCALAR >
  INLINE double BNdistance< GUM_SCALAR >::klPQ() {
    process_();
    return klPQ_;
  }

  template < typename GUM_SCALAR >
  INLINE double BNdistance< GUM_SCALAR >::klQP() {
    process_();
    return klQP_;
  }

  template < typename GUM_SCALAR >
  INLINE double BNdistance< GUM_SCALAR >::hellinger() {
    process_();
    return hellinger_;
  }

  template < typename GUM_SCALAR >
  INLINE double BNdistance< GUM_SCALAR >::bhattacharya() {
    process_();
    return bhattacharya_;
  }

  template < typename GUM_SCALAR >
  INLINE double BNdistance< GUM_SCALAR >::jsd() {
    process_();
    return jsd_;
  }

  template < typename GUM_SCALAR >
  INLINE Size BNdistance< GUM_SCALAR >::errorPQ() {
    process_();
    return errorPQ_;
  }

  template < typename GUM_SCALAR >
  INLINE Size BNdistance< GUM_SCALAR >::errorQP() {
    process_();
    return errorQP_;
  }

  template < typename GUM_SCALAR >
  INLINE const IBayesNet< GUM_SCALAR >& BNdistance< GUM_SCALAR >::p() const {
    return p_;
  }

  template < typename GUM_SCALAR >
  INLINE const IBayesNet< GUM_SCALAR >& BNdistance< GUM_SCALAR >::q() const {
    return q_;
  }

  // check if the 2 BNs are compatible
  template < typename GUM_SCALAR >
  bool BNdistance< GUM_SCALAR >::checkCompatibility__() const {
    for (auto node: p_.nodes()) {
      const DiscreteVariable& vp = p_.variable(node);

      try {
        const DiscreteVariable& vq = q_.variableFromName(vp.name());

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
    if (p_.size() != q_.size())
      GUM_ERROR(OperationNotAllowed,
                "BNdistance : the 2 BNs are not compatible (not the same size)");

    if (std::fabs(p_.log10DomainSize() - q_.log10DomainSize()) > 1e-14) {
      GUM_ERROR(
         OperationNotAllowed,
         "BNdistance : the 2 BNs are not compatible (not the same domainSize) : p="
            << p_.log10DomainSize() << " q=" << q_.log10DomainSize() << " => "
            << p_.log10DomainSize() - q_.log10DomainSize());
    }

    return true;
  }

  // do the job if not already done__
  template < typename GUM_SCALAR >
  void BNdistance< GUM_SCALAR >::process_() {
    if (!done__) {
      computeKL_();
      done__ = true;
    }
  }

  // in order to keep BNdistance instantiable
  template < typename GUM_SCALAR >
  void BNdistance< GUM_SCALAR >::computeKL_() {
    GUM_ERROR(OperationNotAllowed, "No default computations");
  }
}   // namespace gum
