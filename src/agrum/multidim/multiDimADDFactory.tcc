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
* @brief Template implementation of MultiDimADDFactory.h classe.
*
* @author Jean-Christophe Magnan
*/
// ============================================================================
#include <agrum/multidim/multiDimADDFactory.h>
#include <iostream>
// ============================================================================
using namespace std;
namespace gum {

/* **********************************************************************************************/
/*																								*/
/*                         Constructors, Destructors											*/
/*																								*/
/* **********************************************************************************************/

    // =============================================================================
    // Default constructor.
    // =============================================================================
    template< typename T_DATA >
    MultiDimADDFactory< T_DATA >::MultiDimADDFactory(){
		GUM_CONSTRUCTOR( MultiDimADDFactory ) ;
	}

    // =============================================================================
    // Destructor.
    // @warnings : this will not destroy properties on node. They ahve to be removed on multidim destruction
    // =============================================================================
    template< typename T_DATA >
    MultiDimADDFactory< T_DATA >::~MultiDimADDFactory(){
		GUM_DESTRUCTOR( MultiDimADDFactory );
		for ( NodeGraphPart::NodeIterator iter = __model.beginNodes(); iter != __model.endNodes(); ++iter ) {
		  if ( !__terminalNodeMap.exists(*iter) && __arcMap[*iter] != NULL )
				delete __arcMap[*iter];
		}
	}

/* **********************************************************************************************/
/*																								*/
/*                        Graph Manipulation methods											*/
/*																								*/
/* **********************************************************************************************/
	
	// =============================================================================
    // Adds a non-terminal node in the diagram linked to given variable.
    // Returns the id of that node
    // =============================================================================
    template< typename T_DATA >
	NodeId
	MultiDimADDFactory< T_DATA >::addNonTerminalNode( const DiscreteVariable& var ){
		NodeId node = __model.insertNode();
		__varMap.insert( node, &var );
		__arcMap.insert( node, new HashTable< Idx, NodeId >() );
		if( !__varsSeq.exists( &var ) )
			__varsSeq.insert( &var );
		return node;
	}
	
	// =============================================================================
    // Adds a terminal node in the diagram linked to given value.
    // Returns the id of that node. If a node with such value already exists, it
    // returns that node id.
    // =============================================================================
    template< typename T_DATA >
	NodeId
	MultiDimADDFactory< T_DATA >::addTerminalNode( const T_DATA& value ){
		for( typename Property<T_DATA>::onNodes::iterator iter = __terminalNodeMap.begin(); iter != __terminalNodeMap.end(); ++iter )
			if( *iter == value )
				return iter.key();
		NodeId node = __model.insertNode();
		__terminalNodeMap.insert( node, value );
		return node;
	}
	
	// =============================================================================
    // Removes the node corresponding to the given id from the diagram.
    // @throw InvalidNode if node does not exist
    // =============================================================================
    template< typename T_DATA >
	void
	MultiDimADDFactory< T_DATA >::eraseNode( NodeId n ){
		
		if( !__model.exists( n ) )
			GUM_ERROR( InvalidNode, " No arcs can start from a terminal node" );

		if( __terminalNodeMap.exists( n ) )
			__terminalNodeMap.erase( n );
		else {
			__varMap.erase( n );

			delete __arcMap[n];
			__arcMap.erase(n);
		}
		
		const NodeSet& parents = __model.parents( n );
		for ( NodeSetIterator parentIter = parents.begin(); parentIter != parents.end(); ++parentIter )
			for( HashTableIterator<Idx,NodeId> iter = __arcMap[*parentIter]->begin(); iter != __arcMap[*parentIter]->end(); ++iter )
				if( *iter == n ){
					__arcMap[*parentIter]->erase(iter.key());
					break;
				}
	
		__model.eraseNode( n );
	}
	
	// =============================================================================
    // Adds an arc between two nodes in the graph. The arc will have a value 
    // linked to it.
    // @throw InvalidNode If from and/or tail are not in the DD.
    // @throw OperationNotAllowed if head is a terminal node
    // @throw DuplicateElement if another arc linking those nodes already exists
    // =============================================================================
    template< typename T_DATA >
	void
	MultiDimADDFactory< T_DATA >::insertArc( NodeId from, NodeId to, Idx value ){
		
		if( __terminalNodeMap.exists( from ) )
			GUM_ERROR( OperationNotAllowed, " No arcs can start from a terminal node" );
			
		if( !__model.exists(from) || !__model.exists(to) )
			GUM_ERROR( InvalidNode, " One or both of the specified nodes does not exists." );
			
		for( HashTableIterator<Idx,NodeId> iter = __arcMap[from]->begin(); iter != __arcMap[from]->end(); ++iter )
			if( *iter == to )
				GUM_ERROR( DuplicateElement, " An arc linking those two nodes already exists" );
		
		__model.insertArc( from, to );
		if( !_checkIntegrity() ){
			__model.eraseArc( Arc( from, to ) );
			GUM_ERROR( InvalidArc, " This arc does not respect the variable order property." );
		}
			
		__arcMap[from]->insert(value, to);
	}


