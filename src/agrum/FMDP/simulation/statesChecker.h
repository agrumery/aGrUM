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
 * @brief Headers of the States Checker class.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */

// =========================================================================
#ifndef GUM_STATES_CHECKER_H
#define GUM_STATES_CHECKER_H
// =========================================================================
// =========================================================================
#include <agrum/tools/multidim/implementations/multiDimFunctionGraph.h>
// =========================================================================

namespace gum {


  /**
   * @class StatesChecker statesChecker.h
   * <agrum/FMDP/simulation/statesChecker.h>
   * @brief Provides the necessary to check whether or not states have been
   * already visited.
   * @ingroup fmdp_group
   */
  class StatesChecker {
    public:
    // ==========================================================================
    /// @name Constructor & destructor.
    // ==========================================================================
    /// @{

    /**
     * Default constructor
     */
    StatesChecker();

    /**
     * Default destructor
     */
    ~StatesChecker();

    /// @}

    // ==========================================================================
    /// @name Miscelleanous methods
    // ==========================================================================
    /// @{

    bool checkState(const Instantiation& state) { return _checker_->get(state); }

    void addState(const Instantiation&);

    Idx nbVisitedStates() { return _nbVisitedStates_; }

    void reset(const Instantiation&);

    /// @}

    private:
    void _insertState_(const Instantiation&, NodeId, Idx);

    Idx _nbVisitedStates_;

    MultiDimFunctionGraph< bool >* _checker_;
    NodeId                         _checkerTrueId_, _checkerFalseId_;

    Set< Instantiation* > _visitedStates_;
  };
} /* namespace gum */

#endif   // GUM_STATES_CHECKER_H
