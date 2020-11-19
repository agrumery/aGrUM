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
 * @brief Template implémentations of the NodeDatabase class.
 *
 * @author Jean-Christophe MAGNAN
 */
// =========================================================================
#include <agrum/FMDP/learning/datastructure/variableselector.h>
// =========================================================================

namespace gum {

  // ==========================================================================
  // Constructor & destructor.
  // ==========================================================================

  // ###################################################################
  // Default constructor
  // ###################################################################
  VariableSelector::VariableSelector(Set< const DiscreteVariable* >& startingSet) :
      remainingVars__(startingSet) {
    GUM_CONSTRUCTOR(VariableSelector);
    remainingScores__.insert(0.0, 0.0);
    remainingVarsByScore__.insert(
       0.0, new Set< const DiscreteVariable* >(remainingVars__));

    for (auto varIter = remainingVars__.cbeginSafe();
         varIter != remainingVars__.cendSafe();
         ++varIter) {
      remainingVarsScore__.insert(*varIter, 0.0);
      remainingVarsOtherScore__.insert(*varIter, 0.0);
    }
  }

  // ###################################################################
  // Default constructor
  // ###################################################################
  VariableSelector::~VariableSelector() { GUM_DESTRUCTOR(VariableSelector); }


  // ###################################################################
  //
  // ###################################################################
  void VariableSelector::updateScore(const DiscreteVariable* var,
                                     double                  score,
                                     double                  secondaryscore) {
    removeVar__(var);
    remainingVarsScore__[var] += score;
    addVar__(var);
    remainingVarsOtherScore__[var] += secondaryscore;
  }


  // ###################################################################
  //
  // ###################################################################
  void VariableSelector::downdateScore(const DiscreteVariable* var,
                                       double                  score,
                                       double                  secondaryscore) {
    removeVar__(var);
    remainingVarsScore__[var] -= score;
    addVar__(var);
    remainingVarsOtherScore__[var] -= secondaryscore;
  }


  // ###################################################################
  // Select the most relevant variable
  // ###################################################################
  const DiscreteVariable* VariableSelector::select() {
    double                          bestScore = remainingScores__.top();
    Set< const DiscreteVariable* >* bestSet = remainingVarsByScore__[bestScore];
    const DiscreteVariable*         bestVar = nullptr;

    for (auto varIter = bestSet->beginSafe(); varIter != bestSet->endSafe();
         ++varIter) {
      if (bestVar == nullptr
          || remainingVarsOtherScore__[bestVar]
                < remainingVarsOtherScore__[*varIter]
          || (remainingVarsOtherScore__[bestVar]
                 == remainingVarsOtherScore__[*varIter]
              && bestVar->domainSize() < (*varIter)->domainSize()))
        bestVar = *varIter;
    }
    removeVar__(bestVar);
    remainingVars__ >> bestVar;
    return bestVar;
  }


  // ###################################################################
  // Select the most relevant variable
  // ###################################################################
  void VariableSelector::addVar__(const DiscreteVariable* var) {
    double varScore = remainingVarsScore__[var];

    if (!remainingVarsByScore__.exists(varScore)) {
      remainingVarsByScore__.insert(varScore,
                                    new Set< const DiscreteVariable* >());
      remainingScores__.insert(varScore, varScore);
    }
    remainingVarsByScore__[varScore]->insert(var);
  }


  // ###################################################################
  // Select the most relevant variable
  // ###################################################################
  void VariableSelector::removeVar__(const DiscreteVariable* var) {
    double                          varScore = remainingVarsScore__[var];
    Set< const DiscreteVariable* >* varSet = remainingVarsByScore__[varScore];
    *varSet >> var;
    if (varSet->empty()) {
      remainingScores__.erase(varScore);
      remainingVarsByScore__.erase(varScore);
      delete varSet;
    }
  }
}   // namespace gum
