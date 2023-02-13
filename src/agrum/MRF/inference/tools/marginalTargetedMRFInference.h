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
 * @brief This file contains the abstract inference class definition for
 * computing (incrementally) marginal posteriors.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_MARKOV_RANDOM_FIELD_MARGINAL_TARGETED_INFERENCE_H
#define GUM_MARKOV_RANDOM_FIELD_MARGINAL_TARGETED_INFERENCE_H

#include <string>

#include <agrum/MRF/inference/tools/MRFInference.h>
#include <agrum/agrum.h>


namespace gum {


  /**
   * @class MarginalTargetedMRFInference marginalTargetedMRFInference.h
   * <agrum/MRF/inference/marginalTargetedMRFInference.h>
   * @brief A generic class for the computation of (possibly incrementally)
   * marginal posteriors
   * @ingroup mn_group
   *
   * The goal of this class is to take care of the marginal targets used
   * for computing marginal posteriors. The MarginalTargetedMRFInference class
   * inherits from Inference that takes care of handling both evidence and
   * the current state of the inference. Note that the MarginalTargetedMRFInference
   * is designed to be used in incremental inference engines.
   */
  template < typename GUM_SCALAR >
  class MarginalTargetedMRFInference: public virtual MRFInference< GUM_SCALAR > {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /** @warning By default, all the nodes of the Markov net are targets.
     * @warning note that, by aGrUM's rule, the MRF is not copied but only
     * referenced by the inference algorithm. */
    explicit MarginalTargetedMRFInference(const IMarkovRandomField< GUM_SCALAR >* mn);

    /// destructor
    virtual ~MarginalTargetedMRFInference();

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
    virtual const Potential< GUM_SCALAR >& posterior(NodeId node);

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
    virtual const Potential< GUM_SCALAR >& posterior(const std::string& nodeName);

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
     * @throw UndefinedElement if target is not a NodeId in the Markov net
     */
    virtual void addTarget(NodeId target) final;

    /// Add a marginal target to the list of targets
    /**
     * @throw UndefinedElement if target is not a NodeId in the Markov net
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
     * Create a gum::Potential for P(target|evs) (for all instanciation of target
     * and evs)
     *
     * @warning If some evs are d-separated, they are not included in the Potential
     *
     * @param mn the MarkovRandomField
     * @param target  the nodeId of the targetted variable
     * @param evs the vector of nodeId of the observed variables
     * @return a Potential
     */
    Potential< GUM_SCALAR > evidenceImpact(NodeId target, const NodeSet& evs);

    /**
     * Create a gum::Potential for P(target|evs) (for all instanciation of target
     * and evs)
     *
     * @warning If some evs are d-separated, they are not included in the Potential
     *
     * @param target  the nodeId of the target variable
     * @param evs the nodeId of the observed variable
     * @return a Potential
     */
    Potential< GUM_SCALAR > evidenceImpact(const std::string&                target,
                                           const std::vector< std::string >& evs);

    protected:
    /// fired after a new marginal target is inserted
    /** @param id The target variable's id. */
    virtual void onMarginalTargetAdded_(const NodeId id) = 0;

    /// fired before a marginal target is removed
    /** @param id The target variable's id. */
    virtual void onMarginalTargetErased_(const NodeId id) = 0;

    /// fired after all the nodes of the MRF are added as marginal targets
    virtual void onAllMarginalTargetsAdded_() = 0;

    /// fired before a all marginal targets are removed
    virtual void onAllMarginalTargetsErased_() = 0;

    /// fired after a new Markov net has been assigned to the engine
    virtual void onModelChanged_(const GraphicalModel* mn);

    /// asks derived classes for the posterior of a given variable
    /** @param id The variable's id. */
    virtual const Potential< GUM_SCALAR >& posterior_(NodeId id) = 0;

    protected:
    void setTargetedMode_();
    bool isTargetedMode_() const;

    private:
    /// whether the actual targets are default
    bool _targeted_mode_;

    /// the set of marginal targets
    NodeSet _targets_;


    /*/// remove all the marginal posteriors computed
    void  _invalidatePosteriors_() noexcept;*/

    /// sets all the nodes of the Markov net as targets
    void _setAllMarginalTargets_();
  };


}   // namespace gum


#include <agrum/MRF/inference/tools/marginalTargetedMRFInference_tpl.h>


#endif   // GUM_MARKOV_RANDOM_FIELD_MARGINAL_TARGETED_INFERENCE_H
