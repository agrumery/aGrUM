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
#include <agrum/FMDP/simulation/simulator.h>
#include <agrum/FMDP/learning/spimddi.h>
// ==============================================================================

namespace gum_tests {

  class SPIMDDITestSuite: public CxxTest::TestSuite {

    private :

      void run ( const std::string showSaveFile, gum::Idx nbVar, ... ) {

        // *********************************************************************************************
        // Chargement du fmdp servant de base
        // *********************************************************************************************
        gum::FactoredMarkovDecisionProcess<double> fmdp(true);
        gum::FMDPDatReader<double> reader ( &fmdp, GET_PATH_STR ( "FMDP/coffee/coffee.dat" ) );
        TS_GUM_ASSERT_THROWS_NOTHING ( reader.trace ( false ) );
        TS_GUM_ASSERT_THROWS_NOTHING ( reader.proceed( ) );

        // *********************************************************************************************
        // Initialisation des divers objets
        // *********************************************************************************************
        gum::Simulator sim(&fmdp);
        gum::SPIMDDI spim;
        gum::Instantiation initialState, endState;

        // Enregistrement des actions possibles auprès de SPIMDDI
        for( auto actionIter = fmdp.beginActions(); actionIter != fmdp.endActions(); ++actionIter){
          std::cout << "*actionIter : " << *actionIter << std::endl;
          spim.addAction( *actionIter, fmdp.actionName(*actionIter));
        }

        // Enregistrement des variables caractérisant les états auprès de SPIMDDI
        for(auto varIter = fmdp.beginVariables(); varIter != fmdp.endVariables(); ++varIter){
          spim.addVariable(*varIter);
          initialState.add(**varIter);
        }

        // Récupération des caractéristiques d'états finaux
        va_list ap;
        va_start(ap, nbVar);
        for(gum::Idx nbArg = 0; nbArg < nbVar; ++nbArg){
          std::string varName( va_arg(ap, char*) );
          std::string varModa( va_arg(ap, char*) );

          const gum::DiscreteVariable* varPtr = nullptr;
          for(auto varIter = fmdp.beginVariables(); varIter != fmdp.endVariables(); ++varIter)
            if(!varName.compare( (*varIter)->name())){
              varPtr= *varIter;
              break;
            }

          endState.add(*varPtr);

          gum::Idx varModaIdx = 0;
          for(gum::Idx varm = 0; varm < varPtr->domainSize(); ++varm)
            if(!varModa.compare( varPtr->label(varm))){
                varModaIdx = varm;
                break;
            }
          endState.chgVal(*varPtr, varModaIdx);
        }
        va_end(ap);

        // Création de la variables récompenses (to be deprecated)
//        const gum::MultiDimDecisionGraph<double>* reward = reinterpret_cast<const gum::MultiDimDecisionGraph<double>*>(fmdp.reward());
//        for( auto rewardIter = reward->values().beginSafe(); rewardIter != reward->values().endSafe(); ++rewardIter)
//          spim.addReward(rewardIter.second());

        TS_GUM_ASSERT_THROWS_NOTHING ( sim.setEndState(endState) );
        TS_GUM_ASSERT_THROWS_NOTHING ( spim.initialize() );
        TS_GUM_ASSERT_THROWS_NOTHING ( sim.setInitialStateRandomly() );



        for( gum::Idx nbRun = 0; nbRun < 10; ++nbRun ){

          TS_GUM_ASSERT_THROWS_NOTHING ( spim.setCurrentState(sim.currentState()) );

          while(!sim.hasReachEnd()){

            // Normal Iteration Part
            gum::Idx actionChosenId = spim.takeAction();
            std::cout << "Action Chosen : " << actionChosenId << std::endl;
            sim.perform( actionChosenId );
            std::cout << "Yoh!" << std::endl;

            spim.feedback(sim.currentState(), sim.reward());

          }
          TS_GUM_ASSERT_THROWS_NOTHING ( sim.setInitialStateRandomly() );
        }
        std::cout << spim.toString() << std::endl;
      }

    public:

      void test_Coffee() {
        std::string varName("huc");
        std::string varModa("yes");
        run ( "Coffee", 1, varName.c_str(), varModa.c_str() );
      }

  };
}
// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
