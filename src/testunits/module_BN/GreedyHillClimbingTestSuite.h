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


#include <iostream>

#include <agrum/base/core/approximations/approximationScheme.h>
#include <agrum/base/core/approximations/approximationSchemeListener.h>
#include <agrum/base/database/databaseTable.h>
#include <agrum/base/database/DBInitializerFromCSV.h>
#include <agrum/base/database/DBRowGeneratorParser.h>
#include <agrum/base/database/DBTranslator4LabelizedVariable.h>
#include <agrum/base/database/DBTranslatorSet.h>
#include <agrum/base/graphs/DAG.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/learning/constraints/structuralConstraintDAG.h>
#include <agrum/BN/learning/constraints/structuralConstraintDiGraph.h>
#include <agrum/BN/learning/constraints/structuralConstraintForbiddenArcs.h>
#include <agrum/BN/learning/constraints/structuralConstraintIndegree.h>
#include <agrum/BN/learning/constraints/structuralConstraintSetStatic.h>
#include <agrum/BN/learning/constraints/structuralConstraintSliceOrder.h>
#include <agrum/BN/learning/constraints/structuralConstraintTotalOrder.h>
#include <agrum/BN/learning/greedyHillClimbing.h>
#include <agrum/BN/learning/paramUtils/paramEstimatorML.h>
#include <agrum/BN/learning/priors/DirichletPriorFromDatabase.h>
#include <agrum/BN/learning/priors/smoothingPrior.h>
#include <agrum/BN/learning/scores/scoreBDeu.h>
#include <agrum/BN/learning/scores/scoreBIC.h>
#include <agrum/BN/learning/scores/scoreK2.h>
#include <agrum/BN/learning/structureUtils/graphChangesGenerator4DiGraph.h>
#include <agrum/BN/learning/structureUtils/graphChangesGenerator4UndiGraph.h>
#include <agrum/BN/learning/structureUtils/graphChangesGeneratorOnSubDiGraph.h>
#include <agrum/BN/learning/structureUtils/graphChangesSelector4DiGraph.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  GreedyHillClimbing
#define GUM_CURRENT_MODULE BN

namespace gum_tests {

  class simpleListenerForGHC: public gum::ApproximationSchemeListener {
    private:
    int         _nbr_;
    std::string _mess_;

    public:
    simpleListenerForGHC(gum::ApproximationScheme& sch) :
        gum::ApproximationSchemeListener(sch), _nbr_(0), _mess_("") {}

    void whenProgress(const void* buffer, const gum::Size a, const double b, const double c) {
      _nbr_++;
      std::cout << _nbr_ << ": error = " << b << std::endl;
    }

    void whenStop(const void* buffer, std::string_view s) { _mess_ = s; }

    int getNbr() { return _nbr_; }

    std::string getMess() { return _mess_; }
  };

  struct GreedyHillClimbingTestSuite {
    private:
    static double
        _score_(gum::learning::Score& score, const gum::NodeId& node, const gum::DAG& dag) {
      std::vector< gum::NodeId > cond_set;
      for (const auto par: dag.parents(node)) {
        cond_set.push_back(par);
      }
      return score.score(node, cond_set);
    }

