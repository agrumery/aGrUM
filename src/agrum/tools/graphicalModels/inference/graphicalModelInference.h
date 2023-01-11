/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief This file contains abstract class definitions for graphical models
 *        inference classes.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Christophe GONZALES(_at_AMU)
 */
#ifndef AGRUM_GRAPHICALMODELINFERENCE_H
#define AGRUM_GRAPHICALMODELINFERENCE_H


#include <agrum/tools/graphicalModels/graphicalModel.h>
#include <agrum/tools/multidim/potential.h>
#include <agrum/agrum.h>


namespace gum {

  /**
   * @class GraphicalModelInference graphicalModelInference.h
   * <agrum/tools/graphicalModels/graphicalModel.h>
   * @brief A generic class for graphical model inference: handles evidence and the
   * current state of the inference
   * @ingroup gm_group
   *
   * The goal of the graphicalModelInference class is twofold:
   * i) handling the common resources of graphical Model inference (model,
   * soft/hard evidence); ii) propose a general high-level scheme for all the
   * inference methods.
   *
   * A specialized inference just has to specify how to prepare inference, how
   * to make inference and how to get the posteriors for nodes and set of nodes.
   * The scheme for every inference derived from graphicalModelInference will be
   * the same:
   *
   * 1- ie=SpecificInference(model);              // state <- OutdatedStructure
   * 2- set targets and evidence in ie
   * 3- ie.prepareInference();                 // state <- Ready4Inference
   * 4.a- change values of evidence in ie      // state <- OutdatedPotentials
   * 4.b- change some hard evidence or targets // state <- OutdatedStructure
   * 5- ie.makeInference();                    // state <- Done
   * 6- get posteriors
   * 7- goto 2 or 4
   *
   * graphicalModelInference can be in one of 4 different states:
   * - OutdatedStructure: in this state, the inference is fully unprepared
   *   to be applied because some events changed the "logical" structure of the
   *   model: for instance a node received a hard evidence, which implies that
   *   its outgoing arcs can be removed from the model, hence involving a
   *   structural change in the model.
   * - OutdatedPotentials: in this state, the structure of the model remains
   *   unchanged, only some potentials stored in it have changed. Therefore,
   *   the inference probably just needs to invalidate some already computed
   *   potentials to be ready. Only a light amount of preparation is needed to
   *   be able to perform inference.
   * - Ready4Inference: in this state, all the data structures are ready for
   *   inference. There just remains to perform the inference computations.
   * - Done: the heavy computations of inference have been done. There might
   *   still remain a few light computations to perform to get the posterior
   *   potentials we need.
   */

  template < typename GUM_SCALAR >
  class GraphicalModelInference {
    public:
    /**
     * current state of the inference
     *
     * graphicalModelInference can be in one of 4 different states:
     * - OutdatedStructure: in this state, the inference is fully unprepared
     *   to be applied because some events changed the "logical" structure of the
     *   model: for instance a node received a hard evidence, which implies that
     *   its outgoing arcs can be removed from the model, hence involving a
     *   structural change in the model.
     * - OutdatedPotentials: in this state, the structure of the model remains
     *   unchanged, only some potentials stored in it have changed. Therefore,
     *   the inference probably just needs to invalidate some already computed
     *   potentials to be ready. Only a light amount of preparation is needed to
     *   be able to perform inference.
     * - Ready4Inference: in this state, all the data structures are ready for
     *   inference. There just remains to perform the inference computations.
     * - Done: the heavy computations of inference have been done. There might
     *   still remain a few light computations to perform to get the posterior
     *   potentials we need.
     */
    enum class StateOfInference {
      OutdatedStructure,
      OutdatedPotentials,
      ReadyForInference,
      Done
    };


    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /** @warning note that, by aGrUM's rule, the model is not copied but only
     * referenced by the inference algorithm. */
    explicit GraphicalModelInference(const GraphicalModel* model);

    /// default constructor with a null model (useful for virtual inheritance)
    /** @warning graphicalModelInference is virtually inherited by
     * MarginalTargetedInference. As a result, the lowest descendant of
     * graphicalModelInference will create the latter. To avoid requiring
     * developers to add in the constructors of their inference algorithms a call
     * to graphicalModelInference( model ), we added constructor
     * graphicalModelInference(),
     * which will be called automatically by the lowest descendant.  */
    GraphicalModelInference();

    /// destructor
    virtual ~GraphicalModelInference();

    /// @}


    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// Returns a constant reference over the IBayesNet referenced by this class
    /** @throws UndefinedElement is raised if no Bayes net has been assigned to
     * the inference. */
    virtual const GraphicalModel& model() const final;

    /// get the domain sizes of the random variables of the model
    virtual const NodeProperty< Size >& domainSizes() const final;

