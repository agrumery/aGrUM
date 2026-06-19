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

#include <agrum/base/graphs/algorithms/MeekRules.h>
#include <agrum/base/graphs/PDAG.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  MeekRules
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct MeekRulesTestSuite {
    public:
    static void test_cycle_PDAG() {
      gum::PDAG pdag;
      for (unsigned int i = 0; i < 6; ++i) {
        pdag.addNodeWithId(i);
      }
      GUM_CHECK_ASSERT_THROWS_NOTHING(pdag.addArc(0, 1));
      GUM_CHECK_ASSERT_THROWS_NOTHING(pdag.addArc(1, 2));
      GUM_CHECK_ASSERT_THROWS_NOTHING(pdag.addEdge(2, 3));
      GUM_CHECK_ASSERT_THROWS_NOTHING(pdag.addEdge(3, 5));
      GUM_CHECK_ASSERT_THROWS_NOTHING(pdag.addEdge(5, 4));

      gum::MeekRules mr;

      const auto res_cpdag = mr.propagateToCPDAG(pdag);
      const auto res_dag   = mr.propagateToDAG(pdag);

      CHECK(res_cpdag.existsArc(2, 3));
      CHECK_EQ(res_cpdag.edges().size(), 0u);
    }

    static void testcritereMinParents1() {
      gum::MixedGraph graph;
      gum::MixedGraph graph2;
      for (unsigned int i = 0; i < 7; ++i) {
        graph.addNodeWithId(i);
      }
      graph.addArc(0, 3);
      graph.addArc(1, 3);
      graph.addArc(2, 3);
      graph.addArc(5, 4);
      graph.addArc(6, 4);

      graph.addEdge(5, 4);

      gum::MeekRules mr;

      const auto res_cpdag = mr.propagateToCPDAG(graph);

      CHECK(res_cpdag.existsArc(5, 4));
    }

    // Mixed graph from https://arnauddhaene.github.io/tutorials/network-analytics/pc.html#extra
    // Initial v-structures: 4->6, 1->6, 2->7, 4->7
    // Undirected: 6-3, 3-0, 3-7, 3-2, 4-3, 0-5, 4-0, 0-2, 4-5
    // R1 fires: 6->3, 3->0, 3->7, 3->2, 0->5, 0->2
    // R2 fires: 4->3 (via 4->6->3), 4->0 (via 4->3->0), 4->5 (via 4->0->5)
    // NB: MixedGraph used instead of PDAG because 2->7, 3-7 and 3-2 simultaneously
    //     would violate aGrUM's hasMixedReallyOrientedPath PDAG invariant.
    static void testMeekRulesR1R2FromPDAG() {
      gum::MixedGraph mg;
      for (unsigned int i = 0; i < 8; ++i)
        mg.addNodeWithId(i);

      mg.addArc(4, 6);
      mg.addArc(1, 6);
      mg.addArc(2, 7);
      mg.addArc(4, 7);

      mg.addEdge(6, 3);
      mg.addEdge(3, 0);
      mg.addEdge(3, 7);
      mg.addEdge(3, 2);
      mg.addEdge(4, 3);
      mg.addEdge(0, 5);
      mg.addEdge(4, 0);
      mg.addEdge(0, 2);
      mg.addEdge(4, 5);

      gum::MeekRules mr;
      const auto dag = mr.propagateToDAG(mg);

      CHECK_EQ(dag.arcs().size(), 13u);

      CHECK(dag.existsArc(4, 6));
      CHECK(dag.existsArc(1, 6));
      CHECK(dag.existsArc(2, 7));
      CHECK(dag.existsArc(4, 7));

      CHECK(dag.existsArc(6, 3));
      CHECK(dag.existsArc(3, 0));
      CHECK(dag.existsArc(3, 7));
      CHECK(dag.existsArc(3, 2));
      CHECK(dag.existsArc(0, 5));
      CHECK(dag.existsArc(0, 2));

      CHECK(dag.existsArc(4, 3));
      CHECK(dag.existsArc(4, 0));
      CHECK(dag.existsArc(4, 5));

      CHECK(mr.choices().empty());
    }

    static void testMeekRulesR1R2FromPDAG_cpdag() {
      gum::MixedGraph mg;
      for (unsigned int i = 0; i < 8; ++i)
        mg.addNodeWithId(i);

      mg.addArc(4, 6);
      mg.addArc(1, 6);
      mg.addArc(2, 7);
      mg.addArc(4, 7);

      mg.addEdge(6, 3);
      mg.addEdge(3, 0);
      mg.addEdge(3, 7);
      mg.addEdge(3, 2);
      mg.addEdge(4, 3);
      mg.addEdge(0, 5);
      mg.addEdge(4, 0);
      mg.addEdge(0, 2);
      mg.addEdge(4, 5);

      gum::MeekRules mr;
      const auto cpdag = mr.propagateToCPDAG(mg);

      CHECK_EQ(cpdag.arcs().size(), 13u);
      CHECK_EQ(cpdag.edges().size(), 0u);

      CHECK(cpdag.existsArc(4, 6));
      CHECK(cpdag.existsArc(1, 6));
      CHECK(cpdag.existsArc(2, 7));
      CHECK(cpdag.existsArc(4, 7));

      CHECK(cpdag.existsArc(6, 3));
      CHECK(cpdag.existsArc(3, 0));
      CHECK(cpdag.existsArc(3, 7));
      CHECK(cpdag.existsArc(3, 2));
      CHECK(cpdag.existsArc(0, 5));
      CHECK(cpdag.existsArc(0, 2));

      CHECK(cpdag.existsArc(4, 3));
      CHECK(cpdag.existsArc(4, 0));
      CHECK(cpdag.existsArc(4, 5));
    }
    // R1 isolated: 0->1-2, 0 not adjacent to 2.
    // R1 fires: parents(1)={0} not in boundary(2)={1} -> orient 1->2.
    static void testMeekRuleR1_isolated() {
      gum::MixedGraph mg;
      for (unsigned int i = 0; i < 3; ++i)
        mg.addNodeWithId(i);
      mg.addArc(0, 1);
      mg.addEdge(1, 2);

      gum::MeekRules mr;
      const auto     cpdag = mr.propagateToCPDAG(mg);

      CHECK_EQ(cpdag.arcs().size(), 2u);
      CHECK_EQ(cpdag.edges().size(), 0u);
      CHECK(cpdag.existsArc(0, 1));
      CHECK(cpdag.existsArc(1, 2));
      CHECK(mr.choices().empty());
    }

    // R2 isolated: 0->1->2, edge 0-2.
    // R2 fires: directed path 0->1->2 exists -> orient 0->2.
    static void testMeekRuleR2_isolated() {
      gum::MixedGraph mg;
      for (unsigned int i = 0; i < 3; ++i)
        mg.addNodeWithId(i);
      mg.addArc(0, 1);
      mg.addArc(1, 2);
      mg.addEdge(0, 2);

      gum::MeekRules mr;
      const auto     cpdag = mr.propagateToCPDAG(mg);

      CHECK_EQ(cpdag.arcs().size(), 3u);
      CHECK_EQ(cpdag.edges().size(), 0u);
      CHECK(cpdag.existsArc(0, 1));
      CHECK(cpdag.existsArc(1, 2));
      CHECK(cpdag.existsArc(0, 2));
      CHECK(mr.choices().empty());
    }

    // R3 isolated: arcs 2->1, 3->1; edges 0-1, 0-2, 0-3; no edge between 2 and 3.
    // R3 fires: parents {2,3} of 1 both reachable via mixed path from 0,
    //           and 2 non-adjacent to 3 -> orient 0->1.
    // Edges 0-2 and 0-3 cannot be oriented by any rule -> remain undirected.
    static void testMeekRuleR3_isolated() {
      gum::MixedGraph mg;
      for (unsigned int i = 0; i < 4; ++i)
        mg.addNodeWithId(i);
      mg.addArc(2, 1);
      mg.addArc(3, 1);
      mg.addEdge(0, 1);
      mg.addEdge(0, 2);
      mg.addEdge(0, 3);

      gum::MeekRules mr;
      const auto     cpdag = mr.propagateToCPDAG(mg);

      CHECK(cpdag.existsArc(0, 1));
      CHECK(cpdag.existsArc(2, 1));
      CHECK(cpdag.existsArc(3, 1));
      CHECK_EQ(cpdag.arcs().size(), 3u);
      CHECK(cpdag.existsEdge(0, 2));
      CHECK(cpdag.existsEdge(0, 3));
      CHECK_EQ(cpdag.edges().size(), 2u);
      CHECK(mr.choices().empty());
    }

    // R3 blocked: same as R3 isolated but with edge 2-3 (parents become adjacent).
    // R3 requires non-adjacent parents -> does not fire.
    // R1/R2/R4 also cannot orient 0-1 -> edge stays undirected.
    static void testMeekRuleR3_nofire_adjacent() {
      gum::MixedGraph mg;
      for (unsigned int i = 0; i < 4; ++i)
        mg.addNodeWithId(i);
      mg.addArc(2, 1);
      mg.addArc(3, 1);
      mg.addEdge(0, 1);
      mg.addEdge(0, 2);
      mg.addEdge(0, 3);
      mg.addEdge(2, 3);   // 2 and 3 adjacent -> R3 blocked

      gum::MeekRules mr;
      const auto     cpdag = mr.propagateToCPDAG(mg);

      CHECK(cpdag.existsEdge(0, 1));
      CHECK(cpdag.existsArc(2, 1));
      CHECK(cpdag.existsArc(3, 1));
      CHECK_EQ(cpdag.arcs().size(), 2u);
      CHECK_EQ(cpdag.edges().size(), 4u);   // 0-1, 0-2, 0-3, 2-3
    }

    // R1 cascade then R2: v-structure 0->2<-1, chain 2-3-4, shortcut edge 0-4.
    // R1 fires: 2->3 (parent 0 of 2, not adj 3), then 3->4 (parent 2 of 3, not adj 4).
    // R2 fires: 0->4 (directed path 0->2->3->4 now exists).
    static void testMeekRulesR1Cascade_R2() {
      gum::MixedGraph mg;
      for (unsigned int i = 0; i < 5; ++i)
        mg.addNodeWithId(i);
      mg.addArc(0, 2);
      mg.addArc(1, 2);
      mg.addEdge(2, 3);
      mg.addEdge(3, 4);
      mg.addEdge(0, 4);

      gum::MeekRules mr;
      const auto     cpdag = mr.propagateToCPDAG(mg);

      CHECK_EQ(cpdag.arcs().size(), 5u);
      CHECK_EQ(cpdag.edges().size(), 0u);
      CHECK(cpdag.existsArc(0, 2));
      CHECK(cpdag.existsArc(1, 2));
      CHECK(cpdag.existsArc(2, 3));
      CHECK(cpdag.existsArc(3, 4));
      CHECK(cpdag.existsArc(0, 4));
      CHECK(mr.choices().empty());
    }

    // R3 then R1 then R2: arcs 3->1, 4->1 (non-adjacent); edges 0-1,0-2,0-3,0-4,1-2.
    // R1 fires first: 1->2 (parent 3 of 1, not adj 2).
    // R3 fires: 0->1 (parents {3,4} of 1, mixed paths 0-3 and 0-4, 3 not adj 4).
    // R2 fires: 0->2 (directed path 0->1->2 now exists).
    // Edges 0-3 and 0-4 remain undirected (no rule can orient them).
    static void testMeekRulesR1_R3_R2() {
      gum::MixedGraph mg;
      for (unsigned int i = 0; i < 5; ++i)
        mg.addNodeWithId(i);
      mg.addArc(3, 1);
      mg.addArc(4, 1);
      mg.addEdge(0, 1);
      mg.addEdge(0, 2);
      mg.addEdge(0, 3);
      mg.addEdge(0, 4);
      mg.addEdge(1, 2);

      gum::MeekRules mr;
      const auto     cpdag = mr.propagateToCPDAG(mg);

      CHECK_EQ(cpdag.arcs().size(), 5u);
      CHECK_EQ(cpdag.edges().size(), 2u);
      CHECK(cpdag.existsArc(3, 1));
      CHECK(cpdag.existsArc(4, 1));
      CHECK(cpdag.existsArc(0, 1));
      CHECK(cpdag.existsArc(1, 2));
      CHECK(cpdag.existsArc(0, 2));
      CHECK(cpdag.existsEdge(0, 3));
      CHECK(cpdag.existsEdge(0, 4));
      CHECK(mr.choices().empty());
    }

    // R4 isolated with a longer discriminating path: chain 0->2->3->4, edges 4-1, 0-1.
    // R1 fires: 4->1 (parent 3 of 4, not adj 1).
    // R4 fires: 0->1 (parent 4 of 1, mixed path 0->2->3->4, 4 not adj 0).
    // Cycle check (path 0->2->3->4->1 exists) blocks the conflicting 1->0 direction.
    static void testMeekRuleR4_isolated() {
      gum::MixedGraph mg;
      for (unsigned int i = 0; i < 6; ++i)
        mg.addNodeWithId(i);
      mg.addArc(0, 2);
      mg.addArc(2, 3);
      mg.addArc(3, 4);
      mg.addEdge(4, 1);
      mg.addEdge(0, 1);

      gum::MeekRules mr;
      const auto     cpdag = mr.propagateToCPDAG(mg);

      CHECK_EQ(cpdag.arcs().size(), 5u);
      CHECK_EQ(cpdag.edges().size(), 0u);
      CHECK(cpdag.existsArc(0, 2));
      CHECK(cpdag.existsArc(2, 3));
      CHECK(cpdag.existsArc(3, 4));
      CHECK(cpdag.existsArc(4, 1));
      CHECK(cpdag.existsArc(0, 1));
      CHECK(mr.choices().empty());
    }

    // Variant: v-structure at 0 instead of 1: arcs 2->0, 3->0 (2 not adj 3);
    // edges 1-0, 1-2, 1-3, 0-4, 1-4.
    // R3 fires: 1->0 (parents {2,3} of 0, direct edges 1-2 and 1-3, 2 not adj 3).
    // R1 fires: 0->4 (parent 2 of 0, not adj 4).
    // R2 fires: 1->4 (directed path 1->0->4 now exists).
    // Edges 1-2 and 1-3 remain undirected.
    static void testMeekRulesR1_R3_R2_v2() {
      gum::MixedGraph mg;
      for (unsigned int i = 0; i < 5; ++i)
        mg.addNodeWithId(i);
      mg.addArc(2, 0);
      mg.addArc(3, 0);
      mg.addEdge(1, 0);
      mg.addEdge(1, 2);
      mg.addEdge(1, 3);
      mg.addEdge(0, 4);
      mg.addEdge(1, 4);

      gum::MeekRules mr;
      const auto     cpdag = mr.propagateToCPDAG(mg);

      CHECK_EQ(cpdag.arcs().size(), 5u);
      CHECK_EQ(cpdag.edges().size(), 2u);
      CHECK(cpdag.existsArc(2, 0));
      CHECK(cpdag.existsArc(3, 0));
      CHECK(cpdag.existsArc(1, 0));
      CHECK(cpdag.existsArc(0, 4));
      CHECK(cpdag.existsArc(1, 4));
      CHECK(cpdag.existsEdge(1, 2));
      CHECK(cpdag.existsEdge(1, 3));
      CHECK(mr.choices().empty());
    }

    // R4: purely directed path 0->2->3, undirected neighbor 3-1, xi=0 not adjacent to 3.
    // R1/R2/R3 cannot orient 0-1 (node 1 has no parents); R4 fires (xk=3, path 0->2->3, 3-1 edge).
    // Then R1 fires: parent 2 of 3 not adjacent to 1 -> 3->1.
    static void testMeekRuleR4() {
      gum::MixedGraph mg;
      for (unsigned int i = 0; i < 4; ++i)
        mg.addNodeWithId(i);

      mg.addArc(0, 2);
      mg.addArc(2, 3);
      mg.addEdge(3, 1);
      mg.addEdge(0, 1);

      gum::MeekRules mr;
      const auto     cpdag = mr.propagateToCPDAG(mg);

      CHECK(cpdag.existsArc(0, 2));
      CHECK(cpdag.existsArc(2, 3));
      CHECK(cpdag.existsArc(0, 1));
      CHECK(cpdag.existsArc(3, 1));
      CHECK_EQ(cpdag.arcs().size(), 4u);
      CHECK_EQ(cpdag.edges().size(), 0u);
      CHECK(mr.choices().empty());
    }
  };   // MeekrulesTestSuite

  GUM_TEST_ACTIF(_cycle_PDAG)
  GUM_TEST_ACTIF(critereMinParents1)
  GUM_TEST_ACTIF(MeekRuleR1_isolated)
  GUM_TEST_ACTIF(MeekRuleR2_isolated)
  GUM_TEST_ACTIF(MeekRuleR3_isolated)
  GUM_TEST_ACTIF(MeekRuleR3_nofire_adjacent)
  GUM_TEST_ACTIF(MeekRulesR1Cascade_R2)
  GUM_TEST_ACTIF(MeekRulesR1_R3_R2)
  GUM_TEST_ACTIF(MeekRulesR1_R3_R2_v2)
  GUM_TEST_ACTIF(MeekRuleR4_isolated)
  GUM_TEST_ACTIF(MeekRulesR1R2FromPDAG)
  GUM_TEST_ACTIF(MeekRulesR1R2FromPDAG_cpdag)
  GUM_TEST_ACTIF(MeekRuleR4)
}   // namespace gum_tests