    static bool _applyNextChange_(gum::learning::Score&  score,
                                  std::vector< double >& current_scores,
                                  gum::DAG&              dag,
                                  bool                   allow_additions          = true,
                                  bool                   allow_deletions          = true,
                                  bool                   allow_reversals          = true,
                                  bool                   allow_triangle_deletions = true,
                                  const double           epsilon                  = -1,
                                  const int              max_indegree             = -1,
                                  const gum::Sequence< gum::NodeId >& total_order
                                  = gum::Sequence< gum::NodeId >()) {
      const int nb_vars = int(dag.size());

      std::vector< int > rank;
      if (!total_order.empty()) {
        rank.resize(total_order.size());
        int i = 0;
        for (const auto node: total_order) {
          rank[node] = i++;
        }
        // std::cout << "order: " << rank << std::endl;
      }

      std::vector< std::pair< gum::learning::GraphChange, double > > changes;

      for (int i = 0; i < nb_vars; ++i) {
        for (int j = 0; j < nb_vars; ++j) {
          if ((i != j) && (rank.empty() || (rank[i] < rank[j]))) {
            // check add arc
            if (allow_additions && !dag.existsArc(gum::Arc(i, j))
                && ((max_indegree < 0) || (dag.parents(j).size() < (gum::Size)max_indegree))) {
              try {
                dag.addArc(gum::NodeId(i), gum::NodeId(j));
                double delta_score = _score_(score, j, dag) - current_scores[j];
                if (delta_score > 0) {
                  changes.emplace_back(gum::learning::ArcAddition(gum::NodeId(i), gum::NodeId(j)),
                                       delta_score);
                }
                dag.eraseArc(gum::Arc(i, j));
              } catch (...) {}
            }

            // check remove arc
            if (allow_deletions && dag.existsArc(gum::Arc(i, j))) {
              dag.eraseArc(gum::Arc(i, j));
              double delta_score = _score_(score, j, dag) - current_scores[j];
              if (delta_score > 0) {
                changes.emplace_back(gum::learning::ArcDeletion(gum::NodeId(i), gum::NodeId(j)),
                                     delta_score);
              }
              dag.addArc(gum::NodeId(i), gum::NodeId(j));
            }

            // check reverse arc
            if (allow_reversals && dag.existsArc(gum::Arc(i, j)) && rank.empty()
                && ((max_indegree < 0) || (dag.parents(i).size() < (gum::Size)max_indegree))) {
              dag.eraseArc(gum::Arc(i, j));
              try {
                dag.addArc(j, i);
                double delta_score_i = _score_(score, i, dag) - current_scores[i];
                double delta_score_j = _score_(score, j, dag) - current_scores[j];
                double delta_score   = delta_score_i + delta_score_j;
                if (delta_score > 0) {
                  changes.emplace_back(gum::learning::ArcReversal(gum::NodeId(i), gum::NodeId(j)),
                                       delta_score);
                }
                dag.eraseArc(gum::Arc(j, i));
                dag.addArc(i, j);
              } catch (...) { dag.addArc(i, j); }
            }
          }
        }
      }

      if (allow_triangle_deletions && rank.empty()) {
        for (int i = 0; i < nb_vars; ++i) {
          for (int j = 0; j < nb_vars; ++j) {
            if (dag.existsArc(gum::Arc(i, j))) {
              for (int k = 0; k < nb_vars; ++k) {
                if (dag.existsArc(gum::Arc(i, k)) && dag.existsArc(gum::Arc(j, k))) {
                  // check arc triangle deletion1
                  if ((max_indegree < 0)
                      || (dag.parents(i).size() + 2 <= (gum::Size)max_indegree)) {
                    dag.eraseArc(gum::Arc(i, j));
                    dag.eraseArc(gum::Arc(i, k));
                    dag.eraseArc(gum::Arc(j, k));

                    try {
                      dag.addArc(j, i);
                      dag.addArc(k, i);

                      double delta_score_i = _score_(score, i, dag) - current_scores[i];
                      double delta_score_j = _score_(score, j, dag) - current_scores[j];
                      double delta_score_k = _score_(score, k, dag) - current_scores[k];
                      double delta_score   = delta_score_i + delta_score_j + delta_score_k;
                      if (delta_score > 0) {
                        changes.emplace_back(gum::learning::ArcTriangleDeletion1(gum::NodeId(i),
                                                                                 gum::NodeId(j),
                                                                                 gum::NodeId(k)),
                                             delta_score);
                      }

                      dag.eraseArc(gum::Arc(j, i));
                      dag.eraseArc(gum::Arc(k, i));
                    } catch (...) {
                      dag.eraseArc(gum::Arc(j, i));
                      dag.eraseArc(gum::Arc(k, i));
                    }

                    dag.addArc(i, j);
                    dag.addArc(i, k);
                    dag.addArc(j, k);
                  }

                  // check arc triangle deletion2
                  if ((max_indegree < 0)
                      || (dag.parents(j).size() + 1 <= (gum::Size)max_indegree)) {
                    dag.eraseArc(gum::Arc(i, j));
                    dag.eraseArc(gum::Arc(i, k));
                    dag.eraseArc(gum::Arc(j, k));

                    try {
                      dag.addArc(k, j);

                      double delta_score_j = _score_(score, j, dag) - current_scores[j];
                      double delta_score_k = _score_(score, k, dag) - current_scores[k];
                      double delta_score   = delta_score_j + delta_score_k;
                      if (delta_score > 0) {
                        changes.emplace_back(gum::learning::ArcTriangleDeletion2(gum::NodeId(i),
                                                                                 gum::NodeId(j),
                                                                                 gum::NodeId(k)),
                                             delta_score);
                      }

                      dag.eraseArc(gum::Arc(k, j));
                    } catch (...) { dag.eraseArc(gum::Arc(k, j)); }

                    dag.addArc(i, j);
                    dag.addArc(i, k);
                    dag.addArc(j, k);
                  }
                }
              }
            }
          }
        }
      }

      if (changes.empty()) return false;

      // get the best change
      auto best_i = std::size_t(0);
      for (auto i = std::size_t(0); i < changes.size(); ++i) {
        if (changes[i].second > changes[best_i].second) best_i = i;
      }
      // std::cout << "applying " << changes[best_i].first.toString()
      //           << " delta_score = " << changes[best_i].second << std::endl;

      if (changes[best_i].second < epsilon) return false;
      //      std::cout << "apply " << changes[best_i].first << " score : " <<
      //      changes[best_i].second
      //                << std::endl;

      // apply the best change
      switch (changes[best_i].first.type()) {
        case gum::learning::GraphChangeType::ARC_ADDITION :
          dag.addArc(changes[best_i].first.node1(), changes[best_i].first.node2());
          current_scores[changes[best_i].first.node2()]
              = _score_(score, changes[best_i].first.node2(), dag);
          break;

        case gum::learning::GraphChangeType::ARC_DELETION :
          dag.eraseArc(gum::Arc(changes[best_i].first.node1(), changes[best_i].first.node2()));
          current_scores[changes[best_i].first.node2()]
              = _score_(score, changes[best_i].first.node2(), dag);
          break;

        case gum::learning::GraphChangeType::ARC_REVERSAL :
          dag.eraseArc(gum::Arc(changes[best_i].first.node1(), changes[best_i].first.node2()));
          dag.addArc(changes[best_i].first.node2(), changes[best_i].first.node1());
          current_scores[changes[best_i].first.node1()]
              = _score_(score, changes[best_i].first.node1(), dag);
          current_scores[changes[best_i].first.node2()]
              = _score_(score, changes[best_i].first.node2(), dag);
          break;

        case gum::learning::GraphChangeType::ARC_TRIANGLE_DELETION1 :
          dag.eraseArc(gum::Arc(changes[best_i].first.node1(), changes[best_i].first.node2()));
          dag.eraseArc(gum::Arc(changes[best_i].first.node1(), changes[best_i].first.node3()));
          dag.eraseArc(gum::Arc(changes[best_i].first.node2(), changes[best_i].first.node3()));
          dag.addArc(changes[best_i].first.node2(), changes[best_i].first.node1());
          dag.addArc(changes[best_i].first.node3(), changes[best_i].first.node1());
          current_scores[changes[best_i].first.node1()]
              = _score_(score, changes[best_i].first.node1(), dag);
          current_scores[changes[best_i].first.node2()]
              = _score_(score, changes[best_i].first.node2(), dag);
          current_scores[changes[best_i].first.node3()]
              = _score_(score, changes[best_i].first.node3(), dag);
          break;

        case gum::learning::GraphChangeType::ARC_TRIANGLE_DELETION2 :
          dag.eraseArc(gum::Arc(changes[best_i].first.node1(), changes[best_i].first.node3()));
          dag.eraseArc(gum::Arc(changes[best_i].first.node2(), changes[best_i].first.node3()));
          dag.addArc(changes[best_i].first.node3(), changes[best_i].first.node2());
          current_scores[changes[best_i].first.node2()]
              = _score_(score, changes[best_i].first.node2(), dag);
          current_scores[changes[best_i].first.node3()]
              = _score_(score, changes[best_i].first.node3(), dag);
          break;

        default : break;
      }

      return true;
    }


