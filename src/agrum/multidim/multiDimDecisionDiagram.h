
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
 * @brief
 *
 * @author Jean-Christophe Magnan 
 *
 */
#ifndef GUM_MULTI_DIM_DECISION_DIAGRAM_H
#define GUM_MULTI_DIM_DECISION_DIAGRAM_H
// ============================================================================
#include <agrum/multidim/approximationPolicy.h>
#include <agrum/multidim/multiDimDecisionDiagramBase.h>
#include <agrum/multidim/multiDimDecisionDiagramFactory.h>
// ============================================================================

namespace gum {

template< typename T, template<class> class IApproximationPolicy>
class MultiDimDecisionDiagramFactory;

/**
 * @class MultiDimDecisionDiagramBase multiDimDecisionDiagramBase.h <agrum/multidim/multiDimDecisionDiagramBase.h>
 * @brief Class implementingting an decision diagram model
 * For description of general methods, please refer to multidimdecisiondiagrambase
 * For specific implementation and use of multidimdecisiondiagram this is the right place :
 * The idea is that approximation policy is given as a template class to this class ( please 
 * refer to approximationpolicy.h for more details on such classes ). MultiDimDecisionDiagram inherits from
 * this class. 
 * A default classe exists : Exact Policy whixh commit no approximation on value given. As a default
 * you just have to declare MultiDimDecisionDiagram<T_DATA> and play with.
 * If you want to use a different approximation pattern, just MultiDimDecisionDiagram<T_DATA, CLASS_DESCRIBING_NEW_PATTERN>.
 * Therefor, if do operation on MultiDimDecisionDiagram the return type will likely be MultiDimDecisionDiagramBase
 * @ingroup multidim_group
 *
 */
template<typename T_DATA,template <class> class IApproximationPolicy  = ExactPolicy>

class MultiDimDecisionDiagram :  public MultiDimDecisionDiagramBase<T_DATA>, public IApproximationPolicy<T_DATA> {

public:

    // ===========================================================================
    /// @name Constructors / Destructors
    // ===========================================================================
    /// @{

    /**
     * Default constructor.
     */
    MultiDimDecisionDiagram( );

    /**
     * Destructor.
     */
    ~MultiDimDecisionDiagram();

    /// @}

    // ===========================================================================
    /// @name Various Methods
    // ===========================================================================
    /// @{

	/**
	 * This method creates a clone of this object, without its content
	 * (including variable), you must use this method if you want to ensure
     * that the generated object has the same type than the object containing
     * the called newFactory()
     * For example :
     *   MultiDimArray<double> y;
     *   MultiDimContainer<double>* x = y.newFactory();
     * Then x is a MultiDimArray<double>*
     *
     * @warning you must desallocate by yourself the memory
     * @return an empty clone of this object with the same type
     */
     MultiDimContainer<T_DATA>* newFactory() const;	 
	
	///@}
	
    // ===========================================================================
    /// @name Approximation Handling functions
    // ===========================================================================
    /// @{
	
	/**
	 * Returns a factory that used same approximation pattern
	 */
	 virtual MultiDimDecisionDiagramFactoryBase<T_DATA>* getFactory() const;
	 	
	/**
	 * Returns a factory that used same approximation pattern
	 * Allows to set parameter for that approximation
	 */
	 virtual MultiDimDecisionDiagramFactoryBase<T_DATA>* getFactory( T_DATA epsilon, T_DATA lowerLimit, T_DATA higherLimit) const;
	 
	/// Convert value to approximation representation
	inline T_DATA fromExact( const T_DATA& value ) const { return IApproximationPolicy<T_DATA>::fromExact( value ); };
	 
	/// Convert approximation representation to value
	inline T_DATA toExact( const T_DATA& value ) const { return IApproximationPolicy<T_DATA>::toExact( value ); };
	 
	
	///@}

protected :
};

} /* namespace gum */

// ============================================================================
#include <agrum/multidim/multiDimDecisionDiagram.tcc>
// ============================================================================
#endif /* GUM_MULTI_DIM_DECISION_DIAGRAM_H */
// ============================================================================

// kate: indent-mode cstyle; space-indent on; indent-width 0;  replace-tabs on;


