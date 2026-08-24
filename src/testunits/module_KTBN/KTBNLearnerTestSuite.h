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
#include <fstream>
#include <functional>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include <agrum/base/core/utils_random.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/KTBN/database/KTBNDatabaseGenerator.h>
#include <agrum/KTBN/learning/KTBNLearner.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>


namespace gum_tests {

  struct KTBNLearnerTestSuite {
    static constexpr int AT = gum::KTBN< double >::ATEMPORAL;

    using Learner   = gum::learning::KTBNLearner< double >;
    using Generator = gum::learning::KTBNDatabaseGenerator< double >;
    using Node      = std::pair< std::string, int >;   ///< a (base, slice) node

    /// "every variable is temporal", spelled explicitly. Needed wherever a test
    /// must select the explicit-atemporalVars overload rather than the
    /// inferring one, which is what omitting this argument now selects.
    static inline const std::unordered_set< std::string > NO_ATEMPORAL{};

    // ----- deterministic trajectory fixtures -----
    // Hand-computable by construction: X alternates (X[t+1] = 1 - X[t]) from a
    // per-trajectory start, atemporal columns are constant within a trajectory.

    // writes a single trajectory CSV: the given header and pre-joined data rows
    static void _writeCSV_(const std::string& file, const std::string& header,
                           const std::vector< std::string >& rows) {
      std::ofstream f(file);
      f << header << '\n';
      for (const auto& row: rows)
        f << row << '\n';
    }

    // "std" files: 4 trajectories over X,Y,C,D, lengths {6,5,7,6}. X alternates
    // from starts {0,1,0,1}, Y always equals X, D always equals C, C is
    // {0,0,1,1}: X,Y deterministic within a trajectory, C,D vary only across.
    static std::string _writeStd_() {
      const std::string dir      = GET_RESSOURCES_PATH("outputs");
      const gum::Size   lens[]   = {6, 5, 7, 6};
      const int         starts[] = {0, 1, 0, 1};
      const int         cs[]     = {0, 0, 1, 1};
      for (std::size_t i = 0; i < 4; ++i) {
        std::vector< std::string > rows;
        int                        x = starts[i];
        for (gum::Size t = 0; t < lens[i]; ++t, x = 1 - x)
          rows.push_back(std::to_string(x) + ',' + std::to_string(x) + ','
                         + std::to_string(cs[i]) + ',' + std::to_string(cs[i]));
        _writeCSV_(dir + "/ktbnl_std" + std::to_string(i + 1) + ".csv", "X,Y,C,D", rows);
      }
      return dir;
    }

    // "solo" files: 8 trajectories over X,C,D of length 10 covering every
    // (C, X-start) combination twice; X is independent of C,D. With BIC the
    // only learnable arcs are X[0]->X[1] and one arc between C and D.
    static std::string _writeSolo_() {
      const std::string dir = GET_RESSOURCES_PATH("outputs");
      for (std::size_t i = 0; i < 8; ++i) {
        const int                  c = int(i % 2), start = int((i / 2) % 2);
        std::vector< std::string > rows;
        int                        x = start;
        for (gum::Size t = 0; t < 10; ++t, x = 1 - x)
          rows.push_back(std::to_string(x) + ',' + std::to_string(c) + ','
                         + std::to_string(c));
        _writeCSV_(dir + "/ktbnl_solo" + std::to_string(i + 1) + ".csv", "X,C,D", rows);
      }
      return dir;
    }

    // one binary labelized node per base name; used both as the learner's
    // variable schema and to build hand-made structures with matching domains
    static gum::BayesNet< double > _schema_(const std::vector< std::string >& names) {
      gum::BayesNet< double > bn;
      for (const auto& n: names)
        bn.add(gum::LabelizedVariable(n, "", {"0", "1"}));
      return bn;
    }

    // demo k-TBN of ktbn_build.h: k=4, temporal X,Y,Z,W + atemporal C,D, seven
    // arcs covering every arc type. CPTs copy/flip with p=0.9, parentless nodes
    // uniform: strong signals a learner should recover from samples.
    static gum::KTBN< double > _demoKTBN_() {
      gum::KTBN< double > net(4);
      net.add(gum::LabelizedVariable("X", "", {"0", "1"}), true);
      net.add(gum::LabelizedVariable("Y", "", {"0", "1"}), true);
      net.add(gum::LabelizedVariable("Z", "", {"0", "1"}), true);
      net.add(gum::LabelizedVariable("W", "", {"0", "1"}), true);
      net.add(gum::LabelizedVariable("C", "", {"0", "1"}), false);
      net.add(gum::LabelizedVariable("D", "", {"0", "1"}), false);
      net.addArc("C", AT, "X", 0);   // initial:    atemporal -> slice 0
      net.addArc("Z", 0, "Z", 1);    // initial:    lag-1 (head=1)
      net.addArc("Z", 1, "Z", 2);    // initial:    lag-1 (head=2)
      net.addArc("X", 2, "X", 3);    // transition: lag-1
      net.addArc("W", 0, "W", 3);    // transition: lag-3 (multi-lag)
      net.addArc("X", 3, "Y", 3);    // transition: intra-slice
      net.addArc("C", AT, "D", AT);  // atemporal  -> atemporal

      net.cpt("C", AT).fillWith({0.5, 0.5});
      net.cpt("Z", 0).fillWith({0.5, 0.5});
      net.cpt("Z", 3).fillWith({0.5, 0.5});   // parentless kernel marginal
      for (int s: {1, 2}) net.cpt("X", s).fillWith({0.5, 0.5});
      for (int s: {0, 1, 2}) {
        net.cpt("Y", s).fillWith({0.5, 0.5});
        net.cpt("W", s).fillWith({0.5, 0.5});
      }
      net.fillCPT("X", 0, {{{"C", AT}, 0}}, {0.9, 0.1});   // X[0] ~ C
      net.fillCPT("X", 0, {{{"C", AT}, 1}}, {0.1, 0.9});
      net.fillCPT("D", AT, {{{"C", AT}, 0}}, {0.9, 0.1});   // D ~ C
      net.fillCPT("D", AT, {{{"C", AT}, 1}}, {0.1, 0.9});
      net.fillCPT("Z", 1, {{{"Z", 0}, 0}}, {0.9, 0.1});   // Z copies itself
      net.fillCPT("Z", 1, {{{"Z", 0}, 1}}, {0.1, 0.9});
      net.fillCPT("Z", 2, {{{"Z", 1}, 0}}, {0.9, 0.1});
      net.fillCPT("Z", 2, {{{"Z", 1}, 1}}, {0.1, 0.9});
      net.fillCPT("W", 3, {{{"W", 0}, 0}}, {0.9, 0.1});   // W copies lag-3
      net.fillCPT("W", 3, {{{"W", 0}, 1}}, {0.1, 0.9});
      net.fillCPT("Y", 3, {{{"X", 3}, 0}}, {0.9, 0.1});   // Y ~ X (intra-slice)
      net.fillCPT("Y", 3, {{{"X", 3}, 1}}, {0.1, 0.9});
      net.fillCPT("X", 3, {{{"X", 2}, 0}}, {0.1, 0.9});   // X flips lag-1
      net.fillCPT("X", 3, {{{"X", 2}, 1}}, {0.9, 0.1});
      return net;
    }

