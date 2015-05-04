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

#include <vector>
#include <iostream>

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/graphs/graphElements.h>
#include <agrum/graphs/cliqueGraph.h>

/* The graphs used for the tests:
 * G1:          G2:       G3:
 *      A           A        A          A = {1,2}
 *      |           | \      |          B = {2,3,4}
 *      B--C        B--C     B--C       C = {3,4,5}
 *      |           | /      |          D = {2,4,6}
 *      D           |/       E          E = {2,4,5}
 *                  D
 *
 *  G1 is a join tree, G2 is a hypergraph, and G3 is a hypertree but does not
 *  have the running intersection property.
 */

namespace gum_tests {

  class CliqueGraphTestSuite : public CxxTest::TestSuite {
    private:
    void fillSets(gum::NodeSet &A, gum::NodeSet &B, gum::NodeSet &C, gum::NodeSet &D,
                  gum::NodeSet &E) {
      A << 1 << 2;
      B << 2 << 3 << 4;
      C << 3 << 4 << 5;
      D << 2 << 4 << 6;
      E << 2 << 4 << 5;
    }

    void fillG1(gum::CliqueGraph &graph) {
      gum::NodeSet A, B, C, D, E;
      fillSets(A, B, C, D, E);

      graph.addNode(1, A);
      graph.addNode(2, B);
      graph.addNode(3, C);
      graph.addNode(4, D);

      graph.addEdge(1, 2);
      graph.addEdge(2, 3);
      graph.addEdge(2, 4);
    }

    void fillG2(gum::CliqueGraph &graph) {
      gum::NodeSet A, B, C, D, E;
      fillSets(A, B, C, D, E);

      graph.addNode(1, A);
      graph.addNode(2, B);
      graph.addNode(3, C);
      graph.addNode(4, D);

      graph.addEdge(1, 2);
      graph.addEdge(2, 3);
      graph.addEdge(2, 4);
      graph.addEdge(4, 3);
      graph.addEdge(1, 3);
    }

    void fillG3(gum::CliqueGraph &graph) {
      gum::NodeSet A, B, C, D, E;
      fillSets(A, B, C, D, E);

      graph.addNode(1, A);
      graph.addNode(2, B);
      graph.addNode(3, C);
      graph.addNode(4, E);

      graph.addEdge(1, 2);
      graph.addEdge(2, 3);
      graph.addEdge(2, 4);
    }

    public:
    void setUp() {}

    void teardDown() {}

    void testConstructor() {
      gum::CliqueGraph *graph = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING((graph = new gum::CliqueGraph()));
      TS_GUM_ASSERT_THROWS_NOTHING(delete graph);
    };

    void testFillG1() {
      gum::NodeSet A, B, C, D, E;
      fillSets(A, B, C, D, E);

      gum::CliqueGraph graph;

      TS_GUM_ASSERT_THROWS_NOTHING(fillG1(graph));

      TS_ASSERT_EQUALS(graph.size(), (gum::Size)4);

      TS_ASSERT_EQUALS(graph.clique(1), A);
      TS_ASSERT_EQUALS(graph.clique(2), B);
      TS_ASSERT_EQUALS(graph.clique(3), C);
      TS_ASSERT_EQUALS(graph.clique(4), D);

      TS_ASSERT(graph.existsEdge(1, 2));
      TS_ASSERT(graph.existsEdge(2, 4));
      TS_ASSERT(graph.existsEdge(2, 3));
    }

    void testFillG2() {
      gum::NodeSet A, B, C, D, E;
      fillSets(A, B, C, D, E);

      gum::CliqueGraph graph;
      TS_GUM_ASSERT_THROWS_NOTHING(fillG2(graph));

      TS_ASSERT_EQUALS(graph.size(), (gum::Size)4);

      TS_ASSERT_EQUALS(graph.clique(1), A);
      TS_ASSERT_EQUALS(graph.clique(2), B);
      TS_ASSERT_EQUALS(graph.clique(3), C);
      TS_ASSERT_EQUALS(graph.clique(4), D);

      TS_ASSERT(graph.existsEdge(1, 2));
      TS_ASSERT(graph.existsEdge(2, 4));
      TS_ASSERT(graph.existsEdge(2, 3));
      TS_ASSERT(graph.existsEdge(1, 3));
      TS_ASSERT(graph.existsEdge(3, 4));
    }

