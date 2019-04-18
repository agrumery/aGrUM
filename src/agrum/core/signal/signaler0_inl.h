
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
 * @brief Class of signalers.
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */

// To help IDE parsers
#include <agrum/core/signal/signaler0.h>

namespace gum {

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  namespace __sig__ {

    INLINE
    bool BasicSignaler0::hasListener() { return (!(_connectors.empty())); }

    INLINE
    void BasicSignaler0::detach(Listener* target) {
      auto it = std::find_if(
         _connectors.begin(), _connectors.end(), __find_target(target));

      while (it != _connectors.end()) {
        delete *it;
        target->detachSignal__(this);

        it = _connectors.erase(it);   // it is the next one
        it = std::find_if(it, _connectors.end(), __find_target(target));
      }
    }

    INLINE
    void BasicSignaler0::_detachFromTarget(Listener* target) {
      auto it = std::find_if(
         _connectors.begin(), _connectors.end(), __find_target(target));

      while (it != _connectors.end()) {
        delete *it;

        it = _connectors.erase(it);   // it is the next one
        it = std::find_if(it, _connectors.end(), __find_target(target));
      }
    }

    INLINE
    void BasicSignaler0::_duplicateTarget(const Listener* oldtarget,
                                          Listener*       newtarget) {
      auto it = std::find_if(
         _connectors.begin(), _connectors.end(), __find_target(oldtarget));

      while (it != _connectors.end()) {
        _connectors.push_back((*it)->duplicate(newtarget));

        it++;
        it = std::find_if(it, _connectors.end(), __find_target(oldtarget));
      }
    }

    INLINE
    std::function< bool(IConnector0* el) >
       BasicSignaler0::__find_target(const gum::Listener* l) {
      return [=](IConnector0* el) -> bool { return el->target() == l; };
    }

  }   // namespace __sig__
#endif

}   // namespace gum
