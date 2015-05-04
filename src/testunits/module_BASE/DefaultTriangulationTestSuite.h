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

#include <agrum/variables/labelizedVariable.h>
#include <agrum/graphs/triangulations/defaultTriangulation.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/BIF/BIFReader.h>
#include <agrum/graphs/undiGraph.h>
#include <agrum/BN/inference/lazyPropagation.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  class DefaultTriangulationTestSuite : public CxxTest::TestSuite {
    public:
    gum::LabelizedVariable *var1, *var2, *var3, *var4, *var5;

    void setUp() {
      var1 = new gum::LabelizedVariable("var1", "1");
      var2 = new gum::LabelizedVariable("var2", "2");
      var3 = new gum::LabelizedVariable("var3", "3");
      var4 = new gum::LabelizedVariable("var4", "4");
      var5 = new gum::LabelizedVariable("var5", "5");
    };

    void tearDown() {
      delete var1;
      delete var2;
      delete var3;
      delete var4;
      delete var5;
    };

    void testtriangulatedGraph() {
      gum::BayesNet<float> topo;
      gum::List<gum::NodeId> idList;

      fill(topo, idList);
      const gum::UndiGraph &undiGraph = topo.moralGraph();
      gum::NodeProperty<gum::Size> modalities;

      // Builds a hashTable where the keys are the id of the variable,
      // and the values the variable's domain size.

      for (const auto node : topo.nodes())
        modalities.insert(node, topo.variable(node).domainSize());

      gum::DefaultTriangulation *triangle = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(
          triangle = new gum::DefaultTriangulation(&undiGraph, &modalities));

      TS_GUM_ASSERT_THROWS_NOTHING(triangle->triangulatedGraph());

      TS_ASSERT_EQUALS(triangle->maxLog10CliqueDomainSize(),
                       log10(16)); // clique of 4 binary variables

      TS_GUM_ASSERT_THROWS_NOTHING(if (triangle) delete triangle);
    };

    void testjunctionTree() {
      gum::BayesNet<float> topo;
      gum::List<gum::NodeId> idList;

      fill(topo, idList);

      gum::UndiGraph undiGraph;
      gum::NodeProperty<gum::Size> modalities;

      TS_GUM_ASSERT_THROWS_NOTHING(undiGraph = topo.moralGraph());

      // Builds a hashTable where the keys are the id of the variable,
      // and the values the variable's domain size.
      for (const auto node : topo.nodes())
        modalities.insert(node, topo.variable(node).domainSize());

      gum::DefaultTriangulation *triangle = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(
          triangle = new gum::DefaultTriangulation(&undiGraph, &modalities));
      TS_GUM_ASSERT_THROWS_NOTHING(triangle->junctionTree());

      // TODO : problem here !!
      TS_ASSERT(triangle->junctionTree().hasRunningIntersection());

      TS_GUM_ASSERT_THROWS_NOTHING(if (triangle) delete triangle);
    };

    void xxtestBIFtriangulation1(void) {
      TS_GUM_ASSERT_THROWS_NOTHING(__triangulate_bif(GET_PATH_STR("Barley.bif")));
    };

    void xxtestBIFtriangulation1bis(void) {
      TS_GUM_ASSERT_THROWS_NOTHING(__triangulate_bif(GET_PATH_STR("alarm.bif")));
    };

    void xxtestBIFtriangulation2(void) {
      TS_GUM_ASSERT_THROWS_NOTHING(__triangulate_bif(GET_PATH_STR("carpo.bif")));
    };

    void xxtestBIFtriangulation3(void) {
      TS_GUM_ASSERT_THROWS_NOTHING(__triangulate_bif(GET_PATH_STR("Pigs.bif")));
    };

    void xxtestBIFtriangulation4(void) {
      TS_GUM_ASSERT_THROWS_NOTHING(__triangulate_bif(GET_PATH_STR("Water.bif")));
    };

    void xxtestBIFtriangulation5(void) {
      TS_GUM_ASSERT_THROWS_NOTHING(__triangulate_bif(GET_PATH_STR("Link.bif")));
    };

    void xxtestBIFtriangulation6(void) {
      TS_GUM_ASSERT_THROWS_NOTHING(__triangulate_bif(GET_PATH_STR("Mildew.bif")));
    };

    void xxtestBIFtriangulation7(void) {
      TS_GUM_ASSERT_THROWS_NOTHING(__triangulate_bif(GET_PATH_STR("Munin1.bif")));
    };

    void xxtestBIFtriangulation8(void) {
      TS_GUM_ASSERT_THROWS_NOTHING(
          __triangulate_bif(GET_PATH_STR("hailfinder.bif")));
    };

    void xxtestBIFtriangulation9(void) {
      TS_GUM_ASSERT_THROWS_NOTHING(__triangulate_bif(GET_PATH_STR("Diabetes.bif")));
    };

    void xxtestBIFtriangulation10(void) {
      TS_GUM_ASSERT_THROWS_NOTHING(__triangulate_bif(GET_PATH_STR("insurance.bif")));
    };

    void testTriangulatedGraph1() {
      gum::NodeSet c1, c2, c3, c4, c5;
      c1 << 1 << 2 << 3 << 4;
      c2 << 2 << 4 << 5;
      c3 << 4 << 5 << 6;
      c4 << 2 << 5 << 7;
      c5 << 1 << 3 << 8;

      gum::UndiGraph graph;

      for (unsigned int i = 1; i <= 8; ++i)
        graph.addNode(i);

      createClique(graph, c1);
      createClique(graph, c2);
      createClique(graph, c3);
      createClique(graph, c4);
      createClique(graph, c5);

      gum::NodeProperty<gum::Size> dom;

      for (unsigned int i = 1; i <= 8; ++i)
        dom.insert(i, gum::Size(10));

      gum::DefaultTriangulation triang;

      triang.setGraph(&graph, &dom);

      const gum::UndiGraph &gr2 = triang.triangulatedGraph();

      TS_ASSERT_EQUALS(gr2.sizeNodes(), 8U);

      TS_ASSERT_EQUALS(gr2.sizeEdges(), 14U);

      triang.clear();

      triang.setGraph(&graph, &dom);

      const gum::UndiGraph &gr3 = triang.triangulatedGraph();

      TS_ASSERT_EQUALS(gr3.sizeNodes(), 8U);

      TS_ASSERT_EQUALS(gr3.sizeEdges(), 14U);

      graph.eraseEdge(gum::Edge(2, 5));

      triang.setGraph(&graph, &dom);

      const gum::UndiGraph &gr4 = triang.triangulatedGraph();

      TS_ASSERT_EQUALS(gr4.sizeNodes(), 8U);

      TS_ASSERT_EQUALS(gr4.sizeEdges(), 14U);

      const gum::EdgeSet &edges = triang.fillIns();

      TS_ASSERT_EQUALS(edges.size(), 1U);

      TS_ASSERT_EQUALS(*(edges.begin()), gum::Edge(2, 5));
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

      gum::DefaultTriangulation triang;

      triang.setGraph(&graph, &dom);

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

    void __triangulate_bif(std::string file) {
      gum::BayesNet<float> *bn = new gum::BayesNet<float>();
      gum::BIFReader<float> reader(bn, file);
      reader.trace(false);

      TS_GUM_ASSERT_THROWS_NOTHING(reader.proceed());

      gum::LazyPropagation<float> inf(*bn);

      if (bn != 0)
        delete (bn);
    };

    void fill(gum::BayesNet<float> &topo, gum::List<gum::NodeId> &idList) {
      idList.clear();
      idList.insert(topo.add(*var1));
      idList.insert(topo.add(*var2));
      idList.insert(topo.add(*var3));
      idList.insert(topo.add(*var4));
      idList.insert(topo.add(*var5));

      topo.addArc(idList[0], idList[2]);
      topo.addArc(idList[2], idList[4]);
      topo.addArc(idList[1], idList[3]);
      topo.addArc(idList[0], idList[3]);
      topo.addArc(idList[3], idList[4]);
      topo.addArc(idList[1], idList[4]);
    };

    gum::UndiGraph getRealMoralGraph(const gum::List<gum::NodeId> &idList) {
      gum::UndiGraph graph;
      /*
        idList.clear();
        idList.insert(graph.addNode());
        idList.insert(graph.addNode());
        idList.insert(graph.addNode());
        idList.insert(graph.addNode());
        idList.insert(graph.addNode());*/

      graph.addEdge(idList[0], idList[2]);
      graph.addEdge(idList[0], idList[3]);
      graph.addEdge(idList[0], idList[1]);
      graph.addEdge(idList[2], idList[4]);
      graph.addEdge(idList[3], idList[4]);
      graph.addEdge(idList[2], idList[3]);
      graph.addEdge(idList[1], idList[3]);
      graph.addEdge(idList[1], idList[4]);
      graph.addEdge(idList[1], idList[2]);

      return graph;
    }
  };
}
