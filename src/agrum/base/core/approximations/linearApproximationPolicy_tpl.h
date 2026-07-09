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
 * @brief Inlined implementation of gum::LienarApproxiationPolicy.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Jean-Christophe Magnan
 *
 */

// Help IDE parsers
#include <agrum/base/core/approximations/linearApproximationPolicy.h>

namespace gum {

  // Class constructor
  template < typename GUM_ELEMENT >
  LinearApproximationPolicy< GUM_ELEMENT >::LinearApproximationPolicy(GUM_ELEMENT low,
                                                                      GUM_ELEMENT high,
                                                                      GUM_ELEMENT eps) :
      ApproximationPolicy< GUM_ELEMENT >(), lowLimit_(low), highLimit_(high), epsilon_(eps) {
    if (eps <= 0) { GUM_ERROR(OutOfBounds, "Epsilon must be >0") }

    computeNbInterval_();
  }

  // Copy constructor.
  template < typename GUM_ELEMENT >
  LinearApproximationPolicy< GUM_ELEMENT >::LinearApproximationPolicy(
      const LinearApproximationPolicy< GUM_ELEMENT >* md) :
      ApproximationPolicy< GUM_ELEMENT >(md), epsilon_(md->epsilon_), nbInterval_(md->nbInterval_) {
  }

  // @brief Convert value to his approximation.
  template < typename GUM_ELEMENT >
  GUM_ELEMENT
      LinearApproximationPolicy< GUM_ELEMENT >::fromExact(const GUM_ELEMENT& value) const {
    return _decode_(GUM_ELEMENT(encode(value)));
  }

  // @brief Combine using addition with the given gum::ApproximationPolicy.
  template < typename GUM_ELEMENT >
  void LinearApproximationPolicy< GUM_ELEMENT >::combineAdd(
      const ApproximationPolicy< GUM_ELEMENT >* ap) {
    try {
      const LinearApproximationPolicy< GUM_ELEMENT >* lap
          = dynamic_cast< const LinearApproximationPolicy< GUM_ELEMENT >* >(ap);

      GUM_ELEMENT newHighLimit = lowLimit_ + lap->lowLimit();
      GUM_ELEMENT newLowLimit  = lowLimit_ + lap->lowLimit();

      GUM_ELEMENT newVal = lowLimit_ + lap->highLimit();

      if (newHighLimit < newVal) newHighLimit = newVal;

      if (newLowLimit > newVal) newLowLimit = newVal;

      newVal = highLimit_ + lap->lowLimit();

      if (newHighLimit < newVal) newHighLimit = newVal;

      if (newLowLimit > newVal) newLowLimit = newVal;

      newVal = highLimit_ + lap->highLimit();

      if (newHighLimit < newVal) newHighLimit = newVal;

      if (newLowLimit > newVal) newLowLimit = newVal;

      this->lowLimit_  = newLowLimit;
      this->highLimit_ = newHighLimit;
      computeNbInterval_();
    } catch (const std::bad_cast&) {}
  }

  template < typename GUM_ELEMENT >
  void LinearApproximationPolicy< GUM_ELEMENT >::combineSub(
      const ApproximationPolicy< GUM_ELEMENT >* ap) {
    try {
      const LinearApproximationPolicy< GUM_ELEMENT >* lap
          = dynamic_cast< const LinearApproximationPolicy< GUM_ELEMENT >* >(ap);

      GUM_ELEMENT newHighLimit = lowLimit_ - lap->lowLimit();
      GUM_ELEMENT newLowLimit  = lowLimit_ - lap->lowLimit();

      GUM_ELEMENT newVal = lowLimit_ - lap->highLimit();

      if (newHighLimit < newVal) newHighLimit = newVal;

      if (newLowLimit > newVal) newLowLimit = newVal;

      newVal = highLimit_ - lap->lowLimit();

      if (newHighLimit < newVal) newHighLimit = newVal;

      if (newLowLimit > newVal) newLowLimit = newVal;

      newVal = highLimit_ - lap->highLimit();

      if (newHighLimit < newVal) newHighLimit = newVal;

      if (newLowLimit > newVal) newLowLimit = newVal;

      this->lowLimit_  = newLowLimit;
      this->highLimit_ = newHighLimit;
      computeNbInterval_();
    } catch (const std::bad_cast&) {}
  }

  template < typename GUM_ELEMENT >
  void LinearApproximationPolicy< GUM_ELEMENT >::combineMult(
      const ApproximationPolicy< GUM_ELEMENT >* ap) {
    try {
      const LinearApproximationPolicy< GUM_ELEMENT >* lap
          = dynamic_cast< const LinearApproximationPolicy< GUM_ELEMENT >* >(ap);

      GUM_ELEMENT newHighLimit = lowLimit_ * lap->lowLimit();
      GUM_ELEMENT newLowLimit  = lowLimit_ * lap->lowLimit();

      GUM_ELEMENT newVal = lowLimit_ * lap->highLimit();

      if (newHighLimit < newVal) newHighLimit = newVal;

      if (newLowLimit > newVal) newLowLimit = newVal;

      newVal = highLimit_ * lap->lowLimit();

      if (newHighLimit < newVal) newHighLimit = newVal;

      if (newLowLimit > newVal) newLowLimit = newVal;

      newVal = highLimit_ * lap->highLimit();

      if (newHighLimit < newVal) newHighLimit = newVal;

      if (newLowLimit > newVal) newLowLimit = newVal;

      this->lowLimit_  = newLowLimit;
      this->highLimit_ = newHighLimit;
      computeNbInterval_();
    } catch (const std::bad_cast&) {}
  }

