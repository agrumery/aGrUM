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
#include <agrum/graphs/mixedGraph.h>
#include <agrum/graphs/DAG.h>
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

#include <agrum/learning/threeOffTwo.h>
#include <agrum/learning/paramUtils/paramEstimatorML.h>
#include <agrum/BN/generator/simpleBayesNetGenerator.h>
#include <agrum/BN/generator/simpleCPTGenerator.h>

#define MY_ALARM GET_RESSOURCES_PATH( "alarm.csv" )
#define MY_ASIA GET_RESSOURCES_PATH( "asia.csv" )

namespace gum_tests {

  class ThreeOffTwoTestSuite : public CxxTest::TestSuite {
    public:
    void /*test*/_3off2_asia_nocorr() {
      gum::learning::DatabaseFromCSV database( MY_ASIA );

      gum::learning::DBRowTranslatorSet<
          gum::learning::CellTranslatorCompactIntId>
          translators;
      translators.insertTranslator( 0, database.nbVariables() );

      gum::learning::FilteredRowGeneratorSet<gum::learning::RowGeneratorIdentity>
        generators;
      generators.insertGenerator ();

      auto filter =
          gum::learning::make_DB_row_filter( database, translators, generators );

      std::vector<gum::Idx> modalities = filter.modalities();

      gum::learning::CorrectedMutualInformation<> I( filter, modalities );
      I.useNoCorr();
      std::cout << I.N() << std::endl;


      gum::learning::ThreeOffTwo search;

      //creating complete graph
      gum::MixedGraph graph;
      for ( gum::Size i = 0; i < modalities.size(); ++i ){
    	graph.addNode( i );
    	for ( gum::Size j = 0; j < i; ++j){
    	  graph.addEdge( j, i );
    	}
      }
      std::cout << graph.toString() << std::endl;
	  graph = search.learnMixedStructure( I, graph );
      TS_ASSERT( graph.arcs().size() == 6 );
    }

    void /*test*/_3off2_asia_MDLcorr() {
      gum::learning::DatabaseFromCSV database( MY_ASIA );

      gum::learning::DBRowTranslatorSet<
          gum::learning::CellTranslatorCompactIntId>
          translators;
      translators.insertTranslator( 0, database.nbVariables() );

      gum::learning::FilteredRowGeneratorSet<gum::learning::RowGeneratorIdentity>
        generators;
      generators.insertGenerator ();

      auto filter =
          gum::learning::make_DB_row_filter( database, translators, generators );

      std::vector<gum::Idx> modalities = filter.modalities();

      gum::learning::CorrectedMutualInformation<> cI( filter, modalities );
      cI.useMDL();
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
      TS_ASSERT( graph.arcs().size() == 2 );
      TS_ASSERT( graph.edges().size() == 7 );
    }