    // BN schema supplies the {0,1} domains: the CSV constructor types each
    // variable from trajectory 1 alone, which can't see atemporal domains
    // that are constant within a trajectory - see testCSVConstructor.
    static Learner _std_(gum::Size                                 k = 2,
                         const std::unordered_set< std::string >& atemporal = {"C", "D"}) {
      return Learner(_writeStd_(), "ktbnl_std", 4, k, _schema_({"X", "Y", "C", "D"}), atemporal);
    }

    static Learner _solo_() {
      return Learner(_writeSolo_(), "ktbnl_solo", 8, 2, _schema_({"X", "C", "D"}),
                     {"C", "D"});
    }

    // k=2 structure over the std schema holding exactly the given arcs
    static gum::KTBN< double >
        _structure_(const std::vector< std::pair< Node, Node > >& arcs,
                    const std::unordered_set< std::string >&      atemporal = {"C", "D"}) {
      gum::KTBN< double > s(2);
      for (const std::string name: {"X", "Y", "C", "D"})
        s.add(gum::LabelizedVariable(name, "", {"0", "1"}), !atemporal.contains(name));
      for (const auto& [tail, head]: arcs)
        s.addArc(tail.first, tail.second, head.first, head.second);
      return s;
    }

    // the deterministic learning baseline every structural assertion relies
    // on: BIC + greedy hill climbing, fixed seed
    static gum::KTBN< double > _learn_(Learner& l) {
      l.useScoreBIC().useGreedyHillClimbing();
      gum::initRandom(42);
      return l.learnKTBN();
    }

    // ----- factored assertions ---------------------------------------------

    // the value recorded in state() for the given key ("" if absent)
    static std::string _stateValue_(const Learner& l, const std::string& key) {
      for (const auto& [k, v, comment]: l.state())
        if (k == key) return v;
      return "";
    }

    // no learned arc may go backward in time, and an atemporal head requires
    // an atemporal tail: the structural invariants of the k-TBN definition
    static void _checkTemporalValidity_(const gum::KTBN< double >& m) {
      for (const auto& [tail, head]: m.arcs()) {
        if (head.second == AT) CHECK_EQ(tail.second, AT);
        else if (tail.second != AT) CHECK(tail.second <= head.second);
      }
    }

    // in CPT p, P(target == source) must be 1 (or P(target == 1 - source) when
    // flipped), whatever the values of p's other dimensions
    static void _checkCopyCPT_(const gum::Tensor< double >& p, const std::string& target,
                               const std::string& source, bool flipped = false) {
      gum::Instantiation I(p);
      for (I.setFirst(); !I.end(); ++I) {
        const bool same = I.val(p.variable(target)) == I.val(p.variable(source));
        CHECK(std::abs(p[I] - ((same != flipped) ? 1.0 : 0.0)) < 1e-9);
      }
    }

    // every entry of p equals v (e.g. a uniform learned CPT)
    static void _checkFlatCPT_(const gum::Tensor< double >& p, double v) {
      gum::Instantiation I(p);
      for (I.setFirst(); !I.end(); ++I)
        CHECK(std::abs(p[I] - v) < 1e-9);
    }

    // the atemporal C-D arc, learned in either orientation
    static bool _hasCDedge_(const gum::KTBN< double >& m) {
      return m.existsArc("C", AT, "D", AT) || m.existsArc("D", AT, "C", AT);
    }

    // ========================================================================
    // Constructors
    // ========================================================================

    // the CSV-schema constructor: variables are typed from trajectory 1, so it
    // fully works only when trajectory 1 exhibits every value of every column

    // ========================================================================
    // Shape accessors
    // ========================================================================

    // ========================================================================
    // Main learning methods
    // ========================================================================

    // exact structure recovery on the solo fixture: precisely the two learnable
    // arcs (X[0]->X[1] and one C-D arc), nothing spurious, stable across reruns.

    // every structure-learning algorithm, run end-to-end on the k=4 demo
    // network: sample, learn back, check the planted structure is recovered.
    // p=0.9 signals are strong enough that the algorithm shouldn't matter, so
    // this covers every arc type across greedy, extended greedy, tabu and MIIC.

    // with 0 or 1 atemporal variable no atemporal learner is built; learning
    // and the atemporal-BN placeholder paths must still work

    // learnParameters routes each arc of the given structure to the right
    // internal learner and fills exact maximum-likelihood CPTs

    // the smoothing prior reaches the internal learners: Laplace counts are
    // hand-checkable on the 20 transition windows (10 start at X=0, all flip)

    // ========================================================================
    // Score / algorithm / correction selection
    // ========================================================================

    // algorithm-selection API only: each setter chains and updates the reported
    // state and score/constraint-based flags. No learning here (see testLearnAllAlgorithms).

    // ========================================================================
    // Structural constraints
    // ========================================================================

    // addForbiddenArcAllSlices: tailBase can never be an ancestor of headBase, at
    // any lag (not just matching slices, unlike addForbiddenIntraSliceArc)

    // an intra-slice constraint expands to base[t], so only a temporal process can
    // carry one: an atemporal or unknown base is rejected at the setter (same
    // eager promise as KTBNAdaptiveLearner) instead of failing inside a BNLearner.

    // ========================================================================
    // Diagnostics
    // ========================================================================
  };

