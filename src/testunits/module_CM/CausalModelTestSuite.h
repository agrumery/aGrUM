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

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/CM/doAST.h>

#include <agrum/CM/causalModel.h>

namespace gum_tests {

  class CausalModelTestSuite : public CxxTest::TestSuite {
  public:

    GUM_ACTIVE_TEST(SimpleBN) {
      auto bn = gum::BayesNet<double>::fastPrototype("A[2]->B[3]");
      gum::CausalModel<double> cm(bn);

      // BN has 2 variables
      TS_ASSERT_EQUALS(cm.observationalBN().size(), 2u);
      TS_ASSERT(cm.existsArc("A","B"));
      TS_ASSERT(!cm.existsArc("B","A"));
    }


    GUM_ACTIVE_TEST(LatentFromIds) {
      auto bn = gum::BayesNet<double>::fastPrototype("X->Y");
      std::vector<gum::NodeId> childIds;
      childIds.push_back(bn.idFromName("X"));
      childIds.push_back(bn.idFromName("Y"));

      gum::CausalModel<double> cm(bn);
      cm.addLatentVariable("Hidden", childIds);

      TS_ASSERT(cm.latentVariablesNames().contains("Hidden"));
      TS_ASSERT(cm.existsArc("Hidden","X"));
      TS_ASSERT(cm.existsArc("Hidden","Y"));
    }

    GUM_ACTIVE_TEST(ToDotSmoke) {
      // Smoke network: Smoking -> Tar -> Cancer; Smoking -> Cancer
      auto bn = gum::BayesNet<double>::fastPrototype("Smoking->Tar->Cancer;Smoking->Cancer");
      gum::LatentDescriptorVector descs;
      auto id_smoking = bn.idFromName("Smoking");
      auto id_cancer  = bn.idFromName("Cancer");
      descs.push_back(gum::LatentDescriptorIds("Genotype", {id_smoking,id_cancer}));

      gum::CausalModel<double> cm(bn, descs);

      auto dot = cm.toDot();
      //  GUM_TRACE_VAR(std::string("\n") + dot); // uncomment to see the dot output

      TS_ASSERT(dot.find("Smoking")  != std::string::npos);
      TS_ASSERT(dot.find("Cancer")   != std::string::npos);
      TS_ASSERT(dot.find("Tar")      != std::string::npos);
      TS_ASSERT(dot.find("Genotype") != std::string::npos);

      // Expected arcs
      TS_ASSERT(dot.find("\"Smoking\"->\"Tar\"")    != std::string::npos);
      TS_ASSERT(dot.find("\"Tar\"->\"Cancer\"")     != std::string::npos);
      TS_ASSERT(dot.find("\"Genotype\"->\"Smoking\"") != std::string::npos);
      TS_ASSERT(dot.find("\"Genotype\"->\"Cancer\"")  != std::string::npos);

      // Because keepArcs=false by default, the direct Smoking->Cancer must be gone
      TS_ASSERT(dot.find("\"Smoking\"->\"Cancer\"") == std::string::npos);
    }

    GUM_ACTIVE_TEST(ToDotSimpson) {
      // Simpson network: Gender->Drug->Patient;Gender->Patient
      auto bn = gum::BayesNet<double>::fastPrototype("Gender->Drug->Patient;Gender->Patient");

      gum::CausalModel<double> cm(bn);
      auto dot = cm.toDot();

      //   GUM_TRACE_VAR(std::string("\n") + dot); // uncomment to see the dot output

      // Nodes
      TS_ASSERT(dot.find("Gender")  != std::string::npos);
      TS_ASSERT(dot.find("Drug")    != std::string::npos);
      TS_ASSERT(dot.find("Patient") != std::string::npos);

      // Expected arcs
      TS_ASSERT(dot.find("\"Gender\"->\"Drug\"")    != std::string::npos);
      TS_ASSERT(dot.find("\"Gender\"->\"Patient\"") != std::string::npos);
      TS_ASSERT(dot.find("\"Drug\"->\"Patient\"")   != std::string::npos);
    }


