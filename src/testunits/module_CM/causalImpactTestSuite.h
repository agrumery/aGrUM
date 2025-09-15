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

#include <string>
#include <vector>

#include <agrum/agrum.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/CM/causalModel.h>
#include <agrum/CM/causalImpact.h>
#include <agrum/CM/tools/causalFormula.h>
#include <agrum/base/multidim/tensor.h>


namespace gum_tests {

class CausalImpactTestSuite : public CxxTest::TestSuite {
  using StrSet = gum::Set<std::string>;

  static StrSet names(std::initializer_list<const char*> il) {
    StrSet s; for (auto* p : il) s.insert(std::string(p)); return s;
  }

  static gum::VariableSet vset(const gum::BayesNet<double>& bn,
                               std::initializer_list<const char*> il) {
    gum::VariableSet vs;
    for (auto* p : il) vs.insert(&bn.variableFromName(p)); // expects pointers
    return vs;
  }

  // Build an Instantiation slaved to tensor `t`, and set values using var-name/label pairs.
  // Example use:
  //   auto inst = _inst(got, { {"Patient","Healed"}, {"Drug","With"} });
  //   double val = got.get_(inst);
  static gum::Instantiation _inst(
      const gum::Tensor<double>& t,
      std::initializer_list<std::pair<const char*, const char*>> assigns) {
    gum::Instantiation I(t); // slaved to t; has exactly t's vars in the right order
    for (const auto& p : assigns) I.chgVal(std::string(p.first), std::string(p.second));
    return I;
  }

public:

  // A) Null effect via d-separation: X -> Z -> Y with knowing={Z}
  // Expected: P(Y | do(X), Z) == P(Y | Z) == bn.cpt("Y")
  GUM_ACTIVE_TEST(test_NullEffect_DSeparation) {
    auto bn = gum::BayesNet<double>::fastPrototype("X->Z->Y");
    bn.generateCPTs();
    gum::CausalModel<double> cm(bn);

    gum::CausalImpact<double> ci(
      cm, names({"Y"}), names({"X"}), names({"Z"}),
      gum::HashTable<std::string,std::string>());

    TS_ASSERT_THROWS_NOTHING({
      auto got = ci.eval();
      auto exp = bn.cpt("Y");
      TS_GUM_TENSOR_ALMOST_EQUALS(got, exp);
    });

    // Repeat with directDoCalculus: force do-calculus path, should match above
    gum::CausalImpact<double> ci_dc(
      cm, names({"Y"}), names({"X"}), names({"Z"}),
      gum::HashTable<std::string,std::string>(), true);
    TS_ASSERT_THROWS_NOTHING({
      auto got = ci_dc.eval();
      auto exp = bn.cpt("Y");
      TS_GUM_TENSOR_ALMOST_EQUALS(got, exp);
    });
  }

  // B) Backdoor with observed confounder:
  // BN: Gender -> Drug -> Patient ; Gender -> Patient
  // Expected: sum_G P(Patient | Drug, G) P(G) == (cpt(Patient) * cpt(Gender)).sumOut(Gender)
  GUM_ACTIVE_TEST(test_Backdoor_WithObservedConfounder) {
  // Repeat with directDoCalculus: force do-calculus path, should match above
    auto bn = gum::BayesNet<double>::fastPrototype(
      "Gender{M|F}->Drug{No|Yes}->Patient{No|Yes};Gender{M|F}->Patient{No|Yes}");
    bn.generateCPTs();
    gum::CausalModel<double> cm(bn);

    gum::CausalImpact<double> ci(
      cm, names({"Patient"}), names({"Drug"}), StrSet{},
      gum::HashTable<std::string,std::string>());

    TS_ASSERT_THROWS_NOTHING({
      auto got = ci.eval();
      auto exp = (bn.cpt("Patient") * bn.cpt("Gender")).sumOut(vset(bn, {"Gender"}));
      TS_GUM_TENSOR_ALMOST_EQUALS(got, exp);
    });

    gum::CausalImpact<double> ci_dc(
      cm, names({"Patient"}), names({"Drug"}), StrSet{},
      gum::HashTable<std::string,std::string>(), true);
    TS_ASSERT_THROWS_NOTHING({
      auto got = ci_dc.eval();
      auto exp = (bn.cpt("Patient") * bn.cpt("Gender")).sumOut(vset(bn, {"Gender"}));
      TS_GUM_TENSOR_ALMOST_EQUALS(got, exp);
    });
  }

