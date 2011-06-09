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
* @brief Template implementation of MultiDimDecisionDiagramBase.h classe.
*
* @author Jean-Christophe Magnan
*/
// ============================================================================
#include <agrum/multidim/multiDimDecisionDiagramBase.h>
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
	template< typename T_DATA > INLINE
	MultiDimDecisionDiagramBase< T_DATA >::MultiDimDecisionDiagramBase( ):
							MultiDimReadOnly<T_DATA>(), __name("MultiDimDecisionDiagram"), __isInstanciated(false) {
			GUM_CONSTRUCTOR( MultiDimDecisionDiagramBase ) ;
	}

	// ==============================================================================
	// destructor
	// ==============================================================================
	template< typename T_DATA > INLINE
	MultiDimDecisionDiagramBase< T_DATA >::~MultiDimDecisionDiagramBase() {
		GUM_DESTRUCTOR( MultiDimDecisionDiagramBase );
		for ( NodeGraphPart::NodeIterator iter = __graph.beginNodes(); iter != __graph.endNodes(); ++iter ) {
		  if ( !__valueMap.existsFirst(*iter) && __arcMap[*iter] != NULL )
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
		MultiDimDecisionDiagramBase< T_DATA >::name() const {
			return __name;
		}
	
		// =============================================================================
		// Returns valued pointed by inst
		// =============================================================================
		template< typename T_DATA > INLINE
		T_DATA 
		MultiDimDecisionDiagramBase< T_DATA >::get( const Instantiation& inst) const {
			T_DATA ret = 0;
			NodeId i = __root;
			
			while( ! isTerminalNode( i ) ){
				if( __arcMap[i]->exists( inst.val( __variableMap[i] )) )
					i = (*__arcMap[i])[ inst.val( __variableMap[i] ) ];
				else
					i = __defaultArcMap[i];
			}
				
			ret = this->toExact( this->__valueMap.second( i ) );;
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
		MultiDimDecisionDiagramBase< T_DATA >::add (const DiscreteVariable &v){
			GUM_ERROR(OperationNotAllowed," Can't add a variable to a read only multidim " );
		}
		
		// =============================================================================
		// Removes a var from the variables of the multidimensional matrix.  
		// @throw OperationNotAllowed cause it's not authorize on read only multidim
		// =============================================================================
		template< typename T_DATA >
		void
		MultiDimDecisionDiagramBase< T_DATA >::erase (const DiscreteVariable &v){
			GUM_ERROR(OperationNotAllowed," Can't remove a variable to a read only multidim " );
		}
	 
		// ==============================================================================
		// Returns the real number of parameter used in this table. 
		// ==============================================================================
		template< typename T_DATA >
		Size
		MultiDimDecisionDiagramBase< T_DATA >::realSize () const {
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
		MultiDimDecisionDiagramBase< T_DATA >::changeNotification(Instantiation& i, const DiscreteVariable* var, const Idx& oldval, const Idx& newval) {
		}
		
		// ===============================================================================
		// Listen to setFirst in a given Instantiation
		// ===============================================================================
		template< typename T_DATA >
		void
		MultiDimDecisionDiagramBase< T_DATA >::setFirstNotification( Instantiation& i ){
		}
		 
		// ===============================================================================
		// Listen to setLast in a given Instantiation
		// ===============================================================================
		template< typename T_DATA >
		void
		MultiDimDecisionDiagramBase< T_DATA >::setLastNotification( Instantiation& i ){
		}
		 
		// ===============================================================================
		// Listen to increment in a given Instantiation
		// ===============================================================================
		template< typename T_DATA >
		void
		MultiDimDecisionDiagramBase< T_DATA >::setIncNotification( Instantiation& i ){
		}
		 
		// ===============================================================================
		// Listen to decrement in a given Instantiation
		// ===============================================================================
		template< typename T_DATA >
		void
		MultiDimDecisionDiagramBase< T_DATA >::setDecNotification( Instantiation& i ){
		}
		 
		// ===============================================================================
		// Listen to an assignement of value in a given Instantiation
		// ===============================================================================
		template< typename T_DATA >
		void
		MultiDimDecisionDiagramBase< T_DATA >::setChangeNotification( Instantiation& i ){
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
		MultiDimDecisionDiagramBase< T_DATA >::copy( const MultiDimContainer<T_DATA>& src ) const{
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
		MultiDimDecisionDiagramBase< T_DATA >::toString() const{
			return MultiDimReadOnly<T_DATA>::toString();
		}
		 
		// ===============================================================================
		// Displays the internal representation of i. 
		// ===============================================================================
		template< typename T_DATA >
		const std::string
		MultiDimDecisionDiagramBase< T_DATA >::toString(const Instantiation* i) const{
			std::stringstream sBuff;
			sBuff << (*i) << " = " << get(*i);
			return sBuff.str();
		}
	
		// ===============================================================================
		// Displays the DecisionDiagramBase in the dot format
		// ===============================================================================
		template< typename T_DATA >
		std::string
		MultiDimDecisionDiagramBase< T_DATA >::toDot() const{
			std::stringstream output;
			std::stringstream terminalStream;
			std::stringstream nonTerminalStream;
			std::stringstream arcstream;
			std::stringstream defaultarcstream;
			output << "digraph \"no_name\" {" << std::endl;

			terminalStream << "node [shape = box];" << std::endl;
			nonTerminalStream << "node [shape = ellipse];" << std::endl;
			std::string tab = "  ";

			for ( NodeGraphPart::NodeIterator nodeIter = __graph.beginNodes(); nodeIter != __graph.endNodes(); ++nodeIter ) {
				if ( isTerminalNode( *nodeIter ) )
					terminalStream << tab << *nodeIter << ";" << tab << *nodeIter  << " [label=\"" << this->toExact( this->__valueMap.second( *nodeIter ) ) << "\"]"<< ";" << std::endl;
				else {
					nonTerminalStream << tab << *nodeIter << ";" << tab << *nodeIter  << " [label=\"" << __variableMap[ *nodeIter ]->name() << "\"]"<< ";" << std::endl;

					if( __arcMap[*nodeIter] != NULL )
						for( HashTableConstIterator<Idx,NodeId> arcIter = __arcMap[*nodeIter]->begin(); arcIter != __arcMap[*nodeIter]->end(); ++arcIter )
							arcstream << tab <<  *nodeIter << " -> " << *arcIter << " [label=\"" << arcIter.key() << "\",color=\"#0000ff\"]"<< ";" << std::endl;

					if( __defaultArcMap.exists(*nodeIter) )
						defaultarcstream << tab <<  *nodeIter << " -> " << __defaultArcMap[*nodeIter] << " [color=\"#ff0000\"]"<< ";" << std::endl;
				}
			}

			output << terminalStream.str() << std::endl << nonTerminalStream.str() << std::endl <<  arcstream.str() << std::endl << defaultarcstream.str() << std::endl << std::endl << "}" << std::endl;

			return output.str();
		}
		
		// ==============================================================================
		// Returns the number of variable trully present in the diagram
		// ==============================================================================
		template< typename T_DATA > INLINE
		Size 
		MultiDimDecisionDiagramBase< T_DATA >::diagramVarSize ( ) const{		
		
			Sequence< const DiscreteVariable* > varTopo = this->variablesSequence();
			for( SequenceIterator< const DiscreteVariable* > ite1 = varTopo.begin(); ite1 != varTopo.end(); ){
				bool isin = false;
				HashTableConstIterator< NodeId, const DiscreteVariable* > ite2 = __variableMap.begin();
				while(  ite2 != __variableMap.end() ){
					if( **ite1 == **ite2 ){
						isin = true;
						break;
					}
					++ite2;
				}
				if(isin)
					++ite1;
				else
					varTopo.erase(*ite1);
			}
			
			return varTopo.size();
		}

    /** *************************************************************************************/
    /**								Operators Functions										*/
	/** *************************************************************************************/

		// =============================================================================
		// Returns true if node is a terminal node
		// =============================================================================
		template< typename T_DATA > INLINE
		const NodeId
		MultiDimDecisionDiagramBase< T_DATA >::getRoot (  ) const{
			return __root ;
		}
		
		// =============================================================================
		// Returns associated variable of given node
		// @throw InvalidNode if Node is terminal
		// =============================================================================
		template< typename T_DATA > INLINE
		const DiscreteVariable&
		MultiDimDecisionDiagramBase< T_DATA >::getVariableFromNode( NodeId n ) const{
			if( isTerminalNode( n ) )
				GUM_ERROR( InvalidNode, " This is a terminal node " );
			
			return *__variableMap[ n ];
		}
		
		// =============================================================================
		// Returns value associated to given node
		// @throw InvalidNode if node isn't terminal
		// =============================================================================
		template< typename T_DATA > INLINE
		const T_DATA
		MultiDimDecisionDiagramBase< T_DATA >::getValueFromNode( NodeId n ) const{
			if( !isTerminalNode( n ) )
				GUM_ERROR( InvalidNode, " Not a terminal node " );
			
			return this->toExact( this->__valueMap.second( n ) );
		}
		
		// =============================================================================
		// Returns node's sons map
		// @throw InvalidNode if node is terminal
		// =============================================================================
		template< typename T_DATA > INLINE
		const HashTable< Idx, NodeId >*
		MultiDimDecisionDiagramBase< T_DATA >::getNodeSons( NodeId n ) const{
			if( isTerminalNode( n ) )
				GUM_ERROR( InvalidNode, " This is a terminal node " );
			
			if( !__arcMap.exists( n ) )
				return NULL;
			return __arcMap[ n ];
		}
		
		// =============================================================================
		// Returns true if node has a default son
		// =============================================================================
		template< typename T_DATA > INLINE
		bool
		MultiDimDecisionDiagramBase< T_DATA >::hasNodeDefaultSon( NodeId n ) const{
			
			return __defaultArcMap.exists( n );
		}
		
		// =============================================================================
		// Returns node's default son
		// @throw InvalidNode if node is terminal
		// =============================================================================
		template< typename T_DATA > INLINE
		const NodeId
		MultiDimDecisionDiagramBase< T_DATA >::getNodeDefaultSon( NodeId n ) const{
			if( isTerminalNode( n ) )
				GUM_ERROR( InvalidNode, " This is a terminal node " );
			
			return __defaultArcMap[ n ];
		}

		// =============================================================================
		// Returns true if node is a terminal node
		// =============================================================================
		template< typename T_DATA > INLINE
		bool 
		MultiDimDecisionDiagramBase< T_DATA >::isTerminalNode ( NodeId id ) const{
			return ( __valueMap.existsFirst(id) );
		}
		 
	
	/** *************************************************************************************/
	/**						Fast Large modifications in structure							*/
	/** *************************************************************************************/		

		// ==============================================================================
		// Sets once and for all variable sequence.
		// @throw OperationNotAllowed if function as already been call or if not in multiplechange mode
		// ==============================================================================
		template< typename T_DATA >
		void
		MultiDimDecisionDiagramBase< T_DATA >::setVariableSequence(  const Sequence< const DiscreteVariable* >& varList ){
			
			if( __isInstanciated )
				GUM_ERROR( OperationNotAllowed, "Cannot operates modification a multidimdecisiondiagram once it has been created" );
				
			if( !this->_isInMultipleChangeMethod() )
				GUM_ERROR( OperationNotAllowed, "Must first be in multiple change mode to do such thing" );

			for( Sequence< const DiscreteVariable* >::iterator iter = varList.begin(); iter != varList.end(); ++iter )
				MultiDimImplementation<T_DATA>::add(**iter);
		}

		// ==============================================================================
		// Sets once and for all nodes of the diagram.
		// @throw OperationNotAllowed if function as already been call;
		// ==============================================================================
		template< typename T_DATA >
		void
		MultiDimDecisionDiagramBase< T_DATA >::setDiagramNodes( const NodeGraphPart& model ){
			
			if( __isInstanciated )
				GUM_ERROR( OperationNotAllowed, "Cannot operates modification a multidimdecisiondiagram once it has been created" );
				
			if( !this->_isInMultipleChangeMethod() )
				GUM_ERROR( OperationNotAllowed, "Must first be in multiple change mode to do such thing" );
				
			__graph = model;
		}

		// ==============================================================================
		// Binds once and for all nodes to variables.
		// @throw OperationNotAllowed if function as already been call;
		// ==============================================================================
		template< typename T_DATA >
		void
		MultiDimDecisionDiagramBase< T_DATA >::setVariableMap( const typename Property< const DiscreteVariable* >::onNodes& varMap ){
			
			if( __isInstanciated )
				GUM_ERROR( OperationNotAllowed, "Cannot operates modification a multidimdecisiondiagram once it has been created" );
				
			if( !this->_isInMultipleChangeMethod() )
				GUM_ERROR( OperationNotAllowed, "Must first be in multiple change mode to do such thing" );
				
			__variableMap = varMap;
		}

		// ==============================================================================
		// Binds once and for all terminal nodes to value.
		// @throw OperationNotAllowed if function as already been call;
		// ==============================================================================
		template< typename T_DATA >
		void
		MultiDimDecisionDiagramBase< T_DATA >::setValueMap( const Bijection< NodeId, T_DATA >& valueMap ){
			
			if( __isInstanciated )
				GUM_ERROR( OperationNotAllowed, "Cannot operates modification a multidimdecisiondiagram once it has been created" );
				
			if( !this->_isInMultipleChangeMethod() )
				GUM_ERROR( OperationNotAllowed, "Must first be in multiple change mode to do such thing" );
				
			__valueMap = valueMap;
		}

		// ==============================================================================
		// Links once and for all nodes of the graph.
		// @throw OperationNotAllowed if function as already been call;
		// ==============================================================================
		template< typename T_DATA >
		void
		MultiDimDecisionDiagramBase< T_DATA >::setDiagramArcs( const typename Property< HashTable< Idx, NodeId >* >::onNodes& arcMap, const typename Property< NodeId >::onNodes& defaultArcMap ){
			
			if( __isInstanciated )
				GUM_ERROR( OperationNotAllowed, "Cannot operates modification a multidimdecisiondiagram once it has been created" );
				
			if( !this->_isInMultipleChangeMethod() )
				GUM_ERROR( OperationNotAllowed, "Must first be in multiple change mode to do such thing" );
				
			__defaultArcMap = defaultArcMap;
				
			for( HashTableConstIterator< NodeId, HashTable< Idx, NodeId >* > arcIter = arcMap.begin(); arcIter != arcMap.end(); ++arcIter )
				__arcMap.insert(arcIter.key(), new HashTable< Idx, NodeId >(**arcIter) );
		}

		// ==============================================================================
		// Sets once and for all root node.
		// @throw OperationNotAllowed if function as already been call;
		// ==============================================================================
		template< typename T_DATA >
		void
		MultiDimDecisionDiagramBase< T_DATA >::setRoot( const NodeId& root ){
			
			if( __isInstanciated )
				GUM_ERROR( OperationNotAllowed, "Cannot operates modification a multidimdecisiondiagram once it has been created" );
				
			if( !this->_isInMultipleChangeMethod() )
				GUM_ERROR( OperationNotAllowed, "Must first be in multiple change mode to do such thing" );
				
			__root = root;
		}

		// ==============================================================================
		// Synchronize content after MultipleChanges.
		// ==============================================================================
		template< typename T_DATA >
		void
		MultiDimDecisionDiagramBase< T_DATA >::_commitMultipleChanges(){
		
			__isInstanciated = true;
			
		}
		 
	
	/** *************************************************************************************/
	/**								Protected Methods										*/
	/** *************************************************************************************/
	
		// =============================================================================
		// Returns data addressed by inst
		// =============================================================================
		template< typename T_DATA > INLINE
		T_DATA&
		MultiDimDecisionDiagramBase< T_DATA >::_get( const Instantiation& inst ) const{
			GUM_ERROR(OperationNotAllowed, "a MultiDimDecisionDiagram is a read only MultiDim");
		}
    
		// =============================================================================
		// Supposed to replace var x by y. But not authorized in a MultiDimDecisionDiagramBase
		// @throw OperationNotAllowed without condition.
		// =============================================================================
		template< typename T_DATA >
		void 
		MultiDimDecisionDiagramBase< T_DATA >::_swap( const DiscreteVariable* x, const DiscreteVariable* y ){
			 GUM_ERROR(OperationNotAllowed, "Can't be done in a MultiDimDecisionDiagram");
		}
}
