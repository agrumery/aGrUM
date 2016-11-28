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
#include <agrum/FMDP/learning/fmdpLearner.h>
// =========================================================================

namespace gum {

  // ==========================================================================
  // Constructor & destructor.
  // ==========================================================================

  // ###################################################################
  // Default constructor
  // ###################################################################
  template <TESTNAME    VariableAttributeSelection,
            TESTNAME    RewardAttributeSelection,
            LEARNERNAME LearnerSelection>
  FMDPLearner<VariableAttributeSelection,
              RewardAttributeSelection,
              LearnerSelection>::FMDPLearner( double lT,
                                              bool   actionReward,
                                              double sT )
      : __actionReward( actionReward )
      , __learningThreshold( lT )
      , __similarityThreshold( sT ) {
    GUM_CONSTRUCTOR( FMDPLearner );
    __rewardLearner = nullptr;
  }


  // ###################################################################
  // Default destructor
  // ###################################################################
  template <TESTNAME    VariableAttributeSelection,
            TESTNAME    RewardAttributeSelection,
            LEARNERNAME LearnerSelection>
  FMDPLearner<VariableAttributeSelection,
              RewardAttributeSelection,
              LearnerSelection>::~FMDPLearner() {

    for ( auto actionIter = __actionLearners.beginSafe();
          actionIter != __actionLearners.endSafe();
          ++actionIter ) {
      for ( auto learnerIter = actionIter.val()->beginSafe();
            learnerIter != actionIter.val()->endSafe();
            ++learnerIter )
        delete learnerIter.val();
      delete actionIter.val();
      if ( __actionRewardLearners.exists( actionIter.key() ) )
        delete __actionRewardLearners[actionIter.key()];
    }

    if ( __rewardLearner ) delete __rewardLearner;

    GUM_DESTRUCTOR( FMDPLearner );
  }


  // ==========================================================================
  //
  // ==========================================================================

  // ###################################################################
  //
  // ###################################################################
  template <TESTNAME    VariableAttributeSelection,
            TESTNAME    RewardAttributeSelection,
            LEARNERNAME LearnerSelection>
  void                  FMDPLearner<VariableAttributeSelection,
                   RewardAttributeSelection,
                   LearnerSelection>::initialize( FMDP<double>* fmdp ) {

    __fmdp = fmdp;

    __modaMax = 0;
    __rmax = 0.0;

    Set<const DiscreteVariable*> mainVariables;
    for ( auto varIter = __fmdp->beginVariables();
          varIter != __fmdp->endVariables();
          ++varIter ) {
      mainVariables.insert( *varIter );
      __modaMax = __modaMax < ( *varIter )->domainSize()
                      ? ( *varIter )->domainSize()
                      : __modaMax;
    }

    for ( auto actionIter = __fmdp->beginActions();
          actionIter != __fmdp->endActions();
          ++actionIter ) {

      // Adding a Hashtable for the action
      __actionLearners.insert( *actionIter, new VarLearnerTable() );

      // Adding a learner for each variable
      for ( auto varIter = __fmdp->beginVariables();
            varIter != __fmdp->endVariables();
            ++varIter ) {
        MultiDimFunctionGraph<double>* varTrans = __instantiateFunctionGraph();
        varTrans->setTableName( "ACTION : " + __fmdp->actionName( *actionIter ) +
                                " - VARIABLE : " + ( *varIter )->name() );
        __fmdp->addTransitionForAction( *actionIter, *varIter, varTrans );
        __actionLearners[*actionIter]->insert(
            ( *varIter ),
            __instantiateVarLearner(
                varTrans, mainVariables, __fmdp->main2prime( *varIter ) ) );
      }

      if ( __actionReward ) {
        MultiDimFunctionGraph<double>* reward = __instantiateFunctionGraph();
        reward->setTableName( "REWARD - ACTION : " +
                              __fmdp->actionName( *actionIter ) );
        __fmdp->addRewardForAction( *actionIter, reward );
        __actionRewardLearners.insert(
            *actionIter, __instantiateRewardLearner( reward, mainVariables ) );
      }
    }

    if ( !__actionReward ) {
      MultiDimFunctionGraph<double>* reward = __instantiateFunctionGraph();
      reward->setTableName( "REWARD" );
      __fmdp->addReward( reward );
      __rewardLearner = __instantiateRewardLearner( reward, mainVariables );
    }
  }

  // ###################################################################
  //
  // ###################################################################
  template <TESTNAME    VariableAttributeSelection,
            TESTNAME    RewardAttributeSelection,
            LEARNERNAME LearnerSelection>
  bool                  FMDPLearner<VariableAttributeSelection,
                   RewardAttributeSelection,
                   LearnerSelection>::addObservation( Idx                actionId,
                                                      const Observation* newObs ) {

    for ( SequenceIteratorSafe<const DiscreteVariable*> varIter =
              __fmdp->beginVariables();
          varIter != __fmdp->endVariables();
          ++varIter ) {
      __actionLearners[actionId]
          ->getWithDefault( *varIter, nullptr )
          ->addObservation( newObs );
      __actionLearners[actionId]
          ->getWithDefault( *varIter, nullptr )
          ->updateGraph();
    }

    if ( __actionReward ) {
      __actionRewardLearners[actionId]->addObservation( newObs );
      __actionRewardLearners[actionId]->updateGraph();
    } else {
      __rewardLearner->addObservation( newObs );
      __rewardLearner->updateGraph();
    }

    __rmax = __rmax < std::abs( newObs->reward() ) ? std::abs( newObs->reward() )
                                                   : __rmax;

    return false;
  }

  // ###################################################################
  //
  // ###################################################################
  template <TESTNAME    VariableAttributeSelection,
            TESTNAME    RewardAttributeSelection,
            LEARNERNAME LearnerSelection>
  Size                  FMDPLearner<VariableAttributeSelection,
                   RewardAttributeSelection,
                   LearnerSelection>::size() {

    Size s = 0;
    for ( SequenceIteratorSafe<Idx> actionIter = __fmdp->beginActions();
          actionIter != __fmdp->endActions();
          ++actionIter ) {
      for ( SequenceIteratorSafe<const DiscreteVariable*> varIter =
                __fmdp->beginVariables();
            varIter != __fmdp->endVariables();
            ++varIter )
        s += __actionLearners[*actionIter]
                 ->getWithDefault( *varIter, nullptr )
                 ->size();
      if ( __actionReward ) s += __actionRewardLearners[*actionIter]->size();
    }

    if ( !__actionReward ) s += __rewardLearner->size();

    return s;
  }


  // ###################################################################
  //
  // ###################################################################
  template <TESTNAME    VariableAttributeSelection,
            TESTNAME    RewardAttributeSelection,
            LEARNERNAME LearnerSelection>
  void                  FMDPLearner<VariableAttributeSelection,
                   RewardAttributeSelection,
                   LearnerSelection>::updateFMDP() {

    for ( SequenceIteratorSafe<Idx> actionIter = __fmdp->beginActions();
          actionIter != __fmdp->endActions();
          ++actionIter ) {
      for ( SequenceIteratorSafe<const DiscreteVariable*> varIter =
                __fmdp->beginVariables();
            varIter != __fmdp->endVariables();
            ++varIter )
        __actionLearners[*actionIter]
            ->getWithDefault( *varIter, nullptr )
            ->updateFunctionGraph();
      if ( __actionReward )
        __actionRewardLearners[*actionIter]->updateFunctionGraph();
    }

    if ( !__actionReward ) __rewardLearner->updateFunctionGraph();
  }
}  // End of namespace gum
