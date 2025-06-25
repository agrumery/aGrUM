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


/** @file
 * @brief A class for parameterizing EM's parameter learning approximations
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_EM_APPROX_SCHEME_H
#define GUM_LEARNING_EM_APPROX_SCHEME_H

#include <algorithm>
#include <string>
#include <vector>

#include <agrum/agrum.h>

#include <agrum/base/core/approximations/approximationScheme.h>
#include <agrum/base/core/approximations/approximationSchemeListener.h>

namespace gum {

  namespace learning {

    /**
     * @class EMApproximationScheme
     * @brief A class for parameterizing EM's parameter learning approximations
     * @ingroup learning_param_utils
     */
    class EMApproximationScheme: public ApproximationScheme {
      public:
      /**
       * @brief initializes the EM parameter learning approximation with the min rate criterion
       * @param verbosity if set to true, EM will keep track of its history
       */
      explicit EMApproximationScheme(bool verbosity = false) : ApproximationScheme(verbosity) {
        ApproximationScheme::enableMinEpsilonRate();
        ApproximationScheme::disableEpsilon();
      }

      virtual ~EMApproximationScheme() {}

      /**
       * @brief sets the stopping criterion of EM as being the minimal difference between two
       * consecutive log-likelihoods
       * @param eps the log-likelihood difference below which EM stops its iterations
       * @warning setting this stopping criterion disables the min rate criterion (if it was
       * enabled)
       * @throw OutOfBounds Raised if eps <= 0
       */
      void setEpsilon(double eps) override {
        if (eps <= 0)
          GUM_ERROR(OutOfBounds, "EM's min diff epsilon value must be strictly positive")
        ApproximationScheme::setEpsilon(eps);
        ApproximationScheme::disableMinEpsilonRate();
      }

      /**
       * @brief sets the stopping criterion of EM as being the minimal difference between two
       * consecutive log-likelihoods
       * @param eps the log-likelihood difference below which EM stops its iterations
       * @warning setting this stopping criterion disables the min rate criterion (if it was
       * enabled)
       * @throw OutOfBounds Raised if eps <= 0
       */
      void setMinDiffEpsilon(double eps) { setEpsilon(eps); }

      /**
       * @brief sets the stopping criterion of EM as being the minimal log-likelihood's evolution
       * rate
       * @param rate the log-likelihood evolution rate below which EM stops its iterations
       * @warning setting this stopping criterion disables the min diff criterion (if it was
       * enabled)
       * @throw OutOfBounds if rate<0
       */
      void setMinEpsilonRate(double rate) override {
        if (rate <= 0.0)
          GUM_ERROR(OutOfBounds, "EM's min log-likelihood evolution rate must be strictly positive")
        ApproximationScheme::setMinEpsilonRate(rate);
        ApproximationScheme::disableEpsilon();
      }
    };

  }   // namespace learning

}   // namespace gum

#endif   // GUM_LEARNING_EM_APPROX_SCHEME_H
