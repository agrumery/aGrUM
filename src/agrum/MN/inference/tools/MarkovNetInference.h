
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES
 * (@AMU) info_at_agrum_dot_org
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
 * @brief This file contains abstract class definitions for Markov Networks
 *        inference classes.
 *
 * @author Pierre-Henri WUILLEMIN (@LIP6) and Christophe GONZALES (@AMU)
 */

#ifndef GUM_MARKOV_NET_INFERENCE_H
#define GUM_MARKOV_NET_INFERENCE_H


#include <agrum/MN/IMarkovNet.h>
#include <agrum/agrum.h>

namespace gum {


  /**
   * @class MarkovNetInference inference.h
   * <agrum/MN/inference/MarkovNetInference.h>
   * @brief A generic class for Markov net inference: handles evidence and the
   * current state of the (incremental) inference
   * @ingroup bn_group
   *
   * The goal of the MarkovNetInference class is twofold:
   * i) handling the common resources of MN inference (mn, soft/hard evidence);
   * ii) propose a general high-level scheme for all the inference methods.
   *
   * A specialized inference just has to specify how to prepare inference, how
   * to make inference and how to get the posteriors for nodes and set of nodes.
   * The scheme for every inference derived from MarkovNetInference will be
   * the same:
   *
   * 1- ie=SpecificInference(mn);              // state <- OutdatedMNStructure
   * 2- set targets and evidence in ie
   * 3- ie.prepareInference();                 // state <- Ready4Inference
   * 4.a- change values of evidence in ie      // state <- OutdatedMNPotentials
   * 4.b- change some hard evidence or targets // state <- OutdatedMNStructure
   * 5- ie.makeInference();                    // state <- Done
   * 6- get posteriors
   * 7- goto 2 or 4
   *
   * MarkovNetInference can be in one of 4 different states:
   * - OutdatedMNStructure: in this state, the inference is fully unprepared
   *   to be applied because some events changed the "logical" structure of the
   *   MN: for instance a node received a hard evidence, which implies that
   *   its outgoing arcs can be removed from the MN, hence involving a
   *   structural change in the MN. As a consequence, the (incremental)
   *   inference
   *   (probably) needs a significant amount of preparation to be ready for the
   *   next inference. In a Lazy propagation, for instance, this step amounts to
   *   compute a new join tree, hence a new structure in which inference
   *   will be applied. Note that classes that inherit from MarkovNetInference
   *   may be smarter than MarkovNetInference and may, in some situations,
   *   find out that their
   *   data structures are still ok for inference and, therefore, only resort to
   *   perform the actions related to the OutdatedMNPotentials state. As an
   *   example, consider a LazyPropagation inference in Markov net A->B->C->D->E
   *   in which C has received hard evidence e_C and E is the only target. In
   *   this case, A and B are not needed for inference, the only potentials that
   *   matter are P(D|e_C) and P(E|D). So the smallest join tree needed for
   *   inference contains only one clique DE. Now, adding new evidence e_A on A
   *   has no impact on E given hard evidence e_C. In this case, LazyPropagation
   *   can be smart and not update its join tree.
   * - OutdatedMNPotentials: in this state, the structure of the MN remains
   *   unchanged, only some potentials stored in it have changed. Therefore,
   *   the inference probably just needs to invalidate some already computed
   *   potentials to be ready. Only a light amount of preparation is needed to
   *   be able to perform inference.
   * - Ready4Inference: in this state, all the data structures are ready for
   *   inference. There just remains to perform the inference computations.
   * - Done: the heavy computations of inference have been done. There might
   *   still remain a few light computations to perform to get the posterior
   *   potentials we need. Typically, in Lazy Propagation, all the messages in
   *   the join tree have been computed but, to get the potentials, we still
   *   need to perform the combinations of the potentials in the cliques with
   *   the messages sent to the cliques. In some inference algorithms, this
   *   step may even be empty.
   */

