
/********************************************************************************
*   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES	*
*   {prenom.nom}_at_lip6.fr 										*
*															*
*   This program is free software; you can redistribute it and/or modify 		*
*   it under the terms of the GNU General Public License as published by	*
*   the Free Software Foundation; either version 2 of the License, or 		*
*   (at your option) any later version.								*
* 															*
*   This program is distributed in the hope that it will be useful,			*
*   but WITHOUT ANY WARRANTY; without even the implied warranty of 		*
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 		*
*   GNU General Public License for more details.						*
* 															*
*   You should have received a copy of the GNU General Public License		*
*   along with this program; if not, write to the							*
*   Free Software Foundation, Inc.,									*
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. 			*
*********************************************************************************/

// =====================================================================
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <sstream>
#include <time.h>
// =====================================================================
#include <cxxtest/AgrumTestSuite.h>
#include "testsuite_utils.h"
// =====================================================================
#include <agrum/core/exceptions.h>
#include <agrum/core/approximationPolicy/approximationPolicy.h>
#include <agrum/core/approximationPolicy/linearApproximationPolicy.h>
// =====================================================================
#include <agrum/multidim/potential.h>
#include <agrum/multidim/labelizedVariable.h>
#include <agrum/multidim/instantiation.h>
#include <agrum/multidim/multiDimDecisionDiagram.h>
#include <agrum/multidim/multiDimDecisionDiagramFactory.h>
// =====================================================================

using namespace std;

namespace gum {

    namespace tests {

	class MultiDimOperators4DecisionDiagramTestSuite: public CxxTest::TestSuite {

	    private :

		// ================================================================================================
		// Génération fixe d'une liste de variable
		// ================================================================================================
		Sequence< const DiscreteVariable* >* __generateFixVarList(){
		  
		    Sequence< const DiscreteVariable* >* ret = new Sequence< const DiscreteVariable* >();
		    ret->insert( new LabelizedVariable( "A", "", 2 ) );
		    ret->insert( new LabelizedVariable( "B", "", 2 ) );
		    ret->insert( new LabelizedVariable( "C", "", 2 ) );

		    return ret;
		}
		// ================================================================================================

		// ================================================================================================
		// Génération aléatoire d'une liste de 10 variables
		// ================================================================================================
		Sequence< const DiscreteVariable* >* __generateRandomVarList( int i ){
		  
		    srand(time(NULL) + i);
		    
		    Sequence< const DiscreteVariable* >* ret = new Sequence< const DiscreteVariable* >();
		    for( int j = 0; j < 10; j++ ){
			std::stringstream varName;
			varName << "var" << j;
			ret->insert( new LabelizedVariable( varName.str(), "", 2 + rand()%2 ) );
		    }
		    
		    return ret;
		}
		// ================================================================================================

		// ================================================================================================
		// Brassage aléatoire d'une liste de 10 variables
		// ================================================================================================
		void __shuffleVarList( Sequence< const DiscreteVariable* >* varList, int i ){
		  
		    srand(time(NULL) + i);
		    
		    for( int j = 0; j < 10; j++ )
			varList->swap( rand()%(varList->size() ), rand()%(varList->size() ) );
		}
		// ================================================================================================

		// ================================================================================================
		// Génération fixe de diagramme de décision
		// ================================================================================================
		MultiDimDecisionDiagramBase<double>* __generateDecisionDiagram1( const Sequence< const DiscreteVariable* >* varList, MultiDimDecisionDiagramFactoryBase<double>* facto = NULL){

		    MultiDimDecisionDiagramBase<double>* ret = NULL;
		    bool factoryCreatedHere = false;
		    
		    if( facto == NULL ){
			factoryCreatedHere = true;
			facto = new MultiDimDecisionDiagramFactory<double>();
			facto->setLowLimit( -100 );
			facto->setHighLimit( 100 );
		    }
		    
		    facto->setVariablesSequence(  *varList );
		    NodeId a = facto->addNonTerminalNode( varList->atPos( 0 ) );
		    NodeId b = facto->addNonTerminalNode( varList->atPos( 1 ) );
		    NodeId c = facto->addNonTerminalNode( varList->atPos( 2 ) );

		    NodeId d = facto->addTerminalNode( 6 );
		    NodeId e = facto->addTerminalNode( 2 );
		    NodeId g = facto->addTerminalNode( 3 );

		    facto->insertArc( a, b, 0 );
		    facto->insertArc( a, c, 1 );

		    facto->insertArc( b, d, 0 );
		    facto->insertArc( b, c, 1 );

		    facto->insertArc( c, e, 0 );
		    facto->insertArc( c, g, 1 );

		    ret = facto->getMultiDimDecisionDiagram();
		    if( factoryCreatedHere )
			delete facto;
		    
		    return ret;
		}
		// ================================================================================================

