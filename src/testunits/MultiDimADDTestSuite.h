/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
// =========================================================================
#include <sstream>
#include <vector>
#include <iostream>
// =========================================================================
#include <cxxtest/AgrumTestSuite.h>
#include "testsuite_utils.h"
// =========================================================================
#include <agrum/multidim/multiDimADD.h>
#include <agrum/multidim/multiDimADDFactory.h>
#include <agrum/multidim/instantiation.h>
// =========================================================================
#include <agrum/multidim/labelizedVariable.h>
// =========================================================================



// The diagrams used for the tests:
//           C -> 0.9
//           |   C -> PL
//           PL
//          /  \  PL -> APU & PL -> ADR
//        C2   U1
//       /  \ /  C2 -> D3 & C2 -> D2 & D2 -> U1
//      D3  D2
//      |    |  D3 -> C3 & D2 -> C4
//     C3    C4
//       \  /  C3 -> C5 & C4 -> C5
//        C5   D4
//         \  /  C5 -> U2 & D4 -> U2
//          U2
namespace gum {

  namespace tests {

    class MultiDimBucketTestSuite: public CxxTest::TestSuite {
		
      private:
		void __fillFactory( MultiDimADDFactory<float>* factory, gum::List<NodeId> idList ) {
			
			idList.insert ( factory->addNonTerminalNode ( *Cprimevar ) );//0
            idList.insert ( factory->addNonTerminalNode ( *Cvar ) ); 	//1
            idList.insert ( factory->addNonTerminalNode ( *PLvar ) ); 	//2
            idList.insert ( factory->addNonTerminalNode ( *APUvar ) ); 	//3
            idList.insert ( factory->addNonTerminalNode ( *BPUvar ) );	//4
            idList.insert ( factory->addNonTerminalNode ( *ADRvar ) ); 	//5
            idList.insert ( factory->addNonTerminalNode ( *BDRvar ) ); 	//6
            idList.insert ( factory->addNonTerminalNode ( *BOvar ) ); 	//7
            
            idList.insert ( factory->addNonTerminalNode ( *Cvar ) ); 	//8
            idList.insert ( factory->addNonTerminalNode ( *PLvar ) ); 	//9
            idList.insert ( factory->addNonTerminalNode ( *APUvar ) ); 	//10
            idList.insert ( factory->addNonTerminalNode ( *BPUvar ) );	//11
            idList.insert ( factory->addNonTerminalNode ( *ADRvar ) ); 	//12
            idList.insert ( factory->addNonTerminalNode ( *BDRvar ) ); 	//13
            idList.insert ( factory->addNonTerminalNode ( *BOvar ) ); 	//14
            
            idList.insert ( factory->addTerminalNode ( 0.0 ) );		//15
            idList.insert ( factory->addTerminalNode ( 1.0 ) ); 	//16
            idList.insert ( factory->addTerminalNode ( 0.9 ) ); 	//17
            idList.insert ( factory->addTerminalNode ( 0.1 ) ); 	//18

            factory->insertArc ( idList[0], idList[1], 1 );
            factory->insertArc ( idList[0], idList[8], 0 );
            
            factory->insertArc ( idList[1], idList[17], 1 );
            factory->insertArc ( idList[1], idList[2], 0 );
            factory->insertArc ( idList[2], idList[3], 1 );
            factory->insertArc ( idList[2], idList[5], 0 );
            factory->insertArc ( idList[3], idList[4], 1 );
            factory->insertArc ( idList[3], idList[5], 0 );
            factory->insertArc ( idList[4], idList[7], 1 );
            factory->insertArc ( idList[4], idList[15], 0 );
            factory->insertArc ( idList[5], idList[6], 1 );
            factory->insertArc ( idList[5], idList[15], 0 );
            factory->insertArc ( idList[6], idList[7], 1 );
            factory->insertArc ( idList[6], idList[15], 0 );
            factory->insertArc ( idList[7], idList[17], 1 );
            factory->insertArc ( idList[7], idList[15], 0 );
            
            factory->insertArc ( idList[8], idList[18], 1 );
            factory->insertArc ( idList[8], idList[9], 0 );
            factory->insertArc ( idList[9], idList[10], 1 );
            factory->insertArc ( idList[9], idList[12], 0 );
            factory->insertArc ( idList[10], idList[11], 1 );
            factory->insertArc ( idList[10], idList[12], 0 );
            factory->insertArc ( idList[11], idList[14], 1 );
            factory->insertArc ( idList[11], idList[16], 0 );
            factory->insertArc ( idList[12], idList[13], 1 );
            factory->insertArc ( idList[12], idList[16], 0 );
            factory->insertArc ( idList[13], idList[14], 1 );
            factory->insertArc ( idList[13], idList[16], 0 );
            factory->insertArc ( idList[14], idList[18], 1 );
            factory->insertArc ( idList[14], idList[16], 0 );
		}

