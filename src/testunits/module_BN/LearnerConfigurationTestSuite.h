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
#include <tuple>
#include <vector>

#include <agrum/base/core/configuration/configuration.h>
#include <agrum/base/core/exceptions.h>
#include <agrum/BN/learning/BNLearner.h>
#include <agrum/BN/learning/FCI.h>
#include <agrum/BN/learning/PC.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {

  // The default parameters of the learners (significance level, weight of the priors, noise of EM,
  // parameters of the local search) come from the section `learning` of the configuration. The
  // values checked here are the ones that were written in the code before.
  struct LearnerConfigurationTestSuite {
    public:
    using State = std::vector< std::tuple< std::string, std::string, std::string > >;

    // the value of a row of BNLearner::state() ("" if there is no such row)
    static std::string stateValue(const State& state, const std::string& key) {
      for (const auto& row: state)
        if (std::get< 0 >(row) == key) return std::get< 1 >(row);
      return "";
    }

    static gum::learning::BNLearner< double > learner() {
      return gum::learning::BNLearner< double >(GET_RESSOURCES_PATH("csv/asia3.csv"));
    }
  };

  GUM_TEST(DefaultsAreTheHistoricalValues) {
    CHECK_EQ(gum::config().typed< double >("learning", "alpha"), 0.05);
    CHECK_EQ(gum::config().typed< double >("learning", "priorWeight"), 1.0);
    CHECK_EQ(gum::config().typed< double >("learning", "emNoise"), 0.1);
    CHECK_EQ(gum::config().typed< std::int64_t >("learning", "tabuListSize"), 100);
    CHECK_EQ(gum::config().typed< std::int64_t >("learning", "tabuNbDecrease"), 2);
  }

  GUM_TEST(AlphaComesFromTheConfiguration) {
    auto& conf = gum::Configuration::instance();

    CHECK_EQ(gum::learning::PC().alpha(), 0.05);
    CHECK_EQ(gum::learning::FCI().alpha(), 0.05);

    conf.push();
    conf.set("learning", "alpha", "0.2");
    CHECK_EQ(gum::learning::PC().alpha(), 0.2);
    CHECK_EQ(gum::learning::FCI().alpha(), 0.2);
    conf.pop();

    CHECK_EQ(gum::learning::PC().alpha(), 0.05);
  }

  GUM_TEST(PriorWeightComesFromTheConfiguration) {
    auto& conf = gum::Configuration::instance();
    auto  l    = learner();

    l.useSmoothingPrior();
    CHECK_EQ(stateValue(l.state(), "Prior weight"), "1.000000");

    conf.push();
    conf.set("learning", "priorWeight", "3");
    l.useBDeuPrior();
    CHECK_EQ(stateValue(l.state(), "Prior weight"), "3.000000");
    l.useSmoothingPrior();
    CHECK_EQ(stateValue(l.state(), "Prior weight"), "3.000000");
    l.useDirichletPrior(GET_RESSOURCES_PATH("csv/asia3.csv"));
    CHECK_EQ(stateValue(l.state(), "Prior weight"), "3.000000");

    // an explicit weight never uses the configuration
    l.useBDeuPrior(2.);
    CHECK_EQ(stateValue(l.state(), "Prior weight"), "2.000000");
    conf.pop();

    l.useBDeuPrior();
    CHECK_EQ(stateValue(l.state(), "Prior weight"), "1.000000");
  }

  GUM_TEST(TabuListParametersComeFromTheConfiguration) {
    auto& conf = gum::Configuration::instance();
    auto  l    = learner();

    // the row "Tabu list size" of the state shows the max number of decreasing changes
    l.useLocalSearchWithTabuList();
    CHECK_EQ(stateValue(l.state(), "Tabu list size"), "2");

    conf.push();
    conf.set("learning", "tabuNbDecrease", "5");
    l.useLocalSearchWithTabuList();
    CHECK_EQ(stateValue(l.state(), "Tabu list size"), "5");
    l.useLocalSearchWithTabuList(10);
    CHECK_EQ(stateValue(l.state(), "Tabu list size"), "5");
    l.useLocalSearchWithTabuList(10, 1);
    CHECK_EQ(stateValue(l.state(), "Tabu list size"), "1");
    conf.pop();

    l.useLocalSearchWithTabuList(10);
    CHECK_EQ(stateValue(l.state(), "Tabu list size"), "2");
  }

  GUM_TEST(EMNoiseComesFromTheConfiguration) {
    auto& conf = gum::Configuration::instance();
    auto  l    = learner();

    GUM_CHECK_ASSERT_THROWS_NOTHING(l.useEM(1e-3));
    GUM_CHECK_ASSERT_THROWS_NOTHING(l.useEMWithRateCriterion(1e-3));
    GUM_CHECK_ASSERT_THROWS_NOTHING(l.useEMWithDiffCriterion(1e-3));

    // a noise out of [0,1] is refused, by the configuration (domain of the key) as by the learner
    conf.push();
    CHECK_THROWS_AS(conf.set("learning", "emNoise", "2"), gum::InvalidArgument);
    CHECK_THROWS_AS(l.useEM(1e-3, 2.), gum::OutOfBounds);
    GUM_CHECK_ASSERT_THROWS_NOTHING(l.useEM(1e-3, 0.3));

    conf.set("learning", "emNoise", "0.5");
    GUM_CHECK_ASSERT_THROWS_NOTHING(l.useEM(1e-3));
    conf.pop();

    GUM_CHECK_ASSERT_THROWS_NOTHING(l.useEM(1e-3));
  }

  GUM_TEST(InvalidValuesAreRefused) {
    auto& conf = gum::Configuration::instance();
    auto  l    = learner();

    conf.push();
    CHECK_THROWS_AS(conf.set("learning", "priorWeight", "-1"), gum::InvalidArgument);
    CHECK_THROWS_AS(l.useBDeuPrior(-1.), gum::OutOfBounds);
    CHECK_THROWS_AS(conf.set("learning", "tabuListSize", "0"), gum::InvalidArgument);
    CHECK_THROWS_AS(conf.set("learning", "tabuNbDecrease", "-1"), gum::InvalidArgument);
    CHECK_THROWS_AS(conf.set("learning", "alpha", "1"), gum::InvalidArgument);
    GUM_CHECK_ASSERT_THROWS_NOTHING(l.useBDeuPrior(1.));
    conf.pop();

    GUM_CHECK_ASSERT_THROWS_NOTHING(l.useBDeuPrior());
  }
}   // namespace gum_tests
