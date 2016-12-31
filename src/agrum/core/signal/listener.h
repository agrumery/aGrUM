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
 * @brief Class of listener.
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */

#ifndef GUM_LISTENER_H
#define GUM_LISTENER_H

#include <algorithm>
#include <vector>

#include <agrum/config.h>
#include <agrum/core/debug.h>

namespace gum {

#ifndef DOXYGEN_SHOULD_SKIP_THIS

  class Listener;

  namespace __sig__ {

    /**
     * @class ISignaler listener.h <agrum/core/signal/listener.h>
     * @brief Minimum specification of signaler in order to be contained in a
     * listener.
     * @ingroup signal_group
     */
    class ISignaler {
      public:
      virtual ~ISignaler(){};
      virtual void detachFromTarget( Listener* target ) = 0;
      virtual void duplicateTarget( const Listener* oldtarget,
                                    Listener*       newtarget ) = 0;
      virtual bool hasListener( void ) = 0;
    };
  }  // namespace __sig__

#endif  // DOXYGEN_SHOULD_SKIP_THIS

  /**
   * @class Listener listener.h <agrum/core/signal/listener.h>
   * @brief Every class who would catch signal from signaler should derive from
   * Listener.
   * @ingroup signal_group
   */
  class Listener {
    private:
    /// Alias for the list of signal senders.
    typedef std::vector<__sig__::ISignaler*> Senders_list;

    public:
    /**
     * @brief Class constructor.
     */
    Listener();

    Listener( const Listener& l );

    virtual ~Listener();

    void attachSignal__( __sig__::ISignaler* sender );

    void detachSignal__( __sig__::ISignaler* sender );

    private:
    Senders_list __senders;
  };
}  // namespace gum

#define GUM_CONNECT( sender, signal, receiver, target ) \
  ( sender ).signal.attach( &( receiver ), &target )

#ifndef GUM_NO_INLINE
#include <agrum/core/signal/listener_inl.h>
#endif  // GUM_NO_INLINE

#endif  // GUM_LISTENER_H
