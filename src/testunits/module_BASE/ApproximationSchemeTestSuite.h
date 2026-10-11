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


#include <agrum/base/core/approximations/approximationScheme.h>
#include <agrum/base/core/configuration/configuration.h>
#include <agrum/base/core/exceptions.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {

  struct ApproximationSchemeTestSuite {
    public:
    // the values used before the parameters were read from the configuration
    static void checkDefaults(const gum::ApproximationScheme& scheme) {
      CHECK_EQ(scheme.epsilon(), 5e-2);
      CHECK(scheme.isEnabledEpsilon());
      CHECK_EQ(scheme.minEpsilonRate(), 1e-2);
      CHECK(scheme.isEnabledMinEpsilonRate());
      CHECK_EQ(scheme.maxIter(), gum::Size(10000));
      CHECK(scheme.isEnabledMaxIter());
      CHECK_EQ(scheme.maxTime(), 1.);
      CHECK(!scheme.isEnabledMaxTime());
      CHECK_EQ(scheme.periodSize(), gum::Size(1));
      CHECK(!scheme.verbosity());
    }
  };

  GUM_TEST(DefaultsComeFromTheConfiguration) {
    checkDefaults(gum::ApproximationScheme());
    checkDefaults(gum::ApproximationScheme(false));
  }

  GUM_TEST(ExplicitVerbosityNeverUsesTheConfiguration) {
    auto& conf = gum::Configuration::instance();

    conf.push();
    CHECK(gum::ApproximationScheme(true).verbosity());
    conf.set("approximation", "verbosity", "True");
    CHECK(gum::ApproximationScheme().verbosity());
    CHECK(!gum::ApproximationScheme(false).verbosity());
    CHECK(gum::ApproximationScheme(true).verbosity());
    conf.pop();

    CHECK(!gum::ApproximationScheme().verbosity());
  }

  GUM_TEST(ParametersAreReadFromTheConfiguration) {
    auto& conf = gum::Configuration::instance();

    conf.push();
    conf.set("approximation", "epsilon", "0.5");
    conf.set("approximation", "minEpsilonRate", "0.25");
    conf.set("approximation", "maxIter", "7");
    conf.set("approximation", "maxTime", "3.5");
    conf.set("approximation", "periodSize", "3");
    conf.set("approximation", "verbosity", "True");

    const gum::ApproximationScheme scheme;
    CHECK_EQ(scheme.epsilon(), 0.5);
    CHECK_EQ(scheme.minEpsilonRate(), 0.25);
    CHECK_EQ(scheme.maxIter(), gum::Size(7));
    CHECK_EQ(scheme.maxTime(), 3.5);
    CHECK_EQ(scheme.periodSize(), gum::Size(3));
    CHECK(scheme.verbosity());
    // the timeout is read but the criterion stays disabled until the user enables it
    CHECK(!scheme.isEnabledMaxTime());
    conf.pop();

    checkDefaults(gum::ApproximationScheme());
  }

  GUM_TEST(ACopyKeepsTheParametersOfTheSource) {
    auto& conf = gum::Configuration::instance();

    conf.push();
    conf.set("approximation", "maxIter", "7");
    gum::ApproximationScheme source;
    conf.pop();

    const gum::ApproximationScheme copy(source);
    CHECK_EQ(copy.maxIter(), gum::Size(7));
  }

  GUM_TEST(InvalidParametersAreRefused) {
    auto& conf = gum::Configuration::instance();

    // an invalid value is refused by the configuration itself (domain of the key), so that no
    // scheme can ever be built from it
    conf.push();
    CHECK_THROWS_AS(conf.set("approximation", "maxIter", "0"), gum::InvalidArgument);
    CHECK_THROWS_AS(conf.set("approximation", "epsilon", "-1"), gum::InvalidArgument);
    CHECK_THROWS_AS(conf.set("approximation", "periodSize", "0"), gum::InvalidArgument);
    CHECK_THROWS_AS(conf.set("approximation", "maxTime", "0"), gum::InvalidArgument);
    CHECK_THROWS_AS(conf.set("approximation", "burnIn", "-1"), gum::InvalidArgument);
    GUM_CHECK_ASSERT_THROWS_NOTHING(gum::ApproximationScheme());
    conf.pop();

    checkDefaults(gum::ApproximationScheme());
  }
}   // namespace gum_tests
