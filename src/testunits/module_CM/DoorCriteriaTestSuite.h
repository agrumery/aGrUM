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

/**
 * @file
 * @brief DoorCriteria (backdoor/frontdoor) test suite.
 */
#pragma once
#include <ranges>

#include <gumtest/AgrumTestSuite.h>

#include <agrum/base/graphs/DAG.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/CM/doorCriteria.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  DoorCriteria
#define GUM_CURRENT_MODULE CM

namespace gum_tests {

  struct DoorCriteriaTestSuite {
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
    static void testBackdoor_EmptySetWhenNoParents() {
      auto        bn  = gum::BayesNet< double >::fastPrototype("A->B->C");
      const auto& dag = bn.dag();


      auto idA = bn.idFromName("A");
      auto idC = bn.idFromName("C");

      gum::NodeSet Z;   // {}
      CHECK(gum::DoorCriteria::satisfiesBackdoorCriterion(dag, idA, idC, Z));

      auto sets = gum::DoorCriteria::enumerateBackdoorSets(dag, idA, idC);

      // expected == [{ }]
      gum::DoorCriteria::NodeSetVec expected;
      expected.emplace_back();   // push an empty NodeSet

      CHECK_EQ(sets, expected);

      // Test stopAtFirst returns only the first and is valid
      auto sets_first
          = gum::DoorCriteria::enumerateBackdoorSets(dag, idA, idC, gum::NodeSet{}, 0, true, true);
      CHECK_EQ(sets_first.size(), 1u);
      CHECK(gum::DoorCriteria::satisfiesBackdoorCriterion(dag, idA, idC, sets_first[0]));
    }

    static void testBackdoor_SingleConfounderMinimal() {
      // Classic confounding: U -> X, U -> Y, and X -> Y
      // Minimal backdoor set is {U}
      auto        bn  = gum::BayesNet< double >::fastPrototype("U->X->Y;U->Y");
      const auto& dag = bn.dag();


      auto idU = bn.idFromName("U");
      auto idX = bn.idFromName("X");
      auto idY = bn.idFromName("Y");

      gum::NodeSet Z_bad;
      Z_bad.insert(idX);   // descendant of X ⇒ invalid for backdoor
      CHECK(!gum::DoorCriteria::satisfiesBackdoorCriterion(dag, idX, idY, Z_bad));

      gum::NodeSet Z_ok;
      Z_ok.insert(idU);
      CHECK(gum::DoorCriteria::satisfiesBackdoorCriterion(dag, idX, idY, Z_ok));

      // enumerate: exactly [{U}]
      auto sets = gum::DoorCriteria::enumerateBackdoorSets(dag, idX, idY);

      gum::DoorCriteria::NodeSetVec expected;
      expected.push_back(Z_ok);   // [{U}]

      CHECK_EQ(sets, expected);

      // Test stopAtFirst returns only the first and is valid
      auto sets_first
          = gum::DoorCriteria::enumerateBackdoorSets(dag, idX, idY, gum::NodeSet{}, 0, true, true);
      CHECK_EQ(sets_first.size(), 1u);
      CHECK(gum::DoorCriteria::satisfiesBackdoorCriterion(dag, idX, idY, sets_first[0]));
    }

    static void testBackdoor_MinimalityFilter() {
      // U->X, U->Y, W->X (W not confounding Y). {U} is minimal; {U,W} is valid but not minimal.
      auto        bn  = gum::BayesNet< double >::fastPrototype("U->X->Y;U->Y;W->X");
      const auto& dag = bn.dag();


      auto idU = bn.idFromName("U");
      auto idW = bn.idFromName("W");
      auto idX = bn.idFromName("X");
      auto idY = bn.idFromName("Y");

      auto mins = gum::DoorCriteria::enumerateBackdoorSets(dag, idX, idY);
      CHECK_EQ(mins.size(), 1u);
      CHECK(mins[0].contains(idU));
      CHECK_EQ(mins[0].size(), 1u);

      auto all = gum::DoorCriteria::enumerateBackdoorSets(dag, idX, idY, gum::NodeSet{}, 0, false);

      // must include {U} and {U,W} (order is deterministic but we just check membership)
      bool has_U  = false;
      bool has_UW = false;
      for (const auto& Z: all) {
        if (Z.size() == 1 && Z.contains(idU)) has_U = true;
        if (Z.size() == 2 && Z.contains(idU) && Z.contains(idW)) has_UW = true;
      }
      CHECK(has_U);
      CHECK(has_UW);
    }

