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
 * @brief A class that, given a structure and a parameter estimator returns a
 * full Bayes net
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_DAG_2_BN_LEARNER_H
#define GUM_LEARNING_DAG_2_BN_LEARNER_H

#include <algorithm>
#include <string>
#include <vector>

#include <agrum/agrum.h>

#include <agrum/base/core/approximations/approximationScheme.h>
#include <agrum/base/core/approximations/approximationSchemeListener.h>
#include <agrum/BN/learning/paramUtils/paramEstimator.h>

namespace gum {

  namespace learning {

    /** @class DAG2BNLearner
     * @brief A class that, given a structure and a parameter estimator returns
     * a full Bayes net
     * @headerfile DAG2BNLearner.h <agrum/BN/learning/paramUtils/DAG2BNLearner.h>
     * @ingroup learning_param_utils
     */
    class DAG2BNLearner: public ApproximationScheme {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      DAG2BNLearner();

      /// copy constructor
      DAG2BNLearner(const DAG2BNLearner& from);

      /// move constructor
      DAG2BNLearner(DAG2BNLearner&& from);

      /// virtual copy constructor
      virtual DAG2BNLearner* clone() const;

      /// destructor
      virtual ~DAG2BNLearner();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      DAG2BNLearner& operator=(const DAG2BNLearner& from);

      /// move operator
      DAG2BNLearner& operator=(DAG2BNLearner&& from);

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// create a BN from a DAG using a one pass generator (typically ML)
      template < typename GUM_SCALAR = double >
      static BayesNet< GUM_SCALAR > createBN(ParamEstimator& estimator, const DAG& dag);

      /// create a BN from a DAG using a two pass generator (typically EM)
      /** The bootstrap estimator is used once to provide an inital BN. This
       * one is used by the second estimator. The later is exploited in a loop
       * until the stopping condition is met (max of relative error on every
       * parameter<epsilon) */
      template < typename GUM_SCALAR = double >
      BayesNet< GUM_SCALAR > createBN(ParamEstimator& bootstrap_estimator,
                                      ParamEstimator& general_estimator,
                                      const DAG&      dag);

      /// returns the approximation policy of the learning algorithm
      ApproximationScheme& approximationScheme();

      /// @}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

      private:
      /// copy a tensor into another whose variables' sequence differs
      /** The variables of both tensor should be the same, only their
       * order differs */
      template < typename GUM_SCALAR = double >
      static void _probaVarReordering_(gum::Tensor< GUM_SCALAR >&       pot,
                                       const gum::Tensor< GUM_SCALAR >& other_pot);

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
    };

  } /* namespace learning */

} /* namespace gum */

/// always include templated methods
#include <agrum/BN/learning/paramUtils/DAG2BNLearner_tpl.h>

// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/BN/learning/paramUtils/DAG2BNLearner_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_DAG_2_BN_LEARNER_H */
