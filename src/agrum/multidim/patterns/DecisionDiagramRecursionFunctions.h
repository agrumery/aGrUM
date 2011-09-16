#ifndef GUM_DECISION_DIAGRAM_OPERATOR_DATA
#define GUM_DECISION_DIAGRAM_OPERATOR_DATA


template< typename T >
struct OperatorData {
	
	/// The factory that build up resulting diagram
	MultiDimDecisionDiagramFactoryBase<T>* factory;
	
	/// Table giving for each node of second diagram, the list of variables requiring instantiation before
	HashTable< NodeId, Set< const DiscreteVariable* >* >* retrogradeVarTable;
	
	/// Table giving instantiate variables and their modality
	HashTable< const DiscreteVariable*, Idx >* instantiateVariables;
	
	/// Table giving instantiate variables and their modality
	HashTable< NodeId, List<Idx>* >* remainingModalitiesTable;
	
	/// List giving instantiate variables to their default value
	List< const DiscreteVariable* >* defaultInstantiateVariables;
	
	/// Table to remind us wich part of both diagram have already been explored, and the resulting node
	HashTable< std::string, NodeId >* explorationTable;
	
	Idx nbOpToDo;
	
	OperatorData( const MultiDimDecisionDiagramBase<T>* t1, const MultiDimDecisionDiagramBase<T>* t2, T newLowLimit, T newHighLimit ){
		
		
		// =========================================================================================================
		// First we determine the new var sequence
		
		Sequence< const DiscreteVariable* > a1VarSeq = t1->variablesSequence();
		Sequence< const DiscreteVariable* > a2VarSeq = t2->variablesSequence();
		Sequence< const DiscreteVariable* > fusVarSeq;	
		
		SequenceIterator< const DiscreteVariable* > iterS1 = a1VarSeq.begin(); 
		//~ while( iterS1 != a1VarSeq.end() )
			//~ if( !t1->isInDiagramVariable( *iterS1 ) ){
				//~ if( !t2->isInDiagramVariable( *iterS1 ) )
					//~ fusVarSeq->insert( *iterS1 );
				//~ a1VarSeq.erase( iterS1 );
			//~ }else
				 //~ ++iterS1;	
							//~ 
		SequenceIterator< const DiscreteVariable* > iterS2 = a2VarSeq.begin();
		//~ while( iterS2 != a2VarSeq.end() )
			//~ if( !t2->isInDiagramVariable( *iterS2 ) )
				//~ a2VarSeq.erase( *iterS2 );
			//~ else
				//~ ++iterS2;	
				
		//~ iterS1 = a1VarSeq.begin();
		//~ iterS2 = a2VarSeq.begin();
		
		while( iterS1 != a1VarSeq.end() || iterS2 != a2VarSeq.end() ){
			if( iterS1 == a1VarSeq.end() ){
				for( ; iterS2 != a2VarSeq.end(); ++iterS2 )
					if( !fusVarSeq.exists(*iterS2) )
						fusVarSeq.insert( *iterS2 );
			}
			else if( iterS2 == a2VarSeq.end() ){
				for( ; iterS1 != a1VarSeq.end(); ++iterS1 )
					if( !fusVarSeq.exists(*iterS1) )
						fusVarSeq.insert( *iterS1 );
			} else {
				if( *iterS1 == *iterS2 ){
					if( !fusVarSeq.exists(*iterS1) )
						fusVarSeq.insert( *iterS1 );
					++iterS1;
					++iterS2;
					continue;
				}
				if( a1VarSeq.pos(*iterS1) <= a2VarSeq.pos(*iterS2) || a1VarSeq.exists( *iterS2 ) ){
					if( !fusVarSeq.exists(*iterS1) )
						fusVarSeq.insert( *iterS1 );
					++iterS1;
					continue;
				} else {
					if( !fusVarSeq.exists(*iterS2) )
						fusVarSeq.insert( *iterS2 );
					++iterS2;
					continue;
				} 
			}
		}
				
		//~ GUM_TRACE( " Mixed Sequence variable : " );
		//~ for( SequenceIterator< const DiscreteVariable* > iter = fusVarSeq->begin(); iter != fusVarSeq->end(); ++iter )
			//~ GUM_TRACE( (*iter)->toString() << " - " );
		//~ GUM_TRACE( std::endl );
		
		// =========================================================================================================
			
		// =========================================================================================================
		// Then we instantiate the factory that will create the new multidim
		// and we give it the new bound			
		factory = t1->getFactory( *t1, newLowLimit, newHighLimit);
		factory->setVariablesSequence( fusVarSeq );
		// ==========================================================================================================	
		
		// =========================================================================================================
		// Then we search in second diagram for possible preneeded variable
		retrogradeVarTable = new HashTable< NodeId, Set< const DiscreteVariable* >* >();
		t2->findRetrogradeVariables( &( factory->variablesSequence() ), retrogradeVarTable );
		// =========================================================================================================
		
		// =========================================================================================================
		// Then we get for each node of first diagramm the remaining modalities that on diagramm 2 var associated 
		// to our node takes but are defaulted on our node
		remainingModalitiesTable = new HashTable< NodeId, List<Idx>* >();
		for( NodeGraphPartIterator nodeIter = t1->nodesMap().beginNodes(); nodeIter != t1->nodesMap().endNodes(); ++nodeIter ){
			if( t1->isTerminalNode( *nodeIter ) )
				continue;
			remainingModalitiesTable->insert( *nodeIter, new List<Idx>() );
			const List<NodeId>* varNodes = t2->variableNodes( t1->nodeVariable( *nodeIter ) );
			if( varNodes != NULL ){
				// Then, we get remaining uninvestigated value for that variable
				for(  ListConstIterator<NodeId> nodesIter = varNodes->begin(); nodesIter != varNodes->end(); ++nodesIter )
					for( HashTableConstIterator< Idx, NodeId > arcIter = t2->nodeSons(*nodesIter)->begin(); arcIter != t2->nodeSons(*nodesIter)->end(); ++arcIter )
						if( !t1->nodeSons(*nodeIter)->exists( arcIter.key() ) && !(*remainingModalitiesTable)[*nodeIter]->exists( arcIter.key() ) )
							(*remainingModalitiesTable)[*nodeIter]->insert( arcIter.key() );
			}
		}
		
		// =========================================================================================================
		// And we finally call the recursif methods that will build the diagram		
		instantiateVariables = new HashTable< const DiscreteVariable*, Idx >();	
		defaultInstantiateVariables = new List< const DiscreteVariable* >();
		explorationTable = new HashTable< std::string, NodeId >();
		nbOpToDo = 0;
	}
	
