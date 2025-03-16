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
     * For every CPT in the BN, epsilon is added to each tensor value before
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
