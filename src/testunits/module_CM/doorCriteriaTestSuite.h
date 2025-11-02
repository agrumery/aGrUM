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
/**
 * @file
 * @brief DoorCriteria (backdoor/frontdoor) test suite.
 */
#pragma once

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/graphs/DAG.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/CM/tools/doorCriteria.h>
#include <agrum/CM/tools/separation.h>

namespace gum_tests {

  class GUM_TEST_SUITE(DoorCriteria) {
    private:
    using NodeSet = gum::NodeSet;

    template < typename S >
    static NodeSet ns(const gum::BayesNet< S >& bn, std::initializer_list< const char* > names) {
      NodeSet s;
      for (auto n: names)
        s.insert(bn.idFromName(n));
      return s;
    }

    public:
    GUM_ACTIVE_TEST(Backdoor_EmptySetWhenNoParents) {
      auto        bn  = gum::BayesNet< double >::fastPrototype("A->B->C");
      const auto& dag = bn.dag();

      gum::DoorCriteria dc(dag);
      auto              idA = bn.idFromName("A");
      auto              idC = bn.idFromName("C");

      gum::NodeSet Z;                               // {}
      TS_ASSERT(dc.satisfiesBackdoorCriterion(idA, idC, Z));

      gum::DoorCriteria::EnumerationOptions opts;   // defaults ok
      auto                                  sets = dc.enumerateBackdoorSets(idA, idC, opts);

      // expected == [{ }]
      gum::DoorCriteria::NodeSetVec expected;
      expected.emplace_back();   // push an empty NodeSet

      TS_ASSERT_EQUALS(sets, expected);

      // Test stopAtFirst returns only the first and is valid
      auto sets_first = dc.enumerateBackdoorSets(idA, idC, opts, true);
      TS_ASSERT_EQUALS(sets_first.size(), 1u);
      TS_ASSERT(dc.satisfiesBackdoorCriterion(idA, idC, sets_first[0]));
    }

    GUM_ACTIVE_TEST(Backdoor_SingleConfounderMinimal) {
      // Classic confounding: U -> X, U -> Y, and X -> Y
      // Minimal backdoor set is {U}
      auto        bn  = gum::BayesNet< double >::fastPrototype("U->X->Y;U->Y");
      const auto& dag = bn.dag();

      gum::DoorCriteria dc(dag);
      auto              idU = bn.idFromName("U");
      auto              idX = bn.idFromName("X");
      auto              idY = bn.idFromName("Y");

      gum::NodeSet Z_bad;
      Z_bad.insert(idX);   // descendant of X ⇒ invalid for backdoor
      TS_ASSERT(!dc.satisfiesBackdoorCriterion(idX, idY, Z_bad));

      gum::NodeSet Z_ok;
      Z_ok.insert(idU);
      TS_ASSERT(dc.satisfiesBackdoorCriterion(idX, idY, Z_ok));

      // enumerate: exactly [{U}]
      auto sets = dc.enumerateBackdoorSets(idX, idY);

      gum::DoorCriteria::NodeSetVec expected;
      expected.push_back(Z_ok);   // [{U}]

      TS_ASSERT_EQUALS(sets, expected);

      // Test stopAtFirst returns only the first and is valid
      auto sets_first = dc.enumerateBackdoorSets(idX, idY, true);
      TS_ASSERT_EQUALS(sets_first.size(), 1u);
      TS_ASSERT(dc.satisfiesBackdoorCriterion(idX, idY, sets_first[0]));
    }

    GUM_ACTIVE_TEST(Backdoor_MinimalityFilter) {
      // U->X, U->Y, W->X (W not confounding Y). {U} is minimal; {U,W} is valid but not minimal.
      auto        bn  = gum::BayesNet< double >::fastPrototype("U->X->Y;U->Y;W->X");
      const auto& dag = bn.dag();

      gum::DoorCriteria dc(dag);
      auto              idU = bn.idFromName("U");
      auto              idW = bn.idFromName("W");
      auto              idX = bn.idFromName("X");
      auto              idY = bn.idFromName("Y");

      gum::DoorCriteria::EnumerationOptions opts_min;   // default only_minimal=true
      auto                                  mins = dc.enumerateBackdoorSets(idX, idY, opts_min);
      TS_ASSERT_EQUALS(mins.size(), 1u);
      TS_ASSERT(mins[0].contains(idU) && mins[0].size() == 1);

      gum::DoorCriteria::EnumerationOptions opts_all;
      opts_all.only_minimal = false;
      auto all              = dc.enumerateBackdoorSets(idX, idY, opts_all);

      // must include {U} and {U,W} (order is deterministic but we just check membership)
      bool has_U  = false;
      bool has_UW = false;
      for (const auto& Z: all) {
        if (Z.size() == 1 && Z.contains(idU)) has_U = true;
        if (Z.size() == 2 && Z.contains(idU) && Z.contains(idW)) has_UW = true;
      }
      TS_ASSERT(has_U);
      TS_ASSERT(has_UW);
    }