    static void testFrontdoor_Classic_Mediator() {
      // Frontdoor canonical: X->Z->Y with hidden confounding U->X, U->Y.
      // {Z} is a valid frontdoor set. Note: Z is a descendant of X — that’s OK.
      auto        bn  = gum::BayesNet< double >::fastPrototype("U->X;U->Y;X->Z->Y");
      const auto& dag = bn.dag();


      auto idX = bn.idFromName("X");
      auto idY = bn.idFromName("Y");
      auto idZ = bn.idFromName("Z");

      NodeSet Z;
      Z.insert(idZ);
      CHECK(gum::DoorCriteria::satisfiesFrontdoorCriterion(dag, idX, idY, Z));

      auto fds = gum::DoorCriteria::enumerateFrontdoorSets(dag, idX, idY);

      // fds should contain exactly {Z} (no other nodes lie on every X→…→Y path)
      gum::DoorCriteria::NodeSetVec fds_expected;
      fds_expected.push_back(Z);
      CHECK_EQ(fds, fds_expected);

      // Test stopAtFirst returns only the first and is valid
      auto fds_first
          = gum::DoorCriteria::enumerateFrontdoorSets(dag, idX, idY, gum::NodeSet{}, 0, true, true);
      CHECK_EQ(fds_first.size(), 1u);
      CHECK(gum::DoorCriteria::satisfiesFrontdoorCriterion(dag, idX, idY, fds_first[0]));
    }

    static void testFrontdoor_FailsWhenZDoesNotInterceptAllDirectedPaths() {
      // X -> Z1 -> Y and X -> Z2 -> Y, confounding U->X, U->Y.
      // {Z1} alone does NOT intercept the path via Z2; likewise {Z2} alone fails.
      // {Z1,Z2} satisfies condition (i); check the other backdoor conditions too.
      auto        bn  = gum::BayesNet< double >::fastPrototype("U->X;U->Y;X->Z1->Y;X->Z2->Y");
      const auto& dag = bn.dag();


      auto idX  = bn.idFromName("X");
      auto idY  = bn.idFromName("Y");
      auto idZ1 = bn.idFromName("Z1");
      auto idZ2 = bn.idFromName("Z2");

      NodeSet z1;
      z1.insert(idZ1);
      NodeSet z2;
      z2.insert(idZ2);
      NodeSet z12;
      z12.insert(idZ1);
      z12.insert(idZ2);

      CHECK(!gum::DoorCriteria::satisfiesFrontdoorCriterion(dag, idX, idY, z1));
      CHECK(!gum::DoorCriteria::satisfiesFrontdoorCriterion(dag, idX, idY, z2));

      // {Z1,Z2} should pass (i), and pass (ii) + (iii) backdoor checks
      CHECK(gum::DoorCriteria::satisfiesFrontdoorCriterion(dag, idX, idY, z12));

      // enumeration: exactly [{Z1, Z2}]
      auto         fds = gum::DoorCriteria::enumerateFrontdoorSets(dag, idX, idY);
      gum::NodeSet Z12;
      Z12.insert(idZ1);
      Z12.insert(idZ2);
      gum::DoorCriteria::NodeSetVec expected;
      expected.push_back(Z12);
      CHECK_EQ(fds, expected);
    }

    static void testExistsUnblockedDirectedPath_and_NodesOnDirectedPaths() {
      // X -> Z1 -> Z2 -> Y, plus a side branch X -> W (not leading to Y)
      auto        bn  = gum::BayesNet< double >::fastPrototype("X->Z1->Z2->Y;X->W");
      const auto& dag = bn.dag();


      auto idX  = bn.idFromName("X");
      auto idY  = bn.idFromName("Y");
      auto idZ1 = bn.idFromName("Z1");
      auto idZ2 = bn.idFromName("Z2");
      auto idW  = bn.idFromName("W");

      NodeSet Z_empty;
      NodeSet Z_blockZ1;
      Z_blockZ1.insert(idZ1);

      // There is a directed path X→…→Y if we don't block Z1
      CHECK(gum::DoorCriteria::existsUnblockedDirectedPath(dag, idX, idY, Z_empty));
      // Blocking Z1 kills all directed paths
      CHECK(!gum::DoorCriteria::existsUnblockedDirectedPath(dag, idX, idY, Z_blockZ1));

      // nodesOnDirectedPaths should return {X,Z1,Z2,Y} (W is not on any X→…→Y path)
      auto onPath = gum::DoorCriteria::nodesOnDirectedPaths(dag, idX, idY);
      // CHECK(onPath->contains(idX)); // pyagrum version does not include endpoints; see implementation
      CHECK(onPath->contains(idZ1));
      CHECK(onPath->contains(idZ2));
      // CHECK(onPath->contains(idY)); // pyagrum version does not include endpoints; see implementation
      CHECK(!onPath->contains(idW));
    }

    static void testBackdoor_DescendantInZIsRejected() {
      // X -> Z, Z -> Y; conditioning on Z is invalid for backdoor sets
      auto        bn  = gum::BayesNet< double >::fastPrototype("X->Z->Y");
      const auto& dag = bn.dag();


      auto idX = bn.idFromName("X");
      auto idY = bn.idFromName("Y");
      auto idZ = bn.idFromName("Z");

      NodeSet Z;
      Z.insert(idZ);
      CHECK(!gum::DoorCriteria::satisfiesBackdoorCriterion(dag, idX, idY, Z));

      // enumeration should not include {Z}; {} is valid since X has no parents
      auto                          sets = gum::DoorCriteria::enumerateBackdoorSets(dag, idX, idY);
      gum::DoorCriteria::NodeSetVec expected;
      expected.emplace_back();   // push an empty NodeSet: {}
      CHECK_EQ(sets, expected);
    }

