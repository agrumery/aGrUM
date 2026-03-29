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


#include <gumtest/AgrumTestSuite.h>

#include <agrum/CM/causalModel.h>
#include <agrum/CM/tools/doAST.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  CausalModel
#define GUM_CURRENT_MODULE CM

namespace gum_tests {

  struct CausalModelTestSuite {
    public:
    static void testSimpleBN() {
      auto                       bn = gum::BayesNet< double >::fastPrototype("A[2]->B[3]");
      gum::CausalModel< double > cm(bn);

      // BN has 2 variables
      CHECK_EQ(cm.observationalBN().size(), 2u);
      CHECK(cm.existsArc("A", "B"));
      CHECK(!cm.existsArc("B", "A"));
    }

    static void testLatentFromIds() {
      auto                       bn = gum::BayesNet< double >::fastPrototype("X->Y");
      std::vector< gum::NodeId > childIds;
      childIds.push_back(bn.idFromName("X"));
      childIds.push_back(bn.idFromName("Y"));

      gum::CausalModel< double > cm(bn);
      cm.addLatentVariable("Hidden", childIds);

      CHECK(cm.latentVariablesNames().contains("Hidden"));
      CHECK(cm.existsArc("Hidden", "X"));
      CHECK(cm.existsArc("Hidden", "Y"));
    }

    static void testToDotSmoke() {
      // Smoke network: Smoking -> Tar -> Cancer; Smoking -> Cancer
      auto bn = gum::BayesNet< double >::fastPrototype("Smoking->Tar->Cancer;Smoking->Cancer");
      gum::LatentDescriptorVector descs;
      auto                        id_smoking = bn.idFromName("Smoking");
      auto                        id_cancer  = bn.idFromName("Cancer");
      descs.push_back(gum::LatentDescriptorIds("Genotype", {id_smoking, id_cancer}));

      gum::CausalModel< double > cm(bn, descs);

      auto dot = cm.toDot();
      //  GUM_TRACE_VAR(std::string("\n") + dot); // uncomment to see the dot output

      CHECK_NE(dot.find("Smoking"), std::string::npos);
      CHECK_NE(dot.find("Cancer"), std::string::npos);
      CHECK_NE(dot.find("Tar"), std::string::npos);
      CHECK_NE(dot.find("Genotype"), std::string::npos);

      // Expected arcs
      CHECK_NE(dot.find("\"Smoking\"->\"Tar\""), std::string::npos);
      CHECK_NE(dot.find("\"Tar\"->\"Cancer\""), std::string::npos);
      CHECK_NE(dot.find("\"Genotype\"->\"Smoking\""), std::string::npos);
      CHECK_NE(dot.find("\"Genotype\"->\"Cancer\""), std::string::npos);

      // Because assumeNonSpurious=false by default, the direct Smoking->Cancer must be gone
      CHECK_EQ(dot.find("\"Smoking\"->\"Cancer\""), std::string::npos);
    }

    static void testToDotSimpson() {
      // Simpson network: Gender->Drug->Patient;Gender->Patient
      auto bn = gum::BayesNet< double >::fastPrototype("Gender->Drug->Patient;Gender->Patient");

      gum::CausalModel< double > cm(bn);
      auto                       dot = cm.toDot();

      //   GUM_TRACE_VAR(std::string("\n") + dot); // uncomment to see the dot output

      // Nodes
      CHECK_NE(dot.find("Gender"), std::string::npos);
      CHECK_NE(dot.find("Drug"), std::string::npos);
      CHECK_NE(dot.find("Patient"), std::string::npos);

      // Expected arcs
      CHECK_NE(dot.find("\"Gender\"->\"Drug\""), std::string::npos);
      CHECK_NE(dot.find("\"Gender\"->\"Patient\""), std::string::npos);
      CHECK_NE(dot.find("\"Drug\"->\"Patient\""), std::string::npos);
    }

    static void testInducedCausalSubModel_DropsSingletonLatent() {
      // Base BN: X->Y->Z and W->Z (W will be excluded in subsets)
      auto                       bn = gum::BayesNet< double >::fastPrototype("X->Y->Z;W->Z");
      gum::CausalModel< double > cm(bn);

      // Create latent U with TWO children (valid): confounds X and Z
      std::vector< gum::NodeId > childIds{bn.idFromName("X"), bn.idFromName("Z")};
      cm.addLatentVariable("U", childIds);   // OK: |children| = 2

      // Case A: subset {X, Y, Z} → U kept (still has 2 children)
      {
        gum::NodeSet subsetA;
        subsetA.insert(bn.idFromName("X"));
        subsetA.insert(bn.idFromName("Y"));
        subsetA.insert(bn.idFromName("Z"));

        auto subA = cm.inducedCausalSubModel(cm, subsetA);

        CHECK_EQ(subA.observationalBN().size(), 3u);
        CHECK(subA.existsArc("X", "Y"));
        CHECK(subA.existsArc("Y", "Z"));
        CHECK(!subA.existsArc("X", "Z"));   // removed by latent surgery

        // U exists and still has 2 children
        CHECK(subA.latentVariablesNames().contains("U"));
        CHECK(subA.existsArc("U", "X"));
        CHECK(subA.existsArc("U", "Z"));
      }

      // Case B: subset {Y, Z} → U would have a single child (Z) ⇒ must DISAPPEAR
      {
        gum::NodeSet subsetB;
        subsetB.insert(bn.idFromName("Y"));
        subsetB.insert(bn.idFromName("Z"));

        auto subB = cm.inducedCausalSubModel(cm, subsetB);

        CHECK_EQ(subB.observationalBN().size(), 2u);
        CHECK(subB.existsArc("Y", "Z"));   // observed arc preserved

        // U must be absent; do NOT call existsArc("U", ...)
        CHECK(!subB.latentVariablesNames().contains("U"));
      }

      // Case C: subset {Y} → U also disappears (no children kept)
      {
        gum::NodeSet subsetC;
        subsetC.insert(bn.idFromName("Y"));

        auto subC = cm.inducedCausalSubModel(cm, subsetC);

        CHECK_EQ(subC.observationalBN().size(), 1u);
        // U must be absent; do NOT call existsArc("U", ...)
        CHECK(!subC.latentVariablesNames().contains("U"));
      }
    }

