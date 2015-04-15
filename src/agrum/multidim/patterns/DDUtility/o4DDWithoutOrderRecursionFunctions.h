/*********************************************************************************
 *    Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES        *
 *   {prenom.nom}_at_lip6.fr                                                     *
 *                                                                               *
 *   This program is free software; you can redistribute it and/or modify        *
 *   it under the terms of the GNU General Public License as published by        *
 *   the Free Software Foundation; either version 2 of the License, or           *
 *   (at your option) any later version.                                         *
 *                                                                               *
 *   This program is distributed in the hope that it will be useful,             *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
 *   GNU General Public License for more details.                                *
 *                                                                               *
 *   You should have received a copy of the GNU General Public License           *
 *   along with this program; if not, write to the                               *
 *   Free Software Foundation, Inc.,                                             *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.                   *
 *********************************************************************************/
/**
* @file
* @brief Files containing recursions functions for operators data.
*
* @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
*/

#ifdef GUM_DECISION_DIAGRAM_OPERATOR_FUNCTION_DEFINITION

#include <agrum/variables/discreteVariable.h>
#include <agrum/multidim/multiDimDecisionDiagramBase.h>
#include <agrum/multidim/patterns/DDUtility/o4DDMiscellaneous.h>

namespace gum {

  /// Main recursion function, called every time we move on a node to determine what
  /// we have to do

