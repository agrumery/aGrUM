/**
 *
 *   Copyright (c) 2005-2022 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
#include <agrum/BN/learning/SimpleMiic.h>
#include <agrum/BN/learning/paramUtils/paramEstimatorML.h>

namespace gum_tests {

  class SimpleListenerForSimpleMiic : public gum::Listener {
    public:
    SimpleListenerForSimpleMiic(){};

    void whenStructuralModification(const void* src, gum::NodeId x, gum::NodeId y, std::string action, std::string explain) {
      static int s=0;
      std::cout << std::setfill('0') << std::setw(5) <<++s<<" : " << action << " | " << explain << std::endl;
    };
  };

  class FilterListenerForSimpleMiic : public gum::Listener {
    public:
    gum::Size filter;
    FilterListenerForSimpleMiic(){};

    void whenStructuralModification(const void* src, gum::NodeId x, gum::NodeId y, std::string action, std::string explain) {
      static int s=0;
      if (x == filter || y == filter) {
        std::cout << std::setfill('0') << std::setw(5) <<++s<<" : " << action << " | " << explain << std::endl;
      }
    };
  };

  class SimpleMiicTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(_MIIC_asia_NMLcorr) {
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

      gum::learning::SimpleMiic search;
      // creating complete graph
      gum::MixedGraph graph, g;
      for (gum::Size i = 0; i < modalities.size(); ++i) {
        graph.addNodeWithId(i);
        for (gum::Size j = 0; j < i; ++j) {
          graph.addEdge(j, i);
        }
      }

      graph = search.learnMixedStructure(cI, graph);
      // TS_ASSERT_EQUALS(graph.arcs().size(), (gum::Size)5)
      // TS_ASSERT_EQUALS(graph.edges().size(), (gum::Size)3)
      std::vector< gum::Arc > latents = search.latentVariables();
      TS_ASSERT_EQUALS(latents.size(), (gum::Size)0)
      gum::DAG dag = search.learnStructure(cI, graph);
    }

    GUM_ACTIVE_TEST(_MIIC_asia_constraints) {
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

      gum::learning::SimpleMiic search;

      // adding constraints
      gum::HashTable< std::pair< gum::NodeId, gum::NodeId >, char > initial_marks;
      initial_marks.insert({4, 3}, '>');
      initial_marks.insert({5, 7}, '-');
      search.addConstraints(initial_marks);

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
      TS_ASSERT(graph.existsArc(4, 3))
      TS_ASSERT(graph.existsEdge(5, 7))
    }

    void xtest_tonda() {
      /*
      gum::learning::DatabaseFromCSV database(
        GET_RESSOURCES_PATH("csv/DBN_Tonda.csv"));

      gum::learning::DBRowTranslatorSet<
        gum::learning::CellTranslatorCompactIntId >
        translators;
      translators.insertTranslator(0, database.nbVariables());

      gum::learning::FilteredRowGeneratorSet< gum::learning::RowGeneratorIdentity >
        generators;
      generators.insertGenerator();

      auto filter =
        gum::learning::make_DB_row_filter(database, translators, generators);

      std::vector< gum::Idx > modalities = filter.modalities();

      gum::learning::CorrectedMutualInformation cI(filter, modalities);
      cI.useNML();
      // cI.useMDL();
      // cI.useNoCorr();

      gum::learning::SimpleMiic search;

      // creating complete graph
      gum::MixedGraph graph;
      for (gum::Size i = 0; i < modalities.size(); ++i) {
        graph.addNodeWithId(i);
        for (gum::Size j = 0; j < i; ++j) {
          graph.addEdge(j, i);
        }
      }
      gum::MixedGraph g = search.learnMixedStructure(cI, graph);
      TS_ASSERT_EQUALS(g.arcs().size(), (gum::Size)0)
      TS_ASSERT_EQUALS(g.edges().size(), (gum::Size)9)

      gum::DAG                dag = search.learnStructure(cI, graph);
      std::vector< gum::Arc > latents = search.latentVariables();
      TS_ASSERT_EQUALS(dag.arcs().size(), (gum::Size)9)
      TS_ASSERT_EQUALS(latents.size(), (gum::Size)0)
      */
    }
  };

} /* namespace gum_tests */
