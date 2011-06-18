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
* @brief Template implementation of MultiDimDecisionDiagramFactoryBase.h classe.
*
* @author Jean-Christophe Magnan
*/
// ============================================================================
#include <iostream>
// ============================================================================
#include <agrum/multidim/multiDimDecisionDiagramFactoryBase.h>
// ============================================================================
using namespace std;
using namespace gum;

/* **********************************************************************************************/
/*																								*/
/*                         Constructors, Destructors											*/
/*																								*/
/* **********************************************************************************************/

    // =============================================================================
    // Default constructor.
    // =============================================================================
    template< typename T_DATA >
    MultiDimDecisionDiagramFactoryBase< T_DATA >::MultiDimDecisionDiagramFactoryBase(){
		GUM_CONSTRUCTOR( MultiDimDecisionDiagramFactoryBase ) ;
	}

    // =============================================================================
    // Destructor.
    // @warnings : this will not destroy properties on node. They ahve to be removed on multidim destruction
    // =============================================================================
    template< typename T_DATA >
    MultiDimDecisionDiagramFactoryBase< T_DATA >::~MultiDimDecisionDiagramFactoryBase(){
		GUM_DESTRUCTOR( MultiDimDecisionDiagramFactoryBase );
		
		this->clear();
	}

/* **********************************************************************************************/
/*																								*/
/*                        Graph Manipulation methods											*/
/*																								*/
/* **********************************************************************************************/
		
	// =============================================================================
    // To be done before any insertion of node linked to variable.
	// Specifies the order between variable in the diagram
	//
	// @param a sequence containing the variable (wich will be the referent )
    // =============================================================================
    template< typename T_DATA >
	void
	MultiDimDecisionDiagramFactoryBase< T_DATA >::specifyVariablesSequence( Sequence< const DiscreteVariable* > s ){
		_varsSeq = s;
	}
	
	// =============================================================================
    // Adds a non-terminal node in the diagram linked to given variable.
    // @throw OperationNotAllowed if no sequence of variable have yet been specified
    // Returns the id of that node
    // =============================================================================
    template< typename T_DATA >
	NodeId
	MultiDimDecisionDiagramFactoryBase< T_DATA >::addNonTerminalNode( const DiscreteVariable& var ){
		
		// *******************************************************************************************
		// Verification
		
			// First, we check if variable order has been specified
			if( _varsSeq.size() == 0 )
				GUM_ERROR( OperationNotAllowed, "You must first specify the order of variable");
			else{
				
				// if so we check if var is in the order or not
				if( !_varsSeq.exists( &var ) )
					GUM_ERROR( OperationNotAllowed, "Variable is not in the specify order");
			}
		// *********************************************************************************************
		
		
		// *******************************************************************************************
		// Part were we had the variable's new node
			NodeId node = _model.insertNode();
			
			// We mention that new node to the list of node bound to that variable
			_varMap.insert( node, &var );
						
			_arcMap.insert( node, new HashTable< Idx, NodeId >() );
			
		// **********************************************************************************************
		// Addition of the node to the list of tied to given variable
		
			// If list hasn't be created yet, we create it
			if( !_var2NodeIdMap.exists( &var ) )
				_var2NodeIdMap.insert( &var, new List<NodeId>() );
			
			// And finally we add the node to that list
			_var2NodeIdMap[ &var ]->insert( node );

		//*************************************************************************************************	
		
		return node;
	}
	
	
	
	// =============================================================================
    // Adds a non-terminal node in the diagram linked to given variable.
    // Inserts also arc between that node and mentionned node in the given hashtable
    // @throw OperationNotAllowed if no sequence of variable have yet been specified
    // Returns the id of that node
    // =============================================================================
    template< typename T_DATA >
	NodeId
	MultiDimDecisionDiagramFactoryBase< T_DATA >::addNonTerminalNodeWithArcs( const DiscreteVariable* var, const HashTable< Idx, NodeId >& nodeArcMap, NodeId* defaultArcTo ){
		
		// ************************************************************************
		// If var adresses exists (means we already insert a node with same var )
		// we check if nodes tied to this var aren't the same as the one we want to insert
		// if so, we'll return that node id
		if( _var2NodeIdMap.exists( var ) ){
			
			for( ListConstIterator< NodeId > iterNodeList = _var2NodeIdMap[ var ]->begin(); iterNodeList != _var2NodeIdMap[ var ]->end(); ++iterNodeList ){
				
				bool thesame = true;
				
				if( defaultArcTo == NULL && _defaultArcMap.exists( *iterNodeList ) )
						thesame = false;
				else
					if( defaultArcTo != NULL && ( !_defaultArcMap.exists( *iterNodeList ) || *defaultArcTo != _defaultArcMap[ *iterNodeList ] ) )
								thesame = false;
				
				if( _arcMap[*iterNodeList ]->size() != nodeArcMap.size() )
					thesame = false;
				
				if( thesame )
					for( HashTableConstIterator< Idx, NodeId > iterArcMap = _arcMap[ *iterNodeList ]->begin(); iterArcMap != _arcMap[ *iterNodeList ]->end(); ++iterArcMap )
						if( !nodeArcMap.exists( iterArcMap.key() ) || nodeArcMap[ iterArcMap.key() ] != *iterArcMap ){
							thesame = false;
							break;
						}
					
				if( thesame )
					return *iterNodeList;
				
			}
			
		}
		
		// *******************************************************************************
		// Next we do some cleaning on arcs, in case where all arcs point to the same
		// node.
		// If so we return the id of that node.
		bool sameArc = true;
		NodeId nody = 0;
		HashTableConstIterator< Idx, NodeId > arcIter = nodeArcMap.begin();
		
		if( defaultArcTo != NULL )
			nody = *defaultArcTo;
		else
			if( arcIter != nodeArcMap.end() ){
				nody = *arcIter;
				++arcIter;
			}
		
		for(; arcIter != nodeArcMap.end(); ++arcIter )
			if( *arcIter != nody ){
				sameArc = false;
				break;
			}
		
		if( sameArc )
			return nody;
		
		// ***********************************************************************************
		// if we manage to reach this point, this mean we have to insert the node 
		// we his all bunch of arc
		NodeId node = addNonTerminalNode( *var );
		
		for( HashTableConstIterator< Idx, NodeId > iter = nodeArcMap.begin(); iter != nodeArcMap.end(); ++iter )
			insertArc( node, *iter, iter.key() );
			
		if( defaultArcTo != NULL )
			insertDefaultArc( node, *defaultArcTo );
			
		return node;
	}
	
	// =============================================================================
    // Adds a terminal node in the diagram linked to given value.
    // Returns the id of that node. If a node with such value already exists, it
    // returns that node id.
    // =============================================================================
    template< typename T_DATA >
	NodeId
	MultiDimDecisionDiagramFactoryBase< T_DATA >::addTerminalNode( const T_DATA& value ){
		T_DATA approximate = this->fromExact( value );
		if( _valueMap.existsSecond( approximate ) )
			return _valueMap.first( approximate );
		NodeId node = _model.insertNode();
		_valueMap.insert( node, approximate );
		return node;
	}
	
	// =============================================================================
    // Removes the node corresponding to the given id from the diagram.
    // @throw InvalidNode if node does not exist
    // =============================================================================
    template< typename T_DATA >
	void
	MultiDimDecisionDiagramFactoryBase< T_DATA >::eraseNode( NodeId n ){
		
		if( !_model.exists( n ) )
			GUM_ERROR( InvalidNode, " No arcs can start from a terminal node" );

		if( _valueMap.existsFirst( n ) )
			_valueMap.eraseFirst( n );
		else {
			_varMap.erase( n );

			delete _arcMap[n];
			_arcMap.erase(n);
		}
		
		const NodeSet& parents = _model.parents( n );
		for ( NodeSetIterator parentIter = parents.begin(); parentIter != parents.end(); ++parentIter )
			for( HashTableIterator<Idx,NodeId> iter = _arcMap[*parentIter]->begin(); iter != _arcMap[*parentIter]->end(); ++iter )
				if( *iter == n ){
					_arcMap[*parentIter]->erase(iter.key());
					break;
				}
	
		_model.eraseNode( n );
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
	MultiDimDecisionDiagramFactoryBase< T_DATA >::insertArc( NodeId from, NodeId to, Idx value ){
			
		if( !_model.exists(from) || !_model.exists(to) )
			GUM_ERROR( InvalidNode, " One or both of the specified nodes does not exists." );
		
		if( _valueMap.existsFirst( from ) )
			GUM_ERROR( OperationNotAllowed, " No arcs can start from a terminal node" );
		else
			if( !_valueMap.existsFirst( to ) )
				if( _varsSeq.pos( _varMap[ from ] ) >= _varsSeq.pos( _varMap[ to ] ) )
					GUM_ERROR( InvalidArc, " This arc does not respect the variable order property." );
			
		for( HashTableIterator<Idx,NodeId> iter = _arcMap[from]->begin(); iter != _arcMap[from]->end(); ++iter )
			if( *iter == to &&  iter.key() == value)
				GUM_ERROR( DuplicateElement, " A same (meaning with same value) arc linking those two nodes already exists" );
				
		if( _defaultArcMap.exists(from) && _defaultArcMap[from] == to )
			return;
		
		_model.insertArc( from, to );
			
		_arcMap[from]->insert(value, to);
	}


	// =============================================================================
    // Adds a default arc in the DD
    // @param from and
    // @param to as NodeId
    // @throw InvalidNode If from and/or tail are not in the DD.
    // @throw OperationNotAllowed if head is a terminal node
    // =============================================================================
    template< typename T_DATA >
	void
	MultiDimDecisionDiagramFactoryBase< T_DATA >::insertDefaultArc ( NodeId from, NodeId to ){
			
		if( !_model.exists(from) || !_model.exists(to) )
			GUM_ERROR( InvalidNode, " One or both of the specified nodes does not exists." );
		
		if( _valueMap.existsFirst( from ) )
			GUM_ERROR( OperationNotAllowed, " No arcs can start from a terminal node" );
		else{
			if( !_valueMap.existsFirst( to ) )
				if( _varsSeq.pos( _varMap[ from ] ) >= _varsSeq.pos( _varMap[ to ] ) )
					GUM_ERROR( InvalidArc, " This arc does not respect the variable order property." );
		}
			
		for( HashTableIterator<Idx,NodeId> iter = _arcMap[from]->begin(); iter != _arcMap[from]->end(); ++iter )
			if( *iter == to )
				eraseArc( from, to );
		
		if( !_defaultArcMap.exists( from ) ){
			_model.insertArc( from, to );				
			_defaultArcMap.insert(from, to);
		} else
			if( _defaultArcMap[from] != to )
				GUM_ERROR( DuplicateElement, "A default arc starting from this node already exists" );
	}
	
	
	// =============================================================================
    // Adds an arc between two nodes in the graph.
    // @throw InvalidArc If arc does not exist
    // @warning due to the possibility that several arc with different value have the same from and to,
    // if several arcs have different value but same parent and child, this method will erase all of them .
    // If you want to erase a specific one, use eraseArcWithValue
    // =============================================================================
    template< typename T_DATA >
	void
	MultiDimDecisionDiagramFactoryBase< T_DATA >::eraseArc( NodeId from, NodeId to ){
		
		if( !_model.existsArc( Arc(from, to) ) )
			GUM_ERROR( InvalidArc, " That arc does not exist" );
			
		for( HashTableIterator<Idx,NodeId> iter = _arcMap[from]->begin(); iter != _arcMap[from]->end(); ++iter )
				if( *iter == to ){
					_model.eraseArc( Arc(from, to) );
					_arcMap[from]->erase(iter.key());
				}
				
		if( _defaultArcMap.exists( from ) && _defaultArcMap[from] == to ){
			_model.eraseArc( Arc(from, to) );
			_defaultArcMap.erase(from);
		}
	}


    // =============================================================================
    // Erases an arc in the DD
    // @param from and
    // @param to as NodeId
    // @throw InvalidArc If arc does not exist
    // =============================================================================
    template< typename T_DATA >
	void
	MultiDimDecisionDiagramFactoryBase< T_DATA >::eraseArcWithValue ( NodeId from, NodeId to, Idx value ){
		
		if( !_model.existsArc( Arc(from, to) ) )
			GUM_ERROR( InvalidArc, " That arc does not exist" );
			
		if( _arcMap[from]->exists( value ) && (*_arcMap[from] )[ value ] ){	
				bool eraseInGraph = true;
				for( HashTableIterator<Idx,NodeId> iter = _arcMap[from]->begin(); iter != _arcMap[from]->end(); ++iter )	
					if( *iter == to && iter.key() != value ){
						eraseInGraph = false;
						break;
					}
				if(eraseInGraph)
					_model.eraseArc( Arc(from, to) );
				_arcMap[from]->erase( value );
		}
	}

/* **********************************************************************************************/
/*																								*/
/*                        			Various Methods												*/
/*																								*/
/* **********************************************************************************************/
	
	
    template< typename T_DATA >
	void
	MultiDimDecisionDiagramFactoryBase< T_DATA >::showProperties(){
		
		cout << endl << "Liste des variables : " << endl;
		for(Sequence< const DiscreteVariable* >::iterator varIter = _varsSeq.begin(); varIter != _varsSeq.end(); ++varIter)
			cout << **varIter << " ; ";
		cout << endl << endl;
		
		cout << endl << "Liste des associations noeuds - variables : " << endl;
		for(typename Property<const DiscreteVariable* >::onNodes::iterator nodeIter = _varMap.begin(); nodeIter != _varMap.end(); ++nodeIter)
			cout << nodeIter.key() << " : " << **nodeIter << " ; ";
		cout << endl << endl;
		
		cout << endl << "Liste des noeuds terminaux et de leur valeur : " << endl;
		for( BijectionIterator<NodeId,T_DATA> nodeIter = _valueMap.begin(); nodeIter != _valueMap.end(); ++nodeIter )
			cout << nodeIter.first() << " : " << this->_valueMap.second( nodeIter.second() ) << " ; ";
		cout << endl << endl;
		
		cout << endl << "Liste des associations valeur - noeuds : " << endl;
		for( typename Property<HashTable<Idx, NodeId>*>::onNodes::iterator nodeIter = _arcMap.begin(); nodeIter != _arcMap.end() ; ++nodeIter ){
			cout << endl;
			if( _valueMap.existsFirst( nodeIter.key() ) )
				continue;
			cout << "	Pour le noeud : " << nodeIter.key();
			for( HashTableIterator<Idx,NodeId> hashIter = (*nodeIter)->begin(); hashIter != (*nodeIter)->end(); ++hashIter )
				cout << "		" << hashIter.key() << " : " << *hashIter << " ; ";
		}
		cout << endl << endl;
		
		cout << endl << "Liste des association noeuds - noeuds par défaut : " << endl;
		for( typename Property<NodeId>::onNodes::iterator nodeIter = _defaultArcMap.begin(); nodeIter != _defaultArcMap.end(); ++nodeIter )
			cout << nodeIter.key() << " : " << *nodeIter << " ; ";
		cout << endl << endl;
		
		cout << endl << endl;
	}
	
	//===============================================================================================================
	// Resets the factory
	//===============================================================================================================
	
	
    template< typename T_DATA >
	void
	MultiDimDecisionDiagramFactoryBase< T_DATA >::clear() {
	
		_varsSeq.clear();
	
		_varMap.clear();
    
		_defaultArcMap.clear();
		
		for ( NodeGraphPart::NodeIterator iter = _model.beginNodes(); iter != _model.endNodes(); ++iter )
			if ( !_valueMap.existsFirst(*iter) && _arcMap[*iter] != NULL )
				delete _arcMap[*iter];
		_arcMap.clear();

		_valueMap.clear();
		
		_model.clear();
		
		for( HashTableIterator< const DiscreteVariable*, List<NodeId>* > iter = _var2NodeIdMap.begin(); iter != _var2NodeIdMap.end(); ++iter )
			delete *iter;
		_var2NodeIdMap.clear();
	}
