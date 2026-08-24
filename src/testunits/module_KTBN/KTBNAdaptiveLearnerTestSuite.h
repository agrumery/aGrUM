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
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <agrum/base/core/utils_random.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/KTBN/learning/KTBNAdaptiveLearner.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>


namespace gum_tests {

  struct KTBNAdaptiveLearnerTestSuite {
    static constexpr int AT = gum::KTBN< double >::ATEMPORAL;

    using Learner = gum::learning::KTBNAdaptiveLearner< double >;
    using KTBN    = gum::KTBN< double >;

    /// "every variable is temporal", spelled explicitly. Needed wherever a test
    /// must select the explicit-atemporalVars overload rather than the
    /// inferring one, which is what omitting this argument now selects.
    static inline const std::unordered_set< std::string > NO_ATEMPORAL{};

    // ----- deterministic fixtures ------------------------------------------
    //
    // Each fixture is written by the test itself so every checked structure and
    // probability is hand-computable. Recurring shape: a temporal X driven by a
    // fixed rule, atemporal columns constant within a trajectory.

    static void _writeCSV_(const std::string& file, const std::string& header,
                           const std::vector< std::string >& rows) {
      std::ofstream f(file);
      f << header << '\n';
      for (const auto& row: rows) f << row << '\n';
    }

    // single column "X" alternating (X[t+1] = 1 - X[t]) from a per-trajectory
    // start, one trajectory per (start, length) spec. No atemporal variable, so
    // the plain CSV constructor already types X over {0, 1}.
    static std::string _writeAlt_(const std::string&                                base,
                                  const std::vector< std::pair< int, gum::Size > >& specs) {
      const std::string dir = GET_RESSOURCES_PATH("outputs");
      for (std::size_t i = 0; i < specs.size(); ++i) {
        std::vector< std::string > rows;
        int                        x = specs[i].first;
        for (gum::Size t = 0; t < specs[i].second; ++t, x = 1 - x)
          rows.push_back(std::to_string(x));
        _writeCSV_(dir + "/" + base + std::to_string(i + 1) + ".csv", "X", rows);
      }
      return dir;
    }

    // single column "X" following a pure lag-2 rule (X[t] = 1 - X[t-2], lag-1
    // uninformative): the period-4 pattern S = 0,1,1,0 phase-shifted per
    // trajectory. Only a k >= 3 model can capture it deterministically.
    static std::string _writeLag2_(const std::string& base, gum::Size nbTraj, gum::Size len) {
      const std::string dir = GET_RESSOURCES_PATH("outputs");
      const int         S[] = {0, 1, 1, 0};
      for (gum::Size i = 0; i < nbTraj; ++i) {
        std::vector< std::string > rows;
        for (gum::Size t = 0; t < len; ++t) rows.push_back(std::to_string(S[(i + t) % 4]));
        _writeCSV_(dir + "/" + base + std::to_string(i + 1) + ".csv", "X", rows);
      }
      return dir;
    }

    // "solo" files: 8 trajectories over X,C,D of length 10 covering every
    // (C, X-start) combination twice. X alternates, D = C, X is independent of
    // C and D: the only learnable arcs are X[0]->X[1] and one C-D arc.
    static std::string _writeSolo_() {
      const std::string dir = GET_RESSOURCES_PATH("outputs");
      for (std::size_t i = 0; i < 8; ++i) {
        const int                  c = int(i % 2), start = int((i / 2) % 2);
        std::vector< std::string > rows;
        int                        x = start;
        for (gum::Size t = 0; t < 10; ++t, x = 1 - x)
          rows.push_back(std::to_string(x) + ',' + std::to_string(c) + ',' + std::to_string(c));
        _writeCSV_(dir + "/kad_solo" + std::to_string(i + 1) + ".csv", "X,C,D", rows);
      }
      return dir;
    }

    // one binary labelized node per base name, used as the learner's variable
    // schema (needed for atemporal columns: constant within a trajectory, they
    // would otherwise be mistyped from trajectory 1 alone)
    static gum::BayesNet< double > _schema_(const std::vector< std::string >& names) {
      gum::BayesNet< double > bn;
      for (const auto& n: names) bn.add(gum::LabelizedVariable(n, "", {"0", "1"}));
      return bn;
    }

    // ----- learner factories ------------------------------------------------

    static Learner _alt_(gum::Size kMax = 3) {
      return Learner(_writeAlt_("kad_alt", {{0, 8}, {1, 8}, {0, 8}, {1, 8}}), "kad_alt", 4, kMax);
    }

    static Learner _solo_(gum::Size kMax = 3) {
      return Learner(_writeSolo_(), "kad_solo", 8, kMax, _schema_({"X", "C", "D"}), {"C", "D"});
    }

