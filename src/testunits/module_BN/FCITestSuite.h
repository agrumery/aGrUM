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

#include <algorithm>

#include <agrum/base/database/databaseTable.h>
#include <agrum/base/database/DBInitializerFromCSV.h>
#include <agrum/base/database/DBRowGeneratorIdentity.h>
#include <agrum/base/database/DBRowGeneratorParser.h>
#include <agrum/base/database/DBTranslator4LabelizedVariable.h>
#include <agrum/base/database/DBTranslatorSet.h>
#include <agrum/base/graphs/DAG.h>
#include <agrum/base/graphs/PAG.h>
#include <agrum/base/graphs/mixedGraph.h>
#include <agrum/base/stattests/indepTestChi2.h>
#include <agrum/BN/learning/FCI.h>
#include <agrum/BN/learning/PC.h>
#include <agrum/BN/learning/priors/noPrior.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  FCI
#define GUM_CURRENT_MODULE BN

namespace gum_tests {

  // Reuse the Asia CSV helper (same as PCTestSuite)
  struct FCIAsiaDB {
    gum::learning::DBInitializerFromCSV initializer;
    std::size_t                         nb_vars;
    gum::learning::DBTranslatorSet      translator_set;
    gum::learning::DatabaseTable        database;
    gum::learning::DBRowGeneratorSet    genset;
    gum::learning::DBRowGeneratorParser parser;
    gum::learning::NoPrior              prior;

    FCIAsiaDB() :
        initializer(GET_RESSOURCES_PATH("csv/asia.csv")),
        nb_vars(initializer.variableNames().size()), database(makeTranslatorSet_(nb_vars)),
        parser(database.handler(), genset), prior(database) {
      database.setVariableNames(initializer.variableNames());
      initializer.fillDatabase(database);
    }

    gum::MixedGraph nodeOnlyGraph() const {
      gum::MixedGraph g;
      for (gum::Size i = 0; i < nb_vars; ++i) {
        g.addNodeWithId(i);
      }
      return g;
    }

    private:
    static gum::learning::DBTranslatorSet makeTranslatorSet_(std::size_t n) {
      gum::learning::DBTranslatorSet                ts;
      gum::learning::DBTranslator4LabelizedVariable tr;
      for (std::size_t i = 0; i < n; ++i) {
        ts.insertTranslator(tr, i);
      }
      return ts;
    }
  };

  struct FCITestSuite {
    // ##########################################################################
    // Construction and parameterisation
    // ##########################################################################

    static void test_default_constructor_() {
      gum::learning::FCI fci;
      CHECK(fci.alpha() == doctest::Approx(0.05));
      CHECK(fci.maxPathLength() == gum::Size(-1));
    }

    static void test_setters_() {
      gum::learning::FCI fci;
      fci.setAlpha(0.01);
      fci.setMaxPathLength(5);
      CHECK(fci.alpha() == doctest::Approx(0.01));
      CHECK(fci.maxPathLength() == 5);
    }

    static void test_copy_constructor_() {
      FCIAsiaDB                       db;
      gum::learning::IndepTestChi2    chi2(db.parser, db.prior);
      gum::learning::FCI              fci;
      fci.setIndependenceTest(chi2);
      fci.setAlpha(0.01);
      fci.setMaxPathLength(3);

      gum::learning::FCI fci2(fci);
      CHECK(fci2.alpha() == doctest::Approx(0.01));
      CHECK(fci2.maxPathLength() == 3);
    }

    static void test_move_constructor_() {
      gum::learning::FCI fci;
      fci.setAlpha(0.01);
      fci.setMaxPathLength(3);

      gum::learning::FCI fci2(std::move(fci));
      CHECK(fci2.alpha() == doctest::Approx(0.01));
      CHECK(fci2.maxPathLength() == 3);
    }

    // ##########################################################################
    // Phase 1: skeleton
    // ##########################################################################

