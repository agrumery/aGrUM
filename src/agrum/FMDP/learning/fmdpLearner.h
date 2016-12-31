/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/**
 * @file
 * @brief Headers of the FMDPLearner class.
 *
 * @author Jean-Christophe MAGNAN
 */

// =========================================================================
#ifndef GUM_FMDP_LEARNER_H
#define GUM_FMDP_LEARNER_H
// =========================================================================
#include <agrum/core/hashTable.h>
// =========================================================================
#include <agrum/FMDP/SDyna/Strategies/ILearningStrategy.h>
#include <agrum/FMDP/fmdp.h>
#include <agrum/FMDP/learning/datastructure/imddi.h>
#include <agrum/FMDP/learning/datastructure/iti.h>
#include <agrum/FMDP/learning/observation.h>
// =========================================================================
#include <agrum/variables/discreteVariable.h>
// =========================================================================

namespace gum {

  /**
   * @class FMDPLearner fmdpLearner.h <agrum/FMDP/learning/fmdpLearner.h>
   * @brief
   * @ingroup fmdp_group
   *
   *
   *
   */

  template <TESTNAME    VariableAttributeSelection,
            TESTNAME    RewardAttributeSelection,
            LEARNERNAME LearnerSelection>
  class FMDPLearner : public ILearningStrategy {

    typedef typename LearnerSelect<LearnerSelection,
                                   IMDDI<VariableAttributeSelection, false>,
                                   ITI<VariableAttributeSelection, false>>::type
        VariableLearnerType;

    typedef typename LearnerSelect<LearnerSelection,
                                   IMDDI<RewardAttributeSelection, true>,
                                   ITI<RewardAttributeSelection, true>>::type
        RewardLearnerType;

    typedef HashTable<const DiscreteVariable*, VariableLearnerType*>
        VarLearnerTable;

    public:
    // ==========================================================================
    /// @name Constructor & destructor.
    // ==========================================================================
    /// @{

    // ###################################################################
    /// Default constructor
    // ###################################################################
    FMDPLearner( double learningThreshold,
                 bool   actionReward,
                 double similarityThreshold = 0.05 );

    // ###################################################################
    /// Default destructor
    // ###################################################################
    ~FMDPLearner();

    /// @}

    // ###################################################################
    /// @name Initialization
    // ###################################################################
    /// @{
    public:
    // ==========================================================================
    /// Initializes the learner
    // ==========================================================================
    void initialize( FMDP<double>* fmdp );

    // ==========================================================================
    ///
    // ==========================================================================
    MultiDimFunctionGraph<double>* __instantiateFunctionGraph() {
      return __instantiateFunctionGraph( Int2Type<LearnerSelection>() );
    }

    MultiDimFunctionGraph<double>*
        __instantiateFunctionGraph( Int2Type<IMDDILEARNER> ) {
      return MultiDimFunctionGraph<double, ExactTerminalNodePolicy>::
          getReducedAndOrderedInstance();
    }

    MultiDimFunctionGraph<double>*
        __instantiateFunctionGraph( Int2Type<ITILEARNER> ) {
      return MultiDimFunctionGraph<double,
                                   ExactTerminalNodePolicy>::getTreeInstance();
    }


    // ==========================================================================
    ///
    // ==========================================================================
    VariableLearnerType*
    __instantiateVarLearner( MultiDimFunctionGraph<double>* target,
                             Set<const DiscreteVariable*>&  mainVariables,
                             const DiscreteVariable*        learnedVar ) {
      return __instantiateVarLearner(
          target, mainVariables, learnedVar, Int2Type<LearnerSelection>() );
    }

    VariableLearnerType*
    __instantiateVarLearner( MultiDimFunctionGraph<double>* target,
                             Set<const DiscreteVariable*>&  mainVariables,
                             const DiscreteVariable*        learnedVar,
                             Int2Type<IMDDILEARNER> ) {
      return new VariableLearnerType( target,
                                      __learningThreshold,
                                      __similarityThreshold,
                                      mainVariables,
                                      learnedVar );
    }

    VariableLearnerType*
    __instantiateVarLearner( MultiDimFunctionGraph<double>* target,
                             Set<const DiscreteVariable*>&  mainVariables,
                             const DiscreteVariable*        learnedVar,
                             Int2Type<ITILEARNER> ) {
      return new VariableLearnerType(
          target, __learningThreshold, mainVariables, learnedVar );
    }


    // ==========================================================================
    ///
    // ==========================================================================
    RewardLearnerType*
    __instantiateRewardLearner( MultiDimFunctionGraph<double>* target,
                                Set<const DiscreteVariable*>&  mainVariables ) {
      return __instantiateRewardLearner(
          target, mainVariables, Int2Type<LearnerSelection>() );
    }

    RewardLearnerType*
    __instantiateRewardLearner( MultiDimFunctionGraph<double>* target,
                                Set<const DiscreteVariable*>&  mainVariables,
                                Int2Type<IMDDILEARNER> ) {
      return new RewardLearnerType(
          target, __learningThreshold, __similarityThreshold, mainVariables );
    }

    RewardLearnerType*
    __instantiateRewardLearner( MultiDimFunctionGraph<double>* target,
                                Set<const DiscreteVariable*>&  mainVariables,
                                Int2Type<ITILEARNER> ) {
      return new RewardLearnerType( target, __learningThreshold, mainVariables );
    }

    /// @}


    // ###################################################################
    /// @name Incremental methods
    // ###################################################################
    /// @{
    public:
    // ==========================================================================
    /**
     * Gives to the learner a new transition
     * @param actionId : the action on which the transition was made
     * @param obs : the observed transition
     * @return true if learning this transition implies structural changes
     * (can trigger a new planning)
     */
    // ==========================================================================
    bool addObservation( Idx actionId, const Observation* obs );


    // ==========================================================================
    /**
     * Starts an update of datastructure in the associated FMDP
     */
    // ==========================================================================
    void updateFMDP();

    /// @}


    // ###################################################################
    /// @name Miscelleanous methods
    // ###################################################################
    /// @{
    public:
    // ==========================================================================
    /**
     * @brief learnerSize
     * @return
     */
    // ==========================================================================
    Size size();

    // ==========================================================================
    /// \brief extractCount
    // ==========================================================================
    const IVisitableGraphLearner* varLearner( Idx                     actionId,
                                              const DiscreteVariable* var ) const {
      return __actionLearners[actionId]->getWithDefault( var, nullptr );
    }

    virtual double rMax() const { return __rmax; }

    private:
    double __rmax;

    public:
    virtual double modaMax() const { return __modaMax; }

    private:
    double __modaMax;

    /// @}


    private:
    /// The FMDP to store the learned model
    FMDP<double>* __fmdp;

    HashTable<Idx, VarLearnerTable*> __actionLearners;

    bool __actionReward;
    HashTable<Idx, RewardLearnerType*> __actionRewardLearners;
    RewardLearnerType* __rewardLearner;

    const double __learningThreshold;
    const double __similarityThreshold;
  };


} /* namespace gum */

#include <agrum/FMDP/learning/fmdpLearner_tpl.h>

#endif  // GUM_FMDP_LEARNER_H

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
