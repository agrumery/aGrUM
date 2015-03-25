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
#include <agrum/FMDP/learning/SDyna/sdyna.h>
// =========================================================================

namespace gum {

  // ==========================================================================
  // Constructor & destructor.
  // ==========================================================================

    // ###################################################################
    /**
     * Constructor
     *
     * @param observationPhaseLenght : the number of observation done before a
     * replanning is launch. If equals 0, a planning is done after each structural
     * change.
     * @param nbValueIterationStep : the number of value iteration done during
     * one planning
     * @return an instance of SDyna architecture
     */
    // ###################################################################

    SDYNA::SDYNA (Idx observationPhaseLenght,
                  Idx nbValueIterationStep,
                  double discountFactor ) : __observationPhaseLenght(observationPhaseLenght),
                                            __nbValueIterationStep(nbValueIterationStep){

      GUM_CONSTRUCTOR(SDYNA)

      _fmdp = new FactoredMarkovDecisionProcess<double>();
      _fmdp->setDiscount(discountFactor);

      __nbObservation = 1;
    }

    // ###################################################################
    /// Destructor
    // ###################################################################
    SDYNA::~SDYNA (){

      for(auto obsIter = __bin.beginSafe(); obsIter != __bin.endSafe(); ++obsIter)
          delete *obsIter;

      delete _fmdp;

      GUM_DESTRUCTOR(SDYNA)
    }

  // ==========================================================================
  /// Problem specification methods
  // ==========================================================================

    // ###################################################################
    /**
     * Inserts a new action in the SDyna instance.
     * @warning Without effect until method initialize is called
     * @param actionId : an id to identify the action
     * @param actionName : its human name
     */
    // ###################################################################
    void SDYNA::addAction(const Idx actionId, const std::string &actionName ){
      __actionSeq.insert(actionId);
    }

  // ==========================================================================
  /// Initialization
  // ==========================================================================

    // ###################################################################
    /**
     * Initializes the Sdyna instance.
     * @param initialState : the state of the studied system from which we will
     * begin the explore, learn and exploit process
     */
    // ###################################################################
    void SDYNA::initialize( const Instantiation& initialState ){
      initialize();
      setCurrentState( initialState );
    }

  // ==========================================================================
  /// Incremental methods
  // ==========================================================================

    // ###################################################################
    /**
     * Performs a feedback on the last transition.
     * In extenso, learn from the transition.
     * @param originalState : the state we were in before the transition
     * @param reachedState : the state we reached after
     * @param performedAction : the action we performed
     * @param obtainedReward : the reward we obtained
     */
    // ###################################################################
    void SDYNA::feedback( const Instantiation& curState, const Instantiation& prevState, Idx lastAction, double reward){
      __lastAction = lastAction;
      _lastState = prevState;
      feedback( curState, reward );
    }

    // ###################################################################
    /**
     * Performs a feedback on the last transition.
     * In extenso, learn from the transition.
     * @param reachedState : the state reached after the transition
     * @param obtainedReward : the reward obtained during the transition
     * @warning Uses the __originalState and __performedAction stored in cache
     * If you want to specify the original state and the performed action, see below
     */
    // ###################################################################
    void SDYNA::feedback( const Instantiation& newState, double reward ){

      Observation* obs = new Observation();

      for( auto varIter = _lastState.variablesSequence().beginSafe(); varIter != _lastState.variablesSequence().endSafe(); ++varIter)
        obs->setModality(*varIter, _lastState.val(**varIter));

      for( auto varIter = newState.variablesSequence().beginSafe(); varIter != newState.variablesSequence().endSafe(); ++varIter){
        obs->setModality(_fmdp->main2prime(*varIter), newState.val(**varIter));
        obs->setRModality(*varIter, newState.val(**varIter));
      }

      obs->setReward(reward);

      _learnTransition( __lastAction, obs );
      __bin.insert(obs);

      setCurrentState( newState );

      if( __nbObservation%__observationPhaseLenght == 0)
        _makePlanning(__nbValueIterationStep);

      __nbObservation++;
    }

    // ###################################################################
    /**
     * @return the id of the action the SDyna instance wish to be performed
     * @param the state in which we currently are
     */
    // ###################################################################
    Idx SDYNA::takeAction( const Instantiation& curState ){
        _lastState = curState;
        return takeAction();
    }

    // ###################################################################
    /**
     * @return the id of the action the SDyna instance wish to be performed
     */
    // ###################################################################
    Idx SDYNA::takeAction( ){
      ActionSet actionSet = _stateActionSet( _lastState );
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
    std::string SDYNA::toString( ){
      std::stringstream description;

      description << _fmdp->toString() << std::endl;

      return description.str();
    }

} // End of namespace gum
