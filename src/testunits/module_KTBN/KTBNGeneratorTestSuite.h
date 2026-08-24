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

#include <set>

#include <agrum/KTBN/generator/KTBNGenerator.h>
#include <agrum/KTBN/KTBN.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>


namespace gum_tests {

  // The generator has one job the k-DBN cannot check for itself: every arc it
  // emits must already be legal, so KTBN::addArc never throws. Most cases below
  // are therefore fuzz-shaped -- draw a model, then assert the invariants the
  // k-DBN definition imposes on it -- rather than asserting an exact structure.

  struct KTBNGeneratorTestSuite {
    static constexpr int AT = gum::KTBN< double >::ATEMPORAL;

    // largest lag actually reaching the kernel slice k-1; -1 if nothing does.
    static int maxKernelLag(const gum::KTBN< double >& m) {
      const int k   = static_cast< int >(m.k());
      int       out = -1;
      for (const auto& base: m.temporalVarNames())
        for (const auto& [parBase, parSlice]: m.parents(base, k - 1)) {
          (void)parBase;
          if (parSlice != AT && (k - 1 - parSlice) > out) out = k - 1 - parSlice;
        }
      return out;
    }

    // every arc must respect the k-DBN's causality rules
    static void checkArcsLegal(const gum::KTBN< double >& m) {
      const auto& temporal = m.temporalVarNames();
      for (const auto& [tail, head]: m.arcs()) {
        const bool tailTemporal = temporal.find(tail.first) != temporal.end();
        const bool headTemporal = temporal.find(head.first) != temporal.end();
        if (!headTemporal) CHECK_FALSE(tailTemporal);      // temporal -> atemporal is illegal
        if (tailTemporal && headTemporal) {
          CHECK(tail.second <= head.second);               // never future -> past
          CHECK(head.second - tail.second <= static_cast< int >(m.k()) - 1);
        }
      }
    }
  };

  GUM_TEST(GeneratorShapeAndLegality) {
    gum::initRandom(42);
    gum::KTBNGenerator< double > gen(3, 4, 2);
    const auto                   m = gen.generate();

    CHECK_EQ(m.k(), gum::Size(3));
    CHECK_EQ(m.nbTemporalVars(), gum::Size(4));
    CHECK_EQ(m.nbAtemporalVars(), gum::Size(2));
    CHECK_EQ(m.size(), gum::Size(4 * 3 + 2));   // 4 processes x 3 slices + 2 atemporal
    KTBNGeneratorTestSuite::checkArcsLegal(m);
  }

  GUM_TEST(GeneratorGuaranteesEffectiveOrder) {
    // the point of guaranteeOrder: without it a "k-TBN" can silently have an
    // effective order below k, which makes order recovery ill-posed.
    gum::initRandom(7);
    for (gum::Size k: {2, 3, 5}) {
      gum::KTBNGenerator< double > gen(k, 3, 0);
      gen.setDensity(0.05);   // sparse on purpose: without the guarantee, max lag would often fall short
      for (int rep = 0; rep < 20; ++rep) {
        const auto m = gen.generate();
        CHECK_EQ(KTBNGeneratorTestSuite::maxKernelLag(m), static_cast< int >(k) - 1);
      }
    }
  }

  GUM_TEST(GeneratorGuaranteeOrderIsANoOpForK1) {
    // k=1 has no lag at all, so the guarantee cannot apply and must not throw
    gum::initRandom(3);
    gum::KTBNGenerator< double > gen(1, 3, 1);
    GUM_CHECK_ASSERT_THROWS_NOTHING(gen.generate());
    const auto m = gen.generate();
    CHECK_EQ(m.k(), gum::Size(1));
    KTBNGeneratorTestSuite::checkArcsLegal(m);
  }

  GUM_TEST(GeneratorWithoutGuaranteeCanFallShort) {
    // the phenomenon guaranteeOrder exists to remove: at a low density and a
    // large k, some draws never reach lag k-1.
    gum::initRandom(11);
    gum::KTBNGenerator< double > gen(5, 3, 0);
    gen.setGuaranteeOrder(false).setDensity(0.03);
    int shortfalls = 0;
    for (int rep = 0; rep < 40; ++rep)
      if (KTBNGeneratorTestSuite::maxKernelLag(gen.generate()) < 4) ++shortfalls;
    CHECK(shortfalls > 0);   // at least one draw has an effective order below k
  }

  GUM_TEST(GeneratorReproducibleFromSeed) {
    gum::initRandom(2024);
    gum::KTBNGenerator< double > g1(4, 3, 1);
    const auto                   a = g1.generate();

    gum::initRandom(2024);
    gum::KTBNGenerator< double > g2(4, 3, 1);
    const auto                   b = g2.generate();

    CHECK_EQ(a.sizeArcs(), b.sizeArcs());
    CHECK_EQ(a.toString(), b.toString());
  }

