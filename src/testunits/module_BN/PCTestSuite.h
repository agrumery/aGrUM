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
#include <agrum/base/graphs/mixedGraph.h>
#include <agrum/base/stattests/indepTestChi2.h>
#include <agrum/base/stattests/indepTestG2.h>
#include <agrum/BN/learning/PC.h>
#include <agrum/BN/learning/priors/noPrior.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  PC
#define GUM_CURRENT_MODULE BN

namespace gum_tests {

  class CountingListenerForPC: public gum::Listener {
    public:
    int forbidden_count{0};
    int mandatory_count{0};

    void whenStructuralModification(const void* src,
                                    gum::NodeId x,
                                    gum::NodeId y,
                                    std::string action,   // NOLINT(performance-unnecessary-value-param)
                                    std::string explain) {  // NOLINT(performance-unnecessary-value-param)
      if (explain.find("Forbidden edge") != std::string::npos) { ++forbidden_count; }
      if (explain.find("Mandatory") != std::string::npos) { ++mandatory_count; }
    }
  };

  // helper: build database and parser from asia.csv
  struct AsiaDB {
    gum::learning::DBInitializerFromCSV initializer;
    std::size_t                         nb_vars;
    gum::learning::DBTranslatorSet      translator_set;
    gum::learning::DatabaseTable        database;
    gum::learning::DBRowGeneratorSet    genset;
    gum::learning::DBRowGeneratorParser parser;
    gum::learning::NoPrior              prior;

    AsiaDB() :
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

  struct PCTestSuite {
    public:
    // =========================================================================
    // Basic correctness — phases 1-2-3
    // =========================================================================

    static void test_asia_chi2_() {
      AsiaDB                       db;
      gum::learning::IndepTestChi2 chi2(db.parser, db.prior);

      gum::learning::PC search;
      search.setIndependenceTest(chi2);

      gum::MixedGraph graph = db.nodeOnlyGraph();

      try {
        auto mg = search.learnMixedStructure(graph);
        CHECK(mg.size() == db.nb_vars);
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK(false);
      }

      try {
        auto pdag = search.learnPDAG(graph);
        CHECK(pdag.size() == db.nb_vars);
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK(false);
      }

      try {
        auto dag = search.learnDAG(graph);
        CHECK(dag.size() == db.nb_vars);
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK(false);
      }
    }

    static void test_asia_g2_() {
      AsiaDB                     db;
      gum::learning::IndepTestG2 g2(db.parser, db.prior);

      gum::learning::PC search;
      search.setIndependenceTest(g2);

      gum::MixedGraph graph = db.nodeOnlyGraph();

      try {
        auto mg = search.learnMixedStructure(graph);
        CHECK(mg.size() == db.nb_vars);
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK(false);
      }

      try {
        auto dag = search.learnDAG(graph);
        CHECK(dag.size() == db.nb_vars);
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK(false);
      }
    }

    static void test_asia_stable_vs_unstable_() {
      AsiaDB                       db;
      gum::learning::IndepTestChi2 chi2s(db.parser, db.prior);
      gum::learning::IndepTestChi2 chi2u(db.parser, db.prior);

      gum::learning::PC stable_search;
      stable_search.setIndependenceTest(chi2s);
      stable_search.setStable(true);

      gum::learning::PC unstable_search;
      unstable_search.setIndependenceTest(chi2u);
      unstable_search.setStable(false);

      gum::MixedGraph graph = db.nodeOnlyGraph();

      try {
        auto dag_stable   = stable_search.learnDAG(graph);
        auto dag_unstable = unstable_search.learnDAG(graph);
        CHECK(dag_stable.size() == db.nb_vars);
        CHECK(dag_unstable.size() == db.nb_vars);
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK(false);
      }
    }

    // =========================================================================
    // Structural constraints
    // =========================================================================

