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


#include <atomic>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <thread>
#include <vector>

#include <agrum/base/core/configuration/configuration.h>
#include <agrum/base/core/exceptions.h>
#include <agrum/base/core/math/chi2.h>
#include <agrum/base/core/math/rational.h>

#include <agrum/base/core/math/math_utils.h>
#include <agrum/base/core/utils_misc.h>
#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {

  // The three numerical tolerances of aGrUM (core.epsilonCoarse, core.epsilonStandard and
  // core.epsilonFine) are read without lock from the configuration.
  struct EpsilonConfigurationTestSuite {
    public:
    static std::string writeFile(const std::string& content) {
      const auto path = getTempFilePath(".ini");
      std::ofstream(path) << content;
      return path;
    }
  };

  GUM_TEST(EpsilonDefaults) {
    ConfigurationGuard guard;
    CHECK_EQ(&gum::config(), &gum::Configuration::instance());

    const auto& conf = gum::config();
    CHECK_EQ(conf.epsilonCoarse(), 1e-2);
    CHECK_EQ(conf.epsilonStandard(), 1e-6);
    CHECK_EQ(conf.epsilonFine(), 1e-10);
    CHECK_EQ(conf.typed< double >("core", "epsilonStandard"), conf.epsilonStandard());
  }

  GUM_TEST(EpsilonFollowsSetPopAndReset) {
    ConfigurationGuard guard;
    auto&              conf = gum::config();

    {
      ConfigurationGuard scoped;
      conf.set("core", "epsilonCoarse", "0.05");
      conf.set("core", "epsilonStandard", "1e-7");
      conf.set("core", "epsilonFine", "1e-11");
      CHECK_EQ(conf.epsilonCoarse(), 0.05);
      CHECK_EQ(conf.epsilonStandard(), 1e-7);
      CHECK_EQ(conf.epsilonFine(), 1e-11);

      conf.reset();
      CHECK_EQ(conf.epsilonCoarse(), 1e-2);
      CHECK_EQ(conf.epsilonStandard(), 1e-6);
      CHECK_EQ(conf.epsilonFine(), 1e-10);

      conf.set("core", "epsilonStandard", "1e-5");
      CHECK_EQ(conf.epsilonStandard(), 1e-5);
    }
    CHECK_EQ(conf.epsilonStandard(), 1e-6);
  }

  GUM_TEST(EpsilonInvalidValuesAreRefused) {
    auto& conf = gum::config();

    ConfigurationGuard guard;
    for (const char* bad: {"0", "1", "-1e-3", "2", "nan", "inf", "abc", "1e-4", "1e-9"}) {
      CHECK_THROWS_AS(conf.set("core", "epsilonStandard", bad), gum::InvalidArgument);
      CHECK_EQ(conf.epsilonStandard(), 1e-6);
      CHECK_EQ(conf.typed< double >("core", "epsilonStandard"), 1e-6);
    }
  }

  GUM_TEST(EpsilonDomainsDoNotOverlap) {
    // coarse in [1e-3,1e-1], standard in [1e-8,1e-5], fine in [1e-11,1e-9] : whatever the
    // values chosen, fine < standard < coarse
    auto& conf = gum::config();

    ConfigurationGuard guard;
    CHECK_NOTHROW(conf.set("core", "epsilonCoarse", "1e-3"));
    CHECK_NOTHROW(conf.set("core", "epsilonCoarse", "1e-1"));
    CHECK_THROWS_AS(conf.set("core", "epsilonCoarse", "1e-4"), gum::InvalidArgument);
    CHECK_THROWS_AS(conf.set("core", "epsilonCoarse", "0.5"), gum::InvalidArgument);
    CHECK_NOTHROW(conf.set("core", "epsilonStandard", "1e-8"));
    CHECK_NOTHROW(conf.set("core", "epsilonStandard", "1e-5"));
    CHECK_THROWS_AS(conf.set("core", "epsilonStandard", "1e-2"), gum::InvalidArgument);
    CHECK_NOTHROW(conf.set("core", "epsilonFine", "1e-11"));
    CHECK_NOTHROW(conf.set("core", "epsilonFine", "1e-9"));
    CHECK_THROWS_AS(conf.set("core", "epsilonFine", "1e-6"), gum::InvalidArgument);
    CHECK_THROWS_AS(conf.set("core", "epsilonFine", "1e-12"), gum::InvalidArgument);
    CHECK_LT(conf.epsilonFine(), conf.epsilonStandard());
    CHECK_LT(conf.epsilonStandard(), conf.epsilonCoarse());
  }

  GUM_TEST(Chi2CriticalValueTerminatesWithTheFinestStandardEpsilon) {
    // the accuracy of the bisection is epsilonStandard : its lower bound is above the spacing of
    // the doubles around the largest chi2 value (99999), so that the bisection always ends
    auto& conf = gum::config();

    ConfigurationGuard guard;
    conf.set("core", "epsilonStandard", "1e-8");
    for (const double proba: {0.5, 0.05, 1e-10, 1e-300}) {
      const double v = gum::Chi2::criticalValue(proba, 1000);
      CHECK(std::isfinite(v));
      CHECK_GE(v, 0.);
      CHECK_LE(v, 99999.);
    }
  }

  GUM_TEST(EpsilonFromAFile) {
    ConfigurationGuard guard;
    auto&              conf = gum::config();

    const auto path = EpsilonConfigurationTestSuite::writeFile(
        "[core]\nepsilonFine = 5\nepsilonStandard = 1e-7\n");
    {
      ConfigurationGuard scoped;
      // an invalid value in a file is a notice and changes nothing, the valid ones are applied
      conf.takeNotices();
      conf.load(path);
      CHECK_EQ(conf.epsilonFine(), 1e-10);
      CHECK_EQ(conf.epsilonStandard(), 1e-7);
      CHECK_FALSE(conf.takeNotices().empty());
    }
    std::filesystem::remove(path);

    CHECK_EQ(conf.epsilonStandard(), 1e-6);
  }

  GUM_TEST(EpsilonOfALocalConfigurationKeepsTheDefaults) {
    // a schema without these keys
    gum::Configuration local("[s]\n### int :: a key\nk = 1\n");
    CHECK_EQ(local.epsilonCoarse(), 1e-2);
    CHECK_EQ(local.epsilonStandard(), 1e-6);
    CHECK_EQ(local.epsilonFine(), 1e-10);
  }

  GUM_TEST(EpsilonIsTheDefaultOfTheMathFunctions) {
    ConfigurationGuard guard;
    auto&              conf = gum::config();

    CHECK(!gum::isCloseToZero(5e-10));
    CHECK(!gum::isCloseToOne(1. + 5e-10));

    {
      ConfigurationGuard scoped;
      conf.set("core", "epsilonFine", "1e-9");
      CHECK(gum::isCloseToZero(5e-10));
      CHECK(gum::isCloseToOne(1. + 5e-10));
      // an explicit tolerance is never changed
      CHECK(!gum::isCloseToZero(5e-10, 1e-12));
    }

    CHECK(!gum::isCloseToZero(5e-10));
  }

  GUM_TEST(EpsilonIsTheDefaultOfTheRationalApproximations) {
    ConfigurationGuard guard;
    auto&              conf = gum::config();
    int64_t            num = 0, den = 0;

    // 0.33333 is at 3.3e-6 of 1/3 : out of the default tolerance (1e-6), in the loosest one (1e-5)
    gum::Rational< double >::farey(num, den, 0.33333);
    CHECK((num != 1 || den != 3));

    {
      ConfigurationGuard scoped;
      conf.set("core", "epsilonStandard", "1e-5");
      gum::Rational< double >::farey(num, den, 0.33333);
      CHECK_EQ(num, 1);
      CHECK_EQ(den, 3);

      int64_t num2 = 0, den2 = 0;
      gum::Rational< double >::continuedFracFirst(num2, den2, 0.33333);
      gum::Rational< double >::continuedFracFirst(num, den, 0.33333, 1e-5);
      CHECK_EQ(num2, num);
      CHECK_EQ(den2, den);
    }

    gum::Rational< double >::farey(num, den, 0.33333);
    CHECK((num != 1 || den != 3));
  }

  GUM_TEST(EpsilonIsTheToleranceOfAlmostDifferent) {
    ConfigurationGuard             guard;
    auto&                          conf = gum::config();
    gum::AlmostDifferent< double > different;

    CHECK(different(1.0, 1.0 + 5e-6));
    CHECK(different(0.0, 5e-6));

    {
      ConfigurationGuard scoped;
      conf.set("core", "epsilonStandard", "1e-5");
      // the tolerance is read when the functor is built : a new functor follows the configuration
      gum::AlmostDifferent< double > looser;
      CHECK(!looser(1.0, 1.0 + 5e-6));
      CHECK(!looser(0.0, 5e-6));
      CHECK(looser(1.0, 1.1));
      // ... and the functor built before keeps the tolerance it was built with
      CHECK(different(1.0, 1.0 + 5e-6));
    }

    // back to the default tolerance
    gum::AlmostDifferent< double > again;
    CHECK(again(1.0, 1.0 + 5e-6));
  }

  GUM_TEST(EpsilonCanBeReadWhileItIsChanged) {
    ConfigurationGuard  guard;
    auto&               conf = gum::config();
    std::atomic< bool > stop{false};
    std::atomic< int >  nbBad{0};

    {
      ConfigurationGuard         scoped;
      std::vector< std::thread > readers;
      for (int t = 0; t < 4; ++t) {
        readers.emplace_back([&]() {
          while (!stop.load()) {
            const auto e = conf.epsilonStandard();
            // only the two values written below are possible
            if (e != 1e-6 && e != 1e-5) nbBad++;
          }
        });
      }
      for (int i = 0; i < 200; ++i)
        conf.set("core", "epsilonStandard", (i % 2) ? "1e-6" : "1e-5");
      stop = true;
      for (auto& r: readers)
        r.join();
    }

    CHECK_EQ(nbBad.load(), 0);
  }
}   // namespace gum_tests
