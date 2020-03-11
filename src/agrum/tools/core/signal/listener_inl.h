
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
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
 * @author Pierre-Henri WUILLEMIN (@LIP6) and Christophe GONZALES (@AMU)
 *
 */

// To help IDE Parsers
#include <agrum/tools/core/signal/listener.h>

namespace gum {

  INLINE void Listener::attachSignal__(__sig__::ISignaler* sender) {
    __senders.push_back(sender);
  }

  INLINE void Listener::detachSignal__(__sig__::ISignaler* sender) {
    auto del = std::remove(__senders.begin(), __senders.end(), sender);

    if (del != __senders.end()) __senders.erase(del, __senders.end());
  }

}   // namespace gum
