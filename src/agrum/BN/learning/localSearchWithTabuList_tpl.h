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

#pragma once


/** @file
 * @brief The local search with tabu list learning algorithm (for directed
 *graphs)
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/BN/learning/paramUtils/DAG2BNLearner.h>
#include <agrum/BN/learning/structureUtils/graphChange.h>

namespace gum {

  namespace learning {

    /// learns the structure of a Bayes net
    template < typename GRAPH_CHANGES_SELECTOR >
    DAG LocalSearchWithTabuList::learnStructure(GRAPH_CHANGES_SELECTOR& selector, DAG dag) {
      selector.setGraph(dag);

      unsigned int nb_changes_applied                 = 0;
      Idx          applied_change_with_positive_score = 0;
      Idx          current_N                          = 0;

      initApproximationScheme();

      // the best dag found so far with its score
      DAG    best_dag      = dag;
      double best_score    = 0;
      double current_score = 0;
      double delta_score   = 0;

      do {
        applied_change_with_positive_score = 0;
        delta_score                        = 0;

        try {
          const auto& change = selector.bestChange();
          delta_score        = selector.deltaScore(change, true);

          std::cout << change << "  == score = " << delta_score << std::endl;

          if ((nb_changes_applied > 0) || (delta_score > 0)) {
            // std::cout << "applying " << change.toString()
            //           << " delta score = " << delta_score << std::endl;

            if (delta_score > 0) {
              ++applied_change_with_positive_score;
            } else if (current_score > best_score) {
              best_score = current_score;
              best_dag   = dag;
            }

            selector.applyChange(change);
            current_score += delta_score;
            ++nb_changes_applied;
          }

          updateApproximationScheme(nb_changes_applied);

          // update current_N
          if (applied_change_with_positive_score) {
            current_N          = 0;
            nb_changes_applied = 0;
          } else {
            ++current_N;
          }
        } catch (NotFound&) { break; }
      } while ((current_N <= _MaxNbDecreasing_) && continueApproximationScheme(delta_score));

      stopApproximationScheme();   // just to be sure of the approximationScheme
                                   // has been notified of the end of loop

      // get the dag that we will return
      auto& res_dag = current_score > best_score ? dag : best_dag;

      // here, we add to the dag the set of nodes that were removed by method
      // setGraph because they did not belong to the database
      selector.finalizeGraph(res_dag);

      return res_dag;
    }

    /// learns the structure and the parameters of a BN
    template < GUM_Numeric GUM_SCALAR, typename GRAPH_CHANGES_SELECTOR, typename PARAM_ESTIMATOR >
    BayesNet< GUM_SCALAR > LocalSearchWithTabuList::learnBN(GRAPH_CHANGES_SELECTOR& selector,
                                                            PARAM_ESTIMATOR&        estimator,
                                                            DAG                     initial_dag) {
      return DAG2BNLearner::createBN< GUM_SCALAR >(estimator,
                                                   learnStructure(selector, initial_dag));
    }

  } /* namespace learning */

} /* namespace gum */
