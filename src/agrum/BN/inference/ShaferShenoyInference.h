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
 * @brief Implementation of Shafer-Shenoy's algorithm for inference
 * in Bayesian Networks.
 */
#ifndef GUM_SHAFER_SHENOY_INFERENCE_H
#define GUM_SHAFER_SHENOY_INFERENCE_H

#include <cmath>
#include <utility>

#include <agrum/BN/inference/barrenNodesFinder.h>
#include <agrum/BN/inference/evidenceInference.h>
#include <agrum/BN/inference/jointTargetedInference.h>
#include <agrum/config.h>
#include <agrum/graphs/triangulations/defaultTriangulation.h>

namespace gum {


  // the function used to combine two tables
  template <typename GUM_SCALAR>
  INLINE static Potential<GUM_SCALAR>*
  SSNewmultiPotential( const Potential<GUM_SCALAR>& t1,
                       const Potential<GUM_SCALAR>& t2 ) {
    return new Potential<GUM_SCALAR>( t1 * t2 );
  }

  // the function used to combine two tables
  template <typename GUM_SCALAR>
  INLINE static Potential<GUM_SCALAR>*
  SSNewprojPotential( const Potential<GUM_SCALAR>&        t1,
                      const Set<const DiscreteVariable*>& del_vars ) {
    return new Potential<GUM_SCALAR>( t1.margSumOut( del_vars ) );
  }


