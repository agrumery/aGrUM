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

#include <agrum/base/core/configuration/configuration.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/database/BNDatabaseGenerator.h>
#include <agrum/BN/inference/importanceSampling.h>
#include <agrum/BN/inference/loopySamplingInference.h>
#include <agrum/BN/inference/MonteCarloSampling.h>
#include <agrum/BN/inference/weightedSampling.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

// Evidence with a null probability given to the samplers (Monte Carlo, Weighted, Importance and
// their loopy versions) and to the database generator: they must throw instead of looping forever.

namespace gum_tests {

  struct SamplingIncompatibleEvidenceTestSuite {
    public:
    // sets the CPT of node to the deterministic function f of the values of its parents A and B
    template < typename F >
    static void setDeterministic(gum::BayesNet< double >& bn, const std::string& node, F f) {
      const auto&        x   = bn.variable(node);
      const auto&        a   = bn.variable("A");
      const auto&        b   = bn.variable("B");
      const auto&        cpt = bn.cpt(node);
      gum::Instantiation I(cpt);
      for (I.setFirst(); !I.end(); I.inc())
        cpt.set(I, (I.val(x) == f(I.val(a), I.val(b))) ? 1.0 : 0.0);
    }

    // C = A AND B: the evidence {C=1, A=0} is detected by the pruning of the domains
    static gum::BayesNet< double > andBN() {
      auto bn = gum::BayesNet< double >::fastPrototype("A->C<-B");
      setDeterministic(bn, "C", [](gum::Idx a, gum::Idx b) { return a & b; });
      return bn;
    }

    // Y = A XOR B and Z = NOT(A XOR B): the evidence {Y=1, Z=1} has a null probability but each
    // CPT alone supports every value of A and B, so the pruning cannot detect it
    static gum::BayesNet< double > xorBN() {
      auto bn = gum::BayesNet< double >::fastPrototype("A->Y<-B;A->Z<-B");
      setDeterministic(bn, "Y", [](gum::Idx a, gum::Idx b) { return a ^ b; });
      setDeterministic(bn, "Z", [](gum::Idx a, gum::Idx b) { return 1 - (a ^ b); });
      return bn;
    }

    // the message of the IncompatibleEvidence thrown by f (empty if nothing is thrown)
    template < typename F >
    static std::string incompatibility(F f) {
      try {
        f();
      } catch (const gum::IncompatibleEvidence& e) { return e.errorContent(); }
      return "";
    }

    static bool isProved(const std::string& message) {
      return message.find("has a null probability") != std::string::npos;
    }

    static bool isRejectionBound(const std::string& message) {
      return message.find("consecutive draws rejected") != std::string::npos;
    }

    static bool isTimeLimit(const std::string& message) {
      return message.find("within the time limit") != std::string::npos;
    }

    // runs an inference of type INFERENCE with the given evidence and returns the message
    template < template < typename > class INFERENCE >
    static std::string infer(const gum::BayesNet< double >&                      bn,
                             const std::vector< std::pair< std::string, int > >& evidence,
                             double                                              maxTime = 0) {
      INFERENCE< double > inf(&bn);
      for (const auto& [name, val]: evidence)
        inf.addEvidence(name, val);
      if (maxTime > 0) inf.setMaxTime(maxTime);
      return incompatibility([&]() { inf.makeInference(); });
    }

    template < template < typename > class INFERENCE >
    static void checkProved() {
      const auto bn = andBN();
      CHECK(isProved(infer< INFERENCE >(bn, {{"C", 1}, {"A", 0}})));
    }

    template < template < typename > class INFERENCE >
    static void checkRejectionBound() {
      ConfigurationGuard guard;
      gum::config().set("sampling", "maxConsecutiveRejections", "1000");
      const auto bn = xorBN();
      CHECK(isRejectionBound(infer< INFERENCE >(bn, {{"Y", 1}, {"Z", 1}})));
    }

    template < template < typename > class INFERENCE >
    static void checkTimeLimit() {
      ConfigurationGuard guard;
      gum::config().set("sampling", "maxConsecutiveRejections", "1000000000000");
      const auto bn = xorBN();
      CHECK(isTimeLimit(infer< INFERENCE >(bn, {{"Y", 1}, {"Z", 1}}, 0.2)));
    }

    template < template < typename > class INFERENCE >
    static void checkCompatible() {
      // a compatible evidence (null probabilities in the CPTs) is still accepted
      const auto          bn = xorBN();
      INFERENCE< double > inf(&bn);
      inf.addEvidence("Y", 1);
      inf.addEvidence("Z", 0);
      inf.setEpsilon(1e-2);
      GUM_CHECK_ASSERT_THROWS_NOTHING(inf.makeInference());
    }

    template < typename GUM_SCALAR >
    using LoopyMonteCarlo = gum::LoopySamplingInference< GUM_SCALAR, gum::MonteCarloSampling >;
    template < typename GUM_SCALAR >
    using LoopyWeighted = gum::LoopySamplingInference< GUM_SCALAR, gum::WeightedSampling >;
    template < typename GUM_SCALAR >
    using LoopyImportance = gum::LoopySamplingInference< GUM_SCALAR, gum::ImportanceSampling >;
  };

