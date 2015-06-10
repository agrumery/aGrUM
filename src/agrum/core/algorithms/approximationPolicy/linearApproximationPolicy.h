
/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
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
 * @brief Classes used to practice approximation on value
 *
 * @author Pierre-Henri WUILLEMIN & Jean-Christophe Magnan
 *
 */
#ifndef GUM_LINEAR_APPROXIMATION_POLICY_H
#define GUM_LINEAR_APPROXIMATION_POLICY_H
//**********************************************************************
#include <typeinfo>
//**********************************************************************
#include <agrum/config.h>
#include <agrum/core/algorithms/approximationPolicy/approximationPolicy.h>
//**********************************************************************

namespace gum {

  /**
   * @class LinearApproximationPolicy approximationPolicy.h
   * <agrum/multidim/approximationPolicy.h>
   * @brief Class implementing linear approximation policy (meaning possible value
   * are split out in interval)
   * @ingroup multidim_group
   */
  template <typename GUM_SCALAR>
  class LinearApproximationPolicy : public virtual ApproximationPolicy<GUM_SCALAR> {
    public:
    // ===========================================================================
    /// @name Constructors / Destructors
    // ===========================================================================
    /// @{

    /**
     * Default constructor.
     * @throw OutOfBound if out of bounds (low<high, eps>0)
     */
    LinearApproximationPolicy(GUM_SCALAR low = (GUM_SCALAR)0.0,
                              GUM_SCALAR high = (GUM_SCALAR)1.0,
                              GUM_SCALAR eps = (GUM_SCALAR)0.1)
        : ApproximationPolicy<GUM_SCALAR>(), _lowLimit(low), _highLimit(high),
          _epsilon(eps) {
      if (eps <= 0) {
        GUM_ERROR(OutOfBounds, "Epsilon must be >0");
      }

      _computeNbInterval();
    };

    /**
     * Copy constructor.
     */
    LinearApproximationPolicy(const LinearApproximationPolicy<GUM_SCALAR> *md)
        : ApproximationPolicy<GUM_SCALAR>(md), _epsilon(md->_epsilon),
          _nbInterval(md->_nbInterval){};

    /// @}

    // ===========================================================================
    /// @name Accessors/Modifiers
    // ===========================================================================
    /// @{

    /// Convert value to his approximation
    INLINE GUM_SCALAR fromExact(const GUM_SCALAR &value) const {
      return __decode(encode(value));
    };

    void combineAdd(const ApproximationPolicy<GUM_SCALAR> *ap) {

      try {
        const LinearApproximationPolicy<GUM_SCALAR> *lap =
            dynamic_cast<const LinearApproximationPolicy<GUM_SCALAR> *>(ap);

        GUM_SCALAR newHighLimit = _lowLimit + lap->lowLimit();
        GUM_SCALAR newLowLimit = _lowLimit + lap->lowLimit();

        GUM_SCALAR newVal = _lowLimit + lap->highLimit();

        if (newHighLimit < newVal)
          newHighLimit = newVal;

        if (newLowLimit > newVal)
          newLowLimit = newVal;

        newVal = _highLimit + lap->lowLimit();

        if (newHighLimit < newVal)
          newHighLimit = newVal;

        if (newLowLimit > newVal)
          newLowLimit = newVal;

        newVal = _highLimit + lap->highLimit();

        if (newHighLimit < newVal)
          newHighLimit = newVal;

        if (newLowLimit > newVal)
          newLowLimit = newVal;
      } catch (const std::bad_cast &e) {
      }
    };

    void combineSub(const ApproximationPolicy<GUM_SCALAR> *ap) {

      try {
        const LinearApproximationPolicy<GUM_SCALAR> *lap =
            dynamic_cast<const LinearApproximationPolicy<GUM_SCALAR> *>(ap);

        GUM_SCALAR newHighLimit = _lowLimit - lap->lowLimit();
        GUM_SCALAR newLowLimit = _lowLimit - lap->lowLimit();

        GUM_SCALAR newVal = _lowLimit - lap->highLimit();

        if (newHighLimit < newVal)
          newHighLimit = newVal;

        if (newLowLimit > newVal)
          newLowLimit = newVal;

        newVal = _highLimit - lap->lowLimit();

        if (newHighLimit < newVal)
          newHighLimit = newVal;

        if (newLowLimit > newVal)
          newLowLimit = newVal;

        newVal = _highLimit - lap->highLimit();

        if (newHighLimit < newVal)
          newHighLimit = newVal;

        if (newLowLimit > newVal)
          newLowLimit = newVal;
      } catch (const std::bad_cast &e) {
      }
    };

