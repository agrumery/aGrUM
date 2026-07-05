/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
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

#include <agrum/FMDP/learning/observation.h>

namespace gum {


  INLINE Observation::Observation() {
    GUM_CONSTRUCTOR(Observation);
    ;
  }

  INLINE Observation::~Observation() {
    GUM_DESTRUCTOR(Observation);
    ;
  }

  INLINE void* Observation::operator new(size_t s) {
    return SmallObjectAllocator::instance().allocate(s);
  }

  INLINE void Observation::operator delete(void* p) {
    SmallObjectAllocator::instance().deallocate(p, sizeof(Observation));
  }

  INLINE Idx Observation::modality(const DiscreteVariable* var) const { return _varInst_[var]; }

  INLINE Idx Observation::rModality(const DiscreteVariable* var) const { return _rInst_[var]; }

  INLINE void Observation::setModality(const DiscreteVariable* var, Idx modality) {
    _varInst_.insert(var, modality);
  }

  INLINE void Observation::setRModality(const DiscreteVariable* var, Idx modality) {
    _rInst_.insert(var, modality);
  }

  INLINE double Observation::reward() const { return _reward_; }

  INLINE void Observation::setReward(double reward) { _reward_ = reward; }

  INLINE HashTableConstIteratorSafe< const DiscreteVariable*, Idx >
         Observation::cbeginVariablesSafe() const {
    return _varInst_.cbeginSafe();
  }

  INLINE HashTableConstIteratorSafe< const DiscreteVariable*, Idx >
         Observation::cendVariablesSafe() const {
    return _varInst_.cendSafe();
  }

}   // namespace gum
