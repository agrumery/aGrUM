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
 * @brief This file contains Gibbs sampling class definition.
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN(_at_LIP6)
 */


#ifndef GUM_GIBBS_SAMPLING_H
#define GUM_GIBBS_SAMPLING_H

#include <agrum/BN/inference/tools/samplingInference.h>

namespace gum {

  /**
   * @class GibbsSampling gibbsSampling.h
   *<agrum/BN/inference/gibbsSampling.h>
   * @brief class for making Gibbs sampling inference in Bayesian networks.
   * @ingroup bn_approximation
   *
   * This class overrides pure function declared in the inherited class
   *ApproximateInference.
   * It defines the way Gibbs sampling draws a sample. It also inherits
   *GibbsOperator
   * which contains Gibbs sampling methods.
   *
   */

  template < typename GUM_SCALAR >
  class GibbsSampling: public SamplingInference< GUM_SCALAR >, public GibbsOperator< GUM_SCALAR > {
    public:
    /**
     * Default constructor
     */
    explicit GibbsSampling(const IBayesNet< GUM_SCALAR >* bn);

    /**
     * Destructor
     */
    ~GibbsSampling() override;

    /**
     * @brief Number of burn in for one iteration.
     * @param b The number of burn in.
     * @throw OutOfBounds Raised if b < 1.
     */
    void setBurnIn(Size b) { this->burn_in_ = b; };

    /**
     * @brief Returns the number of burn in.
     * @return Returns the number of burn in.
     */
    Size burnIn() const { return this->burn_in_; };

    protected:
    /// draws a defined number of samples without updating the estimators
    Instantiation burnIn_() override;

    /// draws a sample given previous one according to Gibbs sampling
    /**
     * @param w the weight of sample being generated
     * @param prev the previous sample generated
     * @param bn the Bayesian network containing the evidence
     * @param hardEvNodes hard evidence nodes
     * @param hardEv hard evidences values
     *
     * Uses the Gibbs sampling method to generate a new sample given the previous
     * one. The method is implemented in the inherited class GibbsOperator. This
     * function only makes the call to it. It consists of choosing one node x to
     * sample, given the instantiation of all other nodes. It requires computing of
     * P( x \given instantiation_markovblanket(x)).
     */
    Instantiation draw_(GUM_SCALAR* w, Instantiation prev) override;

    /// draws a Monte Carlo sample
    /**
     * @param bn the reference Bayesian network
     *
     * This Monte Carlo sample generates a good starting point for Gibbs sampling,
     * because it returns
     * a sample consistent with the evidence, but it differs from the one
     * implemented in the inherited
     * class Approximate Inference because it also initializes attributes needed
     * for Gibbs sampling.
     */
    Instantiation monteCarloSample_();
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class GibbsSampling< double >;
#endif
}   // namespace gum

#include <agrum/BN/inference/GibbsSampling_tpl.h>
#endif