    void combineMult(const ApproximationPolicy<GUM_SCALAR> *ap) {

      try {
        const LinearApproximationPolicy<GUM_SCALAR> *lap =
            dynamic_cast<const LinearApproximationPolicy<GUM_SCALAR> *>(ap);

        GUM_SCALAR newHighLimit = _lowLimit * lap->lowLimit();
        GUM_SCALAR newLowLimit = _lowLimit * lap->lowLimit();

        GUM_SCALAR newVal = _lowLimit * lap->highLimit();

        if (newHighLimit < newVal)
          newHighLimit = newVal;

        if (newLowLimit > newVal)
          newLowLimit = newVal;

        newVal = _highLimit * lap->lowLimit();

        if (newHighLimit < newVal)
          newHighLimit = newVal;

        if (newLowLimit > newVal)
          newLowLimit = newVal;

        newVal = _highLimit * lap->highLimit();

        if (newHighLimit < newVal)
          newHighLimit = newVal;

        if (newLowLimit > newVal)
          newLowLimit = newVal;
      } catch (const std::bad_cast &e) {
      }
    };

    void combineDiv(const ApproximationPolicy<GUM_SCALAR> *ap) {

      try {
        const LinearApproximationPolicy<GUM_SCALAR> *lap =
            dynamic_cast<const LinearApproximationPolicy<GUM_SCALAR> *>(ap);

        GUM_SCALAR newHighLimit = _lowLimit / lap->lowLimit();
        GUM_SCALAR newLowLimit = _lowLimit / lap->lowLimit();

        GUM_SCALAR newVal = _lowLimit / lap->highLimit();

        if (newHighLimit < newVal)
          newHighLimit = newVal;

        if (newLowLimit > newVal)
          newLowLimit = newVal;

        newVal = _highLimit / lap->lowLimit();

        if (newHighLimit < newVal)
          newHighLimit = newVal;

        if (newLowLimit > newVal)
          newLowLimit = newVal;

        newVal = _highLimit / lap->highLimit();

        if (newHighLimit < newVal)
          newHighLimit = newVal;

        if (newLowLimit > newVal)
          newLowLimit = newVal;
      } catch (const std::bad_cast &e) {
      }
    };

    void combineMax(const ApproximationPolicy<GUM_SCALAR> *ap) {

      try {
        const LinearApproximationPolicy<GUM_SCALAR> *lap =
            dynamic_cast<const LinearApproximationPolicy<GUM_SCALAR> *>(ap);

        GUM_SCALAR newHighLimit =
            _lowLimit > lap->lowLimit() ? _lowLimit : lap->lowLimit();
        GUM_SCALAR newLowLimit =
            _lowLimit > lap->lowLimit() ? _lowLimit : lap->lowLimit();

        GUM_SCALAR newVal =
            _lowLimit > lap->highLimit() ? _lowLimit : lap->highLimit();

        if (newHighLimit < newVal)
          newHighLimit = newVal;

        if (newLowLimit > newVal)
          newLowLimit = newVal;

        newVal = _highLimit > lap->lowLimit() ? _highLimit : lap->lowLimit();

        if (newHighLimit < newVal)
          newHighLimit = newVal;

        if (newLowLimit > newVal)
          newLowLimit = newVal;

        newVal = _highLimit > lap->highLimit() ? _highLimit : lap->highLimit();

        if (newHighLimit < newVal)
          newHighLimit = newVal;

        if (newLowLimit > newVal)
          newLowLimit = newVal;
      } catch (const std::bad_cast &e) {
      }
    };

    void combineMin(const ApproximationPolicy<GUM_SCALAR> *ap) {

      try {
        const LinearApproximationPolicy<GUM_SCALAR> *lap =
            dynamic_cast<const LinearApproximationPolicy<GUM_SCALAR> *>(ap);

        GUM_SCALAR newHighLimit =
            _lowLimit < lap->lowLimit() ? _lowLimit : lap->lowLimit();
        GUM_SCALAR newLowLimit =
            _lowLimit < lap->lowLimit() ? _lowLimit : lap->lowLimit();

        GUM_SCALAR newVal =
            _lowLimit < lap->highLimit() ? _lowLimit : lap->highLimit();

        if (newHighLimit < newVal)
          newHighLimit = newVal;

        if (newLowLimit > newVal)
          newLowLimit = newVal;

        newVal = _highLimit < lap->lowLimit() ? _highLimit : lap->lowLimit();

        if (newHighLimit < newVal)
          newHighLimit = newVal;

        if (newLowLimit > newVal)
          newLowLimit = newVal;

        newVal = _highLimit < lap->highLimit() ? _highLimit : lap->highLimit();

        if (newHighLimit < newVal)
          newHighLimit = newVal;

        if (newLowLimit > newVal)
          newLowLimit = newVal;
      } catch (const std::bad_cast &e) {
      }
    };

    /// Convert value to his approximation. This method is slower than @ref fromExact
    /// since it verifies the bounds
    /// @throw OutOfLowerBound and OutOfUpperBound
    INLINE GUM_SCALAR safeFromExact(const GUM_SCALAR &value) {
      if (value > this->_highLimit) {
        GUM_ERROR(OutOfUpperBound, "Value asked is higher than high limit");
      }

      if (value < this->_lowLimit) {
        GUM_ERROR(OutOfLowerBound, "Value asked is lower than low limit");
      }

      return fromExact(value);
    };

