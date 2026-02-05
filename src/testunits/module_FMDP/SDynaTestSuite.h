/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#pragma once


// ==============================================================================
#include <cstdarg>
#include <iostream>
#include <string>
// ==============================================================================
#include <utils.h>

#include <gumtest/AgrumTestSuite.h>
// ==============================================================================
#include <agrum/FMDP/fmdp.h>
#include <agrum/FMDP/io/dat/fmdpDatReader.h>
#include <agrum/FMDP/SDyna/sdyna.h>
#include <agrum/FMDP/simulation/fmdpSimulator.h>
#include <agrum/FMDP/simulation/taxiSimulator.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  SDyna
#define GUM_CURRENT_MODULE FMDP

// ==============================================================================

namespace gum_tests {

  struct SDynaTestSuite {
    private:
    static void run(gum::AbstractSimulator& sim) {
      // *********************************************************************************************
      // Initialisation de l'instance de SDyna
      // *********************************************************************************************
      gum::SDYNA* sdyna = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(sdyna = gum::SDYNA::spimddiInstance());

      // Enregistrement des actions possibles auprès de SDyna
      for (auto actionIter = sim.beginActions(); actionIter != sim.endActions(); ++actionIter) {
        sdyna->addAction(*actionIter, sim.actionName(*actionIter));
      }
      // Enregistrement des variables caractérisant les états auprès de SDyna
      for (auto varIter = sim.beginVariables(); varIter != sim.endVariables(); ++varIter) {
        sdyna->addVariable(*varIter);
      }
      GUM_CHECK_ASSERT_THROWS_NOTHING(sim.setInitialStateRandomly());
      GUM_CHECK_ASSERT_THROWS_NOTHING(sdyna->initialize(sim.currentState()));

      for (gum::Idx nbRun = 0; nbRun < 10; ++nbRun) {
        sim.setInitialStateRandomly();
        GUM_CHECK_ASSERT_THROWS_NOTHING(sdyna->setCurrentState(sim.currentState()));
        gum::Idx nbDec = 0;
        while (!sim.hasReachEnd() && nbDec < 25) {
          // Normal Iteration Part
          gum::Idx actionChosenId = 0;
          GUM_CHECK_ASSERT_THROWS_NOTHING(actionChosenId = sdyna->takeAction(););
          GUM_CHECK_ASSERT_THROWS_NOTHING(sim.perform(actionChosenId));
          nbDec++;

          GUM_CHECK_ASSERT_THROWS_NOTHING(sdyna->feedback(sim.currentState(), sim.reward()););
        }
        GUM_CHECK_ASSERT_THROWS_NOTHING(sim.setInitialStateRandomly());
      }
      GUM_CHECK_ASSERT_THROWS_NOTHING(delete sdyna);
    }   // namespace gum_tests

    public:
    // *******************************************************************************
    // Run the tests on a Coffee FMDP
    // *******************************************************************************
    static void test_Coffee() {
      // **************************************************************
      // Chargement du fmdp servant de base
      gum::FMDPSimulator sim(GET_RESSOURCES_PATH("FMDP/coffee/coffee.dat"));

      // **************************************************************
      // Définition of final states
      gum::Instantiation theEnd;
      for (gum::SequenceIteratorSafe< const gum::DiscreteVariable* > varIter = sim.beginVariables();
           varIter != sim.endVariables();
           ++varIter) {
        if ((*varIter)->name().compare("huc")) {
          theEnd.add(**varIter);
          theEnd.chgVal(**varIter, (*varIter)->index("yes"));
          break;
        }
      }
      GUM_CHECK_ASSERT_THROWS_NOTHING(sim.setEndState(theEnd));

      // **************************************************************
      // Lancement
      run(sim);
    }

    // *******************************************************************************
    // Run the tests on a Taxi instance
    // *******************************************************************************
    static void test_Taxi() {
      // **************************************************************
      // Chargement du simulateur
      gum::TaxiSimulator sim;


      // **************************************************************
      // Lancement
      run(sim);
    }
  };

  GUM_TEST_ACTIF(_Coffee)
  GUM_TEST_ACTIF(_Taxi)
}   // namespace gum_tests
