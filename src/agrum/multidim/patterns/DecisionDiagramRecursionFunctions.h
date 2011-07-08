template<typename T>
NodeId
GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( 	const MultiDimDecisionDiagramBase<T>* leader,
												const MultiDimDecisionDiagramBase<T>* follower,
												MultiDimDecisionDiagramFactoryBase<T>* factory,
												const Sequence< const DiscreteVariable* >* fusVarSeq,
												const HashTable< NodeId, Set< const DiscreteVariable* >* >* preneededVarTable,
												HashTable< const DiscreteVariable*, Idx >* instantiateVariables,
												List< const DiscreteVariable* >* defaultInstantiateVariables,
												const DiscreteVariable* leaderParentVar,
												NodeId leaderCurrentNode,
												NodeId followerCurrentNode,
												std::string tabu ) {

	tabu += "\t";
	NodeId newNode = 0;
		
			
	// ******************************************************************************************************
	// If both current nodes are terminal, 
	// we only have to compute the resulting value
		if( leader->isTerminalNode(leaderCurrentNode) && follower->isTerminalNode(followerCurrentNode) ){	
			
			//~ std::cout << std::endl << tabu << "Ajout noeud terminal " << " - Noeud leader : " << leaderCurrentNode << " - Noeud follower : " << followerCurrentNode;
			
			// First we have to get pointers on value from those terminal nodes
			T leaderValue = leader->getValueFromNode( leaderCurrentNode );
			T followerValue = follower->getValueFromNode( followerCurrentNode );
			
			// Then we have to compute new value
			// To do so we have to ensure that given value are in order
			// ( substraction and division aren't commutative for exemple)
			T newVal = 0;
				newVal = GUM_MULTI_DIM_OPERATOR( leaderValue, followerValue );
				
			// And finally we insert a new node in diagram with this value
			newNode = factory->addTerminalNode( newVal );
			
			//~ std ::cout << std::endl << "Insertion noeud terminal : " << newNode << " - Valeur : " << newVal << std::endl;
		}
	// *******************************************************************************************************
	
	if( !follower->isTerminalNode( followerCurrentNode ) )
		if( preneededVarTable->exists( followerCurrentNode ) ){
			
			//~ std::cout << std::endl << "Preneeded Variable : ";
			//~ for( SetIterator< const DiscreteVariable* > iterS = (*preneededVarTable)[ followerCurrentNode ]->begin(); iterS != (*preneededVarTable)[ followerCurrentNode ]->end(); ++iterS )
				//~ std::cout <<  (*iterS)->toString() << " - ";
			//~ std::cout << std::endl;
				
			Idx indexFin = fusVarSeq->size();
			if( !leader->isTerminalNode( leaderCurrentNode ) )
				indexFin = fusVarSeq->pos( leader->getVariableFromNode( leaderCurrentNode ) );
				
			Idx indexDebut = 0;
			if( leaderParentVar != NULL )
				indexDebut = fusVarSeq->pos( leaderParentVar ) + 1;
				
			if( indexFin != indexDebut )
				for( Idx i = indexDebut; i < indexFin; ++i ){
					//~ std::cout << std::endl << "Var in the hole : " << fusVarSeq->atPos( i )->toString() << std::endl ;
					if( (*preneededVarTable)[ followerCurrentNode ]->exists( fusVarSeq->atPos( i ) ) && 
						( !instantiateVariables->exists( fusVarSeq->atPos( i ) ) || !defaultInstantiateVariables->exists( fusVarSeq->atPos( i ) ) ) ) {
							//~ std::cout << std::endl << " Action Taken! - Noeud leader : " << leaderCurrentNode << " - Noeud follower : " << followerCurrentNode << " - Instantiated Variable : " << fusVarSeq->atPos( i )->toString();
								
							const List<NodeId>* varNodes = follower->getNodesFromVariable( fusVarSeq->atPos( i ) );
							HashTable< Idx, NodeId > sonsIds;

							List<Idx> varModalities;

							for(  ListConstIterator<NodeId> nodesIter = varNodes->begin(); nodesIter != varNodes->end(); ++nodesIter )
								for( HashTableConstIterator< Idx, NodeId > arcIter = follower->getNodeSons(*nodesIter)->begin(); arcIter != follower->getNodeSons(*nodesIter)->end(); ++arcIter )
									if( !varModalities.exists( arcIter.key() ) )
										varModalities.insert( arcIter.key() );

							for( ListConstIterator<Idx> modalitiesIter = varModalities.begin(); modalitiesIter != varModalities.end(); ++modalitiesIter){
								//~ std::cout << std::endl << tabu << " Descente sur fils numéro : " << *modalitiesIter;
								instantiateVariables->insert( fusVarSeq->atPos( i ), *modalitiesIter );
								sonsIds.insert( *modalitiesIter, GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, factory, fusVarSeq, preneededVarTable, instantiateVariables,
																defaultInstantiateVariables, fusVarSeq->atPos( i ), leaderCurrentNode, followerCurrentNode, tabu ) );
								instantiateVariables->erase( fusVarSeq->atPos( i ) );
							}

							NodeId* defaultSon = NULL;
							if( varModalities.size() != fusVarSeq->atPos( i )->domainSize() ){
								//~ std::cout << std::endl << tabu << " Descente sur fils par défaut";
								defaultInstantiateVariables->insert( fusVarSeq->atPos( i ) );
								defaultSon = new NodeId();
								*defaultSon = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, factory, fusVarSeq, preneededVarTable, instantiateVariables, 
																	defaultInstantiateVariables, fusVarSeq->atPos( i ), leaderCurrentNode, followerCurrentNode, tabu );
								defaultInstantiateVariables->eraseByVal( fusVarSeq->atPos( i ) );
							}
							return factory->addNonTerminalNodeWithArcs( fusVarSeq->atPos( i ), sonsIds, defaultSon );
							//~ std::cout << std::endl << "End Action Taken! ";

						} 
				}
		}
		
	// *******************************************************************************************************
	// If only one of the current node is terminal, 
	// we have to pursue deeper on the other diagram
		if( leader->isTerminalNode(leaderCurrentNode) && !follower->isTerminalNode(followerCurrentNode) )
			newNode = GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_FOLLOWER_FUNCTION( leader, follower, factory, fusVarSeq, preneededVarTable, instantiateVariables,
							defaultInstantiateVariables, leaderParentVar, leaderCurrentNode, followerCurrentNode, tabu );
			

		if( !leader->isTerminalNode(leaderCurrentNode) && follower->isTerminalNode(followerCurrentNode) )	
			newNode = GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_LEADER_FUNCTION( leader, follower, factory, fusVarSeq, preneededVarTable, instantiateVariables,
							defaultInstantiateVariables, leaderCurrentNode, followerCurrentNode, tabu );
	// *******************************************************************************************************
		

	// *******************************************************************************************************
	// Remains the case where both nodes aren't terminal
		if( !leader->isTerminalNode(leaderCurrentNode) && !follower->isTerminalNode(followerCurrentNode) ){
		
			//==========================================================
			// First we have to determine the position of current node 
			// associated variable in the resulting diagram
				int indexLeader = -1;
				int indexFollower = -1;
				if( fusVarSeq->exists( leader->getVariableFromNode( leaderCurrentNode ) ) )
					indexLeader = fusVarSeq->pos(  leader->getVariableFromNode( leaderCurrentNode ) );
				if( fusVarSeq->exists( follower->getVariableFromNode( followerCurrentNode ) ) )
					indexFollower = fusVarSeq->pos(  follower->getVariableFromNode( followerCurrentNode ) );
				if( indexLeader == -1 || indexFollower == -1 )
							GUM_ERROR( FatalError, "Unable to compute MultiDimDecisionDiagramBase");
			//==========================================================
					
			//==========================================================
			// If one variable preceeds the other on final diagram, we
			// have to go down on this one
				if( indexLeader > indexFollower )
					newNode = GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_FOLLOWER_FUNCTION( leader, follower, factory, fusVarSeq, preneededVarTable, instantiateVariables,
									defaultInstantiateVariables, leaderParentVar, leaderCurrentNode, followerCurrentNode, tabu );
					
				if( indexLeader < indexFollower )
					newNode = GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_LEADER_FUNCTION( leader, follower, factory, fusVarSeq, preneededVarTable, instantiateVariables,
									defaultInstantiateVariables, leaderCurrentNode, followerCurrentNode, tabu );
			//==========================================================

			//==========================================================
			// If not - meaning it's the same variable - we have to go
			// down on both
			if( indexLeader == indexFollower ){
				//~ std::cout << std::endl << tabu << "Recur - Noeud leader : " << leaderCurrentNode << " - Noeud follower : " << followerCurrentNode << " - Instantiated Variable : " << leader->getVariableFromNode( leaderCurrentNode )->toString();

				const HashTable< Idx, NodeId >* leaderSonsMap = leader->getNodeSons( leaderCurrentNode );
				const HashTable< Idx, NodeId >* followerSonsMap = follower->getNodeSons( followerCurrentNode );
				HashTable< Idx, NodeId > sonsIds;
				List<Idx> followerCheckedArcs;
				for( HashTableConstIterator<Idx, NodeId> leaderSonsIter = leaderSonsMap->begin(); leaderSonsIter != leaderSonsMap->end(); ++leaderSonsIter ){
					//~ std::cout << std::endl << tabu << " Descente sur fils numéro : " << leaderSonsIter.key();

					NodeId followerSon = 0;
					if( followerSonsMap->exists( leaderSonsIter.key() ) ){
						followerSon = (*followerSonsMap)[ leaderSonsIter.key() ];
						followerCheckedArcs.insert( leaderSonsIter.key() );
					}else	
						followerSon = follower->getNodeDefaultSon( followerCurrentNode );	
							
					sonsIds.insert( leaderSonsIter.key(), GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, factory, fusVarSeq, preneededVarTable, instantiateVariables,
													defaultInstantiateVariables, leader->getVariableFromNode( leaderCurrentNode ), *leaderSonsIter, followerSon, tabu ) );						
				}
				
				for( HashTableConstIterator<Idx, NodeId> followerSonsIter = followerSonsMap->begin(); followerSonsIter != followerSonsMap->end(); ++followerSonsIter)
					if( !followerCheckedArcs.exists( followerSonsIter.key() ) ){
						//~ std::cout << std::endl << tabu << " Descente sur fils numéro : " << followerSonsIter.key();
						sonsIds.insert( followerSonsIter.key(), GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, factory, fusVarSeq, preneededVarTable, instantiateVariables,
													defaultInstantiateVariables, leader->getVariableFromNode( leaderCurrentNode ), leader->getNodeDefaultSon( leaderCurrentNode ), 
															*followerSonsIter, tabu ) );
					}
				
				
				NodeId* defaultSon = NULL;
				if( leader->hasNodeDefaultSon( leaderCurrentNode ) && follower->hasNodeDefaultSon( followerCurrentNode ) ) {
					//~ std::cout << std::endl << tabu << " Descente sur fils par défaut";
					defaultSon = new NodeId();
					*defaultSon = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, factory, fusVarSeq, preneededVarTable, instantiateVariables, 
													defaultInstantiateVariables, leader->getVariableFromNode( leaderCurrentNode ), leader->getNodeDefaultSon( leaderCurrentNode ), 
															follower->getNodeDefaultSon( followerCurrentNode ), tabu );
				}
					
					
				newNode = factory->addNonTerminalNodeWithArcs( leader->getVariableFromNode( leaderCurrentNode ), sonsIds, defaultSon );

				//~ std::cout << std::endl << tabu << "Fin Recur - Noeud leader : " << leaderCurrentNode << " - Noeud follower : " << followerCurrentNode;
			}
		}
	// *******************************************************************************************************
 
	return newNode;
}



