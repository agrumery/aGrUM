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
#include <agrum/ID/influenceDiagram.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {

  // The influence diagrams are drawn as pyAgrum drew them : one look for the chance nodes, one for
  // the decision nodes and one for the utility nodes, the ones of the section influenceDiagram.
  struct InfluenceDiagramToDotTestSuite {};

  namespace {
    bool idDotHas(const std::string& dot, const std::string& text) {
      return dot.find(text) != std::string::npos;
    }
  }   // namespace

  GUM_TEST(InfluenceDiagramLook) {
    ConfigurationGuard guard;
    auto               diag = gum::InfluenceDiagram< double >::fastPrototype("A->B->*D->$U;B->$U");

    const auto dot = diag.toDot();
    CHECK(idDotHas(dot, "digraph "));
    CHECK(
        idDotHas(dot,
                 "graph [bgcolor=\"transparent\", size=\"6\", rankdir=\"TB\", layout=\"dot\"];\n"));
    // the nodes of each kind, after the default look of the kind
    CHECK(idDotHas(
        dot,
        "node [fillcolor=\"#808080\", fontcolor=\"white\", style=\"filled\", shape=\"ellipse\", "
        "height=\"0\", margin=\"0.1\"];\n  \"A\" [tooltip=\"(0) A\"];\n  \"B\" [tooltip=\"(1) "
        "B\"];\n"));
    CHECK(idDotHas(
        dot,
        "node [fillcolor=\"#9A5050\", fontcolor=\"white\", style=\"filled\", shape=\"box\", "
        "height=\"0\", margin=\"0.1\"];\n  \"D\" [tooltip=\"(2) D\"];\n"));
    CHECK(idDotHas(
        dot,
        "node [fillcolor=\"#50508A\", fontcolor=\"white\", style=\"filled\", shape=\"hexagon\", "
        "height=\"0\", margin=\"0.1\"];\n  \"U\" [tooltip=\"(3) U\"];\n"));
    // the arcs : towards a decision node, towards a utility node, and the others
    CHECK(idDotHas(dot, "  \"A\" -> \"B\";\n"));
    CHECK(idDotHas(dot, "  \"B\" -> \"D\" [style=\"tapered, bold, dotted\"];\n"));
    CHECK(idDotHas(dot, "  \"B\" -> \"U\" [style=\"dashed\"];\n"));
    CHECK(idDotHas(dot, "  \"D\" -> \"U\" [style=\"dashed\"];\n"));
    CHECK_EQ(diag.toDot(gum::dot::DotOptions()), dot);
  }

  GUM_TEST(InfluenceDiagramFollowsTheConfiguration) {
    ConfigurationGuard guard;
    auto               diag = gum::InfluenceDiagram< double >::fastPrototype("A->*D->$U");

    gum::config().set("influenceDiagram", "graphSize", "9");
    gum::config().set("influenceDiagram", "chanceBgColor", "yellow");
    gum::config().set("influenceDiagram", "chanceFgColor", "adaptive");
    gum::config().set("influenceDiagram", "chanceShape", "circle");
    gum::config().set("influenceDiagram", "decisionBgColor", "#101010");
    gum::config().set("influenceDiagram", "decisionFgColor", "adaptive");
    gum::config().set("influenceDiagram", "decisionShape", "box3d");
    gum::config().set("influenceDiagram", "utilityBgColor", "navy");
    gum::config().set("influenceDiagram", "utilityFgColor", "pink");
    gum::config().set("influenceDiagram", "utilityShape", "star");
    gum::config().set("influenceDiagram", "decisionArcStyle", "bold");
    gum::config().set("influenceDiagram", "utilityArcStyle", "dotted");

    const auto dot = diag.toDot();
    CHECK(idDotHas(dot, "size=\"9\""));
    CHECK(idDotHas(
        dot,
        "node [fillcolor=\"yellow\", fontcolor=\"black\", style=\"filled\", shape=\"circle\""));
    CHECK(idDotHas(
        dot,
        "node [fillcolor=\"#101010\", fontcolor=\"white\", style=\"filled\", shape=\"box3d\""));
    CHECK(
        idDotHas(dot,
                 "node [fillcolor=\"navy\", fontcolor=\"pink\", style=\"filled\", shape=\"star\""));
    CHECK(idDotHas(dot, "\"A\" -> \"D\" [style=\"bold\"];\n"));
    CHECK(idDotHas(dot, "\"D\" -> \"U\" [style=\"dotted\"];\n"));
  }

  GUM_TEST(InfluenceDiagramOptions) {
    ConfigurationGuard guard;
    auto               diag = gum::InfluenceDiagram< double >::fastPrototype("A->B->*D->$U;B->$U");

    gum::dot::DotOptions options;
    options.setSize("12!");
    options.setNodeBgColor(0, "burlywood");
    options.setNodeTooltip(2, "the decision");
    options.setArcColor(1, 2, "green");
    options.setArcWidth(2, 3, 2);

    const auto dot = diag.toDot(options);
    CHECK(idDotHas(dot, "size=\"12!\""));
    CHECK(idDotHas(dot,
                   "\"A\" [tooltip=\"(0) A\", fillcolor=\"burlywood\", fontcolor=\"black\"];\n"));
    CHECK(idDotHas(dot, "\"D\" [tooltip=\"the decision\"];\n"));
    // the options and the style of the arc
    CHECK(idDotHas(dot, "\"B\" -> \"D\" [style=\"tapered, bold, dotted\", color=\"green\"];\n"));
    CHECK(idDotHas(dot, "\"D\" -> \"U\" [style=\"dashed\", penwidth=\"2\"];\n"));
    // a style given by the caller wins
    options.setArcAttribute(0, 1, "style", "bold");
    CHECK(idDotHas(diag.toDot(options), "\"A\" -> \"B\" [style=\"bold\"];\n"));
  }

}   // namespace gum_tests
