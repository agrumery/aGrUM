/***************************************************************************
*   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
* @brief Template implementation of MultiDimADD.h classe.
*
* @author Jean-Christophe Magnan
*/
// ============================================================================
#include <agrum/multidim/multiDimADD.h>
#include <cstdio>
#include <iostream>
#include <sstream>
// ============================================================================

namespace gum {

/* **********************************************************************************************/
/*																								*/
/*                       CONSTRUCTOR, DESTRUCTOR & CO											*/
/*																								*/
/* **********************************************************************************************/

	// ==============================================================================
	// Default constructor
	// ==============================================================================
	template<typename T_DATA> INLINE
	MultiDimADD<T_DATA>::MultiDimADD( Sequence< const DiscreteVariable* > varList, NodeGraphPart model, typename Property< const DiscreteVariable* >::onNodes varMap,
					typename Property< T_DATA >::onNodes terminalNodeMap, typename Property< HashTable< Idx, NodeId >* >::onNodes arcMap, NodeId root ):
						MultiDimReadOnly<T_DATA>(), __graph(model), __variableMap( varMap ), __terminalNodeMap( terminalNodeMap ), __name("ADD") {
		GUM_CONSTRUCTOR( MultiDimADD ) ;
		
		for( Sequence< const DiscreteVariable* >::iterator iter = varList.begin(); iter != varList.end(); ++iter )
			MultiDimImplementation<T_DATA>::add(**iter);
			
		for( Property< HashTable< Idx, NodeId >* >::onNodes::iterator arcIter = arcMap.begin(); arcIter != arcMap.end(); ++arcIter )
			__arcMap.insert(arcIter.key(), new HashTable< Idx, NodeId >(**arcIter) );
			
		__root = root;
	}

	// ==============================================================================
	// destructor
	// ==============================================================================
	template<typename T_DATA> INLINE
	MultiDimADD<T_DATA>::~MultiDimADD() {
		GUM_DESTRUCTOR( MultiDimADD );
		for ( NodeGraphPart::NodeIterator iter = __graph.beginNodes(); iter != __graph.endNodes(); ++iter ) {
		  if ( !__terminalNodeMap.exists(*iter) && __arcMap[*iter] != NULL )
				delete __arcMap[*iter];
		}
	}
	

/* **********************************************************************************************/
/*																								*/
/*                       				MULTIDIM METHODS										*/
/*																								*/
/* **********************************************************************************************/

	/** *************************************************************************************/
	/**								Accessors/Modifiers										*/
	/** *************************************************************************************/
		
		// =============================================================================
		// Returns the real name of this multidim implementation
		// =============================================================================
		template< typename T_DATA >
		const std::string&
		MultiDimADD<T_DATA>::name() const {
			return __name;
		}
	
		// =============================================================================
		// Returns valued pointed by inst
		// =============================================================================
		template< typename T_DATA > INLINE
		T_DATA 
		MultiDimADD<T_DATA>::get( const Instantiation& inst) const {
			T_DATA ret = 0;
			NodeId i = __root;
			
			while( ! _isTerminalNode( i ) )
				i = (*__arcMap[i])[ inst.val( __variableMap[i] ) ];
				
			ret = __terminalNodeMap[i];
			return ret;
		}

	/** *************************************************************************************/
	/**							Implementation of MultiDimInterface							*/
	/** *************************************************************************************/
	
		// =============================================================================
		// Adds a new var to the variables of the multidimensional matrix. 
		// throw OperationNotAllowed cause it's not authorize on read only multidim
		// =============================================================================
		template< typename T_DATA >
		void
		MultiDimADD<T_DATA>::add (const DiscreteVariable &v){
			GUM_ERROR(OperationNotAllowed," Can't add a variable to a read only multidim " );
		}
		
		// =============================================================================
		// Removes a var from the variables of the multidimensional matrix.  
		// @throw OperationNotAllowed cause it's not authorize on read only multidim
		// =============================================================================
		template< typename T_DATA >
		void
		MultiDimADD<T_DATA>::erase (const DiscreteVariable &v){
			GUM_ERROR(OperationNotAllowed," Can't remove a variable to a read only multidim " );
		}
	 
		// ==============================================================================
		// Returns the real number of parameter used in this table. 
		// ==============================================================================
		template< typename T_DATA >
		Size
		MultiDimADD<T_DATA>::realSize () const {
			 return __graph.size();
		 }

	/** *************************************************************************************/
	/**					Slave management and extension due to slave management				*/
    /** *************************************************************************************/
		
