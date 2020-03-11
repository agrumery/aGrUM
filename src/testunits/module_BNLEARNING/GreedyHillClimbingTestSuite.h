
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
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


#include <agrum/BN/BayesNet.h>
#include <agrum/tools/graphs/DAG.h>
#include <agrum/tools/variables/labelizedVariable.h>
#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>
#include <iostream>

#include <agrum/tools/database/DBTranslator4LabelizedVariable.h>
#include <agrum/tools/database/DBRowGeneratorParser.h>
#include <agrum/tools/database/DBInitializerFromCSV.h>
#include <agrum/tools/database/databaseTable.h>
#include <agrum/tools/database/DBTranslatorSet.h>

#include <agrum/BN/learning/scores_and_tests/scoreBDeu.h>
#include <agrum/BN/learning/scores_and_tests/scoreK2.h>
#include <agrum/BN/learning/scores_and_tests/scoreBIC.h>

#include <agrum/BN/learning/aprioris/aprioriSmoothing.h>
#include <agrum/BN/learning/aprioris/aprioriDirichletFromDatabase.h>

#include <agrum/BN/learning/constraints/structuralConstraintDAG.h>
#include <agrum/BN/learning/constraints/structuralConstraintDiGraph.h>
#include <agrum/BN/learning/constraints/structuralConstraintIndegree.h>
#include <agrum/BN/learning/constraints/structuralConstraintSetStatic.h>
#include <agrum/BN/learning/constraints/structuralConstraintSliceOrder.h>

#include <agrum/BN/learning/structureUtils/graphChangesGenerator4DiGraph.h>
#include <agrum/BN/learning/structureUtils/graphChangesGenerator4UndiGraph.h>
#include <agrum/BN/learning/structureUtils/graphChangesGeneratorOnSubDiGraph.h>
#include <agrum/BN/learning/structureUtils/graphChangesSelector4DiGraph.h>

#include <agrum/BN/learning/greedyHillClimbing.h>
#include <agrum/BN/learning/paramUtils/paramEstimatorML.h>

#include <agrum/tools/core/approximations/approximationScheme.h>
#include <agrum/tools/core/approximations/approximationSchemeListener.h>

namespace gum_tests {

  class simpleListenerForGHC: public gum::ApproximationSchemeListener {
    private:
    int         __nbr;
    std::string __mess;

    public:
    simpleListenerForGHC(gum::ApproximationScheme& sch) :
        gum::ApproximationSchemeListener(sch), __nbr(0), __mess(""){};

    void whenProgress(const void*     buffer,
                      const gum::Size a,
                      const double    b,
                      const double    c) {
      __nbr++;
      std::cout << __nbr << ": error = " << b << std::endl;
    }

    void whenStop(const void* buffer, const std::string s) { __mess = s; }

    int getNbr() { return __nbr; }

    std::string getMess() { return __mess; }
  };


  class GreedyHillClimbingTestSuite: public CxxTest::TestSuite {
    private:
    double __score(gum::learning::ScoreBIC<>& score,
                   const gum::NodeId&         node,
                   const gum::DAG&            dag) {
      std::vector< gum::NodeId > cond_set;
      for (const auto par: dag.parents(node)) {
        cond_set.push_back(par);
      }
      return score.score(node, cond_set);
    }

