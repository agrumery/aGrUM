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
 *   useful, but WITHOUT ANY KIND, EXPRESS OR IMPLIED,                      *
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
#include <map>
#include <string>
#include <tuple>
#include <vector>

#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/KTBN/KTBN.h>
#include <agrum/KTBN/inference/KTBNInference.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>


namespace gum_tests {

  // Correctness of the forward interventional engine, judged against an
  // INDEPENDENT path: the KTBN unrolled to a plain BayesNet, do-mutilated
  // (intervened nodes: parents cut, CPT set to a point mass), then solved by
  // LazyPropagation. The forward roll and the join tree share no code, so
  // agreement is meaningful -- not a tautology. CPT values are irrelevant
  // (both sides read the same model), so cases use generateCPTs() and never
  // assert a specific number, only engine == oracle.
  struct KTBNInferenceTestSuite {
    static constexpr int    AT  = gum::KTBN< double >::ATEMPORAL;
    static constexpr double TOL = 1e-9;

    using Do = std::tuple< std::string, int, gum::Idx >;   // do(base[slice]=value)
    using Ev = std::tuple< std::string, int, gum::Idx >;   // observe base[slice]=value

    static std::string nodeName(const std::string& base, int slice) {
      return slice == AT ? base : base + "[" + std::to_string(slice) + "]";
    }

    // A 1-axis marginal as a plain probability vector. Decouples from the
    // Tensor's variable lifetime -- a posterior (even a copy) shares the engine-
    // owned variable and is invalidated by the next makeInference(); the oracle's
    // BayesNet likewise dies before we compare.
    static std::vector< double > toVector(const gum::Tensor< double >& t) {
      std::vector< double > v(t.variable(0).domainSize());
      for (gum::Idx i = 0; i < v.size(); ++i) {
        gum::Instantiation I(t);
        I.chgVal(t.variable(0), i);
        v[i] = t[I];
      }
      return v;
    }

    // Marginals of the do-mutilated unrolled BN, keyed by node name. Returned as
    // plain vectors: the local BayesNet dies here, and a Tensor does not own its
    // variables, so returning tensors would dangle.
    static std::map< std::string, std::vector< double > >
        oracleMarginals(const gum::KTBN< double >& ktbn,
                        const std::vector< Do >&   dos,
                        const std::vector< Ev >&   evs,
                        int                        T,
                        double*                    outObservationProba = nullptr) {
      gum::BayesNet< double > bn = ktbn.unroll(static_cast< gum::Size >(T));

      // an intervention is surgery on the graph: cut the node from its causes
      for (const auto& [base, slice, value]: dos) {
        const std::string          n = nodeName(base, slice);
        std::vector< std::string > parents;   // snapshot before mutating the graph
        for (const gum::NodeId p: bn.parents(n)) parents.push_back(bn.variable(p).name());
        for (const auto& p: parents) bn.eraseArc(p, n);
        bn.cpt(n).fillWith(gum::Tensor< double >::deterministicTensor(bn.variable(n), value));
      }

      gum::LazyPropagation< double > lp(&bn);
      // an observation is plain conditioning on the (possibly mutilated) net
      for (const auto& [base, slice, value]: evs) lp.addEvidence(nodeName(base, slice), value);

      std::map< std::string, std::vector< double > > out;
      for (const gum::NodeId n: bn.nodes())
        out.emplace(bn.variable(n).name(), toVector(lp.posterior(n)));
      if (outObservationProba != nullptr) *outObservationProba = lp.evidenceProbability();
      return out;
    }

    // CHECK an engine marginal against an oracle probability vector, cell by cell.
    static void checkMarginal(const gum::Tensor< double >& got,
                              const std::vector< double >& expected,
                              const std::string&           name) {
      INFO("marginal of " << name);
      for (gum::Idx i = 0; i < expected.size(); ++i) {
        gum::Instantiation I(got);
        I.chgVal(got.variable(0), i);
        CHECK(std::fabs(got[I] - expected[i]) < TOL);
      }
    }

    // Run the engine and compare every targeted marginal at every slice to the
    // oracle. Empty `targets` means default-all-targets.
    static void checkAgainstOracle(const gum::KTBN< double >&        ktbn,
                                   const std::vector< Do >&          dos,
                                   const std::vector< std::string >& targets,
                                   int                               T) {
      checkAgainstOracle(ktbn, dos, {}, targets, T);
    }

