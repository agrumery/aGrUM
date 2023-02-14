/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
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
  INLINE Regress< GUM_SCALAR, COMBINEOPERATOR, PROJECTOPERATOR, TerminalNodePolicy >::Regress(
     const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* DG1,
     const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* DG2,
     const Set< const DiscreteVariable* >*                          primedVars,
     const DiscreteVariable*                                        targetVar,
     const GUM_SCALAR                                               neutral) :
      _DG1_(DG1),
      _DG2_(DG2), _neutral_(neutral), _combine_(), _project_(),
      _DG1InstantiationNeeded_(DG1->realSize(), true, false),
      _DG2InstantiationNeeded_(DG2->realSize(), true, false) {
    GUM_CONSTRUCTOR(Regress);
    _rd_ = MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >::getReducedAndOrderedInstance();
    _nbVar_      = 0;
    _default_    = nullptr;
    _primedVars_ = primedVars;
    _targetVar_  = targetVar;
  }

  template < typename GUM_SCALAR,
             template < typename >
             class COMBINEOPERATOR,
             template < typename >
             class PROJECTOPERATOR,
             template < typename >
             class TerminalNodePolicy >
  INLINE Regress< GUM_SCALAR, COMBINEOPERATOR, PROJECTOPERATOR, TerminalNodePolicy >::~Regress() {
    GUM_DESTRUCTOR(Regress);

    for (auto instIter = _DG1InstantiationNeeded_.beginSafe();
         instIter != _DG1InstantiationNeeded_.endSafe();
         ++instIter)
      DEALLOCATE(instIter.val(), sizeof(short int) * _nbVar_);

    for (auto instIter = _DG2InstantiationNeeded_.beginSafe();
         instIter != _DG2InstantiationNeeded_.endSafe();
         ++instIter)
      DEALLOCATE(instIter.val(), sizeof(short int) * _nbVar_);

    if (_nbVar_ != 0) DEALLOCATE(_default_, sizeof(short int) * _nbVar_);
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
         Regress< GUM_SCALAR, COMBINEOPERATOR, PROJECTOPERATOR, TerminalNodePolicy >::compute() {
    _establishVarOrder_();
    _findRetrogradeVariables_(_DG1_, _DG1InstantiationNeeded_);
    _findRetrogradeVariables_(_DG2_, _DG2InstantiationNeeded_);

    Idx* varInst = nullptr;
    if (_nbVar_ != 0) {
      varInst = static_cast< Idx* >(ALLOCATE(sizeof(Idx) * _nbVar_));
      for (Idx i = 0; i < _nbVar_; i++)
        varInst[i] = (Idx)0;
    }

    O4DGContext conti(varInst, _nbVar_);
    conti.setDG1Node(_DG1_->root());
    conti.setDG2Node(_DG2_->root());

    NodeId root = _compute_(conti, (Idx)0 - 1);
    _rd_->manager()->setRootNode(root);

    if (_nbVar_ != 0) DEALLOCATE(varInst, sizeof(Idx) * _nbVar_);

    _rd_->erase(*_targetVar_);

    return _rd_;
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
  INLINE void Regress< GUM_SCALAR, COMBINEOPERATOR, PROJECTOPERATOR, TerminalNodePolicy >::
     _establishVarOrder_() {
    SequenceIteratorSafe< const DiscreteVariable* > fite = _DG1_->variablesSequence().beginSafe();
    SequenceIteratorSafe< const DiscreteVariable* > site = _DG2_->variablesSequence().beginSafe();

    while (fite != _DG1_->variablesSequence().endSafe()
           && site != _DG2_->variablesSequence().endSafe()) {
      // Test : if var from first order is already in final order
      // we move onto the next one
      if (_rd_->variablesSequence().exists(*fite)) {
        ++fite;
        continue;
      }

      // Test : if var from second order is already in final order
      // we move onto the next one
      if (_rd_->variablesSequence().exists(*site)) {
        ++site;
        continue;
      }

      // Test : is current var of the first order present in the second order.
      // if not we add it to final order
      if (!_DG2_->variablesSequence().exists(*fite) && !_primedVars_->exists(*fite)) {
        _rd_->add(**fite);
        ++fite;
        continue;
      }

      // Test : is current var of the second order present in the first order.
      // if not we add it to final order
      if (!_DG1_->variablesSequence().exists(*site) && !_primedVars_->exists(*site)) {
        _rd_->add(**site);
        ++site;
        continue;
      }

      // Test : is current var of the second order present in the first order.
      // if not we add it to final order
      if (*fite == *site) {
        _rd_->add(**fite);
        ++fite;
        ++site;
        continue;
      }

      // Test : if chosing first order var cost less in terms or re exploration,
      // we chose it
      _rd_->add(**fite);
      ++fite;
    }

    // Whenever an iterator has finished its sequence,
    // the other may still be in the middle of its one.
    // Hence, this part ensures that any variables remaining
    // will be added to the final sequence if needed.
    if (fite == _DG1_->variablesSequence().endSafe()) {
      for (; site != _DG2_->variablesSequence().endSafe(); ++site)
        if (!_rd_->variablesSequence().exists(*site)) _rd_->add(**site);
    } else {
      for (; fite != _DG1_->variablesSequence().endSafe(); ++fite)
        if (!_rd_->variablesSequence().exists(*fite)) _rd_->add(**fite);
    }

    // Various initialization needed now that we have a bigger picture
    _nbVar_ = _rd_->variablesSequence().size();

    if (_nbVar_ != 0) {
      _default_ = static_cast< short int* >(ALLOCATE(sizeof(short int) * _nbVar_));
      for (Idx i = 0; i < _nbVar_; i++)
        _default_[i] = (short int)0;
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
  INLINE void Regress< GUM_SCALAR, COMBINEOPERATOR, PROJECTOPERATOR, TerminalNodePolicy >::
     _findRetrogradeVariables_(const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* dg,
                               HashTable< NodeId, short int* >& dgInstNeed) {
    HashTable< NodeId, short int* > nodesVarDescendant;
    Size                            tableSize = Size(_nbVar_ * sizeof(short int));

    for (auto varIter = dg->variablesSequence().rbeginSafe();
         varIter != dg->variablesSequence().rendSafe();
         --varIter) {
      Idx varPos = _rd_->variablesSequence().pos(*varIter);

      const Link< NodeId >* nodeIter = dg->varNodeListe(*varIter)->list();
      while (nodeIter != nullptr) {
        short int* instantiationNeeded = static_cast< short int* >(ALLOCATE(tableSize));
        dgInstNeed.insert(nodeIter->element(), instantiationNeeded);
        short int* varDescendant = static_cast< short int* >(ALLOCATE(tableSize));
        nodesVarDescendant.insert(nodeIter->element(), varDescendant);
        for (Idx j = 0; j < _nbVar_; j++) {
          instantiationNeeded[j] = (short int)0;
          varDescendant[j]       = (short int)0;
        }


        varDescendant[varPos] = (short int)1;
        for (Idx modality = 0; modality < dg->node(nodeIter->element())->nbSons(); ++modality) {
          if (!dg->isTerminalNode(dg->node(nodeIter->element())->son(modality))) {
            short int* sonVarDescendant
               = nodesVarDescendant[dg->node(nodeIter->element())->son(modality)];
            for (Idx varIdx = 0; varIdx < _nbVar_; varIdx++) {
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
        for (Idx modality = 0; modality < dg->node(nodeIter->element())->nbSons(); ++modality) {
          NodeId sonId = dg->node(nodeIter->element())->son(modality);
          if (!dg->isTerminalNode(sonId)) {
            for (Idx varIdx = 0; varIdx < _nbVar_; ++varIdx) {
              if (dgInstNeed[nodeIter->element()][varIdx] && nodesVarDescendant[sonId][varIdx]) {
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
     Regress< GUM_SCALAR, COMBINEOPERATOR, PROJECTOPERATOR, TerminalNodePolicy >::_compute_(
        O4DGContext& currentSituation,
        Idx          lastInstVarPos) {
    NodeId newNode = 0;

    // If both current nodes are terminal,
    // we only have to compute the resulting value
    if (_DG1_->isTerminalNode(currentSituation.DG1Node())
        && _DG2_->isTerminalNode(currentSituation.DG2Node())) {
      // We have to compute new valueand we insert a new node in diagram with
      // this value, ...
      GUM_SCALAR newVal  = _neutral_;
      GUM_SCALAR tempVal = _combine_(_DG1_->nodeValue(currentSituation.DG1Node()),
                                     _DG2_->nodeValue(currentSituation.DG2Node()));
      for (Idx targetModa = 0; targetModa < _targetVar_->domainSize(); ++targetModa)
        newVal = _project_(newVal, tempVal);
      return _rd_->manager()->addTerminalNode(newVal);
    }

    // If not,
    // we'll have to do some exploration

    // First we ensure that we hadn't already visit this pair of node under hte
    // same circumstances
    short int* dg1NeededVar = _DG1InstantiationNeeded_.exists(currentSituation.DG1Node())
                               ? _DG1InstantiationNeeded_[currentSituation.DG1Node()]
                               : _default_;
    Idx        dg1CurrentVarPos
       = _DG1_->isTerminalNode(currentSituation.DG1Node())
          ? _nbVar_
          : _rd_->variablesSequence().pos(_DG1_->node(currentSituation.DG1Node())->nodeVar());
    short int* dg2NeededVar = _DG2InstantiationNeeded_.exists(currentSituation.DG2Node())
                               ? _DG2InstantiationNeeded_[currentSituation.DG2Node()]
                               : _default_;
    Idx        dg2CurrentVarPos
       = _DG2_->isTerminalNode(currentSituation.DG2Node())
          ? _nbVar_
          : _rd_->variablesSequence().pos(_DG2_->node(currentSituation.DG2Node())->nodeVar());

    short int* instNeeded = static_cast< short int* >(ALLOCATE(sizeof(short int) * _nbVar_));

    for (Idx i = 0; i < _nbVar_; i++) {
      instNeeded[i] = dg1NeededVar[i] + dg2NeededVar[i];
    }

    double curSitKey = currentSituation.key(instNeeded);

    if (_explorationTable_.exists(curSitKey)) {
      DEALLOCATE(instNeeded, sizeof(short int) * _nbVar_);

      return _explorationTable_[curSitKey];
    }

    // ====================================================

    NodeId origDG1 = currentSituation.DG1Node(), origDG2 = currentSituation.DG2Node();

    const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* leaddg     = nullptr;
    NodeId                                                         leadNodeId = 0;
    Idx leadVarPos               = _rd_->variablesSequence().size();
    using SetNodeFunction        = void (O4DGContext::*)(const NodeId&);
    SetNodeFunction leadFunction = nullptr;

    bool sameVar = false;

    if (!_DG1_->isTerminalNode(currentSituation.DG1Node())) {
      if (currentSituation.varModality(dg1CurrentVarPos) != 0) {
        // If var associated to current node has already been instanciated, we
        // have to jump it
        currentSituation.setDG1Node(_DG1_->node(currentSituation.DG1Node())
                                       ->son(currentSituation.varModality(dg1CurrentVarPos) - 1));

        newNode = _compute_(currentSituation, lastInstVarPos);
        _explorationTable_.insert(curSitKey, newNode);
        currentSituation.setDG1Node(origDG1);
        currentSituation.setDG2Node(origDG2);

        DEALLOCATE(instNeeded, sizeof(short int) * _nbVar_);

        return newNode;
      }

      leaddg       = _DG1_;
      leadNodeId   = currentSituation.DG1Node();
      leadVarPos   = dg1CurrentVarPos;
      leadFunction = &O4DGContext::setDG1Node;
    }

    if (!_DG2_->isTerminalNode(currentSituation.DG2Node())) {
      if (currentSituation.varModality(dg2CurrentVarPos) != 0) {
        // If var associated to current node has already been instanciated, we
        // have to jump it
        currentSituation.setDG2Node(_DG2_->node(currentSituation.DG2Node())
                                       ->son(currentSituation.varModality(dg2CurrentVarPos) - 1));

        newNode = _compute_(currentSituation, lastInstVarPos);
        _explorationTable_.insert(curSitKey, newNode);
        currentSituation.setDG1Node(origDG1);
        currentSituation.setDG2Node(origDG2);

        DEALLOCATE(instNeeded, sizeof(short int) * _nbVar_);

        return newNode;
      }

      if (leadVarPos == dg2CurrentVarPos) { sameVar = true; }

      if (leadVarPos > dg2CurrentVarPos) {
        leaddg       = _DG2_;
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
        const DiscreteVariable* curVar = _rd_->variablesSequence().atPos(varPos);
        NodeId* sonsIds = static_cast< NodeId* >(ALLOCATE(sizeof(NodeId) * curVar->domainSize()));

        for (Idx modality = 0; modality < curVar->domainSize(); modality++) {
          currentSituation.chgVarModality(varPos, modality + 1);

          sonsIds[modality] = _compute_(currentSituation, varPos);
        }

        newNode = _rd_->manager()->addInternalNode(curVar, sonsIds);

        _explorationTable_.insert(curSitKey, newNode);
        currentSituation.chgVarModality(varPos, 0);
        currentSituation.setDG1Node(origDG1);
        currentSituation.setDG2Node(origDG2);

        DEALLOCATE(instNeeded, sizeof(short int) * _nbVar_);

        return newNode;
      }
    }

    // ====================================================
    // Terminal Exploration
    if (sameVar && _DG1_->node(origDG1)->nodeVar() == _targetVar_) {
      GUM_SCALAR newVal = _neutral_;
      for (Idx targetModa = 0; targetModa < _targetVar_->domainSize(); ++targetModa)
        newVal = _project_(newVal,
                           _combine_(_DG1_->nodeValue(_DG1_->node(origDG1)->son(targetModa)),
                                     _DG2_->nodeValue(_DG2_->node(origDG2)->son(targetModa))));
      newNode = _rd_->manager()->addTerminalNode(newVal);
      _explorationTable_.insert(curSitKey, newNode);
      DEALLOCATE(instNeeded, sizeof(short int) * _nbVar_);
      return newNode;
    }
    if (_DG1_->isTerminalNode(origDG1)) {
      if (_DG2_->node(origDG2)->nodeVar() == _targetVar_) {
        GUM_SCALAR newVal = _neutral_;
        for (Idx targetModa = 0; targetModa < _targetVar_->domainSize(); ++targetModa)
          newVal = _project_(newVal,
                             _combine_(_DG1_->nodeValue(origDG1),
                                       _DG2_->nodeValue(_DG2_->node(origDG2)->son(targetModa))));
        newNode = _rd_->manager()->addTerminalNode(newVal);
        _explorationTable_.insert(curSitKey, newNode);
        DEALLOCATE(instNeeded, sizeof(short int) * _nbVar_);
        return newNode;
      }
    } else {
      if (_DG1_->node(origDG1)->nodeVar() == _targetVar_ && _DG2_->isTerminalNode(origDG2)) {
        GUM_SCALAR newVal = _neutral_;
        for (Idx targetModa = 0; targetModa < _targetVar_->domainSize(); ++targetModa)
          newVal = _project_(newVal,
                             _combine_(_DG1_->nodeValue(_DG1_->node(origDG1)->son(targetModa)),
                                       _DG2_->nodeValue(origDG2)));
        newNode = _rd_->manager()->addTerminalNode(newVal);
        _explorationTable_.insert(curSitKey, newNode);
        DEALLOCATE(instNeeded, sizeof(short int) * _nbVar_);
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
      const InternalNode* dg1Node = _DG1_->node(origDG1);
      const InternalNode* dg2Node = _DG2_->node(origDG2);

      const DiscreteVariable* curVar = dg1Node->nodeVar();
      Idx                     varPos = _rd_->variablesSequence().pos(curVar);
      NodeId* sonsIds = static_cast< NodeId* >(ALLOCATE(sizeof(NodeId) * curVar->domainSize()));

      for (Idx modality = 0; modality < curVar->domainSize(); modality++) {
        currentSituation.chgVarModality(varPos, modality + 1);
        currentSituation.setDG1Node(dg1Node->son(modality));
        currentSituation.setDG2Node(dg2Node->son(modality));

        sonsIds[modality] = _compute_(currentSituation, varPos);
      }

      newNode = _rd_->manager()->addInternalNode(curVar, sonsIds);

      _explorationTable_.insert(curSitKey, newNode);
      currentSituation.chgVarModality(varPos, 0);
      currentSituation.setDG1Node(origDG1);
      currentSituation.setDG2Node(origDG2);

      DEALLOCATE(instNeeded, sizeof(short int) * _nbVar_);

      return newNode;
    }
    // ====================================================
    else {
      const InternalNode* leaddgNode = leaddg->node(leadNodeId);

      const DiscreteVariable* curVar = leaddgNode->nodeVar();
      NodeId* sonsIds = static_cast< NodeId* >(ALLOCATE(sizeof(NodeId) * curVar->domainSize()));

      for (Idx modality = 0; modality < curVar->domainSize(); modality++) {
        currentSituation.chgVarModality(leadVarPos, modality + 1);
        (currentSituation.*leadFunction)(leaddgNode->son(modality));

        sonsIds[modality] = _compute_(currentSituation, leadVarPos);
      }

      newNode = _rd_->manager()->addInternalNode(curVar, sonsIds);

      _explorationTable_.insert(curSitKey, newNode);
      currentSituation.chgVarModality(leadVarPos, 0);
      currentSituation.setDG1Node(origDG1);
      currentSituation.setDG2Node(origDG2);

      DEALLOCATE(instNeeded, sizeof(short int) * _nbVar_);

      return newNode;
    }
  }

}   // namespace gum
