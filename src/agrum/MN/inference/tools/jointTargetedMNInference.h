
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
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
 * @brief This file contains the abstract inference class definition for
 * computing (incrementally) joint posteriors.
 *
 * @author Christophe GONZALES (@AMU) and Pierre-Henri WUILLEMIN (@LIP6)
 */

#ifndef GUM_MARKOV_NET_JOINT_TARGETED_INFERENCE_H
#define GUM_MARKOV_NET_JOINT_TARGETED_INFERENCE_H


#include <agrum/MN/inference/tools/marginalTargetedMNInference.h>
#include <agrum/agrum.h>


namespace gum {


  /**
   * @class JointTargetedMNInference jointTargetedMNInference.h
   * <agrum/MN/inference/jointTargetedMNInference.h>
   * @brief A generic class for the computation of (possibly incrementally)
   * joint posteriors
   * @ingroup mn_group
   *
   * The goal of this class is to take care of the joint targets used
   * for computing joint posteriors. The JointTargetedMNInference class
   * inherits from Inference that takes care of handling both evidence and
   * the current state of the inference and from MarginalTargetedMNInference
   * for the handling of marginal targets. Note that the JointTargetedMNInference
   * is designed to be used in incremental inference engines.
   */
  template < typename GUM_SCALAR >
  class JointTargetedMNInference: public MarginalTargetedMNInference< GUM_SCALAR > {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /** @warning note that, by aGrUM's rule, the MN is not copied but only
     * referenced by the inference algorithm. */
    explicit JointTargetedMNInference(const IMarkovNet< GUM_SCALAR >* mn);

    /// destructor
    virtual ~JointTargetedMNInference();

    /// @}


    // ############################################################################
    /// @name Probability computations
    // ############################################################################
    /// @{

    /// Compute the joint posterior of a set of nodes.
    /**
     * @returns a const ref to the posterior joint probability of the set of
     * nodes.
     * @param nodes the set of nodes whose posterior joint probability is wanted
     *
     * @warning for efficiency reasons, the potential is stored into the
     * inference engine and is returned by reference. In order to ensure
     * that the potential may still exist even if the Inference object is
     * destroyed, the user has to copy it explicitly.
     *
     * @warning prepareInference and makeInference may be applied if needed.
     *
     * @throw UndefinedElement if nodes is not in the targets
     */
    virtual const Potential< GUM_SCALAR >&
       jointPosterior(const NodeSet& nodes) final;

    /// Computes and returns the posterior of a node.
    /**
     * @returns a const ref to the posterior probability of the node.
     * @param node the node for which we need a posterior probability
     *
     * @warning for efficiency reasons, the potential is stored into the
     * inference engine and is returned by reference. In order to ensure
     * that the potential may still exist even if the Inference object is
     * destroyed, the user has to copy it explicitly.
     *
     * @warning prepareInference and makeInference may be applied if needed by
     * the posterior method.
     *
     * @throw UndefinedElement if node is not in the set of targets
     */
    virtual const Potential< GUM_SCALAR >& posterior(NodeId node) final;

    /// Computes and returns the posterior of a node.
    /**
     * @returns a const ref to the posterior probability of the node.
     * @param node the node for which we need a posterior probability
     *
     * @warning for efficiency reasons, the potential is stored into the
     * inference engine and is returned by reference. In order to ensure
     * that the potential may still exist even if the Inference object is
     * destroyed, the user has to copy it explicitly.
     *
     * @warning prepareInference and makeInference may be applied if needed by
     * the posterior method.
     *
     * @throw UndefinedElement if node is not in the set of targets
     */
    virtual const Potential< GUM_SCALAR >&
       posterior(const std::string& nodeName) final;
    /// @}


    // ############################################################################
    /// @name Targets
    // ############################################################################
    /// @{

    /// Clear all previously defined targets (marginal and joint targets)
    /**
     * Clear all previously defined targets. As a result, no posterior can be
     * computed (since we can only compute the posteriors of the marginal or
     * joint
     * targets that have been added by the user).
     */
    virtual void eraseAllTargets();

    /// Clear all previously defined joint targets
    virtual void eraseAllJointTargets() final;

    /// Clear all the previously defined marginal targets
    virtual void eraseAllMarginalTargets() final;

    /// Add a set of nodes as a new joint target. As a collateral effect, every
    /// node is added as a marginal target.
    /**
     * @throw UndefinedElement if some node(s) do not belong to the Markov net
     */
    virtual void addJointTarget(const NodeSet& joint_target) final;

    /// removes an existing joint target
    /** @warning If the joint target does not already exist, the method does
     * nothing. In particular, it does not raise any exception. */
    virtual void eraseJointTarget(const NodeSet& joint_target) final;

