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

// ==============================================================================
#include <iostream>
#include <string>
#include <cstdarg>
// ==============================================================================
#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>
// ==============================================================================
#include <agrum/FMDP/FactoredMarkovDecisionProcess.h>
#include <agrum/FMDP/io/dat/FMDPDatReader.h>
#include <agrum/FMDP/simulation/FMDPSimulator.h>
#include <agrum/FMDP/simulation/taxiSimulator.h>
#include <agrum/FMDP/learning/SDyna/spimddi.h>
#include <agrum/FMDP/learning/SDyna/spiti.h>
// ==============================================================================

namespace gum_tests {

  class SPIMDDITestSuite: public CxxTest::TestSuite {

    private :

      void run ( gum::AbstractSimulator& sim ) {

        // *********************************************************************************************
        // Initialisation de l'instance de SDyna
        // *********************************************************************************************
//        gum::SPIMDDI spim;
        gum::SPITI spim;

        // Enregistrement des actions possibles auprès de SPIMDDI
        for( auto actionIter = sim.beginActions(); actionIter != sim.endActions(); ++actionIter){
          spim.addAction( *actionIter, sim.actionName(*actionIter));
        }

        // Enregistrement des variables caractérisant les états auprès de SPIMDDI
        for(auto varIter = sim.beginVariables(); varIter != sim.endVariables(); ++varIter){
          spim.addVariable(*varIter);
        }

        TS_GUM_ASSERT_THROWS_NOTHING ( spim.initialize() );
        TS_GUM_ASSERT_THROWS_NOTHING ( sim.setInitialStateRandomly() );



        for( gum::Idx nbRun = 0; nbRun < 100; ++nbRun ){

          TS_GUM_ASSERT_THROWS_NOTHING ( spim.setCurrentState(sim.currentState()) );

          while(!sim.hasReachEnd()){

            // Normal Iteration Part
            gum::Idx actionChosenId = 0;
            TS_GUM_ASSERT_THROWS_NOTHING ( actionChosenId = spim.takeAction(); )
            TS_GUM_ASSERT_THROWS_NOTHING ( sim.perform( actionChosenId ) );

            try{
              spim.feedback(sim.currentState(), sim.reward());
            } catch(gum::Exception e){
              std::cout << e.errorType() << std::endl << e.errorCallStack();
              exit(-2);
            }
          }
          TS_GUM_ASSERT_THROWS_NOTHING ( sim.setInitialStateRandomly() );
        }
      }

    public:




      // *******************************************************************************
      // Run the tests on a Coffee FMDP
      // *******************************************************************************
      void est_Coffee(){

        // **************************************************************
        // Chargement du fmdp servant de base
        gum::FMDPSimulator sim(GET_PATH_STR ( "FMDP/coffee/coffee.dat" ));


        // **************************************************************
        // Définition des états finaux
        gum::Instantiation theEnd;
        for(gum::SequenceIteratorSafe<const gum::DiscreteVariable*> varIter = sim.beginVariables(); varIter != sim.endVariables(); ++varIter)
          if( (*varIter)->name().compare("huc")){
            theEnd.add(**varIter);
            theEnd.chgVal(**varIter, (*varIter)->index("yes"));
            break;
          }
        TS_GUM_ASSERT_THROWS_NOTHING (sim.setEndState(theEnd));


        // **************************************************************
        // Lancement
        run ( sim );
      }


      // *******************************************************************************
      // Run the tests on a Taxi instance
      // *******************************************************************************
      void test_Taxi(){

        // **************************************************************
        // Chargement du simulateur
        gum::TaxiSimulator sim;


        // **************************************************************
        // Lancement
        run ( sim );
      }

  };
}
// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
