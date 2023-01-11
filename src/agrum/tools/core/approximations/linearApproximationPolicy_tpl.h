/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief Inlined implementation of gum::LienarApproxiationPolicy.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Jean-Christophe Magnan
 *
 */

// Help IDE parsers
#include <agrum/tools/core/approximations/linearApproximationPolicy.h>

namespace gum {

  // Class constructor
  template < typename GUM_SCALAR >
  LinearApproximationPolicy< GUM_SCALAR >::LinearApproximationPolicy(GUM_SCALAR low,
                                                                     GUM_SCALAR high,
                                                                     GUM_SCALAR eps) :
      ApproximationPolicy< GUM_SCALAR >(),
      lowLimit_(low), highLimit_(high), epsilon_(eps) {
    if (eps <= 0) { GUM_ERROR(OutOfBounds, "Epsilon must be >0") }

    computeNbInterval_();
  }

  // Copy constructor.
  template < typename GUM_SCALAR >
  LinearApproximationPolicy< GUM_SCALAR >::LinearApproximationPolicy(
     const LinearApproximationPolicy< GUM_SCALAR >* md) :
      ApproximationPolicy< GUM_SCALAR >(md),
      epsilon_(md->epsilon_), nbInterval_(md->nbInterval_) {}


  // @brief Convert value to his approximation.
  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR
     LinearApproximationPolicy< GUM_SCALAR >::fromExact(const GUM_SCALAR& value) const {
    return _decode_(GUM_SCALAR(encode(value)));
  }

  // @brief Combine using addition with the given gum::ApproximationPolicy.
  template < typename GUM_SCALAR >
  INLINE void LinearApproximationPolicy< GUM_SCALAR >::combineAdd(
     const ApproximationPolicy< GUM_SCALAR >* ap) {
    try {
      const LinearApproximationPolicy< GUM_SCALAR >* lap
         = dynamic_cast< const LinearApproximationPolicy< GUM_SCALAR >* >(ap);

      GUM_SCALAR newHighLimit = lowLimit_ + lap->lowLimit();
      GUM_SCALAR newLowLimit  = lowLimit_ + lap->lowLimit();

      GUM_SCALAR newVal = lowLimit_ + lap->highLimit();

      if (newHighLimit < newVal) newHighLimit = newVal;

      if (newLowLimit > newVal) newLowLimit = newVal;

      newVal = highLimit_ + lap->lowLimit();

      if (newHighLimit < newVal) newHighLimit = newVal;

      if (newLowLimit > newVal) newLowLimit = newVal;

      newVal = highLimit_ + lap->highLimit();

      if (newHighLimit < newVal) newHighLimit = newVal;

      if (newLowLimit > newVal) newLowLimit = newVal;
    } catch (const std::bad_cast&) {}
  }

  template < typename GUM_SCALAR >
  INLINE void LinearApproximationPolicy< GUM_SCALAR >::combineSub(
     const ApproximationPolicy< GUM_SCALAR >* ap) {
    try {
      const LinearApproximationPolicy< GUM_SCALAR >* lap
         = dynamic_cast< const LinearApproximationPolicy< GUM_SCALAR >* >(ap);

      GUM_SCALAR newHighLimit = lowLimit_ - lap->lowLimit();
      GUM_SCALAR newLowLimit  = lowLimit_ - lap->lowLimit();

      GUM_SCALAR newVal = lowLimit_ - lap->highLimit();

      if (newHighLimit < newVal) newHighLimit = newVal;

      if (newLowLimit > newVal) newLowLimit = newVal;

      newVal = highLimit_ - lap->lowLimit();

      if (newHighLimit < newVal) newHighLimit = newVal;

      if (newLowLimit > newVal) newLowLimit = newVal;

      newVal = highLimit_ - lap->highLimit();

      if (newHighLimit < newVal) newHighLimit = newVal;

      if (newLowLimit > newVal) newLowLimit = newVal;
    } catch (const std::bad_cast&) {}
  }

