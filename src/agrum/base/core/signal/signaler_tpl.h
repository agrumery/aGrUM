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

#include <agrum/base/core/signal/signaler.h>

namespace gum::__sig__ {

  // =========================================================================
  // Connector<TargetClass, Args...>
  // =========================================================================

  template < class TargetClass, typename... Args >
  Connector< TargetClass, Args... >::Connector(TargetClass* target, ActionType action) :
      _target_(target), _action_(action) {
    GUM_CONSTRUCTOR(Connector);
  }

  template < class TargetClass, typename... Args >
  Connector< TargetClass, Args... >::Connector(const Connector& src) :
      IConnector< Args... >(), _target_(src._target_), _action_(src._action_) {
    GUM_CONS_CPY(Connector);
  }

  template < class TargetClass, typename... Args >
  Listener* Connector< TargetClass, Args... >::target() const {
    return _target_;
  }

  template < class TargetClass, typename... Args >
  void Connector< TargetClass, Args... >::notify(const void* src, Args... args) {
    (_target_->*_action_)(src, args...);
  }

  template < class TargetClass, typename... Args >
  IConnector< Args... >* Connector< TargetClass, Args... >::clone() const {
    return new Connector< TargetClass, Args... >(*this);
  }

  template < class TargetClass, typename... Args >
  IConnector< Args... >* Connector< TargetClass, Args... >::duplicate(Listener* newTarget) const {
    return new Connector< TargetClass, Args... >(static_cast< TargetClass* >(newTarget), _action_);
  }

  // =========================================================================
  // BasicSignaler<Args...>
  // =========================================================================

  template < typename... Args >
  BasicSignaler< Args... >::BasicSignaler(const BasicSignaler& other) : ISignaler() {
    GUM_CONS_CPY(BasicSignaler);
    for (const auto* connector: other._connectors_) {
      auto* cloned = connector->clone();
      _connectors_.push_back(cloned);
      cloned->target()->_attachSignal_(this);
    }
  }

  template < typename... Args >
  BasicSignaler< Args... >::~BasicSignaler() {
    GUM_DESTRUCTOR(BasicSignaler);
    for (auto* connector: _connectors_) {
      connector->target()->_detachSignal_(this);
      delete connector;
    }
    _connectors_.clear();
  }

  template < typename... Args >
  bool BasicSignaler< Args... >::hasListener() {
    return !_connectors_.empty();
  }

  template < typename... Args >
  void BasicSignaler< Args... >::detach(Listener* target) {
    for (auto it = _connectors_.rbegin(); it != _connectors_.rend(); ++it) {
      if ((*it)->target() == target) {
        auto* conn = *it;
        _connectors_.erase(std::next(it).base());
        target->_detachSignal_(this);
        delete conn;
        return;
      }
    }
  }

  template < typename... Args >
  void BasicSignaler< Args... >::detachFromTarget(Listener* target) {
    for (auto it = _connectors_.begin(); it != _connectors_.end();) {
      if ((*it)->target() == target) {
        delete *it;
        it = _connectors_.erase(it);
      } else {
        ++it;
      }
    }
  }

  template < typename... Args >
  void BasicSignaler< Args... >::duplicateTarget(const Listener* oldTarget, Listener* newTarget) {
    ConnectorList toAdd;
    for (auto* connector: _connectors_) {
      if (connector->target() == oldTarget) { toAdd.push_back(connector->duplicate(newTarget)); }
    }
    for (auto* newConn: toAdd) {
      _connectors_.push_back(newConn);
    }
  }

}   // namespace gum::__sig__

namespace gum {

  // =========================================================================
  // Signaler<Args...>
  // =========================================================================

  template < typename... Args >
  Signaler< Args... >::Signaler(const Signaler& other) : __sig__::BasicSignaler< Args... >(other) {
    GUM_CONS_CPY(Signaler);
  }

  template < typename... Args >
  template < class TargetClass >
  void Signaler< Args... >::attach(TargetClass* target,
                                   void (TargetClass::*action)(const void*, Args...)) {
    auto* conn = new __sig__::Connector< TargetClass, Args... >(target, action);
    this->_connectors_.push_back(conn);
    target->_attachSignal_(this);
  }

  template < typename... Args >
  void Signaler< Args... >::operator()(const void* src, Args... args) {
    if (this->_connectors_.empty()) { return; }
    for (const auto connectors = this->_connectors_; auto* connector: connectors) {
      connector->notify(src, args...);
    }
  }

}   // namespace gum