    static void test_asia_ForbiddenGraph_() {
      AsiaDB                       db;
      gum::learning::IndepTestChi2 chi2(db.parser, db.prior);

      gum::learning::PC search;
      search.setIndependenceTest(chi2);

      gum::MixedGraph graph = db.nodeOnlyGraph();
      gum::DiGraph    forbidGraph;
      for (gum::Size i = 0; i < db.nb_vars; ++i) {
        forbidGraph.addNodeWithId(i);
      }

      // (2,6): fully forbidden edge (both directions)
      // (1,5): asymmetric — only 1->5 forbidden
      forbidGraph.addArc(2, 6);
      forbidGraph.addArc(6, 2);
      forbidGraph.addArc(1, 5);

      search.setForbiddenGraph(forbidGraph);

      CountingListenerForPC counter;
      GUM_CONNECT(search,
                  onStructuralModification,
                  counter,
                  CountingListenerForPC::whenStructuralModification);

      auto mg = search.learnMixedStructure(graph);

      // fully forbidden edge (2,6) must be absent in both orientations
      CHECK(!mg.existsEdge(2, 6));
      CHECK(!mg.existsArc(2, 6));
      CHECK(!mg.existsArc(6, 2));
      // asymmetric: arc 1->5 forbidden, 5->1 could exist
      CHECK(!mg.existsArc(1, 5));
      // initGraph_ must have emitted a signal for the fully forbidden edge
      CHECK(counter.forbidden_count >= 1);

      auto dag = search.learnDAG(graph);
      CHECK(!dag.existsArc(2, 6));
      CHECK(!dag.existsArc(6, 2));
      CHECK(!dag.existsArc(1, 5));
    }

    static void test_asia_MandatoryGraph_() {
      AsiaDB                       db;
      gum::learning::IndepTestChi2 chi2(db.parser, db.prior);

      gum::learning::PC search;
      search.setIndependenceTest(chi2);

      gum::MixedGraph graph = db.nodeOnlyGraph();
      gum::DAG        mandaGraph;
      for (gum::Size i = 0; i < db.nb_vars; ++i) {
        mandaGraph.addNodeWithId(i);
      }

      mandaGraph.addArc(3, 4);
      search.setMandatoryGraph(mandaGraph);

      CountingListenerForPC counter;
      GUM_CONNECT(search,
                  onStructuralModification,
                  counter,
                  CountingListenerForPC::whenStructuralModification);

      auto mg = search.learnMixedStructure(graph);
      CHECK(mg.existsArc(3, 4));
      CHECK(counter.mandatory_count >= 1);

      auto pdag = search.learnPDAG(graph);
      CHECK(pdag.existsArc(3, 4));

      auto dag = search.learnDAG(graph);
      CHECK(dag.existsArc(3, 4));
    }

    static void test_forbidden_pairs_not_in_skeleton_() {
      // fully forbidden pairs must not appear in skeleton — initGraph_ removes them
      // before any CI test, so they are never in the graph
      AsiaDB                       db;
      gum::learning::IndepTestChi2 chi2(db.parser, db.prior);

      gum::learning::PC search;
      search.setIndependenceTest(chi2);

      gum::MixedGraph graph = db.nodeOnlyGraph();
      gum::DiGraph    forbidGraph;
      for (gum::Size i = 0; i < db.nb_vars; ++i) {
        forbidGraph.addNodeWithId(i);
      }

      // fully forbidden: (0,7) in both directions
      forbidGraph.addArc(0, 7);
      forbidGraph.addArc(7, 0);
      search.setForbiddenGraph(forbidGraph);

      auto skel = search.learnSkeleton(graph);

      // pair (0,7) must be absent from skeleton in all forms
      CHECK(!skel.existsEdge(0, 7));
      CHECK(!skel.existsArc(0, 7));
      CHECK(!skel.existsArc(7, 0));
    }

    static void test_parameter_accessors_() {
      // verify setters/getters round-trip — these will move to CIBasedLearning
      gum::learning::PC search;

      CHECK(search.alpha() == doctest::Approx(0.05));
      search.setAlpha(0.01);
      CHECK(search.alpha() == doctest::Approx(0.01));
      search.setAlpha(0.05);
      CHECK(search.alpha() == doctest::Approx(0.05));

      CHECK(search.ucPriority() == gum::learning::PC::UCPriority::Standard);
      search.setUCPriority(gum::learning::PC::UCPriority::Sorted);
      CHECK(search.ucPriority() == gum::learning::PC::UCPriority::Sorted);
    }

    // =========================================================================
    // Phase 1 — skeleton properties
    // =========================================================================

    static void test_learnSkeleton_is_undirected_() {
      // skeleton output must contain zero arcs — purely undirected
      AsiaDB                       db;
      gum::learning::IndepTestChi2 chi2(db.parser, db.prior);

      gum::learning::PC search;
      search.setIndependenceTest(chi2);

      auto skel = search.learnSkeleton(db.nodeOnlyGraph());
      CHECK(skel.sizeArcs() == 0);
      CHECK(skel.size() == db.nb_vars);
    }

