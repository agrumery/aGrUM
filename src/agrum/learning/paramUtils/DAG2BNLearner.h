
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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


/** @file
 * @brief A class that, given a structure and a parameter estimator returns a
 * full Bayes net
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_DAG_2_BN_LEARNER_H
#define GUM_LEARNING_DAG_2_BN_LEARNER_H

#include <vector>

#include <agrum/agrum.h>
#include <agrum/core/approximations/approximationScheme.h>
#include <agrum/core/approximations/approximationSchemeListener.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/graphs/DAG.h>
#include <agrum/learning/paramUtils/paramEstimator.h>

namespace gum {

  namespace learning {

    /** @class DAG2BNLearner
     * @brief A class that, given a structure and a parameter estimator returns
     * a full Bayes net
     * @headerfile DAG2BNLearner.h <agrum/learning/paramUtils/DAG2BNLearner.h>
     * @ingroup learning_param_utils
     */
    template < template < typename > class ALLOC = std::allocator >
    class DAG2BNLearner
        : public ApproximationScheme
        , private ALLOC< NodeId > {
      public:
      /// type for the allocators passed in arguments of methods
      using allocator_type = ALLOC< NodeId >;

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      DAG2BNLearner(const allocator_type& alloc = allocator_type());

      /// copy constructor
      DAG2BNLearner(const DAG2BNLearner< ALLOC >& from);

      /// copy constructor with a given allocator
      DAG2BNLearner(const DAG2BNLearner< ALLOC >& from,
                    const allocator_type&         alloc);

      /// move constructor
      DAG2BNLearner(DAG2BNLearner< ALLOC >&& from);

      /// move constructor with a given allocator
      DAG2BNLearner(DAG2BNLearner< ALLOC >&& from, const allocator_type& alloc);

      /// virtual copy constructor
      virtual DAG2BNLearner< ALLOC >* clone() const;

      /// virtual copy constructor with a given allocator
      virtual DAG2BNLearner< ALLOC >* clone(const allocator_type& alloc) const;

      /// destructor
      virtual ~DAG2BNLearner();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      DAG2BNLearner< ALLOC >& operator=(const DAG2BNLearner< ALLOC >& from);

      /// move operator
      DAG2BNLearner< ALLOC >& operator=(DAG2BNLearner< ALLOC >&& from);

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// create a BN from a DAG using a one pass generator (typically ML)
      template < typename GUM_SCALAR = double >
      static BayesNet< GUM_SCALAR > createBN(ParamEstimator< ALLOC >& estimator,
                                             const DAG&               dag);

      /// create a BN from a DAG using a two pass generator (typically EM)
      /** The bootstrap estimator is used once to provide an inital BN. This
       * one is used by the second estimator. The later is exploited in a loop
       * until the stopping condition is met (max of relative error on every
       * parameter<epsilon) */
      template < typename GUM_SCALAR = double >
      BayesNet< GUM_SCALAR > createBN(ParamEstimator< ALLOC >& bootstrap_estimator,
                                      ParamEstimator< ALLOC >& general_estimator,
                                      const DAG&               dag);

      /// returns the approximation policy of the learning algorithm
      ApproximationScheme& approximationScheme();

      /// returns the allocator used by the score
      allocator_type getAllocator() const;

      /// @}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

      private:
      /// copy a potential into another whose variables' sequence differs
      /** The variables of both potential should be the same, only their
       * order differs */
      template < typename GUM_SCALAR = double >
      static void
         __probaVarReordering(gum::Potential< GUM_SCALAR >&       pot,
                              const gum::Potential< GUM_SCALAR >& other_pot);

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
    };

  } /* namespace learning */

} /* namespace gum */

/// always include templated methods
#include <agrum/learning/paramUtils/DAG2BNLearner_tpl.h>

#endif /* GUM_LEARNING_DAG_2_BN_LEARNER_H */
