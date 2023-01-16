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
 * @brief This file implements a Hybrid sampling class using LoopyBeliefPropagation
 * and
 * an approximate Inference method.
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_LOOPY_INFERENCE_H
#define GUM_LOOPY_INFERENCE_H
#include <agrum/BN/inference/GibbsSampling.h>
#include <agrum/BN/inference/MonteCarloSampling.h>
#include <agrum/BN/inference/importanceSampling.h>
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
