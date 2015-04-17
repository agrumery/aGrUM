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
 * @brief Template Implementations of the FMDPLearner class.
 *
 * @author Jean-Christophe MAGNAN
 */

// =========================================================================
#include <agrum/FMDP/learning/FMDPLearner.h>
// =========================================================================

namespace gum {

  // ==========================================================================
  // Constructor & destructor.
  // ==========================================================================

    // ###################################################################
    // Default constructor
    // ###################################################################
    template <TESTNAME VariableAttributeSelection, TESTNAME RewardAttributeSelection, LEARNERNAME LearnerSelection>
    FMDPLearner<VariableAttributeSelection, RewardAttributeSelection, LearnerSelection>::FMDPLearner(
        FMDP<double> *target,
        double lT ,
        double sT) : __fmdp(target),
                     __learningThreshold(lT),
                     __similarityThreshold(sT){
      GUM_CONSTRUCTOR(FMDPLearner);
    }


    // ###################################################################
    // Default destructor
    // ###################################################################
    template <TESTNAME VariableAttributeSelection, TESTNAME RewardAttributeSelection, LEARNERNAME LearnerSelection>
    FMDPLearner<VariableAttributeSelection, RewardAttributeSelection, LearnerSelection>::~FMDPLearner(){

      for( auto actionIter = __actionLearners.beginSafe(); actionIter != __actionLearners.endSafe(); ++actionIter ){
          for( auto learnerIter = actionIter.val()->beginSafe(); learnerIter != actionIter.val()->endSafe(); ++learnerIter)
              delete learnerIter.val();
          delete actionIter.val();
      }

      delete __rewardLearner;

      GUM_DESTRUCTOR(FMDPLearner);
    }



  // ==========================================================================
  //
  // ==========================================================================

    // ###################################################################
    //
    // ###################################################################
    template <TESTNAME VariableAttributeSelection, TESTNAME RewardAttributeSelection, LEARNERNAME LearnerSelection>
    void FMDPLearner<VariableAttributeSelection, RewardAttributeSelection, LearnerSelection>::addVariable( const DiscreteVariable* var ){
        __fmdp->addVariable(var);
        __mainVariables.insert(var);
    }

    // ###################################################################
    //
    // ###################################################################
    template <TESTNAME VariableAttributeSelection, TESTNAME RewardAttributeSelection, LEARNERNAME LearnerSelection>
    void FMDPLearner<VariableAttributeSelection, RewardAttributeSelection, LearnerSelection>::addAction(const Idx actionId, const std::string &actionName ){
        __fmdp->addAction(actionId, actionName);
        __actionLearners.insert( actionId, new HashTable<const DiscreteVariable*, IncrementalGraphLearner<VariableAttributeSelection, false>*>());
    }



  // ==========================================================================
  //
  // ==========================================================================

    // ###################################################################
    //
    // ###################################################################
    template <TESTNAME VariableAttributeSelection, TESTNAME RewardAttributeSelection, LEARNERNAME LearnerSelection>
    void FMDPLearner<VariableAttributeSelection, RewardAttributeSelection, LearnerSelection>::initialize(){

      for( auto actionIter = __fmdp->beginActions(); actionIter != __fmdp->endActions(); ++actionIter ){
        for( auto varIter = __fmdp->beginVariables(); varIter != __fmdp->endVariables(); ++varIter ){
            MultiDimFunctionGraph<double>* varTrans = new MultiDimFunctionGraph<double>();
            varTrans->setTableName("ACTION : " + __fmdp->actionName(*actionIter) + " - VARIABLE : " + (*varIter)->name());
            __fmdp->addTransitionForAction(__fmdp->actionName(*actionIter), *varIter, varTrans);
            __actionLearners[*actionIter]->insert( (*varIter), new VariableLearnerType<false>(varTrans,__learningThreshold, __similarityThreshold, __mainVariables,__fmdp->main2prime(*varIter)));
        }
      }

      MultiDimFunctionGraph<double>* reward = new MultiDimFunctionGraph<double>();
      reward->setTableName("REWARD");
      __fmdp->addReward(reward);
      __rewardLearner = new RewardLearnerType<true>(reward, __learningThreshold, __similarityThreshold, __mainVariables);

    }

    // ###################################################################
    //
    // ###################################################################
    template <TESTNAME VariableAttributeSelection, TESTNAME RewardAttributeSelection, LEARNERNAME LearnerSelection>
    void FMDPLearner<VariableAttributeSelection, RewardAttributeSelection, LearnerSelection>::addObservation( Idx actionId, const Observation* newObs ){

        for(SequenceIteratorSafe<const DiscreteVariable*> varIter = __fmdp->beginVariables(); varIter != __fmdp->endVariables(); ++varIter){
            __actionLearners[actionId]->getWithDefault(*varIter, nullptr)->addObservation(newObs);
            __actionLearners[actionId]->getWithDefault(*varIter, nullptr)->updateGraph();
        }

        __rewardLearner->addObservation(newObs);
        __rewardLearner->updateGraph();
    }

    // ###################################################################
    //
    // ###################################################################
    template <TESTNAME VariableAttributeSelection, TESTNAME RewardAttributeSelection, LEARNERNAME LearnerSelection>
    Size FMDPLearner<VariableAttributeSelection, RewardAttributeSelection, LearnerSelection>::size( ){

        Size s = 0;
        for( SequenceIteratorSafe<Idx> actionIter = __fmdp->beginActions(); actionIter != __fmdp->endActions(); ++actionIter )
            for( SequenceIteratorSafe<const DiscreteVariable*> varIter = __fmdp->beginVariables(); varIter != __fmdp->endVariables(); ++varIter)
                s+= __actionLearners[*actionIter]->getWithDefault(*varIter, nullptr)->size();

        s += __rewardLearner->size();

        return s;
    }



    // ###################################################################
    //
    // ###################################################################
    template <TESTNAME VariableAttributeSelection, TESTNAME RewardAttributeSelection, LEARNERNAME LearnerSelection>
    void FMDPLearner<VariableAttributeSelection, RewardAttributeSelection, LearnerSelection>::updateFMDP(){

      for( SequenceIteratorSafe<Idx> actionIter = __fmdp->beginActions(); actionIter != __fmdp->endActions(); ++actionIter )
          for( SequenceIteratorSafe<const DiscreteVariable*> varIter = __fmdp->beginVariables(); varIter != __fmdp->endVariables(); ++varIter)
            __actionLearners[*actionIter]->getWithDefault(*varIter, nullptr)->updateFunctionGraph();
      __rewardLearner->updateFunctionGraph();
    }
} // End of namespace gum