  // C) Frontdoor (canonical): X->Z->Y, with latent U between X and Y
  // Expected: sum_z P(z|x) P(y|z) == (cpt("Z") * cpt("Y")).sumOut(Z)
  GUM_ACTIVE_TEST(test_Frontdoor_SimpleLatent) {
  // Repeat with directDoCalculus: force do-calculus path, should match above
    auto bn = gum::BayesNet<double>::fastPrototype("X->Z->Y");
    bn.generateCPTs();

    // Latent descriptor must use NodeIds
    gum::LatentDescriptorVector lat;
    lat.emplace_back("U", std::vector<gum::NodeId>{bn.idFromName("X"), bn.idFromName("Y")});

    gum::CausalModel<double> cm(bn, lat, /*keepArcs=*/false);

    gum::CausalImpact<double> ci(
      cm, names({"Y"}), names({"X"}), StrSet{},
      gum::HashTable<std::string,std::string>());

    TS_ASSERT_THROWS_NOTHING({
      auto got = ci.eval();
      auto exp = (bn.cpt("Z") * bn.cpt("Y")).sumOut(vset(bn, {"Z"}));
      TS_GUM_TENSOR_ALMOST_EQUALS(got, exp);
    });

    gum::CausalImpact<double> ci_dc(
      cm, names({"Y"}), names({"X"}), StrSet{},
      gum::HashTable<std::string,std::string>(), true);
    TS_ASSERT_THROWS_NOTHING({
      auto got = ci_dc.eval();
      auto exp = (bn.cpt("Z") * bn.cpt("Y")).sumOut(vset(bn, {"Z"}));
      TS_GUM_TENSOR_ALMOST_EQUALS(got, exp);
    });
  }

  // D) “Trivial” backdoor: X -> Y  (backdoor set is ∅)
  // Expected: P(Y|do(X)) == P(Y|X) == cpt("Y")
  GUM_ACTIVE_TEST(test_Backdoor_Trivial_XtoY) {
  // Repeat with directDoCalculus: force do-calculus path, should match above
    auto bn = gum::BayesNet<double>::fastPrototype("X->Y");
    bn.generateCPTs();
    gum::CausalModel<double> cm(bn);

    gum::CausalImpact<double> ci(
      cm, names({"Y"}), names({"X"}), StrSet{},
      gum::HashTable<std::string,std::string>());

    TS_ASSERT_THROWS_NOTHING({
      auto got = ci.eval();
      auto exp = bn.cpt("Y");
      TS_GUM_TENSOR_ALMOST_EQUALS(got, exp);
    });

    gum::CausalImpact<double> ci_dc(
      cm, names({"Y"}), names({"X"}), StrSet{},
      gum::HashTable<std::string,std::string>(), true);
    TS_ASSERT_THROWS_NOTHING({
      auto got = ci_dc.eval();
      auto exp = bn.cpt("Y");
      TS_GUM_TENSOR_ALMOST_EQUALS(got, exp);
    });

  }

  // E) Hedge / unidentifiable:
  // BN: X->Y; U ; latent Z -> {X,Y,U}, keepArcs=true
  // Expected: evaluation throws (null AST / hedge)
  GUM_ACTIVE_TEST(test_WHY19_Hedge_Unidentifiable) {
  // Repeat with directDoCalculus: force do-calculus path, should throw as above
    auto bn = gum::BayesNet<double>::fastPrototype("X->Y;U");
    bn.generateCPTs();

    gum::LatentDescriptorVector lat;
    lat.emplace_back("Z", std::vector<gum::NodeId>{
      bn.idFromName("X"), bn.idFromName("Y"), bn.idFromName("U")});

    gum::CausalModel<double> cm(bn, lat, /*keepArcs=*/true);

    gum::CausalImpact<double> ci(
      cm, names({"Y"}), names({"X"}), StrSet{},
      gum::HashTable<std::string,std::string>());

    TS_ASSERT_THROWS_ANYTHING({
      (void)ci.eval();
    });

    gum::CausalImpact<double> ci_dc(
      cm, names({"Y"}), names({"X"}), StrSet{},
      gum::HashTable<std::string,std::string>(), true);
    TS_ASSERT_THROWS_ANYTHING({
      (void)ci_dc.eval();
    });
  }

