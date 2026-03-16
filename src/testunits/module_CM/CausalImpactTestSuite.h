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

#include <string>
#include <vector>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/agrum.h>

#include <agrum/base/multidim/tensor.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/CM/causalImpact.h>
#include <agrum/CM/causalModel.h>
#include <agrum/CM/tools/causalFormula.h>

#include <agrum/base/core/utils_string.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  CausalImpact
#define GUM_CURRENT_MODULE CM

namespace gum_tests {

  struct CausalImpactTestSuite {
    using StrSet = gum::Set< std::string >;

    static StrSet names(std::initializer_list< const char* > il) {
      StrSet s;
      for (auto* p: il)
        s.insert(std::string(p));
      return s;
    }

    static gum::VariableSet vset(const gum::BayesNet< double >&       bn,
                                 std::initializer_list< const char* > il) {
      gum::VariableSet vs;
      for (auto* p: il)
        vs.insert(&bn.variableFromName(p));   // expects pointers
      return vs;
    }

    // Instantiation slaved to a tensor; set values by name/label.
    static gum::Instantiation
        instFor(const gum::Tensor< double >&                                   t,
                std::initializer_list< std::pair< const char*, const char* > > assigns) {
      gum::Instantiation I(
          const_cast< gum::Tensor< double >& >(t));   // register as slave (non-const API)
      for (const auto& kv: assigns)
        I.chgVal(std::string(kv.first), std::string(kv.second));
      return I;
    }

    // Build an Instantiation slaved to tensor `t`, and set values using var-name/label pairs.
    // Example use:
    //   auto inst = _inst(got, { {"Patient","Healed"}, {"Drug","With"} });
    //   double val = got.get_(inst);
    static gum::Instantiation
        _inst(const gum::Tensor< double >&                                   t,
              std::initializer_list< std::pair< const char*, const char* > > assigns) {
      gum::Instantiation I(t);   // slaved to t; has exactly t's vars in the right order
      for (const auto& p: assigns)
        I.chgVal(std::string(p.first), std::string(p.second));
      return I;
    }

    public:
    // A) Null effect via d-separation: X -> Z -> Y with knowing={Z}
    // Expected: P(Y | do(X), Z) == P(Y | Z) == bn.cpt("Y")
    static void testNullEffect_DSeparation() {
      auto bn = gum::BayesNet< double >::fastPrototype("X->Z->Y");
      bn.generateCPTs();
      gum::CausalModel< double > cm(bn);

      gum::CausalImpact< double > ci(cm, names({"Y"}), names({"X"}), names({"Z"}));

      CHECK_NOTHROW({
        auto got = ci.eval();
        auto exp = bn.cpt("Y");
        GUM_CHECK_TENSOR_ALMOST_EQUALS(got, exp);
      });

      // Repeat with directDoCalculus: force do-calculus path, should match above
      gum::CausalImpact< double > ci_dc(cm, names({"Y"}), names({"X"}), names({"Z"}), true);
      CHECK_NOTHROW({
        auto got = ci_dc.eval();
        auto exp = bn.cpt("Y");
        GUM_CHECK_TENSOR_ALMOST_EQUALS(got, exp);
      });
    }

    // B) Backdoor with observed confounder:
    // BN: Gender -> Drug -> Patient ; Gender -> Patient
    // Expected: sum_G P(Patient | Drug, G) P(G) == (cpt(Patient) * cpt(Gender)).sumOut(Gender)
    static void testBackdoor_WithObservedConfounder() {
      // Repeat with directDoCalculus: force do-calculus path, should match above
      auto bn = gum::BayesNet< double >::fastPrototype(
          "Gender{M|F}->Drug{No|Yes}->Patient{No|Yes};Gender{M|F}->Patient{No|Yes}");
      bn.generateCPTs();
      gum::CausalModel< double > cm(bn);

      gum::CausalImpact< double > ci(cm, names({"Patient"}), names({"Drug"}), StrSet{});

      CHECK_NOTHROW({
        auto got = ci.eval();
        auto exp = (bn.cpt("Patient") * bn.cpt("Gender")).sumOut(vset(bn, {"Gender"}));
        GUM_CHECK_TENSOR_ALMOST_EQUALS(got, exp);
      });

      gum::CausalImpact< double > ci_dc(cm, names({"Patient"}), names({"Drug"}), StrSet{}, true);
      CHECK_NOTHROW({
        auto got = ci_dc.eval();
        auto exp = (bn.cpt("Patient") * bn.cpt("Gender")).sumOut(vset(bn, {"Gender"}));
        GUM_CHECK_TENSOR_ALMOST_EQUALS(got, exp);
      });
    }