    // Same, with observations. The engine and the oracle share no code: the
    // engine rolls the interface algorithm over k-slice windows, the oracle
    // unrolls to a flat BayesNet and calls LazyPropagation, so agreement on
    // smoothed posteriors is meaningful and not a tautology.
    static void checkAgainstOracle(const gum::KTBN< double >&        ktbn,
                                   const std::vector< Do >&          dos,
                                   const std::vector< Ev >&          evs,
                                   const std::vector< std::string >& targets,
                                   int                               T) {
      gum::KTBNInference< double > ie(&ktbn);
      for (const auto& [b, s, v]: dos) ie.addIntervention(b, s, v);
      for (const auto& [b, s, v]: evs) ie.addObservation(b, s, v);
      for (const auto& t: targets) ie.addTarget(t);
      ie.makeInference(static_cast< gum::Size >(T));

      double     pe     = 1.0;
      const auto oracle = oracleMarginals(ktbn, dos, evs, T, &pe);
      const auto wanted = [&](const std::string& b) {
        return targets.empty() || std::find(targets.begin(), targets.end(), b) != targets.end();
      };

      for (const auto& b: ktbn.temporalVarNames())
        if (wanted(b))
          for (int t = 0; t < T; ++t)
            checkMarginal(ie.posterior(b, t), oracle.at(nodeName(b, t)), nodeName(b, t));
      for (const auto& b: ktbn.atemporalVarNames())
        if (wanted(b)) checkMarginal(ie.posterior(b, AT), oracle.at(b), b);

      INFO("P(observation)");
      CHECK(std::fabs(ie.observationProbability() - pe) < 1e-9);
    }

    // -----------------------------------------------------------------------
    // Structural correctness (each also probes one elimination-schedule path).
    // -----------------------------------------------------------------------

    // All-lags ring stress: P depends on itself at every lag 1..k-1 at once, so
    // all k ring slots are simultaneously live -- the tightest slice%k test.

    // k=1: no initial slices, no lag; the transition schedule eliminates slice t at t.

    // Intra-slice arc with adversarial naming: Z sorts AFTER A but is A's same-
    // slice parent, so alphabetical fold order != topological. Guards snapshot-
    // after-full-fold (and pre-places the interleaving regression).

    // Deep reach-back: Y depends on X at lag k-1, so an initial X slice must
    // survive to iteration (k-2)+(k-1). Guards the past-k-1 early-death keys.

    // Barren temporal leaf (maxLag 0) + unused atemporal (purge path).

    // -----------------------------------------------------------------------
    // do-semantics: an intervention is surgery, not conditioning.
    // -----------------------------------------------------------------------

    // -----------------------------------------------------------------------
    // Horizon self-consistency: a slice's marginal is independent of how far
    // the horizon extends past it. Covers T<k / T=1, which unroll cannot reach.
    // -----------------------------------------------------------------------

    // -----------------------------------------------------------------------
    // Target modes and the default-all-targets fallback.
    // -----------------------------------------------------------------------

    // -----------------------------------------------------------------------
    // Exception contract of the public API.
    // -----------------------------------------------------------------------

    // -----------------------------------------------------------------------
    // Name API equivalence, idempotence, lazy re-run, and horizon growth.
    // -----------------------------------------------------------------------

    // Wider model (more simultaneously-live variables) to sanity check
    // moralization/triangulation on a less trivial junction-tree treewidth.
  };

  GUM_TEST(RingStressAllLags) {
    gum::initRandom(1);
    gum::KTBN< double > m(3);
    m.addTemporal("P", 2);
    m.addArc("P", 0, "P", 1);
    m.addArc("P", 1, "P", 2);
    m.addArc("P", 0, "P", 2);
    m.generateCPTs();
    checkAgainstOracle(m, {}, {}, 9);
    checkAgainstOracle(m, {{"P", 4, 1}}, {}, 9);   // do() on a transition slice
  }

  GUM_TEST(OrderOne) {
    gum::initRandom(2);
    gum::KTBN< double > m(1);
    m.addTemporal("P", 3);
    m.addAtemporal("C", 2);
    m.addArc("C", AT, "P", 0);
    m.generateCPTs();
    checkAgainstOracle(m, {}, {}, 5);
    checkAgainstOracle(m, {{"C", AT, 1}}, {}, 5);
  }

  GUM_TEST(IntraSliceArc) {
    gum::initRandom(3);
    gum::KTBN< double > m(2);
    m.addTemporal("A", 2);
    m.addTemporal("Z", 2);
    m.addArc("A", 0, "A", 1);
    m.addArc("Z", 0, "Z", 1);
    m.addArc("Z", 1, "A", 1);
    m.generateCPTs();
    checkAgainstOracle(m, {}, {}, 8);
  }

  GUM_TEST(DeepReachBack) {
    gum::initRandom(4);
    gum::KTBN< double > m(4);
    m.addTemporal("X", 2);
    m.addTemporal("Y", 2);
    for (int s = 1; s <= 3; ++s) m.addArc("X", s - 1, "X", s);
    for (int s = 1; s <= 3; ++s) m.addArc("Y", s - 1, "Y", s);
    m.addArc("X", 0, "Y", 3);
    m.generateCPTs();
    checkAgainstOracle(m, {}, {}, 12);
  }

