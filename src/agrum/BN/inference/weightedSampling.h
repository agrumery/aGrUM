
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @brief This file contains Weighted sampling class definition.
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN
 */


#ifndef GUM_WEIGHTED_INFERENCE_H
#define GUM_WEIGHTED_INFERENCE_H

#include <agrum/BN/inference/tools/samplingInference.h>

namespace gum {

  /**
   * @class WeightedInference weightedInference.h
   *<agrum/BN/inference/weightedInference.h>
   * @brief class for making Weighted sampling inference in bayesian networks.
   * @ingroup bn_approximation
   *
   * This class overrides pure function declared in the inherited class
   *ApproximateInference.
   * It defines the way Weighted sampling draws a sample.
   *
   */

  template < typename GUM_SCALAR >
  class WeightedSampling : public SamplingInference< GUM_SCALAR > {
    public:
    /**
     * Default constructor
     */
    explicit WeightedSampling(const IBayesNet< GUM_SCALAR >* bn);

    /**
     * Destructor
     */
    ~WeightedSampling() override;

    protected:
    /// draws a defined number of samples without updating the estimators
    Instantiation _burnIn() override;

    /// draws a sample according to Weighted sampling
    /**
     * @param w the weight of sample being generated
     * @param prev the previous sample generated
     * @param bn the bayesian network containing the evidence
     * @param hardEvNodes hard evidence nodes
     * @param hardEv hard evidences values
     *
     * Generates a new sample in topological order. Each sample has a weight bias.
     * The sample weight is the product of each node's weight.
     *
     */
    Instantiation _draw(GUM_SCALAR* w, Instantiation prev) override;
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class WeightedSampling< double >;
#endif
}   // namespace gum

#include <agrum/BN/inference/weightedSampling_tpl.h>

#endif
