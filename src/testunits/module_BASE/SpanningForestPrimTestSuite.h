/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/





#include <cstdlib>
#include <iostream>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/graphs/algorithms/spanningForestPrim.h>

namespace gum_tests {

  class [[maybe_unused]] SpanningForesPrimTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(1) {
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
      TS_ASSERT_EQUALS(g2.sizeNodes(), 12U)
      TS_ASSERT_EQUALS(g2.sizeEdges(), 9U)
      TS_ASSERT_EQUALS(g2.existsEdge(id1, id2), true)
      TS_ASSERT_EQUALS(g2.existsEdge(id1, id6), true)
      TS_ASSERT_EQUALS(g2.existsEdge(id2, id4), true)
      TS_ASSERT_EQUALS(g2.existsEdge(id3, id4), true)
      TS_ASSERT_EQUALS(g2.existsEdge(id5, id6), true)
      TS_ASSERT_EQUALS(g2.existsEdge(id7, id8), true)
      TS_ASSERT_EQUALS(g2.existsEdge(id8, id10), true)
      TS_ASSERT_EQUALS(g2.existsEdge(id9, id10), true)
      TS_ASSERT_EQUALS(g2.existsEdge(id10, id11), true)

      gum::SpanningForestPrim prim2(&g, &cost);
      TS_ASSERT_EQUALS(prim2.costOfSpanningForest(), 53)
      const gum::EdgeSet& edges2 = prim2.edgesInSpanningForest();
      TS_ASSERT_EQUALS(edges2.size(), 9U)

      gum::SpanningForestPrim prim3(&g, &cost);
      const gum::EdgeSet&     edges3 = prim3.edgesInSpanningForest();
      TS_ASSERT_EQUALS(edges3.size(), 9U)
    }
  };
}   // namespace gum_tests