  GUM_TEST(BarrenAndAtemporalPurge) {
    gum::initRandom(5);
    gum::KTBN< double > m(3);
    m.addTemporal("Q", 2);
    m.addTemporal("R", 2);   // barren: never a parent
    m.addAtemporal("W", 2);  // unused: never a parent
    m.addArc("Q", 0, "Q", 1);
    m.addArc("Q", 1, "Q", 2);
    m.addArc("Q", 0, "R", 2);
    m.generateCPTs();
    checkAgainstOracle(m, {}, {}, 9);
  }

  GUM_TEST(InterventionSemantics) {
    gum::initRandom(6);
    gum::KTBN< double > m(2);
    m.addTemporal("X", 2);
    m.addArc("X", 0, "X", 1);
    m.generateCPTs();

    gum::KTBNInference< double > base(&m);
    base.makeInference(3);
    const std::vector< double > priorX0 = toVector(base.posterior("X", 0));

    gum::KTBNInference< double > cut(&m);
    cut.addIntervention("X", 1, 0);
    cut.makeInference(3);

    // upstream of the intervention is UNCHANGED (conditioning would move it)
    checkMarginal(cut.posterior("X", 0), priorX0, "X[0] upstream of do(X[1])");

    // the intervened node is a point mass
    checkMarginal(cut.posterior("X", 1), {1.0, 0.0}, "X[1] forced to 0");

    // and the whole trajectory still matches the mutilated-unroll oracle
    checkAgainstOracle(m, {{"X", 1, 0}}, {}, 3);
  }

  GUM_TEST(HorizonConsistency) {
    gum::initRandom(7);
    gum::KTBN< double > m(3);
    m.addTemporal("X", 2);
    m.addAtemporal("C", 2);
    m.addArc("X", 0, "X", 1);
    m.addArc("X", 1, "X", 2);
    m.addArc("C", AT, "X", 2);
    m.generateCPTs();

    gum::KTBNInference< double > big(&m);
    big.makeInference(8);
    std::vector< std::vector< double > > ref;
    for (int t = 0; t < 8; ++t) ref.push_back(toVector(big.posterior("X", t)));

    for (int T = 1; T <= 8; ++T) {
      gum::KTBNInference< double > ie(&m);
      ie.makeInference(static_cast< gum::Size >(T));
      checkMarginal(ie.posterior("X", T - 1),
                    ref[static_cast< std::size_t >(T - 1)],
                    "X[" + std::to_string(T - 1) + "] at horizon " + std::to_string(T));
    }
  }

  GUM_TEST(TargetModes) {
    gum::initRandom(8);
    gum::KTBN< double > m(2);
    m.addTemporal("X", 2);
    m.addTemporal("Y", 2);
    m.addArc("X", 0, "X", 1);
    m.addArc("Y", 0, "Y", 1);
    m.generateCPTs();

    gum::KTBNInference< double > ie(&m);
    CHECK_FALSE(ie.isInTargetMode());   // default: every base is a target

    ie.addTarget("X");
    CHECK(ie.isInTargetMode());
    CHECK(ie.isTarget("X"));
    CHECK_FALSE(ie.isTarget("Y"));

    ie.makeInference(4);
    CHECK_THROWS_AS(ie.posterior("Y", 0), const gum::UndefinedElement&);   // not a target

    ie.eraseTarget("X");
    CHECK_FALSE(ie.isInTargetMode());   // last target removed -> default-all restored
  }

  GUM_TEST(Exceptions) {
    gum::initRandom(9);
    gum::KTBN< double > m(2);
    m.addTemporal("X", 2);
    m.addAtemporal("C", 2);
    m.addArc("X", 0, "X", 1);
    m.generateCPTs();

    CHECK_THROWS_AS(gum::KTBNInference< double > ie(nullptr), const gum::InvalidArgument&);

    {
      gum::KTBNInference< double > ie(&m);
      CHECK_THROWS_AS(ie.makeInference(0), const gum::InvalidArgument&);
      CHECK_THROWS_AS(ie.addTarget("nope"), const gum::NotFound&);
      // an out-of-domain value is an OutOfBounds, as in KTBN::fillCPT
      CHECK_THROWS_AS(ie.addIntervention("X", 0, 99), const gum::OutOfBounds&);
      CHECK_THROWS_AS(ie.posterior("X", 0), const gum::OperationNotAllowed&);   // before run
    }
    {
      gum::KTBNInference< double > ie(&m);
      ie.makeInference(4);
      CHECK_THROWS_AS(ie.posterior("X", 4), const gum::OutOfBounds&);   // slice >= horizon
    }
  }

