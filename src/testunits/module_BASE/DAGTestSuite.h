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
#include <iostream>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#include <agrum/tools/graphs/DAG.h>
#include <agrum/tools/graphs/graphElements.h>
#include <agrum/tools/graphs/graphElements.h>

// The graph used for the tests:
//          0   1_          0 -> 2
//         / \ / /          0 -> 3
//        2   3 /           2 -> 4
//         \ / /            3 -> 5
//          4_/             1 -> 3
//                          1 -> 4

namespace gum_tests {

  class [[maybe_unused]] DAGTestSuite: public CxxTest::TestSuite {
    private:
    static gum::Size simpleDoubleFunction(const gum::NodeId& aNodeId) { return aNodeId * 2; }

    static gum::Size simpleArcMapFunction(const gum::Arc& anArc) {
      return anArc.tail() + anArc.head();
    }

    static gum::Size twistedMapFunction(const gum::NodeId& aNode) { throw(aNode); }

    gum::DAG buildGraph() {
      gum::DAG g;
      id1 = g.addNode();
      id2 = g.addNode();
      id3 = g.addNode();
      id4 = g.addNode();
      id5 = g.addNode();

      g.addArc(id1, id3);
      g.addArc(id3, id5);
      g.addArc(id2, id4);
      g.addArc(id1, id4);
      g.addArc(id4, id5);
      g.addArc(id2, id5);

      return g;
    }

    public:
    gum::NodeId id1, id2, id3, id4, id5;

    GUM_ACTIVE_TEST(Constructor1) {
      gum::DAG* graph = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING((graph = new gum::DAG()))
      TS_GUM_ASSERT_THROWS_NOTHING((delete (graph)))
    }

    GUM_ACTIVE_TEST(Insert1) {
      gum::DAG    graph;
      gum::NodeId id1, id2, id3, id4, id5;
      id1 = id2 = id3 = id4 = id5 = 0;

      TS_GUM_ASSERT_THROWS_NOTHING(id1 = graph.addNode())
      TS_GUM_ASSERT_THROWS_NOTHING(id2 = graph.addNode())
      TS_GUM_ASSERT_THROWS_NOTHING(id3 = graph.addNode())
      TS_GUM_ASSERT_THROWS_NOTHING(id4 = graph.addNode())
      TS_GUM_ASSERT_THROWS_NOTHING(id5 = graph.addNode())

      TS_GUM_ASSERT_THROWS_NOTHING(graph.addArc(id1, id3))
      TS_GUM_ASSERT_THROWS_NOTHING(graph.addArc(id3, id5))
      TS_GUM_ASSERT_THROWS_NOTHING(graph.addArc(id2, id4))
      TS_GUM_ASSERT_THROWS_NOTHING(graph.addArc(id1, id4))
      TS_GUM_ASSERT_THROWS_NOTHING(graph.addArc(id4, id5))
      TS_GUM_ASSERT_THROWS_NOTHING(graph.addArc(id2, id5))
    }

    GUM_ACTIVE_TEST(Insert2) {
      gum::DAG    graph;
      gum::NodeId id1, id2, id3, id4, id5;
      id1 = id2 = id3 = id4 = id5 = 0;

      TS_GUM_ASSERT_THROWS_NOTHING(id1 = graph.addNode())
      TS_GUM_ASSERT_THROWS_NOTHING(id2 = graph.addNode())
      TS_GUM_ASSERT_THROWS_NOTHING(id3 = graph.addNode())
      TS_GUM_ASSERT_THROWS_NOTHING(id4 = graph.addNode())
      TS_GUM_ASSERT_THROWS_NOTHING(id5 = graph.addNode())

      TS_GUM_ASSERT_THROWS_NOTHING(graph.addArc(id1, id3))
      TS_GUM_ASSERT_THROWS_NOTHING(graph.addArc(id3, id5))
      TS_GUM_ASSERT_THROWS_NOTHING(graph.addArc(id2, id4))
      TS_GUM_ASSERT_THROWS_NOTHING(graph.addArc(id1, id4))
      TS_GUM_ASSERT_THROWS_NOTHING(graph.addArc(id4, id5))

      TS_ASSERT_THROWS(graph.addArc(id5, id2), const gum::InvalidDirectedCycle&)
      TS_ASSERT_THROWS(graph.addArc(1000, id2), const gum::InvalidNode&)
      TS_ASSERT_THROWS(graph.addArc(id2, 1000), const gum::InvalidNode&)
    }

    GUM_ACTIVE_TEST(CopyConstructor) {
      gum::DAG graph = buildGraph();

      gum::DAG* copy = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(copy = new gum::DAG(graph))
      TS_ASSERT_EQUALS(graph, *copy)
      delete (copy);

      TS_GUM_ASSERT_THROWS_NOTHING(gum::DAG copy2 = graph)
      TS_GUM_ASSERT_THROWS_NOTHING(gum::DAG copy3(graph))
    }

