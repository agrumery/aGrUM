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

#include <agrum/base/graphs/DAG.h>
#include <agrum/base/graphs/graphElements.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  DAG
#define GUM_CURRENT_MODULE GUMBASE

// The graph used for the tests:
//          0   1_          0 -> 2
//         / \ / /          0 -> 3
//        2   3 /           2 -> 4
//         \ / /            3 -> 5
//          4_/             1 -> 3
//                          1 -> 4

namespace gum_tests {

  struct DAGTestSuite {
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

    static void testConstructor1() {
      gum::DAG* graph = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING((graph = new gum::DAG()));
      GUM_CHECK_ASSERT_THROWS_NOTHING((delete (graph)));
    }

    static void testInsert1() {
      gum::DAG    graph;
      gum::NodeId id1, id2, id3, id4, id5;
      id1 = id2 = id3 = id4 = id5 = 0;

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
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addArc(id2, id5));
    }

    static void testInsert2() {
      gum::DAG    graph;
      gum::NodeId id1, id2, id3, id4, id5;
      id1 = id2 = id3 = id4 = id5 = 0;

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

      CHECK_THROWS_AS(graph.addArc(id5, id2), const gum::InvalidDirectedCycle&);
      CHECK_THROWS_AS(graph.addArc(1000, id2), const gum::InvalidNode&);
      CHECK_THROWS_AS(graph.addArc(id2, 1000), const gum::InvalidNode&);
    }

    void testCopyConstructor() {
      gum::DAG graph = buildGraph();

      gum::DAG* copy = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(copy = new gum::DAG(graph));
      CHECK((graph) == (*copy));
      delete (copy);

      GUM_CHECK_ASSERT_THROWS_NOTHING(gum::DAG copy2 = graph);
      GUM_CHECK_ASSERT_THROWS_NOTHING(gum::DAG copy3(graph));
    }

    void testEmptyNodes() {
      gum::DAG graph;

      CHECK(graph.empty());
      graph = buildGraph();
      CHECK(!graph.empty());
    }

    void testEmptyArcs() {
      gum::DAG graph;
      CHECK(graph.emptyArcs());
      graph = buildGraph();
      CHECK(!graph.emptyArcs());
    }

