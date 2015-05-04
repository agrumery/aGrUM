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

#ifndef SIGNALER_PATRON_ACCEPTED
#error "This file should not be included directly. Please use signaler{x}.h"
#endif // SIGNALER_PATRON_ACCEPTED

namespace gum {

#ifndef DOXYGEN_SHOULD_SKIP_THIS

  namespace __sig__ {

    template <LIST_DECL_CLASSES> class MAKE_NAME(IConnector) {
      public:
      virtual ~MAKE_NAME(IConnector)() {}

      virtual Listener *target() const = 0;
      virtual void notify(const void *, LIST_DECL_ARGS) = 0;
      virtual MAKE_NAME(IConnector)<LIST_CLASSES> *clone() = 0;
      virtual MAKE_NAME(IConnector)<LIST_CLASSES> *duplicate(Listener *target) = 0;
    };

    template <LIST_DECL_CLASSES> class MAKE_NAME(BasicSignaler) : public ISignaler {
      protected:
      typedef List<MAKE_NAME(IConnector)<LIST_CLASSES> *> ConnectorList;
      typedef ListConstIteratorSafe<MAKE_NAME(IConnector)<LIST_CLASSES> *>
          ConnectorIterator;

      MAKE_NAME(BasicSignaler)() { GUM_CONSTRUCTOR(MAKE_NAME(BasicSignaler)); }

      MAKE_NAME(BasicSignaler)(const MAKE_NAME(BasicSignaler) & s) : ISignaler(s) {
        GUM_CONS_CPY(MAKE_NAME(BasicSignaler));

        for (const auto &connector : _connectors) {
          connector->target()->attachSignal__(this);
          _connectors.pushBack(connector->clone());
        }
      }

      public:
      virtual ~MAKE_NAME(BasicSignaler)() {
        GUM_DESTRUCTOR(MAKE_NAME(BasicSignaler));

        for (const auto &connector : _connectors) {
          connector->target()->detachSignal__(this);
          delete connector;
        }

        _connectors.clear();
      }

      bool hasListener(void) { return (!(_connectors.empty())); }

      void detach(Listener *target) {
        for (ConnectorIterator it =
                 _connectors.reginSafe(); // safe iterator needed here
             it != _connectors.rendSafe();
             --it) {
          if ((*it)->target() == target) {
            delete *it;
            _connectors.erase(it);
            target->detachSignal__(this);
            return;
          }
        }
      }

      protected:
      friend class Listener;

      void duplicateTarget(const Listener *oldtarget, Listener *newtarget) {
        for (const auto &connector : _connectors)
          if (connector->target() == oldtarget) {
            _connectors.pushBack(connector->duplicate(newtarget));
          }
      }

      void detachFromTarget(Listener *target) {
        ConnectorIterator itprev;

        for (ConnectorIterator it =
                 _connectors.rbeginSafe(); // safe iterator needed here
             it != _connectors.rendSafe();) {
          itprev = it;
          --it;

          if ((*itprev)->target() == target) {
            delete *itprev;
            _connectors.erase(itprev);
          }
        }
      }

      ConnectorList _connectors;
    };

    template <class TargetClass, LIST_DECL_CLASSES>
    class MAKE_NAME(Connector)
        : public MAKE_NAME(IConnector)<LIST_CLASSES> {
      public:
      MAKE_NAME(Connector)() {
        GUM_CONSTRUCTOR(MAKE_NAME(Connector));
        __target = NULL;
        __action = NULL;
      }

      MAKE_NAME(Connector)
      (TargetClass *target,
       void (TargetClass::*action)(const void *, LIST_CLASSES)) {
        GUM_CONSTRUCTOR(MAKE_NAME(Connector));
        __target = target;
        __action = action;
      }

      MAKE_NAME(Connector)
      (const MAKE_NAME(Connector)<TargetClass, LIST_CLASSES> *src)
          : MAKE_NAME(IConnector)<LIST_CLASSES>(src) {
        GUM_CONS_CPY(MAKE_NAME(Connector));
      }

      virtual ~MAKE_NAME(Connector)() { GUM_DESTRUCTOR(MAKE_NAME(Connector)); }

      INLINE virtual MAKE_NAME(IConnector)<LIST_CLASSES> *clone() {
        return new MAKE_NAME(Connector)<TargetClass, LIST_CLASSES>(*this);
      }

      INLINE virtual MAKE_NAME(IConnector)<LIST_CLASSES> *duplicate(
          Listener *target) {
        return new MAKE_NAME(Connector)<TargetClass, LIST_CLASSES>(
            (TargetClass *)target, __action);
      }

      INLINE virtual void notify(const void *src, LIST_DECL_ARGS) {
        (__target->*__action)(src, LIST_ARGS);
      }

      INLINE virtual Listener *target() const { return __target; }

      private:
      TargetClass *__target;
      void (TargetClass::*__action)(const void *, LIST_CLASSES);
    };

  } // namespace sig

  template <LIST_DECL_CLASSES>
  class MAKE_NAME(Signaler)
      : public __sig__::MAKE_NAME(BasicSignaler)<LIST_CLASSES> {

    typedef typename __sig__::MAKE_NAME(
        BasicSignaler)<LIST_CLASSES>::ConnectorIterator ConnectorIterator;

    public:
    using BasicSignaler = __sig__::MAKE_NAME(BasicSignaler)<LIST_CLASSES>;

    MAKE_NAME(Signaler)() { GUM_CONSTRUCTOR(MAKE_NAME(Signaler)); }

    MAKE_NAME(Signaler)(const MAKE_NAME(Signaler) & s)
        : __sig__::MAKE_NAME(BasicSignaler)<LIST_CLASSES>(s) {
      GUM_CONS_CPY(MAKE_NAME(Signaler));
    }

    virtual ~MAKE_NAME(Signaler)() { GUM_DESTRUCTOR(MAKE_NAME(Signaler)); }

    template <class TargetClass>
    void attach(TargetClass *target,
                void (TargetClass::*action)(const void *, LIST_CLASSES)) {
      __sig__::MAKE_NAME(Connector)<TargetClass, LIST_CLASSES> *conn =
          new __sig__::MAKE_NAME(Connector)<TargetClass, LIST_CLASSES>(target,
                                                                       action);
      this->_connectors.pushBack(conn);
      target->attachSignal__(this);
    }

    INLINE void operator()(const void *src, LIST_DECL_ARGS) {
      for (const auto &connector : this->_connectors) {
        connector->notify(src, LIST_ARGS);
      }
    }
  };

#endif // DOXYGEN_SHOULD_SKIP_THIS

} // namespace gum

#undef MAKE_NAME
#undef LIST_DECL_CLASSES
#undef LIST_CLASSES
#undef LIST_DECL_ARGS
#undef LIST_ARGS
#undef SIGNALER_PATRON_ACCEPTED
