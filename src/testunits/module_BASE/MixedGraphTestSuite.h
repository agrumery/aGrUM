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

#include <agrum/base/graphs/diGraph.h>
#include <agrum/base/graphs/graphElements.h>
#include <agrum/base/graphs/mixedGraph.h>
#include <agrum/base/graphs/undiGraph.h>

#include "agrum/base/graphs/graphElements.h"

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  MixedGraph
#define GUM_CURRENT_MODULE GUMBASE

// The graph used for the tests:
//          0   1_          0 -- 2
//         / \ / /          0 -> 3
//        2   3 /           2 -- 4
//         \ / /            3 -> 4
//          4_/             1 -- 3
//                          4 -> 1

namespace gum_tests {

  struct MixedGraphTestSuite {
    private:
    gum::MixedGraph buildGraph() {
      gum::MixedGraph g;
      id1 = g.addNode();
      id2 = g.addNode();
      id3 = g.addNode();
      id4 = g.addNode();
      id5 = g.addNode();

      g.addEdge(id1, id3);
      g.addEdge(id3, id5);
      g.addEdge(id2, id4);
      g.addArc(id1, id4);
      g.addArc(id4, id5);
      g.addArc(id5, id2);

      return g;
    }

    static gum::Size simpleDoubleFunction(const gum::NodeId& aNodeId) { return aNodeId * 2; }

    static gum::Size simpleEdgeMapFunction(const gum::Edge& anEdge) {
      return anEdge.first() + anEdge.second();
    }

    static gum::Size simpleArcMapFunction(const gum::Arc& anArc) {
      return anArc.tail() + anArc.head();
    }

    static gum::Size twistedMapFunction(const gum::NodeId& aNode) { throw(aNode); }

    public:
    gum::NodeId id1, id2, id3, id4, id5;

    static void testConstructor1() {
      gum::MixedGraph* graph = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING((graph = new gum::MixedGraph()));
      GUM_CHECK_ASSERT_THROWS_NOTHING(delete (graph));
    }