    GUM_ACTIVE_TEST(Frontdoor_Classic_Mediator) {
      // Frontdoor canonical: X->Z->Y with hidden confounding U->X, U->Y.
      // {Z} is a valid frontdoor set. Note: Z is a descendant of X — that’s OK.
      auto        bn  = gum::BayesNet< double >::fastPrototype("U->X;U->Y;X->Z->Y");
      const auto& dag = bn.dag();

      gum::DoorCriteria dc(dag);
      auto              idX = bn.idFromName("X");
      auto              idY = bn.idFromName("Y");
      auto              idZ = bn.idFromName("Z");

      NodeSet Z;
      Z.insert(idZ);
      TS_ASSERT(dc.satisfiesFrontdoorCriterion(idX, idY, Z));

      auto fds = dc.enumerateFrontdoorSets(idX, idY);

      // fds should contain exactly {Z} (no other nodes lie on every X→…→Y path)
      gum::DoorCriteria::NodeSetVec fds_expected;
      fds_expected.push_back(Z);
      TS_ASSERT_EQUALS(fds, fds_expected);

      // Test stopAtFirst returns only the first and is valid
      auto fds_first = dc.enumerateFrontdoorSets(idX, idY, true);
      TS_ASSERT_EQUALS(fds_first.size(), 1u);
      TS_ASSERT(dc.satisfiesFrontdoorCriterion(idX, idY, fds_first[0]));
    }

    GUM_ACTIVE_TEST(Frontdoor_FailsWhenZDoesNotInterceptAllDirectedPaths) {
      // X -> Z1 -> Y and X -> Z2 -> Y, confounding U->X, U->Y.
      // {Z1} alone does NOT intercept the path via Z2; likewise {Z2} alone fails.
      // {Z1,Z2} satisfies condition (i); check the other backdoor conditions too.
      auto        bn  = gum::BayesNet< double >::fastPrototype("U->X;U->Y;X->Z1->Y;X->Z2->Y");
      const auto& dag = bn.dag();

      gum::DoorCriteria dc(dag);
      auto              idX  = bn.idFromName("X");
      auto              idY  = bn.idFromName("Y");
      auto              idZ1 = bn.idFromName("Z1");
      auto              idZ2 = bn.idFromName("Z2");

      NodeSet z1;
      z1.insert(idZ1);
      NodeSet z2;
      z2.insert(idZ2);
      NodeSet z12;
      z12.insert(idZ1);
      z12.insert(idZ2);

      TS_ASSERT(!dc.satisfiesFrontdoorCriterion(idX, idY, z1));
      TS_ASSERT(!dc.satisfiesFrontdoorCriterion(idX, idY, z2));

      // {Z1,Z2} should pass (i), and pass (ii) + (iii) backdoor checks
      TS_ASSERT(dc.satisfiesFrontdoorCriterion(idX, idY, z12));

      // enumeration: exactly [{Z1, Z2}]
      auto         fds = dc.enumerateFrontdoorSets(idX, idY);
      gum::NodeSet Z12;
      Z12.insert(idZ1);
      Z12.insert(idZ2);
      gum::DoorCriteria::NodeSetVec expected;
      expected.push_back(Z12);
      TS_ASSERT_EQUALS(fds, expected);
    }

