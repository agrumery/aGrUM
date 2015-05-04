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

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/graphs/graphElements.h>

#include <agrum/graphs/DAG.h>
#include <agrum/graphs/undiGraph.h>
#include <agrum/graphs/mixedGraph.h>

#include <agrum/graphs/listeners/diGraphListener.h>
#include <agrum/graphs/listeners/undiGraphListener.h>
#include <agrum/graphs/listeners/mixedGraphListener.h>

// The graph used for the tests:
//          0   1_          0 -> 2
//         / \ / /          0 -> 3
//        2   3 /           2 -> 4
//         \ / /            3 -> 4
//          4_/             1 -> 3
//                          1 -> 4

namespace gum_tests {

  class GraphListenerTestSuite : public CxxTest::TestSuite {

    class Counter : public gum::Listener {
      private:
      int __nbrNode, __nbrArcs, __nbrEdges;
      bool __isOn;

      public:
      Counter() {
        __nbrNode = __nbrArcs = __nbrEdges = 0;
        __isOn = false;
      }

      void whenNodeAdded(const void *, gum::NodeId) { __nbrNode++; }

      void whenNodeDeleted(const void *, gum::NodeId) { __nbrNode--; }

      void whenArcAdded(const void *, gum::NodeId, gum::NodeId) { __nbrArcs++; }

      void whenArcDeleted(const void *, gum::NodeId, gum::NodeId) { __nbrArcs--; }

      void whenEdgeAdded(const void *, gum::NodeId f, gum::NodeId s) {
        __nbrEdges++;

        if (__isOn)
          GUM_TRACE(f << "--" << s << "(" << __nbrEdges << ")");
      }

      void whenEdgeDeleted(const void *, gum::NodeId f, gum::NodeId s) {
        __nbrEdges--;

        if (__isOn)
          GUM_TRACE(f << "--" << s << "(" << __nbrEdges << ")");
      }

      const int nodes() const { return __nbrNode; }

      const int arcs() const { return __nbrArcs; }

      const int edges() const { return __nbrEdges; }

      void on() { __isOn = true; }

      void off() { __isOn = false; }
    };

    class DiGraphCounter : public gum::DiGraphListener {
      private:
      int __nbrNode, __nbrArcs;

      public:
      DiGraphCounter(gum::DiGraph *g) : gum::DiGraphListener(g) {
        __nbrNode = __nbrArcs = 0;
      }

      void whenNodeAdded(const void *, gum::NodeId) { __nbrNode++; }

      void whenNodeDeleted(const void *, gum::NodeId) { __nbrNode--; }

      void whenArcAdded(const void *, gum::NodeId, gum::NodeId) { __nbrArcs++; }

      void whenArcDeleted(const void *, gum::NodeId, gum::NodeId) { __nbrArcs--; }

      const int nodes() const { return __nbrNode; }

      const int arcs() const { return __nbrArcs; }
    };

    class UndiGraphCounter : public gum::UndiGraphListener {
      private:
      int __nbrNode, __nbrEdges;

      public:
      UndiGraphCounter(gum::UndiGraph *g) : gum::UndiGraphListener(g) {
        __nbrNode = __nbrEdges = 0;
      }

      void whenNodeAdded(const void *, gum::NodeId) { __nbrNode++; }

      void whenNodeDeleted(const void *, gum::NodeId) { __nbrNode--; }

      void whenEdgeAdded(const void *, gum::NodeId, gum::NodeId) { __nbrEdges++; }

      void whenEdgeDeleted(const void *, gum::NodeId, gum::NodeId) { __nbrEdges--; }

      const int nodes() const { return __nbrNode; }

      const int edges() const { return __nbrEdges; }
    };

    class MixedGraphCounter : public gum::MixedGraphListener {
      private:
      int __nbrNode, __nbrArcs, __nbrEdges;

      public:
      MixedGraphCounter(gum::MixedGraph *g) : gum::MixedGraphListener(g) {
        __nbrNode = __nbrArcs = __nbrEdges = 0;
      }