  template < typename GUM_SCALAR >
  class MarkovNetInference {
    public:
    /**
     * current state of the inference
     *
     * MarkovNetInference can be in one of 4 different states:
     * - OutdatedMNStructure: in this state, the inference is fully unprepared
     *   to be applied because some events changed the "logical" structure of
     *   the MN: for instance a node received a hard evidence, which implies
     *   that its outgoing arcs can be removed from the MN, hence involving a
     *   structural change in the MN. As a consequence, the (incremental)
     *   inference (probably) needs a significant amount of preparation to be
     *   ready for the next inference. In a Lazy propagation, for instance,
     *   this step amounts to compute a new join tree, hence a new structure
     *   in which inference will be applied. Note that classes that inherit
     *   from MarkovNetInference may be smarter than MarkovNetInference and may,
     *   in some situations, find out that their data structures are still ok for
     *   inference and, therefore, only resort to perform the actions related
     *   to the OutdatedMNPotentials state. As an example, consider a
     *   LazyPropagation inference in Markov net A->B->C->D->E
     *   in which C has received hard evidence e_C and E is the only target. In
     *   this case, A and B are not needed for inference, the only potentials
     *   that matter are P(D|e_C) and P(E|D). So the smallest join tree needed
     *   for inference contains only one clique DE. Now, adding new evidence
     *   e_A on A has no impact on E given hard evidence e_C. In this case,
     *   LazyPropagation can be smart and not update its join tree.
     * - OutdatedMNPotentials: in this state, the structure of the MN remains
     *   unchanged, only some potentials stored in it have changed. Therefore,
     *   the inference probably just needs to invalidate some already computed
     *   potentials to be ready. Only a light amount of preparation is needed to
     *   be able to perform inference.
     * - Ready4Inference: in this state, all the data structures are ready for
     *   inference. There just remains to perform the inference computations.
     * - Done: the heavy computations of inference have been done. There might
     *   still remain a few light computations to perform to get the posterior
     *   potentials we need. Typically, in Lazy Propagation, all the messages in
     *   the join tree have been computed but, to get the potentials, we still
     *   need to perform the combinations of the potentials in the cliques with
     *   the messages sent to the cliques. In some inference algorithms, this
     *   step may even be empty.
     */
    enum class StateOfMNInference {
      OutdatedMNStructure,
      OutdatedMNPotentials,
      ReadyForInference,
      Done
    };


    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /** @warning note that, by aGrUM's rule, the MN is not copied but only
     * referenced by the inference algorithm. */
    explicit MarkovNetInference(const IMarkovNet< GUM_SCALAR >* mn);

    /// default constructor with a null MN (useful for virtual inheritance)
    /** @warning MarkovNetInference is virtually inherited by
     * MarginalTargetedInference. As a result, the lowest descendant of
     * MarkovNetInference will create the latter. To avoid requiring developpers
     * to add in the constructors of their inference algorithms a call to
     * MarkovNetInference( mn ), we added constructor MarkovNetInference(),
     * which will be called automatically by the lowest descendant.
     * Then, MarginalTargetedInference and JointTargetedInference will take care
     * of setting the appropriate mn into MarkovNetInference. */
    MarkovNetInference();

    /// destructor
    virtual ~MarkovNetInference();

    /// @}


    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// assigns a new MN to the inference engine
    /** Assigns a new MN to the MarkovNetInference engine and sends messages to the
     * descendants of MarkovNetInference to inform them that the MN has changed.
     * @warning By default, all the nodes of the Markov net are targets.
     * @warning note that, by aGrUM's rule, the mn is not copied into the
     * inference engine but only referenced. */
    virtual void setMN(const IMarkovNet< GUM_SCALAR >* mn);

    /// Returns a constant reference over the IMarkovNet referenced by this class
    /** @throws UndefinedElement is raised if no Markov net has been assigned to
     * the inference. */
    virtual const IMarkovNet< GUM_SCALAR >& MN() const final;

    /// get the domain sizes of the random variables of the MN
    virtual const NodeProperty< Size >& domainSizes() const final;

    /// returns whether the inference object is in a ready state
    virtual bool isInferenceReady() const noexcept final;
    /// returns whether the inference object is in a OutdatedMNStructure state
    virtual bool isInferenceOutdatedMNStructure() const noexcept final;
    /// returns whether the inference object is in a OutdatedMNPotential state
    virtual bool isInferenceOutdatedMNPotentials() const noexcept final;
    /// returns whether the inference object is in a InferenceDone state
    virtual bool isInferenceDone() const noexcept final;

