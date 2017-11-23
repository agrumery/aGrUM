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
#include <agrum/learning/paramUtils/paramEstimatorML.h>
#include <agrum/learning/threeOffTwo.h>

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


      gum::learning::ThreeOffTwo search;

      // creating complete graph
      gum::MixedGraph graph;
      for (gum::Size i = 0; i < modalities.size(); ++i) {
        graph.addNode(i);
        for (gum::Size j = 0; j < i; ++j) {
          graph.addEdge(j, i);
        }
      }
      graph = search.learnMixedStructure(I, graph);
      TS_ASSERT_EQUALS(graph.arcs().size(), gum::Size(6)    );
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

      gum::learning::ThreeOffTwo search;

      // creating complete graph
      gum::MixedGraph graph;
      for (gum::Size i = 0; i < modalities.size(); ++i) {
        graph.addNode(i);
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
    /*
        void est_3off2_asia_NMLcorr_Cycles() {
          gum::learning::DatabaseFromCSV database( GET_RESSOURCES_PATH( "asia.csv"
       ) );

          gum::learning::DBRowTranslatorSet<
              gum::learning::CellTranslatorCompactIntId>
              translators;
          translators.insertTranslator( 0, database.nbVariables() );

          gum::learning::FilteredRowGeneratorSet<gum::learning::RowGeneratorIdentity>
            generators;
          generators.insertGenerator ();

          auto filter =
              gum::learning::make_DB_row_filter( database, translators, generators
       );

          std::vector<gum::Idx> modalities = filter.modalities();

          gum::learning::CorrectedMutualInformation<> cI( filter, modalities );
          cI.useNML();
          //cI.useCache( false );

          gum::learning::ThreeOffTwo search;

          //creating complete graph
          gum::MixedGraph graph;
          for ( gum::Size i = 0; i < modalities.size(); ++i ){
            graph.addNode( i );
            for ( gum::Size j = 0; j < i; ++j){
              graph.addEdge( j, i );
            }
          }

              graph = search.learnMixedStructure( cI, graph );
          TS_ASSERT( graph.arcs().size() == 8 );
          TS_ASSERT( graph.edges().size() == 3 );
        }
    */
    void test_3off2_asia_dag_noCycles() {
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

      gum::learning::ThreeOffTwo search;

      // creating complete graph
      gum::MixedGraph graph;
      for (gum::Size i = 0; i < modalities.size(); ++i) {
        graph.addNode(i);
        for (gum::Size j = 0; j < i; ++j) {
          graph.addEdge(j, i);
        }
      }

      gum::DAG dag = search.learnStructure(cI, graph);
      TS_ASSERT_EQUALS(dag.arcs().size(), gum::Size(9))
      std::vector< gum::Arc > latents = search.latentVariables();
      ;
      TS_ASSERT_EQUALS(latents.size(), gum::Size(2));
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

      gum::learning::ThreeOffTwo search;

      // creating complete graph
      gum::MixedGraph graph;
      for (gum::Size i = 0; i < modalities.size(); ++i) {
        graph.addNode(i);
        for (gum::Size j = 0; j < i; ++j) {
          graph.addEdge(j, i);
        }
      }

      gum::DAG dag = search.learnStructure(cI, graph);
      std::cout << dag.toString() << std::endl;
      std::vector< gum::Arc > latents = search.latentVariables();
      TS_ASSERT(dag.arcs().size() == 9);
      std::cout << latents << std::endl;
      TS_ASSERT(latents.size() == 2);
    }

    void test_alarm_mixed() {
      gum::learning::DatabaseFromCSV database(GET_RESSOURCES_PATH("alarm.csv"));

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

      gum::learning::ThreeOffTwo search;

      // creating complete graph
      gum::MixedGraph graph;
      for (gum::Size i = 0; i < modalities.size(); ++i) {
        graph.addNode(i);
        for (gum::Size j = 0; j < i; ++j) {
          graph.addEdge(j, i);
        }
      }

      gum::MixedGraph g = search.learnMixedStructure(cI, graph);
      TS_ASSERT_EQUALS(g.arcs().size(), 0);
      TS_ASSERT_EQUALS(g.edges().size(), 9);

      gum::DAG                dag = search.learnStructure(cI, graph);
      std::vector< gum::Arc > latents = search.latentVariables();
      TS_ASSERT_EQUALS(dag.arcs().size(), 9);
      TS_ASSERT_EQUALS(latents.size(), 0);
    }


    void test_alarm_dag() {
      gum::learning::DatabaseFromCSV database(GET_RESSOURCES_PATH("alarm.csv"));

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

      gum::learning::ThreeOffTwo search;

      // creating complete graph
      gum::MixedGraph graph;
      for (gum::Size i = 0; i < modalities.size(); ++i) {
        graph.addNode(i);
        for (gum::Size j = 0; j < i; ++j) {
          graph.addEdge(j, i);
        }
      }
      gum::DAG dag = search.learnStructure(cI, graph);
      std::cout << dag.toString() << std::endl;
      std::vector< gum::Arc > latents = search.latentVariables();
      TS_ASSERT(dag.arcs().size() == 51);
      std::cout << latents << std::endl;
      TS_ASSERT(latents.size() == 4);
    }

    void test_perf_generator() {
      gum::Size n_nodes = 8;
      gum::Size n_arcs = 8;
      gum::Size n_bn = 100;

      gum::SimpleBayesNetGenerator< double, gum::SimpleCPTGenerator > generator(
        n_nodes, n_arcs);
      std::cout << std::endl;
      for (gum::Idx i = 0; i < n_bn; ++i) {
        gum::BayesNet< double > bn;
        generator.generateBN(bn);
        std::cout << bn.dag().toString() << std::endl;
      }
    }
  };

} /* namespace gum_tests */
