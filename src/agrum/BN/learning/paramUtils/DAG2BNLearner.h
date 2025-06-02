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

#include <agrum/base/core/approximations/approximationSchemeListener.h>
#include <agrum/BN/learning/paramUtils/EMApproximationScheme.h>
#include <agrum/BN/learning/paramUtils/paramEstimator.h>

namespace gum {

  namespace learning {

    /** @class DAG2BNLearner
     * @brief A class that, given a structure and a parameter estimator returns
     * a full Bayes net
     * @headerfile DAG2BNLearner.h <agrum/BN/learning/paramUtils/DAG2BNLearner.h>
     * @ingroup learning_param_utils
     */
    class DAG2BNLearner: public EMApproximationScheme {
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

      /**
       * sets the noise amount used to perturb the initial CPTs used by EM
       * @param noise When EM starts, it initializes all the CPTs of the Bayes
       * net. EM adds a noise to these CPTs by mixing their values with some
       * random noise. The formula used is, up to some normalizing constant:
       * new_cpt = (1-noise) * cpt + noise * random_cpt(). Of course, noise must
       * belong to interval [0,1].
       */
      DAG2BNLearner& setNoise(const double noise);

      /// create a BN from a DAG using a one pass generator (typically ML)
      template < typename GUM_SCALAR = double >
      static BayesNet< GUM_SCALAR > createBN(ParamEstimator& estimator, const DAG& dag);

      /**
       * @brief creates a BN with a given structure (dag) using the EM algorithm
       * @param bootstrap_estimator the ParamEstimator used to initialize EM
       * @param EM_estimator the ParamEstimator used in all the EM steps (except
       *        the initialization). It is used in a loop until the stopping
       *        condition is met (max of relative error on every parameter<epsilon)
       * @param dag the graphical structure of the returned BN
       * @return a BN learnt by EM whose graphical structure is dag
       */
      template < typename GUM_SCALAR = double >
      BayesNet< GUM_SCALAR > createBNwithEM(ParamEstimator& bootstrap_estimator,
                                            ParamEstimator& EM_estimator,
                                            const DAG&      dag);

      /**
       * @brief creates a BN using the EM algorithm with the structure specified by
       *        bn, initialized by the parameters of bn
       * @param bn a Bayes Net that is used both for specifying the structure of
       *        the returned BN and for initializing the parameters for the EM
       *        algorithm. CPTs filled exclusively with only zeroes are
       *        initialized directly the BNLearner using the bootstrap_estimator.
       *        @warning: the bn is copied, so that it is unmodified by EM. If you
       *        do not care about its current value, prefer using the
       *        createBNwithEM with the ref ref parameter
       * @param bootstrap_estimator the ParamEstimator used to initialize the CPTs
       *        of bn when those are filled exclusively with zeroes
       * @param EM_estimator the ParamEstimator used in all the EM steps (except
       *        the initialization). It is used in a loop until the stopping
       *        condition is met (max of relative error on every parameter<epsilon)
       * @return a BN whose structure is the same as bn and whose parameters are
       *        learnt by EM.
       */
      template < typename GUM_SCALAR >
      BayesNet< GUM_SCALAR > createBNwithEM(ParamEstimator&               bootstrap_estimator,
                                            ParamEstimator&               EM_estimator,
                                            const BayesNet< GUM_SCALAR >& bn);

      /**
       * @brief creates a BN using the EM algorithm with the structure specified by
       *        bn, initialized by the parameters of bn
       * @param bn a Bayes Net that is used both for specifying the structure of
       *        the returned BN and for initializing the parameters for the EM
       *        algorithm. CPTs filled exclusively with only zeroes are
       *        initialized directly the BNLearner using the bootstrap_estimator.
       *        @warning: the bn is modified by EM. If you do not want its value to
       *        be altered, prefer using the createBNwithEM with the const ref parameter
       * @param bootstrap_estimator the ParamEstimator used to initialize the CPTs
       *        of bn when those are filled exclusively with zeroes
       * @param EM_estimator the ParamEstimator used in all the EM steps (except
       *        the initialization). It is used in a loop until the stopping
       *        condition is met (max of relative error on every parameter<epsilon)
       * @return a BN whose structure is the same as bn and whose parameters are
       *        learnt by EM.
       */
      template < typename GUM_SCALAR >
      BayesNet< GUM_SCALAR > createBNwithEM(ParamEstimator&          bootstrap_estimator,
                                            ParamEstimator&          EM_estimator,
                                            BayesNet< GUM_SCALAR >&& bn);

      /// returns the approximation policy of the EM learning algorithm
      EMApproximationScheme& approximationScheme();

      /// @}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

      private:
      /**
       * @brief the noise factor used to perturb the CPTs during EM's initialization
       *
       * When EM starts, it initializes all the CPTs of the Bayes
       * net. EM adds a noise to these CPTs by mixing their values with some
       * random noise. The formula used is, up to some normalizing constant:
       * new_cpt = (1-noise) * cpt + noise * random_cpt().
       */
      double noiseEM_{0.1};

      /// keep trak of the log-likelihood when performing EM
      /**
       * Each time we call _createBN_, we can ask this method to compute the
       * corresponding log-likelihood as well. In this case, this log-likelihood
       * is stored into attribute log_likelihood_EM_
       */
      double log_likelihood_EM_ {0.0};

      /**
       * It may be the case that EM is stuck into decreasing the log-likelihood
       * instead of increasing it. Fortunately, it is most often not the case.
       * But it can happen, see Table 5 on p28 of
       * https://faculty.washington.edu/fxia/courses/LING572/EM_collins97.pdf
       * So, to avoid iterating many times EM while always increasing the
       * log-likelihood, we limit the number of consecutive times the
       * log-likelihood is allowed to be increased.
       */
      unsigned int max_nb_dec_likelihood_iter_ {3};

      /// copy a tensor into another whose variables' sequence differs
      /** The variables of both tensor should be the same, only their
       * order differs */
      template < typename GUM_SCALAR = double >
      static void _probaVarReordering_(gum::Tensor< GUM_SCALAR >&       pot,
                                       const gum::Tensor< GUM_SCALAR >& other_pot);

      /// create a BN from a DAG using a one pass generator (typically ML) and
      /**
       * @brief create a BN from a DAG using a one pass generator (typically ML) and
       * compute the log-likelihood if needed (for EM)
       * @param estimator the ParamEstimator used to fill the BN's CPTs
       * @param dag the graphical structure of the returned BN
       * @param compute_log_likelihood a Boolean indicating whether we should also
       * compute the log-likelihood. In this case, this one is stored into the
       * log_likelihood_EM_ attribute
       * @return the Bayes net whose structure is dag and whose CPT's parameters
       * are learnt from the estimator
       */
      template < typename GUM_SCALAR = double >
      BayesNet< GUM_SCALAR > _createBN_(ParamEstimator& estimator,
                                        const DAG&      dag,
                                        const bool      compute_log_likelihood);
      /**
       * @brief performs the loops of the EM algorithm
       * @param EM_estimator the ParamEstimator used to estimate the values of the CPTs
       * @param bn a Bayes net that both specifies the graphical structure of the
       *        returned Bayes net and contains the initialization of the CPTs used
       *        when EM starts
       * @return a Bayes net whose structure is identical to that of bn, and whose
       *        parameters have been learnt by the EM algorithm
       */
      template < typename GUM_SCALAR >
      BayesNet< GUM_SCALAR > _performEM_(ParamEstimator&          bootstrap_estimator,
                                         ParamEstimator&          EM_estimator,
                                         BayesNet< GUM_SCALAR >&& bn);



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
