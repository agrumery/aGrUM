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
#include <sstream>
#include <cmath>

#include <agrum/agrum.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/CM/causalModel.h>
#include <agrum/CM/counterfactual.h>
#include <agrum/CM/causalImpact.h>
#include <agrum/CM/tools/causalFormula.h>
#include <agrum/base/multidim/tensor.h>

namespace gum_tests {

class CounterfactualTestSuite : public CxxTest::TestSuite {
  using StrSet = gum::Set<std::string>;

  static StrSet names(std::initializer_list<const char*> il) {
    StrSet s; for (auto* p : il) s.insert(std::string(p)); return s;
  }

  // Build an Instantiation slaved to tensor `t`, and set values by name/label.
  static gum::Instantiation instFor(const gum::Tensor<double>& t,
                                    std::initializer_list<std::pair<const char*, const char*>> assigns) {
    gum::Instantiation I(const_cast<gum::Tensor<double>&>(t)); // ok: ctor takes (MultiDimAdressable&)
    for (const auto& p : assigns) I.chgVal(std::string(p.first), std::string(p.second));
    return I;
  }

  // Deterministic CPT filler: set P(child = targetLabel(parents) | parents) = 1
  template <class TargetFn>
  static void fillDetCPT(gum::BayesNet<double>& bn,
                         const std::string& childName,
                         const std::vector<std::string>& parentNames,
                         TargetFn targetLabel) {
    auto& cpt = bn.cpt(bn.idFromName(childName));
    const auto& child = bn.variableFromName(childName);

    gum::Instantiation i(cpt); // over child + parents in CPT order
    for (i.setFirst(); !i.end(); i.inc()) {
      // parent labels from current instantiation line
      std::vector<std::string> parLabels;
      parLabels.reserve(parentNames.size());
      for (const auto& pname : parentNames) {
        const auto& pv = i.variable(pname);
        parLabels.emplace_back(pv.label(i.val(pv)));
      }

      // compute the unique child label for this parent config
      const std::string wanted_label = targetLabel(parLabels);

      // one-hot the whole line
      for (gum::Idx childId = 0; childId < child.domainSize(); ++childId) {
        i.chgVal(child, childId);
        cpt.set(i, 0.0);
      }
      const gum::Idx wanted = child.index(wanted_label);
      i.chgVal(child, wanted);
      cpt.set(i, 1.0);
    }
  }

public:
  // Reproduces the Python test you gave (names-based API).
  GUM_INACTIVE_TEST(test_Counterfactual_ReproducePython_Names) {
    // DAG via fastPrototype
    auto bn = gum::BayesNet<double>::fastPrototype(
      "Ux[-2,10]->experience[0,20]<-education{low|medium|high}->salary[65,150]<-Us[0,25];experience->salary"
    );

    // Priors
    bn.cpt("Us").fillWith(1).normalize();
    bn.cpt("Ux").fillWith(1).normalize();
    bn.cpt("education").fillWith({0.4, 0.4, 0.2}); // low, medium, high

    // experience = 10 - 4*education + Ux (clamp [0,20])
    fillDetCPT(bn, "experience", {"education","Ux"},
      [&](const std::vector<std::string>& pars)->std::string {
        const std::string& eduLabel = pars[0];
        const std::string& uxLabel  = pars[1];
        int edu = (eduLabel=="low")?0: (eduLabel=="medium")?1:2;
        int ux  = std::stoi(uxLabel);
        int val = 10 - 4*edu + ux;
        if (val < 0) val = 0; if (val > 20) val = 20;
        return std::to_string(val);
      });

    // salary = round(65 + 2.5*experience + 5*education + Us) (clamp [65,150])
    fillDetCPT(bn, "salary", {"education","experience","Us"},
      [&](const std::vector<std::string>& pars)->std::string {
        const std::string& eduLabel = pars[0];
        const std::string& exLabel  = pars[1];
        const std::string& usLabel  = pars[2];
        int edu = (eduLabel=="low")?0: (eduLabel=="medium")?1:2;
        int ex  = std::stoi(exLabel);
        int us  = std::stoi(usLabel);
        double s = 65.0 + 2.5*ex + 5.0*edu + us;
        int sval = static_cast<int>(std::llround(s));
        if (sval < 65) sval = 65; if (sval > 150) sval = 150;
        return std::to_string(sval);
      });

    gum::CausalModel<double> cm(bn);

    gum::HashTable<std::string,std::string> profile;
    profile.insert("experience","8");
    profile.insert("education","low");
    profile.insert("salary","86");

    gum::HashTable<std::string,std::string> values;
    values.insert("education","medium");

    gum::Counterfactual<double> cf(cm, /*on*/names({"salary"}),
                                      /*whatif*/names({"education"}),
                                      /*profile*/profile,
                                      /*values*/values);

    // exercise print() and getResult().toString() (should not throw)
    TS_ASSERT_THROWS_NOTHING({
      std::ostringstream oss;
      cf.print(oss);
      (void)cf.getResult().toString();
    });

    // Evaluate & assert: mass 1 at salary=81
    const auto& got = cf.value();
    auto I = instFor(got, { {"salary","81"} });
    double p81 = got.get(I);
    TS_ASSERT_DELTA(p81, 1.0, 1e-12);

    // a couple of neighbors should be ~0
    auto I0 = instFor(got, { {"salary","80"} });
    TS_ASSERT_LESS_THAN(std::abs(got.get(I0)), 1e-12);
    auto I2 = instFor(got, { {"salary","82"} });
    TS_ASSERT_LESS_THAN(std::abs(got.get(I2)), 1e-12);
  }

