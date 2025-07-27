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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief This file contains the abstract inference class definition for
 * computing (incrementally) marginal posteriors.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_BAYES_NET_MARGINAL_TARGETED_INFERENCE_H
#define GUM_BAYES_NET_MARGINAL_TARGETED_INFERENCE_H

#include <string>

#include <agrum/agrum.h>

#include <agrum/BN/inference/tools/BayesNetInference.h>

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
  template < typename GUM_SCALAR >
  class MarginalTargetedInference: public virtual BayesNetInference< GUM_SCALAR > {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /** @warning By default, all the nodes of the Bayes net are targets.
     * @warning note that, by aGrUM's rule, the BN is not copied but only
     * referenced by the inference algorithm. */
    explicit MarginalTargetedInference(const IBayesNet< GUM_SCALAR >* bn);

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
    virtual const Tensor< GUM_SCALAR >& posterior(NodeId node);

    /// Computes and returns the posterior of a node.
    /**
     * @returns a const ref to the posterior probability of the node.
     * @param nodeName the anme of the node for which we need a posterior
     * probability
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
    virtual const Tensor< GUM_SCALAR >& posterior(const std::string& nodeName);

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
    virtual void addTarget(NodeId target) final;

    /// Add a marginal target to the list of targets
    /**
     * @throw UndefinedElement if target is not a NodeId in the Bayes net
     */
    virtual void addTarget(const std::string& nodeName) final;

    /// removes an existing (marginal) target
    /** @warning If the target does not already exist, the method does nothing.
     * In particular, it does not raise any exception. */
    virtual void eraseTarget(NodeId target) final;

    /// removes an existing (marginal) target
    /** @warning If the target does not already exist, the method does nothing.
     * In particular, it does not raise any exception. */
    virtual void eraseTarget(const std::string& nodeName) final;

    /// return true if variable is a (marginal) target
    virtual bool isTarget(NodeId node) const final;

    /// return true if variable is a (marginal) target
    virtual bool isTarget(const std::string& nodeName) const final;

    /// returns the number of marginal targets
    virtual Size nbrTargets() const noexcept final;

    /// returns the list of marginal targets
    virtual const NodeSet& targets() const noexcept final;

    /// indicates whether the inference is in a target mode
    virtual bool isInTargetMode() const noexcept final;

    /// @}

    // ############################################################################
    /// @name Information Theory related functions
    // ############################################################################
    /// @{

    /** Entropy
     * Compute Shanon's entropy of a node given the observation
     * @see http://en.wikipedia.org/wiki/Information_entropy
     */
    virtual GUM_SCALAR H(NodeId X) final;

    /** Entropy
     * Compute Shanon's entropy of a node given the observation
     * @see http://en.wikipedia.org/wiki/Information_entropy
     */
    virtual GUM_SCALAR H(const std::string& nodeName) final;

    ///@}


    /**
     * Create a gum::Tensor for P(target|evs) (for all instanciation of target
     * and evs)
     *
     * @warning If some evs are d-separated, they are not included in the Tensor
     *
     * @param bn the BayesNet
     * @param target  the nodeId of the targetted variable
     * @param evs the vector of nodeId of the observed variables
     * @return a Tensor
     */
    Tensor< GUM_SCALAR > evidenceImpact(NodeId target, const NodeSet& evs);

    /**
     * Create a gum::Tensor for P(target|evs) (for all instanciation of target
     * and evs)
     *
     * @warning If some evs are d-separated, they are not included in the Tensor
     *
     * @param target  the nodeId of the target variable
     * @param evs the nodeId of the observed variable
     * @return a Tensor
     */
    Tensor< GUM_SCALAR > evidenceImpact(const std::string&                target,
                                        const std::vector< std::string >& evs);

    protected:
    /// fired after a new marginal target is inserted
    /** @param id The target variable's id. */
    virtual void onMarginalTargetAdded_(const NodeId id) = 0;

    /// fired before a marginal target is removed
    /** @param id The target variable's id. */
    virtual void onMarginalTargetErased_(const NodeId id) = 0;

    /// fired after all the nodes of the BN are added as marginal targets
    virtual void onAllMarginalTargetsAdded_() = 0;

    /// fired before a all marginal targets are removed
    virtual void onAllMarginalTargetsErased_() = 0;

    /// fired after a new Bayes net has been assigned to the engine
    virtual void onModelChanged_(const GraphicalModel* bn);

    /// asks derived classes for the posterior of a given variable
    /** @param id The variable's id. */
    virtual const Tensor< GUM_SCALAR >& posterior_(NodeId id) = 0;

    void setTargetedMode_();
    bool isTargetedMode_() const;

    private:
    /// whether the actual targets are default
    bool _targeted_mode_;

    /// the set of marginal targets
    NodeSet _targets_;

    /// sets all the nodes of the Bayes net as targets
    void _setAllMarginalTargets_();
  };


}   // namespace gum

#include <agrum/BN/inference/tools/marginalTargetedInference_tpl.h>


#endif   // GUM_BAYES_NET_MARGINAL_TARGETED_INFERENCE_H
