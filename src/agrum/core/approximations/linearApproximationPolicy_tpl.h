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
 * @brief Inlined implementation of gum::LienarApproxiationPolicy.
 *
 * @author Pierre-Henri WUILLEMIN & Jean-Christophe Magnan
 *
 */

// Help IDE parsers
#include <agrum/core/approximations/linearApproximationPolicy.h>

namespace gum {

  // Class constructor
  template <typename GUM_SCALAR>
  LinearApproximationPolicy<GUM_SCALAR>::LinearApproximationPolicy(
      GUM_SCALAR low, GUM_SCALAR high, GUM_SCALAR eps )
      : ApproximationPolicy<GUM_SCALAR>()
      , _lowLimit( low )
      , _highLimit( high )
      , _epsilon( eps ) {
    if ( eps <= 0 ) {
      GUM_ERROR( OutOfBounds, "Epsilon must be >0" );
    }

    _computeNbInterval();
  }

  // Copy constructor.
  template <typename GUM_SCALAR>
  LinearApproximationPolicy<GUM_SCALAR>::LinearApproximationPolicy(
      const LinearApproximationPolicy<GUM_SCALAR>* md )
      : ApproximationPolicy<GUM_SCALAR>( md )
      , _epsilon( md->_epsilon )
      , _nbInterval( md->_nbInterval ) {}


  // @brief Convert value to his approximation.
  template <typename GUM_SCALAR>
  INLINE GUM_SCALAR LinearApproximationPolicy<GUM_SCALAR>::fromExact(
      const GUM_SCALAR& value ) const {
    return __decode( GUM_SCALAR( encode( value ) ) );
  }

  // @brief Combine using addition with the given gum::ApproximationPolicy.
  template <typename GUM_SCALAR>
  INLINE void LinearApproximationPolicy<GUM_SCALAR>::combineAdd(
      const ApproximationPolicy<GUM_SCALAR>* ap ) {

    try {
      const LinearApproximationPolicy<GUM_SCALAR>* lap =
          dynamic_cast<const LinearApproximationPolicy<GUM_SCALAR>*>( ap );

      GUM_SCALAR newHighLimit = _lowLimit + lap->lowLimit();
      GUM_SCALAR newLowLimit = _lowLimit + lap->lowLimit();

      GUM_SCALAR newVal = _lowLimit + lap->highLimit();

      if ( newHighLimit < newVal ) newHighLimit = newVal;

      if ( newLowLimit > newVal ) newLowLimit = newVal;

      newVal = _highLimit + lap->lowLimit();

      if ( newHighLimit < newVal ) newHighLimit = newVal;

      if ( newLowLimit > newVal ) newLowLimit = newVal;

      newVal = _highLimit + lap->highLimit();

      if ( newHighLimit < newVal ) newHighLimit = newVal;

      if ( newLowLimit > newVal ) newLowLimit = newVal;
    } catch ( const std::bad_cast& ) {
    }
  }

  template <typename GUM_SCALAR>
  INLINE void LinearApproximationPolicy<GUM_SCALAR>::combineSub(
      const ApproximationPolicy<GUM_SCALAR>* ap ) {

    try {
      const LinearApproximationPolicy<GUM_SCALAR>* lap =
          dynamic_cast<const LinearApproximationPolicy<GUM_SCALAR>*>( ap );

      GUM_SCALAR newHighLimit = _lowLimit - lap->lowLimit();
      GUM_SCALAR newLowLimit = _lowLimit - lap->lowLimit();

      GUM_SCALAR newVal = _lowLimit - lap->highLimit();

      if ( newHighLimit < newVal ) newHighLimit = newVal;

      if ( newLowLimit > newVal ) newLowLimit = newVal;

      newVal = _highLimit - lap->lowLimit();

      if ( newHighLimit < newVal ) newHighLimit = newVal;

      if ( newLowLimit > newVal ) newLowLimit = newVal;

      newVal = _highLimit - lap->highLimit();

      if ( newHighLimit < newVal ) newHighLimit = newVal;

      if ( newLowLimit > newVal ) newLowLimit = newVal;
    } catch ( const std::bad_cast& ) {
    }
  }

  template <typename GUM_SCALAR>
  INLINE void LinearApproximationPolicy<GUM_SCALAR>::combineMult(
      const ApproximationPolicy<GUM_SCALAR>* ap ) {

    try {
      const LinearApproximationPolicy<GUM_SCALAR>* lap =
          dynamic_cast<const LinearApproximationPolicy<GUM_SCALAR>*>( ap );

      GUM_SCALAR newHighLimit = _lowLimit * lap->lowLimit();
      GUM_SCALAR newLowLimit = _lowLimit * lap->lowLimit();

      GUM_SCALAR newVal = _lowLimit * lap->highLimit();

      if ( newHighLimit < newVal ) newHighLimit = newVal;

      if ( newLowLimit > newVal ) newLowLimit = newVal;

      newVal = _highLimit * lap->lowLimit();

      if ( newHighLimit < newVal ) newHighLimit = newVal;

      if ( newLowLimit > newVal ) newLowLimit = newVal;

      newVal = _highLimit * lap->highLimit();

      if ( newHighLimit < newVal ) newHighLimit = newVal;

      if ( newLowLimit > newVal ) newLowLimit = newVal;
    } catch ( const std::bad_cast& ) {
    }
  }