    void testFillG3() {
      gum::NodeSet A, B, C, D, E;
      fillSets(A, B, C, D, E);

      gum::CliqueGraph graph;
      TS_GUM_ASSERT_THROWS_NOTHING(fillG3(graph));

      TS_ASSERT_EQUALS(graph.size(), (gum::Size)4);

      TS_ASSERT(graph.existsEdge(1, 2));
      TS_ASSERT(graph.existsEdge(2, 4));
      TS_ASSERT(graph.existsEdge(2, 3));

      TS_ASSERT_EQUALS(graph.clique(1), A);
      TS_ASSERT_EQUALS(graph.clique(2), B);
      TS_ASSERT_EQUALS(graph.clique(3), C);
      TS_ASSERT_DIFFERS(graph.clique(4), D);
      TS_ASSERT_EQUALS(graph.clique(4), E);
    }

    void testEquality() {
      gum::CliqueGraph g1, g2, g3, g4;
      fillG1(g1);
      fillG2(g2);
      fillG3(g3);
      fillG1(g4);

      TS_ASSERT_EQUALS(g1, g4);
      TS_ASSERT_EQUALS(g4, g1);

      TS_ASSERT_DIFFERS(g1, g2);
      TS_ASSERT_DIFFERS(g2, g1);

      TS_ASSERT_DIFFERS(g1, g3);
      TS_ASSERT_DIFFERS(g3, g1);

      TS_ASSERT_DIFFERS(g2, g3);
      TS_ASSERT_DIFFERS(g3, g2);

      TS_ASSERT_EQUALS(g1, g1);
      TS_ASSERT_EQUALS(g2, g2);
      TS_ASSERT_EQUALS(g3, g3);
      TS_ASSERT_EQUALS(g4, g4);
    }

    void testCopyConstructor() {
      gum::CliqueGraph g1, g2, g3;
      fillG1(g1);
      fillG2(g2);
      fillG3(g3);

      gum::CliqueGraph *copy1 = nullptr;
      gum::CliqueGraph *copy2 = nullptr;
      gum::CliqueGraph *copy3 = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING((copy1 = new gum::CliqueGraph(g1)));
      TS_GUM_ASSERT_THROWS_NOTHING((copy2 = new gum::CliqueGraph(g2)));
      TS_GUM_ASSERT_THROWS_NOTHING((copy3 = new gum::CliqueGraph(g3)));

      TS_ASSERT_EQUALS(g1, *copy1);
      TS_ASSERT_EQUALS(g2, *copy2);
      TS_ASSERT_EQUALS(g3, *copy3);

      copy1->clear();
      copy2->clearEdges();

      TS_ASSERT_DIFFERS(g1, *copy1);
      TS_ASSERT_DIFFERS(g2, *copy2);

      TS_GUM_ASSERT_THROWS_NOTHING((delete copy1));
      TS_GUM_ASSERT_THROWS_NOTHING((delete copy2));
      TS_GUM_ASSERT_THROWS_NOTHING((delete copy3));
    }

    void testIsJoinTree() {
      gum::CliqueGraph g1, g2, g3;
      fillG1(g1);
      fillG2(g2);
      fillG3(g3);

      TS_ASSERT(!g1.hasUndirectedCycle());
      TS_ASSERT(g2.hasUndirectedCycle());
      TS_ASSERT(!g3.hasUndirectedCycle());

      try {
        g1.hasRunningIntersection();
      } catch (gum::Exception &e) {
        GUM_SHOWERROR(e);
      }

      TS_ASSERT(g1.hasRunningIntersection());

      TS_ASSERT(g2.hasRunningIntersection());
      TS_ASSERT(!g3.hasRunningIntersection());

      TS_ASSERT(g1.isJoinTree());
      TS_ASSERT(!g2.isJoinTree());
      TS_ASSERT(!g3.isJoinTree());
    }

