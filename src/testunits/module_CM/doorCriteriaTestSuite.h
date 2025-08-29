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

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/CM/dSeparation.h>
#include <agrum/base/graphs/DAG.h>


#include <agrum/CM/doorCriteria.h>

namespace gum_tests {

  class DoorCriteriaTestSuite : public CxxTest::TestSuite {
  private:
    using NodeSet = gum::NodeSet;

    template <typename S>
    static NodeSet ns(const gum::BayesNet<S>& bn, std::initializer_list<const char*> names) {
      NodeSet s;
      for (auto n : names) s.insert(bn.idFromName(n));
      return s;
    }

  public:

    GUM_ACTIVE_TEST(Backdoor_EmptySetWhenNoParents) {
      auto bn  = gum::BayesNet<double>::fastPrototype("A->B->C");
      const auto& dag = bn.dag();

      gum::DoorCriteria dc(dag);
      auto idA = bn.idFromName("A");
      auto idC = bn.idFromName("C");

      gum::NodeSet Z; // {}
      TS_ASSERT( dc.satisfiesBackdoorCriterion(idA, idC, Z) );

      gum::DoorCriteria::EnumerationOptions opts; // defaults ok
      auto sets = dc.enumerateBackdoorSets(idA, idC, opts);

      // expected == [{ }]
      gum::DoorCriteria::NodeSetVec expected;
      expected.emplace_back(); // push an empty NodeSet

      TS_ASSERT_EQUALS(sets, expected);
    }

    GUM_ACTIVE_TEST(Backdoor_SingleConfounderMinimal) {
      // Classic confounding: U -> X, U -> Y, and X -> Y
      // Minimal backdoor set is {U}
      auto bn = gum::BayesNet<double>::fastPrototype("U->X->Y;U->Y");
      const auto& dag = bn.dag();

      gum::DoorCriteria dc(dag);
      auto idU = bn.idFromName("U");
      auto idX = bn.idFromName("X");
      auto idY = bn.idFromName("Y");

      gum::NodeSet Z_bad; Z_bad.insert(idX); // descendant of X ⇒ invalid for backdoor
      TS_ASSERT( !dc.satisfiesBackdoorCriterion(idX, idY, Z_bad) );

      gum::NodeSet Z_ok; Z_ok.insert(idU);
      TS_ASSERT( dc.satisfiesBackdoorCriterion(idX, idY, Z_ok) );

      // enumerate: exactly [{U}]
      auto sets = dc.enumerateBackdoorSets(idX, idY);

      gum::DoorCriteria::NodeSetVec expected;
      expected.push_back(Z_ok);  // [{U}]

      TS_ASSERT_EQUALS(sets, expected);
    }


    GUM_ACTIVE_TEST(Backdoor_MinimalityFilter) {
      // U->X, U->Y, W->X (W not confounding Y). {U} is minimal; {U,W} is valid but not minimal.
      auto bn = gum::BayesNet<double>::fastPrototype("U->X->Y;U->Y;W->X");
      const auto& dag = bn.dag();

      gum::DoorCriteria dc(dag);
      auto idU = bn.idFromName("U");
      auto idW = bn.idFromName("W");
      auto idX = bn.idFromName("X");
      auto idY = bn.idFromName("Y");

      gum::DoorCriteria::EnumerationOptions opts_min; // default only_minimal=true
      auto mins = dc.enumerateBackdoorSets(idX, idY, opts_min);
      TS_ASSERT_EQUALS(mins.size(), 1u);
      TS_ASSERT( mins[0].contains(idU) && mins[0].size()==1 );

      gum::DoorCriteria::EnumerationOptions opts_all;
      opts_all.only_minimal = false;
      auto all = dc.enumerateBackdoorSets(idX, idY, opts_all);

      // must include {U} and {U,W} (order is deterministic but we just check membership)
      bool has_U   = false;
      bool has_UW  = false;
      for (const auto& Z : all) {
        if (Z.size()==1 && Z.contains(idU)) has_U = true;
        if (Z.size()==2 && Z.contains(idU) && Z.contains(idW)) has_UW = true;
      }
      TS_ASSERT(has_U);
      TS_ASSERT(has_UW);
    }