  // F) Pairwise overlaps among on/doing/knowing must throw at construction
  GUM_ACTIVE_TEST(test_PairwiseOverlapsThrow) {
  // Repeat with directDoCalculus: force do-calculus path, should throw as above
    auto bn = gum::BayesNet<double>::fastPrototype("A->B");
    bn.generateCPTs();
    gum::CausalModel<double> cm(bn);

    TS_ASSERT_THROWS_ANYTHING(
      (gum::CausalImpact<double>(cm, names({"B"}), names({"B"}), StrSet{},
                                 gum::HashTable<std::string,std::string>())));
    TS_ASSERT_THROWS_ANYTHING(
      (gum::CausalImpact<double>(cm, names({"B"}), names({"A"}), names({"B"}),
                                 gum::HashTable<std::string,std::string>())));
    TS_ASSERT_THROWS_ANYTHING(
      (gum::CausalImpact<double>(cm, names({"B"}), names({"A"}), names({"A"}),
                                 gum::HashTable<std::string,std::string>())));

    // Repeat with directDoCalculus
    TS_ASSERT_THROWS_ANYTHING(
      (gum::CausalImpact<double>(cm, names({"B"}), names({"B"}), StrSet{},
                                 gum::HashTable<std::string,std::string>(), true)));
    TS_ASSERT_THROWS_ANYTHING(
      (gum::CausalImpact<double>(cm, names({"B"}), names({"A"}), names({"B"}),
                                 gum::HashTable<std::string,std::string>(), true)));
    TS_ASSERT_THROWS_ANYTHING(
      (gum::CausalImpact<double>(cm, names({"B"}), names({"A"}), names({"A"}),
                                 gum::HashTable<std::string,std::string>(), true)));
  }

  // G) Accented variable names are fine
  GUM_ACTIVE_TEST(test_AccentsInVariables) {
  // Repeat with directDoCalculus: force do-calculus path, should match above
    auto bn = gum::BayesNet<double>::fastPrototype("héhé->hoho");
    bn.generateCPTs();
    gum::CausalModel<double> cm(bn);

    gum::CausalImpact<double> ci(
      cm, names({"hoho"}), names({"héhé"}), StrSet{},
      gum::HashTable<std::string,std::string>());

    TS_ASSERT_THROWS_NOTHING({ (void)ci.eval(); });

    gum::CausalImpact<double> ci_dc(
      cm, names({"hoho"}), names({"héhé"}), StrSet{},
      gum::HashTable<std::string,std::string>(), true);
    TS_ASSERT_THROWS_NOTHING({ (void)ci_dc.eval(); });
  }

  // H) Complex “From R”-style model: should not raise during eval
  // TODO :Make active test when ID algorithm is implemented
  GUM_ACTIVE_TEST(test_FromR_NoRaise) {
  // Repeat with directDoCalculus: force do-calculus path, should not throw
    auto m = gum::BayesNet<double>::fastPrototype("z2->x->z1->y;z2->z1;z2->z3->y");
    m.generateCPTs();

    gum::CausalModel<double> cm(
      m,
      {{"X-Z2", {m.idFromName("x"), m.idFromName("z2")}},
       {"X-Z3", {m.idFromName("x"), m.idFromName("z3")}},
       {"X-Y",  {m.idFromName("x"), m.idFromName("y")}},
       {"Y-Z2", {m.idFromName("y"), m.idFromName("z2")}}},
      /*keepArcs=*/true);

    TS_ASSERT_THROWS_NOTHING({
      gum::CausalImpact<double> ci(
        cm, names({"y","z2","z1","z3"}), names({"x"}), StrSet{},
        gum::HashTable<std::string,std::string>());
    });
    TS_ASSERT_THROWS_NOTHING({
      gum::CausalImpact<double> ci_dc(
        cm, names({"y","z2","z1","z3"}), names({"x"}), StrSet{},
        gum::HashTable<std::string,std::string>(), true);
    });
  }
  // I) Simpson's paradox: explicit CPTs, check interventional via backdoor
  // BN: Gender -> Drug -> Patient ; Gender -> Patient
  // With fixed CPTs (the classic Simpson example), we should have:
  // P(Healed | do(Drug=With)) = 0.45 and P(Healed | do(Drug=Without)) = 0.60
  // but observationally P(Healed | Drug=With) > P(Healed | Drug=Without).
  GUM_ACTIVE_TEST(test_SimpsonParadox_BackdoorAverage) {
  // Repeat with directDoCalculus: force do-calculus path, should match above
    auto bn = gum::BayesNet<double>::fastPrototype(
      "Gender{F|M}->Drug{Without|With}->Patient{Sick|Healed}<-Gender");

    // P(Gender)
    bn.cpt(bn.idFromName("Gender")).fillWith({0.5, 0.5}); // F, M

    // P(Drug | Gender)
    // Parent order is [Gender], child states {Without, With}
    // Gender=F: 0.25,0.75 ; Gender=M: 0.75,0.25
    bn.cpt(bn.idFromName("Drug")).fillWith({
      0.25, 0.75, // Gender=F
      0.75, 0.25  // Gender=M
    });

    // P(Patient | Drug, Gender)
    // Parent order is [Drug, Gender]; child states {Sick, Healed}
    // F: Without->0.4 Healed, With->0.2 Healed
    // M: Without->0.8 Healed, With->0.7 Healed
    bn.cpt(bn.idFromName("Patient")).fillWith({
      // Gender=F
      0.6, 0.4,  // Drug=Without , Gender=F
      0.8, 0.2,  // Drug=With    , Gender=F
      // Gender=M
      0.2, 0.8,  // Drug=Without , Gender=M
      0.3, 0.7   // Drug=With    , Gender=M
    });

    gum::CausalModel<double> cm(bn);

    gum::CausalImpact<double> ci(
      cm, names({"Patient"}), names({"Drug"}), StrSet{},
      gum::HashTable<std::string,std::string>());

    auto got = ci.eval();
    auto exp = (bn.cpt(bn.idFromName("Patient")) * bn.cpt(bn.idFromName("Gender")))
                 .sumOut(vset(bn, {"Gender"}));
    TS_GUM_TENSOR_ALMOST_EQUALS(got, exp);

    TS_ASSERT_DELTA(
        got.get(_inst(got, {{"Patient","Healed"}, {"Drug","With"}})),
        0.45, 1e-10);

    TS_ASSERT_DELTA(
        got.get(_inst(got, {{"Patient","Healed"}, {"Drug","Without"}})),
        0.60, 1e-10);

    gum::CausalImpact<double> ci_dc(
      cm, names({"Patient"}), names({"Drug"}), StrSet{},
      gum::HashTable<std::string,std::string>(), true);
    auto got_dc = ci_dc.eval();
    TS_GUM_TENSOR_ALMOST_EQUALS(got_dc, exp);
    TS_ASSERT_DELTA(
        got_dc.get(_inst(got_dc, {{"Patient","Healed"}, {"Drug","With"}})),
        0.45, 1e-10);
    TS_ASSERT_DELTA(
        got_dc.get(_inst(got_dc, {{"Patient","Healed"}, {"Drug","Without"}})),
        0.60, 1e-10);

  }

