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
 * @brief Implementation of the Shafer-Shenoy alogorithm for inference in
 * gum::IBayesNet.
 */

#ifndef GUM_SHAFER_SHENOY_INFERENCE_H
#define GUM_SHAFER_SHENOY_INFERENCE_H

#include <vector>
#include <utility>
#include <iostream>
#include <string>

#include <agrum/config.h>

#include <agrum/graphs/triangulations/defaultTriangulation.h>

#include <agrum/multidim/multiDimBucket.h>
#include <agrum/multidim/multiDimSparse.h>

#include <agrum/BN/inference/BayesNetInference.h>

namespace gum {

  template <typename GUM_SCALAR> class CliqueProp;

  /**
   * @class ShaferShenoyInference ShaferShenoyInference.h
   *<agrum/BN/inference/ShaferShenoyInference.h>
   * @brief This class implements the Shafer-Shenoy alogorithm for inference
   * in Bayesian Networks.
   * @ingroup bn_group
   *
   * The class used for the triangulation is DefaultTriangulation.
   * @todo Make it generic to Triangulation.
   * @todo Gestion of multiple connected components
   * @todo generalize joint law of two variables into n variables
   *
   * @warning : this class doesn't seem to infer bayesian network with multiple
   *           connected components
   */
  template <typename GUM_SCALAR>
  class ShaferShenoyInference : public BayesNetInference<GUM_SCALAR> {

    public:
    // ====================================================================
    /// @name Constructor & destructor
    // ====================================================================
    /// @{

    /**
     * Default constructor.
     * @param bayesNet The Bayesian Network used for the inference.
     */
    ShaferShenoyInference(const IBayesNet<GUM_SCALAR> &bayesNet);

    /**
     * Destructor.
     */
    virtual ~ShaferShenoyInference();

    /// @}
    // ====================================================================
    /// @name Inference and evidence management
    // ====================================================================
    /// @{

    /// @see gum::BayesNetInference::makeInference().
    virtual void makeInference();

    /// @see gum::BayesNetInference::insertEvidence().
    virtual void insertEvidence(const List<const Potential<GUM_SCALAR> *> &pot_list);

    /// @see gum::BayesNetInference::eraseEvidence().
    virtual void eraseEvidence(const Potential<GUM_SCALAR> *);

    /// @see gum::BayesNetInference::eraseAllEvidence().
    virtual void eraseAllEvidence();

    /// @}
    // ====================================================================
    /// @name Getters & setters
    // ====================================================================
    /// @{

    /// Returns the Triangulation used by this class.
    Triangulation &triangulation();

    protected:
    /// @see gum::BayesNetInference::_fillPosterior().
    virtual void _fillPosterior(NodeId id, Potential<GUM_SCALAR> &posterior);

    private:
    // ====================================================================
    /// @name Private members
    // ====================================================================
    /// @{

    /// The triangulation algorithm.
    Triangulation *__triangulation;

    /// The set of dummies sparse matrix created.
    Set<Potential<GUM_SCALAR> *> __dummies;

    /// @}
    // ====================================================================
    /// @name Useful computation members
    // ====================================================================
    /// @{

    /// Mapping of the nodes with the clique used to put their CPT
    NodeProperty<NodeId> __node2CliqueMap;

    NodeProperty<CliqueProp<GUM_SCALAR> *> __clique_prop;

    /// Mapping of an arc and the message which transited from pair.first to
    /// pair.second
    ArcProperty<MultiDimBucket<GUM_SCALAR> *> __messagesMap;

    /// @}
    // ====================================================================
    /// @name Private getters & setters
    // ====================================================================
    /// @{

    /// @return Returns the list of neighbours of a given clique
    const NodeSet &__getNeighbours(NodeId cliqueId);

    /// @return Returns a separator given two adjacent cliques
    const NodeSet &__getSeparator(NodeId clique_1, NodeId clique_2);

    /// @return Returns the clique in which the node's cpt must be stored
    NodeId __getClique(const std::vector<NodeId> &eliminationOrder, NodeId id);

    /// @}
    // ====================================================================
    /// @name Inference sub-methods
    // ====================================================================
    /// @{

    /// Builds the cliques tables
    /// Uses __getCliquesTable to initialize the cliques table, and multiply
    /// the tables with the adequate CPT.
    void __buildCliquesTables();

    /// Add the evidences to the clique's table
    void __addEvidences();

    /// Starting collect w.r.t. a clique
    void __collectFromClique(NodeId source);

    /// Collecting phase of the inference
    bool __collect(NodeId source, NodeId current);

    /// Starting diffusion w.r.t. a clique
    void __diffuseFromClique(NodeId source);

    /// Diffusing phase of the inference
    void __diffuse(NodeId source, NodeId current, bool recompute);

    /// Create and saves the message from key.first to key.second in the
    /// __messagesMap.
    void __sendMessage(NodeId tail, NodeId head);

    /// Return true if the message from source to dest exists
    bool __messageExists(NodeId source, NodeId dest);

    /// Removes all diffused message sent by cliqueId
    void __removeDiffusedMessages(NodeId cliqueId);

    /// Returns a pointer over a "dummy" potential, which is a CPT filled with
    /// one MultiDimSparse filled with 1. This is used by empty cliques.
    /// @param cliqueId The NodeId of the cliqueId for which we build a dummy
    /// potential.
    /// @return A pointer over the dummy bucket.
    Potential<GUM_SCALAR> *__makeDummyPotential(NodeId cliqueId);

    /// @}
  };

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  /// @class CliqueProp
  /// Private class to keep clique's properties.
  /// It used MultiDimBucket to compute the cliques potentials.
  template <typename GUM_SCALAR>

  class CliqueProp {
    public:
    /// Default constructor.
    /// @param id the id of this clique on which this properties holds.
    CliqueProp(NodeId id);

    /// Destructor.
    ~CliqueProp();

    /// Returns the name of this clique.
    const std::string &name() const;

    /// Add a variable to this clique
    /// @param v The variable added.
    void addVariable(const DiscreteVariable &v);

    /// Add a potential to this clique
    /// @param cpt v's cpt.
    void addPotential(const Potential<GUM_SCALAR> &cpt);

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
    MultiDimBucket<GUM_SCALAR> &bucket();

    /// @return Returns the bucket of this Clique
    const MultiDimBucket<GUM_SCALAR> &bucket() const;

    /// Flag to know if this clique has been collected.
    bool isCollected;

    private:
    /// Evidences on the variables in this clique
    HashTable<const DiscreteVariable *, const Potential<GUM_SCALAR> *> __evidences;

    /// The bucket of this clique with evidences
    MultiDimBucket<GUM_SCALAR> *__potential;

    /// The bucket of the variables without the evidences
    MultiDimBucket<GUM_SCALAR> *__varsPotential;

    /// The name of the clique.
    std::string __name;
  };
#endif // DOXYGEN_SHOULD_SKIP_THIS

  extern template class ShaferShenoyInference<float>;
  extern template class ShaferShenoyInference<double>;
} /* namespace gum */

#include <agrum/BN/inference/ShaferShenoyInference.tcc>

#endif /* GUM_SHAFER_SHENOY_INFERENCE_H */
