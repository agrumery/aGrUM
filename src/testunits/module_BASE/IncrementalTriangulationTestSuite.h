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

#include <cstdlib>
#include <iostream>
#include <string>

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/graphs/triangulations/incrementalTriangulation.h>
#include <agrum/graphs/triangulations/defaultTriangulation.h>

namespace gum_tests {

  class IncrementalTriangulationTestSuite : public CxxTest::TestSuite {
    public:
    void test1() {
      gum::DefaultTriangulation tr;
      gum::IncrementalTriangulation triang(tr);

      triang.updateTriangulation();
      TS_ASSERT_EQUALS(triang.__check(), true);

      triang.addNode(10, 10);
      TS_ASSERT_EQUALS(triang.__check(), true);

      triang.addNode(30, 10);
      TS_ASSERT_EQUALS(triang.__check(), true);

      triang.addNode(20, 10);
      triang.addNode(40, 10);
      TS_ASSERT_EQUALS(triang.__check(), true);
      const gum::CliqueGraph &JT1 = triang.junctionTree();
      TS_ASSERT_EQUALS(JT1.size(), 4U);

      triang.addEdge(10, 20);
      triang.addEdge(10, 30);
      TS_ASSERT_EQUALS(triang.__check(), true);
      const gum::CliqueGraph &JT2 = triang.junctionTree();
      TS_ASSERT_EQUALS(JT2.size(), 3U);
      TS_ASSERT_EQUALS(JT2.sizeEdges(), 1U);

      triang.eraseEdge(gum::Edge(10, 20));
      triang.updateTriangulation();
      TS_ASSERT_EQUALS(triang.__check(), true);
      triang.eraseEdge(gum::Edge(10, 30));
      TS_ASSERT_EQUALS(triang.__check(), true);

      gum::NodeSet c1;
      c1 << 10 << 20 << 30 << 40;
      createClique(triang, c1);
      TS_ASSERT_EQUALS(triang.__check(), true);

      triang.eraseEdge(gum::Edge(10, 30));
      TS_ASSERT_EQUALS(triang.__check(), true);
      triang.eraseEdge(gum::Edge(10, 20));
      triang.updateTriangulation();
      TS_ASSERT_EQUALS(triang.__check(), true);

      triang.addEdge(10, 20);
      triang.addEdge(10, 30);
      triang.addNode(50, 10);
      triang.addNode(80, 10);
      triang.addNode(60, 10);
      triang.addNode(70, 10);

      gum::NodeSet c2, c3, c4, c5;
      c2 << 20 << 40 << 50;
      c3 << 40 << 50 << 60;
      c4 << 20 << 50 << 70;
      c5 << 10 << 30 << 80;

      createClique(triang, c4);
      triang.updateTriangulation();
      TS_ASSERT_EQUALS(triang.__check(), true);
      createClique(triang, c3);
      triang.updateTriangulation();
      TS_ASSERT_EQUALS(triang.__check(), true);

      createClique(triang, c2);
      createClique(triang, c5);
      TS_ASSERT_EQUALS(triang.__check(), true);

      triang.eraseNode(40);
      triang.updateTriangulation();
      triang.updateTriangulation();
      TS_ASSERT_EQUALS(triang.__check(), true);

      triang.addNode(40, 10);
      createClique(triang, c1);
      createClique(triang, c2);
      createClique(triang, c3);
      createClique(triang, c4);
      createClique(triang, c5);
      triang.updateTriangulation();
      triang.updateTriangulation();
      TS_ASSERT_EQUALS(triang.__check(), true);
    }

    void testPaperError() {
      // check whether Incremental triangulations fix a bug in the paper by
      // Flores et al.
      gum::DefaultTriangulation tr;
      gum::IncrementalTriangulation triang(tr);

      triang.addNode(0, 10);
      triang.updateTriangulation();

      if (!triang.__check())
        return;

      triang.addNode(1, 10);

      triang.updateTriangulation();

      if (!triang.__check())
        return;

      triang.addEdge(0, 1);

      triang.updateTriangulation();

      if (!triang.__check())
        return;

      triang.addNode(2, 10);

      triang.updateTriangulation();

      if (!triang.__check())
        return;

      triang.addEdge(0, 2);

      triang.updateTriangulation();

      if (!triang.__check())
        return;

      triang.addEdge(2, 1);

      triang.eraseNode(0);

      triang.updateTriangulation();

      if (!triang.__check())
        return;
    }

