
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
 * @brief Class implementing Algebraic Decision Diagrams
 *
 * @author Jean-Christophe Magnan
 *
 */
#ifndef GUM_MULTI_DIM_ADD_H
#define GUM_MULTI_DIM_ADD_H
// ============================================================================
#include <utility>
#include <string>
// ============================================================================
#include <agrum/multidim/multiDimReadOnly.h>
#include <agrum/core/hashTable.h>
// ============================================================================
#include <agrum/graphs/nodeGraphPart.h>
// ============================================================================

namespace gum {

/**
 * @class MultiDimADD multiDimADD.h <agrum/multidim/multiDimADD.h>
 * @brief Class implementingting an algebraic decision diagram
 * @ingroup multidim_group
 *
 */
template<typename T_DATA>

class MultiDimADD :  public MultiDimReadOnly<T_DATA> {

    //friend class DecisionDiagramFactory<T_DATA>;

public:

    // ===========================================================================
    /// @name Constructors / Destructors
    // ===========================================================================
    /// @{

    /**
     * Default constructor.
     */
    MultiDimADD( Sequence< const DiscreteVariable* > varList, NodeGraphPart model, typename Property< const DiscreteVariable* >::onNodes varMap,
					typename Property< T_DATA >::onNodes terminalNodeMap, typename Property< HashTable< Idx, NodeId >* >::onNodes arcMap, NodeId root );

    /**
     * Destructor.
     */
    ~MultiDimADD();

    /// @}

    // ===========================================================================
    /// @name Accessors/Modifiers
    // ===========================================================================
    /// @{
	
	/**
	 * Returns the name of the miltiDim implementation
	 */
	virtual const std::string& name() const;
		
	/**
	 * Returns valued pointed by inst
	 */
	 virtual T_DATA get( const Instantiation& inst) const;
		
	/// @}

    // ===========================================================================
    /// @name Implementation of MultiDimInterface
    // ===========================================================================
    /// @{
		
	/**
	 * Adds a new var to the variables of the multidimensional matrix. 
	 * @throw OperationNotAllowed cause it's not authorize on read only multidim
	 */
	 virtual void add (const DiscreteVariable &v);
		
	/**
	 * Removes a var from the variables of the multidimensional matrix.  
	 * @throw OperationNotAllowed cause it's not authorize on read only multidim
	 */
	 virtual void erase (const DiscreteVariable &v);
	 
	/**
	 * Returns the real number of parameter used in this table. 
	 */
	 virtual Size realSize () const;
 	
	 
	/// @}

    // ===========================================================================
    /// @name Slave management and extension due to slave management
    // ===========================================================================
    /// @{
		
	/**
	 * Listen to change in a given Instantiation virtual
	 */
	 virtual void changeNotification(Instantiation& i, const DiscreteVariable* var, const Idx& oldval, const Idx& newval);
 	
	/**
	 * Listen to setFirst in a given Instantiation
	 */
	 virtual void setFirstNotification( Instantiation& i );
	 
	/**
	 * Listen to setLast in a given Instantiation
	 */
	 virtual void setLastNotification( Instantiation& i );
	 
	/**
	 * Listen to increment in a given Instantiation
	 */
	 virtual void setIncNotification( Instantiation& i );
	 
	/**
	 * Listen to decrement in a given Instantiation
	 */
	 virtual void setDecNotification( Instantiation& i );
	 
	/**
	 * Listen to an assignement of value in a given Instantiation
	 */
	 virtual void setChangeNotification( Instantiation& i );
	 
	/// @}

    // ===========================================================================
    /// @name Copy Methods
    // ===========================================================================
    /// @{

	/**
	 * Removes all variables in this Container and copy content from src, variable included
	 * @throw OperationNotAllowed cause this is a read only
     */
     void copy( const MultiDimContainer<T_DATA>& src ) const;
 	
	 
	/// @}

    // ===========================================================================
    /// @name Various Methods
    // ===========================================================================
    /// @{

	/**
	 * This method creates a clone of this object, withouth its content
	 * (including variable), you must use this method if you want to ensure
     * that the generated object has the same type than the object containing
     * the called newFactory()
     * For example :
     *   MultiDimArray<double> y;
     *   MultiDimContainer<double>* x = y.newFactory();
     * Then x is a MultiDimArray<double>*
     *
     * @warning you must desallocate by yourself the memory
	 * @throw OperationNotAllowed without condition.
     * @return an empty clone of this object with the same type
     */
     MultiDimContainer<T_DATA>* newFactory() const;
	
	/**
	 * Displays the multidim. 
	 */
	 virtual const std::string toString() const;
	
	/**
	 * Displays the internal representation of i. 
	 */
	 virtual const std::string toString(const Instantiation* i) const;
 	
	 
	/// @}
	
protected :
	/**
	 * Returns data addressed by inst
	 */
	virtual T_DATA& _get( const Instantiation& inst ) const;

    // ===========================================================================
    /// @name Graph manipulation methods.
    // ===========================================================================
    /// @{

    /**
     * Returns true if node is a chance one
     */
    bool _isTerminalNode ( NodeId varId ) const;

    /**
    * Returns a constant reference over a variabe given its node id.
    * @throw NotFound If no variable's id matches varId.
    */
    //virtual const DiscreteVariable& _nodeVariable ( NodeId id ) const;
    

    /// @}
    
    /**
	 * Supposed to replace var x by y. But not authorized in a MultiDimADD
	 * @throw OperationNotAllowed without condition.
	 */
	 virtual void _swap( const DiscreteVariable* x, const DiscreteVariable* y );     

private:

	/// The graph hidden behin this multidim
	NodeGraphPart __graph;
	
	/// The id of the root of the graph
	NodeId __root;
	
    /// Mapping between id and variable
    typename Property< const DiscreteVariable* >::onNodes __variableMap;

    /// Mapping between terminal nodes and their values
    typename Property< T_DATA >::onNodes __terminalNodeMap;
    
    /// Mapping between utility variable's id and their utility table
    typename Property< HashTable< Idx, NodeId >* >::onNodes __arcMap;

	/// Name of this multiDim
	std::string __name;
};

} /* namespace gum */

// ============================================================================
#include <agrum/multidim/multiDimADD.tcc>
// ============================================================================
#endif /* GUM_MULTI_DIM_ADD_H */
// ============================================================================

// kate: indent-mode cstyle; space-indent on; indent-width 0;  replace-tabs on;