	~OperatorData(){

		delete instantiateVariables;
		
		delete defaultInstantiateVariables;
		
		for( HashTableIterator< NodeId, Set< const DiscreteVariable* >* > iterH = retrogradeVarTable->begin(); iterH != retrogradeVarTable->end(); ++iterH )
			delete *iterH;
		delete retrogradeVarTable;
		
		for( HashTableIterator< NodeId, List<Idx>* > iterH = remainingModalitiesTable->begin(); iterH != remainingModalitiesTable->end(); ++iterH )
			delete *iterH;
		delete remainingModalitiesTable;
		
		delete factory;
		
		delete explorationTable;
	
	}
};

template<typename T>
NodeId
insertNonTerminalNode( OperatorData<T>& opData, const DiscreteVariable* associatedVariable, HashTable< Idx, NodeId >& sonsMap, NodeId* defaultSon ){
	
	if( defaultSon != NULL )
		for( HashTableConstIterator< Idx, NodeId > iterArcMap = sonsMap.begin(); iterArcMap != sonsMap.end(); ++iterArcMap )
			if( *iterArcMap == *defaultSon )
				sonsMap.erase( iterArcMap );
	if( defaultSon != NULL && sonsMap.size() == associatedVariable->domainSize() - 1 )
		for( Idx modality = 0; modality < associatedVariable->domainSize(); ++modality )
			if( !sonsMap.exists( modality ) ){
				sonsMap.insert( modality, *defaultSon );
				defaultSon = NULL;
				break;
			}
	return opData.factory->unsafeAddNonTerminalNodeWithArcs( associatedVariable, sonsMap, defaultSon );
}