    GUM_ACTIVE_TEST(EmptyNodes) {
      gum::DAG graph;

      TS_ASSERT(graph.empty())
      graph = buildGraph();
      TS_ASSERT(!graph.empty())
    }

    GUM_ACTIVE_TEST(EmptyArcs) {
      gum::DAG graph;
      TS_ASSERT(graph.emptyArcs())
      graph = buildGraph();
      TS_ASSERT(!graph.emptyArcs())
    }

    GUM_ACTIVE_TEST(ClearNodes) {
      gum::DAG graph = buildGraph();
      TS_ASSERT(!graph.empty())
      TS_GUM_ASSERT_THROWS_NOTHING(graph.clear())
      TS_ASSERT(graph.empty() && graph.emptyArcs())
    }

    GUM_ACTIVE_TEST(ClearArcs) {
      gum::DAG graph = buildGraph();
      TS_ASSERT(!graph.emptyArcs())
      TS_GUM_ASSERT_THROWS_NOTHING(graph.clearArcs())
      TS_ASSERT(graph.emptyArcs())
      TS_ASSERT(!graph.empty())
    }

    GUM_ACTIVE_TEST(AddDelNodes_2) {
      gum::DAG graph = buildGraph();

      TS_ASSERT(graph.exists(id1))
      TS_ASSERT(graph.exists(id2))
      TS_ASSERT(graph.exists(id3))
      TS_ASSERT(graph.exists(id4))
      TS_ASSERT(graph.exists(id5))
      TS_ASSERT(!graph.exists(id5 + id4 + id3 + id2 + id1))

      TS_ASSERT(graph.existsArc(id3, id5))
      TS_ASSERT(!graph.existsArc(id5, id3))
      TS_ASSERT(!graph.existsArc(id1, id1))

      gum::Size nodeCount = graph.size();
      gum::Size arcCount  = graph.sizeArcs();

      TS_GUM_ASSERT_THROWS_NOTHING(graph.eraseNode(id2))

      TS_ASSERT_EQUALS(nodeCount, graph.size() + 1)
      TS_ASSERT_EQUALS(arcCount, graph.sizeArcs() + 2)

      TS_ASSERT(!graph.exists(id2))
      TS_ASSERT(!graph.existsArc(id2, id4))
      TS_ASSERT(!graph.existsArc(id5, id2))
    }

    GUM_ACTIVE_TEST(RemoveNodes_1) {
      gum::DAG graph = buildGraph();

      gum::Size nodeCount = graph.size();
      gum::Size arcCount  = graph.sizeArcs();

      TS_ASSERT_EQUALS(nodeCount, (gum::Size)5)
      TS_ASSERT_EQUALS(arcCount, (gum::Size)6)

      for (int i = 0; i < 10; i++) {
        TS_GUM_ASSERT_THROWS_NOTHING(graph.eraseNode(id5))
      }

      TS_ASSERT_EQUALS(nodeCount, graph.size() + 1)

      TS_ASSERT_EQUALS(arcCount, graph.sizeArcs() + 3)

      TS_ASSERT(!graph.existsArc(id3, id5))
      TS_ASSERT(!graph.existsArc(id4, id5))
      TS_ASSERT(!graph.existsArc(id5, id2))
    }

    GUM_ACTIVE_TEST(AddDelArcs_2) {
      gum::DAG graph = buildGraph();

      TS_ASSERT(graph.existsArc(id1, id4))
      TS_ASSERT(graph.existsArc(id4, id5))
      TS_ASSERT(graph.existsArc(id2, id5))

      gum::Size nodeCount = graph.size();
      gum::Size arcCount  = graph.sizeArcs();

      TS_GUM_ASSERT_THROWS_NOTHING(graph.eraseArc(gum::Arc(id2, id5)))

      TS_ASSERT_EQUALS(nodeCount, graph.size())
      TS_ASSERT_EQUALS(arcCount, graph.sizeArcs() + 1)

      TS_ASSERT(!graph.existsArc(id2, id5))
    }

    GUM_ACTIVE_TEST(GetNodes) {
      gum::DAG graph = buildGraph();

      gum::NodeSet nodelist = graph.asNodeSet();
      TS_ASSERT_EQUALS(nodelist.size(), graph.size())
      gum::Size nodeCount = graph.size();

      for (const auto node: nodelist)
        graph.eraseNode(node);

      TS_ASSERT(graph.empty())

      TS_ASSERT_EQUALS(nodeCount, nodelist.size())
    }