    static void testBackdoor_pyAgrumExample1() {
      // X -> Z, Z -> Y; conditioning on Z is invalid for backdoor sets
      auto bn = gum::BayesNet< double >::fastPrototype(
          "N0<-N1->N2;N0<-N3->N2;N0<-N4->N2;N2->N0;N1->N4;N1<-N5->N6<-N0");
      const auto& dag = bn.dag();


      auto idN0 = bn.idFromName("N0");
      auto idN1 = bn.idFromName("N1");

      // enumeration should not include {Z}; {} is valid since X has no parents
      auto sets = gum::DoorCriteria::enumerateBackdoorSets(dag, idN1, idN0, gum::NodeSet{});
      gum::DoorCriteria::NodeSetVec expected;
      expected.emplace_back();
      CHECK_EQ(sets, expected);
    }

    static void testFrontdoor_NoDirectedPath_ReturnsEmptyEnumeration() {
      // No directed path X→Y: X and Y disconnected (or only via incoming to X).
      auto        bn  = gum::BayesNet< double >::fastPrototype("A->X;Y->B");
      const auto& dag = bn.dag();


      auto idX = bn.idFromName("X");
      auto idY = bn.idFromName("Y");

      auto fds = gum::DoorCriteria::enumerateFrontdoorSets(dag, idX, idY);
      CHECK_EQ(fds.size(), 0u);
    }

    static void testBackdoor_TwoConfounders_EnumerationAndPruning() {
      // Graph: X<-U1->Y; X<-U2->Y; X->W1->Y; X->W2->Y
      // Backdoor sets must block both X<-U1->Y and X<-U2->Y  ⇒ minimal set = {U1,U2}.
      auto bn = gum::BayesNet< double >::fastPrototype("U1->X;U1->Y;U2->X;U2->Y;X->W1->Y;X->W2->Y");
      const auto& dag = bn.dag();


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

      CHECK(!gum::DoorCriteria::satisfiesBackdoorCriterion(dag, idX, idY, Z_u1));
      CHECK(!gum::DoorCriteria::satisfiesBackdoorCriterion(dag, idX, idY, Z_u2));
      CHECK(gum::DoorCriteria::satisfiesBackdoorCriterion(dag, idX, idY, Z_u12));
      CHECK(!gum::DoorCriteria::satisfiesBackdoorCriterion(dag, idX, idY, Z_bad));

      // Enumeration (minimal-only): exactly [{U1,U2}]
      auto sets_min = gum::DoorCriteria::enumerateBackdoorSets(dag, idX, idY);
      gum::DoorCriteria::NodeSetVec expected_min;
      expected_min.push_back(Z_u12);
      CHECK_EQ(sets_min, expected_min);

      // Excluding U1 ⇒ no valid backdoor set remains
      gum::NodeSet excl_U1;
      excl_U1.insert(idU1);
      auto sets_excl = gum::DoorCriteria::enumerateBackdoorSets(dag, idX, idY, excl_U1);
      CHECK(sets_excl.empty());

      // Limiting cardinality to 1 ⇒ cannot pick both U1 and U2 ⇒ empty
      auto sets_cap1 = gum::DoorCriteria::enumerateBackdoorSets(
          dag,
          idX,
          idY,
          gum::NodeSet{},
          1,
          false);   // even with non-minimal, cap=1 blocks {U1,U2}
      CHECK(sets_cap1.empty());
    }

    static void testFrontdoor_TwoMediators_EnumerationAndPruning() {
      // Same graph. Frontdoor must intercept both directed paths X->W1->Y and X->W2->Y.
      // Minimal frontdoor set = {W1, W2}. Singles fail (don’t intercept both paths).
      auto bn = gum::BayesNet< double >::fastPrototype("U1->X;U1->Y;U2->X;U2->Y;X->W1->Y;X->W2->Y");
      const auto& dag = bn.dag();


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
      CHECK(!gum::DoorCriteria::satisfiesFrontdoorCriterion(dag, idX, idY, Z_w1));
      CHECK(!gum::DoorCriteria::satisfiesFrontdoorCriterion(dag, idX, idY, Z_w2));
      CHECK(gum::DoorCriteria::satisfiesFrontdoorCriterion(dag, idX, idY, Z_w12));

      // Enumeration (minimal-only): exactly [{W1,W2}]
      auto                          fds = gum::DoorCriteria::enumerateFrontdoorSets(dag, idX, idY);
      gum::DoorCriteria::NodeSetVec expected_fds;
      expected_fds.push_back(Z_w12);
      CHECK_EQ(fds, expected_fds);

      // Excluding W1 ⇒ cannot intercept both paths ⇒ empty
      gum::NodeSet excl_W1;
      excl_W1.insert(idW1);
      auto fds_excl = gum::DoorCriteria::enumerateFrontdoorSets(dag, idX, idY, excl_W1);
      CHECK(fds_excl.empty());

      // Limiting cardinality to 1 ⇒ cannot pick both mediators ⇒ empty
      auto fds_cap1 = gum::DoorCriteria::enumerateFrontdoorSets(
          dag,
          idX,
          idY,
          gum::NodeSet{},
          1,
          false);   // even allowing non-minimal, cap=1 blocks {W1,W2}
      CHECK(fds_cap1.empty());
    }

