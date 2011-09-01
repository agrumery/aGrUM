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
				MultiDimDecisionDiagramBase<float>* __generateDecisionDiagram1( const Sequence< const DiscreteVariable* >* varList){
					
					MultiDimDecisionDiagramFactory<float> facto;
					facto.setVariablesSequence(  *varList );
					NodeId a = facto.addNonTerminalNode( varList->atPos( 0 ) );
					NodeId b = facto.addNonTerminalNode( varList->atPos( 1 ) );
					NodeId c = facto.addNonTerminalNode( varList->atPos( 2 ) );
					
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
				MultiDimDecisionDiagramBase<float>* __generateDecisionDiagram2( const Sequence< const DiscreteVariable* >* varList){
					
					MultiDimDecisionDiagramFactory<float> facto;
					facto.setVariablesSequence(  *varList );
					NodeId a = facto.addNonTerminalNode( varList->atPos( 0 ) );
					NodeId c = facto.addNonTerminalNode( varList->atPos( 2 ) );
					
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
				MultiDimDecisionDiagramBase<float>* __generateRandomfloatDecisionDiagram( const Sequence< const DiscreteVariable* >* varList, int i, MultiDimDecisionDiagramFactoryBase<float>* f = NULL ) {
					
					srand( time(NULL) + i );
					MultiDimDecisionDiagramBase<float>* ret = NULL;
					bool factoryCreatedHere = false;
					
					while( ret == NULL || ( ret->diagramVarSize() < 3) || ( ret->diagramVarSize() > 3 ) ){
						
						if( ret != NULL )
							delete ret;
							
						if( f == NULL ){
							factoryCreatedHere = true;
							f = new MultiDimDecisionDiagramFactory<float>();
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
						List<float> tnList;
						float interval = f->highLimit() - f->lowLimit();
						Idx i = 0; 
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
								var2NodeIdMap[ (*varList)[ initVar ] ]->insert( f->addNonTerminalNode( (*varList)[ initVar ] ) );
								
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
											Idx indToVar = ( rand()%( varList->size() - 1 - numVar ) ) + numVar + 1;
											const DiscreteVariable* toVar = (*varList)[ indToVar ];
											//~ std::cout << "Variable : " << (*varList)[ numVar ]->toString() << " d'indice : " << numVar << " se lie à " << toVar->toString() << " d'indice : " << indToVar << std::endl;
											
											// then we take the list of  node id associated to that var and
											// we determine randomly if we add another node to that list or not
											Idx desiredNode = (Idx) rand()%5;
											if( desiredNode  >= var2NodeIdMap[ toVar ]->size() ){
												var2NodeIdMap[ toVar ]->insert( f->addNonTerminalNode( toVar ) );
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
				void __saveDiagrams(MultiDimDecisionDiagramBase<float>* a1, MultiDimDecisionDiagramBase<float>* a2, MultiDimDecisionDiagramBase<float>* a3) {

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
					Sequence< const DiscreteVariable* >* varList = __generateFixVarList();
					
					MultiDimDecisionDiagramBase<float>* a1 = NULL;
					TS_GUM_ASSERT_THROWS_NOTHING( a1 = __generateDecisionDiagram1(varList) );	
					
					MultiDimDecisionDiagramBase<float>* a2 = NULL;
					TS_GUM_ASSERT_THROWS_NOTHING( a2 = __generateDecisionDiagram2(varList) );	
					
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
						if( haserror ){
							GUM_TRACE( std::endl << a1->toDot() << std::endl << a2->toDot() << std::endl << a3->toDot() );
						}
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
						if( haserror ){
							GUM_TRACE( std::endl << a1->toDot() << std::endl << a2->toDot() << std::endl << a3->toDot() );
						}
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
						if( haserror ){
							GUM_TRACE( std::endl << a1->toDot() << std::endl << a2->toDot() << std::endl << a3->toDot() );
						}
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
						if( haserror ){
							GUM_TRACE( std::endl << a1->toDot() << std::endl << a2->toDot() << std::endl << a3->toDot() );
						}
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
						if( haserror ){
							GUM_TRACE( std::endl << a1->toDot() << std::endl << a2->toDot() << std::endl << a3->toDot() );
						}
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
					
					//~ for(int i = 0; i < 100; i++ ){
						//~ 
						//~ GUM_TRACE( i+1 << "ème itération" << std::endl );
						//~ 
						//~ Sequence< const DiscreteVariable* >* varList = __generateRandomVarList( i + 1 );						
						//~ __shuffleVarList( varList, i + 2 );
						//~ 
						//~ MultiDimDecisionDiagramBase<float>* a1 = NULL;
						//~ TS_GUM_ASSERT_THROWS_NOTHING( a1 = __generateRandomfloatDecisionDiagram(varList, i + 3) );
						//~ 
						//~ __shuffleVarList( varList, i + 4 );
						//~ 
						//~ MultiDimDecisionDiagramBase<float>* a2 = NULL;
						//~ TS_GUM_ASSERT_THROWS_NOTHING( a2 = __generateRandomfloatDecisionDiagram(varList, i + 5) );
						//~ 
						//~ MultiDimDecisionDiagramBase<float>* a3 = NULL;
						//~ 
						//~ bool haserror = false;
							//~ 
						//~ //Test addition
						//~ TS_GUM_ASSERT_THROWS_NOTHING( a3 = add2MultiDimDecisionDiagrams( a1 , a2 ) );			
						//~ if( a3 != NULL ){
							//~ Instantiation inst(a3);
							//~ for( inst.setFirst(); ! inst.end(); ++inst ){
								//~ TS_ASSERT_DELTA( a3->get(inst), a1->get(inst) + a2->get(inst), 0.01 );
								//~ if( a3->get(inst) != a1->get(inst) + a2->get(inst)){
									//~ GUM_TRACE("AQUI!");
									//~ __saveDiagrams(a1, a2, a3);
									//~ haserror = true;
									//~ break;
								//~ }
							//~ }
							//~ delete a3;
						//~ } else {
							//~ __saveDiagrams(a1, a2, a3);
							//~ haserror = true;
						//~ }
						//~ 
						//~ // Test Substraction
						//~ if( !haserror ){
							//~ TS_GUM_ASSERT_THROWS_NOTHING( a3 = subtract2MultiDimDecisionDiagrams( a1 , a2 ) );					
							//~ if( a3 != NULL ){
								//~ Instantiation inst(a3);
								//~ for( inst.setFirst(); ! inst.end(); ++inst ){
									//~ TS_ASSERT_DELTA( a3->get(inst), a1->get(inst) - a2->get(inst), 0.01 );
									//~ if( a3->get(inst) != a1->get(inst) - a2->get(inst)){
										//~ GUM_TRACE("AQUI!");
										//~ __saveDiagrams(a1, a2, a3);
										//~ haserror = true;
										//~ break;
									//~ }
								//~ }
								//~ delete a3;
							//~ } else {
								//~ __saveDiagrams(a1, a2, a3);
								//~ haserror = true;
							//~ }
						//~ }
						//~ 
						//~ // Test Multiplication
						//~ if( !haserror ){
							//~ TS_GUM_ASSERT_THROWS_NOTHING( a3 = multiply2MultiDimDecisionDiagrams( a1 , a2 ) );					
							//~ if( a3 != NULL ){
								//~ Instantiation inst(a3);
								//~ for( inst.setFirst(); ! inst.end(); ++inst ){
									//~ TS_ASSERT_DELTA( a3->get(inst), a1->get(inst) * a2->get(inst), 0.01 );
									//~ if( a3->get(inst) != a1->get(inst) * a2->get(inst)){
										//~ GUM_TRACE("AQUI!");
										//~ __saveDiagrams(a1, a2, a3);
										//~ haserror = true;
										//~ break;
									//~ }
								//~ }
								//~ delete a3;
							//~ } else {
								//~ __saveDiagrams(a1, a2, a3);
								//~ haserror = true;
							//~ }
						//~ }
						//~ 
						//~ // Test Maximum
						//~ if( !haserror ){
							//~ TS_GUM_ASSERT_THROWS_NOTHING( a3 = maximize2MultiDimDecisionDiagrams( a1 , a2 ) );					
							//~ if( a3 != NULL ){
								//~ Instantiation inst(a3);
								//~ for( inst.setFirst(); ! inst.end(); ++inst ){
									//~ TS_ASSERT_DELTA( a3->get(inst), a1->get(inst) > a2->get(inst) ? a1->get(inst) : a2->get(inst), 0.01 );
									//~ if( a3->get(inst) != ( a1->get(inst) > a2->get(inst) ? a1->get(inst) : a2->get(inst) )){
										//~ GUM_TRACE("AQUI!");
										//~ __saveDiagrams(a1, a2, a3);
										//~ haserror = true;
										//~ break;
									//~ }
								//~ }
								//~ delete a3;
							//~ } else {
								//~ __saveDiagrams(a1, a2, a3);
								//~ haserror = true;
							//~ }
						//~ }
						//~ 
						//~ delete a1;
						//~ delete a2;
						//~ 
						//~ for( SequenceIterator< const DiscreteVariable*> ite = varList->begin(); ite != varList->end(); ++ite )
							//~ delete *ite;
						//~ delete varList;
						//~ 
						//~ if( haserror )
							//~ break;
					//~ }
				}
				// ================================================================================================
				
				// ================================================================================================
				//
				// Test sur les opérateurs avec valeurs exactes
				//
				// ================================================================================================
				void _Operators_on_MultiDimDecisionDiagrams(){
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
				void _Operation_in_MultiDimDecisionDiagrams_With_LinearApproximation(){
					
					//~ std::cout << std::endl << ", very long ...";
					
					// =====================================================================================
					// First we try with a predefine structure
					// =====================================================================================
					Sequence< const DiscreteVariable* >* varList = __generateFixVarList();
					
					
					MultiDimDecisionDiagramFactory<float,LinearApproximationPolicy> f;
					f.setEpsilon( 0.9 );
					f.setLowLimit( 0 );
					f.setHighLimit( 4.5 );
					f.setVariablesSequence(  *varList );
					NodeId a = f.addNonTerminalNode( varList->atPos( 0 ) );
					NodeId b = f.addNonTerminalNode( varList->atPos( 1 ) );
					NodeId c = f.addNonTerminalNode( varList->atPos( 2 ) );
					
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
					
					f.setVariablesSequence(  *varList );
					a = f.addNonTerminalNode( varList->atPos( 0 ) );
					c = f.addNonTerminalNode( varList->atPos( 2 ) );
					
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
							//~ __saveDiagrams(varList, a1, a2, a3);
						delete a3;
					} else
							return;
					
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
							//~ __saveDiagrams(varList, a1, a2, a3);
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
							//~ __saveDiagrams(varList, a1, a2, a3);
						delete a3;
					} else
							return;
					
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
							//~ __saveDiagrams(varList, a1, a2, a3);
						delete a3;
					} else
							return;
					
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
							//~ __saveDiagrams(varList, a1, a2, a3);
						delete a3;
					} else
							return;
					
					delete a1;
					delete a2;
					
					for( SequenceIterator< const DiscreteVariable*> ite = varList->begin(); ite != varList->end(); ++ite )
						delete *ite;
					delete varList;
					
					// =====================================================================================
					// Then we try with random structure
					// =====================================================================================
					for(int i = 0; i < 100; i++ ){
						
						Sequence< const DiscreteVariable* >* varList = __generateRandomVarList( i + 1 );						
						__shuffleVarList( varList, i + 2 );
						//~ std::cout << std::endl << " Sequence variable : ";
						//~ for( SequenceIterator< const DiscreteVariable* > iter = varList->begin(); iter != varList->end(); ++iter )
							//~ std::cout << (*iter)->toString() << " - ";
						//~ std::cout << std::endl;
						
						f.clear();
						f.setEpsilon( 7 );
						f.setLowLimit( -42 );
						f.setHighLimit( 69 );
						
						MultiDimDecisionDiagramBase<float>* a1 = NULL;
						TS_GUM_ASSERT_THROWS_NOTHING( a1 = __generateRandomfloatDecisionDiagram(varList, i + 3, &f) );
						//~ std::cout << std::endl << a1->toDot();
						
						__shuffleVarList( varList, i + 4 );						
						//~ std::cout << std::endl << " Sequence variable : ";
						//~ for( SequenceIterator< const DiscreteVariable* > iter = varList->begin(); iter != varList->end(); ++iter )
							//~ std::cout << (*iter)->toString() << " - ";
						//~ std::cout << std::endl;
						
						f.clear();
						f.setEpsilon( 11 );
						f.setLowLimit( -53 );
						f.setHighLimit( 79 );
						
						MultiDimDecisionDiagramBase<float>* a2 = NULL;
						TS_GUM_ASSERT_THROWS_NOTHING( a2 = __generateRandomfloatDecisionDiagram(varList, i + 5, &f) );
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
								//~ __saveDiagrams(varList, a1, a2, a3);
							delete a3;
						} else
							return;
						
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
								//~ __saveDiagrams(varList, a1, a2, a3);
							delete a3;
						} else
							return;
						
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
								//~ __saveDiagrams(varList, a1, a2, a3);
							delete a3;
						} else
							return;
						
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
								//~ __saveDiagrams(varList, a1, a2, a3);
							delete a3;
						} else
							return;
						
						delete a1;
						delete a2;
						
						for( SequenceIterator< const DiscreteVariable*> ite = varList->begin(); ite != varList->end(); ++ite )
							delete *ite;
						delete varList;
					}
				}
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				
				void _Reproducteur(){
					
					//~ LabelizedVariable* v0 = new LabelizedVariable( "0", "", 2 );
					LabelizedVariable* v1 = new LabelizedVariable( "1", "", 3 );
					LabelizedVariable* v2 = new LabelizedVariable( "2", "", 2 );
					LabelizedVariable* v3 = new LabelizedVariable( "3", "", 4 );
					LabelizedVariable* v4 = new LabelizedVariable( "4", "", 2 );
					LabelizedVariable* v5 = new LabelizedVariable( "5", "", 2 );
					//~ LabelizedVariable* v6 = new LabelizedVariable( "6", "", 2 );
					//~ LabelizedVariable* v7 = new LabelizedVariable( "7", "", 2 );
					//~ LabelizedVariable* v8 = new LabelizedVariable( "8", "", 2 );
					//~ LabelizedVariable* v9 = new LabelizedVariable( "9", "", 3 );				
					
										
					
					MultiDimDecisionDiagramFactory<float> facto;
					
					
					Sequence< const DiscreteVariable* > seq;
					seq.insert( v5 );
					seq.insert( v1 );
					seq.insert( v2 );
					seq.insert( v3 );
					seq.insert( v4 );
					//~ seq.insert( v0 );
					//~ seq.insert( v8 );
					//~ seq.insert( v3 );
					//~ seq.insert( v6 );
					//~ seq.insert( v7 );
					//~ seq.insert( v9 );
					//~ std::cout << std::endl << " Sequence variable : ";
					//~ for( SequenceIterator< const DiscreteVariable* > iter = seq.begin(); iter != seq.end(); ++iter )
						//~ std::cout << (*iter)->toString() << " - ";
					//~ std::cout << std::endl;
					
					facto.setVariablesSequence( seq );
					
					NodeId n11 = facto.addNonTerminalNode( v1 );
					NodeId n12 = facto.addNonTerminalNode( v2 );
					NodeId n13 = facto.addNonTerminalNode( v3 );
					NodeId n14 = facto.addNonTerminalNode( v4 );
					
					NodeId n1tm42 = facto.addTerminalNode( -42 );
					NodeId n1t21 = facto.addTerminalNode( 21 );
					NodeId n1t0 = facto.addTerminalNode( 0 );
					
					facto.insertArc( n11, n12, 2 );
					facto.insertDefaultArc( n11, n13 );
					
					facto.insertArc( n12, n13, 0 );
					facto.insertArc( n12, n14, 1 );
					
					facto.insertArc( n13, n1tm42, 0 );
					facto.insertArc( n13, n1t21, 1 );
					facto.insertDefaultArc( n13, n14 );
					
					facto.insertArc( n14, n1t21, 1 );
					facto.insertArc( n14, n1t0, 0 );
					
					MultiDimDecisionDiagramBase<float>* a1 = facto.getMultiDimDecisionDiagram();
					//~ std::cout << std::endl << a1->toDot();
					
					facto.clear();
					seq.clear();
					
					//~ seq.insert( v0 );
					seq.insert( v4 );
					seq.insert( v3 );
					seq.insert( v1 );
					seq.insert( v5 );
					//~ seq.insert( v8 );
					//~ seq.insert( v3 );
					//~ seq.insert( v9 );
					//~ seq.insert( v7 );
					//~ seq.insert( v6 );
					//~ std::cout << std::endl << " Sequence variable : ";
					//~ for( SequenceIterator< const DiscreteVariable* > iter = seq.begin(); iter != seq.end(); ++iter )
						//~ std::cout << (*iter)->toString() << " - ";
					//~ std::cout << std::endl;
					
					facto.setVariablesSequence( seq );
					
					NodeId n24 = facto.addNonTerminalNode( v4 );
					NodeId n23 = facto.addNonTerminalNode( v3 );
					NodeId n21 = facto.addNonTerminalNode( v1 );
					NodeId n25 = facto.addNonTerminalNode( v5 );
					
					NodeId n2t42 = facto.addTerminalNode( 42 );
					NodeId n2tm21 = facto.addTerminalNode( -21 );
					NodeId n2t0 = facto.addTerminalNode( 0 );
					
					facto.insertArc( n24, n23, 0 );
					facto.insertArc( n24, n21, 1 );
					
					facto.insertArc( n23, n2t42, 0 );
					facto.insertArc( n23, n25, 2 );
					facto.insertDefaultArc( n23, n21 );
					
					facto.insertArc( n21, n2t0, 0 );
					facto.insertArc( n21, n25, 1 );
					facto.insertArc( n21, n2tm21, 2 );
					
					facto.insertArc( n25, n2t42, 0 );
					facto.insertArc( n25, n2tm21, 1 );
					
					
					MultiDimDecisionDiagramBase<float>* a2 = facto.getMultiDimDecisionDiagram();
					//~ std::cout << std::endl << a2->toDot() << std::endl;
					
					
					
					MultiDimDecisionDiagramBase<float>* a3 = NULL;
					//Test addition
					TS_GUM_ASSERT_THROWS_NOTHING( a3 = add2MultiDimDecisionDiagrams( a1 , a2 ) );					
					if( a3 != NULL ){
							Instantiation inst(a3);
								GUM_TRACE( a3->toDot() );
							bool haserror = false;
							for( inst.setFirst(); ! inst.end(); ++inst ){
								if( a3->get(inst) != a1->get(inst) + a2->get(inst)){
									//~ std::cout << " Error : " << inst.toString() << " : " << a3->get(inst) << " - " << a1->get(inst) + a2->get(inst) << std::endl;
									haserror = true;
								}
							}
							if( haserror )
								GUM_TRACE( a3->toDot() );
							delete a3;
					}
					
					delete a1;
					delete a2;
					
					//~ delete  v9;
					//~ delete  v2;
					//~ delete  v4;
					//~ delete  v7;
					//~ delete  v3;
				}
				// ================================================================================================
			
		};
	}
}
