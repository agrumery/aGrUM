/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
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
 * @brief The greedy thick-thinning learning algorithm (for directed graphs)
 *
 * The GreedyThickThinning class implements a two-phase greedy search:
 * - Thick phase: only arc additions are applied until no addition improves
 *   the score (produces a dense supergraph).
 * - Thin phase: only arc deletions (and optionally arc reversals) are applied
 *   until no such change improves the score.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_LEARNING_GREEDY_THICK_THINNING_H
#define GUM_LEARNING_GREEDY_THICK_THINNING_H

#include <string>
#include <vector>

#include <agrum/agrum.h>

#include <agrum/base/core/approximations/approximationScheme.h>
#include <agrum/BN/BayesNet.h>

namespace gum {

  namespace learning {

    /**
     * @class GreedyThickThinning
     * @brief The greedy thick-thinning learning algorithm (for directed graphs)
     *
     * Two-phase greedy structure learning:
     * 1. Thick phase — adds arcs greedily until score no longer improves.
     * 2. Thin phase  — removes arcs (and optionally reverses them) greedily
     *    until score no longer improves.
     *
     * @ingroup learning_group
     */
    class GreedyThickThinning: public ApproximationScheme {
      public:
      /// @name Constructors / Destructors
      /// @{

      /// default constructor
      GreedyThickThinning();

      /// copy constructor
      GreedyThickThinning(const GreedyThickThinning& from);

      /// move constructor
      GreedyThickThinning(GreedyThickThinning&& from);

      /// destructor
      ~GreedyThickThinning() override;

      /// @}

      /// @name Operators
      /// @{

      /// copy operator
      GreedyThickThinning& operator=(const GreedyThickThinning& from);

      /// move operator
      GreedyThickThinning& operator=(GreedyThickThinning&& from);

      /// @}

      /// @name Accessors / Modifiers
      /// @{

      /// returns the approximation policy of the learning algorithm
      ApproximationScheme& approximationScheme();

      /// enable or disable arc reversals during the thin phase (default: false)
      void setAllowReversalsInThinPhase(bool allow);

      /// returns whether arc reversals are allowed in the thin phase
      bool allowReversalsInThinPhase() const;

      /// learns the structure of a Bayes net
      /** @param selector A selector class that computes the best changes that
       * can be applied and that enables the user to get them very easily.
       * Typically, the selector is a GraphChangesSelector4DiGraph.
       * @param initial_dag the DAG we start from for our learning */
      template < typename GRAPH_CHANGES_SELECTOR >
      DAG learnStructure(GRAPH_CHANGES_SELECTOR& selector, DAG initial_dag = DAG());

      /// learns the structure and the parameters of a BN
      template < GUM_Numeric GUM_SCALAR = double,
                 typename GRAPH_CHANGES_SELECTOR,
                 typename PARAM_ESTIMATOR >
      BayesNet< GUM_SCALAR > learnBN(GRAPH_CHANGES_SELECTOR& selector,
                                     PARAM_ESTIMATOR&        estimator,
                                     DAG                     initial_dag = DAG());

      /// @}

      private:
      bool _allowReversalsInThinPhase_{false};
    };

  } /* namespace learning */

} /* namespace gum */

/// always include templated methods
#include <agrum/BN/learning/greedyThickThinning_tpl.h>

#endif /* GUM_LEARNING_GREEDY_THICK_THINNING_H */