      public:
        LabelizedVariable *Cvar, *PLvar, *APUvar, *BPUvar, *ADRvar, *BDRvar, *BOvar, *Cprimevar;

        void setUp() {
          Cvar = new LabelizedVariable ( "C", "C", 2 );
          PLvar = new LabelizedVariable ( "PL", "PL", 2 );
          APUvar = new LabelizedVariable ( "APU", "APU", 2 );
          BPUvar = new LabelizedVariable ( "BPU", "BPU", 2 );
          ADRvar = new LabelizedVariable ( "ADR", "ADR", 2 );
          BDRvar = new LabelizedVariable ( "BDR", "BDR", 2 );
          BOvar = new LabelizedVariable ( "BO", "BO", 2 );
          Cprimevar = new gum::LabelizedVariable ( "C'", "C'", 2 );
        }

        void tearDown() {
          delete Cvar;
          delete Cprimevar;
          delete PLvar;
          delete APUvar;
          delete BPUvar;
          delete ADRvar;
          delete BDRvar;
          delete BOvar;
        }

        void FactorySimpleCreation() {
			MultiDimADDFactory<float>* factory = NULL;
			
			TS_GUM_ASSERT_THROWS_NOTHING ( factory = new MultiDimADDFactory<float>() );
			
			gum::List<NodeId> idList;
			TS_GUM_ASSERT_THROWS_NOTHING ( __fillFactory ( factory, idList ) );

			TS_GUM_ASSERT_THROWS_NOTHING ( delete factory );
        }

        void FactoryGraphical() {
			MultiDimADDFactory<float>* factory = new MultiDimADDFactory<float>();
			gum::List<NodeId> idList;
			__fillFactory ( factory, idList );
			
			TS_ASSERT_EQUALS( factory->addTerminalNode(1.0), (NodeId) 16 );			
			factory->showProperties();
			
			TS_GUM_ASSERT_THROWS_NOTHING( factory->eraseNode(12) );			
			factory->showProperties();
			
			TS_GUM_ASSERT_THROWS_NOTHING( factory->eraseNode(15) );			
			factory->showProperties();
			
			TS_GUM_ASSERT_THROWS_NOTHING( factory->eraseArc(3,4) );			
			factory->showProperties();
			
			TS_ASSERT_THROWS(factory->eraseNode((NodeId) 15), InvalidNode);
			TS_ASSERT_THROWS(factory->insertArc((NodeId) 15,(NodeId) 1, 2), InvalidNode);
			TS_ASSERT_THROWS(factory->insertArc((NodeId) 16,(NodeId) 1, 3), OperationNotAllowed);
			TS_ASSERT_THROWS(factory->insertArc((NodeId) 8,(NodeId) 18, 4), DuplicateElement);
			TS_ASSERT_THROWS(factory->eraseArc((NodeId) 15,(NodeId) 1), InvalidNode);
			TS_ASSERT_THROWS(factory->eraseArc((NodeId) 16,(NodeId) 1), OperationNotAllowed);
			TS_GUM_ASSERT_THROWS_NOTHING(factory->eraseArc((NodeId) 8,(NodeId) 18));
			TS_ASSERT_THROWS(factory->eraseArc((NodeId) 8,(NodeId) 18), InvalidArc);

			TS_GUM_ASSERT_THROWS_NOTHING ( delete factory );
        }

