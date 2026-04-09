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
#include <agrum/base/graphs/algorithms/generic/minimalConditioningSet.h>
#include <agrum/base/graphs/graphElements.h>
#include <agrum/base/graphs/algorithms/generic/bayesBall.h>

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
      CHECK_EQ(graph, *copy);
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

      CHECK_EQ(nodeCount, graph.size() + 1);
      CHECK_EQ(arcCount, graph.sizeArcs() + 2);

      CHECK(!graph.exists(id2));
      CHECK(!graph.existsArc(id2, id4));
      CHECK(!graph.existsArc(id5, id2));
    }

    void testRemoveNodes_1() {
      gum::DAG graph = buildGraph();

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
      gum::DAG graph = buildGraph();

      CHECK(graph.existsArc(id1, id4));
      CHECK(graph.existsArc(id4, id5));
      CHECK(graph.existsArc(id2, id5));

      gum::Size nodeCount = graph.size();
      gum::Size arcCount  = graph.sizeArcs();

      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.eraseArc(gum::Arc(id2, id5)));

      CHECK_EQ(nodeCount, graph.size());
      CHECK_EQ(arcCount, graph.sizeArcs() + 1);

      CHECK(!graph.existsArc(id2, id5));
    }

    void testGetNodes() {
      gum::DAG graph = buildGraph();

      gum::NodeSet nodelist = graph.asNodeSet();
      CHECK_EQ(nodelist.size(), graph.size());
      gum::Size nodeCount = graph.size();

      for (const auto node: nodelist)
        graph.eraseNode(node);

      CHECK(graph.empty());

      CHECK_EQ(nodeCount, nodelist.size());
    }

    void testGetArcs() {
      gum::DAG graph = buildGraph();

      gum::ArcSet arclist = graph.arcs();
      CHECK_EQ(arclist.size(), graph.sizeArcs());
      gum::Size arcCount = graph.sizeArcs();

      for (const auto& arc: arclist) {
        graph.eraseArc(arc);
      }

      CHECK(graph.emptyArcs());

      CHECK_EQ(arcCount, arclist.size());
    }

    void testNodeListMapNodes() {
      gum::DAG graph = buildGraph();

      auto list = graph.listMapNodes(&simpleDoubleFunction);
      CHECK_EQ(list.size(), graph.size());

      gum::Size s = 0;

      for (auto iter = list.begin(); iter != list.end(); ++iter)
        s += *iter;

      CHECK_EQ(s, 2 * (id1 + id2 + id3 + id4 + id5));
    }

    void testTwistedNodeListMapNodes() {
      gum::DAG graph = buildGraph();

      gum::List< gum::Size > list;
      CHECK_THROWS(list = graph.listMapNodes(&twistedMapFunction));

      CHECK_EQ(list.size(), static_cast< gum::Size >(0));
    }

    void testHashMapNodes() {
      gum::DAG graph = buildGraph();

      auto hashmap = graph.nodesPropertyFromFunction(&simpleDoubleFunction);
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
      gum::DAG graph = buildGraph();

      gum::NodeProperty< gum::Size > hashmap;
      CHECK_THROWS(hashmap = graph.nodesPropertyFromFunction(&twistedMapFunction));

      CHECK_EQ(hashmap.size(), static_cast< gum::Size >(0));
    }

    void testListMapArcs() {
      gum::DAG graph = buildGraph();

      gum::List< gum::Size > list = graph.listMapArcs(&simpleArcMapFunction);
      CHECK_EQ(list.size(), graph.sizeArcs());

      gum::Size s = 0;

      for (auto iter = list.begin(); iter != list.end(); ++iter)
        s += *iter;

      CHECK_EQ(s, (gum::Size)(0 + 0 + 2 + 3 + 1 + 4 + 2 + 3 + 4 + 4 + 3 + 1));
    }

    void testHashMapArcs() {
      gum::DAG graph = buildGraph();

      auto hashmap = graph.arcsProperty(&simpleArcMapFunction);
      CHECK_EQ(hashmap.size(), graph.sizeArcs());

      gum::Size sk = 0;
      gum::Size sv = 0;

      for (const auto& elt: hashmap) {
        sk += elt.first.head() + elt.first.tail();
        sv += elt.second;
      }

      CHECK_EQ(sk, sv);
    }

    void testCopyOperator() {
      gum::DAG graph = buildGraph();

      gum::DAG g2 = buildGraph();
      g2.addNode();

      gum::DAG g3;

      g2 = g3 = graph;

      CHECK_EQ(g2, graph);
      CHECK_EQ(g3, graph);

      g2.clear();
      g3.clearArcs();

      CHECK_NE(g2, graph);
      CHECK_NE(g3, graph);
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
      CHECK_EQ(graph.family(0), gum::NodeSet({0}));
      CHECK_EQ(graph.family(4), gum::NodeSet({1, 2, 3, 4}));
      auto seq = graph.topologicalOrder();
    }

    void testMonoCycle() {
      gum::DAG graph;
      auto     x = graph.addNode();
      CHECK_THROWS_AS(graph.addArc(x, x), const gum::Exception&);
    }

    // Classic BN structure:  0→2←1, 2→3
    //   parents(2) = {0,1}  →  moral edge {0,1}
    static gum::DAG makeDiamond() {
      gum::DAG g;
      g.addNodes(4);
      g.addArc(0, 2);
      g.addArc(1, 2);
      g.addArc(2, 3);
      return g;
    }

    static void testMoralGraph() {
      gum::DAG g = makeDiamond();
      // 0→2←1, 2→3
      gum::UndiGraph m = g.moralGraph();

      CHECK_EQ(m.size(), 4U);
      // arcs → edges
      CHECK(m.existsEdge(0, 2));
      CHECK(m.existsEdge(1, 2));
      CHECK(m.existsEdge(2, 3));
      // parents of 2 are married
      CHECK(m.existsEdge(0, 1));
      // no spurious edges
      CHECK_EQ(m.sizeEdges(), 4U);
    }

    static void testMoralizedAncestralGraph() {
      gum::DAG g = makeDiamond();
      // 0→2←1, 2→3

      // ancestral graph of {3} = all 4 nodes → same moral graph
      gum::UndiGraph m3 = g.moralizedAncestralGraph(gum::NodeSet{3});
      CHECK_EQ(m3.size(), 4U);
      CHECK(m3.existsEdge(0, 1));   // parents of 2 married

      // ancestral graph of {2} = {0,1,2} → no node 3
      gum::UndiGraph m2 = g.moralizedAncestralGraph(gum::NodeSet{2});
      CHECK_EQ(m2.size(), 3U);
      CHECK(!m2.existsNode(3));
      CHECK(m2.existsEdge(0, 2));
      CHECK(m2.existsEdge(1, 2));
      CHECK(m2.existsEdge(0, 1));
      CHECK_EQ(m2.sizeEdges(), 3U);

      // ancestral graph of a root = just that node
      gum::UndiGraph m0 = g.moralizedAncestralGraph(gum::NodeSet{0});
      CHECK_EQ(m0.size(), 1U);
      CHECK_EQ(m0.sizeEdges(), 0U);
    }

    static void testRequisiteNodes() {
      // requisiteNodes uses Shachter's semantics: a node is requisite iff the
      // ball visits it from *below* (upward) or it activates a v-structure.
      // This is a strict SUBSET of the d-connected nodes.

      // --- Collider: 0→1←2 ---
      // No evidence: ball from {0} goes downward through 1 (no activation)
      // → only {0} is requisite (never visited from below)
      gum::DAG collider;
      collider.addNodes(3);
      collider.addArc(0, 1);
      collider.addArc(2, 1);

      gum::NodeSet req0 = gum::graph::requisiteNodes(collider, gum::NodeSet{0});
      CHECK(req0.exists(0));
      CHECK_FALSE(req0.exists(1));
      CHECK_FALSE(req0.exists(2));

      // Hard evidence on 1: activates collider → ball goes up to parents {0,2}
      // → 1 and 2 are visited from below (upward) → requisite = {0, 1, 2}
      gum::NodeSet req0_zhard
          = gum::graph::requisiteNodes(collider, gum::NodeSet{0}, gum::NodeSet{1});
      CHECK(req0_zhard.exists(0));
      CHECK(req0_zhard.exists(1));
      CHECK(req0_zhard.exists(2));

      // Soft evidence on 1: same collider activation as hard, but 1 not blocked
      gum::NodeSet req0_zsoft
          = gum::graph::requisiteNodes(collider, gum::NodeSet{0}, gum::NodeSet{}, gum::NodeSet{1});
      CHECK(req0_zsoft.exists(0));
      CHECK(req0_zsoft.exists(1));
      CHECK(req0_zsoft.exists(2));

      // --- Chain: 0→1→2 ---
      // No evidence: ball from {0} only goes down (never up through 1 or 2)
      // → only {0} is requisite
      gum::DAG chain;
      chain.addNodes(3);
      chain.addArc(0, 1);
      chain.addArc(1, 2);

      gum::NodeSet req_chain_open = gum::graph::requisiteNodes(chain, gum::NodeSet{0});
      CHECK(req_chain_open.exists(0));
      CHECK_FALSE(req_chain_open.exists(1));
      CHECK_FALSE(req_chain_open.exists(2));

      // Hard evidence on 1: blocks chain → ball visits 1 from below (colider
      // path: 0 goes down to 1, 1 in Zhard → continue=true → only 1 in top)
      // Actually: from_child=false at 1, Zhard={1} → is_hard=true,
      //   is_ev=true → top marks 1 as requisite, propagates up to 0 (already seen)
      //   but !is_hard=false → skip downward → 2 not reached
      gum::NodeSet req_chain_blocked
          = gum::graph::requisiteNodes(chain, gum::NodeSet{0}, gum::NodeSet{1});
      CHECK(req_chain_blocked.exists(0));
      CHECK(req_chain_blocked.exists(1));
      CHECK_FALSE(req_chain_blocked.exists(2));

      // Soft evidence on 1: activates collider (up through 0) but does NOT
      // block downward pass → 1 is requisite (via collider), 2 is visited from
      // parent but NOT requisite
      gum::NodeSet req_chain_soft
          = gum::graph::requisiteNodes(chain, gum::NodeSet{0}, gum::NodeSet{}, gum::NodeSet{1});
      CHECK(req_chain_soft.exists(0));
      CHECK(req_chain_soft.exists(1));
      CHECK_FALSE(req_chain_soft.exists(2));

      // --- dConnected covers the full d-connected set ---
      // Chain: 0→1→2, dConnected({0}, {}) = {0, 1, 2}
      gum::NodeSet dc = gum::graph::dConnected(chain, gum::NodeSet{0});
      CHECK(dc.exists(0));
      CHECK(dc.exists(1));
      CHECK(dc.exists(2));

      // Chain: dConnected({0}, {1}) blocks at 1 → only {0, 1}
      gum::NodeSet dc_blocked = gum::graph::dConnected(chain, gum::NodeSet{0}, gum::NodeSet{1});
      CHECK(dc_blocked.exists(0));
      CHECK(dc_blocked.exists(1));
      CHECK_FALSE(dc_blocked.exists(2));

      // Soft evidence on 1 does NOT block → {0, 1, 2}
      gum::NodeSet dc_soft
          = gum::graph::dConnected(chain, gum::NodeSet{0}, gum::NodeSet{}, gum::NodeSet{1});
      CHECK(dc_soft.exists(0));
      CHECK(dc_soft.exists(1));
      CHECK(dc_soft.exists(2));
    }

    static void testDSeparation() {
      // --- Chain: 0→1→2 ---
      gum::DAG chain;
      chain.addNodes(3);
      chain.addArc(0, 1);
      chain.addArc(1, 2);

      // 0 and 2 connected (path 0→1→2 open)
      CHECK_FALSE(chain.dSeparation(0, 2, gum::NodeSet{}));
      // conditioning on 1 blocks the chain
      CHECK(chain.dSeparation(0, 2, gum::NodeSet{1}));
      // adjacent nodes never d-sep without conditioning on a separator
      CHECK_FALSE(chain.dSeparation(0, 1, gum::NodeSet{}));

      // --- Fork: 0←1→2 ---
      gum::DAG fork;
      fork.addNodes(3);
      fork.addArc(1, 0);
      fork.addArc(1, 2);

      CHECK_FALSE(fork.dSeparation(0, 2, gum::NodeSet{}));   // common cause
      CHECK(fork.dSeparation(0, 2, gum::NodeSet{1}));         // conditioning blocks

      // --- Collider: 0→1←2 ---
      gum::DAG collider;
      collider.addNodes(3);
      collider.addArc(0, 1);
      collider.addArc(2, 1);

      // d-sep without evidence: collider inactive
      CHECK(collider.dSeparation(0, 2, gum::NodeSet{}));
      // conditioning on collider activates it
      CHECK_FALSE(collider.dSeparation(0, 2, gum::NodeSet{1}));

      // --- Set-valued overload ---
      // 0→2←1, 2→3, 2→4
      gum::DAG g;
      g.addNodes(5);
      g.addArc(0, 2);
      g.addArc(1, 2);
      g.addArc(2, 3);
      g.addArc(2, 4);

      // {0} d-sep from {1} given {} (no common path except via collider 2)
      CHECK(g.dSeparation(gum::NodeSet{0}, gum::NodeSet{1}, gum::NodeSet{}));
      // conditioning on 2 marries 0 and 1
      CHECK_FALSE(g.dSeparation(gum::NodeSet{0}, gum::NodeSet{1}, gum::NodeSet{2}));
      // {3} and {4} connected through 2 when not conditioning
      CHECK_FALSE(g.dSeparation(gum::NodeSet{3}, gum::NodeSet{4}, gum::NodeSet{}));
      // conditioning on 2 blocks 3 from 4
      CHECK(g.dSeparation(gum::NodeSet{3}, gum::NodeSet{4}, gum::NodeSet{2}));

      // X and Y must be disjoint
      CHECK_THROWS_AS(g.dSeparation(gum::NodeSet{0, 1}, gum::NodeSet{1, 2}, gum::NodeSet{}),
                      const gum::Exception&);
    }

    static void testMinimalCondSet() {
      // --- Chain: 0→1→2 ---
      gum::DAG chain;
      chain.addNodes(3);
      chain.addArc(0, 1);
      chain.addArc(1, 2);

      // target=2, soids={0,1}: conditioning on 1 is enough — 0 irrelevant
      CHECK_EQ(chain.minimalCondSet(2, gum::NodeSet{0, 1}), gum::NodeSet{1});
      // target=2, soids={0}: have to use 0 (only candidate)
      CHECK_EQ(chain.minimalCondSet(2, gum::NodeSet{0}), gum::NodeSet{0});
      // target already in soids → return {target}
      CHECK_EQ(chain.minimalCondSet(1, gum::NodeSet{0, 1}), gum::NodeSet{1});
      // empty soids → empty result
      CHECK(chain.minimalCondSet(2, gum::NodeSet{}).empty());

      // --- Fork: 1→0, 1→2 ---
      gum::DAG fork;
      fork.addNodes(3);
      fork.addArc(1, 0);
      fork.addArc(1, 2);

      // target=0, soids={1,2}: 1 is the direct parent; 2 unreachable from target via soids
      CHECK_EQ(fork.minimalCondSet(0, gum::NodeSet{1, 2}), gum::NodeSet{1});
      // target=2, soids={0,1}: symmetric
      CHECK_EQ(fork.minimalCondSet(2, gum::NodeSet{0, 1}), gum::NodeSet{1});

      // --- Collider: 0→1←2 ---
      gum::DAG collider;
      collider.addNodes(3);
      collider.addArc(0, 1);
      collider.addArc(2, 1);

      // target=1, soids={0,2}: both parents needed
      CHECK_EQ(collider.minimalCondSet(1, gum::NodeSet{0, 2}), (gum::NodeSet{0, 2}));

      // --- Multiple targets ---
      // Chain 0→1→2→3: targets={0,3}, soids={1,2}
      gum::DAG long_chain;
      long_chain.addNodes(4);
      long_chain.addArc(0, 1);
      long_chain.addArc(1, 2);
      long_chain.addArc(2, 3);

      auto mcs = long_chain.minimalCondSet(gum::NodeSet{0, 3}, gum::NodeSet{1, 2});
      CHECK(mcs.contains(1));   // needed for target=0 (via child 1)
      CHECK(mcs.contains(2));   // needed for target=3 (via parent 2)

      // --- generic free function produces same result ---
      CHECK_EQ(gum::graph::minimalCondSet(chain, 2, gum::NodeSet{0, 1}), gum::NodeSet{1});
      CHECK_EQ(gum::graph::minimalCondSet(chain, gum::NodeSet{0, 2}, gum::NodeSet{1}),
               gum::NodeSet{1});
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
  GUM_TEST_ACTIF(MoralGraph)
  GUM_TEST_ACTIF(MoralizedAncestralGraph)
  GUM_TEST_ACTIF(RequisiteNodes)
  GUM_TEST_ACTIF(DSeparation)
  GUM_TEST_ACTIF(MinimalCondSet)
}   // namespace gum_tests
