/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief Class of listener.
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *
 */

#ifndef GUM_LISTENER_H
#define GUM_LISTENER_H

#include <algorithm>
#include <vector>

#include <agrum/agrum.h>
#include <agrum/tools/core/debug.h>

namespace gum {

#ifndef DOXYGEN_SHOULD_SKIP_THIS

  class Listener;

  namespace __sig__ {

    /**
     * @class ISignaler
     * @headerfile listener.h <agrum/tools/core/signal/listener.h>
     * @brief Minimum specification of signaler in order to be contained in a
     * listener.
     * @ingroup signal_group
     */
    class ISignaler {
      public:
      virtual ~ISignaler(){};
      virtual void detachFromTarget(Listener* target) = 0;
      virtual void duplicateTarget(const Listener* oldtarget,
                                   Listener*       newtarget) = 0;
      virtual bool hasListener() = 0;
    };
  }   // namespace __sig__

#endif   // DOXYGEN_SHOULD_SKIP_THIS

  /**
   * @class Listener
   * @headerfile listener.h <agrum/tools/core/signal/listener.h>
   * @brief Every class who would catch signal from signaler should derive from
   * Listener.
   * @ingroup signal_group
   */
  class Listener {
    private:
    /// Alias for the list of signal senders.
    typedef std::vector< __sig__::ISignaler* > Senders_list;

    public:
    /**
     * @brief Class constructor.
     */
    Listener();

    Listener(const Listener& l);

    virtual ~Listener();

    void attachSignal__(__sig__::ISignaler* sender);

    void detachSignal__(__sig__::ISignaler* sender);

    private:
    Senders_list senders__;
  };
}   // namespace gum

#define GUM_CONNECT(sender, signal, receiver, target) \
  (sender).signal.attach(&(receiver), &target)

#ifndef GUM_NO_INLINE
#  include <agrum/tools/core/signal/listener_inl.h>
#endif   // GUM_NO_INLINE

#endif   // GUM_LISTENER_H
