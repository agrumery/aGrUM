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
#include <agrum/FMDP/FMDP.h>
#include <agrum/FMDP/learning/observation.h>
#include <agrum/FMDP/SDyna/Strategies/ILearningStrategy.h>
#include <agrum/FMDP/SDyna/Strategies/IPlanningStrategy.h>
#include <agrum/FMDP/SDyna/Strategies/IDecisionStrategy.h>
#include <agrum/FMDP/planning/actionSet.h>
// =========================================================================
#include <agrum/multidim/instantiation.h>
// =========================================================================
#include <agrum/variables/discreteVariable.h>
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
      /// @name Constructor & destructor.
      // ###################################################################
      /// @{    
    public:

        // ==========================================================================
        /**
         * Constructor
         *
         * @param observationPhaseLenght : the number of observation done before a
         * replanning is launch. If equals 0, a planning is done after each structural
         * change.
         * @param nbValueIterationStep : the number of value iteration done during
         * one planning
         * @return an instance of SDyna architecture
         */
        // ==========================================================================
        SDYNA (Idx observationPhaseLenght = 100,
               Idx nbValueIterationStep = 10,
               double discountFactor = 0.9);

        // ==========================================================================
        /// Destructor
        // ==========================================================================
        ~SDYNA ();

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
        void addAction(const Idx actionId, const std::string &actionName );

        // ==========================================================================
        /**
         * Inserts a new variable in the SDyna instance.
         * @warning Without effect until method initialize is called
         * @param var : the var to be added.
         * Note that variable may or may not have all its modalities given.
         * If not they will be discovered by the SDyna architecture during the process
         */
        // ==========================================================================
        void addVariable( const DiscreteVariable* var ){ __learner->addVariable(var); }

      /// @}


      // ###################################################################
      /// @name Initialization
      // ###################################################################
      /// @{
    public:

        // ==========================================================================
        /**
         * Initializes the Sdyna instance.
         * @param initialState : the state of the studied system from which we will
         * begin the explore, learn and exploit process
         */
        // ==========================================================================
        void initialize(const Instantiation& initialState);

      /// @}


      // ###################################################################
      /// @name Incremental methods
      // ###################################################################
      /// @{
    public:

        // ==========================================================================
        /**
         * Sets last state visited to the given state.
         * During the learning process, we will consider that were in this state before
         * the transition.
         * @param currentState : the state
         */
        // ==========================================================================
        void setCurrentState( const Instantiation&  currentState ) { _lastState = currentState; }

        // ==========================================================================
        /**
         * @return the id of the action the SDyna instance wish to be performed
         * @param the state in which we currently are
         */
        // ==========================================================================
        Idx takeAction( const Instantiation& );

        // ==========================================================================
        /**
         * @return the id of the action the SDyna instance wish to be performed
         */
        // ==========================================================================
        Idx takeAction();

    protected :
        // ==========================================================================
        /**
         * @param the state in which we currently are
         * @return a set containing every optimal actions on that state
         */
        // ==========================================================================
        virtual ActionSet _stateActionSet(const Instantiation&) = 0;

    public :
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
        void feedback(const Instantiation & originalState, const Instantiation& reachedState, Idx performedAction, double obtainedReward);

        // ==========================================================================
        /**
         * Performs a feedback on the last transition.
         * In extenso, learn from the transition.
         * @param reachedState : the state reached after the transition
         * @param obtainedReward : the reward obtained during the transition
         * @warning Uses the __originalState and __performedAction stored in cache
         * If you want to specify the original state and the performed action, see below
         */
        // ==========================================================================
        void feedback(const Instantiation & reachedState, double obtainedReward);

        // ==========================================================================
        /**
         * Starts a new planning
         * @param Idx : the maximal number of value iteration performed in this planning
         */
        // ==========================================================================
        void makePlanning( Idx );

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
      virtual std::string toString();

      virtual std::string optimalPolicy2String(){ return __planer->optimalPolicy2String(); }


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
        Size learnerSize() {return __learner->size(); }

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
        virtual Size valueFunctionSize() {return __planer->vFunctionSize();}

        // ==========================================================================
        /**
         * @brief optimalPolicySize
         * @return
         */
        // ==========================================================================
        virtual Size optimalPolicySize() {return __planer->optimalPolicySize();}

      /// @}


  protected :
      /// The learnt Markovian Decision Process
      FMDP<double>* _fmdp;

      /// The state in which the system is before we perform a new action
      Instantiation _lastState;

  private :
      /// The number of observation we make before using again the planer
      Idx __observationPhaseLenght;

      /// The total number of observation made so far
      Idx __nbObservation;

      /// The number of Value Iteration step we perform
      Idx __nbValueIterationStep;

      Sequence<Idx> __actionSeq;

      /// The last performed action
      Idx __lastAction;

      /// Since SDYNA made these observation, it has to delete them on quitting
      Set<Observation*> __bin;

      /// The learner used to learn the FMDP
      ILearningStrategy* __learner;

      /// The planer used to plan an optimal strategy
      IPlanningStrategy* __planer;

      /// The decider
      IDecisionStrategy* __decider;
  };


} /* namespace gum */


#endif // GUM_SDYNA_H

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;

//      /**
//      /** * @brief extractCount
//      /** * @param actionId
//      /** * @param var
//      /** * @return
//      /** */
//      MultiDimFunctionGraph<double>* extractCount(Idx actionId, const DiscreteVariable* var){
//        return __learner->extractCount(actionId, var); }

//      double rMax(){ return __rmax; }
//        double __rmax;
//          __planer = new RMaxMDD<double>(__fmdp,this, 40);
//          __rmax = std::numeric_limits<double>::min();

//if(reward > __rmax)
//  __rmax = reward;

