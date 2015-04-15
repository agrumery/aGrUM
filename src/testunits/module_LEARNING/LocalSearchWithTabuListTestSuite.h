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
#include <agrum/learning/database/filteredRowGenerators/rowGeneratorIdentity.h>

#include <agrum/learning/scores_and_tests/scoreK2.h>
#include <agrum/learning/scores_and_tests/scoreBDeu.h>

#include <agrum/learning/aprioris/aprioriSmoothing.h>

#include <agrum/learning/constraints/structuralConstraintDiGraph.h>
#include <agrum/learning/constraints/structuralConstraintDAG.h>
#include <agrum/learning/constraints/structuralConstraintIndegree.h>
#include <agrum/learning/constraints/structuralConstraintSliceOrder.h>
#include <agrum/learning/constraints/structuralConstraintTabuList.h>
#include <agrum/learning/constraints/structuralConstraintSetStatic.h>

#include <agrum/learning/structureUtils/graphChangesGenerator4DiGraph.h>
#include <agrum/learning/structureUtils/graphChangesSelector4DiGraph.h>

#include <agrum/learning/paramUtils/paramEstimatorML.h>
#include <agrum/learning/localSearchWithTabuList.h>

#define MY_ALARM GET_PATH_STR("alarm.csv")

namespace gum_tests {

  class LocalSearchWithTabuListTestSuite : public CxxTest::TestSuite {
    public:
    void test_asia() {
      gum::learning::DatabaseFromCSV database(GET_PATH_STR("asia.csv"));

      auto translators1 = gum::learning::make_translators(gum::learning::Create<
          gum::learning::CellTranslatorCompactIntId, gum::learning::Col<0>, 8>());

      // auto translators = translators1;

      gum::learning::DBRowTranslatorSetDynamic<
          gum::learning::CellTranslatorCompactIntId> translators;
      translators.insertTranslator(gum::learning::Col<0>(), 8);

      auto generators1 =
          gum::learning::make_generators(gum::learning::RowGeneratorIdentity());

      auto generators = generators1;

      auto filter1 =
          gum::learning::make_DB_row_filter(database, translators, generators);

      auto filter = filter1;

      auto filter2 =
          gum::learning::make_DB_row_filter(database, translators, generators1);

      filter = std::move(filter2);

      std::vector<unsigned int> modalities = filter.modalities();

      gum::learning::AprioriSmoothing<> apriori;
      gum::learning::ScoreK2<> score(filter, modalities, apriori);

      gum::learning::StructuralConstraintSetStatic<
          gum::learning::StructuralConstraintDAG,
          gum::learning::StructuralConstraintIndegree,
          gum::learning::StructuralConstraintSliceOrder,
          gum::learning::StructuralConstraintTabuList> struct_constraint;

      struct_constraint.setMaxIndegree(2);
      struct_constraint.setTabuListSize(100);

      gum::NodeProperty<unsigned int> slices{std::make_pair(gum::NodeId(0), 0),
                                             std::make_pair(gum::NodeId(1), 0)};
      struct_constraint.setSliceOrder(slices);
      struct_constraint.setDefaultSlice(1);

      gum::learning::ParamEstimatorML<> estimator(filter, modalities, apriori);

      gum::learning::GraphChangesGenerator4DiGraph<decltype(struct_constraint)>
          op_set(struct_constraint);

      gum::learning::GraphChangesSelector4DiGraph<
          decltype(score), decltype(struct_constraint), decltype(op_set)>
          selector(score, struct_constraint, op_set);

      gum::learning::LocalSearchWithTabuList search;
      search.setMaxNbDecreasingChanges(2);

      try {
        gum::BayesNet<float> bn =
            search.learnBN(selector, estimator, database.variableNames(), modalities,
                           filter.translatorSet());
        gum::BayesNet<double> bn2 =
            search.learnBN<double>(selector, estimator, database.variableNames(),
                                   modalities, filter.translatorSet());
        TS_ASSERT(bn.dag().arcs().size() == 11);
      } catch (gum::Exception &e) {
        GUM_SHOWERROR(e);
      }
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
      apriori.setWeight(0);
      gum::learning::ScoreK2<> score(filter, modalities, apriori);

      gum::learning::StructuralConstraintSetStatic<
          gum::learning::StructuralConstraintDAG,
          gum::learning::StructuralConstraintIndegree,
          gum::learning::StructuralConstraintSliceOrder,
          gum::learning::StructuralConstraintTabuList> struct_constraint;

      struct_constraint.setMaxIndegree(4);
      struct_constraint.setTabuListSize(100);

      gum::NodeProperty<unsigned int> slices{std::make_pair(gum::NodeId(0), 0),
                                             std::make_pair(gum::NodeId(1), 0)};
      struct_constraint.setSliceOrder(slices);
      struct_constraint.setDefaultSlice(1);

      gum::learning::ParamEstimatorML<> estimator(filter, modalities, apriori);

      gum::learning::GraphChangesGenerator4DiGraph<decltype(struct_constraint)>
          op_set(struct_constraint);

      gum::learning::GraphChangesSelector4DiGraph<
          decltype(score), decltype(struct_constraint), decltype(op_set)>
          selector(score, struct_constraint, op_set);

      gum::learning::LocalSearchWithTabuList search;

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
      apriori.setWeight(0);
      gum::learning::ScoreK2<> score(filter, modalities, apriori);

      gum::learning::StructuralConstraintSetStatic<
          gum::learning::StructuralConstraintDAG,
          gum::learning::StructuralConstraintIndegree,
          gum::learning::StructuralConstraintSliceOrder,
          gum::learning::StructuralConstraintTabuList> struct_constraint;

      struct_constraint.setMaxIndegree(4);
      struct_constraint.setTabuListSize(100);

      gum::NodeProperty<unsigned int> slices{std::make_pair(gum::NodeId(0), 0),
                                             std::make_pair(gum::NodeId(1), 0)};
      struct_constraint.setSliceOrder(slices);
      struct_constraint.setDefaultSlice(1);

      gum::learning::ParamEstimatorML<> estimator(filter, modalities, apriori);

      gum::learning::GraphChangesGenerator4DiGraph<decltype(struct_constraint)>
          op_set(struct_constraint);

      gum::learning::GraphChangesSelector4DiGraph<
          decltype(score), decltype(struct_constraint), decltype(op_set)>
          selector(score, struct_constraint, op_set);

      gum::learning::LocalSearchWithTabuList search;

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
                                   gum::learning::Col<0>(), 37);

