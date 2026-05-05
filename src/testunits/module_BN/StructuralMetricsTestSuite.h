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

#include <agrum/base/graphs/DAG.h>
#include <agrum/base/graphs/diGraph.h>
#include <agrum/base/graphs/PDAG.h>
#include <agrum/base/graphs/undiGraph.h>
#include <agrum/BN/algorithms/structuralMetrics.h>
#include <agrum/BN/BayesNet.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  StructuralMetrics
#define GUM_CURRENT_MODULE BN

namespace gum_tests {

  struct StructuralMetricsTestSuite {
    public:
    static void test_graph() {
      gum::StructuralMetrics comp;

      gum::DiGraph   dig1, dig2;
      gum::UndiGraph undig1, undig2;
      dig1.addNodeWithId(0);
      undig1.addNodeWithId(0);

      CHECK_THROWS_AS(comp.compare(dig1, dig2), const gum::OperationNotAllowed&);
      CHECK_THROWS_AS(comp.compare(dig2, dig1), const gum::OperationNotAllowed&);
      CHECK_THROWS_AS(comp.compare(undig1, undig2), const gum::OperationNotAllowed&);
      CHECK_THROWS_AS(comp.compare(undig2, undig1), const gum::OperationNotAllowed&);

      dig2.addNodeWithId(2);
      undig2.addNodeWithId(2);
      CHECK_THROWS_AS(comp.compare(dig1, dig2), const gum::InvalidNode&);
      CHECK_THROWS_AS(comp.compare(dig2, dig1), const gum::InvalidNode&);
      CHECK_THROWS_AS(comp.compare(undig1, undig2), const gum::InvalidNode&);
      CHECK_THROWS_AS(comp.compare(undig2, undig1), const gum::InvalidNode&);

      dig1.addNodeWithId(1);
      dig1.addNodeWithId(2);
      dig1.addArc(0, 1);
      dig1.addArc(0, 2);
      dig2.addNodeWithId(1);
      dig2.addNodeWithId(0);
      dig2.addArc(0, 1);
      dig2.addArc(0, 2);
      dig2.addArc(1, 2);

      comp.compare(dig1, dig2);
      CHECK((comp.precision_skeleton()) == doctest::Approx(0.666).epsilon(1e-3));
      CHECK((comp.recall_skeleton()) == doctest::Approx(1).epsilon(1e-3));
      CHECK((comp.f_score_skeleton()) == doctest::Approx(0.8).epsilon(1e-3));
      CHECK((comp.shd_skeleton()) == 1);

      undig1.addNodeWithId(1);
      undig1.addNodeWithId(2);
      undig1.addEdge(0, 1);
      undig1.addEdge(0, 2);
      undig2.addNodeWithId(1);
      undig2.addNodeWithId(0);
      undig2.addEdge(0, 1);
      undig2.addEdge(0, 2);
      undig2.addEdge(2, 1);

      comp.compare(undig1, undig2);
      CHECK((comp.precision_skeleton()) == doctest::Approx(0.666).epsilon(1e-3));
      CHECK((comp.recall_skeleton()) == doctest::Approx(1).epsilon(1e-3));
      CHECK((comp.f_score_skeleton()) == doctest::Approx(0.8).epsilon(1e-3));
      CHECK((comp.shd_skeleton()) == 1);

      // creating complete graph
      gum::PDAG graph;
      for (gum::Size i = 0; i < 8; ++i) {
        graph.addNodeWithId(i);
        for (gum::Size j = 0; j < i; ++j) {
          graph.addEdge(j, i);
        }
      }
      // creating Asia
      gum::PDAG asia;
      for (gum::Size i = 0; i < 8; ++i) {
        asia.addNodeWithId(i);
      }
      asia.addArc(3, 4);
      asia.addArc(4, 5);
      asia.addArc(0, 1);
      asia.addArc(0, 2);
      asia.addArc(1, 5);
      asia.addArc(2, 6);
      asia.addArc(5, 7);
      asia.addArc(5, 6);

      gum::PDAG g;

      CHECK_THROWS_AS(comp.compare(asia, g), const gum::OperationNotAllowed&);

      for (gum::Size i = 0; i < 16; i += 2) {
        g.addNodeWithId(i);
      }

      CHECK_THROWS_AS(comp.compare(asia, g), const gum::InvalidNode&);
      GUM_CHECK_ASSERT_THROWS_NOTHING(comp.compare(asia, graph));

      comp.compare(asia, graph);
      CHECK((comp.precision_skeleton()) == doctest::Approx(0.2857).epsilon(1e-3));
      CHECK((comp.recall_skeleton()) == doctest::Approx(1).epsilon(1e-3));
      CHECK((comp.f_score_skeleton()) == doctest::Approx(0.4444).epsilon(1e-3));
      CHECK((comp.precision()) == doctest::Approx(0).epsilon(1e-3));
      CHECK((comp.shd_skeleton()) == 20);
      CHECK((comp.shd()) == 28);
    }   // namespace gum_tests

