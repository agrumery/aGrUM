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
#include <agrum/base/graphs/undiGraph.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  UndiGraph
#define GUM_CURRENT_MODULE GUMBASE

// The graph used for the tests:
//          0   1_          0 -- 2
//         / \ / /          0 -- 3
//        2   3 /           2 -- 4
//         \ / /            3 -- 4
//          4_/             1 -- 3
//                          4 -- 1

namespace gum_tests {

  struct UndiGraphTestSuite {
    private:
    static gum::Size simpleDoubleFunction(const gum::NodeId& aNodeId) { return aNodeId * 2; }

    static gum::Size simpleEdgeMapFunction(const gum::Edge& anEdge) {
      return anEdge.first() + anEdge.second();
    }

    static gum::Size twistedMapFunction(const gum::NodeId& aNode) { throw(aNode); }

    gum::UndiGraph buildGraph() {
      gum::UndiGraph g;
      id1 = g.addNode();
      id2 = g.addNode();
      id3 = g.addNode();
      id4 = g.addNode();
      id5 = g.addNode();

      g.addEdge(id1, id3);
      g.addEdge(id3, id5);
      g.addEdge(id2, id4);
      g.addEdge(id1, id4);
      g.addEdge(id4, id5);
      g.addEdge(id5, id2);

      return g;
    }

    public:
    gum::NodeId id1, id2, id3, id4, id5;

    static void testConstructor1() {
      gum::UndiGraph* graph = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING((graph = new gum::UndiGraph()));
      GUM_CHECK_ASSERT_THROWS_NOTHING(delete (graph));
    }

