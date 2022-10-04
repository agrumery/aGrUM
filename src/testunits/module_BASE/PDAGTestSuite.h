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
#include <iostream>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#include <agrum/tools/graphs/PDAG.h>
#include <agrum/tools/graphs/graphElements.h>
#include <agrum/tools/graphs/graphElements.h>

// The graph used for the tests:
//  1 -> 3
//  1 -> 4
//  2 -> 4
//  2 -> 5
//  3 -> 5
//  4 -> 5
//  4 -- 6 -- 7
//  7 -> 9
//  8 -> 7
namespace gum_tests {

  class DAGTestSuite: public CxxTest::TestSuite {
    private:
    static gum::Size simpleDoubleFunction(const gum::NodeId& aNodeId) { return aNodeId * 2; }

    static gum::Size simpleArcMapFunction(const gum::Arc& anArc) {
      return anArc.tail() + anArc.head();
    }

    static gum::Size twistedMapFunction(const gum::NodeId& aNode) { throw(aNode); }

    void buildPDAG(gum::PDAG& g) {      
      id0 = g.addNode();
      id1 = g.addNode();
      id2 = g.addNode();
      id3 = g.addNode();
      id4 = g.addNode();
      id5 = g.addNode();
      id6 = g.addNode();
      id7 = g.addNode();
      id8 = g.addNode();

      g.addArc(id0, id2);
      g.addArc(id0, id3);
      g.addArc(id1, id3);
      g.addArc(id1, id4);
      g.addArc(id2, id4);
      g.addArc(id3, id4);

      g.addEdge(id3, id5);
      g.addEdge(id5, id6);

      g.addArc(id6, id8);
      g.addArc(id7, id6);
    }

    gum::PDAG buildPDAGMove() {
      gum::PDAG g;
      id0 = g.addNode();
      id1 = g.addNode();
      id2 = g.addNode();
      id3 = g.addNode();
      id4 = g.addNode();
      id5 = g.addNode();
      id6 = g.addNode();
      id7 = g.addNode();
      id8 = g.addNode();

      g.addArc(id0, id2);
      g.addArc(id0, id3);
      g.addArc(id1, id3);
      g.addArc(id1, id4);
      g.addArc(id2, id4);
      g.addArc(id3, id4);

      g.addEdge(id3, id5);
      g.addEdge(id5, id6);

      g.addArc(id6, id8);
      g.addArc(id7, id6);

      return g;
    }

    public:
    gum::NodeId id0;
    gum::NodeId id1;
    gum::NodeId id2;
    gum::NodeId id3;
    gum::NodeId id4;
    gum::NodeId id5;
    gum::NodeId id6;
    gum::NodeId id7;
    gum::NodeId id8;

    void testConstructor1() {
      gum::PDAG* graph = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING((graph = new gum::PDAG()))
      TS_GUM_ASSERT_THROWS_NOTHING((delete (graph)))
    }

    void testInsert1() {
      gum::PDAG graph;

      TS_GUM_ASSERT_THROWS_NOTHING(id0 = graph.addNode())
      TS_GUM_ASSERT_THROWS_NOTHING(id1 = graph.addNode())
      TS_GUM_ASSERT_THROWS_NOTHING(id2 = graph.addNode())
      TS_GUM_ASSERT_THROWS_NOTHING(id3 = graph.addNode())
      TS_GUM_ASSERT_THROWS_NOTHING(id4 = graph.addNode())
      TS_GUM_ASSERT_THROWS_NOTHING(id5 = graph.addNode())
      TS_GUM_ASSERT_THROWS_NOTHING(id6 = graph.addNode())
      TS_GUM_ASSERT_THROWS_NOTHING(id7 = graph.addNode())
      TS_GUM_ASSERT_THROWS_NOTHING(id8 = graph.addNode())

      TS_GUM_ASSERT_THROWS_NOTHING(graph.addArc(id0, id2))
      TS_GUM_ASSERT_THROWS_NOTHING(graph.addArc(id2, id4))
      TS_GUM_ASSERT_THROWS_NOTHING(graph.addArc(id1, id3))
      TS_GUM_ASSERT_THROWS_NOTHING(graph.addArc(id0, id3))
      TS_GUM_ASSERT_THROWS_NOTHING(graph.addArc(id3, id4))
      TS_GUM_ASSERT_THROWS_NOTHING(graph.addArc(id1, id4))
      TS_GUM_ASSERT_THROWS_NOTHING(graph.addEdge(id3, id5))
      TS_GUM_ASSERT_THROWS_NOTHING(graph.addEdge(id5, id6))
      TS_GUM_ASSERT_THROWS_NOTHING(graph.addArc(id7, id6))
      TS_GUM_ASSERT_THROWS_NOTHING(graph.addArc(id6, id8))
    }

