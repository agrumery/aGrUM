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

#include <agrum/base/core/approximations/approximationScheme.h>
#include <agrum/base/core/approximations/approximationSchemeListener.h>
#include <agrum/base/database/databaseTable.h>
#include <agrum/base/database/DBInitializerFromCSV.h>
#include <agrum/base/database/DBRowGeneratorIdentity.h>
#include <agrum/base/database/DBRowGeneratorParser.h>
#include <agrum/base/database/DBTranslator4LabelizedVariable.h>
#include <agrum/base/database/DBTranslatorSet.h>
#include <agrum/base/graphs/DAG.h>
#include <agrum/base/graphs/mixedGraph.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/generator/simpleBayesNetGenerator.h>
#include <agrum/BN/generator/simpleCPTGenerator.h>
#include <agrum/BN/learning/Miic.h>
#include <agrum/BN/learning/paramUtils/paramEstimatorML.h>
#include <agrum/BN/learning/priors/noPrior.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  Miic
#define GUM_CURRENT_MODULE BN

namespace gum_tests {

  class SimpleListenerForMiic: public gum::Listener {
    public:
    SimpleListenerForMiic() {};

    void whenStructuralModification(const void* src,
                                    gum::NodeId x,
                                    gum::NodeId y,
                                    std::string action,
                                    std::string explain) {
      static int s = 0;
      std::cout << std::setfill('0') << std::setw(5) << ++s << " : " << action << " | " << explain
                << std::endl;
    };
  };

  class FilterListenerForMiic: public gum::Listener {
    public:
    gum::Size filter;
    FilterListenerForMiic() {};

    void whenStructuralModification(const void* src,
                                    gum::NodeId x,
                                    gum::NodeId y,
                                    std::string action,
                                    std::string explain) {
      static int s = 0;
      if (x == filter || y == filter) {
        std::cout << std::setfill('0') << std::setw(5) << ++s << " : " << action << " | " << explain
                  << std::endl;
      }
    };
  };

  struct MiicTestSuite {
    public:
    static void test_latent_var_() {
      gum::learning::DBInitializerFromCSV initializer(
          GET_RESSOURCES_PATH("csv/latent_variable.csv"));
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

      gum::learning::DBRowGeneratorSet    genset;
      gum::learning::DBRowGeneratorParser parser(database.handler(), genset);

      std::vector< gum::Size > modalities(nb_vars, 2);

      gum::learning::NoPrior                    prior(database);
      gum::learning::CorrectedMutualInformation cI(parser, prior);

      gum::learning::Miic search;

      // creating complete graph
      gum::MixedGraph graph;
      for (gum::Size i = 0; i < modalities.size(); ++i) {
        graph.addNodeWithId(i);
        for (gum::Size j = 0; j < i; ++j) {
          graph.addEdge(j, i);
        }
      }

      try {
        auto mg = search.learnMixedStructure(cI, graph);
        // GUM_TRACE_VAR(dag.toDot())
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }

      try {
        auto pdag = search.learnPDAG(cI, graph);
        // GUM_TRACE_VAR(dag.toDot())
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }

      try {
        auto dag = search.learnStructure(cI, graph);
        // GUM_TRACE_VAR(dag.toDot())
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }

      // CHECK(!search.latentVariables().empty());
    }   // namespace gum_tests

    static void test_titanic_learn() {
      gum::learning::DBInitializerFromCSV initializer(GET_RESSOURCES_PATH("csv/titanic.csv"));
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

      std::vector< gum::Size > modalities(nb_vars, 2);

      gum::learning::NoPrior                    prior(database);
      gum::learning::CorrectedMutualInformation cI(parser, prior);
      gum::learning::Miic                       search;

      SimpleListenerForMiic listener;
      // GUM_CONNECT(search,
      //             onStructuralModification,
      //             listener,
      //             SimpleListenerForMiic::whenStructuralModification);

      // creating complete graph
      gum::MixedGraph graph;
      for (gum::Size i = 0; i < modalities.size(); ++i) {
        graph.addNodeWithId(i);
        for (gum::Size j = 0; j < i; ++j) {
          graph.addEdge(j, i);
        }
      }
      try {
        auto mg = search.learnMixedStructure(cI, graph);
        // GUM_TRACE_VAR(dag.toDot())
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }

      try {
        auto pdag = search.learnPDAG(cI, graph);
        // GUM_TRACE_VAR(dag.toDot())
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }

      try {
        auto dag = search.learnStructure(cI, graph);
        // GUM_TRACE_VAR(dag.toDot())
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }
    }