  GUM_TEST(NameApiAndStaleness) {
    gum::initRandom(10);
    gum::KTBN< double > m(2);
    m.addTemporal("X", 2);
    m.addArc("X", 0, "X", 1);
    m.generateCPTs();

    gum::KTBNInference< double > ie(&m);
    ie.makeInference(5);

    // posterior("X", 2) == posterior("X[2]"): both live in the same run
    GUM_CHECK_TENSOR_ALMOST_EQUALS_DELTA(ie.posterior("X", 2), ie.posterior("X[2]"), TOL);

    // snapshot slice 3 as plain values BEFORE any re-run: a posterior Tensor
    // shares the engine-owned variable and is invalidated by the next
    // makeInference(), so it cannot be held across one (even as a copy).
    const std::vector< double > s3 = toVector(ie.posterior("X", 3));

    ie.makeInference(5);   // idempotence: same horizon reproduces the value
    checkMarginal(ie.posterior("X", 3), s3, "X[3] idempotent re-run");

    ie.makeInference(9);   // grow horizon: old slice stable, new slice reachable
    checkMarginal(ie.posterior("X", 3), s3, "X[3] after horizon growth");
    CHECK_NOTHROW(ie.posterior("X", 8));

    // a state change invalidates the cache -> next query re-runs (must not throw)
    ie.addIntervention("X", 1, 0);
    CHECK_NOTHROW(ie.posterior("X", 4));
  }

  GUM_TEST(InterventionByLabelAndBatch) {
    // an intervention value may be an index or a modality label, and several
    // interventions may be recorded in one batched call
    gum::KTBN< double > m(2);
    m.addTemporal(gum::LabelizedVariable("X", "", {"no", "yes"}));
    m.addTemporal(gum::LabelizedVariable("Z", "", {"off", "on"}));
    m.addAtemporal(gum::LabelizedVariable("C", "", {"low", "high"}));
    m.addArc("X", 0, "X", 1);
    m.addArc("Z", 0, "Z", 1);
    m.addArc("C", AT, "X", 1);
    gum::initRandom(5);
    m.generateCPTs();

    // label form == index form
    gum::KTBNInference< double > byIndex(&m), byLabel(&m);
    byIndex.addIntervention("X", 0, 1);
    byLabel.addIntervention("X", 0, "yes");
    byIndex.makeInference(4);
    byLabel.makeInference(4);
    GUM_CHECK_TENSOR_ALMOST_EQUALS_DELTA(byLabel.posterior("X", 2), byIndex.posterior("X", 2), TOL);

    // the batch form, mixing key styles and value styles, equals the one-by-one form
    gum::KTBNInference< double > oneByOne(&m), batched(&m);
    oneByOne.addIntervention("X", 0, 1);
    oneByOne.addIntervention("Z", 0, 0);
    oneByOne.addIntervention("C", AT, 1);
    batched.addIntervention(
        {{"X[0]", "yes"}, {std::pair< std::string, int >{"Z", 0}, 0}, {"C", "high"}});
    CHECK(batched.hasIntervention("X", 0));
    CHECK(batched.hasIntervention("Z", 0));
    CHECK(batched.hasIntervention("C", AT));
    oneByOne.makeInference(4);
    batched.makeInference(4);
    GUM_CHECK_TENSOR_ALMOST_EQUALS_DELTA(batched.posterior("X", 3),
                                         oneByOne.posterior("X", 3),
                                         TOL);

    // within a batch, the last value for a repeated node wins
    gum::KTBNInference< double > repeated(&m), last(&m);
    repeated.addIntervention({{"X[0]", 0}, {"X[0]", "yes"}});
    last.addIntervention("X", 0, 1);
    repeated.makeInference(3);
    last.makeInference(3);
    GUM_CHECK_TENSOR_ALMOST_EQUALS_DELTA(repeated.posterior("X", 2), last.posterior("X", 2), TOL);

    // a bad entry aborts the whole batch: nothing is recorded
    gum::KTBNInference< double > atomic(&m);
    CHECK_THROWS_AS(atomic.addIntervention({{"X[0]", 0}, {"Z[0]", "nope"}}),
                    const gum::OutOfBounds&);
    CHECK_FALSE(atomic.hasIntervention("X", 0));
    CHECK_FALSE(atomic.hasIntervention("Z", 0));

    CHECK_THROWS_AS(byLabel.addIntervention("X", 0, "maybe"), const gum::OutOfBounds&);
    CHECK_THROWS_AS(atomic.addIntervention({{"nope", 0}}), const gum::NotFound&);
  }

  GUM_TEST(WiderModel) {
    gum::initRandom(11);
    gum::KTBN< double > m(3);
    m.addTemporal("A", 2);
    m.addTemporal("B", 2);
    m.addTemporal("C", 2);
    m.addAtemporal("W", 2);
    m.addArc("A", 0, "A", 1);
    m.addArc("A", 1, "A", 2);
    m.addArc("B", 0, "B", 1);
    m.addArc("B", 1, "B", 2);
    m.addArc("A", 0, "B", 1);
    m.addArc("B", 0, "C", 1);
    m.addArc("C", 0, "C", 1);
    m.addArc("C", 1, "C", 2);
    m.addArc("W", AT, "A", 0);
    m.addArc("W", AT, "C", 2);
    m.generateCPTs();
    checkAgainstOracle(m, {}, {}, 10);
    checkAgainstOracle(m, {{"B", 3, 0}}, {}, 10);
  }