    // same files but nothing declared atemporal, so X, C and D are all temporal
    // bases: lets the per-slice / base-pair constraint setters address C and D too.
    // NO_ATEMPORAL is required here, not just documentation: C and D are constant
    // within each of _writeSolo_'s trajectories, so omitting it would select the
    // inferring constructor and reclassify them atemporal instead.
    static Learner _multi_(gum::Size kMax = 3) {
      return Learner(_writeSolo_(), "kad_solo", 8, kMax, NO_ATEMPORAL);
    }

    // ----- factored assertions ----------------------------------------------

    static std::string _stateValue_(const Learner& l, const std::string& key) {
      for (const auto& [k, v, comment]: l.state())
        if (k == key) return v;
      return "";
    }

    // the deterministic learning baseline: BIC structure score + greedy hill
    // climbing, fixed seed (default algorithm is MIIC, which is non-deterministic)
    static KTBN _learn_(Learner& l) {
      l.useScoreBIC().useGreedyHillClimbing();
      gum::initRandom(42);
      return l.learnKTBN();
    }

    static void _checkTemporalValidity_(const KTBN& m) {
      for (const auto& [tail, head]: m.arcs()) {
        if (head.second == AT) CHECK_EQ(tail.second, AT);
        else if (tail.second != AT) CHECK(tail.second <= head.second);
      }
    }

    // ----- independent likelihood oracle ------------------------------------
    //
    // A second, structurally different implementation of the cross-k score, used
    // to validate the learner's private _log2Likelihood_ / _countParameters_
    // through the public scorePerCandidateK(). Where the learner streams a
    // width-k sliding window, this unrolls the k-TBN to each trajectory's length
    // and sums log2 of the complete-data CPT lookups over the flat network.

    static std::vector< std::vector< std::string > >
        _readTraj_(const std::string& path, std::unordered_map< std::string, std::size_t >& colOf) {
      const auto split = [](const std::string& line) {
        std::vector< std::string > cells;
        std::size_t                start = 0;
        for (std::size_t i = 0; i <= line.size(); ++i)
          if (i == line.size() || line[i] == ',') {
            cells.push_back(line.substr(start, i - start));
            start = i + 1;
          }
        return cells;
      };
      std::ifstream f(path);
      std::string   line;
      std::getline(f, line);
      colOf.clear();
      const auto header = split(line);
      for (std::size_t c = 0; c < header.size(); ++c) colOf[header[c]] = c;

      std::vector< std::vector< std::string > > rows;
      while (std::getline(f, line))
        if (!line.empty()) rows.push_back(split(line));
      return rows;
    }

    static double _log2LOracle_(const KTBN& net, const std::string& dir, const std::string& base,
                                gum::Size nbSamples) {
      double ll = 0.0;
      for (gum::Size s = 1; s <= nbSamples; ++s) {
        std::unordered_map< std::string, std::size_t > colOf;
        const auto rows = _readTraj_(dir + "/" + base + std::to_string(s) + ".csv", colOf);
        const auto bn   = net.unroll(rows.size());
        for (const gum::NodeId node: bn.nodes()) {
          const auto&        cpt = bn.cpt(node);
          gum::Instantiation I(cpt);
          for (gum::Idx d = 0; d < cpt.nbrDim(); ++d) {
            const std::string name  = cpt.variable(d).name();   // "V[t]" (temporal) or "V" (atemporal)
            const auto        lb    = name.rfind('[');
            const bool        temp  = (lb != std::string::npos && name.back() == ']');
            const std::string var   = temp ? name.substr(0, lb) : name;
            const std::size_t row    = temp ? std::stoul(name.substr(lb + 1, name.size() - lb - 2)) : 0;
            I.chgVal(name, rows[row][colOf.at(var)]);
          }
          ll += std::log2(cpt[I]);
        }
      }
      return ll;
    }

    // the BIC free-parameter count, recomputed from the model definition
    static double _dfOracle_(const KTBN& net) {
      double df = 0.0;
      for (const auto& [base, slice]: net.nodes()) {
        double cell = net.variable(base, slice).domainSize() - 1.0;
        for (const auto& [pb, ps]: net.parents(base, slice))
          cell *= net.variable(pb, ps).domainSize();
        df += cell;
      }
      return df;
    }