    GUM_ACTIVE_TEST(InducedCausalSubModel_DropsSingletonLatent) {
      // Base BN: X->Y->Z and W->Z (W will be excluded in subsets)
      auto bn = gum::BayesNet<double>::fastPrototype("X->Y->Z;W->Z");
      gum::CausalModel<double> cm(bn);

      // Create latent U with TWO children (valid): confounds X and Z
      std::vector<gum::NodeId> childIds{ bn.idFromName("X"), bn.idFromName("Z") };
      cm.addLatentVariable("U", childIds);  // OK: |children| = 2

      // Case A: subset {X, Y, Z} → U kept (still has 2 children)
      {
        gum::NodeSet subsetA;
        subsetA.insert(bn.idFromName("X"));
        subsetA.insert(bn.idFromName("Y"));
        subsetA.insert(bn.idFromName("Z"));

        auto subA = cm.inducedCausalSubModel(cm, subsetA);

        TS_ASSERT_EQUALS(subA.observationalBN().size(), 3u);
        TS_ASSERT(subA.existsArc("X", "Y"));
        TS_ASSERT(subA.existsArc("Y", "Z"));
        TS_ASSERT(!subA.existsArc("X", "Z")); // removed by latent surgery

        // U exists and still has 2 children
        TS_ASSERT(subA.latentVariablesNames().contains("U"));
        TS_ASSERT(subA.existsArc("U", "X"));
        TS_ASSERT(subA.existsArc("U", "Z"));
      }

      // Case B: subset {Y, Z} → U would have a single child (Z) ⇒ must DISAPPEAR
      {
        gum::NodeSet subsetB;
        subsetB.insert(bn.idFromName("Y"));
        subsetB.insert(bn.idFromName("Z"));

        auto subB = cm.inducedCausalSubModel(cm, subsetB);

        TS_ASSERT_EQUALS(subB.observationalBN().size(), 2u);
        TS_ASSERT(subB.existsArc("Y", "Z"));  // observed arc preserved

        // U must be absent; do NOT call existsArc("U", ...)
        TS_ASSERT(!subB.latentVariablesNames().contains("U"));
      }

      // Case C: subset {Y} → U also disappears (no children kept)
      {
        gum::NodeSet subsetC;
        subsetC.insert(bn.idFromName("Y"));

        auto subC = cm.inducedCausalSubModel(cm, subsetC);

        TS_ASSERT_EQUALS(subC.observationalBN().size(), 1u);
        // U must be absent; do NOT call existsArc("U", ...)
        TS_ASSERT(!subC.latentVariablesNames().contains("U"));
      }
    }



    GUM_ACTIVE_TEST(ParentsChildrenAndConnectedComponents) {
      // Two disjoint observed components: A->B   and   C->D
      auto bn = gum::BayesNet<double>::fastPrototype("A->B;C->D");
      gum::CausalModel<double> cm(bn);

      // --- parents / children (by name)
      {
        auto pB = cm.parents("B");
        TS_ASSERT_EQUALS(pB.size(), 1u);
        TS_ASSERT(pB.contains(cm.idFromName("A")));

        auto cA = cm.children("A");
        TS_ASSERT_EQUALS(cA.size(), 1u);
        TS_ASSERT(cA.contains(cm.idFromName("B")));

        auto pC = cm.parents("C");
        TS_ASSERT_EQUALS(pC.size(), 0u);

        auto cD = cm.children("D");
        TS_ASSERT_EQUALS(cD.size(), 0u);
      }

      // --- same via NodeId overloads (should match)
      {
        auto idB = bn.idFromName("B");
        auto idA = bn.idFromName("A");
        auto idC = bn.idFromName("C");
        auto idD = bn.idFromName("D");

        auto pB = cm.parents(idB);
        TS_ASSERT_EQUALS(pB.size(), 1u);
        TS_ASSERT(pB.contains(idA));

        auto cA = cm.children(idA);
        TS_ASSERT_EQUALS(cA.size(), 1u);
        TS_ASSERT(cA.contains(idB));

        TS_ASSERT_EQUALS(cm.parents(idC).size(), 0u);
        TS_ASSERT_EQUALS(cm.children(idD).size(), 0u);
      }

      // --- connected components: initially 2 ( {A,B} and {C,D} )
      {
        auto comps = cm.connectedComponents();
        TS_ASSERT_EQUALS(comps.size(), 2u);
      }

      // Add latent U that confounds B and C -> bridges the two components
      {
        std::vector<gum::NodeId> childIds{ bn.idFromName("B"), bn.idFromName("C") };
        cm.addLatentVariable("U", childIds /*keepArcs default = false*/);

        // Parents now reflect the latent
        auto pB = cm.parents("B");
        TS_ASSERT_EQUALS(pB.size(), 2u);              // {A, U}
        TS_ASSERT(pB.contains(cm.idFromName("A")));
        TS_ASSERT(pB.contains(cm.idFromName("U")));

        auto pC = cm.parents("C");
        TS_ASSERT_EQUALS(pC.size(), 1u);              // {U}
        TS_ASSERT(pC.contains(cm.idFromName("U")));

        auto cU = cm.children("U");
        TS_ASSERT_EQUALS(cU.size(), 2u);              // {B, C}
        TS_ASSERT(cU.contains(cm.idFromName("B")));
        TS_ASSERT(cU.contains(cm.idFromName("C")));

        // Components collapse to 1 due to the latent bridge
        auto comps2 = cm.connectedComponents();
        TS_ASSERT_EQUALS(comps2.size(), 1u);
      }
    }