    /// returns whether the inference object is in a done state
    /** The inference object is in a done state when the posteriors can be
     * retrieved without performing a new inference, i.e., all the heavy
     * computations have already been performed. Typically, in a junction tree
     * algorithm, this corresponds to a situation in which all the messages
     * needed in the JT have been computed and sent. */
    virtual bool isDone() const noexcept final;

    /// prepare the internal inference structures for the next inference
    virtual void prepareInference() final;

    /// perform the heavy computations needed to compute the targets' posteriors
    /** In a Junction tree propagation scheme, for instance, the heavy
     * computations are those of the messages sent in the JT. This is precisely
     * what makeInference should compute. Later, the computations of the
     * posteriors can be done "lightly" by multiplying and projecting those
     * messages. */
    virtual void makeInference() final;

    /// clears all the data structures allocated for the last inference
    virtual void clear();

    /// returns the state of the inference engine
    virtual StateOfMNInference state() const noexcept final;
    /// @}


    // ############################################################################
    /// @name Evidence
    // ############################################################################
    /// @{

    /// adds a new hard evidence on node id
    /**
     * @throw UndefinedElement if id does not belong to the Markov Network
     * @throw InvalidArgument if val is not a value for id
     * @throw InvalidArgument if id already has an evidence
     */
    virtual void addEvidence(NodeId id, const Idx val) final;

    /// adds a new hard evidence on node named nodeName
    /**
     * @throw UndefinedElement if nodeName does not belong to the Markov Network
     * @throw InvalidArgument if val is not a value for id
     * @throw InvalidArgument if nodeName already has an evidence
     */
    virtual void addEvidence(const std::string& nodeName, const Idx val) final;

    /// adds a new hard evidence on node id
    /**
     * @throw UndefinedElement if id does not belong to the Markov Network
     * @throw InvalidArgument if val is not a value for id
     * @throw InvalidArgument if id already has an evidence
     */
    virtual void addEvidence(NodeId id, const std::string& label) final;

    /// adds a new hard evidence on node named nodeName
    /**
     * @throw UndefinedElement if nodeName does not belong to the Markov Network
     * @throw InvalidArgument if val is not a value for id
     * @throw InvalidArgument if nodeName already has an evidence
     */
    virtual void addEvidence(const std::string& nodeName,
                             const std::string& label) final;

    /// adds a new evidence on node id (might be soft or hard)
    /**
     * @throw UndefinedElement if id does not belong to the Markov Network
     * @throw InvalidArgument if id already has an evidence
     * @throw FatalError if vals=[0,0,...,0]
     * @throw InvalidArgument if the size of vals is different from the domain
     *        size of node id
     */
    virtual void addEvidence(NodeId                           id,
                             const std::vector< GUM_SCALAR >& vals) final;

    /// adds a new evidence on node named nodeName (might be soft or hard)
    /**
     * @throw UndefinedElement if id does not belong to the Markov Network
     * @throw InvalidArgument if nodeName already has an evidence
     * @throw FatalError if vals=[0,0,...,0]
     * @throw InvalidArgument if the size of vals is different from the domain
     *        size of node nodeName
     */
    virtual void addEvidence(const std::string&               nodeName,
                             const std::vector< GUM_SCALAR >& vals) final;

    /// adds a new evidence on node id (might be soft or hard)
    /**
     * @throw UndefinedElement if the potential is defined over several nodes
     * @throw UndefinedElement if the node on which the potential is defined
     * does not belong to the Markov Network
     * @throw InvalidArgument if the node of the potential already has an
     * evidence
     * @throw FatalError if pot=[0,0,...,0]
     */
    virtual void addEvidence(const Potential< GUM_SCALAR >& pot) final;

    /// adds a new evidence on node id (might be soft or hard)
    /**
     * @throw UndefinedElement if the potential is defined over several nodes
     * @throw UndefinedElement if the node on which the potential is defined
     * does not belong to the Markov Network
     * @throw InvalidArgument if the node of the potential already has an
     * evidence
     * @throw FatalError if pot=[0,0,...,0]
     */
    virtual void addEvidence(Potential< GUM_SCALAR >&& pot) final;

