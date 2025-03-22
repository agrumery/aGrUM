/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief This file contains the abstract inference class definition for
 * computing (incrementally) joint posteriors.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_MARKOV_RANDOM_FIELD_JOINT_TARGETED_INFERENCE_H
#define GUM_MARKOV_RANDOM_FIELD_JOINT_TARGETED_INFERENCE_H


#include <agrum/agrum.h>

#include <agrum/MRF/inference/tools/marginalTargetedMRFInference.h>

namespace gum {


  /**
   * @class JointTargetedMRFInference jointTargetedMRFInference.h
   * <agrum/MRF/inference/jointTargetedMRFInference.h>
   * @brief A generic class for the computation of (possibly incrementally)
   * joint posteriors
   * @ingroup mn_group
   *
   * The goal of this class is to take care of the joint targets used
   * for computing joint posteriors. The JointTargetedMRFInference class
   * inherits from Inference that takes care of handling both evidence and
   * the current state of the inference and from MarginalTargetedMRFInference
   * for the handling of marginal targets. Note that the JointTargetedMRFInference
   * is designed to be used in incremental inference engines.
   */
  template < typename GUM_SCALAR >
  class JointTargetedMRFInference: public MarginalTargetedMRFInference< GUM_SCALAR > {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /** @warning note that, by aGrUM's rule, the MRF is not copied but only
     * referenced by the inference algorithm. */
    explicit JointTargetedMRFInference(const IMarkovRandomField< GUM_SCALAR >* mn);

    /// destructor
    virtual ~JointTargetedMRFInference();

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
     * @warning the node with hard evidence are removed from nodes.
     *
     * @warning For efficiency reasons, the tensor is stored into the
     * inference engine and is returned by reference. In order to ensure
     * that the tensor may still exist even if the Inference object is
     * destroyed, the user has to copy it explicitly.
     *
     * @warning prepareInference and makeInference may be applied if needed.
     *
     * @throw UndefinedElement if nodes is not in the targets
     */
    virtual const Tensor< GUM_SCALAR >& jointPosterior(const NodeSet& nodes) final;

    /// Computes and returns the posterior of a node.
    /**
     * @returns a const ref to the posterior probability of the node.
     * @param node the node for which we need a posterior probability
     *
     * @warning for efficiency reasons, the tensor is stored into the
     * inference engine and is returned by reference. In order to ensure
     * that the tensor may still exist even if the Inference object is
     * destroyed, the user has to copy it explicitly.
     *
     * @warning prepareInference and makeInference may be applied if needed by
     * the posterior method.
     *
     * @throw UndefinedElement if node is not in the set of targets
     */
    virtual const Tensor< GUM_SCALAR >& posterior(NodeId node) final;

    /// Computes and returns the posterior of a node.
    /**
     * @returns a const ref to the posterior probability of the node.
     * @param node the node for which we need a posterior probability
     *
     * @warning for efficiency reasons, the tensor is stored into the
     * inference engine and is returned by reference. In order to ensure
     * that the tensor may still exist even if the Inference object is
     * destroyed, the user has to copy it explicitly.
     *
     * @warning prepareInference and makeInference may be applied if needed by
     * the posterior method.
     *
     * @throw UndefinedElement if node is not in the set of targets
     */
    virtual const Tensor< GUM_SCALAR >& posterior(const std::string& nodeName) final;
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
    virtual void eraseAllTargets() final;

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
     * Create a gum::Tensor for P(joint targets|evs) (for all instanciation of
     * targets
     * and evs)
     *
     * @warning If some evs are d-separated, they are not included in the Tensor
     *
     * @param targets  the NodeSet of the targeted variables
     * @param evs the NodeSet of observed variables
     * @return a Tensor
     */
    Tensor< GUM_SCALAR > evidenceJointImpact(const NodeSet& targets, const NodeSet& evs);

    /**
     * Create a gum::Tensor for P(joint targets|evs) (for all instanciation of
     * targets
     * and evs)
     *
     * @warning If some evs are d-separated, they are not included in the Tensor
     *
     * @param targets  the vector of std::string of the targeted variables
     * @param evs the vector of std::string of observed variables
     * @return a Tensor
     */
    Tensor< GUM_SCALAR > evidenceJointImpact(const std::vector< std::string >& targets,
                                             const std::vector< std::string >& evs);

    // ############################################################################
    /// @name Information Theory related functions
    // ############################################################################
    /// @{

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
    /// check if the vars form a possible computable joint (can be redefined by
    /// subclass)
    virtual bool    isExactJointComputable_(const NodeSet& vars);
    virtual NodeSet superForJointComputable_(const NodeSet& vars);

    /// fired after a new Markov net has been assigned to the engine
    virtual void onModelChanged_(const GraphicalModel* mn) override;

    /// fired after a new joint target is inserted
    /** @param set The set of target variable's ids. */
    virtual void onJointTargetAdded_(const NodeSet& set) = 0;

    /// fired before a joint target is removed
    /** @param set The set of target variable's ids. */
    virtual void onJointTargetErased_(const NodeSet& set) = 0;

    /// fired before a all the marginal and joint targets are removed
    virtual void onAllTargetsErased_() = 0;

    ///  fired before a all the joint targets are removed
    virtual void onAllJointTargetsErased_() = 0;


    /// asks derived classes for the joint posterior of a declared target set
    /** @param set The set of ids of the variables whose joint posterior is
     * looked for. */
    virtual const Tensor< GUM_SCALAR >& jointPosterior_(const NodeSet& set) = 0;

    /** @brief asks derived classes for the joint posterior of a set of
     * variables not declared as a joint target
     *
     * @param wanted_target The set of ids of the variables whose joint
     * posterior is looked for.
     * @param declared_target the joint target declared by the user that
     * contains set */
    virtual const Tensor< GUM_SCALAR >& jointPosterior_(const NodeSet& wanted_target,
                                                        const NodeSet& declared_target)
        = 0;

    /** @brief returns a fresh unnormalized joint posterior of
     * a given set of variables
     * @param set The set of ids of the variables whose joint posterior is
     * looked for. */
    virtual Tensor< GUM_SCALAR >* unnormalizedJointPosterior_(const NodeSet& set) = 0;

    /// returns a fresh tensor equal to P(argument,evidence)
    virtual Tensor< GUM_SCALAR >* unnormalizedJointPosterior_(NodeId id) = 0;


    private:
    /// the set of joint targets
    Set< NodeSet > _joint_targets_;
  };


}   // namespace gum

#include <agrum/MRF/inference/tools/jointTargetedMRFInference_tpl.h>


#endif   // GUM_MARKOV_RANDOM_FIELD_JOINT_TARGETED_INFERENCE_H