    // the fNML complexity penalty (Sum_i Sum_j log2Cnr(r_i, N_ij)), recomputed by
    // a route independent of the learner's sliding window: unroll the k-TBN to
    // each trajectory's length, then fold every unrolled node back onto its
    // template node (kernel slice = min(t, k-1)) and, per template node, count the
    // observations of each distinct parent-label tuple. log2Cnr itself is a shared
    // mathematical primitive (as std::log2 is for the likelihood oracle); what is
    // validated is the per-template-node count aggregation.
    static double _fNMLPenaltyOracle_(const KTBN& net, const std::string& dir,
                                      const std::string& base, gum::Size nbSamples) {
      const int k = static_cast< int >(net.k());
      // template key ("base|templateSlice") -> node domain size, and -> (parent
      // label tuple -> count). std::map keeps the reduction order deterministic.
      std::map< std::string, std::size_t >                                     rOf;
      std::map< std::string, std::map< std::vector< std::string >, double > >   counts;

      // decode an engine name "V[t]" / "V" into (base, slice) with slice = -1 atemporal
      const auto decode = [](const std::string& name) -> std::pair< std::string, int > {
        const auto lb = name.rfind('[');
        if (lb == std::string::npos || name.back() != ']') return {name, -1};
        return {name.substr(0, lb), std::stoi(name.substr(lb + 1, name.size() - lb - 2))};
      };

      for (gum::Size s = 1; s <= nbSamples; ++s) {
        std::unordered_map< std::string, std::size_t > colOf;
        const auto rows = _readTraj_(dir + "/" + base + std::to_string(s) + ".csv", colOf);
        const auto bn   = net.unroll(rows.size());
        for (const gum::NodeId node: bn.nodes()) {
          const auto& cpt              = bn.cpt(node);
          const auto [sbase, sslice]   = decode(cpt.variable(0).name());   // dim 0 is the node
          const int         tslice     = (sslice < 0) ? -1 : std::min(sslice, k - 1);
          const std::string key        = sbase + "|" + std::to_string(tslice);
          rOf[key]                     = cpt.variable(0).domainSize();
          // parent-label tuple (dims 1..n-1), read from the data at each lag
          std::vector< std::string > tuple;
          for (gum::Idx d = 1; d < cpt.nbrDim(); ++d) {
            const auto [pbase, pslice] = decode(cpt.variable(d).name());
            const std::size_t row      = (pslice < 0) ? 0 : static_cast< std::size_t >(pslice);
            tuple.push_back(rows[row][colOf.at(pbase)]);
          }
          counts[key][tuple] += 1.0;
        }
      }

      gum::VariableLog2ParamComplexity ctable;
      double                           penalty = 0.0;
      for (const auto& [key, perConfig]: counts)
        for (const auto& [tuple, n]: perConfig) penalty += ctable.log2Cnr(rOf[key], n);
      return penalty;
    }

    // ========================================================================
    // Constructors
    // ========================================================================

    // the BN-schema constructor exists for variables whose full domain is not
    // guaranteed in trajectory 1 - typically atemporal columns, constant within a
    // trajectory. The CSV constructor mistypes them; the BN constructor does not.

    // a trajectory shorter than a candidate k has no complete window: the per-k
    // KTBNLearner build throws once the loop reaches that k, and no partial
    // selection is left readable

    // ========================================================================
    // Eager validation (the record-time promise)
    // ========================================================================

    // ========================================================================
    // Structure-learning configuration (score / algorithm / correction / prior / knobs)
    // ========================================================================

    // setters record into state(), nothing applies until learnKTBN(); useScorefNML()
    // returns void, so it's the one non-chainable setter

    // ========================================================================
    // Constraint recording (they do not apply until learnKTBN)
    // ========================================================================

    // possible edges are undirected (BNLearner stores a gum::Edge): dedup + orientation-insensitive

    // ========================================================================
    // Accessors gated on "learnKTBN has run"
    // ========================================================================

    // ========================================================================
    // Diagnostics
    // ========================================================================

    // ========================================================================
    // Order-score selection (the outer, cross-k criterion learnKTBN() uses to pick bestK)
    // ========================================================================

    // ========================================================================
    // learnKTBN: configuration and constraint replay
    // ========================================================================

    // a recorded constraint changes the learned model, proving the record is
    // actually replayed onto every candidate's learner

    // ========================================================================
    // Kernel-relative arc constraints (KTBNAdaptiveLearner-only: there is no
    // fixed kernel slice to anchor a lag to on IKTBNLearner / KTBNLearner)
    // ========================================================================

    // latent variables are a MIIC-only notion: with a score-based winner the set
    // is empty (and never throws once learnKTBN has run)
  };

