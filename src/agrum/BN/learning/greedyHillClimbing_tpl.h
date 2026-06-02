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
 * @brief The greedy hill learning algorithm (for directed graphs)
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/BN/learning/paramUtils/DAG2BNLearner.h>
#include <agrum/BN/learning/structureUtils/graphChange.h>

namespace gum {

  namespace learning {

    /// learns the structure of a Bayes net
    template < typename GRAPH_CHANGES_SELECTOR >
    DAG GreedyHillClimbing::learnStructure(GRAPH_CHANGES_SELECTOR& selector, DAG dag) {
      selector.setGraph(dag);

      unsigned int nb_changes_applied = 1;
      double       delta_score;

      initApproximationScheme();

      do {
        nb_changes_applied = 0;
        delta_score        = 0;

        try {
          const auto& change = selector.bestChange();
          delta_score        = selector.deltaScore(change, true);

          if (delta_score > 0) {
            // std::cout << "applying " << change.toString()
            //           << " delta score = " << delta_score << std::endl;
            selector.applyChange(change);
            nb_changes_applied = 1;
            updateApproximationScheme(nb_changes_applied);
          }
        } catch (NotFound&) {}
      } while (nb_changes_applied && continueApproximationScheme(delta_score));

      stopApproximationScheme();   // just to be sure of the approximationScheme
                                   // has been notified of the end of loop

      // here, we add to the dag the set of nodes that were removed by method
      // setGraph because they did not belong to the database
      selector.finalizeGraph(dag);

      return dag;
    }

    /// learns the structure and the parameters of a BN
    template < GUM_Numeric GUM_SCALAR, typename GRAPH_CHANGES_SELECTOR, typename PARAM_ESTIMATOR >
    BayesNet< GUM_SCALAR > GreedyHillClimbing::learnBN(GRAPH_CHANGES_SELECTOR& selector,
                                                       PARAM_ESTIMATOR&        estimator,
                                                       DAG                     initial_dag) {
      return DAG2BNLearner::createBN< GUM_SCALAR >(estimator,
                                                   learnStructure(selector, initial_dag));
    }

  } /* namespace learning */

} /* namespace gum */