    /// adds a new set of evidence
    /**
     * @throw UndefinedElement if some potential is defined over several nodes
     * @throw UndefinedElement if the node on which some potential is defined
     * does not belong to the Markov Network
     * @throw InvalidArgument if the node of some potential already has an
     * evidence
     * @throw FatalError if pot=[0,0,...,0]
     */
    virtual void
       addSetOfEvidence(const Set< const Potential< GUM_SCALAR >* >& potset) final;

    /// adds a new list of evidence
    /**
     * @throw UndefinedElement if some potential is defined over several nodes
     * @throw UndefinedElement if the node on which some potential is defined
     * does not belong to the Markov Network
     * @throw InvalidArgument if the node of some potential already has an
     * evidence
     * @throw FatalError if pot=[0,0,...,0]
     */
    virtual void addListOfEvidence(
       const List< const Potential< GUM_SCALAR >* >& potlist) final;

    /// change the value of an already existing hard evidence
    /**
     * @throw UndefinedElement if id does not belong to the Markov Network
     * @throw InvalidArgument if val is not a value for id
     * @throw InvalidArgument if id does not already have an evidence
     */
    virtual void chgEvidence(NodeId id, const Idx val) final;

    /// change the value of an already existing hard evidence
    /**
     * @throw UndefinedElement if nodeName does not belong to the Markov Network
     * @throw InvalidArgument if val is not a value for id
     * @throw InvalidArgument if id does not already have an evidence
     */
    virtual void chgEvidence(const std::string& nodeName, const Idx val) final;

    /// change the value of an already existing hard evidence
    /**
     * @throw UndefinedElement if id does not belong to the Markov Network
     * @throw InvalidArgument if val is not a value for id
     * @throw InvalidArgument if id does not already have an evidence
     */
    virtual void chgEvidence(NodeId id, const std::string& label) final;

    /// change the value of an already existing hard evidence
    /**
     * @throw UndefinedElement if nodeName does not belong to the Markov Network
     * @throw InvalidArgument if val is not a value for id
     * @throw InvalidArgument if id does not already have an evidence
     */
    virtual void chgEvidence(const std::string& nodeName,
                             const std::string& label) final;

    /// change the value of an already existing evidence (might be soft or hard)
    /**
     * @throw UndefinedElement if id does not belong to the Markov Network
     * @throw InvalidArgument if the node does not already have an evidence
     * @throw FatalError if vals=[0,0,...,0]
     * @throw InvalidArgument if the size of vals is different from the domain
     *        size of node id
     */
    virtual void chgEvidence(NodeId                           id,
                             const std::vector< GUM_SCALAR >& vals) final;

    /// change the value of an already existing evidence (might be soft or hard)
    /**
     * @throw UndefinedElement if nodeName does not belong to the Markov Network
     * @throw InvalidArgument if the node does not already have an evidence
     * @throw FatalError if vals=[0,0,...,0]
     * @throw InvalidArgument if the size of vals is different from the domain
     *        size of node id
     */
    virtual void chgEvidence(const std::string&               nodeName,
                             const std::vector< GUM_SCALAR >& vals) final;

    /// change the value of an already existing evidence (might be soft or hard)
    /**
     * @throw UndefinedElement if the potential is defined over several nodes
     * @throw UndefinedElement if the node on which the potential is defined
     *        does not belong to the Markov Network
     * @throw InvalidArgument if the node of the potential does not already
     *        have an evidence
     * @throw FatalError if pot=[0,0,...,0]
     */
    virtual void chgEvidence(const Potential< GUM_SCALAR >& pot) final;

    /// removes all the evidence entered into the network
    virtual void eraseAllEvidence() final;

    /// removed the evidence, if any, corresponding to node id
    virtual void eraseEvidence(NodeId id) final;

    /// removed the evidence, if any, corresponding to node of name nodeName
    virtual void eraseEvidence(const std::string& nodeName) final;

    /// indicates whether some node(s) have received evidence
    virtual bool hasEvidence() const final;

    /// indicates whether node id has received an evidence
    virtual bool hasEvidence(NodeId id) const final;

    /// indicates whether node id has received a hard evidence
    virtual bool hasHardEvidence(NodeId id) const final;

    /// indicates whether node id has received a soft evidence
    virtual bool hasSoftEvidence(NodeId id) const final;
    /// indicates whether node id has received an evidence

    virtual bool hasEvidence(const std::string& nodeName) const final;

