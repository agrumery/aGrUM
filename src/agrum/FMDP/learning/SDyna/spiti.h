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
 * @brief Headers of the SPITI class.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */

// =========================================================================
#ifndef GUM_SPITI_H
#define GUM_SPITI_H
// =========================================================================
#include <agrum/FMDP/planning/svi.h>
#include <agrum/FMDP/learning/FMDPLearner.h>
#include <agrum/FMDP/learning/core/templateStrategy.h>
#include <agrum/FMDP/learning/SDyna/sdyna.h>
// =========================================================================
#include <agrum/multidim/instantiation.h>
#include <agrum/multidim/multiDimDecisionGraph.h>
// =========================================================================
#include <agrum/variables/discreteVariable.h>
// =========================================================================

namespace gum {

  /**
   * @class SPITI SPITI.h <agrum/FMDP/learning/SPITI.h>
   * @brief
   * @ingroup fmdp_group
   *
   *
   *
   */
  class SPITI : public SDYNA {

    public:

      // ###################################################################
      /// @name Constructor & destructor.
      // ###################################################################
      /// @{
    public:


        // ==========================================================================
        /**
         * Constructor
         * @param observationPhaseLenght : see SDYNA
         * @param nbValueIterationStep : see SDYNA
         * @param discountFactor : the \gamma used for the plannings
         * @param epsilon : the epsilon under which we consider V to be \epsilon-optimal
         * @param learningThreshold : threshold under which a variable is not install
         * on a node
         * @param similarityThreshold : threshold above which two leaves won't be merged
         * @param exploThreshold : threshold under which we explore another action than
         * the optimal one
         */
        // ==========================================================================
        SPITI (Idx observationPhaseLenght = 100,
                 Idx nbValueIterationStep = 10,
                 double discountFactor = 0.9,
                 double epsilon = 0.0001,
                 double learningThreshold = 0.95,
                 double similarityThreshold = 0.75,
                 double exploThreshold = 0.1);

        // ==========================================================================
        /// Destructor
        // ==========================================================================
        ~SPITI ();

      /// @}

      // ###################################################################
      /// @name Problem specification methods
      // ###################################################################
      /// @{
    public:

        // ==========================================================================
        /**
         * Inserts a new action in the SPITI.
         * @warning Without effect until method initialize is called
         * @param actionId : an id to identify the action
         * @param actionName : its humanly understandable name
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
        void addVariable( const DiscreteVariable* var );

      /// @}

      // ###################################################################
      /// @name Initialization
      // ###################################################################
      /// @{
    public:

        // ==========================================================================
        /**
         * Initializes the Sdyna instance internal satastructure; i.e. the planer and
         * the learner.
         */
        // ==========================================================================
        void initialize();

      /// @}


      // ###################################################################
      /// @name Incremental methods
      // ###################################################################
      /// @{

    protected :
        // ==========================================================================
        /**
         * @param the state in which we currently are
         * @return a set containing every optimal actions on that state
         */
        // ==========================================================================
        ActionSet _stateActionSet(const Instantiation&);

        // ==========================================================================
        /**
         * Gives to the learner a new transition
         * @param actionId : the action on which the transition was made
         * @param obs : the observed transition
         * @return true if learning this transition implies structural changes
         * (can trigger a new planning)
         */
        // ==========================================================================
        bool _learnTransition( Idx actionId, const Observation* obs );


        // ==========================================================================
        /**
         * Starts a new planning
         * @param Idx : the maximal number of value iteration performed in this planning
         */
        // ==========================================================================
        void _makePlanning( Idx );

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
        virtual Size learnerSize() {return __learner->size();}

        // ==========================================================================
        /**
         * @brief valueFunctionSize
         * @return
         */
        // ==========================================================================
      virtual Size valueFunctionSize() {return __planer->vFunction()->realSize();}

        // ==========================================================================
        /**
         * @brief optimalPolicySize
         * @return
         */
        // ==========================================================================
        virtual Size optimalPolicySize() {return __planer->optimalPolicy()->realSize();}

      /// @}


    private :

      /// The model learner.
      /// It will handle by itself the different decision graph learners
      /// needed to learn the FMDP
      FMDPLearner<CHI2TEST, CHI2TEST, ITILEARNER>* __learner;

      /// The planer we use to find the optimal policy
      SVI<double>* __planer;

      /// Threshold under which we perform a random action instead of exploiting the optimal one
      double __exploThreshold;

      /// The action set to return on exploring
      ActionSet __explorething;

  };


} /* namespace gum */


#endif // GUM_SPITI_H

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;