  // -----------------------------------------------------------------------
  // Observations. Unlike an intervention, an observation must revise the WHOLE
  // network -- earlier slices, later slices and the atemporal layer -- so these
  // exercise the backward pass that pure do()-queries never reach.
  // -----------------------------------------------------------------------

  GUM_TEST(ObservationSmoothingBackwards) {
    // an observation at the last slice must revise every earlier slice
    gum::initRandom(20);
    gum::KTBN< double > m(2);
    m.addTemporal("X", 3);
    m.addTemporal("Y", 2);
    m.addArc("X", 0, "X", 1);
    m.addArc("X", 1, "Y", 1);
    m.generateCPTs();
    checkAgainstOracle(m, {}, {{"Y", 6, 1}}, {}, 7);
    checkAgainstOracle(m, {}, {{"X", 0, 2}, {"Y", 6, 0}}, {}, 7);
  }

  GUM_TEST(ObservationRevisesAtemporal) {
    // the atemporal layer is the one thing that couples the whole horizon:
    // observing a temporal node late must move P(C).
    gum::initRandom(21);
    gum::KTBN< double > m(2);
    m.addTemporal("X", 2);
    m.addAtemporal("C", 3);
    m.addArc("C", AT, "X", 0);
    m.addArc("C", AT, "X", 1);
    m.addArc("X", 0, "X", 1);
    m.generateCPTs();
    checkAgainstOracle(m, {}, {{"X", 5, 1}}, {}, 6);
    checkAgainstOracle(m, {}, {{"X", 1, 0}, {"X", 5, 1}}, {}, 6);
  }

  GUM_TEST(ObservationAndInterventionTogether) {
    // both at once: conditioning flows both ways, surgery only downstream
    gum::initRandom(22);
    gum::KTBN< double > m(3);
    m.addTemporal("A", 2);
    m.addTemporal("B", 3);
    m.addAtemporal("W", 2);
    m.addArc("A", 0, "A", 1);
    m.addArc("A", 1, "A", 2);
    m.addArc("A", 0, "B", 2);
    m.addArc("B", 1, "B", 2);
    m.addArc("W", AT, "A", 0);
    m.generateCPTs();
    checkAgainstOracle(m, {{"A", 4, 1}}, {{"B", 7, 2}}, {}, 9);
    checkAgainstOracle(m, {{"B", 3, 0}}, {{"A", 8, 1}, {"B", 6, 1}}, {}, 9);
  }

  GUM_TEST(ObservationDeepReachBack) {
    // order 4: the interface spans three slices, so a late observation has to
    // travel back through several windows to reach slice 0.
    gum::initRandom(23);
    gum::KTBN< double > m(4);
    m.addTemporal("X", 2);
    m.addTemporal("Y", 2);
    for (int s = 1; s <= 3; ++s) m.addArc("X", s - 1, "X", s);
    for (int s = 1; s <= 3; ++s) m.addArc("Y", s - 1, "Y", s);
    m.addArc("X", 0, "Y", 3);
    m.generateCPTs();
    checkAgainstOracle(m, {}, {{"Y", 10, 1}}, {}, 12);
    checkAgainstOracle(m, {{"X", 5, 0}}, {{"Y", 10, 1}, {"X", 2, 1}}, {}, 12);
  }

  GUM_TEST(ObservationOrderOne) {
    // k=1: slices are coupled only through the atemporal layer, so that layer is
    // the entire interface and the whole backward pass rides on it.
    gum::initRandom(24);
    gum::KTBN< double > m(1);
    m.addTemporal("P", 3);
    m.addAtemporal("C", 2);
    m.addArc("C", AT, "P", 0);
    m.generateCPTs();
    checkAgainstOracle(m, {}, {{"P", 4, 2}}, {}, 5);
    checkAgainstOracle(m, {{"C", AT, 1}}, {{"P", 4, 2}}, {}, 5);
  }

  GUM_TEST(ObservationWithTargets) {
    // requisite pruning must keep the observed nodes and their ancestors even
    // when they are not targets -- their likelihood is what revises the target.
    gum::initRandom(25);
    gum::KTBN< double > m(2);
    m.addTemporal("A", 2);
    m.addTemporal("B", 2);
    m.addAtemporal("W", 2);
    m.addArc("W", AT, "A", 0);
    m.addArc("W", AT, "B", 0);
    m.addArc("A", 0, "A", 1);
    m.addArc("B", 0, "B", 1);
    m.generateCPTs();
    // B is observed but never targeted; it can still only reach A through W
    checkAgainstOracle(m, {}, {{"B", 5, 1}}, {"A"}, 6);
    checkAgainstOracle(m, {}, {{"B", 5, 1}}, {"A", "W"}, 6);
  }