#endif


#ifdef GUM_DECISION_DIAGRAM_OPERATOR_FUNCTION_DEFINITION

template<typename T>
NodeId
GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( 	const MultiDimDecisionDiagramBase<T>* leader,
												const MultiDimDecisionDiagramBase<T>* follower,
												OperatorData<T>& opData,
												const DiscreteVariable* leaderParentVar,
												NodeId leaderCurrentNode,
												NodeId followerCurrentNode,
												std::string tabu ) {
													
											
													
	std::stringstream key;
	key <<  leaderCurrentNode << "-" << followerCurrentNode << "-";
	if( opData.retrogradeVarTable->exists( followerCurrentNode ) ){
		for( HashTableIterator<  const DiscreteVariable*, Idx > instIter = opData.instantiateVariables->begin(); instIter != opData.instantiateVariables->end(); ++instIter )
			 if( (*(opData.retrogradeVarTable))[ followerCurrentNode ]->exists( instIter.key() ) )
				key << instIter.key()->name() << *instIter << "-";
		for( ListIterator< const DiscreteVariable* > listIter = opData.defaultInstantiateVariables->begin(); listIter != opData.defaultInstantiateVariables->end(); ++listIter )
			if( (*(opData.retrogradeVarTable))[ followerCurrentNode ]->exists( *listIter ) )
				key << (*listIter)->name() << "*-";
	}
	if( opData.explorationTable->exists( key.str() ) ){
			//~ GUM_TRACE( "PRUNING! : " << key.str() << std::endl );
			return (*(opData.explorationTable))[key.str()];
	}

	tabu += "\t";
	NodeId newNode = 0;
		
			
	// ******************************************************************************************************
	// If both current nodes are terminal, 
	// we only have to compute the resulting value
		if( leader->isTerminalNode(leaderCurrentNode) && follower->isTerminalNode(followerCurrentNode) ){	
			
			// First we have to get pointers on value from those terminal nodes
			T leaderValue = leader->unsafeNodeValue( leaderCurrentNode );
			T followerValue = follower->unsafeNodeValue( followerCurrentNode );
			
			// Then we have to compute new value
			// To do so we have to ensure that given value are in order
			// ( substraction and division aren't commutative for exemple)
			T newVal = GUM_MULTI_DIM_OPERATOR( leaderValue, followerValue );
			Idx nbOptToDo = opData.nbOpToDo;
			while( nbOptToDo > 1 ){
				newVal = GUM_MULTI_DIM_OPERATOR( newVal, followerValue );
				nbOptToDo--;
			}
				
			// And finally we insert a new node in diagram with this value
			newNode = opData.factory->addTerminalNode( newVal );
			opData.explorationTable->insert( key.str(), newNode );	
		}
	// *******************************************************************************************************
	
	if( !follower->isTerminalNode( followerCurrentNode ) )
		if( opData.retrogradeVarTable->exists( followerCurrentNode ) ){
				
			Idx indexFin = opData.factory->variablesSequence().size();
			if( !leader->isTerminalNode( leaderCurrentNode ) )
				indexFin = opData.factory->variablesSequence().pos( leader->unsafeNodeVariable( leaderCurrentNode ) );
				
			Idx indexDebut = 0;
			if( leaderParentVar != NULL )
				indexDebut = opData.factory->variablesSequence().pos( leaderParentVar ) + 1;
				
			if( indexFin != indexDebut )
				for( Idx i = indexDebut; i < indexFin; ++i ){
					
					if( (*opData.retrogradeVarTable)[ followerCurrentNode ]->exists( opData.factory->variablesSequence().atPos( i ) ) && 
						( !opData.instantiateVariables->exists( opData.factory->variablesSequence().atPos( i ) ) || 
								!opData.defaultInstantiateVariables->exists( opData.factory->variablesSequence().atPos( i ) ) ) ) {
									
							const List<NodeId>* varNodes = follower->variableNodes( opData.factory->variablesSequence().atPos( i ) );
							HashTable< Idx, NodeId > sonsIds;

							List<Idx> doneVarModalities;

							for(  ListConstIterator<NodeId> nodesIter = varNodes->begin(); nodesIter != varNodes->end(); ++nodesIter )
								for( HashTableConstIterator< Idx, NodeId > arcIter = follower->unsafeNodeSons(*nodesIter)->begin(); arcIter != follower->unsafeNodeSons(*nodesIter)->end(); ++arcIter )
									if( !doneVarModalities.exists( arcIter.key() ) ){
										doneVarModalities.insert( arcIter.key() );
										opData.instantiateVariables->insert( opData.factory->variablesSequence().atPos( i ), arcIter.key() );
										sonsIds.insert( arcIter.key(), GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, opData, opData.factory->variablesSequence().atPos( i ), leaderCurrentNode,
																										followerCurrentNode, tabu ) );
										opData.instantiateVariables->erase( opData.factory->variablesSequence().atPos( i ) );
									}

							NodeId* defaultSon = NULL;
							if( doneVarModalities.size() != opData.factory->variablesSequence().atPos( i )->domainSize() ){
								opData.defaultInstantiateVariables->insert( opData.factory->variablesSequence().atPos( i ) );
								defaultSon = new NodeId();
								*defaultSon = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, opData, opData.factory->variablesSequence().atPos( i ), leaderCurrentNode, 
																			followerCurrentNode, tabu );
								opData.defaultInstantiateVariables->eraseByVal( opData.factory->variablesSequence().atPos( i ) );
							}	
			
							newNode = insertNonTerminalNode( opData, opData.factory->variablesSequence().atPos( i ), sonsIds, defaultSon );
							
							return newNode;

						} 
				}
		}
		
	// *******************************************************************************************************
	// If only one of the current node is terminal, 
	// we have to pursue deeper on the other diagram
		if( leader->isTerminalNode(leaderCurrentNode) && !follower->isTerminalNode(followerCurrentNode) )
			newNode = GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_FOLLOWER_FUNCTION( leader, follower, opData, leaderParentVar, leaderCurrentNode, followerCurrentNode, tabu );
			

		if( !leader->isTerminalNode(leaderCurrentNode) && follower->isTerminalNode(followerCurrentNode) )	
			newNode = GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_LEADER_FUNCTION( leader, follower, opData, leaderParentVar, leaderCurrentNode, followerCurrentNode, tabu );
	// *******************************************************************************************************
		

	// *******************************************************************************************************
	// Remains the case where both nodes aren't terminal
		if( !leader->isTerminalNode(leaderCurrentNode) && !follower->isTerminalNode(followerCurrentNode) ){
		
			//==========================================================
			// First we have to determine the position of current node 
			// associated variable in the resulting diagram
				int indexLeader = -1;
				int indexFollower = -1;

				if( opData.factory->variablesSequence().exists( leader->unsafeNodeVariable( leaderCurrentNode ) ) )
					indexLeader = opData.factory->variablesSequence().pos(  leader->unsafeNodeVariable( leaderCurrentNode ) );
				if( opData.factory->variablesSequence().exists( follower->unsafeNodeVariable( followerCurrentNode ) ) )
					indexFollower = opData.factory->variablesSequence().pos(  follower->unsafeNodeVariable( followerCurrentNode ) );
				if( indexLeader == -1 || indexFollower == -1 )
							GUM_ERROR( FatalError, "Unable to compute MultiDimDecisionDiagramBase");
			//==========================================================
					
			//==========================================================
			// If one variable preceeds the other on final diagram, we
			// have to go down on this one
				if( indexLeader > indexFollower )
					newNode = GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_FOLLOWER_FUNCTION( leader, follower, opData, leaderParentVar, leaderCurrentNode, followerCurrentNode, tabu );
					
				if( indexLeader < indexFollower )
					newNode = GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_LEADER_FUNCTION( leader, follower, opData, leaderParentVar, leaderCurrentNode, followerCurrentNode, tabu );
			//==========================================================

			//==========================================================
			// If not - meaning it's the same variable - we have to go
			// down on both
			if( indexLeader == indexFollower ){
				
				const HashTable< Idx, NodeId >* leaderSonsMap = leader->unsafeNodeSons( leaderCurrentNode );
				const HashTable< Idx, NodeId >* followerSonsMap = follower->unsafeNodeSons( followerCurrentNode );

				HashTable< Idx, NodeId > sonsIds;
				List<Idx> followerCheckedArcs;
				for( HashTableConstIterator<Idx, NodeId> leaderSonsIter = leaderSonsMap->begin(); leaderSonsIter != leaderSonsMap->end(); ++leaderSonsIter ){

					NodeId followerSon = 0;
					if( followerSonsMap->exists( leaderSonsIter.key() ) ){
						followerSon = (*followerSonsMap)[ leaderSonsIter.key() ];
						followerCheckedArcs.insert( leaderSonsIter.key() );
					}else	
						followerSon = follower->unsafeNodeDefaultSon( followerCurrentNode );
					sonsIds.insert( leaderSonsIter.key(), GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, opData, leader->unsafeNodeVariable( leaderCurrentNode ), *leaderSonsIter, 
																							followerSon, tabu ) );	
				}
				
				for( HashTableConstIterator<Idx, NodeId> followerSonsIter = followerSonsMap->begin(); followerSonsIter != followerSonsMap->end(); ++followerSonsIter)
					if( !followerCheckedArcs.exists( followerSonsIter.key() ) ){
						sonsIds.insert( followerSonsIter.key(), GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, opData, leader->unsafeNodeVariable( leaderCurrentNode ),
																							leader->unsafeNodeDefaultSon( leaderCurrentNode ), *followerSonsIter, tabu ) );

					}
				
				
				NodeId* defaultSon = NULL;
				
				if( leader->unsafeHasNodeDefaultSon( leaderCurrentNode ) && follower->unsafeHasNodeDefaultSon( followerCurrentNode ) ) {
					defaultSon = new NodeId();
					*defaultSon = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, opData, leader->unsafeNodeVariable( leaderCurrentNode ), leader->unsafeNodeDefaultSon( leaderCurrentNode ), 
															follower->unsafeNodeDefaultSon( followerCurrentNode ), tabu );
				}					
					
				newNode = insertNonTerminalNode( opData, leader->unsafeNodeVariable( leaderCurrentNode ), sonsIds, defaultSon );
				opData.explorationTable->insert( key.str(), newNode );	
			}
		}
	// *******************************************************************************************************
	
	return newNode;
}



