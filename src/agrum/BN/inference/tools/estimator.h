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
 * @brief This file contains estimating tools for approximate inference
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_ESTIMATOR_H
#define GUM_ESTIMATOR_H

#include <agrum/BN/inference/loopyBeliefPropagation.h>
#include <vector>

namespace gum {

  template < typename GUM_SCALAR >
  class Estimator {
    public:
    /**
     * @class Estimator
     * @headerfile estimator.h <agrum/BN/inference/estimator.h>
     * @brief class for estimating tools for approximate inference
     * @ingroup bn_approximation
     *
     */

    /**
     * Default constructor
     */
    Estimator();

    /**
     * Constructor with Bayesian network
     */
    explicit Estimator(const IBayesNet< GUM_SCALAR >* bn);

    /* Destructor */
    ~Estimator();

    /** estimator initializing
     * @{
     */

    /**
     * sets the estimator object with 0-filled vectors corresponding
     * to each non evidence node
     */
    void setFromBN(const IBayesNet< GUM_SCALAR >* bn, const NodeSet& hardEvidence);

    /**
     * sets the estimatoor object with posteriors obtained by
     * LoopyBeliefPropagation
     */
    void setFromLBP(LoopyBeliefPropagation< GUM_SCALAR >* lbp,
                    const NodeSet&                        hardEvidence,
                    GUM_SCALAR                            virtualLBPSize);
    /** @} */

    /// computes the maximum length of confidence interval for each possible value
    /// of each variable
    /**
     * @returns maximum length of confidence interval
     */
    GUM_SCALAR confidence();

    /// updates the estimator with a given sample
    /**
     * @param I the sample used to update the estimators
     * @param weight bias for the given sample
     *
     * adds the sample weight to each node's given value in the estimator
     */
    void update(Instantiation I, GUM_SCALAR w);

    /// returns the posterior of a node
    /**
     * @returns a constant ref to the posterior probability of the variable node
     * @param var the variable node which we want posterior for
     *
     * returns the vector of cumulated weight bias for each value of the variable
     * normalized as a CPT
     *
     * @throw NotFound if variable node is not in estimator.
     */
    const Potential< GUM_SCALAR >& posterior(const DiscreteVariable& var);

    /// refresh the estimator state as empty
    /** this function remove all the statistics in order to restart the
     * computations.
     */
    void clear();

    private:
    /// estimator represented by hashtable between each variable name and a vector
    /// of cumulative sample weights
    HashTable< std::string, std::vector< GUM_SCALAR > > estimator_;

    /// cumulated weights of all samples
    GUM_SCALAR wtotal_;

    /// number of generated samples
    Size ntotal_;

    /// Bayesian network on which approximation is done
    const IBayesNet< GUM_SCALAR >* bn_;

    /// returns expected value of Bernouilli variable (called by it's name) of
    /// given parameter
    /**
     * @returns Expected value of Bernouilli variable (called by it's name) of
     * given parameter
     * @param name variable's name, considered as a Bernouilli variable
     * @param val the parameter of the Bernouilli variable
     *
     * computes the amount of cumulative weights for paramater val over the amount
     * of total cumulative weights
     */
    GUM_SCALAR EV(std::string name, Idx val);

    /// returns variance of Bernouilli variable (called by it's name) of given
    /// parameter
    /**
     * @returns variance of Bernouilli variable (called by it's name) of given
     * parameter
     * @param name variable's name, considered as a Bernouilli variable
     * @param val the parameter of the Bernouilli variable
     *
     * computes variance for Bernouilli law using EV(name, val)
     */
    GUM_SCALAR variance(std::string name, Idx val);   // variance corrigée

    private:
    /// the set of single posteriors computed during the last inference
    /** the posteriors are owned by LazyPropagation. */
    HashTable< std::string, Potential< GUM_SCALAR >* > _target_posteriors_;
  };

#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class Estimator< double >;
#endif

}   // namespace gum

#include <agrum/BN/inference/tools/estimator_tpl.h>
#endif
