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
 * @brief Implements approximate inference algorithms from Loopy Belief
 * Propagation.
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN(_at_LIP6)
 *
 */


#include <agrum/BN/inference/loopySamplingInference.h>

#define DEFAULT_VIRTUAL_LBP_SIZE 5000

namespace gum {


  template < typename GUM_SCALAR, template < typename > class APPROX >
  LoopySamplingInference< GUM_SCALAR, APPROX >::LoopySamplingInference(
     const IBayesNet< GUM_SCALAR >* BN) :
      APPROX< GUM_SCALAR >(BN),
      virtualLBPSize_(DEFAULT_VIRTUAL_LBP_SIZE) {
    GUM_CONSTRUCTOR(LoopySamplingInference);
  }


  template < typename GUM_SCALAR, template < typename > class APPROX >
  LoopySamplingInference< GUM_SCALAR, APPROX >::~LoopySamplingInference() {
    GUM_DESTRUCTOR(LoopySamplingInference);
  }


  template < typename GUM_SCALAR, template < typename > class APPROX >
  void LoopySamplingInference< GUM_SCALAR, APPROX >::makeInference_() {
    LoopyBeliefPropagation< GUM_SCALAR > lbp(&this->BN());
    for (const auto& x: this->hardEvidence()) {
      lbp.addEvidence(x.first, x.second);
    }
    lbp.makeInference();

    if (!this->isSetEstimator) { this->setEstimatorFromLBP_(&lbp, virtualLBPSize_); }

    this->loopApproxInference_();
  }
}   // namespace gum