		// ================================================================================================
		// Génération fixe de diagramme de décision
		// ================================================================================================
		MultiDimDecisionDiagramBase<double>* __generateDecisionDiagram2( const Sequence< const DiscreteVariable* >* varList, MultiDimDecisionDiagramFactoryBase<double>* facto = NULL){

		    MultiDimDecisionDiagramBase<double>* ret = NULL;
		    bool factoryCreatedHere = false;
		    
		    if( facto == NULL ){
			factoryCreatedHere = true;
			facto = new MultiDimDecisionDiagramFactory<double>();
			facto->setLowLimit( -100 );
			facto->setHighLimit( 100 );
		    }
		    
		    facto->setVariablesSequence(  *varList );
		    NodeId a = facto->addNonTerminalNode( varList->atPos( 0 ) );
		    NodeId c = facto->addNonTerminalNode( varList->atPos( 2 ) );

		    NodeId d = facto->addTerminalNode( 4 );
		    NodeId e = facto->addTerminalNode( 5 );
		    NodeId g = facto->addTerminalNode( 1 );

		    facto->insertArc( a, d, 0 );
		    facto->insertArc( a, c, 1 );

		    facto->insertArc( c, e, 0 );
		    facto->insertArc( c, g, 1 );

		    ret = facto->getMultiDimDecisionDiagram();
		    if( factoryCreatedHere )
			delete facto;
		    
		    return ret;

		}
		// ================================================================================================

