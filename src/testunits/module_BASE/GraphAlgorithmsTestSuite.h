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

#include <string>
#include <vector>

#include <agrum/base/graphs/algorithms/ancestral.h>
#include <agrum/base/graphs/algorithms/generic/chordality.h>
#include <agrum/base/graphs/algorithms/generic/cycleDetection.h>
#include <agrum/base/graphs/algorithms/generic/reachability.h>
#include <agrum/base/graphs/algorithms/generic/separation.h>
#include <agrum/base/graphs/algorithms/orientation.h>
#include <agrum/base/graphs/algorithms/separationCriteria.h>
#include <agrum/base/graphs/fastGraph.h>
#include <agrum/base/graphs/fastMarkedGraph.h>
#include <agrum/base/graphs/graphConcepts.h>
#include <agrum/base/graphs/MAG.h>
#include <agrum/base/graphs/MPDAG.h>
#include <agrum/base/graphs/PAG.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

// The free functions of gum::graph for each kind of graph: the immutable graphs satisfy the graph
// concepts, the overloads for CPDAG, MPDAG and MAG, the separation criterion of each kind of graph.

namespace gum_tests {

  // the immutable mixed graphs are directed, undirected and mixed graphs for the generic algorithms
  static_assert(gum::GUM_MixedGraphable< gum::CPDAG >);
  static_assert(gum::GUM_MixedGraphable< gum::MPDAG >);
  // the marked graphs are neither directed nor undirected graphs for the generic algorithms (as in
  // pyagrum.graph): their edges carry marks
  static_assert(gum::GUM_MarkedGraphable< gum::MarkedGraph >);
  static_assert(gum::GUM_MarkedGraphable< gum::PAG >);
  static_assert(gum::GUM_MarkedGraphable< gum::MAG >);
  static_assert(!gum::GUM_MarkedGraphable< gum::MixedGraph >);
  static_assert(!gum::GUM_MarkedGraphable< gum::CPDAG >);
  static_assert(!gum::GUM_DiGraphable< gum::MarkedGraph >);
  static_assert(!gum::GUM_UndiGraphable< gum::MarkedGraph >);
  static_assert(!gum::GUM_UndiGraphable< gum::PAG >);
  static_assert(!gum::GUM_DiGraphable< gum::MAG >);
  static_assert(!gum::GUM_UndiGraphable< gum::MAG >);

  struct GraphAlgorithmsTestSuite {};

  GUM_TEST(GenericAlgorithmsOnImmutableGraphs) {
    const auto dag   = gum::fastGraph< gum::DAG >("A->B->C;D->C");
    const auto cpdag = gum::CPDAG::fromDAG(dag);
    const auto mpdag = gum::MPDAG::fromPDAG(cpdag.graph());
    CHECK_EQ(gum::graph::ancestors(cpdag, dag.idFromName("C").value()),
             gum::graph::ancestors(cpdag.graph(), dag.idFromName("C").value()));
    CHECK_EQ(gum::graph::topologicalOrder(mpdag), gum::graph::topologicalOrder(mpdag.graph()));
    CHECK_EQ(gum::graph::isChordal(cpdag), gum::graph::isChordal(cpdag.graph()));
    CHECK_EQ(gum::graph::hasPartiallyDirectedCycle(mpdag),
             gum::graph::hasPartiallyDirectedCycle(mpdag.graph()));
  }

  GUM_TEST(OrientationOverloads) {
    const auto dag   = gum::fastGraph< gum::DAG >("A->B->C;D->C");
    const auto cpdag = gum::graph::cpdag(dag);
    CHECK(cpdag == gum::CPDAG::fromDAG(dag));
    const auto mpdag = gum::MPDAG::fromPDAG(cpdag.graph());
    CHECK_EQ(gum::graph::consistentExtension(cpdag).value(), cpdag.consistentExtension());
    CHECK_EQ(gum::graph::consistentExtension(mpdag).value(), mpdag.consistentExtension());
    CHECK_EQ(gum::graph::degeneracyOrdering(cpdag), gum::graph::degeneracyOrdering(cpdag.graph()));
    CHECK_EQ(gum::graph::degeneracyOrdering(mpdag), gum::graph::degeneracyOrdering(mpdag.graph()));
  }