		// ===============================================================================
		// Listen to change in a given Instantiation
		// ===============================================================================
		template< typename T_DATA >
		void
		MultiDimADD<T_DATA>::changeNotification(Instantiation& i, const DiscreteVariable* var, const Idx& oldval, const Idx& newval) {
		}
		
		// ===============================================================================
		// Listen to setFirst in a given Instantiation
		// ===============================================================================
		template< typename T_DATA >
		void
		MultiDimADD<T_DATA>::setFirstNotification( Instantiation& i ){
		}
		 
		// ===============================================================================
		// Listen to setLast in a given Instantiation
		// ===============================================================================
		template< typename T_DATA >
		void
		MultiDimADD<T_DATA>::setLastNotification( Instantiation& i ){
		}
		 
		// ===============================================================================
		// Listen to increment in a given Instantiation
		// ===============================================================================
		template< typename T_DATA >
		void
		MultiDimADD<T_DATA>::setIncNotification( Instantiation& i ){
		}
		 
		// ===============================================================================
		// Listen to decrement in a given Instantiation
		// ===============================================================================
		template< typename T_DATA >
		void
		MultiDimADD<T_DATA>::setDecNotification( Instantiation& i ){
		}
		 
		// ===============================================================================
		// Listen to an assignement of value in a given Instantiation
		// ===============================================================================
		template< typename T_DATA >
		void
		MultiDimADD<T_DATA>::setChangeNotification( Instantiation& i ){
		}
		
		
	/** *************************************************************************************/
	/**								Copy Methods											*/
	/** *************************************************************************************/
		 
		// ===============================================================================
		// Removes all variables in this Container and copy content from src, variable included
		// @throw OperationNotAllowed cause this is a read only
		// ===============================================================================
		template< typename T_DATA >
		void
		MultiDimADD<T_DATA>::copy( const MultiDimContainer<T_DATA>& src ) const{
			GUM_ERROR( OperationNotAllowed, "This is a read only");
		}
		
		
	/** *************************************************************************************/
	/**								Various Methods											*/
	/** *************************************************************************************/
		// ===============================================================================
		// Displays the internal representation of i. 
		// ===============================================================================
		template< typename T_DATA >
		const std::string
		MultiDimADD<T_DATA>::toString() const{
			return MultiDimReadOnly<T_DATA>::toString();
		}
		 
		// ===============================================================================
		// Displays the internal representation of i. 
		// ===============================================================================
		template< typename T_DATA >
		const std::string
		MultiDimADD<T_DATA>::toString(const Instantiation* i) const{
			std::stringstream sBuff;
			sBuff << (*i) << " = " << get(*i);
			return sBuff.str();
		}
		
		// ===============================================================================
		// Returns a clone of that object. Or a least should. 
		// @throw OperationNotAllowed without condition.
		// ===============================================================================
		template< typename T_DATA >
		MultiDimContainer<T_DATA>*
		MultiDimADD<T_DATA>::newFactory() const{
			GUM_ERROR(OperationNotAllowed, "A factory exists for that kind of thing. Please fell free to used it");
			return NULL;
		}
		 
	
	/** *************************************************************************************/
	/**								Protected Methods										*/
	/** *************************************************************************************/
	
		// =============================================================================
		// Returns data addressed by inst
		// =============================================================================
		template< typename T_DATA > INLINE
		T_DATA&
		MultiDimADD<T_DATA>::_get( const Instantiation& inst ) const{
			GUM_ERROR(OperationNotAllowed, "a MultiDimADD is a read only MultiDim");
		}

		// =============================================================================
		// Returns true if node is a terminal node
		// =============================================================================
		template< typename T_DATA > INLINE
		bool 
		MultiDimADD<T_DATA>::_isTerminalNode ( NodeId id ) const{
			return ( __terminalNodeMap.exists(id) );
		}
		
		// =============================================================================
		// Returns a constant reference over a variabe given its node id.
		// =============================================================================
		/**template< typename T_DATA > INLINE
		const DiscreteVariable& 
		MultiDimADD<T_DATA>::_nodeVariable ( NodeId id ) const{
			return __variableMap[id];
		}*/
    
		// =============================================================================
		// Supposed to replace var x by y. But not authorized in a MultiDimADD
		// @throw OperationNotAllowed without condition.
		// =============================================================================
		template< typename T_DATA >
		void 
		MultiDimADD<T_DATA>::_swap( const DiscreteVariable* x, const DiscreteVariable* y ){
			 GUM_ERROR(OperationNotAllowed, "Can't be done in a MultiDimADD");
		}
}