    GUM_ACTIVE_TEST(ExistsUnblockedDirectedPath_and_NodesOnDirectedPaths) {
      // X -> Z1 -> Z2 -> Y, plus a side branch X -> W (not leading to Y)
      auto        bn  = gum::BayesNet< double >::fastPrototype("X->Z1->Z2->Y;X->W");
      const auto& dag = bn.dag();

      gum::DoorCriteria dc(dag);
      auto              idX  = bn.idFromName("X");
      auto              idY  = bn.idFromName("Y");
      auto              idZ1 = bn.idFromName("Z1");
      auto              idZ2 = bn.idFromName("Z2");
      auto              idW  = bn.idFromName("W");

      NodeSet Z_empty;
      NodeSet Z_blockZ1;
      Z_blockZ1.insert(idZ1);

      // There is a directed path X→…→Y if we don't block Z1
      TS_ASSERT(dc.existsUnblockedDirectedPath(idX, idY, Z_empty));
      // Blocking Z1 kills all directed paths
      TS_ASSERT(!dc.existsUnblockedDirectedPath(idX, idY, Z_blockZ1));

      // nodesOnDirectedPaths should return {X,Z1,Z2,Y} (W is not on any X→…→Y path)
      auto onPath = dc.nodesOnDirectedPaths(idX, idY);
      TS_ASSERT(onPath.contains(idX));
      TS_ASSERT(onPath.contains(idZ1));
      TS_ASSERT(onPath.contains(idZ2));
      TS_ASSERT(onPath.contains(idY));
      TS_ASSERT(!onPath.contains(idW));
    }

    GUM_ACTIVE_TEST(Backdoor_DescendantInZIsRejected) {
      // X -> Z, Z -> Y; conditioning on Z is invalid for backdoor sets
      auto        bn  = gum::BayesNet< double >::fastPrototype("X->Z->Y");
      const auto& dag = bn.dag();

      gum::DoorCriteria dc(dag);
      auto              idX = bn.idFromName("X");
      auto              idY = bn.idFromName("Y");
      auto              idZ = bn.idFromName("Z");

      NodeSet Z;
      Z.insert(idZ);
      TS_ASSERT(!dc.satisfiesBackdoorCriterion(idX, idY, Z));

      // enumeration should not include {Z}; {} is valid since X has no parents
      auto                          sets = dc.enumerateBackdoorSets(idX, idY);
      gum::DoorCriteria::NodeSetVec expected;
      expected.emplace_back();   // push an empty NodeSet: {}
      TS_ASSERT_EQUALS(sets, expected);
    }

    GUM_ACTIVE_TEST(Frontdoor_NoDirectedPath_ReturnsEmptyEnumeration) {
      // No directed path X→Y: X and Y disconnected (or only via incoming to X).
      auto        bn  = gum::BayesNet< double >::fastPrototype("A->X;Y->B");
      const auto& dag = bn.dag();

      gum::DoorCriteria dc(dag);
      auto              idX = bn.idFromName("X");
      auto              idY = bn.idFromName("Y");

      auto fds = dc.enumerateFrontdoorSets(idX, idY);
      TS_ASSERT_EQUALS(fds.size(), 0u);
    }

