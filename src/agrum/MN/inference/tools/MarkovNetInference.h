
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
 * @brief This file contains abstract class definitions for Markov networks
 *        inference classes.
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */

#ifndef GUM_MARKOV_NET_INFERENCE_H
#define GUM_MARKOV_NET_INFERENCE_H


#include <agrum/agrum.h>
#include <agrum/MN/IMarkovNet.h>

namespace gum {

  // JointTargetedMNInference, the class for computing joint posteriors, should
  // have access to the states of Inference and change them when needed: this
  // will be a friend of Inference
  template < typename GUM_SCALAR >
  class JointTargetedMNInference;

  // MarginalTargetedMNInference, the class for computing marginal posteriors,
  // should have access to the states of Inference and change them when needed:
  // this should be a friend of Inference
  template < typename GUM_SCALAR >
  class MarginalTargetedMNInference;

  // EvidenceMNInference, the class for computing the probability of evidence,
  // should have access to the states of Inference and change them when needed:
  // this will be a friend of Inference
  template < typename GUM_SCALAR >
  class EvidenceMNInference;


  /**
   * @class MarkovNetInference inference.h
   * <agrum/MN/inference/MarkovNetInference.h>
   * @brief A generic class for Markov net inference: handles evidence and the
   * current state of the (incremental) inference
   * @ingroup mn_group
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
   * 3- ie.prepareInference();                 // state <- ReadyForMNInference
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
   *   inference (probably) needs a significant amount of preparation to be ready
   *   for the next inference. In a Lazy propagation, for instance, this step
   *   amounts to compute a new join tree, hence a new structure in which inference
   *   will be applied. Note that classes that inherit from MarkovNetInference
   *   may be smarter than MarkovNetInference and may, in some situations,
   *   find out that their data structures are still ok for inference and,
   *   therefore, only resort to perform the actions related to the
   *   OutdatedMNPotentials state.
   * - OutdatedMNPotentials: in this state, the structure of the MN remains
   *   unchanged, only some potentials stored in it have changed. Therefore,
   *   the inference probably just needs to invalidate some already computed
   *   potentials to be ready. Only a light amount of preparation is needed to
   *   be able to perform inference.
   * - ReadyForMNInference: in this state, all the data structures are ready for
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
     *   to the OutdatedMNPotentials state.
     * - OutdatedMNPotentials: in this state, the structure of the MN remains
     *   unchanged, only some potentials stored in it have changed. Therefore,
     *   the inference probably just needs to invalidate some already computed
     *   potentials to be ready. Only a light amount of preparation is needed to
     *   be able to perform inference.
     * - ReadyForMNInference: in this state, all the data structures are ready for
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
      ReadyForMNInference,
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
     * MarginalTargetedMNInference. As a result, the lowest descendant of
     * MarkovNetInference will create the latter. To avoid requiring developpers
     * to add in the constructors of their inference algorithms a call to
     * MarkovNetInference( mn ), we added constructor MarkovNetInference(),
     * which will be called automatically by the lowest descendant.
     * Then, MarginalTargetedMNInference and JointTargetedMNInference will take
     * care of setting the appropriate mn into MarkovNetInference. */
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
     * @throw UndefinedElement if id does not belong to the Markov network
     * @throw InvalidArgument if val is not a value for id
     * @throw InvalidArgument if id already has an evidence
     */
    virtual void addEvidence(NodeId id, const Idx val) final;

    /// adds a new hard evidence on node named nodeName
    /**
     * @throw UndefinedElement if nodeName does not belong to the Markov network
     * @throw InvalidArgument if val is not a value for id
     * @throw InvalidArgument if nodeName already has an evidence
     */
    virtual void addEvidence(const std::string& nodeName, const Idx val) final;

    /// adds a new hard evidence on node id
    /**
     * @throw UndefinedElement if id does not belong to the Markov network
     * @throw InvalidArgument if val is not a value for id
     * @throw InvalidArgument if id already has an evidence
     */
    virtual void addEvidence(NodeId id, const std::string& label) final;

    /// adds a new hard evidence on node named nodeName
    /**
     * @throw UndefinedElement if nodeName does not belong to the Markov network
     * @throw InvalidArgument if val is not a value for id
     * @throw InvalidArgument if nodeName already has an evidence
     */
    virtual void addEvidence(const std::string& nodeName,
                             const std::string& label) final;

