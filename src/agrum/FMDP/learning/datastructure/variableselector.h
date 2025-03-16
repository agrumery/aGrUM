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
 * @brief Headers of the Variable Selector class.
 *
 * @author Jean-Christophe MAGNAN
 */

// =========================================================================
#ifndef GUM_VARIABLE_SELECTOR_H
#define GUM_VARIABLE_SELECTOR_H
// =========================================================================
#include <agrum/base/core/multiPriorityQueue.h>
// =========================================================================
#include <agrum/base/variables/discreteVariable.h>

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
    explicit VariableSelector(const gum::VariableSet& startingSet);

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

    void begin() { _rvi_ = _remainingVars_.beginSafe(); }

    bool hasNext() const { return _rvi_ != _remainingVars_.endSafe(); }

    void next() { ++_rvi_; }

    const DiscreteVariable* current() const { return *_rvi_; }

    private:
    void _addVar_(const DiscreteVariable* var);
    void _removeVar_(const DiscreteVariable* var);

    /// The set of remaining vars to select among
    gum::VariableSet                           _remainingVars_;
    SetIteratorSafe< const DiscreteVariable* > _rvi_;

    /// Heap keeping best score on top for immediate access
    MultiPriorityQueue< double, double, std::greater< double > > _remainingScores_;

    /// HashTable associating to each score the set of variable having that
    /// score
    HashTable< double, gum::VariableSet* > _remainingVarsByScore_;

    /// HashTable associating to each variable its score
    HashTable< const DiscreteVariable*, double > _remainingVarsScore_;

    /// HashTable associating to each variable its 2nd score
    HashTable< const DiscreteVariable*, double > _remainingVarsOtherScore_;
  };

}   // namespace gum

#endif   // GUM_VARIABLE_SELECTOR_H