    GUM_ACTIVE_TEST(Frontdoor_Classic_Mediator) {
      // Frontdoor canonical: X->Z->Y with hidden confounding U->X, U->Y.
      // {Z} is a valid frontdoor set. Note: Z is a descendant of X — that’s OK.
      auto bn = gum::BayesNet<double>::fastPrototype("U->X;U->Y;X->Z->Y");
      const auto& dag = bn.dag();

      gum::DoorCriteria dc(dag);
      auto idX = bn.idFromName("X");
      auto idY = bn.idFromName("Y");
      auto idZ = bn.idFromName("Z");

      NodeSet Z; Z.insert(idZ);
      TS_ASSERT( dc.satisfiesFrontdoorCriterion(idX, idY, Z) );

      auto fds = dc.enumerateFrontdoorSets(idX, idY);

      // fds should contain exactly {Z} (no other nodes lie on every X→…→Y path)
      gum::DoorCriteria::NodeSetVec fds_expected;
      fds_expected.push_back(Z);
      TS_ASSERT_EQUALS(fds, fds_expected);
    }

    GUM_ACTIVE_TEST(Frontdoor_FailsWhenZDoesNotInterceptAllDirectedPaths) {
      // X -> Z1 -> Y and X -> Z2 -> Y, confounding U->X, U->Y.
      // {Z1} alone does NOT intercept the path via Z2; likewise {Z2} alone fails.
      // {Z1,Z2} satisfies condition (i); check the other backdoor conditions too.
      auto bn = gum::BayesNet<double>::fastPrototype("U->X;U->Y;X->Z1->Y;X->Z2->Y");
      const auto& dag = bn.dag();

      gum::DoorCriteria dc(dag);
      auto idX  = bn.idFromName("X");
      auto idY  = bn.idFromName("Y");
      auto idZ1 = bn.idFromName("Z1");
      auto idZ2 = bn.idFromName("Z2");

      NodeSet z1; z1.insert(idZ1);
      NodeSet z2; z2.insert(idZ2);
      NodeSet z12; z12.insert(idZ1); z12.insert(idZ2);

      TS_ASSERT( !dc.satisfiesFrontdoorCriterion(idX, idY, z1) );
      TS_ASSERT( !dc.satisfiesFrontdoorCriterion(idX, idY, z2) );

      // {Z1,Z2} should pass (i), and pass (ii) + (iii) backdoor checks
      TS_ASSERT( dc.satisfiesFrontdoorCriterion(idX, idY, z12) );

      // enumeration: exactly [{Z1, Z2}]
      auto fds = dc.enumerateFrontdoorSets(idX, idY);
      gum::NodeSet Z12;
      Z12.insert(idZ1);
      Z12.insert(idZ2);
      gum::DoorCriteria::NodeSetVec expected;
      expected.push_back(Z12);
      TS_ASSERT_EQUALS(fds, expected);
    }

    GUM_ACTIVE_TEST(ExistsUnblockedDirectedPath_and_NodesOnDirectedPaths) {
      // X -> Z1 -> Z2 -> Y, plus a side branch X -> W (not leading to Y)
      auto bn = gum::BayesNet<double>::fastPrototype("X->Z1->Z2->Y;X->W");
      const auto& dag = bn.dag();

      gum::DoorCriteria dc(dag);
      auto idX  = bn.idFromName("X");
      auto idY  = bn.idFromName("Y");
      auto idZ1 = bn.idFromName("Z1");
      auto idZ2 = bn.idFromName("Z2");
      auto idW  = bn.idFromName("W");

      NodeSet Z_empty;
      NodeSet Z_blockZ1; Z_blockZ1.insert(idZ1);

      // There is a directed path X→…→Y if we don't block Z1
      TS_ASSERT( dc.existsUnblockedDirectedPath(idX, idY, Z_empty) );
      // Blocking Z1 kills all directed paths
      TS_ASSERT( !dc.existsUnblockedDirectedPath(idX, idY, Z_blockZ1) );

      // nodesOnDirectedPaths should return {X,Z1,Z2,Y} (W is not on any X→…→Y path)
      auto onPath = dc.nodesOnDirectedPaths(idX, idY);
      TS_ASSERT( onPath.contains(idX) );
      TS_ASSERT( onPath.contains(idZ1) );
      TS_ASSERT( onPath.contains(idZ2) );
      TS_ASSERT( onPath.contains(idY) );
      TS_ASSERT( !onPath.contains(idW) );
    }