    static void test_bn() {
      gum::StructuralMetrics comp;

      gum::BayesNet< double > bn1, bn2;
      bn1 = bn1.fastPrototype("0->1;0->2");
      bn2 = bn2.fastPrototype("0->1->2;0->2");

      GUM_CHECK_ASSERT_THROWS_NOTHING(comp.compare(bn1, bn2));
      CHECK((comp.precision_skeleton()) == doctest::Approx(0.666).epsilon(1e-3));
      CHECK((comp.recall_skeleton()) == doctest::Approx(1).epsilon(1e-3));
      CHECK((comp.f_score_skeleton()) == doctest::Approx(0.8).epsilon(1e-3));
      CHECK((comp.shd_skeleton()) == 1);
      CHECK((comp.precision()) == doctest::Approx(0.666).epsilon(1e-3));
      CHECK((comp.recall()) == doctest::Approx(1).epsilon(1e-3));
      CHECK((comp.f_score()) == doctest::Approx(0.8).epsilon(1e-3));
      CHECK((comp.shd()) == 1);

      // creating asia
      gum::BayesNet< double > asia;
      // asia = asia.fastPrototype("3->4->5->7;0->1->5->6;0->2->6");
      // apparition order leads to : id of "3" is 0, if of "4" is 1, etc.
      // it's hard to check the shd between this graph and another created directly with the ids.
      // hence we redefine it in the correct order of apparition:
      asia = asia.fastPrototype("0->1;0->2;3->4->5->6;1->5;2->6;5->7"); // now labels match ids

      // asia = asia.fastPrototype("0->1;0->2;3->4->5->6;1->5->6;2->6");
      // bn1 = bn1.fastPrototype("3->4;7->5->6;5->4->1->0->2->6;5->1");

      gum::PDAG mg;
      for (gum::Idx i = 0; i < 8; ++i) {
        mg.addNodeWithId(i);
      }
      mg.addArc(0, 1);
      mg.addArc(1, 5);
      mg.addArc(2, 1);
      mg.addArc(7, 6);
      mg.addArc(2, 6);
      mg.addEdge(4, 5);
      mg.addEdge(4, 7);
      mg.addEdge(2, 3);

      GUM_CHECK_ASSERT_THROWS_NOTHING(comp.compare(mg, asia));

      /* Tests for asia = asia.fastPrototype("3->4->5->7;0->1->5->6;0->2->6")
      comp.compare(asia, mg);
      CHECK((comp.precision()) == doctest::Approx(0.5).epsilon(1e-3));
      CHECK((comp.recall()) == doctest::Approx(0.8).epsilon(1e-3));
      CHECK((comp.f_score()) == doctest::Approx(0.615384).epsilon(1e-3));
      CHECK((comp.precision_skeleton()) == doctest::Approx(0.875).epsilon(1e-3));
      CHECK((comp.recall_skeleton()) == doctest::Approx(0.875).epsilon(1e-3));
      CHECK((comp.f_score_skeleton()) == doctest::Approx(0.875).epsilon(1e-3));
      */

      comp.compare(asia, mg);

      CHECK((comp.precision()) == doctest::Approx(0.25).epsilon(1e-3));
      CHECK((comp.recall()) == doctest::Approx(0.333).epsilon(1e-3));
      CHECK((comp.f_score()) == doctest::Approx(0.285714).epsilon(1e-3));
      CHECK((comp.shd()) == 10);
      CHECK((comp.precision_skeleton()) == doctest::Approx(0.5).epsilon(1e-3));
      CHECK((comp.recall_skeleton()) == doctest::Approx(0.5).epsilon(1e-3));
      CHECK((comp.f_score_skeleton()) == doctest::Approx(0.5).epsilon(1e-3));
      CHECK((comp.shd_skeleton()) == 8);
    }

