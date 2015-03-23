/***************************************************************************
 *  Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN   *
 *  {prenom.nom}_at_lip6.fr                                                *
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the           *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program; if not, write to the                          *
 *  Free Software Foundation, Inc.,                                        *
 *  59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.               *
 ***************************************************************************/
/**
 * @file
 * @brief Headers of the ModelLearner class.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */

// =========================================================================
#include <random>
#include <cstdlib>
// =========================================================================
#include <agrum/FMDP/learning/SDyna/spimddi.h>
#include <agrum/FMDP/planning/pspumdd.h>
//#include <agrum/FMDP/planning/RMaxMDD.h>
// =========================================================================

namespace gum {

    // ==========================================================================
    // Constructor & destructor.
    // ==========================================================================

        // ###################################################################
        // Default constructor
        // ###################################################################

        SPIMDDI::SPIMDDI (double discountFactor,
                          double epsilon,
                          double learningThreshold,
                          double similarityThreshold,
                          Idx observationPhaseLenght,
                          Idx nbValueIterationStep,
                          double exploThreshold ) : __observationPhaseLenght(observationPhaseLenght),
                                                    __nbValueIterationStep(nbValueIterationStep),
                                                    __exploThreshold(exploThreshold){

          GUM_CONSTRUCTOR(SPIMDDI)

          __fmdp = new FactoredMarkovDecisionProcess<double>();
          __fmdp->setDiscount(discountFactor);

          __learner = new FMDPLearner<GTEST, GTEST, IMDDILEARNER>(__fmdp, learningThreshold, similarityThreshold );

          __planer = new SPUMDD<double>(__fmdp, epsilon);
//          __planer = new RMaxMDD<double>(__fmdp,this, 40);
          __rmax = std::numeric_limits<double>::min();

          __nbObservation = 1;
        }

        // ###################################################################
        // Default destructor
        // ###################################################################

        SPIMDDI::~SPIMDDI (){

          delete __learner;

          for(auto obsIter = __bin.beginSafe(); obsIter != __bin.endSafe(); ++obsIter)
              delete *obsIter;

          delete __planer;
          delete __fmdp;

          GUM_DESTRUCTOR(SPIMDDI)
        }

    // ==========================================================================
    //
    // ==========================================================================

        // ###################################################################
        //
        // ###################################################################

        void SPIMDDI::addAction(const Idx actionId, const std::string &actionName ){
          __learner->addAction(actionId, actionName);
          __actionSeq.insert(actionId);
        }


        void SPIMDDI::addVariable( const DiscreteVariable* var ){
          __learner->addVariable(var);
        }

    // ==========================================================================
    //
    // ==========================================================================

        // ###################################################################
        //
        // ###################################################################
        void SPIMDDI::initialize( const Instantiation& initialState ){
          initialize();
          setCurrentState( initialState );
        }

        // ###################################################################
        //
        // ###################################################################
        void SPIMDDI::initialize(  ){
          __learner->initialize();
          __planer->initialize();
        }

        // ###################################################################
        //
        // ###################################################################
        void SPIMDDI::feedback( const Instantiation& newState, double reward ){

            if(reward > __rmax)
              __rmax = reward;

            std::cout << std::endl << "*********************************************\nBegin Feedback - Observation n°" << __nbObservation
                      << "\n*********************************************\n" <<  std::endl;
            Observation* obs = new Observation();

            for( auto varIter = __lastState.variablesSequence().beginSafe(); varIter != __lastState.variablesSequence().endSafe(); ++varIter)
                obs->setModality(*varIter, __lastState.val(**varIter));

            for( auto varIter = newState.variablesSequence().beginSafe(); varIter != newState.variablesSequence().endSafe(); ++varIter){
              obs->setModality(__fmdp->main2prime(*varIter), newState.val(**varIter));
              obs->setRModality(*varIter, newState.val(**varIter));
            }

            obs->setReward(reward);

            std::cout << "Taking observation into account ..." <<  std::endl;
            __learner->addObservation( __lastAction, obs );
            std::cout << "Done" <<  std::endl;
            __bin.insert(obs);

            setCurrentState( newState );

            if( __nbObservation%__observationPhaseLenght == 0) {
                std::cout << "Updating FMDP ..." <<  std::endl;
                __learner->updateFMDP();
                std::cout << "Done" <<  std::endl;
//                std::cout << __fmdp->toString() << std::endl;

                std::cout << "Performing planning ..." <<  std::endl;
                __planer->makePlanning(__nbValueIterationStep);
                std::cout << "Done" <<  std::endl;
            }

            __nbObservation++;
            std::cout << "\n*********************************************\n" << " " << std::endl << " " << std::endl;
        }

        // ###################################################################
        //
        // ###################################################################
        void SPIMDDI::feedback( const Instantiation& curState, const Instantiation& prevState, Idx lastAction, double reward){
          __lastAction = lastAction;
          __lastState = prevState;
          feedback( curState, reward );
        }

        // ###################################################################
        //
        // ###################################################################
        Idx SPIMDDI::takeAction( ){
          double explo = (double)std::rand( ) / (double)RAND_MAX;
          ActionSet actionSet;
          if( __planer->optimalPolicy()->realSize() && explo > __exploThreshold){
            std::cout << "Exploitons!" << std::endl;
            actionSet = __planer->optimalPolicy()->get(__lastState);
          }else{
            std::cout << "Explorons!" << std::endl;
            for( auto actionIter = __fmdp->beginActions(); actionIter != __fmdp->endActions(); ++actionIter )
              actionSet += *actionIter;
          }
          if( actionSet.size() == 1 ) {
            __lastAction = actionSet[0];
          } else {
              Idx randy = (Idx)( (double)std::rand( ) / (double)RAND_MAX * actionSet.size() );
              __lastAction = actionSet[ randy==actionSet.size()?0:randy ] ;
          }
          return __lastAction;
        }

        // ###################################################################
        //
        // ###################################################################
        Idx SPIMDDI::takeAction( const Instantiation& curState ){
            __lastState = curState;
            return takeAction();
        }

        // ###################################################################
        //
        // ###################################################################
        std::string SPIMDDI::toString( ){
          std::stringstream description;

          description << __fmdp->toString() << std::endl;

          if(__planer->optimalPolicy() )
              description << __planer->optimalPolicy2String() << std::endl;

          for( auto actionIter = __fmdp->beginActions(); actionIter != __fmdp->endActions(); ++actionIter)
              description << *actionIter << " - " << __fmdp->actionName(*actionIter) <<std::endl;

          return description.str();
        }

} // End of namespace gum