    static void testBackdoor_HasBackdoorPath_Equivalence() {
      auto        bn  = gum::BayesNet< double >::fastPrototype("U->X;U->Y");
      const auto& dag = bn.dag();


      auto idU = bn.idFromName("U");
      auto idX = bn.idFromName("X");
      auto idY = bn.idFromName("Y");

      gum::NodeSet Z0;
      CHECK(gum::DoorCriteria::hasBackdoorPath(dag, idX, idY, Z0));
      CHECK(!gum::DoorCriteria::satisfiesBackdoorCriterion(dag, idX, idY, Z0));

      gum::NodeSet ZU;
      ZU.insert(idU);
      CHECK(!gum::DoorCriteria::hasBackdoorPath(dag, idX, idY, ZU));
      CHECK(gum::DoorCriteria::satisfiesBackdoorCriterion(dag, idX, idY, ZU));
    }

    static void testFrontdoor_RejectsZInBackdoorReach() {
      auto        bn  = gum::BayesNet< double >::fastPrototype("U->X;U->Z;Z->Y;X->Y");
      const auto& dag = bn.dag();


      auto idX = bn.idFromName("X");
      auto idY = bn.idFromName("Y");
      auto idZ = bn.idFromName("Z");

      gum::NodeSet Z;
      Z.insert(idZ);
      CHECK(!gum::DoorCriteria::satisfiesFrontdoorCriterion(dag, idX, idY, Z));

      auto fds = gum::DoorCriteria::enumerateFrontdoorSets(dag, idX, idY);
      CHECK(fds.empty());
    }

    static void testFrontdoor_PrunesImpossibleByFD3() {
      auto        bn  = gum::BayesNet< double >::fastPrototype("X->Z->Y;V->Z;V->Y");
      const auto& dag = bn.dag();


      auto idX = bn.idFromName("X");
      auto idY = bn.idFromName("Y");
      auto idZ = bn.idFromName("Z");

      gum::NodeSet Z;
      Z.insert(idZ);
      CHECK(!gum::DoorCriteria::satisfiesFrontdoorCriterion(dag, idX, idY, Z));

      auto fds = gum::DoorCriteria::enumerateFrontdoorSets(dag, idX, idY);
      CHECK_EQ(fds.size(), 0u);
    }

    static void testFrontdoor_NoDiPath_PrunesParentsOfX() {
      auto        bn  = gum::BayesNet< double >::fastPrototype("M->X;M->Y");
      const auto& dag = bn.dag();


      auto idX = bn.idFromName("X");
      auto idY = bn.idFromName("Y");
      auto fds = gum::DoorCriteria::enumerateFrontdoorSets(dag, idX, idY);

      CHECK(fds.empty());
    }

    static void testBackdoorReach_ContainsParents() {
      auto        bn  = gum::BayesNet< double >::fastPrototype("M->X;M->Y");
      const auto& dag = bn.dag();


      auto idX = bn.idFromName("X");
      auto idM = bn.idFromName("M");

      auto br = gum::DoorCriteria::backdoorReach(dag, idX);
      CHECK(br.contains(idM));   // parents of X must be in backdoor reach
    }

    static void testExistsUnblockedDirectedPath_EndpointsNotBlocked() {
      auto        bn  = gum::BayesNet< double >::fastPrototype("X->Y");
      const auto& dag = bn.dag();


      auto idX = bn.idFromName("X");
      auto idY = bn.idFromName("Y");

      gum::NodeSet Z;
      Z.insert(idY);
      CHECK(gum::DoorCriteria::existsUnblockedDirectedPath(dag, idX, idY, Z));
    }

    static void testBackdoor_DeterministicOrderAndNoDup() {
      auto        bn  = gum::BayesNet< double >::fastPrototype("U->X;U->Y;V->X");
      const auto& dag = bn.dag();


      auto idX = bn.idFromName("X");
      auto idY = bn.idFromName("Y");

      auto sets = gum::DoorCriteria::enumerateBackdoorSets(dag, idX, idY, gum::NodeSet{}, 0, false);

      auto toVec = [](const gum::NodeSet& s) {
        std::vector< gum::NodeId > v;
        for (auto n: s)
          v.push_back(n);
        std::ranges::sort(v);
        return v;
      };
      std::vector< std::vector< gum::NodeId > > vs;
      for (const auto& s: sets)
        vs.push_back(toVec(s));
      auto vs_sorted = vs;
      std::ranges::sort(vs_sorted);
      vs_sorted.erase(std::unique(vs_sorted.begin(), vs_sorted.end()), vs_sorted.end());
      CHECK_EQ(vs, vs_sorted);
    }

    static void testBackdoor_Rejects_X_or_Y_in_Z() {
      // Graph with confounding and a direct edge.
      auto        bn  = gum::BayesNet< double >::fastPrototype("U->X->Y;U->Y");
      const auto& dag = bn.dag();


      const auto idX = bn.idFromName("X");
      const auto idY = bn.idFromName("Y");
      const auto idU = bn.idFromName("U");

      // Z that contains X is invalid
      gum::NodeSet ZX;
      ZX.insert(idX);
      CHECK(!gum::DoorCriteria::satisfiesBackdoorCriterion(dag, idX, idY, ZX));

      // Z that contains Y is invalid
      gum::NodeSet ZY;
      ZY.insert(idY);
      CHECK(!gum::DoorCriteria::satisfiesBackdoorCriterion(dag, idX, idY, ZY));

      // Control: {U} is valid
      gum::NodeSet ZU;
      ZU.insert(idU);
      CHECK(gum::DoorCriteria::satisfiesBackdoorCriterion(dag, idX, idY, ZU));
    }