    /// indicates whether node id has received a hard evidence
    virtual bool hasHardEvidence(const std::string& nodeName) const final;

    /// indicates whether node id has received a soft evidence
    virtual bool hasSoftEvidence(const std::string& nodeName) const final;

    /// returns the number of evidence entered into the Markov Network
    virtual Size nbrEvidence() const final;

    /// returns the number of hard evidence entered into the Markov Network
    virtual Size nbrHardEvidence() const final;

    /// returns the number of soft evidence entered into the Markov Network
    virtual Size nbrSoftEvidence() const final;

    /// returns the set of evidence
    const NodeProperty< const Potential< GUM_SCALAR >* >& evidence() const;

    /// returns the set of nodes with soft evidence
    const NodeSet& softEvidenceNodes() const;

    /// returns the set of nodes with hard evidence
    const NodeSet& hardEvidenceNodes() const;

    /// indicate for each node with hard evidence which value it took
    const NodeProperty< Idx >& hardEvidence() const;

    /// @}

    /// returns the probability P(e) of the evidence enterred into the MN
    virtual GUM_SCALAR evidenceProbability() = 0;

    // ############################################################################
    /// @name Probability computations
    // ############################################################################
    /// @{

    /// Compute the joint posterior of a set of nodes.
    /**
     * @returns a const ref to the posterior joint probability of the set of
     * nodes.
     * @param nodes the set of nodes whose posterior joint probability is wanted
     *
     * @warning for efficiency reasons, the potential is stored into the
     * inference engine and is returned by reference. In order to ensure
     * that the potential may still exist even if the Inference object is
     * destroyed, the user has to copy it explicitly.
     *
     * @warning prepareInference and makeInference may be applied if needed.
     *
     * @throw UndefinedElement if nodes is not in the targets
     */
    virtual const Potential< GUM_SCALAR >&
       jointPosterior(const NodeSet& nodes) final;

    /// Computes and returns the posterior of a node.
    /**
     * @returns a const ref to the posterior probability of the node.
     * @param node the node for which we need a posterior probability
     *
     * @warning for efficiency reasons, the potential is stored into the
     * inference engine and is returned by reference. In order to ensure
     * that the potential may still exist even if the Inference object is
     * destroyed, the user has to copy it explicitly.
     *
     * @warning prepareInference and makeInference may be applied if needed by
     * the posterior method.
     *
     * @throw UndefinedElement if node is not in the set of targets
     */
    virtual const Potential< GUM_SCALAR >&
       posterior(const std::string& nodeName) final;
    /// @}


    // ############################################################################
    /// @name Targets
    // ############################################################################
    /// @{

    /// Clear all previously defined targets (marginal and joint targets)
    /**
     * Clear all previously defined targets. As a result, no posterior can be
     * computed (since we can only compute the posteriors of the marginal or
     * joint
     * targets that have been added by the user).
     */
    virtual void eraseAllTargets();

    /// Clear all previously defined joint targets
    virtual void eraseAllJointTargets() final;

    /// Clear all the previously defined marginal targets
    virtual void eraseAllMarginalTargets() final;

    /// Add a set of nodes as a new joint target. As a collateral effect, every
    /// node is added as a marginal target.
    /**
     * @throw UndefinedElement if some node(s) do not belong to the Markov net
     */
    virtual void addJointTarget(const NodeSet& joint_target) final;

    /// removes an existing joint target
    /** @warning If the joint target does not already exist, the method does
     * nothing. In particular, it does not raise any exception. */
    virtual void eraseJointTarget(const NodeSet& joint_target) final;

    /// return true if target is a joint target.
    virtual bool isJointTarget(const NodeSet& vars) const final;

    /// returns the list of joint targets
    virtual const Set< NodeSet >& jointTargets() const noexcept final;

    /// returns the number of joint targets
    virtual Size nbrJointTargets() const noexcept final;
    /// @}

    /**
     * Create a gum::Potential for P(joint targets|evs) (for all instanciation of
     * targets
     * and evs)
     *
     * @warning If some evs are d-separated, they are not included in the Potential
     *
     * @param targets  the NodeSet of the targeted variables
     * @param evs the NodeSet of observed variables
     * @return a Potential
     */
    Potential< GUM_SCALAR > evidenceJointImpact(const NodeSet& targets,
                                                const NodeSet& evs);

