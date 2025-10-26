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
#pragma once


#include <iostream>

#include <gumtest/AgrumTestSuite.h>

#include <agrum/base/graphs/diGraph.h>
#include <agrum/base/graphs/graphElements.h>

// The graph used for the tests:
//          0   1_          0 -> 2
//         / \ / /          0 -> 3
//        2   3 /           2 -> 4
//         \ / /            3 -> 4
//          4_/             1 -> 3
//                          4 -> 1

namespace gum_tests {

  class GUM_TEST_SUITE(DiGraph) {
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

    GUM_ACTIVE_TEST(Constructor1) {
      gum::DiGraph* graph = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING((graph = new gum::DiGraph()))
      TS_GUM_ASSERT_THROWS_NOTHING((delete (graph)))
    }

    GUM_ACTIVE_TEST(Insert1) {
      gum::DiGraph graph;
      gum::NodeId  id1 = 0;
      gum::NodeId  id2 = 0;
      gum::NodeId  id3 = 0;
      gum::NodeId  id4 = 0;
      gum::NodeId  id5 = 0;

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
      TS_GUM_ASSERT_THROWS_NOTHING(graph.addArc(id5, id2))
    }

    GUM_ACTIVE_TEST(CopyConstructor) {
      gum::DiGraph graph = buildGraph();

      gum::DiGraph* copy = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING((copy = new gum::DiGraph(graph)))
      TS_ASSERT_EQUALS(graph, *copy)
      delete (copy);

      TS_GUM_ASSERT_THROWS_NOTHING(gum::DiGraph copy2 = graph)
      TS_GUM_ASSERT_THROWS_NOTHING(gum::DiGraph copy3(graph))
    }

    GUM_ACTIVE_TEST(CopyOperator) {
      gum::DiGraph graph = buildGraph();

      gum::DiGraph g2 = buildGraph();
      g2.addNode();

      gum::DiGraph g3;

      g2 = g3 = graph;

      TS_ASSERT_EQUALS(g2, graph)
      TS_ASSERT_EQUALS(g3, graph)

      g2.clear();
      g3.clearArcs();

      TS_ASSERT_DIFFERS(g2, graph)
      TS_ASSERT_DIFFERS(g3, graph)
    }

    GUM_ACTIVE_TEST(EmptyNodes) {
      gum::DiGraph graph;

      TS_ASSERT(graph.empty())
      graph = buildGraph();
      TS_ASSERT(!graph.empty())
    }

    GUM_ACTIVE_TEST(EmptyArcs) {
      gum::DiGraph graph;
      TS_ASSERT(graph.emptyArcs())
      graph = buildGraph();
      TS_ASSERT(!graph.emptyArcs())
    }

    GUM_ACTIVE_TEST(ClearNodes) {
      gum::DiGraph graph = buildGraph();
      TS_ASSERT(!graph.empty())
      TS_GUM_ASSERT_THROWS_NOTHING(graph.clear())
      TS_ASSERT(graph.empty() && graph.emptyArcs())
    }

    GUM_ACTIVE_TEST(ClearArcs) {
      gum::DiGraph graph = buildGraph();
      TS_ASSERT(!graph.emptyArcs())
      TS_GUM_ASSERT_THROWS_NOTHING(graph.clearArcs())
      TS_ASSERT(graph.emptyArcs())
      TS_ASSERT(!graph.empty())
    }

    GUM_ACTIVE_TEST(AddDelNodes_2) {
      gum::DiGraph graph = buildGraph();

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
      gum::DiGraph graph = buildGraph();

      gum::Size nodeCount = graph.size();
      gum::Size arcCount  = graph.sizeArcs();

      TS_ASSERT_EQUALS(nodeCount, static_cast< gum::Size >(5))
      TS_ASSERT_EQUALS(arcCount, static_cast< gum::Size >(6))

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
      gum::DiGraph graph = buildGraph();

      TS_ASSERT(graph.existsArc(id1, id4))
      TS_ASSERT(graph.existsArc(id4, id5))
      TS_ASSERT(graph.existsArc(id5, id2))

      gum::Size nodeCount = graph.size();
      gum::Size arcCount  = graph.sizeArcs();

      TS_GUM_ASSERT_THROWS_NOTHING(graph.eraseArc(gum::Arc(id5, id2)))

      TS_ASSERT_EQUALS(nodeCount, graph.size())
      TS_ASSERT_EQUALS(arcCount, graph.sizeArcs() + 1)

      TS_ASSERT(!graph.existsArc(id5, id2))
    }

    GUM_ACTIVE_TEST(GetNodes) {
      gum::DiGraph graph = buildGraph();

      gum::NodeSet nodelist = graph.asNodeSet();
      TS_ASSERT_EQUALS(nodelist.size(), graph.size())
      gum::Size nodeCount = graph.size();

      for (const auto node: nodelist)
        graph.eraseNode(node);

      TS_ASSERT(graph.empty())

      TS_ASSERT_EQUALS(nodeCount, nodelist.size())
    }