  template < typename GUM_SCALAR >
  INLINE void LinearApproximationPolicy< GUM_SCALAR >::combineMult(
     const ApproximationPolicy< GUM_SCALAR >* ap) {
    try {
      const LinearApproximationPolicy< GUM_SCALAR >* lap
         = dynamic_cast< const LinearApproximationPolicy< GUM_SCALAR >* >(ap);

      GUM_SCALAR newHighLimit = lowLimit_ * lap->lowLimit();
      GUM_SCALAR newLowLimit  = lowLimit_ * lap->lowLimit();

      GUM_SCALAR newVal = lowLimit_ * lap->highLimit();

      if (newHighLimit < newVal) newHighLimit = newVal;

      if (newLowLimit > newVal) newLowLimit = newVal;

      newVal = highLimit_ * lap->lowLimit();

      if (newHighLimit < newVal) newHighLimit = newVal;

      if (newLowLimit > newVal) newLowLimit = newVal;

      newVal = highLimit_ * lap->highLimit();

      if (newHighLimit < newVal) newHighLimit = newVal;

      if (newLowLimit > newVal) newLowLimit = newVal;
    } catch (const std::bad_cast&) {}
  }

  template < typename GUM_SCALAR >
  INLINE void LinearApproximationPolicy< GUM_SCALAR >::combineDiv(
     const ApproximationPolicy< GUM_SCALAR >* ap) {
    try {
      const LinearApproximationPolicy< GUM_SCALAR >* lap
         = dynamic_cast< const LinearApproximationPolicy< GUM_SCALAR >* >(ap);

      GUM_SCALAR newHighLimit = lowLimit_ / lap->lowLimit();
      GUM_SCALAR newLowLimit  = lowLimit_ / lap->lowLimit();

      GUM_SCALAR newVal = lowLimit_ / lap->highLimit();

      if (newHighLimit < newVal) newHighLimit = newVal;

      if (newLowLimit > newVal) newLowLimit = newVal;

      newVal = highLimit_ / lap->lowLimit();

      if (newHighLimit < newVal) newHighLimit = newVal;

      if (newLowLimit > newVal) newLowLimit = newVal;

      newVal = highLimit_ / lap->highLimit();

      if (newHighLimit < newVal) newHighLimit = newVal;

      if (newLowLimit > newVal) newLowLimit = newVal;
    } catch (const std::bad_cast&) {}
  }

  template < typename GUM_SCALAR >
  INLINE void LinearApproximationPolicy< GUM_SCALAR >::combineMax(
     const ApproximationPolicy< GUM_SCALAR >* ap) {
    try {
      const LinearApproximationPolicy< GUM_SCALAR >* lap
         = dynamic_cast< const LinearApproximationPolicy< GUM_SCALAR >* >(ap);

      GUM_SCALAR newHighLimit = lowLimit_ > lap->lowLimit() ? lowLimit_ : lap->lowLimit();
      GUM_SCALAR newLowLimit  = lowLimit_ > lap->lowLimit() ? lowLimit_ : lap->lowLimit();

      GUM_SCALAR newVal = lowLimit_ > lap->highLimit() ? lowLimit_ : lap->highLimit();

      if (newHighLimit < newVal) newHighLimit = newVal;

      if (newLowLimit > newVal) newLowLimit = newVal;

      newVal = highLimit_ > lap->lowLimit() ? highLimit_ : lap->lowLimit();

      if (newHighLimit < newVal) newHighLimit = newVal;

      if (newLowLimit > newVal) newLowLimit = newVal;

      newVal = highLimit_ > lap->highLimit() ? highLimit_ : lap->highLimit();

      if (newHighLimit < newVal) newHighLimit = newVal;

      if (newLowLimit > newVal) newLowLimit = newVal;
    } catch (const std::bad_cast&) {}
  }

  template < typename GUM_SCALAR >
  INLINE void LinearApproximationPolicy< GUM_SCALAR >::combineMin(
     const ApproximationPolicy< GUM_SCALAR >* ap) {
    try {
      const LinearApproximationPolicy< GUM_SCALAR >* lap
         = dynamic_cast< const LinearApproximationPolicy< GUM_SCALAR >* >(ap);

      GUM_SCALAR newHighLimit = lowLimit_ < lap->lowLimit() ? lowLimit_ : lap->lowLimit();
      GUM_SCALAR newLowLimit  = lowLimit_ < lap->lowLimit() ? lowLimit_ : lap->lowLimit();

      GUM_SCALAR newVal = lowLimit_ < lap->highLimit() ? lowLimit_ : lap->highLimit();

      if (newHighLimit < newVal) newHighLimit = newVal;

      if (newLowLimit > newVal) newLowLimit = newVal;

      newVal = highLimit_ < lap->lowLimit() ? highLimit_ : lap->lowLimit();

      if (newHighLimit < newVal) newHighLimit = newVal;

      if (newLowLimit > newVal) newLowLimit = newVal;

      newVal = highLimit_ < lap->highLimit() ? highLimit_ : lap->highLimit();

      if (newHighLimit < newVal) newHighLimit = newVal;

      if (newLowLimit > newVal) newLowLimit = newVal;
    } catch (const std::bad_cast&) {}
  }