  /**
   * @class ShaferShenoyInference ShaferShenoyInference.h
   * <agrum/BN/inference/ShaferShenoyInference.h>
   * @brief Implementation of Shafer-Shenoy's propagation algorithm
   * for inference in Bayesian Networks
   * @ingroup bn_inference
   */
  template <typename GUM_SCALAR>
  class ShaferShenoyInference : public JointTargetedInference<GUM_SCALAR>,
                                public EvidenceInference<GUM_SCALAR> {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    ShaferShenoyInference(
        const IBayesNet<GUM_SCALAR>* BN,
        FindBarrenNodesType barren_type = FindBarrenNodesType::FIND_BARREN_NODES,
        bool                use_binary_join_tree = true );

    /// destructor
    virtual ~ShaferShenoyInference();

    /// @}


    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// use a new triangulation algorithm
    void setTriangulation( const Triangulation& new_triangulation );

    /// sets how we determine barren nodes
    /** Barren nodes are unnecessary for probability inference, so they can
     * be safely discarded in this case (type = FIND_BARREN_NODES). This
     * speeds-up inference. However, there are some cases in which we do not
     * want to remove barren nodes, typically when we want to answer queries
     * such as Most Probable Explanations (MPE). */
    void setFindBarrenNodesType( FindBarrenNodesType type );

    /// returns the current join tree used
    /** Lazy Propagation does not use a junction tree but a binary join tree
     * because this may enable faster inferences. So do not be surprised to
     * see that somes cliques are contained into others in this tree. */
    const JoinTree* joinTree();

    /// returns the current junction tree
    /** Lazy Propagation does not use a junction tree but a binary join tree
     * because this may enable faster inferences. This method return the junction
     * tree, before optimizations
     **/
    const JunctionTree* junctionTree();

    /// returns the probability of evidence
    GUM_SCALAR evidenceProbability();

    /// @}


    protected:
    /// fired after a new evidence is inserted
    virtual void _onEvidenceAdded( const NodeId id, bool isHardEvidence );

    /// fired before an evidence is removed
    virtual void _onEvidenceErased( const NodeId id, bool isHardEvidence );

    /// fired before all the evidence are erased
    virtual void _onAllEvidenceErased( bool contains_hard_evidence );

    /** @brief fired after an evidence is changed, in particular when its status
     * (soft/hard) changes
     *
     * @param nodeId the node of the changed evidence
     * @param hasChangedSoftHard true if the evidence has changed from Soft to
     * Hard or from Hard to Soft
     */
    virtual void _onEvidenceChanged( const NodeId id, bool hasChangedSoftHard );

    /// fired after a new single target is inserted
    /** @param id The target variable's id. */
    virtual void _onMarginalTargetAdded( const NodeId id );

    /// fired before a single target is removed
    /** @param id The target variable's id. */
    virtual void _onMarginalTargetErased( const NodeId id );

    /// fired after a new joint target is inserted
    /** @param set The set of target variable's ids. */
    virtual void _onJointTargetAdded( const NodeSet& set );

    /// fired before a joint target is removed
    /** @param set The set of target variable's ids. */
    virtual void _onJointTargetErased( const NodeSet& set );

    /// fired after all the nodes of the BN are added as single targets
    virtual void _onAllMarginalTargetsAdded();

    /// fired before a all the single targets are removed
    virtual void _onAllMarginalTargetsErased();

    /// fired before a all the joint targets are removed
    virtual void _onAllJointTargetsErased();

    /// fired before a all single and joint_targets are removed
    virtual void _onAllTargetsErased();

    /// prepares inference when the latter is in OutdatedBNStructure state
    /** Note that the values of evidence are not necessarily
     * known and can be changed between _updateOutdatedBNStructure and
     * _makeInference. */
    virtual void _updateOutdatedBNStructure();

    /// prepares inference when the latter is in OutdatedBNPotentials state
    /** Note that the values of evidence are not necessarily
     * known and can be changed between _updateOutdatedBNStructure and
     * _makeInference. */
    virtual void _updateOutdatedBNPotentials();

    /// called when the inference has to be performed effectively
    /** Once the inference is done, _fillPosterior can be called. */
    virtual void _makeInference();


    /// returns the posterior of a given variable
    /** @param id The variable's id. */
    virtual const Potential<GUM_SCALAR>& _posterior( const NodeId id );

    /// returns the posterior of a declared target set
    /** @param set The set of ids of the variables whose joint posterior is
     * looked for. */
    virtual const Potential<GUM_SCALAR>& _jointPosterior( const NodeSet& set );

    /** @brief asks derived classes for the joint posterior of a set of
     * variables not declared as a joint target
     *
     * @param wanted_target The set of ids of the variables whose joint
     * posterior is looked for.
     * @param declared_target the joint target declared by the user that
     * contains set */
    virtual const Potential<GUM_SCALAR>&
    _jointPosterior( const NodeSet& wanted_target,
                     const NodeSet& declared_target );

    /// returns a fresh potential equal to P(argument,evidence)
    virtual Potential<GUM_SCALAR>* _unnormalizedJointPosterior( const NodeId id );

    /// returns a fresh potential equal to P(argument,evidence)
    virtual Potential<GUM_SCALAR>*
    _unnormalizedJointPosterior( const NodeSet& set );


    private:
    typedef Set<const Potential<GUM_SCALAR>*>             __PotentialSet;
    typedef SetIteratorSafe<const Potential<GUM_SCALAR>*> __PotentialSetIterator;


    /// the type of barren nodes computation we wish
    FindBarrenNodesType __barren_nodes_type;

    /// the operator for performing the projections
    Potential<GUM_SCALAR>* ( *__projection_op )(
        const Potential<GUM_SCALAR>&,
        const Set<const DiscreteVariable*>& ){SSNewprojPotential};

    /// the operator for performing the combinations
    Potential<GUM_SCALAR>* ( *__combination_op )( const Potential<GUM_SCALAR>&,
                                                  const Potential<GUM_SCALAR>& ){
        SSNewmultiPotential};

    /// the triangulation class creating the junction tree used for inference
    Triangulation* __triangulation;

    /** @brief indicates whether we should transform junction trees into
     * binary join trees */
    bool __use_binary_join_tree{true};

    /// the undigraph extracted from the BN and used to construct the join tree
    /** If all nodes are targets, this graph corresponds to the moral graph
     * of the BN. Otherwise, it may be a subgraph of this moral graph. For
     * instance if the BN is A->B->C and only B is a target, __graph will be
     * equal to A-B if we exploit barren nodes (C is a barren node and,
     * therefore, can be removed for inference). */
    UndiGraph __graph;

    /// the join (or junction) tree used to answer the last inference query
    JoinTree* __JT{nullptr};
    
    /// the junction tree to answer the last inference query
    JunctionTree* __junctionTree{nullptr};

    /// indicates whether a new join tree is needed for the next inference
    /** when modifying the set of hard evidence, we can determine that
     * the current JT is no more appropriate for inference. This variable
     * enables us to keep track of this. */
    bool __is_new_jt_needed{true};

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
    HashTable<NodeSet, NodeId> __joint_target_to_clique;

    /// the list of all potentials stored in the cliques
    /** This structure contains a list for each clique in the join tree. If
     * a clique did not received any potential, then its list is empty but
     * the entry for the clique does exist. Note that clique potentials
     * contain also soft evidence and the CPTs that were projected to
     * remove their variables that received hard evidence. The product of all
     * these potentials is precisely the potential stored into
     * __clique_ss_potential */
    NodeProperty<__PotentialSet> __clique_potentials;

    /// the potentials stored into the cliques by Shafer-Shenoy
    /** For a given clique, there is an entry in __clique_ss_potential if and
     * only if the clique received some potential(s). In this case, the
     * potential stored is the combination of all the corresponding list of
     * potentials in __clique_potentials. */
    NodeProperty<const Potential<GUM_SCALAR>*> __clique_ss_potential;

    /// the list of all potentials stored in the separators after inferences
    /** This structure contains all the arcs of the join tree (edges in both
     * directions) whether the arc received any potential or not. */
    ArcProperty<__PotentialSet> __separator_potentials;

    /// the set of potentials created for the last inference messages
    /** This structure contains only the arcs on which potentials have
     * been created.
     * @warning Note that the CPTs that were projected due to hard
     * evidence do not belong to this structure, they are kept in
     * __hard_ev_projected_CPTs. */
    ArcProperty<__PotentialSet> __created_potentials;

    /// the set of single posteriors computed during the last inference
    /** the posteriors are owned by ShaferShenoyInference. */
    NodeProperty<const Potential<GUM_SCALAR>*> __target_posteriors;

    /// the set of set target posteriors computed during the last inference
    /** the posteriors are owned by ShaferShenoyInference. */
    HashTable<NodeSet, const Potential<GUM_SCALAR>*> __joint_target_posteriors;

    /** @brief the constants resulting from the projections of CPTs defined
     * over only hard evidence nodes
     * @TODO remove this constant and insert the notion of a constant into
     * potentials/multidim arrays */
    NodeProperty<GUM_SCALAR> __constants;

    /// indicates whether a message (from one clique to another) has been
    /// computed
    /** Here, all the messages, computed or not, are put into the property, only
     * the Boolean makes the difference between messages computed and those that
     * were not computed */
    ArcProperty<bool> __messages_computed;

    /// the soft evidence stored in the cliques per their assigned node in the BN
    /** This variable is useful for method _updateOutdatedBNPotentials: it
     * enables to know which soft evidence should be removed/added into the
     * cliques of the join tree.
     * @warning These potentials are not owned by ShaferShenoyInference,
     * they are only referenced by it. Only the cliques that contain evidence
     * are filled in this structure. */
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
    enum EvidenceChangeType { EVIDENCE_ADDED, EVIDENCE_ERASED, EVIDENCE_MODIFIED };

    /** @brief indicates which nodes of the BN have evidence that changed
     * since the last inference */
    NodeProperty<EvidenceChangeType> __evidence_changes;

    /// for comparisons with 1 - epsilon
    const GUM_SCALAR __1_minus_epsilon{GUM_SCALAR( 1.0 - 1e-6 )};


    /// check whether a new join tree is really needed for the next inference
    bool __isNewJTNeeded() const;

    /// create a new junction tree as well as its related data structures
    void __createNewJT();
    /// sets the operator for performing the projections
    void __setProjectionFunction( Potential<GUM_SCALAR>* ( *proj )(
        const Potential<GUM_SCALAR>&, const Set<const DiscreteVariable*>&));

    /// sets the operator for performing the combinations
    void __setCombinationFunction( Potential<GUM_SCALAR>* ( *comb )(
        const Potential<GUM_SCALAR>&, const Potential<GUM_SCALAR>&));

    /// invalidate all the messages sent from a given clique
    void __diffuseMessageInvalidations( const NodeId from,
                                        const NodeId to,
                                        NodeSet&     cliques_invalidated );

    /// invalidate all messages, posteriors and created potentials
    void __invalidateAllMessages();

    /// compute a root for each connected component of __JT
    void __computeJoinTreeRoots();

    // remove barren variables and return the newly created projected potentials
    __PotentialSet
    __removeBarrenVariables( __PotentialSet&               pot_list,
                             Set<const DiscreteVariable*>& del_vars );

    /** @brief removes variables del_vars from a list of potentials and
     * returns the resulting list */
    __PotentialSet __marginalizeOut( __PotentialSet                pot_list,
                                     Set<const DiscreteVariable*>& del_vars,
                                     Set<const DiscreteVariable*>& kept_vars );

    /// creates the message sent by clique from_id to clique to_id
    void __produceMessage( const NodeId from_id, const NodeId to_id );

    /// actually perform the collect phase
    void __collectMessage( const NodeId id, const NodeId from );

    /// avoid copy constructors
    ShaferShenoyInference( const ShaferShenoyInference<GUM_SCALAR>& );

    /// avoid copy operators
    ShaferShenoyInference<GUM_SCALAR>&
    operator=( const ShaferShenoyInference<GUM_SCALAR>& );
  };


  extern template class ShaferShenoyInference<float>;
  extern template class ShaferShenoyInference<double>;


} /* namespace gum */

#include <agrum/BN/inference/ShaferShenoyInference_tpl.h>

#endif /* SHAFER_SHENOY_INFERENCE_H */
// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