      auto generators =
          gum::learning::make_generators(gum::learning::RowGeneratorIdentity());
      auto filter =
          gum::learning::make_DB_row_filter(database, translators, generators);

      std::vector<unsigned int> modalities = filter.modalities();

      gum::learning::AprioriSmoothing<> apriori;
      apriori.setWeight(0);
      gum::learning::ScoreK2<> score(filter, modalities, apriori);

      gum::learning::StructuralConstraintSetStatic<
          gum::learning::StructuralConstraintDAG,
          gum::learning::StructuralConstraintIndegree,
          gum::learning::StructuralConstraintSliceOrder,
          gum::learning::StructuralConstraintTabuList> struct_constraint;

      struct_constraint.setMaxIndegree(4);
      struct_constraint.setTabuListSize(100);

      gum::NodeProperty<unsigned int> slices{std::make_pair(gum::NodeId(0), 0),
                                             std::make_pair(gum::NodeId(1), 0)};
      struct_constraint.setSliceOrder(slices);
      struct_constraint.setDefaultSlice(1);

      apriori.setWeight(0);
      gum::learning::ParamEstimatorML<> estimator(filter, modalities, apriori);

      gum::learning::GraphChangesGenerator4DiGraph<decltype(struct_constraint)>
          op_set(struct_constraint);

      gum::learning::GraphChangesSelector4DiGraph<
          decltype(score), decltype(struct_constraint), decltype(op_set)>
          selector(score, struct_constraint, op_set);

      gum::learning::LocalSearchWithTabuList search;

      try {
        gum::Timer timer;
        gum::DAG bn = search.learnStructure(selector, modalities);
        std::cout << timer.step() << " : " << std::endl;
        std::cout << bn << std::endl;
      } catch (gum::Exception &e) {
        GUM_SHOWERROR(e);
      }
    }
  };

} /* namespace gum_tests */
