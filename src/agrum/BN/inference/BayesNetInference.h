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
 * @brief This file contains abstract class definitions for Bayesian networks
 *        inference classes.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_BAYES_NET_INFERENCE_H
#define GUM_BAYES_NET_INFERENCE_H


#include <agrum/BN/IBayesNet.h>
#include <agrum/config.h>


namespace gum {


  /** @brief type of algorithm to determine barren nodes
   *
   * When constructing messages from one clique to its neighbor, we can
   * determine that some nodes are barren, i.e., they are the only one
   * at the left hand side of a conditioning bar and they appear in only one
   * potential. In such case, in a classical BN inference, there is no need
   * to take them into account since their removal will necessarily create
   * a constant potential. So, we can discard their potential from the
   * computation. However, when computing p(evidence), we should not do that
   * because the constant is important and need be computed.
   */
  enum class FindBarrenNodesType {
    FIND_NO_BARREN_NODES,  // do not try to find barren nodes
    FIND_BARREN_NODES      // use a bottom-up algorithm to detect barren nodes
  };


  // JointTargetedInference, the class for computing joint posteriors, should
  // have access to the states of Inference and change them when needed: this
  // will be a friend of Inference
  template <typename GUM_SCALAR>
  class JointTargetedInference;

  // MarginalTargetedInference, the class for computing marginal posteriors,
  // should have access to the states of Inference and change them when needed:
  // this should be a friend of Inference
  template <typename GUM_SCALAR>
  class MarginalTargetedInference;

  // EvidenceInference, the class for computing the probability of evidence,
  // should have access to the states of Inference and change them when needed:
  // this will be a friend of Inference
  template <typename GUM_SCALAR>
  class EvidenceInference;


  /**
   * @class BayesNetInference inference.h
   * <agrum/BN/inference/BayesNetInference.h>
   * @brief A generic class for Bayes net inference: handles evidence and the
   * current state of the (incremental) inference
   * @ingroup bn_group
   *
   * The goal of the BayesNetInference class is twofold:
   * i) handling the common resources of BN inference (bn, soft/hard evidence);
   * ii) propose a general high-level scheme for all the inference methods.
   *
   * A specialized inference just has to specify how to prepare inference, how
   * to make inference and how to get the posteriors for nodes and set of nodes.
   * The scheme for every inference derived from BayesNetInference will be
   * the same:
   *
   * 1- ie=SpecificInference(bn);              // state <- OutdatedBNStructure
   * 2- set targets and evidence in ie
   * 3- ie.prepareInference();                 // state <- Ready4Inference
   * 4.a- change values of evidence in ie      // state <- OutdatedBNPotentials
   * 4.b- change some hard evidence or targets // state <- OutdatedBNStructure
   * 5- ie.makeInference();                    // state <- Done
   * 6- get posteriors
   * 7- goto 2 or 4
   *
   * BayesNetInference can be in one of 4 different states:
   * - OutdatedBNStructure: in this state, the inference is fully unprepared
   *   to be applied because some events changed the "logical" structure of the
   *   BN: for instance a node received a hard evidence, which implies that
   *   its outgoing arcs can be removed from the BN, hence involving a
   *   structural change in the BN. As a consequence, the (incremental)
   *   inference
   *   (probably) needs a significant amount of preparation to be ready for the
   *   next inference. In a Lazy propagation, for instance, this step amounts to
   *   compute a new join tree, hence a new structure in which inference
   *   will be applied. Note that classes that inherit from BayesNetInference
   *   may be smarter than BayesNetInference and may, in some situations,
   *   find out that their
   *   data structures are still ok for inference and, therefore, only resort to
   *   perform the actions related to the OutdatedBNPotentials state. As an
   *   example, consider a LazyPropagation inference in Bayes Net A->B->C->D->E
   *   in which C has received hard evidence e_C and E is the only target. In
   *   this case, A and B are not needed for inference, the only potentials that
   *   matter are P(D|e_C) and P(E|D). So the smallest join tree needed for
   *   inference contains only one clique DE. Now, adding new evidence e_A on A
   *   has no impact on E given hard evidence e_C. In this case, LazyPropagation
   *   can be smart and not update its join tree.
   * - OutdatedBNPotentials: in this state, the structure of the BN remains
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

  template <typename GUM_SCALAR>
  class BayesNetInference {
    public:
    /**
     * current state of the inference
     *
     * BayesNetInference can be in one of 4 different states:
     * - OutdatedBNStructure: in this state, the inference is fully unprepared
     *   to be applied because some events changed the "logical" structure of
     *   the BN: for instance a node received a hard evidence, which implies
     *   that its outgoing arcs can be removed from the BN, hence involving a
     *   structural change in the BN. As a consequence, the (incremental)
     *   inference (probably) needs a significant amount of preparation to be
     *   ready for the next inference. In a Lazy propagation, for instance,
     *   this step amounts to compute a new join tree, hence a new structure
     *   in which inference will be applied. Note that classes that inherit
     *   from BayesNetInference may be smarter than BayesNetInference and may,
     *   in some situations, find out that their data structures are still ok for
     *   inference and, therefore, only resort to perform the actions related
     *   to the OutdatedBNPotentials state. As an example, consider a
     *   LazyPropagation inference in Bayes Net A->B->C->D->E
     *   in which C has received hard evidence e_C and E is the only target. In
     *   this case, A and B are not needed for inference, the only potentials
     *   that matter are P(D|e_C) and P(E|D). So the smallest join tree needed
     *   for inference contains only one clique DE. Now, adding new evidence
     *   e_A on A has no impact on E given hard evidence e_C. In this case,
     *   LazyPropagation can be smart and not update its join tree.
     * - OutdatedBNPotentials: in this state, the structure of the BN remains
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
    enum class StateOfInference {
      OutdatedBNStructure,
      OutdatedBNPotentials,
      InferenceReady,
      Done
    };


    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /** @warning note that, by aGrUM's rule, the BN is not copied but only
     * referenced by the inference algorithm. */
    BayesNetInference( const IBayesNet<GUM_SCALAR>* bn );