    void testInsert2() {
      gum::PDAG graph;buildPDAG(graph);

      TS_ASSERT_THROWS(graph.addArc(id4, id1), const gum::InvalidPartiallyDirectedCycle&)
      TS_ASSERT_THROWS(graph.addArc(id8, id1), const gum::InvalidPartiallyDirectedCycle&)
      TS_ASSERT_THROWS(graph.addEdge(id0, id4), const gum::InvalidPartiallyDirectedCycle&)

      TS_GUM_ASSERT_THROWS_NOTHING(graph.addEdge(id3, id6))

      TS_ASSERT_THROWS(graph.addArc(1000, id1), const gum::InvalidNode&)
      TS_ASSERT_THROWS(graph.addArc(id1, 1000), const gum::InvalidNode&)
    }

    void testCopyConstructor() {
      gum::PDAG graph;buildPDAG(graph);

      gum::PDAG* copy = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(copy = new gum::PDAG(graph))
      TS_ASSERT_EQUALS(graph, *copy)
      delete (copy);

      TS_GUM_ASSERT_THROWS_NOTHING(gum::PDAG copy2 = graph)
      TS_GUM_ASSERT_THROWS_NOTHING(gum::PDAG copy3(graph))
    }

    void testEmptyNodes() {
      gum::PDAG graph;

      TS_ASSERT(graph.empty())
      buildPDAG(graph);
      TS_ASSERT(!graph.empty())
    }

    void testEmptyArcs() {
      gum::PDAG graph;
      TS_ASSERT(graph.emptyArcs())
      buildPDAG(graph);
      TS_ASSERT(!graph.emptyArcs())
    }

    void testEmptyEdges() {
      gum::PDAG graph;
      TS_ASSERT(graph.emptyEdges())
      buildPDAG(graph);
      TS_ASSERT(!graph.emptyEdges())
    }

    void testEmptyEdgesMove() {
      gum::PDAG graph;
      TS_ASSERT(graph.emptyEdges())
      graph=buildPDAGMove();
      TS_ASSERT(!graph.emptyEdges())
    }

    void testClearNodes() {
      gum::PDAG graph;buildPDAG(graph);
      TS_ASSERT(!graph.empty())
      TS_GUM_ASSERT_THROWS_NOTHING(graph.clear())
      TS_ASSERT(graph.empty() && graph.emptyArcs() && graph.emptyEdges())
    }

    void /*test*/ ClearArcs() {
      gum::PDAG graph;buildPDAG(graph);
      TS_ASSERT(!graph.emptyArcs())
      TS_GUM_ASSERT_THROWS_NOTHING(graph.clearArcs())
      TS_ASSERT(graph.emptyArcs())
      TS_ASSERT(!graph.empty())
    }

    void /*test*/ AddDelNodes_2() {
      gum::PDAG graph;buildPDAG(graph);

      TS_ASSERT(graph.exists(id0))
      TS_ASSERT(graph.exists(id1))
      TS_ASSERT(graph.exists(id2))
      TS_ASSERT(graph.exists(id3))
      TS_ASSERT(graph.exists(id4))
      TS_ASSERT(!graph.exists(id5 + id4 + id3 + id2 + id1))

      TS_ASSERT(graph.existsArc(id2, id4))
      TS_ASSERT(!graph.existsArc(id4, id2))
      TS_ASSERT(!graph.existsArc(id0, id0))

      gum::Size nodeCount = graph.size();
      gum::Size arcCount  = graph.sizeArcs();

      TS_GUM_ASSERT_THROWS_NOTHING(graph.eraseNode(id0))

      TS_ASSERT_EQUALS(nodeCount, graph.size() + 1)
      TS_ASSERT_EQUALS(arcCount, graph.sizeArcs() + 2)

      TS_ASSERT(!graph.exists(id1))
      TS_ASSERT(!graph.existsArc(id1, id3))
      TS_ASSERT(!graph.existsArc(id4, id1))
    }

    void /*test*/ RemoveNodes_1() {
      gum::PDAG graph;buildPDAG(graph);

      gum::Size nodeCount = graph.size();
      gum::Size arcCount  = graph.sizeArcs();

      TS_ASSERT_EQUALS(nodeCount, (gum::Size)5)
      TS_ASSERT_EQUALS(arcCount, (gum::Size)6)

      for (int i = 0; i < 10; i++) {
        TS_GUM_ASSERT_THROWS_NOTHING(graph.eraseNode(id4))
      }

      TS_ASSERT_EQUALS(nodeCount, graph.size() + 1)

      TS_ASSERT_EQUALS(arcCount, graph.sizeArcs() + 3)

      TS_ASSERT(!graph.existsArc(id2, id4))
      TS_ASSERT(!graph.existsArc(id3, id4))
      TS_ASSERT(!graph.existsArc(id4, id1))
    }

    void /*test*/ AddDelArcs_2() {
      gum::PDAG graph;buildPDAG(graph);

      TS_ASSERT(graph.existsArc(id0, id3))
      TS_ASSERT(graph.existsArc(id3, id4))
      TS_ASSERT(graph.existsArc(id1, id4))

      gum::Size nodeCount = graph.size();
      gum::Size arcCount  = graph.sizeArcs();

      TS_GUM_ASSERT_THROWS_NOTHING(graph.eraseArc(gum::Arc(id1, id4)))

      TS_ASSERT_EQUALS(nodeCount, graph.size())
      TS_ASSERT_EQUALS(arcCount, graph.sizeArcs() + 1)

      TS_ASSERT(!graph.existsArc(id1, id4))
    }

