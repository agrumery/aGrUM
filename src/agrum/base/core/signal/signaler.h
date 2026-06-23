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
 * @brief C++20 variadic template implementation of the Signaler/Listener pattern.
 *
 * This file provides the single entry point for the signal/listener pattern.
 * All signalers are unified under a single variadic template `Signaler<Args...>`.
 *
 * ### Usage:
 * @code
 * // Declare signals with any number of arguments
 * Signaler0 onSimpleEvent;                     // 0 arguments (alias for Signaler<>)
 * Signaler<int> onProgress;                    // 1 argument
 * Signaler<int, std::string> onMessage;        // 2 arguments
 * Signaler<NodeId, Idx, Idx> onChange;         // 3 arguments
 * // ... any number of arguments supported
 *
 * // Emit signals
 * GUM_EMIT0(onSimpleEvent);
 * GUM_EMIT_ARGS(onProgress, 42);
 * GUM_EMIT_ARGS(onMessage, 1, "hello");
 *
 * // Connect signals
 * GUM_CONNECT(emitter, onProgress, receiver, MyClass::handleProgress);
 * @endcode
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#ifndef GUM_SIGNALER_H
#define GUM_SIGNALER_H

#include <algorithm>
#include <functional>
#include <vector>

#include <agrum/agrum.h>

#include <agrum/base/core/signal/listener.h>

namespace gum {

  namespace __sig__ {

    // =========================================================================
    // IConnector<Args...> : Abstract interface for connectors with any args
    // =========================================================================
    /**
     * @class IConnector
     * @brief Abstract interface for signal connectors with variadic arguments.
     * @tparam Args The types of arguments passed when the signal is emitted.
     */
    template < typename... Args >
    class IConnector {
      public:
      virtual ~IConnector() = default;

      /// @return The listener target of this connector.
      virtual Listener* target() const = 0;

      /// @brief Notify the target with the given source and arguments.
      virtual void notify(const void* src, Args... args) = 0;

      /// @brief Create a deep copy of this connector.
      virtual IConnector< Args... >* clone() const = 0;

      /// @brief Create a duplicate connector pointing to a new target.
      virtual IConnector< Args... >* duplicate(Listener* newTarget) const = 0;
    };

    // =========================================================================
    // Connector<TargetClass, Args...> : Concrete connector implementation
    // =========================================================================
    /**
     * @class Connector
     * @brief Concrete connector binding a target object and its member function.
     * @tparam TargetClass The class of the listener object.
     * @tparam Args The types of arguments passed to the callback.
     */
    template < class TargetClass, typename... Args >
    class Connector: public IConnector< Args... > {
      public:
      /// Type alias for the member function pointer.
      using ActionType = void (TargetClass::*)(const void*, Args...);

      /// Default constructor.
      Connector() : _target_(nullptr), _action_(nullptr) { GUM_CONSTRUCTOR(Connector); }

      /// Construct with target and action.
      Connector(TargetClass* target, ActionType action) : _target_(target), _action_(action) {
        GUM_CONSTRUCTOR(Connector);
      }

      /// Copy constructor.
      Connector(const Connector& src) :
          IConnector< Args... >(), _target_(src._target_), _action_(src._action_) {
        GUM_CONS_CPY(Connector);
      }

      /// Destructor.
      ~Connector() override { GUM_DESTRUCTOR(Connector); }

      /// @copydoc IConnector::target
      Listener* target() const override { return _target_; }

      /// @copydoc IConnector::notify
      void notify(const void* src, Args... args) override { (_target_->*_action_)(src, args...); }

      /// @copydoc IConnector::clone
      IConnector< Args... >* clone() const override {
        return new Connector< TargetClass, Args... >(*this);
      }

      /// @copydoc IConnector::duplicate
      IConnector< Args... >* duplicate(Listener* newTarget) const override {
        return new Connector< TargetClass, Args... >(static_cast< TargetClass* >(newTarget),
                                                     _action_);
      }

      private:
      TargetClass* _target_;
      ActionType   _action_;
    };

    // =========================================================================
    // BasicSignaler<Args...> : Base signaler with connector management
    // =========================================================================
    /**
     * @class BasicSignaler
     * @brief Base class for signalers, managing connector lifecycle.
     * @tparam Args The types of arguments passed when the signal is emitted.
     */
    template < typename... Args >
    class BasicSignaler: public ISignaler {
      protected:
      using ConnectorList = std::vector< IConnector< Args... >* >;

      /// Default constructor.
      BasicSignaler() { GUM_CONSTRUCTOR(BasicSignaler); }

      /// Copy constructor: duplicates all connectors and re-attaches to targets.
      BasicSignaler(const BasicSignaler& other) : ISignaler() {
        GUM_CONS_CPY(BasicSignaler);
        for (const auto* connector: other._connectors_) {
          auto* cloned = connector->clone();
          _connectors_.push_back(cloned);
          cloned->target()->_attachSignal_(this);
        }
      }

      public:
      /// Destructor: detaches from all targets and deletes connectors.
      ~BasicSignaler() override {
        GUM_DESTRUCTOR(BasicSignaler);
        for (auto* connector: _connectors_) {
          connector->target()->_detachSignal_(this);
          delete connector;
        }
        _connectors_.clear();
      }

      /// @return true if at least one listener is attached.
      bool hasListener() override { return !_connectors_.empty(); }