    GUM_ACTIVE_TEST(Backdoor_DescendantInZIsRejected) {
      // X -> Z, Z -> Y; conditioning on Z is invalid for backdoor sets
      auto bn = gum::BayesNet<double>::fastPrototype("X->Z->Y");
      const auto& dag = bn.dag();

      gum::DoorCriteria dc(dag);
      auto idX = bn.idFromName("X");
      auto idY = bn.idFromName("Y");
      auto idZ = bn.idFromName("Z");

      NodeSet Z; Z.insert(idZ);
      TS_ASSERT( !dc.satisfiesBackdoorCriterion(idX, idY, Z) );

      // enumeration should not include {Z}; {} is valid since X has no parents
      auto sets = dc.enumerateBackdoorSets(idX, idY);
      gum::DoorCriteria::NodeSetVec expected;
      expected.emplace_back();  // push an empty NodeSet: {}
      TS_ASSERT_EQUALS(sets, expected);

    }

    GUM_ACTIVE_TEST(Frontdoor_NoDirectedPath_ReturnsEmptyEnumeration) {
      // No directed path X→Y: X and Y disconnected (or only via incoming to X).
      auto bn = gum::BayesNet<double>::fastPrototype("A->X;Y->B");
      const auto& dag = bn.dag();

      gum::DoorCriteria dc(dag);
      auto idX = bn.idFromName("X");
      auto idY = bn.idFromName("Y");

      auto fds = dc.enumerateFrontdoorSets(idX, idY);
      TS_ASSERT_EQUALS(fds.size(), 0u);
    }

    GUM_ACTIVE_TEST(Backdoor_TwoConfounders_EnumerationAndPruning) {
      // Graph: X<-U1->Y; X<-U2->Y; X->W1->Y; X->W2->Y
      // Backdoor sets must block both X<-U1->Y and X<-U2->Y  ⇒ minimal set = {U1,U2}.
      auto bn = gum::BayesNet<double>::fastPrototype("U1->X;U1->Y;U2->X;U2->Y;X->W1->Y;X->W2->Y");
      const auto& dag = bn.dag();
      gum::DoorCriteria dc(dag);

      auto idX  = bn.idFromName("X");
      auto idY  = bn.idFromName("Y");
      auto idU1 = bn.idFromName("U1");
      auto idU2 = bn.idFromName("U2");
      auto idW1 = bn.idFromName("W1"); // descendant of X (never allowed in backdoor sets)
      //   auto idW2 = bn.idFromName("W2"); // not used

      // Predicate checks
      gum::NodeSet Z_u1; Z_u1.insert(idU1);
      gum::NodeSet Z_u2; Z_u2.insert(idU2);
      gum::NodeSet Z_u12; Z_u12.insert(idU1); Z_u12.insert(idU2);
      gum::NodeSet Z_bad; Z_bad.insert(idW1); // descendant of X ⇒ invalid

      TS_ASSERT( !dc.satisfiesBackdoorCriterion(idX,idY,Z_u1) );
      TS_ASSERT( !dc.satisfiesBackdoorCriterion(idX,idY,Z_u2) );
      TS_ASSERT(  dc.satisfiesBackdoorCriterion(idX,idY,Z_u12) );
      TS_ASSERT( !dc.satisfiesBackdoorCriterion(idX,idY,Z_bad) );

      // Enumeration (minimal-only): exactly [{U1,U2}]
      auto sets_min = dc.enumerateBackdoorSets(idX, idY);
      gum::DoorCriteria::NodeSetVec expected_min; expected_min.push_back(Z_u12);
      TS_ASSERT_EQUALS(sets_min, expected_min);

      // Excluding U1 ⇒ no valid backdoor set remains
      gum::DoorCriteria::EnumerationOptions opts_excl;
      opts_excl.excluded_nodes.insert(idU1);
      auto sets_excl = dc.enumerateBackdoorSets(idX, idY, opts_excl);
      TS_ASSERT(sets_excl.empty());

      // Limiting cardinality to 1 ⇒ cannot pick both U1 and U2 ⇒ empty
      gum::DoorCriteria::EnumerationOptions opts_cap1;
      opts_cap1.max_cardinality = 1;
      opts_cap1.only_minimal    = false; // even with non-minimal, cap=1 blocks {U1,U2}
      auto sets_cap1 = dc.enumerateBackdoorSets(idX, idY, opts_cap1);
      TS_ASSERT(sets_cap1.empty());
    }

