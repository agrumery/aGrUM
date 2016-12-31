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
 * @brief Implementation of a variable elimination algorithm
 * for inference in Bayesian Networks.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_VARIABLE_ELIMINATION_H
#define GUM_VARIABLE_ELIMINATION_H

#include <cmath>
#include <utility>

#include <agrum/BN/inference/barrenNodesFinder.h>
#include <agrum/BN/inference/jointTargetedInference.h>
#include <agrum/BN/inference/relevantPotentialsFinderType.h>
#include <agrum/config.h>
#include <agrum/graphs/triangulations/defaultTriangulation.h>

namespace gum {


  // the function used to combine two tables
  template <typename GUM_SCALAR>
  INLINE static Potential<GUM_SCALAR>*
  VENewmultiPotential( const Potential<GUM_SCALAR>& t1,
                       const Potential<GUM_SCALAR>& t2 ) {
    return new Potential<GUM_SCALAR>( t1 * t2 );
  }

  // the function used to combine two tables
  template <typename GUM_SCALAR>
  INLINE static Potential<GUM_SCALAR>*
  VENewprojPotential( const Potential<GUM_SCALAR>&        t1,
                      const Set<const DiscreteVariable*>& del_vars ) {
    return new Potential<GUM_SCALAR>( t1.margSumOut( del_vars ) );
  }


  /**
   * @class VariableElimination VariableElimination.h
   * <agrum/BN/inference/variableElimination.h>
   * @brief Implementation of a Shafer-Shenoy's-like version of lazy
   * propagation for inference in Bayesian Networks
   * @ingroup bn_inference
   */
  template <typename GUM_SCALAR>
  class VariableElimination : public JointTargetedInference<GUM_SCALAR> {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    VariableElimination(
        const IBayesNet<GUM_SCALAR>* BN,
        RelevantPotentialsFinderType relevant_type =
            RelevantPotentialsFinderType::DSEP_BAYESBALL_POTENTIALS,
        FindBarrenNodesType = FindBarrenNodesType::FIND_BARREN_NODES );

    /// destructor
    virtual ~VariableElimination();

    /// @}


    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// use a new triangulation algorithm
    void setTriangulation( const Triangulation& new_triangulation );

    /// sets how we determine the relevant potentials to combine
    /** When a clique sends a message to a separator, it first constitute the
     * set of the potentials it contains and of the potentials contained in the
     * messages it received. If RelevantPotentialsFinderType = FIND_ALL,
     * all these potentials are combined and projected to produce the message
     * sent to the separator.
     * If RelevantPotentialsFinderType = DSEP_BAYESBALL_NODES, then only the
     * set of potentials d-connected to the variables of the separator are kept
     * for combination and projection. */
    void setRelevantPotentialsFinderType( RelevantPotentialsFinderType type );

    /// sets how we determine barren nodes
    /** Barren nodes are unnecessary for probability inference, so they can
     * be safely discarded in this case (type = FIND_BARREN_NODES). This
     * speeds-up inference. However, there are some cases in which we do not
     * want to remove barren nodes, typically when we want to answer queries
     * such as Most Probable Explanations (MPE). */
    void setFindBarrenNodesType( FindBarrenNodesType type );

    /// returns the join tree used for compute the posterior of node id
    const JunctionTree* junctionTree( const NodeId id );

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
     * known and can be changed between _updateOutdatedBNPotentials and
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
     * @param declared_target the joint target declared by the user that contains
     * set */
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


    /// the type of relevant potential finding algorithm to be used
    RelevantPotentialsFinderType __find_relevant_potential_type;

    /** @brief update a set of potentials: the remaining are those to be
     * combined to produce a message on a separator */
    void ( VariableElimination<GUM_SCALAR>::*__findRelevantPotentials )(
        Set<const Potential<GUM_SCALAR>*>& pot_list,
        Set<const DiscreteVariable*>&      kept_vars );

    /// the type of barren nodes computation we wish
    FindBarrenNodesType __barren_nodes_type;

    /// the operator for performing the projections
    Potential<GUM_SCALAR>* ( *__projection_op )(
        const Potential<GUM_SCALAR>&,
        const Set<const DiscreteVariable*>& ){VENewprojPotential};