  GUM_TEST(CSVConstructor) {
    const std::string dir = GET_RESSOURCES_PATH("outputs");
    _writeCSV_(dir + "/ktbnl_csv1.csv", "X,Y", {"0,1", "1,0", "0,1", "1,0", "0,1"});
    _writeCSV_(dir + "/ktbnl_csv2.csv", "X,Y", {"1,0", "0,1", "1,0", "0,1"});
    _writeCSV_(dir + "/ktbnl_csv3.csv", "X,Y", {"0,1", "1,0", "0,1", "1,0", "0,1", "1,0"});

    Learner l(dir, "ktbnl_csv", 3, 2);
    CHECK_EQ(l.k(), gum::Size(2));
    CHECK_EQ(l.nbCols(), gum::Size(2));
    CHECK_EQ(l.nbSamples(), gum::Size(3));
    CHECK_EQ(l.nbRows(), std::vector< gum::Size >({5, 4, 6}));
    CHECK_EQ(l.names(), std::vector< std::string >({"X", "Y"}));
    CHECK_EQ(l.domainSize("X"), gum::Size(2));

    // end-to-end learning on CSV-typed variables
    _checkTemporalValidity_(_learn_(l));

    // induceTypes=false keeps plain labelized columns, same shape
    Learner lraw(dir, "ktbnl_csv", 3, 2, NO_ATEMPORAL, {"?"}, false);
    CHECK_EQ(lraw.domainSize("X"), gum::Size(2));

    // LIMITATION (documented on purpose): domains come from trajectory 1
    // only, so a value first appearing in a later trajectory is rejected
    _writeCSV_(dir + "/ktbnl_dom1.csv", "X,Y", {"0,1", "1,0", "0,1"});
    _writeCSV_(dir + "/ktbnl_dom2.csv", "X,Y", {"0,2", "1,0", "2,1"});
    CHECK_THROWS(Learner(dir, "ktbnl_dom", 2, 2));
  }

  GUM_TEST(InferAtemporalVars) {
    const std::string dir = GET_RESSOURCES_PATH("outputs");

    // C is constant within every trajectory but differs across them (a
    // per-trajectory static covariate, e.g. an individual's fixed context);
    // X alternates every step. Exactly the "atemporal = constant through
    // time, not constant across samples" semantics this constructor targets.
    _writeCSV_(dir + "/ktbnl_infer1.csv", "X,C", {"0,red", "1,red", "0,red", "1,red"});
    _writeCSV_(dir + "/ktbnl_infer2.csv", "X,C", {"1,red", "0,red", "1,red", "0,red"});
    _writeCSV_(dir + "/ktbnl_infer3.csv", "X,C", {"0,red", "0,red", "1,red", "1,red"});

    Learner    l(dir, "ktbnl_infer", 3, 2);   // no atemporalVars -> infer
    const auto m = _learn_(l);
    CHECK(m.atemporalVarNames().contains("C"));
    CHECK(m.temporalVarNames().contains("X"));
    _checkTemporalValidity_(m);

    // the two overloads really are distinct: on the very same data, naming an
    // empty set forces C to stay temporal, where omitting it inferred atemporal
    Learner    lexp(dir, "ktbnl_infer", 3, 2, NO_ATEMPORAL);
    const auto mexp = _learn_(lexp);
    CHECK(mexp.temporalVarNames().contains("C"));
    CHECK(!mexp.atemporalVarNames().contains("C"));

    // D varies within every trajectory (both labels already present in
    // trajectory 1): never an atemporal candidate, however it looks elsewhere.
    _writeCSV_(dir + "/ktbnl_inferv1.csv", "X,D", {"0,red", "1,blue", "0,red", "1,blue"});
    _writeCSV_(dir + "/ktbnl_inferv2.csv", "X,D", {"1,red", "0,blue", "1,red", "0,blue"});
    _writeCSV_(dir + "/ktbnl_inferv3.csv", "X,D", {"0,red", "0,blue", "1,red", "1,blue"});

    Learner    l2(dir, "ktbnl_inferv", 3, 2);
    const auto m2 = _learn_(l2);
    CHECK(m2.temporalVarNames().contains("D"));
    CHECK(!m2.atemporalVarNames().contains("D"));

    // LIMITATION inherited from the explicit-atemporalVars constructor (see
    // CSVConstructor above): domains are still typed from trajectory 1 alone.
    // An atemporal variable inferred here is, by construction, one whose value
    // may differ between trajectories -- exactly the case most likely to show
    // only one label in trajectory 1 and throw once a later trajectory
    // reveals another.
    _writeCSV_(dir + "/ktbnl_inferdom1.csv", "X,C", {"0,red", "1,red", "0,red"});
    _writeCSV_(dir + "/ktbnl_inferdom2.csv", "X,C", {"0,blue", "1,blue", "0,blue"});
    CHECK_THROWS_AS(Learner(dir, "ktbnl_inferdom", 2, 2), const gum::UnknownLabelInDatabase&);
  }

  GUM_TEST(ConstructorErrors) {
    const std::string dir = _writeStd_();
    const auto        bn  = _schema_({"X", "Y", "C", "D"});

    // k < 2 is a static BN, rejected by both constructors
    CHECK_THROWS_AS(Learner(dir, "ktbnl_std", 4, 1), const gum::InvalidArgument&);
    CHECK_THROWS_AS(Learner(dir, "ktbnl_std", 4, 1, bn), const gum::InvalidArgument&);

    // at least one trajectory is required
    CHECK_THROWS_AS(Learner(dir, "ktbnl_std", 0, 2), const gum::InvalidArgument&);
    CHECK_THROWS_AS(Learner(dir, "ktbnl_std", 0, 2, bn), const gum::InvalidArgument&);

    // more samples than files: trajectory 5 does not exist
    CHECK_THROWS_AS(Learner(dir, "ktbnl_std", 5, 2, bn), const gum::IOError&);

    // an atemporal name absent from the CSV header / from the schema BN
    const std::unordered_set< std::string > ghostSet{"GHOST"};
    CHECK_THROWS_AS(Learner(dir, "ktbnl_std", 4, 2, ghostSet), const gum::InvalidArgument&);
    CHECK_THROWS_AS(Learner(dir, "ktbnl_std", 4, 2, bn, ghostSet), const gum::InvalidArgument&);

    // a schema variable absent from the data
    auto ghost = _schema_({"X", "Y", "C", "D", "GHOST"});
    CHECK_THROWS_AS(Learner(dir, "ktbnl_std", 4, 2, ghost, {"C", "D"}),
                    const gum::MissingVariableInDatabase&);

    // trajectory 2's header differs from trajectory 1's
    _writeCSV_(dir + "/ktbnl_badhdr1.csv", "X,Y", {"0,1", "1,0"});
    _writeCSV_(dir + "/ktbnl_badhdr2.csv", "Y,X", {"0,1", "1,0"});
    CHECK_THROWS_AS(Learner(dir, "ktbnl_badhdr", 2, 2), const gum::InvalidArgument&);

    // a row with the wrong number of cells (in trajectory 2, so that the
    // schema pass on trajectory 1 succeeds)
    _writeCSV_(dir + "/ktbnl_badrow1.csv", "X,Y", {"0,1", "1,0"});
    _writeCSV_(dir + "/ktbnl_badrow2.csv", "X,Y", {"0,1", "1,0,1"});
    CHECK_THROWS_AS(Learner(dir, "ktbnl_badrow", 2, 2), const gum::InvalidArgument&);

    // a trajectory shorter than k has no complete window
    _writeCSV_(dir + "/ktbnl_tiny1.csv", "X,Y", {"0,1", "1,0"});
    CHECK_THROWS_AS(Learner(dir, "ktbnl_tiny", 1, 3), const gum::OperationNotAllowed&);
  }

