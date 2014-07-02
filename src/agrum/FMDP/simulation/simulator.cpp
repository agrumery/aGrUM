/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
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
/**
 * @file
 * @brief Sources of
 *
 * @author Jean-Christophe Magnan
 *
 */
// =====================================================================
#include <random>
#include <cstdlib>
//======================================================================
#include <agrum/FMDP/simulation/simulator.h>
//======================================================================

namespace gum {

    // ===========================================================================
    // Constructors, Destructors.
    // ===========================================================================

      /*
       * Default constructor.
       */
      Simulator::Simulator(const FactoredMarkovDecisionProcess<double>* fmdp):__fmdp(fmdp){
          GUM_CONSTRUCTOR(Simulator)
      }

      /*
       * Default destructor.
       */
      Simulator::~Simulator(){
          GUM_DESTRUCTOR(Simulator)
      }


    // ===========================================================================
    //
    // ===========================================================================

      ///
      void Simulator::setInitialStateRandomly() {

          do{
              __currentState.clear();
              for(auto varIter = __fmdp->beginVariables(); varIter != __fmdp->endVariables(); ++varIter){
                  __currentState.add(**varIter);
                  __currentState.chgVal(*varIter,  (Idx)(((double)std::rand( ) / (double)RAND_MAX) * (*varIter)->domainSize()));
              }
          }while(hasReachEnd());
      }

      ///
      bool Simulator::hasReachEnd(){

          for(auto varIter = __endState.variablesSequence().beginSafe(); varIter != __endState.variablesSequence().endSafe(); ++varIter)
              if(__endState.val(**varIter) != __currentState.val(**varIter))
                  return false;
          return true;
      }

      ///
      void Simulator::perform( Idx actionId ){

          srand( time(NULL) );

          Instantiation newState;
          for( auto varIter = __fmdp->beginVariables(); varIter != __fmdp->endVariables(); ++varIter ){

              newState.add(**varIter);
              Instantiation transit(__currentState);
              transit.add(*(__fmdp->main2prime(*varIter)));

              const MultiDimDecisionGraph<double>* varDG = reinterpret_cast<const MultiDimDecisionGraph<double>*>( __fmdp->transition(actionId, *varIter) );

              double proba = (double)std::rand( ) / (double)RAND_MAX;
              double cdd = 0.0;
              for(transit.setFirstOut(__currentState);!transit.end();transit.incOut(__currentState)){
                  cdd += varDG->get(transit);
                  if(proba <= cdd) {
                      newState.chgVal(**varIter, transit.val(*(__fmdp->main2prime(*varIter))));
                      break;
                  }
              }

          }
          __currentState = newState;

      }


} // End of namespace gum
