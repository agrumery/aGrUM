/****************************************************************************
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
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
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
 * @brief KL divergence between BNs implementation
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <complex>

#include <agrum/BN/algorithms/divergence/BNdistance.h>
#include <agrum/BN/IBayesNet.h>

#include <agrum/base/core/math/math_utils.h>

namespace gum {
  template < typename GUM_SCALAR >
  BNdistance< GUM_SCALAR >::BNdistance(const IBayesNet< GUM_SCALAR >& P,
                                       const IBayesNet< GUM_SCALAR >& Q) :
      p_(P), q_(Q), klPQ_(0.0), klQP_(0.0), errorPQ_(0), errorQP_(0),
      _difficulty_(Complexity::Heavy), _done_(false) {
    _checkCompatibility_();   // may throw OperationNotAllowed
    GUM_CONSTRUCTOR(BNdistance);

    double diff = p_.log10DomainSize();

    if (diff > GAP_COMPLEXITY_KL_HEAVY_DIFFICULT) _difficulty_ = Complexity::Heavy;
    else if (diff > GAP_COMPLEXITY_KL_DIFFICULT_CORRECT) _difficulty_ = Complexity::Difficult;
    else _difficulty_ = Complexity::Correct;
  }

  template < typename GUM_SCALAR >
  BNdistance< GUM_SCALAR >::BNdistance(const BNdistance< GUM_SCALAR >& kl) :
      p_(kl.p_), q_(kl.q_), klPQ_(kl.klPQ_), klQP_(kl.klQP_), errorPQ_(kl.errorPQ_),
      errorQP_(kl.errorQP_), _difficulty_(kl._difficulty_), _done_(kl._done_) {
    GUM_CONSTRUCTOR(BNdistance);
  }

  template < typename GUM_SCALAR >
  BNdistance< GUM_SCALAR >::~BNdistance() {
    GUM_DESTRUCTOR(BNdistance);
  }

  template < typename GUM_SCALAR >
  Complexity BNdistance< GUM_SCALAR >::difficulty() const {
    return _difficulty_;
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
  bool BNdistance< GUM_SCALAR >::_checkCompatibility_() const {
    // should not be used
    if (p_.size() != q_.size())
      GUM_ERROR(OperationNotAllowed,
                "BNdistance : the 2 BNs are not compatible (not the same size)")

    for (auto node: p_.nodes()) {
      const DiscreteVariable& vp = p_.variable(node);
      try {
        const DiscreteVariable& vq = q_.variableFromName(vp.name());
        if (vp != vq)
          GUM_ERROR(OperationNotAllowed,
                    "BNdistance : the 2 BNs are not compatible "
                    "(not the same variable for the same name : "
                        + vp.toString() + "and " + vq.toString() + ")");
      } catch (NotFound const&) {
        GUM_ERROR(OperationNotAllowed,
                  "BNdistance : the 2 BNs are not compatible (variable : " + vp.name() + ")");
      }
    }

    if (std::fabs(p_.log10DomainSize() - q_.log10DomainSize()) > 1e-14) {
      GUM_ERROR(OperationNotAllowed,
                "BNdistance : the 2 BNs are not compatible (not the same domainSize) : p="
                    << p_.log10DomainSize() << " q=" << q_.log10DomainSize() << " => "
                    << p_.log10DomainSize() - q_.log10DomainSize());
    }

    return true;
  }

  // do the job if not already  _done_
  template < typename GUM_SCALAR >
  void BNdistance< GUM_SCALAR >::process_() {
    if (!_done_) {
      computeKL_();
      _done_ = true;
    }
  }

  // in order to keep BNdistance instantiable
  template < typename GUM_SCALAR >
  void BNdistance< GUM_SCALAR >::computeKL_() {
    GUM_ERROR(OperationNotAllowed, "No default computations")
  }
}   // namespace gum