  template <typename GUM_SCALAR>
  INLINE void LinearApproximationPolicy<GUM_SCALAR>::combineDiv(
      const ApproximationPolicy<GUM_SCALAR>* ap ) {

    try {
      const LinearApproximationPolicy<GUM_SCALAR>* lap =
          dynamic_cast<const LinearApproximationPolicy<GUM_SCALAR>*>( ap );

      GUM_SCALAR newHighLimit = _lowLimit / lap->lowLimit();
      GUM_SCALAR newLowLimit = _lowLimit / lap->lowLimit();

      GUM_SCALAR newVal = _lowLimit / lap->highLimit();

      if ( newHighLimit < newVal ) newHighLimit = newVal;

      if ( newLowLimit > newVal ) newLowLimit = newVal;

      newVal = _highLimit / lap->lowLimit();

      if ( newHighLimit < newVal ) newHighLimit = newVal;

      if ( newLowLimit > newVal ) newLowLimit = newVal;

      newVal = _highLimit / lap->highLimit();

      if ( newHighLimit < newVal ) newHighLimit = newVal;

      if ( newLowLimit > newVal ) newLowLimit = newVal;
    } catch ( const std::bad_cast& ) {
    }
  }

  template <typename GUM_SCALAR>
  INLINE void LinearApproximationPolicy<GUM_SCALAR>::combineMax(
      const ApproximationPolicy<GUM_SCALAR>* ap ) {

    try {
      const LinearApproximationPolicy<GUM_SCALAR>* lap =
          dynamic_cast<const LinearApproximationPolicy<GUM_SCALAR>*>( ap );

      GUM_SCALAR newHighLimit =
          _lowLimit > lap->lowLimit() ? _lowLimit : lap->lowLimit();
      GUM_SCALAR newLowLimit =
          _lowLimit > lap->lowLimit() ? _lowLimit : lap->lowLimit();

      GUM_SCALAR newVal =
          _lowLimit > lap->highLimit() ? _lowLimit : lap->highLimit();

      if ( newHighLimit < newVal ) newHighLimit = newVal;

      if ( newLowLimit > newVal ) newLowLimit = newVal;

      newVal = _highLimit > lap->lowLimit() ? _highLimit : lap->lowLimit();

      if ( newHighLimit < newVal ) newHighLimit = newVal;

      if ( newLowLimit > newVal ) newLowLimit = newVal;

      newVal = _highLimit > lap->highLimit() ? _highLimit : lap->highLimit();

      if ( newHighLimit < newVal ) newHighLimit = newVal;

      if ( newLowLimit > newVal ) newLowLimit = newVal;
    } catch ( const std::bad_cast& ) {
    }
  }

  template <typename GUM_SCALAR>
  INLINE void LinearApproximationPolicy<GUM_SCALAR>::combineMin(
      const ApproximationPolicy<GUM_SCALAR>* ap ) {

    try {
      const LinearApproximationPolicy<GUM_SCALAR>* lap =
          dynamic_cast<const LinearApproximationPolicy<GUM_SCALAR>*>( ap );

      GUM_SCALAR newHighLimit =
          _lowLimit < lap->lowLimit() ? _lowLimit : lap->lowLimit();
      GUM_SCALAR newLowLimit =
          _lowLimit < lap->lowLimit() ? _lowLimit : lap->lowLimit();

      GUM_SCALAR newVal =
          _lowLimit < lap->highLimit() ? _lowLimit : lap->highLimit();

      if ( newHighLimit < newVal ) newHighLimit = newVal;

      if ( newLowLimit > newVal ) newLowLimit = newVal;

      newVal = _highLimit < lap->lowLimit() ? _highLimit : lap->lowLimit();

      if ( newHighLimit < newVal ) newHighLimit = newVal;

      if ( newLowLimit > newVal ) newLowLimit = newVal;

      newVal = _highLimit < lap->highLimit() ? _highLimit : lap->highLimit();

      if ( newHighLimit < newVal ) newHighLimit = newVal;

      if ( newLowLimit > newVal ) newLowLimit = newVal;
    } catch ( const std::bad_cast& ) {
    }
  }

  // Convert value to his approximation. This method is slower than @ref
  // fromExact since it verifies the bounds
  template <typename GUM_SCALAR>
  INLINE GUM_SCALAR
  LinearApproximationPolicy<GUM_SCALAR>::safeFromExact( const GUM_SCALAR& value ) {
    if ( value > this->_highLimit ) {
      GUM_ERROR( OutOfUpperBound, "Value asked is higher than high limit" );
    }

    if ( value < this->_lowLimit ) {
      GUM_ERROR( OutOfLowerBound, "Value asked is lower than low limit" );
    }

    return fromExact( value );
  }

