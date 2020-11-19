/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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


#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>
#include <iostream>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/algorithms/structuralComparator.h>
#include <agrum/tools/graphs/DAG.h>
#include <agrum/tools/graphs/diGraph.h>
#include <agrum/tools/graphs/mixedGraph.h>
#include <agrum/tools/graphs/undiGraph.h>

namespace gum_tests {

  class StructuralComparatorTestSuite: public CxxTest::TestSuite {
    public:
    void test_graph() {
      gum::StructuralComparator comp;

      gum::DiGraph   dig1, dig2;
      gum::UndiGraph undig1, undig2;
      dig1.addNodeWithId(0);
      undig1.addNodeWithId(0);

      TS_ASSERT_THROWS(comp.compare(dig1, dig2), gum::OperationNotAllowed);
      TS_ASSERT_THROWS(comp.compare(dig2, dig1), gum::OperationNotAllowed);
      TS_ASSERT_THROWS(comp.compare(undig1, undig2), gum::OperationNotAllowed);
      TS_ASSERT_THROWS(comp.compare(undig2, undig1), gum::OperationNotAllowed);

      dig2.addNodeWithId(2);
      undig2.addNodeWithId(2);
      TS_ASSERT_THROWS(comp.compare(dig1, dig2), gum::InvalidNode);
      TS_ASSERT_THROWS(comp.compare(dig2, dig1), gum::InvalidNode);
      TS_ASSERT_THROWS(comp.compare(undig1, undig2), gum::InvalidNode);
      TS_ASSERT_THROWS(comp.compare(undig2, undig1), gum::InvalidNode);

      dig1.addNodeWithId(1);
      dig1.addNodeWithId(2);
      dig1.addArc(0, 1);
      dig1.addArc(0, 2);
      dig2.addNodeWithId(1);
      dig2.addNodeWithId(0);
      dig2.addArc(0, 1);
      dig2.addArc(0, 2);
      dig2.addArc(1, 2);

      comp.compare(dig1, dig2);
      TS_ASSERT_DELTA(comp.precision_skeleton(), 0.666, 1e-3);
      TS_ASSERT_DELTA(comp.recall_skeleton(), 1, 1e-3);
      TS_ASSERT_DELTA(comp.f_score_skeleton(), 0.8, 1e-3);

      undig1.addNodeWithId(1);
      undig1.addNodeWithId(2);
      undig1.addEdge(0, 1);
      undig1.addEdge(0, 2);
      undig2.addNodeWithId(1);
      undig2.addNodeWithId(0);
      undig2.addEdge(0, 1);
      undig2.addEdge(0, 2);
      undig2.addEdge(2, 1);

      comp.compare(undig1, undig2);
      TS_ASSERT_DELTA(comp.precision_skeleton(), 0.666, 1e-3);
      TS_ASSERT_DELTA(comp.recall_skeleton(), 1, 1e-3);
      TS_ASSERT_DELTA(comp.f_score_skeleton(), 0.8, 1e-3);

      // creating complete graph
      gum::MixedGraph graph;
      for (gum::Size i = 0; i < 8; ++i) {
        graph.addNodeWithId(i);
        for (gum::Size j = 0; j < i; ++j) {
          graph.addEdge(j, i);
        }
      }
      // creating Asia
      gum::MixedGraph asia;
      for (gum::Size i = 0; i < 8; ++i) {
        asia.addNodeWithId(i);
      }
      asia.addArc(3, 4);
      asia.addArc(4, 5);
      asia.addArc(0, 1);
      asia.addArc(0, 2);
      asia.addArc(1, 5);
      asia.addArc(2, 6);
      asia.addArc(5, 7);
      asia.addArc(5, 6);

      gum::MixedGraph g;

      TS_ASSERT_THROWS(comp.compare(asia, g), gum::OperationNotAllowed);

      for (gum::Size i = 0; i < 16; i += 2) {
        g.addNodeWithId(i);
      }

      TS_ASSERT_THROWS(comp.compare(asia, g), gum::InvalidNode);
      TS_GUM_ASSERT_THROWS_NOTHING(comp.compare(asia, graph));

      comp.compare(asia, graph);
      TS_ASSERT_DELTA(comp.precision_skeleton(), 0.2857, 1e-3);
      TS_ASSERT_DELTA(comp.recall_skeleton(), 1, 1e-3);
      TS_ASSERT_DELTA(comp.f_score_skeleton(), 0.4444, 1e-3);
      TS_ASSERT_DELTA(comp.precision(), 0, 1e-3);
    }

    void test_bn() {
      gum::StructuralComparator comp;

      gum::BayesNet< double > bn1, bn2;
      bn1 = bn1.fastPrototype("0->1;0->2");
      bn2 = bn2.fastPrototype("0->1->2;0->2");

      TS_GUM_ASSERT_THROWS_NOTHING(comp.compare(bn1, bn2));
      TS_ASSERT_DELTA(comp.precision_skeleton(), 0.666, 1e-3);
      TS_ASSERT_DELTA(comp.recall_skeleton(), 1, 1e-3);
      TS_ASSERT_DELTA(comp.f_score_skeleton(), 0.8, 1e-3);
      TS_ASSERT_DELTA(comp.precision(), 0.666, 1e-3);
      TS_ASSERT_DELTA(comp.recall(), 1, 1e-3);
      TS_ASSERT_DELTA(comp.f_score(), 0.8, 1e-3);

      // creating asia
      gum::BayesNet< double > asia;
      asia = asia.fastPrototype("3->4->5->7;0->1->5->6;0->2->6");
      // asia = asia.fastPrototype("0->1;0->2;3->4->5->6;1->5->6;2->6");
      // bn1 = bn1.fastPrototype("3->4;7->5->6;5->4->1->0->2->6;5->1");

      gum::MixedGraph mg;
      for (gum::Idx i = 0; i < 8; ++i) {
        mg.addNodeWithId(i);
      }
      mg.addArc(0, 1);
      mg.addArc(1, 5);
      mg.addArc(2, 1);
      mg.addArc(7, 6);
      mg.addArc(2, 6);
      mg.addEdge(4, 5);
      mg.addEdge(4, 7);
      mg.addEdge(5, 2);
      mg.addEdge(2, 3);

      TS_GUM_ASSERT_THROWS_NOTHING(comp.compare(mg, asia));

      comp.compare(asia, mg);
      TS_ASSERT_DELTA(comp.precision(), 0.4444, 1e-3);
      TS_ASSERT_DELTA(comp.recall(), 1, 1e-3);
      TS_ASSERT_DELTA(comp.f_score(), 0.615385, 1e-3);
      TS_ASSERT_DELTA(comp.precision_skeleton(), 0.8888, 1e-3);
      TS_ASSERT_DELTA(comp.recall_skeleton(), 1, 1e-3);
      TS_ASSERT_DELTA(comp.f_score_skeleton(), 0.9412, 1e-3);
    }
  };

} /* namespace gum_tests */
