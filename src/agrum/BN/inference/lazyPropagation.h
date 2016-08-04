/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES et Pierre-Henri WUILLEMIN   *
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
 * @brief Implementation of a Shafer-Shenoy's-like version of lazy propagation
 * for inference in Bayesian Networks.
 */
#ifndef GUM_LAZY_PROPAGATION_H
#define GUM_LAZY_PROPAGATION_H

#include <cmath>

#include <agrum/BN/inference/barrenNodesFinder.h>
#include <agrum/BN/inference/inference.h>
#include <agrum/graphs/triangulations/defaultTriangulation.h>

namespace gum {

  /** @brief type of algorithm for determining the relevant potentials for
   * combinations using some d-separation analysis
   *
   * When constructing messages from one clique to its neighbor, we can
   * exploit d-separation to determine that some potentials are irrelevant for
   * the message computation. So we can discard them and, thereby, speed-up
   * the computations.
   */
  enum FindRelevantPotentialsType {
    FIND_RELEVANT_ALL,            // do not perform d-separation analysis
    FIND_RELEVANT_D_SEPARATION,   // BayesBall requisite nodes -> potentials
    FIND_RELEVANT_D_SEPARATION2,  // BayesBall requisite potentials (directly)
    FIND_RELEVANT_D_SEPARATION3   // Koller & Friedman 2009 requisite
                                  // potentials
  };

  
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
  enum FindBarrenNodesType {
    FIND_NO_BARREN_NODES,  // do not try to find barren nodes
    FIND_BARREN_NODES      // use a bottom-up algorithm to detect barren nodes
  };



  /**
   * @class LazyPropagation lazyPropagation.h
   * <agrum/BN/inference/lazyPropagation.h>
   * @brief Implementation of a Shafer-Shenoy's-like version of lazy
   * propagation for inference in Bayesian Networks
   * @ingroup bn_inference
   */
  template <typename GUM_SCALAR>
  class LazyPropagation : public Inference<GUM_SCALAR> {
    public:

    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    LazyPropagation ( const IBayesNet<GUM_SCALAR>* BN,
                      FindRelevantPotentialsType =
                      FindRelevantPotentialsType::FIND_RELEVANT_D_SEPARATION2,
                      FindBarrenNodesType = FIND_BARREN_NODES,
                      bool use_binary_join_tree = true );

    /// destructor
    ~LazyPropagation ();

    /// @}
    

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// use a new triangulation algorithm
    void setTriangulation ( const Triangulation& new_triangulation );

    /// sets how we determine the relevant potentials to combine
    /** When a clique sends a message to a separator, it first constitute the
     * set of the potentials it contains and of the potentials contained in the
     * messages it received. If FindRelevantPotentialsType = FIND_RELEVANT_ALL,
     * all these potentials are combined and projected to produce the message
     * sent to the separator.
     * If FindRelevantPotentialsType = FIND_RELEVANT_D_SEPARATION, then only the
     * set of potentials d-connected to the variables of the separator are kept
     * for combination and projection. */
    void setFindRelevantPotentialsType( FindRelevantPotentialsType type );

    /// sets how we determine barren nodes
    /** Barren nodes are unnecessary for probability inference, so they can
     * be safely discarded in this case (type = FIND_BARREN_NODES). This
     * speeds-up inference. However, there are some cases in which we do not
     * want to remove barren nodes, typically when we want to answer queries
     * such as Most Probable Explanations (MPE). */
    void setFindBarrenNodesType ( FindBarrenNodesType type );

    /// sets the operator for performing the projections
    void setProjectionFunction ( Potential<GUM_SCALAR>* (* proj)
                                 ( const Potential<GUM_SCALAR>&,
                                   const Set<const DiscreteVariable*>& ) );

    /// sets the operator for performing the combinations
    void setCombinationFunction ( Potential<GUM_SCALAR>* (* comb)
                                  ( const Potential<GUM_SCALAR>&,
                                    const Potential<GUM_SCALAR>& ) );

