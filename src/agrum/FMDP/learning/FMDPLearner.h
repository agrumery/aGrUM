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
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */

// =========================================================================
#ifndef GUM_FMDP_LEARNER_H
#define GUM_FMDP_LEARNER_H
// =========================================================================
#include <agrum/core/hashTable.h>
// =========================================================================
#include <agrum/FMDP/FactoredMarkovDecisionProcess.h>
#include <agrum/FMDP/learning/observation.h>
#include <agrum/FMDP/learning/decisionGraph/imddi.h>
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

  class FMDPLearner {

    public:

      // ==========================================================================
      /// @name Constructor & destructor.
      // ==========================================================================
      /// @{

        // ###################################################################
        /// Default constructor
        // ###################################################################
        FMDPLearner ( FactoredMarkovDecisionProcess<double>* target, double learningThreshold );

        // ###################################################################
        /// Default destructor
        // ###################################################################
        ~FMDPLearner ();

      /// @}

      // ==========================================================================
      /// @name
      // ==========================================================================
      /// @{

        // ###################################################################
        ///
        // ###################################################################
        void addVariable(const DiscreteVariable*);

        void addAction(const Idx actionId, const std::string &);

        void addReward(const DiscreteVariable*);

      /// @}

      // ==========================================================================
      /// @name
      // ==========================================================================
      /// @{

        // ###################################################################
        ///
        // ###################################################################
        void initialize();

        // ###################################################################
        ///
        // ###################################################################
        void addObservation(Idx, const Observation*);

        // ###################################################################
        ///
        // ###################################################################
        void updateFMDP();

      /// @}

      Size size();

    private :

      /// The FMDP to store the learned model
      FactoredMarkovDecisionProcess<double>* __fmdp;

      /// Set of main variables describing the system'rdfgll
      Set<const DiscreteVariable*> __mainVariables;


      HashTable<Idx, Set<IMDDI*>*> __actionLearners;
      IMDDI* __rewardLearner;
      const DiscreteVariable* __rewardVar;

      const double __learningThreshold;

  };


} /* namespace gum */


#include <agrum/FMDP/learning/FMDPLearner.tcc>

#endif // GUM_FMDP_LEARNER_H

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;