    void testClearNodes() {
      gum::DAG graph = buildGraph();
      CHECK(!graph.empty());
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.clear());
      CHECK((graph.empty() && graph.emptyArcs()));
    }

    void testClearArcs() {
      gum::DAG graph = buildGraph();
      CHECK(!graph.emptyArcs());
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.clearArcs());
      CHECK(graph.emptyArcs());
      CHECK(!graph.empty());
    }

    void testAddDelNodes_2() {
      gum::DAG graph = buildGraph();

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

      CHECK((nodeCount) == (graph.size() + 1));
      CHECK((arcCount) == (graph.sizeArcs() + 2));

      CHECK(!graph.exists(id2));
      CHECK(!graph.existsArc(id2, id4));
      CHECK(!graph.existsArc(id5, id2));
    }

    void testRemoveNodes_1() {
      gum::DAG graph = buildGraph();

      gum::Size nodeCount = graph.size();
      gum::Size arcCount  = graph.sizeArcs();

      CHECK((nodeCount) == (static_cast< gum::Size >(5)));
      CHECK((arcCount) == (static_cast< gum::Size >(6)));

      for (int i = 0; i < 10; i++) {
        GUM_CHECK_ASSERT_THROWS_NOTHING(graph.eraseNode(id5));
      }

      CHECK((nodeCount) == (graph.size() + 1));

      CHECK((arcCount) == (graph.sizeArcs() + 3));

      CHECK(!graph.existsArc(id3, id5));
      CHECK(!graph.existsArc(id4, id5));
      CHECK(!graph.existsArc(id5, id2));
    }

    void testAddDelArcs_2() {
      gum::DAG graph = buildGraph();

      CHECK(graph.existsArc(id1, id4));
      CHECK(graph.existsArc(id4, id5));
      CHECK(graph.existsArc(id2, id5));

      gum::Size nodeCount = graph.size();
      gum::Size arcCount  = graph.sizeArcs();

      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.eraseArc(gum::Arc(id2, id5)));

      CHECK((nodeCount) == (graph.size()));
      CHECK((arcCount) == (graph.sizeArcs() + 1));

      CHECK(!graph.existsArc(id2, id5));
    }

    void testGetNodes() {
      gum::DAG graph = buildGraph();

      gum::NodeSet nodelist = graph.asNodeSet();
      CHECK((nodelist.size()) == (graph.size()));
      gum::Size nodeCount = graph.size();

      for (const auto node: nodelist)
        graph.eraseNode(node);

      CHECK(graph.empty());

      CHECK((nodeCount) == (nodelist.size()));
    }

    void testGetArcs() {
      gum::DAG graph = buildGraph();

      gum::ArcSet arclist = graph.arcs();
      CHECK((arclist.size()) == (graph.sizeArcs()));
      gum::Size arcCount = graph.sizeArcs();

      for (const auto& arc: arclist) {
        graph.eraseArc(arc);
      }

      CHECK(graph.emptyArcs());

      CHECK((arcCount) == (arclist.size()));
    }

    void testNodeListMapNodes() {
      gum::DAG graph = buildGraph();

      auto list = graph.listMapNodes(&simpleDoubleFunction);
      CHECK((list.size()) == (graph.size()));

      gum::Size s = 0;

      for (auto iter = list.begin(); iter != list.end(); ++iter)
        s += *iter;

      CHECK((s) == (2 * (id1 + id2 + id3 + id4 + id5)));
    }

    void testTwistedNodeListMapNodes() {
      gum::DAG graph = buildGraph();

      gum::List< gum::Size > list;
      CHECK_THROWS(list = graph.listMapNodes(&twistedMapFunction));

      CHECK((list.size()) == (static_cast< gum::Size >(0)));
    }

    void testHashMapNodes() {
      gum::DAG graph = buildGraph();

      auto hashmap = graph.nodesPropertyFromFunction(&simpleDoubleFunction);
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
      gum::DAG graph = buildGraph();

      gum::NodeProperty< gum::Size > hashmap;
      CHECK_THROWS(hashmap = graph.nodesPropertyFromFunction(&twistedMapFunction));

      CHECK((hashmap.size()) == (static_cast< gum::Size >(0)));
    }

    void testListMapArcs() {
      gum::DAG graph = buildGraph();

      gum::List< gum::Size > list = graph.listMapArcs(&simpleArcMapFunction);
      CHECK((list.size()) == (graph.sizeArcs()));

      gum::Size s = 0;

      for (auto iter = list.begin(); iter != list.end(); ++iter)
        s += *iter;

      CHECK((s) == ((gum::Size)(0 + 0 + 2 + 3 + 1 + 4 + 2 + 3 + 4 + 4 + 3 + 1)));
    }

    void testHashMapArcs() {
      gum::DAG graph = buildGraph();

      auto hashmap = graph.arcsProperty(&simpleArcMapFunction);
      CHECK((hashmap.size()) == (graph.sizeArcs()));

      gum::Size sk = 0;
      gum::Size sv = 0;

      for (const auto& elt: hashmap) {
        sk += elt.first.head() + elt.first.tail();
        sv += elt.second;
      }

      CHECK((sk) == (sv));
    }

    void testCopyOperator() {
      gum::DAG graph = buildGraph();

      gum::DAG g2 = buildGraph();
      g2.addNode();

      gum::DAG g3;

      g2 = g3 = graph;

      CHECK((g2) == (graph));
      CHECK((g3) == (graph));

      g2.clear();
      g3.clearArcs();

      CHECK((g2) != (graph));
      CHECK((g3) != (graph));
    }

    void testFamily() {
      // The graph used for the tests:
      //          0   1_          0 -> 2
      //         / \ / /          0 -> 3
      //        2   3 /           2 -> 4
      //         \ / /            3 -> 4
      //          4_/             1 -> 3
      //                          1 -> 4
      gum::DAG graph = buildGraph();
      CHECK((graph.family(0)) == (gum::NodeSet({0})));
      CHECK((graph.family(4)) == (gum::NodeSet({1, 2, 3, 4})));
      auto seq = graph.topologicalOrder();
    }

    void testMonoCycle() {
      gum::DAG graph;
      auto     x = graph.addNode();
      CHECK_THROWS_AS(graph.addArc(x, x), const gum::Exception&);
    }
  };

  GUM_TEST_ACTIF(Constructor1)
  GUM_TEST_ACTIF(Insert1)
  GUM_TEST_ACTIF(Insert2)
  GUM_TEST_ACTIF(CopyConstructor)
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
  GUM_TEST_ACTIF(CopyOperator)
  GUM_TEST_ACTIF(Family)
  GUM_TEST_ACTIF(MonoCycle)
}   // namespace gum_tests
