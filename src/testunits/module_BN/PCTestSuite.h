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
                                    std::string action,
                                    std::string explain) {
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
  };

  GUM_TEST_ACTIF(_asia_chi2_)
  GUM_TEST_ACTIF(_asia_g2_)
  GUM_TEST_ACTIF(_asia_stable_vs_unstable_)
  GUM_TEST_ACTIF(_asia_ForbiddenGraph_)
  GUM_TEST_ACTIF(_asia_MandatoryGraph_)
  GUM_TEST_ACTIF(_forbidden_pairs_not_in_skeleton_)

} /* namespace gum_tests */