    static void test_alarm_learn() {
      FilterListenerForMiic filterlistener;
      filterlistener.filter = 24;

      gum::learning::DBInitializerFromCSV initializer(GET_RESSOURCES_PATH("csv/alarm.csv"));
      const auto&                         var_names = initializer.variableNames();
      const std::size_t                   nb_vars   = var_names.size();

      gum::learning::DBTranslatorSet translator_set;
      gum::LabelizedVariable         xvar("var", "", 0);
      for (int i = 0; i < 10; ++i) {
        xvar.addLabel(std::to_string(i));
      }
      gum::learning::DBTranslator4LabelizedVariable translator(xvar, true);
      for (std::size_t i = 0; i < nb_vars; ++i) {
        translator_set.insertTranslator(translator, i);
      }

      gum::learning::DatabaseTable database(translator_set);
      database.setVariableNames(initializer.variableNames());
      initializer.fillDatabase(database);

      gum::learning::DBRowGeneratorSet    genset;
      gum::learning::DBRowGeneratorParser parser(database.handler(), genset);

      std::vector< gum::Size > modalities(nb_vars, 10);

      gum::learning::NoPrior                    prior(database);
      gum::learning::CorrectedMutualInformation cI(parser, prior);
      cI.useNML();

      gum::learning::Miic search;
      gum::MixedGraph     graph;

      for (gum::Size i = 0; i < modalities.size(); ++i) {
        graph.addNodeWithId(i);
        for (gum::Size j = 0; j < i; ++j) {
          graph.addEdge(j, i);
        }
      }

      try {
        auto mg = search.learnMixedStructure(cI, graph);
        // GUM_TRACE_VAR(dag.toDot())
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }

      try {
        auto pdag = search.learnPDAG(cI, graph);
        // GUM_TRACE_VAR(dag.toDot())
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }

      try {
        auto dag = search.learnStructure(cI, graph);
        // GUM_TRACE_VAR(dag.toDot())
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }
    }

    static void test_census_learn() {
      gum::learning::DBInitializerFromCSV initializer(GET_RESSOURCES_PATH("csv/Census01.csv"));
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

      std::vector< gum::Size > modalities(nb_vars, 2);

      gum::learning::NoPrior                    prior(database);
      gum::learning::CorrectedMutualInformation cI(parser, prior);
      gum::learning::Miic                       search;

      /*SimpleListenerForMiic listener;
      GUM_CONNECT(search,
                  onStructuralModification,
                  listener,
                  SimpleListenerForMiic::whenStructuralModification);*/

      // creating complete graph
      gum::MixedGraph graph;
      for (gum::Size i = 0; i < modalities.size(); ++i) {
        graph.addNodeWithId(i);
        for (gum::Size j = 0; j < i; ++j) {
          graph.addEdge(j, i);
        }
      }

      try {
        auto mg = search.learnMixedStructure(cI, graph);
        // GUM_TRACE_VAR(dag.toDot())
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }

      try {
        auto pdag = search.learnPDAG(cI, graph);
        // GUM_TRACE_VAR(dag.toDot())
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }

      try {
        auto dag = search.learnStructure(cI, graph);
        // GUM_TRACE_VAR(dag.toDot())
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }
    }

    static void test_24_learn() {
      gum::learning::DBInitializerFromCSV initializer(GET_RESSOURCES_PATH("csv/24.csv"));
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

      std::vector< gum::Size > modalities(nb_vars, 2);

      gum::learning::NoPrior                    prior(database);
      gum::learning::CorrectedMutualInformation cI(parser, prior);
      gum::learning::Miic                       search;

      // SimpleListenerForMiic listener;
      /*GUM_CONNECT(search,
                  onStructuralModification,
                  listener,
                  SimpleListenerForMiic::whenStructuralModification);*/

      // creating complete graph
      gum::MixedGraph graph;
      for (gum::Size i = 0; i < modalities.size(); ++i) {
        graph.addNodeWithId(i);
        for (gum::Size j = 0; j < i; ++j) {
          graph.addEdge(j, i);
        }
      }
      try {
        auto mg = search.learnMixedStructure(cI, graph);
        // GUM_TRACE_VAR(dag.toDot())
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }

      try {
        auto pdag = search.learnPDAG(cI, graph);
        // GUM_TRACE_VAR(dag.toDot())
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }

      try {
        auto dag = search.learnStructure(cI, graph);
        // GUM_TRACE_VAR(dag.toDot())
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }
    }