    public:
    static void test_K2_asia() {
      gum::learning::DBInitializerFromCSV initializer(GET_RESSOURCES_PATH("csv/asia.csv"));
      const auto&                         var_names = initializer.variableNames();
      const std::size_t                   nb_vars   = var_names.size();

      gum::learning::DBTranslatorSet                translator_set;
      gum::learning::DBTranslator4LabelizedVariable translator;
      for (std::size_t i = 0; i < nb_vars; ++i) {
        translator_set.insertTranslator(translator, i);
      }

      gum::learning::DatabaseTable database(translator_set);
      database.setVariableNames(initializer.variableNames());
      initializer.fillDatabase(database);
      // database.reorder();

      gum::learning::DBRowGeneratorSet    genset;
      gum::learning::DBRowGeneratorParser parser(database.handler(), genset);
      gum::learning::SmoothingPrior       prior(database);
      gum::learning::ScoreK2              score(parser, prior);

      gum::learning::StructuralConstraintSetStatic< gum::learning::StructuralConstraintDAG,
                                                    gum::learning::StructuralConstraintIndegree >
          struct_constraint;

      struct_constraint.setMaxIndegree(6);

      // gum::NodeProperty<bool> slices {
      //   std::make_pair( gum::NodeId ( 0 ), 0 ),
      //   std::make_pair( gum::NodeId ( 1 ), 0 ),
      //   std::make_pair( gum::NodeId ( 6 ), 0 ),
      //   std::make_pair( gum::NodeId ( 2 ), 1 ) };
      // struct_constraint.setSliceOrder ( slices );
      // struct_constraint.setDefaultSlice ( 1 );

      gum::learning::StructuralConstraintIndegree constraint1;
      constraint1.setMaxIndegree(2);
      static_cast< gum::learning::StructuralConstraintIndegree& >(struct_constraint) = constraint1;

      gum::learning::ParamEstimatorML estimator(parser, prior, score.internalPrior());

      gum::learning::StructuralConstraintSetStatic< gum::learning::StructuralConstraintTotalOrder >
          invariable_constraints;

      gum::Sequence< gum::NodeId > sequence{4, 6, 2, 1, 5, 7, 0};
      invariable_constraints.setTotalOrder(sequence);

      gum::learning::GraphChangesSelector4DiGraph< decltype(invariable_constraints),
                                                   decltype(struct_constraint) >
          selector(score, invariable_constraints, struct_constraint);
      selector.useArcDeletions(false);
      selector.useArcReversals(false);
      selector.useArcTriangleDeletions(false);

      gum::learning::GreedyHillClimbing search;
      search.approximationScheme().enableEpsilon();
      search.approximationScheme().setEpsilon(10);

      try {
        gum::DAG dag = search.learnStructure(selector);
        CHECK_EQ(dag.arcs().size(), (gum::Size)10);

        {
          std::vector< double > myscores;
          gum::DAG              mydag;
          for (int i = 0; i < (int)nb_vars; ++i) {
            mydag.addNodeWithId(i);
            myscores.push_back(_score_(score, i, mydag));
          }

          while (_applyNextChange_(score,
                                   myscores,
                                   mydag,
                                   true,
                                   false,
                                   false,
                                   false,
                                   10,
                                   -1,
                                   sequence)) {}

          // std::cout << dag << std::endl;
          CHECK_EQ(dag, mydag);
        }
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }
      // gum::BayesNet<double> bn =
      // search.learnBN<double> ( selector, estimator,
      // database.variableNames (),
      // modalities );

      // gum::BayesNet<double> bn2 =
      // search.learnBN ( selector, estimator,
      // database.variableNames (),
      // modalities );
    }

    static void test_asia_with_ordered_values() {
      gum::learning::DBInitializerFromCSV initializer(GET_RESSOURCES_PATH("csv/asia.csv"));
      const auto&                         var_names = initializer.variableNames();
      const std::size_t                   nb_vars   = var_names.size();

      gum::learning::DBTranslatorSet translator_set;
      gum::LabelizedVariable         xvar("var", "", 0);
      xvar.addLabel("0");
      xvar.addLabel("1");
      gum::learning::DBTranslator4LabelizedVariable translator(xvar);
      for (std::size_t i = 0; i < nb_vars; ++i) {
        translator_set.insertTranslator(translator, i);
      }

      gum::learning::DatabaseTable database(translator_set);
      database.setVariableNames(initializer.variableNames());
      initializer.fillDatabase(database);

      gum::learning::DBRowGeneratorSet    genset;
      gum::learning::DBRowGeneratorParser parser(database.handler(), genset);
      gum::learning::SmoothingPrior       prior(database);
      gum::learning::ScoreK2              score(parser, prior);

      gum::learning::StructuralConstraintSetStatic< gum::learning::StructuralConstraintDAG,
                                                    gum::learning::StructuralConstraintIndegree >
          struct_constraint;

      struct_constraint.setMaxIndegree(6);

      // gum::NodeProperty<bool> slices {
      //   std::make_pair( gum::NodeId ( 0 ), 0 ),
      //   std::make_pair( gum::NodeId ( 1 ), 0 ),
      //   std::make_pair( gum::NodeId ( 6 ), 0 ),
      //   std::make_pair( gum::NodeId ( 2 ), 1 ) };
      // struct_constraint.setSliceOrder ( slices );
      // struct_constraint.setDefaultSlice ( 1 );

      gum::learning::StructuralConstraintIndegree constraint1;
      constraint1.setMaxIndegree(1);
      static_cast< gum::learning::StructuralConstraintIndegree& >(struct_constraint) = constraint1;

      gum::learning::ParamEstimatorML estimator(parser, prior, score.internalPrior());

      gum::learning::StructuralConstraintSetStatic<
          gum::learning::StructuralConstraintForbiddenArcs >
          invariable_constraints;

      gum::learning::GraphChangesSelector4DiGraph< decltype(invariable_constraints),
                                                   decltype(struct_constraint) >
          selector(score, invariable_constraints, struct_constraint);
      selector.useArcTriangleDeletions(false);   // usual greedy hill climbing

      gum::learning::GreedyHillClimbing search;
      search.approximationScheme().setEpsilon(0);

      gum::DAG dag = search.learnStructure(selector);
      CHECK_EQ(dag.arcs().size(), (gum::Size)7);

      {
        std::vector< double > myscores;
        gum::DAG              mydag;
        for (int i = 0; i < (int)nb_vars; ++i) {
          mydag.addNodeWithId(i);
          myscores.push_back(_score_(score, i, mydag));
        }

        gum::Sequence< gum::NodeId > total_order;
        while (
            _applyNextChange_(score, myscores, mydag, true, true, true, false, 0, 1, total_order)) {
        }

        CHECK_EQ(dag, mydag);
      }

      gum::BayesNet< double > bn = search.learnBN< double >(selector, estimator);

      const std::string s0 = "0";
      const std::string s1 = "1";
      for (gum::Idx i = 0; i < database.nbVariables(); ++i) {
        const gum::DiscreteVariable& var = bn.variable(i);
        CHECK_EQ(var.label(0), s0);
        CHECK_EQ(var.label(1), s1);
      }
    }