		// ================================================================================================
		// Génération aléatoire de diagramme de décision
		// ================================================================================================
		MultiDimDecisionDiagramBase<double>* __generateRandomdoubleDecisionDiagram( const Sequence< const DiscreteVariable* >* varList, int i, MultiDimDecisionDiagramFactoryBase<double>* f = NULL ) {

		    srand( time(NULL) + i );
		    MultiDimDecisionDiagramBase<double>* ret = NULL;
		    bool factoryCreatedHere = false;

		    while( ret == NULL || ( ret->diagramVarSize() < 7 ) || ( ret->diagramVarSize() > 10 ) ){

			if( ret != NULL )
			    delete ret;

			if( f == NULL ){
			    factoryCreatedHere = true;
			    f = new MultiDimDecisionDiagramFactory<double>();
			    f->setLowLimit( -100 );
			    f->setHighLimit( 100 );
			}
			f->clear();
			f->setVariablesSequence( *varList );

			// ***********************************************************************
			// Creation of table var - list of associated node id in the diagram
			// with initialization of those lists
			HashTable< const DiscreteVariable*, List<NodeId>* > var2NodeIdMap;
			for ( Size numVar = 0; numVar < (*varList).size(); ++numVar )
			    var2NodeIdMap.insert( (*varList)[ numVar ], new List<NodeId>() );

			// ***********************************************************************	
			// Creation of a list containing terminal node possible value
			List<double> tnList;
			double interval = f->highLimit() - f->lowLimit();
			Idx i = 0; 
			while( i < 25 ){
			    double newVal = ( rand()% ( (int) interval ) ) - interval/2;
			    if( newVal <= f->highLimit() && newVal >= f->lowLimit() ){
				tnList.insert( newVal );
				i++; 
			    }
			}

			// ***********************************************************************
			// Selection (randomly) of the root var in the diagram
			// all var that are before this one won't be in the diagram
			// for order on var purpose
			Size initVar = rand()%(varList->size()/2);

			// ***********************************************************************
			// So for all remaining variable
			for ( Size numVar = initVar; numVar < varList->size(); ++numVar ) {

			    // if this is the root we add it
			    if( numVar == initVar )
				var2NodeIdMap[ (*varList)[ initVar ] ]->insert( f->unsafeAddNonTerminalNode( (*varList)[ initVar ] ) );

			    // then we check if another variable goes on it 
			    //(or if this is the root). It's done implicitly by the size of the list of node associated to that var.
			    // if not we move on
			    // else
			    if(!var2NodeIdMap[ (*varList)[ numVar ] ]->empty() ) {
				// for each node associated to that var
				for( ListIterator<NodeId> numNode = var2NodeIdMap[ (*varList)[ numVar ] ]->begin(); numNode != var2NodeIdMap[ (*varList)[ numVar ] ]->end();  ++numNode ){

				    // we determine a random number of arc starting from that node
				    Idx nbArc = 1 + rand()%((*varList)[ numVar ]->domainSize());

				    // for those arcs
				    for( Idx label = 0; label < nbArc; label++ ) {

					// We first determine if it goes directly to a terminal node or node
					if( numVar != ( varList->size() - 1 ) && (rand()%100) >= 42 ){

					    // if not, we determine randomly to which var it could go
					    NodeId indToVar = ( rand()%( varList->size() - 1 - numVar ) ) + numVar + 1;
					    const DiscreteVariable* toVar = (*varList)[ indToVar ];
					    // std::cout << "Variable : " << (*varList)[ numVar ]->toString() << " d'indice : " << numVar << " se lie à " << toVar->toString() << " d'indice : " << indToVar << std::endl;

					    // then we take the list of  node id associated to that var and
					    // we determine randomly if we add another node to that list or not
					    NodeId desiredNode = (NodeId) rand()%5;
					    if( desiredNode  >= var2NodeIdMap[ toVar ]->size() ){
						var2NodeIdMap[ toVar ]->insert( f->unsafeAddNonTerminalNode( toVar ) );
						desiredNode = var2NodeIdMap[ toVar ]->size() - 1;
					    }
					    // then we add an arc between our current var associated node id and the considered var random node id
					    f->unsafeInsertArc( *numNode, (*var2NodeIdMap[ toVar ])[desiredNode], label );

					}else{

					    // if we add
					    NodeId toVal = f->addTerminalNode ( tnList[ rand()%tnList.size() ] );
					    f->unsafeInsertArc( *numNode, toVal, label );

					}
				    }
				}
			    }
			}

			for(HashTableIterator< const DiscreteVariable*, List<NodeId>* > ht = var2NodeIdMap.begin(); ht != var2NodeIdMap.end(); ++ht )
			    delete *ht;

			ret = f->getMultiDimDecisionDiagram();
		    }
		    if( factoryCreatedHere )
			delete f;
		    
		    return ret;
		}
		// ================================================================================================

		// ================================================================================================
		// Sauvegarde des diagrammes générant une erreur dans un fichier log
		// ================================================================================================
		void __saveDiagrams( MultiDimDecisionDiagramBase<double>* a1, MultiDimDecisionDiagramBase<double>* a2, MultiDimDecisionDiagramBase<double>* a3 ) {

		    std::string dotfile = GET_PATH_STR ( DecisionDiagramError.log );
		    std::ofstream output ( dotfile.c_str(), std::ios::out );

		    if ( ! output.good() )
			GUM_ERROR ( IOError, "Stream states flags are not all unset." );

		    output << std::endl;
		    for( SequenceIterator< const DiscreteVariable*> ite = a1->variablesSequence().begin(); ite != a1->variablesSequence().end(); ++ite )
			output << (*ite)->toString() << " - ";
		    output << std::endl;

		    output << a1->toDot();

		    output << std::endl;
		    for( SequenceIterator< const DiscreteVariable*> ite = a2->variablesSequence().begin(); ite != a2->variablesSequence().end(); ++ite )
			output << (*ite)->toString() << " - ";
		    output << std::endl;

		    output << a2->toDot();

		    if( a3 != NULL ){
			output << std::endl;
			for( SequenceIterator< const DiscreteVariable*> ite = a3->variablesSequence().begin(); ite != a3->variablesSequence().end(); ++ite )
			    output << (*ite)->toString() << " - ";
			output << std::endl;

			output << a3->toDot();
		    }

		    output.flush();

		    output.close();

		    if ( output.fail() )
			GUM_ERROR ( IOError, "Writting in the ostream failed." );
		}
		// ================================================================================================

