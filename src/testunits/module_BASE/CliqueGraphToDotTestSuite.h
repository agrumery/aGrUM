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
#include <agrum/base/graphs/cliqueGraph.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {

  // The junction trees are drawn as the cliques and the separators were in pyAgrum : the look is
  // the one of the keys junctionTree* of the configuration.
  struct CliqueGraphToDotTestSuite {};

  namespace {
    gum::CliqueGraph dotJunctionTree() {
      gum::CliqueGraph jt;
      jt.addNodeWithId(0, gum::NodeSet{0, 1, 2, 3});
      jt.addNodeWithId(1, gum::NodeSet{3, 4});
      jt.addEdge(0, 1);
      return jt;
    }

    bool dotHas(const std::string& dot, const std::string& text) {
      return dot.find(text) != std::string::npos;
    }
  }   // namespace

  GUM_TEST(JunctionTreeLook) {
    ConfigurationGuard guard;
    const auto         dot = dotJunctionTree().toDot();

    CHECK(dotHas(dot, "graph {\n"));
    CHECK(
        dotHas(dot,
               "graph [bgcolor=\"transparent\", size=\"10\", rankdir=\"TB\", layout=\"dot\"];\n"));
    // a clique : the keys junctionTreeClique*
    CHECK(dotHas(dot,
                 "  \"0\" [fillcolor=\"burlywood\", fontcolor=\"black\", fontsize=\"10\", "
                 "label=\"0-1-2-3\", "
                 "tooltip=\"size : 4\\n0\\n1\\n2\\n3\"];\n"));
    CHECK(dotHas(
        dot,
        "  \"1\" [fillcolor=\"burlywood\", fontcolor=\"black\", fontsize=\"10\", label=\"3-4\", "
        "tooltip=\"size : 2\\n3\\n4\"];\n"));
    // the separator : the keys junctionTreeSeparator*
    CHECK(dotHas(dot,
                 "  \"0~1\" [fillcolor=\"palegreen\", fontcolor=\"black\", fontsize=\"8\", "
                 "shape=\"box\", width=\"0\", "
                 "height=\"0\", margin=\"0.02\", label=\"3\", tooltip=\"size : 1\\n3\"];\n"));
    // c1 -- separator -- c2
    CHECK(dotHas(dot, "  \"0\" -- \"0~1\";\n"));
    CHECK(dotHas(dot, "  \"0~1\" -- \"1\";\n"));
    // without options, it is the same
    CHECK_EQ(dotJunctionTree().toDot(gum::dot::DotOptions()), dotJunctionTree().toDot());
  }

  GUM_TEST(JunctionTreeFollowsTheConfiguration) {
    ConfigurationGuard guard;
    gum::config().set("notebook", "junctionTreeGraphSize", "12!");
    gum::config().set("notebook", "junctionTreeCliqueBgColor", "#101010");
    gum::config().set("notebook", "junctionTreeCliqueFgColor", "adaptive");
    gum::config().set("notebook", "junctionTreeCliqueFontSize", "14");
    gum::config().set("notebook", "junctionTreeSeparatorBgColor", "yellow");
    gum::config().set("notebook", "junctionTreeSeparatorFgColor", "red");
    gum::config().set("notebook", "junctionTreeSeparatorFontSize", "6");
    gum::config().set("notebook", "arcColor", "#AAAAAA");

    const auto dot = dotJunctionTree().toDot();
    CHECK(dotHas(dot, "size=\"12!\""));
    CHECK(dotHas(dot, "edge [color=\"#AAAAAA\"];"));
    CHECK(dotHas(dot, "\"0\" [fillcolor=\"#101010\", fontcolor=\"white\", fontsize=\"14\""));
    CHECK(dotHas(dot, "\"0~1\" [fillcolor=\"yellow\", fontcolor=\"red\", fontsize=\"6\""));
  }

  GUM_TEST(JunctionTreeOptions) {
    ConfigurationGuard   guard;
    gum::dot::DotOptions options;
    // the names of the variables instead of their identifiers (given by the language layer)
    options.setNodeLabel(0, "A B C D");
    options.setNodeLabel(1, "D E");
    options.setArcLabel(0, 1, "D");
    options.setArcColor(0, 1, "#101010");
    options.setArcWidth(0, 1, 2);

    const auto dot = dotJunctionTree().toDot(options);
    CHECK(dotHas(dot, "label=\"A B C D\""));
    CHECK(dotHas(dot, "label=\"D E\""));
    CHECK_FALSE(dotHas(dot, "label=\"0-1-2-3\""));
    // the separator : its label and its fill color, with a font color which is adaptive
    CHECK(dotHas(dot, "fillcolor=\"#101010\", fontcolor=\"white\""));
    CHECK(dotHas(dot, "fontsize=\"8\", shape=\"box\""));
    CHECK(dotHas(dot, "label=\"D\""));
    // the width of the edge is the one of the two links
    CHECK(dotHas(dot, "  \"0\" -- \"0~1\" [penwidth=\"2\"];\n"));
    CHECK(dotHas(dot, "  \"0~1\" -- \"1\" [penwidth=\"2\"];\n"));
  }

  GUM_TEST(MapOfAJunctionTree) {
    ConfigurationGuard guard;
    const auto         dot = dotJunctionTree().mapToDot();

    CHECK(dotHas(dot, "graph {\n"));
    CHECK(dotHas(dot, "graph [bgcolor=\"transparent\", layout=\"neato\", size=\"10\"];\n"));
    CHECK(dotHas(dot, "node [shape=\"point\", style=\"filled\", fillcolor=\"burlywood\"];\n"));
    CHECK(dotHas(dot, "edge [len=\"1\", color=\"#4A4A4A\"];\n"));
    // the width of the cliques : 0.3 by node, the one of the separators 0.1 by node
    CHECK(dotHas(dot, "  \"0\" [tooltip=\"size : 4\\n0\\n1\\n2\\n3\", width=\"1.2\"];\n"));
    CHECK(dotHas(dot, "  \"1\" [tooltip=\"size : 2\\n3\\n4\", width=\"0.6\"];\n"));
    CHECK(dotHas(
        dot,
        "node [shape=\"square\", style=\"filled\", fillcolor=\"palegreen\", label=\"\"];\n"));
    CHECK(dotHas(dot, "  \"0~1\" [tooltip=\"size : 1\\n3\", width=\"0.1\"];\n"));
    CHECK(dotHas(dot, "  \"0\" -- \"0~1\";\n"));
    CHECK(dotHas(dot, "  \"0~1\" -- \"1\";\n"));
  }

  GUM_TEST(MapWithTheValuesOfTheCaller) {
    ConfigurationGuard guard;
    const auto         jt = dotJunctionTree();

    // the values of the configuration are the defaults of the arguments
    CHECK_EQ(jt.mapToDot(0.3, 0.1, 1.0), jt.mapToDot());
    CHECK_EQ(jt.mapToDot(0.3, 0.1, 1.0, "burlywood", "palegreen"), jt.mapToDot());

    const auto dot = jt.mapToDot(2, 0.5, 3, "red", "blue");
    CHECK(dotHas(dot, "fillcolor=\"red\""));
    CHECK(dotHas(dot, "fillcolor=\"blue\""));
    CHECK(dotHas(dot, "len=\"3\""));
    CHECK(dotHas(dot, "width=\"8\""));     // 2 * 4 nodes
    CHECK(dotHas(dot, "width=\"0.5\""));   // 0.5 * 1 node

    // and the configuration changes the map
    gum::config().set("notebook", "junctionTreeMapCliqueScale", "1");
    gum::config().set("notebook", "junctionTreeMapEdgeLen", "2");
    gum::config().set("notebook", "junctionTreeCliqueBgColor", "pink");
    const auto changed = jt.mapToDot();
    CHECK(dotHas(changed, "width=\"4\""));
    CHECK(dotHas(changed, "len=\"2\""));
    CHECK(dotHas(changed, "fillcolor=\"pink\""));
  }

}   // namespace gum_tests
