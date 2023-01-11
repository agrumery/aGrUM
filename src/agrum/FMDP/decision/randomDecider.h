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
 * @brief Headers of the Random decision maker class.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */


// =========================================================================
#ifndef GUM_RANDOM_DECIDER_H
#define GUM_RANDOM_DECIDER_H
// =========================================================================
#include <agrum/FMDP/SDyna/Strategies/IDecisionStrategy.h>
// =========================================================================

namespace gum {

  /**
   * @class RandomDecider
   * @headerfile randomDecider.h <agrum/FMDP/decision/randomDecider.h>
   * @brief Class to make decision randomly
   * @ingroup fmdp_group
   *
   * Does nothing more than the interface for DecisionStrategy does
   *
   */
  class RandomDecider: public IDecisionStrategy {
    // ###################################################################
    /// @name Constructor & destructor.
    // ###################################################################
    /// @{
    public:
    // ==========================================================================
    /// Constructor
    // ==========================================================================
    RandomDecider() {}

    // ==========================================================================
    /// Destructor
    // ==========================================================================
    ~RandomDecider() {}

    /// @}


    // ###################################################################
    /// @name Incremental methods
    // ###################################################################
    /// @{
    public:
    void checkState(const Instantiation& newState, Idx actionId) {}

    ActionSet stateOptimalPolicy(const Instantiation& curState) { return allActions_; }
  };
}   // namespace gum
#endif   // GUM_RANDOM_DECIDER_H