  GUM_TEST(ConstructorErrors) {
    const std::string dir = _writeAlt_("kad_alt", {{0, 8}, {1, 8}, {0, 8}, {1, 8}});
    const auto        bn  = _schema_({"X"});

    // k < 2 is a static BN, rejected by both constructors
    CHECK_THROWS_AS(Learner(dir, "kad_alt", 4, 1), const gum::InvalidArgument&);
    CHECK_THROWS_AS(Learner(dir, "kad_alt", 4, 1, bn), const gum::InvalidArgument&);

    // at least one trajectory is required
    CHECK_THROWS_AS(Learner(dir, "kad_alt", 0, 3), const gum::InvalidArgument&);
    CHECK_THROWS_AS(Learner(dir, "kad_alt", 0, 3, bn), const gum::InvalidArgument&);

    // a declared atemporal name absent from the header / from the schema BN
    const std::unordered_set< std::string > ghost{"GHOST"};
    CHECK_THROWS_AS(Learner(dir, "kad_alt", 4, 3, ghost), const gum::InvalidArgument&);
    CHECK_THROWS_AS(Learner(dir, "kad_alt", 4, 3, bn, ghost), const gum::InvalidArgument&);

    // explicit constructor: a missing trajectory file is NOT seen at
    // construction (only trajectory 1's header is read); it surfaces at
    // learnKTBN(), when every file is opened
    Learner missing(dir, "kad_alt", 5, 3, NO_ATEMPORAL);   // trajectory 5 does not exist
    GUM_CHECK_ASSERT_THROWS_NOTHING(missing.kMax());
    CHECK_THROWS_AS(missing.learnKTBN(), const gum::IOError&);

    // inferring constructor, general case: as long as at least one column is
    // still a live candidate, testing its constancy means opening every
    // trajectory, so a missing file is normally caught at construction rather
    // than deferred to learnKTBN(). _writeAlt_'s single column falsifies
    // inside trajectory 1 itself (it alternates every row), so it cannot
    // demonstrate this case; _writeSolo_'s C/D survive every existing file.
    CHECK_THROWS_AS(Learner(_writeSolo_(), "kad_solo", 9, 2), const gum::IOError&);

    // inferring constructor, exhausted case: once every candidate has already
    // been falsified, no further trajectory is opened at all -- scanning
    // stops early as an optimisation -- so a missing file past that point is
    // NOT caught eagerly either, the same deferred failure as the explicit
    // constructor. This is exactly _writeAlt_'s situation: X alternates from
    // row 1, so nothing survives to ever test file 5.
    Learner exhausted(dir, "kad_alt", 5, 3);   // trajectory 5 does not exist
    GUM_CHECK_ASSERT_THROWS_NOTHING(exhausted.kMax());
    CHECK_THROWS_AS(exhausted.learnKTBN(), const gum::IOError&);
  }

  GUM_TEST(InferAtemporalVars) {
    const std::string dir = GET_RESSOURCES_PATH("outputs");

    // C is constant within every trajectory, and here also across them (so
    // trajectory-1-only domain typing succeeds); X alternates every step.
    _writeCSV_(dir + "/kad_infer1.csv", "X,C", {"0,red", "1,red", "0,red", "1,red"});
    _writeCSV_(dir + "/kad_infer2.csv", "X,C", {"1,red", "0,red", "1,red", "0,red"});
    _writeCSV_(dir + "/kad_infer3.csv", "X,C", {"0,red", "0,red", "1,red", "1,red"});

    // the two overloads really are distinct, on the very same data: naming an
    // empty set forces C to stay temporal, omitting it infers atemporal
    Learner    lexp(dir, "kad_infer", 3, 2, NO_ATEMPORAL);
    const auto mexp = _learn_(lexp);
    CHECK(mexp.temporalVarNames().contains("C"));
    CHECK(!mexp.atemporalVarNames().contains("C"));

    Learner    linfer(dir, "kad_infer", 3, 2);   // no atemporalVars -> infer
    const auto minfer = _learn_(linfer);
    CHECK(minfer.atemporalVarNames().contains("C"));
    CHECK(minfer.temporalVarNames().contains("X"));
    _checkTemporalValidity_(minfer);
  }

  GUM_TEST(BNSchemaConstructor) {
    const std::string dir = _writeSolo_();

    // CSV constructor: C and D are constant within trajectory 1, so a value
    // first appearing later is rejected while learning
    Learner csv(dir, "kad_solo", 8, 2);
    CHECK_THROWS_AS(_learn_(csv), const gum::UnknownLabelInDatabase&);

    // BN-schema constructor: domains fixed up front, learning succeeds
    auto       bnl = _solo_(2);
    const auto m   = _learn_(bnl);
    CHECK_EQ(m.atemporalVarNames(), (std::unordered_set< std::string >{"C", "D"}));
    _checkTemporalValidity_(m);
  }

  GUM_TEST(ShortTrajectoryBoundary) {
    const std::string dir = _writeAlt_("kad_short", {{0, 5}, {1, 2}});   // shortest length 2
    Learner           l(dir, "kad_short", 2, 3);
    CHECK_THROWS_AS(l.learnKTBN(), const gum::OperationNotAllowed&);   // fails at k = 3
    CHECK_THROWS_AS(l.bestK(), const gum::OperationNotAllowed&);       // nothing selected
  }

  GUM_TEST(ValidationUnknownBase) {
    auto l = _solo_();
    // unknown base rejected by every addressing form
    CHECK_THROWS_AS(l.addForbiddenArc("GHOST", 0, "X", 1), const gum::InvalidArgument&);
    CHECK_THROWS_AS(l.addForbiddenArc("GHOST[0]", "X[1]"), const gum::InvalidArgument&);
    CHECK_THROWS_AS(l.addMandatoryArc("X", 0, "GHOST", 1), const gum::InvalidArgument&);
    CHECK_THROWS_AS(l.addNoParentNode("GHOST", 0), const gum::InvalidArgument&);
    CHECK_THROWS_AS(l.addNoChildrenNode("GHOST[1]"), const gum::InvalidArgument&);
    CHECK_THROWS_AS(l.addPossibleEdge("X", 0, "GHOST", 1), const gum::InvalidArgument&);
    CHECK_THROWS_AS(l.addForbiddenIntraSliceArc("GHOST", "X"), const gum::InvalidArgument&);
    CHECK_THROWS_AS(l.addForbiddenArcAllSlices("X", "GHOST"), const gum::InvalidArgument&);
  }

