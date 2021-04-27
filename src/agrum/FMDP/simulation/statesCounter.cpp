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


#include <agrum/FMDP/simulation/statesCounter.h>


namespace gum {


  // =================================================================================================
  //
  // =================================================================================================
  StatesCounter::StatesCounter() :
      counter__(MultiDimFunctionGraph< Size >::getTreeInstance()) {
    GUM_CONSTRUCTOR(StatesCounter);
  }


  // =================================================================================================
  //
  // =================================================================================================
  StatesCounter::~StatesCounter() {
    delete counter__;
    GUM_DESTRUCTOR(StatesCounter);
  }


  // =================================================================================================
  //
  // =================================================================================================
  void StatesCounter::reset(const Instantiation& initialState) {
    counter__->clear();
    for (SequenceIteratorSafe< const DiscreteVariable* > varIter
         = initialState.variablesSequence().beginSafe();
         varIter != initialState.variablesSequence().endSafe();
         ++varIter)
      counter__->add(**varIter);

    counter__->manager()->setRootNode(counter__->manager()->addTerminalNode(0));

    incState__(initialState, 0, 0, 0);
  }


  // =================================================================================================
  //
  // =================================================================================================
  void StatesCounter::incState(const Instantiation& state) {
    Idx nbVisits = counter__->get(state);

    NodeId parId   = counter__->root();
    Idx    parModa = state.valFromPtr(counter__->node(parId)->nodeVar());
    while (!counter__->isTerminalNode(counter__->node(parId)->son(parModa))) {
      parId   = counter__->node(parId)->son(parModa);
      parModa = state.valFromPtr(counter__->node(parId)->nodeVar());
    }
    incState__(state, parId, parModa, nbVisits);
  }


  // =================================================================================================
  //
  // =================================================================================================
  void StatesCounter::incState__(const Instantiation& state,
                                 NodeId               parentId,
                                 Idx                  parentModa,
                                 Size                 nbVisits) {
    Idx varIter = 0;
    if (parentId)
      varIter
         = state.variablesSequence().pos(counter__->node(parentId)->nodeVar()) + 1;


    for (; varIter < state.variablesSequence().size(); ++varIter) {
      const DiscreteVariable* curVar = state.variablesSequence().atPos(varIter);
      NodeId varId = counter__->manager()->addInternalNode(curVar);
      if (parentId)
        counter__->manager()->setSon(parentId, parentModa, varId);
      else
        counter__->manager()->setRootNode(varId);
      for (Idx moda = 0; moda < curVar->domainSize(); ++moda) {
        if (moda == state.valFromPtr(curVar))
          parentModa = moda;
        else
          counter__->manager()->setSon(varId,
                                       moda,
                                       counter__->terminalNodeId(nbVisits));
      }
      parentId = varId;
    }
    counter__->manager()->setSon(
       parentId,
       parentModa,
       counter__->manager()->addTerminalNode(nbVisits + 1));
  }
}   // End of namespace gum