      void whenNodeAdded(const void *, gum::NodeId) { __nbrNode++; }

      void whenNodeDeleted(const void *, gum::NodeId) { __nbrNode--; }

      void whenArcAdded(const void *, gum::NodeId, gum::NodeId) { __nbrArcs++; }

      void whenArcDeleted(const void *, gum::NodeId, gum::NodeId) { __nbrArcs--; }

      void whenEdgeAdded(const void *, gum::NodeId, gum::NodeId) { __nbrEdges++; }

      void whenEdgeDeleted(const void *, gum::NodeId, gum::NodeId) { __nbrEdges--; }

      const int nodes() const { return __nbrNode; }

      const int arcs() const { return __nbrArcs; }

      const int edges() const { return __nbrEdges; }
    };

    private:
    gum::NodeId id1;
    gum::NodeId id2;
    gum::NodeId id3;
    gum::NodeId id4;
    gum::NodeId id5;

    void buildDAG(gum::DiGraph &g) {
      id1 = g.addNode();
      id2 = g.addNode();
      id3 = g.addNode();
      id4 = g.addNode();
      id5 = g.addNode();

      g.addArc(id1, id3);
      g.addArc(id3, id5);
      g.addArc(id2, id4);
      g.addArc(id1, id4);
      g.addArc(id4, id5);
      g.addArc(id2, id5);
    }

    void buildUndiGraph(gum::UndiGraph &g) {
      id3 = g.addNode();
      id4 = g.addNode();
      id5 = g.addNode();
      id1 = g.addNode();
      id2 = g.addNode();

      g.addEdge(id4, id5);
      g.addEdge(id2, id5);
      g.addEdge(id2, id4);
      g.addEdge(id1, id4);
      g.addEdge(id1, id3);
      g.addEdge(id3, id5);
    }

    void buildMixedGraph(gum::MixedGraph &g) {
      id3 = g.addNode();
      id4 = g.addNode();
      id5 = g.addNode();
      id1 = g.addNode();
      id2 = g.addNode();

      g.addEdge(id4, id5);
      g.addEdge(id3, id5);
      g.addArc(id2, id5);
      g.addArc(id2, id4);
      g.addEdge(id1, id4);
      g.addArc(id1, id3);
    }

