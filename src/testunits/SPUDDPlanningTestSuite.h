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

namespace gum {

  namespace tests {

    class SPUDDPlanningTestSuite: public CxxTest::TestSuite {
      
      private :
        std::string file;
        
        void run( const std::string showSaveFile, Idx mode ){

          std::cout << std::endl;
          gum::FactoredMarkovDecisionProcess<double> fmdp;
          gum::MultiDimDecisionDiagramFactory<double> ddf;
          SPUDDPlanning<double> inf( &fmdp );
          
          gum::FMDPDatReader<double> reader( &fmdp, file, &ddf );
          reader.trace( false );
          TS_GUM_ASSERT_THROWS_NOTHING( reader.proceed( ) );
          
              std::string dotfile = GET_PATH_STR ( ../../../res/test.dot );
              std::ofstream output ( dotfile.c_str(), std::ios::out );

              if ( ! output.good() )
                  GUM_ERROR ( IOError, "Stream states flags are not all unset." );

              output << std::endl;
              
              output << fmdp.show();

              output.flush();

              output.close();

              if ( output.fail() )
                  GUM_ERROR ( IOError, "Writting in the ostream failed." );
          
//           MultiDimDecisionDiagramBase<double>* res = NULL;
//           TS_GUM_ASSERT_THROWS_NOTHING( res = inf.makePlanningAlgoEvaluation( showSaveFile, mode ) );
//           TS_GUM_ASSERT_THROWS_NOTHING( inf.makePlanning(  ) );

//           for( int imode = 1; imode < 5; imode++ ){
//               for( int j = 0; j < 5; j++ ){
//                   //******************************************************************************************************************************************
//                   gum::FactoredMarkovDecisionProcess<double> fmdpPrime;
//                   gum::MultiDimDecisionDiagramFactory<double> ddfPrime;
//                   SPUDDPlanning<double> infPrime( &fmdpPrime );
// 
//                   gum::FMDPDatReader<double> readerPrime( &fmdpPrime, file, &ddfPrime );
//                   readerPrime.trace( false );
//                   TS_GUM_ASSERT_THROWS_NOTHING( readerPrime.proceed( ) );
//                       
//                   MultiDimDecisionDiagramBase<double>* resPrime = NULL;
//                   TS_GUM_ASSERT_THROWS_NOTHING( resPrime = infPrime.makePlanningAlgoEvaluation( showSaveFile, imode ) );
//         //           TS_GUM_ASSERT_THROWS_NOTHING( infPrime.makePlanning(  ) );     
//                   
//                   Bijection< const DiscreteVariable*, const DiscreteVariable* > reassignement;
//                   for( SequenceIterator< const DiscreteVariable* > varIter = res->variablesSequence().begin(); varIter != res->variablesSequence().end(); ++varIter ){
//                       for( SequenceIterator< const DiscreteVariable* > varPrimeIter = resPrime->variablesSequence().begin(); varPrimeIter != resPrime->variablesSequence().end(); ++varPrimeIter){
//                             if( **varIter == **varPrimeIter ){
//                                 reassignement.insert( *varPrimeIter, *varIter );
//                                 break;
//                             }
//                       }
//                   }
// 
//                   MultiDimDecisionDiagramBase<double>* comparableResPrime = reinterpret_cast<MultiDimDecisionDiagramBase<double>*>( resPrime->newFactory() );
//                   comparableResPrime->copyAndReassign( resPrime, reassignement );
//                   
//                   MultiDimDecisionDiagramBase<double>* comparisonPrime = subtract2MultiDimDecisionDiagrams( res,  comparableResPrime );
//                     double gapPrime = 0;
//                     for( BijectionIterator< NodeId, double > valIter = comparisonPrime->valuesMap().begin(); valIter != comparisonPrime->valuesMap().end(); ++valIter )
//                         if( gapPrime < fabs( valIter.second() ) )
//                             gapPrime = fabs( valIter.second() );
//                   std::cout << " ------------------------------- Resultat : " << gapPrime  << std::endl ;
//                   delete comparisonPrime;
//                   delete comparableResPrime;
//                   delete resPrime;
//                   //*********************************************************************************************************************************************
//               }
//           }
          
//           delete res;
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
}
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;

