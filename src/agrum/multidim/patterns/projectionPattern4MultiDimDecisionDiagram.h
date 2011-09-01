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
/** @file
 * @brief the pattern used by all the projections of multidimensional tables
 *
 * @author Jean-Christophe Magnan
 */


// check if we allowed these patterns to be used
#ifndef GUM_PROJECTION_PATTERN_ALLOWED

#warning To use projectionPattern, you must define GUM_PROJECTION_PATTERN_ALLOWED

#else

// ================================================================================
/// a specialized function for projecting a multiDimDecisionDiagram over a subset of its vars
// ================================================================================
#ifdef GUM_DECISION_DIAGRAM_PROJECTION_NAME

	#define GUM_MULTI_DIM_PROJECTION_TYPE T_DATA

	template<typename T_DATA>
	MultiDimDecisionDiagramBase<T_DATA>*
	GUM_DECISION_DIAGRAM_PROJECTION_NAME( const MultiDimDecisionDiagramBase<T_DATA>* table, const Set<const DiscreteVariable *>& del_vars ) {
	  
#endif

    
#ifdef GUM_DECISION_DIAGRAM_PROJECTION_IMPL2DECISION_DIAGRAM_NAME

	#define GUM_MULTI_DIM_PROJECTION_TYPE T_DATA

	template<typename T_DATA>
	MultiDimImplementation<T_DATA>*
	GUM_DECISION_DIAGRAM_PROJECTION_IMPL2DECISION_DIAGRAM_NAME( const MultiDimImplementation<T_DATA>* ttable, const Set<const DiscreteVariable *>& del_vars ) {
		
		const MultiDimDecisionDiagramBase<T_DATA>* table = reinterpret_cast<const MultiDimDecisionDiagramBase<T_DATA>*> (ttable);
		
#endif

	MultiDimDecisionDiagramBase< GUM_MULTI_DIM_PROJECTION_TYPE >* ret = reinterpret_cast<MultiDimDecisionDiagramBase< T_DATA >*>( table->newFactory() );
	ret->copy( *table );
		
	//~ GUM_TRACE( ret->toDot() );
	MultiDimDecisionDiagramFactoryBase< GUM_MULTI_DIM_PROJECTION_TYPE >* Resfactory = table->getFactory( );
	Sequence< const DiscreteVariable* > varsSeq = table->variablesSequence();
	
		
	// ************************************************************************************
	// For each deleted variable
	for( SetIterator< const DiscreteVariable* > delVarsIter = del_vars.begin(); delVarsIter != del_vars.end(); ++delVarsIter ){
		
		//~ GUM_TRACE( "Variable to eleminate : " << (*delVarsIter)->name() << endl );
		
		// ************************************************************************************
		// Deleted var needs to be removed from our global sequence of variable
		if( varsSeq.exists( *delVarsIter ) )
			varsSeq.erase( *delVarsIter );
		Resfactory->setVariablesSequence( varsSeq );
		
		
		// ************************************************************************************
		// Setting up the handler on current diagram
			
			
		HashTable< NodeId, NodeId > nodeMap;
		
		// ************************************************************************************
		// If root node is terminal, we have nothing to worried about
		if( ret->isTerminalNode( ret->root() ) ) {
			Resfactory->addTerminalNode( ret->nodeValue( ret->root() ) );
			delete ret;
			ret = Resfactory->getMultiDimDecisionDiagram();
			Resfactory->clear();
		}
			
			
		// ************************************************************************************
		// if not, we have to do more
		else {
				
			// ************************************************************************************
			// First, we have to ensure that root node is node tied to our deleted variable
			// In this case, it gets a lot simplier
			if( ret->nodeVariable( ret->root() ) == *delVarsIter ) {
				//~ GUM_TRACE( "Projection depuis racine." << std::endl );
				MultiDimDecisionDiagramBase<T_DATA>* oldDiagram = GUM_DECISION_DIAGRAM_PROJECTION_ELEMINATE_NODE_FUNCTION( ret, ret->root(), (*delVarsIter)->domainSize() );
				Resfactory->insertSubDecisionDiagram( oldDiagram );
				delete oldDiagram;
				delete ret;
				ret = Resfactory->getMultiDimDecisionDiagram();
				Resfactory->clear();
				//~ GUM_TRACE( "Résultat : " << std::endl << ret->toDot() );
			}
				
			else {
				
				const MultiDimDecisionDiagramBase<T_DATA>* oldDiagram = ret;
				
				//~ GUM_TRACE( oldDiagram->toDot() );
			
				nodeMap.insert( oldDiagram->root(), Resfactory->addNonTerminalNode( oldDiagram->nodeVariable( oldDiagram->root() ) ) );
					
				std::vector<NodeId> fifo;
				fifo.push_back( oldDiagram->root() );
			
				while( !fifo.empty() ){
				
					// ************************************************************************************
					// Recovering current node id and its associated node id in building diagram
					NodeId currentNodeId = fifo.back();
					fifo.pop_back();
						
					// ************************************************************************************
					// For each sons
					const HashTable< Idx, NodeId >* sonsMap = oldDiagram->nodeSons( currentNodeId );
					for( HashTableConstIterator< Idx, NodeId > sonsIter = sonsMap->begin(); sonsIter != sonsMap->end(); ++sonsIter ){
						
						bool subDiagramJustInstanciated = false;
						// ************************************************************************************
						// If node is not in nodeMap, it means that we haven't inserted him yet in resulting diagram
						if( !nodeMap.exists( *sonsIter ) ){
							
							// ************************************************************************************
							// If node is a terminal one, we simply insert it
							if( oldDiagram->isTerminalNode( *sonsIter ) ){
								GUM_MULTI_DIM_PROJECTION_TYPE nonProjectedVal = oldDiagram->nodeValue( *sonsIter );
								GUM_MULTI_DIM_PROJECTION_TYPE projectedVal = nonProjectedVal;
								for( Idx i = 1; i < (*delVarsIter)->domainSize(); ++i )
									projectedVal = GUM_DECISION_DIAGRAM_PROJECTION_OPERATOR( projectedVal, nonProjectedVal );
								nodeMap.insert( *sonsIter, Resfactory->addTerminalNode( projectedVal ) );
								
							} else {
								
								// ************************************************************************************
								// If node is not, we first checked if associated variable isn't in our projected list
								if( *delVarsIter == oldDiagram->nodeVariable( *sonsIter ) ){
									
									// ************************************************************************************
									// If so, projection is done by operating sub diagram rooted by node sons
									MultiDimDecisionDiagramBase< GUM_MULTI_DIM_PROJECTION_TYPE >* projectedDiagram = 
														GUM_DECISION_DIAGRAM_PROJECTION_ELEMINATE_NODE_FUNCTION( oldDiagram, *sonsIter, (*delVarsIter)->domainSize() );
									
									//~ GUM_TRACE( "Projection res : " << std::endl << projectedDiagram->toDot() );
									
									nodeMap.insert( *sonsIter, Resfactory->insertSubDecisionDiagram( projectedDiagram, nodeMap[ currentNodeId ], sonsIter.key() ) );
									
									//~ GUM_TRACE( "And on factory : " << std::endl );									
									//~ Resfactory->showProperties();
									
									delete projectedDiagram;
									subDiagramJustInstanciated = true;
									
								} else {
									
									nodeMap.insert( *sonsIter, Resfactory->addNonTerminalNode( oldDiagram->nodeVariable( *sonsIter ) ) );
								
									// And we also have to insert him in our fifo to explore him later
									fifo.push_back( *sonsIter );
								}
							}
						}
						
						// ************************************************************************************
						// Next we can insert arc in between
						if( !subDiagramJustInstanciated ){
							//~ GUM_TRACE( " From : " << nodeMap[ currentNodeId ] << " - To : " << nodeMap[*sonsIter] << " - Value : " << sonsIter.key() << std::endl );
							//~ Resfactory->showProperties();
							Resfactory->insertArc( nodeMap[ currentNodeId ], nodeMap[ *sonsIter ], sonsIter.key() );
							//~ GUM_TRACE( "Done!" << std::endl );
						}
					}
					
					// ************************************************************************************
					// And if node has a default son, we do quiet the same with it
					if( oldDiagram->hasNodeDefaultSon( currentNodeId ) ){
						
						NodeId currentNodeDefaultSon = oldDiagram->nodeDefaultSon( currentNodeId );						
						bool subDiagramJustInstanciated = false;
						
						if( !nodeMap.exists( currentNodeDefaultSon ) ){
							
							if( oldDiagram->isTerminalNode( currentNodeDefaultSon ) ){
								GUM_MULTI_DIM_PROJECTION_TYPE nonProjectedVal = oldDiagram->nodeValue( currentNodeDefaultSon );
								GUM_MULTI_DIM_PROJECTION_TYPE projectedVal = nonProjectedVal;
								for( Idx i = 1; i < (*delVarsIter)->domainSize(); ++i )
									projectedVal = GUM_DECISION_DIAGRAM_PROJECTION_OPERATOR( projectedVal, nonProjectedVal );
								nodeMap.insert( currentNodeDefaultSon, Resfactory->addTerminalNode( projectedVal ) );
								
							} else {
								
								if( *delVarsIter == oldDiagram->nodeVariable( currentNodeDefaultSon ) ){
									
									// ************************************************************************************
									// If so, projection is done by operating sub diagram rooted by node sons
									MultiDimDecisionDiagramBase< GUM_MULTI_DIM_PROJECTION_TYPE >* projectedDiagram =  
														GUM_DECISION_DIAGRAM_PROJECTION_ELEMINATE_NODE_FUNCTION( oldDiagram, currentNodeDefaultSon, (*delVarsIter)->domainSize() );
									
									//~ GUM_TRACE( "Projection res sur default : " << std::endl << projectedDiagram->toDot() );
									
									nodeMap.insert( currentNodeDefaultSon, Resfactory->insertSubDecisionDiagram( projectedDiagram, nodeMap[ currentNodeId ] ) );
									
									//~ GUM_TRACE( "And on factory : " << std::endl );									
									//~ Resfactory->showProperties();
									
									delete projectedDiagram;
									subDiagramJustInstanciated = true;
									
								} else {
									nodeMap.insert( currentNodeDefaultSon, Resfactory->addNonTerminalNode( oldDiagram->nodeVariable( currentNodeDefaultSon ) ) );
								
									fifo.push_back( currentNodeDefaultSon );
								}
							}
						}
							
						if( !subDiagramJustInstanciated ){
							//~ GUM_TRACE(  " From : " << nodeMap[ currentNodeId ] << " - To : " << nodeMap[currentNodeDefaultSon] << std::endl );
							//~ Resfactory->showProperties();
							Resfactory->insertDefaultArc( nodeMap[ currentNodeId ], nodeMap[ currentNodeDefaultSon ] );
							//~ GUM_TRACE( "Done!" << std::endl );
						}					
					}
				}
				
				delete ret;
				ret = Resfactory->getMultiDimDecisionDiagram();
				Resfactory->clear();
				//~ GUM_TRACE( "Résultat : " << std::endl << ret->toDot() );
			}
		}
		//~ GUM_TRACE( "Fin elemination : " << (*delVarsIter)->name() << " - Resulting diagram : " << std::endl << ret->toDot() );
	}
	
	delete Resfactory;
	return ret;
}


#undef GUM_MULTI_DIM_PROJECTION_TYPE  


#endif /* GUM_PROJECTION_PATTERN_ALLOWED */
