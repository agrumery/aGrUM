/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
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
/**
 * @file
 * @brief Sources of MultiDimDecisionGraphGenerator.
 *
 * @author Jean-Christophe Magnan
 *
 */
// ==========================================================================
#include <random>
#include <cstdlib>
// ==========================================================================
#include <agrum/multidim/multiDimDecisionGraphGenerator.h>
// ==========================================================================
#include <agrum/core/priorityQueue.h>
// ==========================================================================

namespace gum{

    Idx MultiDimDecisionGraphGenerator::__genSeed = 0;

    // ==========================================================================
    // Constructor
    // ==========================================================================
    MultiDimDecisionGraphGenerator::MultiDimDecisionGraphGenerator(Idx maxVar, Idx minVar, const Sequence<const DiscreteVariable *> &varSeq):
        __minNbVarInDiagram(minVar), __maxNbVarInDiagram(maxVar), __varSeq(varSeq){

        GUM_CONSTRUCTOR(MultiDimDecisionGraphGenerator);

        __nbTotalVar = __varSeq.size();
    }

    // ==========================================================================
    // Destructor
    // ==========================================================================
    MultiDimDecisionGraphGenerator::~MultiDimDecisionGraphGenerator(){

        GUM_DESTRUCTOR(MultiDimDecisionGraphGenerator);

    }

