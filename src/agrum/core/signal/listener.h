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
#ifndef GUM_LISTENER_H__
#define GUM_LISTENER_H__

#include <vector>
#include <algorithm>

#include <agrum/core/debug.h>

namespace gum {

  class Listener;

  namespace __sig__ {

    /** @class ISignaler
     * @brief minimum specification of signaler in order to be contained
     * in a listener
     * @ingroup signal
     */
    class ISignaler {
      public:
      virtual ~ISignaler(){};
      virtual void detachFromTarget(Listener *target) = 0;
      virtual void duplicateTarget(const Listener *oldtarget,
                                   Listener *newtarget) = 0;
      virtual bool hasListener(void) = 0;
    };
  } // namespace sig

  /** @class Listener
   * @brief every class who would catch signal from signaler should derive
   * from Listener
   * @ingroup signal_group
   */
  class Listener {
    private:
    // typedef ListBase<__sig__::ISignaler*> Senders_list;
    // typedef ListBucket<__sig__::ISignaler*> Senders_bucket;
    typedef std::vector<__sig__::ISignaler *> Senders_list;

    public:
    Listener() { GUM_CONSTRUCTOR(Listener); };

    Listener(const Listener &l) {
      GUM_CONS_CPY(Listener);

      /*for ( const Senders_bucket* it = l.__senders.frontBucket ();
            it ; it = it->next() ) {
        ( **it )->duplicateTarget( &l, this );
        __senders.pushBack( **it );
      }*/
      for (const auto el : __senders) {
        el->duplicateTarget(&l, this);
      };
    };

    virtual ~Listener() {
      GUM_DESTRUCTOR(Listener);
      /*
            for ( const Senders_bucket* it = __senders.frontBucket ();
                  it; it = it->next() ) {
              ( **it )->detachFromTarget( this );
            }*/

      for (const auto el : __senders) {
        el->detachFromTarget(this);
      }

      __senders.clear();
    };

    inline void attachSignal__(__sig__::ISignaler *sender) {
      __senders.push_back(sender);
    };

    inline void detachSignal__(__sig__::ISignaler *sender) {
      //__senders.eraseByVal ( sender );
      auto del = std::remove(__senders.begin(), __senders.end(), sender);

      if (del != __senders.end())
        __senders.erase(del, __senders.end());
    };

    private:
    Senders_list __senders;
  };
} // namespace gum

#define GUM_CONNECT(sender, signal, receiver, target)                               \
  (sender).signal.attach(&(receiver), &target)

#endif // GUM_LISTENER_H__
