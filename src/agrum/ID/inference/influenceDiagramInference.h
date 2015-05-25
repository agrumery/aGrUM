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
 * @brief Implementation of an influence diagram inference algorithm based upon
 * Shaffer-Shenoy's one for bayes net inferences
 */

#ifndef GUM_INFLUENCE_DIAGRAM_INFERENCE_H
#define GUM_INFLUENCE_DIAGRAM_INFERENCE_H

#include <vector>
#include <utility>
#include <iostream>
#include <string>

#include <agrum/config.h>

#include <agrum/core/list.h>

#include <agrum/graphs/triangulations/partialOrderedTriangulation.h>

#include <agrum/multidim/multiDimBucket.h>
#include <agrum/multidim/multiDimSparse.h>

#include <agrum/ID/inference/IInfluenceDiagramInference.h>

namespace gum {

  template <typename GUM_SCALAR> class CliqueProperties;

  /**
   * @class InfluenceDiagramInference influenceDiagramInference.h
   *<agrum/ID/inference/influenceDiagramInference.h>
   * @brief This class implements an algorithm for inference
   * in influence diagrams based upon  Shaffer-Shenoy's one for bayes net inferences
   * @ingroup id_group
   *
   * The class used for the triangulation is partialOrderedTriangulation.
   */
  template <typename GUM_SCALAR>
  class InfluenceDiagramInference : public IInfluenceDiagramInference<GUM_SCALAR> {

    public:
    // ====================================================================
    /// @name Constructor & destructor
    // ====================================================================
    /// @{

    /**
     * Default constructor.
     * @param infDiag the influence diagram we want to perform inference upon
     */
    InfluenceDiagramInference(const InfluenceDiagram<GUM_SCALAR> &infDiag);

    /**
     * Destructor.
     */
    virtual ~InfluenceDiagramInference();

    /// @}
    // ====================================================================
    /// @name Inference
    // ====================================================================
    /// @{

    /// @see gum::IInfluenceDiagramInference::makeInference().
    virtual void makeInference();

    /// @see gum::IInfluenceDiagramInference::getMEU().
    GUM_SCALAR getMEU();

    /// @see gum::IInfluenceDiagramInference::getBestDecisionChoice( NodeId
    /// decisionId )
    Idx getBestDecisionChoice(NodeId decisionId);

    /// displays the result of an inference
    std::string displayResult(void);

    /// @}
    // ====================================================================
    /// @name Evidence management
    // ====================================================================
    /// @{

    /// @see gum::IInfluenceDiagramInference::insertEvidence().
    virtual void
    insertEvidence(const List<const Potential<GUM_SCALAR> *> &evidenceList);

    /// @see gum::IInfluenceDiagramInference::eraseEvidence().
    virtual void eraseEvidence(const Potential<GUM_SCALAR> *evidence);

    /// @see gum::IInfluenceDiagramInference::eraseAllEvidence().
    virtual void eraseAllEvidence();

    /// @}
    // ====================================================================
    /// @name Getters & setters
    // ====================================================================
    /// @{

    /// Returns the Triangulation used by this class.
    Triangulation &getTriangulation();

    /// Displays on terminal the result of strong junction tree computation
    /// for test purpose only
    void displayStrongJunctionTree(std::ostream &stream = std::cout);

    private:
    // ====================================================================
    /// @name Private members
    // ====================================================================
    /// @{

    /// The triangulation algorithm.
    Triangulation *__triangulation;

    /// The set of dummies sparse potential matrix created.
    Set<Potential<GUM_SCALAR> *> __potentialDummies;

    /// The set of dummies sparse utilities matrix created.
    Set<UtilityTable<GUM_SCALAR> *> __utilityDummies;

    /// The resulting potential from inference
    Potential<GUM_SCALAR> *__inferencePotential;

    /// The resulting utility from inference
    UtilityTable<GUM_SCALAR> *__inferenceUtility;

    /// @}
    // ====================================================================
    /// @name Useful computation members
    // ====================================================================
    /// @{

    /// Mapping of the nodes with the clique used to put their CPT
    NodeProperty<NodeId> __nodeToCliqueMap;

    NodeProperty<CliqueProperties<GUM_SCALAR> *> __cliquePropertiesMap;

    HashTable<Size, NodeId> __cliqueEliminationMap;

    HashTable<NodeId, Idx> __utakenDecisionMap;

    bool __inferenceMade;

    /// @}
    // ====================================================================
    /// @name Private getters & setters
    // ====================================================================
    /// @{

    /// @return Returns a separator given two adjacent cliques
    const NodeSet &__getSeparator(NodeId clique_1, NodeId clique_2);

    /// @return Returns the clique in which the node's potentials must be stored
    NodeId __getClique(const std::vector<NodeId> &eliminationOrder, NodeId id);

    /// @}
    // ====================================================================
    /// @name Inference sub-methods
    // ====================================================================
    /// @{

    /// Builds the cliques tables
    /// Uses __getCliquesTable to initialize the cliques table, and multiply
    /// the tables with the adequate CPT.
    void __makeCliquePropertiesMap();

    /// Makes a strong junction tree that make easier elimination
    void __makeStrongJunctionTree();

    /// Cleans Up remaining stuff due to inference
    void __cleanUp();

    /// collect child clique for inferences
    void __collectChild(NodeId parent, NodeId child);

