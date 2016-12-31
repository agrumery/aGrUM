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
 * computing (incrementally) marginal posteriors.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_BAYES_NET_MARGINAL_TARGETED_INFERENCE_H
#define GUM_BAYES_NET_MARGINAL_TARGETED_INFERENCE_H

#include <string>

#include <agrum/BN/inference/BayesNetInference.h>
#include <agrum/config.h>


namespace gum {


  /**
   * @class MarginalTargetedInference marginalTargetedInference.h
   * <agrum/BN/inference/marginalTargetedInference.h>
   * @brief A generic class for the computation of (possibly incrementally)
   * marginal posteriors
   * @ingroup bn_group
   *
   * The goal of this class is to take care of the marginal targets used
   * for computing marginal posteriors. The MarginalTargetedInference class
   * inherits from Inference that takes care of handling both evidence and
   * the current state of the inference. Note that the MarginalTargetedInference
   * is designed to be used in incremental inference engines.
   */
  template<typename GUM_SCALAR>
  class MarginalTargetedInference : public virtual BayesNetInference<GUM_SCALAR> {
  public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /** @warning By default, all the nodes of the Bayes net are targets.
     * @warning note that, by aGrUM's rule, the BN is not copied but only
     * referenced by the inference algorithm. */
    MarginalTargetedInference(const IBayesNet<GUM_SCALAR> *bn);

    /// destructor
    virtual ~MarginalTargetedInference();

    /// @}


    // ############################################################################
    /// @name Probability computations
    // ############################################################################
    /// @{

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
    virtual const Potential<GUM_SCALAR> &posterior(const NodeId node);

    /// Computes and returns the posterior of a node.
    /**
     * @returns a const ref to the posterior probability of the node.
     * @param nodeName the anme of the node for which we need a posterior
     * probability
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
    virtual const Potential<GUM_SCALAR> &posterior(const std::string &nodeName);

    /// @}


    // ############################################################################
    /// @name Targets
    // ############################################################################
    /// @{

    /// Clear all previously defined targets
    virtual void eraseAllTargets();

    /// adds all nodes as targets
    virtual void addAllTargets() final;

    /// Add a marginal target to the list of targets
    /**
     * @throw UndefinedElement if target is not a NodeId in the Bayes net
     */
    virtual void addTarget(const NodeId target) final;

    /// Add a marginal target to the list of targets
    /**
     * @throw UndefinedElement if target is not a NodeId in the Bayes net
     */
    virtual void addTarget(const std::string &nodeName) final;

    /// removes an existing (marginal) target
    /** @warning If the target does not already exist, the method does nothing.
     * In particular, it does not raise any exception. */
    virtual void eraseTarget(const NodeId target) final;

    /// removes an existing (marginal) target
    /** @warning If the target does not already exist, the method does nothing.
     * In particular, it does not raise any exception. */
    virtual void eraseTarget(const std::string &nodeName) final;

    /// return true if variable is a (marginal) target
    virtual bool isTarget(const NodeId variable) const final;

    /// return true if variable is a (marginal) target
    virtual bool isTarget(const std::string &nodeName) const final;

    /// returns the number of marginal targets
    virtual const Size nbrTargets() const noexcept final;

    /// returns the list of marginal targets
    virtual const NodeSet &targets() const noexcept final;

    /// @}

    // ############################################################################
    /// @name Information Theory related functions
    // ############################################################################
    /// @{

    /** Entropy
     * Compute Shanon's entropy of a node given the observation
     * @see http://en.wikipedia.org/wiki/Information_entropy
     */
    virtual GUM_SCALAR H(const NodeId X) final;

    /** Entropy
     * Compute Shanon's entropy of a node given the observation
     * @see http://en.wikipedia.org/wiki/Information_entropy
     */
    virtual GUM_SCALAR H(const std::string &nodeName) final;

    ///@}


    /**
     * Create a gum::Potential for P(target|evs) (for all instanciation of target
     * and evs)
     *
     * @warning If some evs are d-separated, they are not included in the Potential
     *
     * @param bn the BayesNet
     * @param target  the nodeId of the targetted variable
     * @param evs the nodeId of the observed variable
     * @return a Potential
     */
    Potential<GUM_SCALAR> evidenceImpact(NodeId target,
                                         const std::vector<NodeId> evs);

    /**
    * Create a gum::Potential for P(target|evs) (for all instanciation of target
    * and evs)
    *
    * @warning If some evs are d-separated, they are not included in the Potential
    *
    * @param bn the BayesNet
    * @param target  the nodeId of the target variable
    * @param evs the nodeId of the observed variable
    * @return a Potential
    */
    Potential<GUM_SCALAR> evidenceImpact(std::string target,
                                         const std::vector<std::string> evs);

  protected:
    /// fired after a new marginal target is inserted
    /** @param id The target variable's id. */
    virtual void _onMarginalTargetAdded(const NodeId id) = 0;

    /// fired before a marginal target is removed
    /** @param id The target variable's id. */
    virtual void _onMarginalTargetErased(const NodeId id) = 0;

    /// fired after all the nodes of the BN are added as marginal targets
    virtual void _onAllMarginalTargetsAdded() = 0;

    /// fired before a all marginal targets are removed
    virtual void _onAllMarginalTargetsErased() = 0;

    /// fired after a new Bayes net has been assigned to the engine
    virtual void _onBayesNetChanged(const IBayesNet<GUM_SCALAR> *bn);

    /// asks derived classes for the posterior of a given variable
    /** @param id The variable's id. */
    virtual const Potential<GUM_SCALAR> &_posterior(const NodeId id) = 0;


  private:
    /// the set of marginal targets
    NodeSet __targets;


    /// remove all the marginal posteriors computed
    void __invalidatePosteriors() noexcept;

    /// sets all the nodes of the Bayes net as targets
    void __setAllMarginalTargets();
  };


}  // namespace gum


#include <agrum/BN/inference/marginalTargetedInference_tpl.h>


#endif  // GUM_BAYES_NET_MARGINAL_TARGETED_INFERENCE_H
