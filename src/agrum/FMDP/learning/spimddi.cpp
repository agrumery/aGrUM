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
#include <agrum/FMDP/learning/spimddi.h>
// =========================================================================

namespace gum {

    // ==========================================================================
    // Constructor & destructor.
    // ==========================================================================

        // ###################################################################
        // Default constructor
        // ###################################################################

        SPIMDDI::SPIMDDI ( double discountFactor,
                                       double epsilon,
                                       double learningThreshold,
                                       Idx observationPhaseLenght,
                                       Idx nbValueIterationStep,
                                       double exploThreshold ) : __observationPhaseLenght(observationPhaseLenght),
                                                                 __nbValueIterationStep(nbValueIterationStep),
                                                                 __exploThreshold(exploThreshold){

            GUM_CONSTRUCTOR(SPIMDDI)

            __fmdp = new FactoredMarkovDecisionProcess<double>();
            __fmdp->setDiscount(discountFactor);

            __learner = new FMDPLearner(__fmdp, learningThreshold);

            __planer = new SPUMDD(__fmdp, epsilon);

            __rewardVar = new LabelizedVariable("Reward", "");
            __rewardVar->eraseLabels();

            __nbObservation = 1;

            srand(time(NULL));
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

            delete __rewardVar;

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


        void SPIMDDI::addReward( double r ){
            std::ostringstream strs;
            strs << r;
            __rewardVar->addLabel( strs.str() );
            __rewardValue2Idx.insert(r, __rewardVar->labels().pos(strs.str()));
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

            __learner->addReward(__rewardVar);
            __learner->initialize();
            __planer->initialize();
        }

        // ###################################################################
        //
        // ###################################################################

        void SPIMDDI::feedback( const Instantiation& newState, double reward){

            std::cout << "Begin Feedback - Observation n° " << __nbObservation << std::endl;
            Observation* obs = new Observation();

            for( auto varIter = __lastState.variablesSequence().beginSafe(); varIter != __lastState.variablesSequence().endSafe(); ++varIter)
                obs->setModality(*varIter, __lastState.val(**varIter));

            for( auto varIter = newState.variablesSequence().beginSafe(); varIter != newState.variablesSequence().endSafe(); ++varIter)
                obs->setModality(__fmdp->main2prime(*varIter), newState.val(**varIter));

            obs->setModality( __rewardVar, __rewardValue2Idx.second(reward) );

            __learner->addObservation( __lastAction, obs );
            __bin.insert(obs);

            setCurrentState( newState );

            if( __nbObservation%__observationPhaseLenght == 0) {
                __learner->updateFMDP();
                __planer->makePlanning(__nbValueIterationStep);
            }
            __nbObservation++;
        }

        // ###################################################################
        //
        // ###################################################################

        Idx SPIMDDI::takeAction( ){

            double explo = (double)std::rand( ) / (double)RAND_MAX;
            if( __planer->optimalPolicy()->realSize() && explo > __exploThreshold){
                std::cout << "Exploitons!" << std::endl;
                __lastAction = __planer->optimalPolicy()->get(__lastState);
            }else{
                std::cout << "Explorons!" << std::endl;
                __lastAction = __actionSeq[ (Idx)((double)std::rand( ) / (double)RAND_MAX * __actionSeq.size()) ] ;
            }
            return __lastAction;
        }

        // ###################################################################
        //
        // ###################################################################

        std::string SPIMDDI::toString( ){

            std::stringstream description;

            description << __fmdp->show() << std::endl;

            if(__planer->optimalPolicy() )
                description << __planer->optimalPolicy()->toDot() << std::endl;

            for( auto actionIter = __fmdp->beginActions(); actionIter != __fmdp->endActions(); ++actionIter)
                description << *actionIter << " - " << __fmdp->actionName(*actionIter) <<std::endl;

            return description.str();

        }

} // End of namespace gum
