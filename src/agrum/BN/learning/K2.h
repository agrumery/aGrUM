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


/** @file
 * @brief The K2 algorithm
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_K2_H
#define GUM_LEARNING_K2_H

#include <string>
#include <vector>

#include <agrum/BN/learning/greedyHillClimbing.h>

namespace gum {

  namespace learning {

    /** @class K2
     * @brief The K2 algorithm
     * @ingroup learning_group
     */
    class K2: private GreedyHillClimbing {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      K2();

      /// copy constructor
      K2(const K2& from);

      /// move constructor
      K2(K2&& from);

      /// destructor
      ~K2();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      K2& operator=(const K2& from);

      /// move operator
      K2& operator=(K2&& from);

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// returns the approximation policy of the learning algorithm
      ApproximationScheme& approximationScheme();

      /// sets the order on the variables
      void setOrder(const Sequence< NodeId >& order);

      /// sets the order on the variables
      void setOrder(const std::vector< NodeId >& order);

      /// returns the current order
      const Sequence< NodeId >& order() const noexcept;

      /// learns the structure of a Bayes net
      /**
       * @param selector A selector class that computes the best changes that
       * can be applied and that enables the user to get them very easily.
       * Typically, the selector is a GraphChangesSelector4DiGraph<SCORE,
       * STRUCT_CONSTRAINT, GRAPH_CHANGES_GENERATOR>.
       * @param initial_dag the DAG we start from for our learning */
      template < typename GRAPH_CHANGES_SELECTOR >
      DAG learnStructure(GRAPH_CHANGES_SELECTOR& selector, DAG initial_dag = DAG());

      /// learns the structure and the parameters of a BN
      template < typename GUM_SCALAR, typename GRAPH_CHANGES_SELECTOR, typename PARAM_ESTIMATOR >
      BayesNet< GUM_SCALAR > learnBN(GRAPH_CHANGES_SELECTOR& selector,
                                     PARAM_ESTIMATOR&        estimator,
                                     DAG                     initial_dag = DAG());

      private:
      /// the order on the variable used for learning
      Sequence< NodeId > _order_;

      /** @brief checks that the order passed to K2 is coherent with the
       * variables
       * as specified by their modalities */
      void _checkOrder_(const std::vector< Size >& modal);
      /// @}
    };

  } /* namespace learning */

} /* namespace gum */

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/BN/learning/K2_inl.h>
#endif /* GUM_NO_INLINE */

/// always include templated methods
#include <agrum/BN/learning/K2_tpl.h>

#endif /* GUM_LEARNING_K2_H */
