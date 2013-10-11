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


#include <iostream>
#include <string>

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/core/approximationPolicy/approximationPolicy.h>
#include <agrum/core/approximationPolicy/linearApproximationPolicy.h>
#include <agrum/core/timer.h>

#include <agrum/variables/labelizedVariable.h>
#include <agrum/multidim/multiDimDecisionDiagramFactory.h>

#include <agrum/FMDP/FactoredMarkovDecisionProcess.h>
#include <agrum/FMDP/io/dat/FMDPDatReader.h>
#include <agrum/FMDP/planning/SPUDDPlanning.h>


namespace gum_tests {

  class SPUDDPlanningTestSuite: public CxxTest::TestSuite {

    private :
      std::string file;

      void run ( const std::string showSaveFile, gum::Idx mode ) {

        gum::FactoredMarkovDecisionProcess<double> fmdp;
        gum::MultiDimDecisionDiagramFactory<double> ddf;
        gum::SPUDDPlanning<double> inf ( &fmdp );

        gum::FMDPDatReader<double> reader ( &fmdp, file, &ddf );
        reader.trace ( false );
        TS_GUM_ASSERT_THROWS_NOTHING ( reader.proceed( ) );

        std::ofstream __traceAlgoSaveFile;
        __traceAlgoSaveFile.open ( GET_PATH_STR ( "test.dot" ), std::ios::out | std::ios::trunc );

        if ( !__traceAlgoSaveFile ) {
          std::cerr << "error opening file!" << std::endl;
          return;
        }

        __traceAlgoSaveFile << fmdp.show();
        __traceAlgoSaveFile.close();

        gum::MultiDimDecisionDiagramBase<double>* res = nullptr;
//           TS_GUM_ASSERT_THROWS_NOTHING( res = inf.makePlanningAlgoEvaluation( showSaveFile, mode ) );
        TS_GUM_ASSERT_THROWS_NOTHING ( res = inf.makePlanning( ) );
//           std::cout << res->toDot();
//           gum::Timer t;
//           double tempsTotal = 0;
//           gum::Idx nbNodeInterne = 0;
//           gum::Idx nbLeaves = 0;
//           for( int  i = 1; i <= 30; i++ ){
//               try{
//                   t.reset();
//                   gum::MultiDimDecisionDiagramBase< double >* res = inf.makePlanning();
//                   tempsTotal += t.step();
//                   nbNodeInterne += ( res->realSize() - res->valuesMap().size() );
//                   nbLeaves += res->valuesMap().size();
//                   delete res;
//
//                   std::cout << " ------------------- Fin itération n° " << i  << " Temps : " << tempsTotal/(double) i << " - Nb Intern Nodes : " << (double) nbNodeInterne/(double) i << " - Nb Leaves : " << (double) nbLeaves/(double) i  << std::endl;
//
//               } catch ( gum::Exception e ){
//                 std::cout << e.callStack();
//                 std::cout << std::endl << e.content();
//               }
//           }
//           std::cout << " Temps moyen : " << tempsTotal/30 << " - Nb Intern Nodes : " << (double) nbNodeInterne/30 << " - Nb Leaves : " << (double) nbLeaves/30;
//
        delete res;
      }

    public:
      void test_Coffee() {
        file = GET_PATH_STR ( "FMDP/coffee/coffee.dat" );
        run ( "Coffee", 1 );
      }

      void est_FactoryS() {
        file = GET_PATH_STR ( "FMDP/factory/tiny-factory.dat" );
        run ( "TinyFactory", 1 );
      }

      void est_Elev2() {
        file = GET_PATH_STR ( "FMDP/elev/elev2.dat" );
        run ( "Elev2", 1 );
      }

      void est_Maze() {
        file = GET_PATH_STR ( "FMDP/labyrinth/maze.dat" );
        run ( "maze", 1 );
      }


      void est_MazeB() {
        file = GET_PATH_STR ( "FMDP/labyrinth/mazeb.dat" );
        run ( "mazeb", 1 );
      }


      void est_Factory() {
        file = GET_PATH_STR ( "FMDP/factory/factory.dat" );
        run ( "Factory", 1 );
      }

      void est_FactoryB() {
        file = GET_PATH_STR ( "FMDP/factory/factoryB.dat" );
        run ( "FactoryB", 1 );
      }


      void est_Factory0() {
        file = GET_PATH_STR ( "FMDP/factory/factory0.dat" );
        run ( "Factory0", 1 );
      }

      void est_Factory0B() {
        file = GET_PATH_STR ( "FMDP/factory/factory0B.dat" );
        run ( "Factory0B", 1 );
      }


      void est_Factory1() {
        file = GET_PATH_STR ( "FMDP/factory/factory1.dat" );
        run ( "Factory1", 1 );
      }

      void est_Factory1B() {
        file = GET_PATH_STR ( "FMDP/factory/factory1B.dat" );
        run ( "Factory1B", 1 );
      }


      void est_Factory2() {
        file = GET_PATH_STR ( "FMDP/factory/factory2.dat" );
        run ( "Factory2", 1 );
      }

      void est_Factory2B() {
        file = GET_PATH_STR ( "FMDP/factory/factory2B.dat" );
        run ( "Factory2B", 1 );
      }


      void est_Factory3() {
        file = GET_PATH_STR ( "FMDP/factory/factory3.dat" );
        run ( "Factory3", 1 );
      }

      void est_Factory3B() {
        file = GET_PATH_STR ( "FMDP/factory/factory3B.dat" );
        run ( "Factory3B", 1 );
      }

  };
}
// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;








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
//                   gum::MultiDimDecisionDiagramBase<double>* resPrime = nullptr;
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