        void testFactoryGetMultiDimADD() {
			MultiDimADDFactory<float>* factory = new MultiDimADDFactory<float>();
			gum::List<NodeId> idList;
			__fillFactory ( factory, idList );
			
			MultiDimADD<float>* container = NULL;
			TS_GUM_ASSERT_THROWS_NOTHING( container = factory->getMultiDimADD() );

			TS_GUM_ASSERT_THROWS_NOTHING ( delete factory );
			TS_GUM_ASSERT_THROWS_NOTHING( delete container );
        }
        
        

        void testMultiDimADDAccessorsModifiersMethods() {
			
			// *********************************************************************
			// Création du multidim
			// *********************************************************************
			MultiDimADDFactory<float>* factory = new MultiDimADDFactory<float>();
			gum::List<NodeId> idList;
			__fillFactory ( factory, idList );
			
			MultiDimADD<float>* container = NULL;
			TS_GUM_ASSERT_THROWS_NOTHING( container = factory->getMultiDimADD() );

			TS_GUM_ASSERT_THROWS_NOTHING ( delete factory );
			
						
			// *********************************************************************
			// Test variablesSequence(), et si toutes les variables sont
			// accesoirement bien présentent
			// *********************************************************************
			Sequence< const DiscreteVariable* > varSeq;
			TS_GUM_ASSERT_THROWS_NOTHING( varSeq = container->variablesSequence() );
			TS_ASSERT_EQUALS( varSeq.exists( Cprimevar ), true );
			TS_ASSERT_EQUALS( varSeq.exists( Cvar ), true );
			TS_ASSERT_EQUALS( varSeq.exists( PLvar ), true );
			TS_ASSERT_EQUALS( varSeq.exists( APUvar ), true );
			TS_ASSERT_EQUALS( varSeq.exists( BPUvar ), true );
			TS_ASSERT_EQUALS( varSeq.exists( ADRvar ), true );
			TS_ASSERT_EQUALS( varSeq.exists( BDRvar ), true );
			TS_ASSERT_EQUALS( varSeq.exists( BOvar ), true );
			
			
			// *********************************************************************
			// Test name() et basename()
			// *********************************************************************
			TS_ASSERT_EQUALS( container->name(), "ADD" );
			
			TS_ASSERT_EQUALS( container->basename(), "MultiDimImplementation" );
			
			
			// *********************************************************************
			// Test set(), fill() et fillWith()
			// *********************************************************************
			Instantiation inst(*container);
			
			TS_ASSERT_THROWS(container->set(inst, 14.0), OperationNotAllowed);
			
			TS_ASSERT_THROWS(container->fill(14.0), OperationNotAllowed);
			
			std::vector<float> v;
			for( int i = 0; i < 128; i++ )
				v.push_back(i);
			TS_ASSERT_THROWS(container->fillWith(v), SizeError);
			for( int i = 128; i < 256; i++ )
				v.push_back(i);
			TS_ASSERT_THROWS(container->fillWith(v), OperationNotAllowed);
			
			
			// *********************************************************************
			// Test get(), [], et l'accès aux valeurs
			// *********************************************************************
			TS_GUM_ASSERT_THROWS_NOTHING( container->get(inst) );			
			TS_GUM_ASSERT_THROWS_NOTHING( (*container)[inst] );
			
			for( inst.setFirst(); ! inst.end(); ++inst )
				if( inst.val(*Cvar) == 1 ){
					if( inst.val(*Cprimevar) == 1 ) {
						TS_ASSERT_DELTA(container->get(inst), 0.9, 0.001);
					}else{
						TS_ASSERT_DELTA(container->get(inst), 0.1, 0.001);					
					}
				} else {
					
					if( inst.val(*PLvar) == 1 ){
						
						if( inst.val(*APUvar) == 1 ){
							
							if( inst.val(*BPUvar) == 1 ){ 
								
								if( inst.val(*BOvar) == 1 ){
									
									if( inst.val(*Cprimevar) == 1 ) {
										TS_ASSERT_DELTA(container->get(inst), 0.9, 0.001);
									}else{
										TS_ASSERT_DELTA(container->get(inst), 0.1, 0.001);
									}
											
								} else {
									
									if( inst.val(*Cprimevar) == 1 ){
										TS_ASSERT_DELTA(container->get(inst), 0.0, 0.001);
									}else{
										TS_ASSERT_DELTA(container->get(inst), 1.0, 0.001);	
									}
										
								}
							} else {
									
								if( inst.val(*Cprimevar) == 1 ){
									TS_ASSERT_DELTA(container->get(inst), 0.0, 0.001);
								}else{
									TS_ASSERT_DELTA(container->get(inst), 1.0, 0.001);		
								}
									
							}
						} else {
							
							if( inst.val(*ADRvar) == 1 ) {
								
								if( inst.val(*BDRvar) == 1 ) {
									
									if( inst.val(*BOvar) == 1 ) {
									
										if( inst.val(*Cprimevar) == 1 ) {
											TS_ASSERT_DELTA(container->get(inst), 0.9, 0.001);
										}else{
											TS_ASSERT_DELTA(container->get(inst), 0.1, 0.001);
										}
									
									} else {
									
										if( inst.val(*Cprimevar) == 1 ){
											TS_ASSERT_DELTA(container->get(inst), 0.0, 0.001);
										}else{
											TS_ASSERT_DELTA(container->get(inst), 1.0, 0.001);		
										}	
									
									}
									
								} else {
									
									if( inst.val(*Cprimevar) == 1 ){
										TS_ASSERT_DELTA(container->get(inst), 0.0, 0.001);
									}else{
										TS_ASSERT_DELTA(container->get(inst), 1.0, 0.001);	
									}	
									
								}
								
							} else {
									
									if( inst.val(*Cprimevar) == 1 ){
										TS_ASSERT_DELTA(container->get(inst), 0.0, 0.001);
									}else{
										TS_ASSERT_DELTA(container->get(inst), 1.0, 0.001);
									}
								
							}
						}
					} else {
						
						if( inst.val(*ADRvar) == 1 ) {
								
							if( inst.val(*BDRvar) == 1 ) {
								
								if( inst.val(*BOvar) == 1 ) {
									
									if( inst.val(*Cprimevar) == 1 ) {
										TS_ASSERT_DELTA(container->get(inst), 0.9, 0.001);
									}else{
										TS_ASSERT_DELTA(container->get(inst), 0.1, 0.001);
									}
								
								} else {
									
									if( inst.val(*Cprimevar) == 1 ){
										TS_ASSERT_DELTA(container->get(inst), 0.0, 0.001);
									}else{
										TS_ASSERT_DELTA(container->get(inst), 1.0, 0.001);	
									}	
								
								}
								
							} else {
									
								if( inst.val(*Cprimevar) == 1 ){
										TS_ASSERT_DELTA(container->get(inst), 0.0, 0.001);
									}else{
										TS_ASSERT_DELTA(container->get(inst), 1.0, 0.001);	
								}	
								
							}
							
						} else {
									
							if( inst.val(*Cprimevar) == 1 ){
										TS_ASSERT_DELTA(container->get(inst), 0.0, 0.001);
									}else{
										TS_ASSERT_DELTA(container->get(inst), 1.0, 0.001);	
							}	
							
						}
					}
				}
			
			
			// *********************************************************************
			// Test swap()
			// *********************************************************************
			LabelizedVariable* lv = new LabelizedVariable( "Test", "Cornichon", 2 );
			TS_ASSERT_THROWS( container->swap(*Cvar, *lv), OperationNotAllowed );
			delete lv;
				
			TS_GUM_ASSERT_THROWS_NOTHING( delete container );
        }
        
        
        
