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


#include <agrum/FMDP/simulation/statesChecker.h>


namespace gum {

  StatesChecker::StatesChecker() :
      nbVisitedStates__(0),
      checker__(MultiDimFunctionGraph< bool >::getTreeInstance()),
      checkerTrueId__(0), checkerFalseId__(0) {
    GUM_CONSTRUCTOR(StatesChecker);
  }

  StatesChecker::~StatesChecker() {
    delete checker__;
    GUM_DESTRUCTOR(StatesChecker);
  }

  void StatesChecker::reset(const Instantiation& initialState) {
    checker__->clear();
    for (SequenceIteratorSafe< const DiscreteVariable* > varIter =
            initialState.variablesSequence().beginSafe();
         varIter != initialState.variablesSequence().endSafe();
         ++varIter)
      checker__->add(**varIter);

    nbVisitedStates__ = 1;
    checkerFalseId__ = checker__->manager()->addTerminalNode(false);
    checkerTrueId__ = checker__->manager()->addTerminalNode(true);

    insertState__(initialState, 0, 0);
  }

  void StatesChecker::addState(const Instantiation& state) {
    nbVisitedStates__++;

    NodeId parId = checker__->root();
    Idx    parModa = state.valFromPtr(checker__->node(parId)->nodeVar());
    while (checker__->node(parId)->son(parModa) != checkerFalseId__) {
      parId = checker__->node(parId)->son(parModa);
      parModa = state.valFromPtr(checker__->node(parId)->nodeVar());
    }
    insertState__(state, parId, parModa);
  }

  void StatesChecker::insertState__(const Instantiation& state,
                                    NodeId               parentId,
                                    Idx                  parentModa) {
    Idx varIter = 0;
    if (parentId)
      varIter =
         state.variablesSequence().pos(checker__->node(parentId)->nodeVar()) + 1;


    for (; varIter < state.variablesSequence().size(); ++varIter) {
      const DiscreteVariable* curVar = state.variablesSequence().atPos(varIter);
      NodeId varId = checker__->manager()->addInternalNode(curVar);
      if (parentId)
        checker__->manager()->setSon(parentId, parentModa, varId);
      else
        checker__->manager()->setRootNode(varId);
      for (Idx moda = 0; moda < curVar->domainSize(); ++moda) {
        if (moda == state.valFromPtr(curVar))
          parentModa = moda;
        else
          checker__->manager()->setSon(varId, moda, checkerFalseId__);
      }
      parentId = varId;
    }
    checker__->manager()->setSon(parentId, parentModa, checkerTrueId__);
  }
}   // End of namespace gum
