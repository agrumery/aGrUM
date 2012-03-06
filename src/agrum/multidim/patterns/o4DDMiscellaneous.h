/*********************************************************************************
 *    Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES  	*
 *   {prenom.nom}_at_lip6.fr                                                                           	*
 *                                                                                                                     	*
 *   This program is free software; you can redistribute it and/or modify         	*
 *   it under the terms of the GNU General Public License as published by      	*
 *   the Free Software Foundation; either version 2 of the License, or            	*
 *   (at your option) any later version.                                                             	*
 *                                                                                                                     	*
 *   This program is distributed in the hope that it will be useful,                     	*
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          	*
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         	*
 *   GNU General Public License for more details.                                             	*
 *                                                                                                                     	*
 *   You should have received a copy of the GNU General Public License         	*
 *   along with this program; if not, write to the                                               	*
 *   Free Software Foundation, Inc.,                                                                	*
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.                     	*
 ********************************************************************************/
/**
* @file
* @brief Files containing structures and functions used to manipulate data structures in Decision Diagram Operations
*
* @author Jean-Christophe Magnan
*/
// ============================================================================
#ifndef GUM_DECISION_DIAGRAM_OPERATOR_DATA
#define GUM_DECISION_DIAGRAM_OPERATOR_DATA
// ============================================================================
#include <agrum/core/hashTable.h>
#include <agrum/core/set.h>
// ============================================================================
#include <agrum/multidim/discreteVariable.h>
#include <agrum/multidim/multiDimDecisionDiagramBase.h>
#include <agrum/multidim/multiDimDecisionDiagramFactoryBase.h>
#include <agrum/multidim/patterns/o4DDContext.h>
// ============================================================================

namespace gum {

//========================================================================================================================================
/// OperatorData contains all data structure needed to compute the outcoming diagram.
//========================================================================================================================================
template< typename T >
struct OperatorData {

    /// The factory that build up resulting diagram
    MultiDimDecisionDiagramFactoryBase<T>* factory;

    /// Table giving for each node of second diagram, the list of variables requiring instantiation before
    HashTable< NodeId, Set< const DiscreteVariable* >* >* retrogradeVarTable;

    /// Context handler
    O4DDContext conti;

    /// Table to remind us wich part of both diagram have already been explored, and the resulting node
    HashTable< double, NodeId >* explorationTable;