    static void test_asia_with_extendedGHC() {
      gum::learning::DBInitializerFromCSV initializer(GET_RESSOURCES_PATH("csv/asia.csv"));
      const auto&                         var_names = initializer.variableNames();
      const std::size_t                   nb_vars   = var_names.size();

      gum::learning::DBTranslatorSet translator_set;
      gum::LabelizedVariable         xvar("var", "", 0);
      xvar.addLabel("0");
      xvar.addLabel("1");
      gum::learning::DBTranslator4LabelizedVariable translator(xvar);
      for (std::size_t i = 0; i < nb_vars; ++i) {
        translator_set.insertTranslator(translator, i);
      }

      gum::learning::DatabaseTable database(translator_set);
      database.setVariableNames(initializer.variableNames());
      initializer.fillDatabase(database);

      gum::learning::DBRowGeneratorSet    genset;
      gum::learning::DBRowGeneratorParser parser(database.handler(), genset);
      gum::learning::SmoothingPrior       prior(database);
      gum::learning::ScoreK2              score(parser, prior);

      gum::learning::StructuralConstraintSetStatic< gum::learning::StructuralConstraintDAG,
                                                    gum::learning::StructuralConstraintIndegree >
          struct_constraint;

      struct_constraint.setMaxIndegree(6);

      // gum::NodeProperty<bool> slices {
      //   std::make_pair( gum::NodeId ( 0 ), 0 ),
      //   std::make_pair( gum::NodeId ( 1 ), 0 ),
      //   std::make_pair( gum::NodeId ( 6 ), 0 ),
      //   std::make_pair( gum::NodeId ( 2 ), 1 ) };
      // struct_constraint.setSliceOrder ( slices );
      // struct_constraint.setDefaultSlice ( 1 );

      gum::learning::ParamEstimatorML estimator(parser, prior, score.internalPrior());

      gum::learning::StructuralConstraintSetStatic<
          gum::learning::StructuralConstraintForbiddenArcs >
          invariable_constraints;

      gum::learning::GraphChangesSelector4DiGraph< decltype(invariable_constraints),
                                                   decltype(struct_constraint) >
          selector(score, invariable_constraints, struct_constraint);
      selector.useArcTriangleDeletions(true);   // usual greedy hill climbing

      gum::learning::GreedyHillClimbing search;
      search.approximationScheme().setEpsilon(0);

      gum::DAG dag = search.learnStructure(selector);
      CHECK_EQ(dag.arcs().size(), (gum::Size)9);

      {
        std::vector< double > myscores;
        gum::DAG              mydag;
        for (int i = 0; i < (int)nb_vars; ++i) {
          mydag.addNodeWithId(i);
          myscores.push_back(_score_(score, i, mydag));
        }

        gum::Sequence< gum::NodeId > total_order;
        while (
            _applyNextChange_(score, myscores, mydag, true, true, true, true, 0, 6, total_order)) {}

        CHECK_EQ(dag, mydag);
      }

      gum::BayesNet< double > bn = search.learnBN< double >(selector, estimator);

      const std::string s0 = "0";
      const std::string s1 = "1";
      for (gum::Idx i = 0; i < database.nbVariables(); ++i) {
        const gum::DiscreteVariable& var = bn.variable(i);
        CHECK_EQ(var.label(0), s0);
        CHECK_EQ(var.label(1), s1);
      }
    }