  GUM_TEST(AncestorsStartFromTheParents) {
    const auto g  = gum::fastGraph< gum::DiGraph >("A->B->C->D;E->C");
    const auto id = [&g](const std::string& name) { return g.idFromName(name).value(); };
    CHECK_EQ(gum::graph::ancestors(g, id("C")), (gum::NodeSet{id("A"), id("B"), id("E")}));
    // a node of the set belongs to the result if it is an ancestor of another one: B of D
    CHECK_EQ(gum::graph::ancestors(g, gum::NodeSet{id("B"), id("D")}),
             (gum::NodeSet{id("A"), id("B"), id("C"), id("E")}));
    CHECK_EQ(gum::graph::descendants(g, gum::NodeSet{id("A"), id("C")}),
             (gum::NodeSet{id("B"), id("C"), id("D")}));
    CHECK(gum::graph::ancestors(g, gum::NodeSet()).empty());
    // on a directed cycle, a node is one of its own ancestors and descendants
    const auto cycle = gum::fastGraph< gum::DiGraph >("A->B->A;C->A");
    const auto a     = cycle.idFromName("A").value();
    const auto b     = cycle.idFromName("B").value();
    const auto c     = cycle.idFromName("C").value();
    CHECK_EQ(gum::graph::ancestors(cycle, a), (gum::NodeSet{a, b, c}));
    CHECK_EQ(gum::graph::descendants(cycle, a), (gum::NodeSet{a, b}));
    CHECK_EQ(gum::graph::descendants(cycle, c), (gum::NodeSet{a, b}));
    CHECK_EQ(cycle.ancestors(a), (gum::NodeSet{a, b, c}));
  }

  GUM_TEST(DescendantsOfMarkedGraphs) {
    const auto g  = gum::fastMarkedGraph("A-B->C<->D;C->E");
    const auto id = [&g](const std::string& name) { return g.idFromName(name).value(); };
    CHECK(gum::graph::descendants(g, gum::NodeSet{id("A")}).empty());   // A-B is not directed
    CHECK_EQ(gum::graph::descendants(g, gum::NodeSet{id("B")}), (gum::NodeSet{id("C"), id("E")}));
    CHECK_EQ(gum::graph::descendants(g, gum::NodeSet{id("B"), id("C")}),
             (gum::NodeSet{id("C"), id("E")}));
    const auto mag = gum::MAG::fromMarkedGraph(g);
    CHECK_EQ(gum::graph::descendants(mag, gum::NodeSet{id("B")}),
             gum::graph::descendants(g, gum::NodeSet{id("B")}));
  }

  GUM_TEST(AnteriorsFollowPaths) {
    // going back and forth through an edge is not a cycle; a cycle of edges is one
    {
      const auto g = gum::fastMarkedGraph("A-B");
      const auto a = g.idFromName("A").value();
      const auto b = g.idFromName("B").value();
      CHECK_EQ(gum::graph::anteriors(g, gum::NodeSet{a}), gum::NodeSet{b});
      CHECK_EQ(gum::graph::anteriors(g, gum::NodeSet{a, b}), (gum::NodeSet{a, b}));
    }
    {
      const auto g = gum::fastGraph< gum::PDAG >("A-B");
      const auto a = g.idFromName("A").value();
      const auto b = g.idFromName("B").value();
      CHECK_EQ(gum::graph::anteriors(g, gum::NodeSet{a}), gum::NodeSet{b});
      CHECK_EQ(gum::graph::anteriors(g, gum::NodeSet{a, b}), (gum::NodeSet{a, b}));
    }
    {
      const auto g = gum::fastMarkedGraph("A-B-C-A");
      CHECK_EQ(gum::graph::anteriors(g, gum::NodeSet{g.idFromName("A").value()}),
               g.nodes().asNodeSet());
    }
    {
      const auto g = gum::fastGraph< gum::MixedGraph >("A-B-C-A");
      CHECK_EQ(gum::graph::anteriors(g, gum::NodeSet{g.idFromName("A").value()}),
               g.nodes().asNodeSet());
    }
    {
      // arcs followed in their direction and edges
      const auto g  = gum::fastGraph< gum::PDAG >("A->B-C;D->A");
      const auto id = [&g](const std::string& name) { return g.idFromName(name).value(); };
      CHECK_EQ(gum::graph::anteriors(g, gum::NodeSet{id("C")}),
               (gum::NodeSet{id("A"), id("B"), id("D")}));
      CHECK_EQ(gum::graph::anteriors(g, gum::NodeSet{id("A")}), gum::NodeSet{id("D")});
      CHECK_EQ(gum::graph::anteriors(g, gum::NodeSet{id("A"), id("C")}),
               (gum::NodeSet{id("A"), id("B"), id("D")}));
    }
    {
      // a partially directed cycle A->B-C-A
      const auto g = gum::fastGraph< gum::MixedGraph >("A->B-C-A");
      CHECK_EQ(gum::graph::anteriors(g, gum::NodeSet{g.idFromName("A").value()}),
               g.nodes().asNodeSet());
    }
    {
      const auto cpdag = gum::CPDAG::fromDAG(gum::fastGraph< gum::DAG >("A->B->C;D->C"));
      const auto c     = cpdag.idFromName("C").value();
      CHECK_EQ(gum::graph::anteriors(cpdag, gum::NodeSet{c}),
               gum::graph::anteriors(cpdag.graph(), gum::NodeSet{c}));
    }
  }