  GUM_TEST(ObservationOnAtemporalNode) {
    gum::initRandom(26);
    gum::KTBN< double > m(2);
    m.addTemporal("X", 2);
    m.addAtemporal("C", 3);
    m.addArc("C", AT, "X", 1);
    m.addArc("X", 0, "X", 1);
    m.generateCPTs();
    checkAgainstOracle(m, {}, {{"C", AT, 2}}, {}, 5);
    checkAgainstOracle(m, {}, {{"C", AT, 0}, {"X", 3, 1}}, {}, 5);
  }

  GUM_TEST(SoftObservation) {
    // a likelihood vector, as in gum::BayesNet; a one-hot one must reproduce
    // a hard observation exactly.
    gum::initRandom(27);
    gum::KTBN< double > m(2);
    m.addTemporal("X", 3);
    m.addArc("X", 0, "X", 1);
    m.generateCPTs();

    gum::KTBNInference< double > soft(&m);
    soft.addObservation("X", 3, std::vector< double >{0.0, 1.0, 0.0});
    soft.makeInference(5);

    gum::KTBNInference< double > hard(&m);
    hard.addObservation("X", 3, 1);
    hard.makeInference(5);

    for (int t = 0; t < 5; ++t)
      checkMarginal(soft.posterior("X", t), toVector(hard.posterior("X", t)), nodeName("X", t));

    // a genuinely soft one must land strictly between the two hard answers
    gum::KTBNInference< double > mid(&m);
    mid.addObservation("X", 3, std::vector< double >{0.5, 0.5, 0.0});
    mid.makeInference(5);
    const auto p = toVector(mid.posterior("X", 3));
    CHECK(p[2] < 1e-12);                          // ruled out by the likelihood
    CHECK(std::fabs(p[0] + p[1] - 1.0) < 1e-9);   // and the rest renormalised
  }

  GUM_TEST(ObservationApiContract) {
    gum::initRandom(28);
    gum::KTBN< double > m(2);
    m.addTemporal("X", 2);
    m.addAtemporal("C", 2);
    m.addArc("X", 0, "X", 1);
    m.generateCPTs();

    gum::KTBNInference< double > ie(&m);
    CHECK_FALSE(ie.hasObservation());
    ie.addObservation("X", 2, 1);
    CHECK(ie.hasObservation());
    CHECK(ie.hasObservation("X", 2));
    CHECK(ie.hasObservation("X[2]"));
    CHECK_FALSE(ie.hasObservation("X", 3));

    ie.eraseObservation("X", 2);
    CHECK_FALSE(ie.hasObservation());

    ie.addObservation("X[1]", 0);
    CHECK(ie.hasObservation("X", 1));
    ie.clearObservation();
    CHECK_FALSE(ie.hasObservation());

    // an unobserved run has probability 1 by construction
    ie.makeInference(4);
    CHECK(std::fabs(ie.observationProbability() - 1.0) < 1e-9);
    CHECK(std::fabs(ie.logObservationProbability()) < 1e-9);

    CHECK_THROWS_AS(ie.addObservation("nope", 0, 0), const gum::NotFound&);
    CHECK_THROWS_AS(ie.addObservation("C", 0, 0), const gum::InvalidArgument&);
    CHECK_THROWS_AS(ie.addObservation("X", 1, 7), const gum::OutOfBounds&);
    CHECK_THROWS_AS(ie.addObservation("X", 1, std::vector< double >{1.0}),
                    const gum::InvalidArgument&);
    CHECK_THROWS_AS(ie.addObservation("X", 1, std::vector< double >{0.0, 0.0}),
                    const gum::InvalidArgument&);
    CHECK_THROWS_AS(ie.addObservation("X", 1, std::vector< double >{-1.0, 2.0}),
                    const gum::InvalidArgument&);
  }

  GUM_TEST(ImpossibleObservation) {
    // deterministic transition + a contradicting observation = probability 0
    gum::initRandom(29);
    gum::KTBN< double > m(2);
    m.addTemporal("X", 2);
    m.addArc("X", 0, "X", 1);
    m.fillCPT("X", 0, {}, {1.0, 0.0});                        // X[0] is surely 0
    m.fillCPT("X", 1, {{{"X", 0}, 0}}, {1.0, 0.0});           // and it never moves
    m.fillCPT("X", 1, {{{"X", 0}, 1}}, {0.0, 1.0});

    gum::KTBNInference< double > ie(&m);
    ie.addObservation("X", 3, 1);   // impossible
    CHECK_THROWS_AS(ie.makeInference(5), const gum::FatalError&);
  }

  GUM_TEST(WindowIntrospection) {
    // the repeating window is compiled once and never rebuilt across horizons
    gum::initRandom(30);
    gum::KTBN< double > m(3);
    m.addTemporal("X", 2);
    m.addArc("X", 0, "X", 1);
    m.addArc("X", 0, "X", 2);
    m.generateCPTs();

    gum::KTBNInference< double > ie(&m);
    const gum::Size iface  = ie.interfaceSize();
    const gum::Size nbClq  = ie.windowJunctionTree().size();
    CHECK(iface == 2);   // X is read at lags 1 and 2, so two occurrences persist
    CHECK(nbClq > 0);
    ie.makeInference(5);
    ie.makeInference(50);
    CHECK(ie.interfaceSize() == iface);
    CHECK(ie.windowJunctionTree().size() == nbClq);
  }