    static void testFrontdoor_Rejects_X_or_Y_in_Z() {
      // Canonical frontdoor structure.
      auto        bn  = gum::BayesNet< double >::fastPrototype("U->X;U->Y;X->Z->Y");
      const auto& dag = bn.dag();


      const auto idX = bn.idFromName("X");
      const auto idY = bn.idFromName("Y");
      const auto idZ = bn.idFromName("Z");

      // Z that contains X or Y should be rejected for frontdoor.
      gum::NodeSet ZX;
      ZX.insert(idX);
      gum::NodeSet ZY;
      ZY.insert(idY);
      CHECK(!gum::DoorCriteria::satisfiesFrontdoorCriterion(dag, idX, idY, ZX));
      CHECK(!gum::DoorCriteria::satisfiesFrontdoorCriterion(dag, idX, idY, ZY));

      // Control: {Z} is valid.
      gum::NodeSet ZZ;
      ZZ.insert(idZ);
      CHECK(gum::DoorCriteria::satisfiesFrontdoorCriterion(dag, idX, idY, ZZ));
    }

    static void testFrontdoor_Fails_With_Direct_XY_Edge() {
      // Add a direct edge X->Y : FD-1 ("intercept all directed paths") should fail for {Z}.
      auto        bn  = gum::BayesNet< double >::fastPrototype("U->X;U->Y;X->Z->Y;X->Y");
      const auto& dag = bn.dag();


      const auto idX = bn.idFromName("X");
      const auto idY = bn.idFromName("Y");
      const auto idZ = bn.idFromName("Z");

      gum::NodeSet ZZ;
      ZZ.insert(idZ);
      CHECK(!gum::DoorCriteria::satisfiesFrontdoorCriterion(dag, idX, idY, ZZ));

      // Enumeration should be empty in this case.
      auto fds = gum::DoorCriteria::enumerateFrontdoorSets(dag, idX, idY);
      CHECK_EQ(fds.size(), 0u);
    }

    static void testFrontdoor_Enumeration_MinimalVsSupersets_Z3Hub() {
      // X->Z1->Z3 and X->Z2->Z3, with Z3->Y; U->X, U->Y (confounding).
      // All directed X->Y paths pass through Z3, so {Z3} is inclusion-minimal.
      // {Z1,Z2} also intercepts all directed paths (each singleton fails), and is also
      // inclusion-minimal.
      auto bn = gum::BayesNet< double >::fastPrototype("U->X;U->Y;X->Z1->Z3;X->Z2->Z3;Z3->Y");

      const auto& dag = bn.dag();


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
      CHECK(!gum::DoorCriteria::satisfiesFrontdoorCriterion(dag, idX, idY, Z1));
      CHECK(!gum::DoorCriteria::satisfiesFrontdoorCriterion(dag, idX, idY, Z2));
      CHECK(gum::DoorCriteria::satisfiesFrontdoorCriterion(dag, idX, idY, Z3));
      CHECK(gum::DoorCriteria::satisfiesFrontdoorCriterion(
          dag,
          idX,
          idY,
          Z12));   // inclusion-minimal but larger than {Z3}

      // ---------- Minimal enumeration (only_minimal=true) ----------
      {
        auto fds_min
            = gum::DoorCriteria::enumerateFrontdoorSets(dag,
                                                        idX,
                                                        idY);   // default: only_minimal=true
        CHECK(!fds_min.empty());

        // Presence: both {Z3} and {Z1,Z2} should appear (both are inclusion-minimal)
        auto contains_min = [&](const gum::NodeSet& target) {
          return std::any_of(fds_min.begin(), fds_min.end(), [&](const gum::NodeSet& S) {
            return S == target;
          });
        };
        CHECK(contains_min(Z3));
        CHECK(contains_min(Z12));

        // Inclusion-minimality: no set is a proper subset of another
        auto isProperSubset = [](const gum::NodeSet& A, const gum::NodeSet& B) -> bool {
          if (A.size() >= B.size()) return false;
          for (auto n: A)
            if (!B.contains(n)) return false;
          return true;
        };
        for (size_t i = 0; i < fds_min.size(); ++i) {
          for (size_t j = 0; j < fds_min.size(); ++j)
            if (i != j) { CHECK(!isProperSubset(fds_min[i], fds_min[j])); }
        }
      }

      // ---------- Non-minimal enumeration (only_minimal=false) ----------
      {
        auto fds_all
            = gum::DoorCriteria::enumerateFrontdoorSets(dag, idX, idY, gum::NodeSet{}, 0, false);

        // Must include at least the two minimal solutions
        auto contains_all = [&](const gum::NodeSet& target) {
          return std::any_of(fds_all.begin(), fds_all.end(), [&](const gum::NodeSet& S) {
            return S == target;
          });
        };
        CHECK(contains_all(Z3));
        CHECK(contains_all(Z12));

        // Dedup + deterministic order (same style as your backdoor test)
        auto toVec = [](const gum::NodeSet& s) {
          std::vector< gum::NodeId > v;
          for (auto n: s)
            v.push_back(n);
          std::ranges::sort(v);
          return v;
        };
        std::vector< std::vector< gum::NodeId > > vs;
        for (const auto& s: fds_all)
          vs.push_back(toVec(s));
        auto vs_sorted = vs;
        std::ranges::sort(vs_sorted);
        vs_sorted.erase(std::unique(vs_sorted.begin(), vs_sorted.end()), vs_sorted.end());
        CHECK_EQ(vs, vs_sorted);
      }
    }

