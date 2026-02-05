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
#include <vector>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/graphs/cliqueGraph.h>
#include <agrum/base/graphs/graphElements.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/lazyPropagation.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  CliqueGraph
#define GUM_CURRENT_MODULE BN

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

  struct CliqueGraphTestSuite {
    private:
    static void fillSets(gum::NodeSet& A,
                         gum::NodeSet& B,
                         gum::NodeSet& C,
                         gum::NodeSet& D,
                         gum::NodeSet& E) {
      A << 1 << 2;
      B << 2 << 3 << 4;
      C << 3 << 4 << 5;
      D << 2 << 4 << 6;
      E << 2 << 4 << 5;
    }

    static void fillG1(gum::CliqueGraph& graph) {
      gum::NodeSet A, B, C, D, E;
      fillSets(A, B, C, D, E);

      graph.addNodeWithId(1, A);
      graph.addNodeWithId(2, B);
      graph.addNodeWithId(3, C);
      graph.addNodeWithId(4, D);

      graph.addEdge(1, 2);
      graph.addEdge(2, 3);
      graph.addEdge(2, 4);
    }

    static void fillG2(gum::CliqueGraph& graph) {
      gum::NodeSet A, B, C, D, E;
      fillSets(A, B, C, D, E);

      graph.addNodeWithId(1, A);
      graph.addNodeWithId(2, B);
      graph.addNodeWithId(3, C);
      graph.addNodeWithId(4, D);

      graph.addEdge(1, 2);
      graph.addEdge(2, 3);
      graph.addEdge(2, 4);
      graph.addEdge(4, 3);
      graph.addEdge(1, 3);
    }

    static void fillG3(gum::CliqueGraph& graph) {
      gum::NodeSet A, B, C, D, E;
      fillSets(A, B, C, D, E);

      graph.addNodeWithId(1, A);
      graph.addNodeWithId(2, B);
      graph.addNodeWithId(3, C);
      graph.addNodeWithId(4, E);

      graph.addEdge(1, 2);
      graph.addEdge(2, 3);
      graph.addEdge(2, 4);
    }

    public:
    static void teardDown() {}

    static void testConstructor() {
      gum::CliqueGraph* graph = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING((graph = new gum::CliqueGraph()));
      GUM_CHECK_ASSERT_THROWS_NOTHING(delete graph);
    };

    static void testFillG1() {
      gum::NodeSet A, B, C, D, E;
      fillSets(A, B, C, D, E);

      gum::CliqueGraph graph;

      GUM_CHECK_ASSERT_THROWS_NOTHING(fillG1(graph));

      CHECK((graph.size()) == (static_cast< gum::Size >(4)));

      CHECK((graph.clique(1)) == (A));
      CHECK((graph.clique(2)) == (B));
      CHECK((graph.clique(3)) == (C));
      CHECK((graph.clique(4)) == (D));

      CHECK(graph.existsEdge(1, 2));
      CHECK(graph.existsEdge(2, 4));
      CHECK(graph.existsEdge(2, 3));
    }

    static void testFillG2() {
      gum::NodeSet A, B, C, D, E;
      fillSets(A, B, C, D, E);

      gum::CliqueGraph graph;
      GUM_CHECK_ASSERT_THROWS_NOTHING(fillG2(graph));

      CHECK((graph.size()) == (static_cast< gum::Size >(4)));

      CHECK((graph.clique(1)) == (A));
      CHECK((graph.clique(2)) == (B));
      CHECK((graph.clique(3)) == (C));
      CHECK((graph.clique(4)) == (D));

      CHECK(graph.existsEdge(1, 2));
      CHECK(graph.existsEdge(2, 4));
      CHECK(graph.existsEdge(2, 3));
      CHECK(graph.existsEdge(1, 3));
      CHECK(graph.existsEdge(3, 4));
    }

    static void testFillG3() {
      gum::NodeSet A, B, C, D, E;
      fillSets(A, B, C, D, E);

      gum::CliqueGraph graph;
      GUM_CHECK_ASSERT_THROWS_NOTHING(fillG3(graph));

      CHECK((graph.size()) == (static_cast< gum::Size >(4)));

      CHECK(graph.existsEdge(1, 2));
      CHECK(graph.existsEdge(2, 4));
      CHECK(graph.existsEdge(2, 3));

      CHECK((graph.clique(1)) == (A));
      CHECK((graph.clique(2)) == (B));
      CHECK((graph.clique(3)) == (C));
      CHECK((graph.clique(4)) != (D));
      CHECK((graph.clique(4)) == (E));
    }

    static void testEquality() {
      gum::CliqueGraph g1, g2, g3, g4;
      fillG1(g1);
      fillG2(g2);
      fillG3(g3);
      fillG1(g4);

      CHECK((g1) == (g4));
      CHECK((g4) == (g1));

      CHECK((g1) != (g2));
      CHECK((g2) != (g1));

      CHECK((g1) != (g3));
      CHECK((g3) != (g1));

      CHECK((g2) != (g3));
      CHECK((g3) != (g2));

      CHECK((g1) == (g1));
      CHECK((g2) == (g2));
      CHECK((g3) == (g3));
      CHECK((g4) == (g4));
    }

    static void testCopyConstructor() {
      gum::CliqueGraph g1, g2, g3;
      fillG1(g1);
      fillG2(g2);
      fillG3(g3);

      gum::CliqueGraph* copy1 = nullptr;
      gum::CliqueGraph* copy2 = nullptr;
      gum::CliqueGraph* copy3 = nullptr;

      GUM_CHECK_ASSERT_THROWS_NOTHING((copy1 = new gum::CliqueGraph(g1)));
      GUM_CHECK_ASSERT_THROWS_NOTHING((copy2 = new gum::CliqueGraph(g2)));
      GUM_CHECK_ASSERT_THROWS_NOTHING((copy3 = new gum::CliqueGraph(g3)));

      CHECK((g1) == (*copy1));
      CHECK((g2) == (*copy2));
      CHECK((g3) == (*copy3));

      copy1->clear();
      copy2->clearEdges();

      CHECK((g1) != (*copy1));
      CHECK((g2) != (*copy2));

      GUM_CHECK_ASSERT_THROWS_NOTHING((delete copy1));
      GUM_CHECK_ASSERT_THROWS_NOTHING((delete copy2));
      GUM_CHECK_ASSERT_THROWS_NOTHING((delete copy3));
    }

    static void testIsJoinTree() {
      gum::CliqueGraph g1, g2, g3;
      fillG1(g1);
      fillG2(g2);
      fillG3(g3);

      CHECK(!g1.hasUndirectedCycle());
      CHECK(g2.hasUndirectedCycle());
      CHECK(!g3.hasUndirectedCycle());

      try {
        g1.hasRunningIntersection();
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }

      CHECK(g1.hasRunningIntersection());

      CHECK(g2.hasRunningIntersection());
      CHECK(!g3.hasRunningIntersection());

      CHECK(g1.isJoinTree());
      CHECK(!g2.isJoinTree());
      CHECK(!g3.isJoinTree());
    }

    static void testHasRunningIntersection() {
      gum::CliqueGraph g1, g2, g3;
      fillG1(g1);
      fillG2(g2);
      fillG3(g3);

      CHECK(g1.hasRunningIntersection());
      CHECK(g2.hasRunningIntersection());
      CHECK(!g3.hasRunningIntersection());
    }

    static void testGetNbrEdges() {
      gum::CliqueGraph g1, g2, g3;
      fillG1(g1);
      fillG2(g2);
      fillG3(g3);

      CHECK((g1.sizeEdges()) == (static_cast< gum::Size >(3)));
      CHECK((g2.sizeEdges()) == (static_cast< gum::Size >(5)));
      CHECK((g3.sizeEdges()) == (static_cast< gum::Size >(3)));
    }

    static void testClearNodes() {
      gum::CliqueGraph g1, g2, g3;
      fillG1(g1);
      fillG2(g2);
      fillG3(g3);

      GUM_CHECK_ASSERT_THROWS_NOTHING(g1.clear());
      CHECK((g1.size()) == (static_cast< gum::Size >(0)));
      CHECK((g1.sizeEdges()) == (static_cast< gum::Size >(0)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(g2.clear());
      CHECK((g2.size()) == (static_cast< gum::Size >(0)));
      CHECK((g2.sizeEdges()) == (static_cast< gum::Size >(0)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(g3.clear());
      CHECK((g3.size()) == (static_cast< gum::Size >(0)));
      CHECK((g3.sizeEdges()) == (static_cast< gum::Size >(0)));
    }

    static void testClearEdges() {
      gum::CliqueGraph g1, g2, g3;
      fillG1(g1);
      fillG2(g2);
      fillG3(g3);

      gum::Size nodeG1Count = g1.size();
      GUM_CHECK_ASSERT_THROWS_NOTHING(g1.clearEdges());
      CHECK((g1.size()) == (nodeG1Count));
      CHECK((g1.sizeEdges()) == (static_cast< gum::Size >(0)));

      gum::Size nodeG2Count = g2.size();
      GUM_CHECK_ASSERT_THROWS_NOTHING(g2.clearEdges());
      CHECK((g2.size()) == (nodeG2Count));
      CHECK((g2.sizeEdges()) == (static_cast< gum::Size >(0)));

      gum::Size nodeG3Count = g3.size();
      GUM_CHECK_ASSERT_THROWS_NOTHING(g3.clearEdges());
      CHECK((g3.size()) == (nodeG3Count));
      CHECK((g3.sizeEdges()) == (static_cast< gum::Size >(0)));
    }

    static void testEraseNode() {
      gum::CliqueGraph g1, g2, g3;
      fillG1(g1);
      fillG2(g2);
      fillG3(g3);

      gum::Size nodeG1Count = g1.size();
      GUM_CHECK_ASSERT_THROWS_NOTHING(g1.eraseNode(1));
      CHECK((g1.size()) == (nodeG1Count - 1));
      GUM_CHECK_ASSERT_THROWS_NOTHING(g1.eraseNode(1));
      CHECK((g1.size()) == (nodeG1Count - 1));
      GUM_CHECK_ASSERT_THROWS_NOTHING(g1.eraseNode(15));
      CHECK((g1.size()) == (nodeG1Count - 1));

      gum::Size nodeG2Count = g2.size();
      GUM_CHECK_ASSERT_THROWS_NOTHING(g2.eraseNode(1));
      CHECK((g2.size()) == (nodeG2Count - 1));
      GUM_CHECK_ASSERT_THROWS_NOTHING(g2.eraseNode(1));
      CHECK((g2.size()) == (nodeG2Count - 1));
      GUM_CHECK_ASSERT_THROWS_NOTHING(g2.eraseNode(15));
      CHECK((g2.size()) == (nodeG2Count - 1));

      gum::Size nodeG3Count = g3.size();
      GUM_CHECK_ASSERT_THROWS_NOTHING(g3.eraseNode(1));
      CHECK((g3.size()) == (nodeG3Count - 1));
      GUM_CHECK_ASSERT_THROWS_NOTHING(g3.eraseNode(1));
      CHECK((g3.size()) == (nodeG3Count - 1));
      GUM_CHECK_ASSERT_THROWS_NOTHING(g3.eraseNode(15));
      CHECK((g3.size()) == (nodeG3Count - 1));
    }

    static void testEraseNodeStubborn() {
      gum::CliqueGraph g1;
      fillG1(g1);

      gum::Size nodeG1Count = g1.size();

      for (int i = 0; i < 10; i++) {
        GUM_CHECK_ASSERT_THROWS_NOTHING(g1.eraseNode(1));
        CHECK((g1.size()) == (nodeG1Count - 1));
      }
    }

    static void testGetSeparatorByEdgeG1() {
      gum::CliqueGraph g1;
      fillG1(g1);

      gum::NodeSet separator;

      GUM_CHECK_ASSERT_THROWS_NOTHING(separator = g1.separator(1, 2));
      CHECK((separator.size()) == (static_cast< gum::Size >(1)));
      CHECK(separator.contains(2));

      GUM_CHECK_ASSERT_THROWS_NOTHING(separator = g1.separator(2, 3));
      CHECK((separator.size()) == (static_cast< gum::Size >(2)));
      CHECK(separator.contains(3));
      CHECK(separator.contains(4));

      GUM_CHECK_ASSERT_THROWS_NOTHING(separator = g1.separator(2, 4));
      CHECK((separator.size()) == (static_cast< gum::Size >(2)));
      CHECK(separator.contains(2));
      CHECK(separator.contains(4));
    }

    static void testGetSeparatorByEdgeG2() {
      gum::CliqueGraph g2;
      fillG2(g2);

      gum::NodeSet separator;

      GUM_CHECK_ASSERT_THROWS_NOTHING(separator = g2.separator(1, 2));
      CHECK((separator.size()) == (static_cast< gum::Size >(1)));
      CHECK(separator.contains(2));

      separator.clear();

      GUM_CHECK_ASSERT_THROWS_NOTHING(separator = g2.separator(2, 3));
      CHECK((separator.size()) == (static_cast< gum::Size >(2)));
      CHECK(separator.contains(3));
      CHECK(separator.contains(4));

      separator.clear();

      GUM_CHECK_ASSERT_THROWS_NOTHING(separator = g2.separator(2, 4));
      CHECK((separator.size()) == (static_cast< gum::Size >(2)));
      CHECK(separator.contains(2));
      CHECK(separator.contains(4));

      separator.clear();

      GUM_CHECK_ASSERT_THROWS_NOTHING(separator = g2.separator(1, 3));

      separator.clear();

      GUM_CHECK_ASSERT_THROWS_NOTHING(separator = g2.separator(3, 4));
      CHECK((separator.size()) == (static_cast< gum::Size >(1)));
      CHECK(separator.contains(4));
    }

    static void testGetSeparatorByEdgeG3() {
      gum::CliqueGraph g3;
      fillG3(g3);

      gum::NodeSet separator;

      GUM_CHECK_ASSERT_THROWS_NOTHING(separator = g3.separator(1, 2));
      CHECK((separator.size()) == (static_cast< gum::Size >(1)));
      CHECK(separator.contains(2));

      separator.clear();

      GUM_CHECK_ASSERT_THROWS_NOTHING(separator = g3.separator(2, 3));
      CHECK((separator.size()) == (static_cast< gum::Size >(2)));
      CHECK(separator.contains(3));
      CHECK(separator.contains(4));

      separator.clear();

      GUM_CHECK_ASSERT_THROWS_NOTHING(separator = g3.separator(2, 4));
      CHECK((separator.size()) == (static_cast< gum::Size >(2)));
      CHECK(separator.contains(2));
      CHECK(separator.contains(4));
    }

    static void testEraseEdgeG1() {
      gum::CliqueGraph g1;
      fillG1(g1);

      gum::Size countEdgeG1 = g1.sizeEdges();

      GUM_CHECK_ASSERT_THROWS_NOTHING(g1.eraseEdge(gum::Edge(1, 2)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(g1.eraseEdge(gum::Edge(1, 2)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(g1.eraseEdge(gum::Edge(19, 33)));

      CHECK((g1.sizeEdges()) == (gum::Size(countEdgeG1 - 1)));

      CHECK_THROWS(g1.separator(1, 2));
    }

    static void testEraseEdgeG2() {
      gum::CliqueGraph g2;
      fillG2(g2);

      gum::Size countEdgeG2 = g2.sizeEdges();

      GUM_CHECK_ASSERT_THROWS_NOTHING(g2.eraseEdge(gum::Edge(2, 4)));
      GUM_CHECK_ASSERT_THROWS_NOTHING(g2.eraseEdge(gum::Edge(1, 3)));

      CHECK((g2.sizeEdges()) == (gum::Size(countEdgeG2 - 2)));
    }

    static void testEraseEdgeG3() {
      gum::CliqueGraph g3;
      fillG3(g3);

      gum::Size countEdgeG3 = g3.sizeEdges();

      GUM_CHECK_ASSERT_THROWS_NOTHING(g3.eraseEdge(gum::Edge(2, 4)));

      CHECK((g3.sizeEdges()) == (gum::Size(countEdgeG3 - 1)));
    }

    static void testGetClique_1() {
      gum::CliqueGraph graph;
      fillG1(graph);

      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.clique(1));
      const gum::NodeSet& clique = graph.clique(1);

      CHECK((clique.size()) == (static_cast< gum::Size >(2)));
      CHECK(clique.contains(1));
      CHECK(clique.contains(2));

      CHECK_THROWS(graph.clique(10));
    }

    static void testGetClique_2() {
      gum::CliqueGraph graph;
      fillG1(graph);

      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.clique(1));
      gum::NodeSet clique = graph.clique(1);

      graph.eraseNode(1);

      CHECK_THROWS(graph.clique(1));

      CHECK((clique.size()) == (static_cast< gum::Size >(2)));
      CHECK(clique.contains(1));
      CHECK(clique.contains(2));

      gum::NodeId new_1 = 0;
      GUM_CHECK_ASSERT_THROWS_NOTHING(new_1 = graph.addNode(clique));
      CHECK((clique) == (graph.clique(new_1)));
    }

    static void testGetContainerNode() {
      gum::NodeSet A, B, C, D, E;
      fillSets(A, B, C, D, E);

      gum::CliqueGraph graph;
      fillG2(graph);

      gum::NodeId id = 0;
      GUM_CHECK_ASSERT_THROWS_NOTHING(id = graph.container(1));
      CHECK((id) == ((gum::NodeId)1));

      CHECK_THROWS(graph.container(42));

      gum::NodeId whichId = 0;
      GUM_CHECK_ASSERT_THROWS_NOTHING(whichId = graph.container(2));
      gum::NodeSet firstClique = graph.clique(whichId);
      graph.eraseNode(whichId);

      GUM_CHECK_ASSERT_THROWS_NOTHING(whichId = graph.container(2));
      gum::NodeSet secondClique = graph.clique(whichId);
      graph.eraseNode(whichId);

      GUM_CHECK_ASSERT_THROWS_NOTHING(whichId = graph.container(2));
      gum::NodeSet thirdClique = graph.clique(whichId);
      graph.eraseNode(whichId);

      CHECK_THROWS(graph.container(2));

      CHECK((!(firstClique != A && firstClique != B && firstClique != D)));
      CHECK((!(secondClique != A && secondClique != B && secondClique != D)));
      CHECK((!(thirdClique != A && thirdClique != B && thirdClique != D)));
      CHECK((firstClique != secondClique && firstClique != thirdClique
             && secondClique != thirdClique));
    }

    static void testSetClique_1() {
      gum::CliqueGraph graph;
      fillG3(graph);

      CHECK(!graph.hasRunningIntersection());

      gum::NodeSet clique, result;
      clique << 2 << 3 << 4 << 5;

      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.setClique(2, clique));
      CHECK((graph.clique(2).size()) == (static_cast< gum::Size >(4)));
      CHECK(graph.hasRunningIntersection());
    }

    static void testSetClique_2() {
      gum::CliqueGraph graph;
      fillG3(graph);

      gum::NodeSet emptyClique;

      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.setClique(4, emptyClique));
      CHECK(graph.hasRunningIntersection());
    }

    static void testAddToClique() {
      gum::CliqueGraph graph;
      fillG3(graph);

      CHECK(!graph.hasRunningIntersection());

      gum::NodeSet result;

      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addToClique(2, 5));
      // check a new separator ??
      CHECK((graph.clique(2).size()) == (static_cast< gum::Size >(4)));

      CHECK(graph.hasRunningIntersection());
    }

    static void testAddToCliqueByID() {
      gum::CliqueGraph graph;
      fillG3(graph);

      CHECK(!graph.hasRunningIntersection());

      gum::NodeSet result;

      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.addToClique(2, 5));
      CHECK((graph.clique(2).size()) == (static_cast< gum::Size >(4)));

      CHECK(graph.hasRunningIntersection());
    }

    static void testEraseFromClique() {
      gum::CliqueGraph graph;
      fillG3(graph);

      CHECK(!graph.hasRunningIntersection());

      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.eraseFromClique(3, 5));

      CHECK(graph.hasRunningIntersection());

      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.eraseFromClique(3, 5));
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.eraseFromClique(1, 6));
      /* No Comment on this in the documentation, but can be expected.*/
      GUM_CHECK_ASSERT_THROWS_NOTHING(graph.eraseFromClique(4, 5));
      /* No Comment on this in the documentation, but can be expected.*/
      CHECK_THROWS(graph.eraseFromClique(42, 1));
    }

    static void testGetPath() {
      gum::CliqueGraph graph;
      fillG2(graph);

      std::vector< gum::NodeId > vec;

      try {
        vec = graph.undirectedPath(1, 4);
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }

      GUM_CHECK_ASSERT_THROWS_NOTHING(vec = graph.undirectedPath(1, 4));

      /** Paths from A to D goes threw 3 or 4 cliques.*/
      CHECK(((vec.size() == 3) || (vec.size() == 4)));

      /* From A, must pass throught B or C. */

      if (vec[1] == 2) {
        graph.eraseEdge(gum::Edge(1, 2));
      } else if (vec[1] == 3) {
        /** Paths from A to D  still goes threw 3 or 4 cliques.*/
        CHECK(((vec.size() == 3) || (vec.size() == 4)));

        /** Deleting an edge to force A -> B -> C -> D or A -> C -> B -> D.  */

        if (vec[1] == 2) {
          graph.eraseEdge(gum::Edge(2, 4));
        } else if (vec[1] == 3) {
          graph.eraseEdge(gum::Edge(3, 4));
        }

        GUM_CHECK_ASSERT_THROWS_NOTHING(vec = graph.undirectedPath(1, 4));

        /** Now can only have 3 nodes in the path. */
        CHECK((vec.size()) == (static_cast< gum::Size >(3)));

        graph.eraseEdge(gum::Edge(2, 3));

        CHECK_NOTHROW(vec = graph.undirectedPath(1, 4));
      }
    }

    static void testCopyOperator() {
      gum::CliqueGraph g1, g2, g3;
      fillG1(g1);
      fillG2(g2);
      fillG3(g3);

      gum::CliqueGraph copy1, copy2, copy3;

      GUM_CHECK_ASSERT_THROWS_NOTHING(copy1 = g1);
      GUM_CHECK_ASSERT_THROWS_NOTHING(copy2 = g2);
      GUM_CHECK_ASSERT_THROWS_NOTHING(copy3 = g3);

      CHECK((g1) == (copy1));
      CHECK((g2) == (copy2));
      CHECK((g3) == (copy3));

      copy1.clear();
      copy2.clearEdges();

      CHECK((g1) != (copy1));
      CHECK((g2) != (copy2));
    }

    static void testRunningIntProp() {
      gum::CliqueGraph g;

      gum::NodeSet n1;   // ABC
      n1.insert((gum::NodeId)1);
      n1.insert((gum::NodeId)2);
      n1.insert((gum::NodeId)3);
      g.addNodeWithId((gum::NodeId)1, n1);

      gum::NodeSet n2;   // BCD
      n2.insert((gum::NodeId)2);
      n2.insert((gum::NodeId)3);
      n2.insert((gum::NodeId)4);
      g.addNodeWithId((gum::NodeId)2, n2);

      gum::NodeSet n3;   // CDE
      n3.insert((gum::NodeId)3);
      n3.insert((gum::NodeId)4);
      n3.insert((gum::NodeId)5);
      g.addNodeWithId((gum::NodeId)3, n3);

      gum::NodeSet n4;   // ADE
      n4.insert((gum::NodeId)4);
      n4.insert((gum::NodeId)5);
      n4.insert((gum::NodeId)1);
      g.addNodeWithId((gum::NodeId)4, n4);

      gum::NodeSet n5;   // ABE
      n5.insert((gum::NodeId)5);
      n5.insert((gum::NodeId)1);
      n5.insert((gum::NodeId)2);
      g.addNodeWithId((gum::NodeId)5, n5);

      g.addEdge(1, 2);
      g.addEdge(2, 3);
      g.addEdge(3, 4);
      g.addEdge(4, 5);
      g.addEdge(5, 1);

      CHECK((g.hasRunningIntersection()) == (true));
    }

    void xxipydevtestToDot() {
      const auto bn = gum::BayesNet< double >::fastPrototype("A->B<-C->D<-E->B");
      auto       ie = gum::LazyPropagation< double >(&bn);
      const auto jt = ie.junctionTree();

      GUM_TRACE(jt->toDot())
      GUM_TRACE(jt->mapToDot(0.4, 0.15, 1.0))
    }
  };

  GUM_TEST_ACTIF(Constructor)
  GUM_TEST_ACTIF(FillG1)
  GUM_TEST_ACTIF(FillG2)
  GUM_TEST_ACTIF(FillG3)
  GUM_TEST_ACTIF(Equality)
  GUM_TEST_ACTIF(CopyConstructor)
  GUM_TEST_ACTIF(IsJoinTree)
  GUM_TEST_ACTIF(HasRunningIntersection)
  GUM_TEST_ACTIF(GetNbrEdges)
  GUM_TEST_ACTIF(ClearNodes)
  GUM_TEST_ACTIF(ClearEdges)
  GUM_TEST_ACTIF(EraseNode)
  GUM_TEST_ACTIF(EraseNodeStubborn)
  GUM_TEST_ACTIF(GetSeparatorByEdgeG1)
  GUM_TEST_ACTIF(GetSeparatorByEdgeG2)
  GUM_TEST_ACTIF(GetSeparatorByEdgeG3)
  GUM_TEST_ACTIF(EraseEdgeG1)
  GUM_TEST_ACTIF(EraseEdgeG2)
  GUM_TEST_ACTIF(EraseEdgeG3)
  GUM_TEST_ACTIF(GetClique_1)
  GUM_TEST_ACTIF(GetClique_2)
  GUM_TEST_ACTIF(GetContainerNode)
  GUM_TEST_ACTIF(SetClique_1)
  GUM_TEST_ACTIF(SetClique_2)
  GUM_TEST_ACTIF(AddToClique)
  GUM_TEST_ACTIF(AddToCliqueByID)
  GUM_TEST_ACTIF(EraseFromClique)
  GUM_TEST_ACTIF(GetPath)
  GUM_TEST_ACTIF(CopyOperator)
  GUM_TEST_ACTIF(RunningIntProp)
}   // namespace gum_tests