    public:
    void testDAG() {
      gum::DAG g1;
      gum::DAG g2;

      Counter c1;
      Counter c2;

      // g1 has 2 listeners
      // g2 has 1 listener
      // c1 listens to 1 graph
      // c2 listens to 2 graphs
      GUM_CONNECT(g1, onNodeAdded, c1, Counter::whenNodeAdded);
      GUM_CONNECT(g1, onNodeDeleted, c1, Counter::whenNodeDeleted);
      GUM_CONNECT(g1, onArcAdded, c1, Counter::whenArcAdded);
      GUM_CONNECT(g1, onArcDeleted, c1, Counter::whenArcDeleted);

      GUM_CONNECT(g1, onNodeAdded, c2, Counter::whenNodeAdded);
      GUM_CONNECT(g1, onNodeDeleted, c2, Counter::whenNodeDeleted);
      GUM_CONNECT(g1, onArcAdded, c2, Counter::whenArcAdded);
      GUM_CONNECT(g1, onArcDeleted, c2, Counter::whenArcDeleted);

      GUM_CONNECT(g2, onNodeAdded, c2, Counter::whenNodeAdded);
      GUM_CONNECT(g2, onNodeDeleted, c2, Counter::whenNodeDeleted);
      GUM_CONNECT(g2, onArcAdded, c2, Counter::whenArcAdded);
      GUM_CONNECT(g2, onArcDeleted, c2, Counter::whenArcDeleted);

      buildDAG(g2); // 5 nodes/6 arcs for g2

      g1.addArc(g1.addNode(), g1.addNode());
      buildDAG(g1); // 7 nodes/7 arcs for g1

      TS_ASSERT_EQUALS(c1.nodes(), 7);
      TS_ASSERT_EQUALS(c1.arcs(), 7);
      TS_ASSERT_EQUALS(c2.nodes(), 7 + 5);
      TS_ASSERT_EQUALS(c2.arcs(), 7 + 6);

      g1.eraseNode(id5); // -1 nodes/-3 arcs for g1

      TS_ASSERT_EQUALS(c1.nodes(), 6);
      TS_ASSERT_EQUALS(c1.arcs(), 4);
      TS_ASSERT_EQUALS(c2.nodes(), 6 + 5);
      TS_ASSERT_EQUALS(c2.arcs(), 4 + 6);

      g1.eraseArc(gum::Arc(id1, id3)); // 6 nodes, 3 arcs

      TS_ASSERT_EQUALS(c1.nodes(), 6);
      TS_ASSERT_EQUALS(c1.arcs(), 3);
      TS_ASSERT_EQUALS(c2.nodes(), 6 + 5);
      TS_ASSERT_EQUALS(c2.arcs(), 3 + 6);

      g1.clear(); // 0 node, 0 arc

      TS_ASSERT_EQUALS(c1.nodes(), 0);
      TS_ASSERT_EQUALS(c1.arcs(), 0);
      TS_ASSERT_EQUALS(c2.nodes(), 0 + 5);
      TS_ASSERT_EQUALS(c2.arcs(), 0 + 6);

      g2.clearArcs(); // 5 nodes, 0 arc

      TS_ASSERT_EQUALS(c1.nodes(), 0);
      TS_ASSERT_EQUALS(c1.arcs(), 0);
      TS_ASSERT_EQUALS(c2.nodes(), 0 + 5);
      TS_ASSERT_EQUALS(c2.arcs(), 0 + 0);
    }

    void testUndiGraph() {
      gum::UndiGraph g1;
      gum::UndiGraph g2;

      Counter c1;
      Counter c2;

      // g1 has 2 listeners
      // g2 has 1 listener
      // c1 listens to 1 graph
      // c2 listens to 2 graphs
      GUM_CONNECT(g1, onNodeAdded, c1, Counter::whenNodeAdded);
      GUM_CONNECT(g1, onNodeDeleted, c1, Counter::whenNodeDeleted);
      GUM_CONNECT(g1, onEdgeAdded, c1, Counter::whenEdgeAdded);
      GUM_CONNECT(g1, onEdgeDeleted, c1, Counter::whenEdgeDeleted);

      GUM_CONNECT(g1, onNodeAdded, c2, Counter::whenNodeAdded);
      GUM_CONNECT(g1, onNodeDeleted, c2, Counter::whenNodeDeleted);
      GUM_CONNECT(g1, onEdgeAdded, c2, Counter::whenEdgeAdded);
      GUM_CONNECT(g1, onEdgeDeleted, c2, Counter::whenEdgeDeleted);

      GUM_CONNECT(g2, onNodeAdded, c2, Counter::whenNodeAdded);
      GUM_CONNECT(g2, onNodeDeleted, c2, Counter::whenNodeDeleted);
      GUM_CONNECT(g2, onEdgeAdded, c2, Counter::whenEdgeAdded);
      GUM_CONNECT(g2, onEdgeDeleted, c2, Counter::whenEdgeDeleted);

      buildUndiGraph(g2); // 5 nodes/6 edges for g2

      g1.addEdge(g1.addNode(), g1.addNode());
      buildUndiGraph(g1); // 7 nodes/7 edges for g1

      TS_ASSERT_EQUALS(c1.nodes(), 7);
      TS_ASSERT_EQUALS(c1.edges(), 7);
      TS_ASSERT_EQUALS(c2.nodes(), 7 + 5);
      TS_ASSERT_EQUALS(c2.edges(), 7 + 6);

      g1.eraseNode(id5); // -1 nodes/-3 edges for g1

      TS_ASSERT_EQUALS(c1.nodes(), 6);
      TS_ASSERT_EQUALS(c1.edges(), 4);
      TS_ASSERT_EQUALS(c2.nodes(), 6 + 5);
      TS_ASSERT_EQUALS(c2.edges(), 4 + 6);

      g1.eraseEdge(gum::Edge(id1, id3)); // 6 nodes, 3 edges

      TS_ASSERT_EQUALS(c1.nodes(), 6);
      TS_ASSERT_EQUALS(c1.edges(), 3);
      TS_ASSERT_EQUALS(c2.nodes(), 6 + 5);
      TS_ASSERT_EQUALS(c2.edges(), 3 + 6);

      g1.clear(); // 0 node, 0 arc

      TS_ASSERT_EQUALS(c1.nodes(), 0);
      TS_ASSERT_EQUALS(c1.edges(), 0);
      TS_ASSERT_EQUALS(c2.nodes(), 0 + 5);
      TS_ASSERT_EQUALS(c2.edges(), 0 + 6);

      g2.clearEdges(); // 5 nodes, 0 arc

      TS_ASSERT_EQUALS(c1.nodes(), 0);
      TS_ASSERT_EQUALS(c1.edges(), 0);
      TS_ASSERT_EQUALS(c2.nodes(), 0 + 5);
      TS_ASSERT_EQUALS(c2.edges(), 0 + 0);
    }

