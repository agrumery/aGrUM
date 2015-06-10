/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef SIGNALER0_H__
#define SIGNALER0_H__
#include <functional>

#include "signaler.h"
#include <agrum/core/list.h>

namespace gum {

  namespace __sig__ {

    class IConnector0 {
      public:
      virtual ~IConnector0(){};

      virtual Listener *target() const = 0;
      virtual void notify(const void *) = 0;
      virtual IConnector0 *clone() = 0;
      virtual IConnector0 *duplicate(Listener *target) = 0;
    };

    class BasicSignaler0 : public ISignaler {
      protected:
      typedef std::vector<IConnector0 *> ConnectorList;

      BasicSignaler0() { GUM_CONSTRUCTOR(BasicSignaler0); }

      BasicSignaler0(const BasicSignaler0 &s) : ISignaler(s) {
        GUM_CONS_CPY(BasicSignaler0);

        for (const auto el : _connectors) {
          el->target()->attachSignal__(this);
          _connectors.push_back(el->clone());
        }
      }

      public:
      virtual ~BasicSignaler0() {
        GUM_DESTRUCTOR(BasicSignaler0);

        for (const auto el : _connectors) {
          el->target()->detachSignal__(this);
          delete el;
        }

        _connectors.clear();
      };

      bool hasListener(void) { return (!(_connectors.empty())); };

      void detach(Listener *target) {
        auto it = std::find_if(_connectors.begin(), _connectors.end(),
                               __find_target(target));

        while (it != _connectors.end()) {
          delete *it;
          target->detachSignal__(this);

          it = _connectors.erase(it); // it is the next one
          it = std::find_if(it, _connectors.end(), __find_target(target));
        }
      };

      protected:
      friend class Listener;

      void _detachFromTarget(Listener *target) {
        auto it = std::find_if(_connectors.begin(), _connectors.end(),
                               __find_target(target));

        while (it != _connectors.end()) {
          delete *it;

          it = _connectors.erase(it); // it is the next one
          it = std::find_if(it, _connectors.end(), __find_target(target));
        }
      };

      void _duplicateTarget(const Listener *oldtarget, Listener *newtarget) {
        auto it = std::find_if(_connectors.begin(), _connectors.end(),
                               __find_target(oldtarget));

        while (it != _connectors.end()) {
          _connectors.push_back((*it)->duplicate(newtarget));

          it++;
          it = std::find_if(it, _connectors.end(), __find_target(oldtarget));
        }
      }

      ConnectorList _connectors;

      private:
      std::function<bool(IConnector0 *el)> __find_target(const gum::Listener *l) {
        return [=](IConnector0 *el) -> bool { return el->target() == l; };
      }
    };

    template <class TargetClass>

    class Connector0 : public IConnector0 {
      public:
      Connector0() {
        GUM_CONSTRUCTOR(Connector0);
        __target = nullptr;
        __action = nullptr;
      }

      Connector0(TargetClass *target, void (TargetClass::*action)(const void *)) {
        GUM_CONSTRUCTOR(Connector0);
        __target = target;
        __action = action;
      }

      Connector0(const Connector0<TargetClass> *src) : IConnector0(src) {
        GUM_CONS_CPY(Connector0);
      }

      virtual ~Connector0() { GUM_DESTRUCTOR(Connector0); }

      INLINE virtual IConnector0 *clone() {
        return new Connector0<TargetClass>(*this);
      }

      INLINE virtual IConnector0 *duplicate(Listener *target) {
        return new Connector0<TargetClass>((TargetClass *)target, __action);
      }

      INLINE virtual void notify(const void *src) { (__target->*__action)(src); }

      INLINE virtual Listener *target() const { return __target; }

      private:
      TargetClass *__target;
      void (TargetClass::*__action)(const void *);
    };

  } // namespace sig

  /**
   * @class Signaler0
   * @ingroup signal_group
   * @brief class for signal with 0 args
   * @warning Due to the too-smart-way-for-DOXYGEN of building classes
   * Signaler1,...,Signaler_n, these classes are not documented.
   */

  class Signaler0 : public __sig__::BasicSignaler0 {
    public:
    Signaler0() { GUM_CONSTRUCTOR(Signaler0); }

    Signaler0(const Signaler0 &s) : __sig__::BasicSignaler0(s) {
      GUM_CONS_CPY(Signaler0);
    }

    virtual ~Signaler0() { GUM_DESTRUCTOR(Signaler0); }

    template <class TargetClass>
    void attach(TargetClass *target, void (TargetClass::*action)(const void *)) {
      __sig__::Connector0<TargetClass> *conn =
          new __sig__::Connector0<TargetClass>(target, action);
      this->_connectors.push_back(conn);
      target->attachSignal__(this);
    }

    INLINE void operator()(const void *src) {
      for (const auto el : _connectors) {
        el->notify(src);
      }
    }
  };

} // namespace gum

/// this macro is the good way of emitting a signal.
#define GUM_EMIT0(signal) this->signal(this)

#endif // SIGNALER0_H__

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
