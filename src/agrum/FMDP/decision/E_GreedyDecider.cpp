/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief Sources of the class.
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(@AMU)
 */

// =========================================================================
#include <cstdlib>
#include <random>
// =========================================================================
#include <agrum/FMDP/decision/E_GreedyDecider.h>
// =========================================================================

namespace gum {

  // ==========================================================================
  // Constructor & destructor.
  // ==========================================================================

  // ###################################################################
  /**
   * Constructor
   */
  // ###################################################################
  E_GreedyDecider::E_GreedyDecider() {
    GUM_CONSTRUCTOR(E_GreedyDecider);

    sss__ = 1.0;
  }


  // ###################################################################
  /**
   *
   */
  // ###################################################################
  E_GreedyDecider::~E_GreedyDecider() {
    GUM_DESTRUCTOR(E_GreedyDecider);
    ;
  }


  // ==========================================================================
  // Initialization
  // ==========================================================================

  // ###################################################################
  /**
   *
   */
  // ###################################################################
  void E_GreedyDecider::initialize(const FMDP< double >* fmdp) {
    IDecisionStrategy::initialize(fmdp);
    for (auto varIter = fmdp->beginVariables(); varIter != fmdp->endVariables();
         ++varIter)
      sss__ *= (double)(*varIter)->domainSize();
  }


  // ==========================================================================
  // Incremental methods
  // ==========================================================================

  // ###################################################################
  /*
   * Performs a feedback on the last transition.
   * In extenso, learn from the transition.
   * @param reachedState : the state reached after the transition
   */
  // ###################################################################
  void E_GreedyDecider::checkState(const Instantiation& reachedState,
                                   Idx                  actionId) {
    if (statecpt__.nbVisitedStates() == 0)
      statecpt__.reset(reachedState);
    else if (!statecpt__.checkState(reachedState))
      statecpt__.addState(reachedState);
  }

  // ###################################################################
  /*
   * @param the state in which we currently are
   * @return a set containing every optimal actions on that state
   */
  // ###################################################################
  ActionSet E_GreedyDecider::stateOptimalPolicy(const Instantiation& curState) {
    double explo = (double)std::rand() / (double)RAND_MAX;
    double temp
       = std::pow((sss__ - (double)statecpt__.nbVisitedStates()) / sss__, 3.0);
    double exploThreshold = temp < 0.1 ? 0.1 : temp;

    //      std::cout << exploThreshold << std::endl;

    ActionSet optimalSet = IDecisionStrategy::stateOptimalPolicy(curState);
    if (explo > exploThreshold) {
      //        std::cout << "Exploit : " << optimalSet << std::endl;
      return optimalSet;
    }

    if (allActions_.size() > optimalSet.size()) {
      ActionSet ret(allActions_);
      ret -= optimalSet;
      //        std::cout << "Explore : " << ret << std::endl;
      return ret;
    }

    //      std::cout << "Explore : " << allActions_ << std::endl;
    return allActions_;
  }

}   // End of namespace gum
