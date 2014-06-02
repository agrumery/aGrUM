/****************************************************************************
 *    Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                                *
 *                                                                          *
 *   This program is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public LiceDG2NodeIde as published by   *
 *   the Free Software Foundation; either version 2 of the LiceDG2NodeIde, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   This program is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU General Public LiceDG2NodeIde for more details.                           *
 *                                                                          *
 *   You should have received a copy of the GNU General Public LiceDG2NodeIde      *
 *   along with this program; if not, write to the                          *
 *   Free Software Foundation, Inc.,                                        *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.              *
 ****************************************************************************/
/**
* @file
* @brief Class used to compute the Projection between two decision diagrams
*
* @author Jean-Christophe Magnan
*/

// =======================================================
#include <agrum/multidim/patterns/DGUtility/multiDimDecisionGraphProjector.h>
// =======================================================

namespace gum {

    /* ***************************************************************************************************************************** */
    /* CONSTRUCTOR                                                                                                                   */
    /* ***************************************************************************************************************************** */
    template <typename GUM_SCALAR, template <typename> class FUNCTOR >
    MultiDimDecisionGraphProjector<GUM_SCALAR, FUNCTOR>::MultiDimDecisionGraphProjector( const MultiDimDecisionGraph<GUM_SCALAR>* src,
                                                                                         const Set<const DiscreteVariable*>& delVars ):
                                                                                       __src( src ),
                                                                                       __delVars( delVars ),
                                                                                       __function(){
        GUM_CONSTRUCTOR(MultiDimDecisionGraphProjector);
        __rd = new MultiDimDecisionGraph<GUM_SCALAR>();
    }



    /* ***************************************************************************************************************************** */
    /* DESTRUCTOR                                                                                                                   */
    /* ***************************************************************************************************************************** */
    template <typename GUM_SCALAR, template <typename> class FUNCTOR >
    MultiDimDecisionGraphProjector<GUM_SCALAR, FUNCTOR>::~MultiDimDecisionGraphProjector( ){
        GUM_DESTRUCTOR(MultiDimDecisionGraphProjector);
    }



    /* ***************************************************************************************************************************** */
    /* Compute                                                                                                                       */
    /*                                                                                                                               */
    /* This function is the main function. To be call every time an Projection between the two given Decision Graphs is required      */
    /* ***************************************************************************************************************************** */
    template <typename GUM_SCALAR, template <typename> class FUNCTOR >
    MultiDimDecisionGraph<GUM_SCALAR> *MultiDimDecisionGraphProjector<GUM_SCALAR, FUNCTOR>::project(){

        __rd->copyFrom( *__src );

        for( SetIteratorSafe<const DiscreteVariable*> varIter = __delVars.beginSafe(); varIter != __delVars.endSafe(); ++varIter ){

            const DiscreteVariable* curVar = *varIter;

            __rd->manager()->moveTo( curVar, __rd->variablesSequence().size() - 1 );

            HashTable<NodeId, NodeId> visitedNode;
            std::vector<NodeId> filo;
            filo.push_back( __rd->root() );

            while( !filo.empty() ){
                NodeId curNodeId = filo.back();
                filo.pop_back();

                const typename MultiDimDecisionGraph<GUM_SCALAR>::InternalNode* curNode = __rd->node( curNodeId );

                for(Idx modality = 0; modality < curNode->nodeVar()->domainSize(); ++modality ){

                    NodeId oldSonId = curNode->son(modality);

                    if( !visitedNode.exists( oldSonId ) ) {
                        NodeId newSonId = oldSonId;

                        if( __rd->node( oldSonId )->nodeVar() != curVar ){

                            if( !__rd->isTerminalNode( oldSonId ) ){

                                filo.push_back( oldSonId );

                            } else {

                                GUM_SCALAR newVal = (GUM_SCALAR) 0, oldVal = __rd->nodeValue( oldSonId );
                                for( Idx curVarModality = 0; curVarModality < curVar->domainSize(); ++curVarModality )
                                    newVal = __function( newVal,  oldVal );

                                newSonId = __rd->manager()->addTerminalNode( newVal );
                                __rd->manager()->setSon( curNodeId, modality, newSonId );

                            }

                        } else {

                            const typename MultiDimDecisionGraph<GUM_SCALAR>::InternalNode* curVarNode = __rd->node( oldSonId );
                            GUM_SCALAR newVal = (GUM_SCALAR) 0;
                            for( Idx curVarModality = 0; curVarModality < curVar->domainSize(); ++curVarModality )
                                newVal = __function(newVal, __rd->nodeValue( curVarNode->son( curVarModality ) ) );

                            newSonId = __rd->manager()->addTerminalNode( newVal );

                            __rd->manager()->eraseNode( oldSonId, newSonId );

                        }

                        visitedNode.insert( oldSonId, newSonId );

                    } else {

                        __rd->manager()->setSon( curNodeId, modality, visitedNode[oldSonId] );

                    }
                }
            }
        }

        return __rd;
    }

} // namespace gum

