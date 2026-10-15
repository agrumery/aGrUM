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
 * @brief Implementation of the writer of graphs in the dot language.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <charconv>

#include <agrum/base/core/dot/dotWriter.h>
#include <agrum/base/core/exceptions.h>

namespace gum::dot {

  namespace {
    // letters, digits and underscores, not starting with a digit
    bool _isIdentifier_(std::string_view key) {
      if (key.empty() || (key.front() >= '0' && key.front() <= '9')) return false;
      for (const char c: key) {
        const bool ok = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')
                     || c == '_';
        if (!ok) return false;
      }
      return true;
    }
  }   // namespace

  DotWriter::DotWriter(bool directed, std::string_view name) : _directed_(directed) {
    _buffer_ = directed ? "digraph" : "graph";
    if (!name.empty()) {
      _buffer_ += ' ';
      _buffer_ += quote(name);
    }
    _buffer_ += " {\n";
  }

  std::string DotWriter::quote(std::string_view text) {
    std::string result;
    result.reserve(text.size() + 2);
    result += '"';
    for (const char c: text) {
      switch (c) {
        case '"' : result += "\\\""; break;
        case '\\' : result += "\\\\"; break;
        case '\n' : result += "\\n"; break;
        case '\r' : result += "\\r"; break;
        default : result += c;
      }
    }
    result += '"';
    return result;
  }

  std::string DotWriter::number(double value) {
    char buffer[40];
    const auto [end, error] = std::to_chars(buffer, buffer + sizeof(buffer), value);
    return std::string(buffer, error == std::errc() ? end : buffer);
  }

  std::string DotWriter::_indent_() const { return std::string(2 * (_depth_ + 1), ' '); }

  std::string DotWriter::_attribute_(const DotAttribute& attribute) {
    if (!_isIdentifier_(attribute.key)) {
      GUM_ERROR(InvalidArgument, "'" << attribute.key << "' is not a valid key of dot attribute.")
    }
    return attribute.key + '=' + (attribute.verbatim ? attribute.value : quote(attribute.value));
  }

  std::string DotWriter::_attributes_(const DotAttributes& attributes) {
    std::string result = "[";
    for (std::size_t i = 0; i < attributes.size(); ++i) {
      if (i > 0) result += ", ";
      result += _attribute_(attributes[i]);
    }
    return result + ']';
  }

  void DotWriter::_defaults_(const char* keyword, const DotAttributes& attributes) {
    if (attributes.empty()) return;
    _buffer_ += _indent_() + keyword + ' ' + _attributes_(attributes) + ";\n";
  }

  void DotWriter::graphAttributes(const DotAttributes& attributes) {
    _defaults_("graph", attributes);
  }

  void DotWriter::nodeDefaults(const DotAttributes& attributes) { _defaults_("node", attributes); }

  void DotWriter::edgeDefaults(const DotAttributes& attributes) { _defaults_("edge", attributes); }

  void DotWriter::graphAttribute(const DotAttribute& attribute) {
    _buffer_ += _indent_() + _attribute_(attribute) + ";\n";
  }

  void DotWriter::_node_(const std::string& id, const DotAttributes& attributes) {
    std::string statement = _indent_() + id;
    if (!attributes.empty()) statement += ' ' + _attributes_(attributes);
    _buffer_ += statement + ";\n";
  }

  void DotWriter::_edge_(const std::string&   from,
                         const std::string&   to,
                         const DotAttributes& attributes) {
    std::string statement = _indent_() + from + (_directed_ ? " -> " : " -- ") + to;
    if (!attributes.empty()) statement += ' ' + _attributes_(attributes);
    _buffer_ += statement + ";\n";
  }

  void DotWriter::node(std::string_view id, const DotAttributes& attributes) {
    _node_(quote(id), attributes);
  }

  void DotWriter::node(Size id, const DotAttributes& attributes) {
    _node_(std::to_string(id), attributes);
  }

  void
      DotWriter::edge(std::string_view from, std::string_view to, const DotAttributes& attributes) {
    _edge_(quote(from), quote(to), attributes);
  }

  void DotWriter::edge(Size from, Size to, const DotAttributes& attributes) {
    _edge_(std::to_string(from), std::to_string(to), attributes);
  }

  void DotWriter::beginSubgraph(std::string_view name, const DotAttributes& attributes) {
    // nothing is written if an attribute is refused
    std::string statement = _indent_() + "subgraph ";
    if (!name.empty()) statement += quote(name) + ' ';
    statement += "{\n";
    const std::string inner(2 * (_depth_ + 2), ' ');
    for (const auto& attribute: attributes)
      statement += inner + _attribute_(attribute) + ";\n";
    _buffer_ += statement;
    ++_depth_;
  }

  void DotWriter::endSubgraph() {
    if (_depth_ == 0) { GUM_ERROR(OperationNotAllowed, "No subgraph to close.") }
    --_depth_;
    _buffer_ += _indent_() + "}\n";
  }

  std::string DotWriter::str() const {
    if (_depth_ != 0) {
      GUM_ERROR(OperationNotAllowed, "The graph is not complete : a subgraph is still open.")
    }
    return _buffer_ + "}\n";
  }

}   // namespace gum::dot
