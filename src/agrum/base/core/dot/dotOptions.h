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
 * @brief What the caller gives to a toDot() beyond the configuration: sizes, labels, colors of some
 * nodes and arcs.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_DOT_OPTIONS_H
#define GUM_DOT_OPTIONS_H

#include <map>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include <agrum/agrum.h>

#include <agrum/base/core/dot/dotWriter.h>

namespace gum::dot {

  /// the options of a node : an empty text means "not given", so that the value of the
  /// configuration is used
  struct DotNodeOptions {
    /// background color
    std::string bgColor;
    /// font color (if only the background is given, the font color is `adaptive` against it)
    std::string fgColor;
    /// text displayed in the node
    std::string label;
    /// text displayed when the mouse is over the node
    std::string tooltip;
    /// other attributes, written after the ones above
    DotAttributes attributes;
  };

  /// the options of an arc or an edge : an empty text (no value for the width) means "not given"
  struct DotArcOptions {
    std::string             color;
    std::optional< double > width;
    std::string             label;
    std::string             tooltip;
    /// other attributes, written after the ones above
    DotAttributes attributes;
  };

  /**
   * @class DotOptions
   * @headerfile dotOptions.h <agrum/base/core/dot/dotOptions.h>
   * @ingroup utilities_group
   * @brief The options given to a `toDot(const DotOptions&)`.
   *
   * What is not given is taken from the configuration (see gum::dot::DotStyle). The nodes are
   * designated by their identifier (`NodeId`), the arcs by the identifiers of their tail and their
   * head; an undirected edge is designated by its two ends, given in any order. The options are
   * filled with setters, one value at a time, so that they can be filled from any language (colors
   * computed from a color map for instance).
   *
   * @code
   * gum::dot::DotOptions options;
   * options.setSize("12!");
   * options.setNodeBgColor(0, "#FF7F64");
   * options.setArcWidth(0, 1, 2.5);
   * auto dot = bn.toDot(options);
   * @endcode
   *
   * Errors : gum::InvalidArgument for a width that is negative or not a finite number.
   */
  class GUM_SHARED_PUBLIC DotOptions {
    public:
    /// @name Graph
    /// @{
    /// size of the graph, in inches (`5`, or `12!` for an exact size)
    void setSize(const std::string& size);
    /// direction of the layout, `TB` or `LR`
    void setRankDir(const std::string& rankDir);
    /// graphviz layout engine
    void setLayout(const std::string& layout);
    /// label of the graph (by default, the name of the graph if it has one)
    void setLabel(const std::string& label);

    const std::string& size() const;
    const std::string& rankDir() const;
    const std::string& layout() const;
    const std::string& label() const;
    /// @}

    /// @name Nodes
    /// @{
    void setNodeBgColor(Size node, const std::string& color);
    void setNodeFgColor(Size node, const std::string& color);
    void setNodeLabel(Size node, const std::string& label);
    void setNodeTooltip(Size node, const std::string& tooltip);
    /// @brief any other attribute of the node ; written as is if `verbatim`, see DotAttribute
    void setNodeAttribute(Size               node,
                          const std::string& key,
                          const std::string& value,
                          bool               verbatim = false);

    /// @brief the options of a node, or nullptr if none was given for it
    const DotNodeOptions* node(Size node) const;
    /// @}

    /// @name Arcs and edges
    /// @{
    void setArcColor(Size tail, Size head, const std::string& color);
    /// @param width the width of the line, at least 0
    void setArcWidth(Size tail, Size head, double width);
    void setArcLabel(Size tail, Size head, const std::string& label);
    void setArcTooltip(Size tail, Size head, const std::string& tooltip);
    /// @brief any other attribute of the arc ; written as is if `verbatim`, see DotAttribute
    void setArcAttribute(Size               tail,
                         Size               head,
                         const std::string& key,
                         const std::string& value,
                         bool               verbatim = false);

    /// @brief the options of the arc from `tail` to `head`, or nullptr if none was given for it
    const DotArcOptions* arc(Size tail, Size head) const;
    /// @brief the options of the undirected edge between `a` and `b`, given for (a, b) or for (b,
    /// a), or nullptr
    const DotArcOptions* edge(Size a, Size b) const;
    /// @}

    /// @name Factors of a factor graph
    /// @{
    /// A factor is designated by the identifiers of its variables, in any order.
    void setFactorBgColor(const std::vector< Size >& variables, const std::string& color);
    void setFactorFgColor(const std::vector< Size >& variables, const std::string& color);
    void setFactorLabel(const std::vector< Size >& variables, const std::string& label);
    void setFactorTooltip(const std::vector< Size >& variables, const std::string& tooltip);
    /// @brief any other attribute of the node of the factor ; written as is if `verbatim`, see
    /// DotAttribute
    void setFactorAttribute(const std::vector< Size >& variables,
                            const std::string&         key,
                            const std::string&         value,
                            bool                       verbatim = false);

    /// @brief the options of the factor on these variables (in any order), or nullptr if none was
    /// given
    const DotNodeOptions* factor(const std::vector< Size >& variables) const;
    /// @}

    /// @brief true if nothing was given
    bool empty() const;

    private:
    std::string _size_;
    std::string _rankDir_;
    std::string _layout_;
    std::string _label_;

    std::map< Size, DotNodeOptions >                   _nodes_;
    std::map< std::pair< Size, Size >, DotArcOptions > _arcs_;
    std::map< std::vector< Size >, DotNodeOptions >    _factors_;

    // the variables of a factor, sorted and without duplicate
    static std::vector< Size > _key_(std::vector< Size > variables);
  };

}   // namespace gum::dot

#endif   // GUM_DOT_OPTIONS_H
