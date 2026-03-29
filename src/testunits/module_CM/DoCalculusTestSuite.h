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
#include <agrum/CM/tools/doCalculus.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  DoCalculus
#define GUM_CURRENT_MODULE CM

namespace gum_tests {

  struct DoCalculusTestSuite {
    public:
    // Backdoor on Simpson-style BN: Z->X, Z->Y, X->Y.
    // Expect AST = sum_Z [ P(Y|X,Z) * P(Z) ].
    static void testGetBackDoorTree_Simpson_StructureAndManualParity() {
      auto                       bn = gum::BayesNet< double >::fastPrototype("Z->X;Z->Y;X->Y");
      gum::CausalModel< double > cm(bn);

      const auto X = bn.idFromName("X");
      const auto Y = bn.idFromName("Y");
      const auto Z = bn.idFromName("Z");

      gum::NodeSet Zset;
      Zset.insert(Z);

      // 1) builder under test
      gum::DoCalculus< double > dc(cm);
      auto                      built    = dc.getBackDoorTree(X, Y, Zset);
      auto                      builtStr = built->toString();

      CHECK_NE(built.get(), nullptr);
      CHECK_NE(builtStr.find("sum on Z"), std::string::npos);
      CHECK_NE(builtStr.find("P(Y|X,Z)"), std::string::npos);
      CHECK_NE(builtStr.find("joint P(Z)"), std::string::npos);

      // 2) manual AST (must match)
      gum::Set< std::string > lhsY;
      lhsY.insert("Y");
      gum::Set< std::string > knwXZ;
      knwXZ.insert("X");
      knwXZ.insert("Z");
      auto Py_xz = std::make_unique< gum::ASTposteriorProba< double > >(cm.causalDAG(),
                                                                        cm.id2name(),
                                                                        lhsY,
                                                                        knwXZ);

      gum::Set< std::string > Znames;
      Znames.insert("Z");
      auto Pz = std::make_unique< gum::ASTjointProba< double > >(Znames);

      std::vector< std::unique_ptr< gum::ASTtree< double > > > prodTerms;
      prodTerms.emplace_back(std::move(Py_xz));
      prodTerms.emplace_back(std::move(Pz));
      auto prod = gum::productOfTrees< double >(std::move(prodTerms));

      auto manual = std::make_unique< gum::ASTsum< double > >(std::vector< std::string >{"Z"},
                                                              std::move(prod));
      CHECK_EQ(built->toString(), manual->toString());
    }

    // Backdoor with empty Z: expect reduction to P(Y|X).
    static void testGetBackDoorTree_EmptyZ_ReducesToP_Y_given_X() {
      auto                       bn = gum::BayesNet< double >::fastPrototype("X->Y");
      gum::CausalModel< double > cm(bn);

      const auto X = bn.idFromName("X");
      const auto Y = bn.idFromName("Y");

      gum::NodeSet Zset;   // empty

      gum::DoCalculus< double > dc(cm);
      auto                      built = dc.getBackDoorTree(X, Y, Zset);

      CHECK_NE(built.get(), nullptr);

      // manual P(Y|X)
      gum::Set< std::string > lhsY;
      lhsY.insert("Y");
      gum::Set< std::string > knwX;
      knwX.insert("X");
      auto manual = std::make_unique< gum::ASTposteriorProba< double > >(cm.causalDAG(),
                                                                         cm.id2name(),
                                                                         lhsY,
                                                                         knwX);

      CHECK_EQ(built->toString(), manual->toString());
    }

