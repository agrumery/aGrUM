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

} /* namespace gum_tests */