    /// the operator for performing the combinations
    Potential<GUM_SCALAR>* ( *__combination_op )( const Potential<GUM_SCALAR>&,
                                                  const Potential<GUM_SCALAR>& ){
        VENewmultiPotential};

    /// the triangulation class creating the junction tree used for inference
    Triangulation* __triangulation;

    /// the undigraph extracted from the BN and used to construct the join tree
    /** If all nodes are targets, this graph corresponds to the moral graph
     * of the BN. Otherwise, it may be a subgraph of this moral graph. For
     * instance if the BN is A->B->C and only B is a target, __graph will be
     * equal to A-B if we exploit barren nodes (C is a barren node and,
     * therefore, can be removed for inference). */
    UndiGraph __graph;

    /// the junction tree used to answer the last inference query
    JunctionTree* __JT{nullptr};

    /// for each node of __graph (~ in the Bayes net), associate an ID in the JT
    HashTable<NodeId, NodeId> __node_to_clique;

    /// for each BN node, indicate in which clique its CPT will be stored
    HashTable<NodeId, NodeSet> __clique_potentials;

    /// indicate a clique that contains all the nodes of the target
    NodeId __targets2clique;

    /// the posterior computed during the last inference
    /** the posterior is owned by VariableElimination. */
    Potential<GUM_SCALAR>* __target_posterior{nullptr};

    /// for comparisons with 1 - epsilon
    const GUM_SCALAR __1_minus_epsilon{GUM_SCALAR( 1.0 - 1e-6 )};


    /// create a new junction tree as well as its related data structures
    void __createNewJT( const NodeSet& joint_target );

    /// sets the operator for performing the projections
    void __setProjectionFunction( Potential<GUM_SCALAR>* ( *proj )(
        const Potential<GUM_SCALAR>&, const Set<const DiscreteVariable*>&));

    /// sets the operator for performing the combinations
    void __setCombinationFunction( Potential<GUM_SCALAR>* ( *comb )(
        const Potential<GUM_SCALAR>&, const Potential<GUM_SCALAR>&));

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
    void __findRelevantPotentialsGetAll( __PotentialSet&               pot_list,
                                         Set<const DiscreteVariable*>& kept_vars );

    /** @brief update a set of potentials: the remaining are those to be
     * combined
     * to produce a message on a separator */
    void __findRelevantPotentialsXX( __PotentialSet&               pot_list,
                                     Set<const DiscreteVariable*>& kept_vars );

    // remove barren variables and return the newly created projected potentials
    __PotentialSet
    __removeBarrenVariables( __PotentialSet&               pot_list,
                             Set<const DiscreteVariable*>& del_vars );

    /// actually perform the collect phase
    std::pair<__PotentialSet, __PotentialSet>
    __collectMessage( const NodeId id, const NodeId from );

    /// returns the CPT + evidence of a node projected w.r.t. hard evidence
    std::pair<__PotentialSet, __PotentialSet>
    __NodePotentials( const NodeId node );

    /// creates the message sent by clique from_id to clique to_id
    std::pair<__PotentialSet, __PotentialSet>
    __produceMessage( const NodeId from_id,
                      const NodeId to_id,
                      std::pair<__PotentialSet, __PotentialSet>&& in_mess );

    /** @brief removes variables del_vars from a list of potentials and
     * returns the resulting list */
    __PotentialSet __marginalizeOut( __PotentialSet                pot_list,
                                     Set<const DiscreteVariable*>& del_vars,
                                     Set<const DiscreteVariable*>& kept_vars );

    /// avoid copy constructors
    VariableElimination( const VariableElimination<GUM_SCALAR>& );

    /// avoid copy operators
    VariableElimination<GUM_SCALAR>&
    operator=( const VariableElimination<GUM_SCALAR>& );
  };


  extern template class VariableElimination<float>;
  extern template class VariableElimination<double>;


} /* namespace gum */


#include <agrum/BN/inference/variableElimination_tpl.h>


#endif /* GUM_VARIABLE_ELIMINATION_ */
// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
