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
	
	MultiDimDecisionDiagramBase< GUM_MULTI_DIM_OPERATOR_TYPE >* ret = NULL;
	
	if( ( t1 == NULL || t1->empty() ) && ( t2 == NULL || t2->empty() ) )
		return ret;

	if( t1 == NULL || t1->empty() ){
		ret = reinterpret_cast<MultiDimDecisionDiagramBase<T>*>( t2->newFactory() );
		ret->copy( *t2 );
		return ret;
	}
	
	if( t2 == NULL || t2->empty() ){
		ret = reinterpret_cast<MultiDimDecisionDiagramBase<T>*>( t1->newFactory() );
		ret->copy( *t1 );
		return ret;
	}
	
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
	//~ std::cout << "Début opération" << std::endl;
	OperatorData<T> opData( t1, t2, newLowLimit, newHighLimit );
	
	GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION( t1, t2, opData, NULL, t1->root(), t2->root(), "" );
	
	//~ std::cout << "Fin opération" << std::endl;
	ret = opData.factory->getMultiDimDecisionDiagram();
	
	return ret;
}
#undef GUM_MULTI_DIM_OPERATOR_TYPE

  
#endif /* GUM_OPERATOR_PATTERN_ALLOWED */