  GUM_TEST(BackwardSweepUnderflowIsReported) {
    // A near-deterministic chain whose improbable transition is 1e-100. Forcing
    // that transition at every slice makes P(obs) ~ 1e-600: representable only
    // because the forward sweep rescales per slice, but the backward sweep forms
    // its intra-window product BEFORE any rescaling, so it flushes to exactly 0.
    //
    // Unguarded, that is silent and total: the zero message becomes the next
    // inNext and zeroes every belief back to slice 0, while normalize() -- a
    // no-op on a zero sum -- leaves posterior() returning tensors of zeros that
    // look like distributions. Only the last slice, read before the message is
    // computed, survives. So the engine must raise rather than answer.
    constexpr double eps = 1e-100;
    constexpr int    T   = 4;

    gum::KTBN< double > m(2);
    m.addTemporal(gum::LabelizedVariable("A", "", 2));
    m.addTemporal(gum::LabelizedVariable("B", "", 2));
    m.addArc("A", 0, "A", 1);
    m.addArc("B", 0, "B", 1);
    m.addArc("A", 1, "B", 1);
    m.generateCPTs();

    for (const std::string& base: {std::string("A"), std::string("B")})
      for (int slice = 0; slice < 2; ++slice) {
        auto&              cpt = const_cast< gum::Tensor< double >& >(m.cpt(base, slice));
        gum::Instantiation inst(cpt);
        for (inst.setFirst(); !inst.end(); ++inst) {
          bool same = true;
          for (gum::Idx d = 1; d < cpt.nbrDim(); ++d)
            if (inst.val(cpt.variable(d)) != inst.val(cpt.variable(0))) same = false;
          cpt.set(inst, same ? 1.0 - eps : eps);
        }
        cpt.normalizeAsCPT(0);
      }

    gum::KTBNInference< double > ie(&m);
    for (int t = 0; t < T; ++t) {           // alternate: force the 1e-100 branch
      ie.addObservation("A", t, t % 2);
      ie.addObservation("B", t, (t + 1) % 2);
    }

    // must raise, NOT return all-zero posteriors
    CHECK_THROWS_AS(ie.makeInference(T), const gum::FatalError&);
  }

