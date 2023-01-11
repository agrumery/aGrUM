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
 * @brief This file contains Importance sampling class definition.
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN(_at_LIP6)
 */


#ifndef GUM_IMPORTANCE_INFERENCE_H
#define GUM_IMPORTANCE_INFERENCE_H

#include <agrum/BN/inference/tools/samplingInference.h>

namespace gum {

  /**
   * @class ImportanceInference importanceInference.h
   *<agrum/BN/inference/importanceInference.h>
   * @brief class for making Importance sampling inference in Bayesian networks.
   * @ingroup bn_approximation
   *
   * This class overrides pure function declared in the inherited class
   *ApproximateInference.
   * It defines the way Importance sampling draws a sample.
   *
   */


  template < typename GUM_SCALAR >
  class ImportanceSampling: public SamplingInference< GUM_SCALAR > {
    public:
    /**
     * Default constructor
     */
    explicit ImportanceSampling(const IBayesNet< GUM_SCALAR >* bn);


    /**
     * Destructor
     */
    ~ImportanceSampling() override;

    protected:
    /// draws a defined number of samples without updating the estimators
    Instantiation burnIn_() override;


    /// draws a sample according to Importance sampling
    /**
     * @param w the weight of sample being generated
     * @param prev the previous sample generated
     * @param bn the Bayesian network containing the evidence
     * @param hardEvNodes hard evidence nodes
     * @param hardEv hard evidences values
     *
     * uses the Importance sampling method to generate a new sample using an
     * evidence-mutilated Bayesian network.
     * Each node added to the sample (in a topological order) has a weight.
     * The sample's weight is the product of all weights.
     */
    Instantiation draw_(GUM_SCALAR* w, Instantiation prev) override;


    /// modifies the cpts of a BN in order to tend to uniform distributions
    /**
     * @param bn a BN fragment on which we wish to modify CPTs
     * @param epsilon a default parameter used to scale the modification of the
     * distributions
     *
     * For every CPT in the BN, epsilon is added to each potential value before
     * normalizing
     *
     */
    void unsharpenBN_(BayesNetFragment< GUM_SCALAR >* bn, float epsilon);

    /// fired when Bayesian network is contextualized
    /**
     * @param bn the contextualized BayesNetFragment
     * @param targets inference target variables
     * @param hardEvNodes hard evidence nodes
     * @param hardEv hard evidences values
     *
     * Adds the target variables, erases the evidence variables and unsharpens the
     * BN.
     *
     */
    void onContextualize_(BayesNetFragment< GUM_SCALAR >* bn) override;
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class ImportanceSampling< double >;
#endif
}   // namespace gum

#include <agrum/BN/inference/importanceSampling_tpl.h>

#endif
