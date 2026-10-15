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
#include <limits>
#include <string>
#include <vector>

#include <agrum/base/core/dot/dotOptions.h>
#include <agrum/base/core/exceptions.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {

  struct DotOptionsTestSuite {};

  GUM_TEST(NothingGivenByDefault) {
    const gum::dot::DotOptions options;
    CHECK(options.empty());
    CHECK(options.size().empty());
    CHECK(options.rankDir().empty());
    CHECK(options.layout().empty());
    CHECK(options.label().empty());
    CHECK(options.node(0) == nullptr);
    CHECK(options.arc(0, 1) == nullptr);
    CHECK(options.edge(0, 1) == nullptr);
  }

  GUM_TEST(GraphOptions) {
    gum::dot::DotOptions options;
    options.setSize("12!");
    options.setRankDir("LR");
    options.setLayout("neato");
    options.setLabel("my graph");
    CHECK_FALSE(options.empty());
    CHECK_EQ(options.size(), "12!");
    CHECK_EQ(options.rankDir(), "LR");
    CHECK_EQ(options.layout(), "neato");
    CHECK_EQ(options.label(), "my graph");

    // a later value replaces the previous one
    options.setSize("5");
    CHECK_EQ(options.size(), "5");
  }

  GUM_TEST(NodeOptions) {
    gum::dot::DotOptions options;
    options.setNodeBgColor(3, "#FF7F64");
    options.setNodeFgColor(3, "red");
    options.setNodeLabel(3, "three");
    options.setNodeTooltip(3, "(3) three");
    options.setNodeAttribute(3, "shape", "box");
    options.setNodeAttribute(3, "label2", "<<b>x</b>>", true);
    options.setNodeBgColor(5, "blue");
    CHECK_FALSE(options.empty());

    const auto* node3 = options.node(3);
    REQUIRE(node3 != nullptr);
    CHECK_EQ(node3->bgColor, "#FF7F64");
    CHECK_EQ(node3->fgColor, "red");
    CHECK_EQ(node3->label, "three");
    CHECK_EQ(node3->tooltip, "(3) three");
    REQUIRE(node3->attributes.size() == 2);
    CHECK_EQ(node3->attributes[0].key, "shape");
    CHECK_EQ(node3->attributes[0].value, "box");
    CHECK_FALSE(node3->attributes[0].verbatim);
    CHECK_EQ(node3->attributes[1].key, "label2");
    CHECK(node3->attributes[1].verbatim);

    // what is not given is empty
    const auto* node5 = options.node(5);
    REQUIRE(node5 != nullptr);
    CHECK_EQ(node5->bgColor, "blue");
    CHECK(node5->fgColor.empty());
    CHECK(node5->label.empty());
    CHECK(node5->tooltip.empty());
    CHECK(node5->attributes.empty());

    CHECK(options.node(4) == nullptr);
  }

  GUM_TEST(ArcOptions) {
    gum::dot::DotOptions options;
    options.setArcColor(1, 2, "green");
    options.setArcWidth(1, 2, 2.5);
    options.setArcLabel(1, 2, "weight");
    options.setArcTooltip(1, 2, "1->2");
    options.setArcAttribute(1, 2, "style", "dashed");
    options.setArcWidth(2, 3, 0);

    const auto* arc12 = options.arc(1, 2);
    REQUIRE(arc12 != nullptr);
    CHECK_EQ(arc12->color, "green");
    REQUIRE(arc12->width.has_value());
    CHECK_EQ(*arc12->width, 2.5);
    CHECK_EQ(arc12->label, "weight");
    CHECK_EQ(arc12->tooltip, "1->2");
    REQUIRE(arc12->attributes.size() == 1);
    CHECK_EQ(arc12->attributes[0].key, "style");

    const auto* arc23 = options.arc(2, 3);
    REQUIRE(arc23 != nullptr);
    REQUIRE(arc23->width.has_value());
    CHECK_EQ(*arc23->width, 0.0);
    CHECK(arc23->color.empty());

    // an arc is directed
    CHECK(options.arc(2, 1) == nullptr);
    CHECK(options.arc(3, 2) == nullptr);
  }

  GUM_TEST(EdgesAreUndirected) {
    gum::dot::DotOptions options;
    options.setArcColor(4, 2, "red");
    options.setArcWidth(1, 3, 1.5);

    // given for (4, 2), found in both orders
    REQUIRE(options.edge(4, 2) != nullptr);
    REQUIRE(options.edge(2, 4) != nullptr);
    CHECK_EQ(options.edge(2, 4)->color, "red");
    CHECK_EQ(options.edge(4, 2), options.edge(2, 4));
    REQUIRE(options.edge(3, 1) != nullptr);
    CHECK_EQ(*options.edge(3, 1)->width, 1.5);
    CHECK(options.edge(1, 2) == nullptr);
  }

  GUM_TEST(FactorOptions) {
    gum::dot::DotOptions options;
    CHECK(options.factor({0, 1}) == nullptr);

    // a factor is designated by its variables, in any order
    options.setFactorBgColor({2, 0, 1}, "#FF7F64");
    options.setFactorFgColor({1, 2, 0}, "red");
    options.setFactorLabel({0, 1, 2}, "f");
    options.setFactorTooltip({0, 2, 1}, "on A, B and C");
    options.setFactorAttribute({0, 1, 2, 2}, "penwidth", "2");
    CHECK_FALSE(options.empty());

    for (const auto& variables:
         {std::vector< gum::Size >{0, 1, 2}, std::vector< gum::Size >{2, 1, 0}}) {
      const auto* factor = options.factor(variables);
      REQUIRE(factor != nullptr);
      CHECK_EQ(factor->bgColor, "#FF7F64");
      CHECK_EQ(factor->fgColor, "red");
      CHECK_EQ(factor->label, "f");
      CHECK_EQ(factor->tooltip, "on A, B and C");
      REQUIRE(factor->attributes.size() == 1);
      CHECK_EQ(factor->attributes[0].key, "penwidth");
    }

    // other factors are not concerned
    CHECK(options.factor({0, 1}) == nullptr);
    CHECK(options.factor({0, 1, 2, 3}) == nullptr);
    CHECK(options.factor({}) == nullptr);

    gum::dot::DotOptions onlyFactors;
    onlyFactors.setFactorBgColor({4}, "blue");
    CHECK_FALSE(onlyFactors.empty());
  }

  GUM_TEST(InvalidWidth) {
    gum::dot::DotOptions options;
    CHECK_THROWS_AS(options.setArcWidth(0, 1, -0.5), gum::InvalidArgument);
    CHECK_THROWS_AS(options.setArcWidth(0, 1, std::numeric_limits< double >::quiet_NaN()),
                    gum::InvalidArgument);
    // nothing was recorded
    CHECK(options.empty());
    CHECK_THROWS_AS(options.setArcWidth(0, 1, std::numeric_limits< double >::infinity()),
                    gum::InvalidArgument);
  }

  GUM_TEST(OptionsAreCopyable) {
    gum::dot::DotOptions options;
    options.setNodeBgColor(0, "red");
    gum::dot::DotOptions copy = options;
    copy.setNodeBgColor(0, "blue");
    CHECK_EQ(options.node(0)->bgColor, "red");
    CHECK_EQ(copy.node(0)->bgColor, "blue");
  }

}   // namespace gum_tests
