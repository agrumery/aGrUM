/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#pragma once


#include <iostream>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/graphs/graphElements.h>
#include <agrum/base/graphs/PDAG.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  PDAG
#define GUM_CURRENT_MODULE GUMBASE

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

  struct PDAGTestSuite {
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

    static void testConstructor1() {
      gum::PDAG* graph = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING((graph = new gum::PDAG()));
      GUM_CHECK_ASSERT_THROWS_NOTHING((delete (graph)));
    }

    void testInsert1() {
      gum::PDAG graph;

      GUM_CHECK_ASSERT_THROWS_NOTHING(id0 = graph.addNode());
      GUM_CHECK_ASSERT_THROWS_NOTHING(id1 = graph.addNode());
      GUM_CHECK_ASSERT_THROWS_NOTHING(id2 = graph.addNode());
      GUM_CHECK_ASSERT_THROWS_NOTHING(id3 = graph.addNode());
      GUM_CHECK_ASSERT_THROWS_NOTHING(id4 = graph.addNode());
      GUM_CHECK_ASSERT_THROWS_NOTHING(id5 = graph.addNode());
      GUM_CHECK_ASSERT_THROWS_NOTHING(id6 = graph.addNode());
      GUM_CHECK_ASSERT_THROWS_NOTHING(id7 = graph.addNode());
      GUM_CHECK_ASSERT_THROWS_NOTHING(id8 = graph.addNode());

      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addArc(id0, id2));
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addArc(id2, id4));
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addArc(id1, id3));
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addArc(id0, id3));
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addArc(id3, id4));
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addArc(id1, id4));
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addEdge(id3, id5));
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addEdge(id5, id6));
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addArc(id7, id6));
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addArc(id6, id8));
    }

    void testInsert2() {
      gum::PDAG graph;
      buildPDAG(graph);

      CHECK_THROWS_AS(graph.addArc(id4, id1), const gum::InvalidDirectedCycle&);
      CHECK_THROWS_AS(graph.addArc(id8, id1), const gum::InvalidPartiallyDirectedCycle&);
      CHECK_THROWS_AS(graph.addEdge(id0, id4), const gum::InvalidPartiallyDirectedCycle&);
      CHECK_THROWS_AS(graph.addEdge(id8, id1), const gum::InvalidPartiallyDirectedCycle&);

      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addEdge(id8, id4));
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addEdge(id0, id7));
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addEdge(id3, id6));

      auto id9 = graph.addNode();
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addEdge(id8, id9));
      CHECK_THROWS_AS(graph.addArc(id8, id1), const gum::InvalidPartiallyDirectedCycle&);
      CHECK_THROWS_AS(graph.addEdge(id8, id1), const gum::InvalidPartiallyDirectedCycle&);

      CHECK_THROWS_AS(graph.addArc(1000, id1), const gum::InvalidNode&);
      CHECK_THROWS_AS(graph.addArc(id1, 1000), const gum::InvalidNode&);
    }

    void testCopyConstructor() {
      gum::PDAG graph;
      buildPDAG(graph);

      gum::PDAG* copy = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(copy = new gum::PDAG(graph));
      CHECK((graph) == (*copy));
      delete copy;

      GUM_CHECK_ASSERT_THROWS_NOTHING(gum::PDAG copy2 = graph);
      GUM_CHECK_ASSERT_THROWS_NOTHING(gum::PDAG copy3(graph));
    }

    static void testUndiGraphCopyConstructor() {
      gum::UndiGraph ug;
      ug.addNodes(10);
      ug.addEdge(1, 2);
      ug.addEdge(1, 3);
      ug.addEdge(3, 2);
      ug.addEdge(4, 5);

      gum::PDAG g(ug);
      CHECK((g.size()) == (10u));
      CHECK((g.sizeArcs()) == (0u));
      CHECK((g.sizeEdges()) == (4u));
    }

    static void testDAGCopyConstructor() {
      gum::DAG dag;
      dag.addNodes(10);
      dag.addArc(1, 2);
      dag.addArc(1, 3);
      dag.addArc(3, 2);
      dag.addArc(4, 5);

      gum::PDAG g(dag);
      CHECK((g.size()) == (10u));
      CHECK((g.sizeArcs()) == (4u));
      CHECK((g.sizeEdges()) == (0u));
    }

    static void testMixedGraphCopyConstructor() {
      gum::MixedGraph dag;
      dag.addNodes(10);
      dag.addArc(1, 2);
      dag.addArc(1, 3);
      dag.addEdge(3, 2);
      dag.addEdge(4, 5);

      gum::PDAG g(dag);
      CHECK((g.size()) == (10u));
      CHECK((g.sizeArcs()) == (2u));
      CHECK((g.sizeEdges()) == (2u));
    }

    void testEmptyNodes() {
      gum::PDAG graph;

      CHECK(graph.empty());
      buildPDAG(graph);
      CHECK(!graph.empty());
    }

    void testEmptyArcs() {
      gum::PDAG graph;
      CHECK(graph.emptyArcs());
      buildPDAG(graph);
      CHECK(!graph.emptyArcs());
    }

    void testEmptyEdges() {
      gum::PDAG graph;
      CHECK(graph.emptyEdges());
      buildPDAG(graph);
      CHECK(!graph.emptyEdges());
    }

    void testEmptyEdgesMove() {
      gum::PDAG graph;
      CHECK(graph.emptyEdges());
      buildPDAG(graph);
      CHECK(!graph.emptyEdges());
    }

    void testClearNodes() {
      gum::PDAG graph;
      buildPDAG(graph);
      CHECK(!graph.empty());
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.clear());
      CHECK((graph.empty() && graph.emptyArcs() && graph.emptyEdges()));
    }

    void testClearArcs() {
      gum::PDAG graph;
      buildPDAG(graph);
      CHECK(!graph.emptyArcs());
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.clearArcs());
      CHECK(graph.emptyArcs());
      CHECK(!graph.empty());
    }

    void testAddDelNodes_2() {
      gum::PDAG graph;
      buildPDAG(graph);

      CHECK(graph.exists(id0));
      CHECK(graph.exists(id1));
      CHECK(graph.exists(id2));
      CHECK(graph.exists(id3));
      CHECK(graph.exists(id4));
      CHECK(!graph.exists(id5 + id4 + id3 + id2 + id1));

      CHECK(graph.existsArc(id2, id4));
      CHECK(!graph.existsArc(id4, id2));
      CHECK(!graph.existsArc(id0, id0));

      gum::Size nodeCount = graph.size();
      gum::Size arcCount  = graph.sizeArcs();
      gum::Size edgeCount = graph.sizeEdges();

      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.eraseNode(id0));

      CHECK((nodeCount) == (graph.size() + 1));
      CHECK((arcCount) == (graph.sizeArcs() + 2));
      CHECK((edgeCount) == (graph.sizeEdges()));

      CHECK(!graph.exists(id0));
      CHECK(!graph.existsArc(id0, id2));
      CHECK(!graph.existsArc(id3, id0));

      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.eraseNode(id6));

      CHECK((nodeCount) == (graph.size() + 2));
      CHECK((arcCount) == (graph.sizeArcs() + 4));
      CHECK((edgeCount) == (graph.sizeEdges() + 1));
    }

    void testRemoveNodes_1() {
      gum::PDAG graph;
      buildPDAG(graph);

      gum::Size nodeCount = graph.size();
      gum::Size arcCount  = graph.sizeArcs();
      gum::Size edgeCount = graph.sizeEdges();

      CHECK((nodeCount) == (static_cast< gum::Size >(9)));
      CHECK((arcCount) == (static_cast< gum::Size >(8)));
      CHECK((edgeCount) == (static_cast< gum::Size >(2)));

      for (int i = 0; i < 10; i++) {
        GUM_CHECK_ASSERT_THROWS_NOTHING(graph.eraseNode(id4));
      }

      CHECK((nodeCount) == (graph.size() + 1));

      CHECK((arcCount) == (graph.sizeArcs() + 3));
      CHECK((edgeCount) == (graph.sizeEdges()));

      CHECK(!graph.existsArc(id2, id4));
      CHECK(!graph.existsArc(id3, id4));
      CHECK(!graph.existsArc(id4, id1));
    }

    void testAddDelArcs_2() {
      gum::PDAG graph;
      buildPDAG(graph);

      CHECK(graph.existsArc(id0, id3));
      CHECK(graph.existsArc(id3, id4));
      CHECK(graph.existsArc(id1, id4));

      gum::Size nodeCount = graph.size();
      gum::Size arcCount  = graph.sizeArcs();
      gum::Size edgeCount = graph.sizeEdges();

      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.eraseArc(gum::Arc(id1, id4)));

      CHECK((nodeCount) == (graph.size()));
      CHECK((arcCount) == (graph.sizeArcs() + 1));
      CHECK((edgeCount) == (graph.sizeEdges()));

      CHECK(!graph.existsArc(id1, id4));
    }

    void testDelEdge() {
      gum::PDAG graph;
      buildPDAG(graph);

      CHECK(graph.existsEdge(id3, id5));
      CHECK(graph.existsEdge(id5, id6));

      gum::Size nodeCount = graph.size();
      gum::Size arcCount  = graph.sizeArcs();
      gum::Size edgeCount = graph.sizeEdges();

      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.eraseEdge(gum::Edge(id3, id5)));

      CHECK((nodeCount) == (graph.size()));
      CHECK((arcCount) == (graph.sizeArcs()));
      CHECK((edgeCount) == (graph.sizeEdges() + 1));

      CHECK(!graph.existsEdge(id3, id5));
    }

    void testGetNodes() {
      gum::PDAG graph;
      buildPDAG(graph);

      gum::NodeSet nodelist = graph.asNodeSet();
      CHECK((nodelist.size()) == (graph.size()));
      gum::Size nodeCount = graph.size();

      for (const auto node: nodelist)
        graph.eraseNode(node);

      CHECK(graph.empty());

      CHECK((nodeCount) == (nodelist.size()));
    }

    void testGetArcs() {
      gum::PDAG graph;
      buildPDAG(graph);

      gum::ArcSet arclist = graph.arcs();
      CHECK((arclist.size()) == (graph.sizeArcs()));
      gum::Size arcCount = graph.sizeArcs();

      for (const auto& arc: arclist) {
        graph.eraseArc(arc);
      }

      CHECK(graph.emptyArcs());

      CHECK((arcCount) == (arclist.size()));
    }

    void testGetEdges() {
      gum::PDAG graph;
      buildPDAG(graph);

      gum::EdgeSet edgelist = graph.edges();
      CHECK((edgelist.size()) == (graph.sizeEdges()));
      gum::Size edgeCount = graph.sizeEdges();

      for (const auto& edge: edgelist) {
        graph.eraseEdge(edge);
      }

      CHECK(graph.emptyEdges());

      CHECK((edgeCount) == (edgelist.size()));
    }

    void testNodeListMapNodes() {
      gum::PDAG graph;
      buildPDAG(graph);

      auto list = graph.listMapNodes(&simpleDoubleFunction);
      CHECK((list.size()) == (graph.size()));

      gum::Size s = 0;

      for (const auto elt: list)
        s += elt;

      CHECK((s) == (2 * (id0 + id1 + id2 + id3 + id4 + id5 + id6 + id7 + id8)));
    }

    void testTwistedNodeListMapNodes() {
      gum::PDAG graph;
      buildPDAG(graph);

      gum::List< gum::Size > list;
      CHECK_THROWS(list = graph.listMapNodes(&twistedMapFunction));

      CHECK((list.size()) == (static_cast< gum::Size >(0)));
    }

    void testHashMapNodes() {
      gum::PDAG graph;
      buildPDAG(graph);

      auto hashmap = graph.nodesPropertyFromFunction(&simpleDoubleFunction);
      CHECK((hashmap.size()) == (graph.size()));

      gum::Size sk = 0;
      gum::Size sv = 0;

      for (const auto& [k, v]: hashmap) {
        sk += k;
        sv += v;
      }

      CHECK((sk * 2) == (sv));
    }

    void testTwistedHashMapNodes() {
      gum::PDAG graph;
      buildPDAG(graph);

      gum::NodeProperty< gum::Size > hashmap;
      CHECK_THROWS(hashmap = graph.nodesPropertyFromFunction(&twistedMapFunction));

      CHECK((hashmap.size()) == (static_cast< gum::Size >(0)));
    }

    void testListMapArcs() {
      gum::PDAG graph;
      buildPDAG(graph);

      gum::List< gum::Size > list = graph.listMapArcs(&simpleArcMapFunction);
      CHECK((list.size()) == (graph.sizeArcs()));

      gum::Size s = 0;

      for (const auto elt: list)
        s += elt;

      CHECK((s) == ((gum::Size)(0 + 2 + 0 + 3 + 1 + 6 + 1 + 4 + +1 + 3 + 2 + 4 + 7 + 6 + 6 + 8)));
    }

    void testHashMapArcs() {
      gum::PDAG graph;
      buildPDAG(graph);

      auto hashmap = graph.arcsProperty(&simpleArcMapFunction);
      CHECK((hashmap.size()) == (graph.sizeArcs()));

      gum::Size sk = 0;
      gum::Size sv = 0;

      for (const auto& [k, v]: hashmap) {
        sk += k.head() + k.tail();
        sv += v;
      }

      CHECK((sk) == (sv));
    }

    void testListMapEdges() {
      gum::PDAG graph;
      buildPDAG(graph);

      gum::List< gum::Size > list = graph.listMapEdges(&simpleEdgeMapFunction);
      CHECK((list.size()) == (graph.sizeEdges()));

      gum::Size s = 0;

      for (const auto& elt: list)
        s += elt;

      CHECK((s) == ((gum::Size)(3 + 5 + 5 + 6)));
    }

    void testHashMapEdges() {
      gum::PDAG graph;
      buildPDAG(graph);

      auto hashmap = graph.edgesProperty(&simpleEdgeMapFunction);
      CHECK((hashmap.size()) == (graph.sizeEdges()));

      gum::Size sk = 0;
      gum::Size sv = 0;

      for (const auto& [k, v]: hashmap) {
        sk += k.first() + k.second();
        sv += v;
      }

      CHECK((sk) == (sv));
    }

    void testCopyOperator() {
      gum::PDAG graph;
      buildPDAG(graph);

      gum::PDAG g2;
      buildPDAG(g2);
      g2.addNode();

      gum::PDAG g3;
      gum::PDAG g4;

      g2 = g3 = g4 = graph;

      CHECK((g2) == (graph));
      CHECK((g3) == (graph));
      CHECK((g4) == (graph));

      g2.clear();
      g3.clearArcs();
      g4.clearEdges();

      CHECK((g2) != (graph));
      CHECK((g3) != (graph));
      CHECK((g4) != (graph));
    }

    void testFamily() {
      gum::PDAG graph;
      buildPDAG(graph);
      CHECK((graph.family(0)) == (gum::NodeSet({0})));
      CHECK((graph.family(4)) == (gum::NodeSet({1, 2, 3, 4})));
    }

    static void testMonoCycle() {
      gum::PDAG graph;
      auto      x = graph.addNode();
      CHECK_THROWS_AS(graph.addArc(x, x), const gum::Exception&);
    }

    static void testcSeparationFromKennethLee() {
      {
        gum::PDAG p;
        p.addNodes(10);
        p.addArc(0, 1);
        p.addArc(1, 2);
        GUM_CHECK_ASSERT_THROWS_NOTHING(p.cSeparation(0, 2, gum::NodeSet({1})));
      }
      {
        gum::PDAG p;
        p.addNodes(10);
        p.addEdge(0, 1);
        CHECK(!p.cSeparation(0, 1, gum::NodeSet({2})));
      }
      {
        gum::PDAG p;
        p.addNodes(4);
        p.addEdge(0, 1);
        p.addArc(1, 2);
        p.addArc(3, 2);
        CHECK(!p.cSeparation(0, 2, gum::NodeSet()));
        CHECK(!p.cSeparation(3, 2, gum::NodeSet()));
        CHECK(p.cSeparation(1, 3, gum::NodeSet()));
        CHECK(p.cSeparation(0, 3, gum::NodeSet()));
        CHECK(!p.cSeparation(1, 3, gum::NodeSet({2})));
        CHECK(!p.cSeparation(0, 3, gum::NodeSet({2})));
        CHECK(p.cSeparation(0, 3, gum::NodeSet({1, 2})));
        CHECK(!p.cSeparation(1, 3, gum::NodeSet({0, 2})));
        CHECK(p.cSeparation(1, 3, gum::NodeSet({0})));
      }
    }

    void testMoralGraphFromLouisDerumaux() {
      gum::PDAG graph;
      buildPDAG(graph);
      gum::UndiGraph moral = graph.moralGraph();
      CHECK(moral.existsEdge(id0, id7));
    }

    static void testcSeparationBugFromLouisDerumaux() {
      auto p = gum::PDAG();

      const auto a  = p.addNode();
      const auto b  = p.addNode();
      const auto c  = p.addNode();
      const auto d  = p.addNode();
      const auto e  = p.addNode();
      const auto f  = p.addNode();
      const auto g  = p.addNode();
      const auto h  = p.addNode();
      const auto i  = p.addNode();
      const auto j  = p.addNode();
      const auto k  = p.addNode();
      const auto z1 = p.addNode();
      const auto z2 = p.addNode();
      const auto y  = p.addNode();

      p.addEdge(a, b);
      p.addEdge(a, c);
      p.addEdge(b, d);
      p.addArc(y, z2);
      p.addArc(c, e);
      p.addArc(d, f);
      p.addArc(d, g);
      p.addEdge(z2, z1);
      p.addEdge(z1, e);
      p.addEdge(e, f);
      p.addEdge(g, h);
      p.addArc(e, j);
      p.addArc(f, k);
      p.addArc(g, k);
      p.addEdge(i, j);

      const auto m = p.moralizedAncestralGraph({c, e, f, h});

      CHECK(m.existsEdge(c, d));
      CHECK(m.existsEdge(y, c));
      CHECK(m.existsEdge(y, d));

      CHECK(!p.cSeparation(e, h, {f, c}));
    }
  };

  GUM_TEST_ACTIF(Constructor1)
  GUM_TEST_ACTIF(Insert1)
  GUM_TEST_ACTIF(Insert2)
  GUM_TEST_ACTIF(CopyConstructor)
  GUM_TEST_ACTIF(UndiGraphCopyConstructor)
  GUM_TEST_ACTIF(DAGCopyConstructor)
  GUM_TEST_ACTIF(MixedGraphCopyConstructor)
  GUM_TEST_ACTIF(EmptyNodes)
  GUM_TEST_ACTIF(EmptyArcs)
  GUM_TEST_ACTIF(EmptyEdges)
  GUM_TEST_ACTIF(EmptyEdgesMove)
  GUM_TEST_ACTIF(ClearNodes)
  GUM_TEST_ACTIF(ClearArcs)
  GUM_TEST_ACTIF(AddDelNodes_2)
  GUM_TEST_ACTIF(RemoveNodes_1)
  GUM_TEST_ACTIF(AddDelArcs_2)
  GUM_TEST_ACTIF(DelEdge)
  GUM_TEST_ACTIF(GetNodes)
  GUM_TEST_ACTIF(GetArcs)
  GUM_TEST_ACTIF(GetEdges)
  GUM_TEST_ACTIF(NodeListMapNodes)
  GUM_TEST_ACTIF(TwistedNodeListMapNodes)
  GUM_TEST_ACTIF(HashMapNodes)
  GUM_TEST_ACTIF(TwistedHashMapNodes)
  GUM_TEST_ACTIF(ListMapArcs)
  GUM_TEST_ACTIF(HashMapArcs)
  GUM_TEST_ACTIF(ListMapEdges)
  GUM_TEST_ACTIF(HashMapEdges)
  GUM_TEST_ACTIF(CopyOperator)
  GUM_TEST_ACTIF(Family)
  GUM_TEST_ACTIF(MonoCycle)
  GUM_TEST_ACTIF(cSeparationFromKennethLee)
  GUM_TEST_ACTIF(MoralGraphFromLouisDerumaux)
  GUM_TEST_ACTIF(cSeparationBugFromLouisDerumaux)

}   // namespace gum_tests
