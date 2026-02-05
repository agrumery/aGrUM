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
 * @brief Class of gum::Signaler0.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */

#ifndef GUM_SIGNALER0_H
#define GUM_SIGNALER0_H
#include <functional>

#include <agrum/agrum.h>

#include <agrum/base/core/list.h>
#include <agrum/base/core/signal/signaler.h>

namespace gum {

#ifndef DOXYGEN_SHOULD_SKIP_THIS

  namespace __sig__ {

    class IConnector0 {
      public:
      virtual ~IConnector0() {};

      virtual Listener*    target() const              = 0;
      virtual void         notify(const void*)         = 0;
      virtual IConnector0* clone()                     = 0;
      virtual IConnector0* duplicate(Listener* target) = 0;
    };

    class BasicSignaler0: public ISignaler {
      protected:
      using ConnectorList = std::vector< IConnector0* >;

      BasicSignaler0();

      BasicSignaler0(const BasicSignaler0& s);

      public:
      virtual ~BasicSignaler0();

      bool hasListener();

      void detach(Listener* target);

      protected:
      friend class Listener;

      void detachFromTarget_(Listener* target);

      void duplicateTarget_(const Listener* oldtarget, Listener* newtarget);

      ConnectorList connectors_;

      private:
      std::function< bool(IConnector0* el) > _find_target_(const gum::Listener* l);
    };

    template < class TargetClass >
    class Connector0: public IConnector0 {
      public:
      Connector0();

      Connector0(TargetClass* target, void (TargetClass::*action)(const void*));

      Connector0(const Connector0< TargetClass >* src);

      virtual ~Connector0();

      virtual IConnector0* clone();

      virtual IConnector0* duplicate(Listener* target);

      virtual void notify(const void* src);

      virtual Listener* target() const;

      private:
      TargetClass* _target_;
      void (TargetClass::*_action_)(const void*);
    };

  }   // namespace  __sig__

#endif   // DOXYGEN_SHOULD_SKIP_THIS

  /**
   * @class Signaler0
   * @ingroup signal_group
   * @brief class for signal with 0 args
   * @warning Due to the too-smart-way-for-DOXYGEN of building classes
   * Signaler1,...,Signaler_n, these classes are not documented.
   */
  class Signaler0: public __sig__::BasicSignaler0 {
    public:
    Signaler0() {
      GUM_CONSTRUCTOR(Signaler0);
      ;
    }

    Signaler0(const Signaler0& s) : __sig__::BasicSignaler0(s) { GUM_CONS_CPY(Signaler0); }

    virtual ~Signaler0() {
      GUM_DESTRUCTOR(Signaler0);
      ;
    }

    template < class TargetClass >
    void attach(TargetClass* target, void (TargetClass::*action)(const void*)) {
      __sig__::Connector0< TargetClass >* conn
          = new __sig__::Connector0< TargetClass >(target, action);
      this->connectors_.push_back(conn);
      target->_attachSignal_(this);
    }

    INLINE void operator()(const void* src) {
      for (const auto el: connectors_) {
        el->notify(src);
      }
    }
  };

}   // namespace gum

/// this macro is the good way of emitting a signal.
#define GUM_EMIT0(signal) this->signal(this)

// Template implementation should always be in headers
#include <agrum/base/core/signal/signaler0_tpl.h>

#ifndef GUM_NO_INLINE
#  include <agrum/base/core/signal/signaler0_inl.h>
#endif   // GUM_NO_INLINE

#endif   // GUM_SIGNALER0_H
