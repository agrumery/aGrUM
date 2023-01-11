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
 * @brief Headers of the Statistical lazy decision maker class.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */


// =========================================================================
#ifndef GUM_STATISTICAL_LAZY_DECIDER_H
#define GUM_STATISTICAL_LAZY_DECIDER_H
// =========================================================================
#include <agrum/FMDP/decision/lazyDecider.h>
#include <agrum/FMDP/simulation/statesCounter.h>
// =========================================================================

namespace gum {

  /**
   * @class StatisticalLazyDecider statisticalLazyDecider.h
   * <agrum/FMDP/decision/statisticalLazyDecider.h>
   * @brief Class to make no decision at all yet performing statistic on visited
   * states.
   * Actions are randomly chosen among all actions available
   * @ingroup fmdp_group
   *
   * Does nothing more for decison making
   * But count how many times every visited states have been visited
   *
   */
  class StatisticalLazyDecider: public IDecisionStrategy {
    // ###################################################################
    /// @name Constructor & destructor.
    // ###################################################################
    /// @{
    public:
    // ==========================================================================
    /// Constructor
    // ==========================================================================
    StatisticalLazyDecider() : _counter_(), _initialized_(false) {
      GUM_CONSTRUCTOR(StatisticalLazyDecider);
    }

    // ==========================================================================
    /// Destructor
    // ==========================================================================
    ~StatisticalLazyDecider() {
      GUM_DESTRUCTOR(StatisticalLazyDecider);
      ;
    }

    /// @}


    // ###################################################################
    /// @name Incremental methods
    // ###################################################################
    /// @{
    public:
    void checkState(const Instantiation& newState, Idx actionId = 0) {
      if (!_initialized_) {
        _counter_.reset(newState);
        _initialized_ = true;
      } else _counter_.incState(newState);
    }

    private:
    StatesCounter _counter_;
    bool          _initialized_;
  };
}   // namespace gum
#endif   // GUM_STATISTICAL_LAZY_DECIDER_H