    // test_3 from plan: at Phase 1 (before possibleDSep), FCI and PC produce
    // identical skeletons — same CI test, same alpha, same stable-PC algorithm
    static void test_fci_skeleton_identical_to_pc_() {
      FCIAsiaDB                    db;
      gum::learning::IndepTestChi2 chi2_fci(db.parser, db.prior);
      gum::learning::IndepTestChi2 chi2_pc(db.parser, db.prior);

      gum::learning::FCI fci;
      fci.setIndependenceTest(chi2_fci);
      fci.setAlpha(0.05);

      gum::learning::PC pc;
      pc.setIndependenceTest(chi2_pc);
      pc.setAlpha(0.05);

      gum::MixedGraph skel_fci = fci.learnSkeleton(db.nodeOnlyGraph());
      gum::MixedGraph skel_pc  = pc.learnSkeleton(db.nodeOnlyGraph());

      CHECK(skel_fci.sizeEdges() == skel_pc.sizeEdges());
      CHECK(skel_fci.size() == skel_pc.size());
      for (const auto& e: skel_pc.edges()) {
        CHECK(skel_fci.existsEdge(e));
      }
    }

    // ##########################################################################
    // Phase 2: learnPAG (skeleton + collider orientation)
    // ##########################################################################

    // test_5 from plan: PAG correct node count, no exception, edges well-formed
    static void test_fci_learnPAG_valid_() {
      FCIAsiaDB                    db;
      gum::learning::IndepTestChi2 chi2(db.parser, db.prior);

      gum::learning::FCI fci;
      fci.setIndependenceTest(chi2);

      gum::PAG pag = fci.learnPAG(db.nodeOnlyGraph());

      CHECK(pag.size() == db.nb_vars);
      // every edge must have marks on both endpoints
      for (const auto& e: pag.edges()) {
        // markAt throws NotFound if entry missing — verifies marks_ consistency
        CHECK_NOTHROW(pag.markAt(e.first(), e.second()));
        CHECK_NOTHROW(pag.markAt(e.second(), e.first()));
      }
    }

    // Phase 2 must orient some arrowheads (v-structures exist in asia network)
    static void test_fci_learnPAG_has_arrowheads_() {
      FCIAsiaDB                    db;
      gum::learning::IndepTestChi2 chi2(db.parser, db.prior);

      gum::learning::FCI fci;
      fci.setIndependenceTest(chi2);

      gum::PAG pag = fci.learnPAG(db.nodeOnlyGraph());

      bool found_arrowhead = false;
      for (const auto& e: pag.edges()) {
        if (pag.isArrowhead(e.first(), e.second())
            || pag.isArrowhead(e.second(), e.first())) {
          found_arrowhead = true;
          break;
        }
      }
      CHECK(found_arrowhead);
    }

    // ##########################################################################
    // Phase 5: orientation rules R1–R7
    // ##########################################################################

    // R1 (away from collider) must be at fixed point: no unshielded triple
    // A-B-C should have arrowhead at B from A AND circle at B from C.
    static void test_fci_phase5_r1_converged_() {
      FCIAsiaDB                    db;
      gum::learning::IndepTestChi2 chi2(db.parser, db.prior);

      gum::learning::FCI fci;
      fci.setIndependenceTest(chi2);

      const gum::PAG pag = fci.learnPAG(db.nodeOnlyGraph());

      for (const auto& e: pag.edges()) {
        const gum::NodeId b1 = e.first();
        const gum::NodeId b2 = e.second();
        for (const gum::NodeId b: {b1, b2}) {
          for (const auto& e2: pag.edges()) {
            const gum::NodeId x = e2.first();
            const gum::NodeId y = e2.second();
            for (const gum::NodeId c: {x, y}) {
              const gum::NodeId a = (c == x) ? y : x;
              if (a == b || c == b) { continue; }
              if (!pag.existsEdge(a, b)) { continue; }
              if (pag.existsEdge(a, c)) { continue; }   // unshielded: a not adj c
              // R1 convergence: if a *→ b then b must NOT have circle at b from c
              if (pag.isArrowhead(a, b)) {
                CHECK_MESSAGE(!pag.isCircle(c, b),
                              "R1 not fully applied: arrowhead at b from a, circle at b from c");
              }
            }
          }
        }
      }
    }

    // ##########################################################################
    // Bridge: learnMixedStructure, learnDAG, learnPDAG
    // ##########################################################################

    // test_6 from plan: bridge to DAG works, correct node count
    static void test_fci_learnDAG_valid_() {
      FCIAsiaDB                    db;
      gum::learning::IndepTestChi2 chi2(db.parser, db.prior);

      gum::learning::FCI fci;
      fci.setIndependenceTest(chi2);

      gum::DAG dag = fci.learnDAG(db.nodeOnlyGraph());

      CHECK(dag.size() == db.nb_vars);
    }

    // test_7 from plan
    static void test_fci_learnPDAG_valid_() {
      FCIAsiaDB                    db;
      gum::learning::IndepTestChi2 chi2(db.parser, db.prior);

      gum::learning::FCI fci;
      fci.setIndependenceTest(chi2);

      gum::PDAG pdag = fci.learnPDAG(db.nodeOnlyGraph());

      CHECK(pdag.size() == db.nb_vars);
    }

    // ##########################################################################
    // Phase 3: possibleDSep pruning
    // ##########################################################################

    // Phase 3 can only remove edges → PAG after phases 1-3 has ≤ edges than
    // the phase-1 skeleton (which equals the PC skeleton).
    static void test_fci_phase3_edges_subset_of_skeleton_() {
      FCIAsiaDB                    db;
      gum::learning::IndepTestChi2 chi2_skel(db.parser, db.prior);
      gum::learning::IndepTestChi2 chi2_pag(db.parser, db.prior);

      gum::learning::FCI fci_skel;
      fci_skel.setIndependenceTest(chi2_skel);
      gum::MixedGraph skel = fci_skel.learnSkeleton(db.nodeOnlyGraph());

      gum::learning::FCI fci_pag;
      fci_pag.setIndependenceTest(chi2_pag);
      gum::PAG pag = fci_pag.learnPAG(db.nodeOnlyGraph());

      // every PAG edge must exist in the phase-1 skeleton
      CHECK(pag.sizeEdges() <= skel.sizeEdges());
      for (const auto& e: pag.edges()) {
        CHECK(skel.existsEdge(e.first(), e.second()));
      }
    }

    // learnMixedStructure node count
    static void test_fci_learnMixedStructure_valid_() {
      FCIAsiaDB                    db;
      gum::learning::IndepTestChi2 chi2(db.parser, db.prior);

      gum::learning::FCI fci;
      fci.setIndependenceTest(chi2);

      gum::MixedGraph mg = fci.learnMixedStructure(db.nodeOnlyGraph());

      CHECK(mg.size() == db.nb_vars);
    }

    // Phase 6: latentVariables() accessible after learnPAG; Asia has no hidden confounders
    static void test_fci_latent_couples_after_learnPAG_() {
      FCIAsiaDB                    db;
      gum::learning::IndepTestChi2 chi2(db.parser, db.prior);

      gum::learning::FCI fci;
      fci.setIndependenceTest(chi2);

      CHECK_NOTHROW(fci.learnPAG(db.nodeOnlyGraph()));
      const auto& lv = fci.latentVariables();
      // Asia BN has no latent confounders — FCI should detect zero bidirected edges
      CHECK(lv.size() == 0);
    }

    // ##########################################################################
    // Non-regression: structural invariants that must hold before and after
    // any improvement to FCI (Phase A / Phase B).
    // ##########################################################################

    // alpha=1.0 → pval > 1.0 never true → no edge removed → complete PAG
    static void test_fci_alpha_one_complete_pag_() {
      FCIAsiaDB                    db;
      gum::learning::IndepTestChi2 chi2(db.parser, db.prior);
      gum::learning::FCI           fci;
      fci.setIndependenceTest(chi2);
      fci.setAlpha(1.0);
      const auto pag = fci.learnPAG(db.nodeOnlyGraph());
      const auto n   = static_cast< gum::Size >(db.nb_vars);
      CHECK(pag.sizeEdges() == n * (n - 1) / 2);
    }

    // stable=false must not crash and must produce a structurally valid PAG
    static void test_fci_unstable_mode_valid_() {
      FCIAsiaDB                    db;
      gum::learning::IndepTestChi2 chi2(db.parser, db.prior);
      gum::learning::FCI           fci;
      fci.setIndependenceTest(chi2);
      fci.setStable(false);
      const auto pag = fci.learnPAG(db.nodeOnlyGraph());
      CHECK(pag.size() == db.nb_vars);
      for (const auto& e: pag.edges()) {
        CHECK_NOTHROW(pag.markAt(e.first(), e.second()));
        CHECK_NOTHROW(pag.markAt(e.second(), e.first()));
      }
    }

    // maxPathLength=0 restricts R4 path search; PAG must still be structurally valid
    static void test_fci_max_path_length_zero_valid_() {
      FCIAsiaDB                    db;
      gum::learning::IndepTestChi2 chi2(db.parser, db.prior);
      gum::learning::FCI           fci;
      fci.setIndependenceTest(chi2);
      fci.setMaxPathLength(0);
      const auto pag = fci.learnPAG(db.nodeOnlyGraph());
      CHECK(pag.size() == db.nb_vars);
      CHECK(pag.sizeEdges() <= db.nb_vars * (db.nb_vars - 1) / 2);
    }

    // ##########################################################################
    // Phase B (exhaustive sepset flag) — non-regression + feature tests.
    // These FAIL TO COMPILE until setExhaustiveSepSet / exhaustiveSepSet added.
    // ##########################################################################

    // flag defaults to false
    static void test_fci_exhaustive_sepset_default_false_() {
      gum::learning::FCI fci;
      CHECK(fci.exhaustiveSepSet() == false);
    }

    // setting flag to true and back round-trips correctly
    static void test_fci_exhaustive_sepset_flag_roundtrip_() {
      gum::learning::FCI fci;
      fci.setExhaustiveSepSet(true);
      CHECK(fci.exhaustiveSepSet() == true);
      fci.setExhaustiveSepSet(false);
      CHECK(fci.exhaustiveSepSet() == false);
    }

    // flag is preserved across copy construction
    static void test_fci_exhaustive_sepset_copy_preserves_flag_() {
      gum::learning::FCI fci;
      fci.setExhaustiveSepSet(true);
      const gum::learning::FCI copy(fci);
      CHECK(copy.exhaustiveSepSet() == true);
    }

    // exhaustive mode must not crash; PAG remains structurally valid
    static void test_fci_exhaustive_sepset_valid_pag_() {
      FCIAsiaDB                    db;
      gum::learning::IndepTestChi2 chi2(db.parser, db.prior);
      gum::learning::FCI           fci;
      fci.setIndependenceTest(chi2);
      fci.setExhaustiveSepSet(true);
      const auto pag = fci.learnPAG(db.nodeOnlyGraph());
      CHECK(pag.size() == db.nb_vars);
      for (const auto& e: pag.edges()) {
        CHECK_NOTHROW(pag.markAt(e.first(), e.second()));
        CHECK_NOTHROW(pag.markAt(e.second(), e.first()));
      }
    }

    // exhaustive flag must not change the skeleton (only sepset content changes)
    static void test_fci_exhaustive_sepset_same_skeleton_as_standard_() {
      FCIAsiaDB                    db;
      gum::learning::IndepTestChi2 chi2a(db.parser, db.prior);
      gum::learning::IndepTestChi2 chi2b(db.parser, db.prior);

      gum::learning::FCI standard_fci;
      standard_fci.setIndependenceTest(chi2a);
      standard_fci.setExhaustiveSepSet(false);

      gum::learning::FCI exhaustive_fci;
      exhaustive_fci.setIndependenceTest(chi2b);
      exhaustive_fci.setExhaustiveSepSet(true);

      const auto skel_std = standard_fci.learnSkeleton(db.nodeOnlyGraph());
      const auto skel_exh = exhaustive_fci.learnSkeleton(db.nodeOnlyGraph());

      CHECK(skel_std.sizeEdges() == skel_exh.sizeEdges());
      for (const auto& e: skel_std.edges()) {
        CHECK(skel_exh.existsEdge(e.first(), e.second()));
      }
    }

    // ##########################################################################
    // Phase A (possibleDSep criterion) — unit tests on hand-built PAGs.
    // Tests marked [FAILS NOW] will fail with the current (old) criterion and
    // PASS after Phase A replaces the BFS expansion condition.
    // ##########################################################################