    static void test_alarm_with_ordered_values() {
      gum::learning::DBInitializerFromCSV initializer(GET_RESSOURCES_PATH("csv/alarm.csv"));
      const auto&                         var_names = initializer.variableNames();
      const std::size_t                   nb_vars   = var_names.size();

      gum::learning::DBTranslatorSet translator_set;
      gum::LabelizedVariable         xvar("var", "", 0);
      xvar.addLabel("0");
      xvar.addLabel("1");
      xvar.addLabel("2");
      xvar.addLabel("3");
      gum::learning::DBTranslator4LabelizedVariable translator(xvar, true);
      for (std::size_t i = 0; i < nb_vars; ++i) {
        translator_set.insertTranslator(translator, i);
      }

      gum::learning::DatabaseTable database(translator_set);
      database.setVariableNames(initializer.variableNames());
      initializer.fillDatabase(database);

      gum::learning::DBRowGeneratorSet    genset;
      gum::learning::DBRowGeneratorParser parser(database.handler(), genset);
      gum::learning::SmoothingPrior       prior(database);
      gum::learning::ScoreK2              score(parser, prior);
      // score.setNumberOfThreads(24);

      gum::learning::StructuralConstraintSetStatic< gum::learning::StructuralConstraintDAG,
                                                    gum::learning::StructuralConstraintIndegree >
          struct_constraint;

      struct_constraint.setMaxIndegree(6);

      // gum::NodeProperty<bool> slices {
      //   std::make_pair( gum::NodeId ( 0 ), 0 ),
      //   std::make_pair( gum::NodeId ( 1 ), 0 ),
      //   std::make_pair( gum::NodeId ( 6 ), 0 ),
      //   std::make_pair( gum::NodeId ( 2 ), 1 ) };
      // struct_constraint.setSliceOrder ( slices );
      // struct_constraint.setDefaultSlice ( 1 );

      //      gum::learning::StructuralConstraintIndegree constraint1;
      //      constraint1.setMaxIndegree(6);
      //      static_cast< gum::learning::StructuralConstraintIndegree& >(struct_constraint) =
      //      constraint1;

      gum::learning::ParamEstimatorML estimator(parser, prior, score.internalPrior());

      gum::learning::StructuralConstraintSetStatic<
          gum::learning::StructuralConstraintForbiddenArcs >
          invariable_constraints;

      gum::learning::GraphChangesSelector4DiGraph< decltype(invariable_constraints),
                                                   decltype(struct_constraint) >
          selector(score, invariable_constraints, struct_constraint);

      gum::learning::GreedyHillClimbing search;
      // simpleListenerForGHC agsl ( search );
      search.approximationScheme().setEpsilon(1000);

      gum::BayesNet< double > bn = search.learnBN< double >(selector, estimator);

      {
        std::vector< double > myscores;
        gum::DAG              mydag;
        for (int i = 0; i < (int)nb_vars; ++i) {
          mydag.addNodeWithId(i);
          myscores.push_back(_score_(score, i, mydag));
        }

        gum::Sequence< gum::NodeId > total_order;
        while (_applyNextChange_(score,
                                 myscores,
                                 mydag,
                                 true,
                                 true,
                                 true,
                                 false,
                                 1000,
                                 6,
                                 total_order)) {}

        CHECK_EQ(bn.dag(), mydag);
      }

      const std::string    s0 = "0";
      const std::string    s1 = "1";
      const std::string    s2 = "2";
      gum::Set< gum::Idx > seq{1, 10, 11, 14};
      for (gum::Idx i = 0; i < database.nbVariables(); ++i) {
        const gum::DiscreteVariable& var = bn.variable(i);
        CHECK_EQ(var.label(0), s0);
        CHECK_EQ(var.label(1), s1);
        if (seq.exists(i)) { CHECK_EQ(var.label(2), s2); }
      }
    }

    static void test_alarm_with_ordered_values2() {
      gum::learning::DBInitializerFromCSV initializer(GET_RESSOURCES_PATH("csv/alarm.csv"));
      const auto&                         var_names = initializer.variableNames();
      const std::size_t                   nb_vars   = var_names.size();

      gum::learning::DBTranslatorSet translator_set;
      gum::LabelizedVariable         xvar("var", "", 0);
      xvar.addLabel("0");
      xvar.addLabel("1");
      xvar.addLabel("2");
      xvar.addLabel("3");
      gum::learning::DBTranslator4LabelizedVariable translator1(xvar);
      gum::learning::DBTranslator4LabelizedVariable translator2;
      for (std::size_t i = 0; i < nb_vars; ++i) {
        if ((i == 1) || (i == 10) || (i == 11) || (i == 14))
          translator_set.insertTranslator(translator1, i);
        else translator_set.insertTranslator(translator2, i);
      }

      gum::learning::DatabaseTable database(translator_set);
      database.setVariableNames(initializer.variableNames());
      initializer.fillDatabase(database);

      gum::learning::DBRowGeneratorSet    genset;
      gum::learning::DBRowGeneratorParser parser(database.handler(), genset);
      gum::learning::SmoothingPrior       prior(database);
      gum::learning::ScoreK2              score(parser, prior);

      gum::learning::StructuralConstraintSetStatic< gum::learning::StructuralConstraintDAG,
                                                    gum::learning::StructuralConstraintIndegree >
          struct_constraint;

      struct_constraint.setMaxIndegree(1);

      // gum::NodeProperty<bool> slices {
      //   std::make_pair( gum::NodeId ( 0 ), 0 ),
      //   std::make_pair( gum::NodeId ( 1 ), 0 ),
      //   std::make_pair( gum::NodeId ( 6 ), 0 ),
      //   std::make_pair( gum::NodeId ( 2 ), 1 ) };
      // struct_constraint.setSliceOrder ( slices );
      // struct_constraint.setDefaultSlice ( 1 );

      gum::learning::StructuralConstraintIndegree constraint1;
      constraint1.setMaxIndegree(6);
      static_cast< gum::learning::StructuralConstraintIndegree& >(struct_constraint) = constraint1;

      gum::learning::ParamEstimatorML estimator(parser, prior, score.internalPrior());

      gum::learning::StructuralConstraintSetStatic<
          gum::learning::StructuralConstraintForbiddenArcs >
          invariable_constraints;

      gum::learning::GraphChangesSelector4DiGraph< decltype(invariable_constraints),
                                                   decltype(struct_constraint) >
          selector(score, invariable_constraints, struct_constraint);

      gum::learning::GreedyHillClimbing search;
      // simpleListenerForGHC agsl ( search );
      search.approximationScheme().setEpsilon(1000);

      gum::BayesNet< double > bn = search.learnBN< double >(selector, estimator);

      {
        std::vector< double > myscores;
        gum::DAG              mydag;
        for (int i = 0; i < (int)nb_vars; ++i) {
          mydag.addNodeWithId(i);
          myscores.push_back(_score_(score, i, mydag));
        }

        gum::Sequence< gum::NodeId > total_order;
        while (_applyNextChange_(score,
                                 myscores,
                                 mydag,
                                 true,
                                 true,
                                 true,
                                 false,
                                 1000,
                                 6,
                                 total_order)) {}

        CHECK_EQ(bn.dag(), mydag);
      }

      const std::string    s0 = "0";
      const std::string    s1 = "1";
      const std::string    s2 = "2";
      gum::Set< gum::Idx > seq{1, 10, 11, 14};
      for (auto i: seq) {
        const gum::DiscreteVariable& var = bn.variable(i);
        CHECK_EQ(var.label(0), s0);
        CHECK_EQ(var.label(1), s1);
        CHECK_EQ(var.label(2), s2);
      }
    }