  GUM_TEST(BackwardSweepSurvivesOrdinaryEvidence) {
    // control: the guard must not fire on a well-conditioned model, and the
    // smoothed posteriors must remain proper distributions
    gum::KTBN< double > m(2);
    m.addTemporal(gum::LabelizedVariable("A", "", 2));
    m.addTemporal(gum::LabelizedVariable("B", "", 2));
    m.addArc("A", 0, "A", 1);
    m.addArc("B", 0, "B", 1);
    m.addArc("A", 1, "B", 1);
    gum::initRandom(11);
    m.generateCPTs();

    gum::KTBNInference< double > ie(&m);
    ie.addObservation("A", 0, 0);
    ie.addObservation("B", 5, 1);
    GUM_CHECK_ASSERT_THROWS_NOTHING(ie.makeInference(8));

    for (int t = 0; t < 8; ++t) {
      const auto&        p = ie.posterior("A", t);
      gum::Instantiation inst(p);
      double             sum = 0.0;
      for (inst.setFirst(); !inst.end(); ++inst) sum += p[inst];
      CHECK(std::fabs(sum - 1.0) < 1e-9);
    }
  }
  GUM_TEST(WindowPotentialCacheIsExact) {
    // psi(t) is periodic in t % k on any slice carrying no temporal evidence, so
    // _windowPotentials_ memoizes it in 2k slots -- k for the initial windows,
    // k for the repeating one. Three paths must all agree with exact inference:
    // an unobserved slice (cache hit, handed over uncopied), an observed one
    // (base copied, likelihood applied on top) and an intervened one (full
    // rebuild, since do() REPLACES a CPT the base already applied).
    //
    // k=3 with T=13 exercises both regimes and reuses every phase, which is what
    // a key collision would show up in: keying on t % k alone would confuse
    // slice 2 (an initial window) with slice 5 (the repeating one).
    gum::KTBN< double > m(3);
    m.addTemporal(gum::LabelizedVariable("X", "", 3));
    m.addTemporal(gum::LabelizedVariable("Y", "", 2));
    m.addAtemporal(gum::LabelizedVariable("C", "", 2));
    m.addArc("X", 0, "X", 1);
    m.addArc("X", 1, "X", 2);
    m.addArc("X", 0, "X", 2);
    m.addArc("Y", 0, "Y", 1);
    m.addArc("Y", 1, "Y", 2);
    m.addArc("X", 2, "Y", 2);
    m.addArc("C", AT, "X", 2);
    gum::initRandom(9);
    m.generateCPTs();

    constexpr int                 T  = 13;
    const gum::BayesNet< double > un = m.unroll(T);

    // (temporal observations, atemporal observation or -1, intervention slice or -1)
    struct Case {
      std::vector< std::pair< std::string, int > > obs;
      int                                          atempC;
      int                                          doX;
    };
    const std::vector< Case > cases = {
        {{}, -1, -1},                                  // filtering: pure cache hits
        {{{"X", 1}}, -1, -1},                          // observed, initial block
        {{{"Y", 7}}, -1, -1},                          // observed, repeating window
        {{{"X", 2}, {"Y", 9}}, -1, -1},                // one in each regime
        {{{"X", 4}, {"X", 7}, {"X", 10}}, -1, -1},     // same phase, three slices
        {{}, 1, -1},                                   // atemporal only: still periodic
        {{{"Y", 5}}, 0, -1},                           // atemporal + temporal
        {{{"Y", 5}}, -1, 8},                           // intervention forces a rebuild
    };

    for (const auto& c: cases) {
      gum::KTBNInference< double >  ie(&m);
      gum::LazyPropagation< double > lp(&un);
      for (const auto& [b, t]: c.obs) {
        ie.addObservation(b, t, 0);
        lp.addEvidence(un.idFromName(b + "[" + std::to_string(t) + "]"), 0);
      }
      if (c.atempC >= 0) {
        ie.addObservation("C", AT, c.atempC);
        lp.addEvidence(un.idFromName("C"), c.atempC);
      }
      if (c.doX >= 0) {
        // The intervention path takes the full-rebuild branch of
        // _windowPotentials_ (do() REPLACES a CPT the cached base applied). Its
        // agreement with do-calculus is already covered by this suite's
        // intervention tests; what matters here is that routing around the cache
        // changes nothing, so compare against the same engine with the cache
        // necessarily cold -- a fresh engine on the same model and evidence.
        ie.addIntervention("X", c.doX, 1);
        ie.makeInference(T);

        gum::KTBNInference< double > ref(&m);
        for (const auto& [b, t]: c.obs)
          ref.addObservation(b, t, 0);
        ref.addIntervention("X", c.doX, 1);
        ref.addTarget("Y");            // different requisite set => different windows
        ref.makeInference(T);

        for (int t = 0; t < T; ++t) {
          const auto&        a = ie.posterior("Y", t);
          const auto&        b = ref.posterior("Y", t);
          gum::Instantiation ia(a), ib(b);
          for (ia.setFirst(), ib.setFirst(); !ia.end(); ++ia, ++ib)
            CHECK(std::fabs(a[ia] - b[ib]) < 1e-12);
        }
        continue;
      }
      ie.makeInference(T);
      lp.makeInference();
      for (const char* base: {"X", "Y"})
        for (int t = 0; t < T; ++t) {
          const auto& a = ie.posterior(base, t);
          const auto& b = lp.posterior(un.idFromName(std::string(base) + "[" + std::to_string(t) + "]"));
          gum::Instantiation ia(a), ib(b);
          for (ia.setFirst(), ib.setFirst(); !ia.end(); ++ia, ++ib)
            CHECK(std::fabs(a[ia] - b[ib]) < 1e-12);
        }
    }
  }

  GUM_TEST(WindowPotentialCacheSurvivesEditedCPTs) {
    // The cache is cleared per makeInference(): cpt() hands out a const reference
    // whose CONTENTS are mutable, so values may change between runs.
    gum::KTBN< double > m(2);
    m.addTemporal(gum::LabelizedVariable("X", "", 2));
    m.addArc("X", 0, "X", 1);
    gum::initRandom(4);
    m.generateCPTs();

    gum::KTBNInference< double > ie(&m);
    ie.addObservation("X", 0, 0);
    ie.makeInference(6);
    const auto&  p0 = ie.posterior("X", 5);
    gum::Instantiation i0(p0);
    i0.setFirst();
    const double before = p0[i0];

    // flip the kernel, then re-run on the SAME engine
    const_cast< gum::Tensor< double >& >(m.cpt("X", 1)).fillWith({0.99, 0.01, 0.01, 0.99});
    ie.makeInference(6);
    const auto&  p1 = ie.posterior("X", 5);
    gum::Instantiation i1(p1);
    i1.setFirst();
    CHECK(std::fabs(p1[i1] - before) > 1e-6);   // the edit was picked up

    // and it still agrees with exact inference on the EDITED model
    const gum::BayesNet< double >  un = m.unroll(6);
    gum::LazyPropagation< double > lp(&un);
    lp.addEvidence(un.idFromName("X[0]"), 0);
    lp.makeInference();
    const auto&        ref = lp.posterior(un.idFromName("X[5]"));
    gum::Instantiation ia(p1), ib(ref);
    for (ia.setFirst(), ib.setFirst(); !ia.end(); ++ia, ++ib)
      CHECK(std::fabs(p1[ia] - ref[ib]) < 1e-12);
  }
}   // namespace gum_tests
