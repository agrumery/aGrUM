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
 * @brief Headers of the SDyna abstract class.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */

// =========================================================================
#ifndef GUM_SDYNA_H
#define GUM_SDYNA_H
// =========================================================================
#include <agrum/config.h>
#include <agrum/multidim/instantiation.h>
// =========================================================================
#include <agrum/variables/discreteVariable.h>
// =========================================================================
#include <agrum/FMDP/SDyna/Strategies/IDecisionStrategy.h>
#include <agrum/FMDP/SDyna/Strategies/ILearningStrategy.h>
#include <agrum/FMDP/SDyna/Strategies/IPlanningStrategy.h>
#include <agrum/FMDP/decision/E_GreedyDecider.h>
#include <agrum/FMDP/decision/lazyDecider.h>
#include <agrum/FMDP/decision/randomDecider.h>
#include <agrum/FMDP/decision/statisticalLazyDecider.h>
#include <agrum/FMDP/fmdp.h>
#include <agrum/FMDP/learning/fmdpLearner.h>
#include <agrum/FMDP/learning/observation.h>
#include <agrum/FMDP/planning/actionSet.h>
#include <agrum/FMDP/planning/adaptiveRMaxPlaner.h>
#include <agrum/FMDP/planning/structuredPlaner.h>
// =========================================================================

namespace gum {

  /**
   * @class sdyna sdyna.h <agrum/FMDP/SDyna/sdyna.h>
   * @brief
   * @ingroup fmdp_group
   *
   * The general SDyna architecture abstract class.
   * Instance of SDyna architecture should inherit
   *
   */

  class SDYNA {

    // ###################################################################
    /// @name
    // ###################################################################
    /// @
    public:
    // ==========================================================================
    ///
    // ==========================================================================
    static SDYNA* spitiInstance( double attributeSelectionThreshold = 0.99,
                                 double discountFactor = 0.9,
                                 double epsilon = 1,
                                 Idx    observationPhaseLenght = 100,
                                 Idx    nbValueIterationStep = 10 ) {
      bool               actionReward = false;
      ILearningStrategy* ls = new FMDPLearner<CHI2TEST, CHI2TEST, ITILEARNER>(
          attributeSelectionThreshold, actionReward );
      IPlanningStrategy<double>* ps =
          StructuredPlaner<double>::sviInstance( discountFactor, epsilon );
      IDecisionStrategy* ds = new E_GreedyDecider();
      return new SDYNA(
          ls, ps, ds, observationPhaseLenght, nbValueIterationStep, actionReward );
    }

    // ==========================================================================
    ///
    // ==========================================================================
    static SDYNA* spimddiInstance( double attributeSelectionThreshold = 0.99,
                                   double similarityThreshold = 0.3,
                                   double discountFactor = 0.9,
                                   double epsilon = 1,
                                   Idx    observationPhaseLenght = 100,
                                   Idx    nbValueIterationStep = 10 ) {
      bool               actionReward = false;
      ILearningStrategy* ls = new FMDPLearner<GTEST, GTEST, IMDDILEARNER>(
          attributeSelectionThreshold, actionReward, similarityThreshold );
      IPlanningStrategy<double>* ps = StructuredPlaner<double>::spumddInstance(
          discountFactor, epsilon, false );
      IDecisionStrategy* ds = new E_GreedyDecider();
      return new SDYNA( ls,
                        ps,
                        ds,
                        observationPhaseLenght,
                        nbValueIterationStep,
                        actionReward,
                        false );
    }

    // ==========================================================================
    ///
    // ==========================================================================
    static SDYNA* RMaxMDDInstance( double attributeSelectionThreshold = 0.99,
                                   double similarityThreshold = 0.3,
                                   double discountFactor = 0.9,
                                   double epsilon = 1,
                                   Idx    observationPhaseLenght = 100,
                                   Idx    nbValueIterationStep = 10 ) {
      bool               actionReward = true;
      ILearningStrategy* ls = new FMDPLearner<GTEST, GTEST, IMDDILEARNER>(
          attributeSelectionThreshold, actionReward, similarityThreshold );
      AdaptiveRMaxPlaner* rm = AdaptiveRMaxPlaner::ReducedAndOrderedInstance(
          ls, discountFactor, epsilon );
      IPlanningStrategy<double>* ps = rm;
      IDecisionStrategy*         ds = rm;
      return new SDYNA(
          ls, ps, ds, observationPhaseLenght, nbValueIterationStep, actionReward );
    }

