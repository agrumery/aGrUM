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
 * @brief This file contains general methods for simulation-oriented approximate
 * inference.
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_SAMPLING_INFERENCE_H
#define GUM_SAMPLING_INFERENCE_H

#include <agrum/BN/BayesNetFragment.h>
#include <agrum/BN/inference/tools/estimator.h>
#include <agrum/BN/inference/tools/gibbsOperator.h>

namespace gum {
  /**
   * @class SamplingInference samplingInference.h
   * <agrum/BN/inference/samplingInference.h>
   * @brief A generic class for making sampling inference in Bayesian networks
   * adapted
   *
   * @ingroup bn_approximation
   *
   * The goal of this class is to define the genereal scheme used by all
   * sampling inference algorithms,
   * which are implemented as derived classes of ApproximateInference.
   * This class inherits from MarginalTargetedInference for the handling of
   * marginal targets
   * and from ApproximationScheme.
   */

  template < typename GUM_SCALAR >
  class SamplingInference: public ApproximateInference< GUM_SCALAR > {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor
    /** @warning By default, all the nodes of the Bayes net are targets.
     * @warning note that, by aGrUM's rule, the BN is not copied but only
     * referenced by the inference algorithm. */

    explicit SamplingInference(const IBayesNet< GUM_SCALAR >* bn);

    /// destructor
    ~SamplingInference() override;

    /// Computes and returns the actual estimation of the posterior of a node.
    /**
     * @returns a const ref to the posterior probability of the node.
     * @param id the node for which we need a posterior probability
     *
     * @warning for efficiency reasons, the potential is returned by reference.
     * In order to ensure that the potential may still exist even if the Inference
     * object is destroyed, the user has to copy it explicitly.
     *
     * @throw UndefinedElement if node is not in the set of targets.
     * @throw NotFound if node is not in the BN.
     */
    const Potential< GUM_SCALAR >& currentPosterior(NodeId id);

    /// Computes and returns the actual estimation of the posterior of a node by
    /// its name.
    /**
     * @returns a const ref to the posterior probability of the node referred by
     * name.
     * @param name the name of the node for which we need a posterior probability
     *
     * @warning for efficiency reasons, the potential is returned by reference.
     * In order to ensure that the potential may still exist even if the Inference
     * object is destroyed, the user has to copy it explicitly.
     *
     * @throw UndefinedElement if node corresponding to name is not in the set of
     * targets.
     * @throw NotFound if node corresponding to name is not in the BN.
     */
    const Potential< GUM_SCALAR >& currentPosterior(const std::string& name);
    /// @}
    /// @}


    // ############################################################################
    /// @name Probability computations
    // ############################################################################
    /// @{
    /// get the BayesNet which is used to really perform the sampling
    const IBayesNet< GUM_SCALAR >& samplingBN();

    /// Computes and returns the posterior of a node.
    /**
     * @returns a const ref to the posterior probability of the node.
     * @param id the node for which we need a posterior probability
     *
     * @warning for efficiency reasons, the potential is returned by reference.
     * In order to ensure that the potential may still exist even if the Inference
     * object is destroyed, the user has to copy it explicitly.
     *
     * @throw UndefinedElement if node is not in the set of targets.
     * @throw NotFound if node is not in the BN.
     */
    const Potential< GUM_SCALAR >& posterior_(NodeId id) override;

    /// @}


    /// Simplifying the Bayesian network with relevance reasonning to lighten the
    /// computational charge
    /**
     * Sets the reference Bayesian network as a BayesNetFragment after having
     * eliminated nodes
     * that are idle for simulation and computation, such as barren or d-separated
     * nodes.
     * Eliminates the arcs from evidence nodes to it's children, after setting new
     * CPT's for them.
     *
     */
    virtual void contextualize();

    // ############################################################################
    /// @name Estimator objects initializing
    // ############################################################################
    /// @{


    /// Initializes the estimators object linked to the simulation
    /**
     * Initializes the estimator object by creating a hashtable between non
     * evidence nodes and a 0-filled potential which will approximate the node's
     * posterior
     *
     */
    virtual void setEstimatorFromBN_();

    /// Initializes the estimators object linked to the simulation
    /**
     * @param lbp a LoopyBeliefPropagation object
     * @param virtualLBPSize the size of the equivalent sampling by LBP
     *
     * Initializes the estimator object by creating a hashtable between
     * non evidence nodes and the current approximation of the node's posterior
     * obtained by running LoopyBeliefPropagation algorithm
     *
     */
    virtual void setEstimatorFromLBP_(LoopyBeliefPropagation< GUM_SCALAR >* lbp,
                                      GUM_SCALAR                            virtualLBPSize);
    ///@}

    protected:
    /// Estimator object designed to approximate target posteriors
    Estimator< GUM_SCALAR > _estimator_;

    /// whether the Estimator object has been initialized
    bool isSetEstimator = false;

    /// whether the referenced Bayesian network has been "contextualized"
    bool isContextualized = false;

    /// draws samples without updating the estimators
    virtual Instantiation burnIn_() = 0;

    /// draws a sample in the Bayesian network given a previous one
    /**
     * @param w the weight of sample being generated
     * @param prev the previous sample generated
     *
     */
    virtual Instantiation draw_(GUM_SCALAR* w, Instantiation prev) = 0;

    /// makes the inference by generating samples
    void makeInference_() override;
    void loopApproxInference_();

    /// adds a node to current instantiation
    /**
     * @param nod the node to add to the sample
     * @param I the current sample
     *
     * generates random value based on the BN's CPT's and adds the node to the
     * Instantiation with that value
     */
    virtual void addVarSample_(NodeId nod, Instantiation* I);


    /// fired when Bayesian network is contextualized
    /**
     * @param bn the contextualized BayesNetFragment
     * @param targets inference target variables
     * @param hardEvNodes hard evidence nodes
     * @param hardEv hard evidences values
     *
     */
    virtual void onContextualize_(BayesNetFragment< GUM_SCALAR >* bn);

    void onEvidenceAdded_(const NodeId id, bool isHardEvidence) override;

    void onEvidenceErased_(const NodeId id, bool isHardEvidence) override;

    void onAllEvidenceErased_(bool contains_hard_evidence) override;

    void onEvidenceChanged_(const NodeId id, bool hasChangedSoftHard) override;

    void onModelChanged_(const GraphicalModel* bn) override;

    void updateOutdatedStructure_() override;

    void updateOutdatedPotentials_() override;

    void onMarginalTargetAdded_(const NodeId id) override;

    void onMarginalTargetErased_(const NodeId id) override;

    void onAllMarginalTargetsAdded_() override;

    void onAllMarginalTargetsErased_() override;

    void onStateChanged_() override;

    private:
    BayesNetFragment< GUM_SCALAR >* _samplingBN_;
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class SamplingInference< double >;
#endif

}   // namespace gum

#include <agrum/BN/inference/tools/samplingInference_tpl.h>
#endif
