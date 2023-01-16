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
 * @brief Headers of the FMDPLearner class.
 *
 * @author Jean-Christophe MAGNAN
 */

// =========================================================================
#ifndef GUM_FMDP_LEARNER_H
#define GUM_FMDP_LEARNER_H
// =========================================================================
// =========================================================================
#include <agrum/FMDP/SDyna/Strategies/ILearningStrategy.h>
#include <agrum/FMDP/learning/datastructure/imddi.h>
#include <agrum/FMDP/learning/datastructure/iti.h>
// =========================================================================
// =========================================================================

namespace gum {

  /**
   * @class FMDPLearner
   * @headerfile fmdpLearner.h <agrum/FMDP/learning/fmdpLearner.h>
   * @brief
   * @ingroup fmdp_group
   *
   *
   *
   */

  template < TESTNAME    VariableAttributeSelection,
             TESTNAME    RewardAttributeSelection,
             LEARNERNAME LearnerSelection >
  class FMDPLearner: public ILearningStrategy {
    using VariableLearnerType =
       typename LearnerSelect< LearnerSelection,
                               IMDDI< VariableAttributeSelection, false >,
                               ITI< VariableAttributeSelection, false > >::type;

    using RewardLearnerType = typename LearnerSelect< LearnerSelection,
                                                      IMDDI< RewardAttributeSelection, true >,
                                                      ITI< RewardAttributeSelection, true > >::type;

    using VarLearnerTable = HashTable< const DiscreteVariable*, VariableLearnerType* >;

    public:
    // ==========================================================================
    /// @name Constructor & destructor.
    // ==========================================================================
    /// @{

    // ###################################################################
    /// Default constructor
    // ###################################################################
    FMDPLearner(double learningThreshold, bool actionReward, double similarityThreshold = 0.05);

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
    void initialize(FMDP< double >* fmdp);

    // ==========================================================================
    ///
    // ==========================================================================
    MultiDimFunctionGraph< double >* _instantiateFunctionGraph_() {
      return _instantiateFunctionGraph_(Int2Type< LearnerSelection >());
    }

    MultiDimFunctionGraph< double >* _instantiateFunctionGraph_(Int2Type< IMDDILEARNER >) {
      return MultiDimFunctionGraph< double,
                                    ExactTerminalNodePolicy >::getReducedAndOrderedInstance();
    }

    MultiDimFunctionGraph< double >* _instantiateFunctionGraph_(Int2Type< ITILEARNER >) {
      return MultiDimFunctionGraph< double, ExactTerminalNodePolicy >::getTreeInstance();
    }


    // ==========================================================================
    ///
    // ==========================================================================
    VariableLearnerType* _instantiateVarLearner_(MultiDimFunctionGraph< double >* target,
                                                 Set< const DiscreteVariable* >&  mainVariables,
                                                 const DiscreteVariable*          learnedVar) {
      return _instantiateVarLearner_(target,
                                     mainVariables,
                                     learnedVar,
                                     Int2Type< LearnerSelection >());
    }

    VariableLearnerType* _instantiateVarLearner_(MultiDimFunctionGraph< double >* target,
                                                 Set< const DiscreteVariable* >&  mainVariables,
                                                 const DiscreteVariable*          learnedVar,
                                                 Int2Type< IMDDILEARNER >) {
      return new VariableLearnerType(target,
                                     _learningThreshold_,
                                     _similarityThreshold_,
                                     mainVariables,
                                     learnedVar);
    }

    VariableLearnerType* _instantiateVarLearner_(MultiDimFunctionGraph< double >* target,
                                                 Set< const DiscreteVariable* >&  mainVariables,
                                                 const DiscreteVariable*          learnedVar,
                                                 Int2Type< ITILEARNER >) {
      return new VariableLearnerType(target, _learningThreshold_, mainVariables, learnedVar);
    }


    // ==========================================================================
    ///
    // ==========================================================================
    RewardLearnerType* _instantiateRewardLearner_(MultiDimFunctionGraph< double >* target,
                                                  Set< const DiscreteVariable* >&  mainVariables) {
      return _instantiateRewardLearner_(target, mainVariables, Int2Type< LearnerSelection >());
    }

    RewardLearnerType* _instantiateRewardLearner_(MultiDimFunctionGraph< double >* target,
                                                  Set< const DiscreteVariable* >&  mainVariables,
                                                  Int2Type< IMDDILEARNER >) {
      return new RewardLearnerType(target,
                                   _learningThreshold_,
                                   _similarityThreshold_,
                                   mainVariables);
    }

    RewardLearnerType* _instantiateRewardLearner_(MultiDimFunctionGraph< double >* target,
                                                  Set< const DiscreteVariable* >&  mainVariables,
                                                  Int2Type< ITILEARNER >) {
      return new RewardLearnerType(target, _learningThreshold_, mainVariables);
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
    bool addObservation(Idx actionId, const Observation* obs);


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
    const IVisitableGraphLearner* varLearner(Idx actionId, const DiscreteVariable* var) const {
      return _actionLearners_[actionId]->getWithDefault(var, nullptr);
    }

    virtual double rMax() const { return _rmax_; }

    private:
    double _rmax_;

    public:
    virtual double modaMax() const { return _modaMax_; }

    private:
    double _modaMax_;

    /// @}


    private:
    /// The FMDP to store the learned model
    FMDP< double >* _fmdp_;

    HashTable< Idx, VarLearnerTable* > _actionLearners_;

    bool                                 _actionReward_;
    HashTable< Idx, RewardLearnerType* > _actionRewardLearners_;
    RewardLearnerType*                   _rewardLearner_;

    const double _learningThreshold_;
    const double _similarityThreshold_;
  };


} /* namespace gum */

#include <agrum/FMDP/learning/fmdpLearner_tpl.h>

#endif   // GUM_FMDP_LEARNER_H
