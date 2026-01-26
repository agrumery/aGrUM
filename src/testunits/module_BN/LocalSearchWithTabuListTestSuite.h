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

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

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
#include <agrum/BN/learning/constraints/structuralConstraintIndegree.h>
#include <agrum/BN/learning/constraints/structuralConstraintSetStatic.h>
#include <agrum/BN/learning/constraints/structuralConstraintSliceOrder.h>
#include <agrum/BN/learning/constraints/structuralConstraintTabuList.h>
#include <agrum/BN/learning/localSearchWithTabuList.h>
#include <agrum/BN/learning/paramUtils/paramEstimatorML.h>
#include <agrum/BN/learning/priors/smoothingPrior.h>
#include <agrum/BN/learning/structureUtils/graphChangesGenerator4DiGraph.h>
#include <agrum/BN/learning/structureUtils/graphChangesSelector4DiGraph.h>

#include <agrum/BN/learning/scores_and_tests/scoreBDeu.h>
#include <agrum/BN/learning/scores_and_tests/scoreK2.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  LocalSearchWithTabuList
#define GUM_CURRENT_MODULE BN

namespace gum_tests {

  struct LocalSearchWithTabuListTestSuite {
    public:
    static void test_asia() {
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

      gum::learning::DBRowGeneratorSet    genset;
      gum::learning::DBRowGeneratorParser parser(database.handler(), genset);
      gum::learning::SmoothingPrior       prior(database);
      gum::learning::ScoreK2              score(parser, prior);

      gum::learning::StructuralConstraintSetStatic< gum::learning::StructuralConstraintDAG,
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

      gum::learning::ParamEstimatorML estimator(parser, prior, score.internalPrior());

      gum::learning::GraphChangesGenerator4DiGraph< decltype(struct_constraint) > op_set(
          struct_constraint);

      gum::learning::GraphChangesSelector4DiGraph< decltype(struct_constraint), decltype(op_set) >
          selector(score, struct_constraint, op_set);

      gum::learning::LocalSearchWithTabuList search;
      search.setMaxNbDecreasingChanges(2);

      try {
        gum::BayesNet< double > bn  = search.learnBN< double >(selector, estimator);
        gum::BayesNet< double > bn2 = search.learnBN< double >(selector, estimator);
        CHECK((bn.dag().arcs().size()) == (static_cast< gum::Size >(10)));
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }   // namespace gum_tests

    /*
    void xtest_alarm1() {
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

      std::vector< gum::Size > modalities = filter.modalities();

      gum::learning::SmoothingPrior prior;
      prior.setWeight(0);
      gum::learning::ScoreK2 score(filter, modalities, prior);

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

      gum::learning::ParamEstimatorML estimator(filter, modalities, prior);

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

      std::vector< gum::Size > modalities = filter.modalities();

      gum::learning::SmoothingPrior prior;
      prior.setWeight(0);
      gum::learning::ScoreK2 score(filter, modalities, prior);

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

      gum::learning::ParamEstimatorML estimator(filter, modalities, prior);

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
      gum::learning::DatabaseFromCSV
    database(GET_RESSOURCES_PATH("csv/alarm.csv"));

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

      gum::learning::SmoothingPrior prior;
      prior.setWeight(0);
      gum::learning::ScoreK2 score(filter, modalities, prior);

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

      prior.setWeight(0);
      gum::learning::ParamEstimatorML estimator(filter, modalities, prior);

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

  GUM_TEST_ACTIF(_asia)

} /* namespace gum_tests */
