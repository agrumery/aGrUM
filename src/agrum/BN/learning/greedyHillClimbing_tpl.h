/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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

      // a vector that indicates which queues have valid scores, i.e., scores
      // that were not invalidated by previously applied changes
      std::vector< bool > impacted_queues(dag.size(), false);

      do {
        nb_changes_applied = 0;
        delta_score        = 0;

        std::vector< std::pair< NodeId, double > > ordered_queues
           = selector.nodesSortedByBestScore();

        for (Idx j = 0; j < dag.size(); ++j) {
          Idx i = ordered_queues[j].first;

          if (!(selector.empty(i)) && (selector.bestScore(i) > 0)) {
            // pick up the best change
            const GraphChange& change = selector.bestChange(i);

            // perform the change
            switch (change.type()) {
              case GraphChangeType::ARC_ADDITION:
                if (!impacted_queues[change.node2()] && selector.isChangeValid(change)) {
                  delta_score += selector.bestScore(i);
                  dag.addArc(change.node1(), change.node2());
                  impacted_queues[change.node2()] = true;
                  selector.applyChangeWithoutScoreUpdate(change);
                  ++nb_changes_applied;
                }

                break;

              case GraphChangeType::ARC_DELETION:
                if (!impacted_queues[change.node2()] && selector.isChangeValid(change)) {
                  delta_score += selector.bestScore(i);
                  dag.eraseArc(Arc(change.node1(), change.node2()));
                  impacted_queues[change.node2()] = true;
                  selector.applyChangeWithoutScoreUpdate(change);
                  ++nb_changes_applied;
                }

                break;

              case GraphChangeType::ARC_REVERSAL:
                if ((!impacted_queues[change.node1()]) && (!impacted_queues[change.node2()])
                    && selector.isChangeValid(change)) {
                  delta_score += selector.bestScore(i);
                  dag.eraseArc(Arc(change.node1(), change.node2()));
                  dag.addArc(change.node2(), change.node1());
                  impacted_queues[change.node1()] = true;
                  impacted_queues[change.node2()] = true;
                  selector.applyChangeWithoutScoreUpdate(change);
                  ++nb_changes_applied;
                }

                break;

              default:
                GUM_ERROR(OperationNotAllowed,
                          "edge modifications are not supported by local search")
            }
          }
        }

        selector.updateScoresAfterAppliedChanges();

        // reset the impacted queue and applied changes structures
        for (auto iter = impacted_queues.begin(); iter != impacted_queues.end(); ++iter) {
          *iter = false;
        }

        updateApproximationScheme(nb_changes_applied);

      } while (nb_changes_applied && continueApproximationScheme(delta_score));

      stopApproximationScheme();   // just to be sure of the approximationScheme
                                   // has
                                   // been notified of the end of looop

      return dag;
    }

    /// learns the structure and the parameters of a BN
    template < typename GUM_SCALAR, typename GRAPH_CHANGES_SELECTOR, typename PARAM_ESTIMATOR >
    BayesNet< GUM_SCALAR > GreedyHillClimbing::learnBN(GRAPH_CHANGES_SELECTOR& selector,
                                                       PARAM_ESTIMATOR&        estimator,
                                                       DAG                     initial_dag) {
      return DAG2BNLearner::createBN< GUM_SCALAR >(estimator,
                                                   learnStructure(selector, initial_dag));
    }

  } /* namespace learning */

} /* namespace gum */
