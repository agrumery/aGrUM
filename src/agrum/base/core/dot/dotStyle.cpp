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

/**
 * @file
 * @brief Implementation of the look of the graphs drawn with graphviz.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <cstdint>

#include <agrum/base/core/configuration/configuration.h>
#include <agrum/base/core/dot/dotColor.h>
#include <agrum/base/core/dot/dotStyle.h>

namespace gum::dot {

  namespace {
    std::string _str_(std::string_view section, std::string_view key) {
      return gum::config().get(section, key);
    }

    template < typename T >
    T _typed_(std::string_view section, std::string_view key) {
      return gum::config().typed< T >(section, key);
    }

    // the part of the style which comes from the section `dot`
    DotStyle _dot_() {
      DotStyle style;
      style.size                  = _str_("dot", "graphSize");
      style.rankDir               = _str_("dot", "graphRankDir");
      style.layout                = _str_("dot", "graphLayout");
      style.arcColor              = _str_("dot", "arcColor");
      style.nodeBgColor           = _str_("dot", "nodeBgColor");
      style.nodeFgColor           = configFgColor("dot", "nodeFgColor");
      style.chainComponentBgColor = _str_("dot", "chainComponentBgColor");
      style.compelledArcColor     = _str_("dot", "compelledArcColor");
      style.bidirectedEdgeColor   = _str_("dot", "bidirectedEdgeColor");
      style.circleMarkEdgeColor   = _str_("dot", "circleMarkEdgeColor");
      return style;
    }
  }   // namespace

  std::string configFgColor(std::string_view section, std::string_view key) {
    constexpr std::string_view fgSuffix = "FgColor";
    if (!key.ends_with(fgSuffix)) {
      GUM_ERROR(InvalidArgument, "The key '" << key << "' of a font color must end with FgColor")
    }
    const std::string bgKey = std::string(key.substr(0, key.size() - fgSuffix.size())) + "BgColor";
    return resolveFgColor(_str_(section, key), _str_(section, bgKey), _str_("dot", "arcColor"));
  }

  DotStyle DotStyle::read() { return _dot_(); }

  DotStyle DotStyle::readCredalNet() {
    DotStyle style    = _dot_();
    style.nodeBgColor = _str_("credalnet", "nodeBgColor");
    style.nodeFgColor = configFgColor("credalnet", "nodeFgColor");
    return style;
  }

  JunctionTreeDotStyle JunctionTreeDotStyle::read() {
    JunctionTreeDotStyle style;
    static_cast< DotStyle& >(style) = _dot_();
    style.size                      = _str_("junctionTree", "graphSize");

    style.withNames        = _typed_< bool >("junctionTree", "withNames");
    style.separatorBgColor = _str_("junctionTree", "separatorBgColor");
    style.separatorFgColor = configFgColor("junctionTree", "separatorFgColor");
    style.separatorFontSize
        = static_cast< int >(_typed_< std::int64_t >("junctionTree", "separatorFontSize"));
    style.cliqueBgColor = _str_("junctionTree", "cliqueBgColor");
    style.cliqueFgColor = configFgColor("junctionTree", "cliqueFgColor");
    style.cliqueFontSize
        = static_cast< int >(_typed_< std::int64_t >("junctionTree", "cliqueFontSize"));

    style.mapCliqueScale = _typed_< double >("junctionTree", "mapCliqueScale");
    style.mapSepScale    = _typed_< double >("junctionTree", "mapSepScale");
    style.mapEdgeLen     = _typed_< double >("junctionTree", "mapEdgeLen");
    style.mapSize        = _str_("junctionTree", "mapSize");
    return style;
  }

  FactorGraphDotStyle FactorGraphDotStyle::read() {
    FactorGraphDotStyle style;
    static_cast< DotStyle& >(style) = _dot_();
    style.layout                    = _str_("factorgraph", "graphLayout");
    style.nodeBgColor               = _str_("factorgraph", "nodeBgColor");
    style.nodeFgColor               = configFgColor("factorgraph", "nodeFgColor");

    style.factorBgColor = _str_("factorgraph", "factorBgColor");
    style.factorFgColor = resolveFgColor(ADAPTIVE_COLOR, style.factorBgColor, style.arcColor);
    style.edgeLength    = _typed_< double >("factorgraph", "edgeLength");
    return style;
  }

  InfluenceDiagramDotStyle InfluenceDiagramDotStyle::read() {
    InfluenceDiagramDotStyle style;
    static_cast< DotStyle& >(style) = _dot_();
    style.size                      = _str_("influenceDiagram", "graphSize");

    style.chanceBgColor = _str_("influenceDiagram", "chanceBgColor");
    style.chanceFgColor = configFgColor("influenceDiagram", "chanceFgColor");
    style.chanceShape   = _str_("influenceDiagram", "chanceShape");

    style.utilityBgColor = _str_("influenceDiagram", "utilityBgColor");
    style.utilityFgColor = configFgColor("influenceDiagram", "utilityFgColor");
    style.utilityShape   = _str_("influenceDiagram", "utilityShape");

    style.decisionBgColor = _str_("influenceDiagram", "decisionBgColor");
    style.decisionFgColor = configFgColor("influenceDiagram", "decisionFgColor");
    style.decisionShape   = _str_("influenceDiagram", "decisionShape");

    style.decisionArcStyle = _str_("influenceDiagram", "decisionArcStyle");
    style.utilityArcStyle  = _str_("influenceDiagram", "utilityArcStyle");
    return style;
  }

  CausalDotStyle CausalDotStyle::read() {
    CausalDotStyle style;
    static_cast< DotStyle& >(style) = _dot_();
    style.size                      = _str_("causal", "graphSize");
    style.nodeBgColor               = _str_("causal", "nodeBgColor");
    style.nodeFgColor               = configFgColor("causal", "nodeFgColor");

    style.latentBgColor   = _str_("causal", "latentBgColor");
    style.latentFgColor   = configFgColor("causal", "latentFgColor");
    style.showLatentNames = _typed_< bool >("causal", "showLatentNames");
    return style;
  }

  DynamicBNDotStyle DynamicBNDotStyle::read() {
    DynamicBNDotStyle style;
    static_cast< DotStyle& >(style) = _dot_();
    style.size                      = _str_("dynamicBN", "graphSize");

    style.timeSliceBgColor       = _str_("dynamicBN", "timeSliceBgColor");
    style.replicatedSliceBgColor = _str_("dynamicBN", "replicatedSliceBgColor");
    style.atemporalBgColor       = _str_("dynamicBN", "atemporalBgColor");
    return style;
  }

}   // namespace gum::dot
