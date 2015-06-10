/*********************************************************************************
 *    Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr *
 *                                                                                                                      *
 *   This program is free software; you can redistribute it and/or modify           *
 *   it under the terms of the GNU General Public License as published by       *
 *   the Free Software Foundation; either version 2 of the License, or              *
 *   (at your option) any later version. *
 *                                                                                                                      *
 *   This program is distributed in the hope that it will be useful, *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU General Public License for more details. *
 *                                                                                                                      *
 *   You should have received a copy of the GNU General Public License          *
 *   along with this program; if not, write to the *
 *   Free Software Foundation, Inc., *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.                      *
 ********************************************************************************/
/**
* @file
* @brief Files containing recursions functions for operators data.
*
* @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
*/

// #ifdef GUM_DECISION_DIAGRAM_PROJECTION_EXPLORATION_FUNCTION_DEFINITION

#include <agrum/core/hashTable.h>
#include <agrum/core/set.h>

#include <agrum/variables/discreteVariable.h>
#include <agrum/multidim/multiDimDecisionDiagramBase.h>
#include <agrum/multidim/multiDimDecisionDiagramFactoryBase.h>
#include <agrum/multidim/patterns/DDUtility/o4DDMiscellaneous.h>

namespace gum {

  template <typename GUM_SCALAR>
  NodeId GUM_DECISION_DIAGRAM_PROJECTION_EXPLORATION_FUNCTION(
      const MultiDimDecisionDiagramBase<GUM_SCALAR> *oldDiagram,
      MultiDimDecisionDiagramFactoryBase<GUM_SCALAR> *factory, NodeId currentNode,
      bool delVarAscendant, HashTable<NodeId, NodeId> &explorationTable,
      const Set<const DiscreteVariable *> &delVars, Idx nbOperation) {

    if (oldDiagram->isTerminalNode(currentNode)) {
      GUM_SCALAR resValue = oldDiagram->unsafeNodeValue(currentNode);

      for (Idx i = 1; i < nbOperation; i++)
        resValue = GUM_DECISION_DIAGRAM_PROJECTION_OPERATOR(
            resValue, oldDiagram->unsafeNodeValue(currentNode));

      return factory->addTerminalNode(resValue);
    }

    if (delVarAscendant ||
        delVars.exists(oldDiagram->unsafeNodeVariable(currentNode))) {

      if (!delVarAscendant) {
        if (explorationTable.exists(currentNode))
          return explorationTable[currentNode];
      }

      nbOperation /= oldDiagram->unsafeNodeVariable(currentNode)->domainSize();
      Idx nbExploredModalities = 0;
      std::vector<NodeId>::const_iterator sonIter =
          oldDiagram->unsafeNodeSons(currentNode)->begin();
      GUM_SCALAR resValue = GUM_MULTI_DIM_PROJECTION_NEUTRAL;

      while (sonIter != oldDiagram->unsafeNodeSons(currentNode)->end()) {
        if (*sonIter != 0) {
          NodeId sonValueNode = GUM_DECISION_DIAGRAM_PROJECTION_EXPLORATION_FUNCTION(
              oldDiagram, factory, *sonIter, true, explorationTable, delVars,
              nbOperation);
          resValue = GUM_DECISION_DIAGRAM_PROJECTION_OPERATOR(
              resValue, factory->nodeValue(sonValueNode));
          nbExploredModalities++;
        }

        ++sonIter;
      }

      if (oldDiagram->unsafeHasNodeDefaultSon(currentNode)) {
        NodeId defaultSonValueNode =
            GUM_DECISION_DIAGRAM_PROJECTION_EXPLORATION_FUNCTION(
                oldDiagram, factory, oldDiagram->unsafeNodeDefaultSon(currentNode),
                true, explorationTable, delVars, nbOperation);

        for (Idx i = 0;
             i < oldDiagram->unsafeNodeVariable(currentNode)->domainSize() -
                     nbExploredModalities;
             i++)
          resValue = GUM_DECISION_DIAGRAM_PROJECTION_OPERATOR(
              resValue, factory->nodeValue(defaultSonValueNode));
      }

      nbOperation *= oldDiagram->unsafeNodeVariable(currentNode)->domainSize();

      NodeId resNode = factory->addTerminalNode(resValue);

      if (!delVarAscendant)
        explorationTable.insert(currentNode, resNode);

      return resNode;

    } else {

      if (explorationTable.exists(currentNode))
        return explorationTable[currentNode];

      std::vector<NodeId> sonsMap(
          oldDiagram->unsafeNodeVariable(currentNode)->domainSize(), 0);

      for (std::vector<NodeId>::const_iterator sonIter =
               oldDiagram->unsafeNodeSons(currentNode)->begin();
           sonIter != oldDiagram->unsafeNodeSons(currentNode)->end(); ++sonIter) {
        if (*sonIter != 0) {
          NodeId sonValueNode = GUM_DECISION_DIAGRAM_PROJECTION_EXPLORATION_FUNCTION(
              oldDiagram, factory, *sonIter, false, explorationTable, delVars,
              nbOperation);
          sonsMap[std::distance(oldDiagram->unsafeNodeSons(currentNode)->begin(),
                                sonIter)] = sonValueNode;
        }
      }

      NodeId defaultSon = 0;

      if (oldDiagram->unsafeHasNodeDefaultSon(currentNode)) {
        defaultSon = GUM_DECISION_DIAGRAM_PROJECTION_EXPLORATION_FUNCTION(
            oldDiagram, factory, oldDiagram->unsafeNodeDefaultSon(currentNode),
            false, explorationTable, delVars, nbOperation);
      }

      if (defaultSon != 0) {
        Idx nbDefault = 0;

        for (std::vector<NodeId>::iterator iterArcMap = sonsMap.begin();
             iterArcMap != sonsMap.end(); ++iterArcMap) {
          if (*iterArcMap == 0)
            ++nbDefault;

          if (*iterArcMap == defaultSon) {
            ++nbDefault;
            sonsMap[std::distance(sonsMap.begin(), iterArcMap)] = 0;
          }
        }

        if (nbDefault == 1)
          for (std::vector<NodeId>::iterator iterArcMap = sonsMap.begin();
               iterArcMap != sonsMap.end(); ++iterArcMap)
            if (*iterArcMap == 0) {
              sonsMap[std::distance(sonsMap.begin(), iterArcMap)] = defaultSon;
              defaultSon = 0;
              break;
            }
      }

      NodeId resNode = factory->unsafeAddNonTerminalNodeWithArcs(
          oldDiagram->unsafeNodeVariable(currentNode), sonsMap, defaultSon);
      explorationTable.insert(currentNode, resNode);
      return resNode;
    }
  }
} /* end of namespace GUM*/
// #endif /*GUM_DECISION_DIAGRAM_PROJECTION_EXPLORATION_FUNCTION_DEFINITION*/
