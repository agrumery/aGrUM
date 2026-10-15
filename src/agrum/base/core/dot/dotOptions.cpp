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
 * @brief Implementation of the options given to a toDot().
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <algorithm>
#include <cmath>

#include <agrum/base/core/dot/dotOptions.h>
#include <agrum/base/core/exceptions.h>

namespace gum::dot {

  void DotOptions::setSize(const std::string& size) { _size_ = size; }

  void DotOptions::setRankDir(const std::string& rankDir) { _rankDir_ = rankDir; }

  void DotOptions::setLayout(const std::string& layout) { _layout_ = layout; }

  void DotOptions::setLabel(const std::string& label) { _label_ = label; }

  const std::string& DotOptions::size() const { return _size_; }

  const std::string& DotOptions::rankDir() const { return _rankDir_; }

  const std::string& DotOptions::layout() const { return _layout_; }

  const std::string& DotOptions::label() const { return _label_; }

  void DotOptions::setNodeBgColor(Size node, const std::string& color) {
    _nodes_[node].bgColor = color;
  }

  void DotOptions::setNodeFgColor(Size node, const std::string& color) {
    _nodes_[node].fgColor = color;
  }

  void DotOptions::setNodeLabel(Size node, const std::string& label) {
    _nodes_[node].label = label;
  }

  void DotOptions::setNodeTooltip(Size node, const std::string& tooltip) {
    _nodes_[node].tooltip = tooltip;
  }

  void DotOptions::setNodeAttribute(Size               node,
                                    const std::string& key,
                                    const std::string& value,
                                    bool               verbatim) {
    _nodes_[node].attributes.push_back({key, value, verbatim});
  }

  const DotNodeOptions* DotOptions::node(Size node) const {
    const auto it = _nodes_.find(node);
    return it == _nodes_.end() ? nullptr : &it->second;
  }

  void DotOptions::setArcColor(Size tail, Size head, const std::string& color) {
    _arcs_[{tail, head}].color = color;
  }

  void DotOptions::setArcWidth(Size tail, Size head, double width) {
    if (!std::isfinite(width) || width < 0) {
      GUM_ERROR(InvalidArgument,
                "The width of an arc must be a finite number, at least 0 (not " << width << ").")
    }
    _arcs_[{tail, head}].width = width;
  }

  void DotOptions::setArcLabel(Size tail, Size head, const std::string& label) {
    _arcs_[{tail, head}].label = label;
  }

  void DotOptions::setArcTooltip(Size tail, Size head, const std::string& tooltip) {
    _arcs_[{tail, head}].tooltip = tooltip;
  }

  void DotOptions::setArcAttribute(Size               tail,
                                   Size               head,
                                   const std::string& key,
                                   const std::string& value,
                                   bool               verbatim) {
    _arcs_[{tail, head}].attributes.push_back({key, value, verbatim});
  }

  const DotArcOptions* DotOptions::arc(Size tail, Size head) const {
    const auto it = _arcs_.find({tail, head});
    return it == _arcs_.end() ? nullptr : &it->second;
  }

  const DotArcOptions* DotOptions::edge(Size a, Size b) const {
    if (const auto* options = arc(a, b)) return options;
    return arc(b, a);
  }

  std::vector< Size > DotOptions::_key_(std::vector< Size > variables) {
    std::sort(variables.begin(), variables.end());
    variables.erase(std::unique(variables.begin(), variables.end()), variables.end());
    return variables;
  }

  void DotOptions::setFactorBgColor(const std::vector< Size >& variables,
                                    const std::string&         color) {
    _factors_[_key_(variables)].bgColor = color;
  }

  void DotOptions::setFactorFgColor(const std::vector< Size >& variables,
                                    const std::string&         color) {
    _factors_[_key_(variables)].fgColor = color;
  }

  void DotOptions::setFactorLabel(const std::vector< Size >& variables, const std::string& label) {
    _factors_[_key_(variables)].label = label;
  }

  void DotOptions::setFactorTooltip(const std::vector< Size >& variables,
                                    const std::string&         tooltip) {
    _factors_[_key_(variables)].tooltip = tooltip;
  }

  void DotOptions::setFactorAttribute(const std::vector< Size >& variables,
                                      const std::string&         key,
                                      const std::string&         value,
                                      bool                       verbatim) {
    _factors_[_key_(variables)].attributes.push_back({key, value, verbatim});
  }

  const DotNodeOptions* DotOptions::factor(const std::vector< Size >& variables) const {
    const auto it = _factors_.find(_key_(variables));
    return it == _factors_.end() ? nullptr : &it->second;
  }

  bool DotOptions::empty() const {
    return _size_.empty() && _rankDir_.empty() && _layout_.empty() && _label_.empty()
        && _nodes_.empty() && _arcs_.empty() && _factors_.empty();
  }

}   // namespace gum::dot