    void testHasRunningIntersection() {
      gum::CliqueGraph g1, g2, g3;
      fillG1(g1);
      fillG2(g2);
      fillG3(g3);

      TS_ASSERT(g1.hasRunningIntersection());
      TS_ASSERT(g2.hasRunningIntersection());
      TS_ASSERT(!g3.hasRunningIntersection());
    }

    void testGetNbrEdges() {
      gum::CliqueGraph g1, g2, g3;
      fillG1(g1);
      fillG2(g2);
      fillG3(g3);

      TS_ASSERT_EQUALS(g1.sizeEdges(), (gum::Size)3);
      TS_ASSERT_EQUALS(g2.sizeEdges(), (gum::Size)5);
      TS_ASSERT_EQUALS(g3.sizeEdges(), (gum::Size)3);
    }

    void testClearNodes() {
      gum::CliqueGraph g1, g2, g3;
      fillG1(g1);
      fillG2(g2);
      fillG3(g3);

      TS_GUM_ASSERT_THROWS_NOTHING(g1.clear());
      TS_ASSERT_EQUALS(g1.size(), (gum::Size)0);
      TS_ASSERT_EQUALS(g1.sizeEdges(), (gum::Size)0);

      TS_GUM_ASSERT_THROWS_NOTHING(g2.clear());
      TS_ASSERT_EQUALS(g2.size(), (gum::Size)0);
      TS_ASSERT_EQUALS(g2.sizeEdges(), (gum::Size)0);

      TS_GUM_ASSERT_THROWS_NOTHING(g3.clear());
      TS_ASSERT_EQUALS(g3.size(), (gum::Size)0);
      TS_ASSERT_EQUALS(g3.sizeEdges(), (gum::Size)0);
    }

    void testClearEdges() {
      gum::CliqueGraph g1, g2, g3;
      fillG1(g1);
      fillG2(g2);
      fillG3(g3);

      gum::Size nodeG1Count = g1.size();
      TS_GUM_ASSERT_THROWS_NOTHING(g1.clearEdges());
      TS_ASSERT_EQUALS(g1.size(), nodeG1Count);
      TS_ASSERT_EQUALS(g1.sizeEdges(), (gum::Size)0);

      gum::Size nodeG2Count = g2.size();
      TS_GUM_ASSERT_THROWS_NOTHING(g2.clearEdges());
      TS_ASSERT_EQUALS(g2.size(), nodeG2Count);
      TS_ASSERT_EQUALS(g2.sizeEdges(), (gum::Size)0);

      gum::Size nodeG3Count = g3.size();
      TS_GUM_ASSERT_THROWS_NOTHING(g3.clearEdges());
      TS_ASSERT_EQUALS(g3.size(), nodeG3Count);
      TS_ASSERT_EQUALS(g3.sizeEdges(), (gum::Size)0);
    }

