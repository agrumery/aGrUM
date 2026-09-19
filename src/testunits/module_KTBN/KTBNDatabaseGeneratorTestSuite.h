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
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <agrum/base/variables/discretizedVariable.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/KTBN/database/KTBNDatabaseGenerator.h>

#include <agrum/base/core/utils_random.h>
#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {

  struct KTBNDatabaseGeneratorTestSuite {
    static constexpr int AT = gum::KTBN< double >::ATEMPORAL;

    using VarOrderMode = gum::learning::KTBNDatabaseGenerator< double >::VarOrderMode;

    // a small k=2 DBN: temporal X{off,on}, atemporal C{lo,hi}, X@0->X@1, C->X@1
    static gum::KTBN< double > _model_() {
      gum::KTBN< double > m(2);
      m.add(gum::LabelizedVariable("X", "", {"off", "on"}), true);
      m.add(gum::LabelizedVariable("C", "", {"lo", "hi"}), false);
      m.addArc("X", 0, "X", 1);
      m.addArc("C", AT, "X", 1);
      m.generateCPTs();
      return m;
    }

    // ----- CSV helpers -----

    struct CSV {
      std::vector< std::string >                header;   ///< variable names
      std::vector< std::vector< std::string > > rows;     ///< one cell vector per data row
    };

    static std::vector< std::string > _split_(std::string line) {
      if (!line.empty() && line.back() == '\r') line.pop_back();
      std::vector< std::string > cells;
      std::size_t                start = 0, pos;
      while ((pos = line.find(',', start)) != std::string::npos) {
        cells.push_back(line.substr(start, pos - start));
        start = pos + 1;
      }
      cells.push_back(line.substr(start));
      return cells;
    }

    static CSV _readCSV_(const std::string& url) {
      std::ifstream              in(url);
      std::vector< std::string > lines;
      std::string                line;
      while (std::getline(in, line))
        if (!line.empty()) lines.push_back(line);
      in.close();

      CSV c;
      if (lines.empty()) return c;
      c.header = _split_(lines.front());
      for (std::size_t i = 1; i < lines.size(); ++i)
        c.rows.push_back(_split_(lines[i]));
      return c;
    }

    static std::size_t _colIdx_(const CSV& c, const std::string& name) {
      for (std::size_t i = 0; i < c.header.size(); ++i)
        if (c.header[i] == name) return i;
      throw std::runtime_error("column '" + name + "' not found");
    }

    // checks a drawSamples() result: finite non-positive log2-likelihoods, files
    // <stem>1.csv..<stem>N.csv with the expected row counts, no <stem>0.csv.
    static void _checkDrawSamples_(const std::vector< double >&    lls,
                                   const std::string&              dir,
                                   const std::string&              stem,
                                   const std::vector< gum::Size >& expectedRows) {
      CHECK_EQ(lls.size(), expectedRows.size());
      for (const double ll: lls) {
        CHECK(ll <= 0.0);
        CHECK(std::isfinite(ll));
      }
      for (std::size_t i = 0; i < expectedRows.size(); ++i) {
        const std::string url
            = (std::filesystem::path{dir} / (stem + std::to_string(i + 1) + ".csv")).string();
        CHECK(std::filesystem::exists(url));
        CHECK_EQ(_readCSV_(url).rows.size(), std::size_t(expectedRows[i]));
      }
      CHECK(!std::filesystem::exists((std::filesystem::path{dir} / (stem + "0.csv")).string()));
    }

    // drawSamples(1, T, ...): one column per base variable, T rows, cells are
    // modality indices or labels depending on useLabels. Also covers T==k, where
    // Phase 2 (the t>=k transition loop) never runs.

    // one file per requested length, named <base><i>.csv (1-based).

    // fixed-length overload: nbSamples files all of length T.

    // nbVars() and every VarOrderMode agree on the set of base-name columns.

    // TOPOLOGICAL sorts columns by the contemporaneous structure (same-slice +
    // atemporal arcs); lagged cross-slice arcs are not column constraints.
    // ANTI_TOPOLOGICAL is its exact reverse.

    // useLabels=true must honour the chosen discretized rendering mode.

    // Monte-Carlo: empirical P(X[t] | X[t-1]) over a long trajectory must match
    // the CPT, exercising both Phase 1 (slice 0->1) and Phase 2 (t>=k).

    // k=3, X[t] deterministically copies X[t-2]: checks Phase-2 sampling
    // honours a lag-2 dependency across all t >= k.

    // trajectories shorter than k, and a newline separator, must throw.

    // progress emits 0%..99% non-decreasing, then a stop message with the count.
  };

  GUM_TEST(DrawSingleTrajectory) {
    gum::initRandom(42);
    const auto        m   = _model_();
    const gum::Size   T   = 5;
    const std::string dir = GET_RESSOURCES_PATH("outputs");

    gum::learning::KTBNDatabaseGenerator< double > gen(m);

    // --- default: modality indices ---
    const auto lls = gen.drawSamples(1, T, dir, "kdbn_one", VarOrderMode::RANDOM, false);
    _checkDrawSamples_(lls, dir, "kdbn_one", {T});

    const CSV c = _readCSV_((std::filesystem::path{dir} / "kdbn_one1.csv").string());
    // header is exactly the base variables (in some order)
    CHECK_EQ(c.header.size(), std::size_t(2));
    auto names = std::vector< std::string >(c.header.begin(), c.header.end());
    std::sort(names.begin(), names.end());
    CHECK_EQ(names, std::vector< std::string >({"C", "X"}));

    const std::size_t iC = _colIdx_(c, "C");
    const std::size_t iX = _colIdx_(c, "X");
    const std::string c0 = c.rows[0][iC];
    for (const auto& r: c.rows) {
      CHECK(std::stoi(r[iX]) < 2);   // index in [0, domainSize)
      CHECK(std::stoi(r[iC]) < 2);
      CHECK_EQ(r[iC], c0);           // atemporal C is constant along the trajectory
    }

    // --- useLabels=true: cells are variable labels ---
    gen.drawSamples(1, T, dir, "kdbn_one_lbl", VarOrderMode::RANDOM, true);
    const CSV cl = _readCSV_((std::filesystem::path{dir} / "kdbn_one_lbl1.csv").string());
    for (const auto& r: cl.rows) {
      CHECK((r[_colIdx_(cl, "X")] == "off" || r[_colIdx_(cl, "X")] == "on"));
      CHECK((r[_colIdx_(cl, "C")] == "lo" || r[_colIdx_(cl, "C")] == "hi"));
    }

    // --- boundary T == k: only Phase 1 runs, still a valid 2-row trajectory ---
    gen.drawSamples(1, 2, dir, "kdbn_horizonk", VarOrderMode::RANDOM, false);
    const CSV ck = _readCSV_((std::filesystem::path{dir} / "kdbn_horizonk1.csv").string());
    CHECK_EQ(ck.rows.size(), std::size_t(2));
    CHECK_EQ(ck.rows[0][_colIdx_(ck, "C")], ck.rows[1][_colIdx_(ck, "C")]);

    // --- custom separator: ';' must appear in the output, ',' must not ---
    gen.drawSamples(1, 2, dir, "kdbn_semi", VarOrderMode::RANDOM, false, ";");
    {
      std::ifstream     f((std::filesystem::path{dir} / "kdbn_semi1.csv").string());
      const std::string content((std::istreambuf_iterator< char >(f)), {});
      CHECK(content.find(';') != std::string::npos);
      CHECK(content.find(',') == std::string::npos);
    }
  }

  GUM_TEST(DrawSamplesPerLength) {
    gum::initRandom(7);
    const auto m = _model_();

    gum::learning::KTBNDatabaseGenerator< double > gen(m);
    const std::string                              dir     = GET_RESSOURCES_PATH("outputs");
    const std::vector< gum::Size >                 lengths = {3, 4, 5};

    const auto lls = gen.drawSamples(lengths, dir, "toto");
    _checkDrawSamples_(lls, dir, "toto", lengths);
  }

  GUM_TEST(DrawSamplesFixedLength) {
    gum::initRandom(11);
    const auto m = _model_();

    gum::learning::KTBNDatabaseGenerator< double > gen(m);
    const std::string                              dir       = GET_RESSOURCES_PATH("outputs");
    const gum::Size                                nbSamples = 4;
    const gum::Size                                T         = 5;

    const auto lls = gen.drawSamples(nbSamples, T, dir, "fixed");
    _checkDrawSamples_(lls, dir, "fixed", std::vector< gum::Size >(nbSamples, T));
  }

  GUM_TEST(VarOrderModesAndNbVars) {
    const auto                                     m = _model_();
    gum::learning::KTBNDatabaseGenerator< double > gen(m);
    const std::string                              dir = GET_RESSOURCES_PATH("outputs");

    // nbVars() counts one column per process/atemporal variable
    CHECK_EQ(gen.nbVars(), gum::Size(2));

    // all three modes produce the right column set
    for (const auto mode:
         {VarOrderMode::RANDOM, VarOrderMode::TOPOLOGICAL, VarOrderMode::ANTI_TOPOLOGICAL}) {
      gen.drawSamples(1, 2, dir, "kdbn_varorder", mode, false);
      const CSV c     = _readCSV_((std::filesystem::path{dir} / "kdbn_varorder1.csv").string());
      auto      names = std::vector< std::string >(c.header.begin(), c.header.end());
      std::sort(names.begin(), names.end());
      CHECK_EQ(names, std::vector< std::string >({"C", "X"}));
    }
  }

  GUM_TEST(TopologicalVarOrder) {
    // C (atemporal) is a parent of every X slice; lagged self-arc X@0->X@1
    // must NOT affect the column order.
    gum::KTBN< double > m(2);
    m.add(gum::LabelizedVariable("X", "", 2), true);
    m.add(gum::LabelizedVariable("C", "", 2), false);
    m.addArc("C", AT, "X", 0);
    m.addArc("C", AT, "X", 1);
    m.addArc("X", 0, "X", 1);   // lagged: ignored for column ordering
    m.generateCPTs();

    gum::learning::KTBNDatabaseGenerator< double > gen(m);
    const std::string                              dir = GET_RESSOURCES_PATH("outputs");

    gen.drawSamples(1, 2, dir, "kdbn_topo", VarOrderMode::TOPOLOGICAL);
    const CSV  topo_csv = _readCSV_((std::filesystem::path{dir} / "kdbn_topo1.csv").string());
    const auto topo = std::vector< std::string >(topo_csv.header.begin(), topo_csv.header.end());
    CHECK_EQ(topo, std::vector< std::string >({"C", "X"}));   // contemporaneous parent C before X

    gen.drawSamples(1, 2, dir, "kdbn_antitopo", VarOrderMode::ANTI_TOPOLOGICAL);
    const CSV anti_csv = _readCSV_((std::filesystem::path{dir} / "kdbn_antitopo1.csv").string());
    auto      anti     = std::vector< std::string >(anti_csv.header.begin(), anti_csv.header.end());
    CHECK_EQ(anti, std::vector< std::string >({"X", "C"}));
    std::reverse(anti.begin(), anti.end());
    CHECK_EQ(anti, topo);   // anti is exactly the reverse of topo

    // a purely lagged parent (A@0->B@1) must not constrain the order; D is
    // an atemporal parent of A (D->A) and must precede it.
    gum::KTBN< double > m2(2);
    m2.add(gum::LabelizedVariable("A", "", 2), true);
    m2.add(gum::LabelizedVariable("B", "", 2), true);
    m2.add(gum::LabelizedVariable("D", "", 2), false);
    m2.addArc("A", 0, "B", 1);    // lagged: must NOT force A before B
    m2.addArc("D", AT, "A", 0);   // atemporal: D must precede A
    m2.generateCPTs();
    gum::learning::KTBNDatabaseGenerator< double > gen2(m2);

    gen2.drawSamples(1, 2, dir, "kdbn_topo2_", VarOrderMode::TOPOLOGICAL);
    const CSV  c2 = _readCSV_((std::filesystem::path{dir} / "kdbn_topo2_1.csv").string());
    const auto t2 = std::vector< std::string >(c2.header.begin(), c2.header.end());
    // valid permutation of every base variable
    auto sorted = t2;
    std::sort(sorted.begin(), sorted.end());
    CHECK_EQ(sorted, std::vector< std::string >({"A", "B", "D"}));
    // the contemporaneous constraint D->A is honoured
    const auto posD = std::find(t2.begin(), t2.end(), "D") - t2.begin();
    const auto posA = std::find(t2.begin(), t2.end(), "A") - t2.begin();
    CHECK(posD < posA);
  }

  GUM_TEST(DiscretizedLabelRendering) {
    gum::initRandom(123);
    gum::KTBN< double > m(2);
    m.add(gum::DiscretizedVariable< double >("D", "", {0., 1., 2., 3.}), true);   // 3 intervals
    m.addArc("D", 0, "D", 1);
    m.generateCPTs();

    gum::learning::KTBNDatabaseGenerator< double > gen(m);
    const std::string                              dir = GET_RESSOURCES_PATH("outputs");

    // INTERVAL: the "[a,b[" interval label
    gen.setDiscretizedLabelModeInterval();
    gen.drawSamples(1, 4, dir, "kdbn_discr", VarOrderMode::RANDOM, true);
    {
      const CSV c = _readCSV_((std::filesystem::path{dir} / "kdbn_discr1.csv").string());
      CHECK(c.rows[0][_colIdx_(c, "D")].find('[') != std::string::npos);
    }

    // MEDIAN: a deterministic number (the interval median) within the ticks
    gen.setDiscretizedLabelModeMedian();
    gen.drawSamples(1, 4, dir, "kdbn_discr", VarOrderMode::RANDOM, true);
    {
      const CSV    c   = _readCSV_((std::filesystem::path{dir} / "kdbn_discr1.csv").string());
      const double med = std::stod(c.rows[0][_colIdx_(c, "D")]);
      CHECK(med >= 0.0);
      CHECK(med <= 3.0);
    }

    // RANDOM: a number drawn uniformly inside the chosen interval
    gen.setDiscretizedLabelModeRandom();
    gen.drawSamples(1, 4, dir, "kdbn_discr", VarOrderMode::RANDOM, true);
    {
      const CSV    c   = _readCSV_((std::filesystem::path{dir} / "kdbn_discr1.csv").string());
      const double rnd = std::stod(c.rows[0][_colIdx_(c, "D")]);
      CHECK(rnd >= 0.0);
      CHECK(rnd <= 3.0);
    }
  }

  GUM_TEST(SampledFrequenciesMatchCPT) {
    gum::initRandom(42);
    gum::KTBN< double > m(2);
    m.add(gum::LabelizedVariable("X", "", 2), true);
    m.addArc("X", 0, "X", 1);
    m.cpt("X", 0).fillWith({0.7, 0.3});             // P(X@0=0)=0.7
    m.cpt("X", 1).fillWith({0.9, 0.1, 0.2, 0.8});   // P(X@1=0 | X@0)= .9 / .2

    gum::learning::KTBNDatabaseGenerator< double > gen(m);
    const std::string                              dir = GET_RESSOURCES_PATH("outputs");
    gen.drawSamples(1, 60000, dir, "kdbn_stats", VarOrderMode::RANDOM, false);

    const CSV         c  = _readCSV_((std::filesystem::path{dir} / "kdbn_stats1.csv").string());
    const std::size_t iX = _colIdx_(c, "X");

    gum::Size byPrev[2]      = {0, 0};   // count of X[t-1] == v
    gum::Size next0ByPrev[2] = {0, 0};   // count of (X[t]==0 and X[t-1]==v)
    for (std::size_t t = 1; t < c.rows.size(); ++t) {
      const int prev = std::stoi(c.rows[t - 1][iX]);
      const int cur  = std::stoi(c.rows[t][iX]);
      ++byPrev[prev];
      if (cur == 0) ++next0ByPrev[prev];
    }
    const double tol = 0.02;
    CHECK(std::abs(double(next0ByPrev[0]) / byPrev[0] - 0.9) < tol);   // P(X[t]=0 | X[t-1]=0)
    CHECK(std::abs(double(next0ByPrev[1]) / byPrev[1] - 0.2) < tol);   // P(X[t]=0 | X[t-1]=1)
  }

  GUM_TEST(HigherOrderLag) {
    gum::initRandom(1);
    gum::KTBN< double > m(3);
    m.add(gum::LabelizedVariable("X", "", 2), true);
    m.addArc("X", 0, "X", 2);                         // lag 2: X[t] depends only on X[t-2]

    m.fillCPT("X", 0, {}, {1.0, 0.0});                // X[0] = 0   (deterministic)
    m.fillCPT("X", 1, {}, {0.0, 1.0});                // X[1] = 1   (deterministic)
    m.fillCPT("X", 2, {{{"X", 0}, 0}}, {1.0, 0.0});   // X[t] = 0 when X[t-2] = 0
    m.fillCPT("X", 2, {{{"X", 0}, 1}}, {0.0, 1.0});   // X[t] = 1 when X[t-2] = 1

    const gum::Size                                T = 7;
    gum::learning::KTBNDatabaseGenerator< double > gen(m);
    const std::string                              dir = GET_RESSOURCES_PATH("outputs");
    const auto lls = gen.drawSamples(1, T, dir, "kdbn_lag", VarOrderMode::RANDOM, false);

    // every CPT is deterministic, so each drawn value has probability 1 and
    // the total log2-likelihood is exactly 0.
    CHECK(std::abs(lls[0]) < 1e-9);

    // seeded by X0=0, X1=1 and X[t]=X[t-2], the trajectory is 0,1,0,1,...
    const CSV         c  = _readCSV_((std::filesystem::path{dir} / "kdbn_lag1.csv").string());
    const std::size_t iX = _colIdx_(c, "X");
    CHECK_EQ(c.rows.size(), std::size_t(T));
    for (gum::Size t = 0; t < T; ++t)
      CHECK_EQ(std::stoi(c.rows[t][iX]), int(t % 2));
  }

  GUM_TEST(DrawSamplesErrors) {
    const auto                                     m = _model_();   // k = 2
    gum::learning::KTBNDatabaseGenerator< double > gen(m);
    const std::string                              dir = GET_RESSOURCES_PATH("outputs");

    // per-trajectory vector with T < k must throw
    CHECK_THROWS_AS(gen.drawSamples(std::vector< gum::Size >{3, 1, 4}, dir, "err"),
                    const gum::OperationNotAllowed&);
    // fixed-length overload rejects T < k just the same
    CHECK_THROWS_AS(gen.drawSamples(gum::Size(3), gum::Size(1), dir, "errfix"),
                    const gum::OperationNotAllowed&);
    // a newline in the CSV separator is rejected before any I/O
    CHECK_THROWS_AS(gen.drawSamples(1, 3, dir, "errsep", VarOrderMode::RANDOM, false, "\n"),
                    const gum::InvalidArgument&);
  }

  GUM_TEST(ProgressNotifier) {
    gum::initRandom(42);
    const auto                                     m = _model_();
    gum::learning::KTBNDatabaseGenerator< double > gen(m);

    struct Listener: public gum::ProgressListener {
      std::vector< gum::Size > percents;
      std::string              stopMsg;

      explicit Listener(gum::ProgressNotifier& n) : gum::ProgressListener(n) {}

      void whenProgress(const void*, gum::Size pct, double) override { percents.push_back(pct); }

      void whenStop(const void*, std::string_view msg) override { stopMsg = std::string(msg); }
    };

    Listener                       listener(gen);
    const std::vector< gum::Size > lengths(200, 2);   // 200 short trajectories
    gen.drawSamples(lengths, GET_RESSOURCES_PATH("outputs"), "prog");

    // first signal is always 0% (emitted before the loop)
    CHECK(!listener.percents.empty());
    CHECK_EQ(listener.percents.front(), gum::Size(0));
    // all percents in [0, 99] and non-decreasing (100% is replaced by onStop)
    for (auto p: listener.percents)
      CHECK(p <= gum::Size(99));
    for (std::size_t i = 1; i < listener.percents.size(); ++i)
      CHECK(listener.percents[i] >= listener.percents[i - 1]);
    // stop message received and mentions the trajectory count
    CHECK(!listener.stopMsg.empty());
    CHECK(listener.stopMsg.find("200") != std::string::npos);
  }
}   // namespace gum_tests
