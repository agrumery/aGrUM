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

#include <agrum/BN/BayesNet.h>
#include <agrum/tools/graphs/DAG.h>
#include <agrum/tools/graphs/mixedGraph.h>
#include <agrum/tools/variables/labelizedVariable.h>
#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>
#include <iostream>

#include <agrum/tools/database/DBTranslator4LabelizedVariable.h>
#include <agrum/tools/database/DBRowGeneratorParser.h>
#include <agrum/tools/database/DBRowGeneratorIdentity.h>
#include <agrum/tools/database/DBInitializerFromCSV.h>
#include <agrum/tools/database/databaseTable.h>
#include <agrum/tools/database/DBTranslatorSet.h>

#include <agrum/BN/learning/priors/noPrior.h>

#include <agrum/BN/generator/simpleBayesNetGenerator.h>
#include <agrum/BN/generator/simpleCPTGenerator.h>
#include <agrum/BN/learning/Miic.h>
#include <agrum/BN/learning/paramUtils/paramEstimatorML.h>

#include <agrum/tools/core/approximations/approximationScheme.h>
#include <agrum/tools/core/approximations/approximationSchemeListener.h>

namespace gum_tests {

  class SimpleListenerForMiic : public gum::Listener {
    public:
    SimpleListenerForMiic(){};
    void whenStructuralModification(const void* src, gum::NodeId x, gum::NodeId y, std::string action, std::string explain) {
      static int s=0;
      std::cout << std::setfill('0') << std::setw(5) <<++s<<" : " << action << " | " << explain << std::endl;
    };
  };

  class FilterListenerForMiic : public gum::Listener {
    public:
    gum::Size filter;
    FilterListenerForMiic(){};

    void whenStructuralModification(const void* src, gum::NodeId x, gum::NodeId y, std::string action, std::string explain) {
      static int s=0;
      if (x == filter || y == filter) {
        std::cout << std::setfill('0') << std::setw(5) <<++s<<" : " << action << " | " << explain << std::endl;
      }
    };
  };

  class MiicTestSuite: public CxxTest::TestSuite {
    public:
    GUM_INACTIVE_TEST(_asia_MDLcorr) {
      gum::learning::DBInitializerFromCSV initializer(GET_RESSOURCES_PATH("csv/asia.csv"));
      const auto&                         var_names = initializer.variableNames();
      const std::size_t                   nb_vars   = var_names.size();

      gum::learning::DBTranslatorSet translator_set;
      gum::LabelizedVariable         var_lab("x", "", 0);
      var_lab.addLabel("0");
      var_lab.addLabel("1");
      gum::learning::DBTranslator4LabelizedVariable translator(var_lab);
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

      graph = search.learnMixedStructure(cI, graph);

      TS_ASSERT_EQUALS(graph.arcs().size(), (gum::Size)5)
      TS_ASSERT_EQUALS(graph.edges().size(), (gum::Size)3)
      std::vector< gum::Arc > latents = search.latentVariables();
      TS_ASSERT_EQUALS(latents.size(), (gum::Size)0)
    }

    GUM_INACTIVE_TEST(_asia_ForbiddenGraph) {
      
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

      gum::learning::Miic   search;

      // creating complete graph
      gum::MixedGraph graph;
      gum::DiGraph forbidGraph;
      for (gum::Size i = 0; i < modalities.size(); ++i) {
        forbidGraph.addNodeWithId(i);
        graph.addNodeWithId(i);
        for (gum::Size j = 0; j < i; ++j) {
          graph.addEdge(j, i);
        }
      }    

      // adding constraints
      forbidGraph.addArc(2,6);
      forbidGraph.addArc(6,2);
      forbidGraph.addArc(1,5);
      search.setForbiddenGraph(forbidGraph);

      graph = search.learnMixedStructure(cI, graph);

      TS_ASSERT(!graph.existsArc(2, 6));
      TS_ASSERT(!graph.existsEdge(2, 6));
      TS_ASSERT(!graph.existsArc(1, 5));
    }

    GUM_INACTIVE_TEST(_asia_MandatoryGraph) {
      
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

      gum::learning::Miic   search;

      // creating complete graph
      gum::MixedGraph graph;
      gum::DAG mandaGraph;
      for (gum::Size i = 0; i < modalities.size(); ++i) {
        mandaGraph.addNodeWithId(i);
        graph.addNodeWithId(i);
        for (gum::Size j = 0; j < i; ++j) {
          graph.addEdge(j, i);
        }
      }    

      // adding constraints
      mandaGraph.addArc(3,4);
      search.setMandatoryGraph(mandaGraph);

      graph = search.learnMixedStructure(cI, graph);

      TS_ASSERT(graph.existsArc(3, 4))
    }

    GUM_INACTIVE_TEST(_alarm_) {
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

      graph = search.learnMixedStructure(cI, graph);
      TS_ASSERT(graph.parents(36).size()==3);

    }

    GUM_INACTIVE_TEST(_alarm_MaxIndegree_) {
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

      //Learn Structure
      graph = search.learnMixedStructure(cI, graph);

      gum::NodeSet nodesSet = graph.asNodeSet();
      for(auto& x: nodesSet){
        TS_ASSERT(graph.parents(x).size()<=n)
      }
    }

    GUM_ACTIVE_TEST(_MIIC_ms_order1_) {
      // filterlistener.filter = 6;
      // GUM_CONNECT(search,
      //             onStructuralModification,
      //             filterlistener,
      //             FilterListenerForMiic::whenStructuralModification);

      gum::learning::DBInitializerFromCSV initializer(GET_RESSOURCES_PATH("csv/ordinal_ms_order_1.csv"));
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
      // cI.useCache( false );

      gum::learning::Miic search;

      //SimpleListenerForMiic listener;
      // FilterListenerForMiic filterlistener;

      std::cout << "TEST ORDER 1 :" << std::endl;

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
      gum::DAG dag = search.learnStructure(cI, graph);
      GUM_TRACE("Arcs Order 1: " << dag.arcs())
    }

    GUM_ACTIVE_TEST(_MIIC_ms_order2_) {
      // filterlistener.filter = 6;
      // GUM_CONNECT(search,
      //             onStructuralModification,
      //             filterlistener,
      //             FilterListenerForMiic::whenStructuralModification);

      gum::learning::DBInitializerFromCSV initializer(GET_RESSOURCES_PATH("csv/ordinal_ms_order_2.csv"));
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
      // cI.useCache( false );

      gum::learning::Miic search;

      //SimpleListenerForMiic listener;
      // FilterListenerForMiic filterlistener;

      std::cout << "TEST ORDER 2 :" << std::endl;

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
      gum::DAG dag = search.learnStructure(cI, graph);
      GUM_TRACE("Arcs Order 2: " << dag.arcs())
    }


  }; // MiicTestSuite
}   // namespace gum_test
