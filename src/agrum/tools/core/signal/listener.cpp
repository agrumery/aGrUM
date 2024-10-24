/**
 *
 *   Copyright (c) 2005-2024  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */

#include <agrum/tools/core/signal/listener.h>

namespace gum {

  Listener::Listener() {
    GUM_CONSTRUCTOR(Listener);
    ;
  }

  Listener::Listener(const Listener& l) {
    GUM_CONS_CPY(Listener);

    for (const auto el: _senders_) {
      el->duplicateTarget(&l, this);
    };
  }

  Listener::~Listener() {
    GUM_DESTRUCTOR(Listener);

    for (const auto el: _senders_) {
      el->detachFromTarget(this);
    }

    _senders_.clear();
  }

}   // namespace gum

#ifdef GUM_NO_INLINE
#  include <agrum/tools/core/signal/listener_inl.h>
#endif   // GUM_NO_INLINE
