/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief Meta classes for signalers.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */

#ifndef SIGNALER_PATRON_ACCEPTED
#  error "This file should not be included directly. Please use signaler{x}.h"
#endif   // SIGNALER_PATRON_ACCEPTED

#include <agrum/tools/core/list.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace __sig__ {

    template < LIST_DECL_CLASSES >
    class MAKE_NAME(IConnector) {
      public:
      virtual ~MAKE_NAME(IConnector)() {}

      virtual Listener* target() const                                           = 0;
      virtual void      notify(const void*, LIST_DECL_ARGS)                      = 0;
      virtual MAKE_NAME(IConnector)< LIST_CLASSES >* clone()                     = 0;
      virtual MAKE_NAME(IConnector)< LIST_CLASSES >* duplicate(Listener* target) = 0;
    };

    template < LIST_DECL_CLASSES >
    class MAKE_NAME(BasicSignaler) : public ISignaler {
      protected:
      using ConnectorList     = List< MAKE_NAME(IConnector) < LIST_CLASSES >* > ;
      using ConnectorIterator = ListConstIteratorSafe< MAKE_NAME(IConnector) < LIST_CLASSES >* > ;

      MAKE_NAME(BasicSignaler)() {
        GUM_CONSTRUCTOR(MAKE_NAME(BasicSignaler));
        ;
      }

      MAKE_NAME(BasicSignaler)
      (const MAKE_NAME(BasicSignaler) & s) : ISignaler(s) {
        GUM_CONS_CPY(MAKE_NAME(BasicSignaler));

        for (const auto& connector: connectors_) {
          connector->target()->_attachSignal_(this);
          connectors_.pushBack(connector->clone());
        }
      }

      public:
      virtual ~MAKE_NAME(BasicSignaler)() {
        GUM_DESTRUCTOR(MAKE_NAME(BasicSignaler));

        for (const auto& connector: connectors_) {
          connector->target()->_detachSignal_(this);
          delete connector;
        }

        connectors_.clear();
      }

      bool hasListener() { return (!(connectors_.empty())); }

      void detach(Listener* target) {
        for (ConnectorIterator it = connectors_.reginSafe();   // safe iterator needed here
             it != connectors_.rendSafe();
             --it) {
          if ((*it)->target() == target) {
            delete *it;
            connectors_.erase(it);
            target->_detachSignal_(this);
            return;
          }
        }
      }

      protected:
      friend class Listener;

      void duplicateTarget(const Listener* oldtarget, Listener* newtarget) {
        for (const auto& connector: connectors_)
          if (connector->target() == oldtarget) {
            connectors_.pushBack(connector->duplicate(newtarget));
          }
      }

      void detachFromTarget(Listener* target) {
        ConnectorIterator itprev;

        for (ConnectorIterator it = connectors_.rbeginSafe();   // safe iterator needed here
             it != connectors_.rendSafe();) {
          itprev = it;
          --it;

          if ((*itprev)->target() == target) {
            delete *itprev;
            connectors_.erase(itprev);
          }
        }
      }

      ConnectorList connectors_;
    };

    template < class TargetClass, LIST_DECL_CLASSES >
    class MAKE_NAME(Connector) : public MAKE_NAME(IConnector)< LIST_CLASSES > {
      public:
      MAKE_NAME(Connector)() {
        GUM_CONSTRUCTOR(MAKE_NAME(Connector));
        _target_ = NULL;
        _action_ = NULL;
      }

      MAKE_NAME(Connector)
      (TargetClass* target, void (TargetClass::*action)(const void*, LIST_CLASSES)) {
        GUM_CONSTRUCTOR(MAKE_NAME(Connector));
        _target_ = target;
        _action_ = action;
      }

      MAKE_NAME(Connector)
      (const MAKE_NAME(Connector) < TargetClass, LIST_CLASSES > *src) :
          MAKE_NAME(IConnector)< LIST_CLASSES >(src) {
        GUM_CONS_CPY(MAKE_NAME(Connector));
      }

      virtual ~MAKE_NAME(Connector)() {
        GUM_DESTRUCTOR(MAKE_NAME(Connector));
        ;
      }

      INLINE virtual MAKE_NAME(IConnector)< LIST_CLASSES >* clone() {
        return new MAKE_NAME(Connector)< TargetClass, LIST_CLASSES >(*this);
      }

      INLINE virtual MAKE_NAME(IConnector)< LIST_CLASSES >* duplicate(Listener* target) {
        return new MAKE_NAME(Connector)< TargetClass, LIST_CLASSES >((TargetClass*)target,
                                                                     _action_);
      }

      INLINE virtual void notify(const void* src, LIST_DECL_ARGS) {
        (_target_->*_action_)(src, LIST_ARGS);
      }

      INLINE virtual Listener* target() const { return _target_; }

      private:
      TargetClass* _target_;
      void (TargetClass::*_action_)(const void*, LIST_CLASSES);
    };

  }   // namespace  __sig__

  template < LIST_DECL_CLASSES >
  class MAKE_NAME(Signaler) : public __sig__::MAKE_NAME(BasicSignaler)< LIST_CLASSES > {
    using ConnectorIterator
       = typename __sig__::MAKE_NAME(BasicSignaler)< LIST_CLASSES >::ConnectorIterator;

    public:
    using BasicSignaler = __sig__::MAKE_NAME(BasicSignaler)< LIST_CLASSES >;

    MAKE_NAME(Signaler)() {
      GUM_CONSTRUCTOR(MAKE_NAME(Signaler));
      ;
    }

    MAKE_NAME(Signaler)
    (const MAKE_NAME(Signaler) & s) : __sig__::MAKE_NAME(BasicSignaler)< LIST_CLASSES >(s) {
      GUM_CONS_CPY(MAKE_NAME(Signaler));
    }

    virtual ~MAKE_NAME(Signaler)() {
      GUM_DESTRUCTOR(MAKE_NAME(Signaler));
      ;
    }

    template < class TargetClass >
    void attach(TargetClass* target, void (TargetClass::*action)(const void*, LIST_CLASSES)) {
      __sig__::MAKE_NAME(Connector)< TargetClass, LIST_CLASSES >* conn
         = new __sig__::MAKE_NAME(Connector)< TargetClass, LIST_CLASSES >(target, action);
      this->connectors_.pushBack(conn);
      target->_attachSignal_(this);
    }

    INLINE void operator()(const void* src, LIST_DECL_ARGS) {
      for (const auto& connector: this->connectors_) {
        connector->notify(src, LIST_ARGS);
      }
    }
  };

}   // namespace gum

#endif   // DOXYGEN_SHOULD_SKIP_THIS

#undef MAKE_NAME
#undef LIST_DECL_CLASSES
#undef LIST_CLASSES
#undef LIST_DECL_ARGS
#undef LIST_ARGS
#undef SIGNALER_PATRON_ACCEPTED