    static void testFrontdoor_Enumeration_Excludes_XY() {
      using BN = gum::BayesNet< double >;

      // Canonical frontdoor: U->X; U->Y; X->Z->Y
      auto        bn  = BN::fastPrototype("U->X;U->Y;X->Z->Y");
      const auto& dag = bn.dag();


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

      CHECK(!gum::DoorCriteria::satisfiesFrontdoorCriterion(dag, idX, idY, ZX));
      CHECK(!gum::DoorCriteria::satisfiesFrontdoorCriterion(dag, idX, idY, ZY));
      CHECK(gum::DoorCriteria::satisfiesFrontdoorCriterion(dag, idX, idY, ZZ));

      // --- Enumeration (minimal) must return exactly {Z}
      auto fds_min
          = gum::DoorCriteria::enumerateFrontdoorSets(dag,
                                                      idX,
                                                      idY);   // only_minimal=true (default)
      CHECK_EQ(fds_min.size(), 1u);
      CHECK_EQ(fds_min[0], ZZ);

      // --- Enumeration (non-minimal) must NEVER include X or Y
      auto fds_all
          = gum::DoorCriteria::enumerateFrontdoorSets(dag, idX, idY, gum::NodeSet{}, 0, false);

      // Positive control: {Z} still present
      auto contains = [&](const gum::NodeSet& target) {
        return std::any_of(fds_all.begin(), fds_all.end(), [&](const gum::NodeSet& S) {
          return S == target;
        });
      };
      CHECK(contains(ZZ));

      // Negative controls: sets that include X or Y must be absent
      gum::NodeSet ZXZ;
      ZXZ.insert(idX);
      ZXZ.insert(idZ);
      gum::NodeSet ZYZ;
      ZYZ.insert(idY);
      ZYZ.insert(idZ);
      CHECK(!contains(ZX));
      CHECK(!contains(ZY));
      CHECK(!contains(ZXZ));
      CHECK(!contains(ZYZ));

      // Generic guard: none of the enumerated sets contains X or Y
      for (const auto& S: fds_all) {
        CHECK(!S.contains(idX));
        CHECK(!S.contains(idY));
      }
    }

    // --- Tests for lexicographic ordering (_lexLess, exercised via enumeration) ---

    // Branch coverage of the O(n) _lexLess algorithm:
    //   (A) e∈A, B not exhausted  → A < B   (sets[0]={U} when idU<idV)
    //   (B) e∈A, B exhausted      → A > B   (sets[0]={U,V} when idV<idU)
    //   (C) e∈B, A not exhausted  → A > B
    //   (D) e∈B, A exhausted      → A < B
    // Prefix pairs cover (A)/(B) or (C)/(D) depending on NodeId assignment.
    // A 4-set enumeration exercises all branches across consecutive pairs.

    static void testLexLess_PrefixBeforeExtension() {
      // With only_minimal=false, both {U} and {U,V} are valid backdoor sets.
      // They form a prefix pair in sorted order (whichever id is smaller).
      // The result must be strictly lex-sorted.
      auto        bn  = gum::BayesNet< double >::fastPrototype("U->X->Y;U->Y;V->X");
      const auto& dag = bn.dag();


      auto idX = bn.idFromName("X");
      auto idY = bn.idFromName("Y");
      auto idU = bn.idFromName("U");
      auto idV = bn.idFromName("V");

      auto sets = gum::DoorCriteria::enumerateBackdoorSets(dag, idX, idY, gum::NodeSet{}, 0, false);

      CHECK_EQ(sets.size(), 2u);

      gum::NodeSet setU;
      setU.insert(idU);
      gum::NodeSet setUV;
      setUV.insert(idU);
      setUV.insert(idV);

      CHECK((sets[0] == setU || sets[1] == setU));
      CHECK((sets[0] == setUV || sets[1] == setUV));

      auto toSortedVec = [](const gum::NodeSet& s) {
        std::vector< gum::NodeId > v;
        for (auto n: s)
          v.push_back(n);
        std::ranges::sort(v);
        return v;
      };
      CHECK_LT(toSortedVec(sets[0]), toSortedVec(sets[1]));
    }