    void /*test*/_3off2_asia_NMLcorr_noCycles() {
      gum::learning::DatabaseFromCSV database( MY_ASIA );

      gum::learning::DBRowTranslatorSet<
          gum::learning::CellTranslatorCompactIntId>
          translators;
      translators.insertTranslator( 0, database.nbVariables() );

      gum::learning::FilteredRowGeneratorSet<gum::learning::RowGeneratorIdentity>
        generators;
      generators.insertGenerator ();

      auto filter =
          gum::learning::make_DB_row_filter( database, translators, generators );

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
	  std::cout << graph.toString() << std::endl;
      TS_ASSERT( graph.arcs().size() == 5 );
      TS_ASSERT( graph.edges().size() == 4 );
      std::vector<gum::Arc> latents = search.getLatent();
      std::cout << latents << std::endl;
      TS_ASSERT( latents.size() == 2 );
    }
/*
    void est_3off2_asia_NMLcorr_Cycles() {
      gum::learning::DatabaseFromCSV database( MY_ASIA );

      gum::learning::DBRowTranslatorSet<
          gum::learning::CellTranslatorCompactIntId>
          translators;
      translators.insertTranslator( 0, database.nbVariables() );

      gum::learning::FilteredRowGeneratorSet<gum::learning::RowGeneratorIdentity>
        generators;
      generators.insertGenerator ();

      auto filter =
          gum::learning::make_DB_row_filter( database, translators, generators );

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
    void /*test*/_3off2_asia_dag_noCycles() {
      gum::learning::DatabaseFromCSV database( MY_ASIA );

      gum::learning::DBRowTranslatorSet<
          gum::learning::CellTranslatorCompactIntId>
          translators;
      translators.insertTranslator( 0, database.nbVariables() );

      gum::learning::FilteredRowGeneratorSet<gum::learning::RowGeneratorIdentity>
        generators;
      generators.insertGenerator ();

      auto filter =
          gum::learning::make_DB_row_filter( database, translators, generators );

      std::vector<gum::Idx> modalities = filter.modalities();

      gum::learning::CorrectedMutualInformation<> cI( filter, modalities );
      cI.useNML();

      gum::learning::ThreeOffTwo search;

      //creating complete graph
      gum::MixedGraph graph;
      for ( gum::Size i = 0; i < modalities.size(); ++i ){
    	graph.addNode( i );
    	for ( gum::Size j = 0; j < i; ++j){
    	  graph.addEdge( j, i );
    	}
      }


	  gum::DAG dag = search.learnStructure( cI, graph );
	  std::cout << dag.toString() << std::endl;
      std::vector<gum::Arc> latents = search.getLatent();
      TS_ASSERT( dag.arcs().size() == 9 );
      std::cout << latents << std::endl;
      TS_ASSERT( latents.size() == 2 );
    }

    void test_alarm_mixed() {
      gum::learning::DatabaseFromCSV database( MY_ALARM );

      gum::learning::DBRowTranslatorSet<
          gum::learning::CellTranslatorCompactIntId>
          translators;
      translators.insertTranslator( 0, database.nbVariables() );

      gum::learning::FilteredRowGeneratorSet<gum::learning::RowGeneratorIdentity>
        generators;
      generators.insertGenerator ();

      auto filter =
          gum::learning::make_DB_row_filter( database, translators, generators );

      std::vector<gum::Idx> modalities = filter.modalities();

      gum::learning::CorrectedMutualInformation<> cI( filter, modalities );
      cI.useNML();
      //cI.useMDL();
      //cI.useNoCorr();

      gum::learning::ThreeOffTwo search;

      //creating complete graph
      gum::MixedGraph graph;
      for ( gum::Size i = 0; i < modalities.size(); ++i ){
    	graph.addNode( i );
    	for ( gum::Size j = 0; j < i; ++j){
    	  graph.addEdge( j, i );
    	}
      }
	  gum::MixedGraph g = search.learnMixedStructure( cI, graph );
	  std::cout << g.toString() << std::endl;
	  TS_ASSERT_EQUALS( g.arcs().size(), 39 );
	  TS_ASSERT_EQUALS( g.edges().size(), 12 );
      std::vector<gum::Arc> latents = search.getLatent();
      std::cout << latents << std::endl;
      //TS_ASSERT( latents.size() == 2 );

    }

    void /*test*/_alarm_dag() {
      gum::learning::DatabaseFromCSV database( MY_ALARM );

      gum::learning::DBRowTranslatorSet<
          gum::learning::CellTranslatorCompactIntId>
          translators;
      translators.insertTranslator( 0, database.nbVariables() );

      gum::learning::FilteredRowGeneratorSet<gum::learning::RowGeneratorIdentity>
        generators;
      generators.insertGenerator ();

      auto filter =
          gum::learning::make_DB_row_filter( database, translators, generators );

      std::vector<gum::Idx> modalities = filter.modalities();

      gum::learning::CorrectedMutualInformation<> cI( filter, modalities );
      cI.useNML();

      gum::learning::ThreeOffTwo search;

      //creating complete graph
      gum::MixedGraph graph;
      for ( gum::Size i = 0; i < modalities.size(); ++i ){
    	graph.addNode( i );
    	for ( gum::Size j = 0; j < i; ++j){
    	  graph.addEdge( j, i );
    	}
      }
	  gum::DAG dag = search.learnStructure( cI, graph );
	  std::cout << dag.toString() << std::endl;
      std::vector<gum::Arc> latents = search.getLatent();
      TS_ASSERT( dag.arcs().size() == 51 );
      std::cout << latents << std::endl;
      TS_ASSERT( latents.size() == 4 );

    }

    void /*test*/_perf_generator() {
      gum::Size n_nodes = 8;
      gum::Size n_arcs = 8;
      gum::Size n_bn = 100;

      gum::SimpleBayesNetGenerator<double, gum::SimpleCPTGenerator> generator(n_nodes, n_arcs);
      std::cout << std::endl;
      for ( gum::Idx i=0; i<n_bn; ++i ){
    	gum::BayesNet<double> bn;
		generator.generateBN(bn);
		std::cout << bn.dag().toString() << std::endl;
      }

    }

    void /*test*/_perf_learning() {
      gum::Size n_bn = 100;
      for (gum::Idx i=0; i<n_bn; ++i){
        gum::learning::DatabaseFromCSV database( "/home/falcand/Documents/c3off2/bn" + std::to_string( i ) + ".csv" );

        gum::learning::DBRowTranslatorSet<
            gum::learning::CellTranslatorCompactIntId>
            translators;
        translators.insertTranslator( 0, database.nbVariables() );

        gum::learning::FilteredRowGeneratorSet<gum::learning::RowGeneratorIdentity>
          generators;
        generators.insertGenerator ();

        auto filter =
            gum::learning::make_DB_row_filter( database, translators, generators );

        std::vector<gum::Idx> modalities = filter.modalities();

        gum::learning::CorrectedMutualInformation<> cI( filter, modalities );
        cI.useNML();

        gum::learning::ThreeOffTwo search;

        //creating complete graph
        gum::MixedGraph graph, g_out;
        for ( gum::Size i = 0; i < filter.nbVariables(); ++i ){
      	  graph.addNode( i );
      	  g_out.addNode( i );
      	  for ( gum::Size j = 0; j < i; ++j){
      	    graph.addEdge( j, i );
      	  }
        }

        const std::vector<std::string> vars = filter.variableNames();

  	    gum::MixedGraph g = search.learnMixedStructure( cI, graph );
  	    //reconstructing the graph with th right nodes
  	    for ( const auto& edge : g.edges() ){
  	      gum::NodeId n_out_first = std::stoi(vars[edge.first()]);
          //if ( !g_out.exists( n_out_first ) ) g_out.addNode( n_out_first );
          gum::NodeId n_out_second = std::stoi(vars[edge.second()]);
          //if ( !g_out.exists( n_out_second ) ) g_out.addNode( n_out_second );

          g_out.addEdge( n_out_first, n_out_second);
  	    }
  	    for ( const auto& arc : g.arcs() ){
  	      gum::NodeId n_out_tail = std::stoi(vars[arc.tail()]);
          //if ( !g_out.exists( n_out_tail ) ) g_out.addNode( n_out_tail );
          gum::NodeId n_out_head = std::stoi(vars[arc.head()]);
          //if ( !g_out.exists( n_out_head ) ) g_out.addNode( n_out_head );

          g_out.addArc( n_out_tail, n_out_head);
  	    }
  	    std::cout << g_out.toString() << std::endl;
        //TS_ASSERT( g.arcs().size() == 41 );
        //TS_ASSERT( g.edges().size() == 11 );
        //std::vector<gum::Arc> latents = search.getLatent();
        //std::cout << latents << std::endl;
        //TS_ASSERT( latents.size() == 2 );

      }
    }
  };

} /* namespace gum_tests */
