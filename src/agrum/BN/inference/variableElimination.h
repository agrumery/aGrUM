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
 * @brief Implementation of a variable elimination algorithm
 * for inference in Bayesian networks.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_VARIABLE_ELIMINATION_H
#define GUM_VARIABLE_ELIMINATION_H

#include <utility>

#include <agrum/agrum.h>
#include <agrum/BN/algorithms/barrenNodesFinder.h>
#include <agrum/BN/inference/tools/jointTargetedInference.h>
#include <agrum/BN/inference/tools/relevantPotentialsFinderType.h>
#include <agrum/tools/graphs/algorithms/triangulations/defaultTriangulation.h>

#include <agrum/tools/graphicalModels/inference/scheduler/scheduledInference.h>


namespace gum {


  // the function used to combine two tables
  template < typename GUM_SCALAR >
  INLINE static Potential< GUM_SCALAR > VENewmultiPotential(const Potential< GUM_SCALAR >& t1,
                                                            const Potential< GUM_SCALAR >& t2) {
    return t1 * t2;
  }

  // the function used to combine two tables
  template < typename GUM_SCALAR >
  INLINE static Potential< GUM_SCALAR >
     VENewprojPotential(const Potential< GUM_SCALAR >&        t1,
                        const Set< const DiscreteVariable* >& del_vars) {
    return t1.margSumOut(del_vars);
  }