    /**
     * Create a gum::Potential for P(joint targets|evs) (for all instanciation of
     * targets
     * and evs)
     *
     * @warning If some evs are d-separated, they are not included in the Potential
     *
     * @param targets  the vector of std::string of the targeted variables
     * @param evs the vector of std::string of observed variables
     * @return a Potential
     */
    Potential< GUM_SCALAR >
       evidenceJointImpact(const std::vector< std::string >& targets,
                           const std::vector< std::string >& evs);

    // ############################################################################
    /// @name Information Theory related functions
    // ############################################################################
    /// @{

    /** Mutual information between X and Y
     * @see http://en.wikipedia.org/wiki/Mutual_information
     *
     * @warning Due to limitation of @ref joint, may not be able to compute
     * this value
     * @throw OperationNotAllowed in these cases
     */
    GUM_SCALAR I(NodeId X, NodeId Y);

    /** Variation of information between X and Y
     * @see http://en.wikipedia.org/wiki/Variation_of_information
     *
     * @warning Due to limitation of @ref joint, may not be able to compute
     * this value
     * @throw OperationNotAllowed in these cases
     */
    GUM_SCALAR VI(NodeId X, NodeId Y);

    /** Mutual information between targets
     * @see https://en.wikipedia.org/wiki/Interaction_information
     * @param targets  the NodeSet of the targeted variables
     */
    GUM_SCALAR jointMutualInformation(const NodeSet& targets);

    /** Mutual information between targets
     * @see https://en.wikipedia.org/wiki/Interaction_information
     * @param targets the vector of std::string of the targeted variables
     */
    GUM_SCALAR jointMutualInformation(const std::vector< std::string >& targets);

    /// @}


    // ############################################################################
    /// @name Probability computations
    // ############################################################################
    /// @{

    /// Computes and returns the posterior of a node.
    /**
     * @returns a const ref to the posterior probability of the node.
     * @param node the node for which we need a posterior probability
     *
     * @warning for efficiency reasons, the potential is stored into the
     * inference engine and is returned by reference. In order to ensure
     * that the potential may still exist even if the Inference object is
     * destroyed, the user has to copy it explicitly.
     *
     * @warning prepareInference and makeInference may be applied if needed by
     * the posterior method.
     *
     * @throw UndefinedElement if node is not in the set of targets
     */
    virtual const Potential< GUM_SCALAR >& posterior(NodeId node);


    /// @}


    // ############################################################################
    /// @name Targets
    // ############################################################################
    /// @{

    /// adds all nodes as targets
    virtual void addAllTargets() final;

    /// Add a marginal target to the list of targets
    /**
     * @throw UndefinedElement if target is not a NodeId in the Markov net
     */
    virtual void addTarget(NodeId target) final;

    /// Add a marginal target to the list of targets
    /**
     * @throw UndefinedElement if target is not a NodeId in the Markov net
     */
    virtual void addTarget(const std::string& nodeName) final;

    /// removes an existing (marginal) target
    /** @warning If the target does not already exist, the method does nothing.
     * In particular, it does not raise any exception. */
    virtual void eraseTarget(NodeId target) final;

    /// removes an existing (marginal) target
    /** @warning If the target does not already exist, the method does nothing.
     * In particular, it does not raise any exception. */
    virtual void eraseTarget(const std::string& nodeName) final;

    /// return true if variable is a (marginal) target
    virtual bool isTarget(NodeId node) const final;

    /// return true if variable is a (marginal) target
    virtual bool isTarget(const std::string& nodeName) const final;

    /// returns the number of marginal targets
    virtual const Size nbrTargets() const noexcept final;

    /// returns the list of marginal targets
    virtual const NodeSet& targets() const noexcept final;

    /// @}

    // ############################################################################
    /// @name Information Theory related functions
    // ############################################################################
    /// @{

    /** Entropy
     * Compute Shanon's entropy of a node given the observation
     * @see http://en.wikipedia.org/wiki/Information_entropy
     */
    virtual GUM_SCALAR H(NodeId X) final;

    /** Entropy
     * Compute Shanon's entropy of a node given the observation
     * @see http://en.wikipedia.org/wiki/Information_entropy
     */
    virtual GUM_SCALAR H(const std::string& nodeName) final;

    ///@}


