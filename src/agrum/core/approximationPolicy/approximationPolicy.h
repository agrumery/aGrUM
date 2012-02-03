
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
#ifndef GUM_APPROXIMATION_POLICY_H
#define GUM_APPROXIMATION_POLICY_H
//**********************************************************************
#include <agrum/config.h>

//**********************************************************************

namespace gum {

  /**
   * @class ApproximationPolicy approximationPolicy.h <agrum/multidim/approximationPolicy.h>
   * @brief Mother class for all approximation policy classes
   * @ingroup multidim_group
   *
   */
  template<typename T_DATA>
  class ApproximationPolicy {

    public :

      // ===========================================================================
      /// @name Constructors / Destructors
      // ===========================================================================
      /// @{

      /**
       * Default constructor.
       */
      ApproximationPolicy( T_DATA low=( T_DATA )0.0,T_DATA high=( T_DATA )1.1 ):_lowLimit( low ),_highLimit( high ) {
        if ( low>=high ) {
          GUM_ERROR( OutOfBounds, "lost must be < high" );
        }
      };

      /**
       * copy constructor.
       */
      ApproximationPolicy( const ApproximationPolicy<T_DATA>& md ) : _lowLimit( md._lowLimit ),_highLimit( md._highLimit ) { };

      /// @}

      // ===========================================================================
      /// @name Accessors/Modifiers
      // ===========================================================================
      /// @{

      /// Convert value to his approximation. This method (at least in release mode, should not verify the limits
      virtual T_DATA fromExact( const T_DATA& value ) const = 0;

      /// Convert value to his approximation. This method is slower than @fromExact since it verifies the bounds
      /// @throw OutOfLowerBound and OutOfUpperBound
      INLINE T_DATA safeFromExact( const T_DATA & value ) {
        if ( value > this->_highLimit ) {
          GUM_ERROR( OutOfUpperBound, "Value asked is higher than High limit" );
	}

        if ( value < this->_lowLimit ) {
          GUM_ERROR( OutOfLowerBound, "Value asked is lower than low limit" );
	}

	return fromExact(value);
      };

      /// set bounds in a whole
      /// @throw OutOfBounds
      INLINE virtual void setLimits( const T_DATA& newLowLimit, const T_DATA& newHighLimit ) {
        if ( newLowLimit >newHighLimit ) {
          GUM_ERROR( OutOfBounds, "Asked low value is higher than asked high value" );
	}

        _lowLimit=newLowLimit;
        _highLimit=newHighLimit;
      }

      /// Sets lowest possible value
      /// @throw OutOfUpperBound
      INLINE virtual void setLowLimit( const T_DATA& newLowLimit ) {
        if ( newLowLimit > this->_highLimit ) {
          GUM_ERROR( OutOfUpperBound, "Value asked is higher than High limit" );
	}

        _lowLimit = newLowLimit;
      };

      /// Gets lowest possible value
      INLINE T_DATA lowLimit( ) const {
        return _lowLimit;
      };


      /// Sets Highest possible value
      /// @throw OutOfLowerBound
      INLINE virtual void setHighLimit( const T_DATA& newHighLimit ) {
        if ( newHighLimit < this->_lowLimit ) {
          GUM_ERROR( OutOfLowerBound, "Value asked is lower than low limit" );
	}

        _highLimit = newHighLimit;
      };

      /// Gets Highest possible value
      INLINE T_DATA highLimit( ) const {
        return _highLimit;
      };
      /// @}

    protected:

      /// Lowest value possible
      T_DATA _lowLimit;

      /// Highest value possible
      T_DATA _highLimit;
  };

}

#endif /* GUM_APPROXIMATION_POLICY_H */
