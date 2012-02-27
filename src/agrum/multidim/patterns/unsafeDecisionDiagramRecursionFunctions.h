#ifndef GUM_DECISION_DIAGRAM_OPERATOR_DATA
#define GUM_DECISION_DIAGRAM_OPERATOR_DATA
#include <cctype>

    //========================================================================================================================================
    /// OperatorData contains all data structure needed to compute the outcoming diagram.
    //========================================================================================================================================
    template< typename T >
    struct OperatorData {

	/// The factory that build up resulting diagram
	MultiDimDecisionDiagramFactoryBase<T>* factory;

	/// Table giving for each node of second diagram, the list of variables requiring instantiation before
	HashTable< NodeId, Set< const DiscreteVariable* >* >* retrogradeVarTable;

	/// And the global retorgrade var set
	Sequence< const DiscreteVariable* > retroVarSet;

	/// Table to remind us wich part of both diagram have already been explored, and the resulting node
	HashTable< std::vector<Idx>, NodeId >* explorationTable;


	/** **************************************************************************************************
	** Default constructor
	** Takes both decision diagram as parameter, as well as the upper and lower approximation schemes
	** limits for the outcoming diagram
	** ********************************************************************************************** **/
	OperatorData( const MultiDimDecisionDiagramBase<T>* t1, const MultiDimDecisionDiagramBase<T>* t2, T newLowLimit, T newHighLimit ){

	    // =========================================================================================================
	    // First we determine the new var sequence

	    Sequence< const DiscreteVariable* > a1VarSeq = t1->variablesSequence();
	    Sequence< const DiscreteVariable* > a2VarSeq = t2->variablesSequence();
	    Sequence< const DiscreteVariable* > fusVarSeq;	

	    SequenceIterator< const DiscreteVariable* > iterS1 = a1VarSeq.begin(); 
	    SequenceIterator< const DiscreteVariable* > iterS2 = a2VarSeq.begin();

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

	    // =========================================================================================================
	    // Then we instantiate the factory that will create the new multidim
	    // give it its new bounds, and set its variable sequence
	    factory = t1->getFactory( *t1, newLowLimit, newHighLimit);
	    factory->setVariablesSequence( fusVarSeq );

	    // =========================================================================================================
	    // Then we search in second diagram for possible preneeded variable
	    retrogradeVarTable = new HashTable< NodeId, Set< const DiscreteVariable* >* >();
	    t2->findRetrogradeVariables( &( factory->variablesSequence() ), retrogradeVarTable );
	
	    for( HashTableIterator< NodeId, Set< const DiscreteVariable* >* > retroVarIter = retrogradeVarTable->begin(); retroVarIter != retrogradeVarTable->end(); ++retroVarIter )
		if( *retroVarIter != NULL && !(*retroVarIter)->empty())
		    for( SetIterator< const DiscreteVariable* > iter = (*retroVarIter)->begin(); iter != (*retroVarIter)->end(); ++iter )
			if( ! retroVarSet.exists( *iter ) )
			    retroVarSet.insert( *iter );

	    // =========================================================================================================
	    // And we finally instantiates the other data structures
	    explorationTable = new HashTable< std::vector<Idx>, NodeId >();
	}

	~OperatorData(){

	    for( HashTableIterator< NodeId, Set< const DiscreteVariable* >* > iterH = retrogradeVarTable->begin(); iterH != retrogradeVarTable->end(); ++iterH )
		delete *iterH;
	    delete retrogradeVarTable;

	    delete factory;

	    delete explorationTable;

	}
    };


    // ==========================================================================================================
    /// Insertion of a node when we know all its sons and its possible default son
    // ==========================================================================================================
    template<typename T>
    NodeId
    insertNonTerminalNode( OperatorData<T>& opData, const DiscreteVariable* associatedVariable, std::vector< NodeId >& sonsMap, NodeId defaultSon ){

	if( defaultSon != 0 ){
	    Idx nbDefault = 0;
	    for( std::vector<NodeId >::iterator iterArcMap = sonsMap.begin(); iterArcMap != sonsMap.end(); ++iterArcMap ){
		if( *iterArcMap == 0 )
		    ++nbDefault;
		if( *iterArcMap == defaultSon ){
		    ++nbDefault;
		    sonsMap[ std::distance( sonsMap.begin(), iterArcMap ) ] = 0;
		}
	    }
	    if( nbDefault == 1 )
		for( std::vector<NodeId >::iterator iterArcMap = sonsMap.begin(); iterArcMap != sonsMap.end(); ++iterArcMap )
		    if( *iterArcMap == 0 ){
			sonsMap[ std::distance( sonsMap.begin(), iterArcMap ) ] = defaultSon;
			defaultSon = 0;
			break;
		    }
	}
	return opData.factory->unsafeAddNonTerminalNodeWithArcs( associatedVariable, sonsMap, defaultSon );
    }
