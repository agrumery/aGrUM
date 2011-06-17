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
#include <agrum/core/approximationPolicy.h>
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
				// Génération aléatoire d'une liste de 10 variables
				// ================================================================================================
				Sequence< const DiscreteVariable* >* generateRandomVarList( int i ){
					srand(i);
					Sequence< const DiscreteVariable* >* ret = new Sequence< const DiscreteVariable* >();
					
					for( int i = 0; i < 10; i++ ){
						std::stringstream varName;
						varName << "var" << i;
						ret->insert( new LabelizedVariable( varName.str(), "", 2 + rand()%2 ) );
					}
					return ret;
				}
				// ================================================================================================
				
				// ================================================================================================
				// Génération aléatoire de diagramme de décision
				// ================================================================================================
				MultiDimDecisionDiagramBase<float>* generateRandomfloatDecisionDiagram(Sequence< const DiscreteVariable* >* varList, MultiDimDecisionDiagramFactoryBase<float>* f = NULL) {
					
					MultiDimDecisionDiagramBase<float>* ret = NULL;
					bool factoryCreatedHere = false;
					
					while( ret == NULL || ( ret->diagramVarSize() > 5 ) ){
						
						if( ret != NULL )
							delete ret;
							
						if( f == NULL ){
							factoryCreatedHere = true;
							f = new MultiDimDecisionDiagramFactory<float>();
							f->setEpsilon( 1 );
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
						uint i = 0; 
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
									uint nbArc = 1 + rand()%((*varList)[ numVar ]->domainSize());
									
									// for those arcs
									for( Idx label = 0; label < nbArc; label++ ) {
										
										// We first determine if it goes directly to a terminal node or node
										if( numVar != ( varList->size() - 1 ) && (rand()%100) >= 42 ){
										
											// if not, we determine randomly to which var it could go
											uint indToVar = ( rand()%( varList->size() - 1 - numVar ) ) + numVar + 1;
											const DiscreteVariable* toVar = (*varList)[ indToVar ];
											//~ std::cout << "Variable : " << (*varList)[ numVar ]->toString() << " d'indice : " << numVar << " se lie à " << toVar->toString() << " d'indice : " << indToVar << std::endl;
											
											// then we take the list of  node id associated to that var and
											// we determine randomly if we add another node to that list or not
											uint desiredNode = (uint) rand()%5;
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
				// Génération fixe de diagramme de décision
				// ================================================================================================
				MultiDimDecisionDiagramBase<float>* generateDecisionDiagram1(Sequence< const DiscreteVariable* >* varList){
					
					MultiDimDecisionDiagramFactory<float> f;
					f.specifyVariablesSequence(  *varList );
					NodeId a = f.addNonTerminalNode( *( varList->atPos( 0 ) ) );
					NodeId b = f.addNonTerminalNode( *( varList->atPos( 1 ) ) );
					NodeId c = f.addNonTerminalNode( *( varList->atPos( 2 ) ) );
					
					NodeId d = f.addTerminalNode( 6 );
					NodeId e = f.addTerminalNode( 2 );
					NodeId g = f.addTerminalNode( 3 );
					
					f.insertArc( a, b, 0 );
					f.insertArc( a, c, 1 );
					
					f.insertArc( b, d, 0 );
					f.insertArc( b, c, 1 );
					
					f.insertArc( c, e, 0 );
					f.insertArc( c, g, 1 );
					
					return f.getMultiDimDecisionDiagram(false);
					
				}
				// ================================================================================================
				
				// ================================================================================================
				// Génération fixe de diagramme de décision
				// ================================================================================================
				MultiDimDecisionDiagramBase<float>* generateDecisionDiagram2(Sequence< const DiscreteVariable* >* varList){
					
					MultiDimDecisionDiagramFactory<float> f;
					f.specifyVariablesSequence(  *varList );
					NodeId a = f.addNonTerminalNode( *( varList->atPos( 0 ) ) );
					NodeId c = f.addNonTerminalNode( *( varList->atPos( 2 ) ) );
					
					NodeId d = f.addTerminalNode( 4 );
					NodeId e = f.addTerminalNode( 5 );
					NodeId g = f.addTerminalNode( 1 );
					
					f.insertArc( a, d, 0 );
					f.insertArc( a, c, 1 );
					
					f.insertArc( c, e, 0 );
					f.insertArc( c, g, 1 );
					
					return f.getMultiDimDecisionDiagram(false);
					
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
					
					//~ for(int i = 0; i < 10; i++ ){
						//~ 
						//~ Sequence< const DiscreteVariable* >* varList = generateRandomVarList( i + 1 );
						//~ 
						//~ sleep(1);
						//~ 
						//~ MultiDimDecisionDiagramBase<float>* a1 = NULL;
						//~ TS_GUM_ASSERT_THROWS_NOTHING( a1 = generateRandomfloatDecisionDiagram(varList) );
						//~ 
						//~ sleep(1);
						//~ 
						//~ MultiDimDecisionDiagramBase<float>* a2 = NULL;
						//~ TS_GUM_ASSERT_THROWS_NOTHING( a2 = generateRandomfloatDecisionDiagram(varList) );
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
						//~ // Test Substraction
						//~ TS_GUM_ASSERT_THROWS_NOTHING( a3 = subtract2MultiDimDecisionDiagrams( a1 , a2 ) );					
						//~ if( a3 != NULL ){
							//~ Instantiation inst(a3);
							//~ bool haserror = false;
							//~ for( inst.setFirst(); ! inst.end(); ++inst ){
								//~ TS_ASSERT_DELTA( a3->get(inst), a1->get(inst) - a2->get(inst), 0.01 );
								//~ if( a3->get(inst) != a1->get(inst) - a2->get(inst))
									//~ haserror = true;
							//~ }
							//~ if( haserror )
								//~ saveDiagrams(varList, a1, a2, a3);
							//~ delete a3;
						//~ }
						//~ 
						//~ // Test Multiplication
						//~ TS_GUM_ASSERT_THROWS_NOTHING( a3 = multiply2MultiDimDecisionDiagrams( a1 , a2 ) );					
						//~ if( a3 != NULL ){
							//~ Instantiation inst(a3);
							//~ bool haserror = false;
							//~ for( inst.setFirst(); ! inst.end(); ++inst ){
								//~ TS_ASSERT_DELTA( a3->get(inst), a1->get(inst) * a2->get(inst), 0.01 );
								//~ if( a3->get(inst) != a1->get(inst) * a2->get(inst))
									//~ haserror = true;
							//~ }
							//~ if( haserror )
								//~ saveDiagrams(varList, a1, a2, a3);
							//~ delete a3;
						//~ }
						//~ 
						//~ // Test Maximum
						//~ TS_GUM_ASSERT_THROWS_NOTHING( a3 = maximize2MultiDimDecisionDiagrams( a1 , a2 ) );					
						//~ if( a3 != NULL ){
							//~ Instantiation inst(a3);
							//~ bool haserror = false;
							//~ for( inst.setFirst(); ! inst.end(); ++inst ){
								//~ TS_ASSERT_DELTA( a3->get(inst), a1->get(inst) > a2->get(inst) ? a1->get(inst) : a2->get(inst), 0.01 );
								//~ if( a3->get(inst) != ( a1->get(inst) > a2->get(inst) ? a1->get(inst) : a2->get(inst) ))
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
						//~ for( SequenceIterator< const DiscreteVariable*> ite = varList->begin(); ite != varList->end(); ++ite )
							//~ delete *ite;
						//~ delete varList;
					//~ }
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
					
					//Test subtraction
					TS_GUM_ASSERT_THROWS_NOTHING( a3 = (MultiDimDecisionDiagramBase<float>*) ( a1 - a2 ) );	
					
					//Test multiplication
					TS_GUM_ASSERT_THROWS_NOTHING( a3 = (MultiDimDecisionDiagramBase<float>*) ( a1 * a2 ) );	
					
					//Test division
					TS_GUM_ASSERT_THROWS_NOTHING( a3 = (MultiDimDecisionDiagramBase<float>*) ( a1 / a2 ) );	
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
					//~ for(int i = 0; i < 10; i++ ){
										//~ 
						//~ f.clear();
						//~ f.setEpsilon( 7 );
						//~ f.setLowLimit( -42 );
						//~ f.setHighLimit( 69 );
						//~ 
						//~ Sequence< const DiscreteVariable* >* varList = generateRandomVarList( i + 1 );
						//~ 
						//~ sleep(1);
						//~ 
						//~ MultiDimDecisionDiagramBase<float>* a1 = NULL;
						//~ TS_GUM_ASSERT_THROWS_NOTHING( a1 = generateRandomfloatDecisionDiagram(varList, &f) );
						//~ 
						//~ sleep(1);
						//~ 
										//~ 
						//~ f.clear();
						//~ f.setEpsilon( 11 );
						//~ f.setLowLimit( -53 );
						//~ f.setHighLimit( 79 );
						//~ MultiDimDecisionDiagramBase<float>* a2 = NULL;
						//~ TS_GUM_ASSERT_THROWS_NOTHING( a2 = generateRandomfloatDecisionDiagram(varList, &f) );
						//~ 
						//~ MultiDimDecisionDiagramBase<float>* a3 = NULL;
						//~ //Test addition
						//~ TS_GUM_ASSERT_THROWS_NOTHING( a3 = add2MultiDimDecisionDiagrams( a1 , a2 ) );					
						//~ if( a3 != NULL ){
							//~ Instantiation inst(a3);
							//~ bool haserror = false;
							//~ for( inst.setFirst(); ! inst.end(); ++inst ){
								//~ TS_ASSERT_DELTA( a3->get(inst), a1->get(inst) + a2->get(inst), 7 );
								//~ if( a3->get(inst) != a1->get(inst) + a2->get(inst))
									//~ haserror = true;
							//~ }
							//~ if( haserror )
								//~ saveDiagrams(varList, a1, a2, a3);
							//~ delete a3;
						//~ }
						//~ 
						//~ // Test Substraction
						//~ TS_GUM_ASSERT_THROWS_NOTHING( a3 = subtract2MultiDimDecisionDiagrams( a1 , a2 ) );					
						//~ if( a3 != NULL ){
							//~ Instantiation inst(a3);
							//~ bool haserror = false;
							//~ for( inst.setFirst(); ! inst.end(); ++inst ){
								//~ TS_ASSERT_DELTA( a3->get(inst), a1->get(inst) - a2->get(inst), 7 );
								//~ if( a3->get(inst) != a1->get(inst) - a2->get(inst))
									//~ haserror = true;
							//~ }
							//~ if( haserror )
								//~ saveDiagrams(varList, a1, a2, a3);
							//~ delete a3;
						//~ }
						//~ 
						//~ // Test Multiplication
						//~ TS_GUM_ASSERT_THROWS_NOTHING( a3 = multiply2MultiDimDecisionDiagrams( a1 , a2 ) );					
						//~ if( a3 != NULL ){
							//~ Instantiation inst(a3);
							//~ bool haserror = false;
							//~ for( inst.setFirst(); ! inst.end(); ++inst ){
								//~ TS_ASSERT_DELTA( a3->get(inst), a1->get(inst) * a2->get(inst), 7 );
								//~ if( a3->get(inst) != a1->get(inst) * a2->get(inst))
									//~ haserror = true;
							//~ }
							//~ if( haserror )
								//~ saveDiagrams(varList, a1, a2, a3);
							//~ delete a3;
						//~ }
						//~ 
						//~ // Test Maximum
						//~ TS_GUM_ASSERT_THROWS_NOTHING( a3 = maximize2MultiDimDecisionDiagrams( a1 , a2 ) );	
						//~ if( a3 != NULL ){
							//~ Instantiation inst(a3);
							//~ bool haserror = false;
							//~ for( inst.setFirst(); ! inst.end(); ++inst ){
								//~ float v1 = a1->get(inst), v2 = a2->get(inst);
								//~ float max = (v1>=v2?v1:v2);				
								//~ TS_ASSERT_DELTA( a3->get(inst), max, 7 );
								//~ if( a3->get(inst) != ( max ))
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
						//~ for( SequenceIterator< const DiscreteVariable*> ite = varList->begin(); ite != varList->end(); ++ite )
							//~ delete *ite;
						//~ delete varList;
					//~ }
				}
			
		};
	}
}