    GUM_ACTIVE_TEST(GetArcs) {
      gum::DiGraph graph = buildGraph();

      gum::ArcSet arclist = graph.arcs();
      TS_ASSERT_EQUALS(arclist.size(), graph.sizeArcs())
      gum::Size arcCount = graph.sizeArcs();

      for (const auto& arc: arclist)
        graph.eraseArc(arc);

      TS_ASSERT(graph.emptyArcs())

      TS_ASSERT_EQUALS(arcCount, arclist.size())
    }

    GUM_ACTIVE_TEST(NodeListMapNodes) {
      gum::DiGraph graph = buildGraph();

      gum::List< gum::Size > list = graph.listMapNodes(&simpleDoubleFunction);
      TS_ASSERT_EQUALS(list.size(), graph.size())

      gum::Size s = 0;

      for (gum::List< gum::Size >::iterator iter = list.begin(); iter != list.end(); ++iter) {
        s += *iter;
      }

      TS_ASSERT_EQUALS(s, (gum::Size)(2 * (id1 + id2 + id3 + id4 + id5)))
    }

    GUM_ACTIVE_TEST(TwistedNodeListMapNodes) {
      gum::DiGraph graph = buildGraph();

      gum::List< gum::Size > list;
      TS_ASSERT_THROWS_ANYTHING(list = graph.listMapNodes(&twistedMapFunction))

      TS_ASSERT_EQUALS(list.size(), static_cast< gum::Size >(0))
    }

    GUM_ACTIVE_TEST(HashMapNodes) {
      gum::DiGraph graph = buildGraph();

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
      gum::DiGraph graph = buildGraph();

      gum::NodeProperty< gum::Size > hashmap;
      TS_ASSERT_THROWS_ANYTHING(hashmap = graph.nodesPropertyFromFunction(&twistedMapFunction))

      TS_ASSERT_EQUALS(hashmap.size(), static_cast< gum::Size >(0))
    }

    GUM_ACTIVE_TEST(ListMapArcs) {
      gum::DiGraph graph = buildGraph();

      gum::List< gum::Size > list = graph.listMapArcs(&simpleArcMapFunction);
      TS_ASSERT_EQUALS(list.size(), graph.sizeArcs())

      gum::Size s = 0;

      for (gum::List< gum::Size >::iterator iter = list.begin(); iter != list.end(); ++iter) {
        s += *iter;
      }

      TS_ASSERT_EQUALS(s, (gum::Size)(0 + 0 + 2 + 3 + 1 + 4 + 2 + 3 + 4 + 4 + 3 + 1))
    }

    GUM_ACTIVE_TEST(HashMapArcs) {
      gum::DiGraph graph = buildGraph();

      gum::ArcProperty< gum::Size > hashmap = graph.arcsProperty(&simpleArcMapFunction);
      TS_ASSERT_EQUALS(hashmap.size(), graph.sizeArcs())

      gum::Size sk = 0;
      gum::Size sv = 0;

      for (const auto& elt: hashmap) {
        sv += elt.second;
        sk += elt.first.head() + elt.first.tail();
      }

      TS_ASSERT_EQUALS(sk, sv)
    }

    // this test should be in NodeGraphPartTestSuite.h which does not exist for
    // now
    // ...
    GUM_ACTIVE_TEST(NodeId) {
      gum::DiGraph graph = buildGraph();

      gum::NodeId id  = graph.nextNodeId();
      gum::NodeId id2 = graph.addNode();
      TS_ASSERT_EQUALS(id, id2)
    }

    GUM_ACTIVE_TEST(DirectedPaths) {
      try {
        gum::DiGraph graph = buildGraph();

        std::vector< gum::NodeId > path = graph.directedPath(0, 1);
        TS_ASSERT_EQUALS(path.size(), 4U)
        TS_ASSERT_EQUALS(path[0], 0U)
        TS_ASSERT_EQUALS(path[1], 3U)
        TS_ASSERT_EQUALS(path[2], 4U)
        TS_ASSERT_EQUALS(path[3], 1U)

        TS_ASSERT_EQUALS(graph.hasDirectedPath(0, 1), true)
        TS_ASSERT_EQUALS(graph.hasDirectedPath(0, 0), false)
        TS_ASSERT_EQUALS(graph.hasDirectedPath(0, 4), true)
        TS_ASSERT_EQUALS(graph.hasDirectedPath(1, 1), true)

        TS_ASSERT_THROWS(graph.directedPath(1, 2), const gum::NotFound&)

        std::vector< gum::NodeId > path2 = graph.directedUnorientedPath(1, 2);
        TS_ASSERT_EQUALS(path2.size(), 3U)
        TS_ASSERT_EQUALS(path2[0], 1U)
        TS_ASSERT_EQUALS(path2[1], 4U)
        TS_ASSERT_EQUALS(path2[2], 2U)
      } catch (gum::Exception& e) { GUM_SHOWERROR(e) }
    }

    // GUM_ACTIVE_TEST(ToDot) {
    //   gum::DiGraph graph = buildGraph();
    //   TS_GUM_ASSERT_THROWS_NOTHING(std::cout << graph.toDot())
    // }
  };
}   // namespace gum_tests