    bool __applyNextChange(gum::learning::ScoreBIC<>& score,
                           std::vector< double >&     current_scores,
                           gum::DAG&                  dag) {
      const int nb_vars = int(dag.size());

      std::vector< std::pair< gum::learning::GraphChange, double > > changes;

      for (int i = 0; i < nb_vars; ++i) {
        for (int j = 0; j < nb_vars; ++j) {
          if (i != j) {
            // check add arc
            if (!dag.existsArc(gum::Arc(i, j))) {
              try {
                dag.addArc(gum::NodeId(i), gum::NodeId(j));
                double new_score = __score(score, j, dag) - current_scores[j];
                if (new_score > 0) {
                  changes.push_back(
                     std::pair< gum::learning::GraphChange, double >(
                        gum::learning::GraphChange(
                           gum::learning::GraphChangeType::ARC_ADDITION,
                           gum::NodeId(i),
                           gum::NodeId(j)),
                        new_score));
                }
                dag.eraseArc(gum::Arc(i, j));
              } catch (...) {}
            }

            // check remove arc
            if (dag.existsArc(gum::Arc(i, j))) {
              dag.eraseArc(gum::Arc(i, j));
              double new_score = __score(score, j, dag) - current_scores[j];
              if (new_score > 0) {
                changes.push_back(std::pair< gum::learning::GraphChange, double >(
                   gum::learning::GraphChange(
                      gum::learning::GraphChangeType::ARC_DELETION,
                      gum::NodeId(i),
                      gum::NodeId(j)),
                   new_score));
              }
              dag.addArc(gum::NodeId(i), gum::NodeId(j));
            }

            // check reverse arc
            if (dag.existsArc(gum::Arc(i, j))) {
              dag.eraseArc(gum::Arc(i, j));
              try {
                dag.addArc(j, i);
                double new_score_i = __score(score, i, dag) - current_scores[i];
                double new_score_j = __score(score, j, dag) - current_scores[j];
                double new_score = new_score_i + new_score_j;
                if (new_score > 0) {
                  changes.push_back(
                     std::pair< gum::learning::GraphChange, double >(
                        gum::learning::GraphChange(
                           gum::learning::GraphChangeType::ARC_REVERSAL,
                           gum::NodeId(i),
                           gum::NodeId(j)),
                        new_score));
                }
                dag.eraseArc(gum::Arc(j, i));
                dag.addArc(i, j);
              } catch (...) { dag.addArc(i, j); }
            }
          }
        }
      }

      if (changes.empty()) return false;

      // get the best change
      std::size_t best_i = std::size_t(0);
      for (std::size_t i = std::size_t(0); i < changes.size(); ++i) {
        if (changes[i].second > changes[best_i].second) best_i = i;
      }

      // apply the best change
      switch (changes[best_i].first.type()) {
        case gum::learning::GraphChangeType::ARC_ADDITION:
          dag.addArc(changes[best_i].first.node1(), changes[best_i].first.node2());
          current_scores[changes[best_i].first.node2()] += changes[best_i].second;
          break;

        case gum::learning::GraphChangeType::ARC_DELETION:
          dag.eraseArc(gum::Arc(changes[best_i].first.node1(),
                                changes[best_i].first.node2()));
          current_scores[changes[best_i].first.node2()] += changes[best_i].second;
          break;

        case gum::learning::GraphChangeType::ARC_REVERSAL:
          dag.eraseArc(gum::Arc(changes[best_i].first.node1(),
                                changes[best_i].first.node2()));
          dag.addArc(changes[best_i].first.node2(), changes[best_i].first.node1());
          current_scores[changes[best_i].first.node1()] =
             __score(score, changes[best_i].first.node1(), dag);
          current_scores[changes[best_i].first.node2()] =
             __score(score, changes[best_i].first.node2(), dag);
          break;

        default: break;
      }

      return true;
    }


