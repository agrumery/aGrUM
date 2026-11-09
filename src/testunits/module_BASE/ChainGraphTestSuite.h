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


#include <functional>

#include <agrum/base/graphs/algorithms/generic/cycleDetection.h>
#include <agrum/base/graphs/chainGraph.h>
#include <agrum/base/graphs/graphElements.h>

#include <agrum/base/core/utils_random.h>
#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

// The graph used for the tests (same as PDAGTestSuite):
//  0 -> 2, 0 -> 3, 1 -> 3, 1 -> 4, 2 -> 4, 3 -> 4
//  3 -- 5 -- 6
//  6 -> 8, 7 -> 6
namespace gum_tests {

  struct ChainGraphTestSuite {
    protected:
    void buildChainGraph(gum::ChainGraph& g) {
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

  GUM_TEST(Insert) {
    gum::ChainGraph graph;
    buildChainGraph(graph);

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

  GUM_TEST(PartiallyDirectedCycles) {
    // h-v, h-w : the arc w->v closes the partially directed cycle h-w->v-h
    {
      gum::ChainGraph   g;
      const gum::NodeId h = g.addNode(), v = g.addNode(), w = g.addNode();
      g.addEdge(h, v);
      g.addEdge(h, w);
      CHECK_THROWS_AS(g.addArc(w, v), const gum::InvalidPartiallyDirectedCycle&);
    }

    // x-z-y : adding y->x closes the cycle y->x-z-y, which contains no other arc
    {
      gum::ChainGraph   g;
      const gum::NodeId x = g.addNode(), y = g.addNode(), z = g.addNode();
      g.addEdge(x, z);
      g.addEdge(z, y);
      CHECK_THROWS_AS(g.addArc(y, x), const gum::InvalidPartiallyDirectedCycle&);
      CHECK_THROWS_AS(g.addArc(x, y), const gum::InvalidPartiallyDirectedCycle&);
    }

    // x->z->y : the edge x-y closes the cycle x->z->y-x
    {
      gum::ChainGraph   g;
      const gum::NodeId x = g.addNode(), y = g.addNode(), z = g.addNode();
      g.addArc(x, z);
      g.addArc(z, y);
      CHECK_THROWS_AS(g.addEdge(x, y), const gum::InvalidPartiallyDirectedCycle&);
    }
  }

  GUM_TEST(AtMostOneConnectionPerPair) {
    gum::ChainGraph   g;
    const gum::NodeId x = g.addNode(), y = g.addNode();
    g.addEdge(x, y);
    CHECK_THROWS_AS(g.addArc(x, y), const gum::InvalidArc&);
    CHECK_THROWS_AS(g.addArc(y, x), const gum::InvalidArc&);
    g.eraseEdge(gum::Edge(x, y));
    g.addArc(x, y);
    CHECK_THROWS_AS(g.addEdge(x, y), const gum::InvalidEdge&);
    CHECK_THROWS_AS(g.addEdge(y, x), const gum::InvalidEdge&);
  }

  GUM_TEST(Constructors) {
    gum::ChainGraph graph;
    buildChainGraph(graph);

    gum::ChainGraph copy(graph);
    CHECK_EQ(copy, graph);
    gum::ChainGraph moved(std::move(copy));
    CHECK_EQ(moved, graph);
    gum::ChainGraph assigned;
    assigned = graph;
    CHECK_EQ(assigned, graph);

    // a DAG and an undirected graph are chain graphs
    gum::DAG dag;
    dag.addNodes(3);
    dag.addArc(0, 1);
    dag.addArc(1, 2);
    CHECK_EQ(gum::ChainGraph(dag).sizeArcs(), gum::Size(2));
    gum::UndiGraph ug;
    ug.addNodes(3);
    ug.addEdge(0, 1);
    ug.addEdge(1, 2);
    ug.addEdge(0, 2);
    CHECK_EQ(gum::ChainGraph(ug).sizeEdges(), gum::Size(3));

    // a PDAG with a partially directed cycle is not a chain graph
    gum::PDAG pdag;
    pdag.addNodes(3);
    pdag.addEdge(0, 1);
    pdag.addEdge(1, 2);
    pdag.addArc(2, 0);
    CHECK_THROWS_AS(gum::ChainGraph{pdag}, const gum::InvalidPartiallyDirectedCycle&);
    pdag.eraseArc(gum::Arc(2, 0));
    CHECK_EQ(gum::ChainGraph(pdag).sizeEdges(), gum::Size(2));

    // mixed graphs
    gum::MixedGraph mg;
    mg.addNodes(3);
    mg.addArc(0, 1);
    mg.addArc(1, 0);
    CHECK_THROWS_AS(gum::ChainGraph{mg}, const gum::InvalidDirectedCycle&);
    mg.eraseArc(gum::Arc(1, 0));
    mg.addEdge(0, 1);
    CHECK_THROWS_AS(gum::ChainGraph{mg}, const gum::InvalidArc&);
    mg.eraseEdge(gum::Edge(0, 1));
    mg.addEdge(1, 2);
    mg.addArc(2, 0);
    CHECK_THROWS_AS(gum::ChainGraph{mg}, const gum::InvalidPartiallyDirectedCycle&);
  }

  GUM_TEST(HasPartiallyDirectedCycle) {
    gum::MixedGraph g;
    g.addNodes(4);
    CHECK(!gum::graph::hasPartiallyDirectedCycle(g));
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 0);   // undirected cycle only
    CHECK(!gum::graph::hasPartiallyDirectedCycle(g));
    g.addArc(2, 3);
    CHECK(!gum::graph::hasPartiallyDirectedCycle(g));
    g.addEdge(3, 0);   // 2->3-0-2 (the arc and the edges of a same component)
    CHECK(gum::graph::hasPartiallyDirectedCycle(g));
    g.eraseEdge(gum::Edge(3, 0));
    g.addArc(3, 0);    // 2->3->0-2 : the arcs go back to the same chain component
    CHECK(gum::graph::hasPartiallyDirectedCycle(g));

    gum::DiGraph d;
    d.addNodes(3);
    d.addArc(0, 1);
    d.addArc(1, 2);
    CHECK(!gum::graph::hasDirectedCycle(d));
    d.addArc(2, 0);
    CHECK(gum::graph::hasDirectedCycle(d));
  }

  GUM_TEST(cSeparationFromKennethLee) {
    {
      gum::ChainGraph p;
      p.addNodes(10);
      p.addArc(0, 1);
      p.addArc(1, 2);
      CHECK(p.cSeparation(0, 2, gum::NodeSet({1})));
      CHECK(!p.cSeparation(0, 2, gum::NodeSet()));
    }
    {
      gum::ChainGraph p;
      p.addNodes(10);
      p.addEdge(0, 1);
      CHECK(!p.cSeparation(0, 1, gum::NodeSet({2})));
    }
    {
      gum::ChainGraph p;
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

      // sets of nodes
      CHECK(p.cSeparation(gum::NodeSet({0, 1}), gum::NodeSet({3}), gum::NodeSet()));
      CHECK(!p.cSeparation(gum::NodeSet({0, 1}), gum::NodeSet({3}), gum::NodeSet({2})));
      CHECK(!p.cSeparation(gum::NodeSet({0, 3}), gum::NodeSet({2}), gum::NodeSet()));
      CHECK_THROWS_AS(p.cSeparation(gum::NodeSet({0, 1}), gum::NodeSet({1}), gum::NodeSet()),
                      const gum::InvalidArgument&);
    }
  }

  GUM_TEST(MoralGraphFromLouisDerumaux) {
    gum::ChainGraph graph;
    buildChainGraph(graph);
    gum::UndiGraph moral = graph.moralGraph();
    CHECK(moral.existsEdge(id0, id7));
  }

  GUM_TEST(MoralGraphChainComponent) {
    gum::ChainGraph g;
    g.addNodes(4);
    g.addArc(0, 1);    // P1→X
    g.addEdge(1, 2);   // X--Y
    g.addArc(3, 2);    // P2→Y

    gum::UndiGraph m = g.moralGraph();
    CHECK_EQ(m.size(), 4U);
    CHECK(m.existsEdge(0, 1));     // arc P1→X → edge
    CHECK(m.existsEdge(1, 2));     // edge X--Y preserved
    CHECK(m.existsEdge(2, 3));     // arc P2→Y → edge
    CHECK(m.existsEdge(0, 3));     // chain-component marriage: P1--P2
    CHECK_EQ(m.sizeEdges(), 4U);   // no spurious edges
    // X is inside the chain component, not a parent — must NOT be married with P2
    CHECK(!m.existsEdge(1, 3));
  }

  GUM_TEST(MoralizedAncestralGraphEdgeFollowing) {
    gum::ChainGraph g;
    g.addNodes(4);
    g.addEdge(0, 1);   // A--B
    g.addEdge(1, 2);   // B--C
    g.addArc(2, 3);    // C→D

    // Starting from D, following arc C→D backward gives C, then edges C--B, B--A.
    auto m = g.moralizedAncestralGraph(gum::NodeSet{3});
    CHECK_EQ(m.size(), 4U);
    CHECK(m.existsNode(0));   // A included via edge chain
    CHECK(m.existsNode(1));   // B included
    CHECK(m.existsNode(2));   // C included
    CHECK(m.existsNode(3));   // D (query)

    // chain component {A,B,C}: no parents → no marriage edges
    // only arc C→D becomes edge C--D
    CHECK(m.existsEdge(2, 3));
    CHECK(m.existsEdge(0, 1));
    CHECK(m.existsEdge(1, 2));
    CHECK_EQ(m.sizeEdges(), 3U);

    // moralizedAncestralGraph({D}) in a pure DAG A→B→C→D: same size,
    // but follows arcs only — A is still included via arcs.
    // The difference shows when a node is reachable only via an undirected edge:
    gum::ChainGraph g2;
    g2.addNodes(3);
    g2.addEdge(0, 1);   // A--B (no arc involving A)
    g2.addArc(1, 2);    // B→C

    // A has no arc to B — only an undirected edge.
    // moralizedAncestralGraph({C}) must still include A via the edge B--A.
    auto m2 = g2.moralizedAncestralGraph(gum::NodeSet{2});
    CHECK_EQ(m2.size(), 3U);
    CHECK(m2.existsNode(0));   // A included despite no arc
  }

  GUM_TEST(cSeparationBugFromLouisDerumaux) {
    auto p = gum::ChainGraph();

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

  GUM_TEST(HasMixedReallyOrientedPath) {
    // 0-1->2-3
    gum::ChainGraph g;
    g.addNodes(4);
    g.addEdge(0, 1);
    g.addArc(1, 2);
    g.addEdge(2, 3);
    CHECK(g.hasMixedReallyOrientedPath(0, 3));
    CHECK(g.hasMixedReallyOrientedPath(1, 2));
    CHECK(!g.hasMixedReallyOrientedPath(3, 0));
    CHECK(!g.hasMixedReallyOrientedPath(0, 1));   // no arc
    CHECK(!g.hasMixedReallyOrientedPath(2, 2));
  }

  GUM_TEST(HasMixedReallyOrientedPathEqualsTheExhaustiveSearchOfPaths) {
    const gum::Size size = 7;
    for (int run = 0; run < 100; ++run) {
      gum::ChainGraph g;
      g.addNodes(size);
      for (gum::NodeId x = 0; x < size; ++x) {
        for (gum::NodeId y = x + 1; y < size; ++y) {
          const auto kind = gum::randomValue(4);
          try {
            if (kind == 0) g.addArc(x, y);
            else if (kind == 1) g.addArc(y, x);
            else if (kind == 2) g.addEdge(x, y);
          } catch (const gum::InvalidPartiallyDirectedCycle&) {
          } catch (const gum::InvalidDirectedCycle&) {}
        }
      }

      // every path visiting each node once
      std::function< bool(gum::NodeId, gum::NodeId, bool, gum::NodeSet&) > exists
          = [&](gum::NodeId node, gum::NodeId goal, bool withArc, gum::NodeSet& path) {
              if (node == goal) return withArc;
              path.insert(node);
              bool found = false;
              for (const auto child: g.children(node))
                if (!found && !path.contains(child)) found = exists(child, goal, true, path);
              for (const auto neighbour: g.neighbours(node))
                if (!found && !path.contains(neighbour))
                  found = exists(neighbour, goal, withArc, path);
              path.erase(node);
              return found;
            };

      for (gum::NodeId x = 0; x < size; ++x) {
        for (gum::NodeId y = 0; y < size; ++y) {
          gum::NodeSet path;
          CHECK(g.hasMixedReallyOrientedPath(x, y) == (x != y && exists(x, y, false, path)));
        }
      }
    }
  }

  GUM_TEST(ConversionsRejectMonoCycles) {
    gum::UndiGraph u;
    u.addNodes(2);
    u.addEdge(1, 1);
    CHECK_THROWS_AS(gum::ChainGraph{u}, gum::InvalidDirectedCycle&);
    gum::MixedGraph m;
    m.addNodes(2);
    m.addEdge(0, 0);
    CHECK_THROWS_AS(gum::ChainGraph{m}, gum::InvalidDirectedCycle&);
  }

}   // namespace gum_tests
