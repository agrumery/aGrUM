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
#include <agrum/FMDP/FMDP.h>
#include <agrum/FMDP/SDyna/Strategies/ILearningStrategy.h>
#include <agrum/FMDP/learning/observation.h>
#include <agrum/FMDP/learning/datastructure/imddi.h>
#include <agrum/FMDP/learning/datastructure/iti.h>
// =========================================================================
#include <agrum/variables/discreteVariable.h>
// =========================================================================

namespace gum {

  /**
   * @class FMDPLearner FMDPLearner.h <agrum/FMDP/learning/FMDPLearner.h>
   * @brief
   * @ingroup fmdp_group
   *
   *
   *
   */
  template <TESTNAME VariableAttributeSelection, TESTNAME RewardAttributeSelection, LEARNERNAME LearnerSelection>
  class FMDPLearner : public ILearningStrategy {

    template < bool isScalar >
    using VariableLearnerType = typename LearnerSelect<LearnerSelection, IMDDI<VariableAttributeSelection, isScalar>,
                                                               ITI<VariableAttributeSelection, isScalar> >::type ;
    template < bool isScalar >
    using RewardLearnerType = typename LearnerSelect<LearnerSelection, IMDDI<VariableAttributeSelection, isScalar>,
                                                               ITI<VariableAttributeSelection, isScalar> >::type ;


    public:

      // ==========================================================================
      /// @name Constructor & destructor.
      // ==========================================================================
      /// @{

        // ###################################################################
        /// Default constructor
        // ###################################################################
        FMDPLearner ( FMDP<double>* target, double learningThreshold, double similarityThreshold );

        // ###################################################################
        /// Default destructor
        // ###################################################################
        ~FMDPLearner ();

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

      /// @}


      // ###################################################################
      /// @name Incremental methods
      // ###################################################################
      /// @{
    public :

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
    public :

        // ==========================================================================
        /**
         * @brief learnerSize
         * @return
         */
        // ==========================================================================
        Size size();

      /// @}

      MultiDimFunctionGraph<double>* extractCount(Idx actionId, const DiscreteVariable* var){
        return __actionLearners[actionId]->getWithDefault(var, nullptr)->extractCount();}

    private :

      /// The FMDP to store the learned model
      FMDP<double>* __fmdp;

      HashTable<
            Idx, HashTable<const DiscreteVariable*, IncrementalGraphLearner<VariableAttributeSelection, false>*>*
                > __actionLearners;

      IncrementalGraphLearner<RewardAttributeSelection, true>* __rewardLearner;

      const double __learningThreshold;
      const double __similarityThreshold;

  };


} /* namespace gum */

#include <agrum/FMDP/learning/FMDPLearner.tcc>

#endif // GUM_FMDP_LEARNER_H

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;