    public:
    void test_k2_asia() {
      gum::learning::DBInitializerFromCSV<> initializer(
         GET_RESSOURCES_PATH("asia.csv"));
      const auto&       var_names = initializer.variableNames();
      const std::size_t nb_vars = var_names.size();

      gum::learning::DBTranslatorSet<>                translator_set;
      gum::learning::DBTranslator4LabelizedVariable<> translator;
      for (std::size_t i = 0; i < nb_vars; ++i) {
        translator_set.insertTranslator(translator, i);
      }

      gum::learning::DatabaseTable<> database(translator_set);
      database.setVariableNames(initializer.variableNames());
      initializer.fillDatabase(database);
      // database.reorder();

      gum::learning::DBRowGeneratorSet<>    genset;
      gum::learning::DBRowGeneratorParser<> parser(database.handler(), genset);
      gum::learning::AprioriSmoothing<>     apriori(database);
      gum::learning::ScoreK2<>              score(parser, apriori);

      gum::learning::StructuralConstraintSetStatic<
         gum::learning::StructuralConstraintDAG,
         gum::learning::StructuralConstraintIndegree
         // gum::learning::StructuralConstraintSliceOrder
         >
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
      static_cast< gum::learning::StructuralConstraintIndegree& >(
         struct_constraint) = constraint1;

      gum::learning::ParamEstimatorML<> estimator(
         parser, apriori, score.internalApriori());

      gum::learning::GraphChangesGenerator4DiGraph< decltype(struct_constraint) >
         op_set(struct_constraint);

      gum::learning::GraphChangesSelector4DiGraph< decltype(struct_constraint),
                                                   decltype(op_set) >
         selector(score, struct_constraint, op_set);

      gum::learning::GreedyHillClimbing search;
      // simpleListenerForGHC agsl ( search );
      search.approximationScheme().setEpsilon(1000);

      gum::DAG dag = search.learnStructure(selector);
      TS_ASSERT(dag.arcs().size() == 11);

      // gum::BayesNet<double> bn =
      // search.learnBN<double> ( selector, estimator,
      // database.variableNames (),
      // modalities );

      // gum::BayesNet<double> bn2 =
      // search.learnBN ( selector, estimator,
      // database.variableNames (),
      // modalities );
    }

    void test_asia_with_ordered_values() {
      gum::learning::DBInitializerFromCSV<> initializer(
         GET_RESSOURCES_PATH("asia.csv"));
      const auto&       var_names = initializer.variableNames();
      const std::size_t nb_vars = var_names.size();

      gum::learning::DBTranslatorSet<> translator_set;
      gum::LabelizedVariable           xvar("var", "", 0);
      xvar.addLabel("0");
      xvar.addLabel("1");
      gum::learning::DBTranslator4LabelizedVariable<> translator(xvar);
      for (std::size_t i = 0; i < nb_vars; ++i) {
        translator_set.insertTranslator(translator, i);
      }

      gum::learning::DatabaseTable<> database(translator_set);
      database.setVariableNames(initializer.variableNames());
      initializer.fillDatabase(database);

      gum::learning::DBRowGeneratorSet<>    genset;
      gum::learning::DBRowGeneratorParser<> parser(database.handler(), genset);
      gum::learning::AprioriSmoothing<>     apriori(database);
      gum::learning::ScoreK2<>              score(parser, apriori);

      gum::learning::StructuralConstraintSetStatic<
         gum::learning::StructuralConstraintDAG,
         gum::learning::StructuralConstraintIndegree
         // gum::learning::StructuralConstraintSliceOrder
         >
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
      static_cast< gum::learning::StructuralConstraintIndegree& >(
         struct_constraint) = constraint1;

      gum::learning::ParamEstimatorML<> estimator(
         parser, apriori, score.internalApriori());

      gum::learning::GraphChangesGenerator4DiGraph< decltype(struct_constraint) >
         op_set(struct_constraint);

      gum::learning::GraphChangesSelector4DiGraph< decltype(struct_constraint),
                                                   decltype(op_set) >
         selector(score, struct_constraint, op_set);

      gum::learning::GreedyHillClimbing search;
      // simpleListenerForGHC agsl ( search );
      search.approximationScheme().setEpsilon(1000);

      gum::DAG dag = search.learnStructure(selector);
      TS_ASSERT(dag.arcs().size() == 11);

      gum::BayesNet< double > bn = search.learnBN< double >(selector, estimator);

      const std::string s0 = "0";
      const std::string s1 = "1";
      for (gum::Idx i = 0; i < database.nbVariables(); ++i) {
        const gum::DiscreteVariable& var = bn.variable(i);
        TS_ASSERT(var.label(0) == s0);
        TS_ASSERT(var.label(1) == s1);
      }
    }


