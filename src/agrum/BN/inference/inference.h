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
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 */

#ifndef GUM_INFERENCE_H
#define GUM_INFERENCE_H


#include <agrum/config.h>
#include <agrum/BN/IBayesNet.h>


namespace gum {

  
  /**
   * @class Inference inference.h
   * <agrum/BN/inference/Inference.h>
   * @brief Implementation of a generic class for Bayes net inference.
   * @ingroup bn_group
   *
   * The goal of the Inference class is twofold: i) handling the common resources
   * of BN inference (bn, soft/hard evidence and targets); and
   * ii) propose a general scheme for all inference methods.
   *
   * A specialized inference just has to specify how to prepare inference, how
   * to make inference and how to get the posteriors for nodes in
   * pure virtual protected methods. and the scheme for every inference
   * derived from Inference will be the same:
   *
   * 1- ie=SpecificInference(bn); // state <- Unprepared
   * 2- set targets and evidence in ie
   * 3- ie.prepareInference();    // state <- Ready
   * 4- change values Of evidence in ie
   * 5- ie.makeInference();       // state <- Done
   * 6- get posteriors
   * 7- goto 2 or 4
   *
   */

  template <typename GUM_SCALAR> class Inference {
  public:
    /**
     * current state of the inference
     * Unprepared/Outdated [addEvidence] --(prepareInference)--> Ready
     * [changeEvidence]--(makeInference)--> Done
     */
    enum class StateOfInference {
      UnpreparedStructure,
      OutdatedPotentials,
      Ready4Inference,
      Done
    };


    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    Inference( const IBayesNet<GUM_SCALAR>& bn );

    /// destructor
    virtual ~Inference();

    /// @}

    
    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// Compute the posterior of a node.
    /**
     * @returns a const ref to the internal representation of the posterior
     * probability of the node.
     * @param node the node for which we need a posterior probability
     *
     * @warning for efficiency, the potential is not copied. In order to ensure
     * that the potential may still exist even if the Inference object is
     * destroyed, the copy has to be explicit.
     *
     * @warning prepareInference and makeInference may be applied if needed.
     *
     * @throw UndefinedElement if node is not in the set of targets
     */
    virtual const Potential<GUM_SCALAR>& posterior( NodeId node ) final;

    /// Compute the posterior of a set of nodes.
    /**
     * @returns a const ref to the internal representation of the posterior
     * probability of the set of nodes.
     * @param nodes the set of nodes whose posterior probability is wanted
     *
     * @warning for efficiency, the potential is not copied. In order to ensure
     * that the potential may still exist even if the Inference object is
     * destroyed, the copy has to be explicit.
     *
     * @warning prepareInference and makeInference may be applied if needed.
     *
     * @throw UndefinedElement if nodes is not in the targets
     */
    virtual const Potential<GUM_SCALAR>& posterior ( const NodeSet& nodes ) final;

    /// returns whether the inference object is in a ready state
    virtual bool isReady () const noexcept final;

    /// returns whether the inference object is in a done state
    /** The inference object is in a done state when the posteriors can be
     * retrieved without performing a new inference, i.e., all the heavy
     * computations have already been performed. Typically, in a junction tree
     * algorithm, this corresponds to all the messages sent in the JT being
     * already computed. */
    virtual bool isDone () const noexcept final;

    /// Returns a constant reference over the IBayesNet referenced by this class
    virtual const IBayesNet<GUM_SCALAR>& bn () const noexcept final;

    /// prepare the internal inference structures for the next inference
    virtual void prepareInference () final;

    /// perform the heavy computations needed to compute the targets' posteriors
    /** In a Junction tree propagation scheme, for instance, the heavy
     * computations are those of the messages sent in the JT. This is precisely
     * what makeInference should compute. Later, the computations of the
     * posteriors can be done "lightly" by multiplying and projecting those
     * messages. */
    virtual void makeInference () final;

    /// @}

    
    // ############################################################################
    /// @name Targets
    // ############################################################################
    /// @{

    /// Clear all previously defined targets (single targets and sets of targets)
    /**
     * Clear all previously defined targets. As a result, every node is
     * considered as a potential target (empty target sets = all nodes are
     * targets).
     * This is the default value for the targets manager.
     */
    virtual void clearTargets () final;

    /// Add a single target to the list of targets
    /**
     * @throw UndefinedElement if target is not a NodeId in the Bayes net
     */
    virtual void addTarget ( const NodeId target ) final;

    /// removes an existing target
    /** @warning If the target does not already exist, the method does nothing.
     * In particular, it does not raise any exception. */
    virtual void eraseTarget ( const NodeId target ) final;

    /// return true if variable is a target
    virtual bool isTarget( const NodeId variable ) const final;

    /// returns the list of single targets
    /**
     * If the NodeSet is empty, then every node is a potential target.
     */
    virtual const NodeSet& targets() const noexcept final;

    /// Add a set of nodes as a new target
    /**
     * @throw UndefinedElement if some node(s) do not belong to the Bayes net
     */
    virtual void addSetTarget ( const NodeSet& target ) final;
    
    /// removes an existing set target
    /** @warning If the set target does not already exist, the method does
     * nothing. In particular, it does not raise any exception. */
    virtual void eraseSetTarget ( const NodeSet& target ) final;

    /// return true if target is a nodeset target.
    virtual bool isSetTarget ( const NodeSet& target ) const final;

    /// returns the list of target sets
    virtual const Set<NodeSet>& setTargets() const noexcept final;

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
    virtual void addEvidence( NodeId id,
                              Idx val ) final;

    /// adds a new evidence on node id (might be soft or hard)
    /**
     * @throw UndefinedElement if id does not belong to the Bayesian network
     * @throw InvalidArgument if id already has an evidence
     * @throw FatalError if vals=[0,0,...,0]
     * @throw InvalidArgument if the size of vals is different from the domain
     *        size of node id
     */
    virtual void addEvidence( const NodeId id,
                              const std::vector<GUM_SCALAR>& vals ) final;

    /// adds a new evidence on node id (might be soft or hard)
    /**
     * @throw UndefinedElement if the potential is defined over several nodes
     * @throw UndefinedElement if the node on which the potential is defined
     * does not belong to the Bayesian network
     * @throw InvalidArgument if the node of the potential already has an evidence
     * @throw FatalError if pot=[0,0,...,0]
     */
    virtual void addEvidence( const Potential<GUM_SCALAR>& pot ) final;

    /// adds a new evidence on node id (might be soft or hard)
    /**
     * @throw UndefinedElement if the potential is defined over several nodes
     * @throw UndefinedElement if the node on which the potential is defined
     * does not belong to the Bayesian network
     * @throw InvalidArgument if the node of the potential already has an evidence
     * @throw FatalError if pot=[0,0,...,0]
     */
    virtual void addEvidence( Potential<GUM_SCALAR>&& pot ) final;

    /// change the value of an already existing hard evidence
    /**
     * @throw UndefinedElement if id does not belong to the Bayesian network
     * @throw InvalidArgument if val is not a value for id
     * @throw InvalidArgument if id does not already have an evidence
     */
    virtual void chgEvidence( NodeId id,
                              Idx val ) final;

    /// change the value of an already existing evidence (might be soft or hard)
    /**
     * @throw UndefinedElement if id does not belong to the Bayesian network
     * @throw InvalidArgument if id does not already have an evidence
     * @throw FatalError if vals=[0,0,...,0]
     * @throw InvalidArgument if the size of vals is different from the domain
     *        size of node id
     */
    virtual void chgEvidence( NodeId id,
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

    /// indicates whether some node(s) have received evidence
    virtual bool hasEvidence () const final;

    /// indicates whether node id has received an evidence
    virtual bool hasEvidence( NodeId id ) const final;

    /// indicates whether node id has received a hard evidence
    virtual bool hasHardEvidence( NodeId id ) const final;

    /// indicates whether node id has received a soft evidence
    virtual bool hasSoftEvidence( NodeId id ) const final;

    /// returns the number of evidence entered into the Bayesian network
    virtual Size nbrEvidence() const final;

    /// returns the number of hard evidence entered into the Bayesian network
    virtual Size nbrHardEvidence() const final;

    /// returns the number of soft evidence entered into the Bayesian network
    virtual Size nbrSoftEvidence() const final;

    /// removes all the evidence entered into the network
    virtual void clearEvidence() final;

    /// removed the evidence, if any, corresponding to node id
    virtual void clearEvidence( NodeId id ) final;
    
    /// @}


    
  protected:
    /// the Bayes net on which we perform inferences
    const IBayesNet<GUM_SCALAR>& _bn;

    
    /**
     * _prepareInference is called when the bn, the targets and soft/hard
     * evidence are known. Note that the values of evidence are not necessarily
     * known and can be changed between _prepare and _makeInference.
     */
    virtual void _prepareInference() = 0;

    /// fired when a new evidence is inserted
    virtual void _onEvidenceAdded ( const NodeId id,
                                    bool isHardEvidence ) = 0;

    /// fired when an evidence is removed
    virtual void _onEvidenceErased ( const NodeId id,
                                     bool isHardEvidence ) = 0;

    /// fired when all the evidence are erased
    virtual void _onAllEvidenceErased () = 0;
    
    /** @brief fired when an evidence is changed, in particular when its status
     * (soft/hard) changes
     *
     * @param nodeId the node of the changed evidence
     * @param hasChangedSoftHard true if the evidence has changed from Soft to
     * Hard or from Hard to Soft
     *
     */
    virtual void _onEvidenceChanged( const NodeId id,
                                     bool hasChangedSoftHard ) = 0;

    /**
     * _makeInference() is called when the inference has to be performed
     * effectively. Once the inference is done, _fillPosterior can be called.
     */
    virtual void _makeInference() = 0;

    /**
     *
     * This method is called when a BayesNetInference user asks for the posterior
     * of a given target.
     *
     * The reference "posterior" is a reference over a const Potential that
     * contains the variable of node id (only values can then be changed)
     *
     * @param id The variable's id.
     * @param posterior The completely empty potential to fill.
     * @throw UndefinedElement Raised if no variable matches id in the target.
     */
    virtual void _fillPosterior( NodeId id,
                                 Potential<GUM_SCALAR>& posterior ) = 0;

    /// const access to evidence
    const NodeProperty<const Potential<GUM_SCALAR>*>& _evidence() const;

    /// const access to hard evidence
    const NodeProperty<Idx>& _hardEvidence() const;



    
  private:
    /// the current state of the inference (unprepared/ready/done)
    StateOfInference __state { StateOfInference::UnpreparedStructure };

    /// the set of single posteriors computed during the last inference
    NodeProperty<const Potential<GUM_SCALAR>*> __target_posteriors;

    /// the set of set target posteriors computed during the last inference
    HashTable<NodeSet, const Potential<GUM_SCALAR>*> __settarget_posteriors;

    /// the set of single targets
    NodeSet __targets;

    /// the set of set targets (joint targets)
    Set<NodeSet> __settargets;

    /// the set of evidence entered into the network
    NodeProperty<const Potential<GUM_SCALAR>*> __evidence;

    /// assign to each node with a hard evidence the index of its observed value
    NodeProperty<Idx> __hard_evidence;

    /// the set of nodes that received soft evidence
    NodeSet __soft_evidence_nodes;

    /// the set of nodes that received hard evidence
    NodeSet __hard_evidence_nodes;



    /// create the internal structure for a hard evidence
    Potential<GUM_SCALAR> __createHardEvidence( NodeId id,
                                                Idx val ) const;

    /// checks whether a potential corresponds to a hard evidence or not
    bool __isHardEvidence( const Potential<GUM_SCALAR>& pot, Idx& val ) const;

    /// remove all the posteriors computed (single and set targets)
    void __invalidatePosteriors() noexcept;


  };


}  // namespace gum


#include <agrum/BN/inference/inference.tcc>


#endif  // GUM_INFERENCE_H