    // ==========================================================================
    ///
    // ==========================================================================
    static SDYNA* RMaxTreeInstance( double attributeSelectionThreshold = 0.99,
                                    double discountFactor = 0.9,
                                    double epsilon = 1,
                                    Idx    observationPhaseLenght = 100,
                                    Idx    nbValueIterationStep = 10 ) {
      bool               actionReward = true;
      ILearningStrategy* ls = new FMDPLearner<GTEST, GTEST, ITILEARNER>(
          attributeSelectionThreshold, actionReward );
      AdaptiveRMaxPlaner* rm =
          AdaptiveRMaxPlaner::TreeInstance( ls, discountFactor, epsilon );
      IPlanningStrategy<double>* ps = rm;
      IDecisionStrategy*         ds = rm;
      return new SDYNA(
          ls, ps, ds, observationPhaseLenght, nbValueIterationStep, actionReward );
    }

    // ==========================================================================
    ///
    // ==========================================================================
    static SDYNA* RandomMDDInstance( double attributeSelectionThreshold = 0.99,
                                     double similarityThreshold = 0.3,
                                     double discountFactor = 0.9,
                                     double epsilon = 1,
                                     Idx    observationPhaseLenght = 100,
                                     Idx    nbValueIterationStep = 10 ) {
      bool               actionReward = true;
      ILearningStrategy* ls = new FMDPLearner<GTEST, GTEST, IMDDILEARNER>(
          attributeSelectionThreshold, actionReward, similarityThreshold );
      IPlanningStrategy<double>* ps =
          StructuredPlaner<double>::spumddInstance( discountFactor, epsilon );
      IDecisionStrategy* ds = new RandomDecider();
      return new SDYNA(
          ls, ps, ds, observationPhaseLenght, nbValueIterationStep, actionReward );
    }

    // ==========================================================================
    ///
    // ==========================================================================
    static SDYNA* RandomTreeInstance( double attributeSelectionThreshold = 0.99,
                                      double discountFactor = 0.9,
                                      double epsilon = 1,
                                      Idx    observationPhaseLenght = 100,
                                      Idx    nbValueIterationStep = 10 ) {
      bool               actionReward = true;
      ILearningStrategy* ls = new FMDPLearner<CHI2TEST, CHI2TEST, ITILEARNER>(
          attributeSelectionThreshold, actionReward );
      IPlanningStrategy<double>* ps =
          StructuredPlaner<double>::sviInstance( discountFactor, epsilon );
      IDecisionStrategy* ds = new RandomDecider();
      return new SDYNA(
          ls, ps, ds, observationPhaseLenght, nbValueIterationStep, actionReward );
    }


    /// @}

    // ###################################################################
    /// @name Constructor & destructor.
    // ###################################################################
    /// @{

    // ==========================================================================
    /**
     * Constructor
     *
     * @return an instance of SDyna architecture
     */
    // ==========================================================================
    private:
    SDYNA( ILearningStrategy*         learner,
           IPlanningStrategy<double>* planer,
           IDecisionStrategy*         decider,
           Idx                        observationPhaseLenght,
           Idx                        nbValueIterationStep,
           bool                       actionReward,
           bool                       verbose = true );

    // ==========================================================================
    /// Destructor
    // ==========================================================================
    public:
    ~SDYNA();

    /// @}


    // ###################################################################
    /// @name Problem specification methods
    // ###################################################################
    /// @{
    public:
    // ==========================================================================
    /**
     * Inserts a new action in the SDyna instance.
     * @warning Without effect until method initialize is called
     * @param actionId : an id to identify the action
     * @param actionName : its human name
     */
    // ==========================================================================
    void addAction( const Idx actionId, const std::string& actionName ) {
      _fmdp->addAction( actionId, actionName );
    }

    // ==========================================================================
    /**
     * Inserts a new variable in the SDyna instance.
     * @warning Without effect until method initialize is called
     * @param var : the var to be added.
     * Note that variable may or may not have all its modalities given.
     * If not they will be discovered by the SDyna architecture during the
     * process
     */
    // ==========================================================================
    void addVariable( const DiscreteVariable* var ) { _fmdp->addVariable( var ); }

    /// @}


    // ###################################################################
    /// @name Initialization
    // ###################################################################
    /// @{
    public:
    // ==========================================================================
    /**
     * Initializes the Sdyna instance.
     */
    // ==========================================================================
    void initialize();