    void testRandom() {
      for (unsigned int k = 1; k < 100; ++k) {
        // std::cerr << "<<<<<<<<<<<<<<<<<< k = "<< k << std::endl;

        gum::DefaultTriangulation tr;
        gum::IncrementalTriangulation triang(tr);

        for (unsigned int i = 0; i < 100; ++i) {
          int nb = (int)(((float)rand() / RAND_MAX) * 100);

          // ===================================================
          // remove some node

          if (nb < 10) {
            continue;
            const gum::UndiGraph &graph = triang.graph();
            nb = (int)(((float)rand() / RAND_MAX) * graph.bound());
            // std::cerr << "remove node " << nb << std::endl;
            triang.eraseNode(nb);
          }

          // ===================================================
          // add a new node
          else if (nb < 30) {
            // std::cerr << "create node " << triang.graph().bound() << std::endl;
            triang.addNode(triang.graph().bound(), 10);
          }

          // ===================================================
          // remove some edge
          else if (nb < 50) {
            unsigned int nb1 =
                (int)(((float)rand() / RAND_MAX) * triang.graph().bound());
            unsigned int nb2 =
                (int)(((float)rand() / RAND_MAX) * triang.graph().bound());
            // std::cerr << "remove edge (" << nb1 << "," << nb2 << ")" << std::endl;
            triang.eraseEdge(gum::Edge(nb1, nb2));
          }

          // ===================================================
          // add a new edge
          else {
            unsigned int nb1 =
                (int)(((float)rand() / RAND_MAX) * triang.graph().bound());
            unsigned int nb2 =
                (int)(((float)rand() / RAND_MAX) * triang.graph().bound());
            // std::cerr << "create edge (" << nb1 << "," << nb2 << ")" << std::endl;
            triang.addEdge(nb1, nb2);
          }

          nb = (int)(((float)rand() / RAND_MAX) * 20);

          if (nb <= 3) {
            // std::cerr << "   --- check --- " << std::endl;
            triang.updateTriangulation();
            TS_ASSERT_EQUALS(triang.__check(), true);
          }
        }

        triang.clear();
      }
    }

