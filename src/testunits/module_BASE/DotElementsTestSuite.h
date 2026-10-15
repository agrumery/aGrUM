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
#include <agrum/base/core/dot/dotElements.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {

  struct DotElementsTestSuite {};

  namespace {
    // the look of the graphs, whatever the defaults of the configuration
    gum::dot::DotStyle dotElementsStyle() {
      gum::dot::DotStyle style;
      style.size        = "5";
      style.rankDir     = "TB";
      style.layout      = "dot";
      style.arcColor    = "#4A4A4A";
      style.nodeBgColor = "#404040";
      style.nodeFgColor = "white";
      return style;
    }

    bool hasAttribute(const gum::dot::DotAttributes& attributes,
                      const std::string&             key,
                      const std::string&             value) {
      for (const auto& a: attributes)
        if (a.key == key && a.value == value) return true;
      return false;
    }

    bool hasKey(const gum::dot::DotAttributes& attributes, const std::string& key) {
      for (const auto& a: attributes)
        if (a.key == key) return true;
      return false;
    }
  }   // namespace

  GUM_TEST(PreambleOfTheStyle) {
    gum::dot::DotWriter w;
    gum::dot::writePreamble(w, dotElementsStyle(), gum::dot::DotOptions());
    CHECK_EQ(w.str(),
             "digraph {\n"
             "  graph [bgcolor=\"transparent\", size=\"5\", rankdir=\"TB\", layout=\"dot\"];\n"
             "  node [style=\"filled\", fillcolor=\"#404040\", fontcolor=\"white\", "
             "color=\"#4A4A4A\"];\n"
             "  edge [color=\"#4A4A4A\"];\n"
             "}\n");
  }

  GUM_TEST(PreambleWithOptionsAndLabel) {
    gum::dot::DotOptions options;
    options.setSize("12!");
    options.setRankDir("LR");
    options.setLayout("neato");

    gum::dot::DotWriter w;
    gum::dot::writePreamble(w, dotElementsStyle(), options, "my model");
    const auto dot = w.str();
    CHECK_NE(dot.find("size=\"12!\", rankdir=\"LR\", layout=\"neato\", label=\"my model\", "
                      "fontcolor=\"#4A4A4A\"]"),
             std::string::npos);

    // the label of the options wins
    options.setLabel("given");
    gum::dot::DotWriter v;
    gum::dot::writePreamble(v, dotElementsStyle(), options, "my model");
    CHECK_NE(v.str().find("label=\"given\""), std::string::npos);
    CHECK_EQ(v.str().find("my model"), std::string::npos);
  }

  GUM_TEST(PreambleFollowsTheConfiguration) {
    ConfigurationGuard guard;
    gum::config().set("notebook", "arcColor", "#AAAAAA");
    gum::config().set("notebook", "graphSize", "8");
    gum::config().set("notebook", "nodeFgColor", "adaptive");
    gum::config().set("notebook", "nodeBgColor", "burlywood");

    gum::dot::DotWriter w;
    gum::dot::writePreamble(w, gum::dot::DotStyle::read(), gum::dot::DotOptions());
    CHECK_EQ(w.str(),
             "digraph {\n"
             "  graph [bgcolor=\"transparent\", size=\"8\", rankdir=\"TB\", layout=\"dot\"];\n"
             "  node [style=\"filled\", fillcolor=\"burlywood\", fontcolor=\"black\", "
             "color=\"#AAAAAA\"];\n"
             "  edge [color=\"#AAAAAA\"];\n"
             "}\n");
  }

  GUM_TEST(NodeAttributesOfNothing) {
    const auto style = dotElementsStyle();
    CHECK(gum::dot::nodeAttributes(style, nullptr).empty());
    CHECK(gum::dot::nodeAttributes(style, nullptr, "").empty());
    CHECK(gum::dot::nodeAttributes(style, nullptr, "(0) A").size() == 1);
    CHECK(hasAttribute(gum::dot::nodeAttributes(style, nullptr, "(0) A"), "label", "(0) A"));

    // the default label and tooltip of a node
    const auto both = gum::dot::nodeAttributes(style, nullptr, "A", "(0) A");
    CHECK(both.size() == 2);
    CHECK(hasAttribute(both, "label", "A"));
    CHECK(hasAttribute(both, "tooltip", "(0) A"));
    gum::dot::DotNodeOptions given;
    given.tooltip = "mine";
    CHECK(hasAttribute(gum::dot::nodeAttributes(style, &given, "A", "(0) A"), "tooltip", "mine"));
    CHECK(hasAttribute(gum::dot::nodeAttributes(style, &given, "A", "(0) A"), "label", "A"));

    const gum::dot::DotNodeOptions empty;
    CHECK(gum::dot::nodeAttributes(style, &empty).empty());
  }

  GUM_TEST(NodeAttributes) {
    const auto style = dotElementsStyle();

    gum::dot::DotNodeOptions options;
    options.label   = "three";
    options.tooltip = "(3) three";
    options.attributes.push_back({"shape", "box"});
    auto attributes = gum::dot::nodeAttributes(style, &options, "(3) default");
    CHECK(hasAttribute(attributes, "label", "three"));   // the label of the options wins
    CHECK(hasAttribute(attributes, "tooltip", "(3) three"));
    CHECK(hasAttribute(attributes, "shape", "box"));
    CHECK(attributes.back().key == "shape");             // the other attributes come last
    CHECK_FALSE(hasKey(attributes, "fillcolor"));
    CHECK_FALSE(hasKey(attributes, "fontcolor"));

    // a background without font color : the font color is adaptive
    gum::dot::DotNodeOptions dark;
    dark.bgColor = "#202020";
    attributes   = gum::dot::nodeAttributes(style, &dark);
    CHECK(hasAttribute(attributes, "fillcolor", "#202020"));
    CHECK(hasAttribute(attributes, "fontcolor", "white"));

    gum::dot::DotNodeOptions light;
    light.bgColor = "#FF7F64";
    CHECK(hasAttribute(gum::dot::nodeAttributes(style, &light), "fontcolor", "black"));

    // a font color is respected, or resolved if it is adaptive
    gum::dot::DotNodeOptions red;
    red.bgColor = "#202020";
    red.fgColor = "red";
    CHECK(hasAttribute(gum::dot::nodeAttributes(style, &red), "fontcolor", "red"));
    red.fgColor = "adaptive";
    CHECK(hasAttribute(gum::dot::nodeAttributes(style, &red), "fontcolor", "white"));

    // a font color alone is drawn on the default background
    gum::dot::DotNodeOptions fg;
    fg.fgColor = "yellow";
    attributes = gum::dot::nodeAttributes(style, &fg);
    CHECK_FALSE(hasKey(attributes, "fillcolor"));
    CHECK(hasAttribute(attributes, "fontcolor", "yellow"));
    fg.fgColor = "adaptive";
    CHECK(hasAttribute(gum::dot::nodeAttributes(style, &fg), "fontcolor", "white"));   // on #404040

    // a background which is not a color : the color of the arcs
    gum::dot::DotNodeOptions transparent;
    transparent.bgColor = "transparent";
    CHECK(hasAttribute(gum::dot::nodeAttributes(style, &transparent), "fontcolor", "#4A4A4A"));
  }

  GUM_TEST(ArcAttributes) {
    CHECK(gum::dot::arcAttributes(nullptr).empty());
    const gum::dot::DotArcOptions empty;
    CHECK(gum::dot::arcAttributes(&empty).empty());

    gum::dot::DotArcOptions options;
    options.color   = "green";
    options.width   = 2.5;
    options.label   = "weight";
    options.tooltip = "0 -> 1";
    options.attributes.push_back({"style", "dashed"});
    const auto attributes = gum::dot::arcAttributes(&options);
    CHECK(attributes.size() == 5);
    CHECK(hasAttribute(attributes, "color", "green"));
    CHECK(hasAttribute(attributes, "penwidth", "2.5"));
    CHECK(hasAttribute(attributes, "label", "weight"));
    CHECK(hasAttribute(attributes, "tooltip", "0 -> 1"));
    CHECK(attributes.back().key == "style");

    // a default attribute is added in front, unless the key is already given
    auto given = gum::dot::arcAttributes(&options);
    gum::dot::addDefaultAttribute(given, "color", "red");
    CHECK(hasAttribute(given, "color", "green"));
    CHECK_FALSE(hasAttribute(given, "color", "red"));
    gum::dot::DotAttributes none;
    gum::dot::addDefaultAttribute(none, "color", "red");
    gum::dot::addDefaultAttribute(none, "dir", "none");
    CHECK(none.size() == 2);
    CHECK(none[0].key == "dir");
    gum::dot::addDefaultAttribute(none, "color", "blue");
    CHECK(none.size() == 2);
    CHECK(hasAttribute(none, "color", "red"));

    // a width of 0 is a width
    gum::dot::DotArcOptions zero;
    zero.width = 0.0;
    CHECK(hasAttribute(gum::dot::arcAttributes(&zero), "penwidth", "0"));
  }

}   // namespace gum_tests
