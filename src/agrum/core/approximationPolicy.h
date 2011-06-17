
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
#include <agrum/core/inline.h>
#include <agrum/core/types.h>
#include <agrum/core/exceptions.h>
//**********************************************************************

namespace gum {

/**
 * @class ApproximationPolicy approximationPolicy.h <agrum/multidim/approximationPolicy.h>
 * @brief Mother class for all approximation policy classes
 * @ingroup multidim_group
 *
 */
template<typename T_DATA>
class ApproximationPolicy{
	
	public :

    // ===========================================================================
    /// @name Constructors / Destructors
    // ===========================================================================
    /// @{

    /**
     * Default constructor.
     */
		ApproximationPolicy() : _epsilon(0.1), _lowLimit(0.0), _highLimit(1.0) { };
		
	/// @}

    // ===========================================================================
    /// @name Accessors/Modifiers
    // ===========================================================================
    /// @{
		
		/// Convert value to his approximation
		virtual T_DATA fromExact(const T_DATA& value ) const = 0;
		
		
		/// Sets approximation factor
		INLINE virtual void setEpsilon( const T_DATA& e ) { _epsilon = e; };
		
		/// Gets approximation factor
		INLINE T_DATA epsilon() const { return _epsilon; };
				
		
		/// Sets lowest possible value
		INLINE virtual void setLowLimit( const T_DATA& newLowLimit ) { _lowLimit = newLowLimit; };
		
		/// Gets lowest possible value
		INLINE T_DATA lowLimit( ) const { return _lowLimit; };
		
		
		/// Sets Highest possible value
		INLINE virtual void setHighLimit( const T_DATA& newHighLimit ) { _highLimit = newHighLimit; };
		
		/// Gets Highest possible value
		INLINE T_DATA highLimit( ) const { return _highLimit; };
		
	/// @}
		
	protected :		
		/// Approximation factor
		T_DATA _epsilon;
		
		/// Lowest value possible
		T_DATA _lowLimit;
		
		/// Highest value possible
		T_DATA _highLimit;
};


/**
 * @class ExactPolicy approximationPolicy.h <agrum/multidim/approximationPolicy.h>
 * @brief Class implementing exact approximation policy (meaning a value is approximate to itself)
 * @ingroup multidim_group
 */
template<typename T_DATA>
class ExactPolicy : public virtual ApproximationPolicy<T_DATA> {
	public:
		
    // ===========================================================================
    /// @name Constructors / Destructors
    // ===========================================================================
    /// @{

    /**
     * Default constructor.
     */
		ExactPolicy() {};
		
	/// @}

    // ===========================================================================
    /// @name Accessors/Modifiers
    // ===========================================================================
    /// @{
		
		/// Convert value to approximation representation
		INLINE T_DATA fromExact(const T_DATA& value ) const { return value;};
		
	/// @}
};


/**
 * @class ExactPolicy approximationPolicy.h <agrum/multidim/approximationPolicy.h>
 * @brief Class implementing linear approximation policy (meaning possible value are split out in interval)
 * @ingroup multidim_group
 */
template<typename T_DATA>
class LinearApproximationPolicy : public virtual ApproximationPolicy<T_DATA> {
	public:
		
    // ===========================================================================
    /// @name Constructors / Destructors
    // ===========================================================================
    /// @{

    /**
     * Default constructor.
     */
		LinearApproximationPolicy() : _nbInterval(11) {};
		
	/// @}

    // ===========================================================================
    /// @name Accessors/Modifiers
    // ===========================================================================
    /// @{
		
		/// Convert value to his approximation
		INLINE T_DATA fromExact( const T_DATA& value ) const { return __decode( encode( value ) ); };
		
		
		/// Convert value to approximation representation
		INLINE Idx encode(const T_DATA& value ) const { 
				
			if( value > this->_highLimit )
				GUM_ERROR( OutOfUpperBound, "Value asked is higher than High limit" );
				
			if( value < this->_lowLimit )
				GUM_ERROR( OutOfLowerBound, "Value asked is lower than low limit" );
			
			return __encode( value );
		};
		
		/// Convert approximation representation to value
		INLINE T_DATA decode( Idx representation ) const {
				
			if( representation > _nbInterval )
				GUM_ERROR( OutOfUpperBound, "Interval Number asked is higher than total number of interval" );
				
			return __decode( representation );
		};
		
		
		/// Sets approximation factor
		INLINE virtual void setEpsilon( const T_DATA& e ) { ApproximationPolicy<T_DATA>::setEpsilon( e ); _getNbInterval(); };
		
		
		/// Sets lowest possible value
		INLINE virtual void setLowLimit( const T_DATA& newLowLimit ) { ApproximationPolicy<T_DATA>::setLowLimit( newLowLimit ); _getNbInterval(); };
		
		
		/// Sets Highest possible value
		INLINE virtual void setHighLimit( const T_DATA& newHighLimit ) { ApproximationPolicy<T_DATA>::setHighLimit( newHighLimit ); _getNbInterval(); };
	/// @}

protected :
		/// Concretely computes the approximate representation
		INLINE Idx __encode( const T_DATA& value ) const { 	
			
			if( value == this->_lowLimit )
				return 0;
				
			if( value == this->_highLimit )
				return _nbInterval;
				
			return ( (Idx) ( ( value - this->_lowLimit ) / this->_epsilon ) ) + 1; 
		};
		
		/// Concretely computes the approximate value from representation
		INLINE T_DATA __decode( const T_DATA& representation ) const {
			
			if( representation == 0 )
				return this->_lowLimit;
				
			if( representation == _nbInterval )
				return this->_highLimit;
			
			return ( ( representation * this->_epsilon ) - ( this->_epsilon / 2 ) ) + this->_lowLimit;
		};
		
		/// get the number of interval
		INLINE void _getNbInterval() { _nbInterval =  ( (Idx) ( this->_highLimit - this->_lowLimit ) / this->_epsilon ) + 1; };
		
		Idx _nbInterval;
};

}

#endif /* GUM_APPROXIMATION_POLICY_H */
