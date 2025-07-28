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
#include <agrum/base/graphs/undiGraph.h>

// The graph used for the tests:
//          0   1_          0 -- 2
//         / \ / /          0 -- 3
//        2   3 /           2 -- 4
//         \ / /            3 -- 4
//          4_/             1 -- 3
//                          4 -- 1

namespace gum_tests {

  class [[maybe_unused]] UndiGraphTestSuite: public CxxTest::TestSuite {
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

    GUM_ACTIVE_TEST(Constructor1) {
      gum::UndiGraph* graph = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING((graph = new gum::UndiGraph()))
      TS_GUM_ASSERT_THROWS_NOTHING(delete (graph))
    }

    GUM_ACTIVE_TEST(Insert1) {
      gum::UndiGraph graph;

      TS_GUM_ASSERT_THROWS_NOTHING(graph.addNode())
      TS_GUM_ASSERT_THROWS_NOTHING(graph.addNode())
      TS_GUM_ASSERT_THROWS_NOTHING(graph.addNode())
      TS_GUM_ASSERT_THROWS_NOTHING(graph.addNode())
      TS_GUM_ASSERT_THROWS_NOTHING(graph.addNode())

      TS_GUM_ASSERT_THROWS_NOTHING(graph.addEdge(0, 2))
      TS_GUM_ASSERT_THROWS_NOTHING(graph.addEdge(2, 4))
      TS_GUM_ASSERT_THROWS_NOTHING(graph.addEdge(1, 3))

      TS_GUM_ASSERT_THROWS_NOTHING(graph.addEdge(0, 3))
      TS_GUM_ASSERT_THROWS_NOTHING(graph.addEdge(3, 4))
      TS_GUM_ASSERT_THROWS_NOTHING(graph.addEdge(4, 1))
    }

    GUM_ACTIVE_TEST(CopyConstructor) {
      gum::UndiGraph graph = buildGraph();

      gum::UndiGraph* copy = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING((copy = new gum::UndiGraph(graph)))
      TS_ASSERT_EQUALS(graph, *copy)
      delete (copy);

      TS_GUM_ASSERT_THROWS_NOTHING(gum::UndiGraph copy2 = graph)
      TS_GUM_ASSERT_THROWS_NOTHING(gum::UndiGraph copy3(graph))
    }

    GUM_ACTIVE_TEST(CopyOperator) {
      gum::UndiGraph graph = buildGraph();

      gum::UndiGraph g2 = buildGraph();
      g2.addNode();

      gum::UndiGraph g3;

      g2 = g3 = graph;

      TS_ASSERT_EQUALS(g2, graph)
      TS_ASSERT_EQUALS(g3, graph)

      g2.clear();
      g3.clearEdges();

      TS_ASSERT_DIFFERS(g2, graph)
      TS_ASSERT_DIFFERS(g3, graph)
    }

    GUM_ACTIVE_TEST(EmptyNodes) {
      gum::UndiGraph graph;
      TS_ASSERT(graph.empty())
      graph = buildGraph();
      TS_ASSERT(!graph.empty())
    }

    GUM_ACTIVE_TEST(EmptyEdges) {
      gum::UndiGraph graph;
      TS_ASSERT(graph.emptyEdges())
      graph = buildGraph();
      TS_ASSERT(!graph.emptyEdges())
    }

    GUM_ACTIVE_TEST(ClearNodes) {
      gum::UndiGraph graph = buildGraph();
      TS_ASSERT(!graph.empty())
      TS_GUM_ASSERT_THROWS_NOTHING(graph.clear())
      TS_ASSERT(graph.empty())
    }

    GUM_ACTIVE_TEST(ClearEdges) {
      gum::UndiGraph graph = buildGraph();
      TS_ASSERT(!graph.emptyEdges())
      TS_GUM_ASSERT_THROWS_NOTHING(graph.clearEdges())
      TS_ASSERT(graph.emptyEdges())
      TS_ASSERT(!graph.empty())
    }

    GUM_ACTIVE_TEST(AddDelNodes_2) {
      gum::UndiGraph graph = buildGraph();

      TS_ASSERT(graph.exists(id1))
      TS_ASSERT(graph.exists(id2))
      TS_ASSERT(graph.exists(id3))
      TS_ASSERT(graph.exists(id4))
      TS_ASSERT(graph.exists(id5))
      TS_ASSERT(!graph.exists(id5 + id4 + id3 + id2 + id1))

      TS_ASSERT(graph.existsEdge(id3, id5))
      TS_ASSERT(graph.existsEdge(id5, id3))
      TS_ASSERT(!graph.existsEdge(id1, id1))

      gum::Size nodeCount = graph.size();
      gum::Size edgeCount = graph.sizeEdges();

      TS_GUM_ASSERT_THROWS_NOTHING(graph.eraseNode(id2))

      TS_ASSERT_EQUALS(nodeCount, graph.size() + 1)
      TS_ASSERT_EQUALS(edgeCount, graph.sizeEdges() + 2)

      TS_ASSERT(!graph.exists(id2))
      TS_ASSERT(!graph.existsEdge(id2, id4))
      TS_ASSERT(!graph.existsEdge(id5, id2))
      TS_ASSERT(!graph.existsEdge(id2, id5))
    }