    static void test_identities_by_names() {
      // for every metric M we want to check that M(g1,g2) = 0 where g2=g1 regarding the variables names but not the ids.
      
      // "D" -> "A" <- "B" <- "C"
      gum::BayesNet< double > g1;
      g1 = g1.fastPrototype("D->A<-B<-C");

      // "C" -> "B" -> "A" <- "D"
      gum::BayesNet< double > g2;
      g2 = g2.fastPrototype("C->B->A<-D");

      gum::StructuralMetrics comp;
      comp.compare(g1, g2);
      CHECK(comp.precision_skeleton() == 1);
      CHECK(comp.recall_skeleton() == 1);
      CHECK(comp.f_score_skeleton() == 1);
      CHECK(comp.shd_skeleton() == 0);
      CHECK(comp.precision() == 1);
      CHECK(comp.recall() == 1);
      CHECK(comp.f_score() == 1);
      CHECK(comp.shd() == 0);
      CHECK(comp.sid(g1, g2) == 0);
    }

    static void test_sid_identity() {
      // SID(G, G) = 0 trivially
      gum::DAG g;
      for (gum::NodeId i = 0; i < 4; ++i) g.addNodeWithId(i);
      g.addArc(0, 1);
      g.addArc(0, 2);
      g.addArc(1, 3);

      gum::StructuralMetrics comp;
      CHECK(comp.sid(g, g) == 0);
    }

    static void test_sid_subgraph_zero() {
      // Proposition 7: G included in H => SID(G, H) = 0
      gum::DAG g, h;
      for (gum::NodeId i = 0; i < 3; ++i) {
        g.addNodeWithId(i);
        h.addNodeWithId(i);
      }
      g.addArc(0, 1);
      h.addArc(0, 1);
      h.addArc(0, 2);   // extra arc in h

      gum::StructuralMetrics comp;
      CHECK(comp.sid(g, h) == 0);
    }

    static void test_sid_empty_target() {
      // G: 0 -> 1, H: empty. Only the pair (1, 0) is an error.
      gum::DAG g, h;
      for (gum::NodeId i = 0; i < 2; ++i) {
        g.addNodeWithId(i);
        h.addNodeWithId(i);
      }
      g.addArc(0, 1);

      gum::StructuralMetrics comp;
      CHECK(comp.sid(g, h) == 1);
    }

    static void test_sid_reversed_arc() {
      // G: 0 -> 1, H: 1 -> 0. SHD = 1, SID = 2 (sharp bound 2(p-1) = 2).
      gum::DAG g, h;
      for (gum::NodeId i = 0; i < 2; ++i) {
        g.addNodeWithId(i);
        h.addNodeWithId(i);
      }
      g.addArc(0, 1);
      h.addArc(1, 0);

      gum::StructuralMetrics comp;
      CHECK(comp.sid(g, h) == 2);
    }