  // Same scenario, but using the NodeSet / ID-based overloads
  GUM_INACTIVE_TEST(test_Counterfactual_ID_Overload) {
    auto bn = gum::BayesNet<double>::fastPrototype(
      "Ux[-2,10]->experience[0,20]<-education{low|medium|high}->salary[65,150]<-Us[0,25];experience->salary"
    );

    bn.cpt("Us").fillWith(1).normalize();
    bn.cpt("Ux").fillWith(1).normalize();
    bn.cpt("education").fillWith({0.4, 0.4, 0.2});

    fillDetCPT(bn, "experience", {"education","Ux"},
      [&](const std::vector<std::string>& pars)->std::string {
        int edu = (pars[0]=="low")?0: (pars[0]=="medium")?1:2;
        int ux  = std::stoi(pars[1]);
        int val = 10 - 4*edu + ux;
        if (val < 0) val = 0; if (val > 20) val = 20;
        return std::to_string(val);
      });

    fillDetCPT(bn, "salary", {"education","experience","Us"},
      [&](const std::vector<std::string>& pars)->std::string {
        int edu = (pars[0]=="low")?0: (pars[0]=="medium")?1:2;
        int ex  = std::stoi(pars[1]);
        int us  = std::stoi(pars[2]);
        int sval = static_cast<int>(std::llround(65.0 + 2.5*ex + 5.0*edu + us));
        if (sval < 65) sval = 65; if (sval > 150) sval = 150;
        return std::to_string(sval);
      });

    gum::CausalModel<double> cm(bn);

    // IDs & value IDs
    gum::NodeId idEdu = bn.idFromName("education");
    gum::NodeId idExp = bn.idFromName("experience");
    gum::NodeId idSal = bn.idFromName("salary");

    const auto& vEdu = bn.variable(idEdu);
    const auto& vExp = bn.variable(idExp);
    const auto& vSal = bn.variable(idSal);

    gum::HashTable<gum::NodeId, gum::VariableValueId> profileIds;
    profileIds.insert(idExp, vExp.index("8"));
    profileIds.insert(idEdu, vEdu.index("low"));
    profileIds.insert(idSal, vSal.index("86"));

    gum::HashTable<gum::NodeId, gum::VariableValueId> valuesIds;
    valuesIds.insert(idEdu, vEdu.index("medium"));

    gum::NodeSet onIds;     onIds.insert(idSal);
    gum::NodeSet whatIfIds; whatIfIds.insert(idEdu);

    gum::Counterfactual<double> cf(cm, onIds, whatIfIds, profileIds, valuesIds);

    // print & result access should not throw
    TS_ASSERT_THROWS_NOTHING({
      std::ostringstream oss;
      cf.print(oss);
      (void)cf.getResult().toString();
    });

    const auto& got = cf.value();
    auto I = instFor(got, { {"salary","81"} });
    TS_ASSERT_DELTA(got.get(I), 1.0, 1e-12);
  }

  // Light smoke: empty profile & simple BN; ensures ctor+eval do not throw.
  GUM_INACTIVE_TEST(test_Counterfactual_Smoke_NoProfile) {
    auto bn = gum::BayesNet<double>::fastPrototype("X{a|b}->Y{u|v}");
    bn.generateCPTs();
    gum::CausalModel<double> cm(bn);

    gum::HashTable<std::string,std::string> profile; // empty
    gum::HashTable<std::string,std::string> values;  // do(X=a)
    values.insert("X","a");

    auto Y_set = gum::NameSet();
    Y_set.insert("Y");

    auto X_set = gum::NameSet();
    X_set.insert("X");

    gum::Counterfactual<double> cf(cm, Y_set, X_set, profile, values);

  }
};

} // namespace gum_tests