    /// returns the current join tree used
    /** Lazy Propagation does not use a junction tree but a binary join tree
     * because this may enable faster inferences. So do not be surprised to
     * see that somes cliques are contained into others in this tree. */
    const JoinTree* joinTree() const;

    /// returns the probability of evidence
    GUM_SCALAR evidenceProbability();

    /// @}

    
    // ############################################################################
    /// @name Information Theory related functions
    // ############################################################################
    /// @{

    /** Entropy
     * Compute Shanon's entropy of a node given the observation
     * @see http://en.wikipedia.org/wiki/Information_entropy
     */
    GUM_SCALAR H( NodeId X );

    /** Mutual information between X and Y
     * @see http://en.wikipedia.org/wiki/Mutual_information
     *
     * @warning Due to limitation of @ref joint, may not be able to compute
     * this value
     * @throw OperationNotAllowed in these cases
     */
    GUM_SCALAR I( NodeId X, NodeId Y );

    /** Variation of information between X and Y
     * @see http://en.wikipedia.org/wiki/Variation_of_information
     *
     * @warning Due to limitation of @ref joint, may not be able to compute
     * this value
     * @throw OperationNotAllowed in these cases
     */
    GUM_SCALAR VI( NodeId X, NodeId Y );
    
    /// @}
    
    
  protected:
    /// fired after a new evidence is inserted
    virtual void _onEvidenceAdded ( const NodeId id,
                                    bool isHardEvidence );

    /// fired before an evidence is removed
    virtual void _onEvidenceErased ( const NodeId id,
                                     bool isHardEvidence );

    /// fired before all the evidence are erased
    virtual void _onAllEvidenceErased ( bool contains_hard_evidence );
    
    /** @brief fired after an evidence is changed, in particular when its status
     * (soft/hard) changes
     *
     * @param nodeId the node of the changed evidence
     * @param hasChangedSoftHard true if the evidence has changed from Soft to
     * Hard or from Hard to Soft
     */
    virtual void _onEvidenceChanged( const NodeId id,
                                     bool hasChangedSoftHard );

    /// prepares inference when the latter is in UnpreparedStructure state
    /** Note that the values of evidence are not necessarily
     * known and can be changed between _prepareInferenceStructure and
     * _makeInference. */
    virtual void _prepareInferenceStructure ();

    /// prepares inference when the latter is in OutdatedPotentials state
    /** Note that the values of evidence are not necessarily
     * known and can be changed between _prepareInferenceStructure and
     * _makeInference. */
    virtual void _updateInferencePotentials ();
    
    /// called when the inference has to be performed effectively
    /** Once the inference is done, _fillPosterior can be called. */
    virtual void _makeInference();

    /** @brief This method is called when a BayesNetInference user asks for
     * the posterior of a given target.
     *
     * The reference "posterior" is a reference over a const Potential that
     * contains the variable of node id (only values can then be changed)
     *
     * @param id The variable's id.
     * @param posterior The completely empty potential to fill.
     * @throw UndefinedElement Raised if no variable matches id in the target.
     */
    virtual void _fillPosterior( const NodeId id,
                                 Potential<GUM_SCALAR>& posterior );

    /** @brief This method is called when a BayesNetInference user asks for
     * the posterior of a given set target.
     *
     * The reference "posterior" is a reference over a Potential that
     * contains all the variables of the set target (only values can then
     * be changed)
     *
     * @param set the set of nodes of the potential
     * @param posterior The completely empty potential to fill.
     * @throw UndefinedElement Raised if no variable matches id in the target.
     */
    virtual void _fillSetPosterior( const NodeSet& set,
                                    Potential<GUM_SCALAR>& posterior );

    

  private:
    typedef Set<const Potential<GUM_SCALAR>*> __PotentialSet;
    typedef SetIteratorSafe<const Potential<GUM_SCALAR>*>
        __PotentialSetIterator;
    
    
    /// the type of relevant potential finding algorithm to be used
    FindRelevantPotentialsType __find_relevant_potential_type;
    
