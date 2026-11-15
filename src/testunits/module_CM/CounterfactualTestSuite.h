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
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/io/BIFXML/BIFXMLBNReader.h>
#include <agrum/CM/causalModel.h>
#include <agrum/CM/counterfactual.h>

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


    // Same scenario, using the NodeSet / ID-based overloads, from the same BIFXML.


    // Light smoke: ensures ctor+eval do not throw on empty profile using same file.


    // -------------------------------------------------------------------------
    // Free-function parity with Python API
    // -------------------------------------------------------------------------
  };

  GUM_TEST(Counterfactual_FromBIFXML_Names) {
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
    CHECK_NOTHROW({ const auto _ = cf.toString(); });

    const auto& got = cf.value();
    auto        I   = instFor(got, {{"salary", "81"}});
    CHECK_EQ((got.get(I)), doctest::Approx(1.0).epsilon(1e-12));

    auto I0 = instFor(got, {{"salary", "80"}});
    CHECK_LT(std::abs(got.get(I0)), 1e-12);
    auto I2 = instFor(got, {{"salary", "82"}});
    CHECK_LT(std::abs(got.get(I2)), 1e-12);
  }

  GUM_TEST(Counterfactual_FromBIFXML_IDs) {
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

    CHECK_NOTHROW({ const auto _ = cf.toString(); });

    const auto& got = cf.value();
    auto        I   = instFor(got, {{"salary", "81"}});
    CHECK_EQ((got.get(I)), doctest::Approx(1.0).epsilon(1e-12));
  }

  GUM_TEST(Counterfactual_FromBIFXML_Smoke_NoProfile) {
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

  GUM_TEST(FreeCounterfactual_FromBIFXML_Names) {
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

  GUM_TEST(FreeCounterfactualModel_roundtrip) {
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

    // Build the twin network with the free function
    auto twincm = gum::counterfactualModel< double >(cm, whatif);

    // The counterfactual is a posterior in the twin network: profile in the real world, values on
    // the copies
    const auto&                    twin = twincm.observationalBN();
    gum::LazyPropagation< double > ie(&twin);
    for (const auto& [name, label]: profile)
      ie.addEvidence(name, label);
    ie.addEvidence("education*", "medium");
    ie.makeInference();
    const auto& post = ie.posterior("salary*");

    const auto got_free = gum::counterfactual< double >(cm, on_names, whatif, profile, values);
    CHECK_EQ(post.domainSize(), got_free.domainSize());
    for (gum::Idx i = 0; i < post.domainSize(); ++i) {
      gum::Instantiation Ip(post), If(got_free);
      Ip.chgVal(post.variable(0), i);
      If.chgVal(got_free.variable(0), i);
      CHECK_EQ(post.get(Ip), doctest::Approx(got_free.get(If)).epsilon(1e-10));
    }
  }

  GUM_TEST(TwinNetworkStructure) {
    // the copies are the whatif variables and their descendants; the other variables are shared
    auto bn = gum::BayesNet< double >::fastPrototype("X->Y->Z;W->Y;W->V");
    bn.generateCPTs();
    const gum::CausalModel< double > cm(bn);

    const auto  twincm = gum::counterfactualModel(cm, names({"X"}));
    const auto& twin   = twincm.observationalBN();
    CHECK_EQ(twin.size(), 8u);
    for (const auto& n: {"V", "W", "X", "Y", "Z", "X*", "Y*", "Z*"})
      CHECK_NOTHROW((void)twin.idFromName(n));
    CHECK(twin.parents("X*").empty());
    CHECK_EQ(twin.parents("Y*"), gum::NodeSet{twin.idFromName("X*"), twin.idFromName("W")});
    CHECK_EQ(twin.parents("Z*"), gum::NodeSet{twin.idFromName("Y*")});

    // a copy has the CPT of its original
    gum::Instantiation Io(bn.cpt("Y")), It(twin.cpt("Y*"));
    for (Io.setFirst(); !Io.end(); Io.inc()) {
      It.chgVal("Y*", Io.val(bn.variableFromName("Y")));
      It.chgVal("X*", Io.val(bn.variableFromName("X")));
      It.chgVal("W", Io.val(bn.variableFromName("W")));
      CHECK_EQ(twin.cpt("Y*").get(It), doctest::Approx(bn.cpt("Y").get(Io)));
    }
  }

  GUM_TEST(CounterfactualAbductionIsJoint) {
    // X = U1 xor U2, Y = U1 xor U2 xor T: observing X=1, T=0 (hence Y=1), had T been 1, Y would
    // have been 0. The posteriors of U1 and U2 are uniform, only their joint posterior gives the
    // answer.
    auto bn = gum::BayesNet< double >::fastPrototype("U1->X<-U2;U1->Y<-U2;T->Y");
    for (const auto& n: {"U1", "U2", "T"})
      bn.cpt(n).fillWith({0.5, 0.5});
    const auto xorFill = [&bn](const std::string& node, const std::vector< std::string >& parents) {
      const auto& cpt = bn.cpt(node);
      for (gum::Instantiation I(cpt); !I.end(); I.inc()) {
        gum::Idx x = 0;
        for (const auto& p: parents)
          x ^= I.val(bn.variableFromName(p));
        cpt.set(I, I.val(bn.variableFromName(node)) == x ? 1.0 : 0.0);
      }
    };
    xorFill("X", {"U1", "U2"});
    xorFill("Y", {"U1", "U2", "T"});
    const gum::CausalModel< double > cm(bn);

    gum::HashTable< std::string, std::string > profile;
    profile.insert("X", "1");
    profile.insert("T", "0");
    profile.insert("Y", "1");
    gum::HashTable< std::string, std::string > values;
    values.insert("T", "1");

    const auto         got = gum::counterfactual(cm, names({"Y"}), names({"T"}), profile, values);
    gum::Instantiation I(got);
    I.chgVal("Y", "0");
    CHECK_EQ(got.get(I), doctest::Approx(1.0));
    I.chgVal("Y", "1");
    CHECK_EQ(got.get(I), doctest::Approx(0.0));
  }

  GUM_TEST(CounterfactualWithoutProfileIsTheCausalEffect) {
    auto bn = gum::BayesNet< double >::fastPrototype("U->X->Y<-U");
    bn.generateCPTs();
    const gum::CausalModel< double > cm(bn);

    const auto cf = gum::counterfactual(cm, names({"Y"}), names({"X"}));
    const auto ce = gum::CausalImpact< double >(cm, names({"Y"}), names({"X"}), StrSet{}).eval();
    for (gum::Instantiation I(cf); !I.end(); I.inc()) {
      gum::Instantiation J(ce);
      J.chgVal("X", I.val(cf.variable(cf.pos(cm.observationalBN().variableFromName("X")))));
      J.chgVal("Y", I.val(cf.variable(cf.pos(cm.observationalBN().variableFromName("Y")))));
      CHECK_EQ(cf.get(I), doctest::Approx(ce.get(J)).epsilon(1e-10));
    }
  }

  GUM_TEST(CounterfactualOnANonDescendant) {
    // W is not a descendant of X: its counterfactual value is its factual posterior, whatever x
    auto bn = gum::BayesNet< double >::fastPrototype("W->X->Y;W->Y");
    bn.generateCPTs();
    const gum::CausalModel< double >           cm(bn);
    gum::HashTable< std::string, std::string > profile;
    profile.insert("Y", "1");

    const auto got = gum::counterfactual(cm, names({"W"}), names({"X"}), profile);
    gum::LazyPropagation< double > ie(&bn);
    ie.addEvidence("Y", "1");
    ie.makeInference();
    const auto& pw = ie.posterior("W");
    for (gum::Instantiation I(got); !I.end(); I.inc()) {
      gum::Instantiation J(pw);
      J.chgVal("W", I.val(got.variable(got.pos(cm.observationalBN().variableFromName("W")))));
      CHECK_EQ(got.get(I), doctest::Approx(pw.get(J)).epsilon(1e-10));
    }
  }

  GUM_TEST(CounterfactualRefusesLatentVariables) {
    // a latent variable has no CPT: its posterior given the profile cannot be computed
    auto bn = gum::BayesNet< double >::fastPrototype("X->Y");
    bn.generateCPTs();
    const gum::CausalModel< double > cm(bn, {{"U", bn.ids({"X", "Y"})}});

    gum::HashTable< std::string, std::string > profile;
    profile.insert("X", "0");
    profile.insert("Y", "1");
    gum::Set< std::string > whatif;
    whatif.insert("X");
    gum::Set< std::string > on;
    on.insert("Y");

    CHECK_THROWS_AS(gum::counterfactualModel(cm, whatif), gum::InvalidArgument&);
    CHECK_THROWS_AS((gum::Counterfactual< double >(cm, on, whatif, profile)),
                    gum::InvalidArgument&);
  }

}   // namespace gum_tests
