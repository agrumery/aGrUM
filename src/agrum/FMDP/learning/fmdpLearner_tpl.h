
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
  FMDPLearner< VariableAttributeSelection,
               RewardAttributeSelection,
               LearnerSelection >::FMDPLearner(double lT,
                                               bool   actionReward,
                                               double sT) :
      actionReward__(actionReward),
      learningThreshold__(lT), similarityThreshold__(sT) {
    GUM_CONSTRUCTOR(FMDPLearner);
    rewardLearner__ = nullptr;
  }


  // ###################################################################
  // Default destructor
  // ###################################################################
  template < TESTNAME    VariableAttributeSelection,
             TESTNAME    RewardAttributeSelection,
             LEARNERNAME LearnerSelection >
  FMDPLearner< VariableAttributeSelection,
               RewardAttributeSelection,
               LearnerSelection >::~FMDPLearner() {
    for (auto actionIter = actionLearners__.beginSafe();
         actionIter != actionLearners__.endSafe();
         ++actionIter) {
      for (auto learnerIter = actionIter.val()->beginSafe();
           learnerIter != actionIter.val()->endSafe();
           ++learnerIter)
        delete learnerIter.val();
      delete actionIter.val();
      if (actionRewardLearners__.exists(actionIter.key()))
        delete actionRewardLearners__[actionIter.key()];
    }

    if (rewardLearner__) delete rewardLearner__;

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
  void FMDPLearner< VariableAttributeSelection,
                    RewardAttributeSelection,
                    LearnerSelection >::initialize(FMDP< double >* fmdp) {
    fmdp__ = fmdp;

    modaMax__ = 0;
    rmax__ = 0.0;

    Set< const DiscreteVariable* > mainVariables;
    for (auto varIter = fmdp__->beginVariables();
         varIter != fmdp__->endVariables();
         ++varIter) {
      mainVariables.insert(*varIter);
      modaMax__ = modaMax__ < (*varIter)->domainSize() ? (*varIter)->domainSize()
                                                       : modaMax__;
    }

    for (auto actionIter = fmdp__->beginActions();
         actionIter != fmdp__->endActions();
         ++actionIter) {
      // Adding a Hashtable for the action
      actionLearners__.insert(*actionIter, new VarLearnerTable());

      // Adding a learner for each variable
      for (auto varIter = fmdp__->beginVariables();
           varIter != fmdp__->endVariables();
           ++varIter) {
        MultiDimFunctionGraph< double >* varTrans = instantiateFunctionGraph__();
        varTrans->setTableName("ACTION : " + fmdp__->actionName(*actionIter)
                               + " - VARIABLE : " + (*varIter)->name());
        fmdp__->addTransitionForAction(*actionIter, *varIter, varTrans);
        actionLearners__[*actionIter]->insert(
           (*varIter),
           instantiateVarLearner__(
              varTrans, mainVariables, fmdp__->main2prime(*varIter)));
      }

      if (actionReward__) {
        MultiDimFunctionGraph< double >* reward = instantiateFunctionGraph__();
        reward->setTableName("REWARD - ACTION : "
                             + fmdp__->actionName(*actionIter));
        fmdp__->addRewardForAction(*actionIter, reward);
        actionRewardLearners__.insert(
           *actionIter, instantiateRewardLearner__(reward, mainVariables));
      }
    }

    if (!actionReward__) {
      MultiDimFunctionGraph< double >* reward = instantiateFunctionGraph__();
      reward->setTableName("REWARD");
      fmdp__->addReward(reward);
      rewardLearner__ = instantiateRewardLearner__(reward, mainVariables);
    }
  }

  // ###################################################################
  //
  // ###################################################################
  template < TESTNAME    VariableAttributeSelection,
             TESTNAME    RewardAttributeSelection,
             LEARNERNAME LearnerSelection >
  bool FMDPLearner< VariableAttributeSelection,
                    RewardAttributeSelection,
                    LearnerSelection >::addObservation(Idx                actionId,
                                                       const Observation* newObs) {
    for (SequenceIteratorSafe< const DiscreteVariable* > varIter =
            fmdp__->beginVariables();
         varIter != fmdp__->endVariables();
         ++varIter) {
      actionLearners__[actionId]
         ->getWithDefault(*varIter, nullptr)
         ->addObservation(newObs);
      actionLearners__[actionId]->getWithDefault(*varIter, nullptr)->updateGraph();
    }

    if (actionReward__) {
      actionRewardLearners__[actionId]->addObservation(newObs);
      actionRewardLearners__[actionId]->updateGraph();
    } else {
      rewardLearner__->addObservation(newObs);
      rewardLearner__->updateGraph();
    }

    rmax__ =
       rmax__ < std::abs(newObs->reward()) ? std::abs(newObs->reward()) : rmax__;

    return false;
  }

  // ###################################################################
  //
  // ###################################################################
  template < TESTNAME    VariableAttributeSelection,
             TESTNAME    RewardAttributeSelection,
             LEARNERNAME LearnerSelection >
  Size FMDPLearner< VariableAttributeSelection,
                    RewardAttributeSelection,
                    LearnerSelection >::size() {
    Size s = 0;
    for (SequenceIteratorSafe< Idx > actionIter = fmdp__->beginActions();
         actionIter != fmdp__->endActions();
         ++actionIter) {
      for (SequenceIteratorSafe< const DiscreteVariable* > varIter =
              fmdp__->beginVariables();
           varIter != fmdp__->endVariables();
           ++varIter)
        s += actionLearners__[*actionIter]
                ->getWithDefault(*varIter, nullptr)
                ->size();
      if (actionReward__) s += actionRewardLearners__[*actionIter]->size();
    }

    if (!actionReward__) s += rewardLearner__->size();

    return s;
  }


  // ###################################################################
  //
  // ###################################################################
  template < TESTNAME    VariableAttributeSelection,
             TESTNAME    RewardAttributeSelection,
             LEARNERNAME LearnerSelection >
  void FMDPLearner< VariableAttributeSelection,
                    RewardAttributeSelection,
                    LearnerSelection >::updateFMDP() {
    for (SequenceIteratorSafe< Idx > actionIter = fmdp__->beginActions();
         actionIter != fmdp__->endActions();
         ++actionIter) {
      for (SequenceIteratorSafe< const DiscreteVariable* > varIter =
              fmdp__->beginVariables();
           varIter != fmdp__->endVariables();
           ++varIter)
        actionLearners__[*actionIter]
           ->getWithDefault(*varIter, nullptr)
           ->updateFunctionGraph();
      if (actionReward__)
        actionRewardLearners__[*actionIter]->updateFunctionGraph();
    }

    if (!actionReward__) rewardLearner__->updateFunctionGraph();
  }
}   // End of namespace gum
