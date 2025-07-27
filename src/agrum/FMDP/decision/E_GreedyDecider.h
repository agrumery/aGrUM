/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
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
 * @brief Headers of the epsilon-greedy decision maker class.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */


// =========================================================================
#ifndef GUM_E_GREEDY_DECIDER_H
#define GUM_E_GREEDY_DECIDER_H
// =========================================================================
// =========================================================================
#include <agrum/FMDP/SDyna/Strategies/IDecisionStrategy.h>
#include <agrum/FMDP/SDyna/Strategies/IPlanningStrategy.h>
#include <agrum/FMDP/simulation/statesChecker.h>

// =========================================================================
// =========================================================================

namespace gum {

  /**
   * @class E_GreedyDecider E_GreedyDecider.h
   * <agrum/FMDP/decision/E_GreedyDecider.h>
   * @brief Class to make decision following an epsilon-greedy compromise
   * between exploration and exploitation
   *
   *
   */
  class E_GreedyDecider: public IDecisionStrategy {
    // ###################################################################
    /// @name Constructor & destructor.
    // ###################################################################
    /// @{

    public:
    // ==========================================================================
    /// Constructor
    // ==========================================================================
    E_GreedyDecider();

    // ==========================================================================
    /// Destructor
    // ==========================================================================
    ~E_GreedyDecider();

    /// @}

    // ###################################################################
    /// @name Initialization
    // ###################################################################
    /// @{

    public:
    // ==========================================================================
    /// Initializes the learner
    // ==========================================================================
    void initialize(const FMDP< double >* fmdp);

    /// @}


    // ###################################################################
    /// @name Incremental methods
    // ###################################################################
    /// @{

    public:
    void checkState(const Instantiation& newState, Idx actionId);

    ActionSet stateOptimalPolicy(const Instantiation& curState);

    /// @}

    private:
    StatesChecker _statecpt_;
    double        _sss_;
  };
}   // namespace gum
#endif   // GUM_E_GREEDY_DECIDER_H