    static void test_asia_ForbiddenGraph() {
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

      std::vector< gum::Size > modalities(nb_vars, 2);

      gum::learning::NoPrior                    prior(database);
      gum::learning::CorrectedMutualInformation cI(parser, prior);
      cI.useMDL();

      gum::learning::Miic search;

      // creating complete graph
      gum::MixedGraph graph;
      gum::DiGraph    forbidGraph;
      for (gum::Size i = 0; i < modalities.size(); ++i) {
        forbidGraph.addNodeWithId(i);
        graph.addNodeWithId(i);
        for (gum::Size j = 0; j < i; ++j) {
          graph.addEdge(j, i);
        }
      }

      // GUM_TRACE_VAR((gum::Edge(0,1) == gum::Edge(1,0)))

      // adding constraints
      forbidGraph.addArc(2, 6);
      forbidGraph.addArc(6, 2);
      forbidGraph.addArc(1, 5);

      search.setForbiddenGraph(forbidGraph);

      auto mg = search.learnMixedStructure(cI, graph);
      CHECK(!mg.existsArc(2, 6));
      CHECK(!mg.existsEdge(2, 6));
      CHECK(!mg.existsArc(1, 5));

      auto dag = search.learnStructure(cI, graph);
      CHECK(!dag.existsArc(2, 6));
      CHECK(!dag.existsArc(2, 6));
      CHECK(!dag.existsArc(1, 5));
    }

    static void test_asia_MandatoryGraph() {
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

      std::vector< gum::Size > modalities(nb_vars, 2);

      gum::learning::NoPrior                    prior(database);
      gum::learning::CorrectedMutualInformation cI(parser, prior);
      cI.useMDL();

      gum::learning::Miic search;

      // creating complete graph
      gum::MixedGraph graph;
      gum::DAG        mandaGraph;
      for (gum::Size i = 0; i < modalities.size(); ++i) {
        mandaGraph.addNodeWithId(i);
        graph.addNodeWithId(i);
        for (gum::Size j = 0; j < i; ++j) {
          graph.addEdge(j, i);
        }
      }

      // adding constraints
      mandaGraph.addArc(3, 4);
      search.setMandatoryGraph(mandaGraph);

      auto mg = search.learnMixedStructure(cI, graph);
      CHECK(mg.existsArc(3, 4));

      auto dag = search.learnStructure(cI, graph);
      CHECK(dag.existsArc(3, 4));
    }

    static void test_alarm_MaxIndegree_() {
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

      std::vector< gum::Size > modalities(nb_vars, 2);

      gum::learning::NoPrior                    prior(database);
      gum::learning::CorrectedMutualInformation cI(parser, prior);
      cI.useMDL();
      // cI.useCache( false );

      gum::learning::Miic search;
      // creating complete graph
      gum::MixedGraph graph, g;
      for (gum::Size i = 0; i < modalities.size(); ++i) {
        graph.addNodeWithId(i);
        for (gum::Size j = 0; j < i; ++j) {
          graph.addEdge(j, i);
        }
      }

      // adding constraints
      gum::Size n = 2;
      search.setMaxIndegree(n);

      // Learn Structure
      graph = search.learnMixedStructure(cI, graph);

      gum::NodeSet nodesSet = graph.asNodeSet();
      for (auto& x: nodesSet) {
        CHECK(graph.parents(x).size() <= n);
      }
    }

    static void test_MIIC_ms_order1_() {
      // filterlistener.filter = 6;
      // GUM_CONNECT(search,
      //             onStructuralModification,
      //             filterlistener,
      //             FilterListenerForMiic::whenStructuralModification);

      gum::learning::DBInitializerFromCSV initializer(
          GET_RESSOURCES_PATH("csv/ordinal_ms_order_1.csv"));
      const auto&       var_names = initializer.variableNames();
      const std::size_t nb_vars   = var_names.size();

      gum::learning::DBTranslatorSet translator_set;
      gum::LabelizedVariable         xvar("var", "", 0);
      for (int i = 0; i < 10; ++i) {
        xvar.addLabel(std::to_string(i));
      }

      gum::learning::DBTranslator4LabelizedVariable translator(xvar, true);
      for (std::size_t i = 0; i < nb_vars; ++i) {
        translator_set.insertTranslator(translator, i);
      }

      gum::learning::DatabaseTable database(translator_set);
      database.setVariableNames(initializer.variableNames());
      initializer.fillDatabase(database);

      gum::learning::DBRowGeneratorSet    genset;
      gum::learning::DBRowGeneratorParser parser(database.handler(), genset);

      std::vector< gum::Size > modalities(nb_vars, 10);

      gum::learning::NoPrior                    prior(database);
      gum::learning::CorrectedMutualInformation cI(parser, prior);
      // cI.useCache( false );

      gum::learning::Miic search;

      // SimpleListenerForMiic listener;
      //  FilterListenerForMiic filterlistener;


      /*GUM_CONNECT(search,
                  onStructuralModification,
                  listener,
                  SimpleListenerForMiic::whenStructuralModification);*/

      // creating complete graph
      gum::MixedGraph graph, g;
      for (gum::Size i = 0; i < modalities.size(); ++i) {
        graph.addNodeWithId(i);
        for (gum::Size j = 0; j < i; ++j) {
          graph.addEdge(j, i);
        }
      }
      auto mg   = search.learnMixedStructure(cI, graph);
      auto pdag = search.learnPDAG(cI, graph);
    }

