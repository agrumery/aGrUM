
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
 * @brief Class allowing creation of Algebraic Decision Diagram
 *
 * @author Jean-Christophe Magnan
 *
 */
#ifndef GUM_MULTI_DIM_ADD_FACTORY_H
#define GUM_MULTI_DIM_ADD_FACTORY_H
// ============================================================================
#include <utility>
#include <string>
// ============================================================================
#include <agrum/multidim/multiDimADD.h>
#include <agrum/core/hashTable.h>
// ============================================================================
#include <agrum/graphs/DAG.h>
// ============================================================================

namespace gum {

/**
 * @class MultiDimADDFactory multiDimADDFactory.h <agrum/multidim/multiDimADDFactory.h>
 * @brief Class implementingting an algebraic decision diagram factory
 * @ingroup multidim_group
 *
 */
template<typename T_DATA>

class MultiDimADDFactory {
	
	public :

    // ===========================================================================
    /// @name Constructors, Destructors.
    // ===========================================================================
    /// @{

    /**
     * Default constructor.
     */
    MultiDimADDFactory();

    /**
     * Destructor.
     * @warnings : this will not destroy properties on node. They ahve to be removed on multidim destruction
     */
    ~MultiDimADDFactory();
		
	/// @}

    // ===========================================================================
    /// @name Graph manipulation methods.
    // ===========================================================================
    /// @{
	 
    /**
     * Adds a variable and its associate non terminal node. The id of the new
     * variable is automatically generated.
     * 
     * @param variable The variable added by copy.
     * @return the id of the added variable.
     */
     NodeId addNonTerminalNode( const DiscreteVariable& var );

    /**
	 * Adds a value and it's associate terminal node. The id of the new
	 * variable is automatically generated.
	 *
	 * @param value The value added by copy.
	 * @return the id of the added variable.
	 * 
	 * If a terminal node with such value already exists, its value will be return instead.
	 */
	 NodeId addTerminalNode ( const T_DATA& value );


    /**
     * Erases a node from the diagram
     * If no variable matches the id, then nothing is done.
     *
     * @param id The id of the variable to erase.
     */
    void eraseNode ( NodeId id );


    /**
     * Adds an arc in the DD
     *
     * @param from and
     * @param to as NodeId
     * @param value the value of the arc
     * @throw InvalidNode If from and/or tail are not in the DD.
     * @throw OperationNotAllowed if head is a terminal node
     * @throw DuplicateElement if another arc linking those nodes already exists
     */
    void insertArc ( NodeId from, NodeId to, Idx value );


    /**
     * Adds a default arc in the DD
     *
     * @param from and
     * @param to as NodeId
     * @throw InvalidNode If from and/or tail are not in the DD.
     * @throw OperationNotAllowed if head is a terminal node
     * @throw DuplicateElement if another arc linking those nodes already exists
     */
    void insertDefaultArc ( NodeId from, NodeId to );


    /**
     * Erases an arc in the DD
     *
     * @param from and
     * @param to as NodeId
     * @throw InvalidNode If from and/or tail are not in the DD.
     * @throw OperationNotAllowed if head is a terminal node
     * @throw DuplicateElement if another arc linking those nodes already exists
     */
    void eraseArc ( NodeId from, NodeId to );
	
	/// @}
	
	/**
	 * Returns the multidimADD made
	 */
	MultiDimADD<T_DATA>* getMultiDimADD();
		
	/**
	 * Displays the current ADD structures
	 */
	void showProperties();
	
	protected :
		/// Evaluates if an order on variable exists in this diagram
		bool _checkIntegrity();
		
		/// Creates a list in correspondance to variable order
		Sequence< const DiscreteVariable* > _getVariableOrder();
	
	private :
		
		/// The order of variables in the diagram
		Sequence< const DiscreteVariable* > __varsSeq;
		
		/// The algebraic decision diagram model
		DAG __model;
	
		/// Mapping between id and variable
		typename Property< const DiscreteVariable* >::onNodes __varMap;

		/// Mapping between terminal nodes and their values
		typename Property< T_DATA >::onNodes __terminalNodeMap;
    
		/// Mapping between variable's values and associated node
		typename Property< HashTable< Idx, NodeId >* >::onNodes __arcMap;
		
		/// Mapping between variable's values and associated node
		typename Property< NodeId >::onNodes __defaultArcMap;
	
};

} /* namespace gum */

// ============================================================================
#include <agrum/multidim/multiDimADDFactory.tcc>
// ============================================================================
#endif /* GUM_MULTI_DIM_ADD_FACTORY_H */
// ============================================================================

// kate: indent-mode cstyle; space-indent on; indent-width 0;  replace-tabs on;
