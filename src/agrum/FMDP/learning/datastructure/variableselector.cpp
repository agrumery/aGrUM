/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
      _remainingVars_(startingSet) {
    GUM_CONSTRUCTOR(VariableSelector);
    _remainingScores_.insert(0.0, 0.0);
    _remainingVarsByScore_.insert(0.0, new Set< const DiscreteVariable* >(_remainingVars_));

    for (auto varIter = _remainingVars_.cbeginSafe(); varIter != _remainingVars_.cendSafe();
         ++varIter) {
      _remainingVarsScore_.insert(*varIter, 0.0);
      _remainingVarsOtherScore_.insert(*varIter, 0.0);
    }
  }

  // ###################################################################
  // Default constructor
  // ###################################################################
  VariableSelector::~VariableSelector() {
    GUM_DESTRUCTOR(VariableSelector);
    ;
  }


  // ###################################################################
  //
  // ###################################################################
  void VariableSelector::updateScore(const DiscreteVariable* var,
                                     double                  score,
                                     double                  secondaryscore) {
    _removeVar_(var);
    _remainingVarsScore_[var] += score;
    _addVar_(var);
    _remainingVarsOtherScore_[var] += secondaryscore;
  }


  // ###################################################################
  //
  // ###################################################################
  void VariableSelector::downdateScore(const DiscreteVariable* var,
                                       double                  score,
                                       double                  secondaryscore) {
    _removeVar_(var);
    _remainingVarsScore_[var] -= score;
    _addVar_(var);
    _remainingVarsOtherScore_[var] -= secondaryscore;
  }


  // ###################################################################
  // Select the most relevant variable
  // ###################################################################
  const DiscreteVariable* VariableSelector::select() {
    double                          bestScore = _remainingScores_.top();
    Set< const DiscreteVariable* >* bestSet   = _remainingVarsByScore_[bestScore];
    const DiscreteVariable*         bestVar   = nullptr;

    for (auto varIter = bestSet->beginSafe(); varIter != bestSet->endSafe(); ++varIter) {
      if (bestVar == nullptr
          || _remainingVarsOtherScore_[bestVar] < _remainingVarsOtherScore_[*varIter]
          || (_remainingVarsOtherScore_[bestVar] == _remainingVarsOtherScore_[*varIter]
              && bestVar->domainSize() < (*varIter)->domainSize()))
        bestVar = *varIter;
    }
    _removeVar_(bestVar);
    _remainingVars_ >> bestVar;
    return bestVar;
  }


  // ###################################################################
  // Select the most relevant variable
  // ###################################################################
  void VariableSelector::_addVar_(const DiscreteVariable* var) {
    double varScore = _remainingVarsScore_[var];

    if (!_remainingVarsByScore_.exists(varScore)) {
      _remainingVarsByScore_.insert(varScore, new Set< const DiscreteVariable* >());
      _remainingScores_.insert(varScore, varScore);
    }
    _remainingVarsByScore_[varScore]->insert(var);
  }


  // ###################################################################
  // Select the most relevant variable
  // ###################################################################
  void VariableSelector::_removeVar_(const DiscreteVariable* var) {
    double                          varScore = _remainingVarsScore_[var];
    Set< const DiscreteVariable* >* varSet   = _remainingVarsByScore_[varScore];
    *varSet >> var;
    if (varSet->empty()) {
      _remainingScores_.erase(varScore);
      _remainingVarsByScore_.erase(varScore);
      delete varSet;
    }
  }
}   // namespace gum
