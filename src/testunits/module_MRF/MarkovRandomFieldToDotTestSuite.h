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
#include <agrum/MRF/MarkovRandomField.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {

  // The Markov random fields are drawn as pyAgrum drew them, as a graph of variables or as a factor
  // graph.
  struct MarkovRandomFieldToDotTestSuite {};

  namespace {
    bool mrfDotHas(const std::string& dot, const std::string& text) {
      return dot.find(text) != std::string::npos;
    }

    gum::MarkovRandomField< double > mrfForDot() {
      return gum::MarkovRandomField< double >::fastPrototype("a--b--c;a--c;c--d", 2);
    }
  }   // namespace

  GUM_TEST(GraphOfVariablesLook) {
    ConfigurationGuard guard;
    const auto         mrf = mrfForDot();

    const auto dot = mrf.toDot();
    CHECK(mrfDotHas(dot, "graph "));
    CHECK_FALSE(mrfDotHas(dot, "digraph"));
    // the size is the one of the notebook (not 4 as the Markov random fields were drawn)
    CHECK(mrfDotHas(
        dot,
        "graph [bgcolor=\"transparent\", size=\"5\", rankdir=\"TB\", layout=\"dot\"];\n"));
    CHECK(mrfDotHas(dot,
                    "node [style=\"filled\", fillcolor=\"#404040\", fontcolor=\"white\", "
                    "color=\"#4A4A4A\"];\n"));
    CHECK(mrfDotHas(dot, "  \"a\" [tooltip=\"(0) a\"];\n"));
    CHECK(mrfDotHas(dot, "  \"d\" [tooltip=\"(3) d\"];\n"));
    CHECK(
        mrfDotHas(dot, "  \"a\" -- \"b\" [fontsize=\"10\", tooltip=\"0&nbsp;&mdash;&nbsp;1\"];\n"));
    CHECK(mrfDotHas(dot, "\"c\" -- \"d\" [fontsize=\"10\", tooltip=\"2&nbsp;&mdash;&nbsp;3\"];\n"));
    // each edge once
    CHECK_FALSE(mrfDotHas(dot, "\"b\" -- \"a\""));
    CHECK_EQ(mrf.toDot(gum::dot::DotOptions()), dot);
  }

  GUM_TEST(GraphOfVariablesOptions) {
    ConfigurationGuard guard;
    const auto         mrf = mrfForDot();

    gum::dot::DotOptions options;
    options.setSize("8");
    options.setNodeBgColor(0, "burlywood");
    options.setArcWidth(0, 1, 3);   // an edge, in any order
    options.setArcColor(3, 2, "green");

    const auto dot = mrf.toDot(options);
    CHECK(mrfDotHas(dot, "size=\"8\""));
    CHECK(
        mrfDotHas(dot, "\"a\" [tooltip=\"(0) a\", fillcolor=\"burlywood\", fontcolor=\"black\"]"));
    CHECK(mrfDotHas(
        dot,
        "\"a\" -- \"b\" [fontsize=\"10\", tooltip=\"0&nbsp;&mdash;&nbsp;1\", penwidth=\"3\"]"));
    CHECK(mrfDotHas(
        dot,
        "\"c\" -- \"d\" [fontsize=\"10\", tooltip=\"2&nbsp;&mdash;&nbsp;3\", color=\"green\"]"));
  }

  GUM_TEST(IsolatedVariablesAreDrawn) {
    ConfigurationGuard guard;
    const auto         mrf = gum::MarkovRandomField< double >::fastPrototype("a--b;c", 2);
    CHECK(mrfDotHas(mrf.toDot(), "  \"c\" [tooltip=\"(2) c\"];\n"));
    CHECK(mrfDotHas(mrf.toDotAsFactorGraph(), "\"c\" ["));
  }

  GUM_TEST(FactorGraphLook) {
    ConfigurationGuard guard;
    const auto         mrf = mrfForDot();

    const auto dot = mrf.toDotAsFactorGraph();
    CHECK(mrfDotHas(dot, "graph "));
    CHECK_FALSE(mrfDotHas(dot, "digraph"));
    // the layout of the factor graphs
    CHECK(mrfDotHas(
        dot,
        "graph [bgcolor=\"transparent\", size=\"5\", rankdir=\"TB\", layout=\"neato\"];\n"));
    CHECK(mrfDotHas(
        dot,
        "node [style=\"filled\", fillcolor=\"coral\", fontcolor=\"black\", color=\"#4A4A4A\"];\n"));
    CHECK(mrfDotHas(dot, "edge [color=\"#4A4A4A\"];\n"));
    CHECK(mrfDotHas(dot, "edge [len=\"0.7\"];\n"));
    // the variables
    CHECK(mrfDotHas(dot,
                    "  \"a\" [shape=\"rectangle\", margin=\"0.04\", width=\"0\", height=\"0\", "
                    "tooltip=\"(0) a\"];\n"));
    // the factors, named after the variables in the order of their identifiers
    const std::string factor = "[fillcolor=\"burlywood\", fontcolor=\"black\", shape=\"point\", "
                               "width=\"0.1\", height=\"0.1\"];\n";
    CHECK(mrfDotHas(dot, "  \"f#a#b#c\" " + factor));
    CHECK(mrfDotHas(dot, "  \"f#a#c\" " + factor));
    CHECK(mrfDotHas(dot, "  \"f#c#d\" " + factor));
    // the links
    CHECK(mrfDotHas(dot, "  \"f#a#b#c\" -- \"a\";\n"));
    CHECK(mrfDotHas(dot, "  \"f#a#b#c\" -- \"b\";\n"));
    CHECK(mrfDotHas(dot, "  \"f#a#b#c\" -- \"c\";\n"));
    CHECK(mrfDotHas(dot, "  \"f#c#d\" -- \"d\";\n"));
    CHECK_EQ(mrf.toDotAsFactorGraph(gum::dot::DotOptions()), dot);
  }

  GUM_TEST(FactorGraphFollowsTheConfiguration) {
    ConfigurationGuard guard;
    const auto         mrf = mrfForDot();

    gum::config().set("factorgraph", "graphLayout", "sfdp");
    gum::config().set("factorgraph", "nodeBgColor", "#101010");
    gum::config().set("factorgraph", "nodeFgColor", "adaptive");
    gum::config().set("factorgraph", "factorBgColor", "yellow");
    gum::config().set("factorgraph", "edgeLength", "1.5");

    const auto dot = mrf.toDotAsFactorGraph();
    CHECK(mrfDotHas(dot, "layout=\"sfdp\""));
    CHECK(mrfDotHas(dot, "fillcolor=\"#101010\", fontcolor=\"white\""));
    CHECK(mrfDotHas(dot, "fillcolor=\"yellow\", fontcolor=\"black\", shape=\"point\""));
    CHECK(mrfDotHas(dot, "edge [len=\"1.5\"];\n"));
  }

  GUM_TEST(FactorGraphOptions) {
    ConfigurationGuard guard;
    const auto         mrf = mrfForDot();

    gum::dot::DotOptions options;
    options.setNodeBgColor(2, "#202020");
    options.setFactorBgColor({0, 2}, "red");
    options.setFactorTooltip({3, 2}, "c and d");

    const auto dot = mrf.toDotAsFactorGraph(options);
    CHECK(mrfDotHas(dot, "tooltip=\"(2) c\", fillcolor=\"#202020\", fontcolor=\"white\"];\n"));
    CHECK(mrfDotHas(dot,
                    "\"f#a#c\" [shape=\"point\", width=\"0.1\", height=\"0.1\", fillcolor=\"red\", "
                    "fontcolor=\"white\"];\n"));
    CHECK(mrfDotHas(
        dot,
        "\"f#c#d\" [fillcolor=\"burlywood\", fontcolor=\"black\", shape=\"point\", width=\"0.1\", "
        "height=\"0.1\", tooltip=\"c and d\"];\n"));
    // the factor on a, b and c is not concerned
    CHECK(mrfDotHas(dot,
                    "\"f#a#b#c\" [fillcolor=\"burlywood\", fontcolor=\"black\", shape=\"point\""));
  }

}   // namespace gum_tests