	// =============================================================================
    // Adds a default arc in the DD
    // @param from and
    // @param to as NodeId
    // @throw InvalidNode If from and/or tail are not in the DD.
    // @throw OperationNotAllowed if head is a terminal node
    // @throw DuplicateElement if another arc linking those nodes already exists
    // =============================================================================
    template< typename T_DATA >
	void
	MultiDimADDFactory< T_DATA >::insertDefaultArc ( NodeId from, NodeId to ){
		
		if( __terminalNodeMap.exists( from ) )
			GUM_ERROR( OperationNotAllowed, " No arcs can start from a terminal node" );
			
		if( !__model.exists(from) || !__model.exists(to) )
			GUM_ERROR( InvalidNode, " One or both of the specified nodes does not exists." );
			
		for( HashTableIterator<Idx,NodeId> iter = __arcMap[from]->begin(); iter != __arcMap[from]->end(); ++iter )
			if( *iter == to )
				GUM_ERROR( DuplicateElement, " An arc linking those two nodes already exists" );
		
		__model.insertArc( from, to );
		if( !_checkIntegrity() ){
			__model.eraseArc( Arc( from, to ) );
			GUM_ERROR( InvalidArc, " This arc does not respect the variable order property." );
		}
			
		__defaultArcMap.insert(from, to);
	}
	
	
	// =============================================================================
    // Adds an arc between two nodes in the graph.
    // @throw InvalidArc If arc does not exist
    // =============================================================================
    template< typename T_DATA >
	void
	MultiDimADDFactory< T_DATA >::eraseArc( NodeId from, NodeId to ){
		
		if( !__model.existsArc( Arc(from, to) ) )
			GUM_ERROR( InvalidArc, " That arc does not exist" );
			
		__model.eraseArc( Arc(from, to) );
		bool defaultArc = true;
		for( HashTableIterator<Idx,NodeId> iter = __arcMap[from]->begin(); iter != __arcMap[from]->end(); ++iter )
				if( *iter == to ){
					__arcMap[from]->erase(iter.key());
					defaultArc = false;
					break;
				}
		if( defaultArc )
			__defaultArcMap.erase(from);
	}
	
	// =============================================================================
    // Returns the multidimADD made
    // =============================================================================
    template< typename T_DATA >
	MultiDimADD<T_DATA>* 
	MultiDimADDFactory< T_DATA >::getMultiDimADD(){
		Sequence< const DiscreteVariable* > varTopo = _getVariableOrder();
		
		NodeId root = 0;
		for ( DAG::NodeIterator iter = __model.beginNodes(); iter != __model.endNodes(); ++iter ) {
			if ( !__terminalNodeMap.exists(*iter) && !__model.children(*iter).empty() && __model.parents(*iter).empty() ){
					root = *iter;
					break;
				}
		}
		
		for ( DAG::NodeIterator iter = __model.beginNodes(); iter != __model.endNodes(); ++iter ) {
			if ( !__terminalNodeMap.exists(*iter) && !__defaultArcMap.exists(*iter) )
				__defaultArcMap.insert( *iter, __terminalNodeMap.begin().key() );
		}
			
		return new MultiDimADD< T_DATA >( varTopo, __model, __varMap, __terminalNodeMap, __arcMap, __defaultArcMap, root );
	}

/* **********************************************************************************************/
/*																								*/
/*                        			Protected Methods											*/
/*																								*/
/* **********************************************************************************************/
    
    // =============================================================================
    // Checks if diagram respects order in variable
    // =============================================================================
    template< typename T_DATA >
	bool
	MultiDimADDFactory< T_DATA >::_checkIntegrity(){
		Sequence< const DiscreteVariable* > varList = __varsSeq;
		Sequence< const DiscreteVariable* > varTopo = _getVariableOrder();
		
		for(Sequence< const DiscreteVariable* >::iterator varIter = varTopo.begin(); varIter != varTopo.end();++varIter)
			varList.erase(*varIter);
		return varList.empty();
	}
		