    /** @brief update a set of potentials: the remaining are those to be
     * combined to produce a message on a separator */
    void ( LazyPropagation<GUM_SCALAR>::*__findRelevantPotentials )
    ( __PotentialSet& pot_list, Set<const DiscreteVariable*>& kept_vars );

    /// the type of barren nodes computation we wish
    FindBarrenNodesType __barren_nodes_type;

    /// the operator for performing the projections
    Potential<GUM_SCALAR>* (* __projection_op )
      ( const Potential<GUM_SCALAR>&,
        const Set<const DiscreteVariable*>& );
    
    /// the operator for performing the combinations
    Potential<GUM_SCALAR>* (* __combination_op )
      ( const Potential<GUM_SCALAR>&,
        const Potential<GUM_SCALAR>& );
    
    /// the triangulation class creating the junction tree used for inference
    Triangulation* __triangulation;

    /** @brief indicates whether we should transform junction trees into
     * binary join trees */
    bool __use_binary_join_tree { true };

    /// the undigraph extracted from the BN and used to construct the join tree
    /** If all nodes are targets, this graph corresponds to the moral graph
     * of the BN. Otherwise, it may be a subgraph of this moral graph. For
     * instance if the BN is A->B->C and only B is a target, __graph will be
     * equal to A-B if we exploit barren nodes (C is a barren node and,
     * therefore, can be removed for inference). */
    UndiGraph __graph;

    /// the junction tree used to answer the last inference query
    JoinTree* __JT { nullptr };

    /// indicates whether a new join tree is needed for the next inference
    /** when modifying the set of hard evidence, we can determine that
     * the current JT is no more appropriate for inference. This variable
     * enables us to keep track of this. */
    bool __is_new_jt_needed { true };

    /// a clique node used as a root in each connected component of __JT
    /** For usual probabilistic inference, roots is useless. This is useful
     * when computing the probability of evidence. In this case, we need to
     * compute this probability in every connected component and multiply
     * them to get the overall probability of evidence.
     * @warning __roots should be computed only when evidenceProbability
     * is called. */
    NodeSet __roots;

    /// for each node of __graph (~ in the Bayes net), associate an ID in the JT
    HashTable<NodeId, NodeId> __node_to_clique;

    /// for each set target, assign a clique in the JT that contains it
    HashTable<NodeSet, NodeId> __settarget_to_clique;

    /// the list of all potentials stored in the cliques
    /** This structure contains a list for each clique in the join tree. If
     * a clique did not received any potential, then its list is empty but
     * the entry for the clique does exist. Note that clique potentials
     * contain also soft evidence and the CPTs that were projected to
     * remove their variables that received hard evidence. */
    NodeProperty<__PotentialSet> __clique_potentials;

    /// the list of all potentials stored in the separators after inferences
    /** This structure contains all the arcs of the join tree (edges in both
     * directions) whether the arc received any potential or not. */
    ArcProperty<__PotentialSet> __separator_potentials;

    /// the set of potentials created for the last inference messages
    /** This structure contains only the arcs on which potentials have
     * been created */
    ArcProperty<__PotentialSet> __created_potentials;

    /** @brief the constants resulting from the projections of CPTs defined
     * over only hard evidence nodes
     * @TODO remove this constant and insert the notion of a constant into
     * potentials/multidim arrays */
    NodeProperty<GUM_SCALAR> __constants;
   
    /// indicates whether a message (from one clique to another) has been computed
    /** Here, all the messages, computed or not, are put into the property, only
     * the Boolean makes the difference between messages computed and those that
     * were not computed */
    ArcProperty<bool> __messages_computed;

