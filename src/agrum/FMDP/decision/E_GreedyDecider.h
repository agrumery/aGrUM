/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
