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

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/graphs/algorithms/triangulations/defaultTriangulation.h>
#include <agrum/base/graphs/undiGraph.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/io/BIF/BIFReader.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  DefaultTriangulation
#define GUM_CURRENT_MODULE BN

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  struct DefaultTriangulationTestSuite {
    public:
    gum::LabelizedVariable *var1, *var2, *var3, *var4, *var5;
    ;
    ;

    DefaultTriangulationTestSuite() {
      var1 = new gum::LabelizedVariable("var1", "1");

      var2 = new gum::LabelizedVariable("var2", "2");

      var3 = new gum::LabelizedVariable("var3", "3");

      var4 = new gum::LabelizedVariable("var4", "4");

      var5 = new gum::LabelizedVariable("var5", "5");
    }

    ~DefaultTriangulationTestSuite() {
      delete var1;

      delete var2;

      delete var3;

      delete var4;

      delete var5;
    }

    void testtriangulatedGraph() {
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

      GUM_CHECK_ASSERT_THROWS_NOTHING(triangle
                                      = new gum::DefaultTriangulation(&undiGraph, &modalities));

      GUM_CHECK_ASSERT_THROWS_NOTHING(triangle->triangulatedGraph());

      CHECK((triangle->maxLog10CliqueDomainSize())
            == (log10(16)));   // clique of 4 binary variables

      GUM_CHECK_ASSERT_THROWS_NOTHING(if (triangle) delete triangle);
    };

    void testjunctionTree() {
      gum::BayesNet< double >  topo;
      gum::List< gum::NodeId > idList;

      fill(topo, idList);

      gum::UndiGraph                 undiGraph;
      gum::NodeProperty< gum::Size > modalities;

      GUM_CHECK_ASSERT_THROWS_NOTHING(undiGraph = topo.moralGraph());

      // Builds a hashTable where the keys are the id of the variable,
      // and the values the variable's domain size.
      for (const auto node: topo.nodes())
        modalities.insert(node, topo.variable(node).domainSize());

      gum::DefaultTriangulation* triangle = nullptr;

      GUM_CHECK_ASSERT_THROWS_NOTHING(triangle
                                      = new gum::DefaultTriangulation(&undiGraph, &modalities));
      GUM_CHECK_ASSERT_THROWS_NOTHING(triangle->junctionTree());

      // TODO : problem here !!
      CHECK(triangle->junctionTree().hasRunningIntersection());

      GUM_CHECK_ASSERT_THROWS_NOTHING(if (triangle) delete triangle);
    };

    void xxtestBIFtriangulation1() {
      GUM_CHECK_ASSERT_THROWS_NOTHING(_triangulate_bif_(GET_RESSOURCES_PATH("bif/Barley.bif")));
    };

    void xxtestBIFtriangulation1bis() {
      GUM_CHECK_ASSERT_THROWS_NOTHING(_triangulate_bif_(GET_RESSOURCES_PATH("bif/alarm.bif")));
    };

    void xxtestBIFtriangulation2() {
      GUM_CHECK_ASSERT_THROWS_NOTHING(_triangulate_bif_(GET_RESSOURCES_PATH("bif/carpo.bif")));
    };

    void xxtestBIFtriangulation3() {
      GUM_CHECK_ASSERT_THROWS_NOTHING(_triangulate_bif_(GET_RESSOURCES_PATH("bif/Pigs.bif")));
    };

    void xxtestBIFtriangulation4() {
      GUM_CHECK_ASSERT_THROWS_NOTHING(_triangulate_bif_(GET_RESSOURCES_PATH("bif/Water.bif")));
    };

    void xxtestBIFtriangulation5() {
      GUM_CHECK_ASSERT_THROWS_NOTHING(_triangulate_bif_(GET_RESSOURCES_PATH("bif/Link.bif")));
    };

    void xxtestBIFtriangulation6() {
      GUM_CHECK_ASSERT_THROWS_NOTHING(_triangulate_bif_(GET_RESSOURCES_PATH("bif/Mildew.bif")));
    };

    void xxtestBIFtriangulation7() {
      GUM_CHECK_ASSERT_THROWS_NOTHING(_triangulate_bif_(GET_RESSOURCES_PATH("bif/Munin1.bif")));
    };

    void xxtestBIFtriangulation8() {
      GUM_CHECK_ASSERT_THROWS_NOTHING(_triangulate_bif_(GET_RESSOURCES_PATH("bif/hailfinder.bif")));
    };

    void xxtestBIFtriangulation9() {
      GUM_CHECK_ASSERT_THROWS_NOTHING(_triangulate_bif_(GET_RESSOURCES_PATH("bif/Diabetes.bif")));
    };

    void xxtestBIFtriangulation10() {
      GUM_CHECK_ASSERT_THROWS_NOTHING(_triangulate_bif_(GET_RESSOURCES_PATH("bif/insurance.bif")));
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
        graph.addNodeWithId(i);

      _createClique_(graph, c1);
      _createClique_(graph, c2);
      _createClique_(graph, c3);
      _createClique_(graph, c4);
      _createClique_(graph, c5);

      gum::NodeProperty< gum::Size > dom;

      for (unsigned int i = 1; i <= 8; ++i)
        dom.insert(i, static_cast< gum::Size >(10));

      gum::DefaultTriangulation triang;

      triang.setGraph(&graph, &dom);

      const gum::UndiGraph& gr2 = triang.triangulatedGraph();

      CHECK((gr2.sizeNodes()) == (8U));

      CHECK((gr2.sizeEdges()) == (14U));

      triang.clear();

      triang.setGraph(&graph, &dom);

      const gum::UndiGraph& gr3 = triang.triangulatedGraph();

      CHECK((gr3.sizeNodes()) == (8U));

      CHECK((gr3.sizeEdges()) == (14U));

      graph.eraseEdge(gum::Edge(2, 5));

      triang.setGraph(&graph, &dom);

      const gum::UndiGraph& gr4 = triang.triangulatedGraph();

      CHECK((gr4.sizeNodes()) == (8U));

      CHECK((gr4.sizeEdges()) == (14U));

      const gum::EdgeSet& edges = triang.fillIns();

      CHECK((edges.size()) == (1U));

      CHECK((*(edges.begin())) == (gum::Edge(2, 5)));
    }

    void testTriangulatedGraph2() {
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

      CHECK((elim.sizeNodes()) == (8U));

      CHECK((elim.sizeEdges()) == (7U));

      const gum::CliqueGraph& JT = triang.junctionTree();

      CHECK((JT.sizeNodes()) == (6U));

      CHECK((JT.sizeEdges()) == (5U));

      const gum::UndiGraph& gr = triang.triangulatedGraph();

      CHECK((gr.sizeNodes()) == (8U));

      CHECK((gr.sizeEdges()) == (13U));
    }

    private:
    static void _createClique_(gum::UndiGraph& graph, const gum::NodeSet& clique) {
      for (auto iter = clique.begin(); iter != clique.end(); ++iter) {
        auto iter2 = iter;

        for (++iter2; iter2 != clique.end(); ++iter2) {
          if (!graph.existsEdge(*iter, *iter2)) graph.addEdge(*iter, *iter2);
        }
      }
    }

    static void _triangulate_bif_(std::string file) {
      gum::BayesNet< double >* bn = new gum::BayesNet< double >();
      gum::BIFReader< double > reader(bn, file);
      reader.trace(false);

      GUM_CHECK_ASSERT_THROWS_NOTHING(reader.proceed());

      gum::LazyPropagation< double > inf(bn);

      if (bn != 0) delete (bn);
    };

    void fill(gum::BayesNet< double >& topo, gum::List< gum::NodeId >& idList) const {
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
  };

  GUM_TEST_ACTIF(triangulatedGraph)
  GUM_TEST_ACTIF(junctionTree)
  GUM_TEST_ACTIF(TriangulatedGraph1)
  GUM_TEST_ACTIF(TriangulatedGraph2)
}   // namespace gum_tests
