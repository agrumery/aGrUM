/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Headers of the ModelLearner class.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */


// =========================================================================
#include <cstdlib>
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
      _learner_(learner), _planer_(planer), _decider_(decider),
      _observationPhaseLenght_(observationPhaseLenght),
      _nbValueIterationStep_(nbValueIterationStep), _actionReward_(actionReward),
      verbose_(verbose) {
    GUM_CONSTRUCTOR(SDYNA);

    fmdp_ = new FMDP< double >();

    _nbObservation_ = 1;
  }

  // ###################################################################
  // Destructor
  // ###################################################################
  SDYNA::~SDYNA() {
    delete _decider_;

    delete _learner_;

    delete _planer_;

    for (auto obsIter = _bin_.beginSafe(); obsIter != _bin_.endSafe(); ++obsIter)
      delete *obsIter;

    delete fmdp_;

    GUM_DESTRUCTOR(SDYNA);
  }

  // ==========================================================================
  // Initialization
  // ==========================================================================

  void SDYNA::initialize() {
    _learner_->initialize(fmdp_);
    _planer_->initialize(fmdp_);
    _decider_->initialize(fmdp_);
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
    _lastAction_ = lastAction;
    lastState_   = prevState;
    feedback(curState, reward);
  }

  // ###################################################################
  /*
   * Performs a feedback on the last transition.
   * In extenso, learn from the transition.
   * @param reachedState : the state reached after the transition
   * @param obtainedReward : the reward obtained during the transition
   * @warning Uses the  _originalState_ and  _performedAction_ stored in cache
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

      if (this->_actionReward_) obs->setRModality(*varIter, lastState_.val(**varIter));
      else obs->setRModality(*varIter, newState.val(**varIter));
    }

    obs->setReward(reward);

    _learner_->addObservation(_lastAction_, obs);
    _bin_.insert(obs);

    setCurrentState(newState);
    _decider_->checkState(lastState_, _lastAction_);

    if (_nbObservation_ % _observationPhaseLenght_ == 0) makePlanning(_nbValueIterationStep_);

    _nbObservation_++;
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
    _learner_->updateFMDP();
    // std::cout << << "Done" << std::endl;

    if (verbose_) std::cout << "Planning ..." << std::endl;
    _planer_->makePlanning(nbValueIterationStep);
    // std::cout << << "Done" << std::endl;

    _decider_->setOptimalStrategy(_planer_->optimalPolicy());
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
    ActionSet actionSet = _decider_->stateOptimalPolicy(lastState_);
    if (actionSet.size() == 1) {
      _lastAction_ = actionSet[0];
    } else {
      Idx randy    = randomValue(actionSet.size());
      _lastAction_ = actionSet[randy == actionSet.size() ? 0 : randy];
    }
    return _lastAction_;
  }

  // ###################################################################
  //
  // ###################################################################
  std::string SDYNA::toString() {
    std::stringstream description;

    description << fmdp_->toString() << std::endl;
    description << _planer_->optimalPolicy2String() << std::endl;

    return description.str();
  }

}   // End of namespace gum