  GUM_TEST(ValidationSliceRange) {
    auto l = _solo_(3);   // candidate slices span 0..2

    // temporal slice boundary: kMax-1 fits, kMax is out of range
    GUM_CHECK_ASSERT_THROWS_NOTHING(l.addForbiddenArc("X", 2, "X", 2));
    CHECK_THROWS_AS(l.addForbiddenArc("X", 3, "X", 2), const gum::InvalidArgument&);

    // an atemporal variable has no time slice: only ATEMPORAL is valid
    GUM_CHECK_ASSERT_THROWS_NOTHING(l.addForbiddenArc("C", AT, "X", 0));
    CHECK_THROWS_AS(l.addForbiddenArc("C", 1, "X", 0), const gum::InvalidArgument&);

    // an atemporal variable cannot appear in an intra-slice constraint (add + erase)
    CHECK_THROWS_AS(l.addForbiddenIntraSliceArc("C", "X"), const gum::InvalidArgument&);
    CHECK_THROWS_AS(l.eraseForbiddenIntraSliceArc("X", "C"), const gum::InvalidArgument&);

    // a negative slice other than ATEMPORAL is a bad call and must be caught at
    // record time: _encode_ would build "X[-2]", which _determineNode_ reads back
    // as a bare atemporal name, so the constraint would pass the slice-fit rule
    // and only blow up inside a BNLearner at learnKTBN() time.
    CHECK_THROWS_AS(l.addForbiddenArc("X", -2, "X", 1), const gum::InvalidArgument&);
    CHECK_THROWS_AS(l.addMandatoryArc("X", 0, "X", -2), const gum::InvalidArgument&);
    CHECK_THROWS_AS(l.addNoParentNode("X", -2), const gum::InvalidArgument&);
    CHECK_THROWS_AS(l.addPossibleEdge("X", -2, "X", 1), const gum::InvalidArgument&);
    // ATEMPORAL itself stays valid on a temporal base: the base-name-only setters
    // pass it to skip the slice check
    GUM_CHECK_ASSERT_THROWS_NOTHING(l.addForbiddenArcAllSlices("X", "X"));
  }

  GUM_TEST(StructureLearningConfig) {
    auto l = _solo_();
    l.useGreedyHillClimbing();

    // score branches
    CHECK_EQ(_stateValue_(l, "Structure score"), "BDeu");   // the default score
    CHECK_EQ(&l.useScoreAIC(), &l);   // covariant chaining on the concrete type
    CHECK_EQ(_stateValue_(l, "Structure score"), "AIC");
    l.useScoreBD();
    CHECK_EQ(_stateValue_(l, "Structure score"), "BD");
    l.useScoreLog2Likelihood();
    CHECK_EQ(_stateValue_(l, "Structure score"), "Log2Likelihood");
    l.useScoreMDL();
    CHECK_EQ(_stateValue_(l, "Structure score"), "MDL");
    l.useScorefNML();   // void return, so not chainable
    CHECK_EQ(_stateValue_(l, "Structure score"), "fNML");

    // algorithm branches, each replacing the previous
    l.useExtendedGreedyHillClimbing();
    CHECK_EQ(_stateValue_(l, "Algorithm"), "Extended Greedy Hill Climbing");
    l.useLocalSearchWithTabuList(50, 3);
    CHECK_EQ(_stateValue_(l, "Algorithm"), "Local Search with Tabu List");
    CHECK_EQ(_stateValue_(l, "Tabu list size"), std::to_string(50));
    CHECK_EQ(_stateValue_(l, "Tabu nb decrease"), std::to_string(3));

    // MIIC branch: a correction shows instead of a score
    l.useMIIC().useNMLCorrection();
    CHECK_EQ(_stateValue_(l, "Correction"), "NML");
    CHECK_EQ(_stateValue_(l, "Structure score"), "");   // no score under MIIC
    l.useNoCorrection();
    CHECK_EQ(_stateValue_(l, "Correction"), "No correction");

    // prior
    l.useSmoothingPrior(0.5);
    CHECK_EQ(_stateValue_(l, "Prior"), "Smoothing");
    CHECK_EQ(_stateValue_(l, "Prior weight"), std::to_string(0.5));

    // search knobs: graph-change permissions and the in-degree cap
    l.allowArcAdditions(false).allowArcDeletions(false).allowArcReversals(false).setMaxIndegree(2);
    CHECK_EQ(_stateValue_(l, "Arc additions"), "forbidden");
    CHECK_EQ(_stateValue_(l, "Arc deletions"), "forbidden");
    CHECK_EQ(_stateValue_(l, "Arc reversals"), "forbidden");
    CHECK_EQ(_stateValue_(l, "Max in-degree"), std::to_string(2));

    // toString(): an aligned rendering of the same state()
    const std::string s = l.toString();
    CHECK(s.find("Candidate orders") != std::string::npos);
    CHECK(s.find("Max in-degree") != std::string::npos);

    // MIIC never clashes (correction, not score); BDeu+smoothing warns, BIC doesn't
    auto l2 = _solo_(4);
    l2.useMIIC().useSmoothingPrior(1.0);
    CHECK_EQ(l2.checkScorePriorCompatibility(), "");
    l2.useGreedyHillClimbing().useScoreBDeu().useSmoothingPrior(1.0);
    CHECK(!l2.checkScorePriorCompatibility().empty());
    l2.useScoreBIC();
    CHECK_EQ(l2.checkScorePriorCompatibility(), "");
  }