    static void test_sid_asymmetry() {
      // Asymmetry: SID(G, empty) > 0 but SID(empty, G) = 0 (Prop 7, empty included in G).
      // G: chain 0 -> 1 -> 2.
      gum::DAG g, empty;
      for (gum::NodeId i = 0; i < 3; ++i) {
        g.addNodeWithId(i);
        empty.addNodeWithId(i);
      }
      g.addArc(0, 1);
      g.addArc(1, 2);

      gum::StructuralMetrics comp;
      CHECK(comp.sid(g, empty) == 3);
      CHECK(comp.sid(empty, g) == 0);
    }

    static void test_sid_paper_figure2_h1() {
      // Peters & Bühlmann 2015, Figure 2: G vs H_1.
      // Nodes: 0=X_1, 1=X_2, 2=Y_1, 3=Y_2, 4=Y_3.
      // G: X_1 -> X_2; X_1 and X_2 are parents of each Y_j.
      gum::DAG g;
      for (gum::NodeId i = 0; i < 5; ++i) g.addNodeWithId(i);
      g.addArc(0, 1);
      for (gum::NodeId y = 2; y <= 4; ++y) {
        g.addArc(0, y);
        g.addArc(1, y);
      }

      // H_1 = G + (Y_1 -> Y_2), so G included in H_1.
      gum::DAG h1 = g;
      h1.addArc(2, 3);

      gum::StructuralMetrics comp;
      CHECK(comp.sid(g, h1) == 0);   // by Prop 7
    }

    static void test_sid_paper_figure2_h2() {
      // Peters & Bühlmann 2015, Figure 2: G vs H_2 (arc X_1 -> X_2 reversed).
      gum::DAG g;
      for (gum::NodeId i = 0; i < 5; ++i) g.addNodeWithId(i);
      g.addArc(0, 1);
      for (gum::NodeId y = 2; y <= 4; ++y) {
        g.addArc(0, y);
        g.addArc(1, y);
      }

      // H_2: same as G but X_2 -> X_1 instead of X_1 -> X_2.
      gum::DAG h2;
      for (gum::NodeId i = 0; i < 5; ++i) h2.addNodeWithId(i);
      h2.addArc(1, 0);   // reversed arc
      for (gum::NodeId y = 2; y <= 4; ++y) {
        h2.addArc(0, y);
        h2.addArc(1, y);
      }

      gum::StructuralMetrics comp;
      CHECK(comp.sid(g, h2) == 8);   // result from the paper
    }

    static void test_sid_bn_overload() {
      // Check the sid(BayesNet, BayesNet) overload: it must give the same
      // result as the DAG version by directly using bn.dag().
      // We build h2 by copying g and then modifying it, to ensure NodeIds
      // are aligned (otherwise fastPrototype permutes them according to the
      // order of label appearance).
      gum::BayesNet< double > g;
      g = g.fastPrototype("0->1;0->2;1->2;0->3;1->3;0->4;1->4");

      gum::BayesNet< double > h2 = g;
      const gum::NodeId X1 = g.idFromName("0");
      const gum::NodeId X2 = g.idFromName("1");
      h2.eraseArc(X1, X2);   // remove X_1 -> X_2
      h2.addArc(X2, X1);     // add X_2 -> X_1

      gum::StructuralMetrics comp;
      CHECK(comp.sid(g, h2) == 8);
      CHECK(comp.sid(g, g) == 0);
    }

