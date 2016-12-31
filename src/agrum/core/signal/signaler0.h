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

/**
 * @file
 * @brief Class of gum::Signaler0.
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */

#ifndef GUM_SIGNALER0_H
#define GUM_SIGNALER0_H
#include <functional>

#include <agrum/config.h>
#include <agrum/core/list.h>
#include <agrum/core/signal/signaler.h>

namespace gum {

#ifndef DOXYGEN_SHOULD_SKIP_THIS

  namespace __sig__ {

    class IConnector0 {
      public:
      virtual ~IConnector0(){};

      virtual Listener*    target() const = 0;
      virtual void         notify( const void* ) = 0;
      virtual IConnector0* clone() = 0;
      virtual IConnector0* duplicate( Listener* target ) = 0;
    };

    class BasicSignaler0 : public ISignaler {
      protected:
      typedef std::vector<IConnector0*> ConnectorList;

      BasicSignaler0();

      BasicSignaler0( const BasicSignaler0& s );

      public:
      virtual ~BasicSignaler0();

      bool hasListener( void );

      void detach( Listener* target );

      protected:
      friend class Listener;

      void _detachFromTarget( Listener* target );

      void _duplicateTarget( const Listener* oldtarget, Listener* newtarget );

      ConnectorList _connectors;

      private:
      std::function<bool( IConnector0* el )>
      __find_target( const gum::Listener* l );
    };

    template <class TargetClass>
    class Connector0 : public IConnector0 {
      public:
      Connector0();

      Connector0( TargetClass* target,
                  void ( TargetClass::*action )( const void* ) );

      Connector0( const Connector0<TargetClass>* src );

      virtual ~Connector0();

      virtual IConnector0* clone();

      virtual IConnector0* duplicate( Listener* target );

      virtual void notify( const void* src );

      virtual Listener* target() const;

      private:
      TargetClass* __target;
      void ( TargetClass::*__action )( const void* );
    };

  }  // namespace __sig__

#endif  // DOXYGEN_SHOULD_SKIP_THIS

  /**
   * @class Signaler0
   * @ingroup signal_group
   * @brief class for signal with 0 args
   * @warning Due to the too-smart-way-for-DOXYGEN of building classes
   * Signaler1,...,Signaler_n, these classes are not documented.
   */
  class Signaler0 : public __sig__::BasicSignaler0 {
    public:
    Signaler0() { GUM_CONSTRUCTOR( Signaler0 ); }

    Signaler0( const Signaler0& s )
        : __sig__::BasicSignaler0( s ) {
      GUM_CONS_CPY( Signaler0 );
    }

    virtual ~Signaler0() { GUM_DESTRUCTOR( Signaler0 ); }

    template <class TargetClass>
    void attach( TargetClass* target,
                 void ( TargetClass::*action )( const void* ) ) {
      __sig__::Connector0<TargetClass>* conn =
          new __sig__::Connector0<TargetClass>( target, action );
      this->_connectors.push_back( conn );
      target->attachSignal__( this );
    }

    INLINE void operator()( const void* src ) {
      for ( const auto el : _connectors ) {
        el->notify( src );
      }
    }
  };

}  // namespace gum

/// this macro is the good way of emitting a signal.
#define GUM_EMIT0( signal ) this->signal( this )

// Template implementation should always be in headers
#include <agrum/core/signal/signaler0_tpl.h>

#ifndef GUM_NO_INLINE
#include <agrum/core/signal/signaler0_inl.h>
#endif  // GUM_NO_INLINE

#endif  // GUM_SIGNALER0_H
