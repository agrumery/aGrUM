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


#include <cstdlib>
#include <iostream>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/graphs/algorithms/triangulations/defaultTriangulation.h>
#include <agrum/base/graphs/algorithms/triangulations/incrementalTriangulation.h>

namespace gum_tests {

  class GUM_TEST_SUITE(IncrementalTriangulation) {
    public:
    GUM_ACTIVE_TEST(1) {
      gum::DefaultTriangulation     tr;
      gum::IncrementalTriangulation triang(tr);

      triang.updateTriangulation();
      TS_ASSERT_EQUALS(triang._check_(), true)

      triang.addNode(10, 10);
      TS_ASSERT_EQUALS(triang._check_(), true)

      triang.addNode(30, 10);
      TS_ASSERT_EQUALS(triang._check_(), true)

      triang.addNode(20, 10);
      triang.addNode(40, 10);
      TS_ASSERT_EQUALS(triang._check_(), true)
      const gum::CliqueGraph& JT1 = triang.junctionTree();
      TS_ASSERT_EQUALS(JT1.size(), 4U)

      triang.addEdge(10, 20);
      triang.addEdge(10, 30);
      TS_ASSERT_EQUALS(triang._check_(), true)
      const gum::CliqueGraph& JT2 = triang.junctionTree();
      TS_ASSERT_EQUALS(JT2.size(), 3U)
      TS_ASSERT_EQUALS(JT2.sizeEdges(), 1U)

      triang.eraseEdge(gum::Edge(10, 20));
      triang.updateTriangulation();
      TS_ASSERT_EQUALS(triang._check_(), true)
      triang.eraseEdge(gum::Edge(10, 30));
      TS_ASSERT_EQUALS(triang._check_(), true)

      gum::NodeSet c1;
      c1 << 10 << 20 << 30 << 40;
      createClique(triang, c1);
      TS_ASSERT_EQUALS(triang._check_(), true)

      triang.eraseEdge(gum::Edge(10, 30));
      TS_ASSERT_EQUALS(triang._check_(), true)
      triang.eraseEdge(gum::Edge(10, 20));
      triang.updateTriangulation();
      TS_ASSERT_EQUALS(triang._check_(), true)

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
      TS_ASSERT_EQUALS(triang._check_(), true)
      createClique(triang, c3);
      triang.updateTriangulation();
      TS_ASSERT_EQUALS(triang._check_(), true)

      createClique(triang, c2);
      createClique(triang, c5);
      TS_ASSERT_EQUALS(triang._check_(), true)

      triang.eraseNode(40);
      triang.updateTriangulation();
      triang.updateTriangulation();
      TS_ASSERT_EQUALS(triang._check_(), true)

      triang.addNode(40, 10);
      createClique(triang, c1);
      createClique(triang, c2);
      createClique(triang, c3);
      createClique(triang, c4);
      createClique(triang, c5);
      triang.updateTriangulation();
      triang.updateTriangulation();
      TS_ASSERT_EQUALS(triang._check_(), true)
    }   // namespace gum_tests

    GUM_ACTIVE_TEST(PaperError) {
      // check whether Incremental triangulations fix a bug in the paper by
      // Flores et al.
      gum::DefaultTriangulation     tr;
      gum::IncrementalTriangulation triang(tr);

      triang.addNode(0, 10);
      triang.updateTriangulation();

      if (!triang._check_()) return;

      triang.addNode(1, 10);

      triang.updateTriangulation();

      if (!triang._check_()) return;

      triang.addEdge(0, 1);

      triang.updateTriangulation();

      if (!triang._check_()) return;

      triang.addNode(2, 10);

      triang.updateTriangulation();

      if (!triang._check_()) return;

      triang.addEdge(0, 2);

      triang.updateTriangulation();

      if (!triang._check_()) return;

      triang.addEdge(2, 1);

      triang.eraseNode(0);

      triang.updateTriangulation();

      if (!triang._check_()) return;
    }

