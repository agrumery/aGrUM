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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


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

#include <agrum/base/core/approximations/approximationScheme.h>
#include <agrum/BN/BayesNet.h>

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