    GUM_ACTIVE_TEST(Frontdoor_TwoMediators_EnumerationAndPruning) {
      // Same graph. Frontdoor must intercept both directed paths X->W1->Y and X->W2->Y.
      // Minimal frontdoor set = {W1, W2}. Singles fail (don’t intercept both paths).
      auto bn = gum::BayesNet<double>::fastPrototype("U1->X;U1->Y;U2->X;U2->Y;X->W1->Y;X->W2->Y");
      const auto& dag = bn.dag();
      gum::DoorCriteria dc(dag);

      auto idX  = bn.idFromName("X");
      auto idY  = bn.idFromName("Y");
      auto idW1 = bn.idFromName("W1");
      auto idW2 = bn.idFromName("W2");

      gum::NodeSet Z_w1;   Z_w1.insert(idW1);
      gum::NodeSet Z_w2;   Z_w2.insert(idW2);
      gum::NodeSet Z_w12;  Z_w12.insert(idW1); Z_w12.insert(idW2);

      // Predicate: only {W1,W2} satisfies frontdoor
      TS_ASSERT( !dc.satisfiesFrontdoorCriterion(idX,idY,Z_w1) );
      TS_ASSERT( !dc.satisfiesFrontdoorCriterion(idX,idY,Z_w2) );
      TS_ASSERT(  dc.satisfiesFrontdoorCriterion(idX,idY,Z_w12) );

      // Enumeration (minimal-only): exactly [{W1,W2}]
      auto fds = dc.enumerateFrontdoorSets(idX, idY);
      gum::DoorCriteria::NodeSetVec expected_fds; expected_fds.push_back(Z_w12);
      TS_ASSERT_EQUALS(fds, expected_fds);

      // Excluding W1 ⇒ cannot intercept both paths ⇒ empty
      gum::DoorCriteria::EnumerationOptions opts_excl;
      opts_excl.excluded_nodes.insert(idW1);
      auto fds_excl = dc.enumerateFrontdoorSets(idX, idY, opts_excl);
      TS_ASSERT(fds_excl.empty());

      // Limiting cardinality to 1 ⇒ cannot pick both mediators ⇒ empty
      gum::DoorCriteria::EnumerationOptions opts_cap1;
      opts_cap1.max_cardinality = 1;
      opts_cap1.only_minimal    = false; // even allowing non-minimal, cap=1 blocks {W1,W2}
      auto fds_cap1 = dc.enumerateFrontdoorSets(idX, idY, opts_cap1);
      TS_ASSERT(fds_cap1.empty());
    }


    GUM_ACTIVE_TEST(Backdoor_HasBackdoorPath_Equivalence) {
      auto bn = gum::BayesNet<double>::fastPrototype("U->X;U->Y");
      const auto& dag = bn.dag();
      gum::DoorCriteria dc(dag);

      auto idU = bn.idFromName("U");
      auto idX = bn.idFromName("X");
      auto idY = bn.idFromName("Y");

      gum::NodeSet Z0;
      TS_ASSERT( dc.hasBackdoorPath(idX,idY,Z0) );
      TS_ASSERT( !dc.satisfiesBackdoorCriterion(idX,idY,Z0) );

      gum::NodeSet ZU; ZU.insert(idU);
      TS_ASSERT( !dc.hasBackdoorPath(idX,idY,ZU) );
      TS_ASSERT( dc.satisfiesBackdoorCriterion(idX,idY,ZU) );
    }

