/***************************************************************************
 *   (C) 2007-2013 by Christophe GONZALES and Pierre-Henri WUILLEMIN       *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,        *
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

#include <agrum/BN/BayesNet.h>
#include <agrum/graphs/graphElements.h>
#include <agrum/variables/labelizedVariable.h>
#include <agrum/graphs/triangulations/defaultTriangulation.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  class VariableNodeMapTestSuite : public CxxTest::TestSuite {
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

    void testConstructor() {
      gum::BayesNet<float> *topology = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(topology = new gum::BayesNet<float>());

      TS_GUM_ASSERT_THROWS_NOTHING(if (topology) delete topology);
    }

    void testInsertion() {
      gum::BayesNet<float> topo;
      gum::List<gum::Id> idList;

      TS_GUM_ASSERT_THROWS_NOTHING(idList.insert(topo.add(*var1)));
      TS_GUM_ASSERT_THROWS_NOTHING(idList.insert(topo.add(*var2)));
      TS_GUM_ASSERT_THROWS_NOTHING(idList.insert(topo.add(*var3)));
      TS_GUM_ASSERT_THROWS_NOTHING(idList.insert(topo.add(*var4)));
      TS_GUM_ASSERT_THROWS_NOTHING(idList.insert(topo.add(*var5)));

      TS_ASSERT_EQUALS(topo.size(), (gum::Size)5);
      TS_ASSERT_EQUALS(topo.dag().size(), (gum::Size)5);

      gum::Id ind = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(const gum::DiscreteVariable &varPtr =
                                       topo.variable(idList[0]);
                                   ind = topo.nodeId(varPtr););
      TS_ASSERT_EQUALS(idList[0], ind);

      TS_ASSERT_EQUALS(*((gum::LabelizedVariable *)&topo.variable(idList[0])),
                       *var1);
      TS_ASSERT_EQUALS(*((gum::LabelizedVariable *)&topo.variable(idList[1])),
                       *var2);
      TS_ASSERT_EQUALS(*((gum::LabelizedVariable *)&topo.variable(idList[2])),
                       *var3);
      TS_ASSERT_EQUALS(*((gum::LabelizedVariable *)&topo.variable(idList[3])),
                       *var4);
      TS_ASSERT_EQUALS(*((gum::LabelizedVariable *)&topo.variable(idList[4])),
                       *var5);
    }

    void testArcInsertion() {
      gum::BayesNet<float> topo;
      gum::List<gum::Id> idList;

      idList.insert(topo.add(*var1));
      idList.insert(topo.add(*var2));
      idList.insert(topo.add(*var3));
      idList.insert(topo.add(*var4));
      idList.insert(topo.add(*var5));

      TS_GUM_ASSERT_THROWS_NOTHING(topo.addArc(idList[0], idList[2]));
      TS_GUM_ASSERT_THROWS_NOTHING(topo.addArc(idList[2], idList[4]));
      TS_GUM_ASSERT_THROWS_NOTHING(topo.addArc(idList[1], idList[3]));
      TS_GUM_ASSERT_THROWS_NOTHING(topo.addArc(idList[0], idList[3]));
      TS_GUM_ASSERT_THROWS_NOTHING(topo.addArc(idList[3], idList[4]));
      TS_GUM_ASSERT_THROWS_NOTHING(topo.addArc(idList[1], idList[4]));

      TS_ASSERT_EQUALS(topo.dag().sizeArcs(), (gum::Size)6);
    }

    void testEraseVar() {
      gum::BayesNet<float> topo;
      gum::List<gum::Id> idList;

      TS_ASSERT(topo.empty());
      TS_ASSERT(topo.dag().empty());
      TS_ASSERT(topo.dag().emptyArcs());

      fill(topo, idList);

      TS_ASSERT(!topo.empty());
      TS_ASSERT(!topo.dag().empty());
      TS_ASSERT(!topo.dag().emptyArcs());

      TS_ASSERT_EQUALS(topo.size(), (gum::Size)5);
      TS_ASSERT_EQUALS(topo.dag().size(), (gum::Size)5);
      TS_ASSERT_EQUALS(topo.dag().sizeArcs(), (gum::Size)6);

      for (const auto i : idList)
        topo.erase(i);

      TS_ASSERT(topo.empty());

      TS_ASSERT(topo.dag().empty());
      TS_ASSERT(topo.dag().emptyArcs());

      TS_ASSERT_EQUALS(topo.size(), (gum::Size)0);
      TS_ASSERT_EQUALS(topo.dag().size(), (gum::Size)0);
      TS_ASSERT_EQUALS(topo.dag().sizeArcs(), (gum::Size)0);
    }

    void testEraseArc() {
      gum::BayesNet<float> topo;
      gum::List<gum::Id> idList;

      TS_ASSERT(topo.empty());
      TS_ASSERT(topo.dag().empty());
      TS_ASSERT(topo.dag().emptyArcs());

      fill(topo, idList);

      TS_ASSERT(!topo.empty());
      TS_ASSERT(!topo.dag().empty());
      TS_ASSERT(!topo.dag().emptyArcs());

      TS_ASSERT_EQUALS(topo.size(), (gum::Size)5);
      TS_ASSERT_EQUALS(topo.dag().size(), (gum::Size)5);
      TS_ASSERT_EQUALS(topo.dag().sizeArcs(), (gum::Size)6);

      TS_GUM_ASSERT_THROWS_NOTHING(topo.eraseArc(gum::Arc(idList[0], idList[2])));
      TS_GUM_ASSERT_THROWS_NOTHING(topo.eraseArc(gum::Arc(idList[2], idList[4])));
      TS_GUM_ASSERT_THROWS_NOTHING(topo.eraseArc(gum::Arc(idList[1], idList[3])));
      TS_GUM_ASSERT_THROWS_NOTHING(topo.eraseArc(gum::Arc(idList[0], idList[3])));
      TS_GUM_ASSERT_THROWS_NOTHING(topo.eraseArc(gum::Arc(idList[3], idList[4])));
      TS_GUM_ASSERT_THROWS_NOTHING(topo.eraseArc(gum::Arc(idList[1], idList[4])));

      TS_ASSERT(!topo.empty());
      TS_ASSERT(!topo.dag().empty());
      TS_ASSERT(topo.dag().emptyArcs());
    }

    void testIterator() {
      gum::BayesNet<float> topo;
      gum::List<gum::Id> idList;

      for (const auto node : topo.nodes())
        TS_ASSERT(idList.exists(node));
    }

    void testMoralGraph() {
      gum::BayesNet<float> topo;
      gum::List<gum::Id> idList;

      fill(topo, idList);

      gum::UndiGraph graph;
      TS_GUM_ASSERT_THROWS_NOTHING(graph = topo.moralGraph());
      TS_ASSERT(graph == getRealMoralGraph(topo, idList));
    }

    void testTopologicalOrder() {
      gum::BayesNet<float> topo;
      gum::List<gum::Id> idList;

      fill(topo, idList);

      gum::Sequence<gum::NodeId> topoOrder;
      // TS_GUM_ASSERT_THROWS_NOTHING( topoOrder = topo.topologicalOrder() );

      try {
        topoOrder = topo.topologicalOrder();
      } catch (gum::Exception &e) {
        GUM_SHOWERROR(e);
      }

      TS_ASSERT_EQUALS(topoOrder.size(), (gum::Size)5);
    }

    private:
    void fill(gum::BayesNet<float> &topo, gum::List<gum::Id> &idList) {
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

    gum::UndiGraph getRealMoralGraph(const gum::BayesNet<float> &topo,
                                     gum::List<gum::Id> &idList) {
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
}
