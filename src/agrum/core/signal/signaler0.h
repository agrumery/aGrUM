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

#include "signaler.h"

namespace gum {

  namespace __sig__ {

    class IConnector0 {
      public:
        virtual ~IConnector0() { };

        virtual Listener* target() const = 0;
        virtual void notify( const void * ) = 0;
        virtual IConnector0* clone() = 0;
        virtual IConnector0* duplicate( Listener* target ) = 0;
    };

    class BasicSignaler0 : public ISignaler {
      public:
        typedef List<IConnector0 *>  ConnectorList;
        typedef ConnectorList::const_iterator constConnectors_iterator;
        typedef ConnectorList::iterator Connectors_iterator;

      protected:
        BasicSignaler0() {
          GUM_CONSTRUCTOR( BasicSignaler0 );
        }

        BasicSignaler0( const BasicSignaler0& s ) : ISignaler( s ) {
          GUM_CONS_CPY( BasicSignaler0 );

          for ( constConnectors_iterator it = _connectors.begin(); it != _connectors.end(); ++it ) {
            ( *it )->target()->attachSignal__( this );
            _connectors.pushBack(( *it )->clone() );
          }
        }

      public:
        virtual ~BasicSignaler0() {
          GUM_DESTRUCTOR( BasicSignaler0 );

          for ( constConnectors_iterator it = _connectors.begin(); it != _connectors.end(); ++it ) {
            ( *it )->target()->detachSignal__( this );
            delete *it;
          }

          _connectors.clear();
        }

        void detach( Listener* target ) {
          for ( Connectors_iterator it = _connectors.begin(); it != _connectors.end(); ++it )
            if (( *it )->target() == target ) {
              delete *it;
              _connectors.erase( it );
              target->detachSignal__( this );
              return;
            }
        }

      protected:

        void detachFromTarget( Listener* target ) {
          for ( Connectors_iterator it = _connectors.begin(); it != _connectors.end(); ) {
            Connectors_iterator itNext = it;
            ++itNext;

            if (( *it )->target() == target ) {
              delete *it;
              _connectors.erase( it );
            }

            it = itNext;
          }
        }

        void duplicateTarget( const Listener* oldtarget, Listener* newtarget ) {
          for ( constConnectors_iterator it = _connectors.begin(); it != _connectors.end(); ++it )
            if (( *it )->target() == oldtarget ) {
              _connectors.pushBack(( *it )->duplicate( newtarget ) );
            }
        }

        ConnectorList _connectors;
    };

    template<class TargetClass>

    class Connector0 : public IConnector0 {
      public:
        Connector0() {
          GUM_CONSTRUCTOR( Connector0 );
          __target = NULL;
          __action = NULL;
        }

        Connector0( TargetClass* target, void ( TargetClass::*action )( const void * ) ) {
          GUM_CONSTRUCTOR( Connector0 );
          __target = target;
          __action = action;
        }

        Connector0( const Connector0<TargetClass>* src ) : IConnector0( src ) {
          GUM_CONS_CPY( Connector0 );
        }

        virtual ~Connector0() {
          GUM_DESTRUCTOR( Connector0 );
        }

        INLINE virtual IConnector0* clone() {
          return new Connector0<TargetClass>( *this );
        }

        INLINE virtual IConnector0* duplicate( Listener* target ) {
          return new Connector0<TargetClass>(( TargetClass * )target, __action );
        }

        INLINE virtual void notify( const void * src ) {
          ( __target->*__action )( src );
        }

        INLINE virtual Listener* target() const {
          return __target;
        }

      private:
        TargetClass* __target;
        void ( TargetClass::* __action )( const void * );
    };
  } // namespace sig

  class Signaler0 : public __sig__::BasicSignaler0 {
    public:
      Signaler0() {
        GUM_CONSTRUCTOR( Signaler0 );
      }

      Signaler0( const Signaler0& s ) : __sig__::BasicSignaler0( s ) {
        GUM_CONS_CPY( Signaler0 );
      }

      virtual ~Signaler0() {
        GUM_DESTRUCTOR( Signaler0 );
      }

      template<class TargetClass>
      void attach( TargetClass* target, void ( TargetClass::*action )( const void * ) ) {
        __sig__::Connector0<TargetClass>* conn = new __sig__::Connector0<TargetClass>( target, action );
        this->_connectors.pushBack( conn );
        target->attachSignal__( this );
      }

      INLINE void operator()( const void *src ) {
        for ( constConnectors_iterator it = this->_connectors.begin(); it != this->_connectors.end(); ++it )
          ( *it )->notify( src );
      }
  };

} // namespace gum

#define GUM_EMIT0(signal) this->signal(this)

#endif // SIGNALER0_H__

