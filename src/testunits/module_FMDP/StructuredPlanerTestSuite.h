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


// ==============================================================================
#include <iostream>
#include <memory>
#include <string>
// ==============================================================================
#include <utils.h>

#include <testunits/gumtest/AgrumTestSuite.h>
// ==============================================================================
#include <agrum/base/multidim/implementations/multiDimFunctionGraph.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/FMDP/fmdp.h>
#include <agrum/FMDP/io/dat/fmdpDatReader.h>
#include <agrum/FMDP/planning/structuredPlaner.h>

// ==============================================================================

namespace gum_tests {

  struct StructuredPlanerTestSuite {
    static void run(const std::string& file) {
      gum::FMDP< double >              fmdp(true);
      gum::StructuredPlaner< double >* planer = nullptr;
      GUM_CHECK_ASSERT_THROWS_NOTHING(
          planer = gum::StructuredPlaner< double >::spumddInstance(0.9, 0.01, false));

      gum::FMDPDatReader< double > reader(&fmdp, file);
      GUM_CHECK_ASSERT_THROWS_NOTHING(reader.trace(false));
      GUM_CHECK_ASSERT_THROWS_NOTHING(reader.proceed());

      GUM_CHECK_ASSERT_THROWS_NOTHING(planer->initialize(&fmdp));
      GUM_CHECK_ASSERT_THROWS_NOTHING(planer->makePlanning(10));


      std::ofstream _traceAlgoSaveFile_;
      _traceAlgoSaveFile_.open(GET_RESSOURCES_PATH("outputs/PlannedPolicy.dot"),
                               std::ios::out | std::ios::trunc);
      if (!_traceAlgoSaveFile_) return;
      GUM_CHECK_ASSERT_THROWS_NOTHING(_traceAlgoSaveFile_ << fmdp.toString());
      _traceAlgoSaveFile_.close();

      int deletedFile = std::remove(GET_RESSOURCES_PATH("outputs/PlannedPolicy.dot"));
      if (deletedFile != 0) std::cout << "Couldn't delete output file." << std::endl;

      GUM_CHECK_ASSERT_THROWS_NOTHING(delete planer);
    }

    public:
    static void planWithActionDependentRewards(bool withDefaultReward) {
      // Bellman backup: Q(s,a) = R(s,a) + gamma * E[V(s')|s,a], then V(s) = max_a Q(s,a).
      // Both actions leave the state unchanged; "good" pays 10 and "bad" pays 1, so the optimal
      // policy is "good" everywhere and V = 10 / (1 - gamma) = 100.
      gum::FMDP< double > fmdp(true);
      auto*               x = new gum::LabelizedVariable("x", "", 2);
      fmdp.addVariable(x);
      const auto* xp = fmdp.main2prime(x);
      fmdp.addAction(1, "bad");
      fmdp.addAction(2, "good");

      // a function of x only
      auto reward = [x](double r) {
        auto* fg = gum::MultiDimFunctionGraph< double >::getReducedAndOrderedInstance();
        fg->add(*x);
        const auto root = fg->manager()->addInternalNode(x);
        fg->manager()->setRootNode(root);
        const auto leaf = fg->manager()->addTerminalNode(r);
        for (gum::Idx m = 0; m < x->domainSize(); ++m)
          fg->manager()->setSon(root, m, leaf);
        return fg;
      };
      // P(x'|x) = [x' == x]
      auto identity = [x, xp]() {
        auto* fg = gum::MultiDimFunctionGraph< double >::getReducedAndOrderedInstance();
        fg->add(*x);
        fg->add(*xp);
        const auto root = fg->manager()->addInternalNode(x);
        fg->manager()->setRootNode(root);
        const auto zero = fg->manager()->addTerminalNode(0.0);
        const auto one  = fg->manager()->addTerminalNode(1.0);
        for (gum::Idx m = 0; m < x->domainSize(); ++m) {
          const auto nxp = fg->manager()->addInternalNode(xp);
          fg->manager()->setSon(root, m, nxp);
          for (gum::Idx mp = 0; mp < xp->domainSize(); ++mp)
            fg->manager()->setSon(nxp, mp, (m == mp) ? one : zero);
        }
        return fg;
      };

      if (withDefaultReward) fmdp.addReward(reward(0.0));
      fmdp.addRewardForAction(1, reward(1.0));
      fmdp.addRewardForAction(2, reward(10.0));
      fmdp.addTransitionForAction(1, x, identity());
      fmdp.addTransitionForAction(2, x, identity());

      auto planer = std::unique_ptr< gum::StructuredPlaner< double > >(
          gum::StructuredPlaner< double >::spumddInstance(0.9, 0.0001, false));
      planer->initialize(&fmdp);
      planer->makePlanning(1000);

      gum::Instantiation I;
      I.add(*x);
      for (I.setFirst(); !I.end(); I.inc()) {
        CHECK_EQ(planer->vFunction()->get(I), doctest::Approx(100.0).epsilon(0.01));
        const auto actions = planer->optimalPolicy()->get(I);
        CHECK_EQ(actions.size(), static_cast< gum::Size >(1));
        CHECK(actions.exists(2));
      }
    }