    void testEraseNode() {
      gum::CliqueGraph g1, g2, g3;
      fillG1(g1);
      fillG2(g2);
      fillG3(g3);

      gum::Size nodeG1Count = g1.size();
      TS_GUM_ASSERT_THROWS_NOTHING(g1.eraseNode(1));
      TS_ASSERT_EQUALS(g1.size(), nodeG1Count - 1);
      TS_GUM_ASSERT_THROWS_NOTHING(g1.eraseNode(1));
      TS_ASSERT_EQUALS(g1.size(), nodeG1Count - 1);
      TS_GUM_ASSERT_THROWS_NOTHING(g1.eraseNode(15));
      TS_ASSERT_EQUALS(g1.size(), nodeG1Count - 1);

      gum::Size nodeG2Count = g2.size();
      TS_GUM_ASSERT_THROWS_NOTHING(g2.eraseNode(1));
      TS_ASSERT_EQUALS(g2.size(), nodeG2Count - 1);
      TS_GUM_ASSERT_THROWS_NOTHING(g2.eraseNode(1));
      TS_ASSERT_EQUALS(g2.size(), nodeG2Count - 1);
      TS_GUM_ASSERT_THROWS_NOTHING(g2.eraseNode(15));
      TS_ASSERT_EQUALS(g2.size(), nodeG2Count - 1);

      gum::Size nodeG3Count = g3.size();
      TS_GUM_ASSERT_THROWS_NOTHING(g3.eraseNode(1));
      TS_ASSERT_EQUALS(g3.size(), nodeG3Count - 1);
      TS_GUM_ASSERT_THROWS_NOTHING(g3.eraseNode(1));
      TS_ASSERT_EQUALS(g3.size(), nodeG3Count - 1);
      TS_GUM_ASSERT_THROWS_NOTHING(g3.eraseNode(15));
      TS_ASSERT_EQUALS(g3.size(), nodeG3Count - 1);
    }

    void testEraseNodeStubborn() {
      gum::CliqueGraph g1;
      fillG1(g1);

      gum::Size nodeG1Count = g1.size();

      for (int i = 0; i < 10; i++) {
        TS_GUM_ASSERT_THROWS_NOTHING(g1.eraseNode(1));
        TS_ASSERT_EQUALS(g1.size(), nodeG1Count - 1);
      }
    }

    void testGetSeparatorByEdgeG1() {
      gum::CliqueGraph g1;
      fillG1(g1);

      gum::NodeSet separator;

      TS_GUM_ASSERT_THROWS_NOTHING(separator = g1.separator(1, 2));
      TS_ASSERT_EQUALS(separator.size(), (gum::Size)1);
      TS_ASSERT(separator.contains(2));

      TS_GUM_ASSERT_THROWS_NOTHING(separator = g1.separator(2, 3));
      TS_ASSERT_EQUALS(separator.size(), (gum::Size)2);
      TS_ASSERT(separator.contains(3));
      TS_ASSERT(separator.contains(4));

      TS_GUM_ASSERT_THROWS_NOTHING(separator = g1.separator(2, 4));
      TS_ASSERT_EQUALS(separator.size(), (gum::Size)2);
      TS_ASSERT(separator.contains(2));
      TS_ASSERT(separator.contains(4));
    }

    void testGetSeparatorByEdgeG2() {
      gum::CliqueGraph g2;
      fillG2(g2);

      gum::NodeSet separator;

      TS_GUM_ASSERT_THROWS_NOTHING(separator = g2.separator(1, 2));
      TS_ASSERT_EQUALS(separator.size(), (gum::Size)1);
      TS_ASSERT(separator.contains(2));

      separator.clear();

      TS_GUM_ASSERT_THROWS_NOTHING(separator = g2.separator(2, 3));
      TS_ASSERT_EQUALS(separator.size(), (gum::Size)2);
      TS_ASSERT(separator.contains(3));
      TS_ASSERT(separator.contains(4));

      separator.clear();

      TS_GUM_ASSERT_THROWS_NOTHING(separator = g2.separator(2, 4));
      TS_ASSERT_EQUALS(separator.size(), (gum::Size)2);
      TS_ASSERT(separator.contains(2));
      TS_ASSERT(separator.contains(4));

      separator.clear();

      TS_GUM_ASSERT_THROWS_NOTHING(separator = g2.separator(1, 3));

      separator.clear();

      TS_GUM_ASSERT_THROWS_NOTHING(separator = g2.separator(3, 4));
      TS_ASSERT_EQUALS(separator.size(), (gum::Size)1);
      TS_ASSERT(separator.contains(4));
    }