  GUM_TEST(ConstraintRecording) {
    auto l = _multi_();   // X, C, D all temporal

    // a mandatory arc must be temporally feasible to be accepted; erase mirrors it
    l.addMandatoryArc("X", 0, "X", 1);
    CHECK_EQ(_stateValue_(l, "Mandatory arcs"), "{X[0]->X[1]}");
    l.eraseMandatoryArc("X", 0, "X", 1);
    CHECK_EQ(_stateValue_(l, "Mandatory arcs"), "");
    // erasing a never-added constraint is a silent no-op
    GUM_CHECK_ASSERT_THROWS_NOTHING(l.eraseMandatoryArc("X", 0, "X", 1));

    l.addForbiddenArc("X", 0, "C", 1);
    CHECK_EQ(_stateValue_(l, "Forbidden arcs"), "{X[0]->C[1]}");
    l.eraseForbiddenArc("X", 0, "C", 1);
    CHECK_EQ(_stateValue_(l, "Forbidden arcs"), "");

    l.addNoParentNode("X", 1);
    l.addNoChildrenNode("C", 0);
    CHECK_EQ(_stateValue_(l, "No-parent nodes"), "{X[1]}");
    CHECK_EQ(_stateValue_(l, "No-children nodes"), "{C[0]}");
    l.eraseNoParentNode("X", 1);
    l.eraseNoChildrenNode("C", 0);
    CHECK_EQ(_stateValue_(l, "No-parent nodes"), "");
    CHECK_EQ(_stateValue_(l, "No-children nodes"), "");

    // possible edges are undirected: (A,B) and (B,A) are the same record
    l.addPossibleEdge("X[0]", "X[1]");
    CHECK(_stateValue_(l, "Possible edges").find("X[0]->X[1]") != std::string::npos);
    l.addPossibleEdge("X[1]", "X[0]");   // reverse orientation: still one record
    const std::string e = _stateValue_(l, "Possible edges");
    CHECK_EQ(e.find("X[1]->X[0]"), std::string::npos);
    CHECK_EQ(e, "{X[0]->X[1]}");
    l.erasePossibleEdge("X[1]", "X[0]");   // erase via the reverse orientation
    CHECK_EQ(_stateValue_(l, "Possible edges"), "");

    // base-name-pair constraints, expanded per candidate only at learn time
    l.addForbiddenIntraSliceArc("X", "C");
    l.addForbiddenArcAllSlices("X", "D");
    CHECK_EQ(_stateValue_(l, "Forbidden intra-slice arcs"), "{X->C}");
    CHECK_EQ(_stateValue_(l, "Forbidden all-slices arcs"), "{X->D}");
    l.eraseForbiddenIntraSliceArc("X", "C");
    l.eraseForbiddenArcAllSlices("X", "D");
    CHECK_EQ(_stateValue_(l, "Forbidden intra-slice arcs"), "");
    CHECK_EQ(_stateValue_(l, "Forbidden all-slices arcs"), "");
  }

  GUM_TEST(AccessorsBeforeLearnKTBN) {
    auto l = _alt_();
    CHECK_THROWS_AS(l.bestK(), const gum::OperationNotAllowed&);
    CHECK_THROWS_AS(l.latentVariables(), const gum::OperationNotAllowed&);
    CHECK_THROWS_AS(l.scorePerCandidateK(), const gum::OperationNotAllowed&);
    CHECK_EQ(_stateValue_(l, "Selected k"), "not learned yet");

    _learn_(l);
    GUM_CHECK_ASSERT_THROWS_NOTHING(l.bestK());
    GUM_CHECK_ASSERT_THROWS_NOTHING(l.scorePerCandidateK());
    CHECK_EQ(_stateValue_(l, "Selected k"), std::to_string(l.bestK()));
  }

  GUM_TEST(Diagnostics) {
    auto l = _solo_(4);

    CHECK_EQ(l.kMax(), gum::Size(4));
    CHECK_EQ(_stateValue_(l, "Candidate orders"), "2..4");
  }

