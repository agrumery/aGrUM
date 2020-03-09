
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

#include <agrum/tools/core/signal/signaler0.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {
  namespace __sig__ {

    BasicSignaler0::BasicSignaler0() { GUM_CONSTRUCTOR(BasicSignaler0); }

    BasicSignaler0::BasicSignaler0(const BasicSignaler0& s) : ISignaler(s) {
      GUM_CONS_CPY(BasicSignaler0);

      for (const auto el: _connectors) {
        el->target()->attachSignal__(this);
        _connectors.push_back(el->clone());
      }
    }

    BasicSignaler0::~BasicSignaler0() {
      GUM_DESTRUCTOR(BasicSignaler0);

      for (const auto el: _connectors) {
        el->target()->detachSignal__(this);
        delete el;
      }

      _connectors.clear();
    }

  }   // namespace __sig__

}   // namespace gum

#endif   // DOXYGEN_SHOULD_SKIP_THIS

#ifdef GUM_NO_INLINE
#  include <agrum/tools/core/signal/signaler0_inl.h>
#endif   // GUM_NO_INLINE