    // X <- Z -> Y (observed confounder), X -> Y present:
    // backdoor should return {Z} (non-empty, observed-only).
    GUM_ACTIVE_TEST(BackdoorWithObservedConfounder) {
      auto bn = gum::BayesNet<double>::fastPrototype("Z->X;Z->Y;X->Y");
      gum::CausalModel<double> cm(bn);

      const auto X = bn.idFromName("X");
      const auto Y = bn.idFromName("Y");
      const auto Z = bn.idFromName("Z");

      gum::NodeSet bd = cm.backDoor(X, Y);
      TS_ASSERT(!bd.empty());
      TS_ASSERT_EQUALS(bd.size(), 1u);
      TS_ASSERT(bd.contains(Z));

      // Returned set must not contain latents.
      for (auto n : bd) TS_ASSERT(!cm.latentVariablesIds().contains(n));
    }

    // Only latent confounding U -> {X, Y}, no observed Z:
    // backdoor should find no observed adjustment set => empty.
    GUM_ACTIVE_TEST(BackdoorWithLatentOnly_NoObservedSet) {
      auto bn = gum::BayesNet<double>::fastPrototype("X->Y");
      gum::CausalModel<double> cm(bn);

      std::vector<gum::NodeId> kids{bn.idFromName("X"), bn.idFromName("Y")};
      cm.addLatentVariable("U", kids);

      gum::NodeSet bd = cm.backDoor(bn.idFromName("X"), bn.idFromName("Y"));
      TS_ASSERT(bd.empty());
    }

    // Classic frontdoor with latent confounding:
    // U -> {X, Y}, X -> Z -> Y, no direct X->Y.
    // frontdoor should return {Z}.
    GUM_ACTIVE_TEST(FrontdoorClassicWithLatent_ReturnsMediator) {
      auto bn = gum::BayesNet<double>::fastPrototype("X->Z;Z->Y");
      gum::CausalModel<double> cm(bn);

      std::vector<gum::NodeId> kids{bn.idFromName("X"), bn.idFromName("Y")};
      cm.addLatentVariable("U", kids);

      const auto X = bn.idFromName("X");
      const auto Y = bn.idFromName("Y");
      const auto Z = bn.idFromName("Z");

      gum::NodeSet fd = cm.frontDoor(X, Y);
      TS_ASSERT(!fd.empty());
      TS_ASSERT_EQUALS(fd.size(), 1u);
      TS_ASSERT(fd.contains(Z));

      for (auto n : fd) TS_ASSERT(!cm.latentVariablesIds().contains(n));
    }

    // If X -> Y is present, our frontdoor enumerator short-circuits (no frontdoor set).
    GUM_ACTIVE_TEST(FrontdoorFailsWithDirectEdge) {
      auto bn = gum::BayesNet<double>::fastPrototype("X->Y;X->Z;Z->Y");
      gum::CausalModel<double> cm(bn);

      gum::NodeSet fd = cm.frontDoor(bn.idFromName("X"), bn.idFromName("Y"));
      TS_ASSERT(fd.empty());
    }

