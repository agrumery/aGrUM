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
#include <cstdarg>
#include <iostream>
#include <string>
// ==============================================================================
#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>
// ==============================================================================
#include <agrum/FMDP/SDyna/sdyna.h>
#include <agrum/FMDP/fmdp.h>
#include <agrum/FMDP/io/dat/fmdpDatReader.h>
#include <agrum/FMDP/simulation/fmdpSimulator.h>
#include <agrum/FMDP/simulation/taxiSimulator.h>
// ==============================================================================

namespace gum_tests {

  class SDynaTestSuite : public CxxTest::TestSuite {

    private:
    void run( gum::AbstractSimulator& sim ) {
GUM_CHECKPOINT;
      // *********************************************************************************************
      // Initialisation de l'instance de SDyna
      // *********************************************************************************************
      gum::SDYNA* sdyna = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING( sdyna = gum::SDYNA::spimddiInstance() );
      GUM_CHECKPOINT;

      // Enregistrement des actions possibles auprès de SDyna
      for ( auto actionIter = sim.beginActions();
            actionIter != sim.endActions();
            ++actionIter ) {
        sdyna->addAction( *actionIter, sim.actionName( *actionIter ) );
      }
GUM_CHECKPOINT;
      // Enregistrement des variables caractérisant les états auprès de SDyna
      for ( auto varIter = sim.beginVariables(); varIter != sim.endVariables();
            ++varIter ) {
        sdyna->addVariable( *varIter );
      }
GUM_CHECKPOINT;
      TS_GUM_ASSERT_THROWS_NOTHING( sim.setInitialStateRandomly() );
      TS_GUM_ASSERT_THROWS_NOTHING( sdyna->initialize( sim.currentState() ) );

GUM_CHECKPOINT;
      gum::Idx nbObs = 0;
      for ( gum::Idx nbRun = 0; nbRun < 10; ++nbRun ) {

        sim.setInitialStateRandomly();
        TS_GUM_ASSERT_THROWS_NOTHING(
            sdyna->setCurrentState( sim.currentState() ) );
        gum::Idx nbDec = 0;
GUM_CHECKPOINT;
        while ( !sim.hasReachEnd() && nbDec < 25 ) {

          nbObs++;
          GUM_CHECKPOINT;

          // Normal Iteration Part
          gum::Idx actionChosenId = 0;
          TS_GUM_ASSERT_THROWS_NOTHING( actionChosenId = sdyna->takeAction(); );
          GUM_CHECKPOINT;
          TS_GUM_ASSERT_THROWS_NOTHING( sim.perform( actionChosenId ) );
          GUM_CHECKPOINT;
          nbDec++;

          TS_GUM_ASSERT_THROWS_NOTHING(
              sdyna->feedback( sim.currentState(), sim.reward() ); );
          GUM_CHECKPOINT;
        }
        TS_GUM_ASSERT_THROWS_NOTHING( sim.setInitialStateRandomly() );
        GUM_CHECKPOINT;
      }
      TS_GUM_ASSERT_THROWS_NOTHING( delete sdyna );
      GUM_CHECKPOINT;
    }

    public:
    // *******************************************************************************
    // Run the tests on a Coffee FMDP
    // *******************************************************************************
    void test_Coffee() {
      GUM_CHECKPOINT;
      // **************************************************************
      // Chargement du fmdp servant de base
      gum::FMDPSimulator sim( GET_RESSOURCES_PATH( "FMDP/coffee/coffee.dat" ) );
      GUM_CHECKPOINT;

      // **************************************************************
      // Définition of final states
      gum::Instantiation theEnd;
      GUM_CHECKPOINT;
      for ( gum::SequenceIteratorSafe<const gum::DiscreteVariable*> varIter =
                sim.beginVariables();
            varIter != sim.endVariables();
            ++varIter ) {
        if ( ( *varIter )->name().compare( "huc" ) ) {
          theEnd.add( **varIter );
          theEnd.chgVal( **varIter, ( *varIter )->index( "yes" ) );
          break;
        }
      GUM_CHECKPOINT;
      }
      GUM_CHECKPOINT;
      TS_GUM_ASSERT_THROWS_NOTHING( sim.setEndState( theEnd ) );

      GUM_CHECKPOINT;

      // **************************************************************
      // Lancement
      run( sim );
      GUM_CHECKPOINT;
    }


    // *******************************************************************************
    // Run the tests on a Taxi instance
    // *******************************************************************************
    void test_Taxi() {
      GUM_CHECKPOINT;

      // **************************************************************
      // Chargement du simulateur
      gum::TaxiSimulator sim;


      // **************************************************************
      // Lancement
      run( sim );
    }
  };
}
// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
