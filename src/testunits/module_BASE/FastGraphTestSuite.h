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

#include <agrum/base/graphs/DAG.h>
#include <agrum/base/graphs/PDAG.h>
#include <agrum/base/graphs/fastGraph.h>

#include <testunits/gumtest/AgrumTestSuite.h>

namespace gum_tests {

  struct FastGraphTestSuite {};

  GUM_TEST(DirectedChain) {
    auto g = gum::fastGraph< gum::DiGraph >("A->B->C; B->E");
    CHECK_EQ(g.size(), static_cast< gum::Size >(4));
    CHECK_EQ(g.sizeArcs(), static_cast< gum::Size >(3));
    CHECK(g.idFromName("A").has_value());
    CHECK(g.idFromName("E").has_value());
  }

  GUM_TEST(ReverseArc) {
    // "<-" reverses direction: A->B<-C means arcs A->B and C->B
    auto g = gum::fastGraph< gum::DiGraph >("A->B<-C");
    CHECK_EQ(g.size(), static_cast< gum::Size >(3));
    CHECK_EQ(g.sizeArcs(), static_cast< gum::Size >(2));
    CHECK(g.existsArc(*g.idFromName("A"), *g.idFromName("B")));
    CHECK(g.existsArc(*g.idFromName("C"), *g.idFromName("B")));
  }

  GUM_TEST(ReverseArcIntoUndiGraphThrows) {
    CHECK_THROWS_AS(gum::fastGraph< gum::UndiGraph >("A<-B"), const gum::InvalidArc&);
  }

  GUM_TEST(UndirectedChain) {
    auto g = gum::fastGraph< gum::UndiGraph >("A-B-C");
    CHECK_EQ(g.size(), static_cast< gum::Size >(3));
    CHECK_EQ(g.sizeEdges(), static_cast< gum::Size >(2));
  }

  GUM_TEST(MixedChain) {
    auto g = gum::fastGraph< gum::MixedGraph >("A->B-C");
    CHECK_EQ(g.sizeArcs(), static_cast< gum::Size >(1));
    CHECK_EQ(g.sizeEdges(), static_cast< gum::Size >(1));

    // MixedGraph is the default template argument
    auto g2 = gum::fastGraph("A->B-C");
    CHECK_EQ(g2.sizeArcs(), static_cast< gum::Size >(1));
    CHECK_EQ(g2.sizeEdges(), static_cast< gum::Size >(1));
  }

  GUM_TEST(NodeIdMode) {
    auto g = gum::fastGraph< gum::DiGraph >("1->2->100");
    CHECK(g.existsNode(gum::NodeId(1)));
    CHECK(g.existsNode(gum::NodeId(2)));
    CHECK(g.existsNode(gum::NodeId(100)));
    CHECK_FALSE(g.hasName(1));
  }

  GUM_TEST(NameMode) {
    auto       g   = gum::fastGraph< gum::DiGraph >("A->B->C");
    const auto idA = g.idFromName("A");
    const auto idB = g.idFromName("B");
    CHECK(idA.has_value());
    CHECK(idB.has_value());
    CHECK(g.existsArc(*idA, *idB));
  }

  GUM_TEST(MixedIntAndStringTokensForceNameMode) {
    auto g = gum::fastGraph< gum::DiGraph >("1->B->3");
    CHECK(g.idFromName("1").has_value());
    CHECK(g.idFromName("B").has_value());
    CHECK(g.idFromName("3").has_value());
  }

  GUM_TEST(ArcIntoUndiGraphThrows) {
    CHECK_THROWS_AS(gum::fastGraph< gum::UndiGraph >("A->B"), const gum::InvalidArc&);
  }

  GUM_TEST(EdgeIntoDiGraphThrows) {
    CHECK_THROWS_AS(gum::fastGraph< gum::DiGraph >("A-B"), const gum::InvalidEdge&);
  }

  GUM_TEST(DoubleDashIsMalformed) {
    // "--" is deliberately NOT a valid edge token: MRF's fast syntax already
    // uses "--" to list the variables of a single factor (a clique), a
    // different construct from a chain of pairwise edges. Reusing it here
    // would silently clash, so "A--B" must fail rather than be misread.
    CHECK_THROWS_AS(gum::fastGraph< gum::MixedGraph >("A--B"), const gum::InvalidArc&);
  }

  GUM_TEST(CycleIntoDAGThrows) {
    CHECK_THROWS_AS(gum::fastGraph< gum::DAG >("A->B->C->A"), const gum::InvalidDirectedCycle&);
  }

  GUM_TEST(PDAGChain) {
    auto g = gum::fastGraph< gum::PDAG >("A->B-C");
    CHECK_EQ(g.sizeArcs(), static_cast< gum::Size >(1));
    CHECK_EQ(g.sizeEdges(), static_cast< gum::Size >(1));
  }

  GUM_TEST(CycleIntoPDAGThrows) {
    CHECK_THROWS_AS(gum::fastGraph< gum::PDAG >("A->B->C->A"), const gum::InvalidDirectedCycle&);
  }

  GUM_TEST(WhitespaceIsIgnored) {
    auto g1 = gum::fastGraph< gum::MixedGraph >("A -> B ; B - C");
    auto g2 = gum::fastGraph< gum::MixedGraph >("A->B;B-C");
    CHECK_EQ(g1.size(), g2.size());
    CHECK_EQ(g1.sizeArcs(), g2.sizeArcs());
    CHECK_EQ(g1.sizeEdges(), g2.sizeEdges());
  }

  GUM_TEST(DegenerateDescriptions) {
    auto empty = gum::fastGraph< gum::MixedGraph >("");
    CHECK_EQ(empty.size(), static_cast< gum::Size >(0));

    auto semisOnly = gum::fastGraph< gum::MixedGraph >(";;");
    CHECK_EQ(semisOnly.size(), static_cast< gum::Size >(0));

    auto isolated = gum::fastGraph< gum::MixedGraph >("A");
    CHECK_EQ(isolated.size(), static_cast< gum::Size >(1));
    CHECK_EQ(isolated.sizeArcs(), static_cast< gum::Size >(0));
    CHECK_EQ(isolated.sizeEdges(), static_cast< gum::Size >(0));
  }

  GUM_TEST(TrailingOperatorThrows) {
    CHECK_THROWS_AS(gum::fastGraph< gum::MixedGraph >("A->"), const gum::InvalidArc&);
  }

}   // namespace gum_tests