    static void test_alarm_with_extendedGHC() {
      gum::learning::DBInitializerFromCSV initializer(GET_RESSOURCES_PATH("csv/alarm.csv"));
      const auto&                         var_names = initializer.variableNames();
      const std::size_t                   nb_vars   = var_names.size();

      gum::learning::DBTranslatorSet translator_set;
      gum::LabelizedVariable         xvar("var", "", 0);
      xvar.addLabel("0");
      xvar.addLabel("1");
      xvar.addLabel("2");
      xvar.addLabel("3");
      gum::learning::DBTranslator4LabelizedVariable translator1(xvar);
      gum::learning::DBTranslator4LabelizedVariable translator2;
      for (std::size_t i = 0; i < nb_vars; ++i) {
        if ((i == 1) || (i == 10) || (i == 11) || (i == 14))
          translator_set.insertTranslator(translator1, i);
        else translator_set.insertTranslator(translator2, i);
      }

      gum::learning::DatabaseTable database(translator_set);
      database.setVariableNames(initializer.variableNames());
      initializer.fillDatabase(database);

      gum::learning::DBRowGeneratorSet    genset;
      gum::learning::DBRowGeneratorParser parser(database.handler(), genset);
      gum::learning::SmoothingPrior       prior(database);
      gum::learning::ScoreK2              score(parser, prior);

      gum::learning::StructuralConstraintSetStatic< gum::learning::StructuralConstraintDAG,
                                                    gum::learning::StructuralConstraintIndegree >
          struct_constraint;

      // gum::NodeProperty<bool> slices {
      //   std::make_pair( gum::NodeId ( 0 ), 0 ),
      //   std::make_pair( gum::NodeId ( 1 ), 0 ),
      //   std::make_pair( gum::NodeId ( 6 ), 0 ),
      //   std::make_pair( gum::NodeId ( 2 ), 1 ) };
      // struct_constraint.setSliceOrder ( slices );
      // struct_constraint.setDefaultSlice ( 1 );

      gum::learning::StructuralConstraintIndegree constraint1;
      constraint1.setMaxIndegree(6);
      static_cast< gum::learning::StructuralConstraintIndegree& >(struct_constraint) = constraint1;

      gum::learning::ParamEstimatorML estimator(parser, prior, score.internalPrior());

      gum::learning::StructuralConstraintSetStatic<
          gum::learning::StructuralConstraintForbiddenArcs >
          invariable_constraints;

      gum::learning::GraphChangesSelector4DiGraph< decltype(invariable_constraints),
                                                   decltype(struct_constraint) >
          selector(score, invariable_constraints, struct_constraint);

      selector.useArcAdditions(true);
      selector.useArcDeletions(true);
      selector.useArcReversals(true);
      selector.useArcTriangleDeletions(true);

      gum::learning::GreedyHillClimbing search;
      // simpleListenerForGHC agsl ( search );
      search.approximationScheme().setEpsilon(50);

      gum::BayesNet< double > bn = search.learnBN< double >(selector, estimator);

      {
        std::vector< double > myscores;
        gum::DAG              mydag;
        for (int i = 0; i < (int)nb_vars; ++i) {
          mydag.addNodeWithId(i);
          myscores.push_back(_score_(score, i, mydag));
        }

        gum::Sequence< gum::NodeId > total_order;
        while (
            _applyNextChange_(score, myscores, mydag, true, true, true, true, 50, 6, total_order)) {
        }

        CHECK_EQ(bn.dag(), mydag);
      }

      const std::string    s0 = "0";
      const std::string    s1 = "1";
      const std::string    s2 = "2";
      gum::Set< gum::Idx > seq{1, 10, 11, 14};
      for (auto i: seq) {
        const gum::DiscreteVariable& var = bn.variable(i);
        CHECK_EQ(var.label(0), s0);
        CHECK_EQ(var.label(1), s1);
        CHECK_EQ(var.label(2), s2);
      }
    }

    void test_dirichlet() {
      // read the learning database
      gum::learning::DBInitializerFromCSV initializer(
          GET_RESSOURCES_PATH("csv/db_dirichlet_learning.csv"));
      const auto&       var_names = initializer.variableNames();
      const std::size_t nb_vars   = var_names.size();

      gum::learning::DBTranslatorSet                translator_set;
      gum::learning::DBTranslator4LabelizedVariable translator;
      for (std::size_t i = 0; i < nb_vars; ++i) {
        translator_set.insertTranslator(translator, i);
      }

      gum::learning::DatabaseTable database(translator_set);
      database.setVariableNames(initializer.variableNames());
      initializer.fillDatabase(database);


      // read the prior database
      gum::learning::DBInitializerFromCSV dirichlet_initializer(
          GET_RESSOURCES_PATH("csv/db_dirichlet_prior.csv"));
      const auto&       dirichlet_var_names = initializer.variableNames();
      const std::size_t dirichlet_nb_vars   = dirichlet_var_names.size();

      gum::learning::DBTranslatorSet dirichlet_translator_set;
      for (std::size_t i = 0; i < dirichlet_nb_vars; ++i) {
        dirichlet_translator_set.insertTranslator(translator, i);
      }

      gum::learning::DatabaseTable dirichlet_database(dirichlet_translator_set);
      dirichlet_database.setVariableNames(dirichlet_initializer.variableNames());
      dirichlet_initializer.fillDatabase(dirichlet_database);


      // create the score and the prior
      gum::learning::DBRowGeneratorSet          dirichlet_genset;
      gum::learning::DBRowGeneratorParser       dirichlet_parser(dirichlet_database.handler(),
                                                                 dirichlet_genset);
      gum::learning::DirichletPriorFromDatabase prior(dirichlet_database, dirichlet_parser);

      gum::learning::DBRowGeneratorSet    genset;
      gum::learning::DBRowGeneratorParser parser(database.handler(), genset);

      std::vector< double > weights{0, 1.0, 5.0, 10.0, 1000.0, 7000.0, 100000.0};

      for (const auto weight: weights) {
        prior.setWeight(weight);
        gum::learning::ScoreBIC score(parser, prior);


        // finalize the learning algorithm
        gum::learning::StructuralConstraintSetStatic< gum::learning::StructuralConstraintDAG >
            struct_constraint;

        gum::learning::ParamEstimatorML estimator(parser, prior, score.internalPrior());

        gum::learning::StructuralConstraintSetStatic<
            gum::learning::StructuralConstraintForbiddenArcs >
            invariable_constraints;

        gum::learning::GraphChangesSelector4DiGraph< decltype(invariable_constraints),
                                                     decltype(struct_constraint) >
            selector(score, invariable_constraints, struct_constraint);

        gum::learning::GreedyHillClimbing search;

        gum::DAG dag = search.learnStructure(selector);
        // std::cout << dag << std::endl;

        gum::DAG              xdag;
        std::vector< double > scores(nb_vars);
        for (auto node: dag) {
          xdag.addNodeWithId(node);
          scores[std::size_t(node)] = _score_(score, node, xdag);
        }

        gum::Sequence< gum::NodeId > total_order;
        while (
            _applyNextChange_(score, scores, xdag, true, true, true, false, -1, -1, total_order)) {}

        CHECK_EQ(xdag, dag);
      }
    }