      /// @brief Detach a specific listener from this signaler.
      void detach(Listener* target) {
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

      protected:
      friend class gum::Listener;

      /// @brief Called by Listener to detach when the listener is destroyed.
      void detachFromTarget(Listener* target) override {
        for (auto it = _connectors_.begin(); it != _connectors_.end();) {
          if ((*it)->target() == target) {
            delete *it;
            it = _connectors_.erase(it);
          } else {
            ++it;
          }
        }
      }

      /// @brief Called by Listener copy constructor to duplicate connections.
      void duplicateTarget(const Listener* oldTarget, Listener* newTarget) override {
        // Create a copy of connectors to iterate safely
        ConnectorList toAdd;
        for (auto* connector: _connectors_) {
          if (connector->target() == oldTarget) {
            toAdd.push_back(connector->duplicate(newTarget));
          }
        }
        for (auto* newConn: toAdd) {
          _connectors_.push_back(newConn);
        }
      }

      ConnectorList _connectors_;
    };

  }   // namespace __sig__

  // ===========================================================================
  // Signaler<Args...> : The main variadic signaler class
  // ===========================================================================
  /**
   * @class Signaler
   * @headerfile signaler.h <agrum/base/core/signal/signaler.h>
   * @brief Variadic template signaler for any number of arguments.
   * @ingroup signal_group
   *
   * This class implements the signal/slot pattern with C++20 variadic templates.
   * It can be used with any number of arguments (0 to N).
   *
   * ### Usage example:
   * @code
   * class MyEmitter {
   *   public:
   *     Signaler<int, std::string> onValueChanged;
   *
   *     void setValue(int v, const std::string& name) {
   *       onValueChanged(this, v, name);
   *     }
   * };
   *
   * class MyReceiver : public Listener {
   *   public:
   *     void handleChange(const void* src, int v, std::string name) {
   *       // handle the signal
   *     }
   * };
   *
   * // Connect:
   * MyEmitter emitter;
   * MyReceiver receiver;
   * emitter.onValueChanged.attach(&receiver, &MyReceiver::handleChange);
   * @endcode
   *
   * @tparam Args The types of arguments passed when the signal is emitted.
   */
  template < typename... Args >
  class Signaler: public __sig__::BasicSignaler< Args... > {
    public:
    /// Default constructor.
    Signaler() { GUM_CONSTRUCTOR(Signaler); }

    /// Copy constructor.
    Signaler(const Signaler& other) : __sig__::BasicSignaler< Args... >(other) {
      GUM_CONS_CPY(Signaler);
    }

    /// Destructor.
    ~Signaler() override { GUM_DESTRUCTOR(Signaler); }

    /**
     * @brief Attach a listener with a member function callback.
     * @tparam TargetClass The class type of the listener (must derive from Listener).
     * @param target Pointer to the listener object.
     * @param action Pointer to the member function to call on signal emission.
     */
    template < class TargetClass >
    void attach(TargetClass* target, void (TargetClass::*action)(const void*, Args...)) {
      auto* conn = new __sig__::Connector< TargetClass, Args... >(target, action);
      this->_connectors_.push_back(conn);
      target->_attachSignal_(this);
    }

    /**
     * @brief Emit the signal, notifying all attached listeners.
     * @param src Pointer to the source object (typically `this` of the emitter).
     * @param args The arguments to pass to each listener's callback.
     */
    void operator()(const void* src, Args... args) {
      if (this->_connectors_.empty()) {
        return;   // No listeners attached, do nothing
      }
      for (const auto connectors = this->_connectors_; auto* connector: connectors) {
        connector->notify(src, args...);
      }
    }
  };

  // ===========================================================================
  // Signaler0 : Alias for Signaler<> (signal with no arguments)
  // ===========================================================================
  /**
   * @brief Signaler with no arguments.
   *
   * This is an alias for `Signaler<>` (empty parameter pack).
   * The callback signature is `void handler(const void* src)`.
   */
  using Signaler0 = Signaler<>;

}   // namespace gum

// ===========================================================================
// Emit macros
// ===========================================================================

/// @brief Emit a signal with no additional arguments.
#define GUM_EMIT0(signal) (signal)(this)

/// @brief Emit a signal with variadic arguments.
/// Usage: GUM_EMIT_ARGS(mySignal, arg1, arg2, ...)
#define GUM_EMIT_ARGS(signal, ...) (signal)(this, __VA_ARGS__)

// Legacy macros for backward compatibility (all map to GUM_EMIT_ARGS)
#define GUM_EMIT1(signal, arg1)                   GUM_EMIT_ARGS(signal, arg1)
#define GUM_EMIT2(signal, arg1, arg2)             GUM_EMIT_ARGS(signal, arg1, arg2)
#define GUM_EMIT3(signal, arg1, arg2, arg3)       GUM_EMIT_ARGS(signal, arg1, arg2, arg3)
#define GUM_EMIT4(signal, arg1, arg2, arg3, arg4) GUM_EMIT_ARGS(signal, arg1, arg2, arg3, arg4)
#define GUM_EMIT5(signal, arg1, arg2, arg3, arg4, arg5) \
  GUM_EMIT_ARGS(signal, arg1, arg2, arg3, arg4, arg5)
#define GUM_EMIT6(signal, arg1, arg2, arg3, arg4, arg5, arg6) \
  GUM_EMIT_ARGS(signal, arg1, arg2, arg3, arg4, arg5, arg6)
#define GUM_EMIT7(signal, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
  GUM_EMIT_ARGS(signal, arg1, arg2, arg3, arg4, arg5, arg6, arg7)

/// @brief Macro to connect a signal to a listener's member function.
#define GUM_CONNECT(sender, signal, receiver, target) (sender).signal.attach(&(receiver), &target)

#endif   // GUM_SIGNALER_H