    void testMixedGraph() {
      gum::MixedGraph g1;
      gum::MixedGraph g2;

      Counter c1;
      Counter c2;

      // g1 has 2 listeners
      // g2 has 1 listener
      // c1 listens to 1 graph
      // c2 listens to 2 graphs

      TS_ASSERT(!g1.onNodeAdded.hasListener());
      GUM_CONNECT(g1, onNodeAdded, c1, Counter::whenNodeAdded);
      TS_ASSERT(g1.onNodeAdded.hasListener());

      GUM_CONNECT(g1, onNodeDeleted, c1, Counter::whenNodeDeleted);
      TS_ASSERT(g1.onNodeDeleted.hasListener());

      TS_ASSERT(!g1.onEdgeAdded.hasListener());
      GUM_CONNECT(g1, onEdgeAdded, c1, Counter::whenEdgeAdded);
      TS_ASSERT(g1.onEdgeAdded.hasListener());

      TS_ASSERT(!g1.onEdgeDeleted.hasListener());
      GUM_CONNECT(g1, onEdgeDeleted, c1, Counter::whenEdgeDeleted);
      TS_ASSERT(g1.onEdgeDeleted.hasListener());

      TS_ASSERT(!g1.onArcAdded.hasListener());
      GUM_CONNECT(g1, onArcAdded, c1, Counter::whenArcAdded);
      TS_ASSERT(g1.onArcAdded.hasListener());

      TS_ASSERT(!g1.onArcDeleted.hasListener());
      GUM_CONNECT(g1, onArcDeleted, c1, Counter::whenArcDeleted);
      TS_ASSERT(g1.onArcDeleted.hasListener());

      GUM_CONNECT(g1, onNodeAdded, c2, Counter::whenNodeAdded);
      GUM_CONNECT(g1, onNodeDeleted, c2, Counter::whenNodeDeleted);
      GUM_CONNECT(g1, onEdgeAdded, c2, Counter::whenEdgeAdded);
      GUM_CONNECT(g1, onEdgeDeleted, c2, Counter::whenEdgeDeleted);
      GUM_CONNECT(g1, onArcAdded, c2, Counter::whenArcAdded);
      GUM_CONNECT(g1, onArcDeleted, c2, Counter::whenArcDeleted);

      GUM_CONNECT(g2, onNodeAdded, c2, Counter::whenNodeAdded);
      GUM_CONNECT(g2, onNodeDeleted, c2, Counter::whenNodeDeleted);
      GUM_CONNECT(g2, onEdgeAdded, c2, Counter::whenEdgeAdded);
      GUM_CONNECT(g2, onEdgeDeleted, c2, Counter::whenEdgeDeleted);
      GUM_CONNECT(g2, onArcAdded, c2, Counter::whenArcAdded);
      GUM_CONNECT(g2, onArcDeleted, c2, Counter::whenArcDeleted);

      buildMixedGraph(g2); // 5 nodes/3 edges /3 arcs for g2

      g1.addEdge(g1.addNode(), g1.addNode());
      buildMixedGraph(g1); // 7 nodes/4 edges / 3 arcs for g1

      TS_ASSERT_EQUALS(c1.nodes(), 7);
      TS_ASSERT_EQUALS(c1.edges(), 4);
      TS_ASSERT_EQUALS(c1.arcs(), 3);
      TS_ASSERT_EQUALS(c2.nodes(), 7 + 5);
      TS_ASSERT_EQUALS(c2.edges(), 4 + 3);
      TS_ASSERT_EQUALS(c2.arcs(), 3 + 3);

      g1.eraseNode(id5); // -1 nodes/-2 edges / -1 arcs for g1

      TS_ASSERT_EQUALS(c1.nodes(), 6);
      TS_ASSERT_EQUALS(c1.edges(), 2);
      TS_ASSERT_EQUALS(c1.arcs(), 2);
      TS_ASSERT_EQUALS(c2.nodes(), 6 + 5);
      TS_ASSERT_EQUALS(c2.edges(), 2 + 3);
      TS_ASSERT_EQUALS(c2.arcs(), 2 + 3);

      g1.eraseEdge(
          gum::Edge(id1, id3)); // THIS EDGE DOES NOT EXISTS !!!! => 6 nodes, 2 edges

      TS_ASSERT_EQUALS(c1.nodes(), 6);
      TS_ASSERT_EQUALS(c1.edges(), 2);
      TS_ASSERT_EQUALS(c2.nodes(), 6 + 5);
      TS_ASSERT_EQUALS(c2.edges(), 2 + 3);

      g1.clear(); // 0 node, 0 arc

      TS_ASSERT_EQUALS(c1.nodes(), 0);
      TS_ASSERT_EQUALS(c1.edges(), 0);
      TS_ASSERT_EQUALS(c1.arcs(), 0);
      TS_ASSERT_EQUALS(c2.nodes(), 0 + 5);
      TS_ASSERT_EQUALS(c2.edges(), 0 + 3);
      TS_ASSERT_EQUALS(c2.arcs(), 0 + 3);

      g2.clearEdges(); // 5 nodes, 0 arc

      TS_ASSERT_EQUALS(c1.nodes(), 0);
      TS_ASSERT_EQUALS(c1.edges(), 0);
      TS_ASSERT_EQUALS(c1.arcs(), 0);
      TS_ASSERT_EQUALS(c2.nodes(), 0 + 5);
      TS_ASSERT_EQUALS(c2.edges(), 0 + 0);
      TS_ASSERT_EQUALS(c2.arcs(), 0 + 3);

      g2.clearArcs(); // 5 nodes, 0 arc

      TS_ASSERT_EQUALS(c1.nodes(), 0);
      TS_ASSERT_EQUALS(c1.edges(), 0);
      TS_ASSERT_EQUALS(c1.arcs(), 0);
      TS_ASSERT_EQUALS(c2.nodes(), 0 + 5);
      TS_ASSERT_EQUALS(c2.edges(), 0 + 0);
      TS_ASSERT_EQUALS(c2.arcs(), 0 + 0);
    }