    // C) Frontdoor (canonical): X->Z->Y, with latent U between X and Y
    // Expected: sum_z P(z|x) P(y|z) == (cpt("Z") * cpt("Y")).sumOut(Z)
    static void testFrontdoor_SimpleLatent() {
      // Repeat with directDoCalculus: force do-calculus path, should match above
      auto bn = gum::BayesNet< double >::fastPrototype("X->Z->Y");
      bn.generateCPTs();

      // Latent descriptor must use NodeIds
      gum::LatentDescriptorVector lat;
      lat.emplace_back("U", std::vector< gum::NodeId >{bn.idFromName("X"), bn.idFromName("Y")});

      gum::CausalModel< double > cm(bn, lat, /*assumeNonSpurious=*/false);

      gum::CausalImpact< double > ci(cm, names({"Y"}), names({"X"}), StrSet{});

      CHECK_NOTHROW({
        auto got = ci.eval();
        auto exp = (bn.cpt("Z") * bn.cpt("Y")).sumOut(vset(bn, {"Z"}));
        GUM_CHECK_TENSOR_ALMOST_EQUALS(got, exp);
      });

      gum::CausalImpact< double > ci_dc(cm, names({"Y"}), names({"X"}), StrSet{}, true);
      CHECK_NOTHROW({
        auto got = ci_dc.eval();
        auto exp = (bn.cpt("Z") * bn.cpt("Y")).sumOut(vset(bn, {"Z"}));
        GUM_CHECK_TENSOR_ALMOST_EQUALS(got, exp);
      });
    }

    // D) “Trivial” backdoor: X -> Y  (backdoor set is ∅)
    // Expected: P(Y|do(X)) == P(Y|X) == cpt("Y")
    static void testBackdoor_Trivial_XtoY() {
      // Repeat with directDoCalculus: force do-calculus path, should match above
      auto bn = gum::BayesNet< double >::fastPrototype("X->Y");
      bn.generateCPTs();
      gum::CausalModel< double > cm(bn);

      gum::CausalImpact< double > ci(cm, names({"Y"}), names({"X"}), StrSet{}, true);

      CHECK_NOTHROW({
        auto got = ci.eval();
        auto exp = bn.cpt("Y");
        GUM_CHECK_TENSOR_ALMOST_EQUALS(got, exp);
      });

      gum::CausalImpact< double > ci_dc(cm, names({"Y"}), names({"X"}), StrSet{}, true);
      CHECK_NOTHROW({
        auto got = ci_dc.eval();
        auto exp = bn.cpt("Y");
        GUM_CHECK_TENSOR_ALMOST_EQUALS(got, exp);
      });
    }

    // E) Hedge / unidentifiable:
    // BN: X->Y; U ; latent Z -> {X,Y,U}, assumeNonSpurious=true
    // Expected: evaluation throws (null AST / hedge)
    static void testWHY19_Hedge_Unidentifiable() {
      // Repeat with directDoCalculus: force do-calculus path, should throw as above
      auto bn = gum::BayesNet< double >::fastPrototype("X->Y;U");
      bn.generateCPTs();

      gum::LatentDescriptorVector lat;
      lat.emplace_back(
          "Z",
          std::vector< gum::NodeId >{bn.idFromName("X"), bn.idFromName("Y"), bn.idFromName("U")});

      gum::CausalModel< double > cm(bn, lat, /*assumeNonSpurious=*/true);

      gum::CausalImpact< double > ci(cm, names({"Y"}), names({"X"}), StrSet{});

      CHECK_THROWS((void)ci.eval());

      gum::CausalImpact< double > ci_dc(cm, names({"Y"}), names({"X"}), StrSet{}, true);
      CHECK_THROWS((void)ci_dc.eval());
    }