		// ================================================================================================
		// Evals  given in parameter operation. Returned boolean parameter indicates if all went well or not
		// ================================================================================================
		bool __evalOperation( int operationId, MultiDimDecisionDiagramBase<double>* a1, MultiDimDecisionDiagramBase<double>* a2, double& tempsCalcul, double& tempsEval, double delta = 0.01 ){

		    bool hasNoError = true;
		    MultiDimDecisionDiagramBase<double>* a3 = NULL;
		    
		    Timer timy;
		    timy.reset();

		    // ******************************************************************************************************
		    // Generation du diagramme résultat
		    switch( operationId ){
			case 1 : // Test addition
			    TS_GUM_ASSERT_THROWS_NOTHING( a3 = add2MultiDimDecisionDiagrams( a1 , a2 ) );
			    break;
			case 2 : // Test Substraction
			    TS_GUM_ASSERT_THROWS_NOTHING( a3 = subtract2MultiDimDecisionDiagrams( a1 , a2 ) );
			    break;
			case 3 : // Test Multiplication
			    TS_GUM_ASSERT_THROWS_NOTHING( a3 = multiply2MultiDimDecisionDiagrams( a1 , a2 ) );
			    break;
			case 4 :  // Test Maximum
			    TS_GUM_ASSERT_THROWS_NOTHING( a3 = maximize2MultiDimDecisionDiagrams( a1 , a2 ) );
			    break;
			default :
			    GUM_ERROR( OperationNotAllowed, "HEU .....");
		    }
		    // ******************************************************************************************************

		    tempsCalcul += timy.step();
		    timy.reset();

		    // ******************************************************************************************************
		    if( a3 != NULL ){
			Instantiation inst(a3);
			for( inst.setFirst(); ! inst.end() && hasNoError; ++inst ){

			    switch( operationId ){
				case 1 : // Test addition
				    TS_ASSERT_DELTA( a3->get(inst), a1->get(inst) + a2->get(inst), delta );
				    if( a3->get(inst) != a1->get(inst) + a2->get(inst))
					hasNoError = false;
				    break;
				case 2 : // Test Substraction
				    TS_ASSERT_DELTA( a3->get(inst), a1->get(inst) - a2->get(inst), delta );
				    if( a3->get(inst) != a1->get(inst) - a2->get(inst))
					hasNoError = false;
				    break;
				case 3 : // Test Multiplication
				    TS_ASSERT_DELTA( a3->get(inst), a1->get(inst) * a2->get(inst), delta );
				    if( a3->get(inst) != a1->get(inst) * a2->get(inst))
					hasNoError = false;
				    break;
				case 4 :  // Test Maximum
				    TS_ASSERT_DELTA( a3->get(inst), a1->get(inst) > a2->get(inst) ? a1->get(inst) : a2->get(inst), delta );
				    if( a3->get(inst) != ( a1->get(inst) > a2->get(inst) ? a1->get(inst) : a2->get(inst) ))
					hasNoError = false;
				    break;
				default :
				    GUM_ERROR( OperationNotAllowed, "HEU .....");
			    }
			}
			if( !hasNoError )
			    __saveDiagrams(a1, a2, a3);
			delete a3;
		    } else{
			__saveDiagrams(a1, a2, a3);
			hasNoError = false;
		    }
		    
		    tempsEval +=  timy.step();

		    return hasNoError;

		}

	    public :

