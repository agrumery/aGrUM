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
#include <agrum/base/graphs/DAG.h>
#include <agrum/base/graphs/diGraph.h>
#include <agrum/base/graphs/mixedGraph.h>
#include <agrum/base/graphs/PAG.h>
#include <agrum/base/graphs/PDAG.h>
#include <agrum/base/graphs/undiGraph.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {

  // The look of the graphs drawn by toDot() is the one of the configuration (the same as pyAgrum
  // had), and the options given by the caller are written on top of it.
  struct GraphsToDotTestSuite {};

  namespace {
    bool dotContains(const std::string& dot, const std::string& text) {
      return dot.find(text) != std::string::npos;
    }

    // the defaults of the look of the graphs : the same as the ones of pyAgrum
    const std::string dotPreamble
        = "  graph [bgcolor=\"transparent\", size=\"5\", rankdir=\"TB\", layout=\"dot\"];\n"
          "  node [style=\"filled\", fillcolor=\"#404040\", fontcolor=\"white\", "
          "color=\"#4A4A4A\"];\n"
          "  edge [color=\"#4A4A4A\"];\n";
  }   // namespace

  GUM_TEST(DiGraphLook) {
    ConfigurationGuard guard;
    gum::DiGraph       g;
    g.addNodes(2);
    g.addArc(0, 1);

    CHECK_EQ(g.toDot(),
             "digraph {\n" + dotPreamble
                 + "  0;\n"
                   "  1;\n"
                   "  0 -> 1;\n"
                   "}\n");
    // without options, it is the same
    CHECK_EQ(g.toDot(gum::dot::DotOptions()), g.toDot());
  }

  GUM_TEST(LookFollowsTheConfiguration) {
    ConfigurationGuard guard;
    gum::DiGraph       g;
    g.addNodes(2);
    g.addArc(0, 1);

    // what setDarkTheme() does, and a few other keys
    gum::config().set("notebook", "arcColor", "#AAAAAA");
    gum::config().set("notebook", "graphSize", "12!");
    gum::config().set("notebook", "graphRankDir", "LR");
    gum::config().set("notebook", "nodeBgColor", "burlywood");
    gum::config().set("notebook", "nodeFgColor", "adaptive");

    const auto dot = g.toDot();
    CHECK(dotContains(
        dot,
        "graph [bgcolor=\"transparent\", size=\"12!\", rankdir=\"LR\", layout=\"dot\"];"));
    CHECK(dotContains(dot,
                      "node [style=\"filled\", fillcolor=\"burlywood\", fontcolor=\"black\", "
                      "color=\"#AAAAAA\"];"));
    CHECK(dotContains(dot, "edge [color=\"#AAAAAA\"];"));

    gum::config().set("notebook", "nodeFgColor", "red");
    CHECK(dotContains(g.toDot(), "fontcolor=\"red\""));
  }

  GUM_TEST(OptionsOfTheNodesAndTheArcs) {
    ConfigurationGuard guard;
    gum::DiGraph       g;
    g.addNodes(3);
    g.setName(1, "one");
    g.addArc(0, 1);
    g.addArc(1, 2);

    gum::dot::DotOptions options;
    options.setSize("8");
    options.setRankDir("LR");
    options.setLayout("neato");
    options.setLabel("my graph");
    options.setNodeBgColor(0, "#FF7F64");
    options.setNodeLabel(2, "two");
    options.setNodeTooltip(2, "(2) two");
    options.setArcWidth(0, 1, 2.5);
    options.setArcColor(0, 1, "green");
    options.setArcLabel(1, 2, "weight");

    const auto dot = g.toDot(options);
    CHECK(dotContains(dot, "size=\"8\", rankdir=\"LR\", layout=\"neato\", label=\"my graph\""));
    CHECK(dotContains(
        dot,
        "  0 [fillcolor=\"#FF7F64\", fontcolor=\"black\"];\n"));   // adaptive on this background
    CHECK(dotContains(dot, "  1 [label=\"(1) one\"];\n"));         // the name of the node
    CHECK(dotContains(
        dot,
        "  2 [label=\"two\", tooltip=\"(2) two\"];\n"));           // the label of the options wins
    CHECK(dotContains(dot, "  0 -> 1 [color=\"green\", penwidth=\"2.5\"];\n"));
    CHECK(dotContains(dot, "  1 -> 2 [label=\"weight\"];\n"));
  }

  GUM_TEST(UndiGraphLook) {
    ConfigurationGuard guard;
    gum::UndiGraph     g;
    g.addNodes(3);
    g.addEdge(0, 1);

    CHECK_EQ(g.toDot(),
             "graph {\n" + dotPreamble
                 + "  0;\n"
                   "  1;\n"
                   "  2;\n"
                   "  0 -- 1;\n"
                   "}\n");

    // an edge is designated by its two ends, in any order
    gum::dot::DotOptions options;
    options.setArcColor(1, 0, "red");
    CHECK(dotContains(g.toDot(options), "  0 -- 1 [color=\"red\"];\n"));
  }

  GUM_TEST(MixedGraphLook) {
    ConfigurationGuard guard;
    gum::MixedGraph    g;
    g.addNodes(3);
    g.addEdge(0, 1);
    g.addArc(1, 2);

    gum::dot::DotOptions options;
    options.setArcWidth(0, 1, 3);
    const auto dot = g.toDot(options);
    CHECK(dotContains(dot, "digraph {\n"));
    CHECK(dotContains(dot, "  0 -> 1 [dir=\"none\", penwidth=\"3\"];\n"));
    CHECK(dotContains(dot, "  1 -> 2;\n"));
  }

  GUM_TEST(PDAGLook) {
    ConfigurationGuard guard;
    gum::PDAG          g;
    g.addNodes(4);
    g.addEdge(0, 1);
    g.addArc(1, 2);

    const auto dot = g.toDot();
    CHECK(dotContains(dot, "digraph {\n"));
    // the nodes without undirected edge, then a cluster for the nodes linked by undirected edges
    CHECK(dotContains(dot, "  2;\n"));
    CHECK(dotContains(dot, "  3;\n"));
    CHECK(dotContains(dot,
                      "  subgraph \"cluster_0\" {\n"
                      "    style=\"filled\";\n"
                      "    color=\"#F5F5F5\";\n"
                      "    rank=\"same\";\n"));
    CHECK(dotContains(dot, "    0;\n"));
    CHECK(dotContains(dot, "    1;\n"));
    CHECK(dotContains(dot, "  1 -> 2;\n"));
    CHECK(dotContains(dot, "  0 -> 1 [dir=\"none\"];\n"));
    // the braces of the cluster are single
    CHECK_FALSE(dotContains(dot, "{{"));
    CHECK_FALSE(dotContains(dot, "}}"));
  }

  GUM_TEST(PAGLook) {
    ConfigurationGuard guard;
    gum::PAG           g;
    g.addNodes(3);
    g.addEdge(0, 1);
    g.setMarkAt(0, 1, gum::EdgeMark::Arrowhead);

    gum::dot::DotOptions options;
    options.setArcColor(1, 0, "blue");
    const auto dot = g.toDot(options);
    CHECK(dotContains(dot, "digraph {\n"));
    CHECK(dotContains(dot, "dir=\"both\", arrowhead=\""));
    CHECK(dotContains(dot, "color=\"blue\""));
  }

  GUM_TEST(NamesAreEscapedInLabels) {
    ConfigurationGuard guard;
    gum::UndiGraph     g;
    g.addNodes(2);
    g.setName(0, "a\"b\\c\nd");

    CHECK(dotContains(g.toDot(), "  0 [label=\"(0) a\\\"b\\\\c\\nd\"];\n"));
  }

}   // namespace gum_tests