#endif


#ifdef GUM_DECISION_DIAGRAM_OPERATOR_FUNCTION_DEFINITION
    // A key is used for prunning uneccesary operations since once a node has been visited in a given context, there's no use to revisit him,
    // the result will be the same node, so we just have to do an association context - node.
    // The context consists in :
    //		_ Leader node we are visiting.
    //		_ Follower node we are visiting.
    //		_ For all variables, if it has been instanciated before, current modality instanciated, meaning :
    //			_ 0 means the variable hasn't be instanciated yet,
    //			_ From 1 to domainSize + 1 means that current modality index of variable is value - 1,
    //			_ domainSize + 2 means variable is on default mode.
    // A key - node association is made each time we create a node in resulting diagram.
    // Since GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION is a corner step in algorithm ( meaning each time we explore a node we go trought 
    // this function ), check only have to be at the beginning of that function.
    // ===========================================================================================================
    /// Main recursion function, call every time we move on a node node do determine what we have to do
    // ===========================================================================================================
    template<typename T>
    NodeId
    GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( const MultiDimDecisionDiagramBase<T>* leader,
											  const MultiDimDecisionDiagramBase<T>* follower,
											  OperatorData<T>& opData,
											  const DiscreteVariable* leaderParentVar,
											  std::vector<Idx> key,
											  std::string tabu ) {

	if( opData.explorationTable->exists( key ) )
	    return (*(opData.explorationTable))[key];

	NodeId newNode = 0;


	// ******************************************************************************************************
	// If both current nodes are terminal, 
	// we only have to compute the resulting value
	if( leader->isTerminalNode(key[0]) && follower->isTerminalNode(key[1]) ){

	    // First we have to get pointers on value from those terminal nodes
	    T leaderValue = leader->unsafeNodeValue( key[0] );
	    T followerValue = follower->unsafeNodeValue( key[1] );

	    // Then we have to compute new value
	    // To do so we have to ensure that given value are in order
	    // ( substraction and division aren't commutative for exemple)
	    T newVal = GUM_MULTI_DIM_OPERATOR( leaderValue, followerValue );

	    // And finally we insert a new node in diagram with this value
	    newNode = opData.factory->addTerminalNode( newVal );
	    opData.explorationTable->insert( key, newNode );

	}

	// *******************************************************************************************************
	// If not, first we ensure that all retrograde variable have been considered
    if( !follower->isTerminalNode( key[1] ) )
	if( opData.retrogradeVarTable->exists( key[1] ) ){

	    Idx indexFin = opData.factory->variablesSequence().size();
	    if( !leader->isTerminalNode( key[0] ) )
		indexFin = opData.factory->variablesSequence().pos( leader->unsafeNodeVariable( key[0] ) );

	    Idx indexDebut = 0;
	    if( leaderParentVar != NULL )
		indexDebut = opData.factory->variablesSequence().pos( leaderParentVar ) +1;

	    if( indexFin != indexDebut )
		for( Idx i = indexDebut; i < indexFin; ++i ){
		    const DiscreteVariable* preneededVar = opData.factory->variablesSequence().atPos( i );
		    if( (*opData.retrogradeVarTable)[ key[1] ]->exists( preneededVar ) && ( key[ opData.retroVarSet.pos(preneededVar )  + 2 ] == 0 ) ) {
		      
			Idx indexKey = opData.retroVarSet.pos(preneededVar );

			const List<NodeId>* varNodes = follower->variableNodes( preneededVar );
			std::vector< NodeId > sonsIds( (preneededVar)->domainSize(), 0);

			List<Idx> doneVarModalities;

			for(  ListConstIterator<NodeId> nodesIter = varNodes->begin(); nodesIter != varNodes->end(); ++nodesIter )
			    for( std::vector< NodeId >::const_iterator arcIter = follower->unsafeNodeSons(*nodesIter)->begin(); 
					arcIter != follower->unsafeNodeSons(*nodesIter)->end(); ++arcIter )
				if( *arcIter != 0 ){

				    Idx currentIndex = std::distance( follower->unsafeNodeSons(*nodesIter)->begin(), arcIter );

				    if( !doneVarModalities.exists( currentIndex ) ){

					doneVarModalities.insert( currentIndex );
					key[ indexKey + 2 ] = currentIndex + 1;
					sonsIds[currentIndex] = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, opData, preneededVar, key, tabu );

				    }
				}

			NodeId defaultSon = 0;
			if( doneVarModalities.size() != (preneededVar)->domainSize() ){
			    key[ indexKey + 2 ] = (preneededVar)->domainSize() + 2;
			    defaultSon = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, opData, preneededVar, key, tabu );
			}

			newNode = insertNonTerminalNode( opData, preneededVar, sonsIds, defaultSon );
			key[ indexKey + 2 ] = 0;
			opData.explorationTable->insert( key, newNode );

			return newNode;

		    }
		}
	}
    // *******************************************************************************************************

	// *******************************************************************************************************
	// If only one of the current node is terminal, 
	// we have to pursue deeper on the other diagram
	if( leader->isTerminalNode( key[0] ) && !follower->isTerminalNode( key[1] ) )
	    newNode = GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_FOLLOWER_FUNCTION( leader, follower, opData, leaderParentVar, key, tabu );


	if( !leader->isTerminalNode( key[0] ) && follower->isTerminalNode( key[1] ) )
	    newNode = GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_LEADER_FUNCTION( leader, follower, opData, leaderParentVar, key, tabu );
	// *******************************************************************************************************


	// *******************************************************************************************************
	// Remains the case where both nodes aren't terminal
	if( !leader->isTerminalNode( key[0] ) && !follower->isTerminalNode( key[1] ) ){

	    //==========================================================
	    // First we have to determine the position of current node 
	    // associated variable in the resulting diagram
	    int indexLeader = -1;
	    int indexFollower = -1;

	    if( opData.factory->variablesSequence().exists( leader->unsafeNodeVariable( key[0] ) ) )
		indexLeader = opData.factory->variablesSequence().pos(  leader->unsafeNodeVariable( key[0] ) );
	    if( opData.factory->variablesSequence().exists( follower->unsafeNodeVariable( key[1] ) ) )
		indexFollower = opData.factory->variablesSequence().pos(  follower->unsafeNodeVariable( key[1] ) );
	    if( indexLeader == -1 || indexFollower == -1 )
		GUM_ERROR( FatalError, "Unable to compute MultiDimDecisionDiagramBase");
	    //==========================================================

	    //==========================================================
	    // If one variable preceeds the other on final diagram, we
	    // have to go down on this one
	    if( indexLeader > indexFollower )
		newNode = GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_FOLLOWER_FUNCTION( leader, follower, opData, leaderParentVar, key, tabu );

	    if( indexLeader < indexFollower )
		newNode = GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_LEADER_FUNCTION( leader, follower, opData, leaderParentVar, key, tabu );
	    //==========================================================

	    //==========================================================
	    // If not - meaning it's the same variable - we have to go
	    // down on both
	    if( indexLeader == indexFollower ){

		const std::vector< NodeId >* leaderSonsMap = leader->unsafeNodeSons( key[0] );
		const std::vector< NodeId >* followerSonsMap = follower->unsafeNodeSons( key[1] );

		NodeId leaderCurrentNode = key[0];
		NodeId followerCurrentNode = key[1];

		std::vector< NodeId > sonsIds( leaderSonsMap->size(), 0);
		List<Idx> followerCheckedArcs;
		for( std::vector< NodeId >::const_iterator leaderSonsIter = leaderSonsMap->begin(); leaderSonsIter != leaderSonsMap->end(); ++leaderSonsIter )
		    if( *leaderSonsIter != 0 ){
			Idx leaderSonsKey = std::distance( leaderSonsMap->begin(), leaderSonsIter );

			NodeId followerSon = 0;
			if( (*followerSonsMap)[ leaderSonsKey ] != 0 ){
			    followerSon = (*followerSonsMap)[ leaderSonsKey ];
			    followerCheckedArcs.insert( leaderSonsKey );
			}else
			    followerSon = follower->unsafeNodeDefaultSon( followerCurrentNode );

			key[0] = *leaderSonsIter;
			key[1] = followerSon;
			sonsIds[ leaderSonsKey ] = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, opData, 
					  leader->unsafeNodeVariable( leaderCurrentNode ), key, tabu );
		    }

		for( std::vector< NodeId >::const_iterator followerSonsIter = followerSonsMap->begin(); followerSonsIter != followerSonsMap->end(); ++followerSonsIter)
		    if( *followerSonsIter != 0 ){
			Idx followerSonsKey = std::distance( followerSonsMap->begin(), followerSonsIter );
			if( !followerCheckedArcs.exists( followerSonsKey ) ) {

			    key[0] = leader->unsafeNodeDefaultSon( leaderCurrentNode );
			    key[1] = *followerSonsIter;
			    sonsIds[ followerSonsKey ] = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, opData, leader->unsafeNodeVariable( leaderCurrentNode ),
						      key, tabu );

			}
		    }


		NodeId defaultSon = 0;

		if( leader->unsafeHasNodeDefaultSon( leaderCurrentNode ) && follower->unsafeHasNodeDefaultSon( followerCurrentNode ) ) {
		    key[0] = leader->unsafeNodeDefaultSon( leaderCurrentNode );
		    key[1] = follower->unsafeNodeDefaultSon( followerCurrentNode );
		    defaultSon = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, opData, leader->unsafeNodeVariable( leaderCurrentNode ), key, tabu );
		}

		newNode = insertNonTerminalNode( opData, leader->unsafeNodeVariable( leaderCurrentNode ), sonsIds, defaultSon );
		key[0] = leaderCurrentNode;
		key[1] = followerCurrentNode;
		opData.explorationTable->insert( key, newNode );

	    }
	}
	// *******************************************************************************************************

	return newNode;
    }



    // =========================================================================================================================================================
    /// Called when we investigate a node of the first diagram only
    // =========================================================================================================================================================
    template< typename T >
    NodeId
    GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_LEADER_FUNCTION( const MultiDimDecisionDiagramBase<T>* leader,
														  const MultiDimDecisionDiagramBase<T>* follower,
														  OperatorData<T>& opData,
														  const DiscreteVariable* leaderParentVar,
														  std::vector<Idx> key,
														  std::string tabu ) {

	NodeId newNode = 0;



    Idx nbExploredModalities = 0;
    NodeId leaderCurrentNode = key[0];
    Idx varVectorPos = 0;
    const DiscreteVariable* leaderCurrentVar = leader->unsafeNodeVariable( leaderCurrentNode );
    const std::vector< NodeId >* sonsMap = leader->unsafeNodeSons( leaderCurrentNode );
    std::vector< NodeId > sonsIds( sonsMap->size(), 0);
    const std::vector< bool >* usedModalities = follower->variableUsedModalities( leaderCurrentVar );
    if( opData.retroVarSet.exists( leaderCurrentVar ) )
	varVectorPos = opData.retroVarSet.pos( leaderCurrentVar ) + 2;
    
    // ********************************************************************************************************
    // For each value the current var take on this node, we have to do our computation
    for( std::vector< NodeId>::const_iterator iter = sonsMap->begin(); iter != sonsMap->end(); ++iter){
	Idx sonKey = std::distance( sonsMap->begin(), iter );
	if( *iter != 0 || ( usedModalities != NULL && (*usedModalities)[ sonKey ] ) ){
	    
	    // But we have to indicates to possible node on follower diagram, the current value of the var
	    if( varVectorPos != 0 )
		key[varVectorPos] = sonKey + 1;
	    if( *iter != 0 )
		key[0] = *iter;
	    else
		key[0] = leader->unsafeNodeDefaultSon( leaderCurrentNode );
	    sonsIds[ sonKey ] = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, opData, leaderCurrentVar, 
					key, tabu );
	    
	    ++nbExploredModalities;
	}
    }
    //*********************************************************************************************************
    
    // ********************************************************************************************************
    // Then, if not all possible value of that node have been investigate (meaning we have a default arc)
    // we have to look on second diagram every value that can still take this var (meaning value not taken 
    // on this leader node and the defautl one )
    NodeId defaultSon = 0;
    
    if( leader->unsafeHasNodeDefaultSon( leaderCurrentNode )  && nbExploredModalities != leaderCurrentVar->domainSize() ) {
	key[0] =  leader->unsafeNodeDefaultSon( leaderCurrentNode );
	if( varVectorPos != 0 )
	    key[varVectorPos] = leaderCurrentVar->domainSize() + 2;
	
	defaultSon = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, opData, leaderCurrentVar, key, tabu );

    }

    //*********************************************************************************************************
    
    // ********************************************************************************************************
    // And we finally add this node to our resulting graph
    newNode = insertNonTerminalNode( opData, leaderCurrentVar, sonsIds, defaultSon );

    key[0] = leaderCurrentNode;
    if( varVectorPos != 0 )
	key[varVectorPos] = 0;
    opData.explorationTable->insert( key, newNode );

    return newNode;
}



    // =========================================================================================================================================================
    /// Called when we investigate a node of the second diagram only
    // =========================================================================================================================================================
    template< typename T >
    NodeId
    GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_FOLLOWER_FUNCTION( const MultiDimDecisionDiagramBase<T>* leader,
														      const MultiDimDecisionDiagramBase<T>* follower,
														      OperatorData<T>& opData,
														      const DiscreteVariable* leaderParentVar,
														      std::vector<Idx> key,
														      std::string tabu ) {


	NodeId newNode = 0;

	NodeId followerCurrentNode = key[1];
	const std::vector< NodeId >* sonsMap = follower->unsafeNodeSons( followerCurrentNode );
	Idx varVectorPos = 0;
	if( opData.retroVarSet.exists( follower->unsafeNodeVariable( followerCurrentNode ) ) )
	    varVectorPos = opData.retroVarSet.pos( follower->unsafeNodeVariable( followerCurrentNode ) ) + 2;

	if( varVectorPos != 0 && key[varVectorPos] != 0  ){
	    // ***************************************************************************************************************
	    // If var exists in leader diagram and has already been instantiate to its default value,
	    // we have to do so in second diagram too
	    if( key[varVectorPos] ==  follower->unsafeNodeVariable( followerCurrentNode )->domainSize() + 2 ){

    // 	    GUM_TRACE( tabu << " Saut par fils par défaut " );
		key[1] = follower->unsafeNodeDefaultSon( followerCurrentNode );
		newNode = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, opData, leaderParentVar, key, tabu );
	    
	    // ***************************************************************************************************************
	    }else {
	    // ***************************************************************************************************************
	    // If var exists in leader diagram and has already been instantiate to a value,
	    // we have to go down on this value
    // 	    GUM_TRACE( tabu << " Saut par fils : " << key[varVectorPos] - 1 );
		
		// But we have to check if value has its arc for this node
		if( (*sonsMap)[ key[varVectorPos] - 1 ] != 0 )   
		    key[1] = (*sonsMap)[ key[varVectorPos] - 1 ];
		else
		    key[1] = follower->unsafeNodeDefaultSon( followerCurrentNode );
		
		newNode = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, opData, leaderParentVar, key, tabu );
	    } 
	    // ****************************************************************************************************************
	} else {
	    // ***************************************************************************************************************
	    // If we aren't in one of the above cases, it means that leader diagram hasn't the variable pointed by follower current node.
		std::vector< NodeId > sonsIds( sonsMap->size(), 0 );

		// In that case we do our computation for all sons of that var
		for( std::vector<NodeId>::const_iterator iter = sonsMap->begin(); iter != sonsMap->end(); ++iter)
		    if( *iter != 0 ){
			key[1] = *iter;
			sonsIds[ std::distance( sonsMap->begin(), iter ) ] = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, opData, leaderParentVar, key, tabu );
		    }

		// and for its default son if it has one
		NodeId defaultSon =0;

		if( follower->unsafeHasNodeDefaultSon( followerCurrentNode ) ) {
		    key[1] = follower->unsafeNodeDefaultSon( followerCurrentNode );
		    defaultSon = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( leader, follower, opData, leaderParentVar, key, tabu );
		} 

		// And we had this node to our graph

		newNode = insertNonTerminalNode( opData, follower->unsafeNodeVariable(followerCurrentNode), sonsIds, defaultSon );

		key[1] = followerCurrentNode;
		opData.explorationTable->insert( key, newNode );

	    }
	    // ****************************************************************************************************************
	return newNode;
    }