    // Passing a latent as cause/effect must raise (guard in CausalModel).
    GUM_ACTIVE_TEST(ThrowsWhenCauseOrEffectIsLatent) {
      auto bn = gum::BayesNet<double>::fastPrototype("X->Z;Z->Y");
      gum::CausalModel<double> cm(bn);

      std::vector<gum::NodeId> kids{bn.idFromName("X"), bn.idFromName("Y")};
      cm.addLatentVariable("U", kids);

      const auto U = cm.idFromName("U");
      const auto X = bn.idFromName("X");
      const auto Y = bn.idFromName("Y");

      TS_ASSERT_THROWS(cm.backDoor(U, Y), const gum::InvalidArgument&);
      TS_ASSERT_THROWS(cm.frontDoor(X, U), const gum::InvalidArgument&);
    }

    // Backdoor on Simpson-style BN: Z->X, Z->Y, X->Y.
    // Expect AST = sum_Z [ P(Y|X,Z) * P(Z) ].
    GUM_ACTIVE_TEST(GetBackDoorTree_Simpson_StructureAndManualParity) {
      auto bn = gum::BayesNet<double>::fastPrototype("Z->X;Z->Y;X->Y");
      gum::CausalModel<double> cm(bn);

      const auto X = bn.idFromName("X");
      const auto Y = bn.idFromName("Y");
      const auto Z = bn.idFromName("Z");

      gum::NodeSet Zset; Zset.insert(Z);

      // 1) builder under test
      auto built = cm.getBackDoorTree(X, Y, Zset);
      auto builtStr = built->toString();

      TS_ASSERT_DIFFERS(built.get(), (void*)nullptr);
      TS_ASSERT(builtStr.find("sum on Z") != std::string::npos);
      TS_ASSERT(builtStr.find("P(Y|X,Z)") != std::string::npos);
      TS_ASSERT(builtStr.find("joint P(Z)") != std::string::npos);

      // 2) manual AST (must match)
      gum::Set<std::string> lhsY; lhsY.insert("Y");
      gum::Set<std::string> knwXZ; knwXZ.insert("X"); knwXZ.insert("Z");
      auto Py_xz = std::make_unique<gum::ASTposteriorProba<double>>(cm.causalDAG(), cm.id2name(), lhsY, knwXZ);

      gum::Set<std::string> Znames; Znames.insert("Z");
      auto Pz = std::make_unique<gum::ASTjointProba<double>>(Znames);

      std::vector<std::unique_ptr<gum::ASTtree<double>>> prodTerms;
      prodTerms.emplace_back(std::move(Py_xz));
      prodTerms.emplace_back(std::move(Pz));
      auto prod = gum::productOfTrees<double>(std::move(prodTerms));

      auto manual = std::make_unique<gum::ASTsum<double>>(std::vector<std::string>{"Z"}, std::move(prod));
      TS_ASSERT_EQUALS(built->toString(), manual->toString());
    }

    // Backdoor with empty Z: expect reduction to P(Y|X).
    GUM_ACTIVE_TEST(GetBackDoorTree_EmptyZ_ReducesToP_Y_given_X) {
      auto bn = gum::BayesNet<double>::fastPrototype("X->Y");
      gum::CausalModel<double> cm(bn);

      const auto X = bn.idFromName("X");
      const auto Y = bn.idFromName("Y");

      gum::NodeSet Zset; // empty

      auto built = cm.getBackDoorTree(X, Y, Zset);
      TS_ASSERT_DIFFERS(built.get(), (void*)nullptr);

      // manual P(Y|X)
      gum::Set<std::string> lhsY; lhsY.insert("Y");
      gum::Set<std::string> knwX; knwX.insert("X");
      auto manual = std::make_unique<gum::ASTposteriorProba<double>>(cm.causalDAG(), cm.id2name(), lhsY, knwX);

      TS_ASSERT_EQUALS(built->toString(), manual->toString());
    }

