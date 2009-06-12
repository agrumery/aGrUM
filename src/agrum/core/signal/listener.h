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
/** @file
 * @brief Class of listener
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */
#ifndef LISTENER_H__
#define LISTENER_H__

#include <agrum/core/set.h>
#include <agrum/core/list.h>

namespace gum {

  class Listener;


  namespace __sig__ {

    /** @class ISignaler
       * @brief minimum specification of signaler in order to be contained in a listener
       * @ingroup signal
       */

    class ISignaler {
      public:
        virtual void detachFromTarget( Listener* target ) = 0;
        virtual void duplicateTarget( const Listener* oldtarget, Listener* newtarget ) = 0;
    };
  } //namespace sig

  /** @class Listener
     * @brief every class who would catch signal from signaler should derive from Listener
     * @ingroup signal
     */

  class Listener {
    private:
      typedef Set<__sig__::ISignaler *> Senders_set;
      typedef Senders_set::iterator const_iterator; //Senders_set::const_iterator const_iterator;

    public:
      Listener() {
        GUM_CONSTRUCTOR( Listener );
      }

      Listener( const Listener& l ) {
        GUM_CONS_CPY( Listener );

        for ( const_iterator it = l.__senders.begin(); it != l.__senders.end() ; ++it ) {
          ( *it )->duplicateTarget( &l, this );
          __senders.insert( *it );
        }
      }

      virtual ~Listener() {
        GUM_DESTRUCTOR( Listener );

        for ( const_iterator it = __senders.begin(); it != __senders.end() ; ++it )
          ( *it )->detachFromTarget( this );

        __senders.clear();
      }

      INLINE void attachSignal__( __sig__::ISignaler* sender ) {
        __senders.insert( sender );
      }

      INLINE void detachSignal__( __sig__::ISignaler* sender ) {
        __senders.erase( sender );
      }

    private:
      Senders_set __senders;
  };
} // namespace gum

#define GUM_CONNECT(sender,signal,receiver,target) (sender).signal.attach(&(receiver),&target)

#endif // LISTENER_H__

