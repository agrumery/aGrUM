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

#include <agrum/base/graphs/graphElements.h>
#include <agrum/base/graphs/PAG.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  PAG
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct PAGTestSuite {
    // Helper: 4-node PAG — triangle 0-1-2 plus isolated node 3
    static gum::PAG buildTriangle() {
      gum::PAG g;
      g.addNodeWithId(0);
      g.addNodeWithId(1);
      g.addNodeWithId(2);
      g.addNodeWithId(3);
      g.addEdge(0, 1);
      g.addEdge(1, 2);
      g.addEdge(0, 2);
      return g;
    }

    // ##########################################################################
    // Construction
    // ##########################################################################

    static void test_default_constructor_() {
      gum::PAG g;
      CHECK(g.size() == 0);
      CHECK(g.sizeEdges() == 0);
    }

    static void test_addEdge_circle_circle_default_() {
      // addEdge(x,y) must set both endpoint marks to Circle
      gum::PAG g;
      g.addNodeWithId(0);
      g.addNodeWithId(1);
      g.addEdge(0, 1);

      CHECK(g.sizeEdges() == 1);
      CHECK(g.isCircle(0, 1));   // mark at 1 from 0
      CHECK(g.isCircle(1, 0));   // mark at 0 from 1
    }

    static void test_addEdge_explicit_marks_() {
      // addEdge(x,y, markAtX, markAtY) with explicit marks
      gum::PAG g;
      g.addNodeWithId(0);
      g.addNodeWithId(1);
      g.addEdge(0, 1, gum::EdgeMark::Tail, gum::EdgeMark::Arrowhead);

      CHECK(g.isTail(1, 0));        // mark at 0-endpoint = Tail
      CHECK(g.isArrowhead(0, 1));   // mark at 1-endpoint = Arrowhead
    }

    static void test_addEdge_duplicate_noop_() {
      // adding an existing edge must not reset marks
      gum::PAG g;
      g.addNodeWithId(0);
      g.addNodeWithId(1);
      g.addEdge(0, 1, gum::EdgeMark::Tail, gum::EdgeMark::Arrowhead);
      g.addEdge(0, 1);         // duplicate: should do nothing

      CHECK(g.sizeEdges() == 1);
      CHECK(g.isTail(1, 0));   // marks unchanged
      CHECK(g.isArrowhead(0, 1));
    }

    // ##########################################################################
    // Mark accessors
    // ##########################################################################

    static void test_setMarkAt_() {
      gum::PAG g;
      g.addNodeWithId(0);
      g.addNodeWithId(1);
      g.addEdge(0, 1);   // Circle-Circle

      g.setMarkAt(0, 1, gum::EdgeMark::Arrowhead);
      CHECK(g.isArrowhead(0, 1));
      CHECK(g.isCircle(1, 0));   // other end unchanged
    }

    static void test_markAt_returns_correct_endpoint_() {
      // marks_[Arc(x,y)] = mark at y; marks_[Arc(y,x)] = mark at x
      gum::PAG g;
      g.addNodeWithId(0);
      g.addNodeWithId(1);
      g.addEdge(0, 1, gum::EdgeMark::Tail, gum::EdgeMark::Arrowhead);

      CHECK(g.markAt(0, 1) == gum::EdgeMark::Arrowhead);   // mark at 1
      CHECK(g.markAt(1, 0) == gum::EdgeMark::Tail);        // mark at 0
    }

    // ##########################################################################
    // Predicates
    // ##########################################################################

    static void test_isDefinitelyDirected_() {
      gum::PAG g;
      g.addNodeWithId(0);
      g.addNodeWithId(1);
      g.addEdge(0, 1, gum::EdgeMark::Tail, gum::EdgeMark::Arrowhead);   // 0 → 1

      CHECK(g.isDefinitelyDirected(0, 1));
      CHECK(!g.isDefinitelyDirected(1, 0));
    }

    static void test_isBidirected_() {
      gum::PAG g;
      g.addNodeWithId(0);
      g.addNodeWithId(1);
      g.addEdge(0, 1, gum::EdgeMark::Arrowhead, gum::EdgeMark::Arrowhead);   // 0 ↔ 1

      CHECK(g.isBidirected(0, 1));
      CHECK(g.isBidirected(1, 0));                                           // symmetric
      CHECK(!g.isDefinitelyDirected(0, 1));
    }

    static void test_isDefCollider_() {
      // Path: 0 → 2 ← 1  (collider at 2)
      gum::PAG g;
      g.addNodeWithId(0);
      g.addNodeWithId(1);
      g.addNodeWithId(2);
      g.addEdge(0, 2, gum::EdgeMark::Tail, gum::EdgeMark::Arrowhead);   // 0 → 2
      g.addEdge(1, 2, gum::EdgeMark::Tail, gum::EdgeMark::Arrowhead);   // 1 → 2

      CHECK(g.isDefCollider(0, 2, 1));
      CHECK(!g.isDefCollider(2, 0, 1));                                 // 0 is not a collider
    }

    // ##########################################################################
    // Bulk operations
    // ##########################################################################

    static void test_reorientAllWith_circle_() {
      gum::PAG g;
      g.addNodeWithId(0);
      g.addNodeWithId(1);
      g.addNodeWithId(2);
      g.addEdge(0, 1, gum::EdgeMark::Tail, gum::EdgeMark::Arrowhead);
      g.addEdge(1, 2, gum::EdgeMark::Arrowhead, gum::EdgeMark::Arrowhead);

      g.reorientAllWith(gum::EdgeMark::Circle);

      CHECK(g.isCircle(0, 1));
      CHECK(g.isCircle(1, 0));
      CHECK(g.isCircle(1, 2));
      CHECK(g.isCircle(2, 1));
    }

    // ##########################################################################
    // Edge/node removal
    // ##########################################################################

    static void test_eraseEdge_removes_marks_() {
      gum::PAG g;
      g.addNodeWithId(0);
      g.addNodeWithId(1);
      g.addEdge(0, 1);
      g.eraseEdge(gum::Edge(0, 1));

      CHECK(g.sizeEdges() == 0);
      // marks_ must not contain stale entries (no crash on future addEdge)
      g.addEdge(0, 1);
      CHECK(g.isCircle(0, 1));
      CHECK(g.isCircle(1, 0));
    }

    static void test_eraseNode_removes_marks_() {
      gum::PAG g = buildTriangle();   // 0-1-2, node 3 isolated

      g.eraseNode(1);                 // removes edges 0-1 and 1-2

      CHECK(g.size() == 3);
      CHECK(g.sizeEdges() == 1);      // only 0-2 remains
      // re-add 1 and edge 0-1: marks must be fresh Circle-Circle
      g.addNodeWithId(1);
      g.addEdge(0, 1);
      CHECK(g.isCircle(0, 1));
      CHECK(g.isCircle(1, 0));
    }

    static void test_clear_removes_all_() {
      gum::PAG g = buildTriangle();
      g.clear();

      CHECK(g.size() == 0);
      CHECK(g.sizeEdges() == 0);
    }

    static void test_clearEdges_keeps_nodes_() {
      gum::PAG g = buildTriangle();
      g.clearEdges();

      CHECK(g.size() == 4);
      CHECK(g.sizeEdges() == 0);
    }

    // ##########################################################################
    // Copy / move
    // ##########################################################################

    static void test_copy_constructor_copies_marks_() {
      gum::PAG g;
      g.addNodeWithId(0);
      g.addNodeWithId(1);
      g.addEdge(0, 1, gum::EdgeMark::Tail, gum::EdgeMark::Arrowhead);

      gum::PAG h(g);

      CHECK(h.sizeEdges() == 1);
      CHECK(h.isTail(1, 0));
      CHECK(h.isArrowhead(0, 1));
    }

    static void test_copy_assignment_copies_marks_() {
      gum::PAG g;
      g.addNodeWithId(0);
      g.addNodeWithId(1);
      g.addEdge(0, 1, gum::EdgeMark::Arrowhead, gum::EdgeMark::Arrowhead);

      gum::PAG h;
      h = g;

      CHECK(h.isBidirected(0, 1));
    }

    static void test_move_constructor_() {
      gum::PAG g;
      g.addNodeWithId(0);
      g.addNodeWithId(1);
      g.addEdge(0, 1, gum::EdgeMark::Tail, gum::EdgeMark::Arrowhead);

      gum::PAG h(std::move(g));

      CHECK(h.sizeEdges() == 1);
      CHECK(h.isDefinitelyDirected(0, 1));
    }

    // ##########################################################################
    // toMixedGraph conversion
    // ##########################################################################

    static void test_toMixedGraph_tail_arrow_becomes_arc_() {
      gum::PAG g;
      g.addNodeWithId(0);
      g.addNodeWithId(1);
      g.addEdge(0, 1, gum::EdgeMark::Tail, gum::EdgeMark::Arrowhead);   // 0 → 1

      auto mg = g.toMixedGraph();

      CHECK(mg.existsArc(0, 1));
      CHECK(!mg.existsArc(1, 0));
      CHECK(!mg.existsEdge(0, 1));
    }

    static void test_toMixedGraph_bidirected_becomes_double_arc_() {
      gum::PAG g;
      g.addNodeWithId(0);
      g.addNodeWithId(1);
      g.addEdge(0, 1, gum::EdgeMark::Arrowhead, gum::EdgeMark::Arrowhead);   // 0 ↔ 1

      auto mg = g.toMixedGraph();

      CHECK(mg.existsArc(0, 1));
      CHECK(mg.existsArc(1, 0));
    }

    static void test_toMixedGraph_circle_circle_becomes_edge_() {
      gum::PAG g;
      g.addNodeWithId(0);
      g.addNodeWithId(1);
      g.addEdge(0, 1);   // Circle-Circle

      auto mg = g.toMixedGraph();

      CHECK(mg.existsEdge(0, 1));
      CHECK(!mg.existsArc(0, 1));
      CHECK(!mg.existsArc(1, 0));
    }

    // ##########################################################################
    // Display (no-crash)
    // ##########################################################################

    static void test_toString_no_crash_() {
      gum::PAG g = buildTriangle();
      g.setMarkAt(0, 1, gum::EdgeMark::Arrowhead);
      CHECK(!g.toString().empty());
    }

    static void test_toDot_no_crash_() {
      gum::PAG g = buildTriangle();
      CHECK(!g.toDot().empty());
    }
  };

  GUM_TEST_ACTIF(_default_constructor_)
  GUM_TEST_ACTIF(_addEdge_circle_circle_default_)
  GUM_TEST_ACTIF(_addEdge_explicit_marks_)
  GUM_TEST_ACTIF(_addEdge_duplicate_noop_)
  GUM_TEST_ACTIF(_setMarkAt_)
  GUM_TEST_ACTIF(_markAt_returns_correct_endpoint_)
  GUM_TEST_ACTIF(_isDefinitelyDirected_)
  GUM_TEST_ACTIF(_isBidirected_)
  GUM_TEST_ACTIF(_isDefCollider_)
  GUM_TEST_ACTIF(_reorientAllWith_circle_)
  GUM_TEST_ACTIF(_eraseEdge_removes_marks_)
  GUM_TEST_ACTIF(_eraseNode_removes_marks_)
  GUM_TEST_ACTIF(_clear_removes_all_)
  GUM_TEST_ACTIF(_clearEdges_keeps_nodes_)
  GUM_TEST_ACTIF(_copy_constructor_copies_marks_)
  GUM_TEST_ACTIF(_copy_assignment_copies_marks_)
  GUM_TEST_ACTIF(_move_constructor_)
  GUM_TEST_ACTIF(_toMixedGraph_tail_arrow_becomes_arc_)
  GUM_TEST_ACTIF(_toMixedGraph_bidirected_becomes_double_arc_)
  GUM_TEST_ACTIF(_toMixedGraph_circle_circle_becomes_edge_)
  GUM_TEST_ACTIF(_toString_no_crash_)
  GUM_TEST_ACTIF(_toDot_no_crash_)

} /* namespace gum_tests */