    static void testConstructorFromUG() {
      gum::UndiGraph ug;
      ug.addNode();
      ug.addNode();
      ug.addNode();
      ug.addNode();

      ug.addEdge(0, 2);
      ug.addEdge(1, 2);
      ug.addEdge(2, 3);

      gum::MixedGraph mg;
      mg.addNode();
      mg.addNode();
      mg.addNode();
      mg.addNode();

      mg.addEdge(0, 2);
      mg.addEdge(1, 2);
      mg.addEdge(2, 3);

      gum::MixedGraph* mixed_graph = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING((mixed_graph = new gum::MixedGraph(ug)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(CHECK((*mixed_graph == mg)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(delete (mixed_graph));
    }

    static void testConstructorFromDG() {
      gum::DiGraph dg;
      dg.addNode();
      dg.addNode();
      dg.addNode();
      dg.addNode();

      dg.addArc(0, 2);
      dg.addArc(1, 2);
      dg.addArc(2, 3);

      gum::MixedGraph mg;
      mg.addNode();
      mg.addNode();
      mg.addNode();
      mg.addNode();

      mg.addArc(0, 2);
      mg.addArc(1, 2);
      mg.addArc(2, 3);

      gum::MixedGraph* mixed_graph = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING((mixed_graph = new gum::MixedGraph(dg)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(CHECK((*mixed_graph == mg)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(delete (mixed_graph));
    }

    static void testInsert1() {
      gum::MixedGraph graph;

      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addNode());
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addNode());
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addNode());
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addNode());
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addNode());

      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addEdge(0, 2));
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addEdge(2, 4));
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addEdge(1, 3));

      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addArc(0, 3));
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addArc(3, 4));
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addArc(4, 1));
    }

    void testCopyConstructor() {
      gum::MixedGraph graph = buildGraph();

      gum::MixedGraph* copy = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING((copy = new gum::MixedGraph(graph)));
      CHECK_EQ(graph, *copy);
      delete (copy);

      GUM_CHECK_ASSERT_THROWS_NOTHING(gum::MixedGraph copy2 = graph);
      GUM_CHECK_ASSERT_THROWS_NOTHING(gum::MixedGraph copy3(graph));
    }

    void testEmptyNodes() {
      gum::MixedGraph graph;
      CHECK(graph.empty());
      graph = buildGraph();
      CHECK(!graph.empty());
    }

    void testEmptyEdges() {
      gum::MixedGraph graph;
      CHECK(graph.emptyEdges());
      graph = buildGraph();
      CHECK(!graph.emptyEdges());
    }

    void testEmptyArcs() {
      gum::MixedGraph graph;
      CHECK(graph.emptyArcs());
      graph = buildGraph();
      CHECK(!graph.emptyArcs());
    }

    void testClearNodes() {
      gum::MixedGraph graph = buildGraph();
      CHECK(!graph.empty());
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.clear());
      CHECK((graph.empty() && graph.emptyArcs()));
    }

    void testClearArcs() {
      gum::MixedGraph graph = buildGraph();
      CHECK(!graph.emptyArcs());
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.clearArcs());
      CHECK(graph.emptyArcs());
      CHECK(!graph.empty());
    }

    void testClearEdges() {
      gum::MixedGraph graph = buildGraph();
      CHECK(!graph.emptyEdges());
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.clearEdges());
      CHECK(graph.emptyEdges());
      CHECK(!graph.empty());
    }

    void testAddDelNodes_2() {
      gum::MixedGraph graph = buildGraph();

      CHECK(graph.exists(id1));
      CHECK(graph.exists(id2));
      CHECK(graph.exists(id3));
      CHECK(graph.exists(id4));
      CHECK(graph.exists(id5));
      CHECK(!graph.exists(id5 + id4 + id3 + id2 + id1));

      CHECK(graph.existsEdge(id3, id5));
      CHECK(graph.existsEdge(id5, id3));
      CHECK(!graph.existsEdge(id1, id1));

      gum::Size nodeCount = graph.size();
      gum::Size edgeCount = graph.sizeEdges();
      gum::Size arcCount  = graph.sizeArcs();

      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.eraseNode(id2));

      CHECK_EQ(nodeCount, graph.size() + 1);
      CHECK_EQ(edgeCount, graph.sizeEdges() + 1);
      CHECK_EQ(arcCount, graph.sizeArcs() + 1);

      CHECK(!graph.exists(id2));
      CHECK(!graph.existsEdge(id2, id4));
      CHECK(!graph.existsEdge(id5, id2));
      CHECK(!graph.existsEdge(id2, id5));
    }

    void testRemoveNodesFunky_1() {
      gum::MixedGraph graph = buildGraph();

      gum::Size nodeCount = graph.size();
      gum::Size edgeCount = graph.sizeEdges();
      gum::Size arcCount  = graph.sizeArcs();

      CHECK_EQ(nodeCount, static_cast< gum::Size >(5));
      CHECK_EQ(edgeCount, static_cast< gum::Size >(3));
      CHECK_EQ(arcCount, static_cast< gum::Size >(3));

      for (int i = 0; i < 10; i++) {
        GUM_CHECK_ASSERT_THROWS_NOTHING(graph.eraseNode(id5));
      }

      CHECK_EQ(nodeCount, graph.size() + 1);

      CHECK_EQ(edgeCount, graph.sizeEdges() + 1);
      CHECK_EQ(arcCount, graph.sizeArcs() + 2);

      CHECK(!graph.existsEdge(2, 4));
      CHECK(!graph.existsEdge(3, 4));
      CHECK(!graph.existsEdge(4, 1));
    }

    void testAddDelEdges_1() {
      gum::MixedGraph graph = buildGraph();

      CHECK(graph.existsEdge(0, 2));
      CHECK(graph.existsEdge(2, 4));
      CHECK(graph.existsEdge(1, 3));

      gum::Size nodeCount = graph.size();
      gum::Size edgeCount = graph.sizeEdges();

      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.eraseEdge(gum::Edge(4, 2)));

      CHECK_EQ(nodeCount, graph.size());
      CHECK_EQ(edgeCount, graph.sizeEdges() + 1);

      CHECK(!graph.existsEdge(2, 4));
    }

    void testAddDelEdges_2() {
      gum::MixedGraph graph = buildGraph();

      CHECK(graph.existsEdge(id1, id3));
      CHECK(graph.existsEdge(id3, id5));
      CHECK(graph.existsEdge(id2, id4));

      gum::Size nodeCount = graph.size();
      gum::Size edgeCount = graph.sizeEdges();

      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.eraseEdge(gum::Edge(id3, id5)));

      CHECK_EQ(nodeCount, graph.size());
      CHECK_EQ(edgeCount, graph.sizeEdges() + 1);

      CHECK(!graph.existsEdge(id3, id5));
    }

    void testAddDelArcs_2() {
      gum::MixedGraph graph = buildGraph();

      CHECK(graph.existsArc(id1, id4));
      CHECK(graph.existsArc(id4, id5));
      CHECK(graph.existsArc(id5, id2));

      gum::Size nodeCount = graph.size();
      gum::Size arcCount  = graph.sizeArcs();

      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.eraseArc(gum::Arc(id5, id2)));

      CHECK_EQ(nodeCount, graph.size());
      CHECK_EQ(arcCount, graph.sizeArcs() + 1);

      CHECK(!graph.existsArc(id5, id2));
    }

    void testGetNodes() {
      gum::MixedGraph graph = buildGraph();

      const gum::NodeSet nodeset = graph.asNodeSet();
      CHECK_EQ(nodeset.size(), graph.size());
      gum::Size nodeCount = graph.size();

      for (const auto node: nodeset)
        graph.eraseNode(node);

      CHECK(graph.empty());

      CHECK_EQ(nodeCount, nodeset.size());
    }

    void testGetArcs() {
      gum::MixedGraph graph = buildGraph();

      gum::ArcSet arclist = graph.arcs();
      CHECK_EQ(arclist.size(), graph.sizeArcs());
      gum::Size arcCount = graph.sizeArcs();

      for (const auto& arc: arclist)
        graph.eraseArc(arc);

      CHECK(graph.emptyArcs());

      CHECK_EQ(arcCount, arclist.size());
    }

    void testGetEdges() {
      gum::MixedGraph graph = buildGraph();

      gum::EdgeSet edgeset = graph.edges();
      CHECK_EQ(edgeset.size(), graph.sizeEdges());
      gum::Size edgeCount = graph.sizeEdges();

      for (const auto& edge: edgeset)
        graph.eraseEdge(edge);

      CHECK(graph.emptyEdges());

      CHECK_EQ(edgeCount, edgeset.size());
    }

    void testNodeListMapNodes() {
      gum::MixedGraph graph = buildGraph();

      gum::List< gum::Size > list = graph.listMapNodes(&simpleDoubleFunction);
      CHECK_EQ(list.size(), graph.size());

      gum::Size s = 0;

      for (gum::List< gum::Size >::iterator iter = list.begin(); iter != list.end(); ++iter) {
        s += *iter;
      }

      CHECK_EQ(s, 2 * (id1 + id2 + id3 + id4 + id5));
    }

    void testTwistedNodeListMapNodes() {
      gum::MixedGraph graph = buildGraph();

      gum::List< gum::Size > list;
      CHECK_THROWS(list = graph.listMapNodes(&twistedMapFunction));

      CHECK_EQ(list.size(), static_cast< gum::Size >(0));
    }

    void testHashMapNodes() {
      gum::MixedGraph graph = buildGraph();

      gum::NodeProperty< gum::Size > hashmap
          = graph.nodesPropertyFromFunction(&simpleDoubleFunction);
      CHECK_EQ(hashmap.size(), graph.size());

      gum::Size sk = 0;
      gum::Size sv = 0;

      for (const auto& elt: hashmap) {
        sk += elt.first;
        sv += elt.second;
      }

      CHECK_EQ(sk * 2, sv);
    }

    void testTwistedHashMapNodes() {
      gum::MixedGraph graph = buildGraph();

      gum::NodeProperty< gum::Size > hashmap;
      CHECK_THROWS(hashmap = graph.nodesPropertyFromFunction(&twistedMapFunction));

      CHECK_EQ(hashmap.size(), static_cast< gum::Size >(0));
    }

    void testListMapEdges() {
      gum::MixedGraph graph = buildGraph();

      gum::List< gum::Size > list = graph.listMapEdges(&simpleEdgeMapFunction);
      CHECK_EQ(list.size(), graph.sizeEdges());

      gum::Size s = 0;

      for (gum::List< gum::Size >::iterator iter = list.begin(); iter != list.end(); ++iter) {
        s += *iter;
      }

      // The graph used for the tests:
      //          0   1_          0 -- 2
      //         / \ / /          0 -> 3
      //        2   3 /           2 -- 4
      //         \ / /            3 -> 4
      //          4_/             1 -- 3
      //                          4 -> 1
      CHECK_EQ(s, (gum::Size)(0 + 2 + 2 + 4 + 1 + 3));
    }

    void testHashMapEdges() {
      gum::MixedGraph graph = buildGraph();

      gum::EdgeProperty< gum::Size > hashmap = graph.edgesProperty(&simpleEdgeMapFunction);
      CHECK_EQ(hashmap.size(), graph.sizeEdges());

      gum::Size sk = 0;
      gum::Size sv = 0;

      for (const auto& elt: hashmap) {
        sv += elt.second;
        sk += elt.first.first() + elt.first.second();
      }

      CHECK_EQ(sk, sv);
    }

    void testListMapArcs() {
      gum::MixedGraph graph = buildGraph();

      gum::List< gum::Size > list = graph.listMapArcs(&simpleArcMapFunction);
      CHECK_EQ(list.size(), graph.sizeArcs());

      gum::Size s = 0;

      for (gum::List< gum::Size >::iterator iter = list.begin(); iter != list.end(); ++iter) {
        s += *iter;
      }

      // The graph used for the tests:
      //          0   1_          0 -- 2
      //         / \ / /          0 -> 3
      //        2   3 /           2 -- 4
      //         \ / /            3 -> 4
      //          4_/             1 -- 3
      //                          4 -> 1
      CHECK_EQ(s, (gum::Size)(0 + 3 + 3 + 4 + 4 + 1));
    }

    void testHashMapArcs() {
      gum::MixedGraph graph = buildGraph();

      gum::ArcProperty< gum::Size > hashmap = graph.arcsProperty(&simpleArcMapFunction);
      CHECK_EQ(hashmap.size(), graph.sizeArcs());

      gum::Size sk = 0;
      gum::Size sv = 0;

      for (const auto& elt: hashmap) {
        sv += elt.second;
        sk += elt.first.head() + elt.first.tail();
      }

      CHECK_EQ(sk, sv);
    }

    void testCopyOperator() {
      gum::MixedGraph graph = buildGraph();

      gum::MixedGraph g2 = buildGraph();
      g2.addNode();

      gum::MixedGraph g3;
      gum::MixedGraph g4;

      g2 = g3 = g4 = graph;

      CHECK_EQ(g2, graph);
      CHECK_EQ(g3, graph);
      CHECK_EQ(g4, graph);

      g2.clear();
      g3.clearArcs();
      g4.clearEdges();

      CHECK_NE(g2, graph);
      CHECK_NE(g3, graph);
    }

    // Build the standard test graph locally (id1=0,id2=1,id3=2,id4=3,id5=4)
    // Edges: 0--2, 2--4, 1--3   Arcs: 0→3, 3→4, 4→1
    static gum::MixedGraph makeTestGraph() {
      gum::MixedGraph g;
      g.addNodes(5);
      g.addEdge(0, 2);
      g.addEdge(2, 4);
      g.addEdge(1, 3);
      g.addArc(0, 3);
      g.addArc(3, 4);
      g.addArc(4, 1);
      return g;
    }

    static void testBoundaryAndChainComponent() {
      gum::MixedGraph graph = makeTestGraph();
      // Edges: 0--2, 2--4, 1--3   Arcs: 0→3, 3→4, 4→1

      // boundary(0) = neighbours(0) ∪ parents(0) ∪ children(0) = {2}∪{}∪{3}
      gum::NodeSet b0 = graph.boundary(0);
      CHECK_EQ(b0.size(), 2U);
      CHECK(b0.contains(2));
      CHECK(b0.contains(3));

      // boundary(3) = {1}∪{0}∪{4}
      gum::NodeSet b3 = graph.boundary(3);
      CHECK_EQ(b3.size(), 3U);
      CHECK(b3.contains(1));
      CHECK(b3.contains(0));
      CHECK(b3.contains(4));

      // chainComponent: connected via edges only
      // 0--2--4 form one chain component, 1--3 another
      gum::NodeSet cc0 = graph.chainComponent(0);
      CHECK_EQ(cc0.size(), 3U);
      CHECK(cc0.contains(0));
      CHECK(cc0.contains(2));
      CHECK(cc0.contains(4));
      CHECK(!cc0.contains(1));

      gum::NodeSet cc1 = graph.chainComponent(1);
      CHECK_EQ(cc1.size(), 2U);
      CHECK(cc1.contains(1));
      CHECK(cc1.contains(3));
    }

    static void testChainComponents() {
      gum::MixedGraph graph = makeTestGraph();
      // Chain components: {0,2,4} and {1,3}

      auto cc = graph.chainComponents();
      CHECK_EQ(cc.size(), 5U);
      CHECK_EQ(cc[0], cc[2]);   // 0 and 2 same
      CHECK_EQ(cc[2], cc[4]);   // 2 and 4 same
      CHECK_EQ(cc[1], cc[3]);   // 1 and 3 same
      CHECK_NE(cc[0], cc[1]);   // {0,2,4} ≠ {1,3}
    }

    static void testConnectedComponents() {
      gum::MixedGraph graph = makeTestGraph();
      // Edges: 0--2, 2--4, 1--3   Arcs: 0→3, 3→4, 4→1
      // Weakly connected: 0→3--1 and 0--2--4→1 tie all nodes together

      auto cc = graph.connectedComponents();
      CHECK_EQ(cc.size(), 5U);
      CHECK_EQ(cc[0], cc[1]);
      CHECK_EQ(cc[0], cc[2]);
      CHECK_EQ(cc[0], cc[3]);
      CHECK_EQ(cc[0], cc[4]);

      // connectedComponents ≠ chainComponents on mixed graph
      auto chain = graph.chainComponents();
      CHECK_NE(chain[0], chain[1]);   // different chain components, same connected component

      // Isolated node → own weakly connected component
      gum::MixedGraph g2;
      g2.addNodes(3);
      g2.addArc(0, 1);
      // node 2 is isolated
      auto cc2 = g2.connectedComponents();
      CHECK_EQ(cc2[0], cc2[1]);
      CHECK_NE(cc2[0], cc2[2]);
    }

    static void testMixedPaths() {
      gum::MixedGraph graph = makeTestGraph();
      // Edges: 0--2, 2--4, 1--3   Arcs: 0→3, 3→4, 4→1

      // mixedOrientedPath: arcs forward, edges both ways
      // 0→3--1 (arc 0→3, edge 3--1)
      auto path01 = graph.mixedOrientedPath(0, 1);
      CHECK(path01.has_value());
      CHECK_EQ(path01->front(), static_cast< gum::NodeId >(0));
      CHECK_EQ(path01->back(), static_cast< gum::NodeId >(1));

      CHECK(graph.hasMixedOrientedPath(0, 1));

      // Simple graph: only arc 0→1, node 2 isolated
      gum::MixedGraph g;
      g.addNodes(3);
      g.addArc(0, 1);

      CHECK(g.hasMixedOrientedPath(0, 1));
      // cannot traverse arc backward in mixedOrientedPath
      CHECK(!g.hasMixedOrientedPath(1, 0));
      CHECK_FALSE(g.mixedOrientedPath(1, 0).has_value());
      // isolated node
      CHECK(!g.hasMixedOrientedPath(0, 2));
      CHECK_FALSE(g.mixedOrientedPath(0, 2).has_value());

      // mixedUnorientedPath: arcs and edges traversable in both directions
      // 1→0 now possible by going backward on arc
      auto upath10 = g.mixedUnorientedPath(1, 0);
      CHECK(upath10.has_value());
      CHECK_EQ(upath10->front(), static_cast< gum::NodeId >(1));
      CHECK_EQ(upath10->back(), static_cast< gum::NodeId >(0));

      // isolated node still unreachable
      CHECK_FALSE(g.mixedUnorientedPath(0, 2).has_value());

      // on the original graph, all nodes are mutually reachable via unoriented path
      CHECK(graph.mixedUnorientedPath(1, 0).has_value());
      CHECK(graph.mixedUnorientedPath(4, 2).has_value());
    }

    static void testToDotWithNames() {
      gum::MixedGraph g;
      g.addNodes(3);
      g.setName(0, "alpha");
      g.setName(2, "ga\"mm\\a");
      g.addArc(0, 1);
      g.addEdge(1, 2);

      std::string dot = g.toDot();

      CHECK(dot.find("0 [label=\"(0) alpha\"]") != std::string::npos);
      CHECK(dot.find("2 [label=\"(2) ga\\\"mm\\\\a\"]") != std::string::npos);
      CHECK(dot.find("1 [label=") == std::string::npos);
    }
  };

  GUM_TEST_ACTIF(Constructor1)
  GUM_TEST_ACTIF(ConstructorFromUG)
  GUM_TEST_ACTIF(ConstructorFromDG)
  GUM_TEST_ACTIF(Insert1)
  GUM_TEST_ACTIF(CopyConstructor)
  GUM_TEST_ACTIF(EmptyNodes)
  GUM_TEST_ACTIF(EmptyEdges)
  GUM_TEST_ACTIF(EmptyArcs)
  GUM_TEST_ACTIF(ClearNodes)
  GUM_TEST_ACTIF(ClearArcs)
  GUM_TEST_ACTIF(ClearEdges)
  GUM_TEST_ACTIF(AddDelNodes_2)
  GUM_TEST_ACTIF(RemoveNodesFunky_1)
  GUM_TEST_ACTIF(AddDelEdges_1)
  GUM_TEST_ACTIF(AddDelEdges_2)
  GUM_TEST_ACTIF(AddDelArcs_2)
  GUM_TEST_ACTIF(GetNodes)
  GUM_TEST_ACTIF(GetArcs)
  GUM_TEST_ACTIF(GetEdges)
  GUM_TEST_ACTIF(NodeListMapNodes)
  GUM_TEST_ACTIF(TwistedNodeListMapNodes)
  GUM_TEST_ACTIF(HashMapNodes)
  GUM_TEST_ACTIF(TwistedHashMapNodes)
  GUM_TEST_ACTIF(ListMapEdges)
  GUM_TEST_ACTIF(HashMapEdges)
  GUM_TEST_ACTIF(ListMapArcs)
  GUM_TEST_ACTIF(HashMapArcs)
  GUM_TEST_ACTIF(CopyOperator)
  GUM_TEST_ACTIF(BoundaryAndChainComponent)
  GUM_TEST_ACTIF(ChainComponents)
  GUM_TEST_ACTIF(ConnectedComponents)
  GUM_TEST_ACTIF(MixedPaths)
  GUM_TEST_ACTIF(ToDotWithNames)
}   // namespace gum_tests