  GUM_TEST(OrderScoreSelection) {
    // setters record, independently of the per-k structure score
    {
      auto l = _solo_();
      CHECK_EQ(&l.useOrderScoreAIC(), &l);   // covariant chaining on the concrete type
      CHECK_EQ(_stateValue_(l, "Order selection score"), "AIC");
      CHECK_EQ(&l.useOrderScorefNML(), &l);
      CHECK_EQ(_stateValue_(l, "Order selection score"), "fNML");
      CHECK_EQ(&l.useOrderScoreBIC(), &l);
      CHECK_EQ(_stateValue_(l, "Order selection score"), "BIC");
    }

    // kMax == 2: degenerate single-candidate loop
    {
      auto       l = _alt_(2);
      const auto m = _learn_(l);
      CHECK_EQ(l.bestK(), gum::Size(2));
      CHECK_EQ(m.k(), gum::Size(2));
      CHECK_EQ(l.scorePerCandidateK().size(), std::size_t(1));
      CHECK_EQ(l.scorePerCandidateK()[0].first, gum::Size(2));
    }

    // scorePerCandidateK coherent with bestK: ascending k, argmax = bestK
    {
      auto l = _alt_(4);
      _learn_(l);
      const auto& sc = l.scorePerCandidateK();

      CHECK_EQ(sc.size(), std::size_t(3));   // k = 2, 3, 4
      gum::Size argmaxK = 0;
      double    best    = 0.0;
      for (std::size_t i = 0; i < sc.size(); ++i) {
        CHECK_EQ(sc[i].first, gum::Size(i + 2));   // strictly ascending 2,3,4
        if (argmaxK == 0 || sc[i].second > best) {
          best    = sc[i].second;
          argmaxK = sc[i].first;
        }
      }
      CHECK_EQ(argmaxK, l.bestK());
    }

    // BIC (default): score matches an independent unroll-based oracle
    KTBN best;
    {
      const std::string dir = _writeAlt_("kad_alt", {{0, 8}, {1, 8}, {0, 8}, {1, 8}});
      Learner           l(dir, "kad_alt", 4, 2);
      best = _learn_(l);

      const gum::Size nbObs = 4 * 8;   // four trajectories of length 8
      const double    logN  = std::log2(double(nbObs));
      const double    expected
          = _log2LOracle_(best, dir, "kad_alt", 4) - 0.5 * _dfOracle_(best) * logN;

      CHECK_EQ(l.scorePerCandidateK().size(), std::size_t(1));
      CHECK(std::abs(l.scorePerCandidateK()[0].second - expected) < 1e-6);
    }

    // AIC (bare df) and fNML (per-node regret) vs their oracles; reuse "best"
    // above as the reference net — valid since kMax=2 makes structure learning
    // independent of the order score
    {
      const std::string dir  = _writeAlt_("kad_alt", {{0, 8}, {1, 8}, {0, 8}, {1, 8}});
      const double       logL = _log2LOracle_(best, dir, "kad_alt", 4);
      const double       df   = _dfOracle_(best);

      Learner lAIC(dir, "kad_alt", 4, 2);
      lAIC.useOrderScoreAIC();
      _learn_(lAIC);
      CHECK_EQ(lAIC.scorePerCandidateK().size(), std::size_t(1));
      CHECK(std::abs(lAIC.scorePerCandidateK()[0].second - (logL - df)) < 1e-6);

      Learner lfNML(dir, "kad_alt", 4, 2);
      lfNML.useOrderScorefNML();
      _learn_(lfNML);
      const double penalty = _fNMLPenaltyOracle_(best, dir, "kad_alt", 4);
      CHECK_EQ(lfNML.scorePerCandidateK().size(), std::size_t(1));
      CHECK(std::abs(lfNML.scorePerCandidateK()[0].second - (logL - penalty)) < 1e-6);
    }

    // BIC trades off fit vs complexity: lag-1 picks k=2, lag-2 needs k=3
    {
      auto lAlt = _alt_(3);
      _learn_(lAlt);
      CHECK_EQ(lAlt.bestK(), gum::Size(2));   // extra order would only add parameters

      Learner lLag2(_writeLag2_("kad_lag2", 4, 16), "kad_lag2", 4, 3);
      _learn_(lLag2);
      CHECK_EQ(lLag2.bestK(), gum::Size(3));   // lag-2 signal needs the wider window
    }

    // a second learnKTBN() reproduces the first, doesn't accumulate
    {
      auto            l  = _alt_(4);
      _learn_(l);
      const gum::Size k1 = l.bestK();
      const auto      s1 = l.scorePerCandidateK();

      _learn_(l);
      CHECK_EQ(l.bestK(), k1);
      CHECK_EQ(l.scorePerCandidateK().size(), s1.size());   // cleared, not appended
      CHECK_EQ(l.scorePerCandidateK()[0].second, s1[0].second);
    }
  }