  GUM_TEST(ShapeAccessors) {
    auto l = _std_();
    CHECK_EQ(l.k(), gum::Size(2));
    CHECK_EQ(l.nbCols(), gum::Size(4));   // X, Y, C, D: 2 temporal + 2 atemporal base vars
    CHECK_EQ(l.nbSamples(), gum::Size(4));
    CHECK_EQ(l.nbRows(), std::vector< gum::Size >({6, 5, 7, 6}));
    // base names (no slice suffix), temporals then atemporals in header order
    CHECK_EQ(l.names(), std::vector< std::string >({"X", "Y", "C", "D"}));
    CHECK_EQ(l.domainSizes(), std::vector< gum::Size >(4, 2));
    CHECK_EQ(l.domainSize("X"), gum::Size(2));
    CHECK_EQ(l.domainSize("C"), gum::Size(2));
    CHECK_THROWS_AS(l.domainSize("GHOST"), const gum::MissingVariableInDatabase&);
    CHECK(!l.hasMissingValues());

    // the same files seen as a k=3 process: nbCols() counts base variables,
    // independent of k, so it stays 4
    auto l3 = _std_(3);
    CHECK_EQ(l3.k(), gum::Size(3));
    CHECK_EQ(l3.nbCols(), gum::Size(4));
    CHECK_EQ(l3.names(), std::vector< std::string >({"X", "Y", "C", "D"}));

    // boundary: a trajectory of exactly k steps is accepted (one window)
    const std::string dir = GET_RESSOURCES_PATH("outputs");
    _writeCSV_(dir + "/ktbnl_kexact1.csv", "X,Y", {"0,0", "1,1"});
    Learner lk(dir, "ktbnl_kexact", 1, 2);
    CHECK_EQ(lk.nbRows(), std::vector< gum::Size >({2}));
  }

  GUM_TEST(MissingValues) {
    const std::string dir = GET_RESSOURCES_PATH("outputs");
    _writeCSV_(dir + "/ktbnl_miss1.csv", "X,Y", {"0,1", "1,0", "0,1"});
    _writeCSV_(dir + "/ktbnl_miss2.csv", "X,Y", {"0,1", "?,0", "1,1"});

    // Detection is unchanged; dropping is opt-in (ignoreMissingSymbols).

    // trajectory 1 alone is complete
    Learner lok(dir, "ktbnl_miss", 1, 2);
    CHECK(!lok.hasMissingValues());

    // the default "?" symbol is detected in trajectory 2
    Learner l(dir, "ktbnl_miss", 2, 2);
    CHECK(l.hasMissingValues());
    CHECK_EQ(l.nbDroppedRows(), gum::Size(0));   // nothing dropped without the flag

    // a custom missing symbol is honoured
    _writeCSV_(dir + "/ktbnl_na1.csv", "X,Y", {"0,1", "1,0", "0,1"});
    _writeCSV_(dir + "/ktbnl_na2.csv", "X,Y", {"0,1", "NA,0", "1,1"});
    Learner lna(dir, "ktbnl_na", 2, 2, std::unordered_set< std::string >{},
                std::vector< std::string >{"NA"});
    CHECK(lna.hasMissingValues());

    // with the flag, the incomplete rows are dropped instead and the internal
    // databases end up with no gap at all
    Learner ldrop(dir, "ktbnl_miss", 2, 2, std::unordered_set< std::string >{},
                  std::vector< std::string >{"?"}, true, /*ignoreMissingSymbols*/ true);
    CHECK(!ldrop.hasMissingValues());
    CHECK(ldrop.nbDroppedRows() > gum::Size(0));
    CHECK(ldrop.isIgnoringMissingSymbols());
  }

  GUM_TEST(MissingValuesDoNotBlockLearning) {
    // The point of the complete-case selection: aGrUM's structure learning
    // refuses a database holding any missing value (IBNLearner::learnDag_), so
    // before it, learnKTBN() threw on ANY trajectory with a gap.
    const std::string dir = GET_RESSOURCES_PATH("outputs");
    for (int s = 1; s <= 4; ++s) {
      std::vector< std::string > rows;
      int                        x = s % 2;
      for (int t = 0; t < 12; ++t, x = 1 - x)
        rows.push_back((t == 3 || t == 7) ? "?" : std::to_string(x));
      _writeCSV_(dir + "/ktbnl_gap" + std::to_string(s) + ".csv", "X", rows);
    }

    // without the flag the data is refused ...
    Learner refuse(dir, "ktbnl_gap", 4, 2);
    refuse.useScoreBIC().useGreedyHillClimbing().useSmoothingPrior(1.0);
    CHECK_THROWS_AS(refuse.learnKTBN(), const gum::MissingValueInDatabase&);

    // ... and with it, learning goes through on the complete cases
    Learner l(dir, "ktbnl_gap", 4, 2, std::unordered_set< std::string >{},
              std::vector< std::string >{"?"}, true, /*ignoreMissingSymbols*/ true);
    l.useScoreBIC().useGreedyHillClimbing().useSmoothingPrior(1.0);
    gum::initRandom(1);
    GUM_CHECK_ASSERT_THROWS_NOTHING(l.learnKTBN());
    CHECK(l.nbDroppedRows() > gum::Size(0));
    CHECK(!l.hasMissingValues());
  }