    GUM_ACTIVE_TEST(Backdoor_TwoConfounders_EnumerationAndPruning) {
      // Graph: X<-U1->Y; X<-U2->Y; X->W1->Y; X->W2->Y
      // Backdoor sets must block both X<-U1->Y and X<-U2->Y  ⇒ minimal set = {U1,U2}.
      auto bn = gum::BayesNet< double >::fastPrototype("U1->X;U1->Y;U2->X;U2->Y;X->W1->Y;X->W2->Y");
      const auto&       dag = bn.dag();
      gum::DoorCriteria dc(dag);

      auto idX  = bn.idFromName("X");
      auto idY  = bn.idFromName("Y");
      auto idU1 = bn.idFromName("U1");
      auto idU2 = bn.idFromName("U2");
      auto idW1 = bn.idFromName("W1");   // descendant of X (never allowed in backdoor sets)
      //   auto idW2 = bn.idFromName("W2"); // not used

      // Predicate checks
      gum::NodeSet Z_u1;
      Z_u1.insert(idU1);
      gum::NodeSet Z_u2;
      Z_u2.insert(idU2);
      gum::NodeSet Z_u12;
      Z_u12.insert(idU1);
      Z_u12.insert(idU2);
      gum::NodeSet Z_bad;
      Z_bad.insert(idW1);   // descendant of X ⇒ invalid

      TS_ASSERT(!dc.satisfiesBackdoorCriterion(idX, idY, Z_u1));
      TS_ASSERT(!dc.satisfiesBackdoorCriterion(idX, idY, Z_u2));
      TS_ASSERT(dc.satisfiesBackdoorCriterion(idX, idY, Z_u12));
      TS_ASSERT(!dc.satisfiesBackdoorCriterion(idX, idY, Z_bad));

      // Enumeration (minimal-only): exactly [{U1,U2}]
      auto                          sets_min = dc.enumerateBackdoorSets(idX, idY);
      gum::DoorCriteria::NodeSetVec expected_min;
      expected_min.push_back(Z_u12);
      TS_ASSERT_EQUALS(sets_min, expected_min);

      // Excluding U1 ⇒ no valid backdoor set remains
      gum::DoorCriteria::EnumerationOptions opts_excl;
      opts_excl.excluded_nodes.insert(idU1);
      auto sets_excl = dc.enumerateBackdoorSets(idX, idY, opts_excl);
      TS_ASSERT(sets_excl.empty());

      // Limiting cardinality to 1 ⇒ cannot pick both U1 and U2 ⇒ empty
      gum::DoorCriteria::EnumerationOptions opts_cap1;
      opts_cap1.max_cardinality = 1;
      opts_cap1.only_minimal    = false;   // even with non-minimal, cap=1 blocks {U1,U2}
      auto sets_cap1            = dc.enumerateBackdoorSets(idX, idY, opts_cap1);
      TS_ASSERT(sets_cap1.empty());
    }

    GUM_ACTIVE_TEST(Frontdoor_TwoMediators_EnumerationAndPruning) {
      // Same graph. Frontdoor must intercept both directed paths X->W1->Y and X->W2->Y.
      // Minimal frontdoor set = {W1, W2}. Singles fail (don’t intercept both paths).
      auto bn = gum::BayesNet< double >::fastPrototype("U1->X;U1->Y;U2->X;U2->Y;X->W1->Y;X->W2->Y");
      const auto&       dag = bn.dag();
      gum::DoorCriteria dc(dag);

      auto idX  = bn.idFromName("X");
      auto idY  = bn.idFromName("Y");
      auto idW1 = bn.idFromName("W1");
      auto idW2 = bn.idFromName("W2");

      gum::NodeSet Z_w1;
      Z_w1.insert(idW1);
      gum::NodeSet Z_w2;
      Z_w2.insert(idW2);
      gum::NodeSet Z_w12;
      Z_w12.insert(idW1);
      Z_w12.insert(idW2);

      // Predicate: only {W1,W2} satisfies frontdoor
      TS_ASSERT(!dc.satisfiesFrontdoorCriterion(idX, idY, Z_w1));
      TS_ASSERT(!dc.satisfiesFrontdoorCriterion(idX, idY, Z_w2));
      TS_ASSERT(dc.satisfiesFrontdoorCriterion(idX, idY, Z_w12));

      // Enumeration (minimal-only): exactly [{W1,W2}]
      auto                          fds = dc.enumerateFrontdoorSets(idX, idY);
      gum::DoorCriteria::NodeSetVec expected_fds;
      expected_fds.push_back(Z_w12);
      TS_ASSERT_EQUALS(fds, expected_fds);

      // Excluding W1 ⇒ cannot intercept both paths ⇒ empty
      gum::DoorCriteria::EnumerationOptions opts_excl;
      opts_excl.excluded_nodes.insert(idW1);
      auto fds_excl = dc.enumerateFrontdoorSets(idX, idY, opts_excl);
      TS_ASSERT(fds_excl.empty());

      // Limiting cardinality to 1 ⇒ cannot pick both mediators ⇒ empty
      gum::DoorCriteria::EnumerationOptions opts_cap1;
      opts_cap1.max_cardinality = 1;
      opts_cap1.only_minimal    = false;   // even allowing non-minimal, cap=1 blocks {W1,W2}
      auto fds_cap1             = dc.enumerateFrontdoorSets(idX, idY, opts_cap1);
      TS_ASSERT(fds_cap1.empty());
    }

