template<typename T>
NodeId
GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( 	const MultiDimDecisionDiagramBase<T>* a1, 
									const MultiDimDecisionDiagramBase<T>* a2, 
									MultiDimDecisionDiagramFactoryBase<T>* factory, 
									Sequence< const DiscreteVariable* > fusVarList, 
									NodeId n1, 
									NodeId n2, 
									int nbPermutation ) {

	NodeId newNode = 0;	
	
	
	if( a1->isTerminalNode(n1) && a2->isTerminalNode(n2) ){	
		
		T v1 = a1->getValueFromNode( n1 );
		T v2 = a2->getValueFromNode( n2 );
				
		T *pv1 = const_cast<T*> ( &( v1 ) );
		T *pv2 = const_cast<T*> ( &( v2 ) ); 
		
		T newVal = 0;
		if( nbPermutation%2 == 0 )
			newVal = GUM_MULTI_DIM_OPERATOR( *pv1, *pv2 );
		else
			newVal = GUM_MULTI_DIM_OPERATOR( *pv2, *pv1 );
		newNode = factory->addTerminalNode( newVal );
	}
	

	if( a1->isTerminalNode(n1) && !a2->isTerminalNode(n2) )
		newNode = GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_FUNCTION( a2, a1, factory, fusVarList, n2, n1, nbPermutation + 1 );
		

	if( !a1->isTerminalNode(n1) && a2->isTerminalNode(n2) )	
		newNode = GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_FUNCTION( a1, a2, factory, fusVarList, n1, n2, nbPermutation );
		

	if( !a1->isTerminalNode(n1) && !a2->isTerminalNode(n2) ){
		
		int index1 = -1;
		int index2 = -1;
		if( fusVarList.exists( a1->getVariableFromNode( n1 ) ) )
			index1 = fusVarList.pos(  a1->getVariableFromNode( n1 ) );
		if( fusVarList.exists( a2->getVariableFromNode( n2 ) ) )
			index2 = fusVarList.pos(  a2->getVariableFromNode( n2 ) );
		if( index1 == -1 || index2 == -1 )
					GUM_ERROR( FatalError, "Unable to compute MultiDimDecisionDiagramBase");
				
		if( index1 > index2 )	
			newNode = GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_FUNCTION( a2, a1, factory, fusVarList, n2, n1, nbPermutation + 1 );
			
		if( index1 < index2 )
			newNode = GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_FUNCTION( a1, a2, factory, fusVarList, n1, n2, nbPermutation );

		if( index1 == index2 ){

			const HashTable< Idx, NodeId >* sons1Map = a1->getNodeSons(n1);
			const HashTable< Idx, NodeId >* sons2Map = a2->getNodeSons(n2);
			HashTable< Idx, NodeId > sonsIds;
			List<Idx> n2CheckedArcs;
			for( HashTableConstIterator<Idx, NodeId> iter = sons1Map->begin(); iter != sons1Map->end(); ++iter){

				if( sons2Map->exists( iter.key() ) ){
					NodeId sn2 = (*sons2Map)[ iter.key() ];
					n2CheckedArcs.insert( iter.key() );
	
					sonsIds.insert( iter.key(), GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( a1, a2, factory, fusVarList, *iter, sn2, nbPermutation ) );
					
				} else {
					sonsIds.insert( iter.key(), GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( a1, a2, factory, fusVarList, *iter, a2->getNodeDefaultSon( n2 ), nbPermutation ) );
				}
					
			}
			
			for( HashTableConstIterator<Idx, NodeId> iter = sons2Map->begin(); iter != sons2Map->end(); ++iter)
				if( !n2CheckedArcs.exists( iter.key() ) )
					sonsIds.insert( iter.key(), GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( a1, a2, factory, fusVarList, a1->getNodeDefaultSon( n1 ), *iter, nbPermutation  ) );
			
			
			if( a1->hasNodeDefaultSon( n1 ) && a2->hasNodeDefaultSon( n2 ) ) {
				NodeId defaultSon = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( a1, a2, factory, fusVarList, a1->getNodeDefaultSon( n1 ), a2->getNodeDefaultSon( n2 ), nbPermutation );
				newNode = factory->addNonTerminalNodeWithArcs( a1->getVariableFromNode(n1), sonsIds, &defaultSon );
			}else{
				newNode = factory->addNonTerminalNodeWithArcs( a1->getVariableFromNode(n1), sonsIds );
			}

		}
	}
 
	return newNode;
}

	
	
							
template< typename T >
NodeId
GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_FUNCTION( const MultiDimDecisionDiagramBase<T>* a1, 
												const MultiDimDecisionDiagramBase<T>* a2, 
												MultiDimDecisionDiagramFactoryBase<T>* factory, 
												Sequence< const DiscreteVariable* > fusVarList, 
												NodeId n1, 
												NodeId n2, 
												int nbPermutation ) {

	NodeId newNode = 0;

	const HashTable< Idx, NodeId >* sonsMap = a1->getNodeSons(n1);
	
	HashTable< Idx, NodeId > sonsIds;
	for( HashTableConstIterator<Idx, NodeId> iter = sonsMap->begin(); iter != sonsMap->end(); ++iter){
		sonsIds.insert( iter.key(), GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( a1, a2, factory, fusVarList, *iter, n2, nbPermutation ) );
	}

	if( a1->hasNodeDefaultSon( n1 ) ) {
		NodeId defaultSon = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( a1, a2, factory, fusVarList, a1->getNodeDefaultSon( n1 ), n2, nbPermutation );
		newNode = factory->addNonTerminalNodeWithArcs( a1->getVariableFromNode(n1), sonsIds, &defaultSon );
	} else {
		newNode = factory->addNonTerminalNodeWithArcs( a1->getVariableFromNode(n1), sonsIds );
	}
	return newNode;
}
