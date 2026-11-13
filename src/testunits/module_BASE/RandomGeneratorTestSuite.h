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


#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>

#include <agrum/config.h>

#include <agrum/base/core/configuration/configuration.h>
#include <agrum/base/core/math/pow.h>
#include <agrum/base/core/timer.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/generator/MCBayesNetGenerator.h>

#include <agrum/base/core/utils_random.h>
#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {
  // the tests changing the seed give it back at the end: the seed of a run stays the one displayed
  struct RandomGeneratorTestSuite {
    public:
    // namespace gum_tests


    // MED-6: randomValue(0) must throw (uniform_int_distribution(0,-1) is UB)


    // MED-6: randomValue with valid max stays in range


    // MED-5: int2Pow with exponent >= 64 must throw (1UL << 64 is UB)
  };

  GUM_TEST(RandomSeed) {
    const auto seed = gum::randomGeneratorSeed();

    gum::initRandom(20);
    const auto x1 = gum::randomProba();
    const auto x2 = gum::randomProba();
    const auto x3 = gum::randomProba();

    gum::initRandom(0);
    // can falsely fail in rare occasion
    if (fabs((gum::randomProba()) - (x1)) < GUM_SMALL_ERROR)
      if (fabs((gum::randomProba()) - (x2)) < GUM_SMALL_ERROR)
        if (fabs((gum::randomProba()) - (x3)) < GUM_SMALL_ERROR) CHECK(false);
    gum::initRandom(20);
    CHECK(fabs((gum::randomProba()) - (x1)) < 1e-5);
    CHECK(fabs((gum::randomProba()) - (x2)) < 1e-5);
    CHECK(fabs((gum::randomProba()) - (x3)) < 1e-5);

    gum::initRandom(seed);
  }

  GUM_TEST(SeedInUse) {
    const auto seed = gum::randomGeneratorSeed();
    gum::initRandom(42);
    CHECK_EQ(gum::randomGeneratorSeed(), 42u);
    gum::initRandom(0);   // the clock: the value in use is known, hence replayable
    const auto clock = gum::randomGeneratorSeed();
    const auto x     = gum::randomProba();
    gum::initRandom(clock);
    CHECK_EQ(gum::randomProba(), x);
    gum::initRandom(seed);
  }

  GUM_TEST(RandomSeedKeyHasNoEffectOnTheGenerator) {
    const auto seed = gum::randomGeneratorSeed();
    {
      ConfigurationGuard guard;
      gum::config().takeNotices();
      gum::initRandom(7);
      const auto x = gum::randomProba();

      gum::initRandom(7);
      gum::config().set("core", "randomSeed", "99");
      CHECK_EQ(gum::randomProba(), x);
      CHECK_EQ(gum::randomGeneratorSeed(), 7u);

      auto notices = gum::config().takeNotices();
      REQUIRE_EQ(notices.size(), std::size_t(1));
      CHECK(notices[0].kind == gum::Configuration::Notice::Kind::NO_EFFECT);
      CHECK_NE(notices[0].message.find("initRandom()"), std::string::npos);

      // the same value or another key: nothing to say
      gum::config().set("core", "randomSeed", "99");
      gum::config().set("core", "jgumIndent", "2");
      CHECK(gum::config().takeNotices().empty());
    }
    gum::config().takeNotices();   // the guard gave the former value back
    gum::initRandom(seed);
  }

  GUM_TEST(RandomSeedForStructure) {
    // the same seed gives the same generated structure
    const auto seed = gum::randomGeneratorSeed();

    const auto generate = [] {
      gum::MCBayesNetGenerator< double > gen(100, 150, 3);
      gum::BayesNet< double >            bn;
      gum::initRandom(20);
      gen.generateBN(bn);
      return bn;
    };
    const auto bn1 = generate();
    const auto bn2 = generate();
    CHECK_EQ(bn1.size(), static_cast< gum::Size >(100));
    CHECK(bn1.dag() == bn2.dag());
    for (const auto node: bn1.nodes())
      CHECK_EQ(bn1.variable(node).name(), bn2.variable(node).name());

    gum::initRandom(seed);
  }

  GUM_TEST(BugSeed) {
    const auto seed = gum::randomGeneratorSeed();
    gum::initRandom(0);
    const auto x0 = gum::randomProba();
    gum::initRandom(10);
    const auto x10 = gum::randomProba();
    gum::initRandom(42);
    const auto x42 = gum::randomProba();

    gum::initRandom(0);
    CHECK_NE(x0, gum::randomProba());   // may fail but highly improbable
    gum::initRandom(10);
    CHECK_EQ(x10, gum::randomProba());
    gum::initRandom(42);
    CHECK_EQ(x42, gum::randomProba());
    gum::initRandom(seed);
  }

  GUM_TEST(BugSeed2) {
    const auto seed = gum::randomGeneratorSeed();
    gum::initRandom(0);
    const auto bn0 = gum::BayesNet< double >::fastPrototype("A->B<-C");
    gum::initRandom(10);
    const auto bn10 = gum::BayesNet< double >::fastPrototype("A->B<-C");
    gum::initRandom(42);
    const auto bn42 = gum::BayesNet< double >::fastPrototype("A->B<-C");

    gum::Timer t;
    t.reset();
    double second = 0.5;
    while (t.step() <= second)

      gum::initRandom(0);
    const auto bn0b = gum::BayesNet< double >::fastPrototype("A->B<-C");
    gum::initRandom(10);
    const auto bn10b = gum::BayesNet< double >::fastPrototype("A->B<-C");
    gum::initRandom(42);
    const auto bn42b = gum::BayesNet< double >::fastPrototype("A->B<-C");
    const auto q42b  = bn42b.cpt("B") + bn42b.cpt("C") + bn42b.cpt("A");

    for (const auto i: bn0.nodes())
      // may fail but highly improbable
      GUM_CHECK_TENSOR_DIFFERS(bn0.cpt(i), bn0b.cpt(bn0.variable(i).name()));

    for (const auto i: bn10.nodes())
      GUM_CHECK_TENSOR_ALMOST_EQUALS(bn10.cpt(i), bn10b.cpt(bn10.variable(i).name()));

    for (const auto i: bn42.nodes())
      GUM_CHECK_TENSOR_ALMOST_EQUALS(bn42.cpt(i), bn42b.cpt(bn42.variable(i).name()));

    gum::initRandom(seed);
  }

  GUM_TEST(RandomValueZeroThrows) { CHECK_THROWS_AS(gum::randomValue(0), const gum::OutOfBounds&); }

  GUM_TEST(RandomValueRange) {
    const auto seed = gum::randomGeneratorSeed();
    gum::initRandom(42);
    for (int i = 0; i < 1000; ++i) {
      const auto v = gum::randomValue(5);
      CHECK(v < gum::Idx(5));
    }
    gum::initRandom(seed);
  }

  // the draws depend only on the outputs of std::mt19937 (fixed by the standard): the same values
  // on every platform (the distributions of <random> differ between the standard libraries)
  GUM_TEST(PortableDraws) {
    const auto seed = gum::randomGeneratorSeed();
    gum::initRandom(42);

    std::vector< gum::Idx > values;
    for (int i = 0; i < 8; ++i)
      values.push_back(gum::randomValue(10));
    CHECK_EQ(values, std::vector< gum::Idx >{0, 4, 8, 0, 0, 0, 8, 6});

    CHECK_EQ(gum::randomValue(gum::Size(1) << 40), gum::Idx(201990867249));

    CHECK_EQ(gum::randomProba(), 0.29793956029886792);
    CHECK_EQ(gum::randomProba(), 0.78780346920631805);

    std::vector< int > shuffled{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    gum::randomShuffle(shuffled.begin(), shuffled.end());
    CHECK_EQ(shuffled, std::vector< int >{6, 8, 2, 5, 0, 3, 7, 9, 1, 4});

    // std::log and std::pow may differ in the last bit between the libraries
    CHECK(std::fabs(gum::randomGamma(2.5) - 3.4218334707071598) < 1e-12);
    CHECK(std::fabs(gum::randomGamma(0.5) - 0.095172959702188795) < 1e-12);

    gum::initRandom(seed);
  }

  GUM_TEST(RandomValueIsUniform) {
    const auto seed = gum::randomGeneratorSeed();
    gum::initRandom(42);
    std::vector< int > counts(7, 0);
    for (int i = 0; i < 70000; ++i)
      ++counts[gum::randomValue(7)];
    for (const auto count: counts)
      CHECK(std::abs(count - 10000) < 500);   // about 5 standard deviations
    for (int i = 0; i < 1000; ++i)
      CHECK(gum::randomValue((gum::Size(1) << 40) + 3) < (gum::Size(1) << 40) + 3);
    gum::initRandom(seed);
  }

  GUM_TEST(RandomProbaRange) {
    const auto seed = gum::randomGeneratorSeed();
    gum::initRandom(42);
    double sum = 0.0;
    for (int i = 0; i < 100000; ++i) {
      const double p = gum::randomProba();
      CHECK((p >= 0.0 && p < 1.0));
      sum += p;
    }
    CHECK(std::fabs(sum / 100000 - 0.5) < 0.01);
    gum::initRandom(seed);
  }

  GUM_TEST(RandomShuffleIsAPermutation) {
    std::vector< int > values(50);
    std::iota(values.begin(), values.end(), 0);
    gum::randomShuffle(values.begin(), values.end());
    std::vector< int > sorted = values;
    std::sort(sorted.begin(), sorted.end());
    std::vector< int > expected(50);
    std::iota(expected.begin(), expected.end(), 0);
    CHECK_EQ(sorted, expected);

    std::vector< int > empty;
    gum::randomShuffle(empty.begin(), empty.end());
    CHECK(empty.empty());
  }

  GUM_TEST(RandomGammaMean) {
    const auto seed = gum::randomGeneratorSeed();
    gum::initRandom(42);
    for (const double shape: {0.3, 1.0, 2.5, 10.0}) {
      double sum = 0.0;
      for (int i = 0; i < 20000; ++i) {
        const double g = gum::randomGamma(shape);
        CHECK(g >= 0.0);
        sum += g;
      }
      CHECK(std::fabs(sum / 20000 - shape) < 0.05 * shape + 0.02);   // the mean of Gamma(shape, 1)
    }
    CHECK_THROWS_AS(gum::randomGamma(0.0), const gum::OutOfBounds&);
    CHECK_THROWS_AS(gum::randomGamma(-1.0), const gum::OutOfBounds&);
    gum::initRandom(seed);
  }

  GUM_TEST(Int2PowBounds) {
    CHECK_EQ(gum::int2Pow(0), static_cast< uint64_t >(1));
    CHECK_EQ(gum::int2Pow(63), static_cast< uint64_t >(1) << 63);
    CHECK_THROWS_AS(gum::int2Pow(64), const gum::OutOfBounds&);
    CHECK_THROWS_AS(gum::int2Pow(100), const gum::OutOfBounds&);
  }

  // the draws after restore() continue the stream saved at construction, whatever happened in
  // between
  GUM_TEST(GuardRestoresTheStream) {
    const auto                  seed = gum::randomGeneratorSeed();
    std::vector< unsigned int > expected, actual;
    {
      gum::RandomGeneratorGuard reference;
      for (int i = 0; i < 5; ++i)
        expected.push_back(gum::currentRandomGeneratorValue());
    }   // the destructor restores
    CHECK_EQ(gum::randomGeneratorSeed(), seed);
    {
      gum::RandomGeneratorGuard guard;
      gum::initRandom(42);
      for (int i = 0; i < 10; ++i)
        gum::randomProba();
      CHECK_EQ(gum::randomGeneratorSeed(), 42u);
      CHECK(!guard.isRestored());
      guard.restore();
      CHECK(guard.isRestored());
      CHECK_EQ(gum::randomGeneratorSeed(), seed);
      for (int i = 0; i < 5; ++i)
        actual.push_back(gum::currentRandomGeneratorValue());
      // only the first call restores: neither a second call nor the destructor go back again
      guard.restore();
      actual.push_back(gum::currentRandomGeneratorValue());
    }
    CHECK_EQ(std::vector< unsigned int >(actual.begin(), actual.begin() + 5), expected);
    // the destructor did not restore again: the stream goes on after the 6 draws
    gum::RandomGeneratorGuard after;
    const auto                next = gum::currentRandomGeneratorValue();
    after.restore();
    CHECK_NE(next, expected[0]);
  }

  // the guard restores when the block is left by an exception
  GUM_TEST(GuardRestoresOnException) {
    const auto   seed = gum::randomGeneratorSeed();
    unsigned int expected;
    {
      gum::RandomGeneratorGuard reference;
      expected = gum::currentRandomGeneratorValue();
    }
    try {
      gum::RandomGeneratorGuard guard;
      gum::initRandom(7);
      GUM_ERROR(gum::OperationNotAllowed, "leaving the block")
    } catch (const gum::OperationNotAllowed&) {}
    CHECK_EQ(gum::randomGeneratorSeed(), seed);
    gum::RandomGeneratorGuard guard;
    CHECK_EQ(gum::currentRandomGeneratorValue(), expected);
  }
}   // namespace gum_tests
