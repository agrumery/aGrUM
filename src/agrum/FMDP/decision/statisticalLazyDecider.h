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
 * @brief Headers of the Statistical lazy decision maker class.
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(@AMU)
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
    StatisticalLazyDecider() : counter__(), initialized__(false) {
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
      if (!initialized__) {
        counter__.reset(newState);
        initialized__ = true;
      } else
        counter__.incState(newState);
    }

    private:
    StatesCounter counter__;
    bool          initialized__;
  };
}   // namespace gum
#endif   // GUM_STATISTICAL_LAZY_DECIDER_H