template< typename T >
NodeId
GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_LEADER_FUNCTION( const MultiDimDecisionDiagramBase<T>* leader,
														const MultiDimDecisionDiagramBase<T>* follower,
														MultiDimDecisionDiagramFactoryBase<T>* factory,
														const Sequence< const DiscreteVariable* >* fusVarSeq,
														const HashTable< NodeId, Set< const DiscreteVariable* >* >* preneededVarTable,
														HashTable< const DiscreteVariable*, Idx >* instantiateVariables,
														List< const DiscreteVariable* >* defaultInstantiateVariables,
														NodeId leaderCurrentNode,
														NodeId followerCurrentNode,
														std::string tabu ) {

	NodeId newNode = 0;
	//~ std::cout << std::endl << tabu << "GoDownLeader  - Noeud Leader : " << leaderCurrentNode << " - Noeud Follower : " << followerCurrentNode << " - Instantiated Variable : " << leader->getVariableFromNode( leaderCurrentNode )->toString();
		
		const HashTable< Idx, NodeId >* sonsMap = leader->getNodeSons(leaderCurrentNode);

		HashTable< Idx, NodeId > sonsIds;
		
		// ********************************************************************************************************
		// For each value the current var take on this node, we have to do our computation
			for( HashTableConstIterator<Idx, NodeId> iter = sonsMap->begin(); iter != sonsMap->end(); ++iter){
				
				//~ std::cout << std::endl << tabu << " Descente sur fils numéro : " << iter.key();
				
				// But we have to indicates to possible node on follower diagram, the current value of the var
				instantiateVariables->insert( leader->getVariableFromNode( leaderCurrentNode ), iter.key() );
				
				sonsIds.insert( iter.key(), GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, factory, fusVarSeq, preneededVarTable, instantiateVariables, 
											defaultInstantiateVariables, leader->getVariableFromNode( leaderCurrentNode ), *iter, followerCurrentNode, tabu ) );
						
				// And do some cleaning after the call of course					
				instantiateVariables->erase( leader->getVariableFromNode( leaderCurrentNode ) );
			}
		//*********************************************************************************************************
		
		// ********************************************************************************************************
		// Then, if not all possible value of that node have been investigate (meaning we have a default arc)
		// we have to look on second diagram every value that can still take this var (meaning value not taken 
		// on this leader node and the defautl one )
			
			NodeId* defaultSon = NULL;
			
			if( leader->hasNodeDefaultSon( leaderCurrentNode ) ) {
				
				List<Idx> varRemainingModalities;
				
				// First we get on the second diagram all nodes bound to that variable
				const List<NodeId>* varNodes = follower->getNodesFromVariable( leader->getVariableFromNode( leaderCurrentNode ) );

				if( varNodes != NULL ){

					// Then, we get remaining uninvestigated value for that variable
					for(  ListConstIterator<NodeId> nodesIter = varNodes->begin(); nodesIter != varNodes->end(); ++nodesIter )
						for( HashTableConstIterator< Idx, NodeId > arcIter = follower->getNodeSons(*nodesIter)->begin(); arcIter != follower->getNodeSons(*nodesIter)->end(); ++arcIter )
							if( !sonsMap->exists( arcIter.key() ) && !varRemainingModalities.exists( arcIter.key() ) )
								varRemainingModalities.insert( arcIter.key() );

					// Then, for each of those remaining value, we go down
					for( ListConstIterator<Idx> modalitiesIter = varRemainingModalities.begin(); modalitiesIter != varRemainingModalities.end(); ++modalitiesIter){
						
						//~ std::cout << std::endl << tabu << " Descente sur fils numéro : " << *modalitiesIter;
						
						// with usual indicators
						instantiateVariables->insert( leader->getVariableFromNode( leaderCurrentNode ), *modalitiesIter );
						
						sonsIds.insert( *modalitiesIter, GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, factory, fusVarSeq, preneededVarTable, instantiateVariables,
																	defaultInstantiateVariables, leader->getVariableFromNode( leaderCurrentNode ), leader->getNodeDefaultSon( leaderCurrentNode ), 
																		followerCurrentNode, tabu ) );
														
						instantiateVariables->erase( leader->getVariableFromNode( leaderCurrentNode ) );
					}

				}

				// Next, we go down on default variable, just in case
				if( sonsMap->size() + varRemainingModalities.size() != leader->getVariableFromNode( leaderCurrentNode )->domainSize() ){
					//~ std::cout << std::endl << tabu << " Descente sur fils par défaut";
					
					defaultInstantiateVariables->insert( leader->getVariableFromNode( leaderCurrentNode ) );
					
					defaultSon = new NodeId();
					*defaultSon = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, factory, fusVarSeq, preneededVarTable, instantiateVariables, 
												defaultInstantiateVariables, leader->getVariableFromNode( leaderCurrentNode ), leader->getNodeDefaultSon( leaderCurrentNode ), 
														followerCurrentNode, tabu );
														
					defaultInstantiateVariables->eraseByVal( leader->getVariableFromNode( leaderCurrentNode ) );
				}

			}
		//*********************************************************************************************************
		
		// ********************************************************************************************************
		// And we finally add this node to our resulting graph
		newNode = factory->addNonTerminalNodeWithArcs( leader->getVariableFromNode(leaderCurrentNode), sonsIds, defaultSon );
		
	//~ std::cout << std::endl << tabu << "Fin GoDownLeader  - Noeud Leader : " << leaderCurrentNode << " - Noeud Follower : " << followerCurrentNode;
	
	return newNode;
}