  GUM_TEST(GeneratorDensityAndMaxArcs) {
    gum::initRandom(5);
    gum::KTBNGenerator< double > gen(3, 4, 1);

    CHECK(gen.nbLegalArcs() > gum::Size(0));

    gen.setDensity(0.0).setGuaranteeOrder(false);
    CHECK_EQ(gen.generate().sizeArcs(), gum::Size(0));   // no arcs at all

    gen.setGuaranteeOrder(true);
    CHECK_EQ(gen.generate().sizeArcs(), gum::Size(1));   // only the guaranteed one

    gum::KTBNGenerator< double > capped(3, 4, 1, 5);     // maxArcs = 5
    CHECK(capped.generate().sizeArcs() <= gum::Size(5));

    gum::KTBNGenerator< double > saturated(3, 3, 1, 100000);
    CHECK_EQ(saturated.generate().sizeArcs(), saturated.nbLegalArcs());
  }

  GUM_TEST(GeneratorMaxParentsIsRespected) {
    gum::initRandom(17);
    gum::KTBNGenerator< double > gen(3, 4, 1);
    gen.setDensity(1.0).setMaxParents(2).setGuaranteeOrder(false);
    const auto m = gen.generate();
    for (const auto& [base, slice]: m.nodes())
      CHECK(m.parents(base, slice).size() <= std::size_t(2));
  }

  GUM_TEST(GeneratorDomainRangeAndNoAtemporal) {
    gum::initRandom(23);
    gum::KTBNGenerator< double > gen(2, 5, 0);
    gen.setDomainRange(3, 5);
    const auto m = gen.generate();

    CHECK_EQ(m.nbAtemporalVars(), gum::Size(0));
    for (const auto& [base, slice]: m.nodes()) {
      const auto d = m.variable(base, slice).domainSize();
      CHECK(d >= gum::Size(3));
      CHECK(d <= gum::Size(5));
    }
  }

  GUM_TEST(GeneratorSingleAtemporalHasNoAtemporalArc) {
    // one atemporal variable admits no atemporal -> atemporal arc
    gum::initRandom(31);
    gum::KTBNGenerator< double > gen(2, 2, 1);
    gen.setDensity(1.0);
    const auto m = gen.generate();
    for (const auto& [tail, head]: m.arcs()) {
      const bool bothAtemporal = (tail.second == AT) && (head.second == AT);
      CHECK_FALSE(bothAtemporal);   // doctest cannot parse && inside its macros
    }
  }

  GUM_TEST(GeneratorOutputSurvivesUnrollAndRoundTrip) {
    // a generated model must be a first-class KTBN: unrollable and serialisable
    gum::initRandom(99);
    gum::KTBNGenerator< double > gen(3, 3, 1);
    const auto                   m = gen.generate();

    GUM_CHECK_ASSERT_THROWS_NOTHING(m.unroll(6));
    CHECK_EQ(m.unroll(6).size(), gum::Size(3 * 6 + 1));

    const std::string path = GET_RESSOURCES_PATH("outputs/ktbn_generated.jgum");
    GUM_CHECK_ASSERT_THROWS_NOTHING(m.save(path));
    const auto back = gum::KTBN< double >::load(path);
    CHECK_EQ(back.k(), m.k());
    CHECK_EQ(back.sizeArcs(), m.sizeArcs());
  }

  GUM_TEST(GeneratorArgumentValidation) {
    CHECK_THROWS_AS(gum::KTBNGenerator< double >(0, 3), const gum::InvalidArgument&);
    CHECK_THROWS_AS(gum::KTBNGenerator< double >(2, 3, 0, 0, 1), const gum::InvalidArgument&);

    gum::KTBNGenerator< double > gen(2, 3);
    CHECK_THROWS_AS(gen.setDensity(-0.1), const gum::OutOfBounds&);
    CHECK_THROWS_AS(gen.setDensity(1.5), const gum::OutOfBounds&);
    CHECK_THROWS_AS(gen.setDomainRange(1, 4), const gum::InvalidArgument&);
    CHECK_THROWS_AS(gen.setDomainRange(4, 2), const gum::InvalidArgument&);
    CHECK_THROWS_AS(gen.setNamePrefixes("", "A"), const gum::InvalidArgument&);
    CHECK_THROWS_AS(gen.setNamePrefixes("X", "X"), const gum::InvalidArgument&);
  }

  GUM_TEST(GeneratorNbLegalArcsMatchesSaturation) {
    // the closed-form count must agree with what a saturating draw produces
    for (gum::Size k: {1, 2, 3}) {
      for (gum::Size n: {0, 1, 3}) {
        for (gum::Size m: {0, 1, 2}) {
          gum::initRandom(4);
          gum::KTBNGenerator< double > gen(k, n, m, 1000000);
          gen.setGuaranteeOrder(false);
          CHECK_EQ(gen.generate().sizeArcs(), gen.nbLegalArcs());
        }
      }
    }
  }
}   // namespace gum_tests
