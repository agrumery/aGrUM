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
				Sequence< const DiscreteVariable* >* generateFixVarList(){

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
				Sequence< const DiscreteVariable* >* generateRandomVarList( int i ){
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
				void shuffleVarList( Sequence< const DiscreteVariable* >* varList, int i ){
					srand(time(NULL) + i);

					for( int j = 0; j < 10; j++ )
						varList->swap( rand()%(varList->size() ), rand()%(varList->size() ) );
				}
				// ================================================================================================

				// ================================================================================================
				// Génération fixe de diagramme de décision
				// ================================================================================================
				MultiDimDecisionDiagramBase<float>* generateDecisionDiagram1( const Sequence< const DiscreteVariable* >* varList){

					MultiDimDecisionDiagramFactory<float> facto;
					facto.specifyVariablesSequence(  *varList );
					NodeId a = facto.addNonTerminalNode( *( varList->atPos( 0 ) ) );
					NodeId b = facto.addNonTerminalNode( *( varList->atPos( 1 ) ) );
					NodeId c = facto.addNonTerminalNode( *( varList->atPos( 2 ) ) );

					NodeId d = facto.addTerminalNode( 6 );
					NodeId e = facto.addTerminalNode( 2 );
					NodeId g = facto.addTerminalNode( 3 );

					facto.insertArc( a, b, 0 );
					facto.insertArc( a, c, 1 );

					facto.insertArc( b, d, 0 );
					facto.insertArc( b, c, 1 );

					facto.insertArc( c, e, 0 );
					facto.insertArc( c, g, 1 );

					return facto.getMultiDimDecisionDiagram(false);

				}
				// ================================================================================================

				// ================================================================================================
				// Génération fixe de diagramme de décision
				// ================================================================================================
				MultiDimDecisionDiagramBase<float>* generateDecisionDiagram2( const Sequence< const DiscreteVariable* >* varList){

					MultiDimDecisionDiagramFactory<float> facto;
					facto.specifyVariablesSequence(  *varList );
					NodeId a = facto.addNonTerminalNode( *( varList->atPos( 0 ) ) );
					NodeId c = facto.addNonTerminalNode( *( varList->atPos( 2 ) ) );

					NodeId d = facto.addTerminalNode( 4 );
					NodeId e = facto.addTerminalNode( 5 );
					NodeId g = facto.addTerminalNode( 1 );

					facto.insertArc( a, d, 0 );
					facto.insertArc( a, c, 1 );

					facto.insertArc( c, e, 0 );
					facto.insertArc( c, g, 1 );

					return facto.getMultiDimDecisionDiagram(false);

				}
				// ================================================================================================

				// ================================================================================================
				// Génération aléatoire de diagramme de décision
				// ================================================================================================
				MultiDimDecisionDiagramBase<float>* generateRandomfloatDecisionDiagram( const Sequence< const DiscreteVariable* >* varList, int i, MultiDimDecisionDiagramFactoryBase<float>* f = NULL ) {

					srand( time(NULL) + i );
					MultiDimDecisionDiagramBase<float>* ret = NULL;
					bool factoryCreatedHere = false;

					while( ret == NULL || ( ret->diagramVarSize() < 3 ) || ( ret->diagramVarSize() > 5 ) ){

						if( ret != NULL )
							delete ret;

						if( f == NULL ){
							factoryCreatedHere = true;
							f = new MultiDimDecisionDiagramFactory<float>();
							f->setLowLimit( -100 );
							f->setHighLimit( 100 );
						}
						f->clear();
						f->specifyVariablesSequence( *varList );

						// ***********************************************************************
						// Creation of table var - list of associated node id in the diagram
						// with initialization of those lists
						HashTable< const DiscreteVariable*, List<NodeId>* > var2NodeIdMap;
						for ( Size numVar = 0; numVar < (*varList).size(); ++numVar )
							var2NodeIdMap.insert( (*varList)[ numVar ], new List<NodeId>() );

						// ***********************************************************************
						// Creation of a list containing terminal node possible value
						List<float> tnList;
						float interval = f->highLimit() - f->lowLimit();
						gum::Idx i = 0;
						while( i < 25 ){
							float newVal = ( rand()% ( (int) interval ) ) - interval/2;
							if( newVal <= f->highLimit() && newVal >= f->lowLimit() ){
								tnList.insert( newVal );
								i++;
							}
						}

						// ***********************************************************************
						// Selection (randomly) of the root var in the diagram
						// all var that are before that one won't be in the diagram
						// for order on var purpose
						Size initVar = rand()%(varList->size()/2);

						// ***********************************************************************
						// So for all remaining variable
						for ( Size numVar = initVar; numVar < varList->size(); ++numVar ) {

							// if this is the root we add it
							if( numVar == initVar )
								var2NodeIdMap[ (*varList)[ initVar ] ]->insert( f->addNonTerminalNode( *( (*varList)[ initVar ] ) ) );

							// then we check if another variable goes on it
							//(or if this is the root). It's done implicitly by the size of the list of node associated to that var.
							// if not we move on
							// else
							if(!var2NodeIdMap[ (*varList)[ numVar ] ]->empty() ) {

								// for each node associated to that var
								for( ListIterator<NodeId> numNode = var2NodeIdMap[ (*varList)[ numVar ] ]->begin(); numNode != var2NodeIdMap[ (*varList)[ numVar ] ]->end();  ++numNode ){

									// we determine a random number of arc starting from that node
									gum::Size nbArc = 1 + rand()%((*varList)[ numVar ]->domainSize());

									// for those arcs
									for( gum::Idx label = 0; label < nbArc; label++ ) {

										// We first determine if it goes directly to a terminal node or node
										if( numVar != ( varList->size() - 1 ) && (rand()%100) >= 42 ){

											// if not, we determine randomly to which var it could go
											gum::Idx indToVar = ( rand()%( varList->size() - 1 - numVar ) ) + numVar + 1;
											const DiscreteVariable* toVar = (*varList)[ indToVar ];
											//~ std::cout << "Variable : " << (*varList)[ numVar ]->toString() << " d'indice : " << numVar << " se lie à " << toVar->toString() << " d'indice : " << indToVar << std::endl;

											// then we take the list of  node id associated to that var and
											// we determine randomly if we add another node to that list or not
											gum::Idx desiredNode = (gum::Idx) rand()%5;
											if( desiredNode  >= var2NodeIdMap[ toVar ]->size() ){
												var2NodeIdMap[ toVar ]->insert( f->addNonTerminalNode( *toVar ) );
												desiredNode = var2NodeIdMap[ toVar ]->size() - 1;
											}
											// then we add an arc between our current var associated node id and the considered var random node id
											f->insertArc( *numNode, (*var2NodeIdMap[ toVar ])[desiredNode], label );

										}else{

											// if we add
											NodeId toVal = f->addTerminalNode ( tnList[ rand()%tnList.size() ] );
											f->insertArc( *numNode, toVal, label );

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
				void saveDiagrams(Sequence< const DiscreteVariable* >* varList, MultiDimDecisionDiagramBase<float>* a1, MultiDimDecisionDiagramBase<float>* a2, MultiDimDecisionDiagramBase<float>* a3) {

					std::string dotfile = GET_PATH_STR ( DecisionDiagramError.log );
					std::ofstream output ( dotfile.c_str(), std::ios::out | std::ios::app );

					if ( ! output.good() )
						GUM_ERROR ( IOError, "Stream states flags are not all unset." );

					output << std::endl;
					for( SequenceIterator< const DiscreteVariable*> ite = varList->begin(); ite != varList->end(); ++ite )
						output << (*ite)->toString() << " - ";
					output << std::endl;

					output << a1->toDot();

					output << a2->toDot();

					output << a3->toDot();

					output.flush();

					output.close();

					if ( output.fail() )
						GUM_ERROR ( IOError, "Writting in the ostream failed." );
				}



			public :

				// ================================================================================================
				//
				// Test sur les fonctions avec valeurs exactes
				//
				// ================================================================================================
				void test_Operators_Functions_on_MultiDimDecisionDiagrams(){

					//~ std::cout << std::endl << "Don't worry, this test can be quiet long ...";

					// =====================================================================================
					// First we try with a predefine structure
					// =====================================================================================
					Sequence< const DiscreteVariable* >* varList = generateFixVarList();

					MultiDimDecisionDiagramBase<float>* a1 = NULL;
					TS_GUM_ASSERT_THROWS_NOTHING( a1 = generateDecisionDiagram1(varList) );

					MultiDimDecisionDiagramBase<float>* a2 = NULL;
					TS_GUM_ASSERT_THROWS_NOTHING( a2 = generateDecisionDiagram2(varList) );

					MultiDimDecisionDiagramBase<float>* a3 = NULL;

					//Test addition
					TS_GUM_ASSERT_THROWS_NOTHING( a3 = add2MultiDimDecisionDiagrams( a1 , a2 ) );
					if( a3 != NULL ){
						Instantiation inst(a3);
						bool haserror = false;
						for( inst.setFirst(); ! inst.end(); ++inst ){
							TS_ASSERT_DELTA( a3->get(inst), a1->get(inst) + a2->get(inst), 0.01 );
							if( a3->get(inst) != a1->get(inst) + a2->get(inst))
								haserror = true;
						}
						if( haserror )
							saveDiagrams(varList, a1, a2, a3);
						delete a3;
					}

					// Test Substraction
					TS_GUM_ASSERT_THROWS_NOTHING( a3 = subtract2MultiDimDecisionDiagrams( a1 , a2 ) );
					if( a3 != NULL ){
						Instantiation inst(a3);
						bool haserror = false;
						for( inst.setFirst(); ! inst.end(); ++inst ){
							TS_ASSERT_DELTA( a3->get(inst), a1->get(inst) - a2->get(inst), 0.01 );
							if( a3->get(inst) != a1->get(inst) - a2->get(inst))
								haserror = true;
						}
						if( haserror )
							saveDiagrams(varList, a1, a2, a3);
						delete a3;
					}

					// Test Multiplication
					TS_GUM_ASSERT_THROWS_NOTHING( a3 = multiply2MultiDimDecisionDiagrams( a1 , a2 ) );
					if( a3 != NULL ){
						Instantiation inst(a3);
						bool haserror = false;
						for( inst.setFirst(); ! inst.end(); ++inst ){
							TS_ASSERT_DELTA( a3->get(inst), a1->get(inst) * a2->get(inst), 0.01 );
							if( a3->get(inst) != a1->get(inst) * a2->get(inst))
								haserror = true;
						}
						if( haserror )
							saveDiagrams(varList, a1, a2, a3);
						delete a3;
					}

					// Test Division
					TS_GUM_ASSERT_THROWS_NOTHING( a3 = divide2MultiDimDecisionDiagrams( a1 , a2 ) );
					if( a3 != NULL ){
						Instantiation inst(a3);
						bool haserror = false;
						for( inst.setFirst(); ! inst.end(); ++inst ){
							TS_ASSERT_DELTA( a3->get(inst), a1->get(inst) / a2->get(inst), 0.01 );
							if( a3->get(inst) != a1->get(inst) / a2->get(inst))
								haserror = true;
						}
						if( haserror )
							saveDiagrams(varList, a1, a2, a3);
						delete a3;
					}

					// Test Maximum
					TS_GUM_ASSERT_THROWS_NOTHING( a3 = maximize2MultiDimDecisionDiagrams( a1 , a2 ) );
					if( a3 != NULL ){
						Instantiation inst(a3);
						bool haserror = false;
						for( inst.setFirst(); ! inst.end(); ++inst ){
							float v1 = a1->get(inst), v2 = a2->get(inst);
							float max = (v1>=v2?v1:v2);
							TS_ASSERT_DELTA( a3->get(inst), max, 0.01 );
							if( a3->get(inst) != ( max ))
								haserror = true;
						}
						if( haserror )
							saveDiagrams(varList, a1, a2, a3);
						delete a3;
					}

					delete a1;
					delete a2;

					for( SequenceIterator< const DiscreteVariable*> ite = varList->begin(); ite != varList->end(); ++ite )
						delete *ite;
					delete varList;

					// =====================================================================================
					// Then we try with random structure
					// =====================================================================================

					for(int i = 0; i < 100; i++ ){

						Sequence< const DiscreteVariable* >* varList = generateRandomVarList( i + 1 );
						shuffleVarList( varList, i + 2 );

						//~ std::cout << std::endl << " Sequence variable : ";
						//~ for( SequenceIterator< const DiscreteVariable* > iter = varList->begin(); iter != varList->end(); ++iter )
							//~ std::cout << (*iter)->toString() << " - ";
						//~ std::cout << std::endl;
						MultiDimDecisionDiagramBase<float>* a1 = NULL;
						TS_GUM_ASSERT_THROWS_NOTHING( a1 = generateRandomfloatDecisionDiagram(varList, i + 3) );
						//~ std::cout << std::endl << a1->toDot();

						shuffleVarList( varList, i + 4 );

						//~ std::cout << std::endl << " Sequence variable : ";
						//~ for( SequenceIterator< const DiscreteVariable* > iter = varList->begin(); iter != varList->end(); ++iter )
							//~ std::cout << (*iter)->toString() << " - ";
						//~ std::cout << std::endl;
						MultiDimDecisionDiagramBase<float>* a2 = NULL;
						TS_GUM_ASSERT_THROWS_NOTHING( a2 = generateRandomfloatDecisionDiagram(varList, i + 5) );
						//~ std::cout << std::endl << a2->toDot();

						MultiDimDecisionDiagramBase<float>* a3 = NULL;

						//Test addition
						TS_GUM_ASSERT_THROWS_NOTHING( a3 = add2MultiDimDecisionDiagrams( a1 , a2 ) );
						if( a3 != NULL ){
							Instantiation inst(a3);
							bool haserror = false;
							for( inst.setFirst(); ! inst.end(); ++inst ){
								TS_ASSERT_DELTA( a3->get(inst), a1->get(inst) + a2->get(inst), 0.01 );
								if( a3->get(inst) != a1->get(inst) + a2->get(inst))
									haserror = true;
							}
							if( haserror )
								saveDiagrams(varList, a1, a2, a3);
							delete a3;
						}

						// Test Substraction
						TS_GUM_ASSERT_THROWS_NOTHING( a3 = subtract2MultiDimDecisionDiagrams( a1 , a2 ) );
						if( a3 != NULL ){
							Instantiation inst(a3);
							bool haserror = false;
							for( inst.setFirst(); ! inst.end(); ++inst ){
								TS_ASSERT_DELTA( a3->get(inst), a1->get(inst) - a2->get(inst), 0.01 );
								if( a3->get(inst) != a1->get(inst) - a2->get(inst))
									haserror = true;
							}
							if( haserror )
								saveDiagrams(varList, a1, a2, a3);
							delete a3;
						}

						// Test Multiplication
						TS_GUM_ASSERT_THROWS_NOTHING( a3 = multiply2MultiDimDecisionDiagrams( a1 , a2 ) );
						if( a3 != NULL ){
							Instantiation inst(a3);
							bool haserror = false;
							for( inst.setFirst(); ! inst.end(); ++inst ){
								TS_ASSERT_DELTA( a3->get(inst), a1->get(inst) * a2->get(inst), 0.01 );
								if( a3->get(inst) != a1->get(inst) * a2->get(inst))
									haserror = true;
							}
							if( haserror )
								saveDiagrams(varList, a1, a2, a3);
							delete a3;
						}

						// Test Maximum
						TS_GUM_ASSERT_THROWS_NOTHING( a3 = maximize2MultiDimDecisionDiagrams( a1 , a2 ) );
						if( a3 != NULL ){
							Instantiation inst(a3);
							bool haserror = false;
							for( inst.setFirst(); ! inst.end(); ++inst ){
								TS_ASSERT_DELTA( a3->get(inst), a1->get(inst) > a2->get(inst) ? a1->get(inst) : a2->get(inst), 0.01 );
								if( a3->get(inst) != ( a1->get(inst) > a2->get(inst) ? a1->get(inst) : a2->get(inst) ))
									haserror = true;
							}
							if( haserror )
								saveDiagrams(varList, a1, a2, a3);
							delete a3;
						}

						delete a1;
						delete a2;

						for( SequenceIterator< const DiscreteVariable*> ite = varList->begin(); ite != varList->end(); ++ite )
							delete *ite;
						delete varList;
					}
				}
				// ================================================================================================

				// ================================================================================================
				//
				// Test sur les opérateurs avec valeurs exactes
				//
				// ================================================================================================
				void test_Operators_on_MultiDimDecisionDiagrams(){
					operators4MultiDimInit<float> ();

					// =====================================================================================
					// First we try with a predefine structure
					// =====================================================================================

					MultiDimDecisionDiagram<float> a1, a2;
					MultiDimDecisionDiagramBase<float>* a3 = NULL;

					//Test addition
					TS_GUM_ASSERT_THROWS_NOTHING( a3 = (MultiDimDecisionDiagramBase<float>*) ( a1 + a2 ) );
					if( a3 != NULL )
						delete a3;

					//Test subtraction
					TS_GUM_ASSERT_THROWS_NOTHING( a3 = (MultiDimDecisionDiagramBase<float>*) ( a1 - a2 ) );
					if( a3 != NULL )
						delete a3;

					//Test multiplication
					TS_GUM_ASSERT_THROWS_NOTHING( a3 = (MultiDimDecisionDiagramBase<float>*) ( a1 * a2 ) );
					if( a3 != NULL )
						delete a3;

					//Test division
					TS_GUM_ASSERT_THROWS_NOTHING( a3 = (MultiDimDecisionDiagramBase<float>*) ( a1 / a2 ) );
					if( a3 != NULL )
						delete a3;
				}

				// ================================================================================================
				//
				// Test sur les fonctions avec approximation
				//
				// ================================================================================================
				void test_Operation_in_MultiDimDecisionDiagrams_With_LinearApproximation(){

					//~ std::cout << std::endl << ", very long ...";

					// =====================================================================================
					// First we try with a predefine structure
					// =====================================================================================
					Sequence< const DiscreteVariable* >* varList = generateFixVarList();


					MultiDimDecisionDiagramFactory<float,LinearApproximationPolicy> f;
					f.setEpsilon( 0.9 );
					f.setLowLimit( 0 );
					f.setHighLimit( 4.5 );
					f.specifyVariablesSequence(  *varList );
					NodeId a = f.addNonTerminalNode( *( varList->atPos( 0 ) ) );
					NodeId b = f.addNonTerminalNode( *( varList->atPos( 1 ) ) );
					NodeId c = f.addNonTerminalNode( *( varList->atPos( 2 ) ) );

					NodeId d = f.addTerminalNode( 1 );
					NodeId e = f.addTerminalNode( 2 );
					NodeId g = f.addTerminalNode( 3 );

					f.insertArc( a, b, 0 );
					f.insertArc( a, c, 1 );

					f.insertArc( b, d, 0 );
					f.insertArc( b, c, 1 );

					f.insertArc( c, e, 0 );
					f.insertArc( c, g, 1 );

					MultiDimDecisionDiagramBase<float>* a1 = f.getMultiDimDecisionDiagram(false);


					f.clear();
					f.setEpsilon( 1.3 );
					f.setLowLimit( 3.5 );
					f.setHighLimit( 9 );

					f.specifyVariablesSequence(  *varList );
					a = f.addNonTerminalNode( *( varList->atPos( 0 ) ) );
					c = f.addNonTerminalNode( *( varList->atPos( 2 ) ) );

					d = f.addTerminalNode( 4 );
					e = f.addTerminalNode( 6 );
					g = f.addTerminalNode( 7 );

					f.insertArc( a, d, 0 );
					f.insertArc( a, c, 1 );

					f.insertArc( c, e, 0 );
					f.insertArc( c, g, 1 );

					MultiDimDecisionDiagramBase<float>* a2 = f.getMultiDimDecisionDiagram(false);


					MultiDimDecisionDiagramBase<float>* a3 = NULL;
					//Test addition
					TS_GUM_ASSERT_THROWS_NOTHING( a3 = add2MultiDimDecisionDiagrams( a1 , a2 ) );
					if( a3 != NULL ){
						Instantiation inst(a3);
						bool haserror = false;
						for( inst.setFirst(); ! inst.end(); ++inst ){
							TS_ASSERT_DELTA( a3->get(inst), a1->get(inst) + a2->get(inst), 0.9 );
							if( a3->get(inst) != a1->get(inst) + a2->get(inst))
								haserror = true;
						}
						if( haserror )
							saveDiagrams(varList, a1, a2, a3);
						delete a3;
					}

					// Test Substraction
					TS_GUM_ASSERT_THROWS_NOTHING( a3 = subtract2MultiDimDecisionDiagrams( a1 , a2 ) );
					if( a3 != NULL ){
						Instantiation inst(a3);
						bool haserror = false;
						for( inst.setFirst(); ! inst.end(); ++inst ){
							TS_ASSERT_DELTA( a3->get(inst), a1->get(inst) - a2->get(inst), 0.9 );
							if( a3->get(inst) != a1->get(inst) - a2->get(inst))
								haserror = true;
						}
						if( haserror )
							saveDiagrams(varList, a1, a2, a3);
						delete a3;
					}

					// Test Multiplication
					TS_GUM_ASSERT_THROWS_NOTHING( a3 = multiply2MultiDimDecisionDiagrams( a1 , a2 ) );
					if( a3 != NULL ){
						Instantiation inst(a3);
						bool haserror = false;
						for( inst.setFirst(); ! inst.end(); ++inst ){
							TS_ASSERT_DELTA( a3->get(inst), a1->get(inst) * a2->get(inst), 0.9 );
							if( a3->get(inst) != a1->get(inst) * a2->get(inst))
								haserror = true;
						}
						if( haserror )
							saveDiagrams(varList, a1, a2, a3);
						delete a3;
					}

					// Test Division
					TS_GUM_ASSERT_THROWS_NOTHING( a3 = divide2MultiDimDecisionDiagrams( a1 , a2 ) );
					if( a3 != NULL ){
						Instantiation inst(a3);
						bool haserror = false;
						for( inst.setFirst(); ! inst.end(); ++inst ){
							TS_ASSERT_DELTA( a3->get(inst), a1->get(inst) / a2->get(inst), 0.9 );
							if( a3->get(inst) != a1->get(inst) / a2->get(inst))
								haserror = true;
						}
						if( haserror )
							saveDiagrams(varList, a1, a2, a3);
						delete a3;
					}

					// Test Maximum
					TS_GUM_ASSERT_THROWS_NOTHING( a3 = maximize2MultiDimDecisionDiagrams( a1 , a2 ) );
					if( a3 != NULL ){
						Instantiation inst(a3);
						bool haserror = false;
						for( inst.setFirst(); ! inst.end(); ++inst ){
							float v1 = a1->get(inst), v2 = a2->get(inst);
							float max = (v1>=v2?v1:v2);
							TS_ASSERT_DELTA( a3->get(inst), max, 0.9 );
							if( a3->get(inst) != ( max ))
								haserror = true;
						}
						if( haserror )
							saveDiagrams(varList, a1, a2, a3);
						delete a3;
					}

					delete a1;
					delete a2;

					for( SequenceIterator< const DiscreteVariable*> ite = varList->begin(); ite != varList->end(); ++ite )
						delete *ite;
					delete varList;

					// =====================================================================================
					// Then we try with random structure
					// =====================================================================================
					for(int i = 0; i < 100; i++ ){

						Sequence< const DiscreteVariable* >* varList = generateRandomVarList( i + 1 );
						shuffleVarList( varList, i + 2 );
						//~ std::cout << std::endl << " Sequence variable : ";
						//~ for( SequenceIterator< const DiscreteVariable* > iter = varList->begin(); iter != varList->end(); ++iter )
							//~ std::cout << (*iter)->toString() << " - ";
						//~ std::cout << std::endl;

						f.clear();
						f.setEpsilon( 7 );
						f.setLowLimit( -42 );
						f.setHighLimit( 69 );

						MultiDimDecisionDiagramBase<float>* a1 = NULL;
						TS_GUM_ASSERT_THROWS_NOTHING( a1 = generateRandomfloatDecisionDiagram(varList, i + 3, &f) );
						//~ std::cout << std::endl << a1->toDot();

						shuffleVarList( varList, i + 4 );
						//~ std::cout << std::endl << " Sequence variable : ";
						//~ for( SequenceIterator< const DiscreteVariable* > iter = varList->begin(); iter != varList->end(); ++iter )
							//~ std::cout << (*iter)->toString() << " - ";
						//~ std::cout << std::endl;

						f.clear();
						f.setEpsilon( 11 );
						f.setLowLimit( -53 );
						f.setHighLimit( 79 );

						MultiDimDecisionDiagramBase<float>* a2 = NULL;
						TS_GUM_ASSERT_THROWS_NOTHING( a2 = generateRandomfloatDecisionDiagram(varList, i + 5, &f) );
						//~ std::cout << std::endl << a2->toDot();

						MultiDimDecisionDiagramBase<float>* a3 = NULL;

						//Test addition
						TS_GUM_ASSERT_THROWS_NOTHING( a3 = add2MultiDimDecisionDiagrams( a1 , a2 ) );
						if( a3 != NULL ){
							Instantiation inst(a3);
							bool haserror = false;
							for( inst.setFirst(); ! inst.end(); ++inst ){
								TS_ASSERT_DELTA( a3->get(inst), a1->get(inst) + a2->get(inst), 7 );
								if( a3->get(inst) != a1->get(inst) + a2->get(inst))
									haserror = true;
							}
							if( haserror )
								saveDiagrams(varList, a1, a2, a3);
							delete a3;
						}

						// Test Substraction
						TS_GUM_ASSERT_THROWS_NOTHING( a3 = subtract2MultiDimDecisionDiagrams( a1 , a2 ) );
						if( a3 != NULL ){
							Instantiation inst(a3);
							bool haserror = false;
							for( inst.setFirst(); ! inst.end(); ++inst ){
								TS_ASSERT_DELTA( a3->get(inst), a1->get(inst) - a2->get(inst), 7 );
								if( a3->get(inst) != a1->get(inst) - a2->get(inst))
									haserror = true;
							}
							if( haserror )
								saveDiagrams(varList, a1, a2, a3);
							delete a3;
						}

						// Test Multiplication
						TS_GUM_ASSERT_THROWS_NOTHING( a3 = multiply2MultiDimDecisionDiagrams( a1 , a2 ) );
						if( a3 != NULL ){
							Instantiation inst(a3);
							bool haserror = false;
							for( inst.setFirst(); ! inst.end(); ++inst ){
								TS_ASSERT_DELTA( a3->get(inst), a1->get(inst) * a2->get(inst), 7 );
								if( a3->get(inst) != a1->get(inst) * a2->get(inst))
									haserror = true;
							}
							if( haserror )
								saveDiagrams(varList, a1, a2, a3);
							delete a3;
						}

						// Test Maximum
						TS_GUM_ASSERT_THROWS_NOTHING( a3 = maximize2MultiDimDecisionDiagrams( a1 , a2 ) );
						if( a3 != NULL ){
							Instantiation inst(a3);
							bool haserror = false;
							for( inst.setFirst(); ! inst.end(); ++inst ){
								float v1 = a1->get(inst), v2 = a2->get(inst);
								float max = (v1>=v2?v1:v2);
								TS_ASSERT_DELTA( a3->get(inst), max, 7 );
								if( a3->get(inst) != ( max ))
									haserror = true;
							}
							if( haserror )
								saveDiagrams(varList, a1, a2, a3);
							delete a3;
						}

						delete a1;
						delete a2;

						for( SequenceIterator< const DiscreteVariable*> ite = varList->begin(); ite != varList->end(); ++ite )
							delete *ite;
						delete varList;
					}
				}











































				void _Reproducteur(){

					//~ LabelizedVariable* v0 = new LabelizedVariable( "0", "", 3 );
					//~ LabelizedVariable* v1 = new LabelizedVariable( "1", "", 3 );
					//~ LabelizedVariable* v2 = new LabelizedVariable( "2", "", 2 );
					//~ LabelizedVariable* v3 = new LabelizedVariable( "3", "", 2 );
					//~ LabelizedVariable* v4 = new LabelizedVariable( "4", "", 3 );
					//~ LabelizedVariable* v5 = new LabelizedVariable( "5", "", 3 );
					//~ LabelizedVariable* v6 = new LabelizedVariable( "6", "", 3 );
					//~ LabelizedVariable* v7 = new LabelizedVariable( "7", "", 2 );
					//~ LabelizedVariable* v8 = new LabelizedVariable( "8", "", 2 );
					//~ LabelizedVariable* v9 = new LabelizedVariable( "9", "", 3 );
					//~
										//~
					//~
					//~ MultiDimDecisionDiagramFactory<float> facto;
					//~
					//~
					//~ Sequence< const DiscreteVariable* > seq;
					//~ seq.insert( v6 );
					//~ seq.insert( v5 );
					//~ seq.insert( v1 );
					//~ seq.insert( v9 );
					//~ seq.insert( v2 );
					//~ seq.insert( v3 );
					//~ seq.insert( v0 );
					//~ seq.insert( v7 );
					//~ seq.insert( v8 );
					//~ seq.insert( v4 );
					//~ std::cout << std::endl << " Sequence variable : ";
					//~ for( SequenceIterator< const DiscreteVariable* > iter = seq.begin(); iter != seq.end(); ++iter )
						//~ std::cout << (*iter)->toString() << " - ";
					//~ std::cout << std::endl;
					//~
					//~ facto.specifyVariablesSequence( seq );
					//~
					//~ NodeId n111 = facto.addNonTerminalNode( *v1 );
					//~ NodeId n121 = facto.addNonTerminalNode( *v2 );
					//~ NodeId n141 = facto.addNonTerminalNode( *v4 );
					//~ NodeId n181 = facto.addNonTerminalNode( *v8 );
					//~ NodeId n182 = facto.addNonTerminalNode( *v8 );
					//~ NodeId n191 = facto.addNonTerminalNode( *v9 );
					//~
					//~ NodeId n1t38 = facto.addTerminalNode( 38 );
					//~ NodeId n1t27 = facto.addTerminalNode( 27 );
					//~ NodeId n1tm93 = facto.addTerminalNode( -93 );
					//~ NodeId n1t2 = facto.addTerminalNode( 2 );
					//~ NodeId n1tm50 = facto.addTerminalNode( -50 );
					//~ NodeId n1t61 = facto.addTerminalNode( 61 );
					//~
					//~ facto.insertArc( n111, n191, 0 );
					//~ facto.insertArc( n111, n1t38, 1 );
					//~
					//~ facto.insertArc( n121, n182, 0 );
					//~ facto.insertArc( n121, n1t27, 1 );
					//~
					//~ facto.insertArc( n141, n1t2, 0 );
					//~ facto.insertArc( n141, n1tm50, 1 );
					//~ facto.insertArc( n141, n1t61, 2 );
					//~
					//~ facto.insertArc( n181, n141, 0 );
					//~
					//~ facto.insertArc( n182, n1tm93, 0 );
					//~
					//~ facto.insertArc( n191, n121, 0 );
					//~ facto.insertArc( n191, n181, 1 );
					//~
					//~ MultiDimDecisionDiagramBase<float>* a1 = facto.getMultiDimDecisionDiagram();
					//~ std::cout << std::endl << a1->toDot();
					//~
					//~
					//~
					//~ facto.clear();
					//~ seq.clear();
					//~ seq.insert( v8 );
					//~ seq.insert( v5 );
					//~ seq.insert( v6 );
					//~ seq.insert( v9 );
					//~ seq.insert( v4 );
					//~ seq.insert( v3 );
					//~ seq.insert( v0 );
					//~ seq.insert( v1 );
					//~ seq.insert( v7 );
					//~ seq.insert( v2 );
					//~ std::cout << std::endl << " Sequence variable : ";
					//~ for( SequenceIterator< const DiscreteVariable* > iter = seq.begin(); iter != seq.end(); ++iter )
						//~ std::cout << (*iter)->toString() << " - ";
					//~ std::cout << std::endl;
					//~
					//~ facto.specifyVariablesSequence( seq );
					//~
					//~ NodeId n201 = facto.addNonTerminalNode( *v0 );
					//~ NodeId n211 = facto.addNonTerminalNode( *v1 );
					//~ NodeId n221 = facto.addNonTerminalNode( *v2 );
					//~ NodeId n222 = facto.addNonTerminalNode( *v2 );
					//~ NodeId n241 = facto.addNonTerminalNode( *v4 );
					//~ NodeId n251 = facto.addNonTerminalNode( *v5 );
					//~
					//~ NodeId n2t59 = facto.addTerminalNode( 59 );
					//~ NodeId n2t12 = facto.addTerminalNode( 12 );
					//~ NodeId n2t92 = facto.addTerminalNode( 92 );
					//~ NodeId n2tm93 = facto.addTerminalNode( -93 );
					//~ NodeId n2tm99 = facto.addTerminalNode( -99 );
					//~
					//~ facto.insertArc( n201, n211, 0 );
					//~
					//~ facto.insertArc( n211, n221, 0 );
					//~ facto.insertArc( n211, n222, 1 );
					//~
					//~ facto.insertArc( n221, n2t92, 0 );
					//~ facto.insertArc( n221, n2t92, 1 );
					//~
					//~ facto.insertArc( n222, n2tm93, 0 );
					//~ facto.insertArc( n222, n2tm99, 1 );
					//~
					//~ facto.insertArc( n241, n2t12, 0 );
					//~
					//~ facto.insertArc( n251, n2t59, 0 );
					//~ facto.insertArc( n251, n201, 1 );
					//~ facto.insertArc( n251, n241, 2 );
					//~
					//~ MultiDimDecisionDiagramBase<float>* a2 = facto.getMultiDimDecisionDiagram();
					//~ std::cout << std::endl << a2->toDot();
					//~
					//~
					//~
					//~ MultiDimDecisionDiagramBase<float>* a3 = NULL;
					//~
					//~ //Test addition
					//~ TS_GUM_ASSERT_THROWS_NOTHING( a3 = add2MultiDimDecisionDiagrams( a1 , a2 ) );
					//~ if( a3 != NULL ){
						//~ Instantiation inst(a3);
						//~ bool haserror = false;
						//~ for( inst.setFirst(); ! inst.end(); ++inst ){
							//~ TS_ASSERT_DELTA( a3->get(inst), a1->get(inst) + a2->get(inst), 0.01 );
							//~ if( a3->get(inst) != a1->get(inst) + a2->get(inst))
								//~ haserror = true;
						//~ }
						//~ if( haserror )
							//~ saveDiagrams(varList, a1, a2, a3);
						//~ delete a3;
					//~ }
					//~
					//~ delete a1;
					//~ delete a2;
					//~
					//~ delete  v0;
					//~ delete  v1;
					//~ delete  v2;
					//~ delete  v3;
					//~ delete  v4;
					//~ delete  v5;
					//~ delete  v6;
					//~ delete  v7;
					//~ delete  v8;
					//~ delete  v9;
				}
				// ================================================================================================

		};
	}
}
