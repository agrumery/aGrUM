/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief Template Implementations of the FMDPLearner class.
 *
 * @author Jean-Christophe MAGNAN
 */

// =========================================================================
#include <agrum/FMDP/learning/fmdpLearner.h>
// =========================================================================

namespace gum {

  // ==========================================================================
  // Constructor & destructor.
  // ==========================================================================

  // ###################################################################
  // Default constructor
  // ###################################################################
  template < TESTNAME    VariableAttributeSelection,
             TESTNAME    RewardAttributeSelection,
             LEARNERNAME LearnerSelection >
  FMDPLearner< VariableAttributeSelection, RewardAttributeSelection, LearnerSelection >::
     FMDPLearner(double lT, bool actionReward, double sT) :
      _actionReward_(actionReward),
      _learningThreshold_(lT), _similarityThreshold_(sT) {
    GUM_CONSTRUCTOR(FMDPLearner);
    _rewardLearner_ = nullptr;
  }


  // ###################################################################
  // Default destructor
  // ###################################################################
  template < TESTNAME    VariableAttributeSelection,
             TESTNAME    RewardAttributeSelection,
             LEARNERNAME LearnerSelection >
  FMDPLearner< VariableAttributeSelection, RewardAttributeSelection, LearnerSelection >::
     ~FMDPLearner() {
    for (auto actionIter = _actionLearners_.beginSafe(); actionIter != _actionLearners_.endSafe();
         ++actionIter) {
      for (auto learnerIter = actionIter.val()->beginSafe();
           learnerIter != actionIter.val()->endSafe();
           ++learnerIter)
        delete learnerIter.val();
      delete actionIter.val();
      if (_actionRewardLearners_.exists(actionIter.key()))
        delete _actionRewardLearners_[actionIter.key()];
    }

    if (_rewardLearner_) delete _rewardLearner_;

    GUM_DESTRUCTOR(FMDPLearner);
  }


  // ==========================================================================
  //
  // ==========================================================================

  // ###################################################################
  //
  // ###################################################################
  template < TESTNAME    VariableAttributeSelection,
             TESTNAME    RewardAttributeSelection,
             LEARNERNAME LearnerSelection >
  void FMDPLearner< VariableAttributeSelection, RewardAttributeSelection, LearnerSelection >::
     initialize(FMDP< double >* fmdp) {
    _fmdp_ = fmdp;

    _modaMax_ = 0;
    _rmax_    = 0.0;

    Set< const DiscreteVariable* > mainVariables;
    for (auto varIter = _fmdp_->beginVariables(); varIter != _fmdp_->endVariables(); ++varIter) {
      mainVariables.insert(*varIter);
      _modaMax_ = _modaMax_ < (*varIter)->domainSize() ? (*varIter)->domainSize() : _modaMax_;
    }

    for (auto actionIter = _fmdp_->beginActions(); actionIter != _fmdp_->endActions();
         ++actionIter) {
      // Adding a Hashtable for the action
      _actionLearners_.insert(*actionIter, new VarLearnerTable());

      // Adding a learner for each variable
      for (auto varIter = _fmdp_->beginVariables(); varIter != _fmdp_->endVariables(); ++varIter) {
        MultiDimFunctionGraph< double >* varTrans = _instantiateFunctionGraph_();
        varTrans->setTableName("ACTION : " + _fmdp_->actionName(*actionIter)
                               + " - VARIABLE : " + (*varIter)->name());
        _fmdp_->addTransitionForAction(*actionIter, *varIter, varTrans);
        _actionLearners_[*actionIter]->insert(
           (*varIter),
           _instantiateVarLearner_(varTrans, mainVariables, _fmdp_->main2prime(*varIter)));
      }

      if (_actionReward_) {
        MultiDimFunctionGraph< double >* reward = _instantiateFunctionGraph_();
        reward->setTableName("REWARD - ACTION : " + _fmdp_->actionName(*actionIter));
        _fmdp_->addRewardForAction(*actionIter, reward);
        _actionRewardLearners_.insert(*actionIter,
                                      _instantiateRewardLearner_(reward, mainVariables));
      }
    }

    if (!_actionReward_) {
      MultiDimFunctionGraph< double >* reward = _instantiateFunctionGraph_();
      reward->setTableName("REWARD");
      _fmdp_->addReward(reward);
      _rewardLearner_ = _instantiateRewardLearner_(reward, mainVariables);
    }
  }

  // ###################################################################
  //
  // ###################################################################
  template < TESTNAME    VariableAttributeSelection,
             TESTNAME    RewardAttributeSelection,
             LEARNERNAME LearnerSelection >
  bool FMDPLearner< VariableAttributeSelection, RewardAttributeSelection, LearnerSelection >::
     addObservation(Idx actionId, const Observation* newObs) {
    for (SequenceIteratorSafe< const DiscreteVariable* > varIter = _fmdp_->beginVariables();
         varIter != _fmdp_->endVariables();
         ++varIter) {
      _actionLearners_[actionId]->getWithDefault(*varIter, nullptr)->addObservation(newObs);
      _actionLearners_[actionId]->getWithDefault(*varIter, nullptr)->updateGraph();
    }

    if (_actionReward_) {
      _actionRewardLearners_[actionId]->addObservation(newObs);
      _actionRewardLearners_[actionId]->updateGraph();
    } else {
      _rewardLearner_->addObservation(newObs);
      _rewardLearner_->updateGraph();
    }

    _rmax_ = _rmax_ < std::abs(newObs->reward()) ? std::abs(newObs->reward()) : _rmax_;

    return false;
  }

  // ###################################################################
  //
  // ###################################################################
  template < TESTNAME    VariableAttributeSelection,
             TESTNAME    RewardAttributeSelection,
             LEARNERNAME LearnerSelection >
  Size
     FMDPLearner< VariableAttributeSelection, RewardAttributeSelection, LearnerSelection >::size() {
    Size s = 0;
    for (SequenceIteratorSafe< Idx > actionIter = _fmdp_->beginActions();
         actionIter != _fmdp_->endActions();
         ++actionIter) {
      for (SequenceIteratorSafe< const DiscreteVariable* > varIter = _fmdp_->beginVariables();
           varIter != _fmdp_->endVariables();
           ++varIter)
        s += _actionLearners_[*actionIter]->getWithDefault(*varIter, nullptr)->size();
      if (_actionReward_) s += _actionRewardLearners_[*actionIter]->size();
    }

    if (!_actionReward_) s += _rewardLearner_->size();

    return s;
  }


  // ###################################################################
  //
  // ###################################################################
  template < TESTNAME    VariableAttributeSelection,
             TESTNAME    RewardAttributeSelection,
             LEARNERNAME LearnerSelection >
  void FMDPLearner< VariableAttributeSelection, RewardAttributeSelection, LearnerSelection >::
     updateFMDP() {
    for (SequenceIteratorSafe< Idx > actionIter = _fmdp_->beginActions();
         actionIter != _fmdp_->endActions();
         ++actionIter) {
      for (SequenceIteratorSafe< const DiscreteVariable* > varIter = _fmdp_->beginVariables();
           varIter != _fmdp_->endVariables();
           ++varIter)
        _actionLearners_[*actionIter]->getWithDefault(*varIter, nullptr)->updateFunctionGraph();
      if (_actionReward_) _actionRewardLearners_[*actionIter]->updateFunctionGraph();
    }

    if (!_actionReward_) _rewardLearner_->updateFunctionGraph();
  }
}   // End of namespace gum