    // F) Pairwise overlaps among on/doing/knowing must throw at construction
    static void testPairwiseOverlapsThrow() {
      // Repeat with directDoCalculus: force do-calculus path, should throw as above
      auto bn = gum::BayesNet< double >::fastPrototype("A->B");
      bn.generateCPTs();
      gum::CausalModel< double > cm(bn);

      CHECK_THROWS((gum::CausalImpact< double >(cm, names({"B"}), names({"B"}), StrSet{})));
      CHECK_THROWS((gum::CausalImpact< double >(cm, names({"B"}), names({"A"}), names({"B"}))));
      CHECK_THROWS((gum::CausalImpact< double >(cm, names({"B"}), names({"A"}), names({"A"}))));

      // Repeat with directDoCalculus
      CHECK_THROWS((gum::CausalImpact< double >(cm, names({"B"}), names({"B"}), StrSet{}, true)));
      CHECK_THROWS(
          (gum::CausalImpact< double >(cm, names({"B"}), names({"A"}), names({"B"}), true)));
      CHECK_THROWS(
          (gum::CausalImpact< double >(cm, names({"B"}), names({"A"}), names({"A"}), true)));
    }

    // G) Accented variable names are fine
    static void testAccentsInVariables() {
      // Repeat with directDoCalculus: force do-calculus path, should match above
      auto bn = gum::BayesNet< double >::fastPrototype("héhé->hoho");
      bn.generateCPTs();
      gum::CausalModel< double > cm(bn);

      gum::CausalImpact< double > ci(cm, names({"hoho"}), names({"héhé"}), StrSet{});

      CHECK_NOTHROW({ (void)ci.eval(); });

      gum::CausalImpact< double > ci_dc(cm, names({"hoho"}), names({"héhé"}), StrSet{}, true);
      CHECK_NOTHROW({ (void)ci_dc.eval(); });
    }

    // H) Complex “From R”-style model: should not raise during eval
    // TODO :Make active test when ID algorithm is implemented
    static void testFromR_NoRaise() {
      // Repeat with directDoCalculus: force do-calculus path, should not throw
      auto m = gum::BayesNet< double >::fastPrototype("z2->x->z1->y;z2->z1;z2->z3->y");
      m.generateCPTs();

      gum::CausalModel< double > cm(m,
                                    {{"X-Z2", {m.idFromName("x"), m.idFromName("z2")}},
                                     {"X-Z3", {m.idFromName("x"), m.idFromName("z3")}},
                                     {"X-Y", {m.idFromName("x"), m.idFromName("y")}},
                                     {"Y-Z2", {m.idFromName("y"), m.idFromName("z2")}}},
                                    /*assumeNonSpurious=*/true);

      CHECK_NOTHROW({
        gum::CausalImpact< double > ci(cm, names({"y", "z2", "z1", "z3"}), names({"x"}), StrSet{});
      });
      CHECK_NOTHROW({
        gum::CausalImpact< double > ci_dc(cm,
                                          names({"y", "z2", "z1", "z3"}),
                                          names({"x"}),
                                          StrSet{},
                                          true);
      });
    }