  GUM_TEST(MissingAtemporalOnRowZero) {
    // An atemporal value is constant down the trajectory, so any row carries it.
    // Reading row 0 blindly would drop every row of a trajectory whose atemporal
    // cell happens to be missing exactly there; the first NON-missing occurrence
    // is used instead.
    const std::string dir = GET_RESSOURCES_PATH("outputs");
    for (int s = 1; s <= 4; ++s) {
      std::vector< std::string > rows;
      int                        x = s % 2;
      for (int t = 0; t < 12; ++t, x = 1 - x)
        rows.push_back(std::to_string(x) + "," + (t == 0 ? "?" : "a"));
      _writeCSV_(dir + "/ktbnl_c0_" + std::to_string(s) + ".csv", "X,C", rows);
    }

    Learner l(dir, "ktbnl_c0_", 4, 2, std::unordered_set< std::string >{"C"},
              std::vector< std::string >{"?"}, true, /*ignoreMissingSymbols*/ true);
    l.useScoreBIC().useGreedyHillClimbing().useSmoothingPrior(1.0);
    gum::initRandom(1);
    gum::KTBN< double > m;
    GUM_CHECK_ASSERT_THROWS_NOTHING(m = l.learnKTBN());
    // C resolved to "a", not to the row-0 marker
    CHECK_EQ(m.variable("C", AT).domainSize(), gum::Size(1));
    CHECK_EQ(l.nbDroppedRows(), gum::Size(0));
  }

  GUM_TEST(ExactStructureRecovery) {
    auto       l = _solo_();
    const auto m = _learn_(l);

    CHECK_EQ(m.k(), gum::Size(2));
    CHECK_EQ(m.temporalVarNames(), std::unordered_set< std::string >{"X"});
    CHECK_EQ(m.atemporalVarNames(), (std::unordered_set< std::string >{"C", "D"}));
    _checkTemporalValidity_(m);

    CHECK(m.existsArc("X", 0, "X", 1));
    CHECK(_hasCDedge_(m));
    CHECK_EQ(m.sizeArcs(), gum::Size(2));   // ... and nothing else

    // relearning from the same learner is stable
    const auto m2 = _learn_(l);
    CHECK(m2.existsArc("X", 0, "X", 1));
    CHECK_EQ(m2.sizeArcs(), gum::Size(2));
  }

  GUM_TEST(LearnAllAlgorithms) {
    const std::string dir = GET_RESSOURCES_PATH("outputs");
    const auto        net = _demoKTBN_();

    // fixed seed -> reproducible files; TOPOLOGICAL gives a stable column order
    gum::initRandom(42);
    Generator gen(net);
    gen.drawSamples(3000, 8, dir, "ktbnl_gen", Generator::VarOrderMode::TOPOLOGICAL);

    // BN-schema constructor: CSV-typing can't recover atemporal domains here
    Learner l(dir, "ktbnl_gen", 3000, 4, _schema_({"X", "Y", "Z", "W", "C", "D"}),
              {"C", "D"});

    const std::vector< std::pair< std::string, std::function< void(Learner&) > > > algos{
        {"greedy hill climbing", [](Learner& x) { x.useScoreBIC().useGreedyHillClimbing(); }},
        {"extended greedy hill climbing",
         [](Learner& x) { x.useScoreBIC().useExtendedGreedyHillClimbing(); }},
        {"local search with tabu list",
         [](Learner& x) { x.useScoreBIC().useLocalSearchWithTabuList(); }},
        {"MIIC", [](Learner& x) { x.useMIIC().useMDLCorrection(); }},
    };

    for (const auto& [name, configure]: algos) {
      INFO("algorithm: " << name);   // tags which algorithm a failure came from
      configure(l);
      gum::initRandom(42);
      const auto m = l.learnKTBN();

      CHECK_EQ(m.k(), gum::Size(4));
      CHECK_EQ(m.temporalVarNames(),
               (std::unordered_set< std::string >{"X", "Y", "Z", "W"}));
      CHECK_EQ(m.atemporalVarNames(), (std::unordered_set< std::string >{"C", "D"}));
      _checkTemporalValidity_(m);

      // every planted arc recovered; time forces the temporal directions, the
      // atemporal C-D edge may be oriented either way
      CHECK(m.existsArc("C", AT, "X", 0));   // atemporal -> slice 0  (initial)
      CHECK(m.existsArc("Z", 0, "Z", 1));    // lag-1                 (initial)
      CHECK(m.existsArc("Z", 1, "Z", 2));    // lag-1                 (initial)
      CHECK(m.existsArc("X", 2, "X", 3));    // lag-1                 (transition)
      CHECK(m.existsArc("W", 0, "W", 3));    // lag-3, multi-lag      (transition)
      CHECK(m.existsArc("X", 3, "Y", 3));    // intra-slice           (transition)
      CHECK(_hasCDedge_(m));
    }
  }

  GUM_TEST(WithoutAtemporalLearner) {
    // a single atemporal variable (D becomes temporal here)
    auto mono = _std_(2, {"C"});
    CHECK(mono.toString().find("Atemporal learner") == std::string::npos);
    GUM_CHECK_ASSERT_THROWS_NOTHING(mono.addNoParentNode("C"));   // silent no-op
    GUM_CHECK_ASSERT_THROWS_NOTHING(mono.eraseNoParentNode("C"));

    const auto m = _learn_(mono);
    CHECK_EQ(m.atemporalVarNames(), std::unordered_set< std::string >{"C"});
    CHECK(m.parents("C", AT).empty());   // an atemporal variable stays a root
    _checkTemporalValidity_(m);

    // parameter learning: the lone atemporal keeps its initial-learner
    // marginal (C is 0 in trajectories 1-2 and 1 in 3-4)
    const auto mp = mono.learnParameters(_structure_({}, {"C"}), false);
    CHECK_EQ(mp.sizeArcs(), gum::Size(0));
    _checkFlatCPT_(mp.cpt("C", AT), 0.5);

    // no atemporal variable at all
    auto none = _std_(2, {});
    CHECK(none.toString().find("Atemporal learner") == std::string::npos);
    CHECK_EQ(none.nbCols(), gum::Size(4));   // X, Y, C, D, all temporal now
    const auto m0 = _learn_(none);
    CHECK_EQ(m0.nbAtemporalVars(), gum::Size(0));
    _checkTemporalValidity_(m0);
  }