    static void xtest_alarm1() {
      /*
      gum::learning::DatabaseFromCSV
      database(GET_RESSOURCES_PATH("csv/alarm.csv"));

      gum::learning::DBRowTranslatorSet<
        gum::learning::CellTranslatorCompactIntId >
        translators;
      translators.insertTranslator(0, 37);

      gum::learning::FilteredRowGeneratorSet< gum::learning::RowGeneratorIdentity >
        generators;
      generators.insertGenerator();

      auto filter =
        gum::learning::make_DB_row_filter(database, translators, generators);

      std::vector< gum::Idx > modalities = filter.modalities();

      gum::learning::SmoothingPrior prior;
      gum::learning::ScoreBDeu        score(filter, modalities, prior);

      gum::learning::StructuralConstraintSetStatic<
        gum::learning::StructuralConstraintDAG >
        struct_constraint;

      gum::learning::ParamEstimatorML estimator(
        filter, modalities, prior, score.internalPrior());

      gum::learning::GraphChangesGenerator4DiGraph< decltype(struct_constraint) >
        op_set(struct_constraint);

      gum::learning::GraphChangesSelector4DiGraph< decltype(score),
                                                   decltype(struct_constraint),
                                                   decltype(op_set) >
        selector(score, struct_constraint, op_set);

      gum::learning::GreedyHillClimbing search;

      try {
        gum::Timer timer;
        gum::DAG   bn = search.learnStructure(selector, modalities);
        std::cout << timer.step() << " : " << std::endl;
        std::cout << bn << std::endl;
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
      }
      */
    }

    void xtest_alarm1bis() {
      /*
      gum::learning::DatabaseFromCSV
      database(GET_RESSOURCES_PATH("csv/alarm.csv"));

      gum::learning::DBRowTranslatorSet<
        gum::learning::CellTranslatorCompactIntId >
        translators;
      translators.insertTranslator(0, 37);

      gum::learning::FilteredRowGeneratorSet< gum::learning::RowGeneratorIdentity >
        generators;
      generators.insertGenerator();

      auto filter =
        gum::learning::make_DB_row_filter(database, translators, generators);

      std::vector< gum::Idx > modalities = filter.modalities();

      gum::learning::SmoothingPrior prior;
      gum::learning::ScoreBDeu        score(filter, modalities, prior);

      gum::learning::StructuralConstraintSetStatic<
        gum::learning::StructuralConstraintDAG >
        struct_constraint;

      gum::learning::ParamEstimatorML estimator(
        filter, modalities, prior, score.internalPrior());

      gum::learning::GraphChangesGenerator4DiGraph< decltype(struct_constraint) >
        op_set(struct_constraint);

      gum::learning::GraphChangesSelector4DiGraph< decltype(score),
                                                   decltype(struct_constraint),
                                                   decltype(op_set) >
        selector(score, struct_constraint, op_set);

      gum::learning::GreedyHillClimbing search;

      try {
        gum::Timer timer;
        gum::DAG   bn = search.learnStructure(selector, modalities);
        std::cout << timer.step() << " : " << std::endl;
        std::cout << bn << std::endl;
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
      }
      */
    }

    void xtest_alarm1ter() {
      /*
      gum::learning::DatabaseFromCSV
      database(GET_RESSOURCES_PATH("csv/alarm.csv"));

      gum::learning::DBRowTranslatorSet<
        gum::learning::CellTranslatorCompactIntId >
        translators;
      translators.insertTranslator(0, 37);

      gum::learning::FilteredRowGeneratorSet< gum::learning::RowGeneratorIdentity >
        generators;
      generators.insertGenerator();

      auto filter =
        gum::learning::make_DB_row_filter(database, translators, generators);

      std::vector< gum::Idx > modalities = filter.modalities();

      gum::learning::SmoothingPrior prior;
      gum::learning::ScoreBDeu        score(filter, modalities, prior);

      gum::learning::StructuralConstraintSetStatic<
        gum::learning::StructuralConstraintDAG >
        struct_constraint;

      gum::learning::ParamEstimatorML estimator(
        filter, modalities, prior, score.internalPrior());

      gum::learning::GraphChangesGenerator4DiGraph< decltype(struct_constraint) >
        op_set(struct_constraint);

      gum::learning::GraphChangesSelector4DiGraph< decltype(score),
                                                   decltype(struct_constraint),
                                                   decltype(op_set) >
        selector(score, struct_constraint, op_set);

      gum::learning::GreedyHillClimbing search;

      try {
        gum::Timer timer;
        gum::DAG   bn = search.learnStructure(selector, modalities);
        std::cout << timer.step() << " : " << std::endl;
        std::cout << bn << std::endl;
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
      }
      */
    }

