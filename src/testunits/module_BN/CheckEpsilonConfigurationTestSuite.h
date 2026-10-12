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


#include <string>
#include <vector>

#include <agrum/base/core/configuration/configuration.h>
#include <agrum/BN/BayesNet.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {

  // The tolerances of IBayesNet::check() come from the configuration: the sum of a distribution can
  // differ from 1 by core.epsilonStandard, and the problem is a probable normalization problem if
  // the difference is smaller than core.epsilonCoarse.
  struct CheckEpsilonConfigurationTestSuite {
    public:
    // the distribution of a sums to 1 + excess
    static gum::BayesNet< double > build(double excess) {
      auto bn = gum::BayesNet< double >::fastPrototype("a->b");
      bn.cpt("a").fillWith(std::vector< double >{0.5, 0.5 + excess});
      return bn;
    }

    static bool hasNormalizationProblem(const std::vector< std::string >& comments) {
      return !comments.empty() && comments[0].find("normalization problem") != std::string::npos;
    }
  };

  GUM_TEST(CheckDefaultTolerances) {
    ConfigurationGuard guard;
    // the tolerance is 1e-6 and the limit of the normalization problems is 1e-2
    CHECK(CheckEpsilonConfigurationTestSuite::build(1e-7).check().empty());
    CHECK(CheckEpsilonConfigurationTestSuite::build(0.).check().empty());

    const auto small = CheckEpsilonConfigurationTestSuite::build(1e-3).check();
    REQUIRE_EQ(small.size(), std::size_t(1));
    CHECK(CheckEpsilonConfigurationTestSuite::hasNormalizationProblem(small));

    const auto big = CheckEpsilonConfigurationTestSuite::build(0.05).check();
    REQUIRE_EQ(big.size(), std::size_t(1));
    CHECK(!CheckEpsilonConfigurationTestSuite::hasNormalizationProblem(big));
  }

  GUM_TEST(CheckToleranceComesFromTheConfiguration) {
    auto& conf = gum::config();

    {
      ConfigurationGuard scoped;
      conf.set("core", "epsilonStandard", "1e-8");
      CHECK_EQ(CheckEpsilonConfigurationTestSuite::build(1e-7).check().size(), std::size_t(1));
      conf.set("core", "epsilonStandard", "1e-5");
      CHECK(CheckEpsilonConfigurationTestSuite::build(5e-6).check().empty());
    }

    CHECK(CheckEpsilonConfigurationTestSuite::build(1e-7).check().empty());
  }

  GUM_TEST(CheckNormalizationLimitComesFromTheConfiguration) {
    auto& conf = gum::config();

    ConfigurationGuard guard;
    conf.set("core", "epsilonCoarse", "0.1");
    CHECK(CheckEpsilonConfigurationTestSuite::hasNormalizationProblem(
        CheckEpsilonConfigurationTestSuite::build(0.05).check()));
    conf.set("core", "epsilonCoarse", "1e-3");
    CHECK(!CheckEpsilonConfigurationTestSuite::hasNormalizationProblem(
        CheckEpsilonConfigurationTestSuite::build(5e-3).check()));
  }
}   // namespace gum_tests