  // J) Tobacco #1: Direct causality Smoking -> Cancer (trivial backdoor ∅)
  // Expected: P(Cancer | do(Smoking)) == P(Cancer | Smoking) == CPT(Cancer)
  GUM_ACTIVE_TEST(test_Tobacco_DirectCausality_TrivialBackdoor) {
  // Repeat with directDoCalculus: force do-calculus path, should match above
    auto bn = gum::BayesNet<double>::fastPrototype("Smoking->Cancer");
    bn.generateCPTs();
    gum::CausalModel<double> cm(bn);

    gum::CausalImpact<double> ci(
      cm, names({"Cancer"}), names({"Smoking"}), StrSet{},
      gum::HashTable<std::string,std::string>());

    TS_ASSERT_THROWS_NOTHING({
      auto got = ci.eval();
      auto exp = bn.cpt(bn.idFromName("Cancer"));
      TS_GUM_TENSOR_ALMOST_EQUALS(got, exp);
    });

    gum::CausalImpact<double> ci_dc(
      cm, names({"Cancer"}), names({"Smoking"}), StrSet{},
      gum::HashTable<std::string,std::string>(), true);
    TS_ASSERT_THROWS_NOTHING({
      auto got = ci_dc.eval();
      auto exp = bn.cpt(bn.idFromName("Cancer"));
      TS_GUM_TENSOR_ALMOST_EQUALS(got, exp);
    });
  }

  // L) Tobacco #3: Frontdoor with mediator Tar and latent U between Smoking,Cancer
  // Graph: Smoking -> Tar -> Cancer, U -> {Smoking, Cancer} (latent)
  // Expected (frontdoor): P(Cancer | do(Smoking)) = sum_z P(z|Smoking) P(Cancer|z)
  GUM_ACTIVE_TEST(test_Tobacco_Frontdoor_WithMediatorTar) {
  // Repeat with directDoCalculus: force do-calculus path, should match above
    auto bn = gum::BayesNet<double>::fastPrototype("Smoking->Tar->Cancer");
    bn.generateCPTs();

    gum::LatentDescriptorVector lat;
    lat.emplace_back("U",
                     std::vector<gum::NodeId>{bn.idFromName("Smoking"),
                                              bn.idFromName("Cancer")});

    gum::CausalModel<double> cm(bn, lat, /*keepArcs=*/false);

    gum::CausalImpact<double> ci(
      cm, names({"Cancer"}), names({"Smoking"}), StrSet{},
      gum::HashTable<std::string,std::string>());

    TS_ASSERT_THROWS_NOTHING({
      auto got = ci.eval();
      auto exp = (bn.cpt(bn.idFromName("Tar")) * bn.cpt(bn.idFromName("Cancer")))
                   .sumOut(vset(bn, {"Tar"}));
      TS_GUM_TENSOR_ALMOST_EQUALS(got, exp);
    });

    gum::CausalImpact<double> ci_dc(
      cm, names({"Cancer"}), names({"Smoking"}), StrSet{},
      gum::HashTable<std::string,std::string>(), true);
    TS_ASSERT_THROWS_NOTHING({
      auto got = ci_dc.eval();
      auto exp = (bn.cpt(bn.idFromName("Tar")) * bn.cpt(bn.idFromName("Cancer")))
                   .sumOut(vset(bn, {"Tar"}));
      TS_GUM_TENSOR_ALMOST_EQUALS(got, exp);
    });
  }

