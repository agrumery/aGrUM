/**
 *
 *   Copyright (c) 2005-2024  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief Implementation of a Shafer-Shenoy's-like version of lazy propagation
 * for inference in Bayesian networks.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LAZY_PROPAGATION_H
#define GUM_LAZY_PROPAGATION_H

#include <utility>

#include <agrum/agrum.h>

#include <agrum/tools/graphicalModels/inference/scheduler/scheduledInference.h>
#include <agrum/tools/graphs/algorithms/triangulations/defaultTriangulation.h>

#include <agrum/BN/algorithms/barrenNodesFinder.h>
#include <agrum/BN/inference/tools/evidenceInference.h>
#include <agrum/BN/inference/tools/jointTargetedInference.h>
#include <agrum/BN/inference/tools/relevantPotentialsFinderType.h>

namespace gum {


  // the function used to combine two tables
  template < typename GUM_SCALAR >
  INLINE static Potential< GUM_SCALAR > LPNewmultiPotential(const Potential< GUM_SCALAR >& t1,
                                                            const Potential< GUM_SCALAR >& t2) {
    return t1 * t2;
  }

  // the function used to combine two tables with summations
  template < typename GUM_SCALAR >
  INLINE static Potential< GUM_SCALAR > LPNewprojPotential(const Potential< GUM_SCALAR >& t1,
                                                           const gum::VariableSet& del_vars) {
    return t1.sumOut(del_vars);
  }

  // the function used to combine two tables with max
  template < typename GUM_SCALAR >
  INLINE static Potential< GUM_SCALAR > LPMaxprojPotential(const Potential< GUM_SCALAR >& t1,
                                                           const gum::VariableSet& del_vars) {
    return t1.maxOut(del_vars);
  }

  /**
   * @class LazyPropagation lazyPropagation.h
   * @headerfile lazyPropagation.h <agrum/BN/inference/lazyPropagation.h>
   * @brief Implementation of a Shafer-Shenoy's-like version of lazy
   * propagation for inference in Bayesian networks
   * @ingroup bn_inference
   */
  template < typename GUM_SCALAR >
  class LazyPropagation:
      public JointTargetedInference< GUM_SCALAR >,
      public EvidenceInference< GUM_SCALAR >,
      public ScheduledInference {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    explicit LazyPropagation(const IBayesNet< GUM_SCALAR >* BN,
                             RelevantPotentialsFinderType
                             = RelevantPotentialsFinderType::DSEP_BAYESBALL_POTENTIALS,
                             FindBarrenNodesType       = FindBarrenNodesType::FIND_BARREN_NODES,
                             bool use_binary_join_tree = true);

    /// avoid copy constructors
    LazyPropagation(const LazyPropagation< GUM_SCALAR >&) = delete;

    /// avoid copy operators
    LazyPropagation< GUM_SCALAR >& operator=(const LazyPropagation< GUM_SCALAR >&) = delete;

    /// destructor
    ~LazyPropagation();

    /// @}


    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// use a new triangulation algorithm
    void setTriangulation(const Triangulation& new_triangulation);

    /// sets how we determine the relevant potentials to combine
    /** When a clique sends a message to a separator, it first determines the
     * set of the potentials it contains and the potentials contained in the
     * messages it received. If RelevantPotentialsFinderType = FIND_ALL,
     * all these potentials are combined and projected to produce the message
     * sent to the separator.
     * If RelevantPotentialsFinderType = DSEP_BAYESBALL_NODES, then only the
     * set of potentials d-connected to the variables of the separator are kept
     * for combination and projection. */
    void setRelevantPotentialsFinderType(RelevantPotentialsFinderType type);

    /// sets how we determine barren nodes
    /** Barren nodes are unnecessary for probability inference, so they can
     * be safely discarded in this case (type = FIND_BARREN_NODES). This
     * speeds-up inference. However, there are some cases in which we do not
     * want to remove barren nodes, typically when we want to answer queries
     * such as Most Probable Explanations (MPE). */
    void setFindBarrenNodesType(FindBarrenNodesType type);

    /// returns the current join tree used
    /** Lazy Propagation does not use a junction tree but a binary join tree
     * because this may enable faster inferences. So do not be surprised to
     * see that some cliques are contained into others in this tree. */
    const JoinTree* joinTree();

    /// returns the current junction tree
    /** Lazy Propagation does not use a junction tree but a binary join tree
     * because this may enable faster inferences. This method return the junction
     * tree, before optimizations
     **/
    const JunctionTree* junctionTree();

    /// returns the probability of evidence
    GUM_SCALAR evidenceProbability() final;

    /// returns the most probable explanation, i.e., X* = argmax_X P(X|e)
    /** the MPE request corresponds to computing X* = argmax_X P(X|e), where X is
     * the set of all the variables in the Bayesian network and e is the set of
     * evidence entered into the network. Method mpe() returns X*, i.e., the
     * instantiation of all the variables in the bayesian network with the
     * highest posterior. If you also wish to get P(X*,e), use Method
     * mpeLog2Posterior() instead.
     * @return an instantiation of all the variables, including those that
     * received hard evidence, such that their joint has the highest possible
     * probability.
     */
    Instantiation mpe();

    /** @brief returns the most probable explanation and its probability, i.e.,
     * (X*, log2(P(X*|e))), where X* = argmax_X P(X|e)
     *
     * @return a pair such that:
     * - its first element is an instantiation of all the variables, including
     *   those that received hard evidence, such that their joint has the highest
     *   possible probability.
     * - its second element is the log2 of the posterior probability of this
     *   instantiation.
     * @warning if the posterior is equal to 0, the log2 value will be equal
     * to std::numeric_limits< GUM_SCALAR >::lowest().
     */
    std::pair< Instantiation, GUM_SCALAR > mpeLog2Posterior();

    /// @}


    protected:
    /// fired after a new evidence is inserted
    void onEvidenceAdded_(const NodeId id, bool isHardEvidence) final;

    /// fired before an evidence is removed
    void onEvidenceErased_(const NodeId id, bool isHardEvidence) final;

    /// fired before all the evidence are erased
    void onAllEvidenceErased_(bool has_hard_evidence) final;

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

    /// fired after a new Bayes net has been assigned to the inference engine
    void onModelChanged_(const GraphicalModel* bn) final;

    /// fired after a new joint target is inserted
    /** @param set The set of target variable's ids. */
    void onJointTargetAdded_(const NodeSet& set) final;

    /// fired before a joint target is removed
    /** @param set The set of target variable's ids. */
    void onJointTargetErased_(const NodeSet& set) final;

    /// fired after all the nodes of the BN are added as single targets
    void onAllMarginalTargetsAdded_() final;

    /// fired before all the single targets are removed
    void onAllMarginalTargetsErased_() final;

    /// fired before all the joint targets are removed
    void onAllJointTargetsErased_() final;

    /// fired before all single and joint targets are removed
    void onAllTargetsErased_() final;

    /// fired when the state of the inference engine is changed
    void onStateChanged_() final {};

    /// prepares inference when the latter is in OutdatedStructure state
    /** Note that the values of evidence are not necessarily
     * known and can be changed between updateOutdatedStructure_ and
     * makeInference_. */
    void updateOutdatedStructure_() final;

    /// prepares inference when the latter is in OutdatedPotentials state
    /** Note that the values of evidence are not necessarily
     * known and can be changed between updateOutdatedPotentials_ and
     * makeInference_. */
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
     * @param declared_target the joint target declared by the user that contains
     * set */
    const Potential< GUM_SCALAR >& jointPosterior_(const NodeSet& wanted_target,
                                                   const NodeSet& declared_target) final;

    /// returns a fresh potential equal to P(argument,evidence)
    Potential< GUM_SCALAR >* unnormalizedJointPosterior_(NodeId id) final;

    /// returns a fresh potential equal to P(argument,evidence)
    Potential< GUM_SCALAR >* unnormalizedJointPosterior_(const NodeSet& set) final;


    private:
    using _PotentialSet_        = Set< const Potential< GUM_SCALAR >* >;
    using _ScheduleMultiDimSet_ = Set< const IScheduleMultiDim* >;


    /// the type of relevant potential finding algorithm to be used
    RelevantPotentialsFinderType _find_relevant_potential_type_{
        RelevantPotentialsFinderType::DSEP_BAYESBALL_POTENTIALS};

    /** @brief update a set of potentials: the remaining are those to be
     * combined to produce a message on a separator */
    void (LazyPropagation< GUM_SCALAR >::*_findRelevantPotentials_)(
        Set< const IScheduleMultiDim* >& pot_list,
        gum::VariableSet&                kept_vars);

    /// the type of barren nodes computation we wish
    FindBarrenNodesType _barren_nodes_type_{FindBarrenNodesType::FIND_BARREN_NODES};

    /// the operator for performing the projections
    Potential< GUM_SCALAR > (*_projection_op_)(const Potential< GUM_SCALAR >&,
                                               const gum::VariableSet&){LPNewprojPotential};

    /// the operator for performing the combinations
    Potential< GUM_SCALAR > (*_combination_op_)(const Potential< GUM_SCALAR >&,
                                                const Potential< GUM_SCALAR >&){
        LPNewmultiPotential};

    /// the triangulation class creating the junction tree used for inference
    Triangulation* _triangulation_;

    /** @brief indicates whether we should transform junction trees into
     * binary join trees */
    bool _use_binary_join_tree_{true};

    /// the undigraph extracted from the BN and used to construct the join tree
    /** If all nodes are targets, this graph corresponds to the moral graph
     * of the BN. Otherwise, it may be a subgraph of this moral graph. For
     * instance if the BN is A->B->C and only B is a target, _graph_ will be
     * equal to A-B if we exploit barren nodes (C is a barren node and,
     * therefore, can be removed for inference). */
    UndiGraph _graph_;

    /// the join (or junction) tree used to answer the last inference query
    JoinTree* _JT_{nullptr};

    /// the junction tree to answer the last inference query
    JunctionTree* _junctionTree_{nullptr};

    /// indicates whether a new join tree is needed for the next inference
    /** when modifying the set of hard evidence, we can determine that
     * the current JT is no more appropriate for inference. This variable
     * enables us to keep track of this. */
    bool _is_new_jt_needed_{true};

    /// a clique node used as a root in each connected component of _JT_
    /** For usual probabilistic inference, roots is useless. This is useful
     * when computing the probability of evidence. In this case, we need to
     * compute this probability in every connected component and multiply
     * them to get the overall probability of evidence.
     * @warning _roots_ should be computed only when evidenceProbability
     * is called. */
    NodeSet _roots_;

    /// for each node of _graph_ (~ in the Bayes net), associate an ID in the JT
    HashTable< NodeId, NodeId > _node_to_clique_;

    /// for each set target, assign a clique in the JT that contains it
    HashTable< NodeSet, NodeId > _joint_target_to_clique_;

    /// the list of all potentials stored in the cliques
    /** This structure contains a list for each clique in the join tree. If
     * a clique did not received any potential, then its list is empty but
     * the entry for the clique does exist. Note that clique potentials
     * contain also soft evidence and the CPTs that were projected to
     * remove their variables that received hard evidence. */
    NodeProperty< _ScheduleMultiDimSet_ > _clique_potentials_;

    /// the list of all potentials stored in the separators after inferences
    /** This structure contains all the arcs of the join tree (edges in both
     * directions) whether the arc received any potential or not. */
    ArcProperty< _ScheduleMultiDimSet_ > _separator_potentials_;

    /// the set of potentials created for the last inference messages
    /** This structure contains only the arcs on which potentials have
     * been created.
     * @warning Note that the CPTs that were projected due to hard
     * evidence do not belong to this structure, they are kept in
     *  _node_to_hard_ev_projected_CPTs_. */
    ArcProperty< _ScheduleMultiDimSet_ > _arc_to_created_potentials_;

    /// the set of single posteriors computed during the last inference
    /** the posteriors are owned by LazyPropagation. */
    NodeProperty< const Potential< GUM_SCALAR >* > _target_posteriors_;

    /// the set of set target posteriors computed during the last inference
    /** the posteriors are owned by LazyPropagation. */
    HashTable< NodeSet, const Potential< GUM_SCALAR >* > _joint_target_posteriors_;

    /** @brief the constants resulting from the projections of CPTs defined
     * over only hard evidence nodes
     * @TODO remove this constant and insert the notion of a constant into
     * potentials/multidim arrays */
    NodeProperty< GUM_SCALAR > _constants_;

    /// indicates whether a message (from one clique to another) has been computed
    /** Here, all the messages, computed or not, are put into the property, only
     * the Boolean makes the difference between messages computed and those that
     * were not computed */
    ArcProperty< bool > _messages_computed_;

    /// the soft evidence stored in the cliques per their assigned node in the BN
    /** This variable is useful for method updateOutdatedPotentials_: it
     * enables to know which soft evidence should be removed/added into the
     * cliques of the join tree.
     * @warning These potentials are not owned by LazyPropagation, they are only
     * referenced by it. Only the cliques that contain evidence are
     * filled in this structure. */
    NodeProperty< const IScheduleMultiDim* > _node_to_soft_evidence_;

    /// the CPTs that were projected due to hard evidence nodes
    /** For each node whose CPT is defined over some nodes that contain some
     * hard evidence, assigns a new projected CPT that does not contain
     * these nodes anymore.
     * @warning These potentials are owned by LayPropagation.
     * @warning As these potentials are also inserted in _clique_potentials_, we
     * should not parse _node_to_hard_ev_projected_CPTs_ to delete them but rather
     * parse _clique_potentials_ to do it. */
    NodeProperty< const IScheduleMultiDim* > _node_to_hard_ev_projected_CPTs_;

    /// the hard evidence nodes which were projected in CPTs
    NodeSet _hard_ev_nodes_;

    /// the possible types of evidence changes
    enum EvidenceChangeType { EVIDENCE_ADDED, EVIDENCE_ERASED, EVIDENCE_MODIFIED };

    /** @brief indicates which nodes of the BN have evidence that changed
     * since the last inference */
    NodeProperty< EvidenceChangeType > _evidence_changes_;

    /// indicates whether we should use schedules for inference
    bool _use_schedules_{false};

    /// minimal number of operations to perform in the JT to use schedules
    static constexpr double _schedule_threshold_{1000000.0};

    /// for comparisons with 1 - epsilon
    static constexpr GUM_SCALAR _one_minus_epsilon_{GUM_SCALAR(1.0 - 1e-6)};


    /// check whether a new join tree is really needed for the next inference
    bool _isNewJTNeeded_() const;

    /// create a new junction tree as well as its related data structures
    void _createNewJT_();

    /// put all the CPTs into the cliques when creating the JT using a schedule
    void _initializeJTCliques_(Schedule& schedule);

    /// put all the CPTs into the cliques when creating the JT without using a schedule
    void _initializeJTCliques_();

    /// sets the operator for performing the projections
    void _setProjectionFunction_(Potential< GUM_SCALAR > (*proj)(const Potential< GUM_SCALAR >&,
                                                                 const gum::VariableSet&));

    /// sets the operator for performing the combinations
    void _setCombinationFunction_(Potential< GUM_SCALAR > (*comb)(const Potential< GUM_SCALAR >&,
                                                                  const Potential< GUM_SCALAR >&));

    /// invalidate all the messages sent from a given clique
    void _diffuseMessageInvalidations_(NodeId from_id, NodeId to_id, NodeSet& invalidated_cliques);

    /// invalidate all messages, posteriors and created potentials
    void _invalidateAllMessages_();

    /// compute a root for each connected component of  _JT_
    void _computeJoinTreeRoots_();

    /** @brief update a set of potentials: the remaining are those to be
     * combined to produce a message on a separator */
    void _findRelevantPotentialsWithdSeparation_(_ScheduleMultiDimSet_& pot_list,
                                                 gum::VariableSet&      kept_vars);

    /** @brief update a set of potentials: the remaining are those to be
     * combined to produce a message on a separator */
    void _findRelevantPotentialsWithdSeparation2_(_ScheduleMultiDimSet_& pot_list,
                                                  gum::VariableSet&      kept_vars);

    /** @brief update a set of potentials: the remaining are those to be
     * combined to produce a message on a separator */
    void _findRelevantPotentialsWithdSeparation3_(_ScheduleMultiDimSet_& pot_list,
                                                  gum::VariableSet&      kept_vars);

    /** @brief update a set of potentials: the remaining are those to be
     * combined
     * to produce a message on a separator */
    void _findRelevantPotentialsGetAll_(_ScheduleMultiDimSet_& pot_list,
                                        gum::VariableSet&      kept_vars);

    /** @brief update a set of potentials: the remaining are those to be
     * combined to produce a message on a separator */
    void _findRelevantPotentialsXX_(_ScheduleMultiDimSet_& pot_list, gum::VariableSet& kept_vars);

    /// remove barren variables using schedules and return the newly created projected potentials
    _ScheduleMultiDimSet_ _removeBarrenVariables_(Schedule&              schedule,
                                                  _ScheduleMultiDimSet_& pot_list,
                                                  gum::VariableSet&      del_vars);

    /// remove barren variables without schedules and return the newly created projected potentials
    _PotentialSet_ _removeBarrenVariables_(_PotentialSet_& pot_list, gum::VariableSet& del_vars);

    /** @brief removes variables del_vars from a list of potentials and
     * returns the resulting list using schedules */
    _ScheduleMultiDimSet_ _marginalizeOut_(Schedule&             schedule,
                                           _ScheduleMultiDimSet_ pot_list,
                                           gum::VariableSet&     del_vars,
                                           gum::VariableSet&     kept_vars);

    /** @brief removes variables del_vars from a list of potentials and
     * returns the resulting list directly without schedules */
    _ScheduleMultiDimSet_ _marginalizeOut_(_ScheduleMultiDimSet_& pot_list,
                                           gum::VariableSet&      del_vars,
                                           gum::VariableSet&      kept_vars);

    /// creates the message sent by clique from_id to clique to_id using schedules
    void _produceMessage_(Schedule& schedule, NodeId from_id, NodeId to_id);

    /// creates the message sent by clique from_id to clique to_id without schedules
    void _produceMessage_(NodeId from_id, NodeId to_id);

    /// perform the collect phase using schedules
    void _collectMessage_(Schedule& schedule, NodeId id, NodeId from);

    /// perform the collect phase directly without schedules
    void _collectMessage_(NodeId id, NodeId from);

    /// computes the unnormalized posterior of a node using schedules
    Potential< GUM_SCALAR >* _unnormalizedJointPosterior_(Schedule& schedule, NodeId id);

    /// computes the unnormalized posterior of a node without using schedules
    Potential< GUM_SCALAR >* _unnormalizedJointPosterior_(NodeId id);

    /// returns a fresh potential equal to P(argument,evidence) using schedules
    Potential< GUM_SCALAR >* _unnormalizedJointPosterior_(Schedule& schedule, const NodeSet& set);

    /// returns a fresh potential equal to P(argument,evidence) without using schedules
    Potential< GUM_SCALAR >* _unnormalizedJointPosterior_(const NodeSet& set);
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class LazyPropagation< double >;
#endif


} /* namespace gum */

#include <agrum/BN/inference/lazyPropagation_tpl.h>

#endif /* GUM_LAZY_PROPAGATION_H */
