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
 * @brief Headers of the Variable Selector class.
 *
 * @author Jean-Christophe MAGNAN
 */

// =========================================================================
#ifndef GUM_VARIABLE_SELECTOR_H
#define GUM_VARIABLE_SELECTOR_H
// =========================================================================
#include <agrum/tools/core/multiPriorityQueue.h>
#include <agrum/tools/core/set.h>
// =========================================================================
#include <agrum/tools/variables/discreteVariable.h>
// =========================================================================

namespace gum {

  /**
   * @class VariableSelector variableselector.h
   * <agrum/FMDP/planning/FunctionGraph/variableselector.h>
   * @brief
   * @ingroup fmdp_group
   *
   *
   *
   */

  class VariableSelector {
    public:
    // ==========================================================================
    /// @name Constructor & destructor.
    // ==========================================================================
    /// @{

    // ###################################################################
    /// Default constructor
    // ###################################################################
    explicit VariableSelector(const Set< const DiscreteVariable* >& startingSet);

    // ###################################################################
    /// Default destructor
    // ###################################################################
    ~VariableSelector();

    /// @}

    // ==========================================================================
    /// @name
    // ==========================================================================
    /// @{

    // ###################################################################
    ///
    // ###################################################################
    void updateScore(const DiscreteVariable* var, double score, double secondaryscore);
    void downdateScore(const DiscreteVariable* var, double score, double secondaryscore);


    // ###################################################################
    /// Select the most relevant variable
    // ###################################################################
    const DiscreteVariable* select();

    bool isEmpty() const { return _remainingVars_.empty(); }

    void                    begin() { _rvi_ = _remainingVars_.beginSafe(); }
    bool                    hasNext() const { return _rvi_ != _remainingVars_.endSafe(); }
    void                    next() { ++_rvi_; }
    const DiscreteVariable* current() const { return *_rvi_; }

    private:
    void _addVar_(const DiscreteVariable* var);
    void _removeVar_(const DiscreteVariable* var);

    /// The set of remaining vars to select among
    Set< const DiscreteVariable* >             _remainingVars_;
    SetIteratorSafe< const DiscreteVariable* > _rvi_;

    /// Heap keeping best score on top for immediate access
    MultiPriorityQueue< double, double, std::greater< double > > _remainingScores_;

    /// HashTable associating to each score the set of variable having that
    /// score
    HashTable< double, Set< const DiscreteVariable* >* > _remainingVarsByScore_;

    /// HashTable associating to each variable its score
    HashTable< const DiscreteVariable*, double > _remainingVarsScore_;

    /// HashTable associating to each variable its 2nd score
    HashTable< const DiscreteVariable*, double > _remainingVarsOtherScore_;
  };

}   // namespace gum

#endif   // GUM_VARIABLE_SELECTOR_H