  GUM_TEST(LearnParameters) {
    auto       l = _std_();
    const auto s = _structure_({
        {{"C", AT}, {"D", AT}},   // atemporal -> atemporal      (atemporal learner)
        {{"C", AT}, {"X", 0}},    // atemporal -> past temporal  (initial learner)
        {{"X", 0}, {"Y", 0}},     // intra-slice, past           (initial learner)
        {{"X", 0}, {"X", 1}},     // cross-slice                 (transition learner)
        {{"X", 1}, {"Y", 1}},     // intra-slice, last           (transition learner)
        {{"C", AT}, {"Y", 1}},    // atemporal -> last slice     (transition learner)
    });

    const auto m = l.learnParameters(s, false);   // false: exact ML, no score prior

    // exactly the requested arcs, nothing else
    CHECK_EQ(m.sizeArcs(), gum::Size(6));
    for (const auto& [tail, head]: s.arcs())
      CHECK(m.existsArc(tail.first, tail.second, head.first, head.second));

    // the deterministic relations of the data give 0/1 CPTs ...
    _checkCopyCPT_(m.cpt("D", AT), "D", "C");              // D = C
    _checkCopyCPT_(m.cpt("Y", 0), "Y[0]", "X[0]");         // Y = X at slice 0
    _checkCopyCPT_(m.cpt("Y", 1), "Y[1]", "X[1]");         // ... and at slice 1
    _checkCopyCPT_(m.cpt("X", 1), "X[1]", "X[0]", true);   // X always flips
    // ... and the balanced ones are exactly uniform
    _checkFlatCPT_(m.cpt("C", AT), 0.5);   // C is 0 in half the trajectories
    _checkFlatCPT_(m.cpt("X", 0), 0.5);    // X starts at 0 or 1 evenly, given C

    // score-aware variant (default) also succeeds and stays normalized
    const auto md = l.learnParameters(s);
    CHECK_EQ(md.sizeArcs(), gum::Size(6));
    CHECK(std::abs(md.cpt("X", 1).sum() - 2.0) < 1e-9);
  }

  GUM_TEST(LearnParametersSmoothing) {
    auto l = _std_();
    CHECK_EQ(&l.useSmoothingPrior(1.0), &l);
    CHECK_EQ(_stateValue_(l, "Prior"), "Smoothing");

    const auto  m = l.learnParameters(_structure_({{{"X", 0}, {"X", 1}}}), false);
    const auto& p = m.cpt("X", 1);

    gum::Instantiation I(p);
    for (I.setFirst(); !I.end(); ++I) {
      const bool flip = I.val(p.variable("X[1]")) != I.val(p.variable("X[0]"));
      CHECK(std::abs(p[I] - (flip ? 11.0 / 12.0 : 1.0 / 12.0)) < 1e-9);
    }
  }

  GUM_TEST(ScoreAndPriorSelection) {
    auto l = _std_();
    l.useGreedyHillClimbing();   // scores are only reported for score-based algos

    CHECK_EQ(&l.useScoreAIC(), &l);   // setters chain on the learner itself
    CHECK_EQ(_stateValue_(l, "Score"), "AIC");
    CHECK_EQ(_stateValue_(l.useScoreBD(), "Score"), "BD");
    CHECK_EQ(_stateValue_(l.useScoreBDeu(), "Score"), "BDeu");
    CHECK_EQ(_stateValue_(l.useScoreBIC(), "Score"), "BIC");
    CHECK_EQ(_stateValue_(l.useScoreLog2Likelihood(), "Score"), "Log2Likelihood");
    CHECK_EQ(_stateValue_(l.useScoreMDL(), "Score"), "MDL");
    l.useScorefNML();   // the only non-chaining score setter
    CHECK_EQ(_stateValue_(l, "Score"), "fNML");

    // BIC without a prior is compatible; BDeu + smoothing carries an implicit
    // prior of its own and must raise a warning
    l.useScoreBIC();
    CHECK_EQ(l.checkScorePriorCompatibility(), "");
    l.useScoreBDeu().useSmoothingPrior(1.0);
    CHECK(!l.checkScorePriorCompatibility().empty());
  }

  GUM_TEST(AlgorithmSelection) {
    auto l = _std_();

    CHECK_EQ(&l.useGreedyHillClimbing(), &l);
    CHECK_EQ(_stateValue_(l, "Algorithm"), "Greedy Hill Climbing");
    CHECK(l.isScoreBased());
    CHECK(!l.isConstraintBased());

    l.useExtendedGreedyHillClimbing();
    CHECK_EQ(_stateValue_(l, "Algorithm"), "Extended Greedy Hill Climbing");
    CHECK(l.isScoreBased());

    l.useLocalSearchWithTabuList(50, 3);
    CHECK_EQ(_stateValue_(l, "Algorithm"), "Local Search with Tabu List");
    CHECK(l.isScoreBased());

    l.useMIIC();
    CHECK_EQ(_stateValue_(l, "Algorithm"), "MIIC");
    CHECK(l.isConstraintBased());
    CHECK(!l.isScoreBased());
  }

  GUM_TEST(MIICCorrectionsAndLatentVariables) {
    auto l = _solo_();

    // latent variables are a MIIC-only notion: while MIIC has not run, the
    // internal learners simply report none
    l.useGreedyHillClimbing();
    CHECK(l.latentVariables().empty());

    l.useMIIC();
    CHECK_EQ(_stateValue_(l.useMDLCorrection(), "Correction"), "MDL");
    CHECK_EQ(_stateValue_(l.useNMLCorrection(), "Correction"), "NML");
    CHECK_EQ(_stateValue_(l.useNoCorrection(), "Correction"), "No correction");

    // a full MIIC run keeps the temporal invariants and exposes (possibly
    // empty) latent-variable name pairs
    l.useMDLCorrection();
    gum::initRandom(42);
    _checkTemporalValidity_(l.learnKTBN());
    GUM_CHECK_ASSERT_THROWS_NOTHING(l.latentVariables());
  }

  GUM_TEST(ForbiddenArcs) {
    auto l = _solo_();

    // (base, slice) overload: suppress the strongest arc in the data
    l.addForbiddenArc("X", 0, "X", 1);
    CHECK(!_learn_(l).existsArc("X", 0, "X", 1));

    // erase (engine-name overload) restores it
    CHECK_EQ(&l.eraseForbiddenArc("X[0]", "X[1]"), &l);
    CHECK(_learn_(l).existsArc("X", 0, "X", 1));

    // forbidding a backward arc is an accepted no-op
    l.addForbiddenArc("X", 1, "X", 0);
    CHECK(_learn_(l).existsArc("X", 0, "X", 1));

    // atemporal -> atemporal arcs are forbidden in the atemporal learner
    l.addForbiddenArc("C", AT, "D", AT).addForbiddenArc("D", AT, "C", AT);
    auto m = _learn_(l);
    CHECK(!m.existsArc("C", AT, "D", AT));
    CHECK(!m.existsArc("D", AT, "C", AT));
    l.eraseForbiddenArc("C", AT, "D", AT).eraseForbiddenArc("D", AT, "C", AT);
    m = _learn_(l);
    CHECK(_hasCDedge_(m));

    // arcs the k-TBN definition itself forbids cannot be un-forbidden
    CHECK_THROWS_AS(l.eraseForbiddenArc("X", 0, "C", AT),   // temporal -> atemporal
                    const gum::InvalidArgument&);
    CHECK_THROWS_AS(l.eraseForbiddenArc("X", 1, "X", 0),    // backward in time
                    const gum::InvalidArgument&);
  }

