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

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/agrum.h>
#include <agrum/base/multidim/tensor.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/BIFXML/BIFXMLBNReader.h>
#include <agrum/CM/causalModel.h>
#include <agrum/CM/counterfactual.h>

namespace gum_tests {

class CounterfactualTestSuite : public CxxTest::TestSuite {
  using StrSet = gum::Set<std::string>;

  static StrSet names(std::initializer_list<const char*> il) {
    StrSet s; for (auto* p : il) s.insert(std::string(p)); return s;
  }

  // Build an Instantiation slaved to tensor `t`, and set values by name/label.
  static gum::Instantiation instFor(const gum::Tensor<double>& t,
                                    std::initializer_list<std::pair<const char*, const char*>> assigns) {
    gum::Instantiation I(const_cast<gum::Tensor<double>&>(t)); // ctor takes (MultiDimAdressable&)
    for (const auto& p : assigns) I.chgVal(std::string(p.first), std::string(p.second));
    return I;
  }

  // Load BN from resources/bifxml/edex.bifxml, returning by value (no unique_ptr).
  static gum::BayesNet<double> loadBNFromBIFXML() {
    std::string file = GET_RESSOURCES_PATH("bifxml/edex.bifxml");
    gum::BayesNet<double> net;
    gum::BIFXMLBNReader<double> reader(&net, file);
    TS_GUM_ASSERT_THROWS_NOTHING(reader.proceed())
    return net; // NRVO / move
  }

public:
  // Reproduces the Python test (names-based API) using the BIFXML file.
  GUM_ACTIVE_TEST(test_Counterfactual_FromBIFXML_Names) {
    auto bn = loadBNFromBIFXML();
    // --- BEGIN TRACE BLOCK ---
    {
      std::string s;
      for (auto nid : bn.nodes()) {
        if (!s.empty()) s += ", ";
        s += bn.variable(nid).name();
      }
      GUM_TRACE(std::string("[TEST] BN vars: ") + s);
    }
    // quick sanity:
    TS_ASSERT_THROWS_NOTHING(bn.idFromName("Us"));
    // --- END TRACE BLOCK ---

    // quick sanity: CPTs exist & normalize shouldn't throw if called
    TS_ASSERT_THROWS_NOTHING({ (void)bn.size(); })

    gum::CausalModel<double> cm(bn);
    GUM_TRACE("--2-- Constructed CM from BN");

    gum::HashTable<std::string,std::string> profile;
    profile.insert("experience","8");
    profile.insert("education","low");
    profile.insert("salary","86");

    GUM_TRACE("--3-- Profile set");

    gum::HashTable<std::string,std::string> values;
    values.insert("education","medium");

    GUM_TRACE("--4-- Values set");

    auto on_names = gum::Set<std::string>(); on_names.insert("salary");
    auto whatif_names = gum::Set<std::string>(); whatif_names.insert("education");


    gum::Counterfactual<double> cf(cm, /*on*/on_names,
                                      /*whatif*/whatif_names,
                                      /*profile*/profile,
                                      /*values*/values);

    // exercise print() and getResult().toString() (should not throw)
    TS_ASSERT_THROWS_NOTHING({
      std::ostringstream oss; cf.print(oss); (void)cf.getResult().toString();
    })

    const auto& got = cf.value();
    auto I = instFor(got, { {"salary","81"} });
    TS_ASSERT_DELTA(got.get(I), 1.0, 1e-12);

    auto I0 = instFor(got, { {"salary","80"} });
    TS_ASSERT_LESS_THAN(std::abs(got.get(I0)), 1e-12);
    auto I2 = instFor(got, { {"salary","82"} });
    TS_ASSERT_LESS_THAN(std::abs(got.get(I2)), 1e-12);
  }

  // Same scenario, using the NodeSet / ID-based overloads, from the same BIFXML.
  GUM_ACTIVE_TEST(test_Counterfactual_FromBIFXML_IDs) {
    auto bn = loadBNFromBIFXML();
    gum::CausalModel<double> cm(bn);

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

    TS_ASSERT_THROWS_NOTHING({
      std::ostringstream oss; cf.print(oss); (void)cf.getResult().toString();
    })

    const auto& got = cf.value();
    auto I = instFor(got, { {"salary","81"} });
    TS_ASSERT_DELTA(got.get(I), 1.0, 1e-12);
  }

  // Light smoke: ensures ctor+eval do not throw on empty profile using same file.
  GUM_ACTIVE_TEST(test_Counterfactual_FromBIFXML_Smoke_NoProfile) {
    auto bn = loadBNFromBIFXML();
    gum::CausalModel<double> cm(bn);

    gum::HashTable<std::string,std::string> profile; // empty
    gum::HashTable<std::string,std::string> values;  // do(X=a)
    values.insert("education","low");

    gum::NameSet on_set; on_set.insert("salary");
    gum::NameSet whatif_set; whatif_set.insert("education");

    gum::Counterfactual<double> cf(cm, on_set, whatif_set, profile, values);
    TS_ASSERT_THROWS_NOTHING({ (void)cf.value(); })
  }
};

} // namespace gum_tests