    // I) Simpson's paradox: explicit CPTs, check interventional via backdoor
    // BN: Gender -> Drug -> Patient ; Gender -> Patient
    // With fixed CPTs (the classic Simpson example), we should have:
    // P(Healed | do(Drug=With)) = 0.45 and P(Healed | do(Drug=Without)) = 0.60
    // but observationally P(Healed | Drug=With) > P(Healed | Drug=Without).
    static void testSimpsonParadox_BackdoorAverage() {
      // Repeat with directDoCalculus: force do-calculus path, should match above
      auto bn = gum::BayesNet< double >::fastPrototype(
          "Gender{F|M}->Drug{Without|With}->Patient{Sick|Healed}<-Gender");

      // P(Gender)
      bn.cpt(bn.idFromName("Gender")).fillWith({0.5, 0.5});   // F, M

      // P(Drug | Gender)
      // Parent order is [Gender], child states {Without, With}
      // Gender=F: 0.25,0.75 ; Gender=M: 0.75,0.25
      bn.cpt(bn.idFromName("Drug"))
          .fillWith({
              0.25,
              0.75,   // Gender=F
              0.75,
              0.25    // Gender=M
          });

      // P(Patient | Drug, Gender)
      // Parent order is [Drug, Gender]; child states {Sick, Healed}
      // F: Without->0.4 Healed, With->0.2 Healed
      // M: Without->0.8 Healed, With->0.7 Healed
      bn.cpt(bn.idFromName("Patient"))
          .fillWith({
              // Gender=F
              0.6,
              0.4,   // Drug=Without , Gender=F
              0.8,
              0.2,   // Drug=With    , Gender=F
              // Gender=M
              0.2,
              0.8,   // Drug=Without , Gender=M
              0.3,
              0.7    // Drug=With    , Gender=M
          });

      gum::CausalModel< double > cm(bn);

      gum::CausalImpact< double > ci(cm, names({"Patient"}), names({"Drug"}), StrSet{});

      auto got = ci.eval();
      auto exp = (bn.cpt(bn.idFromName("Patient")) * bn.cpt(bn.idFromName("Gender")))
                     .sumOut(vset(bn, {"Gender"}));
      GUM_CHECK_TENSOR_ALMOST_EQUALS(got, exp);

      CHECK((got.get(_inst(got, {{"Patient", "Healed"}, {"Drug", "With"}})))
            == doctest::Approx(0.45).epsilon(1e-10));

      CHECK((got.get(_inst(got, {{"Patient", "Healed"}, {"Drug", "Without"}})))
            == doctest::Approx(0.60).epsilon(1e-10));

      gum::CausalImpact< double > ci_dc(cm, names({"Patient"}), names({"Drug"}), StrSet{}, true);
      auto                        got_dc = ci_dc.eval();
      GUM_CHECK_TENSOR_ALMOST_EQUALS(got_dc, exp);
      CHECK((got_dc.get(_inst(got_dc, {{"Patient", "Healed"}, {"Drug", "With"}})))
            == doctest::Approx(0.45).epsilon(1e-10));
      CHECK((got_dc.get(_inst(got_dc, {{"Patient", "Healed"}, {"Drug", "Without"}})))
            == doctest::Approx(0.60).epsilon(1e-10));
    }

    // J) Tobacco #1: Direct causality Smoking -> Cancer (trivial backdoor ∅)
    // Expected: P(Cancer | do(Smoking)) == P(Cancer | Smoking) == CPT(Cancer)
    static void testTobacco_DirectCausality_TrivialBackdoor() {
      // Repeat with directDoCalculus: force do-calculus path, should match above
      auto bn = gum::BayesNet< double >::fastPrototype("Smoking->Cancer");
      bn.generateCPTs();
      gum::CausalModel< double > cm(bn);

      gum::CausalImpact< double > ci(cm, names({"Cancer"}), names({"Smoking"}), StrSet{});

      CHECK_NOTHROW({
        auto got = ci.eval();
        auto exp = bn.cpt(bn.idFromName("Cancer"));
        GUM_CHECK_TENSOR_ALMOST_EQUALS(got, exp);
      });

      gum::CausalImpact< double > ci_dc(cm, names({"Cancer"}), names({"Smoking"}), StrSet{}, true);
      CHECK_NOTHROW({
        auto got = ci_dc.eval();
        auto exp = bn.cpt(bn.idFromName("Cancer"));
        GUM_CHECK_TENSOR_ALMOST_EQUALS(got, exp);
      });
    }