  GUM_TEST(MandatoryArcs) {
    auto l = _solo_();

    // force arcs the data does not support, one per internal learner
    l.addMandatoryArc("C", AT, "X", 1);    // head at slice k-1    -> transition
    l.addMandatoryArc("C", AT, "X", 0);    // head at a past slice -> initial
    l.addMandatoryArc("D", AT, "C", AT);   // atemporal head       -> atemporal
    auto m = _learn_(l);
    CHECK(m.existsArc("C", AT, "X", 1));
    CHECK(m.existsArc("C", AT, "X", 0));
    CHECK(m.existsArc("D", AT, "C", AT));

    // erasing stops forcing them: the unsupported arcs disappear again
    l.eraseMandatoryArc("C", AT, "X", 1);
    l.eraseMandatoryArc("C", "X[0]");   // engine-name overload
    l.eraseMandatoryArc("D", AT, "C", AT);
    m = _learn_(l);
    CHECK(!m.existsArc("C", AT, "X", 1));
    CHECK(!m.existsArc("C", AT, "X", 0));

    // adding an impossible arc is rejected up front
    CHECK_THROWS_AS(l.addMandatoryArc("X", 0, "C", AT),   // temporal -> atemporal
                    const gum::InvalidArgument&);
    CHECK_THROWS_AS(l.addMandatoryArc("X", 1, "X", 0),    // backward in time
                    const gum::InvalidArgument&);
    // erasing the obligation of an impossible arc is a silent no-op: a backward
    // arc could never have been added, and no table can even hold it -- "X[1]"
    // is outside the (k=2) initial learner's window -- so there is nothing to
    // lift. Same contract as forbidding an already-impossible arc above.
    GUM_CHECK_ASSERT_THROWS_NOTHING(l.eraseMandatoryArc("X", 1, "X", 0));

    // k = 3: a head at a middle slice also routes to the initial learner
    auto l3 = _std_(3);
    l3.addMandatoryArc("C", AT, "X", 1);
    CHECK(_learn_(l3).existsArc("C", AT, "X", 1));
  }

  GUM_TEST(IntraSliceArcs) {
    auto l = _std_();
    // strip every alternative predictor of Y[1] so that X[1] -> Y[1] is the
    // only one left, then forbid the X-Y intra-slice arcs in both directions
    l.addForbiddenArc("X", 0, "Y", 1).addForbiddenArc("Y", 0, "Y", 1);
    l.addForbiddenIntraSliceArc("X", "Y").addForbiddenIntraSliceArc("Y", "X");
    auto m = _learn_(l);
    for (int t = 0; t < 2; ++t) {
      CHECK(!m.existsArc("X", t, "Y", t));
      CHECK(!m.existsArc("Y", t, "X", t));
    }

    // re-allowing X -> Y resurrects the deterministic X[1] -> Y[1]
    l.eraseForbiddenIntraSliceArc("X", "Y");
    CHECK(_learn_(l).existsArc("X", 1, "Y", 1));
  }

  GUM_TEST(IntraSliceArcsRejectNonTemporalBase) {
    auto              l      = _std_();   // C, D atemporal
    const std::string before = l.toString();

    CHECK_THROWS_AS(l.addForbiddenIntraSliceArc("C", "X"), const gum::InvalidArgument&);
    CHECK_THROWS_AS(l.addForbiddenIntraSliceArc("X", "C"), const gum::InvalidArgument&);
    CHECK_THROWS_AS(l.eraseForbiddenIntraSliceArc("C", "X"), const gum::InvalidArgument&);
    CHECK_THROWS_AS(l.addForbiddenIntraSliceArc("GHOST", "X"), const gum::InvalidArgument&);
    CHECK_THROWS_AS(l.addForbiddenIntraSliceArc("X", "GHOST"), const gum::InvalidArgument&);

    // both endpoints are validated before any internal learner is touched, so a
    // rejected call leaves the recorded configuration exactly as it was -- no
    // half-expanded base[t] constraint from the slices already walked.
    CHECK_EQ(l.toString(), before);
  }

  GUM_TEST(ForbiddenArcAllSlices) {
    // temporal -> temporal: once forbidden, no X->Y arc appears at any lag
    auto hasXYarc = [](const gum::KTBN< double >& m, int k) {
      for (int ts = 0; ts < k; ++ts)
        for (int hs = ts; hs < k; ++hs)
          if (m.existsArc("X", ts, "Y", hs)) return true;
      return false;
    };
    auto l3 = _std_(3);
    l3.addForbiddenArcAllSlices("X", "Y");
    CHECK(!hasXYarc(_learn_(l3), 3));

    // erasing lifts the restriction: a specific cross-slice arc can be forced again
    l3.eraseForbiddenArcAllSlices("X", "Y");
    l3.addMandatoryArc("X", 0, "Y", 1);
    CHECK(_learn_(l3).existsArc("X", 0, "Y", 1));

    // atemporal tail -> temporal head: never produced once forbidden at every slice
    auto l2 = _std_();
    l2.addForbiddenArcAllSlices("C", "X");
    auto mForbidden = _learn_(l2);
    CHECK(!mForbidden.existsArc("C", AT, "X", 0));
    CHECK(!mForbidden.existsArc("C", AT, "X", 1));

    // erase lifts it: the arc can be forced again (same pattern as testMandatoryArcs)
    l2.eraseForbiddenArcAllSlices("C", "X");
    l2.addMandatoryArc("C", AT, "X", 0);
    CHECK(_learn_(l2).existsArc("C", AT, "X", 0));

    // temporal tail -> atemporal head: already structurally impossible, so both
    // add and erase are harmless no-ops
    auto l4 = _solo_();
    GUM_CHECK_ASSERT_THROWS_NOTHING(l4.addForbiddenArcAllSlices("X", "C"));
    GUM_CHECK_ASSERT_THROWS_NOTHING(l4.eraseForbiddenArcAllSlices("X", "C"));

    // atemporal -> atemporal (single ATEMPORAL/ATEMPORAL pair)
    l4.addForbiddenArcAllSlices("C", "D").addForbiddenArcAllSlices("D", "C");
    auto mAt = _learn_(l4);
    CHECK(!mAt.existsArc("C", AT, "D", AT));
    CHECK(!mAt.existsArc("D", AT, "C", AT));
  }

