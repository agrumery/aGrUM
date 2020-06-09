
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief Implementations of the classes defined in
 * InfluenceDiagram/inference/influenceDiagramInference.h.
 *  @note For deep understanding of the inherent algorithms implemented here,
 *  a strong look at "From Influence Diagrams To Junction Trees, Frank Jensen,
 * Finn V.
 * Jensen, Soren L; Dittmer, 1994" is
 *  highly recommended.
 *  @note triangulation__->eliminationOrder() gives nodes in order in which they
 * disappear, meaning that the first one
 *  is the first one to be eliminated.
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// to ease parsing by IDE
#  include <agrum/ID/inference/influenceDiagramInference.h>

namespace gum {
  // Default constructor.
  // @param inDiag The influence diagram on which we perform inferences
  template < typename GUM_SCALAR >
  InfluenceDiagramInference< GUM_SCALAR >::InfluenceDiagramInference(
     const InfluenceDiagram< GUM_SCALAR >& infDiag) :
      IInfluenceDiagramInference< GUM_SCALAR >(infDiag),
      triangulation__(0), inferencePotential__(0), inferenceUtility__(0),
      inferenceMade__(false) {
    GUM_CONSTRUCTOR(InfluenceDiagramInference);

    // Make modalities map
    NodeProperty< Size > modalitiesMap__;

    // the moral graph does not include utility nodes
    UndiGraph partialMoralGraph(this->influenceDiagram().moralGraph());

    for (const auto node: this->influenceDiagram().nodes()) {
      if (this->influenceDiagram().isUtilityNode(node)) {
        partialMoralGraph.eraseNode(node);
      } else {
        modalitiesMap__.insert(
           node, this->influenceDiagram().variable(node).domainSize());
      }
    }

    const List< NodeSet >* partialTemporalOrder =
       &(this->influenceDiagram().getPartialTemporalOrder());

    // Make Junction Tree
    triangulation__ = new PartialOrderedTriangulation(
       &partialMoralGraph, &modalitiesMap__, partialTemporalOrder);
    triangulation__->junctionTree();
    makeCliquePropertiesMap__();
    makeStrongJunctionTree__();
  }

  // Default destructor.

  template < typename GUM_SCALAR >
  InfluenceDiagramInference< GUM_SCALAR >::~InfluenceDiagramInference() {
    GUM_DESTRUCTOR(InfluenceDiagramInference);

    delete triangulation__;

    cleanUp__();

    for (const auto& prop: cliquePropertiesMap__)
      delete prop.second;

    for (const auto dummyPot: potentialDummies__)
      delete dummyPot;

    for (const auto dummyUtility: utilityDummies__)
      delete dummyUtility;
  }

  template < typename GUM_SCALAR >
  void InfluenceDiagramInference< GUM_SCALAR >::makeInference() {
    cleanUp__();
    NodeId rootClique = cliqueEliminationMap__[0];

    for (const auto chil: triangulation__->junctionTree().neighbours(rootClique))
      collectChild__(rootClique, chil);

    NodeSet separator;
    reduceClique__(cliquePropertiesMap__[rootClique],
                   separator,
                   inferencePotential__,
                   inferenceUtility__);
    inferenceMade__ = true;
  }

  // getMEU : Returns the maximum expected utility for this inference on this
  // diagram

  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR InfluenceDiagramInference< GUM_SCALAR >::getMEU() {
    if (!inferenceMade__)
      GUM_ERROR(OperationNotAllowed, "No inference have yet been made");

    Instantiation res(inferenceUtility__);
    auto          poe = inferencePotential__->get(res);
    if (poe == GUM_SCALAR(0))
      GUM_ERROR(OperationNotAllowed, "Probability of evidence is null !");
    return inferenceUtility__->get(res) / poe;
  }

  // getBestDecisionChoice : Returns for given decision node the best decision
  // to
  // take under this infernece
  template < typename GUM_SCALAR >
  INLINE Idx InfluenceDiagramInference< GUM_SCALAR >::getBestDecisionChoice(
     NodeId decisionId) {
    if (!inferenceMade__)
      GUM_ERROR(OperationNotAllowed, "No inference have yet been made");

    if (!this->influenceDiagram().isDecisionNode(decisionId))
      GUM_ERROR(InvalidNode, "Node is not a decision node");

    return utakenDecisionMap__[decisionId];
  }