    static void testParentsChildrenAndConnectedComponents() {
      // Two disjoint observed components: A->B   and   C->D
      auto                       bn = gum::BayesNet< double >::fastPrototype("A->B;C->D");
      gum::CausalModel< double > cm(bn);

      // --- parents / children (by name)
      {
        auto pB = cm.parents("B");
        CHECK_EQ(pB.size(), 1u);
        CHECK(pB.contains(cm.idFromName("A")));

        auto cA = cm.children("A");
        CHECK_EQ(cA.size(), 1u);
        CHECK(cA.contains(cm.idFromName("B")));

        auto pC = cm.parents("C");
        CHECK_EQ(pC.size(), 0u);

        auto cD = cm.children("D");
        CHECK_EQ(cD.size(), 0u);
      }

      // --- same via NodeId overloads (should match)
      {
        auto idB = bn.idFromName("B");
        auto idA = bn.idFromName("A");
        auto idC = bn.idFromName("C");
        auto idD = bn.idFromName("D");

        auto pB = cm.parents(idB);
        CHECK_EQ(pB.size(), 1u);
        CHECK(pB.contains(idA));

        auto cA = cm.children(idA);
        CHECK_EQ(cA.size(), 1u);
        CHECK(cA.contains(idB));

        CHECK_EQ(cm.parents(idC).size(), 0u);
        CHECK_EQ(cm.children(idD).size(), 0u);
      }

      // --- connected components: initially 2 ( {A,B} and {C,D} )
      {
        auto comps = cm.connectedComponents();
        CHECK_EQ(comps.size(), 2u);
      }

      // Add latent U that confounds B and C -> bridges the two components
      {
        std::vector< gum::NodeId > childIds{bn.idFromName("B"), bn.idFromName("C")};
        cm.addLatentVariable("U", childIds /*assumeNonSpurious default = false*/);

        // Parents now reflect the latent
        auto pB = cm.parents("B");
        CHECK_EQ(pB.size(), 2u);   // {A, U}
        CHECK(pB.contains(cm.idFromName("A")));
        CHECK(pB.contains(cm.idFromName("U")));

        auto pC = cm.parents("C");
        CHECK_EQ(pC.size(), 1u);   // {U}
        CHECK(pC.contains(cm.idFromName("U")));

        auto cU = cm.children("U");
        CHECK_EQ(cU.size(), 2u);   // {B, C}
        CHECK(cU.contains(cm.idFromName("B")));
        CHECK(cU.contains(cm.idFromName("C")));

        // Components collapse to 1 due to the latent bridge
        auto comps2 = cm.connectedComponents();
        CHECK_EQ(comps2.size(), 1u);
      }
    }

    // X <- Z -> Y (observed confounder), X -> Y present:
    // backdoor should return {Z} (non-empty, observed-only).
    static void testBackdoorWithObservedConfounder() {
      auto                       bn = gum::BayesNet< double >::fastPrototype("Z->X;Z->Y;X->Y");
      gum::CausalModel< double > cm(bn);

      const auto X = bn.idFromName("X");
      const auto Y = bn.idFromName("Y");
      const auto Z = bn.idFromName("Z");

      auto bd = cm.backDoor(X, Y);
      CHECK(bd.has_value());
      CHECK_EQ(bd.value().size(), 1u);
      CHECK(bd.value().contains(Z));

      // Returned set must not contain latents.
      for (auto n: bd.value())
        CHECK(!cm.latentVariablesIds().contains(n));
    }

    // Only latent confounding U -> {X, Y}, no observed Z:
    // backdoor should find no observed adjustment set => empty.
    static void testBackdoorWithLatentOnly_NoObservedSet() {
      auto                       bn = gum::BayesNet< double >::fastPrototype("X->Y");
      gum::CausalModel< double > cm(bn);

      std::vector< gum::NodeId > kids{bn.idFromName("X"), bn.idFromName("Y")};
      cm.addLatentVariable("U", kids);

      auto bd = cm.backDoor(bn.idFromName("X"), bn.idFromName("Y"));
      CHECK(!bd.has_value());
    }