    void testUndiGraphWithGraphListener() {
      gum::UndiGraph g;

      UndiGraphCounter c(&g);

      buildUndiGraph(g); // 5 nodes/6 edges for g

      TS_ASSERT_EQUALS(c.nodes(), 5);
      TS_ASSERT_EQUALS(c.edges(), 6);

      g.eraseNode(id5); // -1 nodes/-3 edges for g

      TS_ASSERT_EQUALS(c.nodes(), 4);
      TS_ASSERT_EQUALS(c.edges(), 3);

      g.eraseEdge(gum::Edge(id1, id3)); // -1 edges

      TS_ASSERT_EQUALS(c.nodes(), 4);
      TS_ASSERT_EQUALS(c.edges(), 2);

      g.clear(); // 0 node, 0 arc

      TS_ASSERT_EQUALS(c.nodes(), 0);
      TS_ASSERT_EQUALS(c.edges(), 0);
    }

    void testDiGraphWithGraphListener() {
      gum::DiGraph g;

      DiGraphCounter c(&g);

      buildDAG(g); // 5 nodes/6 arcs for g

      TS_ASSERT_EQUALS(c.nodes(), 5);
      TS_ASSERT_EQUALS(c.arcs(), 6);

      g.eraseNode(id5); // -1 nodes/-3 arcs for g

      TS_ASSERT_EQUALS(c.nodes(), 4);
      TS_ASSERT_EQUALS(c.arcs(), 3);

      g.eraseArc(gum::Arc(id1, id3)); // -1 arcs

      TS_ASSERT_EQUALS(c.nodes(), 4);
      TS_ASSERT_EQUALS(c.arcs(), 2);

      g.clear(); // 0 node, 0 arc

      TS_ASSERT_EQUALS(c.nodes(), 0);
      TS_ASSERT_EQUALS(c.arcs(), 0);
    }