    void xtest_alarm2() {
      /*
      gum::learning::DatabaseFromCSV
      database(GET_RESSOURCES_PATH("csv/alarm.csv"));

      gum::learning::DBRowTranslatorSet<
        gum::learning::CellTranslatorCompactIntId >
        translators;
      translators.insertTranslator(
        gum::learning::CellTranslatorCompactIntId(), 0, 37);

      gum::learning::FilteredRowGeneratorSet< gum::learning::RowGeneratorIdentity >
        generators;
      generators.insertGenerator();

      auto filter =
        gum::learning::make_DB_row_filter(database, translators, generators);

      std::vector< gum::Idx > modalities = filter.modalities();

      gum::learning::SmoothingPrior prior;
      gum::learning::ScoreBDeu        score(filter, modalities, prior);

      gum::learning::StructuralConstraintSetStatic<
        gum::learning::StructuralConstraintDAG >
        struct_constraint;

      gum::learning::ParamEstimatorML estimator(
        filter, modalities, prior, score.internalPrior());

      gum::learning::GraphChangesGenerator4DiGraph< decltype(struct_constraint) >
        op_set(struct_constraint);

      gum::learning::GraphChangesSelector4DiGraph< decltype(score),
                                                   decltype(struct_constraint),
                                                   decltype(op_set) >
        selector(score, struct_constraint, op_set);

      gum::learning::GreedyHillClimbing search;

      try {
        gum::Timer timer;
        gum::DAG   bn = search.learnStructure(selector, modalities);
        std::cout << timer.step() << " : " << std::endl;
        std::cout << bn << std::endl;
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
      }
      */
    }

    void xtest_alarm3() {
      /*
      gum::learning::DatabaseFromCSV
      database(GET_RESSOURCES_PATH("csv/alarm.csv"));

      gum::learning::DBRowTranslatorSet< gum::learning::DBCellTranslator< 1, 1 > >
        translators;
      translators.insertTranslator(gum::learning::CellTranslatorCompactIntId(),
                                   0,
                                   database.content()[0].size());

      gum::learning::FilteredRowGeneratorSet< gum::learning::RowGeneratorIdentity >
        generators;
      generators.insertGenerator();

      auto filter =
        gum::learning::make_DB_row_filter(database, translators, generators);

      std::vector< gum::Idx > modalities = filter.modalities();

      gum::learning::SmoothingPrior prior;
      gum::learning::ScoreBDeu        score(filter, modalities, prior);

      gum::learning::StructuralConstraintSetStatic<
        gum::learning::StructuralConstraintDAG >
        struct_constraint;

      gum::learning::ParamEstimatorML estimator(
        filter, modalities, prior, score.internalPrior());

      gum::learning::GraphChangesGenerator4DiGraph< decltype(struct_constraint) >
        op_set(struct_constraint);

      gum::learning::GraphChangesSelector4DiGraph< decltype(score),
                                                   decltype(struct_constraint),
                                                   decltype(op_set) >
        selector(score, struct_constraint, op_set);

      gum::learning::GreedyHillClimbing search;

      try {
        gum::Timer timer;
        gum::DAG   bn = search.learnStructure(selector, modalities);
        std::cout << timer.step() << " : " << std::endl;
        std::cout << bn << std::endl;
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
      }
      */
    }

    void xtest_alarm4() {
      /*
      gum::learning::DatabaseFromCSV
      database(GET_RESSOURCES_PATH("csv/alarm.csv"));

      gum::learning::DBRowTranslatorSet< gum::learning::DBCellTranslator< 1, 1 > >
        translators;
      translators.insertTranslator(gum::learning::CellTranslatorCompactIntId(),
                                   0,
                                   database.content()[0].size());

      gum::learning::FilteredRowGeneratorSet< gum::learning::RowGeneratorIdentity >
        generators;
      generators.insertGenerator();

      auto filter =
        gum::learning::make_DB_row_filter(database, translators, generators);

      std::vector< gum::Idx > modalities = filter.modalities();

      gum::learning::SmoothingPrior prior;
      gum::learning::ScoreBDeu        score(filter, modalities, prior);

      gum::learning::StructuralConstraintSetStatic<
        gum::learning::StructuralConstraintDAG >
        struct_constraint;

      gum::learning::ParamEstimatorML estimator(filter, modalities, prior);

      gum::learning::GraphChangesGeneratorOnSubDiGraph< decltype(
        struct_constraint) >
        op_set(struct_constraint);

      gum::NodeSet targets{0, 1, 2};
      op_set.setTargets(targets);
      op_set.setTails(gum::Size(modalities.size()));

      gum::learning::GraphChangesSelector4DiGraph< decltype(score),
                                                   decltype(struct_constraint),
                                                   decltype(op_set) >
        selector(score, struct_constraint, op_set);

      gum::learning::GreedyHillClimbing search;

      try {
        gum::Timer timer;
        gum::DAG   bn_dag = search.learnStructure(selector, modalities);
        std::cout << timer.step() << " : " << std::endl;
        std::cout << bn_dag << std::endl;

        gum::BayesNet< double > bn = search.learnBN(selector,
                                                    estimator,
                                                    database.variableNames(),
                                                    modalities,
                                                    filter.translatorSet());

      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
      }
      */
    }
  };

  GUM_TEST_ACTIF(_K2_asia)
  GUM_TEST_ACTIF(_asia_with_ordered_values)
  GUM_TEST_ACTIF(_asia_with_extendedGHC)
  GUM_TEST_ACTIF(_alarm_with_ordered_values)
  GUM_TEST_ACTIF(_alarm_with_ordered_values2)
  GUM_TEST_ACTIF(_alarm_with_extendedGHC)
  GUM_TEST_ACTIF(_dirichlet)

} /* namespace gum_tests */
