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
#include <agrum/base/core/dot/dotOptions.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/BayesNetFragment.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {

  // The Bayesian networks are drawn as pyAgrum drew them : the nodes are named after the variables,
  // with the tooltip "(id) name", and the look is the one of the configuration.
  struct BayesNetToDotTestSuite {};

  namespace {
    bool bnDotHas(const std::string& dot, const std::string& text) {
      return dot.find(text) != std::string::npos;
    }
  }   // namespace

  GUM_TEST(BayesNetLook) {
    ConfigurationGuard guard;
    auto               bn = gum::BayesNet< double >::fastPrototype("a->b");

    const auto dot = bn.toDot();
    CHECK(bnDotHas(dot, "digraph "));
    CHECK(
        bnDotHas(dot,
                 "graph [bgcolor=\"transparent\", size=\"5\", rankdir=\"TB\", layout=\"dot\"];\n"));
    CHECK(bnDotHas(dot,
                   "node [style=\"filled\", fillcolor=\"#404040\", fontcolor=\"white\", "
                   "color=\"#4A4A4A\"];\n"));
    CHECK(bnDotHas(dot, "edge [color=\"#4A4A4A\"];\n"));
    CHECK(bnDotHas(dot, "  \"a\" [tooltip=\"(0) a\"];\n"));
    CHECK(bnDotHas(dot, "  \"b\" [tooltip=\"(1) b\"];\n"));
    CHECK(bnDotHas(dot, "  \"a\" -> \"b\" [fontsize=\"10\", tooltip=\"0&nbsp;&rarr;&nbsp;1\"];\n"));
    // without options, it is the same
    CHECK_EQ(bn.toDot(gum::dot::DotOptions()), dot);
  }

  GUM_TEST(IsolatedNodesAreDrawn) {
    ConfigurationGuard guard;
    auto               bn = gum::BayesNet< double >::fastPrototype("a->b;c");

    const auto dot = bn.toDot();
    CHECK(bnDotHas(dot, "  \"c\" [tooltip=\"(2) c\"];\n"));
  }

  GUM_TEST(BayesNetFollowsTheConfiguration) {
    ConfigurationGuard guard;
    auto               bn = gum::BayesNet< double >::fastPrototype("a->b");

    gum::config().set("notebook", "arcColor", "#AAAAAA");
    gum::config().set("notebook", "graphRankDir", "LR");
    gum::config().set("notebook", "nodeBgColor", "burlywood");
    gum::config().set("notebook", "nodeFgColor", "adaptive");
    const auto dot = bn.toDot();
    CHECK(bnDotHas(dot, "rankdir=\"LR\""));
    CHECK(bnDotHas(dot,
                   "node [style=\"filled\", fillcolor=\"burlywood\", fontcolor=\"black\", "
                   "color=\"#AAAAAA\"];\n"));
    CHECK(bnDotHas(dot, "edge [color=\"#AAAAAA\"];\n"));
  }

  GUM_TEST(BayesNetOptions) {
    ConfigurationGuard guard;
    auto               bn = gum::BayesNet< double >::fastPrototype("a->b->c");

    gum::dot::DotOptions options;
    options.setSize("12!");
    options.setNodeBgColor(0, "#FF7F64");
    options.setNodeTooltip(1, "(1) b : 0.50000");
    options.setNodeLabel(2, "see");
    options.setArcWidth(0, 1, 2.5);
    options.setArcColor(0, 1, "green");
    options.setArcLabel(1, 2, "weight");

    const auto dot = bn.toDot(options);
    CHECK(bnDotHas(dot, "size=\"12!\""));
    CHECK(bnDotHas(dot,
                   "  \"a\" [tooltip=\"(0) a\", fillcolor=\"#FF7F64\", fontcolor=\"black\"];\n"));
    CHECK(bnDotHas(dot, "  \"b\" [tooltip=\"(1) b : 0.50000\"];\n"));
    CHECK(bnDotHas(dot, "  \"c\" [label=\"see\", tooltip=\"(2) c\"];\n"));
    CHECK(bnDotHas(
        dot,
        "  \"a\" -> \"b\" [fontsize=\"10\", tooltip=\"0&nbsp;&rarr;&nbsp;1\", color=\"green\", "
        "penwidth=\"2.5\"];\n"));
    CHECK(bnDotHas(dot, "label=\"weight\""));
  }

  GUM_TEST(NamesAreEscaped) {
    ConfigurationGuard      guard;
    gum::BayesNet< double > bn;
    bn.add(gum::LabelizedVariable("with space", "", 2));
    bn.add(gum::LabelizedVariable("other", "", 2));
    bn.addArc("with space", "other");

    const auto dot = bn.toDot();
    CHECK(bnDotHas(dot, "\"with space\" [tooltip=\"(0) with space\"];\n"));
    CHECK(bnDotHas(dot, "\"with space\" -> \"other\""));
  }

  GUM_TEST(FragmentHighlightsWhatIsInstalled) {
    ConfigurationGuard              guard;
    auto                            bn = gum::BayesNet< double >::fastPrototype("a->b->c");
    gum::BayesNetFragment< double > fragment(bn);

    // nothing is installed : everything is grey
    auto dot = fragment.toDot();
    CHECK(bnDotHas(dot, "fillcolor=\"#f0f0f0\", color=\"#f0f0f0\", fontcolor=\"#000000\""));
    CHECK_FALSE(bnDotHas(dot, "#ffffaa"));
    CHECK(bnDotHas(dot, "\"a\" -> \"b\" [color=\"#f0f0f0\""));
    CHECK(bnDotHas(dot, "label=\"Fragment of "));

    // a and b are installed with their arc
    fragment.installAscendants("b");
    dot = fragment.toDot();
    CHECK(bnDotHas(dot, "\"a\" [fillcolor=\"#ffffaa\", color=\"#000000\", fontcolor=\"#000000\""));
    CHECK(bnDotHas(dot, "\"b\" [fillcolor=\"#ffffaa\", color=\"#000000\", fontcolor=\"#000000\""));
    CHECK(bnDotHas(dot, "\"c\" [fillcolor=\"#f0f0f0\""));
    CHECK(bnDotHas(dot, "\"a\" -> \"b\" [color=\"#000000\""));
    CHECK(bnDotHas(dot, "\"b\" -> \"c\" [color=\"#f0f0f0\""));

    // the options of the caller win
    gum::dot::DotOptions options;
    options.setNodeBgColor(0, "red");
    options.setArcColor(0, 1, "blue");
    const auto given = fragment.toDot(options);
    CHECK(bnDotHas(
        given,
        "\"a\" [color=\"#000000\", tooltip=\"(0) a\", fillcolor=\"red\", fontcolor=\"white\"]"));
    CHECK(bnDotHas(given, "color=\"blue\""));
    CHECK_FALSE(bnDotHas(given, "\"a\" -> \"b\" [color=\"#000000\""));
    // through the interface of the Bayesian networks, it is the same
    const gum::IBayesNet< double >& asBn = fragment;
    CHECK_EQ(asBn.toDot(), fragment.toDot());
  }

}   // namespace gum_tests