    void test_alarm_with_ordered_values() {
      gum::learning::DBInitializerFromCSV<> initializer(
         GET_RESSOURCES_PATH("alarm.csv"));
      const auto&       var_names = initializer.variableNames();
      const std::size_t nb_vars = var_names.size();

      gum::learning::DBTranslatorSet<> translator_set;
      gum::LabelizedVariable           xvar("var", "", 0);
      xvar.addLabel("0");
      xvar.addLabel("1");
      xvar.addLabel("2");
      xvar.addLabel("3");
      gum::learning::DBTranslator4LabelizedVariable<> translator(xvar, true);
      for (std::size_t i = 0; i < nb_vars; ++i) {
        translator_set.insertTranslator(translator, i);
      }

      gum::learning::DatabaseTable<> database(translator_set);
      database.setVariableNames(initializer.variableNames());
      initializer.fillDatabase(database);

      gum::learning::DBRowGeneratorSet<>    genset;
      gum::learning::DBRowGeneratorParser<> parser(database.handler(), genset);
      gum::learning::AprioriSmoothing<>     apriori(database);
      gum::learning::ScoreK2<>              score(parser, apriori);
      // score.setMaxNbThreads(24);

      gum::learning::StructuralConstraintSetStatic<
         gum::learning::StructuralConstraintDAG,
         gum::learning::StructuralConstraintIndegree
         // gum::learning::StructuralConstraintSliceOrder
         >
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
      static_cast< gum::learning::StructuralConstraintIndegree& >(
         struct_constraint) = constraint1;

      gum::learning::ParamEstimatorML<> estimator(
         parser, apriori, score.internalApriori());

      gum::learning::GraphChangesGenerator4DiGraph< decltype(struct_constraint) >
         op_set(struct_constraint);

      gum::learning::GraphChangesSelector4DiGraph< decltype(struct_constraint),
                                                   decltype(op_set) >
         selector(score, struct_constraint, op_set);

      gum::learning::GreedyHillClimbing search;
      // simpleListenerForGHC agsl ( search );
      search.approximationScheme().setEpsilon(1000);

      gum::BayesNet< double > bn = search.learnBN< double >(selector, estimator);

      const std::string    s0 = "0";
      const std::string    s1 = "1";
      const std::string    s2 = "2";
      gum::Set< gum::Idx > seq{1, 10, 11, 14};
      for (gum::Idx i = 0; i < database.nbVariables(); ++i) {
        const gum::DiscreteVariable& var = bn.variable(i);
        TS_ASSERT(var.label(0) == s0);
        TS_ASSERT(var.label(1) == s1);
        if (seq.exists(i)) { TS_ASSERT(var.label(2) == s2); }
      }
    }

