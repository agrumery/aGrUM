
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @author Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *
 */

#include <agrum/tools/core/signal/signaler0.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {
  namespace __sig__ {

    BasicSignaler0::BasicSignaler0() { GUM_CONSTRUCTOR(BasicSignaler0); }

    BasicSignaler0::BasicSignaler0(const BasicSignaler0& s) : ISignaler(s) {
      GUM_CONS_CPY(BasicSignaler0);

      for (const auto el: connectors_) {
        el->target()->attachSignal__(this);
        connectors_.push_back(el->clone());
      }
    }

    BasicSignaler0::~BasicSignaler0() {
      GUM_DESTRUCTOR(BasicSignaler0);

      for (const auto el: connectors_) {
        el->target()->detachSignal__(this);
        delete el;
      }

      connectors_.clear();
    }

  }   // namespace __sig__

}   // namespace gum

#endif   // DOXYGEN_SHOULD_SKIP_THIS

#ifdef GUM_NO_INLINE
#  include <agrum/tools/core/signal/signaler0_inl.h>
#endif   // GUM_NO_INLINE
