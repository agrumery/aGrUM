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
#include <string>
#include <iostream>
#include <cstdio>
#include <sstream>
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
			
			putOffNoVariableCheckMode();
		
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
		// Returns the sequence of variables on which is based the diagram construction
		// =============================================================================
		template< typename T_DATA > INLINE
		const Sequence< const DiscreteVariable* >&
		MultiDimDecisionDiagramFactoryBase< T_DATA >::variablesSequence(  ){
			
			return _varsSeq;
			
		}
		
		// =============================================================================
		// Specifies the order between variable in the diagram
		// To be done before any insertion of node linked to variable if not in
		// NoVariableCheckMode.
		// @param a sequence containing the variable (wich will be the referent )
		// =============================================================================
		template< typename T_DATA >
		void
		MultiDimDecisionDiagramFactoryBase< T_DATA >::setVariablesSequence( Sequence< const DiscreteVariable* > s ){
			
			_varsSeq = s;
			
		}
			

		/** *********************************************************************************** **/
		/**									Nodes Manipulation									**/
		/** *********************************************************************************** **/
		
			// =============================================================================
			// Adds a non-terminal node in the diagram linked to given variable.
			// @throw OperationNotAllowed if no sequence of variable have yet been specified
			// Returns the id of that node
			// =============================================================================
			template< typename T_DATA >
			NodeId
			MultiDimDecisionDiagramFactoryBase< T_DATA >::addNonTerminalNode( const DiscreteVariable* var ){
			
				// *******************************************************************************************
				// Verification

				if( _noVariableCheckMode ){
					if( !_varsSeq.exists( var ) )
						_varsSeq.insert( var );
						
				} else {
					// First, we check if variable order has been specified
					if( _varsSeq.size() == 0 ){
						GUM_ERROR( OperationNotAllowed, "You must first specify the order of variable");						
					} else {
						// if so we check if var is in the order or not
						if( !_varsSeq.exists( var ) ){
							GUM_ERROR( OperationNotAllowed, "Variable " << var->name() << " is not in the specify order" );
						}
					}
				}
				// *********************************************************************************************
				
				return unsafeAddNonTerminalNode( var );
			}
				
			template< typename T_DATA > INLINE
			NodeId
			MultiDimDecisionDiagramFactoryBase< T_DATA >::unsafeAddNonTerminalNode( const DiscreteVariable* var ){


				// *******************************************************************************************
				// Part were we had the variable's new node
				NodeId node = _model.insertNode();

				// We mention that new node to the list of node bound to that variable
				_varMap.insert( node, var );

				_arcMap.insert( node, new HashTable< Idx, NodeId >() );

				// **********************************************************************************************
				// Addition of the node to the list of tied to given variable

				// If list hasn't be created yet, we create it
				if( !_var2NodeIdMap.exists( var ) )
					_var2NodeIdMap.insert( var, new List<NodeId>() );

				// And finally we add the node to that list
				_var2NodeIdMap[ var ]->insert( node );

				//*************************************************************************************************	

				return node;
			}
				
		
			// =============================================================================
			// Adds a non-terminal node in the diagram linked to given variable.
			// Inserts also arc between that node and mentionned node in the given hashtable
			// @throw OperationNotAllowed if no sequence of variable have yet been specified
			// Returns the id of that node
			// =============================================================================
			template< typename T_DATA > INLINE
			NodeId
			MultiDimDecisionDiagramFactoryBase< T_DATA >::addNonTerminalNodeWithArcs( const DiscreteVariable* var, const HashTable< Idx, NodeId >& nodeArcMap, NodeId* defaultArcTo ){
				
				std::pair<bool, NodeId> check = checkredundancy( var, nodeArcMap, defaultArcTo );
				if( check.first )
					return check.second;
					
				// ***********************************************************************************
				// if we manage to reach this point, this mean we have to insert the node 
				// with all his bunch of arc
				NodeId node = addNonTerminalNode( var );
				
				//~ GUM_TRACE( "insertion noeud pour Var : " << var->toString() << " - Id : " << node << " - Fils : " << nodeArcMap );
				//~ if( defaultArcTo != NULL )
					//~ GUM_TRACE( " - Defaut : " << *defaultArcTo );
				//~ GUM_TRACE( std::endl << std::endl );
				
				for( HashTableConstIterator< Idx, NodeId > iter = nodeArcMap.begin(); iter != nodeArcMap.end(); ++iter )
					insertArc( node, *iter, iter.key() );
					
				if( defaultArcTo != NULL )
					insertDefaultArc( node, *defaultArcTo );
					
				return node;
			}
			
			template< typename T_DATA > INLINE
			NodeId
			MultiDimDecisionDiagramFactoryBase< T_DATA >::unsafeAddNonTerminalNodeWithArcs( const DiscreteVariable* var, const HashTable< Idx, NodeId >& nodeArcMap, NodeId* defaultArcTo ){
				
				std::pair<bool, NodeId> check = checkredundancy( var, nodeArcMap, defaultArcTo );
				if( check.first )
					return check.second;
				
				// ***********************************************************************************
				// if we manage to reach this point, this mean we have to insert the node 
				// with all his bunch of arc
				NodeId node = unsafeAddNonTerminalNode( var );
				
				//~ GUM_TRACE( "insertion noeud pour Var : " << var->toString() << " - Id : " << node << " - Fils : " << nodeArcMap );
				//~ if( defaultArcTo != NULL )
					//~ GUM_TRACE( " - Defaut : " << *defaultArcTo );
				//~ GUM_TRACE( std::endl << std::endl );
				
				for( HashTableConstIterator< Idx, NodeId > iter = nodeArcMap.begin(); iter != nodeArcMap.end(); ++iter )
					unsafeInsertArc( node, *iter, iter.key() );
					
				if( defaultArcTo != NULL )
					unsafeInsertDefaultArc( node, *defaultArcTo );
					
				return node;
			}
			
			template< typename T_DATA > INLINE
			std::pair<bool, NodeId>
			MultiDimDecisionDiagramFactoryBase< T_DATA >::checkredundancy( const DiscreteVariable* var, const HashTable< Idx, NodeId >& nodeArcMap, NodeId* defaultArcTo ){
				
				std::pair<bool, NodeId> res;
				res.first = false;
				//~ GUM_TRACE( "On m'appelle avec : " << var->name() << " - Fils : " << nodeArcMap);
				//~ if( defaultArcTo != NULL ){
					//~ GUM_TRACE( " - Fils par défaut : " << *defaultArcTo );
				//~ }else{
					//~ GUM_TRACE( " - Aucun fils par défaut.");
				//~ }
				//~ GUM_TRACE( std::endl);
				
				
				// ************************************************************************
				// If var adresses exists (means we already insert a node with same var )
				// we check if nodes tied to this var aren't the same as the one we want to insert
				// if so, we'll return that node id
				if( _var2NodeIdMap.exists( var ) ){
					
					for( ListConstIterator< NodeId > iterNodeList = _var2NodeIdMap[ var ]->begin(); iterNodeList != _var2NodeIdMap[ var ]->end(); ++iterNodeList ){
						//~ GUM_TRACE( "\t Noeud observé : " << *iterNodeList);
						bool thesame = true;
						
						if( defaultArcTo == NULL && _defaultArcMap.exists( *iterNodeList ) ){
							thesame = false;
							//~ GUM_TRACE( " -> Inexistant par défaut existant");
						}else{
							if( defaultArcTo != NULL && ( !_defaultArcMap.exists( *iterNodeList ) || *defaultArcTo != _defaultArcMap[ *iterNodeList ] ) ){
								thesame = false;
								//~ GUM_TRACE( " -> Inégaux par défaut");	
							}
						}
						
						if( thesame && nodeArcMap.size() != _arcMap[ *iterNodeList ]->size() )
							thesame = false;
						
						if( thesame )
							for( HashTableConstIterator< Idx, NodeId > iterArcMap = nodeArcMap.begin(); iterArcMap != nodeArcMap.end(); ++iterArcMap ){
								if( !_arcMap[ *iterNodeList ]->exists( iterArcMap.key() )  || (*_arcMap[ *iterNodeList ])[ iterArcMap.key() ] != *iterArcMap ){
									thesame = false;
									//~ GUM_TRACE( " -> Fils différent avec modalité");
									break;
								}
							}
						//~ GUM_TRACE( std::endl);
						if( thesame ){		
							//~ GUM_TRACE( std::endl << std::endl << "Calquage sur noeud pour Var : " << var->toString() << " - Id : " << *iterNodeList << std::endl);
							res.first = true;
							res.second = *iterNodeList;
							return res;
						}
						
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
				
				if( sameArc ){		
					//~ GUM_TRACE( std::endl << std::endl << "Gobage par arc pour Var : " << var->toString() << " - Id : " << nody << std::endl);
					res.first = true;
					res.second = nody;
				}
				return res;
				
			}
		
			// =============================================================================
			// Adds a terminal node in the diagram linked to given value.
			// Returns the id of that node. If a node with such value already exists, it
			// returns that node id.
			// =============================================================================
			template< typename T_DATA > INLINE
			NodeId
			MultiDimDecisionDiagramFactoryBase< T_DATA >::addTerminalNode( const T_DATA& value ){
				
				T_DATA approximate = this->fromExact( value );
				
				if( approximate == (T_DATA) -0 )
					approximate =  (T_DATA) 0;
					
				if( _valueMap.existsSecond( approximate ) )
					return _valueMap.first( approximate );
					
				NodeId node = _model.insertNode();
				_valueMap.insert( node, approximate );
				return node;
			}
		
			// =============================================================================
			// Removes the node corresponding to the given id from the diagram.
			// @throw NotFound if node does not exist
			// =============================================================================
			template< typename T_DATA >
			void
			MultiDimDecisionDiagramFactoryBase< T_DATA >::eraseNode( NodeId n ){
				
				if( !_model.exists( n ) ){
					GUM_ERROR( NotFound, "Node " <<  n << " does not exist in diagram." );
				}

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
			

		/** *********************************************************************************** **/
		/**									Arcs Manipulation									**/
		/** *********************************************************************************** **/
	
			// =============================================================================
			// Adds an arc between two nodes in the graph. The arc will have a value 
			// linked to it.
			// @throw NotFound If from and/or tail are not in the DD.
			// @throw InvalidNode if head is a terminal node
			// @throw OperationNotAllowed arc doesn't respect variable order property
			// @throw DuplicateElement if another arc linking those nodes already exists
			// =============================================================================
			template< typename T_DATA > 
			void
			MultiDimDecisionDiagramFactoryBase< T_DATA >::insertArc( NodeId from, NodeId to, Idx value ){
					
				if( !_model.exists(from) ){
					GUM_ERROR( NotFound, " Origin node " <<  from << " does not exist." );
				}
					
				if( !_model.exists(to) ){
					GUM_ERROR( NotFound, " Destination node " <<  to << " does not exist." );
				}
				
				if( _valueMap.existsFirst( from ) ){
					GUM_ERROR( InvalidNode, " Origin node " <<  from << " is a terminal Node. No arcs can start from a terminal node" );
				}else
					if( !_noVariableCheckMode && !_valueMap.existsFirst( to ) ){
						//~ GUM_TRACE( "From : " << _varMap[from]->toString() << " - To : " << _varMap[ to ]->toString() << std::endl );
						if( _varsSeq.pos( _varMap[ from ] ) >= _varsSeq.pos( _varMap[ to ] ) ){
							GUM_ERROR( OperationNotAllowed, " This arc does not respect the variable order property. Variable " <<  _varMap[from]->name() << " tied to node " << from <<
										" is after Variable " << _varMap[to]->name() << " tied to node " << to << " in variable order." );
						}
					}
					
				for( HashTableIterator<Idx,NodeId> iter = _arcMap[from]->begin(); iter != _arcMap[from]->end(); ++iter )
					if( *iter == to &&  iter.key() == value){
						GUM_ERROR( DuplicateElement, " A same (meaning with same value " <<  value << " ) arc linking those two nodes " << from << " -> " << to << " already exist." );
						break;
					}
						
				unsafeInsertArc( from, to, value );
			}
			
			
			template< typename T_DATA > INLINE
			void
			MultiDimDecisionDiagramFactoryBase< T_DATA >::unsafeInsertArc( NodeId from, NodeId to, Idx value ){
						
				if( _defaultArcMap.exists(from) && _defaultArcMap[from] == to )
					return;
				
				_model.insertArc( from, to );
					
				_arcMap[from]->insert(value, to);
			}

			// =============================================================================
			// Adds a default arc in the DD
			// @param from and
			// @param to as NodeId
			// @throw NotFound If from and/or tail are not in the DD.
			// @throw InvalidNode if head is a terminal node
			// @throw OperationNotAllowed arc doesn't respect variable order property
			// @throw DuplicateElement if another arc linking those nodes already exists
			// =============================================================================
			template< typename T_DATA >
			void
			MultiDimDecisionDiagramFactoryBase< T_DATA >::insertDefaultArc ( NodeId from, NodeId to ){
					
				if( !_model.exists(from) ){
					GUM_ERROR( NotFound, " Origin node " <<  from << " does not exist." );
				}
					
				if( !_model.exists(to) ){
					GUM_ERROR( NotFound, " Destination node " <<  to << " does not exist." );
				}
				
				if( _valueMap.existsFirst( from ) ){
					GUM_ERROR( InvalidNode, " Origin node " <<  from << " is a terminal Node. No arcs can start from a terminal node" );
				} else
					if( !_valueMap.existsFirst( to ) && !_noVariableCheckMode ){
						//~ GUM_TRACE( "Par defaut - From : " << _varMap[from]->toString() << " - To : " << _varMap[ to ]->toString() << std::endl );
						if( _varsSeq.pos( _varMap[ from ] ) >= _varsSeq.pos( _varMap[ to ] ) ){
							GUM_ERROR( OperationNotAllowed, " This arc does not respect the variable order property. Variable " <<  _varMap[from]->name() << " tied to node " << from <<
										" is after Variable " << _varMap[to]->name() << " tied to node " << to << " in variable order." );
						}
					}
				
				
				if( _defaultArcMap.exists( from ) && _defaultArcMap[from] != to )
					GUM_ERROR(DuplicateElement, "A default arc starting from this node " <<  from << " already exist." );
					
				unsafeInsertDefaultArc( from, to );
			}
			
			template< typename T_DATA > INLINE
			void
			MultiDimDecisionDiagramFactoryBase< T_DATA >::unsafeInsertDefaultArc ( NodeId from, NodeId to ){
					
				for( HashTableIterator<Idx,NodeId> iter = _arcMap[from]->begin(); iter != _arcMap[from]->end(); ++iter )
					if( *iter == to ){
						_model.eraseArc( Arc(from, to) );
						_arcMap[from]->erase(iter.key());
					}
				
				if( !_defaultArcMap.exists( from ) ){
					_model.insertArc( from, to );				
					_defaultArcMap.insert(from, to);
				}
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
				
				if( !_model.existsArc( Arc(from, to) ) ){
					GUM_ERROR( InvalidArc, " That arc " <<  from << " - " << to << " does not exist" );
				}
					
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
				
				if( !_model.existsArc( Arc(from, to) ) ){
					GUM_ERROR( InvalidArc, " That arc " <<  from << " - " << to << " does not exist" );
				}
					
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


		// =============================================================================
		// Inserts sub decision diagram at specified node on specified modality
		// It does a copy of given decision diagram.
		// @throw NotFound if node does not yet exist
		// @throw OperationNotAllowed if node var has already this modality used
		// =============================================================================
		template< typename T_DATA >
		NodeId
		MultiDimDecisionDiagramFactoryBase< T_DATA >::insertSubDecisionDiagram( const MultiDimDecisionDiagramBase<T_DATA>* subdd, NodeId parent, Idx parentModality ){		
			
			if( !_arcMap.exists( parent ) ){
				GUM_ERROR( NotFound, "Parent node " << parent << " does not exist in diagram." );
			}
				
			if( _arcMap[ parent ]->exists( parentModality ) ){
				GUM_ERROR( OperationNotAllowed, "Parent node " <<  parent << " already has a son for given modality " <<  parentModality 	);
			}
					
			NodeId implantatedRoot = insertSubDecisionDiagram( subdd );
			insertArc( parent, implantatedRoot, parentModality );
			return implantatedRoot;
		}

		// =============================================================================
		// Inserts sub decision diagram at specified node on default modality
		// It does a copy of given decision diagram.
		// @throw NotFound if node does not yet exist
		// @throw OperationNotAllowed if node var has already this modality used
		// =============================================================================
		template< typename T_DATA >
		NodeId
		MultiDimDecisionDiagramFactoryBase< T_DATA >::insertSubDecisionDiagram( const MultiDimDecisionDiagramBase<T_DATA>* subdd, NodeId parent ){		
			
			if( !_arcMap.exists( parent ) ){
				GUM_ERROR( NotFound, "Parent node " << parent << " does not exist in diagram." );
			}
			
			if( _defaultArcMap.exists( parent ) ){
				GUM_ERROR( OperationNotAllowed, "Parent node " <<  parent << " already has a default son " );
			}
					
			NodeId implantatedRoot = insertSubDecisionDiagram( subdd );
			insertDefaultArc( parent, implantatedRoot );
			return implantatedRoot;
		}

		// =============================================================================
		// Inserts sub decision diagram at specified node on specified modality
		// It does a copy of given decision diagram.
		// @throw NotFound if node does not yet exist
		// @throw OperationNotAllowed if node var has already this modality used
		// =============================================================================
		template< typename T_DATA >
		NodeId
		MultiDimDecisionDiagramFactoryBase< T_DATA >::insertSubDecisionDiagram( const MultiDimDecisionDiagramBase<T_DATA>* subdd ){
			
			HashTable< NodeId, NodeId > nodeWrapper;
			bool modified = true;
			
			while( modified ){
				
				modified = false;
				
				for( NodeGraphPartIterator nodeIter = subdd->nodesMap().beginNodes(); nodeIter != subdd->nodesMap().endNodes(); ++nodeIter ){
					
					if( !nodeWrapper.exists( *nodeIter ) ){
						
						if( subdd->isTerminalNode( *nodeIter ) ){
							
							nodeWrapper.insert( *nodeIter, this->addTerminalNode( subdd->nodeValue( *nodeIter ) ) );
							modified = true;
							
						} else {
							
							HashTable< Idx, NodeId > sonsArcMap;
							for( HashTableConstIterator< Idx, NodeId > sonsIter = subdd->nodeSons( *nodeIter )->begin(); sonsIter != subdd->nodeSons( *nodeIter )->end(); ++sonsIter )
								if( nodeWrapper.exists( *sonsIter ) )
									sonsArcMap.insert( sonsIter.key(), nodeWrapper[ *sonsIter ] );
								else
									break;
									
							if( sonsArcMap.size() == subdd->nodeSons( *nodeIter )->size() ){
								
								NodeId* defaultArcTo = NULL;
								if( subdd->hasNodeDefaultSon( *nodeIter ) ){
									
									if( nodeWrapper.exists( subdd->nodeDefaultSon( *nodeIter ) ) ) {
										
										defaultArcTo = new NodeId();
										*defaultArcTo = nodeWrapper[ subdd->nodeDefaultSon( *nodeIter ) ];
										nodeWrapper.insert( *nodeIter, this->addNonTerminalNodeWithArcs( subdd->nodeVariable( *nodeIter ), sonsArcMap, defaultArcTo ) );
										modified = true;
										
									}
									
								} else {
									
									nodeWrapper.insert( *nodeIter, this->addNonTerminalNodeWithArcs( subdd->nodeVariable( *nodeIter ), sonsArcMap, defaultArcTo ) );
									modified = true;
									
								}
								
							}
							
						}
						
					}
					
				}
				
			}
			
			return nodeWrapper[ subdd->root() ];
			//~ HashTable< NodeId, NodeId > nodeMap;
			//~ NodeId root = subdd->root();
			//~ 
			//~ if( subdd->isTerminalNode( root ) )
				//~ nodeMap.insert( root, addTerminalNode( subdd->nodeValue( root ) ) );
			//~ else {		
				//~ nodeMap.insert( root, addNonTerminalNode( subdd->nodeVariable( root ) ) );
					//~ 
				//~ std::vector<NodeId> fifo;
				//~ fifo.push_back( root );
				//~ 
				//~ while( !fifo.empty() ){
					//~ 
					//~ // ************************************************************************************
					//~ // Recovering current node id and its associated node id in building diagram
					//~ NodeId currentNodeId = fifo.back();
					//~ fifo.pop_back();
					//~ 
					//~ // ************************************************************************************
					//~ // if node is not terminal, we have to insert in new diagram his sons
					//~ // and arcs linking them
					//~ if( !subdd->isTerminalNode( currentNodeId ) ) {
						//~ 
						//~ // ************************************************************************************
						//~ // For each sons
						//~ const HashTable< Idx, NodeId >* sonsMap = subdd->nodeSons( currentNodeId );
						//~ for( HashTableConstIterator< Idx, NodeId > sonsIter = sonsMap->begin(); sonsIter != sonsMap->end(); ++sonsIter ){
							//~ 
							//~ // ************************************************************************************
							//~ // If node is not in nodeMap, it means that we haven't inserted him yet in sub diagram
							//~ if( !nodeMap.exists( *sonsIter ) ){
								//~ 
								//~ if( subdd->isTerminalNode( *sonsIter ) )
									//~ nodeMap.insert( *sonsIter, addTerminalNode( subdd->nodeValue( *sonsIter ) ) );
								//~ else
									//~ nodeMap.insert( *sonsIter, addNonTerminalNode( subdd->nodeVariable( *sonsIter ) ) );
									//~ 
								//~ // And we also have to insert him in our fifo to explore him later
								//~ fifo.push_back( *sonsIter );
							//~ }
							//~ 
							//~ // ************************************************************************************
							//~ // Next we can insert arc in between
							//~ insertArc( nodeMap[ currentNodeId ], nodeMap[ *sonsIter ], sonsIter.key() );
						//~ }
						//~ 
						//~ // ************************************************************************************
						//~ // And if node has a default son, we do quiet the same with it
						//~ if( subdd->hasNodeDefaultSon( currentNodeId ) ){
							//~ 
							//~ if( !nodeMap.exists( subdd->nodeDefaultSon( currentNodeId ) ) ){
								//~ 
								//~ if( subdd->isTerminalNode( subdd->nodeDefaultSon( currentNodeId ) ) )
									//~ nodeMap.insert( subdd->nodeDefaultSon( currentNodeId ), addTerminalNode( subdd->nodeValue(  subdd->nodeDefaultSon( currentNodeId ) ) ) );
								//~ else
									//~ nodeMap.insert( subdd->nodeDefaultSon( currentNodeId ), addNonTerminalNode( subdd->nodeVariable( subdd->nodeDefaultSon( currentNodeId ) ) ) );
									//~ 
								//~ fifo.push_back( subdd->nodeDefaultSon( currentNodeId ) );
							//~ }
							//~ 
							//~ insertDefaultArc( nodeMap[ currentNodeId ], nodeMap[ subdd->nodeDefaultSon( currentNodeId ) ] );						
						//~ }
					//~ }
				//~ }
			//~ }
			//~ 
			//~ return nodeMap[root];
		}
		
		
		template< typename T_DATA >
		void
		MultiDimDecisionDiagramFactoryBase< T_DATA >::showProperties(){
				
				std::stringstream output;
				std::stringstream terminalStream;
				std::stringstream nonTerminalStream;
				std::stringstream arcstream;
				std::stringstream defaultarcstream;
				output << "digraph \"no_name\" {" << std::endl;

				terminalStream << "node [shape = box];" << std::endl;
				nonTerminalStream << "node [shape = ellipse];" << std::endl;
				std::string tab = "  ";

				for ( NodeGraphPart::NodeIterator nodeIter = _model.beginNodes(); nodeIter != _model.endNodes(); ++nodeIter ) {
					if ( _valueMap.existsFirst( *nodeIter ) )
						terminalStream << tab << *nodeIter << ";" << tab << *nodeIter  << " [label=\"" << this->_valueMap.second( *nodeIter ) << "\"]"<< ";" << std::endl;
					else {
						nonTerminalStream << tab << *nodeIter << ";" << tab << *nodeIter  << " [label=\"" << _varMap[ *nodeIter ]->name() << "\"]"<< ";" << std::endl;

						if ( _arcMap[*nodeIter] != NULL )
							for ( HashTableConstIterator<Idx,NodeId> arcIter = _arcMap[*nodeIter]->begin(); arcIter != _arcMap[*nodeIter]->end(); ++arcIter )
								arcstream << tab <<  *nodeIter << " -> " << *arcIter << " [label=\"" << arcIter.key() << "\",color=\"#0000ff\"]"<< ";" << std::endl;

						if ( _defaultArcMap.exists( *nodeIter ) )
							defaultarcstream << tab <<  *nodeIter << " -> " << _defaultArcMap[*nodeIter] << " [color=\"#ff0000\"]"<< ";" << std::endl;
					}
				}

				output << terminalStream.str() << std::endl << nonTerminalStream.str() << std::endl <<  arcstream.str() << std::endl << defaultarcstream.str() << std::endl << std::endl << "}" << std::endl;

				GUM_TRACE( output.str() << std::endl );
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
		
		//===============================================================================================================
		// Puts the factory in a mode where no verification are made on variable
		// ( including arc respecting a variable order )
		// Helpful when diagram is build on stream
		//===============================================================================================================
		template< typename T_DATA > INLINE
		void
		MultiDimDecisionDiagramFactoryBase< T_DATA >::putOnNoVariableCheckMode(){
			_noVariableCheckMode = true;
		}
		
		//===============================================================================================================
		// Puts the factory out of no check on variable mode
		//===============================================================================================================
		template< typename T_DATA > INLINE
		void
		MultiDimDecisionDiagramFactoryBase< T_DATA >::putOffNoVariableCheckMode(){
			_noVariableCheckMode = false;
		}
		
		//===============================================================================================================
		// Finds an order of variable compatible to the diagram
		// ==============================================================================================================
		template< typename T_DATA >
		Sequence< const DiscreteVariable* >
		MultiDimDecisionDiagramFactoryBase< T_DATA >::_findVariableOrder(){
			
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
				for( Sequence< const DiscreteVariable* >::iterator varIter = _varsSeq.begin(); varIter != _varsSeq.end();++varIter){
					
					// ***************************************************************
					// Si elle est déjà dans la liste, on passe
					if( varTopo.exists(*varIter) ){
						continue;
					}
					
					// ***************************************************************
					// Sinon on cherche les noeuds ratachés à cette variable
					bool addVar = true;
					for( ListIterator<NodeId> nodeIter = _var2NodeIdMap[*varIter]->begin(); nodeIter != _var2NodeIdMap[*varIter]->end() ; ++nodeIter ){
							
						const NodeSet& parents = _model.parents( *nodeIter );
						// ***************************************************************
						// Pour chaque noeud lié à cette variable, on voit si la variable
						// associée au noeuds parents est déjà dans la liste
						for ( NodeSetIterator parentIter = parents.begin(); parentIter != parents.end(); ++parentIter ){
							
							// **********************************************************************
							// Si ce n'est pas le cas, cette variable ci ne sera pas ajoutée
							if( !varTopo.exists( _varMap[ *parentIter ] ) ){
								addVar = false;
								break;
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