  // A key is used for prunning uneccesary operations since once a node has been
  // visited in a given context, there's no use to revisit him,
  // the result will be the same node, so we just have to do an association context -
  // node.
  // The context consists in :
  //              _ Leader node we are visiting.
  //              _ Follower node we are visiting.
  //              _ For all retrograde variables, if it has been instanciated before,
  //              current modality instanciated, meaning :
  //                      _ 0 means the variable hasn't be instanciated yet,
  //                      _ From 1 to domainSize + 1 means that current modality
  //                      index of variable is value - 1,
  //                      _ domainSize + 2 means variable is on default mode.
  // A key - node association is made each time we create a node in resulting
  // diagram.
  // Since GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION is a corner step in
  // algorithm ( meaning each time we explore a node we go trought
  // this function ), check only have to be at the beginning of that function.
  template <typename T>
  NodeId GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION(
      NonOrderedOperatorData<T> &opData, const DiscreteVariable *leaderParentVar,
      std::string tabu) {

    if (opData.explorationTable->exists(opData.conti.contextKey()))
      return (*(opData.explorationTable))[opData.conti.contextKey()];

    NodeId newNode = 0;

    // ******************************************************************************************************
    // If both current nodes are terminal,
    // we only have to compute the resulting value
    if (opData.DD1->isTerminalNode(opData.conti.getDD1Node()) &&
        opData.DD2->isTerminalNode(opData.conti.getDD2Node())) {
      // We have to compute new valueand we insert a new node in diagram with this
      // value, ...
      newNode = opData.factory->addTerminalNode(GUM_MULTI_DIM_OPERATOR(
          opData.DD1->unsafeNodeValue(opData.conti.getDD1Node()),
          opData.DD2->unsafeNodeValue(opData.conti.getDD2Node())));

      // And ensure that if we get back to those node we won't explore them again
      opData.explorationTable->insert(opData.conti.contextKey(), newNode);
    }

    // *******************************************************************************************************

    // ******************************************************************************************************
    // If not, first we ensure that all retrograde variable have been considered
    if (!opData.DD2->isTerminalNode(opData.conti.getDD2Node()))
      if (opData.retrogradeVarTable->exists(opData.conti.getDD2Node())) {

        Idx indexFin = opData.factory->variablesSequence().size();

        if (!opData.DD1->isTerminalNode(opData.conti.getDD1Node()))
          indexFin = opData.factory->variablesSequence().pos(
              opData.DD1->unsafeNodeVariable(opData.conti.getDD1Node()));

        Idx indexDebut = 0;

        if (leaderParentVar != nullptr)
          indexDebut = opData.factory->variablesSequence().pos(leaderParentVar) + 1;

        if (indexFin != indexDebut)
          for (Idx i = indexDebut; i < indexFin; ++i) {
            const DiscreteVariable *preneededVar =
                opData.factory->variablesSequence().atPos(i);

            if ((*opData.retrogradeVarTable)[opData.conti.getDD2Node()]->exists(
                    preneededVar) &&
                (opData.conti.variableModality(preneededVar) == 0)) {

              const std::vector<Idx> *usedModalities =
                  opData.DD2->variableUsedModalities(preneededVar);

              if (usedModalities == nullptr)
                continue;

              std::vector<NodeId> sonsIds(preneededVar->domainSize(), 0);

              Idx nbUnexploredModalities = preneededVar->domainSize();
              HashTable<NodeId, Idx> nodeCount(sonsIds.size(), false, false);

              for (Idx modality = 0; modality < preneededVar->domainSize();
                   modality++) {
                if ((*usedModalities)[modality]) {
                  opData.conti.chgVarModality(preneededVar, modality + 1);
                  sonsIds[modality] = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION(
                      opData, preneededVar, tabu);

                  --nbUnexploredModalities;

                  if (!nodeCount.exists(sonsIds[modality]))
                    nodeCount.insert(sonsIds[modality], 1);
                  else
                    nodeCount[sonsIds[modality]]++;
                }
              }

              NodeId defaultSon = 0;

              if (nbUnexploredModalities != 0) {
                opData.conti.chgVarModality(preneededVar,
                                            preneededVar->domainSize() + 1);
                defaultSon = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION(
                    opData, preneededVar, tabu);

                if (!nodeCount.exists(defaultSon))
                  nodeCount.insert(defaultSon, nbUnexploredModalities);
                else
                  nodeCount[defaultSon] += nbUnexploredModalities;
              }

              newNode = insertNonTerminalNode(opData, preneededVar, sonsIds,
                                              defaultSon, nodeCount);

              opData.conti.chgVarModality(preneededVar, 0);
              opData.explorationTable->insert(opData.conti.contextKey(), newNode);

              return newNode;
            }
          }
      }

    // *******************************************************************************************************

    // *******************************************************************************************************
    // If only one of the current node is terminal,
    // we have to pursue deeper on the other diagram
    if (opData.DD1->isTerminalNode(opData.conti.getDD1Node()) &&
        !opData.DD2->isTerminalNode(opData.conti.getDD2Node()))
      newNode = GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_FOLLOWER_FUNCTION(
          opData, leaderParentVar, tabu);

    if (!opData.DD1->isTerminalNode(opData.conti.getDD1Node()) &&
        opData.DD2->isTerminalNode(opData.conti.getDD2Node()))
      newNode = GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_LEADER_FUNCTION(
          opData, leaderParentVar, tabu);

    // *******************************************************************************************************

    // *******************************************************************************************************
    // Remains the case where both nodes aren't terminal
    if (!opData.DD1->isTerminalNode(opData.conti.getDD1Node()) &&
        !opData.DD2->isTerminalNode(opData.conti.getDD2Node())) {

      //==========================================================
      // First we have to determine the position of current node
      // associated variable in the resulting diagram
      int indexLeader = -1;
      int indexFollower = -1;

      if (opData.factory->variablesSequence().exists(
              opData.DD1->unsafeNodeVariable(opData.conti.getDD1Node())))
        indexLeader = opData.factory->variablesSequence().pos(
            opData.DD1->unsafeNodeVariable(opData.conti.getDD1Node()));

      if (opData.factory->variablesSequence().exists(
              opData.DD2->unsafeNodeVariable(opData.conti.getDD2Node())))
        indexFollower = opData.factory->variablesSequence().pos(
            opData.DD2->unsafeNodeVariable(opData.conti.getDD2Node()));

      if (indexLeader == -1 || indexFollower == -1)
        GUM_ERROR(FatalError, "Unable to compute MultiDimDecisionDiagramBase");

      //==========================================================

      //==========================================================
      // If one variable preceeds the other on final diagram, we
      // have to go down on this one
      if (indexLeader > indexFollower)
        newNode = GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_FOLLOWER_FUNCTION(
            opData, leaderParentVar, tabu);

      if (indexLeader < indexFollower)
        newNode = GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_LEADER_FUNCTION(
            opData, leaderParentVar, tabu);

      //==========================================================

      //==========================================================
      // If not - meaning it's the same variable - we have to go
      // down on both
      if (indexLeader == indexFollower) {

        const std::vector<NodeId> *leaderSonsMap =
            opData.DD1->unsafeNodeSons(opData.conti.getDD1Node());
        const std::vector<NodeId> *followerSonsMap =
            opData.DD2->unsafeNodeSons(opData.conti.getDD2Node());

        NodeId leaderCurrentNode = opData.conti.getDD1Node();
        NodeId followerCurrentNode = opData.conti.getDD2Node();
        std::vector<NodeId> sonsIds(leaderSonsMap->size(), 0);

        HashTable<NodeId, Idx> nodeCount(sonsIds.size(), false, false);
        Idx nbUnexploredModalities =
            opData.DD1->unsafeNodeVariable(leaderCurrentNode)->domainSize();

        for (Idx modality = 0;
             modality <
                 opData.DD1->unsafeNodeVariable(leaderCurrentNode)->domainSize();
             modality++) {

          if ((*leaderSonsMap)[modality] == 0 && (*followerSonsMap)[modality] == 0)
            continue;

          if ((*leaderSonsMap)[modality] == 0)
            opData.conti.setDD1Node(
                opData.DD1->unsafeNodeDefaultSon(leaderCurrentNode));
          else
            opData.conti.setDD1Node((*leaderSonsMap)[modality]);

          if ((*followerSonsMap)[modality] == 0)
            opData.conti.setDD2Node(
                opData.DD2->unsafeNodeDefaultSon(followerCurrentNode));
          else
            opData.conti.setDD2Node((*followerSonsMap)[modality]);

          sonsIds[modality] = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION(
              opData, opData.DD1->unsafeNodeVariable(leaderCurrentNode), tabu);

          --nbUnexploredModalities;

          if (!nodeCount.exists(sonsIds[modality]))
            nodeCount.insert(sonsIds[modality], 1);
          else
            nodeCount[sonsIds[modality]]++;
        }

        NodeId defaultSon = 0;

        if (opData.DD1->unsafeHasNodeDefaultSon(leaderCurrentNode) &&
            opData.DD2->unsafeHasNodeDefaultSon(followerCurrentNode)) {
          opData.conti.setDD1Node(
              opData.DD1->unsafeNodeDefaultSon(leaderCurrentNode));
          opData.conti.setDD2Node(
              opData.DD2->unsafeNodeDefaultSon(followerCurrentNode));
          defaultSon = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION(
              opData, opData.DD1->unsafeNodeVariable(leaderCurrentNode), tabu);

          if (!nodeCount.exists(defaultSon))
            nodeCount.insert(defaultSon, nbUnexploredModalities);
          else
            nodeCount[defaultSon] += nbUnexploredModalities;
        }

        newNode = insertNonTerminalNode(
            opData, opData.DD1->unsafeNodeVariable(leaderCurrentNode), sonsIds,
            defaultSon, nodeCount);

        opData.conti.setDD1Node(leaderCurrentNode);
        opData.conti.setDD2Node(followerCurrentNode);
        opData.explorationTable->insert(opData.conti.contextKey(), newNode);
      }
    }

    // *******************************************************************************************************

    return newNode;
  }