    // Frontdoor classic: X->Z->Y with latent U -> {X,Y}.
    // Expect AST = sum_Z [ P(Z|X) * sum_X [ P(Y|X,Z) * P(X) ] ].
    static void testGetFrontDoorTree_ClassicMediator_ParityWithManual() {
      auto                       bn = gum::BayesNet< double >::fastPrototype("X->Z;Z->Y");
      gum::CausalModel< double > cm(bn);

      // add latent confounder (not used by the tree, but validates preconditions)
      std::vector< gum::NodeId > kids{bn.idFromName("X"), bn.idFromName("Y")};
      cm.addLatentVariable("U", kids);

      auto         X = cm.idFromName("X");
      auto         Y = cm.idFromName("Y");
      gum::NodeSet Zset;
      Zset.insert(cm.idFromName("Z"));

      // Built automatically by getFrontDoorTree
      gum::DoCalculus< double > dc(cm);
      auto                      built = dc.getFrontDoorTree(X, Y, Zset);

      // --- Manual construction of front-door formula ---
      // P(Z | X)
      gum::Set< std::string > Znames{"Z"};
      gum::Set< std::string > condX{"X"};
      auto Pz_given_x = std::make_unique< gum::ASTposteriorProba< double > >(cm.causalDAG(),
                                                                             cm.id2name(),
                                                                             Znames,
                                                                             condX);

      // P(Y | X,Z)
      gum::Set< std::string > Yset{"Y"};
      gum::Set< std::string > condXZ{"X", "Z"};
      auto Py_given_xz = std::make_unique< gum::ASTposteriorProba< double > >(cm.causalDAG(),
                                                                              cm.id2name(),
                                                                              Yset,
                                                                              condXZ);

      // P(X)
      gum::Set< std::string > Xset{"X"};
      auto                    Px = std::make_unique< gum::ASTjointProba< double > >(Xset);

      // Inner: Σ_X P(Y|X,Z) · P(X)
      auto inner_prod
          = std::make_unique< gum::ASTmult< double > >(std::move(Py_given_xz), std::move(Px));
      auto inner_sum = std::make_unique< gum::ASTsum< double > >(std::vector< std::string >{"X"},
                                                                 std::move(inner_prod));

      // Outer: Σ_Z P(Z|X) · (…)
      auto outer_prod
          = std::make_unique< gum::ASTmult< double > >(std::move(Pz_given_x), std::move(inner_sum));
      auto manual = std::make_unique< gum::ASTsum< double > >(std::vector< std::string >{"Z"},
                                                              std::move(outer_prod));

      // --- Comparison ---
      CHECK_EQ(built->toString(), manual->toString());
    }

    // Preconditions: zset must be non-empty for frontdoor; cause/effect and zset must be observed.
    static void testFormulaBuilders_Preconditions_ThrowOnLatentsAndEmptyZ() {
      auto                       bn = gum::BayesNet< double >::fastPrototype("X->Z;Z->Y");
      gum::CausalModel< double > cm(bn);

      // add latent U -> {X,Y}
      std::vector< gum::NodeId > kids{bn.idFromName("X"), bn.idFromName("Y")};
      cm.addLatentVariable("U", kids);

      const auto U = cm.idFromName("U");
      const auto X = bn.idFromName("X");
      const auto Y = bn.idFromName("Y");
      const auto Z = bn.idFromName("Z");

      gum::DoCalculus< double > dc(cm);

      // frontdoor: empty Z → InvalidArgument
      gum::NodeSet emptyZ;
      CHECK_THROWS_AS(dc.getFrontDoorTree(X, Y, emptyZ), const gum::InvalidArgument&);

      // backdoor: latent as cause → InvalidArgument
      gum::NodeSet Zset;
      Zset.insert(Z);
      CHECK_THROWS_AS(dc.getBackDoorTree(U, Y, Zset), const gum::InvalidArgument&);

      // backdoor: latent inside Zset → InvalidArgument
      gum::NodeSet ZsetLat;
      ZsetLat.insert(U);
      CHECK_THROWS_AS(dc.getBackDoorTree(X, Y, ZsetLat), const gum::InvalidArgument&);

      // frontdoor: latent as effect → InvalidArgument
      CHECK_THROWS_AS(dc.getFrontDoorTree(X, U, Zset), const gum::InvalidArgument&);
    }

