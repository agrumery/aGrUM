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
 * @brief Headers of the Planning Strategy interface.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */


// =========================================================================
#ifndef GUM_SDYNA_PLANNING_STRATEGY_H
#define GUM_SDYNA_PLANNING_STRATEGY_H
// =========================================================================
#include <string>
// =========================================================================
// =========================================================================
#include <agrum/FMDP/planning/actionSet.h>
// =========================================================================

namespace gum {

  /**
   * @class IPlanningStrategy IPlanningStrategy.h
   * <agrum/FMDP/SDyna/IPlanningStrategy.h>
   * @brief Interface for manipulating FMDP planer
   * @ingroup fmdp_group
   *
   */
  template < typename GUM_SCALAR >
  class IPlanningStrategy {
    // ###################################################################
    /// @name Constructor & destructor.
    // ###################################################################
    /// @{
    public:
    // ==========================================================================
    /// Destructor (virtual and empty since it's an interface)
    // ==========================================================================
    virtual ~IPlanningStrategy() {}

    /// @}

    // ###################################################################
    /// @name Initialization
    // ###################################################################
    /// @{
    public:
    // ==========================================================================
    /// Initializes the learner
    // ==========================================================================
    virtual void initialize(const FMDP< GUM_SCALAR >* fmdp) = 0;

    /// @}


    // ###################################################################
    /// @name Incremental methods
    // ###################################################################
    /// @{
    public:
    // ==========================================================================
    /**
     * Starts a new planning
     * @param nbIte : the maximal number of value iteration performed in this
     * planning
     */
    // ==========================================================================
    virtual void makePlanning(Idx nbIte) = 0;

    // ==========================================================================
    /// Returns optimalPolicy computed so far current size
    // ==========================================================================
    virtual const MultiDimFunctionGraph< ActionSet, SetTerminalNodePolicy >* optimalPolicy() = 0;

    /// @}


    // ###################################################################
    /// @name Miscelleanous methods
    // ###################################################################
    /// @{
    public:
    // ==========================================================================
    /// Returns vFunction computed so far current size
    // ==========================================================================
    virtual Size vFunctionSize() = 0;

    // ==========================================================================
    /// Returns optimalPolicy computed so far current size
    // ==========================================================================
    virtual Size optimalPolicySize() = 0;

    // ==========================================================================
    /// Returns a string describing the optimal policy in a dot format
    // ==========================================================================
    virtual std::string optimalPolicy2String() = 0;

    /// @}
  };
}   // namespace gum
#endif   // GUM_SDYNA_LEARNING_STRATEGY_H