  /// Called when we investigate a node of the first diagram only

  template <typename T>
  NodeId GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_LEADER_FUNCTION(
      NonOrderedOperatorData<T> &opData, const DiscreteVariable *leaderParentVar,
      std::string tabu) {

    NodeId newNode = 0;

    NodeId leaderCurrentNode = opData.conti.getDD1Node();
    const DiscreteVariable *leaderCurrentVar =
        opData.DD1->unsafeNodeVariable(leaderCurrentNode);
    bool isIndeedRetorgrade = opData.conti.isRetrogradeVar(leaderCurrentVar);
    const std::vector<NodeId> *sonsMap =
        opData.DD1->unsafeNodeSons(leaderCurrentNode);

    const std::vector<Idx> *usedModalities =
        opData.DD2->variableUsedModalities(leaderCurrentVar);
    std::vector<NodeId> sonsIds(sonsMap->size(), 0);

    HashTable<NodeId, Idx> nodeCount(sonsIds.size(), false, false);
    Idx nbUnexploredModalities = leaderCurrentVar->domainSize();

    // ********************************************************************************************************
    // For each value the current var take on this node, we have to do our
    // computation
    for (Idx modality = 0; modality < leaderCurrentVar->domainSize(); modality++) {
      NodeId sonId = (*sonsMap)[modality];

      if (sonId != 0 || (usedModalities != nullptr && (*usedModalities)[modality])) {

        // But we have to indicates to possible node on follower diagram, the current
        // value of the var
        if (isIndeedRetorgrade)
          opData.conti.chgVarModality(leaderCurrentVar, modality + 1);

        if (sonId != 0)
          opData.conti.setDD1Node(sonId);
        else
          opData.conti.setDD1Node(
              opData.DD1->unsafeNodeDefaultSon(leaderCurrentNode));

        sonsIds[modality] = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION(
            opData, leaderCurrentVar, tabu);

        --nbUnexploredModalities;

        if (!nodeCount.exists(sonsIds[modality]))
          nodeCount.insert(sonsIds[modality], 1);
        else
          nodeCount[sonsIds[modality]]++;
      }
    }

    //*********************************************************************************************************

    // ********************************************************************************************************
    // Then, if not all possible value of that node have been investigate (meaning we
    // have a default arc)
    // we have to look on second diagram every value that can still take this var
    // (meaning value not taken
    // on this leader node and the defautl one )
    NodeId defaultSon = 0;

    if (nbUnexploredModalities != 0) {
      opData.conti.setDD1Node(opData.DD1->unsafeNodeDefaultSon(leaderCurrentNode));

      if (isIndeedRetorgrade)
        opData.conti.chgVarModality(leaderCurrentVar,
                                    leaderCurrentVar->domainSize() + 1);

      defaultSon = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION(
          opData, leaderCurrentVar, tabu);

      if (!nodeCount.exists(defaultSon))
        nodeCount.insert(defaultSon, nbUnexploredModalities);
      else
        nodeCount[defaultSon] += nbUnexploredModalities;
    }

    //*********************************************************************************************************

    // ********************************************************************************************************
    // And we finally add this node to our resulting graph
    newNode = insertNonTerminalNode(
        opData, opData.DD1->unsafeNodeVariable(leaderCurrentNode), sonsIds,
        defaultSon, nodeCount);

    opData.conti.setDD1Node(leaderCurrentNode);

    if (isIndeedRetorgrade)
      opData.conti.chgVarModality(leaderCurrentVar, 0);

    opData.explorationTable->insert(opData.conti.contextKey(), newNode);

    return newNode;
  }

