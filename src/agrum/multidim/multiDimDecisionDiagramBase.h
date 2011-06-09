
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
#ifndef GUM_MULTI_DIM_DECISION_DIAGRAM_BASE_H
#define GUM_MULTI_DIM_DECISION_DIAGRAM_BASE_H
// ============================================================================
#include <string>
// ============================================================================
#include <agrum/core/hashTable.h>
#include <agrum/core/bijection.h>
// ============================================================================
#include <agrum/multidim/approximationPolicy.h>
#include <agrum/multidim/multiDimReadOnly.h>
#include <agrum/multidim/multiDimDecisionDiagramFactoryBase.h>
// ============================================================================
#include <agrum/graphs/nodeGraphPart.h>
// ============================================================================

namespace gum {

template<typename T_DATA >
class MultiDimDecisionDiagramFactoryBase;

/**
 * @class MultiDimDecisionDiagramBase multiDimDecisionDiagramBase.h <agrum/multidim/multiDimDecisionDiagramBase.h>
 * @brief Class implementingting an decision diagram model
 * Contains all methods used by multidimdecisiondiagram, except for those concerning approximation policy
 * For concrete usage of such multidim, please refer to multiDimDecisionDiagram
 * @ingroup multidim_group
 *
 */
template<typename T_DATA>

class MultiDimDecisionDiagramBase :  public MultiDimReadOnly<T_DATA>, public virtual ApproximationPolicy<T_DATA> {

public:

    // ===========================================================================
    /// @name Constructors / Destructors
    // ===========================================================================
    /// @{

    /**
     * Default constructor.
     */
    MultiDimDecisionDiagramBase( );

    /**
     * Destructor.
     */
    virtual ~MultiDimDecisionDiagramBase();

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
	 * Displays the multidim. 
	 */
	 virtual const std::string toString() const;
	
	/**
	 * Displays the internal representation of i. 
	 */
	 virtual const std::string toString(const Instantiation* i) const;
	
	/**
	 * Displays the DecisionDiagramBase in the dot format
	 */
	 std::string toDot() const;
    
    /**
     * Returns the number of variable truly present in diagram
     */
    Size diagramVarSize ( ) const;
 	
	 
	/// @}

    // ===========================================================================
    /// @name Operators Functions
    // ===========================================================================
    /// @{
	 
	/**
	 * Returns the id of the root node from the diagram
	 */
	const NodeId getRoot() const;
	
	/**
	 * Returns value associated to given node
	 * @throw InvalidNode if node isn't terminal
	 */ 
	 const T_DATA getValueFromNode( NodeId n ) const;
	 
	/**
	 * Returns node's sons map
	 * @throw InvalidNode if node is terminal
	 */
	 const HashTable< Idx, NodeId >* getNodeSons( NodeId n ) const;
	 
	/**
	 * Returns true if node has a default son
	 */
	 bool hasNodeDefaultSon( NodeId n ) const;
	 
	/**
	 * Returns node's default son
	 * @throw InvalidNode if node is terminal
	 */
	 const NodeId getNodeDefaultSon( NodeId n ) const;
	 
	/**
	 * Returns associated variable of given node
	 * @throw InvalidNode if Node is terminal
	 */
	 const DiscreteVariable& getVariableFromNode( NodeId n ) const;

    /**
     * Returns true if node is a chance one
     */
    bool isTerminalNode ( NodeId varId ) const;
	
	///@}

    // ===========================================================================
    /// @name Approximation Handling functions
    // ===========================================================================
    /// @{
	
	/**
	 * Returns a factory that used same approximation pattern
	 */
	 virtual MultiDimDecisionDiagramFactoryBase<T_DATA>* getFactory() const = 0;
	 	
	/**
	 * Returns a factory that used same approximation pattern
	 * Allows to set parameter for that approximation
	 */
	 virtual MultiDimDecisionDiagramFactoryBase<T_DATA>* getFactory( T_DATA epsilon, T_DATA lowerLimit, T_DATA higherLimit) const = 0;
	
	///@}

    // ===========================================================================
    /// @name Fast Large modifications in structure
    // ===========================================================================
    /// @{
    
    /**
     * Sets once and for all variable sequence.
     * @throw OperationNotAllowed if function as already been call;
     */
     void setVariableSequence(  const Sequence< const DiscreteVariable* >& varList );
     
    /**
     * Sets once and for all nodes of the diagram.
     * @throw OperationNotAllowed if function as already been call;
     */
     void setDiagramNodes( const NodeGraphPart& model );
     
    /**
     * Binds once and for all nodes to variables.
     * @throw OperationNotAllowed if function as already been call;
     */
     void setVariableMap( const typename Property< const DiscreteVariable* >::onNodes& varMap );
     
    /**
     * Binds once and for all terminal nodes to value.
     * @throw OperationNotAllowed if function as already been call;
     */
     void setValueMap( const Bijection< NodeId, T_DATA >& valueMap );
     
    /**
     * Links once and for all nodes of the graph.
     * @throw OperationNotAllowed if function as already been call;
     */
     void setDiagramArcs( const typename Property< HashTable< Idx, NodeId >* >::onNodes& arcMap, const typename Property< NodeId >::onNodes& defaultArcMap );
     
     /**
     * Sets once and for all root node.
     * @throw OperationNotAllowed if function as already been call;
     */
     void setRoot( const NodeId& root );

protected :	
	/**
	 * Synchronize content after MultipleChanges. 
	 */
	 void _commitMultipleChanges();
		
	/// @}
	
protected :
	/**
	 * Returns data addressed by inst
	 */
	virtual T_DATA& _get( const Instantiation& inst ) const;
    
    /**
	 * Supposed to replace var x by y. But not authorized in a MultiDimDecisionDiagramBase
	 * @throw OperationNotAllowed without condition.
	 */
	 virtual void _swap( const DiscreteVariable* x, const DiscreteVariable* y );

private:

    /// Mapping between terminal nodes and their values
    Bijection< NodeId, T_DATA > __valueMap;

	/// The graph hidden behin this multidim
	NodeGraphPart __graph;
	
	/// The id of the root of the graph
	NodeId __root;
	
    /// Mapping between id and variable
    typename Property< const DiscreteVariable* >::onNodes __variableMap;
    
    /// Mapping between variable's values and associated node
    typename Property< HashTable< Idx, NodeId >* >::onNodes __arcMap;
    
    /// Mapping between variable's values and associated node
    typename Property< NodeId >::onNodes __defaultArcMap;

	/// Name of this multiDim
	std::string __name;
	
	///Just a boolean to indicates if diagram has been instanciated or not
	bool __isInstanciated;
};

} /* namespace gum */

// ============================================================================
#include <agrum/multidim/multiDimDecisionDiagramBase.tcc>
// ============================================================================
#endif /* GUM_MULTI_DIM_DECISION_DIAGRAM_BASE_H */
// ============================================================================

// kate: indent-mode cstyle; space-indent on; indent-width 0;  replace-tabs on;

