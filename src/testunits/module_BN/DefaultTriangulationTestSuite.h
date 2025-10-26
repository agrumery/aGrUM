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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
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

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/graphs/algorithms/triangulations/defaultTriangulation.h>
#include <agrum/base/graphs/undiGraph.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/io/BIF/BIFReader.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  class GUM_TEST_SUITE(DefaultTriangulation) {
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

    GUM_ACTIVE_TEST(triangulatedGraph) {
      gum::BayesNet< double >  topo;
      gum::List< gum::NodeId > idList;

      fill(topo, idList);
      const gum::UndiGraph&          undiGraph = topo.moralGraph();
      gum::NodeProperty< gum::Size > modalities;

      // Builds a hashTable where the keys are the id of the variable,
      // and the values the variable's domain size.

      for (const auto node: topo.nodes())
        modalities.insert(node, topo.variable(node).domainSize());

      gum::DefaultTriangulation* triangle = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(triangle
                                   = new gum::DefaultTriangulation(&undiGraph, &modalities));

      TS_GUM_ASSERT_THROWS_NOTHING(triangle->triangulatedGraph())

      TS_ASSERT_EQUALS(triangle->maxLog10CliqueDomainSize(),
                       log10(16));   // clique of 4 binary variables

      TS_GUM_ASSERT_THROWS_NOTHING(if (triangle) delete triangle)
    };

    GUM_ACTIVE_TEST(junctionTree) {
      gum::BayesNet< double >  topo;
      gum::List< gum::NodeId > idList;

      fill(topo, idList);

      gum::UndiGraph                 undiGraph;
      gum::NodeProperty< gum::Size > modalities;

      TS_GUM_ASSERT_THROWS_NOTHING(undiGraph = topo.moralGraph())

      // Builds a hashTable where the keys are the id of the variable,
      // and the values the variable's domain size.
      for (const auto node: topo.nodes())
        modalities.insert(node, topo.variable(node).domainSize());

      gum::DefaultTriangulation* triangle = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(triangle
                                   = new gum::DefaultTriangulation(&undiGraph, &modalities));
      TS_GUM_ASSERT_THROWS_NOTHING(triangle->junctionTree())

      // TODO : problem here !!
      TS_ASSERT(triangle->junctionTree().hasRunningIntersection())

      TS_GUM_ASSERT_THROWS_NOTHING(if (triangle) delete triangle)
    };

    void xxtestBIFtriangulation1() {
      TS_GUM_ASSERT_THROWS_NOTHING(__triangulate_bif(GET_RESSOURCES_PATH("bif/Barley.bif")))
    };

    void xxtestBIFtriangulation1bis() {
      TS_GUM_ASSERT_THROWS_NOTHING(__triangulate_bif(GET_RESSOURCES_PATH("bif/alarm.bif")))
    };

    void xxtestBIFtriangulation2() {
      TS_GUM_ASSERT_THROWS_NOTHING(__triangulate_bif(GET_RESSOURCES_PATH("bif/carpo.bif")))
    };

    void xxtestBIFtriangulation3() {
      TS_GUM_ASSERT_THROWS_NOTHING(__triangulate_bif(GET_RESSOURCES_PATH("bif/Pigs.bif")))
    };

    void xxtestBIFtriangulation4() {
      TS_GUM_ASSERT_THROWS_NOTHING(__triangulate_bif(GET_RESSOURCES_PATH("bif/Water.bif")))
    };

    void xxtestBIFtriangulation5() {
      TS_GUM_ASSERT_THROWS_NOTHING(__triangulate_bif(GET_RESSOURCES_PATH("bif/Link.bif")))
    };

    void xxtestBIFtriangulation6() {
      TS_GUM_ASSERT_THROWS_NOTHING(__triangulate_bif(GET_RESSOURCES_PATH("bif/Mildew.bif")))
    };

    void xxtestBIFtriangulation7() {
      TS_GUM_ASSERT_THROWS_NOTHING(__triangulate_bif(GET_RESSOURCES_PATH("bif/Munin1.bif")))
    };

    void xxtestBIFtriangulation8() {
      TS_GUM_ASSERT_THROWS_NOTHING(__triangulate_bif(GET_RESSOURCES_PATH("bif/hailfinder.bif")))
    };

    void xxtestBIFtriangulation9() {
      TS_GUM_ASSERT_THROWS_NOTHING(__triangulate_bif(GET_RESSOURCES_PATH("bif/Diabetes.bif")))
    };

    void xxtestBIFtriangulation10() {
      TS_GUM_ASSERT_THROWS_NOTHING(__triangulate_bif(GET_RESSOURCES_PATH("bif/insurance.bif")))
    };

    GUM_ACTIVE_TEST(TriangulatedGraph1) {
      gum::NodeSet c1, c2, c3, c4, c5;
      c1 << 1 << 2 << 3 << 4;
      c2 << 2 << 4 << 5;
      c3 << 4 << 5 << 6;
      c4 << 2 << 5 << 7;
      c5 << 1 << 3 << 8;

      gum::UndiGraph graph;

      for (unsigned int i = 1; i <= 8; ++i)
        graph.addNodeWithId(i);

      createClique(graph, c1);
      createClique(graph, c2);
      createClique(graph, c3);
      createClique(graph, c4);
      createClique(graph, c5);

      gum::NodeProperty< gum::Size > dom;

      for (unsigned int i = 1; i <= 8; ++i)
        dom.insert(i, static_cast< gum::Size >(10));

      gum::DefaultTriangulation triang;

      triang.setGraph(&graph, &dom);

      const gum::UndiGraph& gr2 = triang.triangulatedGraph();

      TS_ASSERT_EQUALS(gr2.sizeNodes(), 8U)

      TS_ASSERT_EQUALS(gr2.sizeEdges(), 14U)

      triang.clear();

      triang.setGraph(&graph, &dom);

      const gum::UndiGraph& gr3 = triang.triangulatedGraph();

      TS_ASSERT_EQUALS(gr3.sizeNodes(), 8U)

      TS_ASSERT_EQUALS(gr3.sizeEdges(), 14U)

      graph.eraseEdge(gum::Edge(2, 5));

      triang.setGraph(&graph, &dom);

      const gum::UndiGraph& gr4 = triang.triangulatedGraph();

      TS_ASSERT_EQUALS(gr4.sizeNodes(), 8U)

      TS_ASSERT_EQUALS(gr4.sizeEdges(), 14U)

      const gum::EdgeSet& edges = triang.fillIns();

      TS_ASSERT_EQUALS(edges.size(), 1U)

      TS_ASSERT_EQUALS(*(edges.begin()), gum::Edge(2, 5))
    }

    GUM_ACTIVE_TEST(TriangulatedGraph2) {
      gum::UndiGraph graph;

      for (unsigned int i = 1; i <= 8; ++i)
        graph.addNodeWithId(i);

      for (unsigned int i = 1; i <= 7; ++i)
        graph.addEdge(i, i + 1);

      graph.addEdge(8, 1);

      gum::NodeProperty< gum::Size > dom;

      for (unsigned int i = 1; i <= 8; ++i)
        dom.insert(i, 10);

      gum::DefaultTriangulation triang;

      triang.setGraph(&graph, &dom);

      const gum::CliqueGraph& elim = triang.eliminationTree();

      TS_ASSERT_EQUALS(elim.sizeNodes(), 8U)

      TS_ASSERT_EQUALS(elim.sizeEdges(), 7U)

      const gum::CliqueGraph& JT = triang.junctionTree();

      TS_ASSERT_EQUALS(JT.sizeNodes(), 6U)

      TS_ASSERT_EQUALS(JT.sizeEdges(), 5U)

      const gum::UndiGraph& gr = triang.triangulatedGraph();

      TS_ASSERT_EQUALS(gr.sizeNodes(), 8U)

      TS_ASSERT_EQUALS(gr.sizeEdges(), 13U)
    }

    private:
    void createClique(gum::UndiGraph& graph, const gum::NodeSet& clique) {
      for (auto iter = clique.begin(); iter != clique.end(); ++iter) {
        auto iter2 = iter;

        for (++iter2; iter2 != clique.end(); ++iter2) {
          if (!graph.existsEdge(*iter, *iter2)) graph.addEdge(*iter, *iter2);
        }
      }
    }

    void __triangulate_bif(std::string file) {
      gum::BayesNet< double >* bn = new gum::BayesNet< double >();
      gum::BIFReader< double > reader(bn, file);
      reader.trace(false);

      TS_GUM_ASSERT_THROWS_NOTHING(reader.proceed())

      gum::LazyPropagation< double > inf(bn);

      if (bn != 0) delete (bn);
    };

    void fill(gum::BayesNet< double >& topo, gum::List< gum::NodeId >& idList) {
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

    gum::UndiGraph getRealMoralGraph(const gum::List< gum::NodeId >& idList) {
      gum::UndiGraph graph;
      /*
        idList.clear();
        idList.insert(graph.addNode());
        idList.insert(graph.addNode());
        idList.insert(graph.addNode());
        idList.insert(graph.addNode());
        idList.insert(graph.addNodeWithId());*/

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
}   // namespace gum_tests
