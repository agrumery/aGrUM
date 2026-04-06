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


#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/agrum.h>

#include <agrum/base/multidim/tensor.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/BIFXML/BIFXMLBNReader.h>
#include <agrum/CM/causalModel.h>
#include <agrum/CM/counterfactual.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  Counterfactual
#define GUM_CURRENT_MODULE CM

namespace gum_tests {

  struct CounterfactualTestSuite {
    using StrSet = gum::Set< std::string >;

    static StrSet names(std::initializer_list< const char* > il) {
      StrSet s;
      for (auto* p: il) {
        s.insert(std::string(p));
      }
      return s;
    }

    // Build an Instantiation slaved to tensor `t`, and set values by name/label.
    static gum::Instantiation
        instFor(const gum::Tensor< double >&                                         t,
                const std::initializer_list< std::pair< const char*, const char* > > assigns) {
      gum::Instantiation I(
          const_cast< gum::Tensor< double >& >(t));   // ctor takes (MultiDimAdressable&)
      for (const auto& p: assigns) {
        I.chgVal(std::string(p.first), std::string(p.second));
      }
      return I;
    }

    // Load BN from resources/bifxml/edex.bifxml, returning by value (no unique_ptr).
    static gum::BayesNet< double > loadBNFromBIFXML() {
      std::string                   file = GET_RESSOURCES_PATH("bifxml/edex.bifxml");
      gum::BayesNet< double >       net;
      gum::BIFXMLBNReader< double > reader(&net, file);
      GUM_CHECK_ASSERT_THROWS_NOTHING(reader.proceed());
      return net;   // NRVO / move
    }

    public:
    // Reproduces the Python test (names-based API) using the BIFXML file.
    static void testCounterfactual_FromBIFXML_Names() {
      auto bn = loadBNFromBIFXML();
      {
        std::string s;
        for (auto nid: bn.nodes()) {
          if (!s.empty()) { s += ", "; }
          s += bn.variable(nid).name();
        }
      }
      // quick sanity:
      CHECK_NOTHROW(bn.idFromName("Us"));

      // quick sanity: CPTs exist & normalize shouldn't throw if called
      CHECK_NOTHROW({ (void)bn.size(); });

      gum::CausalModel< double > cm(bn);

      gum::HashTable< std::string, std::string > profile;
      profile.insert("experience", "8");
      profile.insert("education", "low");
      profile.insert("salary", "86");

      gum::HashTable< std::string, std::string > values;
      values.insert("education", "medium");

      auto on_names = gum::Set< std::string >();
      on_names.insert("salary");
      auto whatif_names = gum::Set< std::string >();
      whatif_names.insert("education");


      gum::Counterfactual cf(cm, on_names, whatif_names, profile, values);
      CHECK_NOTHROW({
        const auto _ = cf.toString();
        (void)cf.impact().toString();
      });

      const auto& got = cf.value();
      auto        I   = instFor(got, {{"salary", "81"}});
      CHECK_EQ((got.get(I)), doctest::Approx(1.0).epsilon(1e-12));

      auto I0 = instFor(got, {{"salary", "80"}});
      CHECK_LT(std::abs(got.get(I0)), 1e-12);
      auto I2 = instFor(got, {{"salary", "82"}});
      CHECK_LT(std::abs(got.get(I2)), 1e-12);
    }

    // Same scenario, using the NodeSet / ID-based overloads, from the same BIFXML.
    static void testCounterfactual_FromBIFXML_IDs() {
      auto                       bn = loadBNFromBIFXML();
      gum::CausalModel< double > cm(bn);

      gum::NodeId idEdu = bn.idFromName("education");
      gum::NodeId idExp = bn.idFromName("experience");
      gum::NodeId idSal = bn.idFromName("salary");

      const auto& vEdu = bn.variable(idEdu);
      const auto& vExp = bn.variable(idExp);
      const auto& vSal = bn.variable(idSal);

      gum::HashTable< gum::NodeId, gum::Idx > profileIds;
      profileIds.insert(idExp, vExp.index("8"));
      profileIds.insert(idEdu, vEdu.index("low"));
      profileIds.insert(idSal, vSal.index("86"));

      gum::HashTable< gum::NodeId, gum::Idx > valuesIds;
      valuesIds.insert(idEdu, vEdu.index("medium"));

      gum::NodeSet onIds;
      onIds.insert(idSal);
      gum::NodeSet whatIfIds;
      whatIfIds.insert(idEdu);

      gum::Counterfactual cf(cm, onIds, whatIfIds, profileIds, valuesIds);

      CHECK_NOTHROW({
        const auto _ = cf.toString();
        (void)cf.impact().toString();
      });

      const auto& got = cf.value();
      auto        I   = instFor(got, {{"salary", "81"}});
      CHECK_EQ((got.get(I)), doctest::Approx(1.0).epsilon(1e-12));
    }

