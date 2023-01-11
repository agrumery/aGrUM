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
 * @brief Class of signalers.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */

// To help IDE parsers
#include <agrum/tools/core/signal/signaler0.h>

namespace gum {

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  namespace __sig__ {

    INLINE
    bool BasicSignaler0::hasListener() { return (!(connectors_.empty())); }

    INLINE
    void BasicSignaler0::detach(Listener* target) {
      auto it = std::find_if(connectors_.begin(), connectors_.end(), _find_target_(target));

      while (it != connectors_.end()) {
        delete *it;
        target->_detachSignal_(this);

        it = connectors_.erase(it);   // it is the next one
        it = std::find_if(it, connectors_.end(), _find_target_(target));
      }
    }

    INLINE
    void BasicSignaler0::detachFromTarget_(Listener* target) {
      auto it = std::find_if(connectors_.begin(), connectors_.end(), _find_target_(target));

      while (it != connectors_.end()) {
        delete *it;

        it = connectors_.erase(it);   // it is the next one
        it = std::find_if(it, connectors_.end(), _find_target_(target));
      }
    }

    INLINE
    void BasicSignaler0::duplicateTarget_(const Listener* oldtarget, Listener* newtarget) {
      auto it = std::find_if(connectors_.begin(), connectors_.end(), _find_target_(oldtarget));

      while (it != connectors_.end()) {
        connectors_.push_back((*it)->duplicate(newtarget));

        it++;
        it = std::find_if(it, connectors_.end(), _find_target_(oldtarget));
      }
    }

    INLINE
    std::function< bool(IConnector0* el) > BasicSignaler0::_find_target_(const gum::Listener* l) {
      return [=](IConnector0* el) -> bool { return el->target() == l; };
    }

  }   // namespace  __sig__
#endif

}   // namespace gum