    static void testLexLess_GlobalSortedOrder() {
      // Graph: one real confounder U, two extra parents of X (V, W).
      // With only_minimal=false: 4 valid backdoor sets → {U}, {U,V}, {U,W}, {U,V,W}
      // in some order that depends on NodeId assignment.
      // Every consecutive pair in the result must satisfy strict lex order,
      // exercising all branches of the O(n) _lexLess implementation.
      auto        bn  = gum::BayesNet< double >::fastPrototype("U->X->Y;U->Y;V->X;W->X");
      const auto& dag = bn.dag();


      auto idX = bn.idFromName("X");
      auto idY = bn.idFromName("Y");
      auto idU = bn.idFromName("U");
      auto idV = bn.idFromName("V");
      auto idW = bn.idFromName("W");

      auto sets = gum::DoorCriteria::enumerateBackdoorSets(dag, idX, idY, gum::NodeSet{}, 0, false);

      CHECK_EQ(sets.size(), 4u);

      // All four expected sets must be present
      gum::NodeSet setU;
      setU.insert(idU);
      gum::NodeSet setUV;
      setUV.insert(idU);
      setUV.insert(idV);
      gum::NodeSet setUW;
      setUW.insert(idU);
      setUW.insert(idW);
      gum::NodeSet setUVW;
      setUVW.insert(idU);
      setUVW.insert(idV);
      setUVW.insert(idW);

      auto containsSet = [&](const gum::NodeSet& target) {
        return std::any_of(sets.begin(), sets.end(), [&](const gum::NodeSet& s) {
          return s == target;
        });
      };
      CHECK(containsSet(setU));
      CHECK(containsSet(setUV));
      CHECK(containsSet(setUW));
      CHECK(containsSet(setUVW));

      // Every consecutive pair must be strictly lex-ordered
      auto toSortedVec = [](const gum::NodeSet& s) {
        std::vector< gum::NodeId > v;
        for (auto n: s)
          v.push_back(n);
        std::ranges::sort(v);

        return v;
      };
      for (size_t i = 1; i < sets.size(); ++i)
        CHECK_LT(toSortedVec(sets[i - 1]), toSortedVec(sets[i]));
    }

    static void testFrontdoor_NoDirectedPath_EnumeratesSingletons() {
      using BN = gum::BayesNet< double >;

      // No directed path X→Y; they are in the same weakly connected component:
      //   X → A ← B → Y
      auto        bn  = BN::fastPrototype("X->A;B->A;B->Y");
      const auto& dag = bn.dag();


      const auto idX = bn.idFromName("X");
      const auto idY = bn.idFromName("Y");
      const auto idA = bn.idFromName("A");
      const auto idB = bn.idFromName("B");

      // Sanity: truly no directed path X→Y (unblocked by ∅).
      gum::NodeSet none;
      CHECK(!gum::DoorCriteria::existsUnblockedDirectedPath(dag, idX, idY, none));

      // Minimal enumeration (default): should return exactly {B}.
      auto fds_min = gum::DoorCriteria::enumerateFrontdoorSets(dag, idX, idY);
      CHECK_EQ(fds_min.size(), 1u);
      gum::NodeSet ZB;
      ZB.insert(idB);
      CHECK_EQ(fds_min[0], ZB);

      // Non-minimal mode still must not introduce A, X, or Y.
      auto fds_all
          = gum::DoorCriteria::enumerateFrontdoorSets(dag, idX, idY, gum::NodeSet{}, 0, false);
      CHECK_EQ(fds_all.size(), 1u);
      CHECK_EQ(fds_all[0], ZB);

      // Guard: A is pruned by FD-3 (A ← B → Y is a backdoor from A to Y).
      gum::NodeSet ZA;
      ZA.insert(idA);
      auto contains = [&](const gum::NodeSet& target) {
        return std::any_of(fds_all.begin(), fds_all.end(), [&](const gum::NodeSet& S) {
          return S == target;
        });
      };
      CHECK(!contains(ZA));

      // Guard: no set should ever contain X or Y.
      for (const auto& S: fds_all) {
        CHECK(!S.contains(idX));
        CHECK(!S.contains(idY));
      }
    }

    static void testNoDirectedPaths() {
      gum::DAG d;
      d.addNodes(6u);
      d.addArc(2, 1);
      d.addArc(4, 1);
      d.addArc(5, 1);
      d.addArc(0, 2);
      d.addArc(3, 0);
      d.addArc(3, 2);
      d.addArc(4, 2);
      d.addArc(5, 0);

      auto v = gum::DoorCriteria::nodesOnDirectedPaths(d, 2, 1);
      GUM_TRACE_VAR(*v);
      CHECK_EQ(v->size(), 0u);
    }