    static void test_alpha_one_skeleton_complete_() {
      // alpha=1.0: condition pval > 1.0 is never satisfied → no edge removed
      AsiaDB                       db;
      gum::learning::IndepTestChi2 chi2(db.parser, db.prior);

      gum::learning::PC search;
      search.setIndependenceTest(chi2);
      search.setAlpha(1.0);

      auto         skel = search.learnSkeleton(db.nodeOnlyGraph());
      auto n = static_cast< gum::Size >(db.nb_vars);
      CHECK(skel.sizeEdges() == n * (n - 1) / 2);
    }

    static void test_skeleton_depth0_monotone_in_alpha_() {
      // at depth 0 (maxCondSetSize=0), only marginal tests: edge removed iff pval > alpha
      // lower alpha (0.001) → edge removed if pval > 0.001 → more edges removed → sparser
      // higher alpha (0.5)  → edge removed if pval > 0.5   → fewer edges removed → denser
      // monotonicity is guaranteed at depth 0 because there is no sequential coupling
      // (note: with deeper conditioning the property can break — see plan §Step 0 note)
      AsiaDB                       db;
      gum::learning::IndepTestChi2 chi2_strict(db.parser, db.prior);
      gum::learning::IndepTestChi2 chi2_lenient(db.parser, db.prior);

      gum::learning::PC search_strict;
      search_strict.setIndependenceTest(chi2_strict);
      search_strict.setAlpha(0.001);
      search_strict.setMaxCondSetSize(0);

      gum::learning::PC search_lenient;
      search_lenient.setIndependenceTest(chi2_lenient);
      search_lenient.setAlpha(0.5);
      search_lenient.setMaxCondSetSize(0);

      gum::MixedGraph graph        = db.nodeOnlyGraph();
      auto            skel_strict  = search_strict.learnSkeleton(graph);
      auto            skel_lenient = search_lenient.learnSkeleton(graph);

      CHECK(skel_strict.sizeEdges() <= skel_lenient.sizeEdges());
    }

    static void test_maxCondSetSize_zero_not_fewer_edges_() {
      // maxCondSetSize=0: only depth-0 (marginal) tests, no conditioning
      // → same or more edges than with unlimited conditioning depth
      AsiaDB                       db;
      gum::learning::IndepTestChi2 chi2_unlimited(db.parser, db.prior);
      gum::learning::IndepTestChi2 chi2_zero(db.parser, db.prior);

      gum::learning::PC search_unlimited;
      search_unlimited.setIndependenceTest(chi2_unlimited);

      gum::learning::PC search_zero;
      search_zero.setIndependenceTest(chi2_zero);
      search_zero.setMaxCondSetSize(0);

      gum::MixedGraph graph          = db.nodeOnlyGraph();
      auto            skel_unlimited = search_unlimited.learnSkeleton(graph);
      auto            skel_zero      = search_zero.learnSkeleton(graph);

      CHECK(skel_zero.sizeEdges() >= skel_unlimited.sizeEdges());
    }

    static void test_single_node_graph_() {
      // 1-node graph: no pairs to test, skeleton and DAG have 1 node, 0 edges
      AsiaDB                       db;
      gum::learning::IndepTestChi2 chi2(db.parser, db.prior);

      gum::learning::PC search;
      search.setIndependenceTest(chi2);

      gum::MixedGraph g;
      g.addNodeWithId(0);

      auto skel = search.learnSkeleton(g);
      CHECK(skel.size() == 1);
      CHECK(skel.sizeEdges() == 0);

      auto dag = search.learnDAG(g);
      CHECK(dag.size() == 1);
      CHECK(dag.sizeArcs() == 0);
    }

    // =========================================================================
    // Phase 2 — v-structure orientation
    // =========================================================================

    static void test_orientation_produces_arcs_() {
      // asia has known v-structures; orientation must produce at least one arc
      AsiaDB                       db;
      gum::learning::IndepTestChi2 chi2(db.parser, db.prior);

      gum::learning::PC search;
      search.setIndependenceTest(chi2);

      auto mg = search.learnMixedStructure(db.nodeOnlyGraph());
      CHECK(mg.sizeArcs() > 0);
    }

    static void test_latentVariables_accessible_after_learn_() {
      // latentVariables() is callable after learnMixedStructure
      // on clean data, typically empty (no forced orientation conflicts)
      AsiaDB                       db;
      gum::learning::IndepTestChi2 chi2(db.parser, db.prior);

      gum::learning::PC search;
      search.setIndependenceTest(chi2);

      search.learnMixedStructure(db.nodeOnlyGraph());
      auto lv = search.latentVariables();
      // result is accessible (size >= 0); exact value depends on data
      CHECK(lv.size() >= 0);
    }