  // displayResult : displays results obtained from inference
  template < typename GUM_SCALAR >
  std::string InfluenceDiagramInference< GUM_SCALAR >::displayResult() {
    std::stringstream stream;

    if (!inferenceMade__)
      GUM_ERROR(OperationNotAllowed, "No inference have yet been made");

    stream << "max EU : " << getMEU() << std::endl;

    stream << "Best choices : " << std::endl;

    for (const auto& ut: utakenDecisionMap__)
      stream << "  - Decision " << this->influenceDiagram().variable(ut.first)
             << " : "
             << this->influenceDiagram().variable(ut.first).label(ut.second)
             << std::endl;

    return stream.str();
  }

  // insertEvidence : inserts new evidence in the graph
  template < typename GUM_SCALAR >
  void InfluenceDiagramInference< GUM_SCALAR >::insertEvidence(
     const List< const Potential< GUM_SCALAR >* >& evidenceList) {
    for (const auto ev: evidenceList)
      cliquePropertiesMap__[nodeToCliqueMap__[this->influenceDiagram().nodeId(
                               (ev)->variable(0))]]
         ->addEvidence(*ev);
  }

  // eraseEvidence : removes a given evidence from the graph

  template < typename GUM_SCALAR >
  void InfluenceDiagramInference< GUM_SCALAR >::eraseEvidence(
     const Potential< GUM_SCALAR >* evidence) {
    if (!(evidence->variablesSequence().size() != 1))
      cliquePropertiesMap__[nodeToCliqueMap__[this->influenceDiagram().nodeId(
                               evidence->variable(0))]]
         ->removeEvidence(evidence->variable(0));
  }

  // eraseAllEvidence : removes all evidence from the graph

  template < typename GUM_SCALAR >
  void InfluenceDiagramInference< GUM_SCALAR >::eraseAllEvidence() {
    for (const auto& elt: cliquePropertiesMap__)
      elt.second->removeAllEvidence();
  }

  /* ****************************************************************************************************
   * **/
  /* ** **/
  /* **                                                                Getters
   * **/
  /* ** **/
  /* ****************************************************************************************************
   * **/

  // getTriangulation : returns the triangulation obtained for this influence
  // diagram

  template < typename GUM_SCALAR >
  INLINE Triangulation&
         InfluenceDiagramInference< GUM_SCALAR >::getTriangulation() {
    return *triangulation__;
  }

  // getSeparator__ :: returns the set of node contains in clique1 inter clique2

  template < typename GUM_SCALAR >
  INLINE const NodeSet&
               InfluenceDiagramInference< GUM_SCALAR >::getSeparator__(NodeId clique_1,
                                                             NodeId clique_2) {
    return triangulation__->junctionTree().separator(clique_1, clique_2);
  }

  // getClique__ : for a given node in diagram, returns the clique id  of the
  // first
  // variable to disappear between node variable and its parents variables

  template < typename GUM_SCALAR >
  NodeId InfluenceDiagramInference< GUM_SCALAR >::getClique__(
     const std::vector< NodeId >& eliminationOrder, NodeId id) {
    //***********************************************************************************************************************************
    // First, we create a node set with node id and parents id
    NodeSet idSet;
    idSet.insert(id);

    for (const auto par: this->influenceDiagram().parents(id))
      idSet.insert(par);

    //***********************************************************************************************************************************

    //***********************************************************************************************************************************
    // Then, we search for the first one to be eliminated in elimination order
    for (size_t i = 0; i < eliminationOrder.size(); ++i)
      if (idSet.contains(eliminationOrder[i]))
        return triangulation__->createdJunctionTreeClique(eliminationOrder[i]);

    GUM_ERROR(FatalError, "No clique found for node " << id);
    //***********************************************************************************************************************************
  }

  // makeCliquePropertiesMap__ : Builds up clique data structure for the
  // inference