  GUM_TEST(ConnectedComponentsOfMarkedGraphs) {
    // all the edges, whatever their marks
    const auto g  = gum::fastMarkedGraph("A-B->C<->D;E o-o F;G");
    const auto cc = gum::graph::connectedComponents(g);
    const auto id = [&g](const std::string& name) { return g.idFromName(name).value(); };
    CHECK_EQ(cc[id("A")], cc[id("D")]);
    CHECK_EQ(cc[id("E")], cc[id("F")]);
    CHECK_NE(cc[id("A")], cc[id("E")]);
    CHECK_NE(cc[id("G")], cc[id("A")]);
    CHECK_EQ(cc, g.connectedComponents());
    const auto mag = gum::MAG::fromMarkedGraph(gum::fastMarkedGraph("A-B->C<->D;E"));
    const auto mcc = gum::graph::connectedComponents(mag);
    CHECK_EQ(mcc[mag.idFromName("A").value()], mcc[mag.idFromName("D").value()]);
    CHECK_NE(mcc[mag.idFromName("A").value()], mcc[mag.idFromName("E").value()]);
  }

  GUM_TEST(MAGOverloads) {
    const auto mag = gum::MAG::fromMarkedGraph(gum::fastMarkedGraph("A-B->C<->D"));
    const auto c   = mag.idFromName("C").value();
    CHECK_EQ(gum::graph::ancestors(mag, gum::NodeSet{c}),
             gum::graph::ancestors(mag.graph(), gum::NodeSet{c}));
    CHECK_EQ(gum::graph::anteriors(mag, gum::NodeSet{c}),
             gum::graph::anteriors(mag.graph(), gum::NodeSet{c}));
    CHECK(gum::graph::isAncestral(mag));
    CHECK(gum::graph::isMaximal(mag));
  }

  GUM_TEST(SeparationCriterionOfEachKindOfGraph) {
    // the three criteria are equal on a DAG and on the representations of its class
    const auto dag   = gum::fastGraph< gum::DAG >("A->C<-B;C->D");
    const auto id    = [&dag](const std::string& name) { return dag.idFromName(name).value(); };
    const auto cpdag = gum::CPDAG::fromDAG(dag);
    const auto mpdag = gum::MPDAG::fromPDAG(cpdag.graph());
    const auto mag   = gum::MAG::fromDAG(dag);
    const gum::NodeSet A{id("A")}, B{id("B")}, C{id("C")}, D{id("D")}, none;
    for (const auto& Z: {none, C, D}) {
      const bool expected = gum::graph::dSeparated(dag, A, B, Z);
      CHECK_EQ(gum::graph::separation(dag, A, B, Z), expected);
      CHECK_EQ(gum::graph::separation(cpdag, A, B, Z), expected);
      CHECK_EQ(gum::graph::separation(mpdag, A, B, Z), expected);
      CHECK_EQ(gum::graph::separation(mag, A, B, Z), expected);
    }
    CHECK(gum::graph::separation(dag, A, B, none));
    CHECK(!gum::graph::separation(dag, A, B, D));

    // c-separation on a chain graph: the parents of the chain component {B, C} are married
    const auto chain = gum::fastGraph< gum::ChainGraph >("A->B-C<-D");
    const auto cid   = [&chain](const std::string& name) { return chain.idFromName(name).value(); };
    const gum::NodeSet cA{cid("A")}, cB{cid("B")}, cD{cid("D")};
    CHECK(gum::graph::separation(chain, cA, cD, gum::NodeSet()));
    CHECK(!gum::graph::separation(chain, cA, cD, cB));
    CHECK_EQ(gum::graph::separation(chain, cA, cD, cB), gum::graph::cSeparated(chain, cA, cD, cB));

    CHECK_THROWS_AS(gum::graph::separation(dag, A, A, none), gum::InvalidArgument);
  }

