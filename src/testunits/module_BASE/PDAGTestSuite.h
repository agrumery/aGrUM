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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


#include <iostream>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/graphs/graphElements.h>
#include <agrum/base/graphs/PDAG.h>

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

  class PDAGTestSuite: public CxxTest::TestSuite {
    private:
    static gum::Size simpleDoubleFunction(const gum::NodeId& aNodeId) { return aNodeId * 2; }

    static gum::Size simpleArcMapFunction(const gum::Arc& anArc) {
      return anArc.tail() + anArc.head();
    }

    static gum::Size simpleEdgeMapFunction(const gum::Edge& anEdge) {
      return anEdge.first() + anEdge.second();
    }

    static gum::Size twistedMapFunction(const gum::NodeId& aNode) { throw aNode; }

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

    GUM_ACTIVE_TEST(Constructor1) {
      gum::PDAG* graph = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING((graph = new gum::PDAG()))
      TS_GUM_ASSERT_THROWS_NOTHING((delete (graph)))
    }

    GUM_ACTIVE_TEST(Insert1) {
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

    GUM_ACTIVE_TEST(Insert2) {
      gum::PDAG graph;
      buildPDAG(graph);

      TS_ASSERT_THROWS(graph.addArc(id4, id1), const gum::InvalidDirectedCycle&)
      // TS_ASSERT_THROWS(graph.addArc(id8, id1), const gum::InvalidPartiallyDirectedCycle&)
      // TS_ASSERT_THROWS(graph.addEdge(id0, id4), const gum::InvalidPartiallyDirectedCycle&)

      TS_GUM_ASSERT_THROWS_NOTHING(graph.addEdge(id8, id1))
      TS_GUM_ASSERT_THROWS_NOTHING(graph.addEdge(id0, id4))
      TS_GUM_ASSERT_THROWS_NOTHING(graph.addEdge(id3, id6))

      TS_ASSERT_THROWS(graph.addArc(1000, id1), const gum::InvalidNode&)
      TS_ASSERT_THROWS(graph.addArc(id1, 1000), const gum::InvalidNode&)
    }

    GUM_ACTIVE_TEST(CopyConstructor) {
      gum::PDAG graph;
      buildPDAG(graph);

      gum::PDAG* copy = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(copy = new gum::PDAG(graph))
      TS_ASSERT_EQUALS(graph, *copy)
      delete copy;

      TS_GUM_ASSERT_THROWS_NOTHING(gum::PDAG copy2 = graph)
      TS_GUM_ASSERT_THROWS_NOTHING(gum::PDAG copy3(graph))
    }

    GUM_ACTIVE_TEST(UndiGraphCopyConstructor) {
      gum::UndiGraph ug;
      ug.addNodes(10);
      ug.addEdge(1, 2);
      ug.addEdge(1, 3);
      ug.addEdge(3, 2);
      ug.addEdge(4, 5);

      gum::PDAG g(ug);
      TS_ASSERT_EQUALS(g.size(), 10u)
      TS_ASSERT_EQUALS(g.sizeArcs(), 0u)
      TS_ASSERT_EQUALS(g.sizeEdges(), 4u)
    }

    GUM_ACTIVE_TEST(DAGCopyConstructor) {
      gum::DAG dag;
      dag.addNodes(10);
      dag.addArc(1, 2);
      dag.addArc(1, 3);
      dag.addArc(3, 2);
      dag.addArc(4, 5);

      gum::PDAG g(dag);
      TS_ASSERT_EQUALS(g.size(), 10u)
      TS_ASSERT_EQUALS(g.sizeArcs(), 4u)
      TS_ASSERT_EQUALS(g.sizeEdges(), 0u)
    }

    GUM_ACTIVE_TEST(MixedGraphCopyConstructor) {
      gum::MixedGraph dag;
      dag.addNodes(10);
      dag.addArc(1, 2);
      dag.addArc(1, 3);
      dag.addEdge(3, 2);
      dag.addEdge(4, 5);

      gum::PDAG g(dag);
      TS_ASSERT_EQUALS(g.size(), 10u)
      TS_ASSERT_EQUALS(g.sizeArcs(), 2u)
      TS_ASSERT_EQUALS(g.sizeEdges(), 2u)
    }

    GUM_ACTIVE_TEST(EmptyNodes) {
      gum::PDAG graph;

      TS_ASSERT(graph.empty())
      buildPDAG(graph);
      TS_ASSERT(!graph.empty())
    }

    GUM_ACTIVE_TEST(EmptyArcs) {
      gum::PDAG graph;
      TS_ASSERT(graph.emptyArcs())
      buildPDAG(graph);
      TS_ASSERT(!graph.emptyArcs())
    }

    GUM_ACTIVE_TEST(EmptyEdges) {
      gum::PDAG graph;
      TS_ASSERT(graph.emptyEdges())
      buildPDAG(graph);
      TS_ASSERT(!graph.emptyEdges())
    }

    GUM_ACTIVE_TEST(EmptyEdgesMove) {
      gum::PDAG graph;
      TS_ASSERT(graph.emptyEdges())
      buildPDAG(graph);
      TS_ASSERT(!graph.emptyEdges())
    }

    GUM_ACTIVE_TEST(ClearNodes) {
      gum::PDAG graph;
      buildPDAG(graph);
      TS_ASSERT(!graph.empty())
      TS_GUM_ASSERT_THROWS_NOTHING(graph.clear())
      TS_ASSERT(graph.empty() && graph.emptyArcs() && graph.emptyEdges())
    }

    GUM_ACTIVE_TEST(ClearArcs) {
      gum::PDAG graph;
      buildPDAG(graph);
      TS_ASSERT(!graph.emptyArcs())
      TS_GUM_ASSERT_THROWS_NOTHING(graph.clearArcs())
      TS_ASSERT(graph.emptyArcs())
      TS_ASSERT(!graph.empty())
    }

    GUM_ACTIVE_TEST(AddDelNodes_2) {
      gum::PDAG graph;
      buildPDAG(graph);

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
      gum::Size edgeCount = graph.sizeEdges();

      TS_GUM_ASSERT_THROWS_NOTHING(graph.eraseNode(id0))

      TS_ASSERT_EQUALS(nodeCount, graph.size() + 1)
      TS_ASSERT_EQUALS(arcCount, graph.sizeArcs() + 2)
      TS_ASSERT_EQUALS(edgeCount, graph.sizeEdges())

      TS_ASSERT(!graph.exists(id0))
      TS_ASSERT(!graph.existsArc(id0, id2))
      TS_ASSERT(!graph.existsArc(id3, id0))

      TS_GUM_ASSERT_THROWS_NOTHING(graph.eraseNode(id6))

      TS_ASSERT_EQUALS(nodeCount, graph.size() + 2)
      TS_ASSERT_EQUALS(arcCount, graph.sizeArcs() + 4)
      TS_ASSERT_EQUALS(edgeCount, graph.sizeEdges() + 1)
    }

    GUM_ACTIVE_TEST(RemoveNodes_1) {
      gum::PDAG graph;
      buildPDAG(graph);

      gum::Size nodeCount = graph.size();
      gum::Size arcCount  = graph.sizeArcs();
      gum::Size edgeCount = graph.sizeEdges();

      TS_ASSERT_EQUALS(nodeCount, (gum::Size)9)
      TS_ASSERT_EQUALS(arcCount, (gum::Size)8)
      TS_ASSERT_EQUALS(edgeCount, (gum::Size)2)

      for (int i = 0; i < 10; i++) {
        TS_GUM_ASSERT_THROWS_NOTHING(graph.eraseNode(id4))
      }

      TS_ASSERT_EQUALS(nodeCount, graph.size() + 1)

      TS_ASSERT_EQUALS(arcCount, graph.sizeArcs() + 3)
      TS_ASSERT_EQUALS(edgeCount, graph.sizeEdges())

      TS_ASSERT(!graph.existsArc(id2, id4))
      TS_ASSERT(!graph.existsArc(id3, id4))
      TS_ASSERT(!graph.existsArc(id4, id1))
    }

    GUM_ACTIVE_TEST(AddDelArcs_2) {
      gum::PDAG graph;
      buildPDAG(graph);

      TS_ASSERT(graph.existsArc(id0, id3))
      TS_ASSERT(graph.existsArc(id3, id4))
      TS_ASSERT(graph.existsArc(id1, id4))

      gum::Size nodeCount = graph.size();
      gum::Size arcCount  = graph.sizeArcs();
      gum::Size edgeCount = graph.sizeEdges();

      TS_GUM_ASSERT_THROWS_NOTHING(graph.eraseArc(gum::Arc(id1, id4)))

      TS_ASSERT_EQUALS(nodeCount, graph.size())
      TS_ASSERT_EQUALS(arcCount, graph.sizeArcs() + 1)
      TS_ASSERT_EQUALS(edgeCount, graph.sizeEdges())

      TS_ASSERT(!graph.existsArc(id1, id4))
    }

    GUM_ACTIVE_TEST(DelEdge) {
      gum::PDAG graph;
      buildPDAG(graph);

      TS_ASSERT(graph.existsEdge(id3, id5))
      TS_ASSERT(graph.existsEdge(id5, id6))

      gum::Size nodeCount = graph.size();
      gum::Size arcCount  = graph.sizeArcs();
      gum::Size edgeCount = graph.sizeEdges();

      TS_GUM_ASSERT_THROWS_NOTHING(graph.eraseEdge(gum::Edge(id3, id5)))

      TS_ASSERT_EQUALS(nodeCount, graph.size())
      TS_ASSERT_EQUALS(arcCount, graph.sizeArcs())
      TS_ASSERT_EQUALS(edgeCount, graph.sizeEdges() + 1)

      TS_ASSERT(!graph.existsEdge(id3, id5))
    }

    GUM_ACTIVE_TEST(GetNodes) {
      gum::PDAG graph;
      buildPDAG(graph);

      gum::NodeSet nodelist = graph.asNodeSet();
      TS_ASSERT_EQUALS(nodelist.size(), graph.size())
      gum::Size nodeCount = graph.size();

      for (const auto node: nodelist)
        graph.eraseNode(node);

      TS_ASSERT(graph.empty())

      TS_ASSERT_EQUALS(nodeCount, nodelist.size())
    }

    GUM_ACTIVE_TEST(GetArcs) {
      gum::PDAG graph;
      buildPDAG(graph);

      gum::ArcSet arclist = graph.arcs();
      TS_ASSERT_EQUALS(arclist.size(), graph.sizeArcs())
      gum::Size arcCount = graph.sizeArcs();

      for (const auto& arc: arclist) {
        graph.eraseArc(arc);
      }

      TS_ASSERT(graph.emptyArcs())

      TS_ASSERT_EQUALS(arcCount, arclist.size())
    }

    GUM_ACTIVE_TEST(GetEdges) {
      gum::PDAG graph;
      buildPDAG(graph);

      gum::EdgeSet edgelist = graph.edges();
      TS_ASSERT_EQUALS(edgelist.size(), graph.sizeEdges())
      gum::Size edgeCount = graph.sizeEdges();

      for (const auto& edge: edgelist) {
        graph.eraseEdge(edge);
      }

      TS_ASSERT(graph.emptyEdges())

      TS_ASSERT_EQUALS(edgeCount, edgelist.size())
    }

    GUM_ACTIVE_TEST(NodeListMapNodes) {
      gum::PDAG graph;
      buildPDAG(graph);

      auto list = graph.listMapNodes(&simpleDoubleFunction);
      TS_ASSERT_EQUALS(list.size(), graph.size())

      gum::Size s = 0;

      for (const auto elt: list)
        s += elt;

      TS_ASSERT_EQUALS(s, 2 * (id0 + id1 + id2 + id3 + id4 + id5 + id6 + id7 + id8))
    }

    GUM_ACTIVE_TEST(TwistedNodeListMapNodes) {
      gum::PDAG graph;
      buildPDAG(graph);

      gum::List< gum::Size > list;
      TS_ASSERT_THROWS_ANYTHING(list = graph.listMapNodes(&twistedMapFunction))

      TS_ASSERT_EQUALS(list.size(), (gum::Size)0)
    }

    GUM_ACTIVE_TEST(HashMapNodes) {
      gum::PDAG graph;
      buildPDAG(graph);

      auto hashmap = graph.nodesPropertyFromFunction(&simpleDoubleFunction);
      TS_ASSERT_EQUALS(hashmap.size(), graph.size())

      gum::Size sk = 0;
      gum::Size sv = 0;

      for (const auto& [k, v]: hashmap) {
        sk += k;
        sv += v;
      }

      TS_ASSERT_EQUALS(sk * 2, sv)
    }

    GUM_ACTIVE_TEST(TwistedHashMapNodes) {
      gum::PDAG graph;
      buildPDAG(graph);

      gum::NodeProperty< gum::Size > hashmap;
      TS_ASSERT_THROWS_ANYTHING(hashmap = graph.nodesPropertyFromFunction(&twistedMapFunction))

      TS_ASSERT_EQUALS(hashmap.size(), (gum::Size)0)
    }

    GUM_ACTIVE_TEST(ListMapArcs) {
      gum::PDAG graph;
      buildPDAG(graph);

      gum::List< gum::Size > list = graph.listMapArcs(&simpleArcMapFunction);
      TS_ASSERT_EQUALS(list.size(), graph.sizeArcs())

      gum::Size s = 0;

      for (const auto elt: list)
        s += elt;

      TS_ASSERT_EQUALS(s,
                       (gum::Size)(0 + 2 + 0 + 3 + 1 + 6 + 1 + 4 + +1 + 3 + 2 + 4 + 7 + 6 + 6 + 8))
    }

    GUM_ACTIVE_TEST(HashMapArcs) {
      gum::PDAG graph;
      buildPDAG(graph);

      auto hashmap = graph.arcsProperty(&simpleArcMapFunction);
      TS_ASSERT_EQUALS(hashmap.size(), graph.sizeArcs())

      gum::Size sk = 0;
      gum::Size sv = 0;

      for (const auto& [k, v]: hashmap) {
        sk += k.head() + k.tail();
        sv += v;
      }

      TS_ASSERT_EQUALS(sk, sv)
    }

    GUM_ACTIVE_TEST(ListMapEdges) {
      gum::PDAG graph;
      buildPDAG(graph);

      gum::List< gum::Size > list = graph.listMapEdges(&simpleEdgeMapFunction);
      TS_ASSERT_EQUALS(list.size(), graph.sizeEdges())

      gum::Size s = 0;

      for (const auto& elt: list)
        s += elt;

      TS_ASSERT_EQUALS(s, (gum::Size)(3 + 5 + 5 + 6))
    }

    GUM_ACTIVE_TEST(HashMapEdges) {
      gum::PDAG graph;
      buildPDAG(graph);

      auto hashmap = graph.edgesProperty(&simpleEdgeMapFunction);
      TS_ASSERT_EQUALS(hashmap.size(), graph.sizeEdges())

      gum::Size sk = 0;
      gum::Size sv = 0;

      for (const auto& [k, v]: hashmap) {
        sk += k.first() + k.second();
        sv += v;
      }

      TS_ASSERT_EQUALS(sk, sv)
    }

    GUM_ACTIVE_TEST(CopyOperator) {
      gum::PDAG graph;
      buildPDAG(graph);

      gum::PDAG g2;
      buildPDAG(g2);
      g2.addNode();

      gum::PDAG g3;
      gum::PDAG g4;

      g2 = g3 = g4 = graph;

      TS_ASSERT_EQUALS(g2, graph)
      TS_ASSERT_EQUALS(g3, graph)
      TS_ASSERT_EQUALS(g4, graph)

      g2.clear();
      g3.clearArcs();
      g4.clearEdges();

      TS_ASSERT_DIFFERS(g2, graph)
      TS_ASSERT_DIFFERS(g3, graph)
      TS_ASSERT_DIFFERS(g4, graph)
    }

    GUM_ACTIVE_TEST(Family) {
      gum::PDAG graph;
      buildPDAG(graph);
      TS_ASSERT_EQUALS(graph.family(0), gum::NodeSet({0}))
      TS_ASSERT_EQUALS(graph.family(4), gum::NodeSet({1, 2, 3, 4}))
    }

    GUM_ACTIVE_TEST(MonoCycle) {
      gum::PDAG graph;
      auto      x = graph.addNode();
      TS_ASSERT_THROWS(graph.addArc(x, x), const gum::Exception&)
    }

    GUM_ACTIVE_TEST(cSeparationFromKennethLee) {
      {
        gum::PDAG p;
        p.addNodes(10);
        p.addArc(0, 1);
        p.addArc(1, 2);
        TS_GUM_ASSERT_THROWS_NOTHING(p.cSeparation(0, 2, gum::NodeSet({1})))
      }
      {
        gum::PDAG p;
        p.addNodes(10);
        p.addEdge(0, 1);
        TS_ASSERT(!p.cSeparation(0, 1, gum::NodeSet({2})))
      }
      {
        gum::PDAG p;
        p.addNodes(4);
        p.addEdge(0, 1);
        p.addArc(1, 2);
        p.addArc(3, 2);
        TS_ASSERT(!p.cSeparation(0, 2, gum::NodeSet()))
        TS_ASSERT(!p.cSeparation(3, 2, gum::NodeSet()))
        TS_ASSERT(p.cSeparation(1, 3, gum::NodeSet()))
        TS_ASSERT(p.cSeparation(0, 3, gum::NodeSet()))
        TS_ASSERT(!p.cSeparation(1, 3, gum::NodeSet({2})))
        TS_ASSERT(!p.cSeparation(0, 3, gum::NodeSet({2})))
        TS_ASSERT(p.cSeparation(0, 3, gum::NodeSet({1, 2})))
        TS_ASSERT(!p.cSeparation(1, 3, gum::NodeSet({0, 2})))
        TS_ASSERT(p.cSeparation(1, 3, gum::NodeSet({0})))
      }
    }
  };
}   // namespace gum_tests