  template < typename GUM_SCALAR >
  void InfluenceDiagramInference< GUM_SCALAR >::makeCliquePropertiesMap__() {
    const std::vector< NodeId > elim = triangulation__->eliminationOrder();

    // Those two sets will contains cliques id for cliques which doesn't have a
    // potential or a utility table at all
    NodeSet potentialsCliquesSet, utilitiesCliqueSet;

    // First pass to create the clique's table
    for (const auto cli: triangulation__->junctionTree().nodes()) {
      cliquePropertiesMap__.insert(cli, new CliqueProperties< GUM_SCALAR >());

      potentialsCliquesSet.insert(cli);
      utilitiesCliqueSet.insert(cli);

      // Insertion in clique properties of the variables contains in the clique
      for (const auto node: triangulation__->junctionTree().clique(cli))
        cliquePropertiesMap__[cli]->addVariable(
           this->influenceDiagram().variable(node));

      // Creation of clique own elimination order (based on the general one)
      cliquePropertiesMap__[cli]->makeEliminationOrder(elim,
                                                       this->influenceDiagram());
    }

    // Second pass to add the potentials into good cliques
    for (size_t i = 0; i < elim.size(); i++) {
      // Récupération de la bonne clique
      NodeId cliqueId = getClique__(elim, elim[i]);
      nodeToCliqueMap__.insert(elim[i], cliqueId);

      // Ajout de la cpt si le noeud est un noeud chance
      if (this->influenceDiagram().isChanceNode(elim[i])) {
        cliquePropertiesMap__[cliqueId]->addPotential(
           this->influenceDiagram().cpt(elim[i]));
        potentialsCliquesSet.erase(cliqueId);
      }
    }

    // Third pass to fill empty cliques with "one" matrices for potentials and
    // "zero"
    // matrices for utilities.
    for (const auto cliq: potentialsCliquesSet)
      cliquePropertiesMap__[cliq]->addPotential(*makeDummyPotential__(cliq));

    // Fourth pass to adress utility table to the good clique
    // We go trought all diagram's nodes in search of utility nodes since they
    // do not
    // appear in elimination order
    for (const auto node: this->influenceDiagram().nodes())
      if (this->influenceDiagram().isUtilityNode(node)) {
        // Récupération de la bonne clique
        NodeId cliqueId = getClique__(elim, node);
        cliquePropertiesMap__[cliqueId]->addUtility(
           this->influenceDiagram().utility(node));
        utilitiesCliqueSet.erase(cliqueId);
      }

    // Fifth pass to fill empty cliques with "zero" matrices for utilities.
    for (const auto cliq: utilitiesCliqueSet)
      cliquePropertiesMap__[cliq]->addUtility(*makeDummyUtility__(cliq));
  }

  template < typename GUM_SCALAR >
  void InfluenceDiagramInference< GUM_SCALAR >::makeStrongJunctionTree__() {
    // Pour chaque clique
    for (NodeId cli: triangulation__->junctionTree().nodes()) {
      Sequence< NodeId > eliminationOrder =
         cliquePropertiesMap__[cli]->cliqueEliminationOrder();
      SequenceIterator< NodeId > cliqueNodesIter = eliminationOrder.begin();
      bool                       validIndex = false;

      // On parcours chaque noeud de la clique par ordre d'élimination, ...
      while (cliqueNodesIter != eliminationOrder.end() && !validIndex) {
        SequenceIterator< NodeId > cliqueRemainingNodesIter = cliqueNodesIter;
        ++cliqueRemainingNodesIter;

        if (cliqueRemainingNodesIter != eliminationOrder.end()) {
          NodeSet suspectedNodes(triangulation__->triangulatedGraph().neighbours(
             *cliqueRemainingNodesIter));

          while (cliqueRemainingNodesIter != eliminationOrder.end()
                 && !suspectedNodes.empty()) {
            NodeSet possiblesNodes(suspectedNodes);

            for (const auto possibleNode: possiblesNodes)
              if (!triangulation__->triangulatedGraph()
                      .neighbours(*cliqueRemainingNodesIter)
                      .exists(possibleNode))
                suspectedNodes.erase(possibleNode);

            ++cliqueRemainingNodesIter;
          }

          if (!suspectedNodes.empty())
            for (const auto suspectedNode: suspectedNodes)
              if (!eliminationOrder.exists(suspectedNode)
                  && IsEliminatedAfter__(suspectedNode, *cliqueNodesIter)) {
                validIndex = true;
                break;
              }
        }

        if (!validIndex) ++cliqueNodesIter;
      }

      if (validIndex) {
        Idx index = 1;

        for (std::vector< NodeId >::const_iterator eliminationOrderIter =
                triangulation__->eliminationOrder().begin();
             eliminationOrderIter != triangulation__->eliminationOrder().end()
             && *eliminationOrderIter != *cliqueNodesIter;
             ++eliminationOrderIter, ++index)
          ;

        cliqueEliminationMap__.insert(
           Size(triangulation__->eliminationOrder().size() - index), cli);
      } else
        try {
          cliqueEliminationMap__.insert(0, cli);
        } catch (Exception& e) { throw(e); }
    }
  }