    void /*test*/ GetNodes() {
      gum::PDAG graph;buildPDAG(graph);

      gum::NodeSet nodelist = graph.asNodeSet();
      TS_ASSERT_EQUALS(nodelist.size(), graph.size())
      gum::Size nodeCount = graph.size();

      for (const auto node: nodelist)
        graph.eraseNode(node);

      TS_ASSERT(graph.empty())

      TS_ASSERT_EQUALS(nodeCount, nodelist.size())
    }

    void /*test*/ GetArcs() {
      gum::PDAG graph;buildPDAG(graph);

      gum::ArcSet arclist = graph.arcs();
      TS_ASSERT_EQUALS(arclist.size(), graph.sizeArcs())
      gum::Size arcCount = graph.sizeArcs();

      for (const auto& arc: arclist) {
        graph.eraseArc(arc);
      }

      TS_ASSERT(graph.emptyArcs())

      TS_ASSERT_EQUALS(arcCount, arclist.size())
    }

    void /*test*/ NodeListMapNodes() {
      gum::PDAG graph;buildPDAG(graph);

      auto list = graph.listMapNodes(&simpleDoubleFunction);
      TS_ASSERT_EQUALS(list.size(), graph.size())

      gum::Size s = 0;

      for (auto iter = list.begin(); iter != list.end(); ++iter)
        s += *iter;

      TS_ASSERT_EQUALS(s, 2 * (id0 + id1 + id2 + id3 + id4))
    }

    void /*test*/ TwistedNodeListMapNodes() {
      gum::PDAG graph;buildPDAG(graph);

      gum::List< gum::Size > list;
      TS_ASSERT_THROWS_ANYTHING(list = graph.listMapNodes(&twistedMapFunction))

      TS_ASSERT_EQUALS(list.size(), (gum::Size)0)
    }

    void /*test*/ HashMapNodes() {
      gum::PDAG graph;buildPDAG(graph);

      auto hashmap = graph.nodesProperty(&simpleDoubleFunction);
      TS_ASSERT_EQUALS(hashmap.size(), graph.size())

      gum::Size sk = 0;
      gum::Size sv = 0;

      for (const auto& elt: hashmap) {
        sk += elt.first;
        sv += elt.second;
      }

      TS_ASSERT_EQUALS(sk * 2, sv)
    }

    void /*test*/ TwistedHashMapNodes() {
      gum::PDAG graph;buildPDAG(graph);

      gum::NodeProperty< gum::Size > hashmap;
      TS_ASSERT_THROWS_ANYTHING(hashmap = graph.nodesProperty(&twistedMapFunction))

      TS_ASSERT_EQUALS(hashmap.size(), (gum::Size)0)
    }

    void /*test*/ ListMapArcs() {
      gum::PDAG graph;buildPDAG(graph);

      gum::List< gum::Size > list = graph.listMapArcs(&simpleArcMapFunction);
      TS_ASSERT_EQUALS(list.size(), graph.sizeArcs())

      gum::Size s = 0;

      for (auto iter = list.begin(); iter != list.end(); ++iter)
        s += *iter;

      TS_ASSERT_EQUALS(s, (gum::Size)(0 + 0 + 2 + 3 + 1 + 4 + 2 + 3 + 4 + 4 + 3 + 1))
    }

    void /*test*/ HashMapArcs() {
      gum::PDAG graph;buildPDAG(graph);

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

    void /*test*/ CopyOperator() {
      gum::PDAG graph;buildPDAG(graph);

      gum::PDAG g2; buildPDAG(g2);
      g2.addNode();

      gum::PDAG g3;

      g2 = g3 = graph;

      TS_ASSERT_EQUALS(g2, graph)
      TS_ASSERT_EQUALS(g3, graph)

      g2.clear();
      g3.clearArcs();

      TS_ASSERT_DIFFERS(g2, graph)
      TS_ASSERT_DIFFERS(g3, graph)
    }

    void /*test*/ Family() {
      // The graph used for the tests:
      //          0   1_          0 -> 2
      //         / \ / /          0 -> 3
      //        2   3 /           2 -> 4
      //         \ / /            3 -> 5
      //          4_/             1 -> 3
      //                          1 -> 4
      gum::PDAG graph;buildPDAG(graph);
      TS_ASSERT_EQUALS(graph.family(0), gum::NodeSet({0}))
      TS_ASSERT_EQUALS(graph.family(4), gum::NodeSet({1, 2, 3, 4}))
    }

    void /*test*/ MonoCycle() {
      gum::PDAG graph;
      auto      x = graph.addNode();
      TS_ASSERT_THROWS(graph.addArc(x, x), const gum::Exception&)
    }
  };
}   // namespace gum_tests
