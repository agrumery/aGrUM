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

#include <testunits/gumtest/AgrumTestSuite.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  DiGraph
#define GUM_CURRENT_MODULE GUMBASE

// The graph used for the tests:
//          0   1_          0 -> 2
//         / \ / /          0 -> 3
//        2   3 /           2 -> 4
//         \ / /            3 -> 4
//          4_/             1 -> 3
//                          4 -> 1

namespace gum_tests {

  struct DiGraphTestSuite {
    private:
    static gum::Size simpleDoubleFunction(const gum::NodeId& aNodeId) { return aNodeId * 2; }

    static gum::Size simpleArcMapFunction(const gum::Arc& anArc) {
      return anArc.tail() + anArc.head();
    }

    static gum::Size twistedMapFunction(const gum::NodeId& aNode) { throw(aNode); }

    gum::DiGraph buildGraph() {
      gum::DiGraph g;
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
      g.addArc(id5, id2);

      return g;
    }

    public:
    gum::NodeId id1, id2, id3, id4, id5;

    static void testConstructor1() {
      gum::DiGraph* graph = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING((graph = new gum::DiGraph()));
      GUM_CHECK_ASSERT_THROWS_NOTHING((delete (graph)));
    }

    static void testInsert1() {
      gum::DiGraph graph;
      gum::NodeId  id1 = 0;
      gum::NodeId  id2 = 0;
      gum::NodeId  id3 = 0;
      gum::NodeId  id4 = 0;
      gum::NodeId  id5 = 0;

      GUM_CHECK_ASSERT_THROWS_NOTHING(id1 = graph.addNode());
      GUM_CHECK_ASSERT_THROWS_NOTHING(id2 = graph.addNode());
      GUM_CHECK_ASSERT_THROWS_NOTHING(id3 = graph.addNode());
      GUM_CHECK_ASSERT_THROWS_NOTHING(id4 = graph.addNode());
      GUM_CHECK_ASSERT_THROWS_NOTHING(id5 = graph.addNode());

      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addArc(id1, id3));
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addArc(id3, id5));
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addArc(id2, id4));
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addArc(id1, id4));
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addArc(id4, id5));
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addArc(id5, id2));
    }

    void testCopyConstructor() {
      gum::DiGraph graph = buildGraph();

      gum::DiGraph* copy = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING((copy = new gum::DiGraph(graph)));
      CHECK_EQ(graph, *copy);
      delete (copy);

      GUM_CHECK_ASSERT_THROWS_NOTHING(gum::DiGraph copy2 = graph);
      GUM_CHECK_ASSERT_THROWS_NOTHING(gum::DiGraph copy3(graph));
    }

    void testCopyOperator() {
      gum::DiGraph graph = buildGraph();

      gum::DiGraph g2 = buildGraph();
      g2.addNode();

      gum::DiGraph g3;

      g2 = g3 = graph;

      CHECK_EQ(g2, graph);
      CHECK_EQ(g3, graph);

      g2.clear();
      g3.clearArcs();

      CHECK_NE(g2, graph);
      CHECK_NE(g3, graph);
    }

    void testEmptyNodes() {
      gum::DiGraph graph;

      CHECK(graph.empty());
      graph = buildGraph();
      CHECK(!graph.empty());
    }

    void testEmptyArcs() {
      gum::DiGraph graph;
      CHECK(graph.emptyArcs());
      graph = buildGraph();
      CHECK(!graph.emptyArcs());
    }

    void testClearNodes() {
      gum::DiGraph graph = buildGraph();
      CHECK(!graph.empty());
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.clear());
      CHECK((graph.empty() && graph.emptyArcs()));
    }

    void testClearArcs() {
      gum::DiGraph graph = buildGraph();
      CHECK(!graph.emptyArcs());
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.clearArcs());
      CHECK(graph.emptyArcs());
      CHECK(!graph.empty());
    }

    void testAddDelNodes_2() {
      gum::DiGraph graph = buildGraph();

      CHECK(graph.exists(id1));
      CHECK(graph.exists(id2));
      CHECK(graph.exists(id3));
      CHECK(graph.exists(id4));
      CHECK(graph.exists(id5));
      CHECK(!graph.exists(id5 + id4 + id3 + id2 + id1));

      CHECK(graph.existsArc(id3, id5));
      CHECK(!graph.existsArc(id5, id3));
      CHECK(!graph.existsArc(id1, id1));

      gum::Size nodeCount = graph.size();
      gum::Size arcCount  = graph.sizeArcs();

      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.eraseNode(id2));

      CHECK_EQ(nodeCount, graph.size() + 1);
      CHECK_EQ(arcCount, graph.sizeArcs() + 2);

      CHECK(!graph.exists(id2));
      CHECK(!graph.existsArc(id2, id4));
      CHECK(!graph.existsArc(id5, id2));
    }

    void testRemoveNodes_1() {
      gum::DiGraph graph = buildGraph();

      gum::Size nodeCount = graph.size();
      gum::Size arcCount  = graph.sizeArcs();

      CHECK_EQ(nodeCount, static_cast< gum::Size >(5));
      CHECK_EQ(arcCount, static_cast< gum::Size >(6));

      for (int i = 0; i < 10; i++) {
        GUM_CHECK_ASSERT_THROWS_NOTHING(graph.eraseNode(id5));
      }

      CHECK_EQ(nodeCount, graph.size() + 1);

      CHECK_EQ(arcCount, graph.sizeArcs() + 3);

      CHECK(!graph.existsArc(id3, id5));
      CHECK(!graph.existsArc(id4, id5));
      CHECK(!graph.existsArc(id5, id2));
    }

    void testAddDelArcs_2() {
      gum::DiGraph graph = buildGraph();

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
      gum::DiGraph graph = buildGraph();

      gum::NodeSet nodelist = graph.asNodeSet();
      CHECK_EQ(nodelist.size(), graph.size());
      gum::Size nodeCount = graph.size();

      for (const auto node: nodelist)
        graph.eraseNode(node);

      CHECK(graph.empty());

      CHECK_EQ(nodeCount, nodelist.size());
    }

    void testGetArcs() {
      gum::DiGraph graph = buildGraph();

      gum::ArcSet arclist = graph.arcs();
      CHECK_EQ(arclist.size(), graph.sizeArcs());
      gum::Size arcCount = graph.sizeArcs();

      for (const auto& arc: arclist)
        graph.eraseArc(arc);

      CHECK(graph.emptyArcs());

      CHECK_EQ(arcCount, arclist.size());
    }

    void testNodeListMapNodes() {
      gum::DiGraph graph = buildGraph();

      gum::List< gum::Size > list = graph.listMapNodes(&simpleDoubleFunction);
      CHECK_EQ(list.size(), graph.size());

      gum::Size s = 0;

      for (gum::List< gum::Size >::iterator iter = list.begin(); iter != list.end(); ++iter) {
        s += *iter;
      }

      CHECK_EQ(s, (gum::Size)(2 * (id1 + id2 + id3 + id4 + id5)));
    }

    void testTwistedNodeListMapNodes() {
      gum::DiGraph graph = buildGraph();

      gum::List< gum::Size > list;
      CHECK_THROWS(list = graph.listMapNodes(&twistedMapFunction));

      CHECK_EQ(list.size(), static_cast< gum::Size >(0));
    }

    void testHashMapNodes() {
      gum::DiGraph graph = buildGraph();

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
      gum::DiGraph graph = buildGraph();

      gum::NodeProperty< gum::Size > hashmap;
      CHECK_THROWS(hashmap = graph.nodesPropertyFromFunction(&twistedMapFunction));

      CHECK_EQ(hashmap.size(), static_cast< gum::Size >(0));
    }

    void testListMapArcs() {
      gum::DiGraph graph = buildGraph();

      gum::List< gum::Size > list = graph.listMapArcs(&simpleArcMapFunction);
      CHECK_EQ(list.size(), graph.sizeArcs());

      gum::Size s = 0;

      for (gum::List< gum::Size >::iterator iter = list.begin(); iter != list.end(); ++iter) {
        s += *iter;
      }

      CHECK_EQ(s, (gum::Size)(0 + 0 + 2 + 3 + 1 + 4 + 2 + 3 + 4 + 4 + 3 + 1));
    }

    void testHashMapArcs() {
      gum::DiGraph graph = buildGraph();

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

    // this test should be in NodeGraphPartTestSuite.h which does not exist for
    // now
    // ...
    void testNodeId() {
      gum::DiGraph graph = buildGraph();

      gum::NodeId id  = graph.nextNodeId();
      gum::NodeId id2 = graph.addNode();
      CHECK_EQ(id, id2);
    }

    void testDirectedPaths() {
      try {
        gum::DiGraph graph = buildGraph();

        auto path_opt = graph.directedPath(0, 1);
        CHECK(path_opt.has_value());
        const auto& path = *path_opt;
        CHECK_EQ(path.size(), 4U);
        CHECK_EQ(path[0], 0U);
        CHECK_EQ(path[1], 3U);
        CHECK_EQ(path[2], 4U);
        CHECK_EQ(path[3], 1U);

        CHECK_EQ(graph.hasDirectedPath(0, 1), true);
        CHECK_EQ(graph.hasDirectedPath(0, 0), false);
        CHECK_EQ(graph.hasDirectedPath(0, 4), true);
        CHECK_EQ(graph.hasDirectedPath(1, 1), true);

        CHECK_FALSE(graph.directedPath(1, 2).has_value());

        auto path2_opt = graph.directedUnorientedPath(1, 2);
        CHECK(path2_opt.has_value());
        const auto& path2 = *path2_opt;
        CHECK_EQ(path2.size(), 3U);
        CHECK_EQ(path2[0], 1U);
        CHECK_EQ(path2[1], 4U);
        CHECK_EQ(path2[2], 2U);
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }
    }

    static void testAncestorsDescendants() {
      // DAG: 0→1, 0→2, 1→3, 2→3, 3→4
      gum::DiGraph g;
      g.addNodes(5);
      g.addArc(0, 1);
      g.addArc(0, 2);
      g.addArc(1, 3);
      g.addArc(2, 3);
      g.addArc(3, 4);

      // ancestors
      CHECK(g.ancestors(0).empty());

      gum::NodeSet anc3 = g.ancestors(3);
      CHECK_EQ(anc3.size(), 3U);
      CHECK(anc3.contains(0));
      CHECK(anc3.contains(1));
      CHECK(anc3.contains(2));

      gum::NodeSet anc4 = g.ancestors(4);
      CHECK_EQ(anc4.size(), 4U);
      CHECK(anc4.contains(0));
      CHECK(anc4.contains(1));
      CHECK(anc4.contains(2));
      CHECK(anc4.contains(3));
      CHECK(!anc4.contains(4));

      // descendants
      CHECK(g.descendants(4).empty());

      gum::NodeSet desc0 = g.descendants(0);
      CHECK_EQ(desc0.size(), 4U);
      CHECK(desc0.contains(1));
      CHECK(desc0.contains(2));
      CHECK(desc0.contains(3));
      CHECK(desc0.contains(4));
      CHECK(!desc0.contains(0));

      gum::NodeSet desc1 = g.descendants(1);
      CHECK_EQ(desc1.size(), 2U);
      CHECK(desc1.contains(3));
      CHECK(desc1.contains(4));

      // family = {self} ∪ parents(self)
      gum::NodeSet fam0 = g.family(0);
      CHECK_EQ(fam0.size(), 1U);
      CHECK(fam0.contains(0));

      gum::NodeSet fam3 = g.family(3);
      CHECK_EQ(fam3.size(), 3U);
      CHECK(fam3.contains(1));
      CHECK(fam3.contains(2));
      CHECK(fam3.contains(3));

      // family of a NodeSet = union of individual families
      gum::NodeSet ids{1, 3};
      gum::NodeSet fam13 = g.family(ids);
      CHECK_EQ(fam13.size(), 4U);   // {0,1} ∪ {1,2,3}
      CHECK(fam13.contains(0));
      CHECK(fam13.contains(1));
      CHECK(fam13.contains(2));
      CHECK(fam13.contains(3));
    }

    static void testConnectedComponents() {
      // 0→1→2 and 3→4: two weakly connected components; 5: isolated
      gum::DiGraph g;
      g.addNodes(6);
      g.addArc(0, 1);
      g.addArc(1, 2);
      g.addArc(3, 4);

      auto cc = g.connectedComponents();
      CHECK_EQ(cc.size(), 6U);

      CHECK_EQ(cc[0], cc[1]);
      CHECK_EQ(cc[1], cc[2]);
      CHECK_NE(cc[0], cc[3]);
      CHECK_EQ(cc[3], cc[4]);
      CHECK_NE(cc[5], cc[0]);
      CHECK_NE(cc[5], cc[3]);

      // a single-node graph has one component
      gum::DiGraph g2;
      g2.addNode();
      auto cc2 = g2.connectedComponents();
      CHECK_EQ(cc2.size(), 1U);
    }

    static void testTopologicalOrder() {
      // 0 -> 1 -> 3
      //  \-> 2 -> 3 -> 4
      gum::DiGraph g;
      g.addNodes(5);
      g.addArc(0, 1);
      g.addArc(0, 2);
      g.addArc(1, 3);
      g.addArc(2, 3);
      g.addArc(3, 4);

      auto order = g.topologicalOrder();
      CHECK_EQ(order.size(), 5U);

      // each node must appear exactly once
      for (gum::NodeId n = 0; n < 5; ++n)
        CHECK(order.exists(n));

      // every arc must go from an earlier to a later position
      for (const auto& arc: g.arcs())
        CHECK_LT(order.pos(arc.tail()), order.pos(arc.head()));

      // empty graph returns empty sequence
      gum::DiGraph empty;
      CHECK_EQ(empty.topologicalOrder().size(), 0U);

      // graph with a cycle must throw
      gum::DiGraph cyclic;
      cyclic.addNodes(3);
      cyclic.addArc(0, 1);
      cyclic.addArc(1, 2);
      cyclic.addArc(2, 0);
      CHECK_THROWS_AS(cyclic.topologicalOrder(), gum::InvalidDirectedCycle);
    }
  };

  GUM_TEST_ACTIF(Constructor1)
  GUM_TEST_ACTIF(Insert1)
  GUM_TEST_ACTIF(CopyConstructor)
  GUM_TEST_ACTIF(CopyOperator)
  GUM_TEST_ACTIF(EmptyNodes)
  GUM_TEST_ACTIF(EmptyArcs)
  GUM_TEST_ACTIF(ClearNodes)
  GUM_TEST_ACTIF(ClearArcs)
  GUM_TEST_ACTIF(AddDelNodes_2)
  GUM_TEST_ACTIF(RemoveNodes_1)
  GUM_TEST_ACTIF(AddDelArcs_2)
  GUM_TEST_ACTIF(GetNodes)
  GUM_TEST_ACTIF(GetArcs)
  GUM_TEST_ACTIF(NodeListMapNodes)
  GUM_TEST_ACTIF(TwistedNodeListMapNodes)
  GUM_TEST_ACTIF(HashMapNodes)
  GUM_TEST_ACTIF(TwistedHashMapNodes)
  GUM_TEST_ACTIF(ListMapArcs)
  GUM_TEST_ACTIF(HashMapArcs)
  GUM_TEST_ACTIF(NodeId)
  GUM_TEST_ACTIF(DirectedPaths)
  GUM_TEST_ACTIF(AncestorsDescendants)
  GUM_TEST_ACTIF(ConnectedComponents)
  GUM_TEST_ACTIF(TopologicalOrder)

}   // namespace gum_tests
