
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */

#include <agrum/core/signal/listener.h>

namespace gum {

  Listener::Listener() { GUM_CONSTRUCTOR(Listener); }

  Listener::Listener(const Listener& l) {
    GUM_CONS_CPY(Listener);

    for (const auto el : __senders) {
      el->duplicateTarget(&l, this);
    };
  }

  Listener::~Listener() {
    GUM_DESTRUCTOR(Listener);

    for (const auto el : __senders) {
      el->detachFromTarget(this);
    }

    __senders.clear();
  }

}   // namespace gum

#ifdef GUM_NO_INLINE
#  include <agrum/core/signal/listener_inl.h>
#endif   // GUM_NO_INLINE
