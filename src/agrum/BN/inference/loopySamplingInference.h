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
 * @brief This file implements a Hybrid sampling class using LoopyBeliefPropagation
 * and
 * an approximate Inference method.
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_LOOPY_INFERENCE_H
#define GUM_LOOPY_INFERENCE_H
#include <agrum/BN/inference/GibbsSampling.h>
#include <agrum/BN/inference/importanceSampling.h>
#include <agrum/BN/inference/MonteCarloSampling.h>
#include <agrum/BN/inference/weightedSampling.h>

namespace gum {

  /**
   * @class LoopySamplingInference LoopySamplingInference.h
   *<agrum/BN/inference/loopySamplingInference.h>
   * @brief class for making hybrid sampling inference with loopy belief
   *propagation and an approximation inference method in Bayesian networks.
   * @ingroup bn_approximation
   *
   * This class inherits of template class APPROX, which SHOULD be one of the 4
   * approximate inference methods (MonteCarlo, Weighted, Importance, Gibbs).
   * It makes the inference with respect to the inherited class' method, after
   * having initialized the estimators with the posteriors obtained by running
   * LoopyBeliefPropagation algorithm.
   *
   */

  template < typename GUM_SCALAR, template < typename > class APPROX >
  class LoopySamplingInference: public APPROX< GUM_SCALAR > {
    public:
    /**
     * Default constructor
     */
    explicit LoopySamplingInference(const IBayesNet< GUM_SCALAR >* bn);

    /**
     * destructor
     */
    virtual ~LoopySamplingInference();

    /// makes the inference by generating samples w.r.t the mother class' sampling
    /// method after initalizing  estimators with loopy belief propagation
    virtual void makeInference_();

    void setVirtualLBPSize(GUM_SCALAR vlbpsize) {
      if (vlbpsize > 0) virtualLBPSize_ = vlbpsize;
    };

    protected:
    GUM_SCALAR virtualLBPSize_;
  };

#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#    ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#      ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#        ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#          ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#            ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#              ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class LoopySamplingInference< float, WeightedSampling >;
#              endif
#            endif
#          endif
#        endif
#      endif
#    endif
#  endif
#endif
#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#    ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#      ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#        ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#          ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#            ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#              ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class LoopySamplingInference< double, WeightedSampling >;
#              endif
#            endif
#          endif
#        endif
#      endif
#    endif
#  endif
#endif

#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#    ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#      ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#        ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#          ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#            ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#              ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class LoopySamplingInference< float, ImportanceSampling >;
#              endif
#            endif
#          endif
#        endif
#      endif
#    endif
#  endif
#endif
#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#    ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#      ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#        ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#          ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#            ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#              ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class LoopySamplingInference< double, ImportanceSampling >;
#              endif
#            endif
#          endif
#        endif
#      endif
#    endif
#  endif
#endif

#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#    ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#      ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#        ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#          ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#            ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#              ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class LoopySamplingInference< float, MonteCarloSampling >;
#              endif
#            endif
#          endif
#        endif
#      endif
#    endif
#  endif
#endif
#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#    ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#      ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#        ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#          ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#            ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#              ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class LoopySamplingInference< double, MonteCarloSampling >;
#              endif
#            endif
#          endif
#        endif
#      endif
#    endif
#  endif
#endif

#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#    ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#      ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#        ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#          ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#            ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#              ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class LoopySamplingInference< float, GibbsSampling >;
#              endif
#            endif
#          endif
#        endif
#      endif
#    endif
#  endif
#endif
#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#    ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#      ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#        ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#          ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#            ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#              ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class LoopySamplingInference< double, GibbsSampling >;
#              endif
#            endif
#          endif
#        endif
#      endif
#    endif
#  endif
#endif

  template < typename GUM_SCALAR >
  using HybridMonteCarloSampling = LoopySamplingInference< GUM_SCALAR, MonteCarloSampling >;
  template < typename GUM_SCALAR >
  using HybridWeightedSampling = LoopySamplingInference< GUM_SCALAR, WeightedSampling >;
  template < typename GUM_SCALAR >
  using HybridImportanceSampling = LoopySamplingInference< GUM_SCALAR, ImportanceSampling >;
  template < typename GUM_SCALAR >
  using HybridGibbsSampling = LoopySamplingInference< GUM_SCALAR, GibbsSampling >;
}   // namespace gum

#include <agrum/BN/inference/loopySamplingInference_tpl.h>
#endif
