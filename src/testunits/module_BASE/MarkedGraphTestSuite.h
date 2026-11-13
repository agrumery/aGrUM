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

#include <agrum/base/core/configuration/configuration.h>
#include <agrum/base/core/dot/dotOptions.h>
#include <agrum/base/graphs/fastGraph.h>
#include <agrum/base/graphs/fastMarkedGraph.h>
#include <agrum/base/graphs/markedGraph.h>
#include <agrum/base/graphs/PAG.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {

  /// G can erase the edges of a node without the virtual erasure of each edge
  template < typename G >
  concept CanEraseNeighboursUnvirtualized
      = requires(G& g) { g.unvirtualizedEraseNeighbours(gum::NodeId(0)); };

  struct MarkedGraphTestSuite {};

  GUM_TEST(AddEdgeCreatesOrUpdatesTheMarks) {
    gum::MarkedGraph g;
    g.addNodes(3);
    g.addEdge(0, 1, gum::EdgeMark::Tail, gum::EdgeMark::Arrowhead);
    CHECK(g.isDefinitelyDirected(0, 1));
    CHECK(!g.isDefinitelyDirected(1, 0));

    // an existing edge gets the new marks (in any order of its ends)
    g.addEdge(1, 0, gum::EdgeMark::Arrowhead, gum::EdgeMark::Arrowhead);
    CHECK(g.isBidirected(0, 1));
    CHECK_EQ(g.sizeEdges(), 1u);

    // without marks: circles for a new edge, nothing changed for an existing one
    g.addEdge(1, 2);
    CHECK(g.isCircle(1, 2));
    CHECK(g.isCircle(2, 1));
    g.addEdge(0, 1);
    CHECK(g.isBidirected(0, 1));

    g.setMarkAt(1, 2, gum::EdgeMark::Tail);
    CHECK_EQ(g.markAt(1, 2), gum::EdgeMark::Tail);
    CHECK_EQ(g.markAt(2, 1), gum::EdgeMark::Circle);

    CHECK_THROWS_AS(g.markAt(0, 2), gum::NotFound&);
    CHECK_THROWS_AS(g.setMarkAt(0, 2, gum::EdgeMark::Tail), gum::NotFound&);
    CHECK_THROWS_AS(g.addEdge(0, 7, gum::EdgeMark::Tail, gum::EdgeMark::Tail), gum::InvalidNode&);
    // no loop
    CHECK_THROWS_AS(g.addEdge(1, 1), gum::InvalidEdge&);
    CHECK_THROWS_AS(g.addEdge(1, 1, gum::EdgeMark::Tail, gum::EdgeMark::Arrowhead),
                    gum::InvalidEdge&);
    CHECK(!g.existsEdge(1, 1));
  }

  GUM_TEST(DirectedView) {
    // 0->1, 1<->2, 2-3, 3 o-> 4
    const auto g = gum::fastMarkedGraph("0->1;1<->2;2-3;3 o-> 4");
    CHECK_EQ(g.parents(1), gum::NodeSet{0});
    CHECK_EQ(g.children(0), gum::NodeSet{1});
    CHECK_EQ(g.spouses(1), gum::NodeSet{2});
    CHECK_EQ(g.undirectedNeighbours(2), gum::NodeSet{3});
    CHECK(g.parents(4).empty());   // o-> is not a definite arc
    CHECK_EQ(g.neighbours(2), (gum::NodeSet{1, 3}));
    CHECK(g.isDefCollider(0, 1, 2));
    CHECK(g.isUndirected(3, 2));
  }

  GUM_TEST(ErasuresKeepTheMarksConsistent) {
    auto g = gum::fastMarkedGraph("0->1;1<->2;2 o-o 0");
    g.eraseNode(1);
    CHECK_EQ(g.sizeEdges(), 1u);
    CHECK(g.isCircle(0, 2));
    // a new edge on a pair whose edge was erased starts with circles
    g.addNodeWithId(1);
    g.addEdge(0, 1);
    CHECK(g.isCircle(0, 1));
    g.eraseEdge(gum::Edge(0, 2));
    CHECK_THROWS_AS(g.markAt(0, 2), gum::NotFound&);
    g.clearEdges();
    CHECK_EQ(g.sizeEdges(), 0u);
    g.clear();
    CHECK(g.empty());
  }

  GUM_TEST(CopyEqualityNamesAndSkeleton) {
    auto g = gum::fastMarkedGraph("A->B;B<->C;D");
    auto h = g;
    CHECK(h == g);
    h.setMarkAt(h.idFromName("B").value(), h.idFromName("C").value(), gum::EdgeMark::Tail);
    CHECK(!(h == g));

    const auto skeleton = g.skeleton();
    CHECK_EQ(skeleton.sizeEdges(), 2u);
    CHECK_EQ(skeleton.nameFromId(g.idFromName("A").value()), "A");
    const auto components = g.connectedComponents();
    CHECK_EQ(components.size(), 4u);   // one entry per node
    gum::NodeSet distinct;
    for (const auto& [node, component]: components)
      distinct.insert(component);
    CHECK_EQ(distinct.size(), 2u);
    CHECK_EQ(components[g.idFromName("A").value()], components[g.idFromName("C").value()]);
    CHECK_NE(components[g.idFromName("A").value()], components[g.idFromName("D").value()]);
  }

  GUM_TEST(FastMarkedGraph) {
    const auto g  = gum::fastMarkedGraph("A o-> B; B <-o C; C o-o D; D<->E; E-F; F<-G");
    const auto id = [&g](const std::string& name) { return g.idFromName(name).value(); };
    CHECK(g.isCircle(id("B"), id("A")));
    CHECK(g.isArrowhead(id("A"), id("B")));
    CHECK(g.isArrowhead(id("C"), id("B")));
    CHECK(g.isCircle(id("B"), id("C")));
    CHECK(g.isCircle(id("D"), id("C")));
    CHECK(g.isCircle(id("C"), id("D")));
    CHECK(g.isBidirected(id("D"), id("E")));
    CHECK(g.isUndirected(id("E"), id("F")));
    CHECK(g.isDefinitelyDirected(id("G"), id("F")));

    // a circle needs a space: "Ao->B" is an arc from the node "Ao"
    const auto h = gum::fastMarkedGraph("Ao->B");
    CHECK(h.idFromName("Ao").has_value());
    CHECK(h.isDefinitelyDirected(h.idFromName("Ao").value(), h.idFromName("B").value()));

    // a circle needs a node name on its side: else "o" is a node
    for (const auto& desc: {"o->B", "A->o->B", "A -> o-> B", "A-o", "A -o -> B"}) {
      const auto k = gum::fastMarkedGraph(desc);
      CHECK_MESSAGE(k.idFromName("o").has_value(), desc);
      for (const auto& e: k.edges()) {
        CHECK_MESSAGE(!k.isCircle(e.first(), e.second()), desc);
        CHECK_MESSAGE(!k.isCircle(e.second(), e.first()), desc);
      }
    }
    const auto ao = gum::fastMarkedGraph("A->o->B");
    CHECK(ao.isDefinitelyDirected(ao.idFromName("A").value(), ao.idFromName("o").value()));
    CHECK(ao.isDefinitelyDirected(ao.idFromName("o").value(), ao.idFromName("B").value()));
    const auto oo = gum::fastMarkedGraph("A o-o o");
    CHECK_EQ(oo.size(), gum::Size(2));
    CHECK(oo.isCircle(oo.idFromName("A").value(), oo.idFromName("o").value()));
    CHECK(oo.isCircle(oo.idFromName("o").value(), oo.idFromName("A").value()));

    // ids, and the PAG type
    const auto p = gum::fastMarkedGraph< gum::PAG >("0 o-> 1;1<->2");
    CHECK(p.isCircle(1, 0));
    CHECK(p.isBidirected(1, 2));

    CHECK_THROWS_AS(gum::fastMarkedGraph("A->"), gum::InvalidArgument&);
    CHECK_THROWS_AS(gum::fastMarkedGraph("A->A"), gum::InvalidEdge&);
  }

  GUM_TEST(FastMarkedGraphConflictingMarks) {
    // the same marks repeated: accepted
    const auto g = gum::fastMarkedGraph("A->B;A->B;B<-A");
    CHECK_EQ(g.sizeEdges(), gum::Size(1));
    CHECK(g.isDefinitelyDirected(g.idFromName("A").value(), g.idFromName("B").value()));
    CHECK_EQ(gum::fastMarkedGraph("0 o-o 1;1 o-o 0").sizeEdges(), gum::Size(1));

    // different marks: rejected instead of keeping the last ones
    CHECK_THROWS_AS(gum::fastMarkedGraph("A->B;B->A"), gum::InvalidEdge&);
    CHECK_THROWS_AS(gum::fastMarkedGraph("A o-o B;A->B"), gum::InvalidEdge&);
    CHECK_THROWS_AS(gum::fastMarkedGraph("A->B->C-B"), gum::InvalidEdge&);
    CHECK_THROWS_AS(gum::fastMarkedGraph< gum::PAG >("0 o-> 1;0<->1"), gum::InvalidEdge&);
  }

  GUM_TEST(FastMarkedGraphMalformedNames) {
    // '<', '>' and the inner blanks are not part of a name: "A o -B" was the edge "A o"-B
    for (const std::string_view desc: {"A>-B", "A-<B", "A o -B", "A B->C"}) {
      CHECK_THROWS_AS(gum::fastMarkedGraph(desc), gum::InvalidArgument&);
    }
    CHECK_EQ(gum::fastMarkedGraph("A<->B").sizeEdges(), gum::Size(1));
    CHECK_THROWS_AS(gum::fastMarkedGraph("0->2000000000"), gum::InvalidArgument&);
  }

  GUM_TEST(FastMarkedGraphQuotedNames) {
    // a circle may touch a quoted name
    for (const std::string_view desc: {"'A'o->'B'", "'A' o-> 'B'", "'A'o->B", "B <-o'A'"}) {
      const auto g = gum::fastMarkedGraph(desc);
      const auto a = g.idFromName("A").value();
      const auto b = g.idFromName("B").value();
      CHECK_MESSAGE(g.isCircle(b, a), desc);
      CHECK_MESSAGE(g.isArrowhead(a, b), desc);
    }
    const auto g = gum::fastMarkedGraph("'Visit Asia' o-o 'l''eau'");
    CHECK(g.isCircle(g.idFromName("Visit Asia").value(), g.idFromName("l'eau").value()));
    // a node named o, and a node named Ao
    CHECK(gum::fastMarkedGraph("o->B").idFromName("o").has_value());
    CHECK(gum::fastMarkedGraph("A->o->B").idFromName("o").has_value());
    CHECK(gum::fastMarkedGraph("Ao->B").idFromName("Ao").has_value());
    // text after a quoted name
    CHECK_THROWS_AS(gum::fastMarkedGraph("'A'o"), gum::InvalidArgument&);
    CHECK_THROWS_AS(gum::fastMarkedGraph("'A->B"), gum::InvalidArgument&);
  }

  GUM_TEST(ToStringAndToDot) {
    ConfigurationGuard guard;
    const auto         g = gum::fastMarkedGraph("0->1;1<->2;2 o-o 3");
    const auto         s = g.toString();
    CHECK_NE(s.find("0 --> 1"), std::string::npos);
    CHECK_NE(s.find("1 <-> 2"), std::string::npos);
    CHECK_NE(s.find("2 o-o 3"), std::string::npos);

    auto dot = g.toDot();
    CHECK_NE(dot.find("  0 -> 1 [dir=\"both\", arrowhead=\"normal\", arrowtail=\"none\"];\n"),
             std::string::npos);
    CHECK_NE(dot.find("  2 -> 3 [dir=\"both\", arrowhead=\"odot\", arrowtail=\"odot\"];\n"),
             std::string::npos);

    gum::config().set("dot", "bidirectedEdgeColor", "red");
    gum::config().set("dot", "circleMarkEdgeColor", "blue");
    dot = g.toDot();
    CHECK_NE(dot.find("arrowtail=\"normal\", color=\"red\""), std::string::npos);
    CHECK_NE(dot.find("arrowtail=\"odot\", color=\"blue\""), std::string::npos);

    gum::dot::DotOptions options;
    options.setArcColor(1, 2, "green");
    CHECK_NE(g.toDot(options).find("arrowtail=\"normal\", color=\"green\""), std::string::npos);
  }

  GUM_TEST(ClearNodesErasesTheMarks) {
    auto                g     = gum::fastMarkedGraph("0->1");
    gum::NodeGraphPart& nodes = g;
    nodes.clearNodes();   // virtual: the edges and their marks are erased too
    CHECK(g.empty());
    CHECK_EQ(g.sizeEdges(), gum::Size(0));
    g.addNodeWithId(0);
    g.addNodeWithId(1);
    CHECK_THROWS_AS(g.markAt(0, 1), gum::NotFound&);
    // erasing the edges without their marks is not accessible from a marked graph
    static_assert(CanEraseNeighboursUnvirtualized< gum::UndiGraph >);
    static_assert(!CanEraseNeighboursUnvirtualized< gum::MarkedGraph >);
  }

}   // namespace gum_tests