    /// returns whether the inference object is in a ready state
    virtual bool isInferenceReady() const noexcept final;
    /// returns whether the inference object is in a OutdatedStructure state
    virtual bool isInferenceOutdatedStructure() const noexcept final;
    /// returns whether the inference object is in a OutdatedPotential state
    virtual bool isInferenceOutdatedPotentials() const noexcept final;
    /// returns whether the inference object is in a InferenceDone state
    /** The inference object is in a done state when the posteriors can be
     * retrieved without performing a new inference, i.e., all the heavy
     * computations have already been performed. Typically, in a junction tree
     * algorithm, this corresponds to a situation in which all the messages
     * needed in the JT have been computed and sent. */
    virtual bool isInferenceDone() const noexcept final;


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
    virtual void addEvidence(NodeId id, const Idx val) final;

    /// adds a new hard evidence on node named nodeName
    /**
     * @throw UndefinedElement if nodeName does not belong to the Bayesian network
     * @throw InvalidArgument if val is not a value for id
     * @throw InvalidArgument if nodeName already has an evidence
     */
    virtual void addEvidence(const std::string& nodeName, const Idx val) final;

    /// adds a new hard evidence on node id
    /**
     * @throw UndefinedElement if id does not belong to the Bayesian network
     * @throw InvalidArgument if val is not a value for id
     * @throw InvalidArgument if id already has an evidence
     */
    virtual void addEvidence(NodeId id, const std::string& label) final;

    /// adds a new hard evidence on node named nodeName
    /**
     * @throw UndefinedElement if nodeName does not belong to the Bayesian network
     * @throw InvalidArgument if val is not a value for id
     * @throw InvalidArgument if nodeName already has an evidence
     */
    virtual void addEvidence(const std::string& nodeName, const std::string& label) final;

    /// adds a new evidence on node id (might be soft or hard)
    /**
     * @throw UndefinedElement if id does not belong to the Bayesian network
     * @throw InvalidArgument if id already has an evidence
     * @throw FatalError if vals=[0,0,...,0]
     * @throw InvalidArgument if the size of vals is different from the domain
     *        size of node id
     */
    virtual void addEvidence(NodeId id, const std::vector< GUM_SCALAR >& vals) final;

    /// adds a new evidence on node named nodeName (might be soft or hard)
    /**
     * @throw UndefinedElement if id does not belong to the Bayesian network
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
     * does not belong to the Bayesian network
     * @throw InvalidArgument if the node of the potential already has an
     * evidence
     * @throw FatalError if pot=[0,0,...,0]
     */
    virtual void addEvidence(const Potential< GUM_SCALAR >& pot) final;

    /// adds a new evidence on node id (might be soft or hard)
    /**
     * @throw UndefinedElement if the potential is defined over several nodes
     * @throw UndefinedElement if the node on which the potential is defined
     * does not belong to the Bayesian network
     * @throw InvalidArgument if the node of the potential already has an
     * evidence
     * @throw FatalError if pot=[0,0,...,0]
     */
    virtual void addEvidence(Potential< GUM_SCALAR >&& pot) final;

    /// adds a new set of evidence
    /**
     * @throw UndefinedElement if some potential is defined over several nodes
     * @throw UndefinedElement if the node on which some potential is defined
     * does not belong to the Bayesian network
     * @throw InvalidArgument if the node of some potential already has an
     * evidence
     * @throw FatalError if pot=[0,0,...,0]
     */
    virtual void addSetOfEvidence(const Set< const Potential< GUM_SCALAR >* >& potset) final;

    /// adds a new list of evidence
    /**
     * @throw UndefinedElement if some potential is defined over several nodes
     * @throw UndefinedElement if the node on which some potential is defined
     * does not belong to the Bayesian network
     * @throw InvalidArgument if the node of some potential already has an
     * evidence
     * @throw FatalError if pot=[0,0,...,0]
     */
    virtual void addListOfEvidence(const List< const Potential< GUM_SCALAR >* >& potlist) final;

    /// change the value of an already existing hard evidence
    /**
     * @throw UndefinedElement if id does not belong to the Bayesian network
     * @throw InvalidArgument if val is not a value for id
     * @throw InvalidArgument if id does not already have an evidence
     */
    virtual void chgEvidence(NodeId id, const Idx val) final;

    /// change the value of an already existing hard evidence
    /**
     * @throw UndefinedElement if nodeName does not belong to the Bayesian network
     * @throw InvalidArgument if val is not a value for id
     * @throw InvalidArgument if id does not already have an evidence
     */
    virtual void chgEvidence(const std::string& nodeName, const Idx val) final;

    /// change the value of an already existing hard evidence
    /**
     * @throw UndefinedElement if id does not belong to the Bayesian network
     * @throw InvalidArgument if val is not a value for id
     * @throw InvalidArgument if id does not already have an evidence
     */
    virtual void chgEvidence(NodeId id, const std::string& label) final;

    /// change the value of an already existing hard evidence
    /**
     * @throw UndefinedElement if nodeName does not belong to the Bayesian network
     * @throw InvalidArgument if val is not a value for id
     * @throw InvalidArgument if id does not already have an evidence
     */
    virtual void chgEvidence(const std::string& nodeName, const std::string& label) final;

