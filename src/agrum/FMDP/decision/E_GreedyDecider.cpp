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
 * @brief Headers of the ModelLearner class.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */

// =========================================================================
#include <random>
#include <cstdlib>
// =========================================================================
#include <agrum/FMDP/decision/E_GreedyDecider.h>
// =========================================================================

namespace gum {

  // ==========================================================================
  /// @name Constructor & destructor.
  // ==========================================================================

    // ###################################################################
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
    // ###################################################################
    E_GreedyDecider::E_GreedyDecider (IPlanningStrategy<double> *planer ) : __planer(planer) {

      GUM_CONSTRUCTOR(E_GreedyDecider)

      __sss = 1.0;
    }


    // ###################################################################
    /**
     * Returns
     * @return a string describing the learned FMDP, and the associated
     * optimal policy.
     * Both in DOT language.
     */
    // ###################################################################
    E_GreedyDecider::~E_GreedyDecider (){

      GUM_DESTRUCTOR(E_GreedyDecider)
    }



  // ==========================================================================
  /// @name Initialization
  // ==========================================================================

    // ###################################################################
    /**
     * Initializes the Sdyna instance internal satastructure; i.e. the planer and
     * the learner.
     */
    // ###################################################################
    void E_GreedyDecider::initialize( const FMDP<double>* fmdp ){
      for( auto actionIter = fmdp->beginActions(); actionIter != fmdp->endActions(); ++actionIter )
        __explorething += *actionIter;
      for( auto varIter = fmdp->beginVariables(); varIter != fmdp->endVariables(); ++varIter )
        __sss *= (double) (*varIter)->domainSize();
    }



  // ==========================================================================
  /// @name Incremental methods
  // ==========================================================================

    // ###################################################################
    /**
     * Performs a feedback on the last transition.
     * In extenso, learn from the transition.
     * @param reachedState : the state reached after the transition
     * @param obtainedReward : the reward obtained during the transition
     * @warning Uses the __originalState and __performedAction stored in cache
     * If you want to specify the original state and the performed action, see below
     */
    // ###################################################################
    void E_GreedyDecider::checkState(const Instantiation & reachedState){
      if( __statecpt.nbVisitedStates() == 0 )
        __statecpt.reset(reachedState);
      else
        if( !__statecpt.checkState(reachedState) )
            __statecpt.addState(reachedState);
    }

    // ###################################################################
    /**
     * @param the state in which we currently are
     * @return a set containing every optimal actions on that state
     */
    // ###################################################################
    ActionSet E_GreedyDecider::getStateOptimalPolicy(const Instantiation& curState){

      double explo = (double)std::rand( ) / (double)RAND_MAX;
      double temp = std::pow( (__sss - (double) __statecpt.nbVisitedStates() ) / __sss, 3.0);
      double exploThreshold = temp < 0.1 ? 0.1 : temp;

      //std::cout << << exploThreshold << std::endl;

      if( __planer->optimalPolicySize() ){
        ActionSet optimalSet = __planer->getStateOptimalPolicy(curState);
        if( explo > exploThreshold ) {
          //std::cout << "Exploit : " << optimalSet << std::endl;
          return optimalSet;
        } else {
          if( __explorething.size() > optimalSet.size()){
            ActionSet ret(__explorething);
            ret -= optimalSet;
            //std::cout << << "Explore : " << ret << std::endl;
            return ret;
          }
        }
      }

      //std::cout << "Explore : " << __explorething << std::endl;
      return __explorething;
    }

} // End of namespace gum
