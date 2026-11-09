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


#include <algorithm>
#include <optional>
#include <string>
#include <vector>

#include <agrum/base/core/configuration/configuration.h>
#include <agrum/base/graphs/algorithms/generic/chordality.h>
#include <agrum/base/graphs/algorithms/generic/cycleDetection.h>
#include <agrum/base/graphs/algorithms/orientation.h>

#include <agrum/base/core/utils_random.h>
#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {

  struct OrientationTestSuite {
    public:
    static bool adjacent(const gum::MixedGraph& g, gum::NodeId a, gum::NodeId b) {
      return g.existsEdge(a, b) || g.existsArc(a, b) || g.existsArc(b, a);
    }

    /// reference algorithm (Dor and Tarsi 1992): naive search of the potential sinks
    static bool naiveExtendable(const gum::PDAG& pdag) {
      gum::MixedGraph g(pdag);
      while (g.sizeArcs() + g.sizeEdges() > 0) {
        bool found = false;
        for (const auto s: g.nodes()) {
          if (!g.children(s).empty()) continue;
          bool ok = true;
          for (const auto x: g.neighbours(s)) {
            for (const auto y: g.neighbours(s))
              if (x < y && !adjacent(g, x, y)) ok = false;
            for (const auto y: g.parents(s))
              if (!adjacent(g, x, y)) ok = false;
          }
          if (!ok) continue;
          g.eraseNode(s);
          found = true;
          break;
        }
        if (!found) return false;
      }
      return true;
    }

    /// checks that dag is a consistent extension of pdag
    static bool isConsistentExtension(const gum::PDAG& pdag, const gum::DAG& dag) {
      if (dag.size() != pdag.size()) return false;
      if (dag.sizeArcs() != pdag.sizeArcs() + pdag.sizeEdges()) return false;
      for (const auto& arc: pdag.arcs())
        if (!dag.existsArc(arc.tail(), arc.head())) return false;
      for (const auto& edge: pdag.edges())
        if (!dag.existsArc(edge.first(), edge.second())
            && !dag.existsArc(edge.second(), edge.first()))
          return false;
      // same v-structures (the v-structures of pdag are kept since its arcs are)
      for (const auto y: dag.nodes())
        for (const auto a: dag.parents(y))
          for (const auto b: dag.parents(y))
            if (a < b && !dag.existsArc(a, b) && !dag.existsArc(b, a)
                && !(pdag.existsArc(a, y) && pdag.existsArc(b, y)))
              return false;
      return true;
    }

    /// reference algorithm: rules R1-R4 of Meek (1995) applied until no rule applies
    static gum::MixedGraph naiveMeekClosure(const gum::PDAG& pdag) {
      gum::MixedGraph g(pdag);
      const auto      orientable = [&g](gum::NodeId x, gum::NodeId y) {
        // R1: z->x-y, z and y not adjacent
        for (const auto z: g.parents(x))
          if (!adjacent(g, z, y)) return true;
        // R2: x->z->y
        for (const auto z: g.children(x))
          if (g.existsArc(z, y)) return true;
        // R3: x-z1, x-z2, z1->y, z2->y, z1 and z2 not adjacent
        for (const auto z1: g.parents(y))
          for (const auto z2: g.parents(y))
            if (z1 < z2 && g.existsEdge(x, z1) && g.existsEdge(x, z2) && !adjacent(g, z1, z2))
              return true;
        // R4: x-z, z->w->y, x and w adjacent, z and y not adjacent
        for (const auto w: g.parents(y))
          for (const auto z: g.parents(w))
            if (g.existsEdge(x, z) && adjacent(g, x, w) && !adjacent(g, z, y)) return true;
        return false;
      };
      bool changed = true;
      while (changed) {
        changed = false;
        for (const auto& edge: gum::EdgeSet(g.edges()))
          for (const auto& [x, y]:
               {std::pair{edge.first(), edge.second()}, std::pair{edge.second(), edge.first()}})
            if (g.existsEdge(x, y) && orientable(x, y)) {
              g.eraseEdge(edge);
              g.addArc(x, y);
              changed = true;
            }
      }
      return g;
    }

    /// random extendable PDAG: the arcs of the v-structures of a random DAG and some other arcs
    static gum::PDAG randomExtendablePDAG(gum::Size n, double density, double arcProba) {
      const auto dag = randomDAG(n, density);
      gum::PDAG  pdag(gum::graph::cpdag(dag));
      for (const auto& edge: gum::EdgeSet(pdag.edges()))
        if (gum::randomProba() < arcProba) {
          pdag.eraseEdge(edge);
          if (dag.existsArc(edge.first(), edge.second())) pdag.addArc(edge.first(), edge.second());
          else pdag.addArc(edge.second(), edge.first());
        }
      return pdag;
    }

    /// random DAG: the arcs follow a random order of the nodes
    static gum::DAG randomDAG(gum::Size n, double density) {
      std::vector< gum::NodeId > order;
      for (gum::NodeId i = 0; i < n; ++i)
        order.push_back(i);
      for (gum::Size i = n; i > 1; --i)
        std::swap(order[i - 1], order[gum::randomValue(i)]);
      gum::DAG dag;
      dag.addNodes(n);
      for (gum::Size i = 0; i < n; ++i)
        for (gum::Size j = i + 1; j < n; ++j)
          if (gum::randomProba() < density) dag.addArc(order[i], order[j]);
      return dag;
    }

    /// the total size of the CPTs of dag
    static double cptSize(const gum::DAG& dag, const gum::NodeProperty< gum::Size >& sizes) {
      double total = 0.0;
      for (const auto node: dag.nodes()) {
        double size = double(sizes[node]);
        for (const auto parent: dag.parents(node))
          size *= double(sizes[parent]);
        total += size;
      }
      return total;
    }

    /// the minimal total size of the CPTs over the consistent extensions of pdag (enumeration of
    /// the orders of the nodes), or -1 if pdag is not extendable
    static double naiveMinimalCPTSize(const gum::PDAG&                      pdag,
                                      const gum::NodeProperty< gum::Size >& sizes) {
      std::vector< gum::NodeId > order(pdag.nodes().begin(), pdag.nodes().end());
      std::sort(order.begin(), order.end());
      double best = -1.0;
      do {
        gum::NodeProperty< gum::Idx > pos;
        for (gum::Idx i = 0; i < order.size(); ++i)
          pos.insert(order[i], i);
        bool respectsArcs = true;
        for (const auto& arc: pdag.arcs())
          if (pos[arc.tail()] > pos[arc.head()]) respectsArcs = false;
        if (!respectsArcs) continue;
        gum::DAG dag;
        for (const auto node: order)
          dag.addNodeWithId(node);
        for (const auto& arc: pdag.arcs())
          dag.addArc(arc.tail(), arc.head());
        for (const auto& edge: pdag.edges())
          if (pos[edge.first()] < pos[edge.second()]) dag.addArc(edge.first(), edge.second());
          else dag.addArc(edge.second(), edge.first());
        if (!isConsistentExtension(pdag, dag)) continue;
        const double size = cptSize(dag, sizes);
        if (best < 0 || size < best) best = size;
      } while (std::next_permutation(order.begin(), order.end()));
      return best;
    }

    /// random PDAG (maybe not extendable): random skeleton, edges or arcs following an order
    static gum::PDAG randomPDAG(gum::Size n, double density, double arcProba) {
      gum::PDAG g;
      g.addNodes(n);
      for (gum::NodeId i = 0; i < n; ++i)
        for (gum::NodeId j = i + 1; j < n; ++j)
          if (gum::randomProba() < density) {
            if (gum::randomProba() < arcProba) g.addArc(i, j);
            else g.addEdge(i, j);
          }
      return g;
    }
  };

  GUM_TEST(DegeneracyOrdering) {
    // a tree is 1-degenerate, a clique of size k is (k-1)-degenerate
    gum::MixedGraph g;
    g.addNodes(8);
    for (gum::NodeId i = 1; i < 5; ++i)
      g.addEdge(0, i);
    g.addArc(5, 6);
    g.addArc(6, 7);
    g.addArc(5, 7);
    const auto order = gum::graph::degeneracyOrdering(g);
    CHECK_EQ(order.size(), g.size());
    gum::Size degeneracy = 0;
    for (const auto node: order) {
      gum::Size before = 0;
      for (const auto x: g.neighbours(node))
        if (order.pos(x) < order.pos(node)) ++before;
      for (const auto x: g.parents(node))
        if (order.pos(x) < order.pos(node)) ++before;
      for (const auto x: g.children(node))
        if (order.pos(x) < order.pos(node)) ++before;
      degeneracy = std::max(degeneracy, before);
    }
    CHECK_EQ(degeneracy, gum::Size(2));
  }

  GUM_TEST(DegeneracyOrderingWithDoubleConnections) {
    // a pair connected twice or thrice: the degrees exceed the number of nodes
    gum::MixedGraph g;
    g.addNodes(2);
    g.addArc(0, 1);
    g.addArc(1, 0);
    g.addEdge(0, 1);
    const auto order = gum::graph::degeneracyOrdering(g);
    CHECK_EQ(order.size(), g.size());
    CHECK(order.exists(0));
    CHECK(order.exists(1));
  }

  GUM_TEST(OrientAsDAGWithOverflowingCosts) {
    // a clique whose CPT sizes overflow the doubles: every orientation costs infinity, one of
    // them is still returned
    gum::PDAG                      clique;
    gum::NodeProperty< gum::Size > sizes;
    const gum::Size                n = 20;
    clique.addNodes(n);
    for (gum::NodeId i = 0; i < n; ++i) {
      sizes.insert(i, gum::Size(1) << 60);
      for (gum::NodeId j = i + 1; j < n; ++j)
        clique.addEdge(i, j);
    }
    const auto dag = gum::graph::orientAsDAG(clique, sizes);
    CHECK_EQ(dag.size(), n);
    CHECK_EQ(dag.sizeArcs(), n * (n - 1) / 2);
  }

  GUM_TEST(ConsistentExtensionExamples) {
    // an undirected 4-cycle is not extendable (it is not chordal)
    gum::PDAG c4;
    c4.addNodes(4);
    for (gum::NodeId i = 0; i < 4; ++i)
      c4.addEdge(i, (i + 1) % 4);
    CHECK(!gum::graph::consistentExtension(c4).has_value());

    // a chordal undirected graph is extended without v-structure
    c4.addEdge(0, 2);
    const auto ext = gum::graph::consistentExtension(c4);
    REQUIRE(ext.has_value());
    CHECK(isConsistentExtension(c4, *ext));

    // a->b-c, a and c non adjacent: b->c is forced (no new v-structure a->b<-c)
    gum::PDAG r1;
    r1.addNodes(3);
    r1.addArc(0, 1);
    r1.addEdge(1, 2);
    const auto e1 = gum::graph::consistentExtension(r1);
    REQUIRE(e1.has_value());
    CHECK(e1->existsArc(1, 2));

    // a->b-c<-d (a-c, a-d, b-d non adjacent): b->c creates b->c<-d, c->b creates a->b<-c
    gum::PDAG bad;
    bad.addNodes(4);
    bad.addArc(0, 1);
    bad.addEdge(1, 2);
    bad.addArc(3, 2);
    CHECK(!gum::graph::consistentExtension(bad).has_value());

    // isolated nodes and empty graph
    gum::PDAG empty;
    CHECK(gum::graph::consistentExtension(empty).has_value());
    empty.addNodes(3);
    const auto e2 = gum::graph::consistentExtension(empty);
    REQUIRE(e2.has_value());
    CHECK_EQ(e2->size(), gum::Size(3));
  }

  GUM_TEST(ConsistentExtensionVsNaive) {
    for (int i = 0; i < 300; ++i) {
      const auto pdag = randomPDAG(4 + gum::randomValue(9), 0.4, 0.3);
      const auto ext  = gum::graph::consistentExtension(pdag);
      CHECK_EQ(ext.has_value(), naiveExtendable(pdag));
      if (ext.has_value()) CHECK(isConsistentExtension(pdag, *ext));
    }
  }

  GUM_TEST(CPDAGExamples) {
    // chain a->b->c: all reversible
    gum::DAG chain;
    chain.addNodes(3);
    chain.addArc(0, 1);
    chain.addArc(1, 2);
    auto cp = gum::graph::cpdag(chain);
    CHECK_EQ(cp.sizeArcs(), gum::Size(0));
    CHECK_EQ(cp.sizeEdges(), gum::Size(2));

    // v-structure a->c<-b, and c->d compelled by R1
    gum::DAG v;
    v.addNodes(4);
    v.addArc(0, 2);
    v.addArc(1, 2);
    v.addArc(2, 3);
    cp = gum::graph::cpdag(v);
    CHECK_EQ(cp.sizeArcs(), gum::Size(3));
    CHECK_EQ(cp.sizeEdges(), gum::Size(0));

    // a v-structure whose parents are adjacent is not a v-structure: complete DAG on 3 nodes
    gum::DAG complete;
    complete.addNodes(3);
    complete.addArc(0, 1);
    complete.addArc(0, 2);
    complete.addArc(1, 2);
    cp = gum::graph::cpdag(complete);
    CHECK_EQ(cp.sizeArcs(), gum::Size(0));
    CHECK_EQ(cp.sizeEdges(), gum::Size(3));
  }

  GUM_TEST(CPDAGProperties) {
    for (int i = 0; i < 200; ++i) {
      const auto dag = randomDAG(3 + gum::randomValue(12), 0.3);
      const auto cp  = gum::graph::cpdag(dag);

      // a chain graph (checked by the constructor) whose chain components are chordal
      // (Andersson et al. 1997), with the skeleton of dag
      CHECK_EQ(cp.sizeArcs() + cp.sizeEdges(), dag.sizeArcs());
      CHECK(gum::graph::isChordal(cp));
      for (const auto& arc: cp.arcs())
        CHECK(dag.existsArc(arc.tail(), arc.head()));

      // dag is a consistent extension of its CPDAG, and so is any extension of it, which is
      // Markov equivalent to dag and has the same CPDAG
      const gum::PDAG pdag(cp);
      CHECK(isConsistentExtension(pdag, dag));
      const auto ext = gum::graph::consistentExtension(pdag);
      CHECK(ext.has_value());
      if (ext.has_value()) {
        CHECK(isConsistentExtension(pdag, *ext));
        CHECK(gum::graph::cpdag(*ext) == cp);
      }
    }
  }

  GUM_TEST(MeekClosureExamples) {
    // R1: 0->1-2 gives 1->2
    gum::PDAG r1;
    r1.addNodes(3);
    r1.addArc(0, 1);
    r1.addEdge(1, 2);
    auto c = gum::graph::meekClosure(r1);
    REQUIRE(c.has_value());
    CHECK(c->existsArc(1, 2));

    // R2: 0->1->2, 0-2 gives 0->2
    gum::PDAG r2;
    r2.addNodes(3);
    r2.addArc(0, 1);
    r2.addArc(1, 2);
    r2.addEdge(0, 2);
    c = gum::graph::meekClosure(r2);
    REQUIRE(c.has_value());
    CHECK(c->existsArc(0, 2));

    // R3: 1->3<-2, 0-1, 0-2, 0-3 gives 0->3; 0-1 and 0-2 stay undirected
    gum::PDAG r3;
    r3.addNodes(4);
    r3.addArc(1, 3);
    r3.addArc(2, 3);
    r3.addEdge(0, 1);
    r3.addEdge(0, 2);
    r3.addEdge(0, 3);
    c = gum::graph::meekClosure(r3);
    REQUIRE(c.has_value());
    CHECK(c->existsArc(0, 3));
    CHECK_EQ(c->sizeEdges(), gum::Size(2));

    // R4: 0->3->2, 1-0, 1-3, 1-2 gives 1->2; 1-0 and 1-3 stay undirected
    gum::PDAG r4;
    r4.addNodes(4);
    r4.addArc(0, 3);
    r4.addArc(3, 2);
    r4.addEdge(0, 1);
    r4.addEdge(1, 2);
    r4.addEdge(1, 3);
    c = gum::graph::meekClosure(r4);
    REQUIRE(c.has_value());
    CHECK(c->existsArc(1, 2));
    CHECK_EQ(c->sizeEdges(), gum::Size(2));

    // not extendable: the cycle 0-1-2-3-0 is not chordal
    gum::PDAG cycle;
    cycle.addNodes(4);
    for (gum::NodeId i = 0; i < 4; ++i)
      cycle.addEdge(i, (i + 1) % 4);
    CHECK(!gum::graph::meekClosure(cycle).has_value());
  }

  GUM_TEST(MeekClosureVsNaive) {
    for (int i = 0; i < 300; ++i) {
      const auto pdag    = randomExtendablePDAG(3 + gum::randomValue(12), 0.4, 0.2);
      const auto closure = gum::graph::meekClosure(pdag);
      CHECK(closure.has_value());
      if (closure.has_value())
        CHECK(static_cast< const gum::MixedGraph& >(*closure) == naiveMeekClosure(pdag));
    }
    // extendable PDAGs that are not built from a CPDAG
    for (int i = 0; i < 300; ++i) {
      const auto pdag    = randomPDAG(3 + gum::randomValue(9), 0.4, 0.3);
      const auto closure = gum::graph::meekClosure(pdag);
      CHECK_EQ(closure.has_value(), naiveExtendable(pdag));
      if (closure.has_value())
        CHECK(static_cast< const gum::MixedGraph& >(*closure) == naiveMeekClosure(pdag));
    }
  }

  GUM_TEST(MeekClosureOfVStructuresIsTheCPDAG) {
    for (int i = 0; i < 200; ++i) {
      const auto dag = randomDAG(3 + gum::randomValue(12), 0.3);
      // the arcs of the v-structures of dag only
      gum::PDAG pdag;
      pdag.addNodes(dag.size());
      for (const auto& arc: dag.arcs()) {
        bool vstructure = false;
        for (const auto z: dag.parents(arc.head()))
          if (z != arc.tail() && !dag.existsArc(z, arc.tail()) && !dag.existsArc(arc.tail(), z))
            vstructure = true;
        if (vstructure) pdag.addArc(arc.tail(), arc.head());
        else pdag.addEdge(arc.tail(), arc.head());
      }
      const auto closure = gum::graph::meekClosure(pdag);
      CHECK(closure.has_value());
      if (closure.has_value())
        CHECK(static_cast< const gum::MixedGraph& >(*closure)
              == static_cast< const gum::MixedGraph& >(gum::graph::cpdag(dag)));
      // dag is a consistent extension of pdag: the closure does not depend on the given extension
      CHECK(static_cast< const gum::MixedGraph& >(gum::graph::meekClosure(pdag, dag))
            == static_cast< const gum::MixedGraph& >(gum::graph::cpdag(dag)));
    }
  }

  GUM_TEST(OrientAsDAGExamples) {
    // a single edge: the small domain is the parent
    gum::PDAG edge;
    edge.addNodes(2);
    edge.addEdge(0, 1);
    gum::NodeProperty< gum::Size > sizes{{0, 4}, {1, 3}};
    auto                           dag = gum::graph::orientAsDAG(edge, sizes);
    CHECK(dag.existsArc(1, 0));
    sizes = gum::NodeProperty< gum::Size >{{0, 3}, {1, 4}};
    dag   = gum::graph::orientAsDAG(edge, sizes);
    CHECK(dag.existsArc(0, 1));
    // equal domains: the smallest id is the sink
    sizes = gum::NodeProperty< gum::Size >{{0, 2}, {1, 2}};
    dag   = gum::graph::orientAsDAG(edge, sizes);
    CHECK(dag.existsArc(1, 0));

    // star 0-1, 0-2, 0-3 with a large center: the center is the root
    gum::PDAG star;
    star.addNodes(4);
    for (gum::NodeId i = 1; i < 4; ++i)
      star.addEdge(0, i);
    sizes = gum::NodeProperty< gum::Size >{{0, 2}, {1, 5}, {2, 5}, {3, 5}};
    dag   = gum::graph::orientAsDAG(star, sizes);
    for (gum::NodeId i = 1; i < 4; ++i)
      CHECK(dag.existsArc(0, i));

    // domains of size 1: the cost of a node does not change when its neighbours are removed
    gum::PDAG path;
    path.addNodes(4);
    for (gum::NodeId i = 0; i < 3; ++i)
      path.addEdge(i, i + 1);
    sizes = gum::NodeProperty< gum::Size >{{0, 1}, {1, 1}, {2, 1}, {3, 1}};
    dag   = gum::graph::orientAsDAG(path, sizes);
    CHECK_EQ(dag.sizeArcs(), gum::Size(3));

    // a triangle of constant variables (as learned by MIIC from constant columns)
    gum::PDAG triangle;
    triangle.addNodes(3);
    triangle.addEdge(0, 1);
    triangle.addEdge(1, 2);
    triangle.addEdge(0, 2);
    sizes = gum::NodeProperty< gum::Size >{{0, 1}, {1, 1}, {2, 1}};
    dag   = gum::graph::orientAsDAG(triangle, sizes);
    CHECK_EQ(dag.sizeArcs(), gum::Size(3));

    // a missing domain size
    sizes = gum::NodeProperty< gum::Size >{{0, 2}};
    CHECK_THROWS_AS(gum::graph::orientAsDAG(edge, sizes), gum::InvalidArgument&);

    // not extendable (non chordal cycle 0-1-2-3-0): a DAG with the skeleton of the cycle
    gum::PDAG cycle;
    cycle.addNodes(4);
    for (gum::NodeId i = 0; i < 4; ++i)
      cycle.addEdge(i, (i + 1) % 4);
    sizes = gum::NodeProperty< gum::Size >{{0, 2}, {1, 2}, {2, 2}, {3, 2}};
    dag   = gum::graph::orientAsDAG(cycle, sizes);
    CHECK_EQ(dag.sizeArcs(), gum::Size(4));
    for (gum::NodeId i = 0; i < 4; ++i)
      CHECK((dag.existsArc(i, (i + 1) % 4) || dag.existsArc((i + 1) % 4, i)));
  }

  GUM_TEST(OrientAsDAGIsMinimal) {
    // path 0-1-2 with domain sizes 4, 5, 5: the greedy removal of the sink with the smallest CPT
    // removes 0 (CPT 20), then 1 (25), then 2 (5): 50; the minimum is 0->1->2: 4 + 20 + 25 = 49
    gum::PDAG path;
    path.addNodes(3);
    path.addEdge(0, 1);
    path.addEdge(1, 2);
    gum::NodeProperty< gum::Size > sizes{{0, 4}, {1, 5}, {2, 5}};
    const auto                     dag = gum::graph::orientAsDAG(path, sizes);
    CHECK(dag.existsArc(0, 1));
    CHECK(dag.existsArc(1, 2));
    CHECK_EQ(cptSize(dag, sizes), 49.0);
    {
      // components larger than graph.orientAsDAGExactMaxSize: the greedy heuristic
      ConfigurationGuard guard;
      gum::config().set("graph", "orientAsDAGExactMaxSize", "2");
      CHECK_EQ(cptSize(gum::graph::orientAsDAG(path, sizes), sizes), 50.0);
      gum::config().set("graph", "orientAsDAGExactMaxSize", "3");
      CHECK_EQ(cptSize(gum::graph::orientAsDAG(path, sizes), sizes), 49.0);
    }

    // the CPDAGs of random DAGs: the minimum over all the consistent extensions
    for (int i = 0; i < 200; ++i) {
      const auto dag   = randomDAG(3 + gum::randomValue(5), 0.5);
      const auto graph = gum::graph::cpdag(dag);
      gum::PDAG  pdag;
      for (const auto node: graph.nodes())
        pdag.addNodeWithId(node);
      for (const auto& arc: graph.arcs())
        pdag.addArc(arc.tail(), arc.head());
      for (const auto& edge: graph.edges())
        pdag.addEdge(edge.first(), edge.second());
      gum::NodeProperty< gum::Size > domains;
      for (const auto node: pdag.nodes())
        domains.insert(node, 1 + gum::randomValue(10));
      const auto oriented = gum::graph::orientAsDAG(pdag, domains);
      CHECK(isConsistentExtension(pdag, oriented));
      CHECK_EQ(cptSize(oriented, domains), naiveMinimalCPTSize(pdag, domains));
    }
  }

  GUM_TEST(OrientAsDAGProperties) {
    for (int i = 0; i < 300; ++i) {
      const auto                     pdag = randomPDAG(3 + gum::randomValue(10), 0.4, 0.3);
      gum::NodeProperty< gum::Size > sizes;
      for (const auto node: pdag.nodes())
        sizes.insert(node, 1 + gum::randomValue(5));
      const auto dag = gum::graph::orientAsDAG(pdag, sizes);
      // a consistent extension if there is one, else a DAG orienting pdag
      if (naiveExtendable(pdag)) CHECK(isConsistentExtension(pdag, dag));
      else {
        CHECK_EQ(dag.sizeArcs(), pdag.sizeArcs() + pdag.sizeEdges());
        for (const auto& arc: pdag.arcs())
          CHECK(dag.existsArc(arc.tail(), arc.head()));
        for (const auto& edge: pdag.edges())
          CHECK((dag.existsArc(edge.first(), edge.second())
                 || dag.existsArc(edge.second(), edge.first())));
      }
    }
  }

  GUM_TEST(NamesAreKept) {
    // A->C<-B, C->D, D-E (as a PDAG)
    gum::PDAG pdag;
    pdag.addNodes(5);
    for (gum::NodeId i = 0; i < 5; ++i)
      pdag.setName(i, std::string(1, char('A' + i)));
    pdag.addArc(0, 2);
    pdag.addArc(1, 2);
    pdag.addEdge(2, 3);
    pdag.addEdge(3, 4);
    const auto checkNames = [](const gum::NodeGraphPart& g) {
      for (gum::NodeId i = 0; i < 5; ++i)
        CHECK_EQ(g.nameFromId(i), std::string(1, char('A' + i)));
    };

    const auto ext = gum::graph::consistentExtension(pdag);
    REQUIRE(ext.has_value());
    checkNames(*ext);
    checkNames(gum::graph::cpdag(*ext));
    const auto closure = gum::graph::meekClosure(pdag);
    REQUIRE(closure.has_value());
    checkNames(*closure);
    gum::NodeProperty< gum::Size > sizes;
    for (gum::NodeId i = 0; i < 5; ++i)
      sizes.insert(i, 2);
    checkNames(gum::graph::orientAsDAG(pdag, sizes));

    // the closure from an extension without names has the names of pdag
    gum::DAG unnamed;
    for (const auto node: ext->nodes())
      unnamed.addNodeWithId(node);
    for (const auto& arc: ext->arcs())
      unnamed.addArc(arc.tail(), arc.head());
    checkNames(gum::graph::meekClosure(pdag, unnamed));
  }

  GUM_TEST(DomainSizeZero) {
    gum::PDAG pdag;
    pdag.addNodes(2);
    pdag.addEdge(0, 1);
    gum::NodeProperty< gum::Size > sizes;
    sizes.insert(0, 2);
    sizes.insert(1, 0);
    CHECK_THROWS_AS(gum::graph::orientAsDAG(pdag, sizes), gum::InvalidArgument&);
  }

}   // namespace gum_tests
