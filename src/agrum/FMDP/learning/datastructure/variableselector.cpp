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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


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
  VariableSelector::VariableSelector(const gum::VariableSet& startingSet) :
      _remainingVars_(startingSet) {
    GUM_CONSTRUCTOR(VariableSelector)
    _remainingScores_.insert(0.0, 0.0);
    _remainingVarsByScore_.insert(0.0, new gum::VariableSet(_remainingVars_));

    for (auto varIter = _remainingVars_.cbeginSafe(); varIter != _remainingVars_.cendSafe();
         ++varIter) {
      _remainingVarsScore_.insert(*varIter, 0.0);
      _remainingVarsOtherScore_.insert(*varIter, 0.0);
    }
  }

  // ###################################################################
  // Default constructor
  // ###################################################################
  VariableSelector::~VariableSelector() { GUM_DESTRUCTOR(VariableSelector) }

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
    double                  bestScore = _remainingScores_.top();
    const gum::VariableSet* bestSet   = _remainingVarsByScore_[bestScore];
    const DiscreteVariable* bestVar   = nullptr;

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
      _remainingVarsByScore_.insert(varScore, new gum::VariableSet());
      _remainingScores_.insert(varScore, varScore);
    }
    _remainingVarsByScore_[varScore]->insert(var);
  }

  // ###################################################################
  // Select the most relevant variable
  // ###################################################################
  void VariableSelector::_removeVar_(const DiscreteVariable* var) {
    double            varScore = _remainingVarsScore_[var];
    gum::VariableSet* varSet   = _remainingVarsByScore_[varScore];
    *varSet >> var;
    if (varSet->empty()) {
      _remainingScores_.erase(varScore);
      _remainingVarsByScore_.erase(varScore);
      delete varSet;
    }
  }
}   // namespace gum