  /// Called when we investigate a node of the second diagram only

  template <typename T>
  NodeId GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_FOLLOWER_FUNCTION(
      NonOrderedOperatorData<T> &opData, const DiscreteVariable *leaderParentVar,
      std::string tabu) {

    NodeId newNode = 0;

    NodeId followerCurrentNode = opData.conti.getDD2Node();
    const DiscreteVariable *followerCurrentVar =
        opData.DD2->unsafeNodeVariable(followerCurrentNode);
    bool isIndeedRetorgrade = opData.conti.isRetrogradeVar(followerCurrentVar);
    const std::vector<NodeId> *sonsMap =
        opData.DD2->unsafeNodeSons(followerCurrentNode);

    if (isIndeedRetorgrade &&
        opData.conti.variableModality(followerCurrentVar) != 0) {
      // ***************************************************************************************************************
      // If var exists in leader diagram and has already been instantiate to its
      // default value,
      // we have to do so in second diagram too
      Idx varModality = opData.conti.variableModality(followerCurrentVar) - 1;

      if (varModality == followerCurrentVar->domainSize()) {
        opData.conti.setDD2Node(
            opData.DD2->unsafeNodeDefaultSon(followerCurrentNode));
        newNode = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION(
            opData, leaderParentVar, tabu);

        // ***************************************************************************************************************
      } else {
        // ***************************************************************************************************************
        // If var exists in leader diagram and has already been instantiate to a
        // value,
        // we have to go down on this value

        // But we have to check if value has its arc for this node
        if ((*sonsMap)[varModality] != 0)
          opData.conti.setDD2Node((*sonsMap)[varModality]);
        else
          opData.conti.setDD2Node(
              opData.DD2->unsafeNodeDefaultSon(followerCurrentNode));

        newNode = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION(
            opData, leaderParentVar, tabu);
      }

      opData.conti.setDD2Node(followerCurrentNode);
      // ****************************************************************************************************************
    } else {
      // ***************************************************************************************************************
      // If we aren't in one of the above cases, it means that leader diagram hasn't
      // the variable pointed by follower current node.
      std::vector<NodeId> sonsIds(sonsMap->size(), 0);

      HashTable<NodeId, Idx> nodeCount(sonsIds.size(), false, false);
      Idx nbUnexploredModalities = followerCurrentVar->domainSize();

      // In that case we do our computation for all sons of that var
      for (Idx modality = 0; modality < followerCurrentVar->domainSize();
           modality++) {
        NodeId sonId = (*sonsMap)[modality];

        if (sonId != 0) {
          opData.conti.setDD2Node(sonId);
          sonsIds[modality] = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION(
              opData, leaderParentVar, tabu);

          --nbUnexploredModalities;

          if (!nodeCount.exists(sonsIds[modality]))
            nodeCount.insert(sonsIds[modality], 1);
          else
            nodeCount[sonsIds[modality]]++;
        }
      }

      // and for its default son if it has one
      NodeId defaultSon = 0;

      if (opData.DD2->unsafeHasNodeDefaultSon(followerCurrentNode)) {
        opData.conti.setDD2Node(
            opData.DD2->unsafeNodeDefaultSon(followerCurrentNode));
        defaultSon = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION(
            opData, leaderParentVar, tabu);

        if (!nodeCount.exists(defaultSon))
          nodeCount.insert(defaultSon, nbUnexploredModalities);
        else
          nodeCount[defaultSon] += nbUnexploredModalities;
      }

      // And we had this node to our graph
      newNode = insertNonTerminalNode(opData, followerCurrentVar, sonsIds,
                                      defaultSon, nodeCount);

      opData.conti.setDD2Node(followerCurrentNode);
      opData.explorationTable->insert(opData.conti.contextKey(), newNode);
    }

    // ****************************************************************************************************************

    return newNode;
  }

} /* end of namespace GUM */

#endif /* GUM_DECISION_DIAGRAM_OPERATOR_FUNCTION_DEFINITION */
