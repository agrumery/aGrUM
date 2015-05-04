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
      const Set<const DiscreteVariable *> &delVars, Idx nbOperation,
      std::string tabu) {
    tabu += "\t";

    std::cout << tabu << "Status - curent Node : " << currentNode
              << " - delVarAscendant : " << delVarAscendant
              << " - nbOperation : " << nbOperation << std::endl;

    if (oldDiagram->isTerminalNode(currentNode)) {
      std::cout << tabu << "TERMINAL NODE." << std::endl;
      GUM_SCALAR resValue = oldDiagram->nodeValue(currentNode);

      for (Idx i = 1; i < nbOperation; i++)
        resValue = GUM_DECISION_DIAGRAM_PROJECTION_OPERATOR(
            resValue, oldDiagram->nodeValue(currentNode));

      NodeId resNode = factory->addTerminalNode(resValue);

      std::cout << tabu << "Creation Noeud Terminal => Valeur : " << resValue
                << " | Noeud : " << resNode << std::endl;

      std::cout << tabu << "END TERMINAL NODE." << std::endl;
      return resNode;
    }

    if (delVarAscendant || delVars.exists(oldDiagram->nodeVariable(currentNode))) {
      std::cout << tabu << "ELIMINATED VAR NODE." << std::endl;

      if (!delVarAscendant) {
        if (explorationTable.exists(currentNode)) {
          NodeId resNode = explorationTable[currentNode];
          std::cout << tabu << "Noeud déjà visité : " << currentNode
                    << " | Noeud résultant : " << resNode << std::endl;
          return resNode;
        }
      }

      nbOperation /= oldDiagram->nodeVariable(currentNode)->domainSize();
      Idx nbExploredModalities = 0;
      std::vector<NodeId>::const_iterator sonIter =
          oldDiagram->nodeSons(currentNode)->begin();
      GUM_SCALAR resValue = GUM_MULTI_DIM_PROJECTION_NEUTRAL;

      while (sonIter != oldDiagram->nodeSons(currentNode)->end()) {
        if (*sonIter != 0) {
          std::cout << tabu << "Descente sur fils : " << *sonIter << std::endl;
          NodeId sonValueNode = GUM_DECISION_DIAGRAM_PROJECTION_EXPLORATION_FUNCTION(
              oldDiagram, factory, *sonIter, true, explorationTable, delVars,
              nbOperation, tabu);
          resValue = GUM_DECISION_DIAGRAM_PROJECTION_OPERATOR(
              resValue, factory->nodeValue(sonValueNode));
          nbExploredModalities++;
          std::cout << tabu << "Fin descente sur fils : " << *sonIter << std::endl;
        }

        ++sonIter;
      }

      if (oldDiagram->hasNodeDefaultSon(currentNode)) {
        std::cout << tabu << "Descente sur fils par défaut" << std::endl;
        NodeId defaultSonValueNode =
            GUM_DECISION_DIAGRAM_PROJECTION_EXPLORATION_FUNCTION(
                oldDiagram, factory, oldDiagram->nodeDefaultSon(currentNode), true,
                explorationTable, delVars, nbOperation, tabu);

        for (Idx i = 0; i < oldDiagram->nodeVariable(currentNode)->domainSize() -
                                nbExploredModalities;
             i++)
          resValue = GUM_DECISION_DIAGRAM_PROJECTION_OPERATOR(
              resValue, factory->nodeValue(defaultSonValueNode));

        std::cout << tabu << "Fin descente sur fils par défaut" << std::endl;
      }

      nbOperation *= oldDiagram->nodeVariable(currentNode)->domainSize();

      NodeId resNode = factory->addTerminalNode(resValue);

      if (!delVarAscendant)
        explorationTable.insert(currentNode, resNode);

      std::cout << tabu << "Creation Noeud Terminal => Valeur : " << resValue
                << " | Noeud : " << resNode << std::endl;

      std::cout << tabu << "END ELIMINATED VAR NODE." << std::endl;
      return resNode;

    } else {

      std::cout << tabu << "REMAINING NODE." << std::endl;

      if (explorationTable.exists(currentNode)) {
        NodeId ret = explorationTable[currentNode];
        std::cout << tabu << "Noeud déjà visité : " << currentNode
                  << " | Noeud résultant : " << ret << std::endl;
        return ret;
      }

      std::vector<NodeId> sonsMap(
          oldDiagram->nodeVariable(currentNode)->domainSize(), 0);

      for (std::vector<NodeId>::const_iterator sonIter =
               oldDiagram->nodeSons(currentNode)->begin();
           sonIter != oldDiagram->nodeSons(currentNode)->end(); ++sonIter) {
        if (*sonIter != 0) {
          std::cout << tabu << "Descente sur fils : " << *sonIter << std::endl;
          NodeId sonValueNode = GUM_DECISION_DIAGRAM_PROJECTION_EXPLORATION_FUNCTION(
              oldDiagram, factory, *sonIter, false, explorationTable, delVars,
              nbOperation, tabu);
          sonsMap[std::distance(oldDiagram->nodeSons(currentNode)->begin(),
                                sonIter)] = sonValueNode;
          std::cout << tabu << "Fin descente sur fils : " << *sonIter << std::endl;
        }
      }

      NodeId defaultSon = 0;

      if (oldDiagram->hasNodeDefaultSon(currentNode)) {
        std::cout << tabu << "Descente sur fils par défaut" << std::endl;
        defaultSon = GUM_DECISION_DIAGRAM_PROJECTION_EXPLORATION_FUNCTION(
            oldDiagram, factory, oldDiagram->nodeDefaultSon(currentNode), false,
            explorationTable, delVars, nbOperation, tabu);
        std::cout << tabu << "Fin descente sur fils par défaut" << std::endl;
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
          oldDiagram->nodeVariable(currentNode), sonsMap, defaultSon);
      explorationTable.insert(currentNode, resNode);

      std::cout << tabu << "Creation Noeud : " << resNode << std::endl;
      std::cout << tabu << "END REMAINING NODE." << std::endl;
      return resNode;
    }
  }
} /* end of namespace GUM*/
// #endif /*GUM_DECISION_DIAGRAM_PROJECTION_EXPLORATION_FUNCTION_DEFINITION*/
