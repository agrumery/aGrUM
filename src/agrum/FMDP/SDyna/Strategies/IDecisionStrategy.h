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
 * @brief Headers of the Decision Strategy interface.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 */


// =========================================================================
#ifndef GUM_SDYNA_DECISION_STRATEGY_H
#define GUM_SDYNA_DECISION_STRATEGY_H
// =========================================================================
// =========================================================================
#include <agrum/FMDP/planning/actionSet.h>
// =========================================================================
// =========================================================================

namespace gum {

  /**
   * @class IDecisionStrategy IDecisionStrategy.h
   * <agrum/FMDP/SDyna/IDecisionStrategy.h>
   * @brief Interface for manipulating decision center
   * @ingroup fmdp_group
   *
   */
  class IDecisionStrategy {
    // ###################################################################
    /// @name Constructor & destructor.
    // ###################################################################
    /// @{
    public:
    // ==========================================================================
    /// Destructor (virtual and empty since it's an interface)
    // ==========================================================================
    virtual ~IDecisionStrategy() {}

    /// @}

    // ###################################################################
    /// @name Initialization
    // ###################################################################
    /// @{
    public:
    // ==========================================================================
    /// Initializes the learner
    // ==========================================================================
    virtual void initialize(const FMDP< double >* fmdp) {
      optPol_ = nullptr;
      if (allActions_.size() == 0)
        for (auto actionIter = fmdp->beginActions(); actionIter != fmdp->endActions(); ++actionIter)
          allActions_ += *actionIter;
    }
    /// @}


    // ###################################################################
    /// @name Incremental methods
    // ###################################################################
    /// @{
    public:
    virtual void checkState(const Instantiation& newState, Idx actionId) = 0;

    void
       setOptimalStrategy(const MultiDimFunctionGraph< ActionSet, SetTerminalNodePolicy >* optPol) {
      optPol_ = const_cast< MultiDimFunctionGraph< ActionSet, SetTerminalNodePolicy >* >(optPol);
    }

    virtual ActionSet stateOptimalPolicy(const Instantiation& curState) {
      return (optPol_ && optPol_->realSize() != 0) ? optPol_->get(curState) : allActions_;
    }

    protected:
    ///
    const MultiDimFunctionGraph< ActionSet, SetTerminalNodePolicy >* optPol_;

    ///
    ActionSet allActions_;

    /// @}
  };
}   // namespace gum
#endif   // GUM_SDYNA_DECISION_STRATEGY_H
