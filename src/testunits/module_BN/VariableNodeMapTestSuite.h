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
#include <agrum/base/graphs/graphElements.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  VariableNodeMap
#define GUM_CURRENT_MODULE BN

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  struct VariableNodeMapTestSuite {
    public:
    gum::LabelizedVariable *var1, *var2, *var3, *var4, *var5;

    VariableNodeMapTestSuite() {
      var1 = new gum::LabelizedVariable("var1", "1");
      var2 = new gum::LabelizedVariable("var2", "2");
      var3 = new gum::LabelizedVariable("var3", "3");
      var4 = new gum::LabelizedVariable("var4", "4");
      var5 = new gum::LabelizedVariable("var5", "5");
    }

    ~VariableNodeMapTestSuite() {
      delete var1;
      delete var2;
      delete var3;
      delete var4;
      delete var5;
    }

    static void testConstructor() {
      gum::BayesNet< double >* topology = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(topology = new gum::BayesNet< double >());

      GUM_CHECK_ASSERT_THROWS_NOTHING(if (topology) delete topology);
    }

    void testInsertion() const {
      gum::BayesNet< double >  topo;
      gum::List< gum::NodeId > idList;

      GUM_CHECK_ASSERT_THROWS_NOTHING(idList.insert(topo.add(*var1)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(idList.insert(topo.add(*var2)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(idList.insert(topo.add(*var3)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(idList.insert(topo.add(*var4)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(idList.insert(topo.add(*var5)));

      CHECK((topo.size()) == (static_cast< gum::Size >(5)));
      CHECK((topo.dag().size()) == (static_cast< gum::Size >(5)));

      gum::NodeId ind = 0;
      GUM_CHECK_ASSERT_THROWS_NOTHING(const gum::DiscreteVariable& varPtr
                                      = topo.variable(idList[0]);
                                      ind = topo.nodeId(varPtr););
      CHECK((idList[0]) == (ind));

      CHECK((*((gum::LabelizedVariable*)&topo.variable(idList[0]))) == (*var1));
      CHECK((*((gum::LabelizedVariable*)&topo.variable(idList[1]))) == (*var2));
      CHECK((*((gum::LabelizedVariable*)&topo.variable(idList[2]))) == (*var3));
      CHECK((*((gum::LabelizedVariable*)&topo.variable(idList[3]))) == (*var4));
      CHECK((*((gum::LabelizedVariable*)&topo.variable(idList[4]))) == (*var5));
    }

    void testArcInsertion() const {
      gum::BayesNet< double >  topo;
      gum::List< gum::NodeId > idList;

      idList.insert(topo.add(*var1));
      idList.insert(topo.add(*var2));
      idList.insert(topo.add(*var3));
      idList.insert(topo.add(*var4));
      idList.insert(topo.add(*var5));

      GUM_CHECK_ASSERT_THROWS_NOTHING(topo.addArc(idList[0], idList[2]));
      GUM_CHECK_ASSERT_THROWS_NOTHING(topo.addArc(idList[2], idList[4]));
      GUM_CHECK_ASSERT_THROWS_NOTHING(topo.addArc(idList[1], idList[3]));
      GUM_CHECK_ASSERT_THROWS_NOTHING(topo.addArc(idList[0], idList[3]));
      GUM_CHECK_ASSERT_THROWS_NOTHING(topo.addArc(idList[3], idList[4]));
      GUM_CHECK_ASSERT_THROWS_NOTHING(topo.addArc(idList[1], idList[4]));

      CHECK((topo.dag().sizeArcs()) == (static_cast< gum::Size >(6)));
    }

    void testEraseVar() const {
      gum::BayesNet< double >  topo;
      gum::List< gum::NodeId > idList;

      CHECK(topo.empty());
      CHECK(topo.dag().empty());
      CHECK(topo.dag().emptyArcs());

      _fill_(topo, idList);

      CHECK(!topo.empty());
      CHECK(!topo.dag().empty());
      CHECK(!topo.dag().emptyArcs());

      CHECK((topo.size()) == (static_cast< gum::Size >(5)));
      CHECK((topo.dag().size()) == (static_cast< gum::Size >(5)));
      CHECK((topo.dag().sizeArcs()) == (static_cast< gum::Size >(6)));

      for (const auto i: idList)
        topo.erase(i);

      CHECK(topo.empty());

      CHECK(topo.dag().empty());
      CHECK(topo.dag().emptyArcs());

      CHECK((topo.size()) == (static_cast< gum::Size >(0)));
      CHECK((topo.dag().size()) == (static_cast< gum::Size >(0)));
      CHECK((topo.dag().sizeArcs()) == (static_cast< gum::Size >(0)));
    }

    void testEraseArc() const {
      gum::BayesNet< double >  topo;
      gum::List< gum::NodeId > idList;

      CHECK(topo.empty());
      CHECK(topo.dag().empty());
      CHECK(topo.dag().emptyArcs());

      _fill_(topo, idList);

      CHECK(!topo.empty());
      CHECK(!topo.dag().empty());
      CHECK(!topo.dag().emptyArcs());

      CHECK((topo.size()) == (static_cast< gum::Size >(5)));
      CHECK((topo.dag().size()) == (static_cast< gum::Size >(5)));
      CHECK((topo.dag().sizeArcs()) == (static_cast< gum::Size >(6)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(topo.eraseArc(gum::Arc(idList[0], idList[2])));
      GUM_CHECK_ASSERT_THROWS_NOTHING(topo.eraseArc(gum::Arc(idList[2], idList[4])));
      GUM_CHECK_ASSERT_THROWS_NOTHING(topo.eraseArc(gum::Arc(idList[1], idList[3])));
      GUM_CHECK_ASSERT_THROWS_NOTHING(topo.eraseArc(gum::Arc(idList[0], idList[3])));
      GUM_CHECK_ASSERT_THROWS_NOTHING(topo.eraseArc(gum::Arc(idList[3], idList[4])));
      GUM_CHECK_ASSERT_THROWS_NOTHING(topo.eraseArc(gum::Arc(idList[1], idList[4])));

      CHECK(!topo.empty());
      CHECK(!topo.dag().empty());
      CHECK(topo.dag().emptyArcs());
    }

    static void testIterator() {
      gum::BayesNet< double >  topo;
      gum::List< gum::NodeId > idList;

      for (const auto node: topo.nodes())
        CHECK(idList.exists(node));
    }

    void testMoralGraph() const {
      gum::BayesNet< double >  topo;
      gum::List< gum::NodeId > idList;

      _fill_(topo, idList);

      gum::UndiGraph graph;
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph = topo.moralGraph());
      CHECK((graph) == (_getRealMoralGraph_(topo, idList)));
    }

    void testTopologicalOrder() const {
      gum::BayesNet< double >  topo;
      gum::List< gum::NodeId > idList;

      _fill_(topo, idList);

      gum::Sequence< gum::NodeId > topoOrder;
      // GUM_CHECK_ASSERT_THROWS_NOTHING( topoOrder = topo.topologicalOrder() );

      try {
        topoOrder = topo.topologicalOrder();
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }

      CHECK((topoOrder.size()) == (static_cast< gum::Size >(5)));
    }

    private:
    void _fill_(gum::BayesNet< double >& topo, gum::List< gum::NodeId >& idList) const {
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
    }

    static gum::UndiGraph _getRealMoralGraph_(const gum::BayesNet< double >& topo,
                                              gum::List< gum::NodeId >&      idList) {
      gum::UndiGraph graph;

      graph.populateNodes(topo.dag());

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

  GUM_TEST_ACTIF(Constructor)
  GUM_TEST_ACTIF(Insertion)
  GUM_TEST_ACTIF(ArcInsertion)
  GUM_TEST_ACTIF(EraseVar)
  GUM_TEST_ACTIF(EraseArc)
  GUM_TEST_ACTIF(Iterator)
  GUM_TEST_ACTIF(MoralGraph)
  GUM_TEST_ACTIF(TopologicalOrder)
}   // namespace gum_tests