    // =========================================================================
    // UCPriority
    // =========================================================================

    static void test_sorted_ucpriority_basic_() {
      AsiaDB                       db;
      gum::learning::IndepTestChi2 chi2(db.parser, db.prior);

      gum::learning::PC search;
      search.setIndependenceTest(chi2);
      search.setUCPriority(gum::learning::PC::UCPriority::Sorted);

      CHECK(search.ucPriority() == gum::learning::PC::UCPriority::Sorted);

      gum::MixedGraph graph = db.nodeOnlyGraph();

      try {
        auto mg = search.learnMixedStructure(graph);
        CHECK(mg.size() == db.nb_vars);
        auto lv = search.latentVariables();
        CHECK(lv.size() >= 0);
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK(false);
      }

      try {
        auto dag = search.learnDAG(graph);
        CHECK(dag.size() == db.nb_vars);
      } catch (gum::Exception& e) {
        GUM_SHOWERROR(e);
        CHECK(false);
      }
    }

    static void test_sorted_same_skeleton_as_standard_() {
      // skeleton phase is identical for both UCPriority modes
      AsiaDB                       db;
      gum::learning::IndepTestChi2 chi2a(db.parser, db.prior);
      gum::learning::IndepTestChi2 chi2b(db.parser, db.prior);

      gum::learning::PC std_search;
      std_search.setIndependenceTest(chi2a);
      std_search.setUCPriority(gum::learning::PC::UCPriority::Standard);

      gum::learning::PC srt_search;
      srt_search.setIndependenceTest(chi2b);
      srt_search.setUCPriority(gum::learning::PC::UCPriority::Sorted);

      gum::MixedGraph graph = db.nodeOnlyGraph();

      auto skel_std = std_search.learnSkeleton(graph);
      auto skel_srt = srt_search.learnSkeleton(graph);

      CHECK(skel_std.sizeEdges() == skel_srt.sizeEdges());
      for (const auto& edge: skel_std.edges()) {
        CHECK(skel_srt.existsEdge(edge.first(), edge.second()));
      }
    }

    static void test_default_ucpriority_is_standard_() {
      gum::learning::PC search;
      CHECK(search.ucPriority() == gum::learning::PC::UCPriority::Standard);

      search.setUCPriority(gum::learning::PC::UCPriority::Sorted);
      CHECK(search.ucPriority() == gum::learning::PC::UCPriority::Sorted);

      search.setUCPriority(gum::learning::PC::UCPriority::Standard);
      CHECK(search.ucPriority() == gum::learning::PC::UCPriority::Standard);
    }

    // =========================================================================
    // Copy / move
    // =========================================================================

    static void test_copy_constructor_same_result_() {
      // copy of PC with same (non-owning) test pointer produces identical skeleton
      AsiaDB                       db;
      gum::learning::IndepTestChi2 chi2(db.parser, db.prior);

      gum::learning::PC search1;
      search1.setIndependenceTest(chi2);

      gum::MixedGraph graph = db.nodeOnlyGraph();
      auto            dag1  = search1.learnDAG(graph);

      gum::learning::PC search2 = search1;   // copy — test_ shares pointer to chi2 (still valid)
      auto              dag2    = search2.learnDAG(graph);

      CHECK(dag1.sizeArcs() == dag2.sizeArcs());
      CHECK(dag1.size() == dag2.size());
    }

    static void test_move_constructor_same_result_() {
      // move transfers test_ pointer; moved-from object has null test_
      // moved-into object produces same result as original
      AsiaDB                       db;
      gum::learning::IndepTestChi2 chi2_ref(db.parser, db.prior);
      gum::learning::IndepTestChi2 chi2_move(db.parser, db.prior);

      gum::learning::PC ref;
      ref.setIndependenceTest(chi2_ref);

      gum::MixedGraph graph   = db.nodeOnlyGraph();
      auto            dag_ref = ref.learnDAG(graph);

      gum::learning::PC src;
      src.setIndependenceTest(chi2_move);
      gum::learning::PC dst = std::move(src);   // move: dst.test_ = &chi2_move

      auto dag_moved = dst.learnDAG(graph);

      CHECK(dag_ref.sizeArcs() == dag_moved.sizeArcs());
      CHECK(dag_ref.size() == dag_moved.size());
    }