    /// change the value of an already existing evidence (might be soft or hard)
    /**
     * @throw UndefinedElement if id does not belong to the Bayesian network
     * @throw InvalidArgument if the node does not already have an evidence
     * @throw FatalError if vals=[0,0,...,0]
     * @throw InvalidArgument if the size of vals is different from the domain
     *        size of node id
     */
    virtual void chgEvidence(NodeId id, const std::vector< GUM_SCALAR >& vals) final;

    /// change the value of an already existing evidence (might be soft or hard)
    /**
     * @throw UndefinedElement if nodeName does not belong to the Bayesian network
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
     *        does not belong to the Bayesian network
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

    /// returns the number of evidence entered into the Bayesian network
    virtual Size nbrEvidence() const final;

    /// returns the number of hard evidence entered into the Bayesian network
    virtual Size nbrHardEvidence() const final;

    /// returns the number of soft evidence entered into the Bayesian network
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

    /// fired after a new Bayes net has been assigned to the engine
    virtual void onModelChanged_(const GraphicalModel* model) = 0;

    /// prepares inference when the latter is in OutdatedStructure state
    /** Note that the values of evidence are not necessarily
     * known and can be changed between updateOutdatedStructure_ and
     * makeInference_. */
    virtual void updateOutdatedStructure_() = 0;

    /// prepares inference when the latter is in OutdatedPotentials state
    /** Note that the values of evidence are not necessarily
     * known and can be changed between updateOutdatedPotentials_ and
     * makeInference_. */
    virtual void updateOutdatedPotentials_() = 0;

    /// called when the inference has to be performed effectively
    /** Once the inference is done, fillPosterior_ can be called. */
    virtual void makeInference_() = 0;

    /// put the inference into an outdated model structure state
    /** OutdatedStructure: in this state, the inference is fully unprepared
     * to be applied because some events changed the "logical" structure of the
     * model: for instance a node received a hard evidence, which implies that
     * its outgoing arcs can be removed from the model, hence involving a
     * structural change in the model. As a consequence, the (incremental)
     * inference (probably) needs a significant amount of preparation to be
     * ready for the next inference. In a Lazy propagation, for instance, this
     * step amounts to compute a new join tree, hence a new structure in which
     * inference will be applied. Note that classes that inherit from
     * graphicalModelInference may be smarter than graphicalModelInference and may,
     * in some situations, find out that their data structures are still ok for
     * inference and, therefore, only resort to perform the actions related to the
     * OutdatedPotentials state. As an example, consider a LazyPropagation
     * inference in Bayes Net A->B->C->D->E in which C has received hard evidence
     * e_C and E is the only target. In this case, A and B are not needed for
     * inference, the only potentials that matter are P(D|e_C) and P(E|D). So the
     * smallest join tree needed for inference contains only one clique DE. Now,
     * adding new evidence e_A on A has no impact on E given hard evidence e_C. In
     * this case, LazyPropagation can be smart and not update its join tree.*/
    void setOutdatedStructureState_();

    /** @brief puts the inference into an OutdatedPotentials state if it is
     * not already in an OutdatedStructure state
     *
     * OutdatedPotentials: in this state, the structure of the model remains
     * unchanged, only some potentials stored in it have changed. Therefore,
     * the inference probably just needs to invalidate some already computed
     * potentials to be ready. Only a light amount of preparation is needed to
     * be able to perform inference.
     */
    void setOutdatedPotentialsState_();


    private:
    /// the current state of the inference (outdated/ready/done)
    StateOfInference _state_{StateOfInference::OutdatedStructure};

    /// the Bayes net on which we perform inferences
    const GraphicalModel* _model_{nullptr};

    /// the domain sizes of the random variables
    NodeProperty< Size > _domain_sizes_;

    /// the set of evidence entered into the network
    NodeProperty< const Potential< GUM_SCALAR >* > _evidence_;

    /// assign to each node with a hard evidence the index of its observed value
    NodeProperty< Idx > _hard_evidence_;

    /// the set of nodes that received soft evidence
    NodeSet _soft_evidence_nodes_;

    /// the set of nodes that received hard evidence
    NodeSet _hard_evidence_nodes_;


    /// create the internal structure for a hard evidence
    Potential< GUM_SCALAR > _createHardEvidence_(NodeId id, Idx val) const;

    /// checks whether a potential corresponds to a hard evidence or not
    bool _isHardEvidence_(const Potential< GUM_SCALAR >& pot, Idx& val) const;

    /// computes the domain sizes of the random variables
    void _computeDomainSizes_();

    protected:
    /// set the state of the inference engine and
    /// call the notification onStateChanged_
    /// when necessary (i.e. when the state has effectively changed).
    virtual void setState_(const StateOfInference state) final;

    void setModel_(const GraphicalModel* model);

    /// assigns a model during the inference engine construction
    void setModelDuringConstruction_(const GraphicalModel* model);

    bool hasNoModel_() const { return _model_ == nullptr; };
  };
}   // namespace gum

#include <agrum/tools/graphicalModels/inference/graphicalModelInference_tpl.h>


#endif   // AGRUM_GRAPHICALMODELINFERENCE_H