  // IsEliminatedAfter__ :   checks if  observed  node is eliminated after
  // current
  // node.

  template < typename GUM_SCALAR >
  bool InfluenceDiagramInference< GUM_SCALAR >::IsEliminatedAfter__(
     NodeId observedNode, NodeId currentNode) {
    for (std::vector< NodeId >::const_iterator eliminationOrderIter =
            triangulation__->eliminationOrder().begin();
         eliminationOrderIter != triangulation__->eliminationOrder().end();
         ++eliminationOrderIter) {
      if (*eliminationOrderIter == currentNode) return true;

      if (*eliminationOrderIter == observedNode) return false;
    }

    return false;
  }

  // displayStrongJunctionTree : displays the junction tree obtained for this
  // influence diagram
  template < typename GUM_SCALAR >
  void InfluenceDiagramInference< GUM_SCALAR >::displayStrongJunctionTree(
     std::ostream& stream) {
    stream << std::endl << "Strong junction tree map : " << std::endl;

    for (const auto& elt: cliqueEliminationMap__)
      stream << "Clique  : " << triangulation__->junctionTree().clique(elt.second)
             << " - Index : " << elt.first << std::endl;
  }

  template < typename GUM_SCALAR >
  void InfluenceDiagramInference< GUM_SCALAR >::cleanUp__() {
    if (inferencePotential__ != nullptr) {
      delete inferencePotential__;
      inferencePotential__ = nullptr;
    }

    if (inferenceUtility__ != nullptr) {
      delete inferenceUtility__;
      inferenceUtility__ = nullptr;
    }

    for (const auto& prop: cliquePropertiesMap__)
      prop.second->cleanFromInference();

    utakenDecisionMap__.clear();
    inferenceMade__ = false;
  }

  //  collectChild__ : or when the parents collects message from its child
  template < typename GUM_SCALAR >
  void InfluenceDiagramInference< GUM_SCALAR >::collectChild__(NodeId parent,
                                                               NodeId child) {
    for (const auto nei: triangulation__->junctionTree().neighbours(child))
      if (nei != parent) collectChild__(child, nei);

    absorbClique__(child, parent);
  }

  //  absorbClique__ : Performs a clique absorption by another one
  template < typename GUM_SCALAR >
  void InfluenceDiagramInference< GUM_SCALAR >::absorbClique__(
     NodeId absorbedCliqueId, NodeId absorbingCliqueId) {
    // Recuperation of absorbed clique properties
    CliqueProperties< GUM_SCALAR >* absorbedClique =
       cliquePropertiesMap__[absorbedCliqueId];

    // Get the nodes making the separtion between the absorbed clique and the
    // absorbing one
    NodeSet separator = getSeparator__(absorbedCliqueId, absorbingCliqueId);

    Potential< GUM_SCALAR >* potentialMarginal = 0;
    Potential< GUM_SCALAR >* utilityMarginal = 0;

    // First  we reduce absorbed clique by eleminating clique variables which
    // aren't
    // in the separator.
    // The aim of this operation is to obtained by marginalization a potential
    // and a
    // utility over the variable presents
    // in the separator (and only those one ). Those tables represent the
    // "messages"
    // sent by child
    // clique to its parent.
    reduceClique__(absorbedClique, separator, potentialMarginal, utilityMarginal);

    // Then those tables are add in parents clique property.
    // For the potential, we just add it
    cliquePropertiesMap__[absorbingCliqueId]->addPotential(*potentialMarginal,
                                                           true);

    // For  the utility table, we need first to divide it by the potential
    Instantiation utilityMarginalInst(utilityMarginal);

    for (utilityMarginalInst.setFirst(); !utilityMarginalInst.end();
         utilityMarginalInst.inc()) {
      GUM_SCALAR uVal = (GUM_SCALAR)0;

      if (potentialMarginal->get(utilityMarginalInst) != (GUM_SCALAR)0)
        uVal = utilityMarginal->get(utilityMarginalInst)
               / potentialMarginal->get(utilityMarginalInst);

      utilityMarginal->set(utilityMarginalInst, uVal);
    }

    // And then we can add the utility table.
    cliquePropertiesMap__[absorbingCliqueId]->addUtility(*utilityMarginal, true);
  }

  // reduceClique__ : Performs a clique reduction

