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
#include <utility>
#include <vector>

#include <agrum/base/core/configuration/configuration.h>
#include <agrum/base/core/dot/dotStyle.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {

  struct DotStyleTestSuite {};

  namespace {
    void setDotKey(const char* section, const char* key, const std::string& value) {
      gum::config().set(section, key, value);
    }
  }   // namespace

  // the style is a copy of the values of the configuration, taken when read() is called
  GUM_TEST(StyleOfTheGraphsFollowsTheConfiguration) {
    ConfigurationGuard guard;

    for (const auto& [key, value]: std::vector< std::pair< std::string, std::string > >{
             {"graphSize", "7"},
             {"graphRankDir", "LR"},
             {"graphLayout", "circo"},
             {"arcColor", "#123456"},
             {"nodeBgColor", "#654321"},
             {"nodeFgColor", "orange"},
         }) {
      setDotKey("notebook", key.c_str(), value);
    }

    auto style = gum::dot::DotStyle::read();
    CHECK_EQ(style.size, "7");
    CHECK_EQ(style.rankDir, "LR");
    CHECK_EQ(style.layout, "circo");
    CHECK_EQ(style.arcColor, "#123456");
    CHECK_EQ(style.nodeBgColor, "#654321");
    CHECK_EQ(style.nodeFgColor, "orange");

    // a later change is seen by the next call, not by the style already read
    setDotKey("notebook", "graphSize", "12!");
    CHECK_EQ(style.size, "7");
    CHECK_EQ(gum::dot::DotStyle::read().size, "12!");
  }

  GUM_TEST(DarkAndLightThemeOnlyChangeTheArcColor) {
    ConfigurationGuard guard;
    // what setDarkTheme() and setLightTheme() of pyAgrum do
    setDotKey("notebook", "arcColor", "#AAAAAA");
    CHECK_EQ(gum::dot::DotStyle::read().arcColor, "#AAAAAA");
    setDotKey("notebook", "arcColor", "#4A4A4A");
    CHECK_EQ(gum::dot::DotStyle::read().arcColor, "#4A4A4A");
  }

  GUM_TEST(AdaptiveFontColor) {
    ConfigurationGuard guard;
    setDotKey("notebook", "nodeFgColor", "adaptive");

    setDotKey("notebook", "nodeBgColor", "#404040");
    CHECK_EQ(gum::dot::DotStyle::read().nodeFgColor, "white");
    setDotKey("notebook", "nodeBgColor", "burlywood");
    CHECK_EQ(gum::dot::DotStyle::read().nodeFgColor, "black");

    // a background which is not a color: the color of the arcs, so the theme
    setDotKey("notebook", "nodeBgColor", "transparent");
    setDotKey("notebook", "arcColor", "#AAAAAA");
    CHECK_EQ(gum::dot::DotStyle::read().nodeFgColor, "#AAAAAA");

    // an explicit color is never changed
    setDotKey("notebook", "nodeFgColor", "red");
    setDotKey("notebook", "nodeBgColor", "#404040");
    CHECK_EQ(gum::dot::DotStyle::read().nodeFgColor, "red");
  }

  GUM_TEST(CredalNetStyle) {
    ConfigurationGuard guard;
    setDotKey("notebook", "graphSize", "3");
    setDotKey("notebook", "nodeBgColor", "yellow");
    setDotKey("credalnet", "nodeBgColor", "#202020");
    setDotKey("credalnet", "nodeFgColor", "adaptive");

    const auto style = gum::dot::DotStyle::readCredalNet();
    CHECK_EQ(style.size, "3");
    CHECK_EQ(style.nodeBgColor, "#202020");
    CHECK_EQ(style.nodeFgColor, "white");
    // the notebook style is not changed
    CHECK_EQ(gum::dot::DotStyle::read().nodeBgColor, "yellow");
  }

  GUM_TEST(JunctionTreeStyle) {
    ConfigurationGuard guard;
    setDotKey("notebook", "junctionTreeGraphSize", "11");
    setDotKey("notebook", "junctionTreeWithNames", "false");
    setDotKey("notebook", "junctionTreeSeparatorBgColor", "#000000");
    setDotKey("notebook", "junctionTreeSeparatorFgColor", "adaptive");
    setDotKey("notebook", "junctionTreeSeparatorFontSize", "9");
    setDotKey("notebook", "junctionTreeCliqueBgColor", "white");
    setDotKey("notebook", "junctionTreeCliqueFgColor", "adaptive");
    setDotKey("notebook", "junctionTreeCliqueFontSize", "12");
    setDotKey("notebook", "junctionTreeMapCliqueScale", "0.5");
    setDotKey("notebook", "junctionTreeMapSepScale", "0.25");
    setDotKey("notebook", "junctionTreeMapEdgeLen", "2");
    setDotKey("notebook", "junctionTreeMapSize", "13");

    const auto style = gum::dot::JunctionTreeDotStyle::read();
    CHECK_EQ(style.size, "11");   // the size of the tree, not the one of the graphs
    CHECK_FALSE(style.withNames);
    CHECK_EQ(style.separatorBgColor, "#000000");
    CHECK_EQ(style.separatorFgColor, "white");
    CHECK_EQ(style.separatorFontSize, 9);
    CHECK_EQ(style.cliqueBgColor, "white");
    CHECK_EQ(style.cliqueFgColor, "black");
    CHECK_EQ(style.cliqueFontSize, 12);
    CHECK_EQ(style.mapCliqueScale, 0.5);
    CHECK_EQ(style.mapSepScale, 0.25);
    CHECK_EQ(style.mapEdgeLen, 2.0);
    CHECK_EQ(style.mapSize, "13");
    // the common part is the one of the notebook
    CHECK_EQ(style.rankDir, gum::dot::DotStyle::read().rankDir);
    CHECK_EQ(style.arcColor, gum::dot::DotStyle::read().arcColor);
  }

  GUM_TEST(FactorGraphStyle) {
    ConfigurationGuard guard;
    setDotKey("notebook", "graphSize", "4");
    setDotKey("notebook", "graphLayout", "dot");
    setDotKey("factorgraph", "graphLayout", "sfdp");
    setDotKey("factorgraph", "nodeBgColor", "coral");
    setDotKey("factorgraph", "nodeFgColor", "adaptive");
    setDotKey("factorgraph", "factorBgColor", "#101010");
    setDotKey("factorgraph", "edgeLength", "1.5");

    const auto style = gum::dot::FactorGraphDotStyle::read();
    CHECK_EQ(style.size, "4");        // the size of the notebook
    CHECK_EQ(style.layout, "sfdp");   // the layout of the factor graphs
    CHECK_EQ(style.nodeBgColor, "coral");
    CHECK_EQ(style.nodeFgColor, "black");
    CHECK_EQ(style.factorBgColor, "#101010");
    CHECK_EQ(style.factorFgColor, "white");
    CHECK_EQ(style.edgeLength, 1.5);
  }

  GUM_TEST(InfluenceDiagramStyle) {
    ConfigurationGuard guard;
    setDotKey("influenceDiagram", "graphSize", "8");
    setDotKey("influenceDiagram", "chanceBgColor", "#808080");
    setDotKey("influenceDiagram", "chanceFgColor", "adaptive");
    setDotKey("influenceDiagram", "chanceShape", "circle");
    setDotKey("influenceDiagram", "utilityBgColor", "yellow");
    setDotKey("influenceDiagram", "utilityFgColor", "adaptive");
    setDotKey("influenceDiagram", "utilityShape", "star");
    setDotKey("influenceDiagram", "decisionBgColor", "navy");
    setDotKey("influenceDiagram", "decisionFgColor", "pink");
    setDotKey("influenceDiagram", "decisionShape", "box3d");
    setDotKey("influenceDiagram", "decisionArcStyle", "bold");
    setDotKey("influenceDiagram", "utilityArcStyle", "dotted");

    const auto style = gum::dot::InfluenceDiagramDotStyle::read();
    CHECK_EQ(style.size, "8");
    CHECK_EQ(style.chanceBgColor, "#808080");
    CHECK_EQ(style.chanceFgColor, "white");
    CHECK_EQ(style.chanceShape, "circle");
    CHECK_EQ(style.utilityBgColor, "yellow");
    CHECK_EQ(style.utilityFgColor, "black");
    CHECK_EQ(style.utilityShape, "star");
    CHECK_EQ(style.decisionBgColor, "navy");
    CHECK_EQ(style.decisionFgColor, "pink");
    CHECK_EQ(style.decisionShape, "box3d");
    CHECK_EQ(style.decisionArcStyle, "bold");
    CHECK_EQ(style.utilityArcStyle, "dotted");
  }

  GUM_TEST(CausalStyle) {
    ConfigurationGuard guard;
    setDotKey("causal", "graphSize", "1.9");
    setDotKey("causal", "nodeBgColor", "#404040");
    setDotKey("causal", "nodeFgColor", "adaptive");
    setDotKey("causal", "latentBgColor", "#A08080");
    setDotKey("causal", "latentFgColor", "adaptive");
    setDotKey("causal", "showLatentNames", "true");

    const auto style = gum::dot::CausalDotStyle::read();
    CHECK_EQ(style.size, "1.9");
    CHECK_EQ(style.nodeBgColor, "#404040");
    CHECK_EQ(style.nodeFgColor, "white");
    CHECK_EQ(style.latentBgColor, "#A08080");
    CHECK_EQ(style.latentFgColor, "white");   // brightness 137
    CHECK(style.showLatentNames);
  }

  GUM_TEST(DynamicBNStyle) {
    ConfigurationGuard guard;
    setDotKey("dynamicBN", "graphSize", "9");
    setDotKey("dynamicBN", "timeSliceBgColor", "#EEEEEE");
    setDotKey("dynamicBN", "replicatedSliceBgColor", "pink");
    setDotKey("dynamicBN", "atemporalBgColor", "ivory");

    const auto style = gum::dot::DynamicBNDotStyle::read();
    CHECK_EQ(style.size, "9");
    CHECK_EQ(style.timeSliceBgColor, "#EEEEEE");
    CHECK_EQ(style.replicatedSliceBgColor, "pink");
    CHECK_EQ(style.atemporalBgColor, "ivory");
  }

  // all the keys read by the styles exist in the defaults of aGrUM (not only in the ones of
  // pyAgrum)
  GUM_TEST(StylesOfTheDefaults) {
    ConfigurationGuard guard;
    CHECK_NOTHROW(gum::dot::DotStyle::read());
    CHECK_NOTHROW(gum::dot::DotStyle::readCredalNet());
    CHECK_NOTHROW(gum::dot::JunctionTreeDotStyle::read());
    CHECK_NOTHROW(gum::dot::FactorGraphDotStyle::read());
    CHECK_NOTHROW(gum::dot::InfluenceDiagramDotStyle::read());
    CHECK_NOTHROW(gum::dot::CausalDotStyle::read());
    CHECK_NOTHROW(gum::dot::DynamicBNDotStyle::read());

    // the style of the defaults, as pyAgrum drew the graphs
    const auto style = gum::dot::DotStyle::read();
    CHECK_EQ(style.size, "5");
    CHECK_EQ(style.rankDir, "TB");
    CHECK_EQ(style.layout, "dot");
    CHECK_EQ(style.arcColor, "#4A4A4A");
    CHECK_EQ(style.nodeBgColor, "#404040");
    CHECK_EQ(style.nodeFgColor, "white");
  }

}   // namespace gum_tests
