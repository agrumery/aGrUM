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
 * @brief Class used to compute the operation between two decision diagrams
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 * @author Pierre-Henri WUILLEMIN(@LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(@AMU)
 */

#include <agrum/tools/multidim/utils/FunctionGraphUtilities/internalNode.h>
#include <agrum/tools/multidim/utils/FunctionGraphUtilities/operators/regress.h>

#define ALLOCATE(x)      SmallObjectAllocator::instance().allocate(x)
#define DEALLOCATE(x, y) SmallObjectAllocator::instance().deallocate(x, y)

namespace gum {

  template < typename GUM_SCALAR,
             template < typename >
             class COMBINEOPERATOR,
             template < typename >
             class PROJECTOPERATOR,
             template < typename >
             class TerminalNodePolicy >
  INLINE
     Regress< GUM_SCALAR, COMBINEOPERATOR, PROJECTOPERATOR, TerminalNodePolicy >::
        Regress(const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* DG1,
                const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* DG2,
                const Set< const DiscreteVariable* >* primedVars,
                const DiscreteVariable*               targetVar,
                const GUM_SCALAR                      neutral) :
      DG1__(DG1),
      DG2__(DG2), neutral__(neutral), combine__(), project__(),
      DG1InstantiationNeeded__(DG1->realSize(), true, false),
      DG2InstantiationNeeded__(DG2->realSize(), true, false) {
    GUM_CONSTRUCTOR(Regress);
    rd__ = MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >::
       getReducedAndOrderedInstance();
    nbVar__      = 0;
    default__    = nullptr;
    primedVars__ = primedVars;
    targetVar__  = targetVar;
  }

  template < typename GUM_SCALAR,
             template < typename >
             class COMBINEOPERATOR,
             template < typename >
             class PROJECTOPERATOR,
             template < typename >
             class TerminalNodePolicy >
  INLINE
     Regress< GUM_SCALAR, COMBINEOPERATOR, PROJECTOPERATOR, TerminalNodePolicy >::
        ~Regress() {
    GUM_DESTRUCTOR(Regress);

    for (auto instIter = DG1InstantiationNeeded__.beginSafe();
         instIter != DG1InstantiationNeeded__.endSafe();
         ++instIter)
      DEALLOCATE(instIter.val(), sizeof(short int) * nbVar__);

    for (auto instIter = DG2InstantiationNeeded__.beginSafe();
         instIter != DG2InstantiationNeeded__.endSafe();
         ++instIter)
      DEALLOCATE(instIter.val(), sizeof(short int) * nbVar__);

    if (nbVar__ != 0) DEALLOCATE(default__, sizeof(short int) * nbVar__);
  }


  // This function is the main function. To be call every time an operation
  // between the two given Function Graphs is required
  template < typename GUM_SCALAR,
             template < typename >
             class COMBINEOPERATOR,
             template < typename >
             class PROJECTOPERATOR,
             template < typename >
             class TerminalNodePolicy >
  INLINE MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >*
     Regress< GUM_SCALAR, COMBINEOPERATOR, PROJECTOPERATOR, TerminalNodePolicy >::
        compute() {
    establishVarOrder__();
    findRetrogradeVariables__(DG1__, DG1InstantiationNeeded__);
    findRetrogradeVariables__(DG2__, DG2InstantiationNeeded__);

    Idx* varInst = nullptr;
    if (nbVar__ != 0) {
      varInst = static_cast< Idx* >(ALLOCATE(sizeof(Idx) * nbVar__));
      for (Idx i = 0; i < nbVar__; i++)
        varInst[i] = (Idx)0;
    }

    O4DGContext conti(varInst, nbVar__);
    conti.setDG1Node(DG1__->root());
    conti.setDG2Node(DG2__->root());

    NodeId root = compute__(conti, (Idx)0 - 1);
    rd__->manager()->setRootNode(root);

    if (nbVar__ != 0) DEALLOCATE(varInst, sizeof(Idx) * nbVar__);

    rd__->erase(*targetVar__);

    return rd__;
  }

  // This function computes an efficient order for the final decision diagrams.
  // Its main criterion to do so is the number of
  // re-exploration to be done
  template < typename GUM_SCALAR,
             template < typename >
             class COMBINEOPERATOR,
             template < typename >
             class PROJECTOPERATOR,
             template < typename >
             class TerminalNodePolicy >
  INLINE void
     Regress< GUM_SCALAR, COMBINEOPERATOR, PROJECTOPERATOR, TerminalNodePolicy >::
        establishVarOrder__() {
    SequenceIteratorSafe< const DiscreteVariable* > fite
       = DG1__->variablesSequence().beginSafe();
    SequenceIteratorSafe< const DiscreteVariable* > site
       = DG2__->variablesSequence().beginSafe();

    while (fite != DG1__->variablesSequence().endSafe()
           && site != DG2__->variablesSequence().endSafe()) {
      // Test : if var from first order is already in final order
      // we move onto the next one
      if (rd__->variablesSequence().exists(*fite)) {
        ++fite;
        continue;
      }

      // Test : if var from second order is already in final order
      // we move onto the next one
      if (rd__->variablesSequence().exists(*site)) {
        ++site;
        continue;
      }

      // Test : is current var of the first order present in the second order.
      // if not we add it to final order
      if (!DG2__->variablesSequence().exists(*fite)
          && !primedVars__->exists(*fite)) {
        rd__->add(**fite);
        ++fite;
        continue;
      }

      // Test : is current var of the second order present in the first order.
      // if not we add it to final order
      if (!DG1__->variablesSequence().exists(*site)
          && !primedVars__->exists(*site)) {
        rd__->add(**site);
        ++site;
        continue;
      }

      // Test : is current var of the second order present in the first order.
      // if not we add it to final order
      if (*fite == *site) {
        rd__->add(**fite);
        ++fite;
        ++site;
        continue;
      }

      // Test : if chosing first order var cost less in terms or re exploration,
      // we chose it
      rd__->add(**fite);
      ++fite;
    }

    // Whenever an iterator has finished its sequence,
    // the other may still be in the middle of its one.
    // Hence, this part ensures that any variables remaining
    // will be added to the final sequence if needed.
    if (fite == DG1__->variablesSequence().endSafe()) {
      for (; site != DG2__->variablesSequence().endSafe(); ++site)
        if (!rd__->variablesSequence().exists(*site)) rd__->add(**site);
    } else {
      for (; fite != DG1__->variablesSequence().endSafe(); ++fite)
        if (!rd__->variablesSequence().exists(*fite)) rd__->add(**fite);
    }

    // Various initialization needed now that we have a bigger picture
    nbVar__ = rd__->variablesSequence().size();

    if (nbVar__ != 0) {
      default__ = static_cast< short int* >(ALLOCATE(sizeof(short int) * nbVar__));
      for (Idx i = 0; i < nbVar__; i++)
        default__[i] = (short int)0;
    }
  }

  // This function computes for every nodes if any retrograde variable is
  // present below
  template < typename GUM_SCALAR,
             template < typename >
             class COMBINEOPERATOR,
             template < typename >
             class PROJECTOPERATOR,
             template < typename >
             class TerminalNodePolicy >
  INLINE void
     Regress< GUM_SCALAR, COMBINEOPERATOR, PROJECTOPERATOR, TerminalNodePolicy >::
        findRetrogradeVariables__(
           const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* dg,
           HashTable< NodeId, short int* >& dgInstNeed) {
    HashTable< NodeId, short int* > nodesVarDescendant;
    Size                            tableSize = Size(nbVar__ * sizeof(short int));

    for (auto varIter = dg->variablesSequence().rbeginSafe();
         varIter != dg->variablesSequence().rendSafe();
         --varIter) {
      Idx varPos = rd__->variablesSequence().pos(*varIter);

      const Link< NodeId >* nodeIter = dg->varNodeListe(*varIter)->list();
      while (nodeIter != nullptr) {
        short int* instantiationNeeded
           = static_cast< short int* >(ALLOCATE(tableSize));
        dgInstNeed.insert(nodeIter->element(), instantiationNeeded);
        short int* varDescendant = static_cast< short int* >(ALLOCATE(tableSize));
        nodesVarDescendant.insert(nodeIter->element(), varDescendant);
        for (Idx j = 0; j < nbVar__; j++) {
          instantiationNeeded[j] = (short int)0;
          varDescendant[j]       = (short int)0;
        }


        varDescendant[varPos] = (short int)1;
        for (Idx modality = 0; modality < dg->node(nodeIter->element())->nbSons();
             ++modality) {
          if (!dg->isTerminalNode(dg->node(nodeIter->element())->son(modality))) {
            short int* sonVarDescendant
               = nodesVarDescendant[dg->node(nodeIter->element())->son(modality)];
            for (Idx varIdx = 0; varIdx < nbVar__; varIdx++) {
              varDescendant[varIdx] += sonVarDescendant[varIdx];
              if (varDescendant[varIdx] && varIdx < varPos)
                instantiationNeeded[varIdx] = (short int)1;
            }
          }
        }
        nodeIter = nodeIter->nextLink();
      }
    }

    for (auto varIter = dg->variablesSequence().beginSafe();
         varIter != dg->variablesSequence().endSafe();
         ++varIter) {
      const Link< NodeId >* nodeIter = dg->varNodeListe(*varIter)->list();
      while (nodeIter != nullptr) {
        for (Idx modality = 0; modality < dg->node(nodeIter->element())->nbSons();
             ++modality) {
          NodeId sonId = dg->node(nodeIter->element())->son(modality);
          if (!dg->isTerminalNode(sonId)) {
            for (Idx varIdx = 0; varIdx < nbVar__; ++varIdx) {
              if (dgInstNeed[nodeIter->element()][varIdx]
                  && nodesVarDescendant[sonId][varIdx]) {
                dgInstNeed[sonId][varIdx] = (short int)1;
              }
            }
          }
        }
        nodeIter = nodeIter->nextLink();
      }
    }

    for (HashTableIterator< NodeId, short int* > it = nodesVarDescendant.begin();
         it != nodesVarDescendant.end();
         ++it) {
      DEALLOCATE(it.val(), tableSize);
    }

    nodesVarDescendant.clear();
  }

  // A key is used for prunning uneccesary operations since once a node has been
  // visited in a given context, there's no use to revisit him,
  // the result will be the same node, so we just have to do an association
  // context - node.
  // The context consists in :
  //              _ Leader node we are visiting.
  //              _ Follower node we are visiting.
  //              _ For all retrograde variables, if it has been instanciated
  //              before, current modality instanciated, meaning :
  //                      _ 0 means the variable hasn't be instanciated yet,
  //                      _ From 1 to domainSize + 1 means that current modality
  //                      index of variable is value - 1,
  //                      _ domainSize + 2 means variable is on default mode.
  // A key - node association is made each time we create a node in resulting
  // diagram.
  // Since GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION is a corner step in
  // algorithm ( meaning each time we explore a node we go trought
  // this function ), check only have to be at the beginning of that function.
  template < typename GUM_SCALAR,
             template < typename >
             class COMBINEOPERATOR,
             template < typename >
             class PROJECTOPERATOR,
             template < typename >
             class TerminalNodePolicy >
  INLINE NodeId
     Regress< GUM_SCALAR, COMBINEOPERATOR, PROJECTOPERATOR, TerminalNodePolicy >::
        compute__(O4DGContext& currentSituation, Idx lastInstVarPos) {
    NodeId newNode = 0;

    // If both current nodes are terminal,
    // we only have to compute the resulting value
    if (DG1__->isTerminalNode(currentSituation.DG1Node())
        && DG2__->isTerminalNode(currentSituation.DG2Node())) {
      // We have to compute new valueand we insert a new node in diagram with
      // this value, ...
      GUM_SCALAR newVal  = neutral__;
      GUM_SCALAR tempVal = combine__(DG1__->nodeValue(currentSituation.DG1Node()),
                                     DG2__->nodeValue(currentSituation.DG2Node()));
      for (Idx targetModa = 0; targetModa < targetVar__->domainSize();
           ++targetModa)
        newVal = project__(newVal, tempVal);
      return rd__->manager()->addTerminalNode(newVal);
    }

    // If not,
    // we'll have to do some exploration

    // First we ensure that we hadn't already visit this pair of node under hte
    // same circumstances
    short int* dg1NeededVar
       = DG1InstantiationNeeded__.exists(currentSituation.DG1Node())
          ? DG1InstantiationNeeded__[currentSituation.DG1Node()]
          : default__;
    Idx        dg1CurrentVarPos = DG1__->isTerminalNode(currentSituation.DG1Node())
                                   ? nbVar__
                                   : rd__->variablesSequence().pos(
                               DG1__->node(currentSituation.DG1Node())->nodeVar());
    short int* dg2NeededVar
       = DG2InstantiationNeeded__.exists(currentSituation.DG2Node())
          ? DG2InstantiationNeeded__[currentSituation.DG2Node()]
          : default__;
    Idx dg2CurrentVarPos = DG2__->isTerminalNode(currentSituation.DG2Node())
                            ? nbVar__
                            : rd__->variablesSequence().pos(
                               DG2__->node(currentSituation.DG2Node())->nodeVar());

    short int* instNeeded
       = static_cast< short int* >(ALLOCATE(sizeof(short int) * nbVar__));

    for (Idx i = 0; i < nbVar__; i++) {
      instNeeded[i] = dg1NeededVar[i] + dg2NeededVar[i];
    }

    double curSitKey = currentSituation.key(instNeeded);

    if (explorationTable__.exists(curSitKey)) {
      DEALLOCATE(instNeeded, sizeof(short int) * nbVar__);

      return explorationTable__[curSitKey];
    }

    // ====================================================

    NodeId origDG1 = currentSituation.DG1Node(),
           origDG2 = currentSituation.DG2Node();

    const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* leaddg
       = nullptr;
    NodeId leadNodeId = 0;
    Idx    leadVarPos = rd__->variablesSequence().size();
    typedef void (O4DGContext::*SetNodeFunction)(const NodeId&);
    SetNodeFunction leadFunction = nullptr;

    bool sameVar = false;

    if (!DG1__->isTerminalNode(currentSituation.DG1Node())) {
      if (currentSituation.varModality(dg1CurrentVarPos) != 0) {
        // If var associated to current node has already been instanciated, we
        // have to jump it
        currentSituation.setDG1Node(
           DG1__->node(currentSituation.DG1Node())
              ->son(currentSituation.varModality(dg1CurrentVarPos) - 1));

        newNode = compute__(currentSituation, lastInstVarPos);
        explorationTable__.insert(curSitKey, newNode);
        currentSituation.setDG1Node(origDG1);
        currentSituation.setDG2Node(origDG2);

        DEALLOCATE(instNeeded, sizeof(short int) * nbVar__);

        return newNode;
      }

      leaddg       = DG1__;
      leadNodeId   = currentSituation.DG1Node();
      leadVarPos   = dg1CurrentVarPos;
      leadFunction = &O4DGContext::setDG1Node;
    }

    if (!DG2__->isTerminalNode(currentSituation.DG2Node())) {
      if (currentSituation.varModality(dg2CurrentVarPos) != 0) {
        // If var associated to current node has already been instanciated, we
        // have to jump it
        currentSituation.setDG2Node(
           DG2__->node(currentSituation.DG2Node())
              ->son(currentSituation.varModality(dg2CurrentVarPos) - 1));

        newNode = compute__(currentSituation, lastInstVarPos);
        explorationTable__.insert(curSitKey, newNode);
        currentSituation.setDG1Node(origDG1);
        currentSituation.setDG2Node(origDG2);

        DEALLOCATE(instNeeded, sizeof(short int) * nbVar__);

        return newNode;
      }

      if (leadVarPos == dg2CurrentVarPos) { sameVar = true; }

      if (leadVarPos > dg2CurrentVarPos) {
        leaddg       = DG2__;
        leadNodeId   = currentSituation.DG2Node();
        leadVarPos   = dg2CurrentVarPos;
        leadFunction = &O4DGContext::setDG2Node;
      }
    }

    // ====================================================
    // Anticipated Exploration

    // Before exploring nodes, we have to ensure that every anticipated
    // exploration is done
    for (Idx varPos = lastInstVarPos + 1; varPos < leadVarPos; ++varPos) {
      if (instNeeded[varPos]) {
        const DiscreteVariable* curVar  = rd__->variablesSequence().atPos(varPos);
        NodeId*                 sonsIds = static_cast< NodeId* >(
           ALLOCATE(sizeof(NodeId) * curVar->domainSize()));

        for (Idx modality = 0; modality < curVar->domainSize(); modality++) {
          currentSituation.chgVarModality(varPos, modality + 1);

          sonsIds[modality] = compute__(currentSituation, varPos);
        }

        newNode = rd__->manager()->addInternalNode(curVar, sonsIds);

        explorationTable__.insert(curSitKey, newNode);
        currentSituation.chgVarModality(varPos, 0);
        currentSituation.setDG1Node(origDG1);
        currentSituation.setDG2Node(origDG2);

        DEALLOCATE(instNeeded, sizeof(short int) * nbVar__);

        return newNode;
      }
    }

    // ====================================================
    // Terminal Exploration
    if (sameVar && DG1__->node(origDG1)->nodeVar() == targetVar__) {
      GUM_SCALAR newVal = neutral__;
      for (Idx targetModa = 0; targetModa < targetVar__->domainSize();
           ++targetModa)
        newVal = project__(
           newVal,
           combine__(DG1__->nodeValue(DG1__->node(origDG1)->son(targetModa)),
                     DG2__->nodeValue(DG2__->node(origDG2)->son(targetModa))));
      newNode = rd__->manager()->addTerminalNode(newVal);
      explorationTable__.insert(curSitKey, newNode);
      DEALLOCATE(instNeeded, sizeof(short int) * nbVar__);
      return newNode;
    }
    if (DG1__->isTerminalNode(origDG1)) {
      if (DG2__->node(origDG2)->nodeVar() == targetVar__) {
        GUM_SCALAR newVal = neutral__;
        for (Idx targetModa = 0; targetModa < targetVar__->domainSize();
             ++targetModa)
          newVal = project__(
             newVal,
             combine__(DG1__->nodeValue(origDG1),
                       DG2__->nodeValue(DG2__->node(origDG2)->son(targetModa))));
        newNode = rd__->manager()->addTerminalNode(newVal);
        explorationTable__.insert(curSitKey, newNode);
        DEALLOCATE(instNeeded, sizeof(short int) * nbVar__);
        return newNode;
      }
    } else {
      if (DG1__->node(origDG1)->nodeVar() == targetVar__
          && DG2__->isTerminalNode(origDG2)) {
        GUM_SCALAR newVal = neutral__;
        for (Idx targetModa = 0; targetModa < targetVar__->domainSize();
             ++targetModa)
          newVal = project__(
             newVal,
             combine__(DG1__->nodeValue(DG1__->node(origDG1)->son(targetModa)),
                       DG2__->nodeValue(origDG2)));
        newNode = rd__->manager()->addTerminalNode(newVal);
        explorationTable__.insert(curSitKey, newNode);
        DEALLOCATE(instNeeded, sizeof(short int) * nbVar__);
        return newNode;
      }
    }

    // ====================================================
    // Normal Exploration

    // If only one of the current node is terminal,
    // we have to pursue deeper on the other diagram
    if (sameVar) {
      // If so - meaning it's the same variable - we have to go
      // down on both
      const InternalNode* dg1Node = DG1__->node(origDG1);
      const InternalNode* dg2Node = DG2__->node(origDG2);

      const DiscreteVariable* curVar = dg1Node->nodeVar();
      Idx                     varPos = rd__->variablesSequence().pos(curVar);
      NodeId*                 sonsIds
         = static_cast< NodeId* >(ALLOCATE(sizeof(NodeId) * curVar->domainSize()));

      for (Idx modality = 0; modality < curVar->domainSize(); modality++) {
        currentSituation.chgVarModality(varPos, modality + 1);
        currentSituation.setDG1Node(dg1Node->son(modality));
        currentSituation.setDG2Node(dg2Node->son(modality));

        sonsIds[modality] = compute__(currentSituation, varPos);
      }

      newNode = rd__->manager()->addInternalNode(curVar, sonsIds);

      explorationTable__.insert(curSitKey, newNode);
      currentSituation.chgVarModality(varPos, 0);
      currentSituation.setDG1Node(origDG1);
      currentSituation.setDG2Node(origDG2);

      DEALLOCATE(instNeeded, sizeof(short int) * nbVar__);

      return newNode;
    }
    // ====================================================
    else {
      const InternalNode* leaddgNode = leaddg->node(leadNodeId);

      const DiscreteVariable* curVar = leaddgNode->nodeVar();
      NodeId*                 sonsIds
         = static_cast< NodeId* >(ALLOCATE(sizeof(NodeId) * curVar->domainSize()));

      for (Idx modality = 0; modality < curVar->domainSize(); modality++) {
        currentSituation.chgVarModality(leadVarPos, modality + 1);
        (currentSituation.*leadFunction)(leaddgNode->son(modality));

        sonsIds[modality] = compute__(currentSituation, leadVarPos);
      }

      newNode = rd__->manager()->addInternalNode(curVar, sonsIds);

      explorationTable__.insert(curSitKey, newNode);
      currentSituation.chgVarModality(leadVarPos, 0);
      currentSituation.setDG1Node(origDG1);
      currentSituation.setDG2Node(origDG2);

      DEALLOCATE(instNeeded, sizeof(short int) * nbVar__);

      return newNode;
    }
  }

}   // namespace gum
