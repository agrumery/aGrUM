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
 * @brief Headers of the SPIMDDI class.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */

// =========================================================================
#ifndef GUM_SPIMDDI_H
#define GUM_SPIMDDI_H
// =========================================================================
#include <agrum/core/hashTable.h>
// =========================================================================
#include <agrum/FMDP/FactoredMarkovDecisionProcess.h>
#include <agrum/FMDP/planning/spumdd.h>
#include <agrum/FMDP/learning/FMDPLearner.h>
#include <agrum/FMDP/learning/observation.h>
// =========================================================================
#include <agrum/multidim/instantiation.h>
#include <agrum/multidim/multiDimDecisionGraph.h>
// =========================================================================
#include <agrum/variables/discreteVariable.h>
// =========================================================================

namespace gum {

  /**
   * @class SPIMDDI spimddi.h <agrum/FMDP/learning/spimddi.h>
   * @brief
   * @ingroup fmdp_group
   *
   *
   *
   */

  class SPIMDDI {

    public:

      // ==========================================================================
      /// @name Constructor & destructor.
      // ==========================================================================
      /// @{

        // ###################################################################
        /// Default constructor
        // ###################################################################
        SPIMDDI (double discountFactor = 0.75,
                 double epsilon = 0.0001,
                 double learningThreshold = 0.95,
                 double similarityThreshold = 0.75,
                 Idx observationPhaseLenght = 100,
                 Idx nbValueIterationStep = 10,
                 double exploThreshold = 0.1);

        // ###################################################################
        /// Default destructor
        // ###################################################################
        ~SPIMDDI ();

      /// @}

      // ==========================================================================
      /// @name
      // ==========================================================================
      /// @{

        // ###################################################################
        ///
        // ###################################################################
        void addAction(const Idx actionId, const std::string &actionName );

        // ###################################################################
        ///
        // ###################################################################
        void addVariable( const DiscreteVariable* var );
        void addReward( double );

      /// @}

      // ==========================================================================
      /// @name
      // ==========================================================================
      /// @{

        // ###################################################################
        ///
        // ###################################################################
        void initialize(const Instantiation& initialState);

        // ###################################################################
        ///
        // ###################################################################
        void initialize();

        // ###################################################################
        ///
        // ###################################################################
        INLINE void setCurrentState( const Instantiation&  currentState ) { __lastState = currentState; }

        // ###################################################################
        ///
        // ###################################################################
        void feedback(const Instantiation &, double );
        void feedback(const Instantiation &, const Instantiation&, Idx, double );

        // ###################################################################
        ///
        // ###################################################################
        Idx takeAction();
        Idx takeAction( const Instantiation& );

      /// @}        

      // ###################################################################
      /// Returns a string describing the learned FMDP, and the associated
      /// optimal policy.
      /// Both as describing using DOT language.
      // ###################################################################
      std::string toString();

      Size learnerSize() { return __learner->size(); }

      Size modelSize() { return __fmdp->size(); }

      Size valueFunctionSize() { return __planer->vFunction()->realSize(); }

      Size optimalPolicySize() { return __planer->optimalPolicy()->realSize(); }


    private :

      /// The learnt Markovian Decision Process
      FactoredMarkovDecisionProcess<double>* __fmdp;

      /// The model learner.
      /// It will handle by itself the different decision graph learners
      /// needed to learn the FMDP
      FMDPLearner* __learner;

      LabelizedVariable* __rewardVar;
      Bijection<double,Idx> __rewardValue2Idx;

      /// The planer we use to find the optimal policy
      SPUMDD<double>* __planer;

      /// The number of observation we make before using again the planer
      Idx __observationPhaseLenght;

      /// The total number of observation made so far
      Idx __nbObservation;

      /// The number of Value Iteration step we perform
      Idx __nbValueIterationStep;

      /// Threshold under which we perform a random action instead of exploiting the optimal one
      double __exploThreshold;

      Sequence<Idx> __actionSeq;

      /// The state in which the system is before we perform a new action
      Instantiation __lastState;
      Idx __lastAction;

      /// Since SPIMMDI made these observation, it has to delete them on quitting
      Set<Observation*> __bin;

  };


} /* namespace gum */


#endif // GUM_SPIMDDI_H

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;

