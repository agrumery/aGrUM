
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
		ApproximationPolicy() : _epsilon(0.1), _lowerLimit(0.0), _higherLimit(1.0) { };
		
	/// @}

    // ===========================================================================
    /// @name Accessors/Modifiers
    // ===========================================================================
    /// @{
		
		/// Convert value to approximation representation
		virtual T_DATA fromExact(const T_DATA& value ) const = 0;
		
		/// Convert approximation representation to value
		virtual T_DATA toExact(const T_DATA& value ) const = 0;
		
		/// Sets approximation factor
		inline virtual void setEpsilon( const T_DATA& e ) { _epsilon = e; };
		
		/// Gets approximation factor
		inline T_DATA getEpsilon() const { return _epsilon; };
				
		
		/// Sets lowest possible value
		inline virtual void setLowerLimit( const T_DATA& newLowerLimit ) { _lowerLimit = newLowerLimit; };
		
		/// Gets lowest possible value
		inline T_DATA getLowerLimit( ) const { return _lowerLimit; };
		
		
		/// Sets Highest possible value
		inline virtual void setHigherLimit( const T_DATA& newHigherLimit ) { _higherLimit = newHigherLimit; };
		
		/// Gets Highest possible value
		inline T_DATA getHigherLimit( ) const { return _higherLimit; };
		
	/// @}
		
	protected :		
		/// Approximation factor
		T_DATA _epsilon;
		
		/// Lowest value possible
		T_DATA _lowerLimit;
		
		/// Highest value possible
		T_DATA _higherLimit;
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
		inline T_DATA fromExact(const T_DATA& value ) const { return value;};
		
		/// Convert approximation representation to value
		inline T_DATA toExact(const T_DATA& value ) const {return value;};
		
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
		
		/// Convert value to approximation representation
		inline T_DATA fromExact(const T_DATA& value ) const { 
			
			if( value == this->_lowerLimit )
				return 0;
				
			if( value == this->_higherLimit )
				return _nbInterval;
				
			if( value > this->_higherLimit )
				GUM_ERROR( OutOfUpperBound, "Value asked is higher than High limit" );
				
			if( value < this->_lowerLimit )
				GUM_ERROR( OutOfLowerBound, "Value asked is lower than low limit" );
			
			return ( (int) ( ( value - this->_lowerLimit ) / this->_epsilon ) ) + 1;
		};
		
		/// Convert approximation representation to value
		inline T_DATA toExact(const T_DATA& value ) const {
			
			if( value == 0 )
				return this->_lowerLimit;
				
			if( value == _nbInterval )
				return this->_higherLimit;
				
			if( value > _nbInterval )
				GUM_ERROR( OutOfUpperBound, "Interval Number asked is higher than total number of interval" );
				
			if( value < 0 )
				GUM_ERROR( OutOfLowerBound, "Interval Number asked is negative" );
				
			return ( ( value * this->_epsilon ) - ( this->_epsilon / 2 ) ) + this->_lowerLimit;
		};
		
		
		/// Sets approximation factor
		inline virtual void setEpsilon( const T_DATA& e ) { ApproximationPolicy<T_DATA>::setEpsilon( e ); _getNbInterval(); };
		
		
		/// Sets lowest possible value
		inline virtual void setLowerLimit( const T_DATA& newLowerLimit ) { ApproximationPolicy<T_DATA>::setLowerLimit( newLowerLimit ); _getNbInterval(); };
		
		
		/// Sets Highest possible value
		inline virtual void setHigherLimit( const T_DATA& newHigherLimit ) { ApproximationPolicy<T_DATA>::setHigherLimit( newHigherLimit ); _getNbInterval(); };
	/// @}

protected :
		/// get the number of interval
		inline void _getNbInterval() { _nbInterval =  ( (int) ( this->_higherLimit - this->_lowerLimit ) / this->_epsilon ) + 1; };
		
		T_DATA _nbInterval;
};

}

#endif /* GUM_APPROXIMATION_POLICY_H */
