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
 * @brief Class of signalers.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */

#include <agrum/base/core/signal/signaler0.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {
  namespace __sig__ {

    BasicSignaler0::BasicSignaler0() {
      GUM_CONSTRUCTOR(BasicSignaler0);
      ;
    }

    BasicSignaler0::BasicSignaler0(const BasicSignaler0& s) : ISignaler(s) {
      GUM_CONS_CPY(BasicSignaler0);

      for (const auto el: connectors_) {
        el->target()->_attachSignal_(this);
        connectors_.push_back(el->clone());
      }
    }

    BasicSignaler0::~BasicSignaler0() {
      GUM_DESTRUCTOR(BasicSignaler0);

      for (const auto el: connectors_) {
        el->target()->_detachSignal_(this);
        delete el;
      }

      connectors_.clear();
    }

  }   // namespace  __sig__

}   // namespace gum

#endif   // DOXYGEN_SHOULD_SKIP_THIS

#ifdef GUM_NO_INLINE
#  include <agrum/base/core/signal/signaler0_inl.h>
#endif   // GUM_NO_INLINE