  /**
   * @class VariableElimination VariableElimination.h
   * @headerfile variableElimination.h <agrum/BN/inference/variableElimination.h>
   * @brief Implementation of a Variable Elimination's-like version of lazy
   * propagation for inference in Bayesian networks
   * @ingroup bn_inference
   */
  template < typename GUM_SCALAR >
  class VariableElimination:
      public JointTargetedInference< GUM_SCALAR >,
      public ScheduledInference {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    explicit VariableElimination(const IBayesNet< GUM_SCALAR >* BN,
                                 RelevantPotentialsFinderType
                                 = RelevantPotentialsFinderType::DSEP_BAYESBALL_POTENTIALS,
                                 FindBarrenNodesType = FindBarrenNodesType::FIND_BARREN_NODES);

    /// avoid copy constructors
    VariableElimination(const VariableElimination< GUM_SCALAR >&) = delete;

    /// avoid copy operators
    VariableElimination< GUM_SCALAR >& operator=(const VariableElimination< GUM_SCALAR >&) = delete;

    /// destructor
    ~VariableElimination();

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

    /// returns the join tree used for compute the posterior of node id
    const JunctionTree* junctionTree(NodeId id);

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
    virtual void onModelChanged_(const GraphicalModel* bn) final;

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
    void onStateChanged_() final{};

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
    void (VariableElimination< GUM_SCALAR >::*_findRelevantPotentials_)(
       Set< const IScheduleMultiDim* >& pot_list,
       Set< const DiscreteVariable* >&  kept_vars);

    /// the type of barren nodes computation we wish
    FindBarrenNodesType _barren_nodes_type_{FindBarrenNodesType::FIND_BARREN_NODES};

    /// the operator for performing the projections
    Potential< GUM_SCALAR > (*_projection_op_)(const Potential< GUM_SCALAR >&,
                                               const Set< const DiscreteVariable* >&){
       VENewprojPotential};

    /// the operator for performing the combinations
    Potential< GUM_SCALAR > (*_combination_op_)(const Potential< GUM_SCALAR >&,
                                                const Potential< GUM_SCALAR >&){
       VENewmultiPotential};

    /// the triangulation class creating the junction tree used for inference
    Triangulation* _triangulation_;

    /// the undigraph extracted from the BN and used to construct the join tree
    /** If all nodes are targets, this graph corresponds to the moral graph
     * of the BN. Otherwise, it may be a subgraph of this moral graph. For
     * instance if the BN is A->B->C and only B is a target, _graph_ will be
     * equal to A-B if we exploit barren nodes (C is a barren node and,
     * therefore, can be removed for inference). */
    UndiGraph _graph_;

    /// the junction tree used to answer the last inference query
    JunctionTree* _JT_{nullptr};

    /// for each node of _graph_ (~ in the Bayes net), associate an ID in the JT
    HashTable< NodeId, NodeId > _node_to_clique_;

    /// for each clique, indicate the set of nodes whose CPTs will be stored into it
    HashTable< NodeId, NodeSet > _clique_to_nodes_;

    /// indicate a clique that contains all the nodes of the target
    NodeId _targets2clique_;

    /// the posterior computed during the last inference
    /** the posterior is owned by VariableElimination. */
    Potential< GUM_SCALAR >* _target_posterior_{nullptr};

    /// minimal number of operations to perform in the JT to use schedules
    static constexpr double _schedule_threshold_{1000000.0};

    /// for comparisons with 1 - epsilon
    const GUM_SCALAR _one_minus_epsilon_{GUM_SCALAR(1.0 - 1e-6)};


    /// create a new junction tree as well as its related data structures
    void _createNewJT_(const NodeSet& targets);

    /// sets the operator for performing the projections
    void _setProjectionFunction_(Potential< GUM_SCALAR > (
       *proj)(const Potential< GUM_SCALAR >&, const Set< const DiscreteVariable* >&));

    /// sets the operator for performing the combinations
    void _setCombinationFunction_(Potential< GUM_SCALAR > (*comb)(const Potential< GUM_SCALAR >&,
                                                                  const Potential< GUM_SCALAR >&));

    /** @brief update a set of potentials: the remaining are those to be
     * combined to produce a message on a separator */
    void _findRelevantPotentialsWithdSeparation_(_ScheduleMultiDimSet_&          pot_list,
                                                 Set< const DiscreteVariable* >& kept_vars);

    /** @brief update a set of potentials: the remaining are those to be
     * combined to produce a message on a separator */
    void _findRelevantPotentialsWithdSeparation2_(_ScheduleMultiDimSet_&          pot_list,
                                                  Set< const DiscreteVariable* >& kept_vars);

    /** @brief update a set of potentials: the remaining are those to be
     * combined to produce a message on a separator */
    void _findRelevantPotentialsWithdSeparation3_(_ScheduleMultiDimSet_&          pot_list,
                                                  Set< const DiscreteVariable* >& kept_vars);

    /** @brief update a set of potentials: the remaining are those to be
     * combined
     * to produce a message on a separator */
    void _findRelevantPotentialsGetAll_(_ScheduleMultiDimSet_&          pot_list,
                                        Set< const DiscreteVariable* >& kept_vars);

    /** @brief update a set of potentials: the remaining are those to be
     * combined to produce a message on a separator */
    void _findRelevantPotentialsXX_(_ScheduleMultiDimSet_&          pot_list,
                                    Set< const DiscreteVariable* >& kept_vars);

    /// remove barren variables using schedules and return the newly created projected potentials
    _ScheduleMultiDimSet_ _removeBarrenVariables_(Schedule&                       schedule,
                                                  _ScheduleMultiDimSet_&          pot_list,
                                                  Set< const DiscreteVariable* >& del_vars);

    /// remove barren variables without schedules and return the newly created projected potentials
    _PotentialSet_ _removeBarrenVariables_(_PotentialSet_&                 pot_list,
                                           Set< const DiscreteVariable* >& del_vars);

    /// perform the collect phase using schedules
    _ScheduleMultiDimSet_ _collectMessage_(Schedule& schedule, NodeId id, NodeId from);

    /// perform the collect phase directly without schedules
    std::pair< _PotentialSet_, _PotentialSet_ > _collectMessage_(NodeId id, NodeId from);

    /// returns the CPT + evidence of a node projected w.r.t. hard evidence
    std::pair< _PotentialSet_, _PotentialSet_ > _NodePotentials_(NodeId node);

    /// returns the CPT + evidence of a node projected w.r.t. hard evidence
    _ScheduleMultiDimSet_ _NodePotentials_(Schedule& schedule, NodeId node);

    /// creates the message sent by clique from_id to clique to_id without using schedules
    std::pair< _PotentialSet_, _PotentialSet_ >
       _produceMessage_(NodeId                                        from_id,
                        NodeId                                        to_id,
                        std::pair< _PotentialSet_, _PotentialSet_ >&& incoming_messages);

    /// creates the message sent by clique from_id to clique to_id using schedules
    _ScheduleMultiDimSet_ _produceMessage_(Schedule&               schedule,
                                           NodeId                  from_id,
                                           NodeId                  to_id,
                                           _ScheduleMultiDimSet_&& incoming_messages);

    /** @brief removes variables del_vars from a list of potentials and
     * returns the resulting list directly without schedules */
    _PotentialSet_ _marginalizeOut_(_PotentialSet_                  pot_list,
                                    Set< const DiscreteVariable* >& del_vars,
                                    Set< const DiscreteVariable* >& kept_vars);

    /** @brief removes variables del_vars from a list of potentials and
     * returns the resulting list using schedules */
    _ScheduleMultiDimSet_ _marginalizeOut_(Schedule&                       schedule,
                                           _ScheduleMultiDimSet_           pot_list,
                                           Set< const DiscreteVariable* >& del_vars,
                                           Set< const DiscreteVariable* >& kept_vars);

    /// returns a fresh potential equal to P(1st arg,evidence) without using schedules
    /** This function is used by unnormalizedJointPosterior_ */
    Potential< GUM_SCALAR >* _unnormalizedJointPosterior_(NodeId id);

    /// returns a fresh potential equal to P(1st arg,evidence) without using schedules
    /** This function is used by unnormalizedJointPosterior_ */
    Potential< GUM_SCALAR >* _unnormalizedJointPosterior_(Schedule& schedule, NodeId id);

    /// returns a fresh potential equal to P(1st arg,evidence) without using schedules
    /** This function is used by unnormalizedJointPosterior_ */
    Potential< GUM_SCALAR >* _unnormalizedJointPosterior_(const NodeSet& set,
                                                          const NodeSet& targets,
                                                          const NodeSet& hard_evidence_nodes);

    /// returns a fresh potential equal to P(1st arg,evidence) without using schedules
    /** This function is used by unnormalizedJointPosterior_ */
    Potential< GUM_SCALAR >* _unnormalizedJointPosterior_(Schedule&      schedule,
                                                          const NodeSet& set,
                                                          const NodeSet& targets,
                                                          const NodeSet& hard_evidence_nodes);
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class VariableElimination< double >;
#endif


} /* namespace gum */

#include <agrum/BN/inference/variableElimination_tpl.h>


#endif /* GUM_VARIABLE_ELIMINATION_ */