    /// the soft evidence stored in the cliques per their assigned node in the BN
    /** This variable is useful for method _updateInferencePotentials: it enables
     * to know which soft evidence should be removed/added into the cliques of the
     * join tree.
     * @warning These potentials are not owned by LazyPropagation, they are only
     * referenced by it. Only the cliques that contain evidence are
     * filled in this structure. */
    NodeProperty<const Potential<GUM_SCALAR>*> __node_to_soft_evidence;

    /// the CPTs that were projected due to hard evidence nodes
    /** For each node whose CPT is defined over some nodes that contain some
     * hard evidence, assigns a new projected CPT that does not contain
     * these nodes anymore.
     * @warning These potentials are owned by LayPropagation. */
    NodeProperty<const Potential<GUM_SCALAR>*> __hard_ev_projected_CPTs;

    /// the hard evidence nodes which were projected in CPTs
    NodeSet __hard_ev_nodes;

    /// the possible types of evidence changes 
    enum EvidenceChangeType {
      EVIDENCE_ADDED,
      EVIDENCE_ERASED,
      EVIDENCE_MODIFIED
    };
    
    /** @brief indicates which nodes of the BN have evidence that changed
     * since the last inference */
    NodeProperty<EvidenceChangeType> __evidence_changes;



    /// check whether a new join tree is really needed for the next inference
    bool __isNewJTNeeded () const;

    /// create a new junction tree as well as its related data structures
    void __createNewJT ();

    /// invalidate all the messages sent from a given clique
    void __diffuseMessageInvalidations ( const NodeId from,
                                         const NodeId to );

    /// compute a root for each connected component of __JT
    void __computeJoinTreeRoots ();

    /** @brief update a set of potentials: the remaining are those to be
     * combined
     * to produce a message on a separator */
    void __findRelevantPotentialsWithdSeparation(
        __PotentialSet& pot_list, Set<const DiscreteVariable*>& kept_vars );

    /** @brief update a set of potentials: the remaining are those to be
     * combined
     * to produce a message on a separator */
    void __findRelevantPotentialsWithdSeparation2(
        __PotentialSet& pot_list, Set<const DiscreteVariable*>& kept_vars );

    /** @brief update a set of potentials: the remaining are those to be
     * combined
     * to produce a message on a separator */
    void __findRelevantPotentialsWithdSeparation3(
        __PotentialSet& pot_list, Set<const DiscreteVariable*>& kept_vars );

    /** @brief update a set of potentials: the remaining are those to be
     * combined
     * to produce a message on a separator */
    void
    __findRelevantPotentialsGetAll( __PotentialSet& pot_list,
                                    Set<const DiscreteVariable*>& kept_vars );
    
    /** @brief removes variables del_vars from a list of potentials and
     * returns the resulting list */
    __PotentialSet __marginalizeOut( __PotentialSet& pot_list,
                                     Set<const DiscreteVariable*>& del_vars,
                                     Set<const DiscreteVariable*>& kept_vars );

    /// creates the message sent by clique from_id to clique to_id
    void __produceMessage( const NodeId from_id,
                           const NodeId to_id );

    /// actually perform the collect phase
    void __collectMessage( const NodeId id,
                           const NodeId from );

    /// computes P(1st arg,evidence) and store the result into the second arg
    void __aPosterioriMarginal( const NodeId id,
                                Potential<GUM_SCALAR>& posterior );
    
    /// computes P(1st arg,evidence) and store the result into the second arg
    void __aPosterioriSetMarginal ( const NodeSet& ids,
                                    Potential<GUM_SCALAR>& posterior );


    
    /// avoid copy constructors
    LazyPropagation( const LazyPropagation<GUM_SCALAR>& );

    /// avoid copy operators
    LazyPropagation<GUM_SCALAR>&
    operator=( const LazyPropagation<GUM_SCALAR>& );
  };


  extern template class LazyPropagation<float>;
  extern template class LazyPropagation<double>;


} /* namespace gum */

#include <agrum/BN/inference/lazyPropagation.tcc>

#endif /* GUM_LAZY_PROPAGATION_H */
// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