    /// default constructor with a null BN (useful for virtual inheritance)
    /** @warning BayesNetInference is virtually inherited by
     * MarginalTargetedInference. As a result, the lowest descendant of
     * BayesNetInference will create the latter. To avoid requiring developpers
     * to add in the constructors of their inference algorithms a call to
     * BayesNetInference( bn ), we added constructor BayesNetInference(),
     * which will be called automatically by the lowest descendant.
     * Then, MarginalTargetedInference and JointTargetedInference will take care
     * of setting the appropriate bn into BayesNetInference. */
    BayesNetInference();

    /// destructor
    virtual ~BayesNetInference();

    /// @}


    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// assigns a new BN to the inference engine
    /** Assigns a new BN to the BayesNetInference engine and sends messages to the
     * descendants of BayesNetInference to inform them that the BN has changed.
     * @warning By default, all the nodes of the Bayes net are targets.
     * @warning note that, by aGrUM's rule, the bn is not copied into the
     * inference engine but only referenced. */
    virtual void setBN(const IBayesNet<GUM_SCALAR> *bn);

    /// Returns a constant reference over the IBayesNet referenced by this class
    /** @throws UndefinedElement is raised if no Bayes net has been assigned to
     * the inference. */
    virtual const IBayesNet<GUM_SCALAR>& BN() const final;

    /// get the domain sizes of the random variables of the BN
    virtual const NodeProperty<Size>& domainSizes() const final;

    /// returns whether the inference object is in a ready state
    virtual bool isInferenceReady() const noexcept final;

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
    virtual StateOfInference state() const noexcept final;

    /// @}


    // ############################################################################
    /// @name Evidence
    // ############################################################################
    /// @{

    /// adds a new hard evidence on node id
    /**
     * @throw UndefinedElement if id does not belong to the Bayesian network
     * @throw InvalidArgument if val is not a value for id
     * @throw InvalidArgument if id already has an evidence
     */
    virtual void addEvidence( const NodeId id, const Idx val ) final;

    /// adds a new hard evidence on node named nodeName
    /**
     * @throw UndefinedElement if nodeName does not belong to the Bayesian network
     * @throw InvalidArgument if val is not a value for id
     * @throw InvalidArgument if nodeName already has an evidence
     */
    virtual void addEvidence( const std::string& nodeName, const Idx val ) final;

    /// adds a new hard evidence on node id
    /**
     * @throw UndefinedElement if id does not belong to the Bayesian network
     * @throw InvalidArgument if val is not a value for id
     * @throw InvalidArgument if id already has an evidence
     */
    virtual void addEvidence( const NodeId id, const std::string& label ) final;