    GUM_ACTIVE_TEST(Backdoor_HasBackdoorPath_Equivalence) {
      auto              bn  = gum::BayesNet< double >::fastPrototype("U->X;U->Y");
      const auto&       dag = bn.dag();
      gum::DoorCriteria dc(dag);

      auto idU = bn.idFromName("U");
      auto idX = bn.idFromName("X");
      auto idY = bn.idFromName("Y");

      gum::NodeSet Z0;
      TS_ASSERT(dc.hasBackdoorPath(idX, idY, Z0));
      TS_ASSERT(!dc.satisfiesBackdoorCriterion(idX, idY, Z0));

      gum::NodeSet ZU;
      ZU.insert(idU);
      TS_ASSERT(!dc.hasBackdoorPath(idX, idY, ZU));
      TS_ASSERT(dc.satisfiesBackdoorCriterion(idX, idY, ZU));
    }

    GUM_ACTIVE_TEST(Frontdoor_RejectsZInBackdoorReach) {
      auto              bn  = gum::BayesNet< double >::fastPrototype("U->X;U->Z;Z->Y;X->Y");
      const auto&       dag = bn.dag();
      gum::DoorCriteria dc(dag);

      auto idX = bn.idFromName("X");
      auto idY = bn.idFromName("Y");
      auto idZ = bn.idFromName("Z");

      gum::NodeSet Z;
      Z.insert(idZ);
      TS_ASSERT(!dc.satisfiesFrontdoorCriterion(idX, idY, Z));

      auto fds = dc.enumerateFrontdoorSets(idX, idY);
      TS_ASSERT(fds.empty());
    }

    GUM_ACTIVE_TEST(Frontdoor_PrunesImpossibleByFD3) {
      auto              bn  = gum::BayesNet< double >::fastPrototype("X->Z->Y;V->Z;V->Y");
      const auto&       dag = bn.dag();
      gum::DoorCriteria dc(dag);

      auto idX = bn.idFromName("X");
      auto idY = bn.idFromName("Y");
      auto idZ = bn.idFromName("Z");

      gum::NodeSet Z;
      Z.insert(idZ);
      TS_ASSERT(!dc.satisfiesFrontdoorCriterion(idX, idY, Z));

      auto fds = dc.enumerateFrontdoorSets(idX, idY);
      TS_ASSERT_EQUALS(fds.size(), 0u);
    }

    GUM_ACTIVE_TEST(Frontdoor_NoDiPath_PrunesParentsOfX) {
      auto              bn  = gum::BayesNet< double >::fastPrototype("M->X;M->Y");
      const auto&       dag = bn.dag();
      gum::DoorCriteria dc(dag);

      auto idX = bn.idFromName("X");
      auto idY = bn.idFromName("Y");
      auto fds = dc.enumerateFrontdoorSets(idX, idY);

      TS_ASSERT(fds.empty());
    }

    GUM_ACTIVE_TEST(BackdoorReach_ContainsParents) {
      auto              bn  = gum::BayesNet< double >::fastPrototype("M->X;M->Y");
      const auto&       dag = bn.dag();
      gum::DoorCriteria dc(dag);

      auto idX = bn.idFromName("X");
      auto idM = bn.idFromName("M");

      auto br = dc.backdoorReach(idX);
      TS_ASSERT(br.contains(idM));   // parents of X must be in backdoor reach
    }

    GUM_ACTIVE_TEST(ExistsUnblockedDirectedPath_EndpointsNotBlocked) {
      auto              bn  = gum::BayesNet< double >::fastPrototype("X->Y");
      const auto&       dag = bn.dag();
      gum::DoorCriteria dc(dag);

      auto idX = bn.idFromName("X");
      auto idY = bn.idFromName("Y");

      gum::NodeSet Z;
      Z.insert(idY);
      TS_ASSERT(dc.existsUnblockedDirectedPath(idX, idY, Z));
    }

