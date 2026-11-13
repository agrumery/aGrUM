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

#include <agrum/base/graphs/chainGraph.h>
#include <agrum/base/graphs/graphElements.h>
#include <agrum/base/graphs/PDAG.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

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
    protected:
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
  };

  GUM_TEST(Constructor1) {
    gum::PDAG* graph = nullptr;
    GUM_CHECK_ASSERT_THROWS_NOTHING((graph = new gum::PDAG()));
    GUM_CHECK_ASSERT_THROWS_NOTHING((delete (graph)));
  }

  GUM_TEST(Insert1) {
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

  GUM_TEST(Insert2) {
    gum::PDAG graph;
    buildPDAG(graph);

    CHECK_THROWS_AS(graph.addArc(id4, id1), const gum::InvalidDirectedCycle&);

    // partially directed cycles are allowed in a PDAG (not in a ChainGraph)
    {
      gum::PDAG g(graph);
      GUM_CHECK_ASSERT_THROWS_NOTHING(g.addArc(id8, id1));
      CHECK(g.existsArc(id8, id1));
    }
    {
      gum::PDAG g(graph);
      GUM_CHECK_ASSERT_THROWS_NOTHING(g.addEdge(id0, id4));
      CHECK(g.existsEdge(id0, id4));
    }
    {
      gum::PDAG g(graph);
      GUM_CHECK_ASSERT_THROWS_NOTHING(g.addEdge(id8, id1));
      CHECK(g.existsEdge(id8, id1));
    }

    GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addEdge(id8, id4));
    GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addEdge(id0, id7));
    GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addEdge(id3, id6));
    CHECK(graph.existsEdge(id8, id4));
    CHECK(graph.existsEdge(id0, id7));
    CHECK(graph.existsEdge(id3, id6));

    auto id9 = graph.addNode();
    GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addEdge(id8, id9));
    CHECK(graph.existsEdge(id8, id9));

    CHECK_THROWS_AS(graph.addArc(1000, id1), const gum::InvalidNode&);
    CHECK_THROWS_AS(graph.addArc(id1, 1000), const gum::InvalidNode&);
  }

  GUM_TEST(PartiallyDirectedCyclesAreAllowed) {
    // h-v, h-w, w->v : the partially directed cycle h-w->v-h is a valid PDAG
    gum::PDAG         g;
    const gum::NodeId h = g.addNode(), v = g.addNode(), w = g.addNode(), t = g.addNode();
    g.addEdge(h, v);
    g.addEdge(h, w);
    GUM_CHECK_ASSERT_THROWS_NOTHING(g.addArc(w, v));
    g.addEdge(v, t);
    CHECK_THROWS_AS(g.addArc(v, w), const gum::InvalidDirectedCycle&);
    GUM_CHECK_ASSERT_THROWS_NOTHING(g.addArc(t, h));
    // ... but not a directed cycle
    gum::PDAG         d;
    const gum::NodeId x = d.addNode(), y = d.addNode(), z = d.addNode();
    d.addArc(x, y);
    d.addArc(y, z);
    CHECK_THROWS_AS(d.addArc(z, x), const gum::InvalidDirectedCycle&);
  }

  GUM_TEST(AtMostOneConnectionPerPair) {
    gum::PDAG         g;
    const gum::NodeId x = g.addNode(), y = g.addNode();
    g.addEdge(x, y);
    CHECK_THROWS_AS(g.addArc(x, y), const gum::InvalidArc&);
    CHECK_THROWS_AS(g.addArc(y, x), const gum::InvalidArc&);
    g.eraseEdge(gum::Edge(x, y));
    g.addArc(x, y);
    CHECK_THROWS_AS(g.addEdge(x, y), const gum::InvalidEdge&);
    CHECK_THROWS_AS(g.addEdge(y, x), const gum::InvalidEdge&);
    CHECK_EQ(g.sizeArcs(), gum::Size(1));
    CHECK_EQ(g.sizeEdges(), gum::Size(0));

    // copy of a mixed graph
    gum::MixedGraph mg;
    mg.addNodes(2);
    mg.addArc(0, 1);
    mg.addEdge(0, 1);
    CHECK_THROWS_AS(gum::PDAG{mg}, const gum::InvalidArc&);
    mg.eraseEdge(gum::Edge(0, 1));
    mg.addArc(1, 0);
    CHECK_THROWS_AS(gum::PDAG{mg}, const gum::InvalidDirectedCycle&);
  }

  GUM_TEST(CopyConstructor) {
    gum::PDAG graph;
    buildPDAG(graph);

    gum::PDAG* copy = nullptr;
    GUM_CHECK_ASSERT_THROWS_NOTHING(copy = new gum::PDAG(graph));
    CHECK_EQ(graph, *copy);
    delete copy;

    GUM_CHECK_ASSERT_THROWS_NOTHING(gum::PDAG copy2 = graph);
    GUM_CHECK_ASSERT_THROWS_NOTHING(gum::PDAG copy3(graph));
  }

  GUM_TEST(UndiGraphCopyConstructor) {
    gum::UndiGraph ug;
    ug.addNodes(10);
    ug.addEdge(1, 2);
    ug.addEdge(1, 3);
    ug.addEdge(3, 2);
    ug.addEdge(4, 5);

    gum::PDAG g(ug);
    CHECK_EQ(g.size(), 10u);
    CHECK_EQ(g.sizeArcs(), 0u);
    CHECK_EQ(g.sizeEdges(), 4u);
  }

  GUM_TEST(DAGCopyConstructor) {
    gum::DAG dag;
    dag.addNodes(10);
    dag.addArc(1, 2);
    dag.addArc(1, 3);
    dag.addArc(3, 2);
    dag.addArc(4, 5);

    gum::PDAG g(dag);
    CHECK_EQ(g.size(), 10u);
    CHECK_EQ(g.sizeArcs(), 4u);
    CHECK_EQ(g.sizeEdges(), 0u);
  }

  GUM_TEST(MixedGraphCopyConstructor) {
    gum::MixedGraph dag;
    dag.addNodes(10);
    dag.addArc(1, 2);
    dag.addArc(1, 3);
    dag.addEdge(3, 2);
    dag.addEdge(4, 5);

    gum::PDAG g(dag);
    CHECK_EQ(g.size(), 10u);
    CHECK_EQ(g.sizeArcs(), 2u);
    CHECK_EQ(g.sizeEdges(), 2u);
  }

  GUM_TEST(EmptyNodes) {
    gum::PDAG graph;

    CHECK(graph.empty());
    buildPDAG(graph);
    CHECK(!graph.empty());
  }

  GUM_TEST(EmptyArcs) {
    gum::PDAG graph;
    CHECK(graph.emptyArcs());
    buildPDAG(graph);
    CHECK(!graph.emptyArcs());
  }

  GUM_TEST(EmptyEdges) {
    gum::PDAG graph;
    CHECK(graph.emptyEdges());
    buildPDAG(graph);
    CHECK(!graph.emptyEdges());
  }

  GUM_TEST(EmptyEdgesMove) {
    gum::PDAG graph;
    CHECK(graph.emptyEdges());
    buildPDAG(graph);
    CHECK(!graph.emptyEdges());
  }

  GUM_TEST(ClearNodes) {
    gum::PDAG graph;
    buildPDAG(graph);
    CHECK(!graph.empty());
    GUM_CHECK_ASSERT_THROWS_NOTHING(graph.clear());
    CHECK((graph.empty() && graph.emptyArcs() && graph.emptyEdges()));
  }

  GUM_TEST(ClearArcs) {
    gum::PDAG graph;
    buildPDAG(graph);
    CHECK(!graph.emptyArcs());
    GUM_CHECK_ASSERT_THROWS_NOTHING(graph.clearArcs());
    CHECK(graph.emptyArcs());
    CHECK(!graph.empty());
  }

  GUM_TEST(AddDelNodes_2) {
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

    CHECK_EQ(nodeCount, graph.size() + 1);
    CHECK_EQ(arcCount, graph.sizeArcs() + 2);
    CHECK_EQ(edgeCount, graph.sizeEdges());

    CHECK(!graph.exists(id0));
    CHECK(!graph.existsArc(id0, id2));
    CHECK(!graph.existsArc(id3, id0));

    GUM_CHECK_ASSERT_THROWS_NOTHING(graph.eraseNode(id6));

    CHECK_EQ(nodeCount, graph.size() + 2);
    CHECK_EQ(arcCount, graph.sizeArcs() + 4);
    CHECK_EQ(edgeCount, graph.sizeEdges() + 1);
  }

  GUM_TEST(RemoveNodes_1) {
    gum::PDAG graph;
    buildPDAG(graph);

    gum::Size nodeCount = graph.size();
    gum::Size arcCount  = graph.sizeArcs();
    gum::Size edgeCount = graph.sizeEdges();

    CHECK_EQ(nodeCount, static_cast< gum::Size >(9));
    CHECK_EQ(arcCount, static_cast< gum::Size >(8));
    CHECK_EQ(edgeCount, static_cast< gum::Size >(2));

    for (int i = 0; i < 10; i++) {
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.eraseNode(id4));
    }

    CHECK_EQ(nodeCount, graph.size() + 1);

    CHECK_EQ(arcCount, graph.sizeArcs() + 3);
    CHECK_EQ(edgeCount, graph.sizeEdges());

    CHECK(!graph.existsArc(id2, id4));
    CHECK(!graph.existsArc(id3, id4));
    CHECK(!graph.existsArc(id4, id1));
  }

  GUM_TEST(AddDelArcs_2) {
    gum::PDAG graph;
    buildPDAG(graph);

    CHECK(graph.existsArc(id0, id3));
    CHECK(graph.existsArc(id3, id4));
    CHECK(graph.existsArc(id1, id4));

    gum::Size nodeCount = graph.size();
    gum::Size arcCount  = graph.sizeArcs();
    gum::Size edgeCount = graph.sizeEdges();

    GUM_CHECK_ASSERT_THROWS_NOTHING(graph.eraseArc(gum::Arc(id1, id4)));

    CHECK_EQ(nodeCount, graph.size());
    CHECK_EQ(arcCount, graph.sizeArcs() + 1);
    CHECK_EQ(edgeCount, graph.sizeEdges());

    CHECK(!graph.existsArc(id1, id4));
  }

  GUM_TEST(DelEdge) {
    gum::PDAG graph;
    buildPDAG(graph);

    CHECK(graph.existsEdge(id3, id5));
    CHECK(graph.existsEdge(id5, id6));

    gum::Size nodeCount = graph.size();
    gum::Size arcCount  = graph.sizeArcs();
    gum::Size edgeCount = graph.sizeEdges();

    GUM_CHECK_ASSERT_THROWS_NOTHING(graph.eraseEdge(gum::Edge(id3, id5)));

    CHECK_EQ(nodeCount, graph.size());
    CHECK_EQ(arcCount, graph.sizeArcs());
    CHECK_EQ(edgeCount, graph.sizeEdges() + 1);

    CHECK(!graph.existsEdge(id3, id5));
  }

  GUM_TEST(GetNodes) {
    gum::PDAG graph;
    buildPDAG(graph);

    gum::NodeSet nodelist = graph.asNodeSet();
    CHECK_EQ(nodelist.size(), graph.size());
    gum::Size nodeCount = graph.size();

    for (const auto node: nodelist)
      graph.eraseNode(node);

    CHECK(graph.empty());

    CHECK_EQ(nodeCount, nodelist.size());
  }

  GUM_TEST(GetArcs) {
    gum::PDAG graph;
    buildPDAG(graph);

    gum::ArcSet arclist = graph.arcs();
    CHECK_EQ(arclist.size(), graph.sizeArcs());
    gum::Size arcCount = graph.sizeArcs();

    for (const auto& arc: arclist) {
      graph.eraseArc(arc);
    }

    CHECK(graph.emptyArcs());

    CHECK_EQ(arcCount, arclist.size());
  }

  GUM_TEST(GetEdges) {
    gum::PDAG graph;
    buildPDAG(graph);

    gum::EdgeSet edgelist = graph.edges();
    CHECK_EQ(edgelist.size(), graph.sizeEdges());
    gum::Size edgeCount = graph.sizeEdges();

    for (const auto& edge: edgelist) {
      graph.eraseEdge(edge);
    }

    CHECK(graph.emptyEdges());

    CHECK_EQ(edgeCount, edgelist.size());
  }

  GUM_TEST(NodeListMapNodes) {
    gum::PDAG graph;
    buildPDAG(graph);

    auto list = graph.listMapNodes(&simpleDoubleFunction);
    CHECK_EQ(list.size(), graph.size());

    gum::Size s = 0;

    for (const auto elt: list)
      s += elt;

    CHECK_EQ(s, 2 * (id0 + id1 + id2 + id3 + id4 + id5 + id6 + id7 + id8));
  }

  GUM_TEST(TwistedNodeListMapNodes) {
    gum::PDAG graph;
    buildPDAG(graph);

    gum::List< gum::Size > list;
    CHECK_THROWS(list = graph.listMapNodes(&twistedMapFunction));

    CHECK_EQ(list.size(), static_cast< gum::Size >(0));
  }

  GUM_TEST(HashMapNodes) {
    gum::PDAG graph;
    buildPDAG(graph);

    auto hashmap = graph.nodesPropertyFromFunction(&simpleDoubleFunction);
    CHECK_EQ(hashmap.size(), graph.size());

    gum::Size sk = 0;
    gum::Size sv = 0;

    for (const auto& [k, v]: hashmap) {
      sk += k;
      sv += v;
    }

    CHECK_EQ(sk * 2, sv);
  }

  GUM_TEST(TwistedHashMapNodes) {
    gum::PDAG graph;
    buildPDAG(graph);

    gum::NodeProperty< gum::Size > hashmap;
    CHECK_THROWS(hashmap = graph.nodesPropertyFromFunction(&twistedMapFunction));

    CHECK_EQ(hashmap.size(), static_cast< gum::Size >(0));
  }

  GUM_TEST(ListMapArcs) {
    gum::PDAG graph;
    buildPDAG(graph);

    gum::List< gum::Size > list = graph.listMapArcs(&simpleArcMapFunction);
    CHECK_EQ(list.size(), graph.sizeArcs());

    gum::Size s = 0;

    for (const auto elt: list)
      s += elt;

    CHECK_EQ(s, (gum::Size)(0 + 2 + 0 + 3 + 1 + 6 + 1 + 4 + +1 + 3 + 2 + 4 + 7 + 6 + 6 + 8));
  }

  GUM_TEST(HashMapArcs) {
    gum::PDAG graph;
    buildPDAG(graph);

    auto hashmap = graph.arcsProperty(&simpleArcMapFunction);
    CHECK_EQ(hashmap.size(), graph.sizeArcs());

    gum::Size sk = 0;
    gum::Size sv = 0;

    for (const auto& [k, v]: hashmap) {
      sk += k.head() + k.tail();
      sv += v;
    }

    CHECK_EQ(sk, sv);
  }

  GUM_TEST(ListMapEdges) {
    gum::PDAG graph;
    buildPDAG(graph);

    gum::List< gum::Size > list = graph.listMapEdges(&simpleEdgeMapFunction);
    CHECK_EQ(list.size(), graph.sizeEdges());

    gum::Size s = 0;

    for (const auto& elt: list)
      s += elt;

    CHECK_EQ(s, (gum::Size)(3 + 5 + 5 + 6));
  }

  GUM_TEST(HashMapEdges) {
    gum::PDAG graph;
    buildPDAG(graph);

    auto hashmap = graph.edgesProperty(&simpleEdgeMapFunction);
    CHECK_EQ(hashmap.size(), graph.sizeEdges());

    gum::Size sk = 0;
    gum::Size sv = 0;

    for (const auto& [k, v]: hashmap) {
      sk += k.first() + k.second();
      sv += v;
    }

    CHECK_EQ(sk, sv);
  }

  GUM_TEST(CopyOperator) {
    gum::PDAG graph;
    buildPDAG(graph);

    gum::PDAG g2;
    buildPDAG(g2);
    g2.addNode();

    gum::PDAG g3;
    gum::PDAG g4;

    g2 = g3 = g4 = graph;

    CHECK_EQ(g2, graph);
    CHECK_EQ(g3, graph);
    CHECK_EQ(g4, graph);

    g2.clear();
    g3.clearArcs();
    g4.clearEdges();

    CHECK_NE(g2, graph);
    CHECK_NE(g3, graph);
    CHECK_NE(g4, graph);
  }

  GUM_TEST(Family) {
    gum::PDAG graph;
    buildPDAG(graph);
    CHECK_EQ(graph.family(0), gum::NodeSet({0}));
    CHECK_EQ(graph.family(4), gum::NodeSet({1, 2, 3, 4}));
  }

  GUM_TEST(MonoCycle) {
    gum::PDAG graph;
    auto      x = graph.addNode();
    CHECK_THROWS_AS(graph.addArc(x, x), const gum::Exception&);
  }

  GUM_TEST(ConversionsRejectMonoCycles) {
    // an edge from a node to itself, refused by addEdge, is refused by the conversions too
    gum::UndiGraph u;
    u.addNodes(2);
    u.addEdge(0, 1);
    u.addEdge(1, 1);
    CHECK_THROWS_AS(gum::PDAG{u}, gum::InvalidDirectedCycle&);
    gum::MixedGraph m;
    m.addNodes(2);
    m.addArc(0, 1);
    m.addEdge(1, 1);
    CHECK_THROWS_AS(gum::PDAG{m}, gum::InvalidDirectedCycle&);
  }

