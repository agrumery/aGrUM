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


/**
 * @file
 * @brief Implementation of the MultiDimBijArray class.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 * @author Lionel TORTI
 */

#include <agrum/base/multidim/implementations/multiDimBijArray.h>

namespace gum {

  template < typename GUM_ELEMENT >
  MultiDimBijArray< GUM_ELEMENT >::MultiDimBijArray(const MultiDimBijArray< GUM_ELEMENT >& from) :
      MultiDimWithOffset< GUM_ELEMENT >(), _array_(from._array_), _name_(from._name_) {
    GUM_CONS_CPY(MultiDimBijArray);

    for (auto var: from.variablesSequence()) {
      MultiDimWithOffset< GUM_ELEMENT >::add(*var);
    }
  }

  template < typename GUM_ELEMENT >
  MultiDimBijArray< GUM_ELEMENT >::MultiDimBijArray(const VarBijection&                bijection,
                                                   const MultiDimArray< GUM_ELEMENT >& array) :
      MultiDimWithOffset< GUM_ELEMENT >(), _array_(array), _name_("MultiDimBijArray") {
    GUM_CONSTRUCTOR(MultiDimBijArray);

    for (auto var: array.variablesSequence()) {
      MultiDimWithOffset< GUM_ELEMENT >::add(*(bijection.second(var)));
    }
  }

  template < typename GUM_ELEMENT >
  MultiDimBijArray< GUM_ELEMENT >::MultiDimBijArray(const VarBijection&                   bijection,
                                                   const MultiDimBijArray< GUM_ELEMENT >& array) :
      MultiDimWithOffset< GUM_ELEMENT >(), _array_(array._array_), _name_("MultiDimBijArray") {
    GUM_CONSTRUCTOR(MultiDimBijArray);

    for (auto var: array.variablesSequence()) {
      MultiDimWithOffset< GUM_ELEMENT >::add(*(bijection.second(var)));
    }
  }

  template < typename GUM_ELEMENT >
  INLINE MultiDimBijArray< GUM_ELEMENT >::~MultiDimBijArray() {
    GUM_DESTRUCTOR(MultiDimBijArray);
  }

  template < typename GUM_ELEMENT >
  MultiDimBijArray< GUM_ELEMENT >&
      MultiDimBijArray< GUM_ELEMENT >::operator=(const MultiDimBijArray< GUM_ELEMENT >& from) {
    GUM_ERROR(OperationNotAllowed, "MultiDimBijArray are readonly.")
  }

  template < typename GUM_ELEMENT >
  INLINE MultiDimBijArray< GUM_ELEMENT >* MultiDimBijArray< GUM_ELEMENT >::newFactory() const {
    return new MultiDimBijArray< GUM_ELEMENT >(*this);
  }

  template < typename GUM_ELEMENT >
  INLINE const std::string& MultiDimBijArray< GUM_ELEMENT >::name() const {
    return _name_;
  }

  template < typename GUM_ELEMENT >
  INLINE void MultiDimBijArray< GUM_ELEMENT >::add(const DiscreteVariable& v) {
    GUM_ERROR(OperationNotAllowed, "MultiDimBijArray<GUM_ELEMENT> are read only.")
  }

  template < typename GUM_ELEMENT >
  INLINE void MultiDimBijArray< GUM_ELEMENT >::erase(const DiscreteVariable& v) {
    GUM_ERROR(OperationNotAllowed, "MultiDimBijArray<GUM_ELEMENT> are read only.")
  }

  template < typename GUM_ELEMENT >
  INLINE Size MultiDimBijArray< GUM_ELEMENT >::realSize() const {
    return (Size)0;
  }

  template < typename GUM_ELEMENT >
  INLINE void MultiDimBijArray< GUM_ELEMENT >::fill(const GUM_ELEMENT& d) const {
    GUM_ERROR(OperationNotAllowed, "MultiDimBijArray<GUM_ELEMENT> are read only.")
  }

  template < typename GUM_ELEMENT >
  INLINE void MultiDimBijArray< GUM_ELEMENT >::commitMultipleChanges_() {
    // Do nothing
  }

  template < typename GUM_ELEMENT >
  INLINE GUM_ELEMENT MultiDimBijArray< GUM_ELEMENT >::get(const Instantiation& i) const {
    if (i.isMaster(this)) {
      return _array_.values_[this->offsets_[&i]];
    } else {
      return _array_.values_[this->getOffs_(i)];
    }
  }

  template < typename GUM_ELEMENT >
  INLINE void MultiDimBijArray< GUM_ELEMENT >::set(const Instantiation& i,
                                                  const GUM_ELEMENT&    value) const {
    GUM_ERROR(OperationNotAllowed, "MultiDimBijArray<GUM_ELEMENT> are read only.")
  }

  template < typename GUM_ELEMENT >
  INLINE void MultiDimBijArray< GUM_ELEMENT >::populate(const std::vector< GUM_ELEMENT >& v) const {
    GUM_ERROR(OperationNotAllowed, "MultiDimBijArray<GUM_ELEMENT> are read only.")
  }

  template < typename GUM_ELEMENT >
  INLINE void
      MultiDimBijArray< GUM_ELEMENT >::populate(std::initializer_list< GUM_ELEMENT > l) const {
    GUM_ERROR(OperationNotAllowed, "MultiDimBijArray<GUM_ELEMENT> are read only.")
  }

  template < typename GUM_ELEMENT >
  INLINE GUM_ELEMENT& MultiDimBijArray< GUM_ELEMENT >::get_(const Instantiation& i) const {
    GUM_ERROR(OperationNotAllowed, "MultiDimBijArray<GUM_ELEMENT> are read only.")
  }

  template < typename GUM_ELEMENT >
  INLINE void MultiDimBijArray< GUM_ELEMENT >::replace_(const DiscreteVariable* x,
                                                       const DiscreteVariable* y) {
    MultiDimImplementation< GUM_ELEMENT >::replace_(x, y);
  }

}   // namespace gum
