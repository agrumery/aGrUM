#ifdef GUM_DECISION_DIAGRAM_PROJECTION_EXPLORATION_FUNCTION_DEFINITION
namespace gum {
template<typename T_DATA>
NodeId
GUM_DECISION_DIAGRAM_PROJECTION_EXPLORATION_FUNCTION( const MultiDimDecisionDiagramBase<T_DATA>* oldDiagram,
        MultiDimDecisionDiagramFactoryBase<T_DATA>* factory,
        NodeId currentNode,
        bool delVarAscendant,
        HashTable< NodeId, NodeId >& explorationTable,
        const Set<const DiscreteVariable*>& delVars,
        Idx nbOperation ) {

    if ( oldDiagram->isTerminalNode( currentNode ) ) {
        T_DATA resValue = oldDiagram->nodeValue( currentNode );
        for ( Idx i = 1; i < nbOperation; i++ )
            resValue = GUM_DECISION_DIAGRAM_PROJECTION_OPERATOR( resValue, oldDiagram->nodeValue( currentNode ) );

        return factory->addTerminalNode( resValue );
    }

    if ( delVarAscendant || delVars.exists( oldDiagram->nodeVariable(currentNode) ) ) {

        if ( !delVarAscendant ) {
            if ( explorationTable.exists( currentNode ) )
                return explorationTable[currentNode];
        }

        nbOperation /= oldDiagram->nodeVariable( currentNode )->domainSize();
        Idx nbExploredModalities = 0;
        std::vector<NodeId>::const_iterator sonIter = oldDiagram->nodeSons( currentNode )->begin();
        T_DATA resValue = GUM_MULTI_DIM_PROJECTION_NEUTRAL;
        while ( sonIter !=  oldDiagram->nodeSons( currentNode )->end() ) {
            if ( *sonIter != 0 ) {
                NodeId sonValueNode = GUM_DECISION_DIAGRAM_PROJECTION_EXPLORATION_FUNCTION( oldDiagram, factory, *sonIter, true, explorationTable, delVars, nbOperation );
                resValue = GUM_DECISION_DIAGRAM_PROJECTION_OPERATOR( resValue, factory->nodeValue( sonValueNode ) );
                nbExploredModalities++;
            }
            ++sonIter;
        }
        if ( oldDiagram->hasNodeDefaultSon( currentNode ) ) {
            NodeId defaultSonValueNode = GUM_DECISION_DIAGRAM_PROJECTION_EXPLORATION_FUNCTION( oldDiagram, factory, oldDiagram->nodeDefaultSon(currentNode),
                                         true, explorationTable, delVars, nbOperation );
            for ( Idx i = 0; i < oldDiagram->nodeVariable(currentNode)->domainSize() - nbExploredModalities; i++ )
                resValue = GUM_DECISION_DIAGRAM_PROJECTION_OPERATOR( resValue, factory->nodeValue( defaultSonValueNode ) );
        }

        nbOperation *= oldDiagram->nodeVariable( currentNode )->domainSize();

        NodeId resNode = factory->addTerminalNode( resValue );
        if ( !delVarAscendant )
            explorationTable.insert( currentNode, resNode );
        return resNode;

    } else {

        if ( explorationTable.exists( currentNode ) )
            return explorationTable[currentNode];

        std::vector<NodeId> sonsMap( oldDiagram->nodeVariable( currentNode )->domainSize(), 0 );
        for ( std::vector<NodeId>::const_iterator sonIter = oldDiagram->nodeSons( currentNode )->begin(); sonIter !=  oldDiagram->nodeSons( currentNode )->end(); ++sonIter ) {
            if ( *sonIter != 0 ) {
                NodeId sonValueNode = GUM_DECISION_DIAGRAM_PROJECTION_EXPLORATION_FUNCTION( oldDiagram, factory, *sonIter, false, explorationTable, delVars, nbOperation );
                sonsMap[ std::distance( oldDiagram->nodeSons( currentNode )->begin(), sonIter ) ] = sonValueNode;
            }
        }
        NodeId defaultSon = 0;
        if ( oldDiagram->hasNodeDefaultSon( currentNode ) ) {
            defaultSon = GUM_DECISION_DIAGRAM_PROJECTION_EXPLORATION_FUNCTION( oldDiagram, factory, oldDiagram->nodeDefaultSon(currentNode),
                         true, explorationTable, delVars, nbOperation );
        }


        if ( defaultSon != 0 ) {
            Idx nbDefault = 0;
            for ( std::vector<NodeId >::iterator iterArcMap = sonsMap.begin(); iterArcMap != sonsMap.end(); ++iterArcMap ) {
                if ( *iterArcMap == 0 )
                    ++nbDefault;
                if ( *iterArcMap == defaultSon ) {
                    ++nbDefault;
                    sonsMap[ std::distance( sonsMap.begin(), iterArcMap ) ] = 0;
                }
            }
            if ( nbDefault == 1 )
                for ( std::vector<NodeId >::iterator iterArcMap = sonsMap.begin(); iterArcMap != sonsMap.end(); ++iterArcMap )
                    if ( *iterArcMap == 0 ) {
                        sonsMap[ std::distance( sonsMap.begin(), iterArcMap ) ] = defaultSon;
                        defaultSon = 0;
                        break;
                    }
        }
        NodeId resNode = factory->addNonTerminalNodeWithArcs( oldDiagram->nodeVariable(currentNode), sonsMap, defaultSon );
        explorationTable.insert( currentNode, resNode );
        return resNode;
    }
}
}/* end of namespace GUM*/
#endif /*GUM_DECISION_DIAGRAM_PROJECTION_EXPLORATION_FUNCTION_DEFINITION*/