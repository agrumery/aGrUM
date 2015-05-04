/***************************************************************************
*   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
*   {prenom.nom}_at_lip6.fr                                               *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/
/**
* @file
* @brief Implementations of the classes defined in
* InfluenceDiagram/inference/influenceDiagramInference.h.
*  @note For deep understanding of the inherent algorithms implemented here,
*  a strong look at "From Influence Diagrams To Junction Trees, Frank Jensen, Finn V.
* Jensen, Soren L; Dittmer, 1994" is
*  highly recommended.
*  @note __triangulation->eliminationOrder() gives nodes in order in which they
* disappear, meaning that the first one
*  is the first one to be eliminated.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// to ease parsing by IDE
#include <agrum/ID/inference/influenceDiagramInference.h>

namespace gum {
  // Default constructor.
  // @param inDiag The influence diagram on which we perform inferences
  template <typename GUM_SCALAR>
  InfluenceDiagramInference<GUM_SCALAR>::InfluenceDiagramInference(
      const InfluenceDiagram<GUM_SCALAR> &infDiag)
      : IInfluenceDiagramInference<GUM_SCALAR>(infDiag), __triangulation(0),
        __inferencePotential(0), __inferenceUtility(0), __inferenceMade(false) {

    GUM_CONSTRUCTOR(InfluenceDiagramInference);

    // Make modalities map
    NodeProperty<Size> __modalitiesMap;

    // the moral graph does not include utility nodes
    UndiGraph partialMoralGraph(this->influenceDiagram().moralGraph());

    for (const auto node : this->influenceDiagram().nodes()) {
      if (this->influenceDiagram().isUtilityNode(node)) {
        partialMoralGraph.eraseNode(node);
      } else {
        __modalitiesMap.insert(node,
                               this->influenceDiagram().variable(node).domainSize());
      }
    }

    const List<NodeSet> *partialTemporalOrder =
        &(this->influenceDiagram().getPartialTemporalOrder());

    // Make Junction Tree
    __triangulation = new PartialOrderedTriangulation(
        &partialMoralGraph, &__modalitiesMap, partialTemporalOrder);
    __triangulation->junctionTree();
    __makeCliquePropertiesMap();
    __makeStrongJunctionTree();
  }

  // Default destructor.

  template <typename GUM_SCALAR>
  InfluenceDiagramInference<GUM_SCALAR>::~InfluenceDiagramInference() {

    GUM_DESTRUCTOR(InfluenceDiagramInference);

    delete __triangulation;

    __cleanUp();

    for (const auto &prop : __cliquePropertiesMap)
      delete prop.second;

    for (const auto dummyPot : __potentialDummies)
      delete dummyPot;

    for (const auto dummyUtility : __utilityDummies)
      delete dummyUtility;
  }

  template <typename GUM_SCALAR>
  void InfluenceDiagramInference<GUM_SCALAR>::makeInference() {
    __cleanUp();
    NodeId rootClique = __cliqueEliminationMap[0];

    for (const auto chil : __triangulation->junctionTree().neighbours(rootClique))
      __collectChild(rootClique, chil);

    NodeSet separator;
    __reduceClique(__cliquePropertiesMap[rootClique], separator,
                   __inferencePotential, __inferenceUtility);
    __inferenceMade = true;
  }

  // getMEU : Returns the maximum expected utility for this inference on this diagram

  template <typename GUM_SCALAR>
  INLINE GUM_SCALAR InfluenceDiagramInference<GUM_SCALAR>::getMEU() {

    if (!__inferenceMade)
      GUM_ERROR(OperationNotAllowed, "No inference have yet been made");

    Instantiation res(__inferenceUtility);
    return __inferenceUtility->get(res);
  }

  // getBestDecisionChoice : Returns for given decision node the best decision to
  // take under this infernece
  template <typename GUM_SCALAR>
  INLINE Idx InfluenceDiagramInference<GUM_SCALAR>::getBestDecisionChoice(
      NodeId decisionId) {

    if (!__inferenceMade)
      GUM_ERROR(OperationNotAllowed, "No inference have yet been made");

    if (!this->influenceDiagram().isDecisionNode(decisionId))
      GUM_ERROR(InvalidNode, "Node is not a decision node");

    return __utakenDecisionMap[decisionId];
  }

  // displayResult : displays results obtained from inference
  template <typename GUM_SCALAR>
  std::string InfluenceDiagramInference<GUM_SCALAR>::displayResult() {
    std::stringstream stream;

    if (!__inferenceMade)
      GUM_ERROR(OperationNotAllowed, "No inference have yet been made");

    stream << "max EU : " << std::endl;
    stream << *__inferenceUtility << std::endl;

    stream << "Best choices : " << std::endl;

    for (const auto &ut : __utakenDecisionMap)
      stream << "  - Decision " << this->influenceDiagram().variable(ut.first)
             << " : " << this->influenceDiagram().variable(ut.first).label(ut.second)
             << std::endl;

    return stream.str();
  }

  // insertEvidence : inserts new evidence in the graph
  template <typename GUM_SCALAR>
  void InfluenceDiagramInference<GUM_SCALAR>::insertEvidence(
      const List<const Potential<GUM_SCALAR> *> &evidenceList) {

    for (const auto ev : evidenceList)
      __cliquePropertiesMap[__nodeToCliqueMap[this->influenceDiagram().nodeId(
                                (ev)->variable(0))]]->addEvidence(*ev);
  }

  // eraseEvidence : removes a given evidence from the graph

  template <typename GUM_SCALAR>
  void InfluenceDiagramInference<GUM_SCALAR>::eraseEvidence(
      const Potential<GUM_SCALAR> *evidence) {

    if (not(evidence->variablesSequence().size() != 1))
      __cliquePropertiesMap[__nodeToCliqueMap[this->influenceDiagram().nodeId(
                                evidence->variable(0))]]
          ->removeEvidence(evidence->variable(0));
  }

  // eraseAllEvidence : removes all evidence from the graph

  template <typename GUM_SCALAR>
  void InfluenceDiagramInference<GUM_SCALAR>::eraseAllEvidence() {

    for (const auto &elt : __cliquePropertiesMap)
      elt.second->removeAllEvidence();
  }

  /* ****************************************************************************************************
   * **/
  /* ** **/
  /* **                                                                Getters **/
  /* ** **/
  /* ****************************************************************************************************
   * **/

  // getTriangulation : returns the triangulation obtained for this influence diagram

  template <typename GUM_SCALAR>
  INLINE Triangulation &InfluenceDiagramInference<GUM_SCALAR>::getTriangulation() {

    return *__triangulation;
  }

  // __getSeparator :: returns the set of node contains in clique1 inter clique2

  template <typename GUM_SCALAR>
  INLINE const NodeSet &
  InfluenceDiagramInference<GUM_SCALAR>::__getSeparator(NodeId clique_1,
                                                        NodeId clique_2) {

    return __triangulation->junctionTree().separator(clique_1, clique_2);
  }

  // __getClique : for a given node in diagram, returns the clique id  of the first
  // variable to disappear between node variable and its parents variables

  template <typename GUM_SCALAR>
  NodeId InfluenceDiagramInference<GUM_SCALAR>::__getClique(
      const std::vector<NodeId> &eliminationOrder, NodeId id) {

    //***********************************************************************************************************************************
    // First, we create a node set with node id and parents id
    NodeSet idSet;
    idSet.insert(id);

    for (const auto par : this->influenceDiagram().dag().parents(id))
      idSet.insert(par);

    //***********************************************************************************************************************************

    //***********************************************************************************************************************************
    // Then, we search for the first one to be eliminated in elimination order
    for (size_t i = 0; i < eliminationOrder.size(); ++i)
      if (idSet.contains(eliminationOrder[i]))
        return __triangulation->createdJunctionTreeClique(eliminationOrder[i]);

    GUM_ERROR(FatalError, "No clique found for node " << id);
    //***********************************************************************************************************************************
  }

  // __makeCliquePropertiesMap : Builds up clique data structure for the inference

  template <typename GUM_SCALAR>
  void InfluenceDiagramInference<GUM_SCALAR>::__makeCliquePropertiesMap() {

    const std::vector<NodeId> elim = __triangulation->eliminationOrder();

    // Those two sets will contains cliques id for cliques which doesn't have a
    // potential or a utility table at all
    NodeSet potentialsCliquesSet, utilitiesCliqueSet;

    // First pass to create the clique's table
    for (const auto cli : __triangulation->junctionTree().nodes()) {
      __cliquePropertiesMap.insert(cli, new CliqueProperties<GUM_SCALAR>());

      potentialsCliquesSet.insert(cli);
      utilitiesCliqueSet.insert(cli);

      // Insertion in clique properties of the variables contains in the clique
      for (const auto node : __triangulation->junctionTree().clique(cli))
        __cliquePropertiesMap[cli]->addVariable(
            this->influenceDiagram().variable(node));

      // Creation of clique own elimination order (based on the general one)
      __cliquePropertiesMap[cli]->makeEliminationOrder(elim,
                                                       this->influenceDiagram());
    }

    // Second pass to add the potentials into good cliques
    for (size_t i = 0; i < elim.size(); i++) {

      // Récupération de la bonne clique
      NodeId cliqueId = __getClique(elim, elim[i]);
      __nodeToCliqueMap.insert(elim[i], cliqueId);

      // Ajout de la cpt si le noeud est un noeud chance
      if (this->influenceDiagram().isChanceNode(elim[i])) {
        __cliquePropertiesMap[cliqueId]->addPotential(
            this->influenceDiagram().cpt(elim[i]));
        potentialsCliquesSet.erase(cliqueId);
      }
    }

    // Third pass to fill empty cliques with "one" matrices for potentials and "zero"
    // matrices for utilities.
    for (const auto cliq : potentialsCliquesSet)
      __cliquePropertiesMap[cliq]->addPotential(*__makeDummyPotential(cliq));

    // Fourth pass to adress utility table to the good clique
    // We go trought all diagram's nodes in search of utility nodes since they do not
    // appear in elimination order
    for (const auto node : this->influenceDiagram().nodes())
      if (this->influenceDiagram().isUtilityNode(node)) {
        // Récupération de la bonne clique
        NodeId cliqueId = __getClique(elim, node);
        __cliquePropertiesMap[cliqueId]->addUtility(
            this->influenceDiagram().utility(node));
        utilitiesCliqueSet.erase(cliqueId);
      }

    // Fifth pass to fill empty cliques with "zero" matrices for utilities.
    for (const auto cliq : utilitiesCliqueSet)
      __cliquePropertiesMap[cliq]->addUtility(*__makeDummyUtility(cliq));
  }

  template <typename GUM_SCALAR>
  void InfluenceDiagramInference<GUM_SCALAR>::__makeStrongJunctionTree() {

    // Pour chaque clique
    for (const auto cli : __triangulation->junctionTree().nodes()) {

      Sequence<NodeId> eliminationOrder =
          __cliquePropertiesMap[cli]->cliqueEliminationOrder();
      SequenceIterator<NodeId> cliqueNodesIter = eliminationOrder.begin();
      bool validIndex = false;

      // On parcours chaque noeud de la clique par ordre d'élimination, ...
      while (cliqueNodesIter != eliminationOrder.end() && !validIndex) {

        SequenceIterator<NodeId> cliqueRemainingNodesIter = cliqueNodesIter;
        ++cliqueRemainingNodesIter;

        if (cliqueRemainingNodesIter != eliminationOrder.end()) {

          NodeSet suspectedNodes(__triangulation->triangulatedGraph().neighbours(
              *cliqueRemainingNodesIter));

          while (cliqueRemainingNodesIter != eliminationOrder.end() &&
                 !suspectedNodes.empty()) {
            NodeSet possiblesNodes(suspectedNodes);

            for (const auto possibleNode : possiblesNodes)
              if (!__triangulation->triangulatedGraph()
                       .neighbours(*cliqueRemainingNodesIter)
                       .exists(possibleNode))
                suspectedNodes.erase(possibleNode);

            ++cliqueRemainingNodesIter;
          }

          if (!suspectedNodes.empty())
            for (const auto suspectedNode : suspectedNodes)
              if (!eliminationOrder.exists(suspectedNode) &&
                  __IsEliminatedAfter(suspectedNode, *cliqueNodesIter)) {
                validIndex = true;
                break;
              }
        }

        if (!validIndex)
          ++cliqueNodesIter;
      }

      if (validIndex) {
        Idx index = 1;

        for (std::vector<NodeId>::const_iterator
                 eliminationOrderIter = __triangulation->eliminationOrder().begin();
             eliminationOrderIter != __triangulation->eliminationOrder().end() &&
                 *eliminationOrderIter != *cliqueNodesIter;
             ++eliminationOrderIter, ++index)
          ;

        __cliqueEliminationMap.insert(
            __triangulation->eliminationOrder().size() - index, cli);
      } else
        try {
          __cliqueEliminationMap.insert(0, cli);
        } catch (Exception e) {
          throw(e);
        }
    }
  }

  // __IsEliminatedAfter :   checks if  observed  node is eliminated after current
  // node.

  template <typename GUM_SCALAR>
  bool
  InfluenceDiagramInference<GUM_SCALAR>::__IsEliminatedAfter(NodeId observedNode,
                                                             NodeId currentNode) {

    for (std::vector<NodeId>::const_iterator eliminationOrderIter =
             __triangulation->eliminationOrder().begin();
         eliminationOrderIter != __triangulation->eliminationOrder().end();
         ++eliminationOrderIter) {
      if (*eliminationOrderIter == currentNode)
        return true;

      if (*eliminationOrderIter == observedNode)
        return false;
    }

    return false;
  }

  // displayStrongJunctionTree : displays the junction tree obtained for this
  // influence diagram
  template <typename GUM_SCALAR>
  void InfluenceDiagramInference<GUM_SCALAR>::displayStrongJunctionTree(
      std::ostream &stream) {
    stream << std::endl << "Strong junction tree map : " << std::endl;

    for (const auto &elt : __cliqueEliminationMap)
      stream << "Clique  : " << __triangulation->junctionTree().clique(elt.second)
             << " - Index : " << elt.first << std::endl;
  }

  template <typename GUM_SCALAR>
  void InfluenceDiagramInference<GUM_SCALAR>::__cleanUp() {
    if (__inferencePotential != nullptr) {
      delete __inferencePotential;
      __inferencePotential = nullptr;
    }

    if (__inferenceUtility != nullptr) {
      delete __inferenceUtility;
      __inferenceUtility = nullptr;
    }

    for (const auto &prop : __cliquePropertiesMap)
      prop.second->cleanFromInference();

    __utakenDecisionMap.clear();
    __inferenceMade = false;
  }

  //  __collectChild : or when the parents collects message from its child
  template <typename GUM_SCALAR>
  void InfluenceDiagramInference<GUM_SCALAR>::__collectChild(NodeId parent,
                                                             NodeId child) {

    for (const auto nei : __triangulation->junctionTree().neighbours(child))
      if (nei != parent)
        __collectChild(child, nei);

    __absorbClique(child, parent);
  }

  //  __absorbClique : Performs a clique absorption by another one
  template <typename GUM_SCALAR>
  void
  InfluenceDiagramInference<GUM_SCALAR>::__absorbClique(NodeId absorbedCliqueId,
                                                        NodeId absorbingCliqueId) {

    // Recuperation of absorbed clique properties
    CliqueProperties<GUM_SCALAR> *absorbedClique =
        __cliquePropertiesMap[absorbedCliqueId];

    // Get the nodes making the separtion between the absorbed clique and the
    // absorbing one
    NodeSet separator = __getSeparator(absorbedCliqueId, absorbingCliqueId);

    Potential<GUM_SCALAR> *potentialMarginal = 0;
    UtilityTable<GUM_SCALAR> *utilityMarginal = 0;

    // First  we reduce absorbed clique by eleminating clique variables which aren't
    // in the separator.
    // The aim of this operation is to obtained by marginalization a potential and a
    // utility over the variable presents
    // in the separator (and only those one ). Those tables represent the "messages"
    // sent by child
    // clique to its parent.
    __reduceClique(absorbedClique, separator, potentialMarginal, utilityMarginal);

    // Then those tables are add in parents clique property.
    // For the potential, we just add it
    __cliquePropertiesMap[absorbingCliqueId]->addPotential(*potentialMarginal, true);

    // For  the utility table, we need first to divide it by the potential
    Instantiation utilityMarginalInst(utilityMarginal);

    for (utilityMarginalInst.setFirst(); not utilityMarginalInst.end();
         utilityMarginalInst.inc()) {

      GUM_SCALAR uVal = (GUM_SCALAR)0;

      if (potentialMarginal->get(utilityMarginalInst) != (GUM_SCALAR)0)
        uVal = utilityMarginal->get(utilityMarginalInst) /
               potentialMarginal->get(utilityMarginalInst);

      utilityMarginal->set(utilityMarginalInst, uVal);
    }

    // And then we can add the utility table.
    __cliquePropertiesMap[absorbingCliqueId]->addUtility(*utilityMarginal, true);
  }

  // __reduceClique : Performs a clique reduction

  // This operations consists in eliminating each variable of the clique wich are not
  // in the separtor between this clique and its parent clique.
  // Variable elimination is done by performing the correct marging operation on
  // clique potential. That operation is determined by the nature
  // of currently eleminated variable : if its a chance variable, we sum over its
  // modalities, if its a decision node we maximise over its modalities.
  template <typename GUM_SCALAR>
  INLINE void InfluenceDiagramInference<GUM_SCALAR>::__reduceClique(
      CliqueProperties<GUM_SCALAR> *absorbedClique, NodeSet &separator,
      Potential<GUM_SCALAR> *&potentialMarginal,
      UtilityTable<GUM_SCALAR> *&utilityMarginal) {

    Instantiation cliqueInstance(absorbedClique->cliqueInstantiation());
    Sequence<const DiscreteVariable *> cliqueRemainVarList(
        cliqueInstance.variablesSequence());

    // So for each variable of that clique ...
    for (const auto node : absorbedClique->cliqueEliminationOrder()) {
      // if it's not on separtor with its parent
      if (!separator.contains(node)) {
        // Initialisation Operations

        // First we create the tables that will result from variable elimination
        Potential<GUM_SCALAR> *newPotential = new Potential<GUM_SCALAR>();
        UtilityTable<GUM_SCALAR> *newUtility = new UtilityTable<GUM_SCALAR>();

        // Then we need to add all not yet eliminated variables of the clique in ours
        // new table
        cliqueRemainVarList.erase(&(this->influenceDiagram().variable(node)));

        for (const auto remain : cliqueRemainVarList) {
          newPotential->add(*remain);
          newUtility->add(*remain);
        }

        // And finally, before doing marginalizing operations,
        // we add an instanciation to those tables to ease marginalizing operations.
        // Note that we only need one tablel instance because the other one 's got
        // exactly sames variables
        Instantiation newInstantiation(newPotential);

        //=====================================================================================
        // Marginalization

        // Then we fill the new potentials over all their values by marginalizing the
        // previous one
        for (newInstantiation.setFirst(); not newInstantiation.end();
             newInstantiation.inc()) {

          // Various initialization
          GUM_SCALAR potentialValue = (GUM_SCALAR)0;
          GUM_SCALAR utilityValue = (GUM_SCALAR)0;

          if (this->influenceDiagram().isDecisionNode(node))
            utilityValue = -1 * (std::numeric_limits<GUM_SCALAR>::max());

          // Then we compute value for current newInstanciation
          cliqueInstance.setVals(newInstantiation);

          for (cliqueInstance.setFirstOut(newInstantiation);
               not cliqueInstance.end(); cliqueInstance.incOut(newInstantiation)) {

            // Récupération de la valeur de la cpt de la clique pour cette instance
            GUM_SCALAR currentPotential = (GUM_SCALAR)1;

            if (potentialMarginal == 0) {
              // If there's no ancient potential it means that we haven't yet compute
              // him
              for (const auto &pot : absorbedClique->potentialBucket()) {
                pot.second->setVals(cliqueInstance);
                currentPotential *= pot.first->get(*pot.second);
              }
            } else {
              Instantiation potentialMarginalInst(potentialMarginal);
              potentialMarginalInst.setVals(cliqueInstance);
              currentPotential = potentialMarginal->get(potentialMarginalInst);
            }

            // Récupération de la valeur d'utilité de la clique pour cette instance
            GUM_SCALAR currentUtility = (GUM_SCALAR)0;

            if (utilityMarginal == 0) {
              for (const auto &uti : absorbedClique->utilityBucket()) {
                uti.second->setVals(cliqueInstance);
                currentUtility += uti.first->get(*uti.second);
              }

              currentUtility *= currentPotential;
            } else {
              Instantiation utilityMarginalInst(utilityMarginal);
              utilityMarginalInst.setVals(cliqueInstance);
              currentUtility = utilityMarginal->get(utilityMarginalInst);
            }

            // Marginalization
            if (this->influenceDiagram().isDecisionNode(node)) {
              if (potentialValue < currentPotential) {
                potentialValue = currentPotential;
              }

              if (utilityValue < currentUtility) {
                utilityValue = currentUtility;

                if (__utakenDecisionMap.exists(node))
                  __utakenDecisionMap.erase(node);

                __utakenDecisionMap.insert(
                    node,
                    cliqueInstance.val(this->influenceDiagram().variable(node)));
              }
            } else {
              potentialValue += currentPotential;
              utilityValue += currentUtility;
            }
          }

          // And finally we update  the potentials with computed value for
          // newInstanciation
          newPotential->set(newInstantiation, potentialValue);
          newUtility->set(newInstantiation, utilityValue);
        }

        //=====================================================================================
        // Updates of tables
        if (potentialMarginal != 0)
          delete potentialMarginal;

        potentialMarginal = newPotential;

        if (utilityMarginal != 0)
          delete utilityMarginal;

        utilityMarginal = newUtility;

        //=====================================================================================
        // Then we removed variable from clique list of variable  ...
        cliqueInstance.erase(this->influenceDiagram().variable(node));
      }
    }
  }

  //  __makeDummyPotential : creates a generic potential

  template <typename GUM_SCALAR>
  INLINE Potential<GUM_SCALAR> *
  InfluenceDiagramInference<GUM_SCALAR>::__makeDummyPotential(NodeId cliqueId) {

    Potential<GUM_SCALAR> *pot =
        new Potential<GUM_SCALAR>(new MultiDimSparse<GUM_SCALAR>((GUM_SCALAR)1));
    __potentialDummies.insert(pot);

    for (const auto cliqueNode : __triangulation->junctionTree().clique(cliqueId))
      pot->add(this->influenceDiagram().variable(cliqueNode));

    pot->normalize();
    return pot;
  }

  //  __makeDummyUtility : creates a generic utility

  template <typename GUM_SCALAR>
  INLINE UtilityTable<GUM_SCALAR> *
  InfluenceDiagramInference<GUM_SCALAR>::__makeDummyUtility(NodeId cliqueId) {
    UtilityTable<GUM_SCALAR> *ut =
        new UtilityTable<GUM_SCALAR>(new MultiDimSparse<GUM_SCALAR>((GUM_SCALAR)0));
    __utilityDummies.insert(ut);

    for (const auto cliqueNode : __triangulation->junctionTree().clique(cliqueId))
      ut->add(this->influenceDiagram().variable(cliqueNode));

    return ut;
  }

  // Default constructor
  template <typename GUM_SCALAR> CliqueProperties<GUM_SCALAR>::CliqueProperties() {
    GUM_CONSTRUCTOR(CliqueProperties);
  }

  // Default destructor
  template <typename GUM_SCALAR> CliqueProperties<GUM_SCALAR>::~CliqueProperties() {
    GUM_DESTRUCTOR(CliqueProperties);

    cleanFromInference();
    removeAllEvidence();

    for (const auto &pot : __potentialBucket)
      delete pot.second;

    for (const auto &uti : __utilityBucket)
      delete uti.second;
  }

  // addVariable : adds a variable to the clique
  template <typename GUM_SCALAR>
  void CliqueProperties<GUM_SCALAR>::addVariable(const DiscreteVariable &v) {
    try {
      __allVarsInst.add(v);
    } catch (DuplicateElement &e) {
      // Nothing to do then!
    }
  }

  // cliqueVariables : returns List containing all variables contained in this clique
  // @return returns List containing all variables contained in this clique
  template <typename GUM_SCALAR>
  INLINE const Sequence<const DiscreteVariable *> &
  CliqueProperties<GUM_SCALAR>::cliqueVariables() {
    return __allVarsInst.variablesSequence();
  }

  // cliqueInstantiation : returns instanciation on variable within this clique
  // @return returns instanciation on variable within this clique
  template <typename GUM_SCALAR>
  INLINE Instantiation &CliqueProperties<GUM_SCALAR>::cliqueInstantiation() {
    return __allVarsInst;
  }

  // addPotential : adds a potential to this clique
  // The removable boolean inidcates if this potential can be cleaned off after an
  // inference or not
  template <typename GUM_SCALAR>
  void CliqueProperties<GUM_SCALAR>::addPotential(const Potential<GUM_SCALAR> &cpt,
                                                  bool removable) {
    __potentialBucket.insert(&cpt, new Instantiation(cpt));

    if (removable)
      __removablePotentialList.insert(&cpt);

    for (Sequence<const DiscreteVariable *>::iterator_safe iter = cpt.begin();
         iter != cpt.end(); ++iter) {
      if (removable && !__allVarsInst.contains(**iter))
        try {
          __removableVarList.insert(*iter);
        } catch (DuplicateElement &e) {
          // Nothing to do then!
        }

      addVariable(**iter);
    }
  }

  // potentialBucket : Returns the potential bucket of this Clique
  template <typename GUM_SCALAR>
  INLINE const HashTable<const Potential<GUM_SCALAR> *, Instantiation *> &
  CliqueProperties<GUM_SCALAR>::potentialBucket() {
    return __potentialBucket;
  }

  // addUtility : adds a utility table to this clique
  // The removable boolean inidcates if this utilityTable can be cleaned off after an
  // inference or not
  template <typename GUM_SCALAR>
  void CliqueProperties<GUM_SCALAR>::addUtility(const UtilityTable<GUM_SCALAR> &ut,
                                                bool removable) {
    __utilityBucket.insert(&ut, new Instantiation(ut));

    if (removable)
      __removableUtilityList.insert(&ut);

    for (Sequence<const DiscreteVariable *>::iterator_safe iter = ut.begin();
         iter != ut.end(); ++iter) {
      if (removable && !__allVarsInst.contains(**iter))
        try {
          __removableVarList.insert(*iter);
        } catch (DuplicateElement &e) {
          // Nothing to do then!
        }

      addVariable(**iter);
    }
  }

  //  utilityBucket : Returns the utiluty table bucket of this Clique
  template <typename GUM_SCALAR>
  INLINE const HashTable<const UtilityTable<GUM_SCALAR> *, Instantiation *> &
  CliqueProperties<GUM_SCALAR>::utilityBucket() {
    return __utilityBucket;
  }

  // cleanFromInference : performs a cleaning of the clique after an inference
  // This is done by removing the "message" given by a child clique to its parents,
  // meaning the potentials and utility tables obtained by
  // variable elemination in the clique.
  template <typename GUM_SCALAR>
  void CliqueProperties<GUM_SCALAR>::cleanFromInference() {
    // Removed added variables during inference (normally, the __removableVarList is
    // empty, but we never know )
    for (const auto removableVar : __removableVarList)
      __allVarsInst.erase(*removableVar);

    // Removed added potentials during inference
    for (const auto removablePot : __removablePotentialList) {
      delete __potentialBucket[removablePot];
      __potentialBucket.erase(removablePot);
      delete removablePot;
    }

    // Removed added utility tables during inference
    for (const auto removabledUt : __removableUtilityList) {
      delete __utilityBucket[removabledUt];
      __utilityBucket.erase(removabledUt);
      delete removabledUt;
    }

    __removableVarList.clear();
    __removablePotentialList.clear();
    __removableUtilityList.clear();
  }

  // makeEliminationOrder : creates an elimination order in the clique compatible
  // with global elimination order
  // The global elimination order is given by elim.
  template <typename GUM_SCALAR>
  void CliqueProperties<GUM_SCALAR>::makeEliminationOrder(
      const std::vector<NodeId> &elim, const InfluenceDiagram<GUM_SCALAR> &infDiag) {
    for (size_t i = 0; i < elim.size(); i++) {
      if (__allVarsInst.contains(infDiag.variable(elim[i])))
        __eliminationOrder.insert(elim[i]);
    }
  }

  // cliqueEliminationOrder : returns the elimination sequence for this clique
  template <typename GUM_SCALAR>
  INLINE const Sequence<NodeId> &
  CliqueProperties<GUM_SCALAR>::cliqueEliminationOrder() {
    return __eliminationOrder;
  }

  // addEvidence : add evidence over one variable present in the clique
  // @throw OperationNotAllowed if evidence has more than one variable.
  // @throw NotFound Raised if the evidence is on a variable not present in this
  // clique.
  // @throw DuplicateElement, if another evidence over this variable exists for this
  // clique
  template <typename GUM_SCALAR>
  void
  CliqueProperties<GUM_SCALAR>::addEvidence(const Potential<GUM_SCALAR> &evidence) {

    // To avoid interference
    cleanFromInference();

    // First, we check if evidence is over only one variable.
    if (evidence.variablesSequence().size() != 1) {
      GUM_ERROR(OperationNotAllowed, "expected evidence on 1 variable, found on "
                                         << evidence.variablesSequence().size());
    }

    // Then we assure us that that variable is in the clique
    if (!__allVarsInst.contains(evidence.variablesSequence().atPos(0))) {
      GUM_ERROR(NotFound, evidence.variablesSequence().atPos(0)->name()
                              << " not found in clique ");
    }

    __evidences.insert(evidence.variablesSequence().atPos(0), &evidence);

    __potentialBucket.insert(&evidence, new Instantiation(evidence));
  }

  // evidences : Returns the mapping of evidences on variables in this clique
  template <typename GUM_SCALAR>
  INLINE const HashTable<const DiscreteVariable *, const Potential<GUM_SCALAR> *> &
  CliqueProperties<GUM_SCALAR>::evidences() const {
    return __evidences;
  }

  // removeEvidence : Removes the evidence over v
  template <typename GUM_SCALAR>
  INLINE void
  CliqueProperties<GUM_SCALAR>::removeEvidence(const DiscreteVariable &v) {
    const Potential<GUM_SCALAR> *evi = __evidences[&v];

    delete __potentialBucket[evi];
    __potentialBucket.erase(evi);
    __evidences.erase(&v);
  }

  // removeAllEvidence : Removes all the evidences
  template <typename GUM_SCALAR>
  INLINE void CliqueProperties<GUM_SCALAR>::removeAllEvidence() {
    for (const auto &elt : __evidences)
      removeEvidence(*elt.first);

    __evidences.clear();
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

// kate: indent-mode cstyle; indent-width 2; replace-tabs on;
