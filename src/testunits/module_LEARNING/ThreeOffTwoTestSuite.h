/***************************************************************************
 *   Copyright (C) 2007 by Quentin Falcand & Pierre-Henri Wuillemin        *
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

#include <agrum/BN/BayesNet.h>
#include <agrum/graphs/DAG.h>
#include <agrum/graphs/mixedGraph.h>
#include <agrum/variables/labelizedVariable.h>
#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>
#include <iostream>

#include <agrum/learning/database/DBCellTranslators/cellTranslatorCompactIntId.h>
#include <agrum/learning/database/databaseFromCSV.h>
#include <agrum/learning/database/filteredRowGenerators/rowGeneratorIdentity.h>
#include <agrum/learning/scores_and_tests/correctedMutualInformation.h>

#include <agrum/learning/database/DBCellTranslators/cellTranslatorNumber.h>
#include <agrum/learning/database/DBCellTranslators/cellTranslatorString.h>
#include <agrum/learning/database/DBCellTranslators/cellTranslatorUniversal.h>

#include <agrum/BN/generator/simpleBayesNetGenerator.h>
#include <agrum/BN/generator/simpleCPTGenerator.h>
#include <agrum/learning/Miic.h>
#include <agrum/learning/paramUtils/paramEstimatorML.h>

namespace gum_tests {

  class ThreeOffTwoTestSuite : public CxxTest::TestSuite {
    public:
    void test_3off2_asia_nocorr() {
      gum::learning::DatabaseFromCSV database(GET_RESSOURCES_PATH("asia.csv"));

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

      gum::learning::CorrectedMutualInformation<> I(filter, modalities);
      I.useNoCorr();


      gum::learning::Miic search;

      // creating complete graph
      gum::MixedGraph graph;
      for (gum::Size i = 0; i < modalities.size(); ++i) {
          graph.addNodeWithId(i);
        for (gum::Size j = 0; j < i; ++j) {
          graph.addEdge(j, i);
        }
      }
      graph = search.learnMixedStructure(I, graph);
      TS_ASSERT_EQUALS(graph.arcs().size(), gum::Size(0));
    }

    void test_3off2_asia_MDLcorr() {
      gum::learning::DatabaseFromCSV database(GET_RESSOURCES_PATH("asia.csv"));

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

      gum::learning::CorrectedMutualInformation<> cI(filter, modalities);
      cI.useMDL();
      // cI.useCache( false );

      gum::learning::Miic search;

      // creating complete graph
      gum::MixedGraph graph;
      for (gum::Size i = 0; i < modalities.size(); ++i) {
          graph.addNodeWithId(i);
        for (gum::Size j = 0; j < i; ++j) {
          graph.addEdge(j, i);
        }
      }

      graph = search.learnMixedStructure(cI, graph);
      TS_ASSERT_EQUALS(graph.arcs().size(), gum::Size(5));
      TS_ASSERT_EQUALS(graph.edges().size(), gum::Size(3));
    }

    void test_3off2_asia_NMLcorr() {
      gum::learning::DatabaseFromCSV database(GET_RESSOURCES_PATH("asia.csv"));

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

      gum::learning::CorrectedMutualInformation<> cI(filter, modalities);
      cI.useNML();
      // cI.useCache( false );

      gum::learning::Miic search;

      // creating complete graph
      gum::MixedGraph graph;
      for (gum::Size i = 0; i < modalities.size(); ++i) {
          graph.addNodeWithId(i);
        for (gum::Size j = 0; j < i; ++j) {
          graph.addEdge(j, i);
        }
      }

      graph = search.learnMixedStructure(cI, graph);
      TS_ASSERT_EQUALS(graph.arcs().size(), gum::Size(8));
      TS_ASSERT_EQUALS(graph.edges().size(), gum::Size(1));
      std::vector< gum::Arc > latents = search.latentVariables();
      TS_ASSERT_EQUALS(latents.size(), gum::Size(2));
    }

    void test_3off2_asia_dag() {
      gum::learning::DatabaseFromCSV database(GET_RESSOURCES_PATH("asia.csv"));

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

      gum::learning::CorrectedMutualInformation<> cI(filter, modalities);
      cI.useNML();

      gum::learning::Miic search;

      // creating complete graph
      gum::MixedGraph graph;
      for (gum::Size i = 0; i < modalities.size(); ++i) {
          graph.addNodeWithId(i);
        for (gum::Size j = 0; j < i; ++j) {
          graph.addEdge(j, i);
        }
      }


      gum::DAG                dag = search.learnStructure(cI, graph);
      std::vector< gum::Arc > latents = search.latentVariables();
      TS_ASSERT_EQUALS(dag.arcs().size(), gum::Size(9));
      TS_ASSERT_EQUALS(latents.size(), gum::Size(3));
    }

    void test_tonda() {
      gum::learning::DatabaseFromCSV database(
        GET_RESSOURCES_PATH("DBN_Tonda.csv"));

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

      gum::learning::CorrectedMutualInformation<> cI(filter, modalities);
      cI.useNML();
      // cI.useMDL();
      // cI.useNoCorr();

      gum::learning::Miic search;

      // creating complete graph
      gum::MixedGraph graph;
      for (gum::Size i = 0; i < modalities.size(); ++i) {
          graph.addNodeWithId(i);
        for (gum::Size j = 0; j < i; ++j) {
          graph.addEdge(j, i);
        }
      }
      gum::MixedGraph g = search.learnMixedStructure(cI, graph);
      TS_ASSERT_EQUALS(g.arcs().size(), gum::Size(0));
      TS_ASSERT_EQUALS(g.edges().size(), gum::Size(9));

      gum::DAG                dag = search.learnStructure(cI, graph);
      std::vector< gum::Arc > latents = search.latentVariables();
      TS_ASSERT_EQUALS(dag.arcs().size(), gum::Size(9));
      TS_ASSERT_EQUALS(latents.size(), gum::Size(0));
    }

    void test_3off2_asia_constraints() {
      gum::learning::DatabaseFromCSV database(GET_RESSOURCES_PATH("asia.csv"));

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

      gum::learning::CorrectedMutualInformation<> cI(filter, modalities);
      cI.useNML();
      // cI.useCache( false );

      gum::learning::Miic search;

      // adding constraints
      gum::HashTable< std::pair< gum::Idx, gum::Idx >, char > initial_marks;
      initial_marks.insert({4, 3}, '>');
      initial_marks.insert({5, 7}, '-');
      search.addConstraints(initial_marks);

      // creating complete graph
      gum::MixedGraph graph;
      for (gum::Size i = 0; i < modalities.size(); ++i) {
          graph.addNodeWithId(i);
        for (gum::Size j = 0; j < i; ++j) {
          graph.addEdge(j, i);
        }
      }

      graph = search.learnMixedStructure(cI, graph);
      TS_ASSERT_EQUALS(graph.arcs().size(), gum::Size(4));
      TS_ASSERT_EQUALS(graph.edges().size(), gum::Size(5));
      TS_ASSERT(graph.existsEdge(5, 7));
      TS_ASSERT(graph.existsArc(4, 3));
      std::vector< gum::Arc > latents = search.latentVariables();
      TS_ASSERT_EQUALS(latents.size(), gum::Size(3));
    }

    void test_MIIC_asia_NMLcorr() {
      gum::learning::DatabaseFromCSV database(GET_RESSOURCES_PATH("asia.csv"));

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

      gum::learning::CorrectedMutualInformation<> cI(filter, modalities);
      cI.useMDL();
      // cI.useCache( false );

      gum::learning::Miic search;
      search.setMiicBehaviour();
      search.set3off2Behaviour();
      search.setMiicBehaviour();
      // creating complete graph
      gum::MixedGraph graph, g;
      for (gum::Size i = 0; i < modalities.size(); ++i) {
          graph.addNodeWithId(i);
        for (gum::Size j = 0; j < i; ++j) {
          graph.addEdge(j, i);
        }
      }

      graph = search.learnMixedStructure(cI, graph);
      TS_ASSERT_EQUALS(graph.arcs().size(), gum::Size(5));
      TS_ASSERT_EQUALS(graph.edges().size(), gum::Size(3));
      std::vector< gum::Arc > latents = search.latentVariables();
      TS_ASSERT_EQUALS(latents.size(), gum::Size(0));
      gum::DAG dag = search.learnStructure(cI, graph);
    }

    void test_MIIC_asia_constraints() {
      gum::learning::DatabaseFromCSV database(GET_RESSOURCES_PATH("asia.csv"));

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

      gum::learning::CorrectedMutualInformation<> cI(filter, modalities);
      cI.useMDL();
      // cI.useCache( false );

      gum::learning::Miic search;
      search.setMiicBehaviour();
      search.set3off2Behaviour();
      search.setMiicBehaviour();

      // adding constraints
      gum::HashTable< std::pair< gum::Idx, gum::Idx >, char > initial_marks;
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
      TS_ASSERT_EQUALS(graph.arcs().size(), gum::Size(5));
      TS_ASSERT_EQUALS(graph.edges().size(), gum::Size(3));
      std::vector< gum::Arc > latents = search.latentVariables();
      TS_ASSERT_EQUALS(latents.size(), gum::Size(0));
      TS_ASSERT(graph.existsArc(4, 3));
      TS_ASSERT(graph.existsEdge(5, 7));
    }
  };

} /* namespace gum_tests */