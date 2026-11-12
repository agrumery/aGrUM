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
#include <tuple>

#include <agrum/base/core/configuration/configuration.h>
#include <agrum/base/database/databaseTable.h>
#include <agrum/base/database/DBInitializerFromCSV.h>
#include <agrum/base/database/DBRowGeneratorIdentity.h>
#include <agrum/base/database/DBRowGeneratorParser.h>
#include <agrum/base/database/DBTranslator4LabelizedVariable.h>
#include <agrum/base/database/DBTranslatorSet.h>
#include <agrum/base/graphs/DAG.h>
#include <agrum/base/graphs/mixedGraph.h>
#include <agrum/base/graphs/PAG.h>
#include <agrum/base/stattests/indepTestChi2.h>
#include <agrum/base/stattests/priors/noPrior.h>
#include <agrum/BN/learning/FCI.h>
#include <agrum/BN/learning/PC.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

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

    protected:
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


    // ##########################################################################
    // Phase 1: skeleton
    // ##########################################################################

    // test_3 from plan: at Phase 1 (before possibleDSep), FCI and PC produce
    // identical skeletons — same CI test, same alpha, same stable-PC algorithm


    // ##########################################################################
    // Phase 2: learnPAG (skeleton + collider orientation)
    // ##########################################################################

    // test_5 from plan: PAG correct node count, no exception, edges well-formed


    // Phase 2 must orient some arrowheads (v-structures exist in asia network)


    // ##########################################################################
    // Phase 5: orientation rules R1–R7
    // ##########################################################################

    // R1 (away from collider) must be at fixed point: no unshielded triple
    // A-B-C should have arrowhead at B from A AND circle at B from C.


    // ##########################################################################
    // Bridge: learnMixedStructure, learnDAG, learnPDAG
    // ##########################################################################

    // test_6 from plan: bridge to DAG works, correct node count


    // test_7 from plan


    // ##########################################################################
    // Phase 3: possibleDSep pruning
    // ##########################################################################

    // Phase 3 can only remove edges → PAG after phases 1-3 has ≤ edges than
    // the phase-1 skeleton (which equals the PC skeleton).


    // learnMixedStructure node count


    // Phase 6: latentVariables() accessible after learnPAG; Asia has no hidden confounders


    // ##########################################################################
    // Non-regression: structural invariants that must hold before and after
    // any improvement to FCI (Phase A / Phase B).
    // ##########################################################################

    // alpha=1.0 → pval > 1.0 never true → no edge removed → complete PAG


    // stable=false must not crash and must produce a structurally valid PAG


    // maxPathLength=0 restricts R4 path search; PAG must still be structurally valid


    // ##########################################################################
    // Phase B (exhaustive sepset flag) — non-regression + feature tests.
    // These FAIL TO COMPILE until setExhaustiveSepSet / exhaustiveSepSet added.
    // ##########################################################################

    // flag defaults to false


    // setting flag to true and back round-trips correctly


    // flag is preserved across copy construction


    // exhaustive mode must not crash; PAG remains structurally valid


    // exhaustive flag must not change the skeleton (only sepset content changes)


    // ##########################################################################
    // Phase A (possibleDSep criterion) — unit tests on hand-built PAGs.
    // Tests marked [FAILS NOW] will fail with the current (old) criterion and
    // PASS after Phase A replaces the BFS expansion condition.
    // ##########################################################################

    // Definite collider at B on (X, B, C) → C must be in possibleDSep(X, Y).
    // Both old and new criteria agree on this case.


    // Non-neighbor excluded from possibleDSep when no expansion path leads there.
    // Both criteria agree on this: isolated node is never reachable.


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


    // [FAILS NOW with old criterion — PASSES after Phase A]
    // Tail-Tail at B (definite non-collider) → no expansion under either criterion.
    // Both must agree: definite non-collider → C not in possibleDSep.


    // ##########################################################################
    // Phase C — background knowledge respected in orientation rules (R0/R1/R2/R4)
    // ##########################################################################

    // no crash when a forbidden arc is set for an existing edge


    // if arc src→dst is forbidden, the PAG must not have an arrowhead at dst from src
    // (unless the edge was removed from the skeleton entirely by isForbiddenEdge_)


    // forbidden arc on a non-adjacent pair must not change the PAG structure
  };

  GUM_TEST(_default_constructor_) {
    gum::learning::FCI fci;
    CHECK(fci.alpha() == doctest::Approx(0.05));
    CHECK(fci.maxPathLength() == gum::Size(-1));
  }

  GUM_TEST(_setters_) {
    gum::learning::FCI fci;
    fci.setAlpha(0.01);
    fci.setMaxPathLength(5);
    CHECK(fci.alpha() == doctest::Approx(0.01));
    CHECK(fci.maxPathLength() == 5);
  }

  GUM_TEST(_copy_constructor_) {
    FCIAsiaDB                    db;
    gum::learning::IndepTestChi2 chi2(db.parser, db.prior);
    gum::learning::FCI           fci;
    fci.setIndependenceTest(chi2);
    fci.setAlpha(0.01);
    fci.setMaxPathLength(3);

    gum::learning::FCI fci2(fci);
    CHECK(fci2.alpha() == doctest::Approx(0.01));
    CHECK(fci2.maxPathLength() == 3);
  }

  GUM_TEST(_move_constructor_) {
    gum::learning::FCI fci;
    fci.setAlpha(0.01);
    fci.setMaxPathLength(3);

    gum::learning::FCI fci2(std::move(fci));
    CHECK(fci2.alpha() == doctest::Approx(0.01));
    CHECK(fci2.maxPathLength() == 3);
  }

  GUM_TEST(_fci_skeleton_identical_to_pc_) {
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

  GUM_TEST(_fci_learnPAG_valid_) {
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

  GUM_TEST(_fci_learnPAG_has_arrowheads_) {
    FCIAsiaDB                    db;
    gum::learning::IndepTestChi2 chi2(db.parser, db.prior);

    gum::learning::FCI fci;
    fci.setIndependenceTest(chi2);

    gum::PAG pag = fci.learnPAG(db.nodeOnlyGraph());

    bool found_arrowhead = false;
    for (const auto& e: pag.edges()) {
      if (pag.isArrowhead(e.first(), e.second()) || pag.isArrowhead(e.second(), e.first())) {
        found_arrowhead = true;
        break;
      }
    }
    CHECK(found_arrowhead);
  }

  GUM_TEST(_fci_phase3_edges_subset_of_skeleton_) {
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

  GUM_TEST(_fci_phase5_r1_converged_) {
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

  GUM_TEST(_fci_learnDAG_valid_) {
    FCIAsiaDB                    db;
    gum::learning::IndepTestChi2 chi2(db.parser, db.prior);

    gum::learning::FCI fci;
    fci.setIndependenceTest(chi2);

    gum::DAG dag = fci.learnDAG(db.nodeOnlyGraph());

    CHECK(dag.size() == db.nb_vars);

    // the DAG orients the representative MAG of the learned PAG (Zhang 2008, Theorem 2): an edge
    // with an arrowhead at one end only, or a tail and a circle, is an arc ; the orientation of
    // the edges o-o is free
    gum::learning::FCI fci2;
    fci2.setIndependenceTest(chi2);
    const auto pag = fci2.learnPAG(db.nodeOnlyGraph());
    CHECK_EQ(dag.sizeArcs(), pag.sizeEdges());
    for (const auto& e: pag.edges()) {
      const auto x = e.first();
      const auto y = e.second();
      CHECK((dag.existsArc(x, y) || dag.existsArc(y, x)));
      if (pag.isArrowhead(x, y) && !pag.isArrowhead(y, x)) CHECK(dag.existsArc(x, y));
      if (pag.isArrowhead(y, x) && !pag.isArrowhead(x, y)) CHECK(dag.existsArc(y, x));
    }
  }

  GUM_TEST(_fci_learnPDAG_valid_) {
    FCIAsiaDB                    db;
    gum::learning::IndepTestChi2 chi2(db.parser, db.prior);

    gum::learning::FCI fci;
    fci.setIndependenceTest(chi2);

    gum::PDAG pdag = fci.learnPDAG(db.nodeOnlyGraph());

    CHECK(pdag.size() == db.nb_vars);

    // the PDAG of the representative MAG: its arcs ->, its edges -- and <-> as edges
    gum::learning::FCI fci2;
    fci2.setIndependenceTest(chi2);
    const auto pag = fci2.learnPAG(db.nodeOnlyGraph());
    CHECK_EQ(pdag.sizeArcs() + pdag.sizeEdges(), pag.sizeEdges());
    for (const auto& e: pag.edges()) {
      const auto x = e.first();
      const auto y = e.second();
      if (pag.isBidirected(x, y) || (pag.isTail(x, y) && pag.isTail(y, x)))
        CHECK(pdag.existsEdge(x, y));
      else if (pag.isArrowhead(x, y) && !pag.isArrowhead(y, x)) CHECK(pdag.existsArc(x, y));
      else if (pag.isArrowhead(y, x) && !pag.isArrowhead(x, y)) CHECK(pdag.existsArc(y, x));
      else CHECK((pdag.existsArc(x, y) || pdag.existsArc(y, x)));
    }
  }

  GUM_TEST(_fci_error_on_invalid_PAG_option_) {
    {
      ConfigurationGuard guard;
      CHECK_EQ(gum::learning::FCI().errorOnInvalidPAG(),
               gum::config().typed< bool >("learning", "fciErrorOnInvalidPAG"));
      gum::config().set("learning", "fciErrorOnInvalidPAG", "True");
      gum::learning::FCI fci;
      CHECK(fci.errorOnInvalidPAG());
      fci.setErrorOnInvalidPAG(false);
      CHECK(!fci.errorOnInvalidPAG());
    }
    // a valid PAG: no error even with the option
    FCIAsiaDB                    db;
    gum::learning::IndepTestChi2 chi2(db.parser, db.prior);
    gum::learning::FCI           fci;
    fci.setIndependenceTest(chi2);
    fci.setErrorOnInvalidPAG(true);
    CHECK_NOTHROW(fci.learnPDAG(db.nodeOnlyGraph()));
  }

  GUM_TEST(_fci_learning_through_the_base_class_) {
    // learnPDAG/learnDAG of FCI are used through a ConstraintBasedLearning (as in learnBN)
    FCIAsiaDB                    db;
    gum::learning::IndepTestChi2 chi2(db.parser, db.prior);

    gum::learning::FCI fci;
    fci.setIndependenceTest(chi2);
    gum::learning::FCI fci2;
    fci2.setIndependenceTest(chi2);
    gum::learning::ConstraintBasedLearning& base = fci2;

    CHECK_EQ(base.learnPDAG(db.nodeOnlyGraph()), fci.learnPDAG(db.nodeOnlyGraph()));
    CHECK_EQ(base.learnDAG(db.nodeOnlyGraph()), fci.learnDAG(db.nodeOnlyGraph()));
  }

  GUM_TEST(_fci_copy_and_move_keep_the_options_) {
    gum::learning::FCI fci;
    const bool         error = !fci.errorOnInvalidPAG();
    fci.setErrorOnInvalidPAG(error);
    fci.setMaxPathLength(3);

    gum::learning::FCI copy(fci);
    CHECK_EQ(copy.errorOnInvalidPAG(), error);
    CHECK_EQ(copy.maxPathLength(), gum::Size(3));

    gum::learning::FCI moved(std::move(copy));
    CHECK_EQ(moved.errorOnInvalidPAG(), error);
    CHECK_EQ(moved.maxPathLength(), gum::Size(3));

    gum::learning::FCI assigned;
    assigned = fci;
    CHECK_EQ(assigned.errorOnInvalidPAG(), error);
    CHECK_EQ(assigned.maxPathLength(), gum::Size(3));

    gum::learning::FCI moveAssigned;
    moveAssigned = std::move(moved);
    CHECK_EQ(moveAssigned.errorOnInvalidPAG(), error);
    CHECK_EQ(moveAssigned.maxPathLength(), gum::Size(3));
  }

  GUM_TEST(_fci_learnMixedStructure_valid_) {
    FCIAsiaDB                    db;
    gum::learning::IndepTestChi2 chi2(db.parser, db.prior);

    gum::learning::FCI fci;
    fci.setIndependenceTest(chi2);

    gum::MixedGraph mg = fci.learnMixedStructure(db.nodeOnlyGraph());

    CHECK(mg.size() == db.nb_vars);
  }

  GUM_TEST(_fci_latent_couples_after_learnPAG_) {
    FCIAsiaDB                    db;
    gum::learning::IndepTestChi2 chi2(db.parser, db.prior);

    gum::learning::FCI fci;
    fci.setIndependenceTest(chi2);

    CHECK_NOTHROW(fci.learnPAG(db.nodeOnlyGraph()));
    const auto& lv = fci.latentVariables();
    // Asia BN has no latent confounders — FCI should detect zero bidirected edges
    CHECK(lv.size() == 0);
  }

  // non-regression
  GUM_TEST(_fci_alpha_bounds_refused_) {
    // alpha must belong to ]0,1[: the degenerate thresholds 0 and 1 are refused
    gum::learning::FCI fci;
    const double       before = fci.alpha();
    CHECK_THROWS_AS(fci.setAlpha(1.0), gum::OutOfBounds);
    CHECK_THROWS_AS(fci.setAlpha(0.0), gum::OutOfBounds);
    CHECK_EQ(fci.alpha(), before);
  }

  GUM_TEST(_fci_unstable_mode_valid_) {
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

  GUM_TEST(_fci_max_path_length_zero_valid_) {
    FCIAsiaDB                    db;
    gum::learning::IndepTestChi2 chi2(db.parser, db.prior);
    gum::learning::FCI           fci;
    fci.setIndependenceTest(chi2);
    fci.setMaxPathLength(0);
    const auto pag = fci.learnPAG(db.nodeOnlyGraph());
    CHECK(pag.size() == db.nb_vars);
    CHECK(pag.sizeEdges() <= db.nb_vars * (db.nb_vars - 1) / 2);
  }

  // Phase B: exhaustive sepset flag
  GUM_TEST(_fci_exhaustive_sepset_default_false_) {
    gum::learning::FCI fci;
    CHECK(fci.exhaustiveSepSet() == false);
  }

  GUM_TEST(_fci_exhaustive_sepset_flag_roundtrip_) {
    gum::learning::FCI fci;
    fci.setExhaustiveSepSet(true);
    CHECK(fci.exhaustiveSepSet() == true);
    fci.setExhaustiveSepSet(false);
    CHECK(fci.exhaustiveSepSet() == false);
  }

  GUM_TEST(_fci_exhaustive_sepset_copy_preserves_flag_) {
    gum::learning::FCI fci;
    fci.setExhaustiveSepSet(true);
    const gum::learning::FCI copy(fci);
    CHECK(copy.exhaustiveSepSet() == true);
  }

  GUM_TEST(_fci_exhaustive_sepset_valid_pag_) {
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

  GUM_TEST(_fci_exhaustive_sepset_same_skeleton_as_standard_) {
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

  // Phase A: possibleDSep criterion
  GUM_TEST(_fci_possibledsep_def_collider_included_) {
    // PAG: X(0) *→ B(1) ←* C(2),  X(0) o-o Y(3)
    //   X-B: circle at X from B, arrowhead at B from X → addEdge(0,1, Circle, Arrowhead)
    //   B-C: arrowhead at B from C, circle at C from B → addEdge(1,2, Arrowhead, Circle)
    gum::PAG pag;
    for (gum::NodeId i = 0; i < 4; ++i) {
      pag.addNodeWithId(i);
    }
    pag.addEdge(0, 3, gum::EdgeMark::Circle, gum::EdgeMark::Circle);      // X o-o Y
    pag.addEdge(0, 1, gum::EdgeMark::Circle, gum::EdgeMark::Arrowhead);   // X *→ B
    pag.addEdge(1, 2, gum::EdgeMark::Arrowhead, gum::EdgeMark::Circle);   // C *→ B

    gum::learning::FCI fci;
    const auto         dsep = fci.possibleDSep(pag, 0, 3);

    // B(1): direct neighbor of X → always in possibleDSep
    CHECK(std::find(dsep.begin(), dsep.end(), gum::NodeId(1)) != dsep.end());
    // C(2): B is def collider on (X,B,C) → expansion happens → C in possibleDSep
    CHECK(std::find(dsep.begin(), dsep.end(), gum::NodeId(2)) != dsep.end());
  }

  GUM_TEST(_fci_possibledsep_unreachable_node_excluded_) {
    // PAG: X(0) o-o Y(1),  Z(2) has no edges → unreachable from X via BFS
    gum::PAG pag;
    for (gum::NodeId i = 0; i < 3; ++i) {
      pag.addNodeWithId(i);
    }
    pag.addEdge(0, 1, gum::EdgeMark::Circle, gum::EdgeMark::Circle);   // X o-o Y

    gum::learning::FCI fci;
    const auto         dsep = fci.possibleDSep(pag, 0, 1);

    // Z(2) has no edges → never reachable → not in possibleDSep
    CHECK(std::find(dsep.begin(), dsep.end(), gum::NodeId(2)) == dsep.end());
  }

  GUM_TEST(_fci_possibledsep_circle_tail_node_excluded_) {
    gum::PAG pag;
    for (gum::NodeId i = 0; i < 4; ++i) {
      pag.addNodeWithId(i);
    }
    pag.addEdge(0, 3, gum::EdgeMark::Circle, gum::EdgeMark::Circle);   // X o-o Y
    pag.addEdge(0, 1, gum::EdgeMark::Tail, gum::EdgeMark::Circle);     // X -o B
    pag.addEdge(1, 2, gum::EdgeMark::Tail, gum::EdgeMark::Tail);       // B -- C

    gum::learning::FCI fci;
    const auto         dsep = fci.possibleDSep(pag, 0, 3);

    // B(1): direct neighbor of X → always in possibleDSep
    CHECK(std::find(dsep.begin(), dsep.end(), gum::NodeId(1)) != dsep.end());
    // C(2): circle-tail at B → B not def collider → Zhang does not expand
    // [FAILS with old criterion; PASSES after Phase A]
    CHECK(std::find(dsep.begin(), dsep.end(), gum::NodeId(2)) == dsep.end());
  }

  GUM_TEST(_fci_possibledsep_definite_noncollider_excluded_) {
    // Graph:  X(0) → B(1) → C(2),  X(0) o-o Y(3)
    //   X-B: tail at X-endpoint from B (→ isTail(B,X)=true), arrowhead at B from X
    //       → addEdge(0,1, Circle, Arrowhead)   (circle at X from B, arrowhead at B from X)
    // Actually we need definite non-collider: both marks at B are Tail.
    //   X→B: tail at B from X (actually for directed X→B: markAtX=Tail, markAtB=Arrowhead)
    // A definite non-collider on (X,B,C) means: NOT (both arrowheads at B).
    // The strongest form: both marks at B's endpoint are Tail = X--B--C (undirected chain).
    gum::PAG pag;
    for (gum::NodeId i = 0; i < 4; ++i) {
      pag.addNodeWithId(i);
    }
    pag.addEdge(0, 3, gum::EdgeMark::Circle, gum::EdgeMark::Circle);   // X o-o Y
    pag.addEdge(0, 1, gum::EdgeMark::Tail, gum::EdgeMark::Tail);       // X -- B (Tail-Tail)
    pag.addEdge(1, 2, gum::EdgeMark::Tail, gum::EdgeMark::Tail);       // B -- C (Tail-Tail)

    gum::learning::FCI fci;
    const auto         dsep = fci.possibleDSep(pag, 0, 3);

    // B(1): direct neighbor of X → in possibleDSep
    CHECK(std::find(dsep.begin(), dsep.end(), gum::NodeId(1)) != dsep.end());
    // B has Tail-Tail at B: definite non-collider on (X,B,C)
    // old criterion: !isTail(X,B) || !isTail(C,B) = !Tail || !Tail = false → does NOT expand → C
    // excluded new criterion: isDefCollider(X,B,C) || adj(X,C) = false || false = false → does
    // NOT expand → C excluded BOTH criteria agree: C not in possibleDSep
    CHECK(std::find(dsep.begin(), dsep.end(), gum::NodeId(2)) == dsep.end());
  }

  // Phase C: background knowledge respected in orientation rules
  GUM_TEST(_fci_forbidden_arc_no_crash_) {
    FCIAsiaDB                    db;
    gum::learning::IndepTestChi2 chi2(db.parser, db.prior);
    gum::learning::FCI           fci;
    fci.setIndependenceTest(chi2);

    gum::DiGraph forbid;
    for (gum::NodeId i = 0; i < static_cast< gum::NodeId >(db.nb_vars); ++i) {
      forbid.addNodeWithId(i);
    }
    forbid.addArc(0, 1);   // forbid arc 0→1 (one direction only)
    fci.setForbiddenGraph(forbid);

    CHECK_NOTHROW(fci.learnPAG(db.nodeOnlyGraph()));
  }

  GUM_TEST(_fci_forbidden_arc_arrowhead_blocked_) {
    FCIAsiaDB                    db;
    gum::learning::IndepTestChi2 chi2(db.parser, db.prior);
    gum::learning::FCI           fci;
    fci.setIndependenceTest(chi2);

    // forbid ALL arcs in direction i→j for i < j
    // this prevents any arrowhead at j from i via R0/R1/R2/R4
    gum::DiGraph forbid;
    for (gum::NodeId i = 0; i < static_cast< gum::NodeId >(db.nb_vars); ++i) {
      forbid.addNodeWithId(i);
    }
    for (gum::NodeId i = 0; i < static_cast< gum::NodeId >(db.nb_vars); ++i) {
      for (gum::NodeId j = i + 1; j < static_cast< gum::NodeId >(db.nb_vars); ++j) {
        forbid.addArc(i, j);
      }
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

  GUM_TEST(_fci_forbidden_arc_does_not_affect_unrelated_edge_) {
    FCIAsiaDB                    db;
    gum::learning::IndepTestChi2 chi2a(db.parser, db.prior);
    gum::learning::IndepTestChi2 chi2b(db.parser, db.prior);

    gum::learning::FCI ref_fci;
    ref_fci.setIndependenceTest(chi2a);
    const auto ref_pag = ref_fci.learnPAG(db.nodeOnlyGraph());

    // find a pair not adjacent in ref_pag; add forbidden arc for it
    gum::DiGraph forbid;
    for (gum::NodeId i = 0; i < static_cast< gum::NodeId >(db.nb_vars); ++i) {
      forbid.addNodeWithId(i);
    }
    for (gum::NodeId i = 0; i < static_cast< gum::NodeId >(db.nb_vars); ++i) {
      for (gum::NodeId j = i + 1; j < static_cast< gum::NodeId >(db.nb_vars); ++j) {
        if (!ref_pag.existsEdge(i, j)) {
          forbid.addArc(i, j);   // forbidden arc on non-edge → no effect
          break;
        }
      }
      if (forbid.sizeArcs() > 0) { break; }
    }
    if (forbid.sizeArcs() == 0) { return; }   // no non-adjacent pair found (trivial pass)

    gum::learning::FCI bk_fci;
    bk_fci.setIndependenceTest(chi2b);
    bk_fci.setForbiddenGraph(forbid);
    const auto bk_pag = bk_fci.learnPAG(db.nodeOnlyGraph());

    CHECK(bk_pag.sizeEdges() == ref_pag.sizeEdges());
  }

  // Independence test given by a list of independences (any other pair is dependent)
  struct FCIOracle: public gum::learning::IndepTestChi2 {
    using gum::learning::IndepTestChi2::IndepTestChi2;
    using Independence = std::tuple< gum::NodeId, gum::NodeId, std::vector< gum::NodeId > >;

    std::vector< Independence > independences;   // (x, y, cond) with x < y and cond sorted

    std::pair< double, double > statistics(gum::NodeId                       var1,
                                           gum::NodeId                       var2,
                                           const std::vector< gum::NodeId >& rhs_ids) override {
      const auto [x, y] = std::minmax(var1, var2);
      auto cond         = rhs_ids;
      std::sort(cond.begin(), cond.end());
      const bool indep
          = std::ranges::find(independences, Independence{x, y, cond}) != independences.end();
      return {0.0, indep ? 1.0 : 0.0};
    }
  };

  // the graph with the nodes 0..n-1 and no edge
  inline gum::MixedGraph fciNodes(gum::Size n) {
    gum::MixedGraph graph;
    for (gum::NodeId i = 0; i < n; ++i) {
      graph.addNodeWithId(i);
    }
    return graph;
  }

  // X=0, B=1, C=2, Y=3: X _||_ C, Y _||_ B, Y _||_ C and X _||_ Y | xyCond. Phase 1 keeps X-Y (C is
  // adjacent neither to X nor to Y), Phase 2 orients the collider X *-> B <-* C, so C is in
  // Possible-D-Sep(X, Y) and Phase 3 removes X-Y.
  inline std::vector< FCIOracle::Independence >
      fciPossibleDSepCase(std::vector< gum::NodeId > xyCond) {
    return {{0, 2, {}}, {1, 3, {}}, {2, 3, {}}, {0, 3, std::move(xyCond)}};
  }

  GUM_TEST(_fci_possibledsep_keeps_mandatory_arc_) {
    FCIAsiaDB db;
    FCIOracle oracle(db.parser, db.prior);
    oracle.independences = fciPossibleDSepCase({1, 2});
    const auto graph     = fciNodes(4);

    // without constraint, Phase 3 removes X-Y
    {
      gum::learning::FCI fci;
      fci.setIndependenceTest(oracle);
      CHECK_FALSE(fci.learnPAG(graph).existsEdge(0, 3));
    }

    // a mandatory arc X->Y is kept by learnPAG, learnPDAG and learnDAG
    gum::DAG mandatory;
    for (gum::NodeId i = 0; i < 4; ++i) {
      mandatory.addNodeWithId(i);
    }
    mandatory.addArc(0, 3);
    gum::learning::FCI fci;
    fci.setIndependenceTest(oracle);
    fci.setMandatoryGraph(mandatory);
    CHECK(fci.learnPAG(graph).existsEdge(0, 3));
    CHECK(fci.learnPDAG(graph).existsArc(0, 3));
    CHECK(fci.learnDAG(graph).existsArc(0, 3));
  }

  GUM_TEST(_fci_possibledsep_tests_the_singletons_) {
    FCIAsiaDB db;
    FCIOracle oracle(db.parser, db.prior);
    oracle.independences = fciPossibleDSepCase({2});   // X _||_ Y | {C}

    gum::learning::FCI fci;
    fci.setIndependenceTest(oracle);
    CHECK_FALSE(fci.learnPAG(fciNodes(4)).existsEdge(0, 3));
  }

  // R4 on the discriminating path <D, A, B, C> (D=0, A=1, B=2, C=3): D _||_ B and D _||_ C |
  // dcCond, hence D *-> A <-* B (collider), A -> C (R1) and B o-* C, oriented by R4 according to
  // whether B is in the separating set of D and C
  GUM_TEST(_fci_r4_discriminating_path_) {
    FCIAsiaDB db;
    FCIOracle oracle(db.parser, db.prior);

    // B in the separating set: B -> C
    {
      oracle.independences = {{0, 2, {}}, {0, 3, {1, 2}}};
      gum::learning::FCI fci;
      fci.setIndependenceTest(oracle);
      const auto pag = fci.learnPAG(fciNodes(4));
      CHECK(pag.isTail(3, 2));
      CHECK(pag.isArrowhead(2, 3));
    }
    // B not in the separating set: A <-> B <-> C
    {
      oracle.independences = {{0, 2, {}}, {0, 3, {1}}};
      gum::learning::FCI fci;
      fci.setIndependenceTest(oracle);
      const auto pag = fci.learnPAG(fciNodes(4));
      CHECK(pag.isBidirected(1, 2));
      CHECK(pag.isBidirected(2, 3));
    }
  }

  // R5 on the cycle of circles A-C-X-D-B-A with the chords A-X and X-B (A=0, B=1, C=2, D=3, X=4),
  // without collider: every circle path between the ends of an edge is covered (e.g. <B, D, X, C,
  // A> by B-X and A-X), so R5 orients nothing
  GUM_TEST(_fci_r5_covered_circle_path_) {
    FCIAsiaDB db;
    FCIOracle oracle(db.parser, db.prior);
    oracle.independences = {{0, 3, {1, 4}}, {1, 2, {0, 4}}, {2, 3, {4}}};

    gum::learning::FCI fci;
    fci.setIndependenceTest(oracle);
    const auto pag = fci.learnPAG(fciNodes(5));

    CHECK_EQ(pag.sizeEdges(), gum::Size(7));
    for (const auto& edge: pag.edges()) {
      CHECK(pag.isCircle(edge.first(), edge.second()));
      CHECK(pag.isCircle(edge.second(), edge.first()));
    }
  }

  // R3 on the double triangle A *-> B <-* C, A o-o D o-o C, D o-* B (A=0, B=1, C=2, D=3): A _||_ C
  // given {D}, hence the collider A *-> B <-* C and the non-collider A, D, C, then D *-> B. A
  // forbidden arc D->B gives B -> D from the skeleton on (never D o-* B, so R3 does not apply)
  GUM_TEST(_fci_r3_double_triangle_) {
    FCIAsiaDB db;
    FCIOracle oracle(db.parser, db.prior);
    oracle.independences = {{0, 2, {3}}};

    {
      gum::learning::FCI fci;
      fci.setIndependenceTest(oracle);
      const auto pag = fci.learnPAG(fciNodes(4));
      CHECK(pag.isArrowhead(0, 1));
      CHECK(pag.isArrowhead(2, 1));
      CHECK(pag.isArrowhead(3, 1));
    }
    {
      gum::DiGraph forbidden;
      for (gum::NodeId i = 0; i < 4; ++i) {
        forbidden.addNodeWithId(i);
      }
      forbidden.addArc(3, 1);
      gum::learning::FCI fci;
      fci.setIndependenceTest(oracle);
      fci.setForbiddenGraph(forbidden);
      const auto pag = fci.learnPAG(fciNodes(4));
      CHECK(pag.isArrowhead(0, 1));
      CHECK(pag.isArrowhead(2, 1));
      CHECK_FALSE(pag.isArrowhead(3, 1));
    }
  }

  // an invalid PAG (A=0, B=1, C=2, X=3): the mandatory arcs A->B->C, X _||_ B and X _||_ A | {C},
  // hence the collider X *-> C <- B and C -> A (R1): the directed cycle A->B->C->A
  GUM_TEST(_fci_error_on_invalid_PAG_raised_) {
    FCIAsiaDB db;
    FCIOracle oracle(db.parser, db.prior);
    oracle.independences = {{0, 3, {2}}, {1, 3, {}}};
    gum::DAG mandatory;
    for (gum::NodeId i = 0; i < 4; ++i) {
      mandatory.addNodeWithId(i);
    }
    mandatory.addArc(0, 1);
    mandatory.addArc(1, 2);

    gum::learning::FCI fci;
    fci.setIndependenceTest(oracle);
    fci.setMandatoryGraph(mandatory);
    const auto pag = fci.learnPAG(fciNodes(4));
    CHECK(pag.isArrowhead(2, 0));
    CHECK(pag.isTail(0, 2));

    fci.setErrorOnInvalidPAG(true);
    CHECK_THROWS_AS(fci.learnPDAG(fciNodes(4)), gum::InvalidArgument);
  }

  // the DAG with the nodes 0..n-1 and the given arcs
  inline gum::DAG fciMandatory(gum::Size n, const std::vector< gum::Arc >& arcs) {
    gum::DAG dag;
    for (gum::NodeId i = 0; i < n; ++i) {
      dag.addNodeWithId(i);
    }
    for (const auto& arc: arcs) {
      dag.addArc(arc.tail(), arc.head());
    }
    return dag;
  }

  // R10 (A=0, B=1, C=2, D=3, X=4): the mandatory arcs B->C<-D, B _||_ D | {A}, X _||_ A, B, D,
  // hence A o-> C (collider X *-> C <-* A), A o-o B and A o-o D. The uncovered pd-paths <A, B> and
  // <A, D> start with the non-adjacent nodes B and D: A -> C
  GUM_TEST(_fci_r10_orients_) {
    FCIAsiaDB db;
    FCIOracle oracle(db.parser, db.prior);
    oracle.independences = {{1, 3, {0}}, {0, 4, {}}, {1, 4, {}}, {3, 4, {}}};

    gum::learning::FCI fci;
    fci.setIndependenceTest(oracle);
    fci.setMandatoryGraph(fciMandatory(5, {gum::Arc(1, 2), gum::Arc(3, 2)}));
    const auto pag = fci.learnPAG(fciNodes(5));
    CHECK(pag.isArrowhead(0, 2));
    CHECK(pag.isTail(2, 0));
  }

  // R10 needs uncovered paths (A=0, B=1, C=2, D=3, M=4, X=5): the mandatory arcs B->C<-D, B and D
  // adjacent, M _||_ C and M _||_ D | {A, B}, X _||_ A, B, D, M, hence A o-> C. The only candidates
  // for the first nodes are D and M (B is adjacent to both), and every path from A through D or M
  // to B or D, as <A, M, B> or <A, D, B>, is covered by the edge A-B: A o-> C remains
  GUM_TEST(_fci_r10_needs_uncovered_paths_) {
    FCIAsiaDB db;
    FCIOracle oracle(db.parser, db.prior);
    oracle.independences
        = {{2, 4, {0, 1}}, {3, 4, {0, 1}}, {0, 5, {}}, {1, 5, {}}, {3, 5, {}}, {4, 5, {}}};

    gum::learning::FCI fci;
    fci.setIndependenceTest(oracle);
    fci.setMandatoryGraph(fciMandatory(6, {gum::Arc(1, 2), gum::Arc(3, 2)}));
    const auto pag = fci.learnPAG(fciNodes(6));
    CHECK_EQ(pag.sizeEdges(), gum::Size(9));
    CHECK(pag.isArrowhead(0, 2));
    CHECK(pag.isCircle(2, 0));
  }

} /* namespace gum_tests */