  // M) Tobacco #4: Unidentifiable hedge (latent confounder + direct edge kept)
  // Graph: Smoking -> Cancer and latent U -> {Smoking, Cancer}, keepArcs=true
  // Expected: Identification fails => eval() throws
  GUM_ACTIVE_TEST(test_Tobacco_Unidentifiable_Hedge) {
  // Repeat with directDoCalculus: force do-calculus path, should throw as above
    auto bn = gum::BayesNet<double>::fastPrototype("Smoking->Cancer");
    bn.generateCPTs();

    gum::LatentDescriptorVector lat;
    lat.emplace_back("U",
                     std::vector<gum::NodeId>{bn.idFromName("Smoking"),
                                              bn.idFromName("Cancer")});

    gum::CausalModel<double> cm(bn, lat, /*keepArcs=*/true);

    gum::CausalImpact<double> ci(
      cm, names({"Cancer"}), names({"Smoking"}), StrSet{},
      gum::HashTable<std::string,std::string>());

    TS_ASSERT_THROWS_ANYTHING({ (void)ci.eval(); });

    gum::CausalImpact<double> ci_dc(
      cm, names({"Cancer"}), names({"Smoking"}), StrSet{},
      gum::HashTable<std::string,std::string>(), true);
    TS_ASSERT_THROWS_ANYTHING({ (void)ci_dc.eval(); });
  }

  // N) Do-calculus chain: W->X->Z->Y and W->Z
  // Effect of X on Y (no observations) is identified by backdoor over W:
  // P(Y | do(X)) = sum_w P(Y | X,w) P(w), with P(Y|X,w) = sum_z P(Y|z) P(z|X,w).
  GUM_ACTIVE_TEST(test_DoCalculus_WXZYW_BackdoorW) {
  // Repeat with directDoCalculus: force do-calculus path, should match above
    auto bn = gum::BayesNet<double>::fastPrototype("w->x->z->y;w->z");
    bn.generateCPTs();
    gum::CausalModel<double> cm(bn);

    gum::CausalImpact<double> ci(
      cm, names({"y"}), names({"x"}), StrSet{},
      gum::HashTable<std::string,std::string>());

    TS_ASSERT_THROWS_NOTHING({
      auto got = ci.eval();
      auto p_y_given_z  = bn.cpt(bn.idFromName("y"));
      auto p_z_given_xw = bn.cpt(bn.idFromName("z"));
      auto p_w          = bn.cpt(bn.idFromName("w"));
      auto p_y_given_xw = (p_y_given_z * p_z_given_xw).sumOut(vset(bn, {"z"}));
      auto exp          = (p_y_given_xw * p_w).sumOut(vset(bn, {"w"}));
      TS_GUM_TENSOR_ALMOST_EQUALS(got, exp);
    });

    gum::CausalImpact<double> ci_dc(
      cm, names({"y"}), names({"x"}), StrSet{},
      gum::HashTable<std::string,std::string>(), true);
    TS_ASSERT_THROWS_NOTHING({
      auto got = ci_dc.eval();
      auto p_y_given_z  = bn.cpt(bn.idFromName("y"));
      auto p_z_given_xw = bn.cpt(bn.idFromName("z"));
      auto p_w          = bn.cpt(bn.idFromName("w"));
      auto p_y_given_xw = (p_y_given_z * p_z_given_xw).sumOut(vset(bn, {"z"}));
      auto exp          = (p_y_given_xw * p_w).sumOut(vset(bn, {"w"}));
      TS_GUM_TENSOR_ALMOST_EQUALS(got, exp);
    });
  }





};

} // namespace gum_tests