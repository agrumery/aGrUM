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

#include <agrum/learning/3off2utils/graphComparator.h>
#include <agrum/graphs/DAG.h>
#include <agrum/graphs/diGraph.h>
#include <agrum/graphs/undiGraph.h>
#include <agrum/graphs/mixedGraph.h>
#include <agrum/BN/BayesNet.h>

namespace gum_tests {

  class GraphComparatorTestSuite : public CxxTest::TestSuite {
    public:
    void test_graph() {

      gum::learning::GraphComparator comp;

      gum::DiGraph dig1, dig2;
      gum::UndiGraph undig1, undig2;
      dig1.addNode(0);
      undig1.addNode(0);

      TS_ASSERT_THROWS( comp.compare( dig1, dig2 ), gum::OperationNotAllowed );
      TS_ASSERT_THROWS( comp.compare( dig2, dig1 ), gum::OperationNotAllowed );
      TS_ASSERT_THROWS( comp.compare( undig1, undig2 ), gum::OperationNotAllowed );
      TS_ASSERT_THROWS( comp.compare( undig2, undig1 ), gum::OperationNotAllowed );

      dig2.addNode(2);
      undig2.addNode(2);
      TS_ASSERT_THROWS( comp.compare( dig1, dig2 ), gum::InvalidNode );
      TS_ASSERT_THROWS( comp.compare( dig2, dig1 ), gum::InvalidNode );
      TS_ASSERT_THROWS( comp.compare( undig1, undig2 ), gum::InvalidNode );
      TS_ASSERT_THROWS( comp.compare( undig2, undig1 ), gum::InvalidNode );

      dig1.addNode(1);
      dig1.addNode(2);
      dig1.addArc(0, 1);
      dig1.addArc(0, 2);
      dig2.addNode(1);
      dig2.addNode(0);
      dig2.addArc(0, 1);
      dig2.addArc(0, 2);
      dig2.addArc(1, 2);

      comp.compare( dig1, dig2 );
      std::cout << "P=" << comp.precision_skeleton() << " R=" << comp.recall_skeleton() << " F=" << comp.f_score_skeleton() << std::endl;
      TS_ASSERT_DELTA( comp.precision_skeleton(), 0.666, 1e-3 );
      TS_ASSERT_DELTA( comp.recall_skeleton(), 1, 1e-3 );
      TS_ASSERT_DELTA( comp.f_score_skeleton(), 0.8, 1e-3 );

      undig1.addNode(1);
      undig1.addNode(2);
      undig1.addEdge(0, 1);
      undig1.addEdge(0, 2);
      undig2.addNode(1);
      undig2.addNode(0);
      undig2.addEdge(0, 1);
      undig2.addEdge(0, 2);
      undig2.addEdge(2, 1);

      comp.compare( undig1, undig2 );
      std::cout << "P=" << comp.precision_skeleton() << " R=" << comp.recall_skeleton() << " F=" << comp.f_score_skeleton() << std::endl;
      TS_ASSERT_DELTA( comp.precision_skeleton(), 0.666, 1e-3 );
      TS_ASSERT_DELTA( comp.recall_skeleton(), 1, 1e-3 );
      TS_ASSERT_DELTA( comp.f_score_skeleton(), 0.8, 1e-3 );

      //creating complete graph
      gum::MixedGraph graph;
      for ( gum::Size i = 0; i < 8; ++i ){
    	graph.addNode( i );
    	for ( gum::Size j = 0; j < i; ++j){
    	  graph.addEdge( j, i );
    	}
      }
      std::cout << "    " << graph.edges().size() << std::endl;
      //creating Asia
      gum::MixedGraph asia;
      for ( gum::Size i = 0; i < 8; ++i ){
    	asia.addNode( i );
      }
      asia.addArc( 3, 4 );
      asia.addArc( 4, 5 );
      asia.addArc( 0, 1 );
      asia.addArc( 0, 2 );
      asia.addArc( 1, 5 );
      asia.addArc( 2, 6 );
      asia.addArc( 5, 7 );
      asia.addArc( 5, 6 );

      gum::MixedGraph g;

      TS_ASSERT_THROWS( comp.compare( asia, g ), gum::OperationNotAllowed );

      for ( gum::Size i = 0; i < 16; i+=2 ){
    	g.addNode( i );
      }

      TS_ASSERT_THROWS( comp.compare( asia, g ), gum::InvalidNode );
      TS_GUM_ASSERT_THROWS_NOTHING( comp.compare( asia, graph ) );

      comp.compare( asia, graph );
      std::cout << "P=" << comp.precision_skeleton() << " R=" << comp.recall_skeleton() << " F=" << comp.f_score_skeleton() << std::endl;
      TS_ASSERT_DELTA( comp.precision_skeleton(), 0.2857, 1e-3 );
      TS_ASSERT_DELTA( comp.recall_skeleton(), 1, 1e-3 );
      TS_ASSERT_DELTA( comp.f_score_skeleton(), 0.4444, 1e-3 );

      std::cout << "P=" << comp.precision() << " R=" << comp.recall() << " F=" << comp.f_score() << std::endl;
      TS_ASSERT_DELTA( comp.precision(), 0, 1e-3 );
    }