    // L) Tobacco #3: Frontdoor with mediator Tar and latent U between Smoking,Cancer
    // Graph: Smoking -> Tar -> Cancer, U -> {Smoking, Cancer} (latent)
    // Expected (frontdoor): P(Cancer | do(Smoking)) = sum_z P(z|Smoking) P(Cancer|z)
    static void testTobacco_Frontdoor_WithMediatorTar() {
      // Repeat with directDoCalculus: force do-calculus path, should match above
      auto bn = gum::BayesNet< double >::fastPrototype("Smoking->Tar->Cancer");
      bn.generateCPTs();

      gum::LatentDescriptorVector lat;
      lat.emplace_back(
          "U",
          std::vector< gum::NodeId >{bn.idFromName("Smoking"), bn.idFromName("Cancer")});

      gum::CausalModel< double > cm(bn, lat, /*assumeNonSpurious=*/false);

      gum::CausalImpact< double > ci(cm, names({"Cancer"}), names({"Smoking"}), StrSet{});

      CHECK_NOTHROW({
        auto got = ci.eval();
        auto exp = (bn.cpt(bn.idFromName("Tar")) * bn.cpt(bn.idFromName("Cancer")))
                       .sumOut(vset(bn, {"Tar"}));
        GUM_CHECK_TENSOR_ALMOST_EQUALS(got, exp);
      });

      gum::CausalImpact< double > ci_dc(cm, names({"Cancer"}), names({"Smoking"}), StrSet{}, true);
      CHECK_NOTHROW({
        auto got = ci_dc.eval();
        auto exp = (bn.cpt(bn.idFromName("Tar")) * bn.cpt(bn.idFromName("Cancer")))
                       .sumOut(vset(bn, {"Tar"}));
        GUM_CHECK_TENSOR_ALMOST_EQUALS(got, exp);
      });
    }

    // M) Tobacco #4: Unidentifiable hedge (latent confounder + direct edge kept)
    // Graph: Smoking -> Cancer and latent U -> {Smoking, Cancer}, assumeNonSpurious=true
    // Expected: Identification fails => eval() throws
    static void testTobacco_Unidentifiable_Hedge() {
      // Repeat with directDoCalculus: force do-calculus path, should throw as above
      auto bn = gum::BayesNet< double >::fastPrototype("Smoking->Cancer");
      bn.generateCPTs();

      gum::LatentDescriptorVector lat;
      lat.emplace_back(
          "U",
          std::vector< gum::NodeId >{bn.idFromName("Smoking"), bn.idFromName("Cancer")});

      gum::CausalModel< double > cm(bn, lat, /*assumeNonSpurious=*/true);

      gum::CausalImpact< double > ci(cm, names({"Cancer"}), names({"Smoking"}), StrSet{});

      CHECK_THROWS((void)ci.eval());

      gum::CausalImpact< double > ci_dc(cm, names({"Cancer"}), names({"Smoking"}), StrSet{}, true);
      CHECK_THROWS((void)ci_dc.eval());
    }

    // N) Do-calculus chain: W->X->Z->Y and W->Z
    // Effect of X on Y (no observations) is identified by backdoor over W:
    // P(Y | do(X)) = sum_w P(Y | X,w) P(w), with P(Y|X,w) = sum_z P(Y|z) P(z|X,w).
    static void testDoCalculus_WXZYW_BackdoorW() {
      // Repeat with directDoCalculus: force do-calculus path, should match above
      auto bn = gum::BayesNet< double >::fastPrototype("w->x->z->y;w->z");
      bn.generateCPTs();
      gum::CausalModel< double > cm(bn);

      gum::CausalImpact< double > ci(cm, names({"y"}), names({"x"}), StrSet{});

      CHECK_NOTHROW({
        const auto got          = ci.eval();
        const auto p_y_given_z  = bn.cpt(bn.idFromName("y"));
        const auto p_z_given_xw = bn.cpt(bn.idFromName("z"));
        const auto p_w          = bn.cpt(bn.idFromName("w"));
        const auto p_y_given_xw = (p_y_given_z * p_z_given_xw).sumOut(vset(bn, {"z"}));
        const auto exp          = (p_y_given_xw * p_w).sumOut(vset(bn, {"w"}));
        GUM_CHECK_TENSOR_ALMOST_EQUALS(got, exp);
      });

      gum::CausalImpact< double > ci_dc(cm, names({"y"}), names({"x"}), StrSet{}, true);
      CHECK_NOTHROW({
        const auto got          = ci_dc.eval();
        const auto p_y_given_z  = bn.cpt(bn.idFromName("y"));
        const auto p_z_given_xw = bn.cpt(bn.idFromName("z"));
        const auto p_w          = bn.cpt(bn.idFromName("w"));
        const auto p_y_given_xw = (p_y_given_z * p_z_given_xw).sumOut(vset(bn, {"z"}));
        const auto exp          = (p_y_given_xw * p_w).sumOut(vset(bn, {"w"}));
        GUM_CHECK_TENSOR_ALMOST_EQUALS(got, exp);
      });
    }

