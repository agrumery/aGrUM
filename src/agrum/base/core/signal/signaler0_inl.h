/****************************************************************************
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
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/
#pragma once


/**
 * @file
 * @brief Class of signalers.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */

// To help IDE parsers
#include <agrum/base/core/signal/signaler0.h>

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
