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


#include <agrum/FMDP/simulation/statesCounter.h>

namespace gum {


  // =================================================================================================
  //
  // =================================================================================================
  StatesCounter::StatesCounter() : _counter_(MultiDimFunctionGraph< Size >::getTreeInstance()) {
    GUM_CONSTRUCTOR(StatesCounter);
  }

  // =================================================================================================
  //
  // =================================================================================================
  StatesCounter::~StatesCounter() {
    delete _counter_;
    GUM_DESTRUCTOR(StatesCounter);
  }

  // =================================================================================================
  //
  // =================================================================================================
  void StatesCounter::reset(const Instantiation& initialState) {
    _counter_->clear();
    for (SequenceIteratorSafe< const DiscreteVariable* > varIter
         = initialState.variablesSequence().beginSafe();
         varIter != initialState.variablesSequence().endSafe();
         ++varIter)
      _counter_->add(**varIter);

    _counter_->manager()->setRootNode(_counter_->manager()->addTerminalNode(0));

    _incState_(initialState, 0, 0, 0);
  }

  // =================================================================================================
  //
  // =================================================================================================
  void StatesCounter::incState(const Instantiation& state) {
    Idx nbVisits = _counter_->get(state);

    NodeId parId   = _counter_->root();
    Idx    parModa = state.valFromPtr(_counter_->node(parId)->nodeVar());
    while (!_counter_->isTerminalNode(_counter_->node(parId)->son(parModa))) {
      parId   = _counter_->node(parId)->son(parModa);
      parModa = state.valFromPtr(_counter_->node(parId)->nodeVar());
    }
    _incState_(state, parId, parModa, nbVisits);
  }

  // =================================================================================================
  //
  // =================================================================================================
  void StatesCounter::_incState_(const Instantiation& state,
                                 NodeId               parentId,
                                 Idx                  parentModa,
                                 Size                 nbVisits) {
    Idx varIter = 0;
    if (parentId) varIter = state.variablesSequence().pos(_counter_->node(parentId)->nodeVar()) + 1;


    for (; varIter < state.variablesSequence().size(); ++varIter) {
      const DiscreteVariable* curVar = state.variablesSequence().atPos(varIter);
      NodeId                  varId  = _counter_->manager()->addInternalNode(curVar);
      if (parentId) _counter_->manager()->setSon(parentId, parentModa, varId);
      else _counter_->manager()->setRootNode(varId);
      for (Idx moda = 0; moda < curVar->domainSize(); ++moda) {
        if (moda == state.valFromPtr(curVar)) parentModa = moda;
        else _counter_->manager()->setSon(varId, moda, _counter_->terminalNodeId(nbVisits));
      }
      parentId = varId;
    }
    _counter_->manager()->setSon(parentId,
                                 parentModa,
                                 _counter_->manager()->addTerminalNode(nbVisits + 1));
  }
}   // End of namespace gum