	// =============================================================================
    // Returns a topological order on var, or at least try to do so
    // =============================================================================
    template< typename T_DATA >
	Sequence< const DiscreteVariable* >
	MultiDimADDFactory< T_DATA >::_getVariableOrder(){
		
		Sequence< const DiscreteVariable* > varTopo;
		bool modified = true;
		
		// ****************************************************************
		// On itère jusqu'à ce qu'il n'y ait plus de modification
		// (signe que toutes les variables sont dans la liste ou qu'un
		// problème existe)
		while( modified ){
			modified = false;
			
			// ***************************************************************
			// Pour chaque variable
			for( Sequence< const DiscreteVariable* >::iterator varIter = __varsSeq.begin(); varIter != __varsSeq.end();++varIter){
				
				// ***************************************************************
				// Si elle est déjà dans la liste, on passe
				if( varTopo.exists(*varIter) ){
					continue;
				}
				
				// ***************************************************************
				// Sinon on cherche les noeuds ratachés à cette variable
				bool addVar = true;
				for( DAG::NodeIterator nodeIter = __model.beginNodes(); nodeIter != __model.endNodes() ; ++nodeIter ){
					
					// ***************************************************************
					// Dans ce cadre les noeuds terminaux ne nous intéressent pas
					if( __terminalNodeMap.exists( *nodeIter ) ){
						continue;
					}
					
					// ***************************************************************
					// Pour les noeuds ratachés
					if( __varMap[*nodeIter] == *varIter ){
						
						const NodeSet& parents = __model.parents( *nodeIter );
						// ***************************************************************
						// Pour chaque noeud lié à cette variable, on voit si la variable
						// associée au noeuds parents est déjà dans la liste
						for ( NodeSetIterator parentIter = parents.begin(); parentIter != parents.end(); ++parentIter ){
							
							// **********************************************************************
							// Si ce n'est pas le cas, cette variable ci ne sera pas ajoutée
							if( !varTopo.exists( __varMap[ *parentIter ] ) ){
								addVar = false;
								break;
							}
						}
					}
					if( !addVar )
						break;
				}
				if( addVar ){
					varTopo.insert( *varIter );
					modified = true;
				}
			}
		}
		return varTopo;
	}
	
	
    template< typename T_DATA >
	void
	MultiDimADDFactory< T_DATA >::showProperties(){
		
		cout << endl << "Liste des variables : " << endl;
		for(Sequence< const DiscreteVariable* >::iterator varIter = __varsSeq.begin(); varIter != __varsSeq.end(); ++varIter)
			cout << **varIter << " ; ";
		cout << endl << endl;
		
		cout << endl << "Liste des associations noeuds - variables : " << endl;
		for(typename Property<const DiscreteVariable* >::onNodes::iterator nodeIter = __varMap.begin(); nodeIter != __varMap.end(); ++nodeIter)
			cout << nodeIter.key() << " : " << **nodeIter << " ; ";
		cout << endl << endl;
		
		cout << endl << "Liste des noeuds terminaux et de leur valeur : " << endl;
		for( typename Property<T_DATA>::onNodes::iterator nodeIter = __terminalNodeMap.begin(); nodeIter != __terminalNodeMap.end(); ++nodeIter )
			cout << nodeIter.key() << " : " << *nodeIter << " ; ";
		cout << endl << endl;
		
		cout << endl << "Liste des associations valeur - noeuds : " << endl;
		for( typename Property<HashTable<Idx, NodeId>*>::onNodes::iterator nodeIter = __arcMap.begin(); nodeIter != __arcMap.end() ; ++nodeIter ){
			cout << endl;
			if( __terminalNodeMap.exists( nodeIter.key() ) )
				continue;
			cout << "	Pour le noeud : " << nodeIter.key();
			for( HashTableIterator<Idx,NodeId> hashIter = (*nodeIter)->begin(); hashIter != (*nodeIter)->end(); ++hashIter )
				cout << "		" << hashIter.key() << " : " << *hashIter << " ; ";
		}
		cout << endl << endl;
		
		cout << endl << "Liste des association noeuds - noeuds par défaut : " << endl;
		for( typename Property<NodeId>::onNodes::iterator nodeIter = __defaultArcMap.begin(); nodeIter != __defaultArcMap.end(); ++nodeIter )
			cout << nodeIter.key() << " : " << *nodeIter << " ; ";
		cout << endl << endl;
		
		cout << endl << endl;
	}
}