  // Convert value to approximation representation
  template <typename GUM_SCALAR>
  INLINE Idx
  LinearApproximationPolicy<GUM_SCALAR>::encode( const GUM_SCALAR& value ) const {
// we keep the bounds checked in debug mode
#ifndef NDEBUG
    if ( value > this->_highLimit ) {
      GUM_TRACE( value << " not in (" << this->_lowLimit << "-" << this->_highLimit
                       << ")" );
      GUM_ERROR( OutOfUpperBound, "Value asked is higher than High limit" );
    }

    if ( value < this->_lowLimit ) {
      GUM_TRACE( value << " not in (" << this->_lowLimit << "-" << this->_highLimit
                       << ")" );
      GUM_ERROR( OutOfLowerBound, "Value asked is lower than low limit" );
    }

#endif  // NDEBUG
    return __encode( value );
  }

  // Convert approximation representation to value
  template <typename GUM_SCALAR>
  INLINE GUM_SCALAR
  LinearApproximationPolicy<GUM_SCALAR>::decode( Idx representation ) const {

    if ( representation > _nbInterval ) {
      GUM_ERROR( OutOfUpperBound,
                 "Interval Number asked is higher than total number of interval" );
    }

    return __decode( GUM_SCALAR( representation ) );
  }

  // Sets approximation factor
  template <typename GUM_SCALAR>
  INLINE void
  LinearApproximationPolicy<GUM_SCALAR>::setEpsilon( const GUM_SCALAR& e ) {
    _epsilon = e;
    _computeNbInterval();
  }

  // set bounds in a whole
  template <typename GUM_SCALAR>
  INLINE void LinearApproximationPolicy<GUM_SCALAR>::setLimits(
      const GUM_SCALAR& newLowLimit, const GUM_SCALAR& newHighLimit ) {
    if ( newLowLimit > newHighLimit ) {
      GUM_ERROR( OutOfBounds, "Asked low value is higher than asked high value" );
    }

    _lowLimit = newLowLimit;
    _highLimit = newHighLimit;
    _computeNbInterval();
  }

  // Sets lowest possible value
  template <typename GUM_SCALAR>
  INLINE void LinearApproximationPolicy<GUM_SCALAR>::setLowLimit(
      const GUM_SCALAR& newLowLimit ) {
    if ( newLowLimit > this->_highLimit ) {
      GUM_ERROR( OutOfUpperBound, "Value asked is higher than High limit" );
    }

    _lowLimit = newLowLimit;

    _computeNbInterval();
  }

  // Gets lowest possible value
  template <typename GUM_SCALAR>
  INLINE const GUM_SCALAR&
               LinearApproximationPolicy<GUM_SCALAR>::lowLimit() const {
    return _lowLimit;
  }

  // Sets Highest possible value
  template <typename GUM_SCALAR>
  INLINE void LinearApproximationPolicy<GUM_SCALAR>::setHighLimit(
      const GUM_SCALAR& newHighLimit ) {

    if ( newHighLimit < this->_lowLimit ) {
      GUM_ERROR( OutOfLowerBound, "Value asked is lower than low limit" );
    }

    _highLimit = newHighLimit;

    _computeNbInterval();
  }

  // Gets Highest possible value
  template <typename GUM_SCALAR>
  INLINE const GUM_SCALAR&
               LinearApproximationPolicy<GUM_SCALAR>::highLimit() const {
    return _highLimit;
  }

  // Concretely computes the approximate representation
  template <typename GUM_SCALAR>
  INLINE Idx LinearApproximationPolicy<GUM_SCALAR>::__encode(
      const GUM_SCALAR& value ) const {

    if ( value <= this->_lowLimit ) return 0;

    if ( value >= this->_highLimit ) return _nbInterval;

    return 1 + Idx( ( ( value - this->_lowLimit ) / this->_epsilon ) );
  }

  // Concretely computes the approximate value from representation
  template <typename GUM_SCALAR>
  INLINE GUM_SCALAR LinearApproximationPolicy<GUM_SCALAR>::__decode(
      const GUM_SCALAR& representation ) const {

    if ( representation == 0 ) return this->_lowLimit;

    if ( representation == _nbInterval ) return this->_highLimit;

    return ( GUM_SCALAR )(
        ( ( representation * this->_epsilon ) - ( this->_epsilon / 2 ) ) +
        this->_lowLimit );
  }

  // get the number of interval
  template <typename GUM_SCALAR>
  INLINE void LinearApproximationPolicy<GUM_SCALAR>::_computeNbInterval() {
    _nbInterval =
        1 + Idx( ( this->_highLimit - this->_lowLimit ) / this->_epsilon );
  }
}