template< typename T >
NodeId
GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_LEADER_FUNCTION( const MultiDimDecisionDiagramBase<T>* leader,
														const MultiDimDecisionDiagramBase<T>* follower,
														OperatorData<T>& opData,
														const DiscreteVariable* leaderParentVar,
														NodeId leaderCurrentNode,
														NodeId followerCurrentNode,
														std::string tabu ) {
													
													
	std::stringstream key;
	key <<  leaderCurrentNode << "-" << followerCurrentNode << "-";
	if( opData.retrogradeVarTable->exists( followerCurrentNode ) ){
		for( HashTableIterator<  const DiscreteVariable*, Idx > instIter = opData.instantiateVariables->begin(); instIter != opData.instantiateVariables->end(); ++instIter )
			 if( (*(opData.retrogradeVarTable))[ followerCurrentNode ]->exists( instIter.key() ) )
				key << instIter.key()->name() << *instIter << "-";
		for( ListIterator< const DiscreteVariable* > listIter = opData.defaultInstantiateVariables->begin(); listIter != opData.defaultInstantiateVariables->end(); ++listIter )
			if( (*(opData.retrogradeVarTable))[ followerCurrentNode ]->exists( *listIter ) )
				key << (*listIter)->name() << "*-";
	}
	//~ if( opData.explorationTable->exists( key.str() ) ){
			//~ std::cout << "PRUNING! : " << key.str() << std::endl;
			//~ return (*(opData.explorationTable))[key.str()];
	//~ }

	NodeId newNode = 0;

		
		const HashTable< Idx, NodeId >* sonsMap = leader->unsafeNodeSons(leaderCurrentNode);

		HashTable< Idx, NodeId > sonsIds;
		
		// ********************************************************************************************************
		// For each value the current var take on this node, we have to do our computation
			for( HashTableConstIterator<Idx, NodeId> iter = sonsMap->begin(); iter != sonsMap->end(); ++iter){
				
				// But we have to indicates to possible node on follower diagram, the current value of the var
				opData.instantiateVariables->insert( leader->unsafeNodeVariable( leaderCurrentNode ), iter.key() );
				
				sonsIds.insert( iter.key(), GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, opData, leader->unsafeNodeVariable( leaderCurrentNode ), *iter, followerCurrentNode, tabu ) );
						
				// And do some cleaning after the call of course					
				opData.instantiateVariables->erase( leader->unsafeNodeVariable( leaderCurrentNode ) );
			}
		
		// ********************************************************************************************************
		// Then, if not all possible value of that node have been investigate (meaning we have a default arc)
		// we have to look on second diagram every value that can still take this var (meaning value not taken 
		// on this leader node and the defautl one )
			
			NodeId* defaultSon = NULL;
			
			if( leader->unsafeHasNodeDefaultSon( leaderCurrentNode ) ) {

				// Then, for each of those remaining value, we go down
				for( ListConstIterator<Idx> modalitiesIter = (*(opData.remainingModalitiesTable))[leaderCurrentNode]->begin(); 
															modalitiesIter != (*(opData.remainingModalitiesTable))[leaderCurrentNode]->end(); ++modalitiesIter){
					
					// with usual indicators
					opData.instantiateVariables->insert( leader->unsafeNodeVariable( leaderCurrentNode ), *modalitiesIter );
					
					sonsIds.insert( *modalitiesIter, GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, opData, leader->unsafeNodeVariable( leaderCurrentNode ), 
																				leader->unsafeNodeDefaultSon( leaderCurrentNode ), followerCurrentNode, tabu ) );
													
					opData.instantiateVariables->erase( leader->unsafeNodeVariable( leaderCurrentNode ) );
				}

				// Next, we go down on default variable, just in case
				if( sonsMap->size() + (*(opData.remainingModalitiesTable))[leaderCurrentNode]->size() != leader->unsafeNodeVariable( leaderCurrentNode )->domainSize() ){
					
					opData.defaultInstantiateVariables->insert( leader->unsafeNodeVariable( leaderCurrentNode ) );
					
					defaultSon = new NodeId();
					*defaultSon = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, opData, leader->unsafeNodeVariable( leaderCurrentNode ), leader->unsafeNodeDefaultSon( leaderCurrentNode ), 
															followerCurrentNode, tabu );
														
					opData.defaultInstantiateVariables->eraseByVal( leader->unsafeNodeVariable( leaderCurrentNode ) );
				}

			}

		//*********************************************************************************************************
		
		// ********************************************************************************************************
		// And we finally add this node to our resulting graph
		newNode = insertNonTerminalNode( opData, leader->unsafeNodeVariable(leaderCurrentNode), sonsIds, defaultSon );
		opData.explorationTable->insert( key.str(), newNode );	
	
	return newNode;
}



