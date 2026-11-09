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

#include <agrum/base/graphs/algorithms/generic/chordality.h>
#include <agrum/base/graphs/algorithms/triangulations/defaultTriangulation.h>
#include <agrum/base/graphs/mixedGraph.h>
#include <agrum/base/graphs/undiGraph.h>

#include <agrum/base/core/utils_random.h>
#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {

  struct ChordalityTestSuite {
    public:
    static gum::UndiGraph cycle(gum::Size n) {
      gum::UndiGraph g;
      g.addNodes(n);
      for (gum::NodeId i = 0; i < n; ++i)
        g.addEdge(i, (i + 1) % n);
      return g;
    }

    static gum::UndiGraph randomGraph(gum::Size n, double density) {
      gum::UndiGraph g;
      g.addNodes(n);
      for (gum::NodeId i = 0; i < n; ++i)
        for (gum::NodeId j = i + 1; j < n; ++j)
          if (gum::randomProba() < density) g.addEdge(i, j);
      return g;
    }
  };

  GUM_TEST(SearchesVisitEveryNode) {
    auto g = randomGraph(30, 0.15);
    g.addNode();   // an isolated node
    const auto lex = gum::graph::lexBFS(g);
    const auto mcs = gum::graph::maximumCardinalitySearch(g);
    CHECK_EQ(lex.size(), g.size());
    CHECK_EQ(mcs.size(), g.size());
    for (const auto node: g.nodes()) {
      CHECK(lex.exists(node));
      CHECK(mcs.exists(node));
    }
  }

  GUM_TEST(LexBFSOrder) {
    // path 0-1-2-3 with chord 0-2: from 0, LexBFS visits 1 and 2 (labelled by 0), then the one
    // adjacent to the most recently visited ones
    gum::UndiGraph g;
    g.addNodes(4);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    g.addEdge(0, 2);
    const auto order = gum::graph::lexBFS(g);
    CHECK_EQ(order[0], gum::NodeId(0));
    // 3 is visited last: its label (the visit step of 2) is smaller than the others'
    CHECK_EQ(order[3], gum::NodeId(3));
  }

  GUM_TEST(LexBFSFirstClique) {
    CHECK_EQ(gum::graph::lexBFS(gum::UndiGraph()).size(), gum::Size(0));

    // a chain of triangles: {2, 3, 4} is visited first, in the given order
    gum::UndiGraph g;
    g.addNodes(6);
    for (gum::NodeId i = 0; i < 6; ++i) {
      if (i + 1 < 6) g.addEdge(i, i + 1);
      if (i + 2 < 6) g.addEdge(i, i + 2);
    }
    const std::vector< gum::NodeId > clique{4, 2, 3};
    const auto                       order = gum::graph::lexBFS(g, clique);
    CHECK_EQ(order.size(), g.size());
    for (gum::Idx i = 0; i < clique.size(); ++i)
      CHECK_EQ(order[i], clique[i]);

    gum::Sequence< gum::NodeId > elimination;
    for (gum::Idx i = order.size(); i > 0; --i)
      elimination.insert(order[i - 1]);
    CHECK(gum::graph::isPerfectEliminationOrdering(g, elimination));

    // only a clique of nodes of the graph can be visited first
    CHECK_THROWS_AS(gum::graph::lexBFS(g, {0, 3}), const gum::InvalidArgument&);
    CHECK_THROWS_AS(gum::graph::lexBFS(g, {0, 0}), const gum::InvalidArgument&);
    CHECK_THROWS_AS(gum::graph::lexBFS(g, {42}), const gum::InvalidArgument&);
    CHECK_EQ(gum::graph::lexBFS(g, {0, 1, 2}).size(), g.size());
  }

  GUM_TEST(ChordalGraphs) {
    CHECK(gum::graph::isChordal(gum::UndiGraph()));
    CHECK(gum::graph::isChordal(cycle(3)));
    CHECK(!gum::graph::isChordal(cycle(4)));
    CHECK(!gum::graph::isChordal(cycle(5)));
    auto c4 = cycle(4);
    c4.addEdge(0, 2);   // a chord
    CHECK(gum::graph::isChordal(c4));
    auto c6 = cycle(6);
    c6.addEdge(0, 3);   // two 4-cycles
    CHECK(!gum::graph::isChordal(c6));

    // a tree is chordal
    gum::UndiGraph tree;
    tree.addNodes(7);
    for (gum::NodeId i = 1; i < 7; ++i)
      tree.addEdge((i - 1) / 2, i);
    CHECK(gum::graph::isChordal(tree));

    // the arcs of a mixed graph are ignored
    gum::MixedGraph mg;
    mg.addNodes(4);
    mg.addEdge(0, 1);
    mg.addEdge(1, 2);
    mg.addEdge(2, 3);
    mg.addArc(3, 0);
    CHECK(gum::graph::isChordal(mg));
    mg.addEdge(0, 3);
    mg.eraseArc(gum::Arc(3, 0));
    CHECK(!gum::graph::isChordal(mg));
  }

  GUM_TEST(PerfectEliminationOrdering) {
    auto c4 = cycle(4);
    c4.addEdge(0, 2);
    gum::Sequence< gum::NodeId > good;
    for (const auto n: {1, 3, 0, 2})
      good.insert(gum::NodeId(n));
    CHECK(gum::graph::isPerfectEliminationOrdering(c4, good));
    gum::Sequence< gum::NodeId > bad;
    for (const auto n: {0, 1, 2, 3})
      bad.insert(gum::NodeId(n));   // 0 first: its neighbours 1 and 3 are not adjacent
    CHECK(!gum::graph::isPerfectEliminationOrdering(c4, bad));
    gum::Sequence< gum::NodeId > incomplete;
    incomplete.insert(0);
    CHECK_THROWS_AS(gum::graph::isPerfectEliminationOrdering(c4, incomplete),
                    const gum::InvalidArgument&);
  }

  GUM_TEST(TriangulatedGraphsAreChordal) {
    for (int i = 0; i < 50; ++i) {
      const auto                     g       = randomGraph(25, 0.12);
      gum::NodeProperty< gum::Size > domains = g.nodesPropertyFromVal(gum::Size(2));
      gum::DefaultTriangulation      triangulation(&g, &domains);
      const auto&                    triangulated = triangulation.triangulatedGraph();
      CHECK(gum::graph::isChordal(triangulated));

      // the reverse of both searches of a chordal graph are perfect elimination orderings
      for (const auto& visit:
           {gum::graph::lexBFS(triangulated), gum::graph::maximumCardinalitySearch(triangulated)}) {
        gum::Sequence< gum::NodeId > elimination;
        for (gum::Idx k = visit.size(); k > 0; --k)
          elimination.insert(visit[k - 1]);
        CHECK(gum::graph::isPerfectEliminationOrdering(triangulated, elimination));
      }

      // a graph that is not chordal needs fill-ins to be triangulated
      if (!gum::graph::isChordal(g)) CHECK(triangulated.sizeEdges() > g.sizeEdges());
    }
  }

}   // namespace gum_tests
