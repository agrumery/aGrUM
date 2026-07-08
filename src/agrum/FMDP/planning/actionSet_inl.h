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

// to ease IDE parser
#include <agrum/FMDP/planning/actionSet.h>

namespace gum {

  INLINE ActionSet::ActionSet() {
    GUM_CONSTRUCTOR(ActionSet);
    _actionSeq_ = new Sequence< Idx >();
  }

  INLINE ActionSet::ActionSet(const ActionSet& src) {
    GUM_CONSTRUCTOR(ActionSet);
    _actionSeq_ = new Sequence< Idx >();
    for (auto idi = src.beginSafe(); idi != src.endSafe(); ++idi)
      _actionSeq_->insert(*idi);
  }

  INLINE ActionSet& ActionSet::operator=(const ActionSet& src) {
    _actionSeq_ = new Sequence< Idx >();
    for (auto idi = src.beginSafe(); idi != src.endSafe(); ++idi)
      _actionSeq_->insert(*idi);
    return *this;
  }

  INLINE ActionSet::~ActionSet() {
    GUM_DESTRUCTOR(ActionSet);
    delete _actionSeq_;
  }

  INLINE void* ActionSet::operator new(size_t s) {
    return SmallObjectAllocator::instance().allocate(s);
  }

  INLINE void ActionSet::operator delete(void* p) {
    SmallObjectAllocator::instance().deallocate(p, sizeof(ActionSet));
  }

  INLINE SequenceIteratorSafe< Idx > ActionSet::beginSafe() const {
    return _actionSeq_->beginSafe();
  }

  INLINE SequenceIteratorSafe< Idx > ActionSet::endSafe() const { return _actionSeq_->endSafe(); }

  INLINE ActionSet& ActionSet::operator+=(const Idx& elem) {
    _actionSeq_->insert(elem);
    return *this;
  }

  INLINE ActionSet& ActionSet::operator+=(const ActionSet& src) {
    for (auto iter = src.beginSafe(); iter != src.endSafe(); ++iter)
      if (!_actionSeq_->exists(*iter)) _actionSeq_->insert(*iter);
    return *this;
  }

  INLINE ActionSet& ActionSet::operator-=(const ActionSet& src) {
    for (auto iter = src.beginSafe(); iter != src.endSafe(); ++iter)
      if (_actionSeq_->exists(*iter)) _actionSeq_->erase(*iter);
    return *this;
  }

  INLINE bool ActionSet::operator==(const ActionSet& compared) const {
    for (auto iter = compared.beginSafe(); iter != compared.endSafe(); ++iter)
      if (!_actionSeq_->exists(*iter)) return false;
    for (auto iter = this->beginSafe(); iter != this->endSafe(); ++iter)
      if (!compared.exists(*iter)) return false;
    return true;
  }

  INLINE Size ActionSet::size() const { return _actionSeq_->size(); }

  INLINE bool ActionSet::exists(const Idx& elem) const { return _actionSeq_->exists(elem); }

  INLINE std::ostream& operator<<(std::ostream& streamy, const ActionSet& objy) {
    streamy << objy._actionSeq_->toString();
    return streamy;
  }

}   // namespace gum
