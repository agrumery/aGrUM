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
 * @brief This file contains general methods for approximate inference.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_APPROXIMATE_INFERENCE_H
#define GUM_APPROXIMATE_INFERENCE_H

#include <agrum/BN/inference/tools/marginalTargetedInference.h>
#include <agrum/tools/core/approximations/approximationScheme.h>

namespace gum {

  template < typename GUM_SCALAR >
  class ApproximateInference:
      public MarginalTargetedInference< GUM_SCALAR >,
      public ApproximationScheme {
    public:
    explicit ApproximateInference(const IBayesNet< GUM_SCALAR >* bn) :
        MarginalTargetedInference< GUM_SCALAR >(bn){};
  };
}   // namespace gum

#endif   // GUM_APPROXIMATE_INFERENCE_H