    void test_alarm_with_ordered_values2() {
      gum::learning::DBInitializerFromCSV<> initializer(
         GET_RESSOURCES_PATH("alarm.csv"));
      const auto&       var_names = initializer.variableNames();
      const std::size_t nb_vars = var_names.size();

      gum::learning::DBTranslatorSet<> translator_set;
      gum::LabelizedVariable           xvar("var", "", 0);
      xvar.addLabel("0");
      xvar.addLabel("1");
      xvar.addLabel("2");
      xvar.addLabel("3");
      gum::learning::DBTranslator4LabelizedVariable<> translator1(xvar);
      gum::learning::DBTranslator4LabelizedVariable<> translator2;
      for (std::size_t i = 0; i < nb_vars; ++i) {
        if ((i == 1) || (i == 10) || (i == 11) || (i == 14))
          translator_set.insertTranslator(translator1, i);
        else
          translator_set.insertTranslator(translator2, i);
      }

      gum::learning::DatabaseTable<> database(translator_set);
      database.setVariableNames(initializer.variableNames());
      initializer.fillDatabase(database);

      gum::learning::DBRowGeneratorSet<>    genset;
      gum::learning::DBRowGeneratorParser<> parser(database.handler(), genset);
      gum::learning::AprioriSmoothing<>     apriori(database);
      gum::learning::ScoreK2<>              score(parser, apriori);

      gum::learning::StructuralConstraintSetStatic<
         gum::learning::StructuralConstraintDAG,
         gum::learning::StructuralConstraintIndegree
         // gum::learning::StructuralConstraintSliceOrder
         >
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
      static_cast< gum::learning::StructuralConstraintIndegree& >(
         struct_constraint) = constraint1;

      gum::learning::ParamEstimatorML<> estimator(
         parser, apriori, score.internalApriori());

      gum::learning::GraphChangesGenerator4DiGraph< decltype(struct_constraint) >
         op_set(struct_constraint);

      gum::learning::GraphChangesSelector4DiGraph< decltype(struct_constraint),
                                                   decltype(op_set) >
         selector(score, struct_constraint, op_set);

      gum::learning::GreedyHillClimbing search;
      // simpleListenerForGHC agsl ( search );
      search.approximationScheme().setEpsilon(1000);

      gum::BayesNet< double > bn = search.learnBN< double >(selector, estimator);

      const std::string    s0 = "0";
      const std::string    s1 = "1";
      const std::string    s2 = "2";
      gum::Set< gum::Idx > seq{1, 10, 11, 14};
      for (auto i: seq) {
        const gum::DiscreteVariable& var = bn.variable(i);
        TS_ASSERT(var.label(0) == s0);
        TS_ASSERT(var.label(1) == s1);
        TS_ASSERT(var.label(2) == s2);
      }
    }


    void test_dirichlet() {
      // read the learning database
      gum::learning::DBInitializerFromCSV<> initializer(
         GET_RESSOURCES_PATH("db_dirichlet_learning.csv"));
      const auto&       var_names = initializer.variableNames();
      const std::size_t nb_vars = var_names.size();

      gum::learning::DBTranslatorSet<>                translator_set;
      gum::learning::DBTranslator4LabelizedVariable<> translator;
      for (std::size_t i = 0; i < nb_vars; ++i) {
        translator_set.insertTranslator(translator, i);
      }

      gum::learning::DatabaseTable<> database(translator_set);
      database.setVariableNames(initializer.variableNames());
      initializer.fillDatabase(database);


      // read the apriori database
      gum::learning::DBInitializerFromCSV<> dirichlet_initializer(
         GET_RESSOURCES_PATH("db_dirichlet_apriori.csv"));
      const auto&       dirichlet_var_names = initializer.variableNames();
      const std::size_t dirichlet_nb_vars = dirichlet_var_names.size();

      gum::learning::DBTranslatorSet<> dirichlet_translator_set;
      for (std::size_t i = 0; i < dirichlet_nb_vars; ++i) {
        dirichlet_translator_set.insertTranslator(translator, i);
      }

      gum::learning::DatabaseTable<> dirichlet_database(dirichlet_translator_set);
      dirichlet_database.setVariableNames(dirichlet_initializer.variableNames());
      dirichlet_initializer.fillDatabase(dirichlet_database);


      // create the score and the apriori
      gum::learning::DBRowGeneratorSet<>    dirichlet_genset;
      gum::learning::DBRowGeneratorParser<> dirichlet_parser(
         dirichlet_database.handler(), dirichlet_genset);
      gum::learning::AprioriDirichletFromDatabase<> apriori(dirichlet_database,
                                                            dirichlet_parser);

      gum::learning::DBRowGeneratorSet<>    genset;
      gum::learning::DBRowGeneratorParser<> parser(database.handler(), genset);

      std::vector< double > weights{0, 1.0, 5.0, 10.0, 1000.0, 7000.0, 100000.0};

      for (const auto weight: weights) {
        apriori.setWeight(weight);
        gum::learning::ScoreBIC<> score(parser, apriori);


        // finalize the learning algorithm
        gum::learning::StructuralConstraintSetStatic<
           gum::learning::StructuralConstraintDAG >
           struct_constraint;

        gum::learning::ParamEstimatorML<> estimator(
           parser, apriori, score.internalApriori());

        gum::learning::GraphChangesGenerator4DiGraph< decltype(struct_constraint) >
           op_set(struct_constraint);

        gum::learning::GraphChangesSelector4DiGraph< decltype(struct_constraint),
                                                     decltype(op_set) >
           selector(score, struct_constraint, op_set);

        gum::learning::GreedyHillClimbing search;

        gum::DAG dag = search.learnStructure(selector);
        // std::cout << dag << std::endl;

        gum::DAG xdag;
        for (auto node: dag)
          xdag.addNodeWithId(node);

        std::vector< double > scores(nb_vars);
        for (auto node: xdag)
          scores[std::size_t(node)] = __score(score, node, xdag);

        while (__applyNextChange(score, scores, xdag)) {}

        TS_ASSERT(xdag == dag);
      }
    }


