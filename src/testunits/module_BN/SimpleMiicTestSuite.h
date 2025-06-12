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

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

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
#include <agrum/BN/learning/paramUtils/paramEstimatorML.h>
#include <agrum/BN/learning/priors/noPrior.h>
#include <agrum/BN/learning/SimpleMiic.h>

namespace gum_tests {

  class SimpleListenerForSimpleMiic: public gum::Listener {
    public:
    SimpleListenerForSimpleMiic() {};

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

  class FilterListenerForSimpleMiic: public gum::Listener {
    public:
    gum::Size filter;
    FilterListenerForSimpleMiic() {};

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

  class SimpleMiicTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(_latent_var_) {
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

      gum::learning::SimpleMiic search;

      // creating complete graph
      gum::MixedGraph graph;
      for (gum::Size i = 0; i < modalities.size(); ++i) {
        graph.addNodeWithId(i);
        for (gum::Size j = 0; j < i; ++j) {
          graph.addEdge(j, i);
        }
      }

      graph = search.learnMixedStructure(cI, graph);
      // GUM_TRACE(search.latentVariables())
    }

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