    void test_bn(){
      gum::learning::GraphComparator comp;

      gum::BayesNet<double> bn1, bn2;
      bn1 = bn1.fastPrototype("0->1;0->2");
      bn2 = bn2.fastPrototype("0->1->2;0->2");

      TS_GUM_ASSERT_THROWS_NOTHING( comp.compare( bn1, bn2 ) );

      std::cout << "P=" << comp.precision_skeleton() << " R=" << comp.recall_skeleton() << " F=" << comp.f_score_skeleton() << std::endl;
      TS_ASSERT_DELTA( comp.precision_skeleton(), 0.666, 1e-3 );
      TS_ASSERT_DELTA( comp.recall_skeleton(), 1, 1e-3 );
      TS_ASSERT_DELTA( comp.f_score_skeleton(), 0.8, 1e-3 );

      std::cout << "P=" << comp.precision() << " R=" << comp.recall() << " F=" << comp.f_score() << std::endl;
      TS_ASSERT_DELTA( comp.precision(), 0.666, 1e-3 );
      TS_ASSERT_DELTA( comp.recall(), 1, 1e-3 );
      TS_ASSERT_DELTA( comp.f_score(), 0.8, 1e-3 );

      //creating asia
      gum::BayesNet<double> asia;
      asia = asia.fastPrototype("3->4->5->7;0->1->5->6;0->2->6");
      //asia = asia.fastPrototype("0->1;0->2;3->4->5->6;1->5->6;2->6");
      std::cout << asia.dag().toString() << std::endl;
      //bn1 = bn1.fastPrototype("3->4;7->5->6;5->4->1->0->2->6;5->1");

      gum::MixedGraph mg;
      for ( gum::Idx i = 0; i < 8; ++i ){
    	mg.addNode( i );
      }
      mg.addArc( 0, 1 );
      mg.addArc( 1, 5 );
      mg.addArc( 2, 1 );
      mg.addArc( 7, 6 );
      mg.addArc( 2, 6 );
      mg.addEdge( 4, 5 );
      mg.addEdge( 4, 7 );
      mg.addEdge( 5, 2 );
      mg.addEdge( 2, 3 );
      std::cout << mg.toString() << std::endl;

      TS_GUM_ASSERT_THROWS_NOTHING( comp.compare( mg, asia ) );

      comp.compare( asia, mg );
      std::cout << "P=" << comp.precision() << " R=" << comp.recall() << " F=" << comp.f_score() << std::endl;
      TS_ASSERT_DELTA( comp.precision(), 0.4444, 1e-3 );
      TS_ASSERT_DELTA( comp.recall(), 1, 1e-3 );
      TS_ASSERT_DELTA( comp.f_score(), 0.615385, 1e-3 );

      std::cout << "P=" << comp.precision_skeleton() << " R=" << comp.recall_skeleton() << " F=" << comp.f_score_skeleton() << std::endl;
      TS_ASSERT_DELTA( comp.precision_skeleton(), 0.8888, 1e-3 );
      TS_ASSERT_DELTA( comp.recall_skeleton(), 1, 1e-3 );
      TS_ASSERT_DELTA( comp.f_score_skeleton(), 0.9412, 1e-3 );
    }
  };

} /* namespace gum_tests */
