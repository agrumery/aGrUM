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

#include <vector>

#include <agrum/base/graphs/algorithms/ancestral.h>
#include <agrum/base/graphs/fastGraph.h>
#include <agrum/base/graphs/fastMarkedGraph.h>
#include <agrum/base/graphs/MAG.h>

#include <agrum/base/core/utils_random.h>
#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/randomGraphs.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {

  struct MAGTestSuite {
    /// the subsets of nodes
    static std::vector< gum::NodeSet > subsets(const std::vector< gum::NodeId >& nodes) {
      std::vector< gum::NodeSet > result;
      for (std::size_t mask = 0; mask < (std::size_t(1) << nodes.size()); ++mask) {
        gum::NodeSet s;
        for (std::size_t i = 0; i < nodes.size(); ++i)
          if (mask & (std::size_t(1) << i)) s.insert(nodes[i]);
        result.push_back(s);
      }
      return result;
    }
  };

  GUM_TEST(FromDAGExamples) {
    // Richardson and Spirtes 2002, Fig. 10: a->x<-t->y<-b
    gum::DAG dag;
    dag.addNodes(5);   // a=0, x=1, t=2, y=3, b=4
    dag.addArc(0, 1);
    dag.addArc(2, 1);
    dag.addArc(2, 3);
    dag.addArc(4, 3);
    dag.setName(0, "a");

    // t latent: a->x<->y<-b
    auto mag = gum::MAG::fromDAG(dag, gum::NodeSet{2});
    CHECK_EQ(mag.size(), 4u);
    CHECK_EQ(mag.sizeEdges(), 3u);
    CHECK(mag.graph().isDefinitelyDirected(0, 1));
    CHECK(mag.graph().isBidirected(1, 3));
    CHECK(mag.graph().isDefinitelyDirected(4, 3));
    CHECK_EQ(mag.nameFromId(0), "a");

    // t selected: only a->x and b->y
    mag = gum::MAG::fromDAG(dag, gum::NodeSet(), gum::NodeSet{2});
    CHECK_EQ(mag.sizeEdges(), 2u);
    CHECK(mag.graph().isDefinitelyDirected(0, 1));
    CHECK(!mag.existsEdge(1, 3));

    // without latent and selected nodes: the DAG itself
    mag = gum::MAG::fromDAG(dag);
    CHECK_EQ(mag.sizeEdges(), 4u);
    for (const auto& arc: dag.arcs())
      CHECK(mag.graph().isDefinitelyDirected(arc.tail(), arc.head()));

    CHECK_THROWS_AS(gum::MAG::fromDAG(dag, gum::NodeSet{2}, gum::NodeSet{2}),
                    gum::InvalidArgument&);
    CHECK_THROWS_AS(gum::MAG::fromDAG(dag, gum::NodeSet{9}), gum::InvalidArgument&);
  }

  GUM_TEST(FromDAGVsNaive) {
    for (int i = 0; i < 60; ++i) {
      const auto   dag = randomDAG(5 + gum::randomValue(4), 0.4);
      gum::NodeSet latents, selection;
      for (const auto node: dag.nodes()) {
        const auto p = gum::randomProba();
        if (p < 0.2) latents.insert(node);
        else if (p < 0.3) selection.insert(node);
      }
      const auto mag = gum::MAG::fromDAG(dag, latents, selection);
      CHECK(gum::graph::isAncestral(mag.graph()));
      CHECK(gum::graph::isMaximal(mag.graph()));

      std::vector< gum::NodeId > observed(mag.nodes().begin(), mag.nodes().end());
      for (std::size_t a = 0; a < observed.size(); ++a)
        for (std::size_t b = a + 1; b < observed.size(); ++b) {
          std::vector< gum::NodeId > others;
          for (const auto o: observed)
            if (o != observed[a] && o != observed[b]) others.push_back(o);
          bool separable = false;
          for (const auto& z: subsets(others)) {
            const bool dsep = dag.dSeparation(observed[a], observed[b], z + selection);
            // the m-separation of the MAG is the d-separation of the DAG given the selection
            CHECK_EQ(mag.mSeparation(observed[a], observed[b], z), dsep);
            separable = separable || dsep;
          }
          // adjacency iff no separation (Richardson and Spirtes 2002, Theorem 4.2 (v))
          CHECK_EQ(mag.existsEdge(observed[a], observed[b]), !separable);
        }
      // X and Y must be disjoint, as for the d-separation
      if (!observed.empty()) {
        const gum::NodeSet x{observed.front()};
        CHECK_THROWS_AS(mag.mSeparation(x, x, gum::NodeSet()), gum::InvalidArgument&);
      }
    }
  }

  GUM_TEST(FromMarkedGraph) {
    // a DAG is a MAG
    CHECK_NOTHROW(gum::MAG::fromMarkedGraph(gum::fastMarkedGraph("A->B->C;A->C")));
    // a circle
    CHECK_THROWS_AS(gum::MAG::fromMarkedGraph(gum::fastMarkedGraph("A o-> B")),
                    gum::InvalidArgument&);
    // an almost directed cycle: A->B->C and A<->C
    CHECK_THROWS_AS(gum::MAG::fromMarkedGraph(gum::fastMarkedGraph("A->B->C;A<->C")),
                    gum::InvalidArgument&);
    // an arrowhead at a node with an undirected edge
    CHECK_THROWS_AS(gum::MAG::fromMarkedGraph(gum::fastMarkedGraph("A-B<-C")),
                    gum::InvalidArgument&);
    // ancestral but not maximal (Zhang 2008, Fig. 1(a)): C<->A<->B<->D, A->D, B->C, with the
    // inducing path C,A,B,D between the non adjacent C and D
    const auto g = gum::fastMarkedGraph("C<->A<->B<->D;A->D;B->C");
    CHECK(gum::graph::isAncestral(g));
    CHECK(!gum::graph::isMaximal(g));
    CHECK_THROWS_AS(gum::MAG::fromMarkedGraph(g), gum::InvalidArgument&);
  }

  GUM_TEST(FromPAG) {
    // o-> and --o become ->
    auto       mag = gum::MAG::fromPAG(gum::fastMarkedGraph< gum::PAG >("A o-> B;C o-> B;B -o D"));
    const auto id  = [&mag](const char* name) { return mag.idFromName(name).value(); };
    CHECK(mag.graph().isDefinitelyDirected(id("A"), id("B")));
    CHECK(mag.graph().isDefinitelyDirected(id("C"), id("B")));
    CHECK(mag.graph().isDefinitelyDirected(id("B"), id("D")));

    // the edges o-o: a DAG without unshielded collider
    mag = gum::MAG::fromPAG(gum::fastMarkedGraph< gum::PAG >("0 o-o 1;1 o-o 2"));
    CHECK_EQ(mag.sizeEdges(), 2u);
    CHECK(!(mag.graph().isArrowhead(0, 1) && mag.graph().isArrowhead(2, 1)));

    // not a valid PAG: a non chordal cycle of edges o-o
    CHECK_THROWS_AS(
        gum::MAG::fromPAG(gum::fastMarkedGraph< gum::PAG >("0 o-o 1;1 o-o 2;2 o-o 3;3 o-o 0")),
        gum::InvalidArgument&);
  }

  GUM_TEST(RepresentativeGraph) {
    const auto pag
        = gum::fastMarkedGraph< gum::PAG >("A o-> B;B -o C;C o-o D;D o-o E;E <-> F;F - G");
    const auto id = [&pag](const char* name) { return pag.idFromName(name).value(); };

    // the circle component: the edges o-o only, over all the nodes
    const auto circles = gum::graph::circleComponent(pag);
    CHECK_EQ(circles.size(), pag.size());
    CHECK_EQ(circles.sizeEdges(), 2u);
    CHECK_EQ(circles.sizeArcs(), 0u);
    CHECK(circles.existsEdge(id("C"), id("D")));
    CHECK(circles.existsEdge(id("D"), id("E")));

    gum::DAG orientation;
    for (const auto node: pag.nodes())
      orientation.addNodeWithId(node);
    orientation.addArc(id("D"), id("C"));
    orientation.addArc(id("D"), id("E"));
    const auto g = gum::graph::representativeGraph(pag, orientation);
    CHECK(g.isDefinitelyDirected(id("A"), id("B")));   // o->
    CHECK(g.isDefinitelyDirected(id("B"), id("C")));   // -o
    CHECK(g.isDefinitelyDirected(id("D"), id("C")));   // o-o, oriented
    CHECK(g.isDefinitelyDirected(id("D"), id("E")));
    CHECK(g.isBidirected(id("E"), id("F")));           // kept
    CHECK(g.isUndirected(id("F"), id("G")));
    CHECK_EQ(g.nameFromId(id("A")), "A");

    // an edge o-o left without orientation, an arc which is not an edge o-o
    gum::DAG partial(orientation);
    partial.eraseArc(gum::Arc(id("D"), id("E")));
    CHECK_THROWS_AS(gum::graph::representativeGraph(pag, partial), gum::InvalidArgument&);
    gum::DAG extra(orientation);
    extra.addArc(id("A"), id("G"));
    CHECK_THROWS_AS(gum::graph::representativeGraph(pag, extra), gum::InvalidArgument&);
  }

  GUM_TEST(CopyAndToDot) {
    gum::DAG dag;
    dag.addNodes(3);
    dag.addArc(0, 1);
    dag.addArc(2, 1);
    const auto mag  = gum::MAG::fromDAG(dag);
    auto       copy = mag;
    CHECK(copy == mag);
    copy = gum::MAG::fromDAG(dag, gum::NodeSet{2});
    CHECK(!(copy == mag));
    CHECK_NE(mag.toDot().find("arrowhead=\"normal\""), std::string::npos);
    CHECK(!mag.toString().empty());
  }

  GUM_TEST(NeighboursAndAdjacents) {
    // neighbours: the undirected edges only (as for a mixed graph); adjacents: any edge
    const auto mag = gum::MAG::fromMarkedGraph(gum::fastMarkedGraph("A-B->C<->D"));
    const auto id  = [&mag](const std::string& name) { return *mag.idFromName(name); };
    CHECK_EQ(mag.neighbours(id("B")), gum::NodeSet{id("A")});
    CHECK_EQ(mag.adjacents(id("B")), (gum::NodeSet{id("A"), id("C")}));
    CHECK(mag.neighbours(id("C")).empty());
    CHECK_EQ(mag.adjacents(id("C")), (gum::NodeSet{id("B"), id("D")}));
    CHECK_EQ(mag.parents(id("C")), gum::NodeSet{id("B")});
    CHECK_EQ(mag.spouses(id("C")), gum::NodeSet{id("D")});
    CHECK_EQ(mag.children(id("B")), gum::NodeSet{id("C")});
    CHECK(mag.children(id("C")).empty());
  }

  GUM_TEST(AncestorsAndAnteriors) {
    // ancestors: directed paths only; anteriors: directed and undirected edges; a node of x belongs
    // to the result only if it has such a path to a node of x
    const auto g  = gum::fastMarkedGraph("A-B->C<->D");
    const auto id = [&g](const std::string& name) { return g.idFromName(name).value(); };
    CHECK_EQ(gum::graph::ancestors(g, gum::NodeSet{id("C")}), gum::NodeSet{id("B")});
    CHECK_EQ(gum::graph::anteriors(g, gum::NodeSet{id("C")}), (gum::NodeSet{id("A"), id("B")}));
    // the bidirected edge is not followed
    CHECK(gum::graph::ancestors(g, gum::NodeSet{id("D")}).empty());
    CHECK(gum::graph::anteriors(g, gum::NodeSet{id("D")}).empty());
    // a set of nodes: the union; B is an ancestor of C, hence of {B, C}
    CHECK_EQ(gum::graph::anteriors(g, gum::NodeSet{id("C"), id("D")}),
             (gum::NodeSet{id("A"), id("B")}));
    CHECK_EQ(gum::graph::ancestors(g, gum::NodeSet{id("B"), id("C")}), gum::NodeSet{id("B")});
    CHECK_EQ(gum::graph::anteriors(g, gum::NodeSet{id("B"), id("C")}),
             (gum::NodeSet{id("A"), id("B")}));
    CHECK(gum::graph::anteriors(g, gum::NodeSet()).empty());
  }

  GUM_TEST(ReadAccess) {
    const auto mag = gum::MAG::fromMarkedGraph(gum::fastMarkedGraph("A-B->C;D<->E"));
    const auto id  = [&mag](const std::string& name) { return *mag.idFromName(name); };
    CHECK(!mag.empty());
    CHECK(gum::MAG::fromMarkedGraph(gum::MarkedGraph()).empty());
    CHECK(mag.hasName(id("A")));
    const auto cc = mag.connectedComponents();
    CHECK_EQ(cc[id("A")], cc[id("C")]);
    CHECK_NE(cc[id("A")], cc[id("D")]);
    const auto skeleton = mag.skeleton();
    CHECK_EQ(skeleton.size(), gum::Size(5));
    CHECK_EQ(skeleton.sizeEdges(), gum::Size(3));
    CHECK(skeleton.existsEdge(id("D"), id("E")));
    CHECK_EQ(skeleton.nameFromId(id("C")), "C");
  }

  GUM_TEST(MSeparationOfObservedNodesAsDSeparation) {
    // without latent nor selection node, the MAG of a DAG is the DAG: the m-separation is the
    // d-separation, also when z meets x or y (an observed node is separated from any node)
    for (int i = 0; i < 40; ++i) {
      const auto dag = randomDAG(4 + gum::randomValue(4), 0.4);
      const auto mag = gum::MAG::fromDAG(dag);
      for (int k = 0; k < 20; ++k) {
        gum::NodeSet x, y, z;
        for (const auto node: dag.nodes()) {
          const auto p = gum::randomProba();
          if (p < 0.3) x.insert(node);
          else if (p < 0.6) y.insert(node);
          if (gum::randomProba() < 0.4) z.insert(node);
        }
        CHECK_EQ(mag.mSeparation(x, y, z), dag.dSeparation(x, y, z));
        CHECK_EQ(mag.mSeparation(y, x, z), mag.mSeparation(x, y, z));
      }
    }

    // a->x<-y, x observed: a and y are connected through the collider x, x being in X or not
    const auto mag = gum::MAG::fromMarkedGraph(gum::fastMarkedGraph("a->x<-y"));
    const auto a   = *mag.idFromName("a");
    const auto x   = *mag.idFromName("x");
    const auto y   = *mag.idFromName("y");
    CHECK_FALSE(mag.mSeparation(gum::NodeSet{a, x}, gum::NodeSet{y}, gum::NodeSet{x}));
    CHECK_FALSE(mag.mSeparation(gum::NodeSet{y}, gum::NodeSet{a, x}, gum::NodeSet{x}));
    CHECK(mag.mSeparation(gum::NodeSet{x}, gum::NodeSet{y}, gum::NodeSet{x}));
    CHECK(mag.mSeparation(gum::NodeSet{y}, gum::NodeSet{x}, gum::NodeSet{x}));
  }

}   // namespace gum_tests