    static void test_MIIC_ms_order2_() {
      // filterlistener.filter = 6;
      // GUM_CONNECT(search,
      //             onStructuralModification,
      //             filterlistener,
      //             FilterListenerForMiic::whenStructuralModification);

      gum::learning::DBInitializerFromCSV initializer(
          GET_RESSOURCES_PATH("csv/ordinal_ms_order_2.csv"));
      const auto&       var_names = initializer.variableNames();
      const std::size_t nb_vars   = var_names.size();

      gum::learning::DBTranslatorSet translator_set;
      gum::LabelizedVariable         xvar("var", "", 0);
      for (int i = 0; i < 10; ++i) {
        xvar.addLabel(std::to_string(i));
      }

      gum::learning::DBTranslator4LabelizedVariable translator(xvar, true);
      for (std::size_t i = 0; i < nb_vars; ++i) {
        translator_set.insertTranslator(translator, i);
      }

      gum::learning::DatabaseTable database(translator_set);
      database.setVariableNames(initializer.variableNames());
      initializer.fillDatabase(database);

      gum::learning::DBRowGeneratorSet    genset;
      gum::learning::DBRowGeneratorParser parser(database.handler(), genset);

      std::vector< gum::Size > modalities(nb_vars, 10);

      gum::learning::NoPrior                    prior(database);
      gum::learning::CorrectedMutualInformation cI(parser, prior);
      // cI.useCache( false );

      gum::learning::Miic search;

      // SimpleListenerForMiic listener;
      //  FilterListenerForMiic filterlistener;


      /*GUM_CONNECT(search,
                  onStructuralModification,
                  listener,
                  SimpleListenerForMiic::whenStructuralModification);*/

      // creating complete graph
      gum::MixedGraph graph;
      for (gum::Size i = 0; i < modalities.size(); ++i) {
        graph.addNodeWithId(i);
        for (gum::Size j = 0; j < i; ++j) {
          graph.addEdge(j, i);
        }
      }
      auto mg   = search.learnMixedStructure(cI, graph);
      auto pdag = search.learnPDAG(cI, graph);
    }

    static void test_125_learn() {
      gum::learning::DBInitializerFromCSV initializer(GET_RESSOURCES_PATH("csv/bn125.csv"));
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

      std::vector< gum::Size > modalities(nb_vars, 2);

      gum::learning::NoPrior                    prior(database);
      gum::learning::CorrectedMutualInformation cI(parser, prior);
      gum::learning::Miic                       search;

      // creating complete graph
      gum::MixedGraph graph;
      for (gum::Size i = 0; i < modalities.size(); ++i) {
        graph.addNodeWithId(i);
        for (gum::Size j = 0; j < i; ++j) {
          graph.addEdge(j, i);
        }
      }
      try {
        auto mg = search.learnMixedStructure(cI, graph);
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }

      try {
        auto pdag = search.learnPDAG(cI, graph);
        // GUM_TRACE_VAR(dag.toDot())
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }

      try {
        auto dag = search.learnStructure(cI, graph);
        // GUM_TRACE_VAR(dag.toDot())
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }
    }
  };   // MiicTestSuite

  GUM_TEST_ACTIF(_latent_var_)
  GUM_TEST_ACTIF(_titanic_learn)
  GUM_TEST_ACTIF(_alarm_learn)
  GUM_TEST_ACTIF(_census_learn)
  GUM_TEST_ACTIF(_24_learn)
  GUM_TEST_ACTIF(_asia_ForbiddenGraph)
  GUM_TEST_ACTIF(_asia_MandatoryGraph)
  GUM_TEST_ACTIF(_alarm_MaxIndegree_)
  GUM_TEST_ACTIF(_MIIC_ms_order1_)
  GUM_TEST_ACTIF(_MIIC_ms_order2_)
  GUM_TEST_ACTIF(_125_learn)
}   // namespace gum_tests
