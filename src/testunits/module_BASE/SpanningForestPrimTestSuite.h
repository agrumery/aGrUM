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


#include <cstdlib>
#include <iostream>
#include <string>

#include <agrum/base/graphs/algorithms/spanningForestPrim.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  SpanningForesPrim
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct SpanningForesPrimTestSuite {
    public:
    static void test1() {
      gum::UndiGraph g;

      gum::NodeId id1  = g.addNode();
      gum::NodeId id2  = g.addNode();
      gum::NodeId id3  = g.addNode();
      gum::NodeId id4  = g.addNode();
      gum::NodeId id5  = g.addNode();
      gum::NodeId id6  = g.addNode();
      gum::NodeId id7  = g.addNode();
      gum::NodeId id8  = g.addNode();
      gum::NodeId id9  = g.addNode();
      gum::NodeId id10 = g.addNode();
      gum::NodeId id11 = g.addNode();
      g.addNode();

      g.addEdge(id1, id2);
      g.addEdge(id1, id3);
      g.addEdge(id1, id6);
      g.addEdge(id2, id4);
      g.addEdge(id2, id5);
      g.addEdge(id3, id4);
      g.addEdge(id5, id6);

      g.addEdge(id7, id8);
      g.addEdge(id8, id9);
      g.addEdge(id9, id10);
      g.addEdge(id10, id11);
      g.addEdge(id8, id10);

      gum::EdgeProperty< float > cost;
      cost.insert(gum::Edge(id1, id2), 1);
      cost.insert(gum::Edge(id1, id3), 10);
      cost.insert(gum::Edge(id1, id6), 2);
      cost.insert(gum::Edge(id2, id4), 3);
      cost.insert(gum::Edge(id2, id5), 3);
      cost.insert(gum::Edge(id3, id4), 5);
      cost.insert(gum::Edge(id5, id6), 2);

      cost.insert(gum::Edge(id7, id8), 10);
      cost.insert(gum::Edge(id8, id9), 5);
      cost.insert(gum::Edge(id9, id10), 2);
      cost.insert(gum::Edge(id10, id11), 25);
      cost.insert(gum::Edge(id8, id10), 3);

      gum::SpanningForestPrim prim(&g, &cost);

      const gum::UndiGraph& g2 = prim.spanningForest();
      CHECK_EQ(g2.sizeNodes(), 12U);
      CHECK_EQ(g2.sizeEdges(), 9U);
      CHECK_EQ(g2.existsEdge(id1, id2), true);
      CHECK_EQ(g2.existsEdge(id1, id6), true);
      CHECK_EQ(g2.existsEdge(id2, id4), true);
      CHECK_EQ(g2.existsEdge(id3, id4), true);
      CHECK_EQ(g2.existsEdge(id5, id6), true);
      CHECK_EQ(g2.existsEdge(id7, id8), true);
      CHECK_EQ(g2.existsEdge(id8, id10), true);
      CHECK_EQ(g2.existsEdge(id9, id10), true);
      CHECK_EQ(g2.existsEdge(id10, id11), true);

      gum::SpanningForestPrim prim2(&g, &cost);
      CHECK_EQ(prim2.costOfSpanningForest(), 53);
      const gum::EdgeSet& edges2 = prim2.edgesInSpanningForest();
      CHECK_EQ(edges2.size(), 9U);

      gum::SpanningForestPrim prim3(&g, &cost);
      const gum::EdgeSet&     edges3 = prim3.edgesInSpanningForest();
      CHECK_EQ(edges3.size(), 9U);
    }   // namespace gum_tests

    static void testMissingEdgeCostThrows() {
      // HIGH-20: edge with no cost entry must throw NotFound
      gum::UndiGraph g;
      gum::NodeId    n0 = g.addNode();
      gum::NodeId    n1 = g.addNode();
      g.addEdge(n0, n1);

      gum::EdgeProperty< float > cost;   // no entry for (n0,n1)

      gum::SpanningForestPrim prim(&g, &cost);
      CHECK_THROWS_AS(prim.spanningForest(), const gum::NotFound&);
    }

    static void testIdempotentResults() {
      // HIGH-11: repeated calls on the same object must return consistent results
      gum::UndiGraph g;
      gum::NodeId    n0 = g.addNode();
      gum::NodeId    n1 = g.addNode();
      gum::NodeId    n2 = g.addNode();
      g.addEdge(n0, n1);
      g.addEdge(n1, n2);
      g.addEdge(n0, n2);

      gum::EdgeProperty< float > cost;
      cost.insert(gum::Edge(n0, n1), 1.0f);
      cost.insert(gum::Edge(n1, n2), 2.0f);
      cost.insert(gum::Edge(n0, n2), 4.0f);

      gum::SpanningForestPrim prim(&g, &cost);
      float c = prim.costOfSpanningForest();   // triggers computation
      CHECK_EQ(prim.edgesInSpanningForest().size(), 2U);
      CHECK_EQ(prim.costOfSpanningForest(), c);
      CHECK_EQ(prim.spanningForest().sizeEdges(), 2U);
    }
  };

  GUM_TEST_ACTIF(1)
  GUM_TEST_ACTIF(MissingEdgeCostThrows)
  GUM_TEST_ACTIF(IdempotentResults)
}   // namespace gum_tests
