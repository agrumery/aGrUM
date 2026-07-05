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

#include <agrum/FMDP/learning/datastructure/leaves/leafPair.h>

namespace gum {


  INLINE LeafPair::LeafPair(AbstractLeaf* l1, AbstractLeaf* l2) : _l1_(l1), _l2_(l2) {
    GUM_CONSTRUCTOR(LeafPair);
  }

  INLINE LeafPair::~LeafPair() {
    GUM_DESTRUCTOR(LeafPair);
    ;
  }

  INLINE void* LeafPair::operator new(size_t s) {
    return SmallObjectAllocator::instance().allocate(s);
  }

  INLINE void LeafPair::operator delete(void* p) {
    SmallObjectAllocator::instance().deallocate(p, sizeof(LeafPair));
  }

  INLINE AbstractLeaf* LeafPair::firstLeaf() { return _l1_; }

  INLINE AbstractLeaf* LeafPair::secondLeaf() { return _l2_; }

  INLINE bool LeafPair::contains(NodeId testedId) {
    return _l1_->contains(testedId) || _l2_->contains(testedId);
  }

  INLINE AbstractLeaf* LeafPair::convert2Leaf(NodeId leafId) const {
    return new ComposedLeaf(leafId, _l1_, _l2_);
  }

  INLINE AbstractLeaf* LeafPair::otherLeaf(AbstractLeaf* l) const {
    return l == _l1_ ? _l2_ : _l1_;
  }

}   // namespace gum
