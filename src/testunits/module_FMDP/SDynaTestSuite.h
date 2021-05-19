/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


// ==============================================================================
#include <cstdarg>
#include <iostream>
#include <string>
// ==============================================================================
#include <gumtest/AgrumTestSuite.h>
#include <testsuite_utils.h>
// ==============================================================================
#include <agrum/FMDP/SDyna/sdyna.h>
#include <agrum/FMDP/fmdp.h>
#include <agrum/FMDP/io/dat/fmdpDatReader.h>
#include <agrum/FMDP/simulation/fmdpSimulator.h>
#include <agrum/FMDP/simulation/taxiSimulator.h>
// ==============================================================================

namespace gum_tests {

  class SDynaTestSuite: public CxxTest::TestSuite {
    private:
    void run(gum::AbstractSimulator& sim) {
      // *********************************************************************************************
      // Initialisation de l'instance de SDyna
      // *********************************************************************************************
      gum::SDYNA* sdyna = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(sdyna = gum::SDYNA::spimddiInstance());

      // Enregistrement des actions possibles auprès de SDyna
      for (auto actionIter = sim.beginActions(); actionIter != sim.endActions();
           ++actionIter) {
        sdyna->addAction(*actionIter, sim.actionName(*actionIter));
      }
      // Enregistrement des variables caractérisant les états auprès de SDyna
      for (auto varIter = sim.beginVariables(); varIter != sim.endVariables();
           ++varIter) {
        sdyna->addVariable(*varIter);
      }
      TS_GUM_ASSERT_THROWS_NOTHING(sim.setInitialStateRandomly());
      TS_GUM_ASSERT_THROWS_NOTHING(sdyna->initialize(sim.currentState()));

      gum::Idx nbObs = 0;
      for (gum::Idx nbRun = 0; nbRun < 10; ++nbRun) {
        sim.setInitialStateRandomly();
        TS_GUM_ASSERT_THROWS_NOTHING(sdyna->setCurrentState(sim.currentState()));
        gum::Idx nbDec = 0;
        while (!sim.hasReachEnd() && nbDec < 25) {
          nbObs++;

          // Normal Iteration Part
          gum::Idx actionChosenId = 0;
          TS_GUM_ASSERT_THROWS_NOTHING(actionChosenId = sdyna->takeAction(););
          TS_GUM_ASSERT_THROWS_NOTHING(sim.perform(actionChosenId));
          nbDec++;

          TS_GUM_ASSERT_THROWS_NOTHING(
             sdyna->feedback(sim.currentState(), sim.reward()););
        }
        TS_GUM_ASSERT_THROWS_NOTHING(sim.setInitialStateRandomly());
      }
      TS_GUM_ASSERT_THROWS_NOTHING(delete sdyna);
    }

    public:
    // *******************************************************************************
    // Run the tests on a Coffee FMDP
    // *******************************************************************************
    void test_Coffee() {
      // **************************************************************
      // Chargement du fmdp servant de base
      gum::FMDPSimulator sim(GET_RESSOURCES_PATH("FMDP/coffee/coffee.dat"));

      // **************************************************************
      // Définition of final states
      gum::Instantiation theEnd;
      for (gum::SequenceIteratorSafe< const gum::DiscreteVariable* > varIter
           = sim.beginVariables();
           varIter != sim.endVariables();
           ++varIter) {
        if ((*varIter)->name().compare("huc")) {
          theEnd.add(**varIter);
          theEnd.chgVal(**varIter, (*varIter)->index("yes"));
          break;
        }
      }
      TS_GUM_ASSERT_THROWS_NOTHING(sim.setEndState(theEnd));

      // **************************************************************
      // Lancement
      run(sim);
    }


    // *******************************************************************************
    // Run the tests on a Taxi instance
    // *******************************************************************************
    void test_Taxi() {
      // **************************************************************
      // Chargement du simulateur
      gum::TaxiSimulator sim;


      // **************************************************************
      // Lancement
      run(sim);
    }
  };
}   // namespace gum_tests