  GUM_TEST(NoParentNodes) {
    auto l = _solo_();

    // temporal node at the last slice: routed to the transition learner
    l.addNoParentNode("X", 1);
    auto m = _learn_(l);
    CHECK(m.parents("X", 1).empty());
    CHECK(_hasCDedge_(m));   // untouched

    l.eraseNoParentNode("X[1]");   // engine-name overload
    CHECK(_learn_(l).existsArc("X", 0, "X", 1));

    // atemporal node: the constraint lives in the atemporal learner, so the
    // C-D signal must flow in the other direction
    l.addNoParentNode("D");
    m = _learn_(l);
    CHECK(m.parents("D", AT).empty());
    CHECK(m.existsArc("D", AT, "C", AT));

    l.eraseNoParentNode("D", AT);
    m = _learn_(l);
    CHECK(_hasCDedge_(m));
  }

  GUM_TEST(NoChildrenNodes) {
    auto l = _solo_();

    l.addNoChildrenNode("X", 0);
    auto m = _learn_(l);
    CHECK(m.children("X", 0).empty());

    l.eraseNoChildrenNode("X[0]");   // engine-name overload
    CHECK(_learn_(l).existsArc("X", 0, "X", 1));

    // atemporal node: no outgoing arc anywhere, so the C-D signal reverses
    l.addNoChildrenNode("C");
    m = _learn_(l);
    CHECK(m.children("C", AT).empty());
    CHECK(m.existsArc("D", AT, "C", AT));

    l.eraseNoChildrenNode("C", AT);
    m = _learn_(l);
    CHECK(_hasCDedge_(m));
  }

  GUM_TEST(PossibleEdges) {
    auto l = _solo_();

    // once a possible-edge list exists, arcs outside it cannot be added: the
    // informative X[0]-X[1] is not listed, only the uninformative C-X[1]
    l.addPossibleEdge("C", AT, "X", 1);
    auto m = _learn_(l);
    CHECK(!m.existsArc("X", 0, "X", 1));
    // no atemporal->atemporal edge was whitelisted, so the atemporal learner
    // is suppressed entirely: the C-D signal cannot appear either
    CHECK(!m.existsArc("C", AT, "D", AT));
    CHECK(!m.existsArc("D", AT, "C", AT));

    // dropping the last possible edge lifts the restriction
    l.erasePossibleEdge("C", AT, "X", 1);
    CHECK(_learn_(l).existsArc("X", 0, "X", 1));

    // atemporal-atemporal edges are also whitelisted on the transition/initial
    // learners (harmless there: atemporal heads are already forced roots), so
    // the whole-model promise holds symmetrically: X[0]-X[1] disappears too,
    // leaving only the atemporal learner's C-D as a candidate
    l.addPossibleEdge("C", "D");   // engine-name overload
    m = _learn_(l);
    CHECK(!m.existsArc("X", 0, "X", 1));
    CHECK(_hasCDedge_(m));
    CHECK_EQ(&l.erasePossibleEdge("C", "D"), &l);
  }

  GUM_TEST(SearchSpaceControls) {
    auto l = _solo_();

    CHECK_EQ(&l.setMaxIndegree(0), &l);
    CHECK_EQ(_learn_(l).sizeArcs(), gum::Size(0));   // no node may have a parent
    l.setMaxIndegree(10);
    CHECK_EQ(_learn_(l).sizeArcs(), gum::Size(2));

    // the allowArc* switches only steer extended greedy hill climbing and
    // tabu search: plain greedy hill climbing always uses all move kinds
    CHECK_EQ(&l.allowArcAdditions(false), &l);
    CHECK_EQ(&l.allowArcDeletions(true), &l);
    CHECK_EQ(&l.allowArcReversals(true), &l);
    l.useScoreBIC().useExtendedGreedyHillClimbing();
    gum::initRandom(42);
    CHECK_EQ(l.learnKTBN().sizeArcs(), gum::Size(0));   // nothing can be added
    l.allowArcAdditions(true);
    gum::initRandom(42);
    CHECK_EQ(l.learnKTBN().sizeArcs(), gum::Size(2));
  }

  GUM_TEST(Diagnostics) {
    auto l = _std_();

    // toString names the k-TBN geometry and each internal learner
    const std::string s = l.toString();
    CHECK(s.find("2 temporal, 2 atemporal") != std::string::npos);
    CHECK(s.find("Transition learner") != std::string::npos);
    CHECK(s.find("Initial learner") != std::string::npos);
    CHECK(s.find("Atemporal learner") != std::string::npos);

    // state() mirrors the (shared) internal-learner configuration
    l.useScoreAIC().useLocalSearchWithTabuList();
    CHECK_EQ(_stateValue_(l, "Algorithm"), "Local Search with Tabu List");
    CHECK_EQ(_stateValue_(l, "Score"), "AIC");
  }

  GUM_TEST(CopyState) {
    auto src = _solo_();
    src.useScoreAIC().useSmoothingPrior(0.5).useLocalSearchWithTabuList();
    src.addForbiddenArc("X", 0, "X", 1);

    auto dst = _solo_();
    dst.copyState(src);
    CHECK_EQ(_stateValue_(dst, "Score"), "AIC");
    CHECK_EQ(_stateValue_(dst, "Prior"), "Smoothing");
    CHECK_EQ(_stateValue_(dst, "Algorithm"), "Local Search with Tabu List");

    // constraints travel too: the copied forbidden arc still bites, even
    // after the copied score/algorithm are overridden by _learn_
    CHECK(!_learn_(dst).existsArc("X", 0, "X", 1));

    // copying from a learner with a different variable set (and towards one
    // without an atemporal learner) is a safe no-op for unknown names
    auto mono = _std_(2, {"C"});
    GUM_CHECK_ASSERT_THROWS_NOTHING(mono.copyState(src));
  }
}   // namespace gum_tests