    /** **************************************************************************************************
      ** Default constructor
      ** Takes both decision diagram as parameter, as well as the upper and lower approximation schemes
      ** limits for the outcoming diagram
      ** ********************************************************************************************** **/
    OperatorData( const MultiDimDecisionDiagramBase<T>* t1, const MultiDimDecisionDiagramBase<T>* t2, T newLowLimit, T newHighLimit, bool withoutOrder = true ) {

        // =========================================================================================================
        // First we determine the new var sequence

        Sequence< const DiscreteVariable* > fusVarSeq;

        if ( withoutOrder ) {
            Sequence< const DiscreteVariable* > a1VarSeq = t1->variablesSequence();
            Sequence< const DiscreteVariable* > a2VarSeq = t2->variablesSequence();

            SequenceIterator< const DiscreteVariable* > iterS1 = a1VarSeq.begin();
            SequenceIterator< const DiscreteVariable* > iterS2 = a2VarSeq.begin();

            while ( iterS1 != a1VarSeq.end() || iterS2 != a2VarSeq.end() ) {
                if ( iterS1 == a1VarSeq.end() ) {
                    for ( ; iterS2 != a2VarSeq.end(); ++iterS2 )
                        if ( !fusVarSeq.exists(*iterS2) )
                            fusVarSeq.insert( *iterS2 );
                }
                else if ( iterS2 == a2VarSeq.end() ) {
                    for ( ; iterS1 != a1VarSeq.end(); ++iterS1 )
                        if ( !fusVarSeq.exists(*iterS1) )
                            fusVarSeq.insert( *iterS1 );
                } else {
                    if ( *iterS1 == *iterS2 ) {
                        if ( !fusVarSeq.exists(*iterS1) )
                            fusVarSeq.insert( *iterS1 );
                        ++iterS1;
                        ++iterS2;
                        continue;
                    }
                    if ( a1VarSeq.pos(*iterS1) <= a2VarSeq.pos(*iterS2) || a1VarSeq.exists( *iterS2 ) ) {
                        if ( !fusVarSeq.exists(*iterS1) )
                            fusVarSeq.insert( *iterS1 );
                        ++iterS1;
                        continue;
                    } else {
                        if ( !fusVarSeq.exists(*iterS2) )
                            fusVarSeq.insert( *iterS2 );
                        ++iterS2;
                        continue;
                    }
                }
            }
        } else {
            fusVarSeq = t1->variablesSequence();
        }

#ifdef O4DDDEBUG
GUM_TRACE( " Mixed Sequence variable : " ); for( SequenceIterator< const DiscreteVariable* > iter = fusVarSeq.begin(); iter != fusVarSeq.end(); ++iter )
GUM_TRACE( (*iter)->toString() << " - " );
GUM_TRACE( std::endl );
#endif

        // =========================================================================================================
        // Then we instantiate the factory that will create the new multidim
        // give it its new bounds, and set its variable sequence
        factory = t1->getFactory( *t1, newLowLimit, newHighLimit);
        factory->setVariablesSequence( fusVarSeq );

        // =========================================================================================================
        // Then we search in second diagram for possible preneeded variable
        retrogradeVarTable = new HashTable< NodeId, Set< const DiscreteVariable* >* >( t2->realSize(), true, false  );

        if ( withoutOrder ) {
            t2->findRetrogradeVariables( &( factory->variablesSequence() ), retrogradeVarTable );


#ifdef O4DDDEBUG
GUM_TRACE( "RETRO VAR TABLE : " );
#endif
            for ( HashTableIterator< NodeId, Set< const DiscreteVariable* >* > retroVarIter = retrogradeVarTable->begin(); retroVarIter != retrogradeVarTable->end(); ++retroVarIter )
                if ( *retroVarIter != NULL && !(*retroVarIter)->empty()) {
#ifdef O4DDDEBUG
GUM_TRACE( "\tNode : " << retroVarIter.key() );
#endif
                    for ( SetIterator< const DiscreteVariable* > iter = (*retroVarIter)->begin(); iter != (*retroVarIter)->end(); ++iter ) {
#ifdef O4DDDEBUG
GUM_TRACE( "\t\tVariable : " << (*iter)->name() );
#endif
                        if ( ! conti.isRetrogradeVar( *iter ) )
                            conti.addRetrogradeVar( *iter );
                    }
                }
        }

        // =========================================================================================================
        // And we finally instantiates the other data structures
        explorationTable = new HashTable< double, NodeId >( t1->realSize()*t2->realSize(), true, false );
    }

    ~OperatorData() {

        for ( HashTableIterator< NodeId, Set< const DiscreteVariable* >* > iterH = retrogradeVarTable->begin(); iterH != retrogradeVarTable->end(); ++iterH )
            delete *iterH;
        delete retrogradeVarTable;

        delete factory;

        delete explorationTable;

    }
};


// =========================================================================================================================================================
/// Insertion of a node when we know all its sons and its possible default son
// =========================================================================================================================================================
template<typename T>
NodeId
insertNonTerminalNode( OperatorData<T>& opData, const DiscreteVariable* associatedVariable, std::vector< NodeId >& sonsMap, NodeId defaultSon ) {

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
#ifdef O4DDDEBUG
    //     opData.factory->showProperties();
    return opData.factory->addNonTerminalNodeWithArcs( associatedVariable, sonsMap, defaultSon );
#else
    return opData.factory->unsafeAddNonTerminalNodeWithArcs( associatedVariable, sonsMap, defaultSon );
#endif
}

}/* end of namespace GUM */

#endif /*GUM_DECISION_DIAGRAM_OPERATOR_DATA*/