    /// adds a new evidence on node id (might be soft or hard)
    /**
     * @throw UndefinedElement if id does not belong to the Markov network
     * @throw InvalidArgument if id already has an evidence
     * @throw FatalError if vals=[0,0,...,0]
     * @throw InvalidArgument if the size of vals is different from the domain
     *        size of node id
     */
    virtual void addEvidence(NodeId                           id,
                             const std::vector< GUM_SCALAR >& vals) final;

    /// adds a new evidence on node named nodeName (might be soft or hard)
    /**
     * @throw UndefinedElement if id does not belong to the Markov network
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
     * does not belong to the Markov network
     * @throw InvalidArgument if the node of the potential already has an
     * evidence
     * @throw FatalError if pot=[0,0,...,0]
     */
    virtual void addEvidence(const Potential< GUM_SCALAR >& pot) final;

    /// adds a new evidence on node id (might be soft or hard)
    /**
     * @throw UndefinedElement if the potential is defined over several nodes
     * @throw UndefinedElement if the node on which the potential is defined
     * does not belong to the Markov network
     * @throw InvalidArgument if the node of the potential already has an
     * evidence
     * @throw FatalError if pot=[0,0,...,0]
     */
    virtual void addEvidence(Potential< GUM_SCALAR >&& pot) final;

    /// adds a new set of evidence
    /**
     * @throw UndefinedElement if some potential is defined over several nodes
     * @throw UndefinedElement if the node on which some potential is defined
     * does not belong to the Markov network
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
     * does not belong to the Markov network
     * @throw InvalidArgument if the node of some potential already has an
     * evidence
     * @throw FatalError if pot=[0,0,...,0]
     */
    virtual void addListOfEvidence(
       const List< const Potential< GUM_SCALAR >* >& potlist) final;

    /// change the value of an already existing hard evidence
    /**
     * @throw UndefinedElement if id does not belong to the Markov network
     * @throw InvalidArgument if val is not a value for id
     * @throw InvalidArgument if id does not already have an evidence
     */
    virtual void chgEvidence(NodeId id, const Idx val) final;

    /// change the value of an already existing hard evidence
    /**
     * @throw UndefinedElement if nodeName does not belong to the Markov network
     * @throw InvalidArgument if val is not a value for id
     * @throw InvalidArgument if id does not already have an evidence
     */
    virtual void chgEvidence(const std::string& nodeName, const Idx val) final;

    /// change the value of an already existing hard evidence
    /**
     * @throw UndefinedElement if id does not belong to the Markov network
     * @throw InvalidArgument if val is not a value for id
     * @throw InvalidArgument if id does not already have an evidence
     */
    virtual void chgEvidence(NodeId id, const std::string& label) final;

    /// change the value of an already existing hard evidence
    /**
     * @throw UndefinedElement if nodeName does not belong to the Markov network
     * @throw InvalidArgument if val is not a value for id
     * @throw InvalidArgument if id does not already have an evidence
     */
    virtual void chgEvidence(const std::string& nodeName,
                             const std::string& label) final;

    /// change the value of an already existing evidence (might be soft or hard)
    /**
     * @throw UndefinedElement if id does not belong to the Markov network
     * @throw InvalidArgument if the node does not already have an evidence
     * @throw FatalError if vals=[0,0,...,0]
     * @throw InvalidArgument if the size of vals is different from the domain
     *        size of node id
     */
    virtual void chgEvidence(NodeId                           id,
                             const std::vector< GUM_SCALAR >& vals) final;

    /// change the value of an already existing evidence (might be soft or hard)
    /**
     * @throw UndefinedElement if nodeName does not belong to the Markov network
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
     *        does not belong to the Markov network
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

    /// returns the number of evidence entered into the Markov network
    virtual Size nbrEvidence() const final;

    /// returns the number of hard evidence entered into the Markov network
    virtual Size nbrHardEvidence() const final;

    /// returns the number of soft evidence entered into the Markov network
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


    protected:
    /// fired when the stage is changed
    virtual void onStateChanged_() = 0;

    /// fired after a new evidence is inserted
    virtual void onEvidenceAdded_(const NodeId id, bool isHardEvidence) = 0;

    /// fired before an evidence is removed
    virtual void onEvidenceErased_(const NodeId id, bool isHardEvidence) = 0;

    /// fired before all the evidence are erased
    virtual void onAllEvidenceErased_(bool contains_hard_evidence) = 0;

    /** @brief fired after an evidence is changed, in particular when its status
     * (soft/hard) changes
     *
     * @param nodeId the node of the changed evidence
     * @param hasChangedSoftHard true if the evidence has changed from Soft to
     * Hard or from Hard to Soft
     *
     */
    virtual void onEvidenceChanged_(const NodeId id, bool hasChangedSoftHard) = 0;