    // Definite collider at B on (X, B, C) → C must be in possibleDSep(X, Y).
    // Both old and new criteria agree on this case.
    static void test_fci_possibledsep_def_collider_included_() {
      // PAG: X(0) *→ B(1) ←* C(2),  X(0) o-o Y(3)
      //   X-B: circle at X from B, arrowhead at B from X → addEdge(0,1, Circle, Arrowhead)
      //   B-C: arrowhead at B from C, circle at C from B → addEdge(1,2, Arrowhead, Circle)
      gum::PAG pag;
      for (gum::NodeId i = 0; i < 4; ++i) { pag.addNodeWithId(i); }
      pag.addEdge(0, 3, gum::EdgeMark::Circle,    gum::EdgeMark::Circle);    // X o-o Y
      pag.addEdge(0, 1, gum::EdgeMark::Circle,    gum::EdgeMark::Arrowhead); // X *→ B
      pag.addEdge(1, 2, gum::EdgeMark::Arrowhead, gum::EdgeMark::Circle);    // C *→ B

      gum::learning::FCI fci;
      const auto         dsep = fci.possibleDSep(pag, 0, 3);

      // B(1): direct neighbor of X → always in possibleDSep
      CHECK(std::find(dsep.begin(), dsep.end(), gum::NodeId(1)) != dsep.end());
      // C(2): B is def collider on (X,B,C) → expansion happens → C in possibleDSep
      CHECK(std::find(dsep.begin(), dsep.end(), gum::NodeId(2)) != dsep.end());
    }

    // Non-neighbor excluded from possibleDSep when no expansion path leads there.
    // Both criteria agree on this: isolated node is never reachable.
    static void test_fci_possibledsep_unreachable_node_excluded_() {
      // PAG: X(0) o-o Y(1),  Z(2) has no edges → unreachable from X via BFS
      gum::PAG pag;
      for (gum::NodeId i = 0; i < 3; ++i) { pag.addNodeWithId(i); }
      pag.addEdge(0, 1, gum::EdgeMark::Circle, gum::EdgeMark::Circle);  // X o-o Y

      gum::learning::FCI fci;
      const auto         dsep = fci.possibleDSep(pag, 0, 1);

      // Z(2) has no edges → never reachable → not in possibleDSep
      CHECK(std::find(dsep.begin(), dsep.end(), gum::NodeId(2)) == dsep.end());
    }

    // [FAILS NOW with old criterion — PASSES after Phase A]
    // Node B with Circle-Tail mark (not a def collider, not def non-collider) must
    // NOT lead to expansion under Zhang's criterion.
    //
    // Graph:  X(0) -o B(1) -- C(2),  X(0) o-o Y(3)  (C not adjacent to X)
    //   X-B: tail at X-endpoint, circle at B-endpoint → addEdge(0,1, Tail, Circle)
    //   B-C: tail at both endpoints (undirected)      → addEdge(1,2, Tail, Tail)
    //
    // marks at B when traversing (X→B): Circle  (not Arrowhead → B not def collider)
    // marks at B when traversing (C→B): Tail    (not Arrowhead → B not def collider)
    // → B is NOT a definite collider on (X, B, C)
    // → Zhang criterion: do NOT expand → C must NOT be in possibleDSep(X, Y)
    static void test_fci_possibledsep_circle_tail_node_excluded_() {
      gum::PAG pag;
      for (gum::NodeId i = 0; i < 4; ++i) { pag.addNodeWithId(i); }
      pag.addEdge(0, 3, gum::EdgeMark::Circle, gum::EdgeMark::Circle);  // X o-o Y
      pag.addEdge(0, 1, gum::EdgeMark::Tail,   gum::EdgeMark::Circle);  // X -o B
      pag.addEdge(1, 2, gum::EdgeMark::Tail,   gum::EdgeMark::Tail);    // B -- C

      gum::learning::FCI fci;
      const auto         dsep = fci.possibleDSep(pag, 0, 3);

      // B(1): direct neighbor of X → always in possibleDSep
      CHECK(std::find(dsep.begin(), dsep.end(), gum::NodeId(1)) != dsep.end());
      // C(2): circle-tail at B → B not def collider → Zhang does not expand
      // [FAILS with old criterion; PASSES after Phase A]
      CHECK(std::find(dsep.begin(), dsep.end(), gum::NodeId(2)) == dsep.end());
    }