    /// return true if target is a joint target.
    virtual bool isJointTarget(const NodeSet& vars) const final;

    /// returns the list of joint targets
    virtual const Set< NodeSet >& jointTargets() const noexcept final;

    /// returns the number of joint targets
    virtual Size nbrJointTargets() const noexcept final;
    /// @}

    /**
     * Create a gum::Potential for P(joint targets|evs) (for all instanciation of
     * targets
     * and evs)
     *
     * @warning If some evs are d-separated, they are not included in the Potential
     *
     * @param targets  the NodeSet of the targeted variables
     * @param evs the NodeSet of observed variables
     * @return a Potential
     */
    Potential< GUM_SCALAR > evidenceJointImpact(const NodeSet& targets,
                                                const NodeSet& evs);

    /**
     * Create a gum::Potential for P(joint targets|evs) (for all instanciation of
     * targets
     * and evs)
     *
     * @warning If some evs are d-separated, they are not included in the Potential
     *
     * @param targets  the vector of std::string of the targeted variables
     * @param evs the vector of std::string of observed variables
     * @return a Potential
     */
    Potential< GUM_SCALAR >
       evidenceJointImpact(const std::vector< std::string >& targets,
                           const std::vector< std::string >& evs);

    // ############################################################################
    /// @name Information Theory related functions
    // ############################################################################
    /// @{

    /** Mutual information between X and Y
     * @see http://en.wikipedia.org/wiki/Mutual_information
     *
     * @warning Due to limitation of @ref joint, may not be able to compute
     * this value
     * @throw OperationNotAllowed in these cases
     */
    GUM_SCALAR I(NodeId X, NodeId Y);

    /** Variation of information between X and Y
     * @see http://en.wikipedia.org/wiki/Variation_of_information
     *
     * @warning Due to limitation of @ref joint, may not be able to compute
     * this value
     * @throw OperationNotAllowed in these cases
     */
    GUM_SCALAR VI(NodeId X, NodeId Y);

    /** Mutual information between targets
     * @see https://en.wikipedia.org/wiki/Interaction_information
     * @param targets  the NodeSet of the targeted variables
     */
    GUM_SCALAR jointMutualInformation(const NodeSet& targets);

    /** Mutual information between targets
     * @see https://en.wikipedia.org/wiki/Interaction_information
     * @param targets the vector of std::string of the targeted variables
     */
    GUM_SCALAR jointMutualInformation(const std::vector< std::string >& targets);

    /// @}


    protected:
    /// fired after a new Markov net has been assigned to the engine
    virtual void _onMarkovNetChanged(const IMarkovNet< GUM_SCALAR >* mn);

    /// fired after a new joint target is inserted
    /** @param set The set of target variable's ids. */
    virtual void _onJointTargetAdded(const NodeSet& set) = 0;

    /// fired before a joint target is removed
    /** @param set The set of target variable's ids. */
    virtual void _onJointTargetErased(const NodeSet& set) = 0;

    /// fired before a all the marginal and joint targets are removed
    virtual void _onAllTargetsErased() = 0;

    ///  fired before a all the joint targets are removed
    virtual void _onAllJointTargetsErased() = 0;


    /// asks derived classes for the joint posterior of a declared target set
    /** @param set The set of ids of the variables whose joint posterior is
     * looked for. */
    virtual const Potential< GUM_SCALAR >& _jointPosterior(const NodeSet& set) = 0;

    /** @brief asks derived classes for the joint posterior of a set of
     * variables not declared as a joint target
     *
     * @param wanted_target The set of ids of the variables whose joint
     * posterior is looked for.
     * @param declared_target the joint target declared by the user that
     * contains set */
    virtual const Potential< GUM_SCALAR >&
       _jointPosterior(const NodeSet& wanted_target,
                       const NodeSet& declared_target) = 0;

    /** @brief returns a fresh unnormalized joint posterior of
     * a given set of variables
     * @param set The set of ids of the variables whose joint posterior is
     * looked for. */
    virtual Potential< GUM_SCALAR >*
       _unnormalizedJointPosterior(const NodeSet& set) = 0;

    /// returns a fresh potential equal to P(argument,evidence)
    virtual Potential< GUM_SCALAR >* _unnormalizedJointPosterior(NodeId id) = 0;


    private:
    /// the set of joint targets
    Set< NodeSet > __joint_targets;
  };


}   // namespace gum


#include <agrum/MN/inference/tools/jointTargetedMNInference_tpl.h>


#endif   // GUM_MARKOV_NET_JOINT_TARGETED_INFERENCE_H