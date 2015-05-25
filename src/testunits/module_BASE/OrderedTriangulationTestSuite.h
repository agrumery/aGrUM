/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <iostream>
#include <string>

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/graphs/triangulations/orderedTriangulation.h>

namespace gum_tests {

  class OrderedTriangulationTestSuite : public CxxTest::TestSuite {
    public:
    void testTriangulatedGraph1() {
      gum::NodeSet c1, c2, c3, c4, c5;
      c1 << 10 << 20 << 30 << 40;
      c2 << 20 << 40 << 50;
      c3 << 40 << 50 << 60;
      c4 << 20 << 50 << 70;
      c5 << 10 << 30 << 80;

      gum::UndiGraph graph;

      for (unsigned int i = 1; i <= 8; ++i)
        graph.addNode(10 * i);

      createClique(graph, c1);

      createClique(graph, c2);

      createClique(graph, c3);

      createClique(graph, c4);

      createClique(graph, c5);

      gum::NodeProperty<gum::Size> dom;

      for (unsigned int i = 1; i <= 8; ++i)
        dom.insert(10 * i, 10);

      std::vector<unsigned int> sequence(8);

      for (unsigned int i = 0; i < 8; ++i)
        sequence[i] = (8 - i) * 10;

      gum::OrderedTriangulation triang;

      triang.setGraph(&graph, &dom, &sequence);

      const gum::UndiGraph &gr2 = triang.triangulatedGraph();

      TS_ASSERT_EQUALS(gr2.sizeNodes(), 8U);

      TS_ASSERT_EQUALS(gr2.sizeEdges(), 14U);

      triang.clear();

      triang.setGraph(&graph, &dom, &sequence);

      const gum::UndiGraph &gr3 = triang.triangulatedGraph();

      TS_ASSERT_EQUALS(gr3.sizeNodes(), 8U);

      TS_ASSERT_EQUALS(gr3.sizeEdges(), 14U);

      triang.clear();

      for (unsigned int i = 0; i < 8; ++i)
        sequence[i] = (i + 1) * 10;

      triang.setGraph(&graph, &dom, &sequence);

      const gum::UndiGraph &gr5 = triang.triangulatedGraph();

      TS_ASSERT_EQUALS(gr5.sizeNodes(), 8U);

      TS_ASSERT_EQUALS(gr5.sizeEdges(), 23U);

      const gum::EdgeSet &edges2 = triang.fillIns();

      TS_ASSERT_EQUALS(edges2.size(), 9U);

      TS_ASSERT_EQUALS(edges2.exists(gum::Edge(20, 80)), true);

      TS_ASSERT_EQUALS(edges2.exists(gum::Edge(40, 80)), true);

      TS_ASSERT_EQUALS(edges2.exists(gum::Edge(30, 50)), true);

      TS_ASSERT_EQUALS(edges2.exists(gum::Edge(30, 70)), true);

      TS_ASSERT_EQUALS(edges2.exists(gum::Edge(40, 70)), true);

      TS_ASSERT_EQUALS(edges2.exists(gum::Edge(50, 80)), true);

      TS_ASSERT_EQUALS(edges2.exists(gum::Edge(70, 80)), true);

      TS_ASSERT_EQUALS(edges2.exists(gum::Edge(60, 70)), true);

      TS_ASSERT_EQUALS(edges2.exists(gum::Edge(60, 80)), true);

      const gum::CliqueGraph &JT = triang.junctionTree();

      TS_ASSERT_EQUALS(JT.sizeNodes(), 3U);

      TS_ASSERT_EQUALS(JT.sizeEdges(), 2U);

      const gum::NodeSet &clique1 = JT.clique(0);

      const gum::NodeSet &clique2 = JT.clique(1);

      const gum::NodeSet &clique3 = JT.clique(3);

      gum::NodeSet cc1, cc2, cc3;

      cc1 << 10 << 20 << 30 << 40 << 80;

      cc2 << 20 << 30 << 40 << 50 << 70 << 80;

      cc3 << 40 << 50 << 60 << 70 << 80;

      TS_ASSERT_EQUALS(clique1, cc1);

      TS_ASSERT_EQUALS(clique2, cc2);

      TS_ASSERT_EQUALS(clique3, cc3);

      for (unsigned int i = 0; i < 8; ++i)
        sequence[i] = (8 - i) * 10;

      graph.eraseEdge(gum::Edge(20, 50));

      triang.setGraph(&graph, &dom, &sequence);

      const gum::UndiGraph &gr4 = triang.triangulatedGraph();

      TS_ASSERT_EQUALS(gr4.sizeNodes(), 8U);

      TS_ASSERT_EQUALS(gr4.sizeEdges(), 14U);

      const gum::EdgeSet &edges = triang.fillIns();

      TS_ASSERT_EQUALS(edges.size(), 1U);

      TS_ASSERT_EQUALS(*(edges.begin()), gum::Edge(20, 50));
    }

    void testTriangulatedGraph2() {
      gum::UndiGraph graph;

      for (unsigned int i = 1; i <= 8; ++i)
        graph.addNode(i);

      for (unsigned int i = 1; i <= 7; ++i)
        graph.addEdge(i, i + 1);

      graph.addEdge(8, 1);

      gum::NodeProperty<gum::Size> dom;

      for (unsigned int i = 1; i <= 8; ++i)
        dom.insert(i, 10);

      std::vector<unsigned int> sequence(8);

      for (unsigned int i = 0; i < 8; ++i)
        sequence[i] = 8 - i;

      gum::OrderedTriangulation triang;

      triang.setGraph(&graph, &dom, &sequence);

      const gum::CliqueGraph &elim = triang.eliminationTree();

      TS_ASSERT_EQUALS(elim.sizeNodes(), 8U);

      TS_ASSERT_EQUALS(elim.sizeEdges(), 7U);

      const gum::CliqueGraph &JT = triang.junctionTree();

      TS_ASSERT_EQUALS(JT.sizeNodes(), 6U);

      TS_ASSERT_EQUALS(JT.sizeEdges(), 5U);

      const gum::UndiGraph &gr = triang.triangulatedGraph();

      TS_ASSERT_EQUALS(gr.sizeNodes(), 8U);

      TS_ASSERT_EQUALS(gr.sizeEdges(), 13U);
    }

    private:
    void createClique(gum::UndiGraph &graph, const gum::NodeSet &clique) {
      for (auto iter = clique.begin(); iter != clique.end(); ++iter) {
        auto iter2 = iter;

        for (++iter2; iter2 != clique.end(); ++iter2) {
          if (!graph.existsEdge(*iter, *iter2))
            graph.addEdge(*iter, *iter2);
        }
      }
    }
  };
}