    void xtest_alarm1() {
      /*
      gum::learning::DatabaseFromCSV database(GET_RESSOURCES_PATH("alarm.csv"));

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

      gum::learning::AprioriSmoothing<> apriori;
      gum::learning::ScoreBDeu<>        score(filter, modalities, apriori);

      gum::learning::StructuralConstraintSetStatic<
        gum::learning::StructuralConstraintDAG >
        struct_constraint;

      gum::learning::ParamEstimatorML<> estimator(
        filter, modalities, apriori, score.internalApriori());

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
      gum::learning::DatabaseFromCSV database(GET_RESSOURCES_PATH("alarm.csv"));

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

      gum::learning::AprioriSmoothing<> apriori;
      gum::learning::ScoreBDeu<>        score(filter, modalities, apriori);

      gum::learning::StructuralConstraintSetStatic<
        gum::learning::StructuralConstraintDAG >
        struct_constraint;

      gum::learning::ParamEstimatorML<> estimator(
        filter, modalities, apriori, score.internalApriori());

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
      gum::learning::DatabaseFromCSV database(GET_RESSOURCES_PATH("alarm.csv"));

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

      gum::learning::AprioriSmoothing<> apriori;
      gum::learning::ScoreBDeu<>        score(filter, modalities, apriori);

      gum::learning::StructuralConstraintSetStatic<
        gum::learning::StructuralConstraintDAG >
        struct_constraint;

      gum::learning::ParamEstimatorML<> estimator(
        filter, modalities, apriori, score.internalApriori());

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
      gum::learning::DatabaseFromCSV database(GET_RESSOURCES_PATH("alarm.csv"));

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

      gum::learning::AprioriSmoothing<> apriori;
      gum::learning::ScoreBDeu<>        score(filter, modalities, apriori);

      gum::learning::StructuralConstraintSetStatic<
        gum::learning::StructuralConstraintDAG >
        struct_constraint;

      gum::learning::ParamEstimatorML<> estimator(
        filter, modalities, apriori, score.internalApriori());

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
      gum::learning::DatabaseFromCSV database(GET_RESSOURCES_PATH("alarm.csv"));

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

      gum::learning::AprioriSmoothing<> apriori;
      gum::learning::ScoreBDeu<>        score(filter, modalities, apriori);

      gum::learning::StructuralConstraintSetStatic<
        gum::learning::StructuralConstraintDAG >
        struct_constraint;

      gum::learning::ParamEstimatorML<> estimator(
        filter, modalities, apriori, score.internalApriori());

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
      gum::learning::DatabaseFromCSV database(GET_RESSOURCES_PATH("alarm.csv"));

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

      gum::learning::AprioriSmoothing<> apriori;
      gum::learning::ScoreBDeu<>        score(filter, modalities, apriori);

      gum::learning::StructuralConstraintSetStatic<
        gum::learning::StructuralConstraintDAG >
        struct_constraint;

      gum::learning::ParamEstimatorML<> estimator(filter, modalities, apriori);

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

} /* namespace gum_tests */
