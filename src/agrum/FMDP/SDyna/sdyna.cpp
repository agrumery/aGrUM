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


/**
 * @file
 * @brief Headers of the ModelLearner class.
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(@AMU)
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

  SDYNA::SDYNA(ILearningStrategy*           learner,
               IPlanningStrategy< double >* planer,
               IDecisionStrategy*           decider,
               Idx                          observationPhaseLenght,
               Idx                          nbValueIterationStep,
               bool                         actionReward,
               bool                         verbose) :
      learner__(learner),
      planer__(planer), decider__(decider),
      observationPhaseLenght__(observationPhaseLenght),
      nbValueIterationStep__(nbValueIterationStep), actionReward__(actionReward),
      verbose_(verbose) {
    GUM_CONSTRUCTOR(SDYNA);

    fmdp_ = new FMDP< double >();

    nbObservation__ = 1;
  }

  // ###################################################################
  // Destructor
  // ###################################################################
  SDYNA::~SDYNA() {
    delete decider__;

    delete learner__;

    delete planer__;

    for (auto obsIter = bin__.beginSafe(); obsIter != bin__.endSafe(); ++obsIter)
      delete *obsIter;

    delete fmdp_;

    GUM_DESTRUCTOR(SDYNA);
  }

  // ==========================================================================
  // Initialization
  // ==========================================================================

  void SDYNA::initialize() {
    learner__->initialize(fmdp_);
    planer__->initialize(fmdp_);
    decider__->initialize(fmdp_);
  }

  // ###################################################################
  /*
   * Initializes the Sdyna instance.
   * @param initialState : the state of the studied system from which we will
   * begin the explore, learn and exploit process
   */
  // ###################################################################
  void SDYNA::initialize(const Instantiation& initialState) {
    initialize();
    setCurrentState(initialState);
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
  void SDYNA::feedback(const Instantiation& curState,
                       const Instantiation& prevState,
                       Idx                  lastAction,
                       double               reward) {
    lastAction__ = lastAction;
    lastState_   = prevState;
    feedback(curState, reward);
  }

  // ###################################################################
  /*
   * Performs a feedback on the last transition.
   * In extenso, learn from the transition.
   * @param reachedState : the state reached after the transition
   * @param obtainedReward : the reward obtained during the transition
   * @warning Uses the originalState__ and performedAction__ stored in cache
   * If you want to specify the original state and the performed action, see
   * below
   */
  // ###################################################################
  void SDYNA::feedback(const Instantiation& newState, double reward) {
    Observation* obs = new Observation();

    for (auto varIter = lastState_.variablesSequence().beginSafe();
         varIter != lastState_.variablesSequence().endSafe();
         ++varIter)
      obs->setModality(*varIter, lastState_.val(**varIter));

    for (auto varIter = newState.variablesSequence().beginSafe();
         varIter != newState.variablesSequence().endSafe();
         ++varIter) {
      obs->setModality(fmdp_->main2prime(*varIter), newState.val(**varIter));

      if (this->actionReward__)
        obs->setRModality(*varIter, lastState_.val(**varIter));
      else
        obs->setRModality(*varIter, newState.val(**varIter));
    }

    obs->setReward(reward);

    learner__->addObservation(lastAction__, obs);
    bin__.insert(obs);

    setCurrentState(newState);
    decider__->checkState(lastState_, lastAction__);

    if (nbObservation__ % observationPhaseLenght__ == 0)
      makePlanning(nbValueIterationStep__);

    nbObservation__++;
  }

  // ###################################################################
  /*
   * Starts a new planning
   * @param Idx : the maximal number of value iteration performed in this
   * planning
   */
  // ###################################################################
  void SDYNA::makePlanning(Idx nbValueIterationStep) {
    if (verbose_) std::cout << "Updating decision trees ..." << std::endl;
    learner__->updateFMDP();
    // std::cout << << "Done" << std::endl;

    if (verbose_) std::cout << "Planning ..." << std::endl;
    planer__->makePlanning(nbValueIterationStep);
    // std::cout << << "Done" << std::endl;

    decider__->setOptimalStrategy(planer__->optimalPolicy());
  }

  // ##################################################################
  /*
   * @return the id of the action the SDyna instance wish to be performed
   * @param the state in which we currently are
   */
  // ###################################################################
  Idx SDYNA::takeAction(const Instantiation& curState) {
    lastState_ = curState;
    return takeAction();
  }

  // ###################################################################
  /*
   * @return the id of the action the SDyna instance wish to be performed
   */
  // ###################################################################
  Idx SDYNA::takeAction() {
    ActionSet actionSet = decider__->stateOptimalPolicy(lastState_);
    if (actionSet.size() == 1) {
      lastAction__ = actionSet[0];
    } else {
      Idx randy = (Idx)((double)std::rand() / (double)RAND_MAX * actionSet.size());
      lastAction__ = actionSet[randy == actionSet.size() ? 0 : randy];
    }
    return lastAction__;
  }

  // ###################################################################
  //
  // ###################################################################
  std::string SDYNA::toString() {
    std::stringstream description;

    description << fmdp_->toString() << std::endl;
    description << planer__->optimalPolicy2String() << std::endl;

    return description.str();
  }

}   // End of namespace gum