    // trees: the model has to use tree function graphs (as SVI does) instead of MDDs
    static void planWithFrameVariable(gum::StructuredPlaner< double >* planer, bool trees) {
      // f has no transition at all (frame variable: it keeps its value). "good" pays 10 when
      // f = 1 and 0 otherwise, "bad" always pays 1. Hence V(f=0) = 1 / (1 - gamma) = 10 with
      // "bad" and V(f=1) = 10 / (1 - gamma) = 100 with "good".
      gum::FMDP< double > fmdp(true);
      auto*               f = new gum::LabelizedVariable("f", "", 2);
      fmdp.addVariable(f);
      fmdp.addAction(1, "bad");
      fmdp.addAction(2, "good");

      auto reward = [f, trees](double r0, double r1) {
        auto* fg = trees ? gum::MultiDimFunctionGraph< double >::getTreeInstance()
                         : gum::MultiDimFunctionGraph< double >::getReducedAndOrderedInstance();
        fg->add(*f);
        const auto root = fg->manager()->addInternalNode(f);
        fg->manager()->setRootNode(root);
        fg->manager()->setSon(root, 0, fg->manager()->addTerminalNode(r0));
        fg->manager()->setSon(root, 1, fg->manager()->addTerminalNode(r1));
        return fg;
      };
      fmdp.addRewardForAction(1, reward(1.0, 1.0));
      fmdp.addRewardForAction(2, reward(0.0, 10.0));

      auto owner = std::unique_ptr< gum::StructuredPlaner< double > >(planer);
      planer->initialize(&fmdp);
      planer->makePlanning(1000);

      gum::Instantiation I;
      I.add(*f);
      I.chgVal(*f, 0);
      CHECK_EQ(planer->vFunction()->get(I), doctest::Approx(10.0).epsilon(0.01));
      auto actions = planer->optimalPolicy()->get(I);
      CHECK_EQ(actions.size(), static_cast< gum::Size >(1));
      CHECK(actions.exists(1));
      I.chgVal(*f, 1);
      CHECK_EQ(planer->vFunction()->get(I), doctest::Approx(100.0).epsilon(0.01));
      actions = planer->optimalPolicy()->get(I);
      CHECK_EQ(actions.size(), static_cast< gum::Size >(1));
      CHECK(actions.exists(2));
    }
  };

  GUM_TEST(PlanningCoffee) { run(GET_RESSOURCES_PATH("FMDP/coffee/coffee.dat")); }

  GUM_TEST(PlanningTinyFactory) { run(GET_RESSOURCES_PATH("FMDP/factory/tiny-factory.dat")); }

  GUM_TEST(PlanningTaxi) { run(GET_RESSOURCES_PATH("FMDP/taxi/taxi.dat")); }

  GUM_TEST(PlanningWithActionDependentRewards) { planWithActionDependentRewards(true); }

  GUM_TEST(PlanningWithActionDependentRewardsOnly) {
    // no action-independent (default) reward: every action has its own reward
    planWithActionDependentRewards(false);
  }

  GUM_TEST(PlanningWithFrameVariableSPUMDD) {
    planWithFrameVariable(gum::StructuredPlaner< double >::spumddInstance(0.9, 0.0001, false),
                          false);
  }

  GUM_TEST(PlanningWithFrameVariableSVI) {
    planWithFrameVariable(gum::StructuredPlaner< double >::sviInstance(0.9, 0.0001, false), true);
  }
}   // namespace gum_tests
