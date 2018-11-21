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

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>
#include <iostream>

#include <agrum/BN/BayesNet.h>
#include <agrum/graphs/DAG.h>
#include <agrum/variables/labelizedVariable.h>

#include <agrum/learning/database/DBTranslator4LabelizedVariable.h>
#include <agrum/learning/database/DBRowGeneratorParser.h>
#include <agrum/learning/database/DBInitializerFromCSV.h>
#include <agrum/learning/database/databaseTable.h>
#include <agrum/learning/database/DBTranslatorSet.h>

#include <agrum/learning/scores_and_tests/scoreBDeu.h>
#include <agrum/learning/scores_and_tests/scoreK2.h>

#include <agrum/learning/aprioris/aprioriSmoothing.h>

#include <agrum/learning/constraints/structuralConstraintDAG.h>
#include <agrum/learning/constraints/structuralConstraintDiGraph.h>
#include <agrum/learning/constraints/structuralConstraintIndegree.h>
#include <agrum/learning/constraints/structuralConstraintSetStatic.h>
#include <agrum/learning/constraints/structuralConstraintSliceOrder.h>
#include <agrum/learning/constraints/structuralConstraintTabuList.h>

#include <agrum/learning/structureUtils/graphChangesGenerator4DiGraph.h>
#include <agrum/learning/structureUtils/graphChangesSelector4DiGraph.h>

#include <agrum/learning/localSearchWithTabuList.h>
#include <agrum/learning/paramUtils/paramEstimatorML.h>

namespace gum_tests {