    GUM_ACTIVE_TEST(Random) {
      for (gum::Idx k = 1; k < 100; ++k) {
        gum::DefaultTriangulation     tr;
        gum::IncrementalTriangulation triang(tr);

        for (gum::Idx i = 0; i < 100; ++i) {
          gum::Size nb = (gum::Size)(((float)rand() / float(RAND_MAX)) * 100);

          // ===================================================
          // remove some node
          if ((nb >= 10) && (nb < 30)) {
            triang.addNode(triang.graph().bound(), 10);
          } else if (nb < 50) {
            gum::NodeId id1
                = (gum::NodeId)(((float)rand() / float(RAND_MAX)) * triang.graph().bound());
            gum::NodeId id2
                = (gum::NodeId)(((float)rand() / float(RAND_MAX)) * triang.graph().bound());
            triang.eraseEdge(gum::Edge(id1, id2));
          } else {
            gum::NodeId id1
                = (gum::NodeId)(((float)rand() / float(RAND_MAX)) * triang.graph().bound());
            gum::NodeId id2
                = (gum::NodeId)(((float)rand() / float(RAND_MAX)) * triang.graph().bound());
            triang.addEdge(id1, id2);
          }

          nb = (gum::Size)(((float)rand() / float(RAND_MAX)) * 20);

          if (nb <= 3) {
            triang.updateTriangulation();
            TS_ASSERT_EQUALS(triang._check_(), true)
          }
        }

        triang.clear();
      }
    }