    GUM_ACTIVE_TEST(Frontdoor_RejectsZInBackdoorReach) {
      auto bn = gum::BayesNet<double>::fastPrototype("U->X;U->Z;Z->Y;X->Y");
      const auto& dag = bn.dag();
      gum::DoorCriteria dc(dag);

      auto idX = bn.idFromName("X");
      auto idY = bn.idFromName("Y");
      auto idZ = bn.idFromName("Z");

      gum::NodeSet Z; Z.insert(idZ);
      TS_ASSERT( !dc.satisfiesFrontdoorCriterion(idX,idY,Z) );

      auto fds = dc.enumerateFrontdoorSets(idX, idY);
      TS_ASSERT(fds.empty());
    }

    GUM_ACTIVE_TEST(Frontdoor_PrunesImpossibleByFD3) {
      auto bn = gum::BayesNet<double>::fastPrototype("X->Z->Y;V->Z;V->Y");
      const auto& dag = bn.dag();
      gum::DoorCriteria dc(dag);

      auto idX = bn.idFromName("X");
      auto idY = bn.idFromName("Y");
      auto idZ = bn.idFromName("Z");

      gum::NodeSet Z; Z.insert(idZ);
      TS_ASSERT( !dc.satisfiesFrontdoorCriterion(idX,idY,Z) );

      auto fds = dc.enumerateFrontdoorSets(idX,idY);
      TS_ASSERT_EQUALS(fds.size(), 0u);
    }

    GUM_ACTIVE_TEST(Frontdoor_NoDiPath_PrunesParentsOfX) {
      auto bn = gum::BayesNet<double>::fastPrototype("M->X;M->Y");
      const auto& dag = bn.dag();
      gum::DoorCriteria dc(dag);

      auto idX = bn.idFromName("X");
      auto idY = bn.idFromName("Y");
      auto fds = dc.enumerateFrontdoorSets(idX, idY);

      TS_ASSERT(fds.empty());
    }

    GUM_ACTIVE_TEST(BackdoorReach_ContainsParents) {
      auto bn = gum::BayesNet<double>::fastPrototype("M->X;M->Y");
      const auto& dag = bn.dag();
      gum::DoorCriteria dc(dag);

      auto idX = bn.idFromName("X");
      auto idM = bn.idFromName("M");

      auto br = dc.backdoorReach(idX);
      TS_ASSERT(br.contains(idM));  // parents of X must be in backdoor reach
    }


    GUM_ACTIVE_TEST(ExistsUnblockedDirectedPath_EndpointsNotBlocked) {
      auto bn = gum::BayesNet<double>::fastPrototype("X->Y");
      const auto& dag = bn.dag();
      gum::DoorCriteria dc(dag);

      auto idX = bn.idFromName("X");
      auto idY = bn.idFromName("Y");

      gum::NodeSet Z; Z.insert(idY);
      TS_ASSERT( dc.existsUnblockedDirectedPath(idX,idY,Z) );
    }

    GUM_ACTIVE_TEST(Backdoor_DeterministicOrderAndNoDup) {
      auto bn = gum::BayesNet<double>::fastPrototype("U->X;U->Y;V->X");
      const auto& dag = bn.dag();
      gum::DoorCriteria dc(dag);

      auto idX = bn.idFromName("X");
      auto idY = bn.idFromName("Y");

      gum::DoorCriteria::EnumerationOptions opts;
      opts.only_minimal = false;
      auto sets = dc.enumerateBackdoorSets(idX, idY, opts);

      auto toVec = [](const gum::NodeSet& s){
        std::vector<gum::NodeId> v; for (auto n : s) v.push_back(n);
        std::sort(v.begin(), v.end()); return v;
      };
      std::vector<std::vector<gum::NodeId>> vs;
      for (const auto& s : sets) vs.push_back(toVec(s));
      auto vs_sorted = vs;
      std::sort(vs_sorted.begin(), vs_sorted.end());
      vs_sorted.erase(std::unique(vs_sorted.begin(), vs_sorted.end()), vs_sorted.end());
      TS_ASSERT_EQUALS(vs, vs_sorted);
    }

  };

} // namespace gum_tests
