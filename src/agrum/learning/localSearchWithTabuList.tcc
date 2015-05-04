/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/** @file
 * @brief The local search with tabu list learning algorithm (for directed graphs)
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/learning/structureUtils/graphChange.h>
#include <agrum/learning/paramUtils/DAG2BNLearner.h>

namespace gum {

  namespace learning {

    /// learns the structure of a Bayes net
    template <typename GRAPH_CHANGES_SELECTOR>
    DAG
    LocalSearchWithTabuList::learnStructure(GRAPH_CHANGES_SELECTOR &selector,
                                            const std::vector<unsigned int> &modal,
                                            DAG dag) {
      selector.setGraph(dag, modal);

      unsigned int nb_changes_applied;
      unsigned int applied_change_with_positive_score;
      unsigned int current_N = 0;
      float delta_score;

      initApproximationScheme();

      // a vector that indicates which queues have valid scores, i.e., scores
      // that were not invalidated by previously applied changes
      std::vector<bool> impacted_queues(dag.size(), false);

      // the best dag found so far with its score
      DAG best_dag = dag;
      float best_score = 0;
      float current_score = 0;

      while (current_N <= __MaxNbDecreasing) {
        applied_change_with_positive_score = 0;

        do {
          nb_changes_applied = 0;
          delta_score = 0;

          std::vector<std::pair<unsigned int, float>> ordered_queues =
              selector.nodesSortedByBestScore();

          for (unsigned int j = 0; j < dag.size(); ++j) {
            unsigned int i = ordered_queues[j].first;

            if (!selector.empty(i) &&
                (!nb_changes_applied || (selector.bestScore(i) > 0))) {
              // pick up the best change
              const GraphChange &change = selector.bestChange(i);

              // std::cout << "change = " << change.type () << " "
              //           << change.node1 () << " " << change.node2 ()
              //           << "  " << selector.bestScore ( i ) << std::endl;

              // perform the change
              switch (change.type()) {
                case GraphChangeType::ARC_ADDITION:
                  if (!impacted_queues[change.node2()] &&
                      selector.isChangeValid(change)) {
                    if (selector.bestScore(i) > 0) {
                      ++applied_change_with_positive_score;
                    } else if (current_score > best_score) {
                      best_score = current_score;
                      best_dag = dag;
                    }

                    delta_score += selector.bestScore(i);
                    current_score += selector.bestScore(i);
                    dag.addArc(change.node1(), change.node2());
                    impacted_queues[change.node2()] = true;
                    selector.applyChangeWithoutScoreUpdate(change);
                    ++nb_changes_applied;
                  }

                  break;

                case GraphChangeType::ARC_DELETION:
                  if (!impacted_queues[change.node2()] &&
                      selector.isChangeValid(change)) {
                    if (selector.bestScore(i) > 0) {
                      ++applied_change_with_positive_score;
                    } else if (current_score > best_score) {
                      best_score = current_score;
                      best_dag = dag;
                    }

                    delta_score += selector.bestScore(i);
                    current_score += selector.bestScore(i);
                    dag.eraseArc(Arc(change.node1(), change.node2()));
                    impacted_queues[change.node2()] = true;
                    selector.applyChangeWithoutScoreUpdate(change);
                    ++nb_changes_applied;
                  }

                  break;

                case GraphChangeType::ARC_REVERSAL:
                  if ((!impacted_queues[change.node1()]) &&
                      (!impacted_queues[change.node2()]) &&
                      selector.isChangeValid(change)) {
                    if (selector.bestScore(i) > 0) {
                      ++applied_change_with_positive_score;
                    } else if (current_score > best_score) {
                      best_score = current_score;
                      best_dag = dag;
                    }

                    delta_score += selector.bestScore(i);
                    current_score += selector.bestScore(i);
                    dag.eraseArc(Arc(change.node1(), change.node2()));
                    dag.addArc(change.node2(), change.node1());
                    impacted_queues[change.node1()] = true;
                    impacted_queues[change.node2()] = true;
                    selector.applyChangeWithoutScoreUpdate(change);
                    ++nb_changes_applied;
                  }

                  break;

                default:
                  GUM_ERROR(OperationNotAllowed, "edge modifications are not "
                                                 "supported by local search");
              }
            }
          }

          selector.updateScoresAfterAppliedChanges();

          // reset the impacted queue and applied changes structures
          for (auto iter = impacted_queues.begin(); iter != impacted_queues.end();
               ++iter) {
            *iter = false;
          }

          updateApproximationScheme(nb_changes_applied);

        } while (nb_changes_applied && continueApproximationScheme(delta_score));

        stopApproximationScheme(); // just to be sure of the approximationScheme has
                                   // been notified of the end of looop

        // update current_N
        if (applied_change_with_positive_score) {
          current_N = 0;
        } else {
          ++current_N;
        }

        // std::cout << "current N = " << current_N << std::endl;
      }

      if (current_score > best_score) {
        return dag;
      } else {
        return best_dag;
      }
    }

    /// learns the structure and the parameters of a BN
    template <typename GUM_SCALAR, typename GRAPH_CHANGES_SELECTOR,
              typename PARAM_ESTIMATOR, typename CELL_TRANSLATORS>
    BayesNet<GUM_SCALAR> LocalSearchWithTabuList::learnBN(
        GRAPH_CHANGES_SELECTOR &selector, PARAM_ESTIMATOR &estimator,
        const std::vector<std::string> &names,
        const std::vector<unsigned int> &modal, const CELL_TRANSLATORS &translator,
        DAG initial_dag) {
      return DAG2BNLearner::createBN<GUM_SCALAR, PARAM_ESTIMATOR, CELL_TRANSLATORS>(
          estimator, learnStructure(selector, modal, initial_dag), names, modal,
          translator);
    }

  } /* namespace learning */

} /* namespace gum */