    // ==========================================================================
    /**
     * Initializes the Sdyna instance at given state.
     * @param initialState : the state of the studied system from which we will
     * begin the explore, learn and exploit process
     */
    // ==========================================================================
    void initialize( const Instantiation& initialState );

    /// @}


    // ###################################################################
    /// @name Incremental methods
    // ###################################################################
    /// @{
    public:
    // ==========================================================================
    /**
     * Sets last state visited to the given state.
     * During the learning process, we will consider that were in this state
     * before the transition.
     * @param currentState : the state
     */
    // ==========================================================================
    void setCurrentState( const Instantiation& currentState ) {
      _lastState = currentState;
    }

    // ==========================================================================
    /**
     * @return actionId the id of the action the SDyna instance wish to be
     * performed
     * @param curState the state in which we currently are
     */
    // ==========================================================================
    Idx takeAction( const Instantiation& curState );

    // ==========================================================================
    /**
     * @return the id of the action the SDyna instance wish to be performed
     */
    // ==========================================================================
    Idx takeAction();

    // ==========================================================================
    /**
     * Performs a feedback on the last transition.
     * In extenso, learn from the transition.
     * @param originalState : the state we were in before the transition
     * @param reachedState : the state we reached after
     * @param performedAction : the action we performed
     * @param obtainedReward : the reward we obtained
     */
    // ==========================================================================
    void feedback( const Instantiation& originalState,
                   const Instantiation& reachedState,
                   Idx                  performedAction,
                   double               obtainedReward );

    // ==========================================================================
    /**
     * Performs a feedback on the last transition.
     * In extenso, learn from the transition.
     * @param reachedState : the state reached after the transition
     * @param obtainedReward : the reward obtained during the transition
     * @warning Uses the __originalState and __performedAction stored in cache
     * If you want to specify the original state and the performed action, see
     * below
     */
    // ==========================================================================
    void feedback( const Instantiation& reachedState, double obtainedReward );

    // ==========================================================================
    /**
     * Starts a new planning
     * @param nbStep : the maximal number of value iteration performed in this
     * planning
     */
    // ==========================================================================
    void makePlanning( Idx nbStep );

    /// @}


    public:
    // ==========================================================================
    /**
     * Returns
     * @return a string describing the learned FMDP, and the associated
     * optimal policy.
     * Both in DOT language.
     */
    // ==========================================================================
    std::string toString();

    std::string optimalPolicy2String() { return __planer->optimalPolicy2String(); }


    // ###################################################################
    /// @name Size methods
    /// @brief just to get the size of the different data structure for
    /// performance evaluation purposes only
    // ###################################################################
    /// @{
    public:
    // ==========================================================================
    /**
     * @brief learnerSize
     * @return
     */
    // ==========================================================================
    Size learnerSize() { return __learner->size(); }

    // ==========================================================================
    /**
     * @brief modelSize
     * @return
     */
    // ==========================================================================
    Size modelSize() { return _fmdp->size(); }

    // ==========================================================================
    /**
     * @brief valueFunctionSize
     * @return
     */
    // ==========================================================================
    Size valueFunctionSize() { return __planer->vFunctionSize(); }

    // ==========================================================================
    /**
     * @brief optimalPolicySize
     * @return
     */
    // ==========================================================================
    Size optimalPolicySize() { return __planer->optimalPolicySize(); }

    /// @}


    protected:
    /// The learnt Markovian Decision Process
    FMDP<double>* _fmdp;

    /// The state in which the system is before we perform a new action
    Instantiation _lastState;

    private:
    /// The learner used to learn the FMDP
    ILearningStrategy* __learner;

    /// The planer used to plan an optimal strategy
    IPlanningStrategy<double>* __planer;

    /// The decider
    IDecisionStrategy* __decider;


    /// The number of observation we make before using again the planer
    Idx __observationPhaseLenght;

    /// The total number of observation made so far
    Idx __nbObservation;

    /// The number of Value Iteration step we perform
    Idx __nbValueIterationStep;

    /// The last performed action
    Idx __lastAction;

    /// Since SDYNA made these observation, it has to delete them on quitting
    Set<Observation*> __bin;

    bool __actionReward;

    bool _verbose;
  };


} /* namespace gum */


#endif  // GUM_SDYNA_H

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
