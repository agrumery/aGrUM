/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
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
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
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


#include <agrum/FMDP/simulation/statesChecker.h>

namespace gum {

  StatesChecker::StatesChecker() :
      _nbVisitedStates_(0), _checker_(MultiDimFunctionGraph< bool >::getTreeInstance()),
      _checkerTrueId_(0), _checkerFalseId_(0) {
    GUM_CONSTRUCTOR(StatesChecker);
  }

  StatesChecker::~StatesChecker() {
    delete _checker_;
    GUM_DESTRUCTOR(StatesChecker);
  }

  void StatesChecker::reset(const Instantiation& initialState) {
    _checker_->clear();
    for (SequenceIteratorSafe< const DiscreteVariable* > varIter
         = initialState.variablesSequence().beginSafe();
         varIter != initialState.variablesSequence().endSafe();
         ++varIter)
      _checker_->add(**varIter);

    _nbVisitedStates_ = 1;
    _checkerFalseId_  = _checker_->manager()->addTerminalNode(false);
    _checkerTrueId_   = _checker_->manager()->addTerminalNode(true);

    _insertState_(initialState, 0, 0);
  }

  void StatesChecker::addState(const Instantiation& state) {
    _nbVisitedStates_++;

    NodeId parId   = _checker_->root();
    Idx    parModa = state.valFromPtr(_checker_->node(parId)->nodeVar());
    while (_checker_->node(parId)->son(parModa) != _checkerFalseId_) {
      parId   = _checker_->node(parId)->son(parModa);
      parModa = state.valFromPtr(_checker_->node(parId)->nodeVar());
    }
    _insertState_(state, parId, parModa);
  }

  void StatesChecker::_insertState_(const Instantiation& state, NodeId parentId, Idx parentModa) {
    Idx varIter = 0;
    if (parentId) varIter = state.variablesSequence().pos(_checker_->node(parentId)->nodeVar()) + 1;


    for (; varIter < state.variablesSequence().size(); ++varIter) {
      const DiscreteVariable* curVar = state.variablesSequence().atPos(varIter);
      NodeId                  varId  = _checker_->manager()->addInternalNode(curVar);
      if (parentId) _checker_->manager()->setSon(parentId, parentModa, varId);
      else _checker_->manager()->setRootNode(varId);
      for (Idx moda = 0; moda < curVar->domainSize(); ++moda) {
        if (moda == state.valFromPtr(curVar)) parentModa = moda;
        else _checker_->manager()->setSon(varId, moda, _checkerFalseId_);
      }
      parentId = varId;
    }
    _checker_->manager()->setSon(parentId, parentModa, _checkerTrueId_);
  }
}   // End of namespace gum