  template < typename GUM_ELEMENT >
  void LinearApproximationPolicy< GUM_ELEMENT >::combineDiv(
      const ApproximationPolicy< GUM_ELEMENT >* ap) {
    try {
      const LinearApproximationPolicy< GUM_ELEMENT >* lap
          = dynamic_cast< const LinearApproximationPolicy< GUM_ELEMENT >* >(ap);

      GUM_ELEMENT newHighLimit = lowLimit_ / lap->lowLimit();
      GUM_ELEMENT newLowLimit  = lowLimit_ / lap->lowLimit();

      GUM_ELEMENT newVal = lowLimit_ / lap->highLimit();

      if (newHighLimit < newVal) newHighLimit = newVal;

      if (newLowLimit > newVal) newLowLimit = newVal;

      newVal = highLimit_ / lap->lowLimit();

      if (newHighLimit < newVal) newHighLimit = newVal;

      if (newLowLimit > newVal) newLowLimit = newVal;

      newVal = highLimit_ / lap->highLimit();

      if (newHighLimit < newVal) newHighLimit = newVal;

      if (newLowLimit > newVal) newLowLimit = newVal;

      this->lowLimit_  = newLowLimit;
      this->highLimit_ = newHighLimit;
      computeNbInterval_();
    } catch (const std::bad_cast&) {}
  }

  template < typename GUM_ELEMENT >
  void LinearApproximationPolicy< GUM_ELEMENT >::combineMax(
      const ApproximationPolicy< GUM_ELEMENT >* ap) {
    try {
      const LinearApproximationPolicy< GUM_ELEMENT >* lap
          = dynamic_cast< const LinearApproximationPolicy< GUM_ELEMENT >* >(ap);

      GUM_ELEMENT newHighLimit = lowLimit_ > lap->lowLimit() ? lowLimit_ : lap->lowLimit();
      GUM_ELEMENT newLowLimit  = lowLimit_ > lap->lowLimit() ? lowLimit_ : lap->lowLimit();

      GUM_ELEMENT newVal = lowLimit_ > lap->highLimit() ? lowLimit_ : lap->highLimit();

      if (newHighLimit < newVal) newHighLimit = newVal;

      if (newLowLimit > newVal) newLowLimit = newVal;

      newVal = highLimit_ > lap->lowLimit() ? highLimit_ : lap->lowLimit();

      if (newHighLimit < newVal) newHighLimit = newVal;

      if (newLowLimit > newVal) newLowLimit = newVal;

      newVal = highLimit_ > lap->highLimit() ? highLimit_ : lap->highLimit();

      if (newHighLimit < newVal) newHighLimit = newVal;

      if (newLowLimit > newVal) newLowLimit = newVal;

      this->lowLimit_  = newLowLimit;
      this->highLimit_ = newHighLimit;
      computeNbInterval_();
    } catch (const std::bad_cast&) {}
  }

  template < typename GUM_ELEMENT >
  void LinearApproximationPolicy< GUM_ELEMENT >::combineMin(
      const ApproximationPolicy< GUM_ELEMENT >* ap) {
    try {
      const LinearApproximationPolicy< GUM_ELEMENT >* lap
          = dynamic_cast< const LinearApproximationPolicy< GUM_ELEMENT >* >(ap);

      GUM_ELEMENT newHighLimit = lowLimit_ < lap->lowLimit() ? lowLimit_ : lap->lowLimit();
      GUM_ELEMENT newLowLimit  = lowLimit_ < lap->lowLimit() ? lowLimit_ : lap->lowLimit();

      GUM_ELEMENT newVal = lowLimit_ < lap->highLimit() ? lowLimit_ : lap->highLimit();

      if (newHighLimit < newVal) newHighLimit = newVal;

      if (newLowLimit > newVal) newLowLimit = newVal;

      newVal = highLimit_ < lap->lowLimit() ? highLimit_ : lap->lowLimit();

      if (newHighLimit < newVal) newHighLimit = newVal;

      if (newLowLimit > newVal) newLowLimit = newVal;

      newVal = highLimit_ < lap->highLimit() ? highLimit_ : lap->highLimit();

      if (newHighLimit < newVal) newHighLimit = newVal;

      if (newLowLimit > newVal) newLowLimit = newVal;

      this->lowLimit_  = newLowLimit;
      this->highLimit_ = newHighLimit;
      computeNbInterval_();
    } catch (const std::bad_cast&) {}
  }

