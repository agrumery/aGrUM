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

#include <agrum/FMDP/learning/datastructure/leaves/concreteLeaf.h>

namespace gum {


  template < TESTNAME AttributeSelection, bool isScalar >
  INLINE ConcreteLeaf< AttributeSelection, isScalar >::ConcreteLeaf(
      NodeId                                        leafId,
      NodeDatabase< AttributeSelection, isScalar >* n1,
      const Sequence< ValueType >*                  valueDomain) :
      AbstractLeaf(leafId), _n1_(n1), _valueDomain_(valueDomain) {
    GUM_CONSTRUCTOR(ConcreteLeaf);
  }

  template < TESTNAME AttributeSelection, bool isScalar >
  INLINE ConcreteLeaf< AttributeSelection, isScalar >::~ConcreteLeaf() {
    GUM_DESTRUCTOR(ConcreteLeaf);
    ;
  }

  template < TESTNAME AttributeSelection, bool isScalar >
  INLINE void* ConcreteLeaf< AttributeSelection, isScalar >::operator new(size_t s) {
    return SmallObjectAllocator::instance().allocate(s);
  }

  template < TESTNAME AttributeSelection, bool isScalar >
  INLINE void ConcreteLeaf< AttributeSelection, isScalar >::operator delete(void* p) {
    SmallObjectAllocator::instance().deallocate(p, sizeof(ConcreteLeaf));
  }

  template < TESTNAME AttributeSelection, bool isScalar >
  INLINE double ConcreteLeaf< AttributeSelection, isScalar >::effectif(Idx moda) const {
    return _effectif_(moda, Int2Type< isScalar >());
  }

  template < TESTNAME AttributeSelection, bool isScalar >
  INLINE double ConcreteLeaf< AttributeSelection, isScalar >::_effectif_(Idx moda,
                                                                         Int2Type< true >) const {
    return (double)_n1_->effectif(Idx(_valueDomain_->atPos(moda)));
  }

  template < TESTNAME AttributeSelection, bool isScalar >
  INLINE double ConcreteLeaf< AttributeSelection, isScalar >::_effectif_(Idx moda,
                                                                         Int2Type< false >) const {
    return (double)_n1_->effectif(moda);
  }

  template < TESTNAME AttributeSelection, bool isScalar >
  INLINE double ConcreteLeaf< AttributeSelection, isScalar >::total() const {
    return double(_n1_->nbObservation());
  }

  template < TESTNAME AttributeSelection, bool isScalar >
  INLINE Idx ConcreteLeaf< AttributeSelection, isScalar >::nbModa() const {
    return _nbModa_(Int2Type< isScalar >());
  }

  template < TESTNAME AttributeSelection, bool isScalar >
  INLINE Idx ConcreteLeaf< AttributeSelection, isScalar >::_nbModa_(Int2Type< true >) const {
    return _valueDomain_->size();
  }

  template < TESTNAME AttributeSelection, bool isScalar >
  INLINE Idx ConcreteLeaf< AttributeSelection, isScalar >::_nbModa_(Int2Type< false >) const {
    return _n1_->valueDomain();
  }

  template < TESTNAME AttributeSelection, bool isScalar >
  INLINE std::string ConcreteLeaf< AttributeSelection, isScalar >::toString() {
    return std::format("{{ Id : {}}}", this->id());
  }

}   // namespace gum