    static void testInsert1() {
      gum::UndiGraph graph;

      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addNode());
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addNode());
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addNode());
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addNode());
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addNode());

      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addEdge(0, 2));
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addEdge(2, 4));
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addEdge(1, 3));

      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addEdge(0, 3));
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addEdge(3, 4));
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addEdge(4, 1));
    }

    void testCopyConstructor() {
      gum::UndiGraph graph = buildGraph();

      gum::UndiGraph* copy = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING((copy = new gum::UndiGraph(graph)));
      CHECK((graph) == (*copy));
      delete (copy);

      GUM_CHECK_ASSERT_THROWS_NOTHING(gum::UndiGraph copy2 = graph);
      GUM_CHECK_ASSERT_THROWS_NOTHING(gum::UndiGraph copy3(graph));
    }

    void testCopyOperator() {
      gum::UndiGraph graph = buildGraph();

      gum::UndiGraph g2 = buildGraph();
      g2.addNode();

      gum::UndiGraph g3;

      g2 = g3 = graph;

      CHECK((g2) == (graph));
      CHECK((g3) == (graph));

      g2.clear();
      g3.clearEdges();

      CHECK((g2) != (graph));
      CHECK((g3) != (graph));
    }

    void testEmptyNodes() {
      gum::UndiGraph graph;
      CHECK(graph.empty());
      graph = buildGraph();
      CHECK(!graph.empty());
    }

    void testEmptyEdges() {
      gum::UndiGraph graph;
      CHECK(graph.emptyEdges());
      graph = buildGraph();
      CHECK(!graph.emptyEdges());
    }

    void testClearNodes() {
      gum::UndiGraph graph = buildGraph();
      CHECK(!graph.empty());
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.clear());
      CHECK(graph.empty());
    }

    void testClearEdges() {
      gum::UndiGraph graph = buildGraph();
      CHECK(!graph.emptyEdges());
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.clearEdges());
      CHECK(graph.emptyEdges());
      CHECK(!graph.empty());
    }

    void testAddDelNodes_2() {
      gum::UndiGraph graph = buildGraph();

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

      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.eraseNode(id2));

      CHECK((nodeCount) == (graph.size() + 1));
      CHECK((edgeCount) == (graph.sizeEdges() + 2));

      CHECK(!graph.exists(id2));
      CHECK(!graph.existsEdge(id2, id4));
      CHECK(!graph.existsEdge(id5, id2));
      CHECK(!graph.existsEdge(id2, id5));
    }

    void testRemoveNodesFunky_1() {
      gum::UndiGraph graph = buildGraph();

      gum::Size nodeCount = graph.size();
      gum::Size edgeCount = graph.sizeEdges();

      CHECK((nodeCount) == (static_cast< gum::Size >(5)));
      CHECK((edgeCount) == (static_cast< gum::Size >(6)));

      for (int i = 0; i < 10; i++) {
        GUM_CHECK_ASSERT_THROWS_NOTHING(graph.eraseNode(id5));
      }

      CHECK((nodeCount) == (graph.size() + 1));

      CHECK((edgeCount) == (graph.sizeEdges() + 3));

      CHECK(!graph.existsEdge(2, 4));
      CHECK(!graph.existsEdge(3, 4));
      CHECK(!graph.existsEdge(4, 1));
    }

    void testAddDelEdges_1() {
      gum::UndiGraph graph = buildGraph();

      CHECK(graph.existsEdge(0, 2));
      CHECK(graph.existsEdge(2, 4));
      CHECK(graph.existsEdge(1, 3));

      gum::Size nodeCount = graph.size();
      gum::Size edgeCount = graph.sizeEdges();

      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.eraseEdge(gum::Edge(4, 2)));

      CHECK((nodeCount) == (graph.size()));
      CHECK((edgeCount) == (graph.sizeEdges() + 1));

      CHECK(!graph.existsEdge(2, 4));
    }

    void testAddDelEdges_2() {
      gum::UndiGraph graph = buildGraph();

      CHECK(graph.existsEdge(id1, id3));
      CHECK(graph.existsEdge(id3, id5));
      CHECK(graph.existsEdge(id2, id4));

      gum::Size nodeCount = graph.size();
      gum::Size edgeCount = graph.sizeEdges();

      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.eraseEdge(gum::Edge(id3, id5)));

      CHECK((nodeCount) == (graph.size()));
      CHECK((edgeCount) == (graph.sizeEdges() + 1));

      CHECK(!graph.existsEdge(id3, id5));
    }

    void testGetNodes() {
      gum::UndiGraph graph = buildGraph();

      const gum::NodeSet nodeset = graph.asNodeSet();
      CHECK((nodeset.size()) == (graph.size()));
      gum::Size nodeCount = graph.size();

      for (const auto node: nodeset)
        graph.eraseNode(node);

      CHECK(graph.empty());

      CHECK((nodeCount) == (nodeset.size()));
    }

    void testGetEdges() {
      gum::UndiGraph graph = buildGraph();

      gum::EdgeSet edgeset = graph.edges();
      CHECK((edgeset.size()) == (graph.sizeEdges()));
      gum::Size edgeCount = graph.sizeEdges();

      for (const auto& edge: edgeset)
        graph.eraseEdge(edge);

      CHECK(graph.emptyEdges());

      CHECK((edgeCount) == (edgeset.size()));
    }

    void testNodeListMapNodes() {
      gum::UndiGraph graph = buildGraph();

      gum::List< gum::Size > list = graph.listMapNodes(&simpleDoubleFunction);
      CHECK((list.size()) == (graph.size()));

      gum::Size s = 0;

      for (gum::List< gum::Size >::iterator iter = list.begin(); iter != list.end(); ++iter) {
        s += *iter;
      }

      CHECK((s) == (2 * (id1 + id2 + id3 + id4 + id5)));
    }

    void testTwistedNodeListMapNodes() {
      gum::UndiGraph graph = buildGraph();

      gum::List< gum::Size > list;
      CHECK_THROWS(list = graph.listMapNodes(&twistedMapFunction));

      CHECK((list.size()) == (static_cast< gum::Size >(0)));
    }

    void testHashMapNodes() {
      gum::UndiGraph graph = buildGraph();

      gum::NodeProperty< gum::Size > hashmap
          = graph.nodesPropertyFromFunction(&simpleDoubleFunction);
      CHECK((hashmap.size()) == (graph.size()));

      gum::Size sk = 0;
      gum::Size sv = 0;

      for (const auto& elt: hashmap) {
        sk += elt.first;
        sv += elt.second;
      }

      CHECK((sk * 2) == (sv));
    }

    void testTwistedHashMapNodes() {
      gum::UndiGraph graph = buildGraph();

      gum::NodeProperty< gum::Size > hashmap;
      CHECK_THROWS(hashmap = graph.nodesPropertyFromFunction(&twistedMapFunction));

      CHECK((hashmap.size()) == (static_cast< gum::Size >(0)));
    }

    void testListMapEdges() {
      gum::UndiGraph graph = buildGraph();

      gum::List< gum::Size > list = graph.listMapEdges(&simpleEdgeMapFunction);
      CHECK((list.size()) == (graph.sizeEdges()));

      gum::Size s = 0;

      for (gum::List< gum::Size >::iterator iter = list.begin(); iter != list.end(); ++iter) {
        s += *iter;
      }

      CHECK((s) == ((gum::Size)(0 + 0 + 2 + 3 + 1 + 4 + 2 + 3 + 4 + 4 + 3 + 1)));
    }

    void testHashMapEdges() {
      gum::UndiGraph graph = buildGraph();

      gum::EdgeProperty< gum::Size > hashmap = graph.edgesProperty(&simpleEdgeMapFunction);
      CHECK((hashmap.size()) == (graph.sizeEdges()));

      gum::Size sk = 0;
      gum::Size sv = 0;

      for (const auto& elt: hashmap) {
        sv += elt.second;
        sk += elt.first.first() + elt.first.second();
      }

      CHECK((sk) == (sv));
    }

    void testUndirectedPaths() {
      gum::UndiGraph graph = buildGraph();
      gum::NodeId    id6   = graph.addNode();
      gum::NodeId    id7   = graph.addNode();
      graph.addEdge(id6, id7);

      std::vector< gum::NodeId > path = graph.undirectedPath(0, 1);
      CHECK((path.size()) == (3U));
      CHECK((path[0]) == (0U));
      CHECK((path[1]) == (3U));
      CHECK((path[2]) == (1U));

      std::vector< gum::NodeId > path2 = graph.undirectedPath(1, 2);
      CHECK((path2.size()) == (3U));
      CHECK((path2[0]) == (1U));
      CHECK((path2[1]) == (4U));
      CHECK((path2[2]) == (2U));

      std::vector< gum::NodeId > path3 = graph.undirectedPath(5, 6);
      CHECK((path3.size()) == (2U));

      CHECK_THROWS_AS(graph.undirectedPath(1, 5), const gum::NotFound&);
    }

    static void testConnexComponents() {
      gum::UndiGraph g;
      g.addNodes(6);

      g.addEdge(0, 1);
      g.addEdge(2, 1);
      g.addEdge(3, 4);

      auto cc = g.nodes2ConnectedComponent();

      CHECK((cc.size()) == (6U));
      CHECK((cc[0]) != (cc[3]));
      CHECK((cc[0]) != (cc[5]));
      CHECK((cc[5]) != (cc[3]));
      CHECK((cc[0]) == (cc[1]));
      CHECK((cc[0]) == (cc[2]));
      CHECK((cc[3]) == (cc[4]));
    }

    static void testConnexComponents2() {
      gum::UndiGraph g;
      g.addNodes(6);

      g.addEdge(0, 1);
      g.addEdge(1, 2);
      g.addEdge(2, 3);
      g.addEdge(1, 4);
      g.addEdge(2, 5);

      auto cc = g.nodes2ConnectedComponent();

      CHECK((cc.size()) == (6U));
      CHECK((cc[0]) == (cc[1]));
      CHECK((cc[0]) == (cc[2]));
      CHECK((cc[0]) == (cc[3]));
      CHECK((cc[0]) == (cc[4]));
      CHECK((cc[0]) == (cc[5]));
    }
  };

  GUM_TEST_ACTIF(Constructor1)
  GUM_TEST_ACTIF(Insert1)
  GUM_TEST_ACTIF(CopyConstructor)
  GUM_TEST_ACTIF(CopyOperator)
  GUM_TEST_ACTIF(EmptyNodes)
  GUM_TEST_ACTIF(EmptyEdges)
  GUM_TEST_ACTIF(ClearNodes)
  GUM_TEST_ACTIF(ClearEdges)
  GUM_TEST_ACTIF(AddDelNodes_2)
  GUM_TEST_ACTIF(RemoveNodesFunky_1)
  GUM_TEST_ACTIF(AddDelEdges_1)
  GUM_TEST_ACTIF(AddDelEdges_2)
  GUM_TEST_ACTIF(GetNodes)
  GUM_TEST_ACTIF(GetEdges)
  GUM_TEST_ACTIF(NodeListMapNodes)
  GUM_TEST_ACTIF(TwistedNodeListMapNodes)
  GUM_TEST_ACTIF(HashMapNodes)
  GUM_TEST_ACTIF(TwistedHashMapNodes)
  GUM_TEST_ACTIF(ListMapEdges)
  GUM_TEST_ACTIF(HashMapEdges)
  GUM_TEST_ACTIF(UndirectedPaths)
  GUM_TEST_ACTIF(ConnexComponents)
  GUM_TEST_ACTIF(ConnexComponents2)
}   // namespace gum_tests