		// ================================================================================================
		//
		// Test sur les fonctions avec valeurs exactes
		//
		// ================================================================================================
		void test_Operators_Functions_on_MultiDimDecisionDiagrams(){
		  
		    Timer time;
		    double tempsGene = 0;
		    double tempsCalcul = 0;
		    double tempsEval = 0;

		    // =====================================================================================
		    // First we try with a predefine structure
		    // =====================================================================================
		    time.reset();
		    
		    Sequence< const DiscreteVariable* >* varList = __generateFixVarList();

		    MultiDimDecisionDiagramBase<double>* a1 = NULL;
		    TS_GUM_ASSERT_THROWS_NOTHING( a1 = __generateDecisionDiagram1(varList) );	

		    MultiDimDecisionDiagramBase<double>* a2 = NULL;
		    TS_GUM_ASSERT_THROWS_NOTHING( a2 = __generateDecisionDiagram2(varList) );
		    
		    tempsGene += time.step();

		    bool evalRes = true;

		    for( int i = 1; i < 5 && evalRes; i++)
			evalRes = __evalOperation( i, a1, a2, tempsCalcul, tempsEval );

		    delete a1;
		    delete a2;

		    for( SequenceIterator< const DiscreteVariable*> ite = varList->begin(); ite != varList->end(); ++ite )
			delete *ite;
		    delete varList;

		    if( !evalRes ){
			std::cout << "An error has occured! Aborting test." <<std::endl;
			return;
		    }

		    std::cout << ".";
		    std::fflush( stdout );

		    // =====================================================================================
		    // Then we try with random structure
		    // =====================================================================================
		    // 
		    for(int i = 0; i < 100; i++ ){
		      
			time.reset();
			
			varList = __generateRandomVarList( i + 1 );
			__shuffleVarList( varList, i + 2 );

			a1 = NULL;
			TS_GUM_ASSERT_THROWS_NOTHING( a1 = __generateRandomdoubleDecisionDiagram(varList, i + 3) );

			__shuffleVarList( varList, i + 4 );

			a2 = NULL;
			TS_GUM_ASSERT_THROWS_NOTHING( a2 = __generateRandomdoubleDecisionDiagram(varList, i + 5) );
		    
			tempsGene += time.step();

			evalRes = true;

			for( int j = 1; j < 5 && evalRes; j++)
			    TS_GUM_ASSERT_THROWS_NOTHING( evalRes = __evalOperation( j, a1, a2, tempsCalcul, tempsEval ) );

			delete a1;
			delete a2;

			for( SequenceIterator< const DiscreteVariable*> ite = varList->begin(); ite != varList->end(); ++ite )
			    delete *ite;
			delete varList;

			if( !evalRes ){
			    std::cout << "An error has occured! Aborting test." <<std::endl;
			    return;
			}

			if( i%25 == 0 )
			    std::cout << "#";
			else if( i%5 == 0 )
			    std::cout << ".";
			std::fflush( stdout );
		    }
// 		    std::cout << std::endl << "Temps Génération : " << tempsGene << "s - Temps Calcul : " << tempsCalcul << "s - Temps Evaluation " << tempsEval << "s ";
		}
		// ================================================================================================

		// ================================================================================================
		//
		// Test sur les opérateurs avec valeurs exactes
		//
		// ================================================================================================
		void test_Operators_on_MultiDimDecisionDiagrams(){
		    operators4MultiDimInit<double> ();

		    // =====================================================================================
		    // First we try with a predefine structure
		    // =====================================================================================

		    MultiDimDecisionDiagram<double> a1, a2;
		    MultiDimDecisionDiagramBase<double>* a3 = NULL;

		    //Test addition
		    TS_GUM_ASSERT_THROWS_NOTHING( a3 = (MultiDimDecisionDiagramBase<double>*) ( a1 + a2 ) );
		    if( a3 != NULL )
			delete a3;

		    //Test subtraction
		    TS_GUM_ASSERT_THROWS_NOTHING( a3 = (MultiDimDecisionDiagramBase<double>*) ( a1 - a2 ) );
		    if( a3 != NULL )
			delete a3;

		    //Test multiplication
		    TS_GUM_ASSERT_THROWS_NOTHING( a3 = (MultiDimDecisionDiagramBase<double>*) ( a1 * a2 ) );
		    if( a3 != NULL )
			delete a3;

		    //Test division
		    TS_GUM_ASSERT_THROWS_NOTHING( a3 = (MultiDimDecisionDiagramBase<double>*) ( a1 / a2 ) );
		    if( a3 != NULL )
			delete a3;
		}