template< typename T >
NodeId
GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_FOLLOWER_FUNCTION( const MultiDimDecisionDiagramBase<T>* leader,
															const MultiDimDecisionDiagramBase<T>* follower,
															OperatorData<T>& opData,
															const DiscreteVariable* leaderParentVar,
															NodeId leaderCurrentNode,
															NodeId followerCurrentNode,
															std::string tabu ) {
													
														
		std::stringstream key;
		key <<  leaderCurrentNode << "-" << followerCurrentNode << "-";
		if( opData.retrogradeVarTable->exists( followerCurrentNode ) ){
			for( HashTableIterator<  const DiscreteVariable*, Idx > instIter = opData.instantiateVariables->begin(); instIter != opData.instantiateVariables->end(); ++instIter )
				 if( (*(opData.retrogradeVarTable))[ followerCurrentNode ]->exists( instIter.key() ) )
					key << instIter.key()->name() << *instIter << "-";
			for( ListIterator< const DiscreteVariable* > listIter = opData.defaultInstantiateVariables->begin(); listIter != opData.defaultInstantiateVariables->end(); ++listIter )
				if( (*(opData.retrogradeVarTable))[ followerCurrentNode ]->exists( *listIter ) )
					key << (*listIter)->name() << "*-";
		}
		//~ if( opData.explorationTable->exists( key.str() ) ){
			//~ std::cout << "PRUNING! : " << key.str() << std::endl;
				//~ return (*(opData.explorationTable))[key.str()];
		//~ }

		NodeId newNode = 0;
		
		const HashTable< Idx, NodeId >* sonsMap = follower->unsafeNodeSons( followerCurrentNode );

		// ***************************************************************************************************************
		// If var exists in leader diagram and has already been instantiate to its default value,
		// we have to do so in second diagram too
			if( opData.defaultInstantiateVariables->exists( follower->unsafeNodeVariable( followerCurrentNode ) ) )
				newNode = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, opData, leaderParentVar, leaderCurrentNode, follower->unsafeNodeDefaultSon( followerCurrentNode ), tabu );
		// ***************************************************************************************************************
			else 
		// ***************************************************************************************************************
		// If var exists in leader diagram and has already been instantiate to a value,
		// we have to go down on this value
			if( opData.instantiateVariables->exists( follower->unsafeNodeVariable( followerCurrentNode ) ) ) {
								
				// But we have to check if value has its arc for this node
				if( sonsMap->exists( (*opData.instantiateVariables)[ follower->unsafeNodeVariable( followerCurrentNode ) ] ) )
				
					newNode = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, opData, leaderParentVar, leaderCurrentNode,
												(*sonsMap)[ (*opData.instantiateVariables)[ follower->unsafeNodeVariable( followerCurrentNode ) ] ], tabu );
				else
				
					newNode = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, opData, leaderParentVar, leaderCurrentNode, follower->unsafeNodeDefaultSon( followerCurrentNode ), tabu );
											
			} 
		// ****************************************************************************************************************			
			else {
		// ***************************************************************************************************************
		// If we aren't in one of the above cases, it means that leader diagram hasn't the variable pointed by follower current node.
			HashTable< Idx, NodeId > sonsIds;
			
			// In that case we do our computation for all sons of that var
			for( HashTableConstIterator<Idx, NodeId> iter = sonsMap->begin(); iter != sonsMap->end(); ++iter){
				sonsIds.insert( iter.key(), GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, opData, leaderParentVar, leaderCurrentNode, *iter, tabu ) );
			}

			// and for its default son if it has one
			NodeId* defaultSon = NULL;
			
			if( follower->unsafeHasNodeDefaultSon( followerCurrentNode ) ) {
				defaultSon = new NodeId();
				*defaultSon = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, opData, leaderParentVar, leaderCurrentNode, follower->unsafeNodeDefaultSon( followerCurrentNode ), tabu );
			} 
			
			// And we had this node to our graph
			newNode = insertNonTerminalNode( opData, follower->unsafeNodeVariable(followerCurrentNode), sonsIds, defaultSon );
			opData.explorationTable->insert( key.str(), newNode );	

			}
		// ****************************************************************************************************************
	//~ opData.explorationTable->insert( key.str(), newNode );	
	return newNode;
}