  // Convert value to his approximation. This method is slower than @ref
  // fromExact since it verifies the bounds
  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR
     LinearApproximationPolicy< GUM_SCALAR >::safeFromExact(const GUM_SCALAR& value) {
    if (value > this->highLimit_) {
      GUM_ERROR(OutOfBounds, "Value asked is higher than high limit")
    }

    if (value < this->lowLimit_) { GUM_ERROR(OutOfBounds, "Value asked is lower than low limit") }

    return fromExact(value);
  }

  // Convert value to approximation representation
  template < typename GUM_SCALAR >
  INLINE Idx LinearApproximationPolicy< GUM_SCALAR >::encode(const GUM_SCALAR& value) const {
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
  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR LinearApproximationPolicy< GUM_SCALAR >::decode(Idx representation) const {
    if (representation > nbInterval_) {
      GUM_ERROR(OutOfBounds, "Interval Number asked is higher than total number of interval")
    }

    return _decode_(GUM_SCALAR(representation));
  }

  // Sets approximation factor
  template < typename GUM_SCALAR >
  INLINE void LinearApproximationPolicy< GUM_SCALAR >::setEpsilon(const GUM_SCALAR& e) {
    epsilon_ = e;
    computeNbInterval_();
  }

  // set bounds in a whole
  template < typename GUM_SCALAR >
  INLINE void LinearApproximationPolicy< GUM_SCALAR >::setLimits(const GUM_SCALAR& newLowLimit,
                                                                 const GUM_SCALAR& newHighLimit) {
    if (newLowLimit > newHighLimit) {
      GUM_ERROR(OutOfBounds, "Asked low value is higher than asked high value")
    }

    lowLimit_  = newLowLimit;
    highLimit_ = newHighLimit;
    computeNbInterval_();
  }

  // Sets lowest possible value
  template < typename GUM_SCALAR >
  INLINE void LinearApproximationPolicy< GUM_SCALAR >::setLowLimit(const GUM_SCALAR& newLowLimit) {
    if (newLowLimit > this->highLimit_) {
      GUM_ERROR(OutOfBounds, "Value asked is higher than High limit")
    }

    lowLimit_ = newLowLimit;

    computeNbInterval_();
  }

  // Gets lowest possible value
  template < typename GUM_SCALAR >
  INLINE const GUM_SCALAR& LinearApproximationPolicy< GUM_SCALAR >::lowLimit() const {
    return lowLimit_;
  }

  // Sets Highest possible value
  template < typename GUM_SCALAR >
  INLINE void
     LinearApproximationPolicy< GUM_SCALAR >::setHighLimit(const GUM_SCALAR& newHighLimit) {
    if (newHighLimit < this->lowLimit_) {
      GUM_ERROR(OutOfBounds, "Value asked is lower than low limit")
    }

    highLimit_ = newHighLimit;

    computeNbInterval_();
  }

  // Gets Highest possible value
  template < typename GUM_SCALAR >
  INLINE const GUM_SCALAR& LinearApproximationPolicy< GUM_SCALAR >::highLimit() const {
    return highLimit_;
  }

  // Concretely computes the approximate representation
  template < typename GUM_SCALAR >
  INLINE Idx LinearApproximationPolicy< GUM_SCALAR >::_encode_(const GUM_SCALAR& value) const {
    if (value <= this->lowLimit_) return 0;

    if (value >= this->highLimit_) return nbInterval_;

    return 1 + Idx(((value - this->lowLimit_) / this->epsilon_));
  }

  // Concretely computes the approximate value from representation
  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR
     LinearApproximationPolicy< GUM_SCALAR >::_decode_(const GUM_SCALAR& representation) const {
    if (representation == 0) return this->lowLimit_;

    if (representation == nbInterval_) return this->highLimit_;

    return (GUM_SCALAR)(((representation * this->epsilon_) - (this->epsilon_ / 2))
                        + this->lowLimit_);
  }

  // get the number of interval
  template < typename GUM_SCALAR >
  INLINE void LinearApproximationPolicy< GUM_SCALAR >::computeNbInterval_() {
    nbInterval_ = 1 + Idx((this->highLimit_ - this->lowLimit_) / this->epsilon_);
  }
}   // namespace gum