    /// Convert value to approximation representation
    /// @throw OutOfLowerBound,OutOfUpperBound
    INLINE Idx encode(const GUM_SCALAR &value) const {
// we keep the bounds checked in debug mode
#ifndef NDEBUG
      if (value > this->_highLimit) {
        GUM_TRACE(value << " not in (" << this->_lowLimit << "-" << this->_highLimit
                        << ")");
        GUM_ERROR(OutOfUpperBound, "Value asked is higher than High limit");
      }

      if (value < this->_lowLimit) {
        GUM_TRACE(value << " not in (" << this->_lowLimit << "-" << this->_highLimit
                        << ")");
        GUM_ERROR(OutOfLowerBound, "Value asked is lower than low limit");
      }

#endif // NDEBUG
      return __encode(value);
    };

    /// Convert approximation representation to value
    INLINE GUM_SCALAR decode(Idx representation) const {

      if (representation > _nbInterval) {
        GUM_ERROR(OutOfUpperBound,
                  "Interval Number asked is higher than total number of interval");
      }

      return __decode(representation);
    };

    /// Sets approximation factor
    INLINE virtual void setEpsilon(const GUM_SCALAR &e) {
      _epsilon = e;
      _computeNbInterval();
    };

    /// set bounds in a whole
    /// @throw OutOfBounds
    INLINE virtual void setLimits(const GUM_SCALAR &newLowLimit,
                                  const GUM_SCALAR &newHighLimit) {
      if (newLowLimit > newHighLimit) {
        GUM_ERROR(OutOfBounds, "Asked low value is higher than asked high value");
      }

      _lowLimit = newLowLimit;
      _highLimit = newHighLimit;
      _computeNbInterval();
    }

    /// Sets lowest possible value
    /// @throw OutOfUpperBound
    INLINE virtual void setLowLimit(const GUM_SCALAR &newLowLimit) {
      if (newLowLimit > this->_highLimit) {
        GUM_ERROR(OutOfUpperBound, "Value asked is higher than High limit");
      }

      _lowLimit = newLowLimit;

      _computeNbInterval();
    };

    /// Gets lowest possible value
    INLINE const GUM_SCALAR &lowLimit() const { return _lowLimit; };

    /// Sets Highest possible value
    /// @throw OutOfLowerBound
    INLINE virtual void setHighLimit(const GUM_SCALAR &newHighLimit) {

      if (newHighLimit < this->_lowLimit) {
        GUM_ERROR(OutOfLowerBound, "Value asked is lower than low limit");
      }

      _highLimit = newHighLimit;

      _computeNbInterval();
    };

    /// Gets Highest possible value
    INLINE const GUM_SCALAR &highLimit() const { return _highLimit; };
    /// @}

    protected:
    /// Lowest value possible
    GUM_SCALAR _lowLimit;

    /// Highest value possible
    GUM_SCALAR _highLimit;

    /// Approximation factor
    GUM_SCALAR _epsilon;

    /// Concretely computes the approximate representation
    /// @warning we accept value smaller or higher than limits : please @see
    /// ApproximationPolicy::safeFromExact
    INLINE Idx __encode(const GUM_SCALAR &value) const {

      if (value <= this->_lowLimit)
        return 0;

      if (value >= this->_highLimit)
        return _nbInterval;

      //~ std::cout << "Original value : " << value << " - Low Limit : " <<
      // this->_lowLimit << " - epsilon : "<< this->_epsilon << " - Encoded Value : "
      //<< (( Idx )(( value - this->_lowLimit ) / this->_epsilon ) ) + 1 <<
      // std::endl;

      return ((Idx)((value - this->_lowLimit) / this->_epsilon)) + 1;
    };

    /// Concretely computes the approximate value from representation
    INLINE GUM_SCALAR __decode(const GUM_SCALAR &representation) const {

      if (representation == 0)
        return this->_lowLimit;

      if (representation == _nbInterval)
        return this->_highLimit;

      //~ std::cout << "Rep value : " << representation << " - Low Limit : " <<
      // this->_lowLimit << " - epsilon : "<< this->_epsilon << " - DecodedValue : "
      //<< (( representation * this->_epsilon ) - ( this->_epsilon / 2 ) ) +
      // this->_lowLimit << std::endl;

      return ((representation * this->_epsilon) - (this->_epsilon / 2)) +
             this->_lowLimit;
    };

    /// get the number of interval
    INLINE void _computeNbInterval() {
      _nbInterval = ((Idx)(this->_highLimit - this->_lowLimit) / this->_epsilon) + 1;
    };

    Idx _nbInterval;
  };
}

#endif /* GUM_LINEAR_APPROXIMATION_POLICY_H */
