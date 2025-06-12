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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


#include <iostream>
#include <sstream>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/database/DBTranslator4LabelizedVariable.h>
#include <agrum/base/database/DBTranslatorSet.h>
#include <agrum/BN/learning/constraints/structuralConstraintDAG.h>
#include <agrum/BN/learning/constraints/structuralConstraintDiGraph.h>
#include <agrum/BN/learning/priors/smoothingPrior.h>
#include <agrum/BN/learning/structureUtils/graphChangesGenerator4DiGraph.h>
#include <agrum/BN/learning/structureUtils/graphChangesSelector4DiGraph.h>

#include <agrum/BN/learning/scores_and_tests/scoreK2.h>

namespace gum_tests {

  class [[maybe_unused]] GraphChangesSelector4DiGraphTestSuite: public CxxTest::TestSuite {
    private:
    void _order_nodes_(const std::vector< std::vector< double > >&      all_scores,
                       const std::vector< gum::NodeId >&                best_nodes,
                       std::vector< std::pair< gum::NodeId, double > >& sorted_nodes) {
      const std::size_t size = best_nodes.size();
      for (std::size_t i = std::size_t(0); i < size; ++i) {
        sorted_nodes[i].first  = gum::NodeId(i);
        sorted_nodes[i].second = all_scores[i][best_nodes[i]];
      }

      std::sort(
          sorted_nodes.begin(),
          sorted_nodes.end(),
          [](const std::pair< gum::NodeId, double >& a,
             const std::pair< gum::NodeId, double >& b) -> bool { return a.second > b.second; });
    }

    void _compute_scores_(gum::learning::ScoreK2&               score,
                          const gum::DAG&                       graph,
                          std::vector< std::vector< double > >& all_scores,
                          std::vector< gum::NodeId >&           best_nodes,
                          gum::NodeId&                          best_node) {
      const std::size_t size = best_nodes.size();

      for (std::size_t i = 0; i < size; ++i) {
        for (std::size_t j = 0; j < size; ++j) {
          const auto& parents = graph.parents(i);
          if (i != j) {
            std::vector< gum::NodeId > pars;
            for (const auto par: parents)
              pars.push_back(par);
            all_scores[i][j] = -score.score(i, pars);

            if (!parents.exists(j)) {
              pars.push_back(gum::NodeId(j));
              all_scores[i][j] += score.score(i, pars);
            } else {
              for (auto& par: pars) {
                if (par == gum::NodeId(j)) {
                  par = *(pars.rbegin());
                  pars.pop_back();
                  break;
                }
              }
              all_scores[i][j] += score.score(i, pars);
            }
          } else {
            all_scores[i][j] = std::numeric_limits< double >::lowest();
          }
        }
      }

      double best_xscore = std::numeric_limits< double >::lowest();
      best_node          = 0;
      for (std::size_t i = 0; i < size; ++i) {
        double best   = all_scores[i][0];
        best_nodes[i] = 0;
        for (std::size_t j = 1; j < size; ++j) {
          if (all_scores[i][j] > best) {
            best          = all_scores[i][j];
            best_nodes[i] = j;
            if (best_xscore < best) {
              best_xscore = best;
              best_node   = i;
            }
          }
        }
      }
    }