    MultiDimDecisionGraph<double>* MultiDimDecisionGraphGenerator::generate(){

    std::cout << std::endl;
        srand( time(NULL) );
        MultiDimDecisionGraph<double>* generatedDecisionGraph = new MultiDimDecisionGraph<double>();

        for ( SequenceIteratorSafe< const DiscreteVariable* > varIter = __varSeq.beginSafe(); varIter != __varSeq.endSafe(); ++varIter ){
//            std::cout << (*varIter)->name() << " - " << (*varIter)->domainSize() << std::endl;
            generatedDecisionGraph->add(**varIter);
        }



        HashTable<NodeId,Idx> node2MinVar;
        Idx nbIter = 0;

//        while ( ( generatedDecisionGraph->variablesSequence().size() < __minNbVarInDiagram ) || ( generatedDecisionGraph->variablesSequence().size() > __maxNbVarInDiagram ) ) {

            std::vector< NodeId > filo;

            // L'idée est de d'abord générer un arbre avant de fusionner les sous-graphe isomorphe
//            generatedDecisionGraph->add( *(__varSeq.atPos(0)));
            generatedDecisionGraph->manager()->setRootNode( generatedDecisionGraph->manager()->addNonTerminalNode( __varSeq.atPos(0) ) );
            node2MinVar.insert(generatedDecisionGraph->root(), 0 );
            filo.push_back(generatedDecisionGraph->root());


//            std::default_random_engine generator;
//            int rutidi = 0;

            while( ! filo.empty() ){ //&& nbIter < 20 ){

//                std::cout << filo << std::endl;

                NodeId currentNodeId = filo.back();
                filo.pop_back();
                NodeId cvp = node2MinVar[ currentNodeId ];
                const typename MultiDimDecisionGraph<double>::InternalNode* currentNode = generatedDecisionGraph->node( currentNodeId );

//                std::cout << std::endl << "New Node : " << currentNodeId << " - Associated Variable : " << currentNode->nodeVar()->name() << std::endl;

                typename MultiDimDecisionGraph<double>::NICLElem* potentialSons = nullptr;
                Idx nbPotentialSons = 0;
                for( Idx varPos = 0; varPos < generatedDecisionGraph->variablesSequence().size(); varPos++ ){
                    const DiscreteVariable* var = generatedDecisionGraph->variablesSequence().atPos( varPos );

//                    std::cout << "Variable courante : " << var->name() << std::endl;
                    Idx vsp = __varSeq.pos( var );
                    if( vsp > cvp ){

//                        std::cout << "Variable visitée" << std::endl;

                        const typename MultiDimDecisionGraph<double>::NICLElem* nicleIter = generatedDecisionGraph->varNodeListe( var );
                        while ( nicleIter ){
                            nbPotentialSons++;
                            typename MultiDimDecisionGraph<double>::NICLElem* newnicle = static_cast<typename MultiDimDecisionGraph<double>::NICLElem*>(
                                        MultiDimDecisionGraph<double>::soa.allocate( sizeof(typename MultiDimDecisionGraph<double>::NICLElem) ) );
//                            std::cout << "No Problem MAN" << std::endl;
                            newnicle->elemId = nicleIter->elemId;
//                            std::cout << "No Problem MAN1" << std::endl;
                            newnicle->nextElem = potentialSons;
//                            std::cout << "No Problem MAN2" << std::endl;
                            potentialSons = newnicle;
//                            std::cout << "No Problem MAN3" << std::endl;
                            nicleIter = nicleIter->nextElem;
//                            std::cout << potentialSons << " - " << potentialSons->nextElem << std::endl;
                        }
                    }
                }

//                std::cout << "Creating children ..." << std::endl;

                for(Idx modality = 0; modality < currentNode->nodeVar()->domainSize(); modality++ ){

//                    std::cout << "Modality : " << modality << std::endl;
                    if( ! potentialSons || (double)std::rand( ) / (double)RAND_MAX > ( 1.0 / ( 1.0 + 3.0 / nbPotentialSons ) ) ){

                        if( __createLeaf( currentNodeId, node2MinVar ) ) {
//                            std::cout << "Creating leaf ..." << std::endl;
                            generatedDecisionGraph->manager()->setSon( currentNodeId, modality, generatedDecisionGraph->manager()->addTerminalNode( (double)std::rand( ) / (double)RAND_MAX*100 ) );
//                            std::cout << " | " << modality  << " - L | " << std::endl;
                        } else {
//                            std::cout << "Creating Internal Node ..." << std::endl;
                            Idx sonVarPos = __generateVarPos( node2MinVar[currentNodeId] + 1, __nbTotalVar - node2MinVar[ currentNodeId ] - 2 );
                            generatedDecisionGraph->manager()->setSon( currentNodeId, modality, generatedDecisionGraph->manager()->addNonTerminalNode( __varSeq.atPos( sonVarPos ) ) );
                            filo.push_back( currentNode->son( modality));
                            node2MinVar.insert( currentNode->son(modality), sonVarPos);
//                            std::cout << " | " << modality  << " - I | " << std::endl;
                        }
                    } else {
//                        std::cout << "Merging ..." << std::endl;
                        Idx sonPos = (Idx) ( ( (double)std::rand( ) / (double)RAND_MAX) * nbPotentialSons);
//                        std::cout << "A" << std::endl;
                        sonPos = sonPos == nbPotentialSons ? nbPotentialSons - 1 : sonPos;
//                        std::cout << "B" << std::endl;
                        typename MultiDimDecisionGraph<double>::NICLElem* nicleIter = potentialSons;
//                        std::cout << "C - " << nbPotentialSons << " - " << sonPos << std::endl;
                        while( sonPos ){
//                            std::cout << sonPos << " - " << nicleIter << " - " << nicleIter->nextElem << std::endl;
                            nicleIter = nicleIter->nextElem;
                            sonPos--;
                        }
//                        std::cout << "D" << std::endl;
                        generatedDecisionGraph->manager()->setSon( currentNodeId, modality, nicleIter->elemId );
//                        std::cout << " | " << modality  << " - F | " << std::endl;
                    }
                }

//                std::cout << std::endl << "Removing potential child ..." << std::endl;

                while(nbPotentialSons--){
                    typename MultiDimDecisionGraph<double>::NICLElem* next = potentialSons->nextElem;
                    MultiDimDecisionGraph<double>::soa.deallocate(potentialSons, sizeof(typename MultiDimDecisionGraph<double>::NICLElem) );
                    potentialSons = next;
                }


//                std::cout << "Fin Iteration." << std::endl;
                ++nbIter;
            }

//        generatedDecisionGraph->manager()->reduce();
//        generatedDecisionGraph->manager()->clean();

        return generatedDecisionGraph;
    }


    bool MultiDimDecisionGraphGenerator::__createLeaf( NodeId currentNodeId, HashTable<NodeId,Idx>& node2MinVar ){
       return !( currentNodeId == 1 ||
           ( (double)std::rand( ) / (double)RAND_MAX < 0.9 + 0.01*( float(__nbTotalVar - node2MinVar[ currentNodeId ]) / float(__nbTotalVar) ) && node2MinVar[ currentNodeId ] < __nbTotalVar - 1 ) );
    }

    Idx MultiDimDecisionGraphGenerator::__generateVarPos(Idx offset, Idx span){

        Idx randOut = 0;

        if( span != 0 ){
            std::default_random_engine generator;
            generator.seed(__genSeed);
            __genSeed = generator();
            std::weibull_distribution<double> distribution( span, 1.0);
            randOut = (Idx) (distribution(generator)*span/2);
            if( randOut > span )
                randOut = span;
        }

        return offset + randOut;

    }

} /* end of namespace */