    GUM_ACTIVE_TEST(RemoveNodesFunky_1) {
      gum::UndiGraph graph = buildGraph();

      gum::Size nodeCount = graph.size();
      gum::Size edgeCount = graph.sizeEdges();

      TS_ASSERT_EQUALS(nodeCount, (gum::Size)5)
      TS_ASSERT_EQUALS(edgeCount, (gum::Size)6)

      for (int i = 0; i < 10; i++) {
        TS_GUM_ASSERT_THROWS_NOTHING(graph.eraseNode(id5))
      }

      TS_ASSERT_EQUALS(nodeCount, graph.size() + 1)

      TS_ASSERT_EQUALS(edgeCount, graph.sizeEdges() + 3)

      TS_ASSERT(!graph.existsEdge(2, 4))
      TS_ASSERT(!graph.existsEdge(3, 4))
      TS_ASSERT(!graph.existsEdge(4, 1))
    }

    GUM_ACTIVE_TEST(AddDelEdges_1) {
      gum::UndiGraph graph = buildGraph();

      TS_ASSERT(graph.existsEdge(0, 2))
      TS_ASSERT(graph.existsEdge(2, 4))
      TS_ASSERT(graph.existsEdge(1, 3))

      gum::Size nodeCount = graph.size();
      gum::Size edgeCount = graph.sizeEdges();

      TS_GUM_ASSERT_THROWS_NOTHING(graph.eraseEdge(gum::Edge(4, 2)))

      TS_ASSERT_EQUALS(nodeCount, graph.size())
      TS_ASSERT_EQUALS(edgeCount, graph.sizeEdges() + 1)

      TS_ASSERT(!graph.existsEdge(2, 4))
    }

    GUM_ACTIVE_TEST(AddDelEdges_2) {
      gum::UndiGraph graph = buildGraph();

      TS_ASSERT(graph.existsEdge(id1, id3))
      TS_ASSERT(graph.existsEdge(id3, id5))
      TS_ASSERT(graph.existsEdge(id2, id4))

      gum::Size nodeCount = graph.size();
      gum::Size edgeCount = graph.sizeEdges();

      TS_GUM_ASSERT_THROWS_NOTHING(graph.eraseEdge(gum::Edge(id3, id5)))

      TS_ASSERT_EQUALS(nodeCount, graph.size())
      TS_ASSERT_EQUALS(edgeCount, graph.sizeEdges() + 1)

      TS_ASSERT(!graph.existsEdge(id3, id5))
    }

    GUM_ACTIVE_TEST(GetNodes) {
      gum::UndiGraph graph = buildGraph();

      const gum::NodeSet nodeset = graph.asNodeSet();
      TS_ASSERT_EQUALS(nodeset.size(), graph.size())
      gum::Size nodeCount = graph.size();

      for (const auto node: nodeset)
        graph.eraseNode(node);

      TS_ASSERT(graph.empty())

      TS_ASSERT_EQUALS(nodeCount, nodeset.size())
    }

    GUM_ACTIVE_TEST(GetEdges) {
      gum::UndiGraph graph = buildGraph();

      gum::EdgeSet edgeset = graph.edges();
      TS_ASSERT_EQUALS(edgeset.size(), graph.sizeEdges())
      gum::Size edgeCount = graph.sizeEdges();

      for (const auto& edge: edgeset)
        graph.eraseEdge(edge);

      TS_ASSERT(graph.emptyEdges())

      TS_ASSERT_EQUALS(edgeCount, edgeset.size())
    }

    GUM_ACTIVE_TEST(NodeListMapNodes) {
      gum::UndiGraph graph = buildGraph();

      gum::List< gum::Size > list = graph.listMapNodes(&simpleDoubleFunction);
      TS_ASSERT_EQUALS(list.size(), graph.size())

      gum::Size s = 0;

      for (gum::List< gum::Size >::iterator iter = list.begin(); iter != list.end(); ++iter) {
        s += *iter;
      }

      TS_ASSERT_EQUALS(s, 2 * (id1 + id2 + id3 + id4 + id5))
    }

    GUM_ACTIVE_TEST(TwistedNodeListMapNodes) {
      gum::UndiGraph graph = buildGraph();

      gum::List< gum::Size > list;
      TS_ASSERT_THROWS_ANYTHING(list = graph.listMapNodes(&twistedMapFunction))

      TS_ASSERT_EQUALS(list.size(), (gum::Size)0)
    }