    // Custom do-operator prefix/suffix test (must be last)
    static void testCustomDoOperatorLatex() {
      auto bn = gum::BayesNet< double >::fastPrototype("X->Y");
      bn.generateCPTs();
      gum::CausalModel< double >  cm(bn);
      gum::CausalImpact< double > ci(cm, names({"Y"}), names({"X"}), StrSet{});
      std::string                 latex = ci.latexQuery("DO<", ">");
      CHECK_NE(latex.find("DO<"), std::string::npos);
      CHECK_NE(latex.find('>'), std::string::npos);
      std::string latex2 = ci.toLatex("DO<", ">");
      CHECK_NE(latex2.find("DO<"), std::string::npos);
      CHECK_NE(latex2.find('>'), std::string::npos);
    }

    // ---------------------------------------------------------------------------
    // Test 1: Compare free function vs class on a deterministic chain X->Y (Y=X)
    // do(X=a) => P(Y=a)=1 and the whole tensor equals the class' evaluation.
    // ---------------------------------------------------------------------------
    static void testcausalImpact_FreeFn_matches_Class_on_deterministic_chain() {
      // BN: X{a|b} -> Y{a|b}
      auto bn = gum::BayesNet< double >::fastPrototype("X{a|b}->Y{a|b}");
      bn.cpt("X").fillWith({0.5, 0.5});   // prior on X

      // Make Y deterministically copy X (no helper; direct CPT fill)
      {
        auto&              cptY = bn.cpt(bn.idFromName("Y"));
        const auto&        Y    = bn.variableFromName("Y");
        const auto&        X    = bn.variableFromName("X");
        gum::Instantiation i(cptY);   // over (Y,X) in CPT order
        for (i.setFirst(); !i.end(); i.inc()) {
          // Current parent assignment:
          const gum::Idx x = i.val(X);
          // One-hot Y on the same index as X
          for (gum::Idx y = 0; y < Y.domainSize(); ++y) {
            i.chgVal(Y, y);
            cptY.set(i, (y == x) ? 1.0 : 0.0);
          }
        }
      }

      gum::CausalModel< double > cm(bn);

      // do(X=a)
      gum::HashTable< std::string, std::string > values;
      values.insert("X", "a");
      // values.insert("Y","a");  // FIXME if a complete instance is given, it would throw.
      const gum::Set< std::string > on{"Y"};
      const gum::Set< std::string > doing{"X"};
      const gum::Set< std::string > know;   // empty

      // Free function
      auto [ff_formula, ff_tensor, ff_expl]
          = gum::causalImpact< double >(cm, on, doing, know, values);


      // Class (for cross-check)
      gum::CausalImpact< double > cls(cm, on, doing, know);
      const auto&                 cls_formula = cls.getResult();
      const auto                  cls_tensor  = cls.eval();   // by value (small)


      // Both should be identifiable -> non-empty tensors
      CHECK_NE(ff_tensor.nbrDim(), gum::Idx(0));
      CHECK_NE(cls_tensor.nbrDim(), gum::Idx(0));

      // creatng partial cls_tensor
      gum::Instantiation J;                    // start empty → partial instantiation
      const auto&        Xv = cls_tensor.variable("X");
      const auto         xa = Xv.index("a");   // label -> index
      J.add(Xv);                               // add just X
      J.chgVal(Xv, xa);                        // set X = "a"
      // auto cls_tensor_partial = cls_tensor.extract(J);      // slice → dims now match free-fn
      // result


      // The numeric results should match
      GUM_CHECK_TENSOR_ALMOST_EQUALS(ff_tensor, cls_tensor.extract(J));

      // Spot check: P(Y=a | do X=a) = 1; P(Y=b | do X=a) = 0
      {
        auto Ia = instFor(ff_tensor, {{"Y", "a"}});
        auto Ib = instFor(ff_tensor, {{"Y", "b"}});
        CHECK((ff_tensor.get(Ia)) == doctest::Approx(1.0).epsilon(1e-12));
        CHECK_LT(std::abs(ff_tensor.get(Ib)), 1e-12);
      }

      // Exercise explanation and result printing (should not throw and not be empty)
      CHECK(!ff_expl.empty());
      CHECK(!ff_formula.toString().empty());
      CHECK(!cls_formula.toString().empty());
    }

