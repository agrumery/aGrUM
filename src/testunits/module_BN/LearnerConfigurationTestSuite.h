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
#include <string>
#include <tuple>
#include <vector>

#include <agrum/base/core/configuration/configuration.h>
#include <agrum/base/core/exceptions.h>
#include <agrum/BN/learning/BNLearner.h>
#include <agrum/BN/learning/FCI.h>
#include <agrum/BN/learning/PC.h>

#include <agrum/base/core/utils_random.h>
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

    // the parameters learned by one iteration of EM from a database with missing values : with a
    // fixed seed, they only depend on the noise added to the initial CPTs
    template < typename NOISE >
    static gum::BayesNet< double > oneEMStep(NOISE useEM) {
      gum::initRandom(42);
      gum::learning::BNLearner< double > l(GET_RESSOURCES_PATH("csv/EM.csv"),
                                           std::vector< std::string >{"?"});
      gum::DAG                           dag;
      for (auto i = std::size_t(0); i < l.database().nbVariables(); ++i)
        dag.addNodeWithId(gum::NodeId(i));
      dag.addArc(gum::NodeId(1), gum::NodeId(0));
      dag.addArc(gum::NodeId(2), gum::NodeId(1));
      dag.addArc(gum::NodeId(3), gum::NodeId(2));

      useEM(l);
      l.useSmoothingPrior();   // some conditioning sets never appear in the database
      l.EMsetMaxIter(1);
      return l.learnParameters(dag, false);
    }

    // the largest difference between two parameters of two Bayesian networks
    static double maxDifference(const gum::BayesNet< double >& a,
                                const gum::BayesNet< double >& b) {
      double res = 0.;
      for (const auto node: a.nodes()) {
        const auto&        p = a.cpt(node);
        const auto&        q = b.cpt(node);
        gum::Instantiation i(p), j(q);
        for (i.setFirst(), j.setFirst(); !i.end(); ++i, ++j)
          res = std::max(res, std::abs(p.get(i) - q.get(j)));
      }
      return res;
    }
  };

  GUM_TEST(DefaultsAreTheHistoricalValues) {
    ConfigurationGuard guard;
    CHECK_EQ(gum::config().typed< double >("learning", "alpha"), 0.05);
    CHECK_EQ(gum::config().typed< double >("learning", "priorWeight"), 1.0);
    CHECK_EQ(gum::config().typed< double >("learning", "emNoise"), 0.1);
    CHECK_EQ(gum::config().typed< std::int64_t >("learning", "tabuListSize"), 100);
    CHECK_EQ(gum::config().typed< std::int64_t >("learning", "tabuNbDecrease"), 2);
    CHECK_EQ(gum::config().typed< std::string >("learning", "BNLearnerPreferredAlgo"), "MIIC");
  }

  GUM_TEST(AlphaComesFromTheConfiguration) {
    ConfigurationGuard guard;
    auto&              conf = gum::Configuration::instance();

    CHECK_EQ(gum::learning::PC().alpha(), 0.05);
    CHECK_EQ(gum::learning::FCI().alpha(), 0.05);

    conf.set("learning", "alpha", "0.2");
    CHECK_EQ(gum::learning::PC().alpha(), 0.2);
    CHECK_EQ(gum::learning::FCI().alpha(), 0.2);
  }

  GUM_TEST(AlphaIsTakenWhenTheLearnerIsConstructed) {
    ConfigurationGuard guard;
    auto&              conf = gum::Configuration::instance();

    auto learnPC = [](auto& l) {
      l.usePC();
      return l.learnPDAG().toString();
    };

    auto       reference   = learner();
    const auto withDefault = learnPC(reference);   // alpha = 0.05

    // a learner built before the change of the configuration keeps the alpha it was built with
    auto early = learner();
    conf.set("learning", "alpha", "0.99999");
    auto       late      = learner();
    const auto withLarge = learnPC(late);
    CHECK_NE(withLarge, withDefault);   // the two alphas do not give the same structure
    CHECK_EQ(learnPC(early), withDefault);

    // an alpha given to the learner wins over the configuration
    auto explicitAlpha = learner();
    explicitAlpha.usePC();
    explicitAlpha.setPCAlpha(0.05);
    CHECK_EQ(explicitAlpha.learnPDAG().toString(), withDefault);
  }

  GUM_TEST(CopyStateKeepsThePriorWeight) {
    const auto bn  = gum::BayesNet< double >::fastPrototype("A->B");
    auto       src = learner();
    src.useDirichletPrior(bn, 3.0);
    CHECK_EQ(stateValue(src.state(), "Prior weight"), "3.000000");

    auto dst = learner();
    dst.copyState(src);
    CHECK_EQ(stateValue(dst.state(), "Prior weight"), "3.000000");

    // the other kinds of prior
    src.useBDeuPrior(2.0);
    dst.copyState(src);
    CHECK_EQ(stateValue(dst.state(), "Prior weight"), "2.000000");
  }

  GUM_TEST(CopyAndMoveKeepTheAlgorithmOptions) {
    // PC : the alpha set on the original must be the one used by its copies
    auto reference = learner();
    reference.usePC();
    const auto withDefault = reference.learnPDAG().toString();

    auto pc = learner();
    pc.usePC();
    pc.setPCAlpha(0.99999);
    const auto withLarge = pc.learnPDAG().toString();
    CHECK_NE(withLarge, withDefault);

    auto copy = pc;
    CHECK_EQ(copy.learnPDAG().toString(), withLarge);
    auto assigned = learner();
    assigned      = pc;
    CHECK_EQ(assigned.learnPDAG().toString(), withLarge);
    auto moved = std::move(copy);
    CHECK_EQ(moved.learnPDAG().toString(), withLarge);
    auto moveAssigned = learner();
    moveAssigned      = std::move(assigned);
    CHECK_EQ(moveAssigned.learnPDAG().toString(), withLarge);

    // FCI : same thing
    auto fci = learner();
    fci.useFCI();
    fci.setFCIAlpha(0.99999);
    const auto fciLarge = fci.learnPAG().toString();
    auto       fciCopy  = fci;
    CHECK_EQ(fciCopy.learnPAG().toString(), fciLarge);

    // local search with tabu list : forbidding the additions of arcs gives an empty graph
    auto ghc = learner();
    ghc.useLocalSearchWithTabuList();
    ghc.allowArcAdditions(false);
    CHECK_EQ(ghc.learnDAG().arcs().size(), gum::Size(0));
    auto ghcCopy = ghc;
    CHECK_EQ(ghcCopy.learnDAG().arcs().size(), gum::Size(0));
    auto ghcMoved = std::move(ghcCopy);
    CHECK_EQ(ghcMoved.learnDAG().arcs().size(), gum::Size(0));
  }

  GUM_TEST(PriorWeightComesFromTheConfiguration) {
    auto& conf = gum::Configuration::instance();
    auto  l    = learner();

    ConfigurationGuard guard;
    l.useSmoothingPrior();
    CHECK_EQ(stateValue(l.state(), "Prior weight"), "1.000000");

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

    conf.reset();   // the defaults again
    l.useBDeuPrior();
    CHECK_EQ(stateValue(l.state(), "Prior weight"), "1.000000");
  }

  GUM_TEST(TabuListParametersComeFromTheConfiguration) {
    auto& conf = gum::Configuration::instance();
    auto  l    = learner();

    ConfigurationGuard guard;
    const auto         size = [&]() { return stateValue(l.state(), "Tabu list size"); };
    const auto         changes
        = [&]() { return stateValue(l.state(), "Max consecutive decreasing changes"); };

    l.useLocalSearchWithTabuList();
    CHECK_EQ(size(), "100");
    CHECK_EQ(changes(), "2");

    conf.set("learning", "tabuListSize", "40");
    conf.set("learning", "tabuNbDecrease", "5");
    l.useLocalSearchWithTabuList();
    CHECK_EQ(size(), "40");
    CHECK_EQ(changes(), "5");
    // only the size is given: the number of decreasing changes comes from the configuration
    l.useLocalSearchWithTabuList(10);
    CHECK_EQ(size(), "10");
    CHECK_EQ(changes(), "5");
    // explicit values never use the configuration
    l.useLocalSearchWithTabuList(20, 1);
    CHECK_EQ(size(), "20");
    CHECK_EQ(changes(), "1");

    conf.reset();   // the defaults again
    l.useLocalSearchWithTabuList(10);
    CHECK_EQ(size(), "10");
    CHECK_EQ(changes(), "2");
  }

  GUM_TEST(TabuListParametersAreCopiedWithTheState) {
    ConfigurationGuard guard;
    auto               l = learner();
    l.useLocalSearchWithTabuList(30, 4);

    auto other = learner();
    other.copyState(l);
    CHECK_EQ(stateValue(other.state(), "Tabu list size"), "30");
    CHECK_EQ(stateValue(other.state(), "Max consecutive decreasing changes"), "4");

    // a copy of the learner keeps them too
    auto copy = l;
    CHECK_EQ(stateValue(copy.state(), "Tabu list size"), "30");
    CHECK_EQ(stateValue(copy.state(), "Max consecutive decreasing changes"), "4");
  }

  GUM_TEST(DirichletPriorFromABayesNetWeightComesFromTheConfiguration) {
    auto& conf = gum::Configuration::instance();
    auto  l    = learner();
    auto  bn   = l.learnBN();

    ConfigurationGuard guard;
    l.useDirichletPrior(bn);
    CHECK_EQ(stateValue(l.state(), "Prior weight"), "1.000000");

    conf.set("learning", "priorWeight", "3");
    l.useDirichletPrior(bn);
    CHECK_EQ(stateValue(l.state(), "Prior weight"), "3.000000");
    // an explicit weight never uses the configuration
    l.useDirichletPrior(bn, 2.);
    CHECK_EQ(stateValue(l.state(), "Prior weight"), "2.000000");
    CHECK_THROWS_AS(l.useDirichletPrior(bn, -1.), gum::OutOfBounds);

    conf.reset();   // the defaults again
    l.useDirichletPrior(bn);
    CHECK_EQ(stateValue(l.state(), "Prior weight"), "1.000000");
  }

  GUM_TEST(EMNoiseIsAppliedToTheInitialCPTs) {
    ConfigurationGuard guard;
    auto&              conf = gum::Configuration::instance();

    const auto explicitNoise
        = [](double noise) { return [noise](auto& l) { l.useEM(1e-3, noise); }; };

    const auto none  = oneEMStep(explicitNoise(0.));
    const auto noisy = oneEMStep(explicitNoise(0.9));
    // the noise perturbs the initial CPTs, so the result of the first iteration changes
    CHECK_GT(maxDifference(none, noisy), 1e-3);
    // ... and nothing else is random (up to the order of the floating point operations of EM)
    CHECK_LT(maxDifference(noisy, oneEMStep(explicitNoise(0.9))), 1e-8);

    // the noise of the configuration is the one of the algorithm when none is given
    conf.set("learning", "emNoise", "0.9");
    CHECK_LT(maxDifference(noisy, oneEMStep([](auto& l) { l.useEM(1e-3); })), 1e-8);
    CHECK_LT(maxDifference(noisy, oneEMStep([](auto& l) { l.useEMWithRateCriterion(1e-3); })),
             1e-8);
    conf.set("learning", "emNoise", "0");
    CHECK_LT(maxDifference(none, oneEMStep([](auto& l) { l.useEM(1e-3); })), 1e-8);
  }

  GUM_TEST(EMNoiseComesFromTheConfiguration) {
    ConfigurationGuard guard;
    auto&              conf = gum::Configuration::instance();
    auto               l    = learner();

    GUM_CHECK_ASSERT_THROWS_NOTHING(l.useEM(1e-3));
    GUM_CHECK_ASSERT_THROWS_NOTHING(l.useEMWithRateCriterion(1e-3));
    GUM_CHECK_ASSERT_THROWS_NOTHING(l.useEMWithDiffCriterion(1e-3));

    // a noise out of [0,1] is refused, by the configuration (domain of the key) as by the learner
    CHECK_THROWS_AS(conf.set("learning", "emNoise", "2"), gum::InvalidArgument);
    CHECK_THROWS_AS(l.useEM(1e-3, 2.), gum::OutOfBounds);
    GUM_CHECK_ASSERT_THROWS_NOTHING(l.useEM(1e-3, 0.3));

    conf.set("learning", "emNoise", "0.5");
    GUM_CHECK_ASSERT_THROWS_NOTHING(l.useEM(1e-3));
  }

  GUM_TEST(PreferredAlgoComesFromTheConfiguration) {
    ConfigurationGuard guard;
    auto&              conf = gum::Configuration::instance();
    auto               l    = learner();
    const auto         algo = [&](const auto& x) { return stateValue(x.state(), "Algorithm"); };

    CHECK_EQ(algo(l), "MIIC");

    // a learner built before the change of the configuration keeps its algorithm
    conf.set("learning", "BNLearnerPreferredAlgo", "PC");
    CHECK_EQ(algo(l), "MIIC");

    // a learner built after the change takes the new one
    auto pc = learner();
    CHECK_EQ(algo(pc), "PC");
    CHECK(pc.isConstraintBased());

    conf.set("learning", "BNLearnerPreferredAlgo", "GreedyHillClimbing");
    auto ghc = learner();
    CHECK_EQ(algo(ghc), "Greedy Hill Climbing");
    CHECK(ghc.isScoreBased());

    // an algorithm selected explicitly wins over the configuration
    ghc.useMIIC();
    CHECK_EQ(algo(ghc), "MIIC");

    // copies and moves keep the algorithm of the original
    auto copy = pc;
    conf.set("learning", "BNLearnerPreferredAlgo", "FCI");
    CHECK_EQ(algo(copy), "PC");
    auto moved = std::move(copy);
    CHECK_EQ(algo(moved), "PC");
  }

  GUM_TEST(PreferredAlgoIsUsedToLearn) {
    ConfigurationGuard guard;
    auto&              conf = gum::Configuration::instance();

    auto reference = learner();
    reference.usePC();
    const auto expected = reference.learnPDAG().toString();

    conf.set("learning", "BNLearnerPreferredAlgo", "PC");
    auto l = learner();
    CHECK_EQ(l.learnPDAG().toString(), expected);

    // the options of the preferred algorithm can be set without selecting it explicitly
    GUM_CHECK_ASSERT_THROWS_NOTHING(l.setPCAlpha(0.05));
    CHECK_EQ(l.learnPDAG().toString(), expected);

    // but not the ones of another algorithm
    CHECK_THROWS_AS(l.setFCIAlpha(0.05), gum::OperationNotAllowed);
  }

  GUM_TEST(InvalidValuesAreRefused) {
    auto& conf = gum::Configuration::instance();
    auto  l    = learner();

    ConfigurationGuard guard;
    CHECK_THROWS_AS(conf.set("learning", "priorWeight", "-1"), gum::InvalidArgument);
    CHECK_THROWS_AS(l.useBDeuPrior(-1.), gum::OutOfBounds);
    CHECK_THROWS_AS(conf.set("learning", "tabuListSize", "0"), gum::InvalidArgument);
    CHECK_THROWS_AS(conf.set("learning", "tabuNbDecrease", "-1"), gum::InvalidArgument);
    CHECK_THROWS_AS(conf.set("learning", "alpha", "1"), gum::InvalidArgument);
    // K2 needs an order : it cannot be the preferred algorithm
    CHECK_THROWS_AS(conf.set("learning", "BNLearnerPreferredAlgo", "K2"), gum::InvalidArgument);
    CHECK_THROWS_AS(conf.set("learning", "BNLearnerPreferredAlgo", "foo"), gum::InvalidArgument);
    GUM_CHECK_ASSERT_THROWS_NOTHING(l.useBDeuPrior(1.));
    GUM_CHECK_ASSERT_THROWS_NOTHING(l.useBDeuPrior());
  }
}   // namespace gum_tests