    void testGetSeparatorByEdgeG3() {
      gum::CliqueGraph g3;
      fillG3(g3);

      gum::NodeSet separator;

      TS_GUM_ASSERT_THROWS_NOTHING(separator = g3.separator(1, 2));
      TS_ASSERT_EQUALS(separator.size(), (gum::Size)1);
      TS_ASSERT(separator.contains(2));

      separator.clear();

      TS_GUM_ASSERT_THROWS_NOTHING(separator = g3.separator(2, 3));
      TS_ASSERT_EQUALS(separator.size(), (gum::Size)2);
      TS_ASSERT(separator.contains(3));
      TS_ASSERT(separator.contains(4));

      separator.clear();

      TS_GUM_ASSERT_THROWS_NOTHING(separator = g3.separator(2, 4));
      TS_ASSERT_EQUALS(separator.size(), (gum::Size)2);
      TS_ASSERT(separator.contains(2));
      TS_ASSERT(separator.contains(4));
    }

    void testEraseEdgeG1() {
      gum::CliqueGraph g1;
      fillG1(g1);

      int countEdgeG1 = g1.sizeEdges();

      TS_GUM_ASSERT_THROWS_NOTHING(g1.eraseEdge(gum::Edge(1, 2)));
      TS_GUM_ASSERT_THROWS_NOTHING(g1.eraseEdge(gum::Edge(1, 2)));
      TS_GUM_ASSERT_THROWS_NOTHING(g1.eraseEdge(gum::Edge(19, 33)));

      TS_ASSERT_EQUALS(g1.sizeEdges(), gum::Size(countEdgeG1 - 1));

      TS_ASSERT_THROWS_ANYTHING(g1.separator(1, 2));
    }

    void testEraseEdgeG2() {
      gum::CliqueGraph g2;
      fillG2(g2);

      int countEdgeG2 = g2.sizeEdges();

      TS_GUM_ASSERT_THROWS_NOTHING(g2.eraseEdge(gum::Edge(2, 4)));
      TS_GUM_ASSERT_THROWS_NOTHING(g2.eraseEdge(gum::Edge(1, 3)));

      TS_ASSERT_EQUALS(g2.sizeEdges(), gum::Size(countEdgeG2 - 2));
    }

    void testEraseEdgeG3() {
      gum::CliqueGraph g3;
      fillG3(g3);

      int countEdgeG3 = g3.sizeEdges();

      TS_GUM_ASSERT_THROWS_NOTHING(g3.eraseEdge(gum::Edge(2, 4)));

      TS_ASSERT_EQUALS(g3.sizeEdges(), gum::Size(countEdgeG3 - 1));
    }

    void testGetClique_1() {
      gum::CliqueGraph graph;
      fillG1(graph);

      TS_GUM_ASSERT_THROWS_NOTHING(graph.clique(1));
      const gum::NodeSet &clique = graph.clique(1);

      TS_ASSERT_EQUALS(clique.size(), (gum::Size)2);
      TS_ASSERT(clique.contains(1));
      TS_ASSERT(clique.contains(2));

      TS_ASSERT_THROWS_ANYTHING(graph.clique(10));
    }

    void testGetClique_2() {
      gum::CliqueGraph graph;
      fillG1(graph);

      TS_GUM_ASSERT_THROWS_NOTHING(graph.clique(1));
      gum::NodeSet clique = graph.clique(1);

      graph.eraseNode(1);

      TS_ASSERT_THROWS_ANYTHING(graph.clique(1));

      TS_ASSERT_EQUALS(clique.size(), (gum::Size)2);
      TS_ASSERT(clique.contains(1));
      TS_ASSERT(clique.contains(2));

      gum::NodeId new_1 = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(new_1 = graph.addNode(clique));
      TS_ASSERT_EQUALS(clique, graph.clique(new_1));
    }

