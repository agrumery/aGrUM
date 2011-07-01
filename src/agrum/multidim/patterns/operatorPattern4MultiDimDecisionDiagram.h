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
 * @brief the pattern used by all binary MultiDimDecisionDiagramBases operators
 *
 * @author Jean-Christophe MAGNAN */


// check if we allowed these patterns to be used
#ifndef GUM_OPERATOR_PATTERN_ALLOWED

#warning To use operatorPattern4MultiDimDecisionDiagram.h, you must define GUM_OPERATOR_PATTERN_ALLOWED

#else

// ================================================================================
/// a specialized function for combining two MultiDimDecisionDiagrams
// ================================================================================
#ifdef GUM_MULTI_DIM_OPERATOR_NAME
	#define GUM_MULTI_DIM_OPERATOR_TYPE T
							
	template<typename T>
	MultiDimDecisionDiagramBase<T>*
	GUM_MULTI_DIM_OPERATOR_NAME ( const MultiDimDecisionDiagramBase<T>* t1,
					const MultiDimDecisionDiagramBase<T>* t2 ) {
#endif
    

#ifdef GUM_MULTI_DIM_OPERATOR_NAME_F
	#define GUM_MULTI_DIM_OPERATOR_TYPE T
								
	template<typename T>
	MultiDimDecisionDiagramBase<T>*
	GUM_MULTI_DIM_OPERATOR_NAME_F ( const MultiDimDecisionDiagramBase<T>* t1,
				          const MultiDimDecisionDiagramBase<T>* t2,
				          const T (*f) ( const T&, const T&) ) {
#endif

    
#ifdef GUM_MULTI_DIM_OPERATOR_IMPL2DECISION_DIAGRAM_NAME
	#define GUM_MULTI_DIM_OPERATOR_TYPE T
								
	template<typename T>
	MultiDimImplementation<T>*
	GUM_MULTI_DIM_OPERATOR_IMPL2DECISION_DIAGRAM_NAME ( const MultiDimImplementation<T>* tt1,
		                          	 const MultiDimImplementation<T>* tt2 ) {
	const MultiDimDecisionDiagramBase<T>* t1 = reinterpret_cast<const MultiDimDecisionDiagramBase<T>*> (tt1);
	const MultiDimDecisionDiagramBase<T>* t2 = reinterpret_cast<const MultiDimDecisionDiagramBase<T>*> (tt2);
#endif
	
	
	if( t1->empty() || t2->empty() )
		return NULL;
	
	// ======================================================================================================
	// First we have to determine both high and low limit of the new multidim
	// This is perform by computing both limit of each multidims by the other's one
	// Then we seek for the highest and the lowest of those value	
	
	GUM_MULTI_DIM_OPERATOR_TYPE a1LowLimit = t1->lowLimit();
	GUM_MULTI_DIM_OPERATOR_TYPE a2LowLimit = t2->lowLimit();
	GUM_MULTI_DIM_OPERATOR_TYPE a1HighLimit = t1->highLimit();
	GUM_MULTI_DIM_OPERATOR_TYPE a2HighLimit = t2->highLimit();
	
	GUM_MULTI_DIM_OPERATOR_TYPE newHighLimit = GUM_MULTI_DIM_OPERATOR( a1LowLimit, a2LowLimit );	
	GUM_MULTI_DIM_OPERATOR_TYPE newLowLimit = GUM_MULTI_DIM_OPERATOR( a1LowLimit, a2LowLimit );
	
	GUM_MULTI_DIM_OPERATOR_TYPE newVal = GUM_MULTI_DIM_OPERATOR( a1LowLimit, a2HighLimit );
	if( newHighLimit < newVal )
		newHighLimit = newVal;
	if( newLowLimit > newVal )
		newLowLimit = newVal;
	
	newVal = GUM_MULTI_DIM_OPERATOR( a1HighLimit, a2LowLimit );
	if( newHighLimit < newVal )
		newHighLimit = newVal;
	if( newLowLimit > newVal )
		newLowLimit = newVal;
		
	newVal = GUM_MULTI_DIM_OPERATOR( a1HighLimit, a2HighLimit );
	if( newHighLimit < newVal )
		newHighLimit = newVal;
	if( newLowLimit > newVal )
		newLowLimit = newVal;
	// =========================================================================================================
		
	// =========================================================================================================
	// Then we determine the new var sequence
	
	Sequence< const DiscreteVariable* > a1VarSeq = t1->variablesSequence();
	Sequence< const DiscreteVariable* > a2VarSeq = t2->variablesSequence();
	Sequence< const DiscreteVariable* >* fusVarSeq = new Sequence< const DiscreteVariable* >();
	
	SequenceIterator< const DiscreteVariable* > iterS1 = a1VarSeq.begin(); 
	while( iterS1 != a1VarSeq.end() )
		if( !t1->isInDiagramVariable( *iterS1 ) ){
			if( !t2->isInDiagramVariable( *iterS1 ) )
				fusVarSeq->insert( *iterS1 );
			a1VarSeq.erase( iterS1 );
		}else
			 ++iterS1;
						
	SequenceIterator< const DiscreteVariable* > iterS2 = a2VarSeq.begin();
	while( iterS2 != a2VarSeq.end() )
		if( !t2->isInDiagramVariable( *iterS2 ) )
			a2VarSeq.erase( *iterS2 );
		else
			++iterS2;
			
	iterS1 = a1VarSeq.begin();
	iterS2 = a2VarSeq.begin();
	
	while( iterS1 != a1VarSeq.end() || iterS2 != a2VarSeq.end() ){
		if( iterS1 == a1VarSeq.end() ){
			for( ; iterS2 != a2VarSeq.end(); ++iterS2 )
				if( !fusVarSeq->exists(*iterS2) )
					fusVarSeq->insert( *iterS2 );
		}
		else if( iterS2 == a2VarSeq.end() ){
			for( ; iterS1 != a1VarSeq.end(); ++iterS1 )
				if( !fusVarSeq->exists(*iterS1) )
					fusVarSeq->insert( *iterS1 );
		} else {
			if( *iterS1 == *iterS2 ){
				if( !fusVarSeq->exists(*iterS1) )
					fusVarSeq->insert( *iterS1 );
				++iterS1;
				++iterS2;
				continue;
			}
			if( a1VarSeq.pos(*iterS1) <= a2VarSeq.pos(*iterS2) || a1VarSeq.exists( *iterS2 ) ){
				if( !fusVarSeq->exists(*iterS1) )
					fusVarSeq->insert( *iterS1 );
				++iterS1;
				continue;
			} else {
				if( !fusVarSeq->exists(*iterS2) )
					fusVarSeq->insert( *iterS2 );
				++iterS2;
				continue;
			} 
		}
	}
			
	//~ std::cout << std::endl << " Mixed Sequence variable : ";
	//~ for( SequenceIterator< const DiscreteVariable* > iter = fusVarSeq->begin(); iter != fusVarSeq->end(); ++iter )
		//~ std::cout << (*iter)->toString() << " - ";
	//~ std::cout << std::endl;
	
	// =========================================================================================================
		
	// =========================================================================================================
	// Then we search in second diagram for possible preneeded variable
	HashTable< NodeId, Set< const DiscreteVariable* >* >* preneededVarTable = new HashTable< NodeId, Set< const DiscreteVariable* >* >();
	Sequence< const DiscreteVariable* > pathVarOrder;
	t2->getPreceedingsVariable ( fusVarSeq, preneededVarTable );
	// =========================================================================================================
		
	// =========================================================================================================
	// Then we instantiate the factory that will create the new multidim
	// and we give it the new bound
		
	MultiDimDecisionDiagramFactoryBase< GUM_MULTI_DIM_OPERATOR_TYPE >* Resfactory = t1->getFactory( *t1, newLowLimit, newHighLimit);
	Resfactory->specifyVariablesSequence( *fusVarSeq );
	// ==========================================================================================================	
	
	// =========================================================================================================
	// And we finally call the recursif methods that will build the diagram
	
	HashTable< const DiscreteVariable*, Idx >* instantiateVariable = new HashTable< const DiscreteVariable*, Idx >();	
	List< const DiscreteVariable* >* defaultInstantiateVariable = new List< const DiscreteVariable* >();
		
	GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( t1, t2, Resfactory, fusVarSeq, preneededVarTable, instantiateVariable, defaultInstantiateVariable, NULL, t1->getRoot(), t2->getRoot(), "" );

	delete instantiateVariable;
	delete defaultInstantiateVariable;
	delete fusVarSeq;
	for( HashTableIterator< NodeId, Set< const DiscreteVariable* >* > iterH = preneededVarTable->begin(); iterH != preneededVarTable->end(); ++iterH )
		delete *iterH;
	delete preneededVarTable;
	
	MultiDimDecisionDiagramBase< GUM_MULTI_DIM_OPERATOR_TYPE >* ret = Resfactory->getMultiDimDecisionDiagram();
	
	delete Resfactory;
	
	return ret;
}
#undef GUM_MULTI_DIM_OPERATOR_TYPE

  
#endif /* GUM_OPERATOR_PATTERN_ALLOWED */