  GUM_TEST(ConstraintReplay) {
    // the strongest arc in the alternating data is X[0]->X[1]: unconstrained it
    // is learned, so forbidding it (and nothing else changing) must remove it —
    // that difference is what proves the recorded constraint is actually replayed
    auto base = _alt_(2);
    CHECK(_learn_(base).existsArc("X", 0, "X", 1));

    auto l = _alt_(2);
    l.addForbiddenArc("X", 0, "X", 1);
    const auto m = _learn_(l);
    CHECK(!m.existsArc("X", 0, "X", 1));

    // a constraint naming a slice no small candidate can host raises kMin
    // (KTBNAdaptiveLearner::_recomputeKMin_) so that candidate is skipped
    // outright, instead of being learned only to have the constraint
    // silently dropped for it: kMax=3 with a slice-2 constraint means only
    // k=3 is ever built
    auto l3 = _alt_(3);
    l3.addForbiddenArc("X", 2, "X", 2);   // vacuous arc, but names slice 2
    GUM_CHECK_ASSERT_THROWS_NOTHING(_learn_(l3));
    CHECK_EQ(l3.scorePerCandidateK().size(), std::size_t(1));
    CHECK_EQ(l3.scorePerCandidateK()[0].first, gum::Size(3));
  }

  GUM_TEST(KernelArcConstraints) {
    // ----- validation -----
    auto l = _solo_();   // kMax=3, X temporal, C/D atemporal
    CHECK_THROWS_AS(l.addForbiddenKernelArc("GHOST", 0, "X"), const gum::InvalidArgument&);
    CHECK_THROWS_AS(l.addForbiddenKernelArc("X", -1, "X"), const gum::InvalidArgument&);
    CHECK_THROWS_AS(l.addForbiddenKernelArc("X", 3, "X"), const gum::InvalidArgument&);   // lag == kMax
    GUM_CHECK_ASSERT_THROWS_NOTHING(l.addForbiddenKernelArc("X", 2, "X"));   // lag == kMax-1
    l.eraseForbiddenKernelArc("X", 2, "X");
    // atemporal endpoints have no kernel-slice instance to anchor a lag to
    CHECK_THROWS_AS(l.addForbiddenKernelArc("C", 0, "X"), const gum::InvalidArgument&);
    CHECK_THROWS_AS(l.addMandatoryKernelArc("X", 0, "C"), const gum::InvalidArgument&);

    // ----- recording, state() rendering, erase -----
    l.addForbiddenKernelArc("X", 2, "X");
    CHECK_EQ(_stateValue_(l, "Forbidden kernel arcs"), "{X->X (lag 2)}");
    l.addMandatoryKernelArc("X", 1, "X");
    CHECK_EQ(_stateValue_(l, "Mandatory kernel arcs"), "{X->X (lag 1)}");
    l.eraseForbiddenKernelArc("X", 2, "X");
    l.eraseMandatoryKernelArc("X", 1, "X");
    CHECK_EQ(_stateValue_(l, "Forbidden kernel arcs"), "");
    CHECK_EQ(_stateValue_(l, "Mandatory kernel arcs"), "");
    // erasing a never-added (or already-erased) kernel arc is a silent no-op
    GUM_CHECK_ASSERT_THROWS_NOTHING(l.eraseMandatoryKernelArc("X", 1, "X"));

    // ----- kMin interaction: a kernel lag of t needs k > t, exactly like a
    // constraint naming slice t (KTBNAdaptiveLearner::_recomputeKMin_) -----
    auto l5 = _alt_(5);
    l5.addForbiddenKernelArc("X", 2, "X");   // lag=2 -> kMin=3
    CHECK_EQ(_stateValue_(l5, "Candidate orders"), "3..5");
    l5.eraseForbiddenKernelArc("X", 2, "X");
    CHECK_EQ(_stateValue_(l5, "Candidate orders"), "2..5");

    // ----- end-to-end replay: kMax=2 pins the kernel slice to 1, so lag=1
    // means tailSlice=0 -- addForbiddenKernelArc("X",1,"X") must have exactly
    // the same effect as testConstraintReplay's addForbiddenArc("X",0,"X",1) -----
    auto forb = _alt_(2);
    forb.addForbiddenKernelArc("X", 1, "X");
    CHECK(!_learn_(forb).existsArc("X", 0, "X", 1));

    auto mand = _alt_(2);
    mand.addMandatoryKernelArc("X", 1, "X");
    CHECK(_learn_(mand).existsArc("X", 0, "X", 1));
  }

  GUM_TEST(LatentVariablesAccessor) {
    auto l = _solo_();
    _learn_(l);   // greedy hill climbing
    CHECK(l.latentVariables().empty());

    // a MIIC run still exposes a (possibly empty) set without throwing
    auto lm = _solo_();
    lm.useMIIC().useMDLCorrection();
    gum::initRandom(42);
    lm.learnKTBN();
    GUM_CHECK_ASSERT_THROWS_NOTHING(lm.latentVariables());
  }
}   // namespace gum_tests
