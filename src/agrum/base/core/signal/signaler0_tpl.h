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

// To help IDE parsers
#include <agrum/base/core/signal/signaler0.h>

namespace gum {
  namespace __sig__ {

    template < class TargetClass >
    Connector0< TargetClass >::Connector0() {
      GUM_CONSTRUCTOR(Connector0);
      _target_ = nullptr;
      _action_ = nullptr;
    }

    template < class TargetClass >
    Connector0< TargetClass >::Connector0(TargetClass* target,
                                          void (TargetClass::*action)(const void*)) {
      GUM_CONSTRUCTOR(Connector0);
      _target_ = target;
      _action_ = action;
    }

    template < class TargetClass >
    Connector0< TargetClass >::Connector0(const Connector0< TargetClass >* src) : IConnector0(src) {
      GUM_CONS_CPY(Connector0);
    }

    template < class TargetClass >
    Connector0< TargetClass >::~Connector0() {
      GUM_DESTRUCTOR(Connector0);
    }

    template < class TargetClass >
    IConnector0* Connector0< TargetClass >::clone() {
      return new Connector0< TargetClass >(*this);
    }

    template < class TargetClass >
    IConnector0* Connector0< TargetClass >::duplicate(Listener* target) {
      return new Connector0< TargetClass >((TargetClass*)target, _action_);
    }

    template < class TargetClass >
    void Connector0< TargetClass >::notify(const void* src) {
      (_target_->*_action_)(src);
    }

    template < class TargetClass >
    Listener* Connector0< TargetClass >::target() const {
      return _target_;
    }

  }   // namespace  __sig__

}   // namespace gum