    // 1) IDC promotion smoke: Z→X→Y, query P(Y | do(X), Z).
    // We only require: builds and evaluates (no structure parity).
    static void testIDC_Promotion_Reduces_Smoke() {
      auto bn = gum::BayesNet< double >::fastPrototype("Z->X;X->Y");
      bn.generateCPTs();
      gum::CausalModel< double > cm(bn);
      gum::DoCalculus< double >  dc(cm);

      auto ast = dc.doCalculusWithObservation(std::vector< std::string >{"Y"},    // on
                                              std::vector< std::string >{"X"},    // doing
                                              std::vector< std::string >{"Z"});   // knowing

      CHECK_NE(ast, nullptr);
      CHECK_NOTHROW({ (void)ast->eval(bn); });
    }

    // 2) IDC promotion on a collider: X→Y←W, query P(Y | do(X), W).
    // Again: builds and evaluates (no parity).
    static void testIDC_Promotion_OnCollider_Smoke() {
      auto bn = gum::BayesNet< double >::fastPrototype("X->Y;W->Y");
      bn.generateCPTs();
      gum::CausalModel< double > cm(bn);
      gum::DoCalculus< double >  dc(cm);

      auto ast = dc.doCalculusWithObservation(std::vector< std::string >{"Y"},
                                              std::vector< std::string >{"X"},
                                              std::vector< std::string >{"W"});

      CHECK_NE(ast, nullptr);
      CHECK_NOTHROW({ (void)ast->eval(bn); });
    }

    // ID should restrict to ancestors of Y under do(X).
    // We check structurally that the AST does not mention a detached node name.
    static void testID_AncestorRestriction_AST_DoesNotMention_Irrelevant() {
      // W only points to Z, not to Y nor X; irrelevant for P(Y|do(X)).
      auto bn = gum::BayesNet< double >::fastPrototype("W->Z;X->Y;Z");
      bn.generateCPTs();
      gum::CausalModel< double > cm(bn);
      gum::DoCalculus< double >  dc(cm);

      auto ast = dc.doCalculus(std::vector< std::string >{"Y"}, std::vector< std::string >{"X"});

      CHECK_NE(ast, nullptr);
      auto s = ast->toString();
      // Heuristic structural check: expression shouldn’t include "W".
      CHECK_EQ(s.find("W"), std::string::npos);
    }

    // ======================================================================
    // Optional: multi c-component product path smoke test
    // G: A->B, C->D (two independent c-components), query P(B,D | do(A,C)).
    // Just assert we build something (no throw). Structure may vary.
    // ======================================================================
    static void testID_MultiComponent_Product_Smoke() {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B;C->D");
      bn.generateCPTs();
      gum::CausalModel< double > cm(bn);
      gum::DoCalculus< double >  dc(cm);

      CHECK_NOTHROW({
        auto ast = dc.doCalculus(std::vector< std::string >{"B", "D"},
                                 std::vector< std::string >{"A", "C"});
      });
      {
        auto ast = dc.doCalculus(std::vector< std::string >{"B", "D"},
                                 std::vector< std::string >{"A", "C"});
        CHECK_NE(ast, nullptr);
        (void)ast->toString();   // ensure it’s buildable/printable
      }
    }
  };

  GUM_TEST_ACTIF(GetBackDoorTree_Simpson_StructureAndManualParity)
  GUM_TEST_ACTIF(GetBackDoorTree_EmptyZ_ReducesToP_Y_given_X)
  GUM_TEST_ACTIF(GetFrontDoorTree_ClassicMediator_ParityWithManual)
  GUM_TEST_ACTIF(FormulaBuilders_Preconditions_ThrowOnLatentsAndEmptyZ)
  GUM_TEST_ACTIF(IDC_Promotion_Reduces_Smoke)
  GUM_TEST_ACTIF(IDC_Promotion_OnCollider_Smoke)
  GUM_TEST_ACTIF(ID_AncestorRestriction_AST_DoesNotMention_Irrelevant)
  GUM_TEST_ACTIF(ID_MultiComponent_Product_Smoke)
}   // namespace gum_tests
