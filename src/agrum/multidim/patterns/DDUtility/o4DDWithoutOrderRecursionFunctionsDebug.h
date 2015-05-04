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
* @brief Files containing recursions functions for operators data. This ones are for
*debug on improvement only, see its normal one for common use
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

    GUM_TRACE(tabu << std::setprecision(20) << "PRUNNING EVALUATION : ");
    GUM_TRACE(tabu << std::setprecision(20)
                   << "KEY : " << opData.conti.contextKey());
    // GUM_TRACE( tabu << std::setprecision( 20 )  << "RECUR EXPLORATION STATUS : "
    // << *(opData.explorationTable) );

    if (opData.explorationTable->exists(opData.conti.contextKey())) {
      GUM_TRACE(tabu << std::setprecision(20) << "PRUNING!");
      return (*(opData.explorationTable))[opData.conti.contextKey()];
    }

    tabu += "\t";
    NodeId newNode = 0;

    // ******************************************************************************************************
    // If both current nodes are terminal,
    // we only have to compute the resulting value
    if (opData.DD1->isTerminalNode(opData.conti.getDD1Node()) &&
        opData.DD2->isTerminalNode(opData.conti.getDD2Node())) {
      GUM_TRACE(tabu << std::setprecision(20) << "Ajout noeud terminal "
                     << " | Noeud leader : " << opData.conti.getDD1Node()
                     << " | Noeud follower : " << opData.conti.getDD2Node());

      // We have to compute new valueand we insert a new node in diagram with this
      // value, ...
      T leaderValue = opData.DD1->nodeValue(opData.conti.getDD1Node());
      T followerValue = opData.DD2->nodeValue(opData.conti.getDD2Node());
      T newVal = GUM_MULTI_DIM_OPERATOR(leaderValue, followerValue);
      newNode = opData.factory->addTerminalNode(newVal);
      GUM_TRACE(tabu << std::setprecision(20) << "Leader value : " << leaderValue
                     << " - Follower Value : " << followerValue
                     << " | Resulting value : " << newVal
                     << " - Resulting node : " << newNode);

      // And ensure that if we get back to those node we won't explore them again
      GUM_TRACE(tabu << std::setprecision(20) << "KEY INSERTION : ");
      GUM_TRACE(tabu << std::setprecision(20)
                     << "Context : " << opData.conti.toString() << "Ajout de : "
                     << opData.conti.contextKey() << " -> " << newNode);
      opData.explorationTable->insert(opData.conti.contextKey(), newNode);
      // GUM_TRACE( tabu << std::setprecision( 20 )  <<  "LEAF EXPLORATION STATUS : "
      // << *(opData.explorationTable) );
    }

    // *******************************************************************************************************

    // ******************************************************************************************************
    // If not, first we ensure that all retrograde variable have been considered
    if (!opData.DD2->isTerminalNode(opData.conti.getDD2Node()))
      if (opData.retrogradeVarTable->exists(opData.conti.getDD2Node())) {

        GUM_TRACE(tabu << "Preneeded Variable : ");

        for (const auto var :
             *(*opData.retrogradeVarTable)[opData.conti.getDD2Node()])
          GUM_TRACE(tabu << var->toString() << " | ");

        Idx indexFin = opData.factory->variablesSequence().size();

        if (!opData.DD1->isTerminalNode(opData.conti.getDD1Node()))
          indexFin = opData.factory->variablesSequence().pos(
              opData.DD1->nodeVariable(opData.conti.getDD1Node()));

        Idx indexDebut = 0;

        if (leaderParentVar != nullptr) {
          GUM_TRACE(tabu << "ParentVar : " << leaderParentVar->toString());
          indexDebut = opData.factory->variablesSequence().pos(leaderParentVar) + 1;
        }

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

              GUM_TRACE(
                  tabu << std::setprecision(20) << " Action Taken! - Noeud leader : "
                       << opData.conti.getDD1Node()
                       << " - Noeud follower : " << opData.conti.getDD2Node()
                       << " - Instantiated Variable : " << preneededVar->toString());

              std::vector<NodeId> sonsIds(preneededVar->domainSize(), 0);

              Idx nbExploredModalities = 0;
              HashTable<NodeId, Idx> nodeCount(sonsIds.size(), false, false);

              for (Idx modality = 0; modality < preneededVar->domainSize();
                   modality++) {
                if ((*usedModalities)[modality]) {
                  GUM_TRACE(tabu << std::setprecision(20)
                                 << " Exploration sur fils numéro : " << modality);
                  opData.conti.chgVarModality(preneededVar, modality + 1);
                  sonsIds[modality] = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION(
                      opData, preneededVar, tabu);

                  ++nbExploredModalities;

                  if (!nodeCount.exists(sonsIds[modality]))
                    nodeCount.insert(sonsIds[modality], 1);
                  else
                    nodeCount[sonsIds[modality]]++;
                }
              }

              NodeId defaultSon = 0;

              if (nbExploredModalities != preneededVar->domainSize()) {
                GUM_TRACE(tabu << std::setprecision(20)
                               << " Exploration sur fils par défaut");
                opData.conti.chgVarModality(preneededVar,
                                            preneededVar->domainSize() + 1);
                defaultSon = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION(
                    opData, preneededVar, tabu);

                Idx defaultDomainSize =
                    preneededVar->domainSize() - nbExploredModalities;

                if (!nodeCount.exists(defaultSon))
                  nodeCount.insert(defaultSon, defaultDomainSize);
                else
                  nodeCount[defaultSon] += defaultDomainSize;
              }

              newNode = insertNonTerminalNode(opData, preneededVar, sonsIds,
                                              defaultSon, nodeCount);

              GUM_TRACE(tabu << std::setprecision(20) << "KEY INSERTION : ");
              opData.conti.chgVarModality(preneededVar, 0);
              GUM_TRACE(tabu << std::setprecision(20)
                             << "Context : " << opData.conti.toString()
                             << "Ajout de : " << opData.conti.contextKey() << " -> "
                             << newNode);
              opData.explorationTable->insert(opData.conti.contextKey(), newNode);
              // GUM_TRACE( tabu << std::setprecision( 20 )  << "RETROGRADE
              // EXPLORATION STATUS : " << *(opData.explorationTable) );

              GUM_TRACE(tabu << std::setprecision(20)
                             << "Fin Action Taken! - Instantiated Variable : "
                             << preneededVar->toString()
                             << " - Resulting Node : " << newNode);

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
              opData.DD1->nodeVariable(opData.conti.getDD1Node())))
        indexLeader = opData.factory->variablesSequence().pos(
            opData.DD1->nodeVariable(opData.conti.getDD1Node()));

      if (opData.factory->variablesSequence().exists(
              opData.DD2->nodeVariable(opData.conti.getDD2Node())))
        indexFollower = opData.factory->variablesSequence().pos(
            opData.DD2->nodeVariable(opData.conti.getDD2Node()));

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
        GUM_TRACE(tabu << std::setprecision(20)
                       << "Recur - Noeud leader : " << opData.conti.getDD1Node()
                       << " - Noeud follower : " << opData.conti.getDD2Node());

        const std::vector<NodeId> *leaderSonsMap =
            opData.DD1->nodeSons(opData.conti.getDD1Node());
        const std::vector<NodeId> *followerSonsMap =
            opData.DD2->nodeSons(opData.conti.getDD2Node());

        NodeId leaderCurrentNode = opData.conti.getDD1Node();
        NodeId followerCurrentNode = opData.conti.getDD2Node();
        std::vector<NodeId> sonsIds(leaderSonsMap->size(), 0);

        HashTable<NodeId, Idx> nodeCount(sonsIds.size(), false, false);
        Idx nbExploredModalities = 0;

        for (Idx modality = 0;
             modality < opData.DD1->nodeVariable(leaderCurrentNode)->domainSize();
             modality++) {
          GUM_TRACE(tabu << std::setprecision(20)
                         << "Recur - Descente sur fils numéro : " << modality);

          if ((*leaderSonsMap)[modality] == 0 && (*followerSonsMap)[modality] == 0)
            continue;

          if ((*leaderSonsMap)[modality] == 0)
            opData.conti.setDD1Node(opData.DD1->nodeDefaultSon(leaderCurrentNode));
          else
            opData.conti.setDD1Node((*leaderSonsMap)[modality]);

          if ((*followerSonsMap)[modality] == 0)
            opData.conti.setDD2Node(opData.DD2->nodeDefaultSon(followerCurrentNode));
          else
            opData.conti.setDD2Node((*followerSonsMap)[modality]);

          sonsIds[modality] = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION(
              opData, opData.DD1->nodeVariable(leaderCurrentNode), tabu);

          ++nbExploredModalities;

          if (!nodeCount.exists(sonsIds[modality]))
            nodeCount.insert(sonsIds[modality], 1);
          else
            nodeCount[sonsIds[modality]]++;
        }

        NodeId defaultSon = 0;

        if (opData.DD1->hasNodeDefaultSon(leaderCurrentNode) &&
            opData.DD2->hasNodeDefaultSon(followerCurrentNode)) {
          GUM_TRACE(tabu << std::setprecision(20)
                         << "Recur - Descente sur fils par défaut");
          opData.conti.setDD1Node(opData.DD1->nodeDefaultSon(leaderCurrentNode));
          opData.conti.setDD2Node(opData.DD2->nodeDefaultSon(followerCurrentNode));
          defaultSon = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION(
              opData, opData.DD1->nodeVariable(leaderCurrentNode), tabu);

          Idx defaultDomainSize =
              opData.DD1->nodeVariable(leaderCurrentNode)->domainSize() -
              nbExploredModalities;

          if (!nodeCount.exists(defaultSon))
            nodeCount.insert(defaultSon, defaultDomainSize);
          else
            nodeCount[defaultSon] += defaultDomainSize;
        }

        newNode = insertNonTerminalNode(opData,
                                        opData.DD1->nodeVariable(leaderCurrentNode),
                                        sonsIds, defaultSon, nodeCount);

        GUM_TRACE(tabu << std::setprecision(20) << "KEY INSERTION : ");
        opData.conti.setDD1Node(leaderCurrentNode);
        opData.conti.setDD2Node(followerCurrentNode);
        GUM_TRACE(tabu << std::setprecision(20)
                       << "Context : " << opData.conti.toString() << "Ajout de : "
                       << opData.conti.contextKey() << " -> " << newNode);
        opData.explorationTable->insert(opData.conti.contextKey(), newNode);
        // GUM_TRACE( tabu << std::setprecision( 20 )  << "EQUALS EXPLORATION STATUS
        // : " << *(opData.explorationTable) );
        GUM_TRACE(tabu << std::setprecision(20)
                       << "Fin Recur - Noeud leader : " << opData.conti.getDD1Node()
                       << " - Noeud follower : " << opData.conti.getDD2Node()
                       << " - Resulting Node : " << newNode);
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
    GUM_TRACE(
        tabu << std::setprecision(20)
             << "GoDownLeader  - Noeud Leader : " << opData.conti.getDD1Node()
             << " - Noeud Follower : " << opData.conti.getDD2Node()
             << " - Instantiated Variable : "
             << opData.DD1->nodeVariable(opData.conti.getDD1Node())->toString());

    NodeId leaderCurrentNode = opData.conti.getDD1Node();
    const DiscreteVariable *leaderCurrentVar =
        opData.DD1->nodeVariable(leaderCurrentNode);
    bool isIndeedRetorgrade = opData.conti.isRetrogradeVar(leaderCurrentVar);
    const std::vector<NodeId> *sonsMap = opData.DD1->nodeSons(leaderCurrentNode);
    GUM_TRACE(tabu << "Sons Map : " << *sonsMap);
    const std::vector<Idx> *usedModalities =
        opData.DD2->variableUsedModalities(leaderCurrentVar);
    std::vector<NodeId> sonsIds(sonsMap->size(), 0);

    HashTable<NodeId, Idx> nodeCount(sonsIds.size(), false, false);
    Idx nbExploredModalities = 0;

    // ********************************************************************************************************
    // For each value the current var take on this node, we have to do our
    // computation
    for (Idx modality = 0; modality < leaderCurrentVar->domainSize(); modality++) {
      NodeId sonId = (*sonsMap)[modality];

      if (sonId != 0 || (usedModalities != nullptr && (*usedModalities)[modality])) {
        GUM_TRACE(tabu << std::setprecision(20)
                       << "GoDownLeader  - Descente sur fils numéro : " << modality);

        // But we have to indicates to possible node on follower diagram, the current
        // value of the var
        if (isIndeedRetorgrade)
          opData.conti.chgVarModality(leaderCurrentVar, modality + 1);

        if (sonId != 0)
          opData.conti.setDD1Node(sonId);
        else
          opData.conti.setDD1Node(opData.DD1->nodeDefaultSon(leaderCurrentNode));

        sonsIds[modality] = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION(
            opData, leaderCurrentVar, tabu);

        ++nbExploredModalities;

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

    if (nbExploredModalities != leaderCurrentVar->domainSize()) {
      GUM_TRACE(tabu << std::setprecision(20)
                     << "GoDownLeader  - Descente sur fils par défaut.");
      opData.conti.setDD1Node(opData.DD1->nodeDefaultSon(leaderCurrentNode));

      if (isIndeedRetorgrade)
        opData.conti.chgVarModality(leaderCurrentVar,
                                    leaderCurrentVar->domainSize() + 1);

      defaultSon = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION(
          opData, leaderCurrentVar, tabu);

      Idx defaultDomainSize = leaderCurrentVar->domainSize() - nbExploredModalities;

      if (!nodeCount.exists(defaultSon))
        nodeCount.insert(defaultSon, defaultDomainSize);
      else
        nodeCount[defaultSon] += defaultDomainSize;
    }

    //*********************************************************************************************************

    // ********************************************************************************************************
    // And we finally add this node to our resulting graph
    newNode =
        insertNonTerminalNode(opData, opData.DD1->nodeVariable(leaderCurrentNode),
                              sonsIds, defaultSon, nodeCount);

    GUM_TRACE(tabu << std::setprecision(20) << "KEY INSERTION : ");
    opData.conti.setDD1Node(leaderCurrentNode);

    if (isIndeedRetorgrade)
      opData.conti.chgVarModality(leaderCurrentVar, 0);

    GUM_TRACE(tabu << std::setprecision(20)
                   << "Context : " << opData.conti.toString() << "Ajout de : "
                   << opData.conti.contextKey() << " -> " << newNode);
    opData.explorationTable->insert(opData.conti.contextKey(), newNode);
    // GUM_TRACE( tabu << std::setprecision( 20 )  << "GODOWNLEADER EXPLORATION
    // STATUS : " << *(opData.explorationTable) );

    GUM_TRACE(tabu << std::setprecision(20)
                   << "GoDownLeader  - Fin GoDownLeader  - Noeud Leader : "
                   << opData.conti.getDD1Node()
                   << " - Noeud Follower : " << opData.conti.getDD2Node()
                   << " - Resulting Node : " << newNode);

    return newNode;
  }

  /// Called when we investigate a node of the second diagram only

  template <typename T>
  NodeId GUM_MULTI_DIM_DECISION_DIAGRAM_GO_DOWN_ON_FOLLOWER_FUNCTION(
      NonOrderedOperatorData<T> &opData, const DiscreteVariable *leaderParentVar,
      std::string tabu) {

    GUM_TRACE(
        tabu << std::setprecision(20)
             << "GoDownFollower - Noeud leader : " << opData.conti.getDD1Node()
             << " - Noeud follower : " << opData.conti.getDD2Node()
             << " - Instantiated Variable : "
             << opData.DD2->nodeVariable(opData.conti.getDD2Node())->toString());

    NodeId newNode = 0;

    NodeId followerCurrentNode = opData.conti.getDD2Node();
    const DiscreteVariable *followerCurrentVar =
        opData.DD2->nodeVariable(followerCurrentNode);
    bool isIndeedRetorgrade = opData.conti.isRetrogradeVar(followerCurrentVar);
    const std::vector<NodeId> *sonsMap = opData.DD2->nodeSons(followerCurrentNode);

    if (isIndeedRetorgrade &&
        opData.conti.variableModality(followerCurrentVar) != 0) {
      GUM_TRACE(tabu << std::setprecision(20)
                     << "GoDownFollower  -  Context : " << opData.conti.toString())
      // ***************************************************************************************************************
      // If var exists in leader diagram and has already been instantiate to its
      // default value,
      // we have to do so in second diagram too
      Idx varModality = opData.conti.variableModality(followerCurrentVar) - 1;

      if (varModality == followerCurrentVar->domainSize()) {
        GUM_TRACE(tabu << std::setprecision(20)
                       << "GoDownFollower  -  Saut par fils par défaut ");
        opData.conti.setDD2Node(opData.DD2->nodeDefaultSon(followerCurrentNode));
        newNode = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION(
            opData, leaderParentVar, tabu);

        // ***************************************************************************************************************
      } else {
        // ***************************************************************************************************************
        // If var exists in leader diagram and has already been instantiate to a
        // value,
        // we have to go down on this value
        GUM_TRACE(tabu << std::setprecision(20)
                       << "GoDownFollower  -  Saut par fils : "
                       << opData.conti.variableModality(followerCurrentVar) - 1);

        // But we have to check if value has its arc for this node
        if ((*sonsMap)[varModality] != 0)
          opData.conti.setDD2Node((*sonsMap)[varModality]);
        else
          opData.conti.setDD2Node(opData.DD2->nodeDefaultSon(followerCurrentNode));

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
      Idx nbExploredModalities = 0;

      // In that case we do our computation for all sons of that var
      for (Idx modality = 0; modality < followerCurrentVar->domainSize();
           modality++) {
        NodeId sonId = (*sonsMap)[modality];

        if (sonId != 0) {
          GUM_TRACE(tabu << std::setprecision(20)
                         << "GoDownFollower  -  Descente sur fils numéro : "
                         << modality);
          opData.conti.setDD2Node(sonId);
          sonsIds[modality] = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION(
              opData, leaderParentVar, tabu);

          ++nbExploredModalities;

          if (!nodeCount.exists(sonsIds[modality]))
            nodeCount.insert(sonsIds[modality], 1);
          else
            nodeCount[sonsIds[modality]]++;
        }
      }

      // and for its default son if it has one
      NodeId defaultSon = 0;

      if (opData.DD2->hasNodeDefaultSon(followerCurrentNode)) {
        GUM_TRACE(tabu << std::setprecision(20)
                       << "GoDownFollower  -  Descente sur fils par défaut");
        opData.conti.setDD2Node(opData.DD2->nodeDefaultSon(followerCurrentNode));
        defaultSon = GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION(
            opData, leaderParentVar, tabu);

        Idx defaultDomainSize =
            followerCurrentVar->domainSize() - nbExploredModalities;

        if (!nodeCount.exists(defaultSon))
          nodeCount.insert(defaultSon, defaultDomainSize);
        else
          nodeCount[defaultSon] += defaultDomainSize;
      }

      // And we had this node to our graph
      newNode = insertNonTerminalNode(opData, followerCurrentVar, sonsIds,
                                      defaultSon, nodeCount);

      GUM_TRACE(tabu << std::setprecision(20) << "KEY INSERTION : ");
      opData.conti.setDD2Node(followerCurrentNode);
      GUM_TRACE(tabu << std::setprecision(20)
                     << "Context : " << opData.conti.toString() << "Ajout de : "
                     << opData.conti.contextKey() << " -> " << newNode);
      opData.explorationTable->insert(opData.conti.contextKey(), newNode);
      // GUM_TRACE( tabu << std::setprecision( 20 )  << "GODOWNFOLLOWER EXPLORATION
      // STATUS : " << *(opData.explorationTable) );
    }

    // ****************************************************************************************************************
    GUM_TRACE(tabu << std::setprecision(20) << "Fin GoDownFollower - Noeud leader : "
                   << opData.conti.getDD1Node()
                   << " - Noeud follower : " << opData.conti.getDD2Node()
                   << " - Resulting Node : " << newNode);

    return newNode;
  }

} /* end of namespace GUM */

#endif /* GUM_DECISION_DIAGRAM_OPERATOR_FUNCTION_DEFINITION */
