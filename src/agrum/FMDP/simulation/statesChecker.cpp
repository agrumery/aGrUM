
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
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


#include <agrum/FMDP/simulation/statesChecker.h>


namespace gum {

  StatesChecker::StatesChecker() :
      __nbVisitedStates(0),
      __checker(MultiDimFunctionGraph< bool >::getTreeInstance()),
      __checkerTrueId(0), __checkerFalseId(0) {
    GUM_CONSTRUCTOR(StatesChecker);
  }

  StatesChecker::~StatesChecker() {
    delete __checker;
    GUM_DESTRUCTOR(StatesChecker);
  }

  void StatesChecker::reset(const Instantiation& initialState) {
    __checker->clear();
    for (SequenceIteratorSafe< const DiscreteVariable* > varIter =
            initialState.variablesSequence().beginSafe();
         varIter != initialState.variablesSequence().endSafe();
         ++varIter)
      __checker->add(**varIter);

    __nbVisitedStates = 1;
    __checkerFalseId = __checker->manager()->addTerminalNode(false);
    __checkerTrueId = __checker->manager()->addTerminalNode(true);

    __insertState(initialState, 0, 0);
  }

  void StatesChecker::addState(const Instantiation& state) {
    __nbVisitedStates++;

    NodeId parId = __checker->root();
    Idx    parModa = state.valFromPtr(__checker->node(parId)->nodeVar());
    while (__checker->node(parId)->son(parModa) != __checkerFalseId) {
      parId = __checker->node(parId)->son(parModa);
      parModa = state.valFromPtr(__checker->node(parId)->nodeVar());
    }
    __insertState(state, parId, parModa);
  }

  void StatesChecker::__insertState(const Instantiation& state,
                                    NodeId               parentId,
                                    Idx                  parentModa) {
    Idx varIter = 0;
    if (parentId)
      varIter =
         state.variablesSequence().pos(__checker->node(parentId)->nodeVar()) + 1;


    for (; varIter < state.variablesSequence().size(); ++varIter) {
      const DiscreteVariable* curVar = state.variablesSequence().atPos(varIter);
      NodeId varId = __checker->manager()->addInternalNode(curVar);
      if (parentId)
        __checker->manager()->setSon(parentId, parentModa, varId);
      else
        __checker->manager()->setRootNode(varId);
      for (Idx moda = 0; moda < curVar->domainSize(); ++moda) {
        if (moda == state.valFromPtr(curVar))
          parentModa = moda;
        else
          __checker->manager()->setSon(varId, moda, __checkerFalseId);
      }
      parentId = varId;
    }
    __checker->manager()->setSon(parentId, parentModa, __checkerTrueId);
  }
}   // End of namespace gum
