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
 * @brief The local search learning with tabu list algorithm (for directed
 * graphs)
 *
 * The LocalSearchWithTabuList class implements a greedy search in which we
 * allow applying at most N consecutive graph changes that decrease the score.
 * To prevent infinite loops, when using local search, you should use a
 * structural constraint that includes a tabu list of at least N elements.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_LOCAL_SEARCH_WITH_TABU_LIST_H
#define GUM_LEARNING_LOCAL_SEARCH_WITH_TABU_LIST_H

#include <string>
#include <vector>

#include <agrum/BN/BayesNet.h>
#include <agrum/tools/core/approximations/approximationScheme.h>

namespace gum {

  namespace learning {

    /** @class LocalSearchWithTabuList
     * @brief The local search with tabu list learning algorithm (for
     * directed graphs)
     *
     * The LocalSearchWithTabuList class implements a greedy search in which we
     * allow applying at most N consecutive graph changes that decrease the
     *score.
     * To prevent infinite loops, when using local search, you should use a
     * structural constraint that includes a tabu list of at least N elements.
     *
     * @ingroup learning_group
     */
    class LocalSearchWithTabuList: public ApproximationScheme {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      LocalSearchWithTabuList();

      /// copy constructor
      LocalSearchWithTabuList(const LocalSearchWithTabuList& from);

      /// move constructor
      LocalSearchWithTabuList(LocalSearchWithTabuList&& from);

      /// destructor
      virtual ~LocalSearchWithTabuList();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      LocalSearchWithTabuList& operator=(const LocalSearchWithTabuList& from);

      /// move operator
      LocalSearchWithTabuList& operator=(LocalSearchWithTabuList&& from);

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// returns the approximation policy of the learning algorithm
      ApproximationScheme& approximationScheme();

      /// set the max number of changes decreasing the score that we allow to
      /// apply
      void setMaxNbDecreasingChanges(Size nb);

      /// learns the structure of a Bayes net
      /** @param selector A selector class that computes the best changes that
       * can be applied and that enables the user to get them very easily.
       * Typically, the selector is a GraphChangesSelector4DiGraph<SCORE,
       * STRUCT_CONSTRAINT, GRAPH_CHANGES_GENERATOR>.
       * @param initial_dag the DAG we start from for our learning */
      template < typename GRAPH_CHANGES_SELECTOR >
      DAG learnStructure(GRAPH_CHANGES_SELECTOR& selector, DAG initial_dag = DAG());

      /// learns the structure and the parameters of a BN
      template < typename GUM_SCALAR = double,
                 typename GRAPH_CHANGES_SELECTOR,
                 typename PARAM_ESTIMATOR >
      BayesNet< GUM_SCALAR > learnBN(GRAPH_CHANGES_SELECTOR& selector,
                                     PARAM_ESTIMATOR&        estimator,
                                     DAG                     initial_dag = DAG());

      /// @}

      private:
      /// the max number of changes decreasing the score that we allow to apply
      Size _MaxNbDecreasing_{2};
    };

  } /* namespace learning */

} /* namespace gum */

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/BN/learning/localSearchWithTabuList_inl.h>
#endif /* GUM_NO_INLINE */

/// always include templated methods
#include <agrum/BN/learning/localSearchWithTabuList_tpl.h>

#endif /* GUM_LEARNING_LOCAL_SEARCH_WITH_TABU_LIST_H */
