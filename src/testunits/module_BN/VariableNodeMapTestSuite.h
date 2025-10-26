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
#include <agrum/base/graphs/graphElements.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  class GUM_TEST_SUITE(VariableNodeMap) {
    public:
    gum::LabelizedVariable *var1, *var2, *var3, *var4, *var5;

    void setUp() {
      var1 = new gum::LabelizedVariable("var1", "1");
      var2 = new gum::LabelizedVariable("var2", "2");
      var3 = new gum::LabelizedVariable("var3", "3");
      var4 = new gum::LabelizedVariable("var4", "4");
      var5 = new gum::LabelizedVariable("var5", "5");
    }

    void tearDown() {
      delete var1;
      delete var2;
      delete var3;
      delete var4;
      delete var5;
    }

    GUM_ACTIVE_TEST(Constructor) {
      gum::BayesNet< double >* topology = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(topology = new gum::BayesNet< double >())

      TS_GUM_ASSERT_THROWS_NOTHING(if (topology) delete topology)
    }

    GUM_ACTIVE_TEST(Insertion) {
      gum::BayesNet< double >  topo;
      gum::List< gum::NodeId > idList;

      TS_GUM_ASSERT_THROWS_NOTHING(idList.insert(topo.add(*var1)))
      TS_GUM_ASSERT_THROWS_NOTHING(idList.insert(topo.add(*var2)))
      TS_GUM_ASSERT_THROWS_NOTHING(idList.insert(topo.add(*var3)))
      TS_GUM_ASSERT_THROWS_NOTHING(idList.insert(topo.add(*var4)))
      TS_GUM_ASSERT_THROWS_NOTHING(idList.insert(topo.add(*var5)))

      TS_ASSERT_EQUALS(topo.size(), static_cast< gum::Size >(5))
      TS_ASSERT_EQUALS(topo.dag().size(), static_cast< gum::Size >(5))

      gum::NodeId ind = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(const gum::DiscreteVariable& varPtr = topo.variable(idList[0]);
                                   ind                                 = topo.nodeId(varPtr);)
      TS_ASSERT_EQUALS(idList[0], ind)

      TS_ASSERT_EQUALS(*((gum::LabelizedVariable*)&topo.variable(idList[0])), *var1)
      TS_ASSERT_EQUALS(*((gum::LabelizedVariable*)&topo.variable(idList[1])), *var2)
      TS_ASSERT_EQUALS(*((gum::LabelizedVariable*)&topo.variable(idList[2])), *var3)
      TS_ASSERT_EQUALS(*((gum::LabelizedVariable*)&topo.variable(idList[3])), *var4)
      TS_ASSERT_EQUALS(*((gum::LabelizedVariable*)&topo.variable(idList[4])), *var5)
    }

    GUM_ACTIVE_TEST(ArcInsertion) {
      gum::BayesNet< double >  topo;
      gum::List< gum::NodeId > idList;

      idList.insert(topo.add(*var1));
      idList.insert(topo.add(*var2));
      idList.insert(topo.add(*var3));
      idList.insert(topo.add(*var4));
      idList.insert(topo.add(*var5));

      TS_GUM_ASSERT_THROWS_NOTHING(topo.addArc(idList[0], idList[2]))
      TS_GUM_ASSERT_THROWS_NOTHING(topo.addArc(idList[2], idList[4]))
      TS_GUM_ASSERT_THROWS_NOTHING(topo.addArc(idList[1], idList[3]))
      TS_GUM_ASSERT_THROWS_NOTHING(topo.addArc(idList[0], idList[3]))
      TS_GUM_ASSERT_THROWS_NOTHING(topo.addArc(idList[3], idList[4]))
      TS_GUM_ASSERT_THROWS_NOTHING(topo.addArc(idList[1], idList[4]))

      TS_ASSERT_EQUALS(topo.dag().sizeArcs(), static_cast< gum::Size >(6))
    }

    GUM_ACTIVE_TEST(EraseVar) {
      gum::BayesNet< double >  topo;
      gum::List< gum::NodeId > idList;

      TS_ASSERT(topo.empty())
      TS_ASSERT(topo.dag().empty())
      TS_ASSERT(topo.dag().emptyArcs())

      fill(topo, idList);

      TS_ASSERT(!topo.empty())
      TS_ASSERT(!topo.dag().empty())
      TS_ASSERT(!topo.dag().emptyArcs())

      TS_ASSERT_EQUALS(topo.size(), static_cast< gum::Size >(5))
      TS_ASSERT_EQUALS(topo.dag().size(), static_cast< gum::Size >(5))
      TS_ASSERT_EQUALS(topo.dag().sizeArcs(), static_cast< gum::Size >(6))

      for (const auto i: idList)
        topo.erase(i);

      TS_ASSERT(topo.empty())

      TS_ASSERT(topo.dag().empty())
      TS_ASSERT(topo.dag().emptyArcs())

      TS_ASSERT_EQUALS(topo.size(), static_cast< gum::Size >(0))
      TS_ASSERT_EQUALS(topo.dag().size(), static_cast< gum::Size >(0))
      TS_ASSERT_EQUALS(topo.dag().sizeArcs(), static_cast< gum::Size >(0))
    }

    GUM_ACTIVE_TEST(EraseArc) {
      gum::BayesNet< double >  topo;
      gum::List< gum::NodeId > idList;

      TS_ASSERT(topo.empty())
      TS_ASSERT(topo.dag().empty())
      TS_ASSERT(topo.dag().emptyArcs())

      fill(topo, idList);

      TS_ASSERT(!topo.empty())
      TS_ASSERT(!topo.dag().empty())
      TS_ASSERT(!topo.dag().emptyArcs())

      TS_ASSERT_EQUALS(topo.size(), static_cast< gum::Size >(5))
      TS_ASSERT_EQUALS(topo.dag().size(), static_cast< gum::Size >(5))
      TS_ASSERT_EQUALS(topo.dag().sizeArcs(), static_cast< gum::Size >(6))

      TS_GUM_ASSERT_THROWS_NOTHING(topo.eraseArc(gum::Arc(idList[0], idList[2])))
      TS_GUM_ASSERT_THROWS_NOTHING(topo.eraseArc(gum::Arc(idList[2], idList[4])))
      TS_GUM_ASSERT_THROWS_NOTHING(topo.eraseArc(gum::Arc(idList[1], idList[3])))
      TS_GUM_ASSERT_THROWS_NOTHING(topo.eraseArc(gum::Arc(idList[0], idList[3])))
      TS_GUM_ASSERT_THROWS_NOTHING(topo.eraseArc(gum::Arc(idList[3], idList[4])))
      TS_GUM_ASSERT_THROWS_NOTHING(topo.eraseArc(gum::Arc(idList[1], idList[4])))

      TS_ASSERT(!topo.empty())
      TS_ASSERT(!topo.dag().empty())
      TS_ASSERT(topo.dag().emptyArcs())
    }

    GUM_ACTIVE_TEST(Iterator) {
      gum::BayesNet< double >  topo;
      gum::List< gum::NodeId > idList;

      for (const auto node: topo.nodes())
        TS_ASSERT(idList.exists(node))
    }

    GUM_ACTIVE_TEST(MoralGraph) {
      gum::BayesNet< double >  topo;
      gum::List< gum::NodeId > idList;

      fill(topo, idList);

      gum::UndiGraph graph;
      TS_GUM_ASSERT_THROWS_NOTHING(graph = topo.moralGraph())
      TS_ASSERT_EQUALS(graph, getRealMoralGraph(topo, idList))
    }

    GUM_ACTIVE_TEST(TopologicalOrder) {
      gum::BayesNet< double >  topo;
      gum::List< gum::NodeId > idList;

      fill(topo, idList);

      gum::Sequence< gum::NodeId > topoOrder;
      // TS_GUM_ASSERT_THROWS_NOTHING( topoOrder = topo.topologicalOrder() )

      try {
        topoOrder = topo.topologicalOrder();
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }

      TS_ASSERT_EQUALS(topoOrder.size(), static_cast< gum::Size >(5))
    }

    private:
    void fill(gum::BayesNet< double >& topo, gum::List< gum::NodeId >& idList) {
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

    gum::UndiGraph getRealMoralGraph(const gum::BayesNet< double >& topo,
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
}   // namespace gum_tests
