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
#include <cstdlib>
#include <random>
// =========================================================================
#include <agrum/FMDP/SDyna/sdyna.h>
// =========================================================================

namespace gum {

  // ==========================================================================
  // Constructor & destructor.
  // ==========================================================================

  // ###################################################################
  /*
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

  SDYNA::SDYNA( ILearningStrategy*         learner,
                IPlanningStrategy<double>* planer,
                IDecisionStrategy*         decider,
                Idx                        observationPhaseLenght,
                Idx                        nbValueIterationStep,
                bool                       actionReward,
                bool                       verbose )
      : __learner( learner )
      , __planer( planer )
      , __decider( decider )
      , __observationPhaseLenght( observationPhaseLenght )
      , __nbValueIterationStep( nbValueIterationStep )
      , __actionReward( actionReward )
      , _verbose( verbose ) {

    GUM_CONSTRUCTOR( SDYNA )

    _fmdp = new FMDP<double>();

    __nbObservation = 1;
  }

  // ###################################################################
  // Destructor
  // ###################################################################
  SDYNA::~SDYNA() {

    delete __decider;

    delete __learner;

    delete __planer;

    for ( auto obsIter = __bin.beginSafe(); obsIter != __bin.endSafe(); ++obsIter )
      delete *obsIter;

    delete _fmdp;

    GUM_DESTRUCTOR( SDYNA )
  }

  // ==========================================================================
  // Initialization
  // ==========================================================================

  void SDYNA::initialize() {
    __learner->initialize( _fmdp );
    __planer->initialize( _fmdp );
    __decider->initialize( _fmdp );
  }

  // ###################################################################
  /*
   * Initializes the Sdyna instance.
   * @param initialState : the state of the studied system from which we will
   * begin the explore, learn and exploit process
   */
  // ###################################################################
  void SDYNA::initialize( const Instantiation& initialState ) {
    initialize();
    setCurrentState( initialState );
  }

  // ==========================================================================
  /// Incremental methods
  // ==========================================================================

  // ###################################################################
  /*
   * Performs a feedback on the last transition.
   * In extenso, learn from the transition.
   * @param originalState : the state we were in before the transition
   * @param reachedState : the state we reached after
   * @param performedAction : the action we performed
   * @param obtainedReward : the reward we obtained
   */
  // ###################################################################
  void SDYNA::feedback( const Instantiation& curState,
                        const Instantiation& prevState,
                        Idx                  lastAction,
                        double               reward ) {
    __lastAction = lastAction;
    _lastState = prevState;
    feedback( curState, reward );
  }

  // ###################################################################
  /*
   * Performs a feedback on the last transition.
   * In extenso, learn from the transition.
   * @param reachedState : the state reached after the transition
   * @param obtainedReward : the reward obtained during the transition
   * @warning Uses the __originalState and __performedAction stored in cache
   * If you want to specify the original state and the performed action, see
   * below
   */
  // ###################################################################
  void SDYNA::feedback( const Instantiation& newState, double reward ) {
    Observation* obs = new Observation();

    for ( auto varIter = _lastState.variablesSequence().beginSafe();
          varIter != _lastState.variablesSequence().endSafe();
          ++varIter )
      obs->setModality( *varIter, _lastState.val( **varIter ) );

    for ( auto varIter = newState.variablesSequence().beginSafe();
          varIter != newState.variablesSequence().endSafe();
          ++varIter ) {
      obs->setModality( _fmdp->main2prime( *varIter ), newState.val( **varIter ) );

      if ( this->__actionReward )
        obs->setRModality( *varIter, _lastState.val( **varIter ) );
      else
        obs->setRModality( *varIter, newState.val( **varIter ) );
    }

    obs->setReward( reward );

    __learner->addObservation( __lastAction, obs );
    __bin.insert( obs );

    setCurrentState( newState );
    __decider->checkState( _lastState, __lastAction );

    if ( __nbObservation % __observationPhaseLenght == 0 )
      makePlanning( __nbValueIterationStep );

    __nbObservation++;
  }

  // ###################################################################
  /*
   * Starts a new planning
   * @param Idx : the maximal number of value iteration performed in this
   * planning
   */
  // ###################################################################
  void SDYNA::makePlanning( Idx nbValueIterationStep ) {
    if ( _verbose ) std::cout << "Updating decision trees ..." << std::endl;
    __learner->updateFMDP();
    // std::cout << << "Done" << std::endl;

    if ( _verbose ) std::cout << "Planning ..." << std::endl;
    __planer->makePlanning( nbValueIterationStep );
    // std::cout << << "Done" << std::endl;

    __decider->setOptimalStrategy( __planer->optimalPolicy() );
  }

  // ##################################################################
  /*
   * @return the id of the action the SDyna instance wish to be performed
   * @param the state in which we currently are
   */
  // ###################################################################
  Idx SDYNA::takeAction( const Instantiation& curState ) {
    _lastState = curState;
    return takeAction();
  }

  // ###################################################################
  /*
   * @return the id of the action the SDyna instance wish to be performed
   */
  // ###################################################################
  Idx SDYNA::takeAction() {
    ActionSet actionSet = __decider->stateOptimalPolicy( _lastState );
    if ( actionSet.size() == 1 ) {
      __lastAction = actionSet[0];
    } else {
      Idx randy =
          ( Idx )( (double)std::rand() / (double)RAND_MAX * actionSet.size() );
      __lastAction = actionSet[randy == actionSet.size() ? 0 : randy];
    }
    return __lastAction;
  }

  // ###################################################################
  //
  // ###################################################################
  std::string SDYNA::toString() {
    std::stringstream description;

    description << _fmdp->toString() << std::endl;
    description << __planer->optimalPolicy2String() << std::endl;

    return description.str();
  }

}  // End of namespace gum