    GUM_ACTIVE_TEST(ElimSeq) {
      gum::DefaultTriangulation     tr;
      gum::IncrementalTriangulation triang(tr);

      triang.updateTriangulation();
      triang.addNode(10, 10);
      triang.addNode(30, 10);
      triang.addNode(20, 10);
      triang.addNode(40, 10);

      const std::vector< gum::NodeId >& elim1 = triang.eliminationOrder();
      TS_ASSERT_EQUALS(elim1.size(), 4U)
      TS_ASSERT_EQUALS(elim1[0], 10U)
      TS_ASSERT_EQUALS(elim1[1], 30U)
      TS_ASSERT_EQUALS(elim1[2], 20U)
      TS_ASSERT_EQUALS(elim1[3], 40U)

      const gum::CliqueGraph& JT1 = triang.junctionTree();
      TS_ASSERT_EQUALS(JT1.size(), 4U)

      triang.addEdge(10, 20);
      triang.addEdge(10, 30);
      triang.eraseEdge(gum::Edge(10, 20));
      triang.updateTriangulation();
      TS_ASSERT_EQUALS(triang._check_(), true)

      triang.eraseEdge(gum::Edge(10, 30));
      TS_ASSERT_EQUALS(triang._check_(), true)

      gum::NodeSet c1;
      c1 << 10 << 20 << 30 << 40;
      createClique(triang, c1);
      TS_ASSERT_EQUALS(triang._check_(), true)

      triang.eraseEdge(gum::Edge(10, 30));
      TS_ASSERT_EQUALS(triang._check_(), true)
      triang.eraseEdge(gum::Edge(10, 20));
      triang.updateTriangulation();
      TS_ASSERT_EQUALS(triang._check_(), true)

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
      TS_ASSERT_EQUALS(triang._check_(), true)

      const std::vector< gum::NodeId >& elim2 = triang.eliminationOrder();
      TS_ASSERT_EQUALS(elim2.size(), 8U)
      TS_ASSERT_EQUALS(elim2[0], 60U)
      TS_ASSERT_EQUALS(elim2[1], 80U)
      TS_ASSERT_EQUALS(elim2[2], 30U)
      TS_ASSERT_EQUALS(elim2[3], 10U)
      TS_ASSERT_EQUALS(elim2[4], 40U)
      TS_ASSERT_EQUALS(elim2[5], 50U)
      TS_ASSERT_EQUALS(elim2[6], 70U)
      TS_ASSERT_EQUALS(elim2[7], 20U)

      triang.eraseNode(40);
      triang.updateTriangulation();
      triang.updateTriangulation();
      TS_ASSERT_EQUALS(triang._check_(), true)

      const std::vector< gum::NodeId >& elim3 = triang.eliminationOrder();
      TS_ASSERT_EQUALS(elim3.size(), 7U)
      TS_ASSERT_EQUALS(elim3[0], 60U)
      TS_ASSERT_EQUALS(elim3[1], 50U)
      TS_ASSERT_EQUALS(elim3[2], 70U)
      TS_ASSERT_EQUALS(elim3[3], 80U)
      TS_ASSERT_EQUALS(elim3[4], 30U)
      TS_ASSERT_EQUALS(elim3[5], 20U)
      TS_ASSERT_EQUALS(elim3[6], 10U)

      triang.addNode(40, 10);
      createClique(triang, c1);
      createClique(triang, c2);
      createClique(triang, c3);
      createClique(triang, c4);
      createClique(triang, c5);
      triang.updateTriangulation();
      triang.updateTriangulation();
      TS_ASSERT_EQUALS(triang._check_(), true)

      const std::vector< gum::NodeId >& elim4 = triang.eliminationOrder();
      TS_ASSERT_EQUALS(elim4.size(), 8U)
      TS_ASSERT_EQUALS(elim4[0], 60U)
      TS_ASSERT((elim4[1] == 80U) || (elim4[1] == 70U))
      TS_ASSERT((elim4[2] == 30U) || (elim4[2] == 80U))
      TS_ASSERT((elim4[3] == 10U) || (elim4[3] == 30U))
      TS_ASSERT((elim4[4] == 70U) || (elim4[4] == 10U))
      TS_ASSERT_EQUALS(elim4[5], 50U)
      TS_ASSERT_EQUALS(elim4[6], 40U)
      TS_ASSERT_EQUALS(elim4[7], 20U)

      TS_ASSERT_EQUALS(triang.eliminationOrder(60U), 0U)
      TS_ASSERT((triang.eliminationOrder(80U) == 1U) || (triang.eliminationOrder(80U) == 2U))
      TS_ASSERT((triang.eliminationOrder(30U) == 2U) || (triang.eliminationOrder(30U) == 3U))
      TS_ASSERT((triang.eliminationOrder(10U) == 3U) || (triang.eliminationOrder(10U) == 4U))
      TS_ASSERT((triang.eliminationOrder(70U) == 4U) || (triang.eliminationOrder(70U) == 1U))
      TS_ASSERT_EQUALS(triang.eliminationOrder(50U), 5U)
      TS_ASSERT_EQUALS(triang.eliminationOrder(40U), 6U)
      TS_ASSERT_EQUALS(triang.eliminationOrder(20U), 7U)

      TS_ASSERT_EQUALS(triang.createdJunctionTreeCliques().size(), 8U)
      TS_ASSERT_EQUALS(triang.createdJunctionTreeClique(10), 2U)
      TS_ASSERT_EQUALS(triang.createdJunctionTreeClique(20), 8U)
      TS_ASSERT_EQUALS(triang.createdJunctionTreeClique(30), 2U)
      TS_ASSERT_EQUALS(triang.createdJunctionTreeClique(40), 8U)
      TS_ASSERT_EQUALS(triang.createdJunctionTreeClique(50), 8U)
      TS_ASSERT_EQUALS(triang.createdJunctionTreeClique(60), 9U)
      TS_ASSERT_EQUALS(triang.createdJunctionTreeClique(70), 5U)
      TS_ASSERT_EQUALS(triang.createdJunctionTreeClique(80), 6U)
    }

    private:
    void createClique(gum::IncrementalTriangulation& triang, const gum::NodeSet& clique) {
      for (auto iter = clique.begin(); iter != clique.end(); ++iter) {
        auto iter2 = iter;

        for (++iter2; iter2 != clique.end(); ++iter2) {
          try {
            triang.addEdge(*iter, *iter2);
          } catch (gum::DuplicateElement&) {}
        }
      }
    }
  };
}   // namespace gum_tests