    /**
     * Create a gum::Potential for P(target|evs) (for all instanciation of target
     * and evs)
     *
     * @warning If some evs are d-separated, they are not included in the Potential
     *
     * @param mn the MarkovNet
     * @param target  the nodeId of the targetted variable
     * @param evs the vector of nodeId of the observed variables
     * @return a Potential
     */
    Potential< GUM_SCALAR > evidenceImpact(NodeId target, const NodeSet& evs);

    /**
     * Create a gum::Potential for P(target|evs) (for all instanciation of target
     * and evs)
     *
     * @warning If some evs are d-separated, they are not included in the Potential
     *
     * @param target  the nodeId of the target variable
     * @param evs the nodeId of the observed variable
     * @return a Potential
     */
    Potential< GUM_SCALAR > evidenceImpact(const std::string& target,
                                           const std::vector< std::string >& evs);

    protected:
    /// fired after a new marginal target is inserted
    /** @param id The target variable's id. */
    virtual void _onMarginalTargetAdded(const NodeId id) = 0;

    /// fired before a marginal target is removed
    /** @param id The target variable's id. */
    virtual void _onMarginalTargetErased(const NodeId id) = 0;

    /// fired after all the nodes of the MN are added as marginal targets
    virtual void _onAllMarginalTargetsAdded() = 0;

    /// fired before a all marginal targets are removed
    virtual void _onAllMarginalTargetsErased() = 0;

    /// asks derived classes for the posterior of a given variable
    /** @param id The variable's id. */
    virtual const Potential< GUM_SCALAR >& _posterior(NodeId id) = 0;

    void _setTargetedMode();
    bool _isTargetedMode() const;

    protected:
    /// fired when the stage is changed
    virtual void _onStateChanged() = 0;

    /// fired after a new evidence is inserted
    virtual void _onEvidenceAdded(const NodeId id, bool isHardEvidence) = 0;

    /// fired before an evidence is removed
    virtual void _onEvidenceErased(const NodeId id, bool isHardEvidence) = 0;

    /// fired before all the evidence are erased
    virtual void _onAllEvidenceErased(bool contains_hard_evidence) = 0;

    /** @brief fired after an evidence is changed, in particular when its status
     * (soft/hard) changes
     *
     * @param nodeId the node of the changed evidence
     * @param hasChangedSoftHard true if the evidence has changed from Soft to
     * Hard or from Hard to Soft
     *
     */
    virtual void _onEvidenceChanged(const NodeId id, bool hasChangedSoftHard) = 0;


    /// prepares inference when the latter is in OutdatedMNStructure state
    /** Note that the values of evidence are not necessarily
     * known and can be changed between _updateOutdatedMNStructure and
     * _makeInference. */
    virtual void _updateOutdatedMNStructure() = 0;

    /// prepares inference when the latter is in OutdatedMNPotentials state
    /** Note that the values of evidence are not necessarily
     * known and can be changed between _updateOutdatedMNPotentials and
     * _makeInference. */
    virtual void _updateOutdatedMNPotentials() = 0;

    /// called when the inference has to be performed effectively
    /** Once the inference is done, _fillPosterior can be called. */
    virtual void _makeInference() = 0;

    /// put the inference into an outdated MN structure state
    /** OutdatedMNStructure: in this state, the inference is fully unprepared
     * to be applied because some events changed the "logical" structure of the
     * MN: for instance a node received a hard evidence, which implies that
     * its outgoing arcs can be removed from the MN, hence involving a
     * structural change in the MN. As a consequence, the (incremental)
     * inference (probably) needs a significant amount of preparation to be
     * ready for the next inference. In a Lazy propagation, for instance, this
     * step amounts to compute a new join tree, hence a new structure in which
     * inference will be applied. Note that classes that inherit from
     * MarkovNetInference may be smarter than MarkovNetInference and may, in some
     * situations, find out that their
     * data structures are still ok for inference and, therefore, only resort to
     * perform the actions related to the OutdatedMNPotentials state. As an
     * example, consider a LazyPropagation inference in Markov net A->B->C->D->E
     * in which C has received hard evidence e_C and E is the only target. In
     * this case, A and B are not needed for inference, the only potentials that
     * matter are P(D|e_C) and P(E|D). So the smallest join tree needed for
     * inference contains only one clique DE. Now, adding new evidence e_A on A
     * has no impact on E given hard evidence e_C. In this case, LazyPropagation
     * can be smart and not update its join tree.*/
    void _setOutdatedMNStructureState();