// PDAG::moralGraph, moralizedAncestralGraph, cSeparation and hasMixedReallyOrientedPath are
// deprecated after 3.3.2: their behaviour is still tested until their removal
#if defined(__GNUC__) || defined(__clang__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4996)
#endif

  GUM_TEST(DeprecatedMethodsGiveTheResultsOfChainGraph) {
    // 0-1->2<-3
    gum::PDAG pdag;
    pdag.addNodes(4);
    pdag.addEdge(0, 1);
    pdag.addArc(1, 2);
    pdag.addArc(3, 2);
    const gum::ChainGraph chain(pdag);

    CHECK(pdag.moralGraph() == chain.moralGraph());
    CHECK(pdag.moralGraph().existsEdge(1, 3));
    const gum::NodeSet nodes{2};
    CHECK(pdag.moralizedAncestralGraph(nodes) == chain.moralizedAncestralGraph(nodes));
    CHECK(pdag.cSeparation(1, 3, gum::NodeSet()));
    CHECK(!pdag.cSeparation(1, 3, gum::NodeSet({2})));
    CHECK(pdag.cSeparation(gum::NodeSet({0, 1}), gum::NodeSet({3}), gum::NodeSet()));
    CHECK(!pdag.cSeparation(gum::NodeSet({0, 1}), gum::NodeSet({3}), gum::NodeSet({2})));
  }

  GUM_TEST(DeprecatedHasMixedReallyOrientedPathFindsWalks) {
    // t-v, v-h, v-a, a->b, b-v: the partially directed cycle v-a->b-v can be walked between t and h
    gum::PDAG         g;
    const gum::NodeId t = g.addNode(), v = g.addNode(), h = g.addNode(), a = g.addNode(),
                      b = g.addNode();
    g.addEdge(t, v);
    g.addEdge(v, h);
    CHECK(!g.hasMixedReallyOrientedPath(t, h));
    g.addEdge(v, a);
    g.addArc(a, b);
    CHECK(g.hasMixedReallyOrientedPath(t, b));   // t-v-a->b
    CHECK(!g.hasMixedReallyOrientedPath(t, t));
    g.addEdge(b, v);
    // t-v-a->b-v-h, whereas the only path t-v-h has no arc
    CHECK(g.hasMixedReallyOrientedPath(t, h));
  }

#if defined(__GNUC__) || defined(__clang__)
#  pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#  pragma warning(pop)
#endif

}   // namespace gum_tests
