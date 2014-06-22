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
// ==============================================================================
#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>
// ==============================================================================
#include <agrum/FMDP/FactoredMarkovDecisionProcess.h>
#include <agrum/FMDP/io/dat/FMDPDatReader.h>
#include <agrum/FMDP/simulation/simulator.h>
#include <agrum/FMDP/learning/spimddi.h>
// ==============================================================================

namespace gum_tests {

  class SPUDDPlanningTestSuite: public CxxTest::TestSuite {

    private :
      std::string file;

      void run ( const std::string showSaveFile ) {

        std::cout << "" << std::endl;

        gum::FactoredMarkovDecisionProcess<double> fmdp(true);
        gum::FMDPDatReader<double> reader ( &fmdp, file );
        TS_GUM_ASSERT_THROWS_NOTHING ( reader.trace ( false ) );
        TS_GUM_ASSERT_THROWS_NOTHING ( reader.proceed( ) );

//        std::cout << fmdp.show() << std::endl;
        gum::Simulator sim(&fmdp);
        gum::Instantiation initialState;

        gum::SPIMDDI<double> spim(true);

        for(auto varIter = fmdp.beginVariables(); varIter != fmdp.endVariables(); ++varIter){
            spim.addVariable(*varIter);
            initialState.add(**varIter);
        }

        for( auto actionIter = fmdp.beginActions(); actionIter != fmdp.endActions(); ++actionIter)
            spim.addAction( *actionIter, fmdp.actionName(*actionIter));

        const gum::MultiDimDecisionGraph<double>* reward = reinterpret_cast<const gum::MultiDimDecisionGraph<double>*>(fmdp.reward());
        for( auto rewardIter = reward->values().beginSafe(); rewardIter != reward->values().endSafe(); ++rewardIter){
            spim.addReward(rewardIter.second());
        }

        initialState.setFirst();
//        TS_GUM_ASSERT_THROWS_NOTHING ( sim.setInitialState(initialState); );
        TS_GUM_ASSERT_THROWS_NOTHING ( spim.initialize(initialState) );

//        std::cout << initialState.toString() << std::endl;
//        for( gum::Idx nbIter = 0; nbIter < 100000; ++nbIter){

//            gum::Idx actionChosenId = spim.askForAction();
//            std::cout << "Action : " << fmdp.actionName(actionChosenId) << std::endl;
//            sim.perform( actionChosenId );
//            std::cout << sim.currentState().toString() << std::endl;

//            spim.feedback(sim.currentState(), sim.reward());
//        }

//        for( gum::Idx i= 0; i < 100; i++){

            for(initialState.setFirst();!initialState.end(); initialState.inc() ){


                TS_GUM_ASSERT_THROWS_NOTHING ( sim.setInitialState(initialState); );
                TS_GUM_ASSERT_THROWS_NOTHING ( spim.setCurrentState(initialState) );

                std::cout << initialState.toString() << std::endl;
//                for( gum::Idx nbIter = 0; nbIter < 100000; ++nbIter){

                    gum::Idx actionChosenId = spim.askForAction();
                    std::cout << "Action : " << fmdp.actionName(actionChosenId) << std::endl;
                    sim.perform( actionChosenId );
                    std::cout << sim.currentState().toString() << std::endl;
                    std::cout << "Obtained Reward : " << sim.reward() << std::endl;
                    spim.feedback(sim.currentState(), sim.reward());
//                }
            }
//        }

      }

    public:

      void test_Coffee() {
        file = GET_PATH_STR ( "FMDP/coffee/coffee.dat" );
        run ( "Coffee" );
      }

  };
}
// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