    /// fired after a new Markov net has been assigned to the engine
    virtual void onMarkovNetChanged_(const IMarkovNet< GUM_SCALAR >* mn) = 0;

    /// prepares inference when the latter is in OutdatedMNStructure state
    /** Note that the values of evidence are not necessarily
     * known and can be changed between updateOutdatedMNStructure_ and
     * makeMNInference_. */
    virtual void updateOutdatedMNStructure_() = 0;

    /// prepares inference when the latter is in OutdatedMNPotentials state
    /** Note that the values of evidence are not necessarily
     * known and can be changed between updateOutdatedMNPotentials_ and
     * makeMNInference_. */
    virtual void updateOutdatedMNPotentials_() = 0;

    /// called when the inference has to be performed effectively
    /** Once the inference is done, fillPosterior_ can be called. */
    virtual void makeInference_() = 0;

    /// put the inference into an outdated MN structure state
    /** OutdatedMNStructure: in this state, the inference is fully unprepared
     * to be applied because some events changed the "logical" structure of the
     * MN: for instance a node received a hard evidence, which implies that
     * its outgoing arcs can be removed from the MN, hence involving a
     * structural change in the MN. As a consequence, the (incremental)
     * inference (probably) needs a significant amount of preparation to be
     * ready for the next inference.*/
    void setOutdatedMNStructureState_();

    /** @brief puts the inference into an OutdatedMNPotentials state if it is
     * not already in an OutdatedMNStructure state
     *
     * OutdatedMNPotentials: in this state, the structure of the MN remains
     * unchanged, only some potentials stored in it have changed. Therefore,
     * the inference probably just needs to invalidate some already computed
     * potentials to be ready. Only a light amount of preparation is needed to
     * be able to perform inference.
     */
    void setOutdatedMNPotentialsState_();


    private:
    /// the current state of the inference (outdated/ready/done)
    StateOfMNInference state__{StateOfMNInference::OutdatedMNStructure};

    /// the Markov net on which we perform inferences
    const IMarkovNet< GUM_SCALAR >* mn__{nullptr};

    /// the domain sizes of the random variables
    NodeProperty< Size > domain_sizes__;

    /// the set of evidence entered into the network
    NodeProperty< const Potential< GUM_SCALAR >* > evidence__;

    /// assign to each node with a hard evidence the index of its observed value
    NodeProperty< Idx > hard_evidence__;

    /// the set of nodes that received soft evidence
    NodeSet soft_evidence_nodes__;

    /// the set of nodes that received hard evidence
    NodeSet hard_evidence_nodes__;

    /// set the state of the inference engine and
    /// call the notification onStateChanged_
    /// when necessary (i.e. when the state has effectively changed).
    virtual void setState__(const StateOfMNInference state) final;

    /// create the internal structure for a hard evidence
    Potential< GUM_SCALAR > createHardEvidence__(NodeId id, Idx val) const;

    /// checks whether a potential corresponds to a hard evidence or not
    bool isHardEvidence__(const Potential< GUM_SCALAR >& pot, Idx& val) const;

    /// computes the domain sizes of the random variables
    void computeDomainSizes__();

    /// assigns a MN during the inference engine construction
    void setMarkovNetDuringConstruction__(const IMarkovNet< GUM_SCALAR >* mn);


    /// allow JointInference to access the single targets and inference states
    friend MarginalTargetedMNInference< GUM_SCALAR >;
    friend JointTargetedMNInference< GUM_SCALAR >;
    friend EvidenceMNInference< GUM_SCALAR >;
  };


}   // namespace gum


#include <agrum/MN/inference/tools/MarkovNetInference_tpl.h>


#endif   // GUM_MARKOV_NET_INFERENCE_H
