/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
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
 * @brief This file contains Weighted sampling class definition.
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN(_at_LIP6)
 */


#ifndef GUM_WEIGHTED_INFERENCE_H
#define GUM_WEIGHTED_INFERENCE_H

#include <agrum/BN/inference/tools/samplingInference.h>

namespace gum {

  /**
   * @class WeightedInference weightedInference.h
   *<agrum/BN/inference/weightedInference.h>
   * @brief class for making Weighted sampling inference in Bayesian networks.
   * @ingroup bn_approximation
   *
   * This class overrides pure function declared in the inherited class
   *ApproximateInference.
   * It defines the way Weighted sampling draws a sample.
   *
   */

  template < typename GUM_SCALAR >
  class WeightedSampling: public SamplingInference< GUM_SCALAR > {
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
    Instantiation burnIn_() override;

    /// draws a sample according to Weighted sampling
    /**
     * @param w the weight of sample being generated
     * @param prev the previous sample generated
     * @param bn the Bayesian network containing the evidence
     * @param hardEvNodes hard evidence nodes
     * @param hardEv hard evidences values
     *
     * Generates a new sample in topological order. Each sample has a weight bias.
     * The sample weight is the product of each node's weight.
     *
     */
    Instantiation draw_(GUM_SCALAR* w, Instantiation prev) override;
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class WeightedSampling< double >;
#endif
}   // namespace gum

#include <agrum/BN/inference/weightedSampling_tpl.h>

#endif
