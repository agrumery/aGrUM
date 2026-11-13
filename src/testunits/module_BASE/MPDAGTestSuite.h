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


#include <agrum/base/graphs/MPDAG.h>

#include <agrum/base/core/utils_random.h>
#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/randomGraphs.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {

  struct MPDAGTestSuite {};

  GUM_TEST(MeekClosure) {
    // a->b-c, a and c non adjacent : R1 orients b->c
    gum::PDAG pdag;
    pdag.addNodes(3);
    pdag.addArc(0, 1);
    pdag.addEdge(1, 2);
    const auto m = gum::MPDAG::meekClosure(pdag);
    CHECK(m.existsArc(1, 2));
    CHECK_EQ(m.sizeEdges(), gum::Size(0));
    CHECK(m.contains(m.consistentExtension()));
    CHECK_THROWS_AS(gum::MPDAG::fromPDAG(pdag), const gum::InvalidArgument&);   // not closed
    CHECK(gum::MPDAG::fromPDAG(m.graph()) == m);

    // not extendable
    gum::PDAG c4;
    c4.addNodes(4);
    for (gum::NodeId i = 0; i < 4; ++i)
      c4.addEdge(i, (i + 1) % 4);
    CHECK_THROWS_AS(gum::MPDAG::meekClosure(c4), const gum::InvalidArgument&);
    CHECK_THROWS_AS(gum::MPDAG::fromPDAG(c4), const gum::InvalidArgument&);
  }

  GUM_TEST(ReadAccess) {
    // a->b-c closed into a->b->c, and an isolated d
    gum::PDAG pdag;
    pdag.addNodes(4);
    pdag.setName(3, "d");
    pdag.addArc(0, 1);
    pdag.addEdge(1, 2);
    const auto m = gum::MPDAG::meekClosure(pdag);
    CHECK(!m.empty());
    CHECK(gum::MPDAG::meekClosure(gum::PDAG()).empty());
    CHECK(m.hasName(3));
    CHECK(!m.hasName(0));
    CHECK_EQ(m.idFromName("d"), std::optional< gum::NodeId >(3));
    const auto cc = m.connectedComponents();
    CHECK_EQ(cc[0], cc[2]);
    CHECK_NE(cc[0], cc[3]);
    const auto skeleton = m.skeleton();
    CHECK_EQ(skeleton.sizeEdges(), gum::Size(2));
    CHECK(skeleton.existsEdge(1, 2));
    CHECK_EQ(skeleton.nameFromId(3), "d");
  }

  GUM_TEST(CopyAndMove) {
    gum::PDAG pdag;
    pdag.addNodes(3);
    pdag.addArc(0, 1);
    pdag.addEdge(1, 2);
    const auto m = gum::MPDAG::meekClosure(pdag);
    gum::MPDAG copy(m);
    CHECK(copy == m);
    gum::MPDAG moved(std::move(copy));
    CHECK(moved == m);
    gum::MPDAG assigned = gum::MPDAG::meekClosure(gum::PDAG());
    assigned            = m;
    CHECK(assigned == m);
    gum::MPDAG moveAssigned = gum::MPDAG::meekClosure(gum::PDAG());
    moveAssigned            = std::move(assigned);
    CHECK(moveAssigned == m);
    CHECK(m.contains(moveAssigned.consistentExtension()));
  }

  GUM_TEST(NotAlwaysAChainGraph) {
    // triangle a-b-c-a with the knowledge a->b : a->b-c-a is a partially directed cycle
    gum::DAG triangle;
    triangle.addNodes(3);
    triangle.addArc(0, 1);
    triangle.addArc(1, 2);
    triangle.addArc(0, 2);
    const auto cp = gum::CPDAG::fromDAG(triangle);
    CHECK_EQ(cp.sizeEdges(), gum::Size(3));
    const auto m = gum::MPDAG::fromCPDAG(cp, gum::ArcSet{gum::Arc(0, 1)});
    CHECK(m.existsArc(0, 1));
    CHECK(m.existsEdge(1, 2));
    CHECK(m.existsEdge(0, 2));
    CHECK_THROWS_AS(gum::ChainGraph{m.graph()}, const gum::InvalidPartiallyDirectedCycle&);
  }

  GUM_TEST(FromCPDAG) {
    // chain a-b-c : the knowledge a->b gives b->c (R1)
    gum::DAG chain;
    chain.addNodes(3);
    chain.addArc(0, 1);
    chain.addArc(1, 2);
    const auto cp = gum::CPDAG::fromDAG(chain);
    const auto m  = gum::MPDAG::fromCPDAG(cp, gum::ArcSet{gum::Arc(0, 1)});
    CHECK(m.existsArc(0, 1));
    CHECK(m.existsArc(1, 2));
    CHECK(m.cpdag() == cp);
    CHECK(m.contains(chain));

    // a->b and c->b would create a new v-structure: not in the class
    CHECK_THROWS_AS(gum::MPDAG::fromCPDAG(cp, gum::ArcSet{gum::Arc(0, 1), gum::Arc(2, 1)}),
                    const gum::InvalidArgument&);
    // a->c does not orient an edge of the CPDAG
    CHECK_THROWS_AS(gum::MPDAG::fromCPDAG(cp, gum::ArcSet{gum::Arc(0, 2)}), const gum::InvalidArc&);
    // a->b and b->a orient the same edge both ways: no DAG of the class contains them
    CHECK_THROWS_AS(gum::MPDAG::fromCPDAG(cp, gum::ArcSet{gum::Arc(0, 1), gum::Arc(1, 0)}),
                    const gum::InvalidArgument&);

    // a directed cycle in the knowledge: not in the class either
    gum::DAG triangle;
    triangle.addNodes(3);
    triangle.addArc(0, 1);
    triangle.addArc(1, 2);
    triangle.addArc(0, 2);
    const auto cp3 = gum::CPDAG::fromDAG(triangle);
    CHECK_THROWS_AS(
        gum::MPDAG::fromCPDAG(cp3, gum::ArcSet{gum::Arc(0, 1), gum::Arc(1, 2), gum::Arc(2, 0)}),
        const gum::InvalidArgument&);
  }

  GUM_TEST(NamesAreKept) {
    gum::DAG dag;
    dag.addNodes(3);
    dag.setName(0, "A");
    dag.setName(1, "B");
    dag.setName(2, "C");
    dag.addArc(0, 1);
    dag.addArc(1, 2);
    const auto g = gum::MPDAG::meekClosure(gum::PDAG(dag));
    CHECK_EQ(g.nameFromId(1), "B");
    CHECK_EQ(g.idFromName("C"), std::optional< gum::NodeId >(2));
    CHECK(!g.idFromName("Z").has_value());
    CHECK_EQ(g.consistentExtension().nameFromId(0), "A");
  }

  GUM_TEST(DSeparationIsTheDSeparationOfTheDAGs) {
    for (int i = 0; i < 100; ++i) {
      const auto dag = randomDAG(4 + gum::randomValue(8), 0.3);
      const auto cp  = gum::CPDAG::fromDAG(dag);
      // background knowledge: some edges oriented as in dag
      gum::ArcSet knowledge;
      for (const auto& edge: cp.edges())
        if (gum::randomProba() < 0.3)
          knowledge.insert(dag.existsArc(edge.first(), edge.second())
                               ? gum::Arc(edge.first(), edge.second())
                               : gum::Arc(edge.second(), edge.first()));
      const auto m = gum::MPDAG::fromCPDAG(cp, knowledge);
      CHECK(m.contains(dag));
      CHECK(m.cpdag() == cp);
      for (const auto& arc: knowledge)
        CHECK(m.existsArc(arc.tail(), arc.head()));
      for (int k = 0; k < 10; ++k) {
        const gum::NodeId x = gum::randomValue(dag.size());
        const gum::NodeId y = gum::randomValue(dag.size());
        if (x == y) continue;
        const auto z = randomSet(dag, x, y);
        CHECK_EQ(m.dSeparation(x, y, z), dag.dSeparation(x, y, z));

        // the same for sets of nodes
        gum::NodeSet xs{x};
        for (const auto node: dag.nodes())
          if (node != x && node != y && !z.contains(node) && gum::randomProba() < 0.3)
            xs.insert(node);
        CHECK_EQ(m.dSeparation(xs, gum::NodeSet{y}, z), dag.dSeparation(xs, gum::NodeSet{y}, z));
      }
    }
  }

  GUM_TEST(ContainsIsTheMembershipOfTheClass) {
    for (int i = 0; i < 100; ++i) {
      const auto  dag = randomDAG(3 + gum::randomValue(7), 0.4);
      const auto  cp  = gum::CPDAG::fromDAG(dag);
      gum::ArcSet knowledge;
      for (const auto& edge: cp.edges())
        if (gum::randomProba() < 0.3)
          knowledge.insert(dag.existsArc(edge.first(), edge.second())
                               ? gum::Arc(edge.first(), edge.second())
                               : gum::Arc(edge.second(), edge.first()));
      const auto m = gum::MPDAG::fromCPDAG(cp, knowledge);

      // orientations of the skeleton along random orders: DAGs inside or outside the class
      for (int k = 0; k < 10; ++k) {
        std::vector< gum::NodeId > order(dag.nodes().begin(), dag.nodes().end());
        for (std::size_t j = order.size() - 1; j > 0; --j)
          std::swap(order[j], order[gum::randomValue(j + 1)]);
        std::vector< std::size_t > rank(order.size());
        for (std::size_t j = 0; j < order.size(); ++j)
          rank[order[j]] = j;
        gum::DAG candidate;
        candidate.addNodes(dag.size());
        for (const auto& arc: dag.arcs())
          if (rank[arc.tail()] < rank[arc.head()]) candidate.addArc(arc.tail(), arc.head());
          else candidate.addArc(arc.head(), arc.tail());

        bool expected = gum::CPDAG::fromDAG(candidate) == cp;
        for (const auto& arc: m.arcs())
          expected = expected && candidate.existsArc(arc.tail(), arc.head());
        CHECK_EQ(m.contains(candidate), expected);
      }
    }

    // same skeleton, different v-structures: a->b<-c is not in the class a-b-c
    gum::DAG chain;
    chain.addNodes(3);
    chain.addArc(0, 1);
    chain.addArc(1, 2);
    const auto m = gum::MPDAG::fromCPDAG(gum::CPDAG::fromDAG(chain), gum::ArcSet{});
    CHECK_EQ(m.sizeEdges(), gum::Size(2));
    gum::DAG collider;
    collider.addNodes(3);
    collider.addArc(0, 1);
    collider.addArc(2, 1);
    CHECK(!m.contains(collider));

    // same number of nodes, but another id for the isolated node
    gum::DAG isolated;
    isolated.addNodes(2);
    isolated.addNodeWithId(5);
    isolated.addArc(0, 1);
    gum::DAG other;
    other.addNodes(3);
    other.addArc(0, 1);
    CHECK(!gum::MPDAG::meekClosure(gum::PDAG(isolated)).contains(other));
  }

}   // namespace gum_tests
