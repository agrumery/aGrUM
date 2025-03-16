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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/





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
#include <agrum/base/multidim/implementations/multiDimFunctionGraph.h>

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
