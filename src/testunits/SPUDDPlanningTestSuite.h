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
#include <iostream>
#include <string>
// =====================================================================
#include <cxxtest/AgrumTestSuite.h>
#include "testsuite_utils.h"
// =====================================================================
#include <agrum/core/approximationPolicy/approximationPolicy.h>
#include <agrum/core/approximationPolicy/linearApproximationPolicy.h>
// =====================================================================
#include <agrum/multidim/labelizedVariable.h>
#include <agrum/multidim/multiDimDecisionDiagramFactory.h>
// =====================================================================
#include <agrum/FMDP/FactoredMarkovDecisionProcess.h>
#include <agrum/FMDP/io/dat/FMDPDatReader.h>
#include <agrum/FMDP/planning/SPUDDPlanning.h>
// =====================================================================

namespace gum_tests {

    class SPUDDPlanningTestSuite: public CxxTest::TestSuite {
      
      private :
        std::string file;
        
        void run( const std::string showSaveFile, gum::Idx mode ){

          std::cout << std::endl;
          gum::FactoredMarkovDecisionProcess<double> fmdp;
          gum::MultiDimDecisionDiagramFactory<double> ddf;
          gum::SPUDDPlanning<double> inf( &fmdp );
          
          gum::FMDPDatReader<double> reader( &fmdp, file, &ddf );
          reader.trace( false );
          TS_GUM_ASSERT_THROWS_NOTHING( reader.proceed( ) );
          
          gum::MultiDimDecisionDiagramBase<double>* res = NULL;
//           TS_GUM_ASSERT_THROWS_NOTHING( res = inf.makePlanningAlgoEvaluation( showSaveFile, mode ) );
          TS_GUM_ASSERT_THROWS_NOTHING( inf.makePlanning(  ) );

//           for( int imode = 1; imode < 5; imode++ ){
//               for( int j = 0; j < 5; j++ ){
//                   //******************************************************************************************************************************************
//                   gum::FactoredMarkovDecisionProcess<double> fmdpPrime;
//                   gum::MultiDimDecisionDiagramFactory<double> ddfPrime;
//                   gum::SPUDDPlanning<double> infPrime( &fmdpPrime );
// 
//                   gum::FMDPDatReader<double> readerPrime( &fmdpPrime, file, &ddfPrime );
//                   readerPrime.trace( false );
//                   TS_GUM_ASSERT_THROWS_NOTHING( readerPrime.proceed( ) );
//                       
//                   gum::MultiDimDecisionDiagramBase<double>* resPrime = NULL;
//                   TS_GUM_ASSERT_THROWS_NOTHING( resPrime = infPrime.makePlanningAlgoEvaluation( showSaveFile, imode ) );
//         //           TS_GUM_ASSERT_THROWS_NOTHING( infPrime.makePlanning(  ) );     
//                   
//                   gum::Bijection< const DiscreteVariable*, const DiscreteVariable* > reassignement;
//                   for( gum::SequenceIterator< const DiscreteVariable* > varIter = res->variablesSequence().begin(); varIter != res->variablesSequence().end(); ++varIter ){
//                       for( gum::SequenceIterator< const DiscreteVariable* > varPrimeIter = resPrime->variablesSequence().begin(); varPrimeIter != resPrime->variablesSequence().end(); ++varPrimeIter){
//                             if( **varIter == **varPrimeIter ){
//                                 reassignement.insert( *varPrimeIter, *varIter );
//                                 break;
//                             }
//                       }
//                   }
// 
//                   gum::MultiDimDecisionDiagramBase<double>* comparableResPrime = reinterpret_cast<gum::MultiDimDecisionDiagramBase<double>*>( resPrime->newFactory() );
//                   comparableResPrime->copyAndReassign( resPrime, reassignement );
//                   
//                   gum::MultiDimDecisionDiagramBase<double>* comparisonPrime = gum::subtract2MultiDimDecisionDiagrams( res,  comparableResPrime );
//                     double gapPrime = 0;
//                     for( gum::BijectionIterator< NodeId, double > valIter = comparisonPrime->valuesMap().begin(); valIter != comparisonPrime->valuesMap().end(); ++valIter )
//                         if( gapPrime < fabs( valIter.second() ) )
//                             gapPrime = fabs( valIter.second() );
//                   std::cout << " ------------------------------- Resultat : " << gapPrime  << std::endl ;
//                   delete comparisonPrime;
//                   delete comparableResPrime;
//                   delete resPrime;
//                   //*********************************************************************************************************************************************
//               }
//           }
          
          delete res;
        }
        
      public:
        
        void test_Coffee() {
          file = GET_PATH_STR( FMDP/coffee/coffee.dat );
          run("Coffee",1);
        }
        
        void est_FactoryS() {
          file = GET_PATH_STR( FMDP/factory/tiny-factory.dat );
          run("TinyFactory", 1);
        }
        
        
        
        void est_FactoryDirect() {
          file = GET_PATH_STR( FMDP/factory/factory.dat );
          run("Factory.Direct", 1);
        }
        
        void est_FactoryIndirect() {
          file = GET_PATH_STR( FMDP/factory/factory.dat );
          run("Factory.Indirect", 2);
        }
        
        void est_FactoryNb() {
          file = GET_PATH_STR( FMDP/factory/factory.dat );
          run("Factory.Nb", 3);
        }
        
        void est_FactoryTaille() {
          file = GET_PATH_STR( FMDP/factory/factory.dat );
          run("Factory.Taille", 4);
        }
        
        
        
        void est_Factory0Direct() {
          file = GET_PATH_STR( FMDP/factory/factory0.dat );
          run("Factory0.Direct", 1);
        }
        
        void est_Factory0Indirect() {
          file = GET_PATH_STR( FMDP/factory/factory0.dat );
          run("Factory0.Indirect", 2);
        }
        
        void est_Factory0Nb() {
          file = GET_PATH_STR( FMDP/factory/factory0.dat );
          run("Factory0.Nb", 3);
        }
        
        void est_Factory0Taille() {
          file = GET_PATH_STR( FMDP/factory/factory0.dat );
          run("Factory0.Taille", 4);
        }
        
        
        
        void est_Factory1Direct() {
          file = GET_PATH_STR( FMDP/factory/factory1.dat );
          run("Factory1.Direct", 1);
        }
        
        void est_Factory1Indirect() {
          file = GET_PATH_STR( FMDP/factory/factory1.dat );
          run("Factory1.Indirect", 2);
        }
        
        void est_Factory1Nb() {
          file = GET_PATH_STR( FMDP/factory/factory1.dat );
          run("Factory1.Nb", 3);
        }
        
        void est_Factory1Taille() {
          file = GET_PATH_STR( FMDP/factory/factory1.dat );
          run("Factory1.Taille", 4);
        }

    };
  }
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;