#endif

#ifdef GUM_DECISION_DIAGRAM_PROJECTION_ELEMINATE_NODE_FUNCTION_DEFINITION

template<typename T>
MultiDimDecisionDiagramBase< T >*
GUM_DECISION_DIAGRAM_PROJECTION_ELEMINATE_NODE_FUNCTION( const MultiDimDecisionDiagramBase< T >* oldDiagram, NodeId node, Idx domainSize ){
	
	// ************************************************************************************
	// If so, projection is done by operating sub diagram rooted by node sons
	Idx nbProjDone = 0;
	const HashTable< Idx, NodeId >* childrenMap = oldDiagram->unsafeNodeSons( node );
	MultiDimDecisionDiagramBase< T >* t1 = NULL;
	HashTableConstIterator< Idx, NodeId > childrenIter = childrenMap->begin();
	NodeId prevChild = *childrenIter;
	++childrenIter;
	
	while( childrenIter != childrenMap->end() ){ 
		
		if( t1 == NULL){
			OperatorData<T> opData( oldDiagram, oldDiagram, oldDiagram->lowLimit(), oldDiagram->highLimit() );
		
			GUM_DECISION_DIAGRAM_PROJECTION_SUB_DIAGRAM_OPERATOR_FUNCTION( oldDiagram, oldDiagram, opData, NULL, prevChild, *childrenIter, "" );
		
			t1 = opData.factory->getMultiDimDecisionDiagram();
		} else {
			OperatorData<T> opData( t1, oldDiagram, t1->lowLimit(), t1->highLimit() );
		
			GUM_DECISION_DIAGRAM_PROJECTION_SUB_DIAGRAM_OPERATOR_FUNCTION( t1, oldDiagram, opData, NULL, t1->root(), *childrenIter, "" );	
			delete t1;	
		
			t1 = opData.factory->getMultiDimDecisionDiagram();	
		}
		
		nbProjDone++;
		++childrenIter;
	}
	
	if( oldDiagram->unsafeHasNodeDefaultSon( node ) ){
		Idx nbProjRemaining = domainSize - nbProjDone;
		
		if( t1 == NULL){
			OperatorData<T> opData( oldDiagram, oldDiagram, oldDiagram->lowLimit(), oldDiagram->highLimit() );
			opData.nbOpToDo = nbProjRemaining - 1 ;
		
			GUM_DECISION_DIAGRAM_PROJECTION_SUB_DIAGRAM_OPERATOR_FUNCTION( oldDiagram, oldDiagram, opData, NULL, prevChild, oldDiagram->unsafeNodeDefaultSon(node), "" );
		
			t1 = opData.factory->getMultiDimDecisionDiagram();
		} else {
			OperatorData<T> opData( t1, oldDiagram, t1->lowLimit(), t1->highLimit() );
			opData.nbOpToDo = nbProjRemaining;
		
			GUM_DECISION_DIAGRAM_PROJECTION_SUB_DIAGRAM_OPERATOR_FUNCTION( t1, oldDiagram, opData, NULL, t1->root(), oldDiagram->unsafeNodeDefaultSon(node), "" );	
			delete t1;	
		
			t1 = opData.factory->getMultiDimDecisionDiagram();	
		}
		
	}
		
	return t1;
}
#endif
