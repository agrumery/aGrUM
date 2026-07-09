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

#include <agrum/base/multidim/utils/FunctionGraphUtilities/terminalNodePolicies/SetTerminalNodePolicy.h>

namespace gum {


  template < typename GUM_ELEMENT >
  SetTerminalNodePolicy< GUM_ELEMENT >::~SetTerminalNodePolicy() {
    clearAllTerminalNodes();
  }

  template < typename GUM_ELEMENT >
  void SetTerminalNodePolicy< GUM_ELEMENT >::addTerminalNode(const NodeId&      n,
                                                             const GUM_ELEMENT& v) {
    if (_map_.exists(n)) *(_map_[n]) += v;
    _map_.insert(n, new GUM_ELEMENT(v));
  }

  template < typename GUM_ELEMENT >
  void SetTerminalNodePolicy< GUM_ELEMENT >::eraseTerminalNode(const NodeId& n) {
    if (_map_.exists(n)) _map_.erase(n);
  }

  template < typename GUM_ELEMENT >
  void SetTerminalNodePolicy< GUM_ELEMENT >::clearAllTerminalNodes() {
    for (auto nodeIter = _map_.beginSafe(); nodeIter != _map_.endSafe(); ++nodeIter)
      delete nodeIter.val();
    _map_.clear();
  }

  template < typename GUM_ELEMENT >
  bool SetTerminalNodePolicy< GUM_ELEMENT >::existsTerminalNodeWithId(const NodeId& n) const {
    return _map_.exists(n);
  }

  template < typename GUM_ELEMENT >
  bool SetTerminalNodePolicy< GUM_ELEMENT >::existsTerminalNodeWithValue(
      const GUM_ELEMENT& v) const {
    return terminalNodeId(v) != 0;
  }

  template < typename GUM_ELEMENT >
  const GUM_ELEMENT&
      SetTerminalNodePolicy< GUM_ELEMENT >::terminalNodeValue(const NodeId& n) const {
    return *(_map_[n]);
  }

  template < typename GUM_ELEMENT >
  const NodeId& SetTerminalNodePolicy< GUM_ELEMENT >::terminalNodeId(const GUM_ELEMENT& v) const {
    for (auto nodeIter = _map_.beginSafe(); nodeIter != _map_.endSafe(); ++nodeIter)
      if (*(nodeIter.val()) == v) return nodeIter.key();
    return jocker;
  }

  template < typename GUM_ELEMENT >
  void SetTerminalNodePolicy< GUM_ELEMENT >::beginValues() const {
    _mappy_ = _map_.cbeginSafe();
  }

  template < typename GUM_ELEMENT >
  bool SetTerminalNodePolicy< GUM_ELEMENT >::hasValue() const {
    return _mappy_ != _map_.cendSafe();
  }

  template < typename GUM_ELEMENT >
  void SetTerminalNodePolicy< GUM_ELEMENT >::nextValue() const {
    ++_mappy_;
  }

  template < typename GUM_ELEMENT >
  const GUM_ELEMENT& SetTerminalNodePolicy< GUM_ELEMENT >::value() const {
    return *(_mappy_.val());
  }

  template < typename GUM_ELEMENT >
  const NodeId& SetTerminalNodePolicy< GUM_ELEMENT >::id() const {
    return _mappy_.key();
  }

}   // namespace gum
