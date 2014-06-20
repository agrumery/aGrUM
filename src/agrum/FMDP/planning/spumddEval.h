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
 * @brief Headers of the SPUMDD class.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */

// #define  TRACE_ALGO
// =========================================================================
#ifndef GUM_SPUMDD_EVAL_H
#define GUM_SPUMDD_EVAL_H
// =========================================================================
#include <string>
#include <cstdio>
#include <iostream>
#include <fstream>
// =========================================================================
#include <agrum/multidim/multiDimDecisionGraph.h>
// =========================================================================
#include <agrum/FMDP/FactoredMarkovDecisionProcess.h>
// =========================================================================

namespace gum {

  /**
   * @class SPUMDDEval spumddEval.h <agrum/FMDP/planning/SPUMDDEval.h>
   * @brief A class to find optimal policy for a given FMDP.
   * @ingroup fmdp_group
   *
   * Perform a SPUDD planning on given in parameter factored markov decision process
   * And eval the performance of this algorithm
   *
   */

  template<typename GUM_SCALAR>
  class SPUMDDEval {

    public:

      // ==========================================================================
      /// @name Constructor & destructor.
      // ==========================================================================
      /// @{

      /**
      * Default constructor
      */
      SPUMDDEval ( FactoredMarkovDecisionProcess<GUM_SCALAR>* fmdp, GUM_SCALAR epsilon = 0.00001 );

      /**
      * Default destructor
      */
      ~SPUMDDEval();

      /// @}
      // ==========================================================================
      /// @name Planning Algorithm Evaluation Methods
      // ==========================================================================
      /// @{

    public:

      /**
       * Method to eval the efficiency of makePlanning algorithm
       */
//      MultiDimDecisionGraph< GUM_SCALAR >* makePlanningAlgoEvaluation ( const std::string saveFilesName, Idx mode = 1 );

    private:

      /**
       * Method to eval the efficiency of Vaction evaluation
       */
//      MultiDimDecisionGraph< GUM_SCALAR >* __evalActionValueAlgoEvaluation ( const MultiDimDecisionGraph< GUM_SCALAR >* Vold, const std::string saveFilesName, Idx mode );

      /**
       * Method to eval addition the multiplication by discount reward and the addition of reward to computed value function
       */
//      MultiDimDecisionGraph< GUM_SCALAR >* __addRewardAlgoEvaluation ( const MultiDimDecisionGraph< GUM_SCALAR >* Vold, const std::string saveFilesName, Idx mode );

      /**
       * Evals the policy corresponding to the given value function
       */
//      std::pair<Idx, Idx> __evalNbRetrogradeEvaluation ( const MultiDimDecisionGraph<GUM_SCALAR>* t1, const MultiDimDecisionGraph<GUM_SCALAR>* t2 );

      /// @}

      /// The Factored Markov Decision Process on which we do our planning (NB : this one must have decision diagram as transitions and reward functions )
      FactoredMarkovDecisionProcess<GUM_SCALAR>* __fmdp;

      /// The hyperparameter determining when we do stop value iteration.
      GUM_SCALAR __epsilon;

      /// RetroVar number save file
      std::ofstream __traceAlgoSaveFile;
  };
} /* namespace gum */


#include <agrum/FMDP/planning/spumddEval.tcc>

#endif // GUM_SPUMDD_EVAL_H

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;