    /// adds a new hard evidence on node named nodeName
    /**
     * @throw UndefinedElement if nodeName does not belong to the Bayesian network
     * @throw InvalidArgument if val is not a value for id
     * @throw InvalidArgument if nodeName already has an evidence
     */
    virtual void addEvidence( const std::string& nodeName,
                              const std::string& label ) final;

    /// adds a new evidence on node id (might be soft or hard)
    /**
     * @throw UndefinedElement if id does not belong to the Bayesian network
     * @throw InvalidArgument if id already has an evidence
     * @throw FatalError if vals=[0,0,...,0]
     * @throw InvalidArgument if the size of vals is different from the domain
     *        size of node id
     */
    virtual void addEvidence( const NodeId                   id,
                              const std::vector<GUM_SCALAR>& vals ) final;

    /// adds a new evidence on node named nodeName (might be soft or hard)
    /**
     * @throw UndefinedElement if id does not belong to the Bayesian network
     * @throw InvalidArgument if nodeName already has an evidence
     * @throw FatalError if vals=[0,0,...,0]
     * @throw InvalidArgument if the size of vals is different from the domain
     *        size of node nodeName
     */
    virtual void addEvidence( const std::string&             nodeName,
                              const std::vector<GUM_SCALAR>& vals ) final;

    /// adds a new evidence on node id (might be soft or hard)
    /**
     * @throw UndefinedElement if the potential is defined over several nodes
     * @throw UndefinedElement if the node on which the potential is defined
     * does not belong to the Bayesian network
     * @throw InvalidArgument if the node of the potential already has an
     * evidence
     * @throw FatalError if pot=[0,0,...,0]
     */
    virtual void addEvidence( const Potential<GUM_SCALAR>& pot ) final;

    /// adds a new evidence on node id (might be soft or hard)
    /**
     * @throw UndefinedElement if the potential is defined over several nodes
     * @throw UndefinedElement if the node on which the potential is defined
     * does not belong to the Bayesian network
     * @throw InvalidArgument if the node of the potential already has an
     * evidence
     * @throw FatalError if pot=[0,0,...,0]
     */
    virtual void addEvidence( Potential<GUM_SCALAR>&& pot ) final;

    /// adds a new set of evidence
    /**
     * @throw UndefinedElement if some potential is defined over several nodes
     * @throw UndefinedElement if the node on which some potential is defined
     * does not belong to the Bayesian network
     * @throw InvalidArgument if the node of some potential already has an
     * evidence
     * @throw FatalError if pot=[0,0,...,0]
     */
    virtual void
    addSetOfEvidence( const Set<const Potential<GUM_SCALAR>*>& potlist ) final;

    /// adds a new list of evidence
    /**
     * @throw UndefinedElement if some potential is defined over several nodes
     * @throw UndefinedElement if the node on which some potential is defined
     * does not belong to the Bayesian network
     * @throw InvalidArgument if the node of some potential already has an
     * evidence
     * @throw FatalError if pot=[0,0,...,0]
     */
    virtual void
    addListOfEvidence( const List<const Potential<GUM_SCALAR>*>& potlist ) final;

    /// change the value of an already existing hard evidence
    /**
     * @throw UndefinedElement if id does not belong to the Bayesian network
     * @throw InvalidArgument if val is not a value for id
     * @throw InvalidArgument if id does not already have an evidence
     */
    virtual void chgEvidence( const NodeId id, const Idx val ) final;

    /// change the value of an already existing hard evidence
    /**
     * @throw UndefinedElement if nodeName does not belong to the Bayesian network
     * @throw InvalidArgument if val is not a value for id
     * @throw InvalidArgument if id does not already have an evidence
     */
    virtual void chgEvidence( const std::string& nodeName, const Idx val ) final;

    /// change the value of an already existing hard evidence
    /**
     * @throw UndefinedElement if id does not belong to the Bayesian network
     * @throw InvalidArgument if val is not a value for id
     * @throw InvalidArgument if id does not already have an evidence
     */
    virtual void chgEvidence( const NodeId id, const std::string& label ) final;