    static void testFirstBackdoor() {
      // Simple chain: empty set is the (only) valid backdoor set
      {
        auto        bn  = gum::BayesNet< double >::fastPrototype("A->B->C");
        const auto& dag = bn.dag();
        auto        idA = bn.idFromName("A");
        auto        idC = bn.idFromName("C");

        auto result = gum::DoorCriteria::firstBackdoor(dag, idA, idC);
        CHECK(result.has_value());
        CHECK_EQ(*result, gum::NodeSet{});
      }
      // Classic confounder U->X->Y;U->Y: first backdoor set is {U}
      {
        auto        bn  = gum::BayesNet< double >::fastPrototype("U->X->Y;U->Y");
        const auto& dag = bn.dag();
        auto        idX = bn.idFromName("X");
        auto        idY = bn.idFromName("Y");
        auto        idU = bn.idFromName("U");

        auto result = gum::DoorCriteria::firstBackdoor(dag, idX, idY);
        CHECK(result.has_value());
        CHECK(gum::DoorCriteria::satisfiesBackdoorCriterion(dag, idX, idY, *result));
        CHECK_EQ(*result, gum::NodeSet{idU});
      }
      // When all blocking nodes are excluded, returns nullopt
      {
        auto        bn  = gum::BayesNet< double >::fastPrototype("U->X->Y;U->Y");
        const auto& dag = bn.dag();
        auto        idX = bn.idFromName("X");
        auto        idY = bn.idFromName("Y");
        auto        idU = bn.idFromName("U");

        auto result = gum::DoorCriteria::firstBackdoor(dag, idX, idY, gum::NodeSet{idU});
        CHECK(!result.has_value());
      }
    }

    static void testFirstFrontdoor() {
      // Classic frontdoor: X->Z->Y with hidden U->X, U->Y; {Z} is the unique frontdoor set
      {
        auto        bn  = gum::BayesNet< double >::fastPrototype("U->X;U->Y;X->Z->Y");
        const auto& dag = bn.dag();
        auto        idX = bn.idFromName("X");
        auto        idY = bn.idFromName("Y");
        auto        idZ = bn.idFromName("Z");

        auto result = gum::DoorCriteria::firstFrontdoor(dag, idX, idY);
        CHECK(result.has_value());
        CHECK(gum::DoorCriteria::satisfiesFrontdoorCriterion(dag, idX, idY, *result));
        CHECK_EQ(*result, gum::NodeSet{idZ});
      }
      // Direct edge X->Y: FD-3 fails (no valid frontdoor set)
      {
        auto        bn  = gum::BayesNet< double >::fastPrototype("X->Y");
        const auto& dag = bn.dag();
        auto        idX = bn.idFromName("X");
        auto        idY = bn.idFromName("Y");

        auto result = gum::DoorCriteria::firstFrontdoor(dag, idX, idY);
        CHECK(!result.has_value());
      }
      // No directed path X->Y: no frontdoor set exists
      {
        auto        bn  = gum::BayesNet< double >::fastPrototype("A->B;C->B");
        const auto& dag = bn.dag();
        auto        idA = bn.idFromName("A");
        auto        idC = bn.idFromName("C");

        auto result = gum::DoorCriteria::firstFrontdoor(dag, idA, idC);
        CHECK(!result.has_value());
      }
    }
  };


  GUM_TEST_ACTIF(Backdoor_EmptySetWhenNoParents)
  GUM_TEST_ACTIF(Backdoor_SingleConfounderMinimal)
  GUM_TEST_ACTIF(Backdoor_MinimalityFilter)
  GUM_TEST_ACTIF(Frontdoor_Classic_Mediator)
  GUM_TEST_ACTIF(Frontdoor_FailsWhenZDoesNotInterceptAllDirectedPaths)
  GUM_TEST_ACTIF(ExistsUnblockedDirectedPath_and_NodesOnDirectedPaths)
  GUM_TEST_ACTIF(Backdoor_DescendantInZIsRejected)
  GUM_TEST_ACTIF(Backdoor_pyAgrumExample1)
  GUM_TEST_ACTIF(Frontdoor_NoDirectedPath_ReturnsEmptyEnumeration)
  GUM_TEST_ACTIF(Backdoor_TwoConfounders_EnumerationAndPruning)
  GUM_TEST_ACTIF(Frontdoor_TwoMediators_EnumerationAndPruning)
  GUM_TEST_ACTIF(Backdoor_HasBackdoorPath_Equivalence)
  GUM_TEST_ACTIF(Frontdoor_RejectsZInBackdoorReach)
  GUM_TEST_ACTIF(Frontdoor_PrunesImpossibleByFD3)
  GUM_TEST_ACTIF(Frontdoor_NoDiPath_PrunesParentsOfX)
  GUM_TEST_ACTIF(BackdoorReach_ContainsParents)
  GUM_TEST_ACTIF(ExistsUnblockedDirectedPath_EndpointsNotBlocked)
  GUM_TEST_ACTIF(Backdoor_DeterministicOrderAndNoDup)
  GUM_TEST_ACTIF(Backdoor_Rejects_X_or_Y_in_Z)
  GUM_TEST_ACTIF(Frontdoor_Rejects_X_or_Y_in_Z)
  GUM_TEST_ACTIF(Frontdoor_Fails_With_Direct_XY_Edge)
  GUM_TEST_ACTIF(Frontdoor_Enumeration_MinimalVsSupersets_Z3Hub)
  GUM_TEST_ACTIF(Frontdoor_Enumeration_Excludes_XY)
  GUM_TEST_ACTIF(Frontdoor_NoDirectedPath_EnumeratesSingletons)
  GUM_TEST_ACTIF(LexLess_PrefixBeforeExtension)
  GUM_TEST_ACTIF(LexLess_GlobalSortedOrder)
  GUM_TEST_ACTIF(NoDirectedPaths)
  GUM_TEST_ACTIF(FirstBackdoor)
  GUM_TEST_ACTIF(FirstFrontdoor)


}   // namespace gum_tests