    // [FAILS NOW with old criterion — PASSES after Phase A]
    // Tail-Tail at B (definite non-collider) → no expansion under either criterion.
    // Both must agree: definite non-collider → C not in possibleDSep.
    static void test_fci_possibledsep_definite_noncollider_excluded_() {
      // Graph:  X(0) → B(1) → C(2),  X(0) o-o Y(3)
      //   X-B: tail at X-endpoint from B (→ isTail(B,X)=true), arrowhead at B from X
      //       → addEdge(0,1, Circle, Arrowhead)   (circle at X from B, arrowhead at B from X)
      // Actually we need definite non-collider: both marks at B are Tail.
      //   X→B: tail at B from X (actually for directed X→B: markAtX=Tail, markAtB=Arrowhead)
      // A definite non-collider on (X,B,C) means: NOT (both arrowheads at B).
      // The strongest form: both marks at B's endpoint are Tail = X--B--C (undirected chain).
      gum::PAG pag;
      for (gum::NodeId i = 0; i < 4; ++i) { pag.addNodeWithId(i); }
      pag.addEdge(0, 3, gum::EdgeMark::Circle, gum::EdgeMark::Circle);  // X o-o Y
      pag.addEdge(0, 1, gum::EdgeMark::Tail,   gum::EdgeMark::Tail);    // X -- B (Tail-Tail)
      pag.addEdge(1, 2, gum::EdgeMark::Tail,   gum::EdgeMark::Tail);    // B -- C (Tail-Tail)

      gum::learning::FCI fci;
      const auto         dsep = fci.possibleDSep(pag, 0, 3);

      // B(1): direct neighbor of X → in possibleDSep
      CHECK(std::find(dsep.begin(), dsep.end(), gum::NodeId(1)) != dsep.end());
      // B has Tail-Tail at B: definite non-collider on (X,B,C)
      // old criterion: !isTail(X,B) || !isTail(C,B) = !Tail || !Tail = false → does NOT expand → C excluded
      // new criterion: isDefCollider(X,B,C) || adj(X,C) = false || false = false → does NOT expand → C excluded
      // BOTH criteria agree: C not in possibleDSep
      CHECK(std::find(dsep.begin(), dsep.end(), gum::NodeId(2)) == dsep.end());
    }

    // ##########################################################################
    // Phase C — background knowledge respected in orientation rules (R0/R1/R2/R4)
    // ##########################################################################

    // no crash when a forbidden arc is set for an existing edge
    static void test_fci_forbidden_arc_no_crash_() {
      FCIAsiaDB                    db;
      gum::learning::IndepTestChi2 chi2(db.parser, db.prior);
      gum::learning::FCI           fci;
      fci.setIndependenceTest(chi2);

      gum::DiGraph forbid;
      for (int i = 0; i < db.nb_vars; ++i) { forbid.addNodeWithId(i); }
      forbid.addArc(0, 1);   // forbid arc 0→1 (one direction only)
      fci.setForbiddenGraph(forbid);

      CHECK_NOTHROW(fci.learnPAG(db.nodeOnlyGraph()));
    }

    // if arc src→dst is forbidden, the PAG must not have an arrowhead at dst from src
    // (unless the edge was removed from the skeleton entirely by isForbiddenEdge_)
    static void test_fci_forbidden_arc_arrowhead_blocked_() {
      FCIAsiaDB                    db;
      gum::learning::IndepTestChi2 chi2(db.parser, db.prior);
      gum::learning::FCI           fci;
      fci.setIndependenceTest(chi2);

      // forbid ALL arcs in direction i→j for i < j
      // this prevents any arrowhead at j from i via R0/R1/R2/R4
      gum::DiGraph forbid;
      for (int i = 0; i < db.nb_vars; ++i) { forbid.addNodeWithId(i); }
      for (int i = 0; i < db.nb_vars; ++i) {
        for (int j = i + 1; j < db.nb_vars; ++j) { forbid.addArc(i, j); }
      }
      fci.setForbiddenGraph(forbid);

      const auto pag = fci.learnPAG(db.nodeOnlyGraph());
      CHECK(pag.size() == db.nb_vars);

      for (const auto& e: pag.edges()) {
        const gum::NodeId src = std::min(e.first(), e.second());
        const gum::NodeId dst = std::max(e.first(), e.second());
        // forbidden arc src→dst: no arrowhead at dst from src
        CHECK_FALSE(pag.isArrowhead(src, dst));
      }
    }

