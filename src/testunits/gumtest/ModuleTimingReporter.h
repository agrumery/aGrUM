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
#include <iomanip>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <vector>

#include <doctest/doctest.h>

namespace gum_test {

  /**
   * @brief Custom doctest listener that aggregates timing per module and test suite.
   *
   * This reporter tracks execution time for each test case and aggregates results
   * by module (e.g., [BN], [GUMBASE]) and by test suite (e.g., [BayesNet], [DAG]).
   *
   * The timing summary is printed at the end of the test run in the format:
   * @code
   * ╔══════════════════════════════════════════════════════════════════════════════╗
   * ║                           Module Timing Summary                              ║
   * ╠══════════════════════════════════════════════════════════════════════════════╣
   * ║ Module      Tests    Time (s)  ║ Top Test Suites                             ║
   * ╠══════════════════════════════════════════════════════════════════════════════╣
   * ║ BN            245      12.34   ║ BayesNet (3.21s), LazyPropagation (2.15s)   ║
   * ║ BASE          189       8.76   ║ MultiDimArray (1.23s), DAG (0.98s)          ║
   * ╚══════════════════════════════════════════════════════════════════════════════╝
   * @endcode
   */
  struct ModuleTimingReporter: doctest::IReporter {
    // Data structures to track timing
    struct SuiteStats {
      int    testCount = 0;
      double totalTime = 0.0;
    };

    struct ModuleStats {
      int                                 testCount = 0;
      double                              totalTime = 0.0;
      std::map< std::string, SuiteStats > suites;
    };

    std::map< std::string, ModuleStats > _modules;
    std::string                          _currentModule;
    std::string                          _currentSuite;
    std::ostream&                        _out;

    explicit ModuleTimingReporter(const doctest::ContextOptions& opts) : _out(*opts.cout) {}

    void report_query(const doctest::QueryData&) override {}

    void test_run_start() override { _modules.clear(); }

    void test_run_end(const doctest::TestRunStats& /*stats*/) override {
      if (_modules.empty()) return;

      printTimingSummary();
    }

    void test_case_start(const doctest::TestCaseData& tc) override {
      // Extract module and suite from test name like "[BN][BayesNet] TestName"
      std::string name = tc.m_name;

      // Extract first tag as module (e.g., [BN])
      std::regex  moduleRegex(R"(\[([A-Z_]+)\])");
      std::smatch match;
      if (std::regex_search(name, match, moduleRegex)) {
        _currentModule = match[1].str();
      } else {
        _currentModule = "OTHER";
      }

      // Extract second tag as suite (e.g., [BayesNet])
      std::regex suiteRegex(R"(\[[A-Z_]+\]\[([^\]]+)\])");
      if (std::regex_search(name, match, suiteRegex)) {
        _currentSuite = match[1].str();
      } else {
        _currentSuite = "Unknown";
      }
    }

    void test_case_reenter(const doctest::TestCaseData&) override {}

    void test_case_end(const doctest::CurrentTestCaseStats& stats) override {
      double seconds = stats.seconds;

      auto& mod = _modules[_currentModule];
      mod.testCount++;
      mod.totalTime += seconds;

      auto& suite = mod.suites[_currentSuite];
      suite.testCount++;
      suite.totalTime += seconds;
    }

    void test_case_exception(const doctest::TestCaseException&) override {}

    void subcase_start(const doctest::SubcaseSignature&) override {}

    void subcase_end() override {}

    void log_assert(const doctest::AssertData&) override {}

    void log_message(const doctest::MessageData&) override {}

    void test_case_skipped(const doctest::TestCaseData&) override {}

    private:
    void printTimingSummary() {
      // Sort modules by total time (descending)
      std::vector< std::pair< std::string, ModuleStats > > sortedModules(_modules.begin(),
                                                                         _modules.end());
      std::sort(sortedModules.begin(), sortedModules.end(), [](const auto& a, const auto& b) {
        return a.second.totalTime > b.second.totalTime;
      });

      // Calculate total time
      double totalTime = 0.0;
      for (const auto& [name, mod]: sortedModules) {
        totalTime += mod.totalTime;
      }

      // Calculate total test count from our stats
      int totalTests = 0;
      for (const auto& [name, mod]: sortedModules) {
        totalTests += mod.testCount;
      }

      // Print header
      _out << "\n";
      _out << "╔════════════════════════════════╗\n";
      _out << "║    Module Timing Summary       ║\n";
      _out << "╠══════════╦═══════╦═════════════╣\n";
      _out << "║ Module   ║ Tests ║    Time (s) ║\n";
      _out << "╠══════════╬═══════╬═════════════╣\n";

      for (const auto& [moduleName, mod]: sortedModules) {
        _out << "║ " << std::left << std::setw(8) << moduleName << " ║" << std::right
             << std::setw(6) << mod.testCount << " ║" << std::setw(12) << std::fixed
             << std::setprecision(2) << mod.totalTime << " ║\n";
      }

      _out << "╠══════════╬═══════╬═════════════╣\n";
      _out << "║ TOTAL    ║" << std::right << std::setw(6) << totalTests << " ║" << std::setw(12)
           << std::fixed << std::setprecision(2) << totalTime << " ║\n";
      _out << "╚══════════╩═══════╩═════════════╝\n";
    }
  };

  // Register as a listener (runs in addition to the default reporter)
  DOCTEST_REGISTER_LISTENER("module_timing", 1, ModuleTimingReporter);

}   // namespace gum_test
