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
#include <cstdio>
#include <fstream>
#include <vector>

#include <agrum/base/multidim/instantiation.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/GUM/GumBNWriter.h>
#include <agrum/KTBN/KTBN.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {

  struct KTBNPersistenceTestSuite {
    static constexpr int AT = gum::KTBN< double >::ATEMPORAL;

    // k=3 model: X, Y temporal; C atemporal; arcs of mixed lag into the kernel slice.
    static gum::KTBN< double > buildK3Model() {
      gum::KTBN< double > m(3);
      m.add(gum::LabelizedVariable("X", "", 2), true);
      m.add(gum::LabelizedVariable("Y", "", 2), true);
      m.add(gum::LabelizedVariable("C", "", 2), false);
      m.addArc("X", 0, "X", 2);
      m.addArc("X", 1, "X", 2);
      m.addArc("X", 2, "Y", 2);
      m.addArc("C", AT, "X", 2);
      m.generateCPTs();
      return m;
    }

    // Flat CPT values in Instantiation order: comparable element-wise across
    // structurally identical KTBNs regardless of which BN instance owns the vars.
    static std::vector< double > flatCPT(const gum::Tensor< double >& cpt) {
      std::vector< double > v;
      v.reserve(cpt.domainSize());
      for (gum::Instantiation inst(cpt); !inst.end(); inst.inc())
        v.push_back(cpt[inst]);
      return v;
    }

    static void checkCPTsEqual(const gum::Tensor< double >& a, const gum::Tensor< double >& b) {
      CHECK_EQ(a.domainSize(), b.domainSize());
      const auto va = flatCPT(a), vb = flatCPT(b);
      for (std::size_t i = 0; i < va.size(); ++i)
        CHECK(std::abs(va[i] - vb[i]) < 1e-9);
    }

    static void checkK3RoundTrip(const gum::KTBN< double >& orig, const gum::KTBN< double >& m2) {
      CHECK_EQ(m2.k(), gum::Size(3));
      CHECK_EQ(m2.nbTemporalVars(), gum::Size(2));
      CHECK_EQ(m2.nbAtemporalVars(), gum::Size(1));
      CHECK(m2.existsArc("X", 0, "X", 2));
      CHECK(m2.existsArc("X", 1, "X", 2));
      CHECK(m2.existsArc("X", 2, "Y", 2));
      CHECK(m2.existsArc("C", AT, "X", 2));
      // CPT values must survive serialisation (not just structure)
      checkCPTsEqual(m2.cpt("X", 0), orig.cpt("X", 0));
      checkCPTsEqual(m2.cpt("X", 1), orig.cpt("X", 1));
      checkCPTsEqual(m2.cpt("X", 2), orig.cpt("X", 2));
      checkCPTsEqual(m2.cpt("Y", 0), orig.cpt("Y", 0));
      checkCPTsEqual(m2.cpt("Y", 1), orig.cpt("Y", 1));
      checkCPTsEqual(m2.cpt("Y", 2), orig.cpt("Y", 2));
      checkCPTsEqual(m2.cpt("C"), orig.cpt("C"));
    }

    // hand-set CPT values survive both formats exactly, and KTBN properties (k,
    // temporal/atemporal sets) are restored from embedded BN properties, not names.

    // ".jgum" is text, anything else is binary with ".bgum" appended when
    // missing; a name with no extension must save to and load from ".bgum".

    // a plain BN file (no KTBN.* properties) falls back to fromBN(), re-deriving
    // k and the temporal/atemporal split from node names.

    // k=1 is the case the property-based format exists to disambiguate
    // (re-derivation from names alone is ambiguous there).
  };

  GUM_TEST(SaveLoadRoundTrip) {
    const auto m = buildK3Model();

    // text format (.jgum)
    const std::string fn = GET_RESSOURCES_PATH("outputs/KTBNTestSuite_roundtrip.jgum");
    GUM_CHECK_ASSERT_THROWS_NOTHING(m.save(fn));
    checkK3RoundTrip(m, gum::KTBN< double >::load(fn));

    // binary format (.bgum)
    const std::string fnb = GET_RESSOURCES_PATH("outputs/KTBNTestSuite_roundtrip.bgum");
    GUM_CHECK_ASSERT_THROWS_NOTHING(m.save(fnb));
    checkK3RoundTrip(m, gum::KTBN< double >::load(fnb));
  }

  GUM_TEST(SaveLoadCPTValues) {
    gum::KTBN< double > m(2);
    m.add(gum::LabelizedVariable("X", "", 2), true);
    m.add(gum::LabelizedVariable("C", "", 2), false);
    m.addArc("X", 0, "X", 1);
    m.addArc("C", AT, "X", 1);

    // known values so we can assert exact (within 1e-9) round-trip
    m.cpt("X", 0).fillWith({0.7, 0.3});
    m.cpt("C").fillWith({0.4, 0.6});
    m.cpt("X", 1).fillWith({0.9, 0.1, 0.2, 0.8, 0.6, 0.4, 0.3, 0.7});

    auto check = [&](const gum::KTBN< double >& loaded) {
      CHECK_EQ(loaded.k(), gum::Size(2));
      CHECK(loaded.exists("X"));
      CHECK(loaded.exists("C"));
      CHECK(loaded.existsArc("X", 0, "X", 1));
      CHECK(loaded.existsArc("C", AT, "X", 1));
      checkCPTsEqual(loaded.cpt("X", 0), m.cpt("X", 0));
      checkCPTsEqual(loaded.cpt("C"), m.cpt("C"));
      checkCPTsEqual(loaded.cpt("X", 1), m.cpt("X", 1));
    };

    const std::string fnj = GET_RESSOURCES_PATH("outputs/KTBN_cptvalues.jgum");
    const std::string fnb = GET_RESSOURCES_PATH("outputs/KTBN_cptvalues.bgum");

    m.save(fnj);
    check(gum::KTBN< double >::load(fnj));

    m.save(fnb);
    check(gum::KTBN< double >::load(fnb));
  }

  GUM_TEST(ExtensionResolution) {
    const auto m = buildK3Model();

    const std::string stem = GET_RESSOURCES_PATH("outputs/KTBN_noext");
    std::remove((stem + ".bgum").c_str());           // clean slate

    GUM_CHECK_ASSERT_THROWS_NOTHING(m.save(stem));   // no extension -> append ".bgum"

    // the binary file was created under the appended extension
    std::ifstream f(stem + ".bgum", std::ios::binary);
    CHECK(f.good());
    f.close();

    // load() resolves the same extension-less name back to ".bgum"
    checkK3RoundTrip(m, gum::KTBN< double >::load(stem));
  }

  GUM_TEST(LoadFallbackFromBN) {
    gum::BayesNet< double > bn;
    bn.add(gum::LabelizedVariable("X[0]", "", 2));
    bn.add(gum::LabelizedVariable("X[1]", "", 2));
    bn.add(gum::LabelizedVariable("C", "", 2));
    bn.addArc(bn.idFromName("X[0]"), bn.idFromName("X[1]"));
    bn.addArc(bn.idFromName("C"), bn.idFromName("X[1]"));
    bn.generateCPTs();

    // write it with a plain BN writer -> NO KTBN.* properties in the file
    const std::string          fn = GET_RESSOURCES_PATH("outputs/KTBN_plainbn.jgum");
    gum::GumBNWriter< double > writer(false);   // text
    writer.write(fn, bn);

    const auto loaded = gum::KTBN< double >::load(fn);
    CHECK_EQ(loaded.k(), gum::Size(2));
    CHECK_EQ(loaded.nbTemporalVars(), gum::Size(1));
    CHECK_EQ(loaded.nbAtemporalVars(), gum::Size(1));
    CHECK(loaded.existsArc("X", 0, "X", 1));
    CHECK(loaded.existsArc("C", AT, "X", 1));
    // CPT values survive the plain-BN round trip too
    checkCPTsEqual(loaded.cpt("X", 1), bn.cpt(bn.idFromName("X[1]")));
  }

  GUM_TEST(K1SaveLoad) {
    gum::KTBN< double > m(1);
    m.add(gum::LabelizedVariable("X", "", 2), true);    // temporal, single slice X[0]
    m.add(gum::LabelizedVariable("C", "", 2), false);   // atemporal
    m.addArc("C", AT, "X", 0);
    m.cpt("C").fillWith({0.4, 0.6});
    m.cpt("X", 0).fillWith({0.3, 0.7, 0.8, 0.2});       // P(X[0] | C)

    auto check = [&](const gum::KTBN< double >& loaded) {
      CHECK_EQ(loaded.k(), gum::Size(1));
      CHECK_EQ(loaded.nbTemporalVars(), gum::Size(1));
      CHECK_EQ(loaded.nbAtemporalVars(), gum::Size(1));
      CHECK(loaded.existsArc("C", AT, "X", 0));
      checkCPTsEqual(loaded.cpt("X", 0), m.cpt("X", 0));
      checkCPTsEqual(loaded.cpt("C"), m.cpt("C"));
    };

    const std::string fnj = GET_RESSOURCES_PATH("outputs/KTBN_k1.jgum");
    const std::string fnb = GET_RESSOURCES_PATH("outputs/KTBN_k1.bgum");

    m.save(fnj);
    check(gum::KTBN< double >::load(fnj));

    m.save(fnb);
    check(gum::KTBN< double >::load(fnb));
  }
}   // namespace gum_tests