		// ================================================================================================
		//
		// Test sur les fonctions avec approximation
		//
		// ================================================================================================		
		void _Operation_in_MultiDimDecisionDiagrams_With_LinearApproximation(){

		    // =====================================================================================
		    // First we try with a predefine structure
		    // =====================================================================================
// 		    Sequence< const DiscreteVariable* >* varList = __generateFixVarList();
// 
// 		    MultiDimDecisionDiagramFactory<double,LinearApproximationPolicy> f;
// 		    f.setEpsilon( 0.9 );
// 		    f.setLowLimit( 0 );
// 		    f.setHighLimit( 4.5 );
// 
// 		    MultiDimDecisionDiagramBase<double>* a1 = NULL;
// 		    TS_GUM_ASSERT_THROWS_NOTHING( a1 = __generateDecisionDiagram1(varList, &f ) );
// 		    
// 		    f.clear();
// 		    f.setEpsilon( 1.3 );
// 		    f.setLowLimit( 3.5 );
// 		    f.setHighLimit( 9 );
// 		    
// 		    MultiDimDecisionDiagramBase<double>* a2 = NULL;
// 		    TS_GUM_ASSERT_THROWS_NOTHING( a2 = __generateDecisionDiagram2(varList, &f ) );
// 
// 		    bool evalRes = true;
// 
// 		    for( int i = 1; i < 5 && evalRes; i++)
// 			evalRes = __evalOperation( i, a1, a2, 0.9 );
// 
// 		    delete a1;
// 		    delete a2;
// 
// 		    for( SequenceIterator< const DiscreteVariable*> ite = varList->begin(); ite != varList->end(); ++ite )
// 			delete *ite;
// 		    delete varList;
// 
// 		    if( !evalRes ){
// 			std::cout << "An error has occured! Aborting test." <<std::endl;
// 			return;
// 		    }
// 
// 		    std::cout << ".";
// 		    std::fflush( stdout );

		    // =====================================================================================
		    // Then we try with random structure
		    // =====================================================================================
		    // 
// 		    for(int i = 0; i < 100; i++ ){
// 			varList = __generateRandomVarList( i + 1 );
// 			
// 			__shuffleVarList( varList, i + 2 );
// 			f.clear();
// 			f.setEpsilon( 7 );
// 			f.setLowLimit( -42 );
// 			f.setHighLimit( 69 );
// 
// 			TS_GUM_ASSERT_THROWS_NOTHING( a1 = __generateRandomdoubleDecisionDiagram(varList, i + 3, &f ) );
// 
// 			__shuffleVarList( varList, i + 4 );
// 			f.clear();
// 			f.setEpsilon( 11 );
// 			f.setLowLimit( -53 );
// 			f.setHighLimit( 79 );
// 
// 			TS_GUM_ASSERT_THROWS_NOTHING( a2 = __generateRandomdoubleDecisionDiagram(varList, i + 5, &f ) );
// 
// 			evalRes = true;
// 
// 			for( int j = 1; j < 5 && evalRes; j++)
// 			    TS_GUM_ASSERT_THROWS_NOTHING( evalRes = __evalOperation( j, a1, a2, 7 ) );
// 
// 			delete a1;
// 			delete a2;
// 
// 			for( SequenceIterator< const DiscreteVariable*> ite = varList->begin(); ite != varList->end(); ++ite )
// 			    delete *ite;
// 			delete varList;
// 
// 			if( !evalRes ){
// 			    std::cout << "An error has occured! Aborting test." <<std::endl;
// 			    return;
// 			}
// 
// 			if( i%25 == 0 )
// 			    std::cout << "#";
// 			else if( i%5 == 0 )
// 			    std::cout << ".";
// 			std::fflush( stdout );
// 		    }
		}