    // =========================================================================
    // Phase B — exhaustive sepset flag
    // (accessor tests compile once setExhaustiveSepSet / exhaustiveSepSet added)
    // =========================================================================

    // flag defaults to false
    static void test_exhaustive_sepset_default_false_() {
      gum::learning::PC search;
      CHECK(search.exhaustiveSepSet() == false);
    }

    // flag round-trips through setter/getter
    static void test_exhaustive_sepset_flag_roundtrip_() {
      gum::learning::PC search;
      search.setExhaustiveSepSet(true);
      CHECK(search.exhaustiveSepSet() == true);
      search.setExhaustiveSepSet(false);
      CHECK(search.exhaustiveSepSet() == false);
    }

    // flag is preserved through copy construction
    static void test_exhaustive_sepset_copy_preserves_flag_() {
      gum::learning::PC search;
      search.setExhaustiveSepSet(true);
      const gum::learning::PC copy(search);
      CHECK(copy.exhaustiveSepSet() == true);
    }

    // exhaustive mode must not crash; result has correct node count
    static void test_exhaustive_sepset_produces_valid_result_() {
      AsiaDB                       db;
      gum::learning::IndepTestChi2 chi2(db.parser, db.prior);
      gum::learning::PC            search;
      search.setIndependenceTest(chi2);
      search.setExhaustiveSepSet(true);
      const auto mg = search.learnMixedStructure(db.nodeOnlyGraph());
      CHECK(mg.size() == db.nb_vars);
    }

    // exhaustive flag must not change the SKELETON — only sepset content differs
    static void test_exhaustive_sepset_same_skeleton_as_standard_() {
      AsiaDB                       db;
      gum::learning::IndepTestChi2 chi2a(db.parser, db.prior);
      gum::learning::IndepTestChi2 chi2b(db.parser, db.prior);

      gum::learning::PC standard_search;
      standard_search.setIndependenceTest(chi2a);
      standard_search.setExhaustiveSepSet(false);

      gum::learning::PC exhaustive_search;
      exhaustive_search.setIndependenceTest(chi2b);
      exhaustive_search.setExhaustiveSepSet(true);

      const auto skel_std = standard_search.learnSkeleton(db.nodeOnlyGraph());
      const auto skel_exh = exhaustive_search.learnSkeleton(db.nodeOnlyGraph());

      CHECK(skel_std.sizeEdges() == skel_exh.sizeEdges());
      for (const auto& e: skel_std.edges()) {
        CHECK(skel_exh.existsEdge(e.first(), e.second()));
      }
    }
  };

  GUM_TEST_ACTIF(_asia_chi2_)
  GUM_TEST_ACTIF(_asia_g2_)
  GUM_TEST_ACTIF(_asia_stable_vs_unstable_)
  GUM_TEST_ACTIF(_asia_ForbiddenGraph_)
  GUM_TEST_ACTIF(_asia_MandatoryGraph_)
  GUM_TEST_ACTIF(_forbidden_pairs_not_in_skeleton_)
  GUM_TEST_ACTIF(_parameter_accessors_)
  GUM_TEST_ACTIF(_learnSkeleton_is_undirected_)
  GUM_TEST_ACTIF(_alpha_one_skeleton_complete_)
  GUM_TEST_ACTIF(_skeleton_depth0_monotone_in_alpha_)
  GUM_TEST_ACTIF(_maxCondSetSize_zero_not_fewer_edges_)
  GUM_TEST_ACTIF(_single_node_graph_)
  GUM_TEST_ACTIF(_orientation_produces_arcs_)
  GUM_TEST_ACTIF(_latentVariables_accessible_after_learn_)
  GUM_TEST_ACTIF(_sorted_ucpriority_basic_)
  GUM_TEST_ACTIF(_sorted_same_skeleton_as_standard_)
  GUM_TEST_ACTIF(_default_ucpriority_is_standard_)
  GUM_TEST_ACTIF(_copy_constructor_same_result_)
  GUM_TEST_ACTIF(_move_constructor_same_result_)

  // Phase B: exhaustive sepset flag
  GUM_TEST_ACTIF(_exhaustive_sepset_default_false_)
  GUM_TEST_ACTIF(_exhaustive_sepset_flag_roundtrip_)
  GUM_TEST_ACTIF(_exhaustive_sepset_copy_preserves_flag_)
  GUM_TEST_ACTIF(_exhaustive_sepset_produces_valid_result_)
  GUM_TEST_ACTIF(_exhaustive_sepset_same_skeleton_as_standard_)

} /* namespace gum_tests */