template< typename T >
NodeId
GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_FOLLOWER_FUNCTION( const MultiDimDecisionDiagramBase<T>* leader,
															const MultiDimDecisionDiagramBase<T>* follower,
															MultiDimDecisionDiagramFactoryBase<T>* factory,
															const Sequence< const DiscreteVariable* >* fusVarSeq,
															const HashTable< NodeId, Set< const DiscreteVariable* >* >* preneededVarTable,
															HashTable< const DiscreteVariable*, Idx >* instantiateVariables,
															List< const DiscreteVariable* >* defaultInstantiateVariables,
															const DiscreteVariable* leaderParentVar,
															NodeId leaderCurrentNode,
															NodeId followerCurrentNode,
															std::string tabu ) {
													
		//~ std::cout << std::endl << tabu << "GoDownFollower - Noeud leader : " << leaderCurrentNode << " - Noeud follower : " << followerCurrentNode << " - Instantiated Variable : " << follower->getVariableFromNode( followerCurrentNode )->toString();

		NodeId newNode = 0;
		
		const HashTable< Idx, NodeId >* sonsMap = follower->getNodeSons( followerCurrentNode );

		// ***************************************************************************************************************
		// If var exists in leader diagram and has already been instantiate to its default value,
		// we have to do so in second diagram too
			if( defaultInstantiateVariables->exists( follower->getVariableFromNode( followerCurrentNode ) ) ){
				
				//~ std::cout << std::endl << tabu << " Saut par fils par défaut ";
				newNode = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, factory, fusVarSeq, preneededVarTable, instantiateVariables, 
											defaultInstantiateVariables, leaderParentVar, leaderCurrentNode, follower->getNodeDefaultSon( followerCurrentNode ), tabu );
			
			}
		// ***************************************************************************************************************
			else 
		// ***************************************************************************************************************
		// If var exists in leader diagram and has already been instantiate to a value,
		// we have to go down on this value
			if( instantiateVariables->exists( follower->getVariableFromNode( followerCurrentNode ) ) ) {
				
				//~ std::cout << std::endl << tabu << " Saut par fils : " << (*instantiateVariables)[ follower->getVariableFromNode( followerCurrentNode ) ];
				
				// But we have to check if value has its arc for this node
				if( sonsMap->exists( (*instantiateVariables)[ follower->getVariableFromNode( followerCurrentNode ) ] ) )
				
					newNode = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, factory, fusVarSeq, preneededVarTable, instantiateVariables,
											defaultInstantiateVariables, leaderParentVar, leaderCurrentNode,
												(*sonsMap)[ (*instantiateVariables)[ follower->getVariableFromNode( followerCurrentNode ) ] ], tabu );
				else
				
					newNode = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, factory, fusVarSeq, preneededVarTable, instantiateVariables,
											defaultInstantiateVariables, leaderParentVar,  leaderCurrentNode,
												follower->getNodeDefaultSon( followerCurrentNode ), tabu );
											
			} 
		// ****************************************************************************************************************			
			else {
		// ***************************************************************************************************************
		// If we aren't in one of the above cases, it means that leader diagram hasn't the variable pointed by follower current node.
			HashTable< Idx, NodeId > sonsIds;
			
			// In that case we do our computation for all sons of that var
			for( HashTableConstIterator<Idx, NodeId> iter = sonsMap->begin(); iter != sonsMap->end(); ++iter){
				//~ std::cout << std::endl << tabu << " Descente sur fils numéro : " << iter.key();
				sonsIds.insert( iter.key(), GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, factory, fusVarSeq, preneededVarTable, instantiateVariables,
										defaultInstantiateVariables, leaderParentVar, leaderCurrentNode, *iter, tabu ) );
			}

			// and for its default son if it has one
			NodeId* defaultSon = NULL;
			if( follower->hasNodeDefaultSon( followerCurrentNode ) ) {
				//~ std::cout << std::endl << tabu << " Descente sur fils par défaut";
				defaultSon = new NodeId();
				*defaultSon = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, factory, fusVarSeq, preneededVarTable, instantiateVariables,
										defaultInstantiateVariables, leaderParentVar, leaderCurrentNode, follower->getNodeDefaultSon( followerCurrentNode ), tabu );
			} 
			
			// And we had this node to our graph
			newNode = factory->addNonTerminalNodeWithArcs( follower->getVariableFromNode(followerCurrentNode), sonsIds, defaultSon );

			}
		// ****************************************************************************************************************	
	//~ std::cout << std::endl << tabu << "Fin GoDownFollower - Noeud leader : " << leaderCurrentNode << " - Noeud follower : " << followerCurrentNode;
	
	return newNode;
}
