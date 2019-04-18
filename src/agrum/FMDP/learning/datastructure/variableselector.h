
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
#include <agrum/core/multiPriorityQueue.h>
#include <agrum/core/set.h>
// =========================================================================
#include <agrum/variables/discreteVariable.h>
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
    VariableSelector(Set< const DiscreteVariable* >& startingSet);

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
    void updateScore(const DiscreteVariable* var,
                     double                  score,
                     double                  secondaryscore);
    void downdateScore(const DiscreteVariable* var,
                       double                  score,
                       double                  secondaryscore);


    // ###################################################################
    /// Select the most relevant variable
    // ###################################################################
    const DiscreteVariable* select();

    bool isEmpty() { return __remainingVars.empty(); }

    void begin() { __rvi = __remainingVars.beginSafe(); }
    bool hasNext() { return __rvi != __remainingVars.endSafe(); }
    void next() { ++__rvi; }
    const DiscreteVariable* current() { return *__rvi; }

    private:
    void __addVar(const DiscreteVariable* var);
    void __removeVar(const DiscreteVariable* var);

    /// The set of remaining vars to select among
    Set< const DiscreteVariable* >             __remainingVars;
    SetIteratorSafe< const DiscreteVariable* > __rvi;

    /// Heap keeping best score on top for immediate access
    MultiPriorityQueue< double, double, std::greater< double > > __remainingScores;

    /// HashTable associating to each score the set of variable having that
    /// score
    HashTable< double, Set< const DiscreteVariable* >* > __remainingVarsByScore;

    /// HashTable associating to each variable its score
    HashTable< const DiscreteVariable*, double > __remainingVarsScore;

    /// HashTable associating to each variable its 2nd score
    HashTable< const DiscreteVariable*, double > __remainingVarsOtherScore;
  };

}   // namespace gum

#endif   // GUM_VARIABLE_SELECTOR_H