  // Convert value to his approximation. This method is slower than @ref
  // fromExact since it verifies the bounds
  template < typename GUM_ELEMENT >
  GUM_ELEMENT
      LinearApproximationPolicy< GUM_ELEMENT >::safeFromExact(const GUM_ELEMENT& value) {
    if (value > this->highLimit_) {
      GUM_ERROR(OutOfBounds, "Value asked is higher than high limit")
    }

    if (value < this->lowLimit_) { GUM_ERROR(OutOfBounds, "Value asked is lower than low limit") }

    return fromExact(value);
  }

  // Convert value to approximation representation
  template < typename GUM_ELEMENT >
  Idx LinearApproximationPolicy< GUM_ELEMENT >::encode(const GUM_ELEMENT& value) const {
// we keep the bounds checked in debug mode
#ifdef GUM_DEBUG_MODE
    if (value > this->highLimit_) {
      GUM_ERROR(OutOfBounds,
                "Value asked is higher than High limit :  not in (" << this->lowLimit_ << "-"
                                                                    << this->highLimit_ << ")")
    }

    if (value < this->lowLimit_) {
      GUM_ERROR(OutOfBounds,
                "Value asked is lower than low limit :  not in (" << this->lowLimit_ << "-"
                                                                  << this->highLimit_ << ")")
    }

#endif   // GUM_DEBUG_MODE
    return _encode_(value);
  }

  // Convert approximation representation to value
  template < typename GUM_ELEMENT >
  GUM_ELEMENT LinearApproximationPolicy< GUM_ELEMENT >::decode(Idx representation) const {
    if (representation > nbInterval_) {
      GUM_ERROR(OutOfBounds, "Interval Number asked is higher than total number of interval")
    }

    return _decode_(GUM_ELEMENT(representation));
  }

  // Sets approximation factor
  template < typename GUM_ELEMENT >
  void LinearApproximationPolicy< GUM_ELEMENT >::setEpsilon(const GUM_ELEMENT& e) {
    if (e <= 0) { GUM_ERROR(OutOfBounds, "Epsilon must be >0") }
    epsilon_ = e;
    computeNbInterval_();
  }

  // set bounds in a whole
  template < typename GUM_ELEMENT >
  void LinearApproximationPolicy< GUM_ELEMENT >::setLimits(const GUM_ELEMENT& newLowLimit,
                                                                  const GUM_ELEMENT& newHighLimit) {
    if (newLowLimit > newHighLimit) {
      GUM_ERROR(OutOfBounds, "Asked low value is higher than asked high value")
    }

    lowLimit_  = newLowLimit;
    highLimit_ = newHighLimit;
    computeNbInterval_();
  }

  // Sets lowest possible value
  template < typename GUM_ELEMENT >
  void
      LinearApproximationPolicy< GUM_ELEMENT >::setLowLimit(const GUM_ELEMENT& newLowLimit) {
    if (newLowLimit > this->highLimit_) {
      GUM_ERROR(OutOfBounds, "Value asked is higher than High limit")
    }

    lowLimit_ = newLowLimit;

    computeNbInterval_();
  }

  // Gets lowest possible value
  template < typename GUM_ELEMENT >
  const GUM_ELEMENT& LinearApproximationPolicy< GUM_ELEMENT >::lowLimit() const {
    return lowLimit_;
  }

  // Sets Highest possible value
  template < typename GUM_ELEMENT >
  void
      LinearApproximationPolicy< GUM_ELEMENT >::setHighLimit(const GUM_ELEMENT& newHighLimit) {
    if (newHighLimit < this->lowLimit_) {
      GUM_ERROR(OutOfBounds, "Value asked is lower than low limit")
    }

    highLimit_ = newHighLimit;

    computeNbInterval_();
  }

  // Gets Highest possible value
  template < typename GUM_ELEMENT >
  const GUM_ELEMENT& LinearApproximationPolicy< GUM_ELEMENT >::highLimit() const {
    return highLimit_;
  }

  // Concretely computes the approximate representation
  template < typename GUM_ELEMENT >
  Idx LinearApproximationPolicy< GUM_ELEMENT >::_encode_(const GUM_ELEMENT& value) const {
    if (value <= this->lowLimit_) return 0;

    if (value >= this->highLimit_) return nbInterval_;

    return 1 + Idx(((value - this->lowLimit_) / this->epsilon_));
  }

  // Concretely computes the approximate value from representation
  template < typename GUM_ELEMENT >
  GUM_ELEMENT
      LinearApproximationPolicy< GUM_ELEMENT >::_decode_(const GUM_ELEMENT& representation) const {
    if (representation == 0) return this->lowLimit_;

    if (representation == nbInterval_) return this->highLimit_;

    return (GUM_ELEMENT)(((representation * this->epsilon_) - (this->epsilon_ / 2))
                         + this->lowLimit_);
  }

  // get the number of interval
  template < typename GUM_ELEMENT >
  void LinearApproximationPolicy< GUM_ELEMENT >::computeNbInterval_() {
    nbInterval_ = 1 + Idx((this->highLimit_ - this->lowLimit_) / this->epsilon_);
  }
}   // namespace gum