    GUM_ACTIVE_TEST(Backdoor_DeterministicOrderAndNoDup) {
      auto              bn  = gum::BayesNet< double >::fastPrototype("U->X;U->Y;V->X");
      const auto&       dag = bn.dag();
      gum::DoorCriteria dc(dag);

      auto idX = bn.idFromName("X");
      auto idY = bn.idFromName("Y");

      gum::DoorCriteria::EnumerationOptions opts;
      opts.only_minimal = false;
      auto sets         = dc.enumerateBackdoorSets(idX, idY, opts);

      auto toVec = [](const gum::NodeSet& s) {
        std::vector< gum::NodeId > v;
        for (auto n: s)
          v.push_back(n);
        std::sort(v.begin(), v.end());
        return v;
      };
      std::vector< std::vector< gum::NodeId > > vs;
      for (const auto& s: sets)
        vs.push_back(toVec(s));
      auto vs_sorted = vs;
      std::sort(vs_sorted.begin(), vs_sorted.end());
      vs_sorted.erase(std::unique(vs_sorted.begin(), vs_sorted.end()), vs_sorted.end());
      TS_ASSERT_EQUALS(vs, vs_sorted);
    }

    GUM_ACTIVE_TEST(Backdoor_Rejects_X_or_Y_in_Z) {
      // Graph with confounding and a direct edge.
      auto        bn  = gum::BayesNet< double >::fastPrototype("U->X->Y;U->Y");
      const auto& dag = bn.dag();

      gum::DoorCriteria dc(dag);
      const auto        idX = bn.idFromName("X");
      const auto        idY = bn.idFromName("Y");
      const auto        idU = bn.idFromName("U");

      // Z that contains X is invalid
      gum::NodeSet ZX;
      ZX.insert(idX);
      TS_ASSERT(!dc.satisfiesBackdoorCriterion(idX, idY, ZX));

      // Z that contains Y is invalid
      gum::NodeSet ZY;
      ZY.insert(idY);
      TS_ASSERT(!dc.satisfiesBackdoorCriterion(idX, idY, ZY));

      // Control: {U} is valid
      gum::NodeSet ZU;
      ZU.insert(idU);
      TS_ASSERT(dc.satisfiesBackdoorCriterion(idX, idY, ZU));
    }

    GUM_ACTIVE_TEST(Frontdoor_Rejects_X_or_Y_in_Z) {
      // Canonical frontdoor structure.
      auto              bn  = gum::BayesNet< double >::fastPrototype("U->X;U->Y;X->Z->Y");
      const auto&       dag = bn.dag();
      gum::DoorCriteria dc(dag);

      const auto idX = bn.idFromName("X");
      const auto idY = bn.idFromName("Y");
      const auto idZ = bn.idFromName("Z");

      // Z that contains X or Y should be rejected for frontdoor.
      gum::NodeSet ZX;
      ZX.insert(idX);
      gum::NodeSet ZY;
      ZY.insert(idY);
      TS_ASSERT(!dc.satisfiesFrontdoorCriterion(idX, idY, ZX));
      TS_ASSERT(!dc.satisfiesFrontdoorCriterion(idX, idY, ZY));

      // Control: {Z} is valid.
      gum::NodeSet ZZ;
      ZZ.insert(idZ);
      TS_ASSERT(dc.satisfiesFrontdoorCriterion(idX, idY, ZZ));
    }

    GUM_ACTIVE_TEST(Frontdoor_Fails_With_Direct_XY_Edge) {
      // Add a direct edge X->Y : FD-1 ("intercept all directed paths") should fail for {Z}.
      auto              bn  = gum::BayesNet< double >::fastPrototype("U->X;U->Y;X->Z->Y;X->Y");
      const auto&       dag = bn.dag();
      gum::DoorCriteria dc(dag);

      const auto idX = bn.idFromName("X");
      const auto idY = bn.idFromName("Y");
      const auto idZ = bn.idFromName("Z");

      gum::NodeSet ZZ;
      ZZ.insert(idZ);
      TS_ASSERT(!dc.satisfiesFrontdoorCriterion(idX, idY, ZZ));

      // Enumeration should be empty in this case.
      auto fds = dc.enumerateFrontdoorSets(idX, idY);
      TS_ASSERT_EQUALS(fds.size(), 0u);
    }

