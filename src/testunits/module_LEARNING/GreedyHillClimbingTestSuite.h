/***************************************************************************
 *   Copyright (C) 2007 by Lionel Torti                                    *
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

#include <iostream>
#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>
#include <agrum/graphs/DAG.h>
#include <agrum/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>

#include <agrum/learning/database/databaseFromCSV.h>
#include <agrum/learning/database/DBCellTranslators/cellTranslatorCompactIntId.h>
#include <agrum/learning/database/DBCellTranslators/cellTranslatorNumber.h>
#include <agrum/learning/database/DBCellTranslators/cellTranslatorString.h>
#include <agrum/learning/database/DBCellTranslators/cellTranslatorUniversal.h>
#include <agrum/learning/database/filteredRowGenerators/rowGeneratorIdentity.h>

#include <agrum/learning/scores_and_tests/scoreK2.h>
#include <agrum/learning/scores_and_tests/scoreBDeu.h>

#include <agrum/learning/aprioris/aprioriSmoothing.h>

#include <agrum/learning/constraints/structuralConstraintDiGraph.h>
#include <agrum/learning/constraints/structuralConstraintDAG.h>
#include <agrum/learning/constraints/structuralConstraintIndegree.h>
#include <agrum/learning/constraints/structuralConstraintSliceOrder.h>
#include <agrum/learning/constraints/structuralConstraintSetStatic.h>

#include <agrum/learning/structureUtils/graphChangesSelector4DiGraph.h>
#include <agrum/learning/structureUtils/graphChangesGenerator4UndiGraph.h>
#include <agrum/learning/structureUtils/graphChangesGeneratorOnSubDiGraph.h>
#include <agrum/learning/structureUtils/graphChangesGenerator4DiGraph.h>

#include <agrum/learning/paramUtils/paramEstimatorML.h>
#include <agrum/learning/greedyHillClimbing.h>

#include <agrum/core/algorithms/approximationScheme/approximationScheme.h>
#include <agrum/core/algorithms/approximationScheme/approximationSchemeListener.h>

#define MY_ALARM GET_PATH_STR("alarm.csv")
#define MY_ASIA GET_PATH_STR("asia.csv")

namespace gum_tests {

  class simpleListenerForGHC : public gum::ApproximationSchemeListener {
    private:
    int __nbr;
    std::string __mess;

    public:
    simpleListenerForGHC(gum::ApproximationScheme &sch)
        : gum::ApproximationSchemeListener(sch), __nbr(0), __mess(""){};

    void whenProgress(const void *buffer, gum::Size a, double b, double c) {
      __nbr++;
      std::cout << __nbr << ": error = " << b << std::endl;
    }

    void whenStop(const void *buffer, std::string s) { __mess = s; }

    int getNbr() { return __nbr; }

    std::string getMess() { return __mess; }
  };

  class GreedyHillClimbingTestSuite : public CxxTest::TestSuite {
    public:
    void test_k2_asia() {
      gum::learning::DatabaseFromCSV database(MY_ASIA);

      gum::learning::DBRowTranslatorSetDynamic<
          gum::learning::CellTranslatorCompactIntId> translators;
      translators.insertTranslator(gum::learning::Col<0>(), database.nbVariables());

      // auto translators = gum::learning::make_translators
      //  ( gum::learning::Create<gum::learning::CellTranslatorCompactIntId,
      //                          gum::learning::Col<0>, 8 > () );

      auto generators =
          gum::learning::make_generators(gum::learning::RowGeneratorIdentity());

      auto filter =
          gum::learning::make_DB_row_filter(database, translators, generators);

      std::vector<unsigned int> modalities = filter.modalities();

      gum::learning::AprioriSmoothing<> apriori;
      gum::learning::ScoreK2<> score(filter, modalities, apriori);

      gum::learning::StructuralConstraintSetStatic<
          gum::learning::StructuralConstraintDAG,
          gum::learning::StructuralConstraintIndegree
          // gum::learning::StructuralConstraintSliceOrder
          > struct_constraint;

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
      static_cast<gum::learning::StructuralConstraintIndegree &>(struct_constraint) =
          constraint1;

      gum::learning::ParamEstimatorML<> estimator(filter, modalities, apriori,
                                                  score.internalApriori());

      gum::learning::GraphChangesGenerator4DiGraph<decltype(struct_constraint)>
          op_set(struct_constraint);

      gum::learning::GraphChangesSelector4DiGraph<
          decltype(score), decltype(struct_constraint), decltype(op_set)>
          selector(score, struct_constraint, op_set);

      gum::learning::GreedyHillClimbing search;
      // simpleListenerForGHC agsl ( search );
      search.approximationScheme().setEpsilon(1000);

      gum::DAG dag = search.learnStructure(selector, modalities);
      TS_ASSERT(dag.arcs().size() == 11);
      /*
        gum::BayesNet<double> bn =
        search.learnBN<double> ( selector, estimator,
        database.variableNames (),
        modalities );

        gum::BayesNet<float> bn2 =
        search.learnBN ( selector, estimator,
        database.variableNames (),
        modalities );
      */
    }

    void test_asia_with_ordered_values() {
      gum::learning::DatabaseFromCSV database(MY_ASIA);

      gum::learning::DBRowTranslatorSetDynamic<
          gum::learning::CellTranslatorUniversal> translators;
      gum::learning::CellTranslatorUniversal one_translator(
          gum::Sequence<float>{0, 1}, false);
      translators.insertTranslator(one_translator, gum::learning::Col<0>(),
                                   database.nbVariables());

      // auto translators = gum::learning::make_translators
      //  ( gum::learning::Create<gum::learning::CellTranslatorCompactIntId,
      //                          gum::learning::Col<0>, 8 > () );

      auto generators =
          gum::learning::make_generators(gum::learning::RowGeneratorIdentity());

      auto filter =
          gum::learning::make_DB_row_filter(database, translators, generators);

      std::vector<unsigned int> modalities = filter.modalities();

      gum::learning::AprioriSmoothing<> apriori;
      gum::learning::ScoreK2<> score(filter, modalities, apriori);

      gum::learning::StructuralConstraintSetStatic<
          gum::learning::StructuralConstraintDAG,
          gum::learning::StructuralConstraintIndegree
          // gum::learning::StructuralConstraintSliceOrder
          > struct_constraint;

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
      static_cast<gum::learning::StructuralConstraintIndegree &>(struct_constraint) =
          constraint1;

      gum::learning::ParamEstimatorML<> estimator(filter, modalities, apriori,
                                                  score.internalApriori());

      gum::learning::GraphChangesGenerator4DiGraph<decltype(struct_constraint)>
          op_set(struct_constraint);

      gum::learning::GraphChangesSelector4DiGraph<
          decltype(score), decltype(struct_constraint), decltype(op_set)>
          selector(score, struct_constraint, op_set);

      gum::learning::GreedyHillClimbing search;
      // simpleListenerForGHC agsl ( search );
      search.approximationScheme().setEpsilon(1000);

      gum::DAG dag = search.learnStructure(selector, modalities);
      TS_ASSERT(dag.arcs().size() == 11);

      gum::BayesNet<double> bn =
          search.learnBN<double>(selector, estimator, database.variableNames(),
                                 modalities, filter.translatorSet());

      const std::string s0 = "0";
      const std::string s1 = "1";
      for (unsigned int i = 0; i < database.nbVariables(); ++i) {
        const gum::DiscreteVariable &var = bn.variable(i);
        TS_ASSERT(var.label(0) == s0);
        TS_ASSERT(var.label(1) == s1);
      }

      /*
        gum::BayesNet<float> bn2 =
        search.learnBN ( selector, estimator,
        database.variableNames (),
        modalities );
      */
    }

    void test_alarm_with_ordered_values() {
      gum::learning::DatabaseFromCSV database(MY_ALARM);

      gum::learning::DBRowTranslatorSetDynamic<
          gum::learning::CellTranslatorUniversal> translators;
      gum::learning::CellTranslatorUniversal translator_tf(
          gum::Sequence<float>{0, 1, 2, 3}, true);
      translators.insertTranslator(translator_tf, gum::learning::Col<0>(),
                                   database.nbVariables());

      // auto translators = gum::learning::make_translators
      //  ( gum::learning::Create<gum::learning::CellTranslatorCompactIntId,
      //                          gum::learning::Col<0>, 8 > () );

      auto generators =
          gum::learning::make_generators(gum::learning::RowGeneratorIdentity());

      auto filter =
          gum::learning::make_DB_row_filter(database, translators, generators);

      std::vector<unsigned int> modalities = filter.modalities();

      gum::learning::AprioriSmoothing<> apriori;
      gum::learning::ScoreK2<> score(filter, modalities, apriori);

      gum::learning::StructuralConstraintSetStatic<
          gum::learning::StructuralConstraintDAG,
          gum::learning::StructuralConstraintIndegree
          // gum::learning::StructuralConstraintSliceOrder
          > struct_constraint;

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
      static_cast<gum::learning::StructuralConstraintIndegree &>(struct_constraint) =
          constraint1;

      gum::learning::ParamEstimatorML<> estimator(filter, modalities, apriori,
                                                  score.internalApriori());

      gum::learning::GraphChangesGenerator4DiGraph<decltype(struct_constraint)>
          op_set(struct_constraint);

      gum::learning::GraphChangesSelector4DiGraph<
          decltype(score), decltype(struct_constraint), decltype(op_set)>
          selector(score, struct_constraint, op_set);

      gum::learning::GreedyHillClimbing search;
      // simpleListenerForGHC agsl ( search );
      search.approximationScheme().setEpsilon(1000);

      gum::BayesNet<double> bn =
          search.learnBN<double>(selector, estimator, database.variableNames(),
                                 modalities, filter.translatorSet());

      const std::string s0 = "0";
      const std::string s1 = "1";
      const std::string s2 = "2";
      gum::Set<unsigned int> seq{1, 10, 11, 14};
      for (unsigned int i = 0; i < database.nbVariables(); ++i) {
        const gum::DiscreteVariable &var = bn.variable(i);
        TS_ASSERT(var.label(0) == s0);
        TS_ASSERT(var.label(1) == s1);
        if (seq.exists(i)) {
          TS_ASSERT(var.label(2) == s2);
        }
      }

      /*
        gum::BayesNet<float> bn2 =
        search.learnBN ( selector, estimator,
        database.variableNames (),
        modalities );
      */
    }

    void test_alarm_with_ordered_values2() {
      gum::learning::DatabaseFromCSV database(MY_ALARM);

      gum::learning::DBRowTranslatorSetDynamic<
          gum::learning::CellTranslatorUniversal> translators;
      translators.insertTranslator(gum::learning::Col<0>(), database.nbVariables());
      translators[1].setUserValues(gum::Sequence<float>{0, 1, 2}, false);
      translators[10].setUserValues(gum::Sequence<float>{0, 1, 2}, false);
      translators[11].setUserValues(gum::Sequence<float>{0, 1, 2}, false);
      translators[14].setUserValues(gum::Sequence<float>{0, 1, 2}, false);

      // auto translators = gum::learning::make_translators
      //  ( gum::learning::Create<gum::learning::CellTranslatorCompactIntId,
      //                          gum::learning::Col<0>, 8 > () );

      auto generators =
          gum::learning::make_generators(gum::learning::RowGeneratorIdentity());

      auto filter =
          gum::learning::make_DB_row_filter(database, translators, generators);

      std::vector<unsigned int> modalities = filter.modalities();

      gum::learning::AprioriSmoothing<> apriori;
      gum::learning::ScoreK2<> score(filter, modalities, apriori);

      gum::learning::StructuralConstraintSetStatic<
          gum::learning::StructuralConstraintDAG,
          gum::learning::StructuralConstraintIndegree
          // gum::learning::StructuralConstraintSliceOrder
          > struct_constraint;

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
      static_cast<gum::learning::StructuralConstraintIndegree &>(struct_constraint) =
          constraint1;

      gum::learning::ParamEstimatorML<> estimator(filter, modalities, apriori,
                                                  score.internalApriori());

      gum::learning::GraphChangesGenerator4DiGraph<decltype(struct_constraint)>
          op_set(struct_constraint);

      gum::learning::GraphChangesSelector4DiGraph<
          decltype(score), decltype(struct_constraint), decltype(op_set)>
          selector(score, struct_constraint, op_set);

      gum::learning::GreedyHillClimbing search;
      // simpleListenerForGHC agsl ( search );
      search.approximationScheme().setEpsilon(1000);

      gum::BayesNet<double> bn =
          search.learnBN<double>(selector, estimator, database.variableNames(),
                                 modalities, filter.translatorSet());

      const std::string s0 = "0";
      const std::string s1 = "1";
      const std::string s2 = "2";
      gum::Set<unsigned int> seq{1, 10, 11, 14};
      for (auto i : seq) {
        const gum::DiscreteVariable &var = bn.variable(i);
        TS_ASSERT(var.label(0) == s0);
        TS_ASSERT(var.label(1) == s1);
        TS_ASSERT(var.label(2) == s2);
      }

      /*
        gum::BayesNet<float> bn2 =
        search.learnBN ( selector, estimator,
        database.variableNames (),
        modalities );
      */
    }

    void xtest_alarm1() {
      gum::learning::DatabaseFromCSV database(MY_ALARM);

      auto translators = gum::learning::make_translators(gum::learning::Create<
          gum::learning::CellTranslatorCompactIntId, gum::learning::Col<0>, 37>());

      auto generators =
          gum::learning::make_generators(gum::learning::RowGeneratorIdentity());
      auto filter =
          gum::learning::make_DB_row_filter(database, translators, generators);

      std::vector<unsigned int> modalities = filter.modalities();

      gum::learning::AprioriSmoothing<> apriori;
      gum::learning::ScoreBDeu<> score(filter, modalities, apriori);

      gum::learning::StructuralConstraintSetStatic<
          gum::learning::StructuralConstraintDAG> struct_constraint;

      gum::learning::ParamEstimatorML<> estimator(filter, modalities, apriori,
                                                  score.internalApriori());

      gum::learning::GraphChangesGenerator4DiGraph<decltype(struct_constraint)>
          op_set(struct_constraint);

      gum::learning::GraphChangesSelector4DiGraph<
          decltype(score), decltype(struct_constraint), decltype(op_set)>
          selector(score, struct_constraint, op_set);

      gum::learning::GreedyHillClimbing search;

      try {
        gum::Timer timer;
        gum::DAG bn = search.learnStructure(selector, modalities);
        std::cout << timer.step() << " : " << std::endl;
        std::cout << bn << std::endl;
      } catch (gum::Exception &e) {
        GUM_SHOWERROR(e);
      }
    }

    void xtest_alarm1bis() {
      gum::learning::DatabaseFromCSV database(MY_ALARM);

      auto translators = gum::learning::make_translators(gum::learning::Create<
          gum::learning::CellTranslatorNumber, gum::learning::Col<0>, 37>());

      auto generators =
          gum::learning::make_generators(gum::learning::RowGeneratorIdentity());
      auto filter =
          gum::learning::make_DB_row_filter(database, translators, generators);

      std::vector<unsigned int> modalities = filter.modalities();

      gum::learning::AprioriSmoothing<> apriori;
      gum::learning::ScoreBDeu<> score(filter, modalities, apriori);

      gum::learning::StructuralConstraintSetStatic<
          gum::learning::StructuralConstraintDAG> struct_constraint;

      gum::learning::ParamEstimatorML<> estimator(filter, modalities, apriori,
                                                  score.internalApriori());

      gum::learning::GraphChangesGenerator4DiGraph<decltype(struct_constraint)>
          op_set(struct_constraint);

      gum::learning::GraphChangesSelector4DiGraph<
          decltype(score), decltype(struct_constraint), decltype(op_set)>
          selector(score, struct_constraint, op_set);

      gum::learning::GreedyHillClimbing search;

      try {
        gum::Timer timer;
        gum::DAG bn = search.learnStructure(selector, modalities);
        std::cout << timer.step() << " : " << std::endl;
        std::cout << bn << std::endl;
      } catch (gum::Exception &e) {
        GUM_SHOWERROR(e);
      }
    }

    void xtest_alarm1ter() {
      gum::learning::DatabaseFromCSV database(MY_ALARM);

      auto translators = gum::learning::make_translators(gum::learning::Create<
          gum::learning::CellTranslatorUniversal, gum::learning::Col<0>, 37>());

      auto generators =
          gum::learning::make_generators(gum::learning::RowGeneratorIdentity());
      auto filter =
          gum::learning::make_DB_row_filter(database, translators, generators);

      std::vector<unsigned int> modalities = filter.modalities();

      gum::learning::AprioriSmoothing<> apriori;
      gum::learning::ScoreBDeu<> score(filter, modalities, apriori);

      gum::learning::StructuralConstraintSetStatic<
          gum::learning::StructuralConstraintDAG> struct_constraint;

      gum::learning::ParamEstimatorML<> estimator(filter, modalities, apriori,
                                                  score.internalApriori());

      gum::learning::GraphChangesGenerator4DiGraph<decltype(struct_constraint)>
          op_set(struct_constraint);

      gum::learning::GraphChangesSelector4DiGraph<
          decltype(score), decltype(struct_constraint), decltype(op_set)>
          selector(score, struct_constraint, op_set);

      gum::learning::GreedyHillClimbing search;

      try {
        gum::Timer timer;
        gum::DAG bn = search.learnStructure(selector, modalities);
        std::cout << timer.step() << " : " << std::endl;
        std::cout << bn << std::endl;
      } catch (gum::Exception &e) {
        GUM_SHOWERROR(e);
      }
    }

    void xtest_alarm2() {
      gum::learning::DatabaseFromCSV database(MY_ALARM);

      gum::learning::DBRowTranslatorSetDynamic<
          gum::learning::CellTranslatorCompactIntId> translators;
      translators.insertTranslator(gum::learning::CellTranslatorCompactIntId(),
                                   gum::learning::Col<0>(), 37);

      auto generators =
          gum::learning::make_generators(gum::learning::RowGeneratorIdentity());
      auto filter =
          gum::learning::make_DB_row_filter(database, translators, generators);

      std::vector<unsigned int> modalities = filter.modalities();

      gum::learning::AprioriSmoothing<> apriori;
      gum::learning::ScoreBDeu<> score(filter, modalities, apriori);

      gum::learning::StructuralConstraintSetStatic<
          gum::learning::StructuralConstraintDAG> struct_constraint;

      gum::learning::ParamEstimatorML<> estimator(filter, modalities, apriori,
                                                  score.internalApriori());

      gum::learning::GraphChangesGenerator4DiGraph<decltype(struct_constraint)>
          op_set(struct_constraint);

      gum::learning::GraphChangesSelector4DiGraph<
          decltype(score), decltype(struct_constraint), decltype(op_set)>
          selector(score, struct_constraint, op_set);

      gum::learning::GreedyHillClimbing search;

      try {
        gum::Timer timer;
        gum::DAG bn = search.learnStructure(selector, modalities);
        std::cout << timer.step() << " : " << std::endl;
        std::cout << bn << std::endl;
      } catch (gum::Exception &e) {
        GUM_SHOWERROR(e);
      }
    }

    void xtest_alarm3() {
      gum::learning::DatabaseFromCSV database(MY_ALARM);

      gum::learning::DBRowTranslatorSetDynamic<gum::learning::DBCellTranslator<1, 1>>
          translators;
      translators.insertTranslator(gum::learning::CellTranslatorCompactIntId(),
                                   gum::learning::Col<0>(),
                                   database.content()[0].size());

      auto generators =
          gum::learning::make_generators(gum::learning::RowGeneratorIdentity());
      auto filter =
          gum::learning::make_DB_row_filter(database, translators, generators);

      std::vector<unsigned int> modalities = filter.modalities();

      gum::learning::AprioriSmoothing<> apriori;
      gum::learning::ScoreBDeu<> score(filter, modalities, apriori);

      gum::learning::StructuralConstraintSetStatic<
          gum::learning::StructuralConstraintDAG> struct_constraint;

      gum::learning::ParamEstimatorML<> estimator(filter, modalities, apriori,
                                                  score.internalApriori());

      gum::learning::GraphChangesGenerator4DiGraph<decltype(struct_constraint)>
          op_set(struct_constraint);

      gum::learning::GraphChangesSelector4DiGraph<
          decltype(score), decltype(struct_constraint), decltype(op_set)>
          selector(score, struct_constraint, op_set);

      gum::learning::GreedyHillClimbing search;

      try {
        gum::Timer timer;
        gum::DAG bn = search.learnStructure(selector, modalities);
        std::cout << timer.step() << " : " << std::endl;
        std::cout << bn << std::endl;
      } catch (gum::Exception &e) {
        GUM_SHOWERROR(e);
      }
    }

    void xtest_alarm4() {
      gum::learning::DatabaseFromCSV database(MY_ALARM);

      gum::learning::DBRowTranslatorSetDynamic<gum::learning::DBCellTranslator<1, 1>>
          translators;
      translators.insertTranslator(gum::learning::CellTranslatorCompactIntId(),
                                   gum::learning::Col<0>(),
                                   database.content()[0].size());

      auto generators =
          gum::learning::make_generators(gum::learning::RowGeneratorIdentity());
      auto filter =
          gum::learning::make_DB_row_filter(database, translators, generators);

      std::vector<unsigned int> modalities = filter.modalities();

      gum::learning::AprioriSmoothing<> apriori;
      gum::learning::ScoreBDeu<> score(filter, modalities, apriori);

      gum::learning::StructuralConstraintSetStatic<
          gum::learning::StructuralConstraintDAG> struct_constraint;

      gum::learning::ParamEstimatorML<> estimator(filter, modalities, apriori);

      gum::learning::GraphChangesGeneratorOnSubDiGraph<decltype(struct_constraint)>
          op_set(struct_constraint);

      gum::NodeSet targets{0, 1, 2};
      op_set.setTargets(targets);
      op_set.setTails(modalities.size());

      gum::learning::GraphChangesSelector4DiGraph<
          decltype(score), decltype(struct_constraint), decltype(op_set)>
          selector(score, struct_constraint, op_set);

      gum::learning::GreedyHillClimbing search;

      try {
        gum::Timer timer;
        gum::DAG bn_dag = search.learnStructure(selector, modalities);
        std::cout << timer.step() << " : " << std::endl;
        std::cout << bn_dag << std::endl;

        gum::BayesNet<float> bn =
            search.learnBN(selector, estimator, database.variableNames(), modalities,
                           filter.translatorSet());

      } catch (gum::Exception &e) {
        GUM_SHOWERROR(e);
      }
    }
  };

} /* namespace gum_tests */