    // ---------------------------------------------------------------------------
    // Test 2: No causal effect — independent roots X and Y.
    // do(X) must not change P(Y).
    // ---------------------------------------------------------------------------
    static void testcausalImpact_FreeFn_no_effect_independent_roots() {
      // BN: X{a|b}; Y{u|v} (no edges)
      auto bn = gum::BayesNet< double >::fastPrototype("X{a|b};Y{u|v}");
      bn.cpt("X").fillWith({0.3, 0.7});
      bn.cpt("Y").fillWith({0.2, 0.8});

      gum::CausalModel< double > cm(bn);

      gum::HashTable< std::string, std::string > values;
      values.insert("X", "a");
      const gum::Set< std::string > on    = names({"Y"});
      const gum::Set< std::string > doing = names({"X"});
      const gum::Set< std::string > know;   // empty

      const auto [formula, tensor, expl] = gum::causalImpact< double >(cm, on, doing, know, values);

      // Identified => tensor not empty
      CHECK_NE(tensor.nbrDim(), gum::Idx(0));

      // P(Y=u)=0.2, P(Y=v)=0.8 unchanged by do(X=a)
      auto Iu = instFor(tensor, {{"Y", "u"}});
      auto Iv = instFor(tensor, {{"Y", "v"}});
      CHECK((tensor.get(Iu)) == doctest::Approx(0.2).epsilon(1e-12));
      CHECK((tensor.get(Iv)) == doctest::Approx(0.8).epsilon(1e-12));
    }

    static void testFromNotebook18() {
      auto bn           = gum::BayesNet< double >::fastPrototype("X2->Ca->X1->Y1<-Y2<-X2");
      auto causal_model = gum::CausalModel< double >(bn, {{"L1", bn.ids({"X1", "Y1"})}});
      const auto [ci, tensor, explanation]
          = gum::causalImpact< double >(causal_model,
                                        gum::Set< std::string >{"X1"},
                                        gum::Set< std::string >{"X2"});
      CHECK_EQ(tensor.variable(0).name(), "X1");
      CHECK_EQ(tensor.nbrDim(), 1u);
      CHECK(gum::contains(explanation, "backdoor"));
      CHECK(gum::contains(explanation, "\"X1\""));
    }
  };

  GUM_TEST_ACTIF(NullEffect_DSeparation)
  GUM_TEST_ACTIF(Backdoor_WithObservedConfounder)
  GUM_TEST_ACTIF(Frontdoor_SimpleLatent)
  GUM_TEST_ACTIF(Backdoor_Trivial_XtoY)
  GUM_TEST_ACTIF(WHY19_Hedge_Unidentifiable)
  GUM_TEST_ACTIF(PairwiseOverlapsThrow)
  GUM_TEST_ACTIF(AccentsInVariables)
  GUM_TEST_ACTIF(FromR_NoRaise)
  GUM_TEST_ACTIF(SimpsonParadox_BackdoorAverage)
  GUM_TEST_ACTIF(Tobacco_DirectCausality_TrivialBackdoor)
  GUM_TEST_ACTIF(Tobacco_Frontdoor_WithMediatorTar)
  GUM_TEST_ACTIF(Tobacco_Unidentifiable_Hedge)
  GUM_TEST_ACTIF(DoCalculus_WXZYW_BackdoorW)
  GUM_TEST_ACTIF(CustomDoOperatorLatex)
  GUM_TEST_ACTIF(causalImpact_FreeFn_matches_Class_on_deterministic_chain)
  GUM_TEST_ACTIF(causalImpact_FreeFn_no_effect_independent_roots)
  GUM_TEST_ACTIF(FromNotebook18)

}   // namespace gum_tests