    public:
    GUM_ACTIVE_TEST(_K2) {
      // create the translator set
      gum::LabelizedVariable var("X1", "", 0);
      var.addLabel("0");
      var.addLabel("1");
      var.addLabel("2");

      gum::learning::DBTranslatorSet trans_set;
      {
        const std::vector< std::string >              miss;
        gum::learning::DBTranslator4LabelizedVariable translator(var, miss);
        std::vector< std::string >                    names{"A", "B", "C", "D", "E", "F"};

        for (std::size_t i = std::size_t(0); i < names.size(); ++i) {
          translator.setVariableName(names[i]);
          trans_set.insertTranslator(translator, i);
        }
      }

      // create the database
      gum::learning::DatabaseTable database(trans_set);
      std::vector< std::string >   row0{"0", "1", "0", "2", "1", "1"};
      std::vector< std::string >   row1{"1", "2", "0", "1", "2", "2"};
      std::vector< std::string >   row2{"2", "1", "0", "1", "1", "0"};
      std::vector< std::string >   row3{"1", "0", "0", "0", "0", "0"};
      std::vector< std::string >   row4{"0", "0", "0", "1", "1", "1"};
      for (int i = 0; i < 1000; ++i)
        database.insertRow(row0);
      for (int i = 0; i < 50; ++i)
        database.insertRow(row1);
      for (int i = 0; i < 75; ++i)
        database.insertRow(row2);
      for (int i = 0; i < 75; ++i)
        database.insertRow(row3);
      for (int i = 0; i < 200; ++i)
        database.insertRow(row4);

      // create the parser
      gum::learning::DBRowGeneratorSet    genset;
      gum::learning::DBRowGeneratorParser parser(database.handler(), genset);

      gum::learning::SmoothingPrior prior(database);
      gum::learning::ScoreK2        score(parser, prior);

      gum::learning::StructuralConstraintSetStatic< gum::learning::StructuralConstraintDiGraph >
          struct_constraint;

      gum::learning::GraphChangesGenerator4DiGraph< decltype(struct_constraint) > op_set(
          struct_constraint);

      gum::learning::GraphChangesSelector4DiGraph< decltype(struct_constraint), decltype(op_set) >
          selector(score, struct_constraint, op_set);

      gum::DAG graph;
      selector.setGraph(graph);

      TS_ASSERT(!selector.empty())
      for (const auto node: graph) {
        TS_ASSERT(!selector.empty(node))
      }

      selector.setGraph(graph);

      TS_ASSERT(!selector.empty())
      for (const auto node: graph) {
        TS_ASSERT(!selector.empty(node))
      }

      gum::learning::GraphChange change(gum::learning::GraphChangeType::ARC_DELETION, 0, 1);
      TS_ASSERT(!selector.isChangeValid(change))

      for (const auto node: graph) {
        const auto& change = selector.bestChange(node);
        TS_ASSERT_EQUALS(change.type(), gum::learning::GraphChangeType::ARC_ADDITION)
      }
      TS_ASSERT_EQUALS(selector.bestChange().type(), gum::learning::GraphChangeType::ARC_ADDITION)

      std::vector< std::vector< double > > all_scores(6, std::vector< double >(6));
      std::vector< gum::NodeId >           best_nodes(6);
      gum::NodeId                          best_node;
      _compute_scores_(score, graph, all_scores, best_nodes, best_node);

      for (const auto node: graph) {
        const auto& change = selector.bestChange(node);
        TS_ASSERT_EQUALS(change.type(), gum::learning::GraphChangeType::ARC_ADDITION)
        if (change.node1() == node) {
          TS_ASSERT_EQUALS(change.node2(), best_nodes[node])
        } else {
          TS_ASSERT_EQUALS(change.node1(), best_nodes[node])
        }
      }

      const double                best_score = selector.bestScore();
      gum::NodeProperty< double > scores;
      for (const auto node: graph) {
        const double sc = selector.bestScore(node);
        scores.insert(node, sc);
        TS_ASSERT(sc <= best_score)
        TS_ASSERT_EQUALS(sc, all_scores[node][best_nodes[node]])
      }
      TS_ASSERT_EQUALS(best_score, all_scores[best_node][best_nodes[best_node]])

      gum::learning::GraphChange change2(gum::learning::GraphChangeType::ARC_ADDITION, 3, 1);
      graph.addArc(change2.node1(), change2.node2());
      selector.applyChangeWithoutScoreUpdate(change2);
      selector.updateScoresAfterAppliedChanges();

      _compute_scores_(score, graph, all_scores, best_nodes, best_node);

      for (const auto node: graph) {
        const double sc = selector.bestScore(node);
        TS_ASSERT_EQUALS(sc, all_scores[node][best_nodes[node]])
        if (node != 1) {
          TS_ASSERT_EQUALS(sc, scores[node])
        } else {
          TS_ASSERT_DIFFERS(sc, scores[node])
        }
      }

      scores[1] = selector.bestScore(1);
      scores[3] = selector.bestScore(3);
      gum::learning::GraphChange change3(gum::learning::GraphChangeType::ARC_ADDITION, 3, 2);
      graph.addArc(change3.node1(), change3.node2());
      selector.applyChange(change3);

      _compute_scores_(score, graph, all_scores, best_nodes, best_node);

      for (const auto node: graph) {
        const double sc = selector.bestScore(node);
        TS_ASSERT_EQUALS(sc, all_scores[node][best_nodes[node]])
        if ((node != 2)) {
          TS_ASSERT_EQUALS(selector.bestScore(node), scores[node])
        } else {
          TS_ASSERT_DIFFERS(selector.bestScore(node), scores[node])
        }
      }

      scores[2] = selector.bestScore(2);
      scores[3] = selector.bestScore(3);
      gum::learning::GraphChange change4(gum::learning::GraphChangeType::ARC_DELETION, 3, 1);
      graph.eraseArc(gum::Arc(change4.node1(), change4.node2()));
      selector.applyChange(change4);

      _compute_scores_(score, graph, all_scores, best_nodes, best_node);

      for (const auto node: graph) {
        const double sc = selector.bestScore(node);
        TS_ASSERT_EQUALS(sc, all_scores[node][best_nodes[node]])
        if ((node != 1)) {
          TS_ASSERT_EQUALS(selector.bestScore(node), scores[node])
        } else {
          TS_ASSERT_DIFFERS(selector.bestScore(node), scores[node])
        }
      }


      const auto xnodes = selector.nodesSortedByBestScore();
      TS_ASSERT_EQUALS(xnodes.size(), std::size_t(6))

      std::vector< std::pair< gum::NodeId, double > > sorted_nodes(6);
      _order_nodes_(all_scores, best_nodes, sorted_nodes);
      for (std::size_t i = 0; i < 6; ++i) {
        TS_ASSERT_EQUALS(xnodes[i], sorted_nodes[i])
      }
    }
  };


} /* namespace gum_tests */