    // forbidden arc on a non-adjacent pair must not change the PAG structure
    static void test_fci_forbidden_arc_does_not_affect_unrelated_edge_() {
      FCIAsiaDB                    db;
      gum::learning::IndepTestChi2 chi2a(db.parser, db.prior);
      gum::learning::IndepTestChi2 chi2b(db.parser, db.prior);

      gum::learning::FCI ref_fci;
      ref_fci.setIndependenceTest(chi2a);
      const auto ref_pag = ref_fci.learnPAG(db.nodeOnlyGraph());

      // find a pair not adjacent in ref_pag; add forbidden arc for it
      gum::DiGraph forbid;
      for (int i = 0; i < db.nb_vars; ++i) { forbid.addNodeWithId(i); }
      for (gum::NodeId i = 0; i < static_cast< gum::NodeId >(db.nb_vars); ++i) {
        for (gum::NodeId j = i + 1; j < static_cast< gum::NodeId >(db.nb_vars); ++j) {
          if (!ref_pag.existsEdge(i, j)) {
            forbid.addArc(i, j);   // forbidden arc on non-edge → no effect
            break;
          }
        }
        if (forbid.sizeArcs() > 0) { break; }
      }
      if (forbid.sizeArcs() == 0) { return; }  // no non-adjacent pair found (trivial pass)

      gum::learning::FCI bk_fci;
      bk_fci.setIndependenceTest(chi2b);
      bk_fci.setForbiddenGraph(forbid);
      const auto bk_pag = bk_fci.learnPAG(db.nodeOnlyGraph());

      CHECK(bk_pag.sizeEdges() == ref_pag.sizeEdges());
    }
  };

  GUM_TEST_ACTIF(_default_constructor_)
  GUM_TEST_ACTIF(_setters_)
  GUM_TEST_ACTIF(_copy_constructor_)
  GUM_TEST_ACTIF(_move_constructor_)
  GUM_TEST_ACTIF(_fci_skeleton_identical_to_pc_)
  GUM_TEST_ACTIF(_fci_learnPAG_valid_)
  GUM_TEST_ACTIF(_fci_learnPAG_has_arrowheads_)
  GUM_TEST_ACTIF(_fci_phase3_edges_subset_of_skeleton_)
  GUM_TEST_ACTIF(_fci_phase5_r1_converged_)
  GUM_TEST_ACTIF(_fci_learnDAG_valid_)
  GUM_TEST_ACTIF(_fci_learnPDAG_valid_)
  GUM_TEST_ACTIF(_fci_learnMixedStructure_valid_)
  GUM_TEST_ACTIF(_fci_latent_couples_after_learnPAG_)

  // non-regression
  GUM_TEST_ACTIF(_fci_alpha_one_complete_pag_)
  GUM_TEST_ACTIF(_fci_unstable_mode_valid_)
  GUM_TEST_ACTIF(_fci_max_path_length_zero_valid_)

  // Phase B: exhaustive sepset flag
  GUM_TEST_ACTIF(_fci_exhaustive_sepset_default_false_)
  GUM_TEST_ACTIF(_fci_exhaustive_sepset_flag_roundtrip_)
  GUM_TEST_ACTIF(_fci_exhaustive_sepset_copy_preserves_flag_)
  GUM_TEST_ACTIF(_fci_exhaustive_sepset_valid_pag_)
  GUM_TEST_ACTIF(_fci_exhaustive_sepset_same_skeleton_as_standard_)

  // Phase A: possibleDSep criterion
  GUM_TEST_ACTIF(_fci_possibledsep_def_collider_included_)
  GUM_TEST_ACTIF(_fci_possibledsep_unreachable_node_excluded_)
  GUM_TEST_ACTIF(_fci_possibledsep_circle_tail_node_excluded_)
  GUM_TEST_ACTIF(_fci_possibledsep_definite_noncollider_excluded_)

  // Phase C: background knowledge respected in orientation rules
  GUM_TEST_ACTIF(_fci_forbidden_arc_no_crash_)
  GUM_TEST_ACTIF(_fci_forbidden_arc_arrowhead_blocked_)
  GUM_TEST_ACTIF(_fci_forbidden_arc_does_not_affect_unrelated_edge_)

} /* namespace gum_tests */