        void testMultiDimADDImplementationMethods(){
			
			// *********************************************************************
			// Création du multidim
			// *********************************************************************
			MultiDimADDFactory<float>* factory = new MultiDimADDFactory<float>();
			gum::List<NodeId> idList;
			__fillFactory ( factory, idList );
			
			MultiDimADD<float>* container = NULL;
			TS_GUM_ASSERT_THROWS_NOTHING( container = factory->getMultiDimADD() );

			TS_GUM_ASSERT_THROWS_NOTHING ( delete factory );
			
			// *********************************************************************
			// Test add(), erase()
			// *********************************************************************
			LabelizedVariable* lv = new LabelizedVariable( "Test", "Cornichon", 2 );
			
			TS_ASSERT_THROWS(container->add( *lv ) , OperationNotAllowed);
			
			TS_ASSERT_THROWS(container->erase( *Cprimevar), OperationNotAllowed);
			
			// *********************************************************************
			// Test nbrDim(), domainSize(), realSize()
			// *********************************************************************
			
			TS_ASSERT_EQUALS( container->nbrDim( ) , (Idx) 8);
			
			TS_ASSERT_EQUALS( container->domainSize( ), (Size) 256);
			
			TS_ASSERT_EQUALS( container->realSize( ), (Size) 19);
			
			TS_GUM_ASSERT_THROWS_NOTHING ( std::cout << "Taux de compression" << container->compressionRate( ) << std::endl );
			
			TS_GUM_ASSERT_THROWS_NOTHING ( container->variable( 0 ) );
			
			TS_GUM_ASSERT_THROWS_NOTHING ( container->pos( *Cvar ) );
			
			TS_ASSERT_EQUALS( container->contains( *Cprimevar ), true );
			TS_ASSERT_EQUALS( container->contains( *lv ), false);
			
			TS_ASSERT_EQUALS( container->empty( ), false);

			
			delete lv;
			
			TS_GUM_ASSERT_THROWS_NOTHING( delete container );
		}
		
		
		
