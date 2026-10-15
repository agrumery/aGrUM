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
 * @brief A minimal writer of graphs in the dot language of graphviz.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_DOT_WRITER_H
#define GUM_DOT_WRITER_H

#include <string>
#include <vector>

#include <agrum/agrum.h>

#include <string_view>

namespace gum::dot {

  /**
   * @brief An attribute of a graph, a node or an edge : `key="value"`.
   *
   * The value is written between double quotes, in which the double quote, the backslash and the
   * line breaks are escaped (see DotWriter::quote), unless `verbatim` is true : the value is then
   * written as is, with its own quotes if it needs some. This is needed for an HTML label
   * (`<<b>A</b>>`) or for a text that uses the escape sequences of graphviz (`"left\lright\l"`).
   */
  struct DotAttribute {
    std::string key;
    std::string value;
    bool        verbatim{false};
  };

  /// a list of attributes
  using DotAttributes = std::vector< DotAttribute >;

  /**
   * @class DotWriter
   * @headerfile dotWriter.h <agrum/base/core/dot/dotWriter.h>
   * @ingroup utilities_group
   * @brief Writes a graph in the dot language, statement after statement, in the order of the
   * calls.
   *
   * The names of the nodes, the labels and the values of the attributes are quoted and escaped, so
   * that any text gives a well formed graph. The keys of the attributes are checked (letters,
   * digits and underscores, not starting with a digit).
   *
   * @code
   * gum::dot::DotWriter w(true, "bn");
   * w.graphAttributes({{"bgcolor", "transparent"}, {"size", "5"}});
   * w.node("A", {{"fillcolor", "#404040"}, {"tooltip", "(0) A"}});
   * w.edge("A", "B", {{"penwidth", gum::dot::DotWriter::number(1.5)}});
   * std::string dot = w.str();
   * @endcode
   *
   * Errors :
   *  - gum::InvalidArgument for a key of attribute that is not an identifier,
   *  - gum::OperationNotAllowed for endSubgraph() without beginSubgraph(), or str() while a
   * subgraph is open.
   */
  class GUM_SHARED_PUBLIC DotWriter {
    public:
    /**
     * @brief Starts a graph.
     * @param directed true for a `digraph` (edges `->`), false for a `graph` (edges `--`).
     * @param name the name of the graph, written quoted ; none if empty.
     */
    explicit DotWriter(bool directed = true, std::string_view name = "");

    /// @name Statements
    /// @{

    /// @brief `graph [attributes];`, the attributes of the graph (does nothing if there is none).
    void graphAttributes(const DotAttributes& attributes);
    /// @brief `node [attributes];`, the default attributes of the nodes that follow (none : does
    /// nothing).
    void nodeDefaults(const DotAttributes& attributes);
    /// @brief `edge [attributes];`, the default attributes of the edges that follow (none : does
    /// nothing).
    void edgeDefaults(const DotAttributes& attributes);

    /// @brief `key="value";`, an attribute of the graph or of the subgraph being written.
    void graphAttribute(const DotAttribute& attribute);

    /// @brief a node, whose identifier is `id` (quoted).
    void node(std::string_view id, const DotAttributes& attributes = {});
    /// @brief a node designated by a number, written without quotes (graphviz reads it as an
    /// identifier).
    void node(Size id, const DotAttributes& attributes = {});

    /// @brief an edge from `from` to `to` (`->` in a directed graph, `--` in an undirected one).
    void edge(std::string_view from, std::string_view to, const DotAttributes& attributes = {});
    /// @brief an edge between two nodes designated by numbers, written without quotes.
    void edge(Size from, Size to, const DotAttributes& attributes = {});

    /**
     * @brief Opens a subgraph : `subgraph "name" { key="value"; ... `. Its statements are the ones
     * written until endSubgraph(). Graphviz draws it as a cluster if the name starts with
     * `cluster`.
     * @param name the name of the subgraph (quoted), anonymous if empty.
     * @param attributes the attributes of the subgraph.
     */
    void beginSubgraph(std::string_view name, const DotAttributes& attributes = {});
    /// @brief Closes the last subgraph that is open.
    void endSubgraph();
    /// @}

    /// @brief The graph written so far, closed. It can be called at any moment, and the writer can
    /// be used after.
    std::string str() const;

    /// @name Formatting
    /// @{

    /// @brief `text` between double quotes, in which `"`, `\`, line feed and carriage return are
    /// escaped.
    static std::string quote(std::string_view text);

    /// @brief The shortest text that reads back as `value` (`0.5`, `1`, `1e-05`).
    static std::string number(double value);
    /// @}

    private:
    std::string _buffer_;
    bool        _directed_;
    int         _depth_{0};

    // the indentation of a statement
    std::string _indent_() const;
    // `[key="value", ...]`
    static std::string _attributes_(const DotAttributes& attributes);
    // `key="value"`
    static std::string _attribute_(const DotAttribute& attribute);
    // the statement `keyword [attributes];`
    void _defaults_(const char* keyword, const DotAttributes& attributes);
    // a node or an edge, whose identifiers are already written
    void _node_(const std::string& id, const DotAttributes& attributes);
    void _edge_(const std::string& from, const std::string& to, const DotAttributes& attributes);
  };

}   // namespace gum::dot

#endif   // GUM_DOT_WRITER_H
