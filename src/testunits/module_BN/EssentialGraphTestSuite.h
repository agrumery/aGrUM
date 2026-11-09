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
#include <string>
#include <vector>

#include <agrum/base/core/dot/dotOptions.h>
#include <agrum/base/graphs/algorithms/orientation.h>
#include <agrum/BN/algorithms/essentialGraph.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/generator/simpleBayesNetGenerator.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {
  struct EssentialGraphTestSuite {
    public:
    /// reference: an arc a->b is strongly protected (Andersson, Madigan and Perlman 1997, Th. 4.1)
    static bool stronglyProtected(const gum::MixedGraph& mg, gum::NodeId a, gum::NodeId b) {
      for (const auto c: mg.parents(a))   // (a) c->a->b, c and b not adjacent
        if (!mg.existsArc(c, b)) return true;
      bool oneFound = false;
      for (const auto c: mg.parents(b)) {
        if (c == a) continue;
        if (mg.existsArc(a, c)) return true;                            // (c) a->c->b
        if (!mg.existsEdge(a, c) && !mg.existsArc(c, a)) return true;   // (b) a->b<-c
        if (mg.existsEdge(c, a)) {                                      // (d) two such c
          if (oneFound) return true;
          oneFound = true;
        }
      }
      return false;
    }

    /// reference: the arcs which are not strongly protected become edges, until a fixpoint
    static gum::MixedGraph naiveEssentialGraph(const gum::DAG& dag) {
      gum::MixedGraph mg;
      for (const auto node: dag.nodes())
        mg.addNodeWithId(node);
      for (const auto& arc: dag.arcs())
        mg.addArc(arc.tail(), arc.head());
      std::vector< gum::Arc > unprotected;
      do {
        unprotected.clear();
        for (const auto& arc: mg.arcs())
          if (!stronglyProtected(mg, arc.tail(), arc.head())) unprotected.push_back(arc);
        for (const auto& arc: unprotected) {
          mg.eraseArc(arc);
          mg.addEdge(arc.tail(), arc.head());
        }
      } while (!unprotected.empty());
      return mg;
    }
  };

  GUM_TEST(Chain) {
    auto bn = gum::BayesNet< float >::fastPrototype("a->b->c");
    auto eg = gum::EssentialGraph(bn);

    CHECK_EQ(eg.size(), 3u);
    CHECK_EQ(eg.sizeArcs(), 0u);
    CHECK_EQ(eg.sizeEdges(), 2u);
  }

  GUM_TEST(Vstructure) {
    auto bn = gum::BayesNet< float >::fastPrototype("a->b;c->b");
    auto eg = gum::EssentialGraph(bn);

    CHECK_EQ(eg.size(), 3u);
    CHECK_EQ(eg.sizeArcs(), 2u);
    CHECK_EQ(eg.sizeEdges(), 0u);
  }

  GUM_TEST(CaseD) {
    auto bn = gum::BayesNet< float >::fastPrototype("a->b;c1->b;c2->b;a->c1;a->c2");
    auto eg = gum::EssentialGraph(bn);

    CHECK_EQ(eg.size(), 4u);
    CHECK_EQ(eg.sizeArcs(), 3u);
    CHECK_EQ(eg.sizeEdges(), 2u);
  }

  GUM_TEST(Notebook1) {
    auto bn = gum::BayesNet< float >::fastPrototype(
        "A->B->C->D;E->B;F->G->D;F->H->I;E->J->K->I->M;K->L");
    auto eg = gum::EssentialGraph(bn);

    CHECK_EQ(eg.size(), 13u);
    CHECK_EQ(eg.sizeArcs(), 8u);
    CHECK_EQ(eg.sizeEdges(), 5u);
  }

  GUM_TEST(Notebook2) {
    auto bn = gum::BayesNet< float >::fastPrototype("A->B;C->B;C->D;B->D;A->C");
    auto eg = gum::EssentialGraph(bn);

    CHECK_EQ(eg.size(), 4u);
    CHECK_EQ(eg.sizeArcs(), 0u);
    CHECK_EQ(eg.sizeEdges(), 5u);
  }

  GUM_TEST(Notebook3) {
    auto bn = gum::BayesNet< float >::fastPrototype("Z->X->U;Y->X;Y->W");
    auto eg = gum::EssentialGraph(bn);

    CHECK_EQ(eg.size(), 5u);
    CHECK_EQ(eg.sizeArcs(), 3u);
    CHECK_EQ(eg.sizeEdges(), 1u);
  }

  GUM_TEST(Skeleton) {
    auto bn   = gum::BayesNet< float >::fastPrototype("Z->X->U;Y->X;Y->W");
    auto eg   = gum::EssentialGraph(bn);
    auto skel = eg.skeleton();

    CHECK_EQ(skel.size(), 5u);
    CHECK_EQ(eg.sizeEdges(), 1u);
    CHECK_EQ(eg.sizeArcs(), 3u);

    CHECK_EQ(eg.idFromName("Z"), 0u);
    CHECK_EQ(eg.idFromName("X"), 1u);
    CHECK_EQ(eg.idFromName("U"), 2u);
    CHECK_EQ(eg.idFromName("Y"), 3u);
    CHECK_EQ(eg.idFromName("W"), 4u);

    CHECK_EQ(eg.nameFromId(0u), "Z");
    CHECK_EQ(eg.nameFromId(1u), "X");
    CHECK_EQ(eg.nameFromId(2u), "U");
    CHECK_EQ(eg.nameFromId(3u), "Y");
    CHECK_EQ(eg.nameFromId(4u), "W");
  }

  GUM_TEST(NonRegression1) {
    auto bn   = gum::BayesNet< float >::fastPrototype("0->1->2<-0");
    auto eg   = gum::EssentialGraph(bn);
    auto skel = eg.skeleton();

    CHECK_EQ(skel.size(), 3u);
    CHECK_EQ(eg.sizeEdges(), 3u);
    CHECK_EQ(eg.sizeArcs(), 0u);
  }

  GUM_TEST(NonRegression2) {
    auto bn   = gum::BayesNet< float >::fastPrototype("0->1->2<-0;3->1");
    auto eg   = gum::EssentialGraph(bn);
    auto skel = eg.skeleton();

    CHECK_EQ(skel.size(), 4u);
    CHECK_EQ(eg.sizeEdges(), 0u);
    CHECK_EQ(eg.sizeArcs(), 4u);
  }

  GUM_TEST(NonRegression3) {
    auto bn   = gum::BayesNet< float >::fastPrototype("0->1->2->3<-4<-2<-5");
    auto eg   = gum::EssentialGraph(bn);
    auto skel = eg.skeleton();
    CHECK_EQ(skel.size(), 6u);
    CHECK_EQ(eg.sizeEdges(), 2u);
    CHECK_EQ(eg.sizeArcs(), 4u);
  }

  GUM_TEST(AncestorsDescendants) {
    // v-structure a(0)->b(1)<-c(2): arcs are kept directed in essential graph
    {
      auto bn = gum::BayesNet< float >::fastPrototype("a->b;c->b");
      auto eg = gum::EssentialGraph(bn);

      CHECK_EQ(eg.ancestors(1), gum::NodeSet({0, 2}));
      CHECK_EQ(eg.descendants(0), gum::NodeSet({1}));
      CHECK_EQ(eg.descendants(1), gum::NodeSet());
    }
    // chain a(0)->b(1)->c(2): all edges become undirected, no directed paths
    {
      auto bn = gum::BayesNet< float >::fastPrototype("a->b->c");
      auto eg = gum::EssentialGraph(bn);

      CHECK_EQ(eg.descendants(0), gum::NodeSet());
      CHECK_EQ(eg.ancestors(2), gum::NodeSet());
    }
  }

  GUM_TEST(ToDot) {
    ConfigurationGuard guard;
    auto               bn = gum::BayesNet< float >::fastPrototype("a->b;c->b;b->d");
    auto               eg = gum::EssentialGraph(bn);
    // a->b<-c is a v-structure, b->d is compelled by the Meek rules : only arcs

    const auto dot = eg.toDot();
    CHECK_NE(dot.find("digraph {\n"), std::string::npos);
    CHECK_NE(dot.find("  0 [label=\"a\", tooltip=\"(0) a\"];\n"), std::string::npos);
    CHECK_NE(dot.find("  0 -> 1 [color=\"red\"];\n"), std::string::npos);
    CHECK_NE(dot.find("  2 -> 1 [color=\"red\"];\n"), std::string::npos);
    CHECK_NE(dot.find("  1 -> 3 [color=\"red\"];\n"), std::string::npos);
    CHECK_EQ(dot.find("dir=\"none\""), std::string::npos);

    // the options of the caller win
    gum::dot::DotOptions options;
    options.setArcColor(0, 1, "green");
    options.setNodeBgColor(3, "yellow");
    const auto given = eg.toDot(options);
    CHECK_NE(given.find("  0 -> 1 [color=\"green\"];\n"), std::string::npos);
    CHECK_NE(given.find("  2 -> 1 [color=\"red\"];\n"), std::string::npos);
    CHECK_NE(given.find("fillcolor=\"yellow\", fontcolor=\"black\""), std::string::npos);

    // a chain has only undirected edges
    auto smallBn = gum::BayesNet< float >::fastPrototype("a->b");
    auto chain   = gum::EssentialGraph(smallBn);
    CHECK_NE(chain.toDot().find("  0 -> 1 [dir=\"none\"];\n"), std::string::npos);
  }

  GUM_TEST(SameAsStronglyProtectedArcs) {
    // the compelled arcs of Chickering (1995) are the strongly protected arcs of Andersson et al.
    // (1997)
    for (int i = 0; i < 30; ++i) {
      gum::BayesNet< double >                bn;
      gum::SimpleBayesNetGenerator< double > generator(15, 25, 2);
      generator.generateBN(bn);
      const auto eg  = gum::EssentialGraph(bn).pdag();
      const auto ref = naiveEssentialGraph(bn.dag());
      CHECK(eg.arcs() == ref.arcs());
      CHECK(eg.edges() == ref.edges());
    }
  }

  GUM_TEST(NamesAndCopy) {
    auto bn = gum::BayesNet< double >::fastPrototype("a->b->c;d->c");
    auto eg = gum::EssentialGraph(bn);
    CHECK_EQ(eg.nameFromId(bn.idFromName("d")), "d");
    CHECK_EQ(eg.idFromName("b"), bn.idFromName("b"));
    CHECK_FALSE(eg.idFromName("z").has_value());
    CHECK_EQ(eg.skeleton().nameFromId(bn.idFromName("a")), "a");

    // a learned PDAG (here, not the essential graph of bn): kept by copy and assignment
    gum::PDAG  pdag(bn.dag());
    const auto learned = gum::EssentialGraph(bn, pdag);
    const auto copy    = learned;
    CHECK_EQ(copy.sizeArcs(), 3u);
    CHECK_EQ(copy.sizeEdges(), 0u);
    gum::EssentialGraph assigned;
    assigned = learned;
    CHECK_EQ(assigned.sizeArcs(), 3u);
    CHECK_EQ(assigned.nameFromId(bn.idFromName("c")), "c");

    // from a named PDAG only: the essential graph does not depend on a model
    gum::EssentialGraph alone;
    {
      auto tmp = gum::BayesNet< double >::fastPrototype("x->y");
      alone    = gum::EssentialGraph(tmp);
    }
    CHECK_EQ(alone.nameFromId(0), "x");
    CHECK_EQ(gum::EssentialGraph(alone.pdag()).idFromName("y"), 1u);
  }

  GUM_TEST(AsCPDAG) {
    auto       bn = gum::BayesNet< double >::fastPrototype("a->b->c;d->c");
    const auto eg = gum::EssentialGraph(bn);
    CHECK(eg.cpdag().contains(bn.dag()));
    CHECK_EQ(eg.cpdag().nameFromId(bn.idFromName("d")), "d");

    // a PDAG which is not a CPDAG (the chain a->b->c is not compelled)
    CHECK_THROWS_AS(gum::EssentialGraph(bn, gum::PDAG(bn.dag())).cpdag(), gum::InvalidArgument&);
  }

  GUM_TEST(GumFormat) {
    auto       bn   = gum::BayesNet< double >::fastPrototype("a->b->c;d->c");
    const auto eg   = gum::EssentialGraph(bn);
    const auto text = gum::graphToGUMString(eg);
    CHECK_EQ(gum::graphGUMType(text), "EssentialGraph");
    const auto read = gum::graphFromGUMString< gum::EssentialGraph >(text);
    CHECK(read.arcs() == eg.arcs());
    CHECK(read.edges() == eg.edges());
    CHECK_EQ(read.nameFromId(bn.idFromName("d")), "d");
    const auto bgum = GET_RESSOURCES_PATH("outputs/essentialGraph.bgum");
    gum::saveGraphGUM(eg, bgum, true);
    CHECK(gum::loadGraphGUM< gum::EssentialGraph >(bgum, true).arcs() == eg.arcs());
  }

  GUM_TEST(NamesOfTheModelReplaceThoseOfThePDAG) {
    const auto bn = gum::BayesNet< double >::fastPrototype("A->B->C");
    const auto ia = bn.idFromName("A");
    const auto ib = bn.idFromName("B");
    const auto ic = bn.idFromName("C");
    // a PDAG whose names are permuted
    gum::PDAG pdag;
    for (const auto node: bn.nodes())
      pdag.addNodeWithId(node);
    for (const auto& arc: bn.arcs())
      pdag.addArc(arc.tail(), arc.head());
    pdag.setName(ia, "B");
    pdag.setName(ib, "C");
    pdag.setName(ic, "A");
    const gum::EssentialGraph eg(bn, pdag);
    CHECK_EQ(eg.nameFromId(ia), "A");
    CHECK_EQ(eg.nameFromId(ib), "B");
    CHECK_EQ(eg.nameFromId(ic), "C");
    CHECK(eg.arcs() == pdag.arcs());

    // without names, the labels and tooltips of the PDAG are kept
    gum::PDAG unnamed;
    unnamed.addNodes(2);
    unnamed.addArc(0, 1);
    const auto dot = gum::EssentialGraph(unnamed).toDot();
    CHECK_EQ(dot.find("label"), std::string::npos);
    CHECK_EQ(dot.find("tooltip"), std::string::npos);
  }

}   // namespace gum_tests