    void testDAGWithGraphListener() {
      gum::DAG g;

      DiGraphCounter c(&g);

      buildDAG(g); // 5 nodes/6 arcs for g

      TS_ASSERT_THROWS(g.addArc(id5, id2),
                       gum::InvalidDirectedCycle); // should throw
                                                   // InvalidDirectedCycle and should
                                                   // not call the listeners

      TS_ASSERT_EQUALS(c.nodes(), 5);
      TS_ASSERT_EQUALS(c.arcs(), 6);

      g.eraseNode(id5); // -1 nodes/-3 arcs for g

      TS_ASSERT_EQUALS(c.nodes(), 4);
      TS_ASSERT_EQUALS(c.arcs(), 3);

      g.eraseArc(gum::Arc(id1, id3)); // -1 arcs

      TS_ASSERT_EQUALS(c.nodes(), 4);
      TS_ASSERT_EQUALS(c.arcs(), 2);

      g.clear(); // 0 node, 0 arc

      TS_ASSERT_EQUALS(c.nodes(), 0);
      TS_ASSERT_EQUALS(c.arcs(), 0);
    }

    void testMixedGraphWithGraphListener() {

      gum::MixedGraph g;

      MixedGraphCounter c(&g);

      buildMixedGraph(g); // 5 nodes/3 arcs/3 edges for g

      TS_ASSERT_EQUALS(c.nodes(), 5);
      TS_ASSERT_EQUALS(c.edges(), 3);
      TS_ASSERT_EQUALS(c.arcs(), 3);

      g.eraseNode(id5); // -1 nodes/-2 edge / -1 arcs for g

      TS_ASSERT_EQUALS(c.nodes(), 4);
      TS_ASSERT_EQUALS(c.edges(), 1);
      TS_ASSERT_EQUALS(c.arcs(), 2);

      g.eraseArc(gum::Arc(id1, id3)); // -1 arcs

      TS_ASSERT_EQUALS(c.nodes(), 4);
      TS_ASSERT_EQUALS(c.edges(), 1);
      TS_ASSERT_EQUALS(c.arcs(), 1);

      g.clear(); // 0 node, 0 arc

      TS_ASSERT_EQUALS(c.nodes(), 0);
      TS_ASSERT_EQUALS(c.edges(), 0);
      TS_ASSERT_EQUALS(c.arcs(), 0);
    }
  };
}