    /// Performs the operation of absorption of a clique by another
    void __absorbClique(NodeId absorbedCliqueId, NodeId absorbingCliqueId);

    /// Reduces a clique down to her separator from another clique elements
    void __reduceClique(CliqueProperties<GUM_SCALAR> *absorbedClique,
                        NodeSet &separator,
                        Potential<GUM_SCALAR> *&potentialMarginal,
                        UtilityTable<GUM_SCALAR> *&utilityMarginal);

    /// Returns a pointer over a "dummy" potential, which is a CPT filled with
    /// one MultiDimSparse filled with 1. This is used by empty cliques.
    /// @param cliqueId The NodeId of the cliqueId for which we build a dummy
    /// potential.
    /// @return A pointer over the dummy bucket.
    Potential<GUM_SCALAR> *__makeDummyPotential(NodeId cliqueId);

    /// Returns a pointer over a "dummy" utility, which is a utility table filled
    /// with
    /// one MultiDimSparse filled with 0. This is used by empty cliques.
    /// @param cliqueId The NodeId of the cliqueId for which we build a dummy
    /// utility.
    /// @return A pointer over the dummy bucket.
    UtilityTable<GUM_SCALAR> *__makeDummyUtility(NodeId cliqueId);

    /// Returns true if observed  node is eliminated after current node
    bool __IsEliminatedAfter(NodeId observedNode, NodeId currentNode);

    /// @}
  };

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  /// @class CliqueProperties
  /// Private class to keep clique's properties.
  /// It used MultiDimBucket to compute the cliques potentials.
  template <typename GUM_SCALAR> class CliqueProperties {
    public:
    /// Default constructor.
    /// @param id the id of this clique on which this properties holds.
    CliqueProperties();

    /// Destructor.
    ~CliqueProperties();

    /// Add a variable to this clique
    /// @param v The variable added.
    void addVariable(const DiscreteVariable &v);

    /// Determines in which order nodes will be eliminated from clique
    /// @param elim the global elimination order.
    void makeEliminationOrder(const std::vector<NodeId> &elim,
                              const InfluenceDiagram<GUM_SCALAR> &infDiag);

    /// Add a potential to this clique
    /// @param cpt v's cpt.
    /// @param removable for cleaning purpose after inference, we have to keep track
    /// of adding potential during inference
    void addPotential(const Potential<GUM_SCALAR> &cpt, bool removable = false);

    /// Add a potential to this clique
    /// @param cpt v's cpt.
    /// @param removable for cleaning purpose after inference, we have to keep track
    /// of adding potential during inference
    void addUtility(const UtilityTable<GUM_SCALAR> &ut, bool removable = false);

    /// Removes all potential and utility table added during an inference
    void cleanFromInference();

    /// @brief Add an evidence on a variable in this clique.
    /// This method will remove any previous evidence on the given variable.
    /// This method will raise an OperationNotAllowed if evidence contains not
    /// exactly one variable.
    /// @throw NotFound Raised if the evidence is on a variable not present
    ///        in this clique.
    /// @throw OperationNotAllowed If the evidence isn't valid.
    void addEvidence(const Potential<GUM_SCALAR> &evidence);

    /// Removes all the evidences containing v
    void removeEvidence(const DiscreteVariable &v);

    /// Remove all the evidences
    void removeAllEvidence();

    /// @return Returns the mapping of evidences on the variables in this clique.
    const HashTable<const DiscreteVariable *, const Potential<GUM_SCALAR> *> &
    evidences() const;

    /// @return Returns the bucket of this Clique
    const HashTable<const Potential<GUM_SCALAR> *, Instantiation *> &
    potentialBucket();

    /// @return Returns the bucket of this Clique
    const HashTable<const UtilityTable<GUM_SCALAR> *, Instantiation *> &
    utilityBucket();

    /// @return returns the elimination sequence for this clique
    const Sequence<NodeId> &cliqueEliminationOrder();

    /// @return returns List containing all variables contained in this clique
    const Sequence<const DiscreteVariable *> &cliqueVariables();

    /// @return returns instanciation on variable within this clique
    Instantiation &cliqueInstantiation();

    private:
    /// Evidences on the variables in this clique
    HashTable<const DiscreteVariable *, const Potential<GUM_SCALAR> *> __evidences;

    /// The potential bucket of this clique with evidences
    HashTable<const Potential<GUM_SCALAR> *, Instantiation *> __potentialBucket;

    /// The utility bucket of this clique
    HashTable<const UtilityTable<GUM_SCALAR> *, Instantiation *> __utilityBucket;

    /// The sequence of elimination of node in the clique
    Sequence<NodeId> __eliminationOrder;

    /// Instantiation on variable in that clique
    Instantiation __allVarsInst;

    /// The list of var that have been had during an inference
    List<const DiscreteVariable *> __removableVarList;

    /// The list of potentials that have been had during an inference
    List<const Potential<GUM_SCALAR> *> __removablePotentialList;

    /// The list of utilities that have been had during an inference
    List<const UtilityTable<GUM_SCALAR> *> __removableUtilityList;
  };

#endif // DOXYGEN_SHOULD_SKIP_THIS

} /* namespace gum */

#include <agrum/ID/inference/influenceDiagramInference.tcc>

#endif /* GUM_INFLUENCE_DIAGRAM_INFERENCE_H */
