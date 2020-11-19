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


/**
 * @file
 * @brief Class used to compute the projection of a function graph
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6) and Jean-Christophe MAGNAN and Christophe GONZALES(@AMU)
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
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
      src__(src),
      delVars__(delVars), function__(), neutral__(neutral) {
    GUM_CONSTRUCTOR(MultiDimFunctionGraphProjector);
    rd__ = MultiDimFunctionGraph< GUM_SCALAR >::getReducedAndOrderedInstance();
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
     MultiDimFunctionGraphProjector< GUM_SCALAR, FUNCTOR, TerminalNodePolicy >::
        project() {
    rd__->copy(*src__);

    for (SetIteratorSafe< const DiscreteVariable* > varIter =
            delVars__.beginSafe();
         varIter != delVars__.endSafe();
         ++varIter) {
      const DiscreteVariable* curVar = *varIter;

      // Tout d'abord, on déplace la variable à projeter en fin de séquence afin
      // de simplifier la projection
      if (rd__->variablesSequence().exists(curVar))
        rd__->manager()->moveTo(curVar, rd__->variablesSequence().size() - 1);

      // 1er cas spécial : le diagramme est un un simple noeud terminal
      if (rd__->isTerminalNode(rd__->root())) {
        GUM_SCALAR newVal = neutral__, oldVal = rd__->nodeValue(rd__->root());
        for (Idx curVarModality = 0; curVarModality < curVar->domainSize();
             ++curVarModality)
          newVal = function__(newVal, oldVal);

        NodeId newSonId = rd__->manager()->addTerminalNode(newVal);
        rd__->manager()->setRootNode(newSonId);

        if (rd__->variablesSequence().exists(curVar)) rd__->erase(*curVar);
        continue;
      }

      // 2ème cas spécial : la racine du diagramme est associée à la variable
      // projetée
      if (rd__->node(rd__->root())->nodeVar() == curVar) {
        const InternalNode* curVarNode = rd__->node(rd__->root());
        GUM_SCALAR          newVal = neutral__;
        for (Idx curVarModality = 0; curVarModality < curVar->domainSize();
             ++curVarModality)
          newVal =
             function__(newVal, rd__->nodeValue(curVarNode->son(curVarModality)));

        NodeId newSonId = rd__->manager()->addTerminalNode(newVal);

        rd__->manager()->eraseNode(rd__->root(), newSonId, false);

        if (rd__->variablesSequence().exists(curVar)) rd__->erase(*curVar);
        continue;
      }

      // Cas général
      HashTable< NodeId, NodeId > visitedNode(2 * rd__->realSize(), true, false);
      std::vector< NodeId >       filo;
      filo.push_back(rd__->root());

      while (!filo.empty()) {
        NodeId curNodeId = filo.back();
        filo.pop_back();

        const InternalNode* curNode = rd__->node(curNodeId);

        for (Idx modality = 0; modality < curNode->nodeVar()->domainSize();
             ++modality) {
          NodeId oldSonId = curNode->son(modality);

          if (!visitedNode.exists(oldSonId)) {
            NodeId newSonId = oldSonId;

            if (!rd__->isTerminalNode(oldSonId)) {
              if (rd__->node(oldSonId)->nodeVar() != curVar) {
                filo.push_back(oldSonId);
              } else {
                const InternalNode* curVarNode = rd__->node(oldSonId);
                GUM_SCALAR          newVal = neutral__;
                for (Idx curVarModality = 0; curVarModality < curVar->domainSize();
                     ++curVarModality)
                  newVal = function__(
                     newVal, rd__->nodeValue(curVarNode->son(curVarModality)));

                newSonId = rd__->manager()->addTerminalNode(newVal);

                rd__->manager()->eraseNode(oldSonId, newSonId, false);
                rd__->manager()->setSon(curNodeId, modality, newSonId);
              }

            } else {
              GUM_SCALAR newVal = neutral__, oldVal = rd__->nodeValue(oldSonId);
              for (Idx curVarModality = 0; curVarModality < curVar->domainSize();
                   ++curVarModality)
                newVal = function__(newVal, oldVal);

              newSonId = rd__->manager()->addTerminalNode(newVal);
              rd__->manager()->setSon(curNodeId, modality, newSonId);
            }

            visitedNode.insert(oldSonId, newSonId);

          } else {
            if (rd__->node(curNodeId)->son(modality) != visitedNode[oldSonId])
              rd__->manager()->setSon(curNodeId, modality, visitedNode[oldSonId]);
          }
        }
      }

      if (rd__->variablesSequence().exists(curVar)) rd__->erase(*curVar);
    }

    return rd__;
  }

}   // namespace gum
