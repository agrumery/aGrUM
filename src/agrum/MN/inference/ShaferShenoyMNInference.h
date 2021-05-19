/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief Implementation of Shafer-Shenoy's algorithm for inference
 * in Markov Networks.
 */
#ifndef GUM_SHAFER_SHENOY_MN_INFERENCE_H
#define GUM_SHAFER_SHENOY_MN_INFERENCE_H

#include <utility>

#include <agrum/agrum.h>
#include <agrum/tools/core/math/math_utils.h>
#include <agrum/MN/inference/tools/evidenceMNInference.h>
#include <agrum/MN/inference/tools/jointTargetedMNInference.h>
#include <agrum/tools/graphs/algorithms/triangulations/defaultTriangulation.h>

namespace gum {


  // the function used to combine two tables
  template < typename GUM_SCALAR >
  INLINE static Potential< GUM_SCALAR >*
     SSNewMNmultiPotential(const Potential< GUM_SCALAR >& t1,
                           const Potential< GUM_SCALAR >& t2) {
    return new Potential< GUM_SCALAR >(t1 * t2);
  }

  // the function used to combine two tables
  template < typename GUM_SCALAR >
  INLINE static Potential< GUM_SCALAR >*
     SSNewMNprojPotential(const Potential< GUM_SCALAR >&        t1,
                          const Set< const DiscreteVariable* >& del_vars) {
    return new Potential< GUM_SCALAR >(t1.margSumOut(del_vars));
  }