  // This operations consists in eliminating each variable of the clique wich
  // are not in the separator between this clique and its parent clique.
  // Variable elimination is done by performing the correct marging operation on
  // clique potential. That operation is determined by the nature
  // of currently eliminated variable : if its a chance variable, we sum over
  // its modalities, if its a decision node we maximise over its modalities.
  template < typename GUM_SCALAR >
  INLINE void InfluenceDiagramInference< GUM_SCALAR >::reduceClique__(
     CliqueProperties< GUM_SCALAR >* absorbedClique,
     NodeSet&                        separator,
     Potential< GUM_SCALAR >*&       potentialMarginal,
     Potential< GUM_SCALAR >*&       utilityMarginal) {
    Instantiation cliqueInstance(absorbedClique->cliqueInstantiation());
    Sequence< const DiscreteVariable* > cliqueRemainVarList(
       cliqueInstance.variablesSequence());

    // So for each variable of that clique ...
    for (const auto node: absorbedClique->cliqueEliminationOrder()) {
      // if it's not on separator with its parent
      if (!separator.contains(node)) {
        // Initialisation Operations

        // First we create the tables that will result from variable elimination
        Potential< GUM_SCALAR >* newPotential = new Potential< GUM_SCALAR >();
        Potential< GUM_SCALAR >* newUtility = new Potential< GUM_SCALAR >();

        // Then we need to add all not yet eliminated variables of the clique in
        // our new table
        cliqueRemainVarList.erase(&(this->influenceDiagram().variable(node)));

        for (const auto remain: cliqueRemainVarList) {
          newPotential->add(*remain);
          newUtility->add(*remain);
        }

        // And finally, before doing marginalizing operations,
        // we add an instanciation to those tables to ease marginalizing
        // operations.
        // Note that we only need one tablel instance because the other one 's
        // got
        // exactly sames variables
        Instantiation newInstantiation(newPotential);

        //=====================================================================================
        // Marginalization

        // Then we fill the new potentials over all their values by
        // marginalizing the
        // previous one
        for (newInstantiation.setFirst(); !newInstantiation.end();
             newInstantiation.inc()) {
          // Various initialization
          GUM_SCALAR potentialValue = (GUM_SCALAR)0;
          GUM_SCALAR utilityValue = (GUM_SCALAR)0;

          if (this->influenceDiagram().isDecisionNode(node))
            utilityValue = -1 * (std::numeric_limits< GUM_SCALAR >::max());

          // Then we compute value for current newInstanciation
          cliqueInstance.setVals(newInstantiation);

          for (cliqueInstance.setFirstOut(newInstantiation); !cliqueInstance.end();
               cliqueInstance.incOut(newInstantiation)) {
            // Récupération de la valeur de la cpt de la clique pour cette
            // instance
            GUM_SCALAR currentPotential = (GUM_SCALAR)1;

            if (potentialMarginal == 0) {
              // If there's no ancient potential it means that we haven't yet
              // compute
              // him
              for (const auto& pot: absorbedClique->potentialBucket()) {
                pot.second->setVals(cliqueInstance);
                currentPotential *= pot.first->get(*pot.second);
              }
            } else {
              Instantiation potentialMarginalInst(potentialMarginal);
              potentialMarginalInst.setVals(cliqueInstance);
              currentPotential = potentialMarginal->get(potentialMarginalInst);
            }

            // Récupération de la valeur d'utilité de la clique pour cette
            // instance
            GUM_SCALAR currentUtility = (GUM_SCALAR)0;

            if (utilityMarginal == 0) {
              for (const auto& uti: absorbedClique->utilityBucket()) {
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

                if (utakenDecisionMap__.exists(node))
                  utakenDecisionMap__.erase(node);

                utakenDecisionMap__.insert(
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
        if (potentialMarginal != nullptr) delete potentialMarginal;

        potentialMarginal = newPotential;

        if (utilityMarginal != nullptr) delete utilityMarginal;

        utilityMarginal = newUtility;

        //=====================================================================================
        // Then we removed variable from clique list of variable  ...
        cliqueInstance.erase(this->influenceDiagram().variable(node));
      }
    }
  }

  //  makeDummyPotential__ : creates a generic potential

  template < typename GUM_SCALAR >
  INLINE Potential< GUM_SCALAR >*
         InfluenceDiagramInference< GUM_SCALAR >::makeDummyPotential__(
        NodeId cliqueId) {
    Potential< GUM_SCALAR >* pot = new Potential< GUM_SCALAR >();

    for (const auto cliqueNode: triangulation__->junctionTree().clique(cliqueId))
      pot->add(this->influenceDiagram().variable(cliqueNode));
    pot->fill(GUM_SCALAR(1));
    pot->normalize();

    potentialDummies__.insert(pot);
    return pot;
  }

  //  makeDummyUtility__ : creates a generic utility

  template < typename GUM_SCALAR >
  INLINE Potential< GUM_SCALAR >*
         InfluenceDiagramInference< GUM_SCALAR >::makeDummyUtility__(NodeId cliqueId) {
    Potential< GUM_SCALAR >* ut = new Potential< GUM_SCALAR >();

    for (const auto cliqueNode: triangulation__->junctionTree().clique(cliqueId))
      ut->add(this->influenceDiagram().variable(cliqueNode));

    ut->fill(GUM_SCALAR(0));
    utilityDummies__.insert(ut);
    return ut;
  }

  // Default constructor
  template < typename GUM_SCALAR >
  CliqueProperties< GUM_SCALAR >::CliqueProperties() {
    GUM_CONSTRUCTOR(CliqueProperties);
  }

  // Default destructor
  template < typename GUM_SCALAR >
  CliqueProperties< GUM_SCALAR >::~CliqueProperties() {
    GUM_DESTRUCTOR(CliqueProperties);

    cleanFromInference();
    removeAllEvidence();

    for (const auto& pot: potentialBucket__)
      delete pot.second;

    for (const auto& uti: utilityBucket__)
      delete uti.second;
  }

  // addVariable : adds a variable to the clique
  template < typename GUM_SCALAR >
  void CliqueProperties< GUM_SCALAR >::addVariable(const DiscreteVariable& v) {
    try {
      allVarsInst__.add(v);
    } catch (DuplicateElement&) {
      // Nothing to do then!
    }
  }

  // cliqueVariables : returns List containing all variables contained in this
  // clique
  // @return returns List containing all variables contained in this clique
  template < typename GUM_SCALAR >
  INLINE const Sequence< const DiscreteVariable* >&
               CliqueProperties< GUM_SCALAR >::cliqueVariables() {
    return allVarsInst__.variablesSequence();
  }

  // cliqueInstantiation : returns instanciation on variable within this clique
  // @return returns instanciation on variable within this clique
  template < typename GUM_SCALAR >
  INLINE Instantiation& CliqueProperties< GUM_SCALAR >::cliqueInstantiation() {
    return allVarsInst__;
  }

  // addPotential : adds a potential to this clique
  // The removable boolean inidcates if this potential can be cleaned off after
  // an
  // inference or not
  template < typename GUM_SCALAR >
  void CliqueProperties< GUM_SCALAR >::addPotential(
     const Potential< GUM_SCALAR >& cpt, bool removable) {
    potentialBucket__.insert(&cpt, new Instantiation(cpt));

    if (removable) removablePotentialList__.insert(&cpt);

    const auto& vars = cpt.variablesSequence();
    for (auto iter = vars.beginSafe(); iter != vars.end(); ++iter) {
      if (removable && !allVarsInst__.contains(**iter)) try {
          removableVarList__.insert(*iter);
        } catch (DuplicateElement&) {
          // Nothing to do then!
        }

      addVariable(**iter);
    }
  }

  // potentialBucket : Returns the potential bucket of this Clique
  template < typename GUM_SCALAR >
  INLINE const HashTable< const Potential< GUM_SCALAR >*, Instantiation* >&
               CliqueProperties< GUM_SCALAR >::potentialBucket() {
    return potentialBucket__;
  }

  // addUtility : adds a utility table to this clique
  // The removable boolean inidcates if this utilityTable can be cleaned off
  // after an
  // inference or not
  template < typename GUM_SCALAR >
  void
     CliqueProperties< GUM_SCALAR >::addUtility(const Potential< GUM_SCALAR >& ut,
                                                bool removable) {
    utilityBucket__.insert(&ut, new Instantiation(ut));

    if (removable) removableUtilityList__.insert(&ut);

    for (auto var: ut.variablesSequence()) {
      if (removable && !allVarsInst__.contains(*var)) try {
          removableVarList__.insert(var);
        } catch (DuplicateElement&) {
          // Nothing to do then!
        }

      addVariable(*var);
    }
  }

  //  utilityBucket : Returns the utiluty table bucket of this Clique
  template < typename GUM_SCALAR >
  INLINE const HashTable< const Potential< GUM_SCALAR >*, Instantiation* >&
               CliqueProperties< GUM_SCALAR >::utilityBucket() {
    return utilityBucket__;
  }

  // cleanFromInference : performs a cleaning of the clique after an inference
  // This is done by removing the "message" given by a child clique to its
  // parents,
  // meaning the potentials and utility tables obtained by
  // variable elemination in the clique.
  template < typename GUM_SCALAR >
  void CliqueProperties< GUM_SCALAR >::cleanFromInference() {
    // Removed added variables during inference (normally, the
    // removableVarList__ is
    // empty, but we never know )
    for (const auto removableVar: removableVarList__)
      allVarsInst__.erase(*removableVar);

    // Removed added potentials during inference
    for (const auto removablePot: removablePotentialList__) {
      delete potentialBucket__[removablePot];
      potentialBucket__.erase(removablePot);
      delete removablePot;
    }

    // Removed added utility tables during inference
    for (const auto removabledUt: removableUtilityList__) {
      delete utilityBucket__[removabledUt];
      utilityBucket__.erase(removabledUt);
      delete removabledUt;
    }

    removableVarList__.clear();
    removablePotentialList__.clear();
    removableUtilityList__.clear();
  }

  // makeEliminationOrder : creates an elimination order in the clique
  // compatible
  // with global elimination order
  // The global elimination order is given by elim.
  template < typename GUM_SCALAR >
  void CliqueProperties< GUM_SCALAR >::makeEliminationOrder(
     const std::vector< NodeId >&          elim,
     const InfluenceDiagram< GUM_SCALAR >& infDiag) {
    for (size_t i = 0; i < elim.size(); i++) {
      if (allVarsInst__.contains(infDiag.variable(elim[i])))
        eliminationOrder__.insert(elim[i]);
    }
  }

  // cliqueEliminationOrder : returns the elimination sequence for this clique
  template < typename GUM_SCALAR >
  INLINE const Sequence< NodeId >&
               CliqueProperties< GUM_SCALAR >::cliqueEliminationOrder() {
    return eliminationOrder__;
  }

  // addEvidence : add evidence over one variable present in the clique
  // @throw OperationNotAllowed if evidence has more than one variable.
  // @throw NotFound Raised if the evidence is on a variable not present in this
  // clique.
  // @throw DuplicateElement, if another evidence over this variable exists for
  // this
  // clique
  template < typename GUM_SCALAR >
  void CliqueProperties< GUM_SCALAR >::addEvidence(
     const Potential< GUM_SCALAR >& evidence) {
    // To avoid interference
    cleanFromInference();

    // First, we check if evidence is over only one variable.
    if (evidence.variablesSequence().size() != 1) {
      GUM_ERROR(OperationNotAllowed,
                "expected evidence on 1 variable, found on "
                   << evidence.variablesSequence().size());
    }

    // Then we assure us that that variable is in the clique
    if (!allVarsInst__.contains(evidence.variablesSequence().atPos(0))) {
      GUM_ERROR(NotFound,
                evidence.variablesSequence().atPos(0)->name()
                   << " not found in clique ");
    }

    evidences__.insert(evidence.variablesSequence().atPos(0), &evidence);

    potentialBucket__.insert(&evidence, new Instantiation(evidence));
  }

  // evidences : Returns the mapping of evidences on variables in this clique
  template < typename GUM_SCALAR >
  INLINE const
     HashTable< const DiscreteVariable*, const Potential< GUM_SCALAR >* >&
     CliqueProperties< GUM_SCALAR >::evidences() const {
    return evidences__;
  }

  // removeEvidence : Removes the evidence over v
  template < typename GUM_SCALAR >
  INLINE void
     CliqueProperties< GUM_SCALAR >::removeEvidence(const DiscreteVariable& v) {
    const Potential< GUM_SCALAR >* evi = evidences__[&v];

    delete potentialBucket__[evi];
    potentialBucket__.erase(evi);
    evidences__.erase(&v);
  }

  // removeAllEvidence : Removes all the evidences
  template < typename GUM_SCALAR >
  INLINE void CliqueProperties< GUM_SCALAR >::removeAllEvidence() {
    for (const auto& elt: evidences__)
      removeEvidence(*elt.first);

    evidences__.clear();
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