    void testGetContainerNode() {
      gum::NodeSet A, B, C, D, E;
      fillSets(A, B, C, D, E);

      gum::CliqueGraph graph;
      fillG2(graph);

      gum::NodeId id = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(id = graph.container(1));
      TS_ASSERT_EQUALS(id, (gum::NodeId)1);

      TS_ASSERT_THROWS_ANYTHING(graph.container(42));

      gum::NodeId whichId = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(whichId = graph.container(2));
      gum::NodeSet firstClique = graph.clique(whichId);
      graph.eraseNode(whichId);

      TS_GUM_ASSERT_THROWS_NOTHING(whichId = graph.container(2));
      gum::NodeSet secondClique = graph.clique(whichId);
      graph.eraseNode(whichId);

      TS_GUM_ASSERT_THROWS_NOTHING(whichId = graph.container(2));
      gum::NodeSet thirdClique = graph.clique(whichId);
      graph.eraseNode(whichId);

      TS_ASSERT_THROWS_ANYTHING(graph.container(2));

      TS_ASSERT(!(firstClique != A && firstClique != B && firstClique != D));
      TS_ASSERT(!(secondClique != A && secondClique != B && secondClique != D));
      TS_ASSERT(!(thirdClique != A && thirdClique != B && thirdClique != D));
      TS_ASSERT(firstClique != secondClique && firstClique != thirdClique &&
                secondClique != thirdClique);
    }

    void testSetClique_1() {
      gum::CliqueGraph graph;
      fillG3(graph);

      TS_ASSERT(!graph.hasRunningIntersection());

      gum::NodeSet clique, result;
      clique << 2 << 3 << 4 << 5;

      TS_GUM_ASSERT_THROWS_NOTHING(graph.setClique(2, clique));
      TS_ASSERT_EQUALS(graph.clique(2).size(), gum::Size(4));
      TS_ASSERT(graph.hasRunningIntersection());
    }

    void testSetClique_2() {

      gum::CliqueGraph graph;
      fillG3(graph);

      gum::NodeSet emptyClique;

      TS_GUM_ASSERT_THROWS_NOTHING(graph.setClique(4, emptyClique));
      TS_ASSERT(graph.hasRunningIntersection());
    }

    void testAddToClique() {
      gum::CliqueGraph graph;
      fillG3(graph);

      TS_ASSERT(!graph.hasRunningIntersection());

      gum::NodeSet result;

      TS_GUM_ASSERT_THROWS_NOTHING(graph.addToClique(2, 5));
      // check a new separator ??
      TS_ASSERT_EQUALS(graph.clique(2).size(), gum::Size(4));

      TS_ASSERT(graph.hasRunningIntersection());
    }

    void testAddToCliqueByID() {
      gum::CliqueGraph graph;
      fillG3(graph);

      TS_ASSERT(!graph.hasRunningIntersection());

      gum::NodeSet result;

      TS_GUM_ASSERT_THROWS_NOTHING(graph.addToClique(2, 5));
      TS_ASSERT_EQUALS(graph.clique(2).size(), gum::Size(4));

      TS_ASSERT(graph.hasRunningIntersection());
    }

    void testEraseFromClique() {
      gum::CliqueGraph graph;
      fillG3(graph);

      TS_ASSERT(!graph.hasRunningIntersection());

      TS_GUM_ASSERT_THROWS_NOTHING(graph.eraseFromClique(3, 5));

      TS_ASSERT(graph.hasRunningIntersection());

      TS_GUM_ASSERT_THROWS_NOTHING(graph.eraseFromClique(3, 5));
      TS_GUM_ASSERT_THROWS_NOTHING(graph.eraseFromClique(1, 6));
      /* No Comment on this in the documentation, but can be expected.*/
      TS_GUM_ASSERT_THROWS_NOTHING(graph.eraseFromClique(4, 5));
      /* No Comment on this in the documentation, but can be expected.*/
      TS_ASSERT_THROWS_ANYTHING(graph.eraseFromClique(42, 1));
    }

