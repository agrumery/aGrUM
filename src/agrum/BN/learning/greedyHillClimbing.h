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
 * @brief The greedy hill learning algorithm (for directed graphs)
 *
 * The GreedyHillClimbing class implements a greedy search in which the only
 * the graph changes that increase the global score are applied. Those that
 * increase it the more are applied first. The algorithm stops when no single
 * change can increase the score anymore.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_GREEDY_HILL_CLIMBING_H
#define GUM_LEARNING_GREEDY_HILL_CLIMBING_H

#include <string>
#include <vector>

#include <agrum/BN/BayesNet.h>
#include <agrum/agrum.h>
#include <agrum/tools/core/approximations/approximationScheme.h>

namespace gum {

  namespace learning {

    /**
     * @class GreedyHillClimbing
     * @brief The greedy hill climbing learning algorithm (for directed graphs)
     *
     * The GreedyHillClimbing class implements a greedy search in which the
     * only the graph changes that increase the global score are applied. Those
     * that increase it the more are applied first. The algorithm stops when no
     * single change can increase the score anymore.
     * @ingroup learning_group
     */
    class GreedyHillClimbing: public ApproximationScheme {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      GreedyHillClimbing();

      /// copy constructor
      GreedyHillClimbing(const GreedyHillClimbing& from);

      /// move constructor
      GreedyHillClimbing(GreedyHillClimbing&& from);

      /// destructor
      ~GreedyHillClimbing();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      GreedyHillClimbing& operator=(const GreedyHillClimbing& from);

      /// move operator
      GreedyHillClimbing& operator=(GreedyHillClimbing&& from);

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// returns the approximation policy of the learning algorithm
      ApproximationScheme& approximationScheme();

      /// learns the structure of a Bayes net
      /** @param selector A selector class that computes the best changes that
       * can be applied and that enables the user to get them very easily.
       * Typically, the selector is a GraphChangesSelector4DiGraph<SCORE,
       * STRUCT_CONSTRAINT, GRAPH_CHANGES_GENERATOR>.
       * @param initial_dag the DAG we start from for our learning */
      template < typename GRAPH_CHANGES_SELECTOR >
      DAG learnStructure(GRAPH_CHANGES_SELECTOR& selector, DAG initial_dag = DAG());

      /// learns the structure and the parameters of a BN
      /** @param selector A selector class that computes the best changes that
       * can be applied and that enables the user to get them very easily.
       * Typically, the selector is a GraphChangesSelector4DiGraph<SCORE,
       * STRUCT_CONSTRAINT, GRAPH_CHANGES_GENERATOR>.
       * @param estimator A estimator.
       * @param initial_dag the DAG we start from for our learning */
      template < typename GUM_SCALAR = double,
                 typename GRAPH_CHANGES_SELECTOR,
                 typename PARAM_ESTIMATOR >
      BayesNet< GUM_SCALAR > learnBN(GRAPH_CHANGES_SELECTOR& selector,
                                     PARAM_ESTIMATOR&        estimator,
                                     DAG                     initial_dag = DAG());

      /// @}
    };

  } /* namespace learning */

} /* namespace gum */

/// always include templated methods
#include <agrum/BN/learning/greedyHillClimbing_tpl.h>

#endif /* GUM_LEARNING_GREEDY_HILL_CLIMBING_H */