  class LocalSearchWithTabuListTestSuite : public CxxTest::TestSuite {
    public:
    void test_asia() {
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

      gum::learning::DBRowGeneratorSet<>    genset;
      gum::learning::DBRowGeneratorParser<> parser(database.handler(), genset);
      gum::learning::AprioriSmoothing2<>    apriori(database);
      gum::learning::ScoreK22<>             score(parser, apriori);

      gum::learning::StructuralConstraintSetStatic<
        gum::learning::StructuralConstraintDAG,
        gum::learning::StructuralConstraintIndegree,
        gum::learning::StructuralConstraintSliceOrder,
        gum::learning::StructuralConstraintTabuList >
        struct_constraint;

      struct_constraint.setMaxIndegree(2);
      struct_constraint.setTabuListSize(100);

      gum::NodeProperty< gum::NodeId > slices{std::make_pair(gum::NodeId(0), 0),
                                              std::make_pair(gum::NodeId(1), 0)};
      struct_constraint.setSliceOrder(slices);
      struct_constraint.setDefaultSlice(1);

      gum::learning::ParamEstimatorML2<> estimator(parser, apriori,
                                                   score.internalApriori());

      gum::learning::GraphChangesGenerator4DiGraph< decltype(struct_constraint) >
        op_set(struct_constraint);

      gum::learning::GraphChangesSelector4DiGraph2< decltype(struct_constraint),
                                                    decltype(op_set) >
        selector(score, struct_constraint, op_set);

      gum::learning::LocalSearchWithTabuList search;
      search.setMaxNbDecreasingChanges(2);

      try {
        gum::BayesNet< float > bn = search.learnBN< float >(selector, estimator);
        gum::BayesNet< double > bn2 =
          search.learnBN< double >(selector, estimator);
        TS_ASSERT(bn.dag().arcs().size() == 10);
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }


    /*
    void xtest_alarm1() {
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

      std::vector< gum::Size > modalities = filter.modalities();

      gum::learning::AprioriSmoothing<> apriori;
      apriori.setWeight(0);
      gum::learning::ScoreK2<> score(filter, modalities, apriori);

      gum::learning::StructuralConstraintSetStatic<
        gum::learning::StructuralConstraintDAG,
        gum::learning::StructuralConstraintIndegree,
        gum::learning::StructuralConstraintSliceOrder,
        gum::learning::StructuralConstraintTabuList >
        struct_constraint;

      struct_constraint.setMaxIndegree(4);
      struct_constraint.setTabuListSize(100);

      gum::NodeProperty< gum::NodeId > slices{std::make_pair(gum::NodeId(0), 0),
                                              std::make_pair(gum::NodeId(1), 0)};
      struct_constraint.setSliceOrder(slices);
      struct_constraint.setDefaultSlice(1);

      gum::learning::ParamEstimatorML<> estimator(filter, modalities, apriori);

      gum::learning::GraphChangesGenerator4DiGraph< decltype(struct_constraint) >
        op_set(struct_constraint);

      gum::learning::GraphChangesSelector4DiGraph< decltype(score),
                                                   decltype(struct_constraint),
                                                   decltype(op_set) >
        selector(score, struct_constraint, op_set);

      gum::learning::LocalSearchWithTabuList search;

      try {
        gum::Timer timer;
        gum::DAG   bn = search.learnStructure(selector, modalities);
        std::cout << timer.step() << " : " << std::endl;
        std::cout << bn << std::endl;
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
      }
    }

    void xtest_alarm2() {
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

      std::vector< gum::Size > modalities = filter.modalities();

      gum::learning::AprioriSmoothing<> apriori;
      apriori.setWeight(0);
      gum::learning::ScoreK2<> score(filter, modalities, apriori);

      gum::learning::StructuralConstraintSetStatic<
        gum::learning::StructuralConstraintDAG,
        gum::learning::StructuralConstraintIndegree,
        gum::learning::StructuralConstraintSliceOrder,
        gum::learning::StructuralConstraintTabuList >
        struct_constraint;

      struct_constraint.setMaxIndegree(4);
      struct_constraint.setTabuListSize(100);

      gum::NodeProperty< gum::NodeId > slices{std::make_pair(gum::NodeId(0), 0),
                                              std::make_pair(gum::NodeId(1), 0)};
      struct_constraint.setSliceOrder(slices);
      struct_constraint.setDefaultSlice(1);

      gum::learning::ParamEstimatorML<> estimator(filter, modalities, apriori);

      gum::learning::GraphChangesGenerator4DiGraph< decltype(struct_constraint) >
        op_set(struct_constraint);

      gum::learning::GraphChangesSelector4DiGraph< decltype(score),
                                                   decltype(struct_constraint),
                                                   decltype(op_set) >
        selector(score, struct_constraint, op_set);

      gum::learning::LocalSearchWithTabuList search;

      try {
        gum::Timer timer;
        gum::DAG   bn = search.learnStructure(selector, modalities);
        std::cout << timer.step() << " : " << std::endl;
        std::cout << bn << std::endl;
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
      }
    }

    void xtest_alarm3() {
      gum::learning::DatabaseFromCSV database(GET_RESSOURCES_PATH("alarm.csv"));

      gum::learning::DBRowTranslatorSet< gum::learning::DBCellTranslator< 1, 1 > >
        translators;
      translators.insertTranslator(
        gum::learning::CellTranslatorCompactIntId(), 0, 37);

      gum::learning::FilteredRowGeneratorSet< gum::learning::RowGeneratorIdentity >
        generators;
      generators.insertGenerator();
      auto filter =
        gum::learning::make_DB_row_filter(database, translators, generators);

      std::vector< gum::Size > modalities = filter.modalities();

      gum::learning::AprioriSmoothing<> apriori;
      apriori.setWeight(0);
      gum::learning::ScoreK2<> score(filter, modalities, apriori);

      gum::learning::StructuralConstraintSetStatic<
        gum::learning::StructuralConstraintDAG,
        gum::learning::StructuralConstraintIndegree,
        gum::learning::StructuralConstraintSliceOrder,
        gum::learning::StructuralConstraintTabuList >
        struct_constraint;

      struct_constraint.setMaxIndegree(4);
      struct_constraint.setTabuListSize(100);

      gum::NodeProperty< gum::NodeId > slices{std::make_pair(gum::NodeId(0), 0),
                                              std::make_pair(gum::NodeId(1), 0)};
      struct_constraint.setSliceOrder(slices);
      struct_constraint.setDefaultSlice(1);

      apriori.setWeight(0);
      gum::learning::ParamEstimatorML<> estimator(filter, modalities, apriori);

      gum::learning::GraphChangesGenerator4DiGraph< decltype(struct_constraint) >
        op_set(struct_constraint);

      gum::learning::GraphChangesSelector4DiGraph< decltype(score),
                                                   decltype(struct_constraint),
                                                   decltype(op_set) >
        selector(score, struct_constraint, op_set);

      gum::learning::LocalSearchWithTabuList search;

      try {
        gum::Timer timer;
        gum::DAG   bn = search.learnStructure(selector, modalities);
        std::cout << timer.step() << " : " << std::endl;
        std::cout << bn << std::endl;
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
      }
    }
    */
  };

} /* namespace gum_tests */
