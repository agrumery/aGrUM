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
 * @brief Look of the graphs drawn with graphviz, read from the configuration.
 *
 * The look of a graph is given by keys of gum::Configuration (sections `notebook`, `factorgraph`,
 * `dynamicBN`, `influenceDiagram`, `credalnet` and `causal`). A style is a copy of the values of
 * these keys, taken at the moment of the call to `read()` : a `toDot()` reads the style once, when
 * it is called, and not when the graph is built, so that it follows the changes of the
 * configuration (the light or dark theme for instance). The font colors of a style are never
 * `adaptive`: this special value is already resolved (see gum::dot::resolveFgColor) against the
 * background color of the same elements, with the color of the arcs when this background is not a
 * color that can be interpreted.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_DOT_STYLE_H
#define GUM_DOT_STYLE_H

#include <string>

#include <agrum/agrum.h>

#include <string_view>

namespace gum::dot {

  /**
   * @class DotStyle
   * @headerfile dotStyle.h <agrum/base/core/dot/dotStyle.h>
   * @ingroup utilities_group
   * @brief The look shared by all the graphs: size, direction, layout, colors of the arcs and of
   * the nodes.
   *
   * `read()` gives the style of the graphs drawn in a notebook (keys of the section `notebook`).
   */
  struct GUM_SHARED_PUBLIC DotStyle {
    /// graphviz size of the graph, in inches (`5`, or `12!` for an exact size)
    std::string size;
    /// direction of the layout : `TB` (top-bottom) or `LR` (left-right)
    std::string rankDir;
    /// graphviz layout engine (`dot`, `neato`...)
    std::string layout;
    /// color of the arcs and edges, and of the border of the nodes
    std::string arcColor;
    /// background color of the nodes
    std::string nodeBgColor;
    /// font color of the nodes
    std::string nodeFgColor;

    /// @brief the style of the graphs of the section `notebook`
    static DotStyle read();
    /// @brief the style of the credal networks : the colors of the nodes are the ones of the
    /// section `credalnet`
    static DotStyle readCredalNet();
  };

  /// @brief The style of a junction tree (section `notebook`): `size` is the size of the tree, not
  /// the one of its map.
  struct GUM_SHARED_PUBLIC JunctionTreeDotStyle: DotStyle {
    /// show the names of the variables in the cliques
    bool        withNames{true};
    std::string separatorBgColor;
    std::string separatorFgColor;
    int         separatorFontSize{0};
    std::string cliqueBgColor;
    std::string cliqueFgColor;
    int         cliqueFontSize{0};
    /// scale of the cliques in the map of a junction tree
    double mapCliqueScale{0.};
    /// scale of the separators in the map of a junction tree
    double mapSepScale{0.};
    /// length of the edges in the map of a junction tree
    double mapEdgeLen{0.};
    /// size of the map of a junction tree
    std::string mapSize;

    static JunctionTreeDotStyle read();
  };

  /// @brief The style of a factor graph (section `factorgraph`): `layout`, `nodeBgColor` and
  /// `nodeFgColor` are the ones of the variables.
  struct GUM_SHARED_PUBLIC FactorGraphDotStyle: DotStyle {
    std::string factorBgColor;
    /// the font color of the factors: `adaptive` against their background
    std::string factorFgColor;
    /// length of the edges
    double edgeLength{0.};

    static FactorGraphDotStyle read();
  };

  /// @brief The style of an influence diagram (section `influenceDiagram`).
  struct GUM_SHARED_PUBLIC InfluenceDiagramDotStyle: DotStyle {
    std::string chanceBgColor;
    std::string chanceFgColor;
    std::string chanceShape;
    std::string utilityBgColor;
    std::string utilityFgColor;
    std::string utilityShape;
    std::string decisionBgColor;
    std::string decisionFgColor;
    std::string decisionShape;
    /// graphviz style of the arcs pointing to a decision node
    std::string decisionArcStyle;
    /// graphviz style of the arcs pointing to a utility node
    std::string utilityArcStyle;

    static InfluenceDiagramDotStyle read();
  };

  /// @brief The style of a causal model (section `causal`): `nodeBgColor` and `nodeFgColor` are the
  /// ones of the observed variables.
  struct GUM_SHARED_PUBLIC CausalDotStyle: DotStyle {
    std::string latentBgColor;
    std::string latentFgColor;
    /// show the names of the latent variables
    bool showLatentNames{false};

    static CausalDotStyle read();
  };

  /// @brief The style of a dynamic Bayesian network (section `dynamicBN`) : the colors of its
  /// clusters.
  struct GUM_SHARED_PUBLIC DynamicBNDotStyle: DotStyle {
    /// background color of the cluster of a time slice
    std::string timeSliceBgColor;
    /// background color of the cluster of the replicated slice
    std::string replicatedSliceBgColor;
    /// background color of the cluster of the atemporal variables
    std::string atemporalBgColor;

    static DynamicBNDotStyle read();
  };

  /**
   * @brief A font color of the configuration, resolved against the background color which goes
   * with it.
   *
   * The background color is the key of the same section whose suffix `FgColor` is replaced by
   * `BgColor` (`nodeFgColor` goes with `nodeBgColor`). The special value `adaptive` is resolved by
   * gum::dot::resolveFgColor, with the color of the arcs (key `arcColor` of the section `notebook`)
   * when the background is not a color that can be interpreted.
   *
   * @param section the section of the configuration (`notebook`, `causal`...).
   * @param key the key of the font color, which ends with `FgColor`.
   * @return the font color, never `adaptive`.
   * @throw InvalidArgument if `key` does not end with `FgColor`.
   * @throw SyntaxError if the font color or the background color is not a key of the configuration.
   */
  GUM_SHARED_PUBLIC std::string configFgColor(std::string_view section, std::string_view key);

}   // namespace gum::dot

#endif   // GUM_DOT_STYLE_H
