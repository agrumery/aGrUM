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
#include <string>
#include <tuple>

#include <agrum/base/core/configuration/configuration.h>
#include <agrum/base/core/exceptions.h>
#include <agrum/base/multidim/tensor.h>
#include <agrum/base/variables/labelizedVariable.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {

  // The text display of a tensor comes from the configuration (keys tensor.displayMaxRows and
  // tensor.displayColumnWidth). The default values are the ones that were written in the code
  // before.
  struct TensorDisplayConfigurationTestSuite {
    public:
    // a binary variable with one parent of 30 values: 30 rows
    static gum::Tensor< double > buildTensor() {
      // a tensor only keeps pointers on its variables
      static const gum::LabelizedVariable x("x", "", 2), p("p", "", 30);
      gum::Tensor< double >               t;
      t << x << p;
      t.fillWith(0.5);
      return t;
    }

    static gum::Size lines(const std::string& s) {
      return static_cast< gum::Size >(std::count(s.begin(), s.end(), '\n'));
    }

    static bool contains(const std::string& s, const std::string& sub) {
      return s.find(sub) != std::string::npos;
    }
  };

  GUM_TEST(TensorDisplayDefaults) {
    const auto text = TensorDisplayConfigurationTestSuite::buildTensor().toString();

    // 6 rows at the beginning, 6 at the end, 30 - 12 rows summarized, 4 decimals
    CHECK(TensorDisplayConfigurationTestSuite::contains(text, "[...18 more line(s) ...]"));
    CHECK(TensorDisplayConfigurationTestSuite::contains(text, "0.5000"));
    CHECK(!TensorDisplayConfigurationTestSuite::contains(text, "0.50000"));
    // empty first line + 3 header lines + 6 rows + summary + 6 rows
    CHECK_EQ(TensorDisplayConfigurationTestSuite::lines(text), gum::Size(17));
  }

  GUM_TEST(TensorDisplayMaxRowsComesFromTheConfiguration) {
    auto& conf = gum::Configuration::instance();

    conf.push();
    conf.set("tensor", "displayMaxRows", "2");
    const auto text = TensorDisplayConfigurationTestSuite::buildTensor().toString();
    CHECK(TensorDisplayConfigurationTestSuite::contains(text, "[...26 more line(s) ...]"));
    CHECK_EQ(TensorDisplayConfigurationTestSuite::lines(text), gum::Size(9));

    // every row is displayed as soon as there are not more than 2 * maxRows of them
    conf.set("tensor", "displayMaxRows", "15");
    const auto full = TensorDisplayConfigurationTestSuite::buildTensor().toString();
    CHECK(!TensorDisplayConfigurationTestSuite::contains(full, "more line(s)"));
    CHECK_EQ(TensorDisplayConfigurationTestSuite::lines(full), gum::Size(34));
    conf.pop();

    CHECK(TensorDisplayConfigurationTestSuite::contains(
        TensorDisplayConfigurationTestSuite::buildTensor().toString(),
        "[...18 more line(s) ...]"));
  }

  GUM_TEST(TensorDisplayColumnWidthComesFromTheConfiguration) {
    auto& conf = gum::Configuration::instance();

    conf.push();
    conf.set("tensor", "displayColumnWidth", "12");
    const auto wide = TensorDisplayConfigurationTestSuite::buildTensor().toString();
    // (width - 5) decimals
    CHECK(TensorDisplayConfigurationTestSuite::contains(wide, "0.5000000"));
    CHECK(!TensorDisplayConfigurationTestSuite::contains(wide, "0.50000000"));

    conf.set("tensor", "displayColumnWidth", "6");
    CHECK(TensorDisplayConfigurationTestSuite::contains(
        TensorDisplayConfigurationTestSuite::buildTensor().toString(),
        "0.5"));
    conf.pop();

    CHECK(!TensorDisplayConfigurationTestSuite::contains(
        TensorDisplayConfigurationTestSuite::buildTensor().toString(),
        "0.50000"));
  }

  GUM_TEST(TensorDisplayInvalidValuesAreRefused) {
    auto& conf = gum::Configuration::instance();
    auto  t    = TensorDisplayConfigurationTestSuite::buildTensor();

    conf.push();
    CHECK_THROWS_AS(conf.set("tensor", "displayMaxRows", "0"), gum::InvalidArgument);
    CHECK_THROWS_AS(conf.set("tensor", "displayColumnWidth", "5"), gum::InvalidArgument);
    CHECK_NOTHROW(std::ignore = t.toString());
    conf.pop();

    CHECK_NOTHROW(std::ignore = t.toString());
  }
}   // namespace gum_tests