    // Frontdoor classic: X->Z->Y with latent U -> {X,Y}.
    // Expect AST = sum_Z [ P(Z|X) * sum_X [ P(Y|X,Z) * P(X) ] ].
    GUM_ACTIVE_TEST(GetFrontDoorTree_ClassicMediator_ParityWithManual) {
      auto bn = gum::BayesNet<double>::fastPrototype("X->Z;Z->Y");
      gum::CausalModel<double> cm(bn);

      // add latent confounder (not used by the tree, but validates preconditions)
      std::vector<gum::NodeId> kids{bn.idFromName("X"), bn.idFromName("Y")};
      cm.addLatentVariable("U", kids);

      auto X = cm.idFromName("X");
      auto Y = cm.idFromName("Y");
      gum::NodeSet Zset; Zset.insert(cm.idFromName("Z"));

      // Built automatically by getFrontDoorTree
      auto built = cm.getFrontDoorTree(X, Y, Zset);

      // --- Manual construction of front-door formula ---
      // P(Z | X)
      gum::Set<std::string> Znames{"Z"};
      gum::Set<std::string> condX{"X"};
      auto Pz_given_x = std::make_unique<gum::ASTposteriorProba<double>>(cm.causalDAG(), cm.id2name(), Znames, condX);

      // P(Y | X,Z)
      gum::Set<std::string> Yset{"Y"};
      gum::Set<std::string> condXZ{"X","Z"};
      auto Py_given_xz = std::make_unique<gum::ASTposteriorProba<double>>(cm.causalDAG(), cm.id2name(), Yset, condXZ);

      // P(X)
      gum::Set<std::string> Xset{"X"};
      auto Px = std::make_unique<gum::ASTjointProba<double>>(Xset);

      // Inner: Σ_X P(Y|X,Z) · P(X)
      auto inner_prod = std::make_unique<gum::ASTmult<double>>(std::move(Py_given_xz), std::move(Px));
      auto inner_sum  = std::make_unique<gum::ASTsum<double>>(std::vector<std::string>{"X"}, std::move(inner_prod));

      // Outer: Σ_Z P(Z|X) · (…)
      auto outer_prod = std::make_unique<gum::ASTmult<double>>(std::move(Pz_given_x), std::move(inner_sum));
      auto manual     = std::make_unique<gum::ASTsum<double>>(std::vector<std::string>{"Z"}, std::move(outer_prod));

      // --- Comparison ---
      TS_ASSERT_EQUALS(built->toString(), manual->toString());
    }

    // Preconditions: zset must be non-empty for frontdoor; cause/effect and zset must be observed.
    GUM_ACTIVE_TEST(FormulaBuilders_Preconditions_ThrowOnLatentsAndEmptyZ) {
      auto bn = gum::BayesNet<double>::fastPrototype("X->Z;Z->Y");
      gum::CausalModel<double> cm(bn);

      // add latent U -> {X,Y}
      std::vector<gum::NodeId> kids{bn.idFromName("X"), bn.idFromName("Y")};
      cm.addLatentVariable("U", kids);

      const auto U = cm.idFromName("U");
      const auto X = bn.idFromName("X");
      const auto Y = bn.idFromName("Y");
      const auto Z = bn.idFromName("Z");

      // frontdoor: empty Z → InvalidArgument
      gum::NodeSet emptyZ;
      TS_ASSERT_THROWS(cm.getFrontDoorTree(X, Y, emptyZ), const gum::InvalidArgument&);

      // backdoor: latent as cause → InvalidArgument
      gum::NodeSet Zset; Zset.insert(Z);
      TS_ASSERT_THROWS(cm.getBackDoorTree(U, Y, Zset), const gum::InvalidArgument&);

      // backdoor: latent inside Zset → InvalidArgument
      gum::NodeSet ZsetLat; ZsetLat.insert(U);
      TS_ASSERT_THROWS(cm.getBackDoorTree(X, Y, ZsetLat), const gum::InvalidArgument&);

      // frontdoor: latent as effect → InvalidArgument
      TS_ASSERT_THROWS(cm.getFrontDoorTree(X, U, Zset), const gum::InvalidArgument&);
    }

  };


}  // namespace gum_tests