    static void test_sum_invariant_digraph() {
      // For a DiGraph compare, tp+tn+fp+fn must equal the number of ordered
      // pairs N*(N-1), and the skeleton variants must sum to N*(N-1)/2.
      gum::DiGraph ref, test;
      for (gum::NodeId i = 0; i < 5; ++i) {
        ref.addNodeWithId(i);
        test.addNodeWithId(i);
      }
      ref.addArc(0, 1);
      ref.addArc(1, 2);
      ref.addArc(0, 3);
      test.addArc(1, 0);   // misoriented
      test.addArc(1, 2);   // matches
      test.addArc(3, 4);   // extra

      gum::StructuralMetrics comp;
      comp.compare(ref, test);

      const double total_ordered   = 5.0 * 4.0;
      const double total_unordered = 5.0 * 4.0 / 2.0;
      CHECK(comp.tp() + comp.tn() + comp.fp() + comp.fn() == total_ordered);
      CHECK(comp.tp_skeleton() + comp.tn_skeleton() + comp.fp_skeleton() + comp.fn_skeleton()
            == total_unordered);
    }

    static void test_sum_invariant_undigraph() {
      // For an UndiGraph compare, tp+tn+fp+fn must equal N*(N-1)/2 (unordered
      // pairs); same total for the skeleton variants.
      gum::UndiGraph ref, test;
      for (gum::NodeId i = 0; i < 5; ++i) {
        ref.addNodeWithId(i);
        test.addNodeWithId(i);
      }
      ref.addEdge(0, 1);
      ref.addEdge(1, 2);
      ref.addEdge(0, 3);
      test.addEdge(0, 1);
      test.addEdge(3, 4);   // extra

      gum::StructuralMetrics comp;
      comp.compare(ref, test);

      const double total = 5.0 * 4.0 / 2.0;
      CHECK(comp.tp() + comp.tn() + comp.fp() + comp.fn() == total);
      CHECK(comp.tp_skeleton() + comp.tn_skeleton() + comp.fp_skeleton() + comp.fn_skeleton()
            == total);
    }

    // NOTE: no sum-invariant test for compare(PDAG, PDAG): the existing
    // _true_none_ formula in compare() omits _wrong_arc_edge_/_wrong_edge_arc_,
    // so the invariant breaks whenever ref/test swap an arc and an edge.

    static void test_accessors_before_compare() {
      // Calling accessors before compare() reads uninitialized members:
      // the values returned are undefined. We only check that nothing throws.
      gum::StructuralMetrics comp;
      CHECK_NOTHROW(comp.tp());
      CHECK_NOTHROW(comp.fp());
      CHECK_NOTHROW(comp.fn());
      CHECK_NOTHROW(comp.tn());
      CHECK_NOTHROW(comp.tp_skeleton());
      CHECK_NOTHROW(comp.fp_skeleton());
      CHECK_NOTHROW(comp.fn_skeleton());
      CHECK_NOTHROW(comp.tn_skeleton());
      CHECK_NOTHROW(comp.precision());
      CHECK_NOTHROW(comp.recall());
      CHECK_NOTHROW(comp.f_score());
      CHECK_NOTHROW(comp.shd());
      CHECK_NOTHROW(comp.precision_skeleton());
      CHECK_NOTHROW(comp.recall_skeleton());
      CHECK_NOTHROW(comp.f_score_skeleton());
      CHECK_NOTHROW(comp.shd_skeleton());
    }
  };

  GUM_TEST_ACTIF(_graph)
  GUM_TEST_ACTIF(_bn)
  GUM_TEST_ACTIF(_identities_by_names)
  GUM_TEST_ACTIF(_sid_identity)
  GUM_TEST_ACTIF(_sid_subgraph_zero)
  GUM_TEST_ACTIF(_sid_empty_target)
  GUM_TEST_ACTIF(_sid_reversed_arc)
  GUM_TEST_ACTIF(_sid_asymmetry)
  GUM_TEST_ACTIF(_sid_paper_figure2_h1)
  GUM_TEST_ACTIF(_sid_paper_figure2_h2)
  GUM_TEST_ACTIF(_sid_bn_overload)
  GUM_TEST_ACTIF(_sum_invariant_digraph)
  GUM_TEST_ACTIF(_sum_invariant_undigraph)
  GUM_TEST_ACTIF(_accessors_before_compare)

} /* namespace gum_tests */
