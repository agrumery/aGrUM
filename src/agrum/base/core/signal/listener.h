/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Class of listener.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */

#ifndef GUM_LISTENER_H
#define GUM_LISTENER_H

#include <algorithm>
#include <vector>

#include <agrum/agrum.h>

#include <agrum/base/core/debug.h>

namespace gum {

#ifndef DOXYGEN_SHOULD_SKIP_THIS

  class Listener;

  namespace __sig__ {

    /**
     * @class ISignaler
     * @headerfile listener.h <agrum/base/core/signal/listener.h>
     * @brief Minimum specification of signaler in order to be contained in a
     * listener.
     * @ingroup signal_group
     */
    class ISignaler {
      public:
      virtual ~ISignaler() {};
      virtual void detachFromTarget(Listener* target)                              = 0;
      virtual void duplicateTarget(const Listener* oldtarget, Listener* newtarget) = 0;
      virtual bool hasListener()                                                   = 0;
    };
  }   // namespace  __sig__

#endif   // DOXYGEN_SHOULD_SKIP_THIS

  /**
   * @class Listener
   * @headerfile listener.h <agrum/base/core/signal/listener.h>
   * @brief Every class who would catch signal from signaler should derive from
   * Listener.
   * @ingroup signal_group
   */
  class Listener {
    private:
    /// Alias for the list of signal senders.
    using Senders_list = std::vector< __sig__::ISignaler* >;

    public:
    /**
     * @brief Class constructor.
     */
    Listener();

    Listener(const Listener& l);

    virtual ~Listener();

    void _attachSignal_(__sig__::ISignaler* sender);

    void _detachSignal_(__sig__::ISignaler* sender);

    private:
    Senders_list _senders_;
  };
}   // namespace gum

#define GUM_CONNECT(sender, signal, receiver, target) (sender).signal.attach(&(receiver), &target)

#ifndef GUM_NO_INLINE
#  include <agrum/base/core/signal/listener_inl.h>
#endif   // GUM_NO_INLINE

#endif   // GUM_LISTENER_H