		// ================================================================================================
		//
		// For debug purpose only, the aim of this function is to reproduce specific diagrams in order to find out where the hell the matter is
		//
		// ================================================================================================		
		void _Reproducteur(){

// 		    LabelizedVariable* v0 = new LabelizedVariable( "0", "", 2 );
// 		    LabelizedVariable* v1 = new LabelizedVariable( "1", "", 2 );
// 		    LabelizedVariable* v2 = new LabelizedVariable( "2", "", 3 );
// 		    LabelizedVariable* v3 = new LabelizedVariable( "3", "", 3 );
// 		    LabelizedVariable* v4 = new LabelizedVariable( "4", "", 3 );
// 		    LabelizedVariable* v5 = new LabelizedVariable( "5", "", 3 );
// 		    LabelizedVariable* v6 = new LabelizedVariable( "6", "", 2 );
// 		    LabelizedVariable* v7 = new LabelizedVariable( "7", "", 2 );
// 		    LabelizedVariable* v8 = new LabelizedVariable( "8", "", 2 );
// 		    LabelizedVariable* v9 = new LabelizedVariable( "9", "", 3 );


// 		    MultiDimDecisionDiagramFactory<double> facto;

// 		    Sequence< const DiscreteVariable* > seq;
// 		    seq.insert( v0 );
// 		    seq.insert( v1 );
// 		    seq.insert( v2 );
// 		    seq.insert( v3 );
// 		    seq.insert( v4 );
// 		    seq.insert( v5 );
// 		    seq.insert( v6 );
// 		    seq.insert( v7 );
// 		    seq.insert( v8 );
// 		    seq.insert( v9 );

// 		    std::cout << std::endl << " Sequence variable : ";
// 		    for( SequenceIterator< const DiscreteVariable* > iter = seq.begin(); iter != seq.end(); ++iter )
// 		    std::cout << (*iter)->toString() << " - ";
// 		    std::cout << std::endl;

// 		    facto->setVariablesSequence( seq );

// 		    NodeId n11 = facto->addNonTerminalNode( v1 );
// 		    NodeId n1t10 = facto->addTerminalNode( 10 );

// 		    facto->insertArc( n11, n121, 0 );
// 		    facto->insertDefaultArc( n11, n122 );

// 		    MultiDimDecisionDiagramBase<double>* a1 = facto->getMultiDimDecisionDiagram();
// 		    std::cout << std::endl << a1->toDot();

// 		    facto->clear();
// 		    seq.clear();

// 		    seq.insert( v0 );
// 		    seq.insert( v2 );
// 		    seq.insert( v3 );
// 		    seq.insert( v1 );
// 		    seq.insert( v5 );
// 		    seq.insert( v8 );
// 		    seq.insert( v3 );
// 		    seq.insert( v9 );
// 		    seq.insert( v7 );
// 		    seq.insert( v6 );

// 		    std::cout << std::endl << " Sequence variable : ";
// 		    for( SequenceIterator< const DiscreteVariable* > iter = seq.begin(); iter != seq.end(); ++iter )
// 		    std::cout << (*iter)->toString() << " - ";
// 		    std::cout << std::endl;

// 		    facto->setVariablesSequence( seq );

// 		    NodeId n21 = facto->addNonTerminalNode( v2 );
// 		    NodeId n2t0 = facto->addTerminalNode( 0 );

// 		    facto->insertArc( n21, n2t0, 0 );
// 		    facto->insertDefaultArc( n21, n2t0 );

// 		    MultiDimDecisionDiagramBase<double>* a2 = facto->getMultiDimDecisionDiagram();
// 		    std::cout << std::endl << a2->toDot() << std::endl;

// 		    bool evalRes = true;
// 		    TS_GUM_ASSERT_THROWS_NOTHING( evalRes = __evalOperation( 1, a1, a2 ) );

// 		    delete a1;
// 		    delete a2;

// 		    delete  v0;
// 		    delete  v1;
// 		    delete  v2;
// 		    delete  v3;
// 		    delete  v4;
// 		    delete  v5;
// 		    delete  v6;
// 		    delete  v7;
// 		    delete  v8;
// 		    delete  v9;
		}
		// ================================================================================================

	};
    }
}
