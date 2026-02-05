/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Sources of the class.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */

// =========================================================================
#include <cstdlib>
// =========================================================================
#include <agrum/base/core/utils_random.h>
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

    _sss_ = 1.0;
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
    for (auto varIter = fmdp->beginVariables(); varIter != fmdp->endVariables(); ++varIter)
      _sss_ *= (double)(*varIter)->domainSize();
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
  void E_GreedyDecider::checkState(const Instantiation& reachedState, Idx actionId) {
    if (_statecpt_.nbVisitedStates() == 0) _statecpt_.reset(reachedState);
    else if (!_statecpt_.checkState(reachedState)) _statecpt_.addState(reachedState);
  }

  // ###################################################################
  /*
   * @param the state in which we currently are
   * @return a set containing every optimal actions on that state
   */
  // ###################################################################
  ActionSet E_GreedyDecider::stateOptimalPolicy(const Instantiation& curState) {
    double explo          = randomProba();
    double temp           = std::pow((_sss_ - (double)_statecpt_.nbVisitedStates()) / _sss_, 3.0);
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