    void testGetPath() {
      gum::CliqueGraph graph;
      fillG2(graph);

      std::vector<gum::Id> vec;

      try {
        vec = graph.undirectedPath(1, 4);
      } catch (gum::Exception &e) {
        GUM_SHOWERROR(e);
      }

      TS_GUM_ASSERT_THROWS_NOTHING(vec = graph.undirectedPath(1, 4));

      /** Paths from A to D goes threw 3 or 4 cliques.*/
      TS_ASSERT((vec.size() == 3) || (vec.size() == 4));

      /* From A, must pass throught B or C. */

      if (vec[1] == 2) {
        graph.eraseEdge(gum::Edge(1, 2));
      } else if (vec[1] == 3) {
        /** Paths from A to D  still goes threw 3 or 4 cliques.*/
        TS_ASSERT((vec.size() == 3) || (vec.size() == 4));

        /** Deleting an edge to force A -> B -> C -> D or A -> C -> B -> D.  */

        if (vec[1] == 2) {
          graph.eraseEdge(gum::Edge(2, 4));
        } else if (vec[1] == 3) {
          graph.eraseEdge(gum::Edge(3, 4));
        }

        TS_GUM_ASSERT_THROWS_NOTHING(vec = graph.undirectedPath(1, 4));

        /** Now can only have 3 nodes in the path. */
        TS_ASSERT_EQUALS(vec.size(), (gum::Size)3);

        graph.eraseEdge(gum::Edge(2, 3));

        TS_ASSERT_THROWS_NOTHING(vec = graph.undirectedPath(1, 4));
      }
    }

    void testCopyOperator() {
      gum::CliqueGraph g1, g2, g3;
      fillG1(g1);
      fillG2(g2);
      fillG3(g3);

      gum::CliqueGraph copy1, copy2, copy3;

      TS_GUM_ASSERT_THROWS_NOTHING(copy1 = g1);
      TS_GUM_ASSERT_THROWS_NOTHING(copy2 = g2);
      TS_GUM_ASSERT_THROWS_NOTHING(copy3 = g3);

      TS_ASSERT_EQUALS(g1, copy1);
      TS_ASSERT_EQUALS(g2, copy2);
      TS_ASSERT_EQUALS(g3, copy3);

      copy1.clear();
      copy2.clearEdges();

      TS_ASSERT_DIFFERS(g1, copy1);
      TS_ASSERT_DIFFERS(g2, copy2);
    }

    void testRunningIntProp() {
      gum::CliqueGraph g;

      gum::NodeSet n1; // ABC
      n1.insert((gum::NodeId)1);
      n1.insert((gum::NodeId)2);
      n1.insert((gum::NodeId)3);
      g.addNode((gum::NodeId)1, n1);

      gum::NodeSet n2; // BCD
      n2.insert((gum::NodeId)2);
      n2.insert((gum::NodeId)3);
      n2.insert((gum::NodeId)4);
      g.addNode((gum::NodeId)2, n2);

      gum::NodeSet n3; // CDE
      n3.insert((gum::NodeId)3);
      n3.insert((gum::NodeId)4);
      n3.insert((gum::NodeId)5);
      g.addNode((gum::NodeId)3, n3);

      gum::NodeSet n4; // ADE
      n4.insert((gum::NodeId)4);
      n4.insert((gum::NodeId)5);
      n4.insert((gum::NodeId)1);
      g.addNode((gum::NodeId)4, n4);

      gum::NodeSet n5; // ABE
      n5.insert((gum::NodeId)5);
      n5.insert((gum::NodeId)1);
      n5.insert((gum::NodeId)2);
      g.addNode((gum::NodeId)5, n5);

      g.addEdge(1, 2);
      g.addEdge(2, 3);
      g.addEdge(3, 4);
      g.addEdge(4, 5);
      g.addEdge(5, 1);

      TS_ASSERT(g.hasRunningIntersection() == true);
    }
  };
}