    // Light smoke: ensures ctor+eval do not throw on empty profile using same file.
    static void testCounterfactual_FromBIFXML_Smoke_NoProfile() {
      const auto             bn = loadBNFromBIFXML();
      const gum::CausalModel cm(bn);

      gum::HashTable< std::string, std::string > profile;   // empty
      gum::HashTable< std::string, std::string > values;    // do(X=a)
      values.insert("education", "low");

      gum::Set< std::string > on_set;
      on_set.insert("salary");
      gum::Set< std::string > whatif_set;
      whatif_set.insert("education");

      const gum::Counterfactual cf(cm, on_set, whatif_set, profile);
      CHECK_NOTHROW({ (void)cf.value(); });
    }

    // -------------------------------------------------------------------------
    // Free-function parity with Python API
    // -------------------------------------------------------------------------
    static void testFreeCounterfactual_FromBIFXML_Names() {
      auto                       bn = loadBNFromBIFXML();
      gum::CausalModel< double > cm(bn);

      gum::HashTable< std::string, std::string > profile;
      profile.insert("experience", "8");
      profile.insert("education", "low");
      profile.insert("salary", "86");

      gum::HashTable< std::string, std::string > values;
      values.insert("education", "medium");

      gum::Set< std::string > on_names;
      on_names.insert("salary");
      gum::Set< std::string > whatif;
      whatif.insert("education");

      // Free function
      auto got_free = gum::counterfactual< double >(cm, on_names, whatif, profile, values);

      // Class parity
      gum::Counterfactual< double > cls(cm, on_names, whatif, profile, values);
      const auto&                   got_cls = cls.value();

      // Numerically identical
      GUM_CHECK_TENSOR_ALMOST_EQUALS(got_free, got_cls);

      // Spot check: mass at salary=81 is 1
      auto I = instFor(got_free, {{"salary", "81"}});
      CHECK_EQ(got_free.get(I), doctest::Approx(1.0).epsilon(1e-12));
    }

    static void testFreeCounterfactualModel_roundtrip() {
      auto                       bn = loadBNFromBIFXML();
      gum::CausalModel< double > cm(bn);

      gum::HashTable< std::string, std::string > profile;
      profile.insert("experience", "8");
      profile.insert("education", "low");
      profile.insert("salary", "86");

      gum::HashTable< std::string, std::string > values;
      values.insert("education", "medium");

      gum::Set< std::string > on_names;
      on_names.insert("salary");
      gum::Set< std::string > whatif;
      whatif.insert("education");

      // Build twin with the free function
      auto twincm = gum::counterfactualModel< double >(cm, profile, whatif);

      // Compute effect on the twin with existing causalImpact free fn
      auto [_, adj, __] = gum::causalImpact< double >(twincm,
                                                      on_names,
                                                      whatif,
                                                      gum::Set< std::string >{},
                                                      values);

      // Adapt to the original BN variables (same pattern as class)
      gum::Tensor< double > adapted;
      for (const auto& v: adj.variablesSequence()) {
        adapted.add(cm.observationalBN().variableFromName(v->name()));
      }
      adapted.fillWith(adj);

      // Should match the free counterfactual helper
      auto got_free = gum::counterfactual< double >(cm, on_names, whatif, profile, values);
      GUM_CHECK_TENSOR_ALMOST_EQUALS(adapted, got_free);
    }

    static void testSimpleCounterFactual() {
      const auto bn = gum::BayesNet< double >::fastPrototype("X->Y->Z");
      const auto cm = gum::CausalModel(bn);

      gum::HashTable< std::string, std::string > profile;
      profile.insert("Y", "1");

      gum::Set< std::string > whatif;
      whatif.insert("X");

      const auto twin = gum::counterfactualModel(cm, profile, whatif);

      CHECK_NE(twin.observationalBN().size(), 0u);
      CHECK_NE(twin.observationalBN().sizeArcs(), 0u);
      CHECK_NE(twin.causalDAG().size(), 0u);
      CHECK_NE(twin.causalDAG().sizeArcs(), 0u);
    }
  };

  GUM_TEST_ACTIF(Counterfactual_FromBIFXML_Names)
  GUM_TEST_ACTIF(Counterfactual_FromBIFXML_IDs)
  GUM_TEST_ACTIF(Counterfactual_FromBIFXML_Smoke_NoProfile)
  GUM_TEST_ACTIF(FreeCounterfactual_FromBIFXML_Names)
  GUM_TEST_ACTIF(FreeCounterfactualModel_roundtrip)
  GUM_TEST_ACTIF(SimpleCounterFactual)

}   // namespace gum_tests
