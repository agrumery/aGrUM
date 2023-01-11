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
 * @brief Class used to compute the projection of a function graph
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/tools/multidim/utils/FunctionGraphUtilities/internalNode.h>
#include <agrum/tools/multidim/utils/FunctionGraphUtilities/operators/multiDimFunctionGraphProjector.h>
#include <agrum/tools/variables/discreteVariable.h>

namespace gum {

  // CONSTRUCTOR
  template < typename GUM_SCALAR,
             template < typename >
             class FUNCTOR,
             template < typename >
             class TerminalNodePolicy >
  MultiDimFunctionGraphProjector< GUM_SCALAR, FUNCTOR, TerminalNodePolicy >::
     MultiDimFunctionGraphProjector(
        const MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >* src,
        const Set< const DiscreteVariable* >&                          delVars,
        const GUM_SCALAR                                               neutral) :
      _src_(src),
      _delVars_(delVars), _function_(), _neutral_(neutral) {
    GUM_CONSTRUCTOR(MultiDimFunctionGraphProjector);
    _rd_ = MultiDimFunctionGraph< GUM_SCALAR >::getReducedAndOrderedInstance();
  }


  // DESTRUCTOR
  template < typename GUM_SCALAR,
             template < typename >
             class FUNCTOR,
             template < typename >
             class TerminalNodePolicy >
  MultiDimFunctionGraphProjector< GUM_SCALAR, FUNCTOR, TerminalNodePolicy >::
     ~MultiDimFunctionGraphProjector() {
    GUM_DESTRUCTOR(MultiDimFunctionGraphProjector);
  }


  // This function is the main function. To be call every time an Projection
  // between the two given Function Graphs is required
  template < typename GUM_SCALAR,
             template < typename >
             class FUNCTOR,
             template < typename >
             class TerminalNodePolicy >
  MultiDimFunctionGraph< GUM_SCALAR, TerminalNodePolicy >*
     MultiDimFunctionGraphProjector< GUM_SCALAR, FUNCTOR, TerminalNodePolicy >::project() {
    _rd_->copy(*_src_);

    for (SetIteratorSafe< const DiscreteVariable* > varIter = _delVars_.beginSafe();
         varIter != _delVars_.endSafe();
         ++varIter) {
      const DiscreteVariable* curVar = *varIter;

      // Tout d'abord, on déplace la variable à projeter en fin de séquence afin
      // de simplifier la projection
      if (_rd_->variablesSequence().exists(curVar))
        _rd_->manager()->moveTo(curVar, _rd_->variablesSequence().size() - 1);

      // 1er cas spécial : le diagramme est un un simple noeud terminal
      if (_rd_->isTerminalNode(_rd_->root())) {
        GUM_SCALAR newVal = _neutral_, oldVal = _rd_->nodeValue(_rd_->root());
        for (Idx curVarModality = 0; curVarModality < curVar->domainSize(); ++curVarModality)
          newVal = _function_(newVal, oldVal);

        NodeId newSonId = _rd_->manager()->addTerminalNode(newVal);
        _rd_->manager()->setRootNode(newSonId);

        if (_rd_->variablesSequence().exists(curVar)) _rd_->erase(*curVar);
        continue;
      }

      // 2ème cas spécial : la racine du diagramme est associée à la variable
      // projetée
      if (_rd_->node(_rd_->root())->nodeVar() == curVar) {
        const InternalNode* curVarNode = _rd_->node(_rd_->root());
        GUM_SCALAR          newVal     = _neutral_;
        for (Idx curVarModality = 0; curVarModality < curVar->domainSize(); ++curVarModality)
          newVal = _function_(newVal, _rd_->nodeValue(curVarNode->son(curVarModality)));

        NodeId newSonId = _rd_->manager()->addTerminalNode(newVal);

        _rd_->manager()->eraseNode(_rd_->root(), newSonId, false);

        if (_rd_->variablesSequence().exists(curVar)) _rd_->erase(*curVar);
        continue;
      }

      // Cas général
      HashTable< NodeId, NodeId > visitedNode(2 * _rd_->realSize(), true, false);
      std::vector< NodeId >       filo;
      filo.push_back(_rd_->root());

      while (!filo.empty()) {
        NodeId curNodeId = filo.back();
        filo.pop_back();

        const InternalNode* curNode = _rd_->node(curNodeId);

        for (Idx modality = 0; modality < curNode->nodeVar()->domainSize(); ++modality) {
          NodeId oldSonId = curNode->son(modality);

          if (!visitedNode.exists(oldSonId)) {
            NodeId newSonId = oldSonId;

            if (!_rd_->isTerminalNode(oldSonId)) {
              if (_rd_->node(oldSonId)->nodeVar() != curVar) {
                filo.push_back(oldSonId);
              } else {
                const InternalNode* curVarNode = _rd_->node(oldSonId);
                GUM_SCALAR          newVal     = _neutral_;
                for (Idx curVarModality = 0; curVarModality < curVar->domainSize();
                     ++curVarModality)
                  newVal = _function_(newVal, _rd_->nodeValue(curVarNode->son(curVarModality)));

                newSonId = _rd_->manager()->addTerminalNode(newVal);

                _rd_->manager()->eraseNode(oldSonId, newSonId, false);
                _rd_->manager()->setSon(curNodeId, modality, newSonId);
              }

            } else {
              GUM_SCALAR newVal = _neutral_, oldVal = _rd_->nodeValue(oldSonId);
              for (Idx curVarModality = 0; curVarModality < curVar->domainSize(); ++curVarModality)
                newVal = _function_(newVal, oldVal);

              newSonId = _rd_->manager()->addTerminalNode(newVal);
              _rd_->manager()->setSon(curNodeId, modality, newSonId);
            }

            visitedNode.insert(oldSonId, newSonId);

          } else {
            if (_rd_->node(curNodeId)->son(modality) != visitedNode[oldSonId])
              _rd_->manager()->setSon(curNodeId, modality, visitedNode[oldSonId]);
          }
        }
      }

      if (_rd_->variablesSequence().exists(curVar)) _rd_->erase(*curVar);
    }

    return _rd_;
  }

}   // namespace gum