    /// change the value of an already existing hard evidence
    /**
     * @throw UndefinedElement if nodeName does not belong to the Bayesian network
     * @throw InvalidArgument if val is not a value for id
     * @throw InvalidArgument if id does not already have an evidence
     */
    virtual void chgEvidence( const std::string& nodeName,
                              const std::string& label ) final;

    /// change the value of an already existing evidence (might be soft or hard)
    /**
     * @throw UndefinedElement if id does not belong to the Bayesian network
     * @throw InvalidArgument if the node does not already have an evidence
     * @throw FatalError if vals=[0,0,...,0]
     * @throw InvalidArgument if the size of vals is different from the domain
     *        size of node id
     */
    virtual void chgEvidence( const NodeId                   id,
                              const std::vector<GUM_SCALAR>& vals ) final;

    /// change the value of an already existing evidence (might be soft or hard)
    /**
     * @throw UndefinedElement if nodeName does not belong to the Bayesian network
     * @throw InvalidArgument if the node does not already have an evidence
     * @throw FatalError if vals=[0,0,...,0]
     * @throw InvalidArgument if the size of vals is different from the domain
     *        size of node id
     */
    virtual void chgEvidence( const std::string&             nodeName,
                              const std::vector<GUM_SCALAR>& vals ) final;

    /// change the value of an already existing evidence (might be soft or hard)
    /**
     * @throw UndefinedElement if the potential is defined over several nodes
     * @throw UndefinedElement if the node on which the potential is defined
     *        does not belong to the Bayesian network
     * @throw InvalidArgument if the node of the potential does not already
     *        have an evidence
     * @throw FatalError if pot=[0,0,...,0]
     */
    virtual void chgEvidence( const Potential<GUM_SCALAR>& pot ) final;

    /// removes all the evidence entered into the network
    virtual void eraseAllEvidence() final;

    /// removed the evidence, if any, corresponding to node id
    virtual void eraseEvidence( const NodeId id ) final;

    /// removed the evidence, if any, corresponding to node of name nodeName
    virtual void eraseEvidence( const std::string& nodeName ) final;

    /// indicates whether some node(s) have received evidence
    virtual bool hasEvidence() const final;

    /// indicates whether node id has received an evidence
    virtual bool hasEvidence( const NodeId id ) const final;

    /// indicates whether node id has received a hard evidence
    virtual bool hasHardEvidence( const NodeId id ) const final;

    /// indicates whether node id has received a soft evidence
    virtual bool hasSoftEvidence( const NodeId id ) const final;
    /// indicates whether node id has received an evidence

    virtual bool hasEvidence( const std::string& nodeName ) const final;

    /// indicates whether node id has received a hard evidence
    virtual bool hasHardEvidence( const std::string& nodeName ) const final;

    /// indicates whether node id has received a soft evidence
    virtual bool hasSoftEvidence( const std::string& nodeName ) const final;

    /// returns the number of evidence entered into the Bayesian network
    virtual Size nbrEvidence() const final;

    /// returns the number of hard evidence entered into the Bayesian network
    virtual Size nbrHardEvidence() const final;

    /// returns the number of soft evidence entered into the Bayesian network
    virtual Size nbrSoftEvidence() const final;

    /// returns the set of evidence
    const NodeProperty<const Potential<GUM_SCALAR>*>& evidence() const;

    /// returns the set of nodes with soft evidence
    const NodeSet& softEvidenceNodes() const;

    /// returns the set of nodes with hard evidence
    const NodeSet& hardEvidenceNodes() const;

    /// indicate for each node with hard evidence which value it took
    const NodeProperty<Idx>& hardEvidence() const;

    /// @}


    protected:
    /// fired after a new evidence is inserted
    virtual void _onEvidenceAdded( const NodeId id, bool isHardEvidence ) = 0;

    /// fired before an evidence is removed
    virtual void _onEvidenceErased( const NodeId id, bool isHardEvidence ) = 0;

    /// fired before all the evidence are erased
    virtual void _onAllEvidenceErased( bool contains_hard_evidence ) = 0;

    /** @brief fired after an evidence is changed, in particular when its status
     * (soft/hard) changes
     *
     * @param nodeId the node of the changed evidence
     * @param hasChangedSoftHard true if the evidence has changed from Soft to
     * Hard or from Hard to Soft
     *
     */
    virtual void _onEvidenceChanged( const NodeId id,
                                     bool         hasChangedSoftHard ) = 0;