  GUM_TEST(SeparationInAClassWithReversibleEdges) {
    // A-B and A-C are reversible in the class of the DAG: its three members (B<-A->C, B->A->C,
    // B<-A<-C) have the same d-separations, those of the CPDAG; the MPDAG which imposes B->A only
    // keeps B->A->C (Meek R1)
    const auto dag   = gum::fastGraph< gum::DAG >("B<-A->C;B->D<-C;D->E");
    const auto id    = [&dag](const std::string& name) { return dag.idFromName(name).value(); };
    const auto cpdag = gum::CPDAG::fromDAG(dag);
    REQUIRE(cpdag.existsEdge(id("A"), id("B")));
    REQUIRE(cpdag.existsEdge(id("A"), id("C")));
    const auto mpdag = gum::MPDAG::fromCPDAG(cpdag, gum::ArcSet{gum::Arc(id("B"), id("A"))});
    REQUIRE(mpdag.existsArc(id("A"), id("C")));

    std::vector< gum::DAG > members{dag, dag, dag};
    members[1].eraseArc(gum::Arc(id("A"), id("B")));
    members[1].addArc(id("B"), id("A"));
    members[2].eraseArc(gum::Arc(id("A"), id("C")));
    members[2].addArc(id("C"), id("A"));
    for (const auto& member: members)
      REQUIRE(gum::CPDAG::fromDAG(member) == cpdag);

    // every pair of nodes, every conditioning set of the other nodes
    const std::vector< gum::NodeId > nodes(dag.nodes().begin(), dag.nodes().end());
    int                              nbSeparated = 0;
    for (std::size_t x = 0; x < nodes.size(); ++x)
      for (std::size_t y = x + 1; y < nodes.size(); ++y) {
        std::vector< gum::NodeId > others;
        for (const auto n: nodes)
          if (n != nodes[x] && n != nodes[y]) others.push_back(n);
        for (unsigned int mask = 0; mask < (1u << others.size()); ++mask) {
          gum::NodeSet Z;
          for (std::size_t i = 0; i < others.size(); ++i)
            if (mask & (1u << i)) Z.insert(others[i]);
          const gum::NodeSet X{nodes[x]}, Y{nodes[y]};
          const bool         expected = gum::graph::dSeparated(dag, X, Y, Z);
          if (expected) ++nbSeparated;
          for (const auto& member: members)
            CHECK_EQ(gum::graph::dSeparated(member, X, Y, Z), expected);
          CHECK_EQ(gum::graph::separation(cpdag, X, Y, Z), expected);
          CHECK_EQ(gum::graph::separation(mpdag, X, Y, Z), expected);
        }
      }
    CHECK_GT(nbSeparated, 0);   // the comparisons are not all about connected pairs

    // a separation which differs from the one of the skeleton: B and C are separated by A only
    CHECK(gum::graph::separation(cpdag,
                                 gum::NodeSet{id("B")},
                                 gum::NodeSet{id("C")},
                                 gum::NodeSet{id("A")}));
    CHECK(!gum::graph::separation(cpdag,
                                  gum::NodeSet{id("B")},
                                  gum::NodeSet{id("C")},
                                  gum::NodeSet{id("A"), id("D")}));
  }

  GUM_TEST(SeparationOfAPDAGIsTheCSeparation) {
    // separation(const PDAG&) on a PDAG which is not a chain graph object: the parents A and D of
    // the chain component {B, C} are married
    const auto pdag = gum::fastGraph< gum::PDAG >("A->B-C<-D;C->E");
    const auto id   = [&pdag](const std::string& name) { return pdag.idFromName(name).value(); };
    const gum::NodeSet A{id("A")}, B{id("B")}, C{id("C")}, D{id("D")}, E{id("E")}, none;
    CHECK(gum::graph::separation(pdag, A, D, none));
    CHECK(!gum::graph::separation(pdag, A, D, B));
    CHECK(!gum::graph::separation(pdag, A, E, none));
    CHECK(gum::graph::separation(pdag, A, E, C));
    for (const auto& Z: {none, B, C, E})
      CHECK_EQ(gum::graph::separation(pdag, A, D, Z), gum::graph::cSeparated(pdag, A, D, Z));
  }

}   // namespace gum_tests