    /** @brief puts the inference into an OutdatedMNPotentials state if it is
     * not already in an OutdatedMNStructure state
     *
     * OutdatedMNPotentials: in this state, the structure of the MN remains
     * unchanged, only some potentials stored in it have changed. Therefore,
     * the inference probably just needs to invalidate some already computed
     * potentials to be ready. Only a light amount of preparation is needed to
     * be able to perform inference.
     */
    void _setOutdatedMNPotentialsState();

    /// fired after a new joint target is inserted
    /** @param set The set of target variable's ids. */
    virtual void _onJointTargetAdded(const NodeSet& set) = 0;

    /// fired before a joint target is removed
    /** @param set The set of target variable's ids. */
    virtual void _onJointTargetErased(const NodeSet& set) = 0;

    /// fired before a all the marginal and joint targets are removed
    virtual void _onAllTargetsErased() = 0;

    ///  fired before a all the joint targets are removed
    virtual void _onAllJointTargetsErased() = 0;


    /// asks derived classes for the joint posterior of a declared target set
    /** @param set The set of ids of the variables whose joint posterior is
     * looked for. */
    virtual const Potential< GUM_SCALAR >& _jointPosterior(const NodeSet& set) = 0;

    /** @brief asks derived classes for the joint posterior of a set of
     * variables not declared as a joint target
     *
     * @param wanted_target The set of ids of the variables whose joint
     * posterior is looked for.
     * @param declared_target the joint target declared by the user that
     * contains set */
    virtual const Potential< GUM_SCALAR >&
       _jointPosterior(const NodeSet& wanted_target,
                       const NodeSet& declared_target) = 0;

    /** @brief returns a fresh unnormalized joint posterior of
     * a given set of variables
     * @param set The set of ids of the variables whose joint posterior is
     * looked for. */
    virtual Potential< GUM_SCALAR >*
       _unnormalizedJointPosterior(const NodeSet& set) = 0;

    /// returns a fresh potential equal to P(argument,evidence)
    virtual Potential< GUM_SCALAR >* _unnormalizedJointPosterior(NodeId id) = 0;


    private:
    /// the set of joint targets
    Set< NodeSet > __joint_targets;
    /// the current state of the inference (outdated/ready/done)
    StateOfMNInference __state{StateOfMNInference::OutdatedMNStructure};

    /// the Markov net on which we perform inferences
    const IMarkovNet< GUM_SCALAR >* __mn{nullptr};

    /// the domain sizes of the random variables
    NodeProperty< Size > __domain_sizes;

    /// the set of evidence entered into the network
    NodeProperty< const Potential< GUM_SCALAR >* > __evidence;

    /// assign to each node with a hard evidence the index of its observed value
    NodeProperty< Idx > __hard_evidence;

    /// the set of nodes that received soft evidence
    NodeSet __soft_evidence_nodes;

    /// the set of nodes that received hard evidence
    NodeSet __hard_evidence_nodes;

    /// set the state of the inference engine and
    /// call the notification _onStateChanged
    /// when necessary (i.e. when the state has effectively changed).
    virtual void __setState(const StateOfMNInference state) final;

    /// create the internal structure for a hard evidence
    Potential< GUM_SCALAR > __createHardEvidence(NodeId id, Idx val) const;

    /// checks whether a potential corresponds to a hard evidence or not
    bool __isHardEvidence(const Potential< GUM_SCALAR >& pot, Idx& val) const;

    /// computes the domain sizes of the random variables
    void __computeDomainSizes();

    /// assigns a MN during the inference engine construction
    void __setMarkovNetDuringConstruction(const IMarkovNet< GUM_SCALAR >* mn);

    private:
    /// whether the actual targets are default
    bool __targeted_mode;

    /// the set of marginal targets
    NodeSet __targets;

    /// sets all the nodes of the Markov net as targets
    void __setAllMarginalTargets();
  };


}   // namespace gum


#include <agrum/MN/inference/tools/MarkovNetInference_tpl.h>


#endif   // GUM_MARKOV_NET_INFERENCE_H
