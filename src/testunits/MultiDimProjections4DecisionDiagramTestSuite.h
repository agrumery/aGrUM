
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
//~ #include <cstdlib>
//~ #include <iostream>
//~ #include <sstream>
#include <fstream>
//~ #include <string>
//~ #include <sstream>
//~ #include <time.h>
// =====================================================================
#include <cxxtest/AgrumTestSuite.h>
#include "testsuite_utils.h"
// =====================================================================
//~ #include <agrum/core/exceptions.h>
//~ #include <agrum/core/approximationPolicy/approximationPolicy.h>
//~ #include <agrum/core/approximationPolicy/linearApproximationPolicy.h>
// =====================================================================
#include <agrum/multidim/potential.h>
#include <agrum/multidim/labelizedVariable.h>
#include <agrum/multidim/multiDimDecisionDiagram.h>
#include <agrum/multidim/multiDimDecisionDiagramFactory.h>
// =====================================================================

using namespace std;

namespace gum {

  namespace tests {

    class MultiDimProjections4DecisionDiagramTestSuite: public CxxTest::TestSuite {

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
				// Génération aléatoire de diagramme de décision
				// ================================================================================================
				MultiDimDecisionDiagramBase<float>* __generateRandomfloatDecisionDiagram( const Sequence< const DiscreteVariable* >* varList, int i, MultiDimDecisionDiagramFactoryBase<float>* f = NULL ) {
					
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



      public :

        // ================================================================================================
        //
        // Test sur les fonctions avec valeurs exactes
        //
        // ================================================================================================
        void test_Operators_Functions_on_MultiDimDecisionDiagrams() {

			// =====================================================================================
			// First we try with a predefine structure
			// =====================================================================================
			Sequence< const DiscreteVariable* >* varList = __generateFixVarList();

			MultiDimDecisionDiagramBase<float>* a1 = NULL;
			TS_GUM_ASSERT_THROWS_NOTHING( a1 = __generateDecisionDiagram1( varList ) );
			//~ GUM_TRACE( a1->toDot() );

			MultiDimDecisionDiagramBase<float>* a3 = NULL;

			Set< const DiscreteVariable* > del_vars;
			del_vars << varList->atPos( 0 );
			del_vars << varList->atPos( 1 );

			//Test addition          
			TS_GUM_ASSERT_THROWS_NOTHING( a3 = projectSumMultiDimDecisionDiagram ( a1, del_vars )  );
			if ( a3 != NULL ) {

				//~ GUM_TRACE( a3->toDot() );

				Instantiation instEleminatedVar( a1 );
				Instantiation instRemainingVar( a3 );

				for ( instRemainingVar.setFirst(); ! instRemainingVar.end(); ++instRemainingVar ) {

					float sum = 0;
					for ( instEleminatedVar.chgValIn( instRemainingVar ); ! instEleminatedVar.end(); instEleminatedVar.incOut( instRemainingVar ) )
						sum += a1->get( instEleminatedVar );

					TS_ASSERT_DELTA( a3->get( instRemainingVar ), sum, 0.01 );
					
				}
				delete a3;
			}

			// Test Multiplication         
			TS_GUM_ASSERT_THROWS_NOTHING( a3 = projectProductMultiDimDecisionDiagram ( a1, del_vars )  );
			if ( a3 != NULL ) {

				//~ GUM_TRACE( a3->toDot() );

				Instantiation instEleminatedVar( a1 );
				Instantiation instRemainingVar( a3 );

				for ( instRemainingVar.setFirst(); ! instRemainingVar.end(); ++instRemainingVar ) {

						float product = 1;
						for ( instEleminatedVar.chgValIn( instRemainingVar ); ! instEleminatedVar.end(); instEleminatedVar.incOut( instRemainingVar ) )
							product *= a1->get( instEleminatedVar );

						TS_ASSERT_DELTA( a3->get( instRemainingVar ), product, 0.01 );
					
				}
				delete a3;
			}

			// Test Minimise
			TS_GUM_ASSERT_THROWS_NOTHING( a3 = projectMinMultiDimDecisionDiagram ( a1, del_vars )  );
			if ( a3 != NULL ) {

				//~ GUM_TRACE( a3->toDot() );

				Instantiation instEleminatedVar( a1 );
				Instantiation instRemainingVar( a3 );

				for ( instRemainingVar.setFirst(); ! instRemainingVar.end(); ++instRemainingVar ) {

					float min = std::numeric_limits<float>::max();
					for ( instEleminatedVar.chgValIn( instRemainingVar ); ! instEleminatedVar.end(); instEleminatedVar.incOut( instRemainingVar ) )
						min = min>=a1->get( instEleminatedVar )?a1->get( instEleminatedVar ):min;

					TS_ASSERT_DELTA( a3->get( instRemainingVar ), min, 0.01 );
					
				}
				delete a3;
			}  

			// Test Maximise
			TS_GUM_ASSERT_THROWS_NOTHING( a3 = projectMaxMultiDimDecisionDiagram ( a1, del_vars )  );
			if ( a3 != NULL ) {

				//~ GUM_TRACE( a3->toDot() );

				Instantiation instEleminatedVar( a1 );
				Instantiation instRemainingVar( a3 );

				for ( instRemainingVar.setFirst(); ! instRemainingVar.end(); ++instRemainingVar ) {

					float max = -1 * ( std::numeric_limits<float>::max() );
					for ( instEleminatedVar.chgValIn( instRemainingVar ); ! instEleminatedVar.end(); instEleminatedVar.incOut( instRemainingVar ) )
						max = max>=a1->get( instEleminatedVar )?max:a1->get( instEleminatedVar );

					TS_ASSERT_DELTA( a3->get( instRemainingVar ), max, 0.01 );
					
				}
				delete a3;
			}
			
			delete a1;

			for ( SequenceIterator< const DiscreteVariable*> ite = varList->begin(); ite != varList->end(); ++ite )
				delete *ite;
			delete varList;

			// =====================================================================================
			// Then we try with random structure
			// =====================================================================================

			for(int i = 0; i < 100; i++ ){
						
				//~ GUM_TRACE( i+1 << "ème itération" << std::endl );

				Sequence< const DiscreteVariable* >* varList = __generateRandomVarList( i + 1 );
				//~ std::stringstream varsseq;
				//~ varsseq << std::endl << "Variables sequence : " << std::endl;
				//~ for( SequenceIterator< const DiscreteVariable* > varsSeqIter = varList->begin(); varsSeqIter != varList->end(); ++varsSeqIter )
					//~ varsseq << (*varsSeqIter)->toString() << " - ";
				//~ GUM_TRACE( varsseq.str() << std::endl );

				MultiDimDecisionDiagramBase<float>* a1 = NULL;
				TS_GUM_ASSERT_THROWS_NOTHING( a1 = __generateRandomfloatDecisionDiagram(varList, i+2) );
				//~ GUM_TRACE( a1->toDot() );

				MultiDimDecisionDiagramBase<float>* a3 = NULL;

				Set< const DiscreteVariable* > del_vars;
				del_vars << varList->atPos( rand()%varList->size() );
				del_vars << varList->atPos( rand()%varList->size() );

				//Test addition          
				TS_GUM_ASSERT_THROWS_NOTHING( a3 = projectSumMultiDimDecisionDiagram ( a1, del_vars )  );
				if ( a3 != NULL ) {

					//~ GUM_TRACE( a3->toDot() );

					Instantiation instEleminatedVar( a1 );
					Instantiation instRemainingVar( a3 );

					for ( instRemainingVar.setFirst(); ! instRemainingVar.end(); ++instRemainingVar ) {

						float sum = 0;
						for ( instEleminatedVar.chgValIn( instRemainingVar ); ! instEleminatedVar.end(); instEleminatedVar.incOut( instRemainingVar ) )
							sum += a1->get( instEleminatedVar );

						TS_ASSERT_DELTA( a3->get( instRemainingVar ), sum, 0.01 );
						if( a3->get( instRemainingVar ) != sum )
							GUM_TRACE( instRemainingVar.toString() );
						//~ 
					}
					delete a3;
				} else
					return;

				// Test Multiplication         
				TS_GUM_ASSERT_THROWS_NOTHING( a3 = projectProductMultiDimDecisionDiagram ( a1, del_vars )  );
				if ( a3 != NULL ) {

					//~ GUM_TRACE( a3->toDot() );

					Instantiation instEleminatedVar( a1 );
					Instantiation instRemainingVar( a3 );

					for ( instRemainingVar.setFirst(); ! instRemainingVar.end(); ++instRemainingVar ) {

						float product = 1;
						float deltaProduct = 0.01;
						for ( instEleminatedVar.chgValIn( instRemainingVar ); ! instEleminatedVar.end(); instEleminatedVar.incOut( instRemainingVar ) ){
							product *= a1->get( instEleminatedVar );
							deltaProduct *= 100;
						}

						TS_ASSERT_DELTA( a3->get( instRemainingVar ), product, deltaProduct );
						if( ( a3->get( instRemainingVar ) > product + deltaProduct ) || ( a3->get( instRemainingVar ) < product - deltaProduct ) )
							GUM_TRACE( "Delta Product = " << deltaProduct );
						
					}
					delete a3;
				} else
					return;

				// Test Minimise
				TS_GUM_ASSERT_THROWS_NOTHING( a3 = projectMinMultiDimDecisionDiagram ( a1, del_vars )  );
				if ( a3 != NULL ) {

					//~ GUM_TRACE( a3->toDot() );

					Instantiation instEleminatedVar( a1 );
					Instantiation instRemainingVar( a3 );

					for ( instRemainingVar.setFirst(); ! instRemainingVar.end(); ++instRemainingVar ) {

						float min = std::numeric_limits<float>::max();
						for ( instEleminatedVar.chgValIn( instRemainingVar ); ! instEleminatedVar.end(); instEleminatedVar.incOut( instRemainingVar ) )
							min = min>=a1->get( instEleminatedVar )?a1->get( instEleminatedVar ):min;

						TS_ASSERT_DELTA( a3->get( instRemainingVar ), min, 0.01 );
						
					}
					delete a3;
				}   else
					return;

				// Test Maximise
				TS_GUM_ASSERT_THROWS_NOTHING( a3 = projectMaxMultiDimDecisionDiagram ( a1, del_vars )  );
				if ( a3 != NULL ) {

					//~ GUM_TRACE( a3->toDot() );

					Instantiation instEleminatedVar( a1 );
					Instantiation instRemainingVar( a3 );

					for ( instRemainingVar.setFirst(); ! instRemainingVar.end(); ++instRemainingVar ) {

						float max = -1 * ( std::numeric_limits<float>::max() );
						for ( instEleminatedVar.chgValIn( instRemainingVar ); ! instEleminatedVar.end(); instEleminatedVar.incOut( instRemainingVar ) )
							max = max>=a1->get( instEleminatedVar )?max:a1->get( instEleminatedVar );

						TS_ASSERT_DELTA( a3->get( instRemainingVar ), max, 0.01 );
						
					}
					delete a3;
				} else
					return;
				
				delete a1;
				
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
        void test_Operators_on_MultiDimDecisionDiagrams() {
          //~ operators4MultiDimInit<float> ();

          // =====================================================================================
          // First we try with a predefine structure
          // =====================================================================================

          //~ MultiDimDecisionDiagram<float> a1, a2;

          //Test addition
          //~ TS_GUM_ASSERT_THROWS_NOTHING( a1 + a2 );

          //Test subtraction
          //~ TS_GUM_ASSERT_THROWS_NOTHING( a1 - a2 );

          //Test multiplication
          //~ TS_GUM_ASSERT_THROWS_NOTHING( a1 * a2 );

          //Test division
          //~ TS_GUM_ASSERT_THROWS_NOTHING( a1 / a2 );
        }

        // ================================================================================================
        //
        // Test sur les fonctions avec approximation
        //
        // ================================================================================================
        void test_Operation_in_MultiDimDecisionDiagrams_With_LinearApproximation() {

            // =====================================================================================
            // First we try with a predefine structure
            // =====================================================================================
            //~ Sequence< const DiscreteVariable* >* varList = generateFixVarList();


            //~ MultiDimDecisionDiagramFactory<float,LinearApproximationPolicy> f;
            //~ f.setEpsilon( 0.9 );
            //~ f.setLowLimit( 0 );
            //~ f.setHighLimit( 4.5 );
            //~ f.setVariablesSequence( *varList );
            //~ NodeId a = f.addNonTerminalNode( *( varList->atPos( 0 ) ) );
            //~ NodeId b = f.addNonTerminalNode( *( varList->atPos( 1 ) ) );
            //~ NodeId c = f.addNonTerminalNode( *( varList->atPos( 2 ) ) );

            //~ NodeId d = f.addTerminalNode( 1 );
            //~ NodeId e = f.addTerminalNode( 2 );
            //~ NodeId g = f.addTerminalNode( 3 );

            //~ f.insertArc( a, b, 0 );
            //~ f.insertArc( a, c, 1 );

            //~ f.insertArc( b, d, 0 );
            //~ f.insertArc( b, c, 1 );

            //~ f.insertArc( c, e, 0 );
            //~ f.insertArc( c, g, 1 );

            //~ MultiDimDecisionDiagramBase<float>* a1 = f.getMultiDimDecisionDiagram( false );


            //~ f.clear();
            //~ f.setEpsilon( 1.3 );
            //~ f.setLowLimit( 3.5 );
            //~ f.setHighLimit( 9 );

            //~ f.setVariablesSequence( *varList );
            //~ a = f.addNonTerminalNode( *( varList->atPos( 0 ) ) );
            //~ c = f.addNonTerminalNode( *( varList->atPos( 2 ) ) );

            //~ d = f.addTerminalNode( 4 );
            //~ e = f.addTerminalNode( 6 );
            //~ g = f.addTerminalNode( 7 );

            //~ f.insertArc( a, d, 0 );
            //~ f.insertArc( a, c, 1 );

            //~ f.insertArc( c, e, 0 );
            //~ f.insertArc( c, g, 1 );

            //~ MultiDimDecisionDiagramBase<float>* a2 = f.getMultiDimDecisionDiagram( false );


            //~ MultiDimDecisionDiagramBase<float>* a3 = NULL;
            //Test addition
            //~ TS_GUM_ASSERT_THROWS_NOTHING( a3 = add2MultiDimDecisionDiagrams( a1 , a2 ) );
            //~ if ( a3 != NULL ) {
              //~ Instantiation inst( a3 );
              //~ bool haserror = false;
              //~ for ( inst.setFirst(); ! inst.end(); ++inst ) {
                //~ TS_ASSERT_DELTA( a3->get( inst ), a1->get( inst ) + a2->get( inst ), 0.9 );
                //~ if ( a3->get( inst ) != a1->get( inst ) + a2->get( inst ) )
                  //~ haserror = true;
              //~ }
              //~ if ( haserror )
                //~ saveDiagrams( varList, a1, a2, a3 );
              //~ delete a3;
            //~ }

            // Test Substraction
            //~ TS_GUM_ASSERT_THROWS_NOTHING( a3 = subtract2MultiDimDecisionDiagrams( a1 , a2 ) );
            //~ if ( a3 != NULL ) {
              //~ Instantiation inst( a3 );
              //~ bool haserror = false;
              //~ for ( inst.setFirst(); ! inst.end(); ++inst ) {
                //~ TS_ASSERT_DELTA( a3->get( inst ), a1->get( inst ) - a2->get( inst ), 0.9 );
                //~ if ( a3->get( inst ) != a1->get( inst ) - a2->get( inst ) )
                  //~ haserror = true;
              //~ }
              //~ if ( haserror )
                //~ saveDiagrams( varList, a1, a2, a3 );
              //~ delete a3;
            //~ }

            // Test Multiplication
            //~ TS_GUM_ASSERT_THROWS_NOTHING( a3 = multiply2MultiDimDecisionDiagrams( a1 , a2 ) );
            //~ if ( a3 != NULL ) {
              //~ Instantiation inst( a3 );
              //~ bool haserror = false;
              //~ for ( inst.setFirst(); ! inst.end(); ++inst ) {
                //~ TS_ASSERT_DELTA( a3->get( inst ), a1->get( inst ) * a2->get( inst ), 0.9 );
                //~ if ( a3->get( inst ) != a1->get( inst ) * a2->get( inst ) )
                  //~ haserror = true;
              //~ }
              //~ if ( haserror )
                //~ saveDiagrams( varList, a1, a2, a3 );
              //~ delete a3;
            //~ }

            // Test Division
            //~ TS_GUM_ASSERT_THROWS_NOTHING( a3 = divide2MultiDimDecisionDiagrams( a1 , a2 ) );
            //~ if ( a3 != NULL ) {
              //~ Instantiation inst( a3 );
              //~ bool haserror = false;
              //~ for ( inst.setFirst(); ! inst.end(); ++inst ) {
                //~ TS_ASSERT_DELTA( a3->get( inst ), a1->get( inst ) / a2->get( inst ), 0.9 );
                //~ if ( a3->get( inst ) != a1->get( inst ) / a2->get( inst ) )
                  //~ haserror = true;
              //~ }
              //~ if ( haserror )
                //~ saveDiagrams( varList, a1, a2, a3 );
              //~ delete a3;
            //~ }

            // Test Maximum
            //~ TS_GUM_ASSERT_THROWS_NOTHING( a3 = maximize2MultiDimDecisionDiagrams( a1 , a2 ) );
            //~ if ( a3 != NULL ) {
              //~ Instantiation inst( a3 );
              //~ bool haserror = false;
              //~ for ( inst.setFirst(); ! inst.end(); ++inst ) {
                //~ float v1 = a1->get( inst ), v2 = a2->get( inst );
                //~ float max = ( v1>=v2?v1:v2 );
                //~ TS_ASSERT_DELTA( a3->get( inst ), max, 0.9 );
                //~ if ( a3->get( inst ) != ( max ) )
                  //~ haserror = true;
              //~ }
              //~ if ( haserror )
                //~ saveDiagrams( varList, a1, a2, a3 );
              //~ delete a3;
            //~ }

            //~ delete a1;
            //~ delete a2;

            //~ for ( SequenceIterator< const DiscreteVariable*> ite = varList->begin(); ite != varList->end(); ++ite )
              //~ delete *ite;
            //~ delete varList;

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
        
        
		void est_reproduction(){
			
			Sequence< const DiscreteVariable* >* varList = new Sequence< const DiscreteVariable* >();
			
			varList->insert( new LabelizedVariable( "3", "", 3 ) );
			varList->insert( new LabelizedVariable( "6", "", 3 ) );
			varList->insert( new LabelizedVariable( "8", "", 2 ) );
			varList->insert( new LabelizedVariable( "9", "", 3 ) );
			
			MultiDimDecisionDiagramFactory<float> facto;
			facto.setVariablesSequence(  *varList );
			NodeId nv3 = facto.addNonTerminalNode( varList->atPos( 0 ) );
			NodeId nv6 = facto.addNonTerminalNode( varList->atPos( 1 ) );
			NodeId nv8 = facto.addNonTerminalNode( varList->atPos( 2 ) );
			NodeId nv9 = facto.addNonTerminalNode( varList->atPos( 3 ) );
			
			NodeId nt0 = facto.addTerminalNode( 0 );
			NodeId nt71 = facto.addTerminalNode( 71 );
			NodeId nt63 = facto.addTerminalNode( 63 );
			NodeId ntm77 = facto.addTerminalNode( -77 );
			
			facto.insertArc( nv3, nv6, 0 );
			facto.insertArc( nv3, ntm77, 1 );
			facto.insertArc( nv3, nv8, 2 );
			
			facto.insertArc( nv6, nt71, 0 );
			facto.insertDefaultArc( nv6, nt0 );
			
			facto.insertArc( nv8, nv9, 0 );
			facto.insertDefaultArc( nv8, nt0 );
			
			facto.insertArc( nv9, ntm77, 0 );
			facto.insertArc( nv9, nt63, 1 );
			facto.insertArc( nv9, nt71, 2 );
			
			std::stringstream varsseq;
			varsseq << std::endl << "Variables sequence : " << std::endl;
			for( SequenceIterator< const DiscreteVariable* > varsSeqIter = varList->begin(); varsSeqIter != varList->end(); ++varsSeqIter )
				varsseq << (*varsSeqIter)->toString() << " - ";
			GUM_TRACE( varsseq.str() << std::endl );
			sleep(1);

			MultiDimDecisionDiagramBase<float>* a1 = NULL;
			TS_GUM_ASSERT_THROWS_NOTHING( a1 = facto.getMultiDimDecisionDiagram() );
			GUM_TRACE( a1->toDot() );

			MultiDimDecisionDiagramBase<float>* a3 = NULL;

			Set< const DiscreteVariable* > del_vars;
			del_vars << varList->atPos( 0 );         
			TS_GUM_ASSERT_THROWS_NOTHING( a3 = projectProductMultiDimDecisionDiagram ( a1, del_vars )  );
			del_vars.clear();
			del_vars << varList->atPos( 1 );

			//Test addition          
			TS_GUM_ASSERT_THROWS_NOTHING( a3 = projectProductMultiDimDecisionDiagram ( a1, del_vars )  );
			if ( a3 != NULL ) {

				GUM_TRACE( a3->toDot() );

				Instantiation instEleminatedVar( a1 );
				Instantiation instRemainingVar( a3 );

				for ( instRemainingVar.setFirst(); ! instRemainingVar.end(); ++instRemainingVar ) {

					//~ float sum = 0;
					//~ for ( instEleminatedVar.chgValIn( instRemainingVar ); ! instEleminatedVar.end(); instEleminatedVar.incOut( instRemainingVar ) )
						//~ sum += a1->get( instEleminatedVar );
//~ 
					//~ TS_ASSERT_DELTA( a3->get( instRemainingVar ), sum, 0.01 );
					//~ if( a3->get( instRemainingVar ) != sum )
						//~ GUM_TRACE( instRemainingVar.toString() );

						float product = 1;
						float deltaProduct = 0.01;
						for ( instEleminatedVar.chgValIn( instRemainingVar ); ! instEleminatedVar.end(); instEleminatedVar.incOut( instRemainingVar ) ){
							product *= a1->get( instEleminatedVar );
							deltaProduct *= 100;
						}

						TS_ASSERT_DELTA( a3->get( instRemainingVar ), product, deltaProduct );
						if( ( a3->get( instRemainingVar ) > product + deltaProduct ) || ( a3->get( instRemainingVar ) < product - deltaProduct ) )
							GUM_TRACE( "Delta Product = " << deltaProduct );
					
				}
				delete a3;
			}
		}

    };
  }
}