  GUM_TEST(ProvedIncompatibility) {
    SamplingIncompatibleEvidenceTestSuite::checkProved< gum::MonteCarloSampling >();
    SamplingIncompatibleEvidenceTestSuite::checkProved< gum::WeightedSampling >();
    SamplingIncompatibleEvidenceTestSuite::checkProved< gum::ImportanceSampling >();
    SamplingIncompatibleEvidenceTestSuite::checkProved<
        SamplingIncompatibleEvidenceTestSuite::LoopyMonteCarlo >();
    SamplingIncompatibleEvidenceTestSuite::checkProved<
        SamplingIncompatibleEvidenceTestSuite::LoopyWeighted >();
    SamplingIncompatibleEvidenceTestSuite::checkProved<
        SamplingIncompatibleEvidenceTestSuite::LoopyImportance >();
  }

  GUM_TEST(RejectionBound) {
    SamplingIncompatibleEvidenceTestSuite::checkRejectionBound< gum::MonteCarloSampling >();
    SamplingIncompatibleEvidenceTestSuite::checkRejectionBound< gum::WeightedSampling >();
    SamplingIncompatibleEvidenceTestSuite::checkRejectionBound< gum::ImportanceSampling >();
  }

  GUM_TEST(TimeLimitDuringRejections) {
    SamplingIncompatibleEvidenceTestSuite::checkTimeLimit< gum::MonteCarloSampling >();
    SamplingIncompatibleEvidenceTestSuite::checkTimeLimit< gum::WeightedSampling >();
    SamplingIncompatibleEvidenceTestSuite::checkTimeLimit< gum::ImportanceSampling >();
  }

  GUM_TEST(CompatibleEvidenceWithNullProbabilities) {
    SamplingIncompatibleEvidenceTestSuite::checkCompatible< gum::MonteCarloSampling >();
    SamplingIncompatibleEvidenceTestSuite::checkCompatible< gum::WeightedSampling >();
    SamplingIncompatibleEvidenceTestSuite::checkCompatible< gum::ImportanceSampling >();
  }

  GUM_TEST(InferenceAfterIncompatibility) {
    // after an incompatible evidence, the same engine runs again with a compatible one
    const auto                      bn = SamplingIncompatibleEvidenceTestSuite::andBN();
    gum::WeightedSampling< double > inf(&bn);
    inf.addEvidence("C", 1);
    inf.addEvidence("A", 0);
    CHECK_THROWS_AS(inf.makeInference(), const gum::IncompatibleEvidence&);
    inf.chgEvidence("A", 1);
    inf.setEpsilon(1e-2);
    GUM_CHECK_ASSERT_THROWS_NOTHING(inf.makeInference());
    gum::Instantiation J(inf.posterior("B"));
    J.chgVal(bn.variable("B"), 1);
    CHECK_EQ(inf.posterior("B")[J], doctest::Approx(1.0));
  }

  GUM_TEST(DatabaseGeneratorProvedIncompatibility) {
    const auto bn = SamplingIncompatibleEvidenceTestSuite::andBN();
    gum::learning::BNDatabaseGenerator< double > gen(bn);
    gum::Instantiation                           evs;
    evs.add(bn.variable("C"));
    evs.add(bn.variable("A"));
    evs.chgVal(bn.variable("C"), 1);
    evs.chgVal(bn.variable("A"), 0);
    CHECK(SamplingIncompatibleEvidenceTestSuite::isProved(
        SamplingIncompatibleEvidenceTestSuite::incompatibility(
            [&]() { gen.drawSamples(10, evs, 0); })));
    CHECK_THROWS_AS(gen.toDatabaseTable(), const gum::OperationNotAllowed&);
  }

  GUM_TEST(DatabaseGeneratorRejectionBound) {
    ConfigurationGuard guard;
    gum::config().set("sampling", "maxConsecutiveRejections", "1000");
    const auto bn = SamplingIncompatibleEvidenceTestSuite::xorBN();
    gum::learning::BNDatabaseGenerator< double > gen(bn);
    gum::Instantiation                           evs;
    evs.add(bn.variable("Y"));
    evs.add(bn.variable("Z"));
    evs.chgVal(bn.variable("Y"), 1);
    evs.chgVal(bn.variable("Z"), 1);
    // without timeout, the generation used to loop forever
    CHECK(SamplingIncompatibleEvidenceTestSuite::isRejectionBound(
        SamplingIncompatibleEvidenceTestSuite::incompatibility(
            [&]() { gen.drawSamples(10, evs, 0); })));
  }

  GUM_TEST(DatabaseGeneratorTimeLimit) {
    ConfigurationGuard guard;
    gum::config().set("sampling", "maxConsecutiveRejections", "1000000000000");
    const auto bn = SamplingIncompatibleEvidenceTestSuite::xorBN();
    gum::learning::BNDatabaseGenerator< double > gen(bn);
    gum::Instantiation                           evs;
    evs.add(bn.variable("Y"));
    evs.add(bn.variable("Z"));
    evs.chgVal(bn.variable("Y"), 1);
    evs.chgVal(bn.variable("Z"), 1);
    // used to return an empty database and a log2-likelihood of 0
    CHECK(SamplingIncompatibleEvidenceTestSuite::isTimeLimit(
        SamplingIncompatibleEvidenceTestSuite::incompatibility(
            [&]() { gen.drawSamples(10, evs, 1); })));
  }

}   // namespace gum_tests