    void testElimSeq() {
      gum::DefaultTriangulation tr;
      gum::IncrementalTriangulation triang(tr);

      triang.updateTriangulation();
      triang.addNode(10, 10);
      triang.addNode(30, 10);
      triang.addNode(20, 10);
      triang.addNode(40, 10);

      const std::vector<gum::NodeId> &elim1 = triang.eliminationOrder();
      TS_ASSERT_EQUALS(elim1.size(), 4U);
      TS_ASSERT_EQUALS(elim1[0], 10U);
      TS_ASSERT_EQUALS(elim1[1], 30U);
      TS_ASSERT_EQUALS(elim1[2], 20U);
      TS_ASSERT_EQUALS(elim1[3], 40U);

      const gum::CliqueGraph &JT1 = triang.junctionTree();
      TS_ASSERT_EQUALS(JT1.size(), 4U);

      triang.addEdge(10, 20);
      triang.addEdge(10, 30);
      triang.eraseEdge(gum::Edge(10, 20));
      triang.updateTriangulation();
      TS_ASSERT_EQUALS(triang.__check(), true);

      triang.eraseEdge(gum::Edge(10, 30));
      TS_ASSERT_EQUALS(triang.__check(), true);

      gum::NodeSet c1;
      c1 << 10 << 20 << 30 << 40;
      createClique(triang, c1);
      TS_ASSERT_EQUALS(triang.__check(), true);

      triang.eraseEdge(gum::Edge(10, 30));
      TS_ASSERT_EQUALS(triang.__check(), true);
      triang.eraseEdge(gum::Edge(10, 20));
      triang.updateTriangulation();
      TS_ASSERT_EQUALS(triang.__check(), true);

      triang.addEdge(10, 20);
      triang.addEdge(10, 30);
      triang.addNode(50, 10);
      triang.addNode(80, 10);
      triang.addNode(60, 10);
      triang.addNode(70, 10);

      gum::NodeSet c2, c3, c4, c5;
      c2 << 20 << 40 << 50;
      c3 << 40 << 50 << 60;
      c4 << 20 << 50 << 70;
      c5 << 10 << 30 << 80;

      createClique(triang, c4);
      triang.updateTriangulation();
      createClique(triang, c3);
      triang.updateTriangulation();
      createClique(triang, c2);
      createClique(triang, c5);
      TS_ASSERT_EQUALS(triang.__check(), true);

      const std::vector<gum::NodeId> &elim2 = triang.eliminationOrder();
      TS_ASSERT_EQUALS(elim2.size(), 8U);
      TS_ASSERT_EQUALS(elim2[0], 60U);
      TS_ASSERT_EQUALS(elim2[1], 80U);
      TS_ASSERT_EQUALS(elim2[2], 30U);
      TS_ASSERT_EQUALS(elim2[3], 10U);
      TS_ASSERT_EQUALS(elim2[4], 40U);
      TS_ASSERT_EQUALS(elim2[5], 50U);
      TS_ASSERT_EQUALS(elim2[6], 70U);
      TS_ASSERT_EQUALS(elim2[7], 20U);

      triang.eraseNode(40);
      triang.updateTriangulation();
      triang.updateTriangulation();
      TS_ASSERT_EQUALS(triang.__check(), true);

      const std::vector<gum::NodeId> &elim3 = triang.eliminationOrder();
      TS_ASSERT_EQUALS(elim3.size(), 7U);
      TS_ASSERT_EQUALS(elim3[0], 60U);
      TS_ASSERT_EQUALS(elim3[1], 50U);
      TS_ASSERT_EQUALS(elim3[2], 70U);
      TS_ASSERT_EQUALS(elim3[3], 80U);
      TS_ASSERT_EQUALS(elim3[4], 30U);
      TS_ASSERT_EQUALS(elim3[5], 20U);
      TS_ASSERT_EQUALS(elim3[6], 10U);

      triang.addNode(40, 10);
      createClique(triang, c1);
      createClique(triang, c2);
      createClique(triang, c3);
      createClique(triang, c4);
      createClique(triang, c5);
      triang.updateTriangulation();
      triang.updateTriangulation();
      TS_ASSERT_EQUALS(triang.__check(), true);

      const std::vector<gum::NodeId> &elim4 = triang.eliminationOrder();
      TS_ASSERT_EQUALS(elim4.size(), 8U);
      TS_ASSERT_EQUALS(elim4[0], 60U);
      TS_ASSERT_EQUALS(elim4[1], 80U);
      TS_ASSERT_EQUALS(elim4[2], 30U);
      TS_ASSERT_EQUALS(elim4[3], 10U);
      TS_ASSERT_EQUALS(elim4[4], 70U);
      TS_ASSERT_EQUALS(elim4[5], 50U);
      TS_ASSERT_EQUALS(elim4[6], 40U);
      TS_ASSERT_EQUALS(elim4[7], 20U);

      TS_ASSERT_EQUALS(triang.eliminationOrder(60U), 0U);
      TS_ASSERT_EQUALS(triang.eliminationOrder(80U), 1U);
      TS_ASSERT_EQUALS(triang.eliminationOrder(30U), 2U);
      TS_ASSERT_EQUALS(triang.eliminationOrder(10U), 3U);
      TS_ASSERT_EQUALS(triang.eliminationOrder(70U), 4U);
      TS_ASSERT_EQUALS(triang.eliminationOrder(50U), 5U);
      TS_ASSERT_EQUALS(triang.eliminationOrder(40U), 6U);
      TS_ASSERT_EQUALS(triang.eliminationOrder(20U), 7U);

      TS_ASSERT_EQUALS(triang.createdJunctionTreeCliques().size(), 8U);
      TS_ASSERT_EQUALS(triang.createdJunctionTreeClique(10), 2U);
      TS_ASSERT_EQUALS(triang.createdJunctionTreeClique(20), 8U);
      TS_ASSERT_EQUALS(triang.createdJunctionTreeClique(30), 2U);
      TS_ASSERT_EQUALS(triang.createdJunctionTreeClique(40), 8U);
      TS_ASSERT_EQUALS(triang.createdJunctionTreeClique(50), 8U);
      TS_ASSERT_EQUALS(triang.createdJunctionTreeClique(60), 9U);
      TS_ASSERT_EQUALS(triang.createdJunctionTreeClique(70), 5U);
      TS_ASSERT_EQUALS(triang.createdJunctionTreeClique(80), 6U);
    }

    private:
    void createClique(gum::IncrementalTriangulation &triang,
                      const gum::NodeSet &clique) {
      for (auto iter = clique.begin(); iter != clique.end(); ++iter) {
        auto iter2 = iter;

        for (++iter2; iter2 != clique.end(); ++iter2) {
          try {
            triang.addEdge(*iter, *iter2);
          } catch (gum::DuplicateElement &) {
          }
        }
      }
    }
  };
}