    GUM_ACTIVE_TEST(HashMapNodes) {
      gum::UndiGraph graph = buildGraph();

      gum::NodeProperty< gum::Size > hashmap
          = graph.nodesPropertyFromFunction(&simpleDoubleFunction);
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
      gum::UndiGraph graph = buildGraph();

      gum::NodeProperty< gum::Size > hashmap;
      TS_ASSERT_THROWS_ANYTHING(hashmap = graph.nodesPropertyFromFunction(&twistedMapFunction))

      TS_ASSERT_EQUALS(hashmap.size(), (gum::Size)0)
    }

    GUM_ACTIVE_TEST(ListMapEdges) {
      gum::UndiGraph graph = buildGraph();

      gum::List< gum::Size > list = graph.listMapEdges(&simpleEdgeMapFunction);
      TS_ASSERT_EQUALS(list.size(), graph.sizeEdges())

      gum::Size s = 0;

      for (gum::List< gum::Size >::iterator iter = list.begin(); iter != list.end(); ++iter) {
        s += *iter;
      }

      TS_ASSERT_EQUALS(s, (gum::Size)(0 + 0 + 2 + 3 + 1 + 4 + 2 + 3 + 4 + 4 + 3 + 1))
    }

    GUM_ACTIVE_TEST(HashMapEdges) {
      gum::UndiGraph graph = buildGraph();

      gum::EdgeProperty< gum::Size > hashmap = graph.edgesProperty(&simpleEdgeMapFunction);
      TS_ASSERT_EQUALS(hashmap.size(), graph.sizeEdges())

      gum::Size sk = 0;
      gum::Size sv = 0;

      for (const auto& elt: hashmap) {
        sv += elt.second;
        sk += elt.first.first() + elt.first.second();
      }

      TS_ASSERT_EQUALS(sk, sv)
    }

    GUM_ACTIVE_TEST(UndirectedPaths) {
      gum::UndiGraph graph = buildGraph();
      gum::NodeId    id6   = graph.addNode();
      gum::NodeId    id7   = graph.addNode();
      graph.addEdge(id6, id7);

      std::vector< gum::NodeId > path = graph.undirectedPath(0, 1);
      TS_ASSERT_EQUALS(path.size(), 3U)
      TS_ASSERT_EQUALS(path[0], 0U)
      TS_ASSERT_EQUALS(path[1], 3U)
      TS_ASSERT_EQUALS(path[2], 1U)

      std::vector< gum::NodeId > path2 = graph.undirectedPath(1, 2);
      TS_ASSERT_EQUALS(path2.size(), 3U)
      TS_ASSERT_EQUALS(path2[0], 1U)
      TS_ASSERT_EQUALS(path2[1], 4U)
      TS_ASSERT_EQUALS(path2[2], 2U)

      std::vector< gum::NodeId > path3 = graph.undirectedPath(5, 6);
      TS_ASSERT_EQUALS(path3.size(), 2U)

      TS_ASSERT_THROWS(graph.undirectedPath(1, 5), const gum::NotFound&)
    }

    GUM_ACTIVE_TEST(ConnexComponents) {
      gum::UndiGraph g;
      g.addNodes(6);

      g.addEdge(0, 1);
      g.addEdge(2, 1);
      g.addEdge(3, 4);

      auto cc = g.nodes2ConnectedComponent();

      TS_ASSERT_EQUALS(cc.size(), 6U)
      TS_ASSERT_DIFFERS(cc[0], cc[3])
      TS_ASSERT_DIFFERS(cc[0], cc[5])
      TS_ASSERT_DIFFERS(cc[5], cc[3])
      TS_ASSERT_EQUALS(cc[0], cc[1])
      TS_ASSERT_EQUALS(cc[0], cc[2])
      TS_ASSERT_EQUALS(cc[3], cc[4])
    }

    GUM_ACTIVE_TEST(ConnexComponents2) {
      gum::UndiGraph g;
      g.addNodes(6);

      g.addEdge(0, 1);
      g.addEdge(1, 2);
      g.addEdge(2, 3);
      g.addEdge(1, 4);
      g.addEdge(2, 5);

      auto cc = g.nodes2ConnectedComponent();

      TS_ASSERT_EQUALS(cc.size(), 6U)
      TS_ASSERT_EQUALS(cc[0], cc[1])
      TS_ASSERT_EQUALS(cc[0], cc[2])
      TS_ASSERT_EQUALS(cc[0], cc[3])
      TS_ASSERT_EQUALS(cc[0], cc[4])
      TS_ASSERT_EQUALS(cc[0], cc[5])
    }
  };
}   // namespace gum_tests