    /// fired after a new Bayes net has been assigned to the engine
    virtual void _onBayesNetChanged( const IBayesNet<GUM_SCALAR>* bn ) = 0;

    /// prepares inference when the latter is in OutdatedBNStructure state
    /** Note that the values of evidence are not necessarily
     * known and can be changed between _updateOutdatedBNStructure and
     * _makeInference. */
    virtual void _updateOutdatedBNStructure() = 0;

    /// prepares inference when the latter is in OutdatedBNPotentials state
    /** Note that the values of evidence are not necessarily
     * known and can be changed between _updateOutdatedBNPotentials and
     * _makeInference. */
    virtual void _updateOutdatedBNPotentials() = 0;

    /// called when the inference has to be performed effectively
    /** Once the inference is done, _fillPosterior can be called. */
    virtual void _makeInference() = 0;

    /// put the inference into an outdated BN structure state
    /** OutdatedBNStructure: in this state, the inference is fully unprepared
     * to be applied because some events changed the "logical" structure of the
     * BN: for instance a node received a hard evidence, which implies that
     * its outgoing arcs can be removed from the BN, hence involving a
     * structural change in the BN. As a consequence, the (incremental)
     * inference (probably) needs a significant amount of preparation to be
     * ready for the next inference. In a Lazy propagation, for instance, this
     * step amounts to compute a new join tree, hence a new structure in which
     * inference will be applied. Note that classes that inherit from
     * BayesNetInference may be smarter than BayesNetInference and may, in some
     * situations, find out that their
     * data structures are still ok for inference and, therefore, only resort to
     * perform the actions related to the OutdatedBNPotentials state. As an
     * example, consider a LazyPropagation inference in Bayes Net A->B->C->D->E
     * in which C has received hard evidence e_C and E is the only target. In
     * this case, A and B are not needed for inference, the only potentials that
     * matter are P(D|e_C) and P(E|D). So the smallest join tree needed for
     * inference contains only one clique DE. Now, adding new evidence e_A on A
     * has no impact on E given hard evidence e_C. In this case, LazyPropagation
     * can be smart and not update its join tree.*/
    void _setOutdatedBNStructureState();

    /** @brief puts the inference into an OutdatedBNPotentials state if it is
     * not already in an OutdatedBNStructure state
     *
     * OutdatedBNPotentials: in this state, the structure of the BN remains
     * unchanged, only some potentials stored in it have changed. Therefore,
     * the inference probably just needs to invalidate some already computed
     * potentials to be ready. Only a light amount of preparation is needed to
     * be able to perform inference.
     */
    void _setOutdatedBNPotentialsState();


    private:
    /// the current state of the inference (outdated/ready/done)
    StateOfInference __state{StateOfInference::OutdatedBNStructure};

    /// the Bayes net on which we perform inferences
    const IBayesNet<GUM_SCALAR>* __bn{nullptr};

    /// the domain sizes of the random variables
    NodeProperty<Size> __domain_sizes;

    /// the set of evidence entered into the network
    NodeProperty<const Potential<GUM_SCALAR>*> __evidence;

    /// assign to each node with a hard evidence the index of its observed value
    NodeProperty<Idx> __hard_evidence;

    /// the set of nodes that received soft evidence
    NodeSet __soft_evidence_nodes;

    /// the set of nodes that received hard evidence
    NodeSet __hard_evidence_nodes;


    /// create the internal structure for a hard evidence
    Potential<GUM_SCALAR> __createHardEvidence( NodeId id, Idx val ) const;

    /// checks whether a potential corresponds to a hard evidence or not
    bool __isHardEvidence( const Potential<GUM_SCALAR>& pot, Idx& val ) const;

    /// computes the domain sizes of the random variables
    void __computeDomainSizes();

    /// assigns a BN during the inference engine construction
    void __setBayesNetDuringConstruction( const IBayesNet<GUM_SCALAR>* bn );


    /// allow JointInference to access the single targets and inference states
    friend MarginalTargetedInference<GUM_SCALAR>;
    friend JointTargetedInference<GUM_SCALAR>;
    friend EvidenceInference<GUM_SCALAR>;
  };


}  // namespace gum


#include <agrum/BN/inference/BayesNetInference_tpl.h>


#endif  // GUM_BAYES_NET_INFERENCE_H
