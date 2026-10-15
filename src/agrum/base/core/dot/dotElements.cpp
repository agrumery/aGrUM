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
 * @brief Implementation of the statements common to the graphs drawn with graphviz.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <agrum/base/core/dot/dotColor.h>
#include <agrum/base/core/dot/dotElements.h>

namespace gum::dot {

  void writePreamble(DotWriter&        writer,
                     const DotStyle&   style,
                     const DotOptions& options,
                     std::string_view  defaultLabel) {
    const auto pick
        = [](const std::string& given, const std::string& byDefault) -> const std::string& {
      return given.empty() ? byDefault : given;
    };

    DotAttributes graph{{"bgcolor", "transparent"},
                        {"size", pick(options.size(), style.size)},
                        {"rankdir", pick(options.rankDir(), style.rankDir)},
                        {"layout", pick(options.layout(), style.layout)}};

    const std::string label = options.label().empty() ? std::string(defaultLabel) : options.label();
    if (!label.empty()) {
      graph.push_back({"label", label});
      graph.push_back({"fontcolor", style.arcColor});
    }
    writer.graphAttributes(graph);

    writer.nodeDefaults({{"style", "filled"},
                         {"fillcolor", style.nodeBgColor},
                         {"fontcolor", style.nodeFgColor},
                         {"color", style.arcColor}});
    writer.edgeDefaults({{"color", style.arcColor}});
  }

  DotAttributes nodeAttributes(const DotStyle&       style,
                               const DotNodeOptions* options,
                               std::string_view      defaultLabel,
                               std::string_view      defaultTooltip) {
    DotAttributes attributes;

    const std::string label = (options != nullptr && !options->label.empty())
                                ? options->label
                                : std::string(defaultLabel);
    if (!label.empty()) attributes.push_back({"label", label});

    const std::string tooltip = (options != nullptr && !options->tooltip.empty())
                                  ? options->tooltip
                                  : std::string(defaultTooltip);
    if (!tooltip.empty()) attributes.push_back({"tooltip", tooltip});

    if (options == nullptr) return attributes;

    if (!options->bgColor.empty()) {
      attributes.push_back({"fillcolor", options->bgColor});
      const std::string fg
          = options->fgColor.empty() ? std::string(ADAPTIVE_COLOR) : options->fgColor;
      attributes.push_back({"fontcolor", resolveFgColor(fg, options->bgColor, style.arcColor)});
    } else if (!options->fgColor.empty()) {
      attributes.push_back(
          {"fontcolor", resolveFgColor(options->fgColor, style.nodeBgColor, style.arcColor)});
    }

    attributes.insert(attributes.end(), options->attributes.begin(), options->attributes.end());
    return attributes;
  }

  DotAttributes arcAttributes(const DotArcOptions* options) {
    DotAttributes attributes;
    if (options == nullptr) return attributes;

    if (!options->color.empty()) attributes.push_back({"color", options->color});
    if (options->width.has_value())
      attributes.push_back({"penwidth", DotWriter::number(*options->width)});
    if (!options->label.empty()) attributes.push_back({"label", options->label});
    if (!options->tooltip.empty()) attributes.push_back({"tooltip", options->tooltip});

    attributes.insert(attributes.end(), options->attributes.begin(), options->attributes.end());
    return attributes;
  }

  void
      addDefaultAttribute(DotAttributes& attributes, std::string_view key, std::string_view value) {
    for (const auto& attribute: attributes)
      if (attribute.key == key) return;
    attributes.insert(attributes.begin(), {std::string(key), std::string(value)});
  }

}   // namespace gum::dot
