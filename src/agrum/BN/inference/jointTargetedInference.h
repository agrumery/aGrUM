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
 * @brief This file contains the abstract inference class definition for
 * computing (incrementally) joint posteriors.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_BAYES_NET_JOINT_TARGETED_INFERENCE_H
#define GUM_BAYES_NET_JOINT_TARGETED_INFERENCE_H


#include <agrum/BN/inference/marginalTargetedInference.h>
#include <agrum/config.h>


namespace gum {


  /**
   * @class JointTargetedInference jointTargetedInference.h
   * <agrum/BN/inference/jointTargetedInference.h>
   * @brief A generic class for the computation of (possibly incrementally)
   * joint posteriors
   * @ingroup bn_group
   *
   * The goal of this class is to take care of the joint targets used
   * for computing joint posteriors. The JointTargetedInference class
   * inherits from Inference that takes care of handling both evidence and
   * the current state of the inference and from MarginalTargetedInference
   * for the handling of marginal targets. Note that the JointTargetedInference
   * is designed to be used in incremental inference engines.
   */
  template <typename GUM_SCALAR>
  class JointTargetedInference : public MarginalTargetedInference<GUM_SCALAR> {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /** @warning note that, by aGrUM's rule, the BN is not copied but only
     * referenced by the inference algorithm. */
    JointTargetedInference( const IBayesNet<GUM_SCALAR>* bn );

    /// destructor
    virtual ~JointTargetedInference();

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
    virtual const Potential<GUM_SCALAR>&
    jointPosterior( const NodeSet& nodes ) final;

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
    virtual const Potential<GUM_SCALAR>& posterior( const NodeId node ) final;

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
    virtual const Potential<GUM_SCALAR>&
    posterior( const std::string& nodeName ) final;
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

    /// Add a set of nodes as a new joint target
    /**
     * @throw UndefinedElement if some node(s) do not belong to the Bayes net
     */
    virtual void addJointTarget( const NodeSet& target ) final;

    /// removes an existing joint target
    /** @warning If the joint target does not already exist, the method does
     * nothing. In particular, it does not raise any exception. */
    virtual void eraseJointTarget( const NodeSet& target ) final;

    /// return true if target is a joint target.
    virtual bool isJointTarget( const NodeSet& target ) const final;

    /// returns the list of joint targets
    virtual const Set<NodeSet>& jointTargets() const noexcept final;

    /// returns the number of joint targets
    virtual Size nbrJointTargets() const noexcept final;
    /// @}


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
    GUM_SCALAR I( const NodeId X, const NodeId Y );

    /** Variation of information between X and Y
     * @see http://en.wikipedia.org/wiki/Variation_of_information
     *
     * @warning Due to limitation of @ref joint, may not be able to compute
     * this value
     * @throw OperationNotAllowed in these cases
     */
    GUM_SCALAR VI( const NodeId X, const NodeId Y );

    /// @}


    protected:
    /// fired after a new Bayes net has been assigned to the engine
    virtual void _onBayesNetChanged( const IBayesNet<GUM_SCALAR>* bn );

    /// fired after a new joint target is inserted
    /** @param set The set of target variable's ids. */
    virtual void _onJointTargetAdded( const NodeSet& set ) = 0;

    /// fired before a joint target is removed
    /** @param set The set of target variable's ids. */
    virtual void _onJointTargetErased( const NodeSet& set ) = 0;

    /// fired before a all the marginal and joint targets are removed
    virtual void _onAllTargetsErased() = 0;

    ///  fired before a all the joint targets are removed
    virtual void _onAllJointTargetsErased() = 0;


    /// asks derived classes for the joint posterior of a declared target set
    /** @param set The set of ids of the variables whose joint posterior is
     * looked for. */
    virtual const Potential<GUM_SCALAR>& _jointPosterior( const NodeSet& set ) = 0;

    /** @brief asks derived classes for the joint posterior of a set of
     * variables not declared as a joint target
     *
     * @param wanted_target The set of ids of the variables whose joint
     * posterior is looked for.
     * @param declared_target the joint target declared by the user that
     * contains set */
    virtual const Potential<GUM_SCALAR>&
    _jointPosterior( const NodeSet& wanted_target,
                     const NodeSet& declared_target ) = 0;

    /** @brief returns a fresh unnormalized joint posterior of
     * a given set of variables
     * @param set The set of ids of the variables whose joint posterior is
     * looked for. */
    virtual Potential<GUM_SCALAR>*
    _unnormalizedJointPosterior( const NodeSet& set ) = 0;

    /// returns a fresh potential equal to P(argument,evidence)
    virtual Potential<GUM_SCALAR>*
    _unnormalizedJointPosterior( const NodeId id ) = 0;


    private:
    /// the set of joint targets
    Set<NodeSet> __joint_targets;
  };


}  // namespace gum


#include <agrum/BN/inference/jointTargetedInference_tpl.h>


#endif  // GUM_BAYES_NET_JOINT_TARGETED_INFERENCE_H