  /**
   * @class ShaferShenoyMNInference ShaferShenoyMNInference.h
   * <agrum/MN/inference/ShaferShenoyMNInference.h>
   * @brief Implementation of Shafer-Shenoy's propagation algorithm
   * for inference in Markov Networks
   * @ingroup mn_inference
   */
  template < typename GUM_SCALAR >
  class ShaferShenoyMNInference:
      public JointTargetedMNInference< GUM_SCALAR >,
      public EvidenceMNInference< GUM_SCALAR > {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    explicit ShaferShenoyMNInference(const IMarkovNet< GUM_SCALAR >* MN,
                                     bool use_binary_join_tree = true);

    /// destructor
    ~ShaferShenoyMNInference() final;

    /// @}


    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// use a new triangulation algorithm
    void setTriangulation(const Triangulation& new_triangulation);

    /// returns the current join tree used
    const JoinTree* joinTree();

    /// returns the current junction tree
    const JunctionTree* junctionTree();

    /// returns the probability of evidence
    GUM_SCALAR evidenceProbability();

    /// @}


    protected:
    /// check if the vars form a possible computable joint (can be redefined by
    /// subclass)
    virtual bool    isExactJointComputable_(const NodeSet& vars) final;
    virtual NodeSet superForJointComputable_(const NodeSet& vars) final;

    /// fired when the stage is changed
    void onStateChanged_() final{};

    /// fired after a new evidence is inserted
    void onEvidenceAdded_(const NodeId id, bool isHardEvidence) final;

    /// fired before an evidence is removed
    void onEvidenceErased_(const NodeId id, bool isHardEvidence) final;

    /// fired before all the evidence are erased
    void onAllEvidenceErased_(bool contains_hard_evidence) final;

    /** @brief fired after an evidence is changed, in particular when its status
     * (soft/hard) changes
     *
     * @param nodeId the node of the changed evidence
     * @param hasChangedSoftHard true if the evidence has changed from Soft to
     * Hard or from Hard to Soft
     */
    void onEvidenceChanged_(const NodeId id, bool hasChangedSoftHard) final;

    /// fired after a new single target is inserted
    /** @param id The target variable's id. */
    void onMarginalTargetAdded_(const NodeId id) final;

    /// fired before a single target is removed
    /** @param id The target variable's id. */
    void onMarginalTargetErased_(const NodeId id) final;

    /// fired after a new Markov net has been assigned to the engine
    virtual void onMarkovNetChanged_(const IMarkovNet< GUM_SCALAR >* mn) final;

    /// fired after a new joint target is inserted
    /** @param set The set of target variable's ids. */
    void onJointTargetAdded_(const NodeSet& set) final;

    /// fired before a joint target is removed
    /** @param set The set of target variable's ids. */
    void onJointTargetErased_(const NodeSet& set) final;

    /// fired after all the nodes of the MN are added as single targets
    void onAllMarginalTargetsAdded_() final;

    /// fired before a all the single targets are removed
    void onAllMarginalTargetsErased_() final;

    /// fired before a all the joint targets are removed
    void onAllJointTargetsErased_() final;

    /// fired before a all single and joint_targets are removed
    void onAllTargetsErased_() final;

    /// prepares inference when the latter is in OutdatedStructure state
    /** Note that the values of evidence are not necessarily
     * known and can be changed between updateOutdatedStructure_ and
     * makeMNInference_. */
    void updateOutdatedStructure_() final;

    /// prepares inference when the latter is in OutdatedPotentials state
    /** Note that the values of evidence are not necessarily
     * known and can be changed between updateOutdatedStructure_ and
     * makeMNInference_. */
    void updateOutdatedPotentials_() final;

    /// called when the inference has to be performed effectively
    /** Once the inference is done, fillPosterior_ can be called. */
    void makeInference_() final;


    /// returns the posterior of a given variable
    /** @param id The variable's id. */
    const Potential< GUM_SCALAR >& posterior_(NodeId id) final;

    /// returns the posterior of a declared target set
    /** @param set The set of ids of the variables whose joint posterior is
     * looked for. */
    const Potential< GUM_SCALAR >& jointPosterior_(const NodeSet& set) final;

    /** @brief asks derived classes for the joint posterior of a set of
     * variables not declared as a joint target
     *
     * @param wanted_target The set of ids of the variables whose joint
     * posterior is looked for.
     * @param declared_target the joint target declared by the user that
     * contains set */
    const Potential< GUM_SCALAR >&
       jointPosterior_(const NodeSet& wanted_target,
                       const NodeSet& declared_target) final;

    /// returns a fresh potential equal to P(argument,evidence)
    Potential< GUM_SCALAR >* unnormalizedJointPosterior_(NodeId id) final;

    /// returns a fresh potential equal to P(argument,evidence)
    Potential< GUM_SCALAR >* unnormalizedJointPosterior_(const NodeSet& set) final;


    private:
    typedef Set< const Potential< GUM_SCALAR >* > PotentialSet__;
    typedef SetIteratorSafe< const Potential< GUM_SCALAR >* >
       PotentialSetIterator__;

    /// the operator for performing the projections
    Potential< GUM_SCALAR >* (*projection_op__)(
       const Potential< GUM_SCALAR >&,
       const Set< const DiscreteVariable* >&){SSNewMNprojPotential};

    /// the operator for performing the combinations
    Potential< GUM_SCALAR >* (*combination_op__)(const Potential< GUM_SCALAR >&,
                                                 const Potential< GUM_SCALAR >&){
       SSNewMNmultiPotential};

    /// the triangulation class creating the junction tree used for inference
    Triangulation* triangulation__;

    /** @brief indicates whether we should transform junction trees into
     * binary join trees */
    bool use_binary_join_tree__{true};

    /// the undigraph extracted from the MN and used to construct the join tree
    /** If all nodes are targets, this graph corresponds to the graph
     * of the MN. Otherwise, it may be a subgraph of this moral graph. */
    UndiGraph reduced_graph__;

    /// the join (or junction) tree used to answer the last inference query
    JoinTree* propagator__{nullptr};

    /// the junction tree to answer the last inference query
    JunctionTree* junctionTree__{nullptr};

    /// indicates whether a new join tree is needed for the next inference
    /** when modifying the set of hard evidence, we can determine that
     * the current JT is no more appropriate for inference. This variable
     * enables us to keep track of this. */
    bool is_new_jt_needed__{true};

    /// a clique node used as a root in each connected component of propagator__
    /** For usual probabilistic inference, roots is useless. This is useful
     * when computing the probability of evidence. In this case, we need to
     * compute this probability in every connected component and multiply
     * them to get the overall probability of evidence.
     * @warning roots__ should be computed only when evidenceProbability
     * is called. */
    NodeSet roots__;

    /// for each node of reduced_graph__ (~ in the Markov net), associate an ID in
    /// the JT
    HashTable< NodeSet, NodeId > factor_to_clique__;

    NodeProperty< NodeId > node_to_clique__;

    /// for each joint target, assign a clique in the JT that contains it
    HashTable< NodeSet, NodeId > joint_target_to_clique__;

    /// the list of all potentials stored in the cliques
    /** This structure contains a list for each clique in the join tree. If
     * a clique did not received any potential, then its list is empty but
     * the entry for the clique does exist. Note that clique potentials
     * contain also soft evidence and the factors that were projected to
     * remove their variables that received hard evidence. The product of all
     * these potentials is precisely the potential stored into
     * clique_potentials__ */
    NodeProperty< PotentialSet__ > clique_potentials_list__;

    /// the potentials stored into the cliques by Shafer-Shenoy
    /** For a given clique, there is an entry in clique_potentials__ if and
     * only if the clique received some potential(s). In this case, the
     * potential stored is the combination of all the corresponding list of
     * potentials in clique_potentials_list__. */
    NodeProperty< const Potential< GUM_SCALAR >* > clique_potentials__;

    /// the list of all potentials stored in the separators after inferences
    /** This structure contains all the arcs of the join tree (edges in both
     * directions) whether the arc received any potential or not. */
    ArcProperty< PotentialSet__ > separator_potentials__;

    /// the set of potentials created for the last inference messages
    /** This structure contains only the arcs on which potentials have
     * been created.
     * @warning Note that the factors that were projected due to hard
     * evidence do not belong to this structure, they are kept in
     * hard_ev_projected_factors__. */
    ArcProperty< PotentialSet__ > created_messages__;

    /// the set of single posteriors computed during the last inference
    /** the posteriors are owned by ShaferShenoyMNInference. */
    NodeProperty< const Potential< GUM_SCALAR >* > target_posteriors__;

    /// the set of set target posteriors computed during the last inference
    /** the posteriors are owned by ShaferShenoyMNInference. */
    HashTable< NodeSet, const Potential< GUM_SCALAR >* > joint_target_posteriors__;

    /** @brief the constants resulting from the projections of CPTs defined
     * over only hard evidence nodes
     * @TODO remove this constant and insert the notion of a constant into
     * potentials/multidim arrays */
    // NodeProperty< GUM_SCALAR > constants__;

    /// indicates whether a message (from one clique to another) has been
    /// computed
    /** Here, all the messages, computed or not, are put into the property, only
     * the Boolean makes the difference between messages computed and those that
     * were not computed */
    ArcProperty< bool > messages_computed__;

    /// the soft evidence stored in the cliques per their assigned node in the MN
    /** This variable is useful for method updateOutdatedPotentials_: it
     * enables to know which soft evidence should be removed/added into the
     * cliques of the join tree.
     * @warning These potentials are not owned by ShaferShenoyMNInference,
     * they are only referenced by it. Only the cliques that contain evidence
     * are filled in this structure. */
    NodeProperty< const Potential< GUM_SCALAR >* > node_to_soft_evidence__;

    /// the factors that were projected due to hard evidence nodes
    /** For each factor containing the nodes that contain some
     * hard evidence, assigns a new projected factor that does not contain
     * these nodes anymore.
     * @warning These potentials are owned by the inference class. */
    HashTable< NodeSet, const Potential< GUM_SCALAR >* >
       hard_ev_projected_factors__;

    /// the hard evidence nodes which were projected in factors
    NodeSet hard_ev_nodes__;

    /// the possible types of evidence changes
    enum EvidenceChangeType
    {
      EVIDENCE_ADDED,
      EVIDENCE_ERASED,
      EVIDENCE_MODIFIED
    };

    /** @brief indicates which nodes of the MN have evidence that changed
     * since the last inference */
    NodeProperty< EvidenceChangeType > evidence_changes__;

    /// for comparisons with 1 - epsilon
    const GUM_SCALAR one_minus_epsilon__{GUM_SCALAR(1.0 - 1e-6)};


    /// check whether a new join tree is really needed for the next inference
    bool isNewJTNeeded__() const;

    /// create a new junction tree as well as its related data structures
    void createNewJT__();

    /// sets the operator for performing the projections
    void setProjectionFunction__(
       Potential< GUM_SCALAR >* (*proj)(const Potential< GUM_SCALAR >&,
                                        const Set< const DiscreteVariable* >&));

    /// sets the operator for performing the combinations
    void setCombinationFunction__(Potential< GUM_SCALAR >* (
       *comb)(const Potential< GUM_SCALAR >&, const Potential< GUM_SCALAR >&));

    /// invalidate all the messages sent from a given clique
    void diffuseMessageInvalidations__(NodeId   from,
                                       NodeId   to,
                                       NodeSet& cliques_invalidated);

    /// invalidate all messages, posteriors and created potentials
    void invalidateAllMessages__();

    /// compute a root for each connected component of propagator__
    void computeJoinTreeRoots__();

    /** @brief removes variables del_vars from a list of potentials and
     * returns the resulting list */
    PotentialSet__ marginalizeOut__(PotentialSet__                  pot_list,
                                    Set< const DiscreteVariable* >& del_vars,
                                    Set< const DiscreteVariable* >& kept_vars);

    /// creates the message sent by clique from_id to clique to_id
    void produceMessage__(NodeId from_id, NodeId to_id);

    /// actually perform the collect phase
    void collectMessage__(NodeId id, NodeId from);

    /// avoid copy constructors
    ShaferShenoyMNInference(const ShaferShenoyMNInference< GUM_SCALAR >&);

    /// avoid copy operators
    ShaferShenoyMNInference< GUM_SCALAR >&
       operator=(const ShaferShenoyMNInference< GUM_SCALAR >&);
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class ShaferShenoyMNInference< double >;
#endif

} /* namespace gum */

#include <agrum/MN/inference/ShaferShenoyMNInference_tpl.h>

#endif /* SHAFER_SHENOY_INFERENCE_H */