#endif





#ifdef GUM_DECISION_DIAGRAM_PROJECTION_EXPLORATION_FUNCTION_DEFINITION

    template<typename T_DATA>
    NodeId
    GUM_DECISION_DIAGRAM_PROJECTION_EXPLORATION_FUNCTION( const MultiDimDecisionDiagramBase<T_DATA>* oldDiagram, 
												      MultiDimDecisionDiagramFactoryBase<T_DATA>* factory,
												      NodeId currentNode, 
												      bool delVarAscendant,
												      HashTable< NodeId, NodeId >& explorationTable, 
												      const Set<const DiscreteVariable*>& delVars,
												      Idx nbOperation ){


	if( oldDiagram->isTerminalNode( currentNode ) ) {
	    T_DATA resValue = oldDiagram->unsafeNodeValue( currentNode );
	    for( Idx i = 1; i < nbOperation; i++ )
		resValue = GUM_DECISION_DIAGRAM_PROJECTION_OPERATOR( resValue, oldDiagram->unsafeNodeValue( currentNode ) );

	    return factory->addTerminalNode( resValue );
	}

	if( delVarAscendant || delVars.exists( oldDiagram->unsafeNodeVariable(currentNode) ) ) {

	    if( !delVarAscendant )
		if( explorationTable.exists( currentNode ) )
		    return explorationTable[currentNode];

	    nbOperation /= oldDiagram->unsafeNodeVariable( currentNode )->domainSize();
	    Idx nbExploredModalities = 0;
	    std::vector<NodeId>::const_iterator sonIter = oldDiagram->unsafeNodeSons( currentNode )->begin();
	    T_DATA resValue = GUM_MULTI_DIM_PROJECTION_NEUTRAL;
	    while( sonIter !=  oldDiagram->unsafeNodeSons( currentNode )->end() ) {
		if( *sonIter != 0 ){
		    NodeId sonValueNode = GUM_DECISION_DIAGRAM_PROJECTION_EXPLORATION_FUNCTION( oldDiagram, factory, *sonIter, true, explorationTable, delVars, nbOperation );
		    resValue = GUM_DECISION_DIAGRAM_PROJECTION_OPERATOR( resValue, factory->nodeValue( sonValueNode ) );
		    nbExploredModalities++;
		}
		++sonIter;
	    }
	    if( oldDiagram->unsafeHasNodeDefaultSon( currentNode ) ){
		NodeId defaultSonValueNode = GUM_DECISION_DIAGRAM_PROJECTION_EXPLORATION_FUNCTION( oldDiagram, factory, oldDiagram->unsafeNodeDefaultSon(currentNode), 
			    true, explorationTable, delVars, nbOperation );
		for( Idx i = 0; i < oldDiagram->unsafeNodeVariable(currentNode)->domainSize() - nbExploredModalities; i++ )
		    resValue = GUM_DECISION_DIAGRAM_PROJECTION_OPERATOR( resValue, factory->nodeValue( defaultSonValueNode ) );
	    }

	    nbOperation *= oldDiagram->unsafeNodeVariable( currentNode )->domainSize();

	    NodeId resNode = factory->addTerminalNode( resValue );
	    if( !delVarAscendant )
		explorationTable.insert( currentNode, resNode );
	    return resNode;

	}else{

	    if( explorationTable.exists( currentNode ) )
		return explorationTable[currentNode];

	    std::vector<NodeId> sonsMap( oldDiagram->unsafeNodeVariable( currentNode )->domainSize(), 0 );
	    for( std::vector<NodeId>::const_iterator sonIter = oldDiagram->unsafeNodeSons( currentNode )->begin(); sonIter !=  oldDiagram->unsafeNodeSons( currentNode )->end(); ++sonIter ){
		if( *sonIter != 0 ){
		    NodeId sonValueNode = GUM_DECISION_DIAGRAM_PROJECTION_EXPLORATION_FUNCTION( oldDiagram, factory, *sonIter, false, explorationTable, delVars, nbOperation );
		    sonsMap[ std::distance( oldDiagram->unsafeNodeSons( currentNode )->begin(), sonIter ) ] = sonValueNode;
		}
	    }
	    NodeId defaultSon = 0;
	    if( oldDiagram->unsafeHasNodeDefaultSon( currentNode ) )
		defaultSon = GUM_DECISION_DIAGRAM_PROJECTION_EXPLORATION_FUNCTION( oldDiagram, factory, oldDiagram->unsafeNodeDefaultSon(currentNode), 
			    true, explorationTable, delVars, nbOperation );


	    if( defaultSon != 0 ){
		Idx nbDefault = 0;
		for( std::vector<NodeId >::iterator iterArcMap = sonsMap.begin(); iterArcMap != sonsMap.end(); ++iterArcMap ){
		    if( *iterArcMap == 0 )
			++nbDefault;
		    if( *iterArcMap == defaultSon ){
			++nbDefault;
			sonsMap[ std::distance( sonsMap.begin(), iterArcMap ) ] = 0;
		    }
		}
		if( nbDefault == 1 )
		    for( std::vector<NodeId >::iterator iterArcMap = sonsMap.begin(); iterArcMap != sonsMap.end(); ++iterArcMap )
			if( *iterArcMap == 0 ){
			    sonsMap[ std::distance( sonsMap.begin(), iterArcMap ) ] = defaultSon;
			    defaultSon = 0;
			    break;
			}
	    }
	    NodeId resNode = factory->addNonTerminalNodeWithArcs( oldDiagram->unsafeNodeVariable(currentNode), sonsMap, defaultSon );

	    explorationTable.insert( currentNode, resNode );
	    return resNode;
	}
    }
#endif