    GUM_ACTIVE_TEST(GetArcs) {
      gum::DAG graph = buildGraph();

      gum::ArcSet arclist = graph.arcs();
      TS_ASSERT_EQUALS(arclist.size(), graph.sizeArcs())
      gum::Size arcCount = graph.sizeArcs();

      for (const auto& arc: arclist) {
        graph.eraseArc(arc);
      }

      TS_ASSERT(graph.emptyArcs())

      TS_ASSERT_EQUALS(arcCount, arclist.size())
    }

    GUM_ACTIVE_TEST(NodeListMapNodes) {
      gum::DAG graph = buildGraph();

      auto list = graph.listMapNodes(&simpleDoubleFunction);
      TS_ASSERT_EQUALS(list.size(), graph.size())

      gum::Size s = 0;

      for (auto iter = list.begin(); iter != list.end(); ++iter)
        s += *iter;

      TS_ASSERT_EQUALS(s, 2 * (id1 + id2 + id3 + id4 + id5))
    }

    GUM_ACTIVE_TEST(TwistedNodeListMapNodes) {
      gum::DAG graph = buildGraph();

      gum::List< gum::Size > list;
      TS_ASSERT_THROWS_ANYTHING(list = graph.listMapNodes(&twistedMapFunction))

      TS_ASSERT_EQUALS(list.size(), (gum::Size)0)
    }

    GUM_ACTIVE_TEST(HashMapNodes) {
      gum::DAG graph = buildGraph();

      auto hashmap = graph.nodesPropertyFromFunction(&simpleDoubleFunction);
      TS_ASSERT_EQUALS(hashmap.size(), graph.size())

      gum::Size sk = 0;
      gum::Size sv = 0;

      for (const auto& elt: hashmap) {
        sk += elt.first;
        sv += elt.second;
      }

      TS_ASSERT_EQUALS(sk * 2, sv)
    }

    GUM_ACTIVE_TEST(TwistedHashMapNodes) {
      gum::DAG graph = buildGraph();

      gum::NodeProperty< gum::Size > hashmap;
      TS_ASSERT_THROWS_ANYTHING(hashmap = graph.nodesPropertyFromFunction(&twistedMapFunction))

      TS_ASSERT_EQUALS(hashmap.size(), (gum::Size)0)
    }

    GUM_ACTIVE_TEST(ListMapArcs) {
      gum::DAG graph = buildGraph();

      gum::List< gum::Size > list = graph.listMapArcs(&simpleArcMapFunction);
      TS_ASSERT_EQUALS(list.size(), graph.sizeArcs())

      gum::Size s = 0;

      for (auto iter = list.begin(); iter != list.end(); ++iter)
        s += *iter;

      TS_ASSERT_EQUALS(s, (gum::Size)(0 + 0 + 2 + 3 + 1 + 4 + 2 + 3 + 4 + 4 + 3 + 1))
    }

    GUM_ACTIVE_TEST(HashMapArcs) {
      gum::DAG graph = buildGraph();

      auto hashmap = graph.arcsProperty(&simpleArcMapFunction);
      TS_ASSERT_EQUALS(hashmap.size(), graph.sizeArcs())

      gum::Size sk = 0;
      gum::Size sv = 0;

      for (const auto& elt: hashmap) {
        sk += elt.first.head() + elt.first.tail();
        sv += elt.second;
      }

      TS_ASSERT_EQUALS(sk, sv)
    }

    GUM_ACTIVE_TEST(CopyOperator) {
      gum::DAG graph = buildGraph();

      gum::DAG g2 = buildGraph();
      g2.addNode();

      gum::DAG g3;

      g2 = g3 = graph;

      TS_ASSERT_EQUALS(g2, graph)
      TS_ASSERT_EQUALS(g3, graph)

      g2.clear();
      g3.clearArcs();

      TS_ASSERT_DIFFERS(g2, graph)
      TS_ASSERT_DIFFERS(g3, graph)
    }

    GUM_ACTIVE_TEST(Family) {
      // The graph used for the tests:
      //          0   1_          0 -> 2
      //         / \ / /          0 -> 3
      //        2   3 /           2 -> 4
      //         \ / /            3 -> 4
      //          4_/             1 -> 3
      //                          1 -> 4
      gum::DAG graph = buildGraph();
      TS_ASSERT_EQUALS(graph.family(0), gum::NodeSet({0}))
      TS_ASSERT_EQUALS(graph.family(4), gum::NodeSet({1, 2, 3, 4}))
      auto seq=graph.topologicalOrder();
    }

    GUM_ACTIVE_TEST(MonoCycle) {
      gum::DAG graph;
      auto     x = graph.addNode();
      TS_ASSERT_THROWS(graph.addArc(x, x), const gum::Exception&)
    }

  };
}   // namespace gum_tests