    GUM_ACTIVE_TEST(Frontdoor_Enumeration_MinimalVsSupersets_Z3Hub) {
      // X->Z1->Z3 and X->Z2->Z3, with Z3->Y; U->X, U->Y (confounding).
      // All directed X->Y paths pass through Z3, so {Z3} is inclusion-minimal.
      // {Z1,Z2} also intercepts all directed paths (each singleton fails), and is also
      // inclusion-minimal.
      auto bn = gum::BayesNet< double >::fastPrototype("U->X;U->Y;X->Z1->Z3;X->Z2->Z3;Z3->Y");

      const auto&       dag = bn.dag();
      gum::DoorCriteria dc(dag);

      const auto idX  = bn.idFromName("X");
      const auto idY  = bn.idFromName("Y");
      const auto idZ1 = bn.idFromName("Z1");
      const auto idZ2 = bn.idFromName("Z2");
      const auto idZ3 = bn.idFromName("Z3");

      gum::NodeSet Z1;
      Z1.insert(idZ1);
      gum::NodeSet Z2;
      Z2.insert(idZ2);
      gum::NodeSet Z12;
      Z12.insert(idZ1);
      Z12.insert(idZ2);
      gum::NodeSet Z3;
      Z3.insert(idZ3);

      // Predicate checks (criterion itself)
      TS_ASSERT(!dc.satisfiesFrontdoorCriterion(idX, idY, Z1));
      TS_ASSERT(!dc.satisfiesFrontdoorCriterion(idX, idY, Z2));
      TS_ASSERT(dc.satisfiesFrontdoorCriterion(idX, idY, Z3));
      TS_ASSERT(dc.satisfiesFrontdoorCriterion(idX,
                                               idY,
                                               Z12));   // inclusion-minimal but larger than {Z3}

      // ---------- Minimal enumeration (only_minimal=true) ----------
      {
        auto fds_min = dc.enumerateFrontdoorSets(idX, idY);   // default: only_minimal=true
        TS_ASSERT(!fds_min.empty());

        // Presence: both {Z3} and {Z1,Z2} should appear (both are inclusion-minimal)
        auto contains_min = [&](const gum::NodeSet& target) {
          return std::any_of(fds_min.begin(), fds_min.end(), [&](const gum::NodeSet& S) {
            return S == target;
          });
        };
        TS_ASSERT(contains_min(Z3));
        TS_ASSERT(contains_min(Z12));

        // Inclusion-minimality: no set is a proper subset of another
        auto isProperSubset = [](const gum::NodeSet& A, const gum::NodeSet& B) -> bool {
          if (A.size() >= B.size()) return false;
          for (auto n: A)
            if (!B.contains(n)) return false;
          return true;
        };
        for (size_t i = 0; i < fds_min.size(); ++i) {
          for (size_t j = 0; j < fds_min.size(); ++j)
            if (i != j) { TS_ASSERT(!isProperSubset(fds_min[i], fds_min[j])); }
        }
      }

      // ---------- Non-minimal enumeration (only_minimal=false) ----------
      {
        gum::DoorCriteria::EnumerationOptions opts_all;
        opts_all.only_minimal = false;
        auto fds_all          = dc.enumerateFrontdoorSets(idX, idY, opts_all);

        // Must include at least the two minimal solutions
        auto contains_all = [&](const gum::NodeSet& target) {
          return std::any_of(fds_all.begin(), fds_all.end(), [&](const gum::NodeSet& S) {
            return S == target;
          });
        };
        TS_ASSERT(contains_all(Z3));
        TS_ASSERT(contains_all(Z12));

        // Dedup + deterministic order (same style as your backdoor test)
        auto toVec = [](const gum::NodeSet& s) {
          std::vector< gum::NodeId > v;
          for (auto n: s)
            v.push_back(n);
          std::sort(v.begin(), v.end());
          return v;
        };
        std::vector< std::vector< gum::NodeId > > vs;
        for (const auto& s: fds_all)
          vs.push_back(toVec(s));
        auto vs_sorted = vs;
        std::sort(vs_sorted.begin(), vs_sorted.end());
        vs_sorted.erase(std::unique(vs_sorted.begin(), vs_sorted.end()), vs_sorted.end());
        TS_ASSERT_EQUALS(vs, vs_sorted);
      }
    }