    // Classic frontdoor with latent confounding:
    // U -> {X, Y}, X -> Z -> Y, no direct X->Y.
    // frontdoor should return {Z}.
    static void testFrontdoorClassicWithLatent_ReturnsMediator() {
      auto                       bn = gum::BayesNet< double >::fastPrototype("X->Z;Z->Y");
      gum::CausalModel< double > cm(bn);

      std::vector< gum::NodeId > kids{bn.idFromName("X"), bn.idFromName("Y")};
      cm.addLatentVariable("U", kids);

      const auto X = bn.idFromName("X");
      const auto Y = bn.idFromName("Y");
      const auto Z = bn.idFromName("Z");

      auto fd = cm.frontDoor(X, Y);
      CHECK(fd.has_value());
      CHECK_EQ(fd.value().size(), 1u);
      CHECK(fd.value().contains(Z));

      for (auto n: fd.value())
        CHECK(!cm.latentVariablesIds().contains(n));
    }

    // If X -> Y is present, our frontdoor enumerator short-circuits (no frontdoor set).
    static void testFrontdoorFailsWithDirectEdge() {
      auto                       bn = gum::BayesNet< double >::fastPrototype("X->Y;X->Z;Z->Y");
      gum::CausalModel< double > cm(bn);

      auto fd = cm.frontDoor(bn.idFromName("X"), bn.idFromName("Y"));
      CHECK(!fd.has_value());
    }

    // Passing a latent as cause/effect must raise (guard in CausalModel).
    static void testThrowsWhenCauseOrEffectIsLatent() {
      auto                       bn = gum::BayesNet< double >::fastPrototype("X->Z;Z->Y");
      gum::CausalModel< double > cm(bn);

      std::vector< gum::NodeId > kids{bn.idFromName("X"), bn.idFromName("Y")};
      cm.addLatentVariable("U", kids);

      const auto U = cm.idFromName("U");
      const auto X = bn.idFromName("X");
      const auto Y = bn.idFromName("Y");

      CHECK_THROWS_AS(cm.backDoor(U, Y), const gum::InvalidArgument&);
      CHECK_THROWS_AS(cm.frontDoor(X, U), const gum::InvalidArgument&);
    }

    // ...existing tests...
    static void testSpuriousArcFunctions() {
      auto                       bn = gum::BayesNet< double >::fastPrototype("A->B;B->C;A->C");
      gum::CausalModel< double > cm(bn);

      // Initially, all arcs present in both observationalBN and causalDAG
      CHECK(cm.existsArc("A", "B"));
      CHECK(cm.existsArc("B", "C"));
      CHECK(cm.existsArc("A", "C"));

      // Mark A->B as spurious (by name)
      cm.assumeSpurious("A", "B");
      CHECK(!cm.existsArc("A", "B"));
      CHECK(cm.isAssumedSpurious("A", "B"));

      // Mark B->C as spurious (by NodeId)
      auto idB = bn.idFromName("B");
      auto idC = bn.idFromName("C");
      cm.assumeSpurious(idB, idC);
      CHECK(!cm.existsArc(idB, idC));
      CHECK(cm.isAssumedSpurious(idB, idC));

      // Mark C->A as non-spurious (should be present already)
      cm.assumeNonSpurious("A", "C");
      CHECK(cm.existsArc("A", "C"));
      CHECK(!cm.isAssumedSpurious("A", "C"));

      // Remove C->A, then re-add as non-spurious
      cm.assumeSpurious("A", "C");
      CHECK(!cm.existsArc("A", "C"));
      CHECK(cm.isAssumedSpurious("A", "C"));
      cm.assumeNonSpurious("A", "C");
      CHECK(cm.existsArc("A", "C"));
      CHECK(!cm.isAssumedSpurious("A", "C"));

      // Throws if arc not present in observationalBN
      CHECK_THROWS_AS(cm.assumeSpurious("B", "A"), const gum::InvalidArgument&);
      CHECK_THROWS_AS(cm.assumeNonSpurious("B", "A"), const gum::InvalidArgument&);
    }
  };

  GUM_TEST_ACTIF(SimpleBN)
  GUM_TEST_ACTIF(LatentFromIds)
  GUM_TEST_ACTIF(ToDotSmoke)
  GUM_TEST_ACTIF(ToDotSimpson)
  GUM_TEST_ACTIF(InducedCausalSubModel_DropsSingletonLatent)
  GUM_TEST_ACTIF(ParentsChildrenAndConnectedComponents)
  GUM_TEST_ACTIF(BackdoorWithObservedConfounder)
  GUM_TEST_ACTIF(BackdoorWithLatentOnly_NoObservedSet)
  GUM_TEST_ACTIF(FrontdoorClassicWithLatent_ReturnsMediator)
  GUM_TEST_ACTIF(FrontdoorFailsWithDirectEdge)
  GUM_TEST_ACTIF(ThrowsWhenCauseOrEffectIsLatent)
  GUM_TEST_ACTIF(SpuriousArcFunctions)

}   // namespace gum_tests