		void testMultiDimADDCopyMethods(){
			
			// *********************************************************************
			// Création du multidim
			// *********************************************************************
			MultiDimADDFactory<float>* factory = new MultiDimADDFactory<float>();
			gum::List<NodeId> idList;
			__fillFactory ( factory, idList );
			
			MultiDimADD<float>* container = NULL;
			TS_GUM_ASSERT_THROWS_NOTHING( container = factory->getMultiDimADD() );
			
			MultiDimADD<float>* container2 = NULL;
			TS_GUM_ASSERT_THROWS_NOTHING( container2 = factory->getMultiDimADD() );


			TS_GUM_ASSERT_THROWS_NOTHING ( delete factory );
			
			
			TS_ASSERT_THROWS(container->copy(*container2) , OperationNotAllowed);
			
			
			TS_ASSERT_THROWS(container->copyFrom(*container2) , OperationNotAllowed);
			
			
			TS_GUM_ASSERT_THROWS_NOTHING( delete container );			
			TS_GUM_ASSERT_THROWS_NOTHING( delete container2 );
		}
		
		
		
		void testMultiDimADDVariousMethods(){
			
			// *********************************************************************
			// Création du multidim
			// *********************************************************************
			MultiDimADDFactory<float>* factory = new MultiDimADDFactory<float>();
			gum::List<NodeId> idList;
			__fillFactory ( factory, idList );
			
			MultiDimADD<float>* container = NULL;
			TS_GUM_ASSERT_THROWS_NOTHING( container = factory->getMultiDimADD() );
			
			MultiDimADD<float>* container2 = NULL;
			TS_GUM_ASSERT_THROWS_NOTHING( container2 = factory->getMultiDimADD() );
			
			factory->eraseArc(2,5);
			factory->insertArc(2,15,0);
			
			MultiDimADD<float>* container3 = NULL;
			TS_GUM_ASSERT_THROWS_NOTHING( container3 = factory->getMultiDimADD() );
			
			
			// *********************************************************************
			// test tostring() et tostring(Instantiation*)
			// *********************************************************************
			TS_GUM_ASSERT_THROWS_NOTHING( std::cout << std::endl << container->toString() << std::endl );
			
			Instantiation inst(*container);
			inst.setFirst();
			TS_GUM_ASSERT_THROWS_NOTHING( std::cout << std::endl << container->toString(&inst) << std::endl );
			
			
			// *********************************************************************
			// test == et !=
			// *********************************************************************
			TS_ASSERT_EQUALS(*container == *container2, true);
			TS_ASSERT_EQUALS(*container == *container3, false);
			
			TS_ASSERT_EQUALS(*container != *container2, false);
			TS_ASSERT_EQUALS(*container != *container3, true);


			TS_GUM_ASSERT_THROWS_NOTHING ( delete factory );			
			
			TS_GUM_ASSERT_THROWS_NOTHING( delete container );			
			TS_GUM_ASSERT_THROWS_NOTHING( delete container2 );			
			TS_GUM_ASSERT_THROWS_NOTHING( delete container3 );
			
			
		}        
    };
  }
}
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