    GUM_ACTIVE_TEST(Frontdoor_Enumeration_Excludes_XY) {
      using BN = gum::BayesNet< double >;

      // Canonical frontdoor: U->X; U->Y; X->Z->Y
      auto              bn  = BN::fastPrototype("U->X;U->Y;X->Z->Y");
      const auto&       dag = bn.dag();
      gum::DoorCriteria dc(dag);

      const auto idX = bn.idFromName("X");
      const auto idY = bn.idFromName("Y");
      const auto idZ = bn.idFromName("Z");

      // --- Predicate sanity: Z must be disjoint from {X,Y}
      gum::NodeSet ZX;
      ZX.insert(idX);
      gum::NodeSet ZY;
      ZY.insert(idY);
      gum::NodeSet ZZ;
      ZZ.insert(idZ);

      TS_ASSERT(!dc.satisfiesFrontdoorCriterion(idX, idY, ZX));
      TS_ASSERT(!dc.satisfiesFrontdoorCriterion(idX, idY, ZY));
      TS_ASSERT(dc.satisfiesFrontdoorCriterion(idX, idY, ZZ));

      // --- Enumeration (minimal) must return exactly {Z}
      auto fds_min = dc.enumerateFrontdoorSets(idX, idY);   // only_minimal=true (default)
      TS_ASSERT_EQUALS(fds_min.size(), 1u);
      TS_ASSERT(fds_min[0] == ZZ);

      // --- Enumeration (non-minimal) must NEVER include X or Y
      gum::DoorCriteria::EnumerationOptions opts_all;
      opts_all.only_minimal = false;
      auto fds_all          = dc.enumerateFrontdoorSets(idX, idY, opts_all);

      // Positive control: {Z} still present
      auto contains = [&](const gum::NodeSet& target) {
        return std::any_of(fds_all.begin(), fds_all.end(), [&](const gum::NodeSet& S) {
          return S == target;
        });
      };
      TS_ASSERT(contains(ZZ));

      // Negative controls: sets that include X or Y must be absent
      gum::NodeSet ZXZ;
      ZXZ.insert(idX);
      ZXZ.insert(idZ);
      gum::NodeSet ZYZ;
      ZYZ.insert(idY);
      ZYZ.insert(idZ);
      TS_ASSERT(!contains(ZX));
      TS_ASSERT(!contains(ZY));
      TS_ASSERT(!contains(ZXZ));
      TS_ASSERT(!contains(ZYZ));

      // Generic guard: none of the enumerated sets contains X or Y
      for (const auto& S: fds_all) {
        TS_ASSERT(!S.contains(idX));
        TS_ASSERT(!S.contains(idY));
      }
    }

    GUM_ACTIVE_TEST(Frontdoor_NoDirectedPath_EnumeratesSingletons) {
      using BN = gum::BayesNet< double >;

      // No directed path X→Y; they are in the same weakly connected component:
      //   X → A ← B → Y
      auto              bn  = BN::fastPrototype("X->A;B->A;B->Y");
      const auto&       dag = bn.dag();
      gum::DoorCriteria dc(dag);

      const auto idX = bn.idFromName("X");
      const auto idY = bn.idFromName("Y");
      const auto idA = bn.idFromName("A");
      const auto idB = bn.idFromName("B");

      // Sanity: truly no directed path X→Y (unblocked by ∅).
      gum::NodeSet none;
      TS_ASSERT(!dc.existsUnblockedDirectedPath(idX, idY, none));

      // Minimal enumeration (default): should return exactly {B}.
      auto fds_min = dc.enumerateFrontdoorSets(idX, idY);
      TS_ASSERT_EQUALS(fds_min.size(), 1u);
      gum::NodeSet ZB;
      ZB.insert(idB);
      TS_ASSERT(fds_min[0] == ZB);

      // Non-minimal mode still must not introduce A, X, or Y.
      gum::DoorCriteria::EnumerationOptions opts_all;
      opts_all.only_minimal = false;
      auto fds_all          = dc.enumerateFrontdoorSets(idX, idY, opts_all);
      TS_ASSERT_EQUALS(fds_all.size(), 1u);
      TS_ASSERT(fds_all[0] == ZB);

      // Guard: A is pruned by FD-3 (A ← B → Y is a backdoor from A to Y).
      gum::NodeSet ZA;
      ZA.insert(idA);
      auto contains = [&](const gum::NodeSet& target) {
        return std::any_of(fds_all.begin(), fds_all.end(), [&](const gum::NodeSet& S) {
          return S == target;
        });
      };
      TS_ASSERT(!contains(ZA));

      // Guard: no set should ever contain X or Y.
      for (const auto& S: fds_all) {
        TS_ASSERT(!S.contains(idX));
        TS_ASSERT(!S.contains(idY));
      }
    }
  };

}   // namespace gum_tests
