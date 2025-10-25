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
 * @brief Implementation of the MultiDimBijArray class.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 * @author Lionel TORTI
 */

#include <agrum/base/multidim/implementations/multiDimBijArray.h>

namespace gum {

  template < typename GUM_SCALAR >
  MultiDimBijArray< GUM_SCALAR >::MultiDimBijArray(const MultiDimBijArray< GUM_SCALAR >& from) :
      MultiDimWithOffset< GUM_SCALAR >(), _array_(from._array_), _name_(from._name_) {
    GUM_CONS_CPY(MultiDimBijArray);

    for (auto var: from.variablesSequence()) {
      MultiDimWithOffset< GUM_SCALAR >::add(*var);
    }
  }

  template < typename GUM_SCALAR >
  MultiDimBijArray< GUM_SCALAR >::MultiDimBijArray(const VarBijection&                bijection,
                                                   const MultiDimArray< GUM_SCALAR >& array) :
      MultiDimWithOffset< GUM_SCALAR >(), _array_(array), _name_("MultiDimBijArray") {
    GUM_CONSTRUCTOR(MultiDimBijArray);

    for (auto var: array.variablesSequence()) {
      MultiDimWithOffset< GUM_SCALAR >::add(*(bijection.second(var)));
    }
  }

  template < typename GUM_SCALAR >
  MultiDimBijArray< GUM_SCALAR >::MultiDimBijArray(const VarBijection&                   bijection,
                                                   const MultiDimBijArray< GUM_SCALAR >& array) :
      MultiDimWithOffset< GUM_SCALAR >(), _array_(array._array_), _name_("MultiDimBijArray") {
    GUM_CONSTRUCTOR(MultiDimBijArray);

    for (auto var: array.variablesSequence()) {
      MultiDimWithOffset< GUM_SCALAR >::add(*(bijection.second(var)));
    }
  }

  template < typename GUM_SCALAR >
  INLINE MultiDimBijArray< GUM_SCALAR >::~MultiDimBijArray() {
    GUM_DESTRUCTOR(MultiDimBijArray);
  }

  template < typename GUM_SCALAR >
  MultiDimBijArray< GUM_SCALAR >&
      MultiDimBijArray< GUM_SCALAR >::operator=(const MultiDimBijArray< GUM_SCALAR >& from) {
    GUM_ERROR(OperationNotAllowed, "MultiDimBijArray are readonly.")
  }

  template < typename GUM_SCALAR >
  INLINE MultiDimBijArray< GUM_SCALAR >* MultiDimBijArray< GUM_SCALAR >::newFactory() const {
    return new MultiDimBijArray< GUM_SCALAR >(*this);
  }

  template < typename GUM_SCALAR >
  INLINE const std::string& MultiDimBijArray< GUM_SCALAR >::name() const {
    return _name_;
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimBijArray< GUM_SCALAR >::add(const DiscreteVariable& v) {
    GUM_ERROR(OperationNotAllowed, "MultiDimBijArray<GUM_SCALAR> are read only.")
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimBijArray< GUM_SCALAR >::erase(const DiscreteVariable& v) {
    GUM_ERROR(OperationNotAllowed, "MultiDimBijArray<GUM_SCALAR> are read only.")
  }

  template < typename GUM_SCALAR >
  INLINE Size MultiDimBijArray< GUM_SCALAR >::realSize() const {
    return (Size)0;
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimBijArray< GUM_SCALAR >::fill(const GUM_SCALAR& d) const {
    GUM_ERROR(OperationNotAllowed, "MultiDimBijArray<GUM_SCALAR> are read only.")
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimBijArray< GUM_SCALAR >::commitMultipleChanges_() {
    // Do nothing
  }

  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR MultiDimBijArray< GUM_SCALAR >::get(const Instantiation& i) const {
    if (i.isMaster(this)) {
      return _array_.values_[this->offsets_[&i]];
    } else {
      return _array_.values_[this->getOffs_(i)];
    }
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimBijArray< GUM_SCALAR >::set(const Instantiation& i,
                                                  const GUM_SCALAR&    value) const {
    GUM_ERROR(OperationNotAllowed, "MultiDimBijArray<GUM_SCALAR> are read only.")
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimBijArray< GUM_SCALAR >::populate(const std::vector< GUM_SCALAR >& v) const {
    GUM_ERROR(OperationNotAllowed, "MultiDimBijArray<GUM_SCALAR> are read only.")
  }

  template < typename GUM_SCALAR >
  INLINE void
      MultiDimBijArray< GUM_SCALAR >::populate(std::initializer_list< GUM_SCALAR > l) const {
    GUM_ERROR(OperationNotAllowed, "MultiDimBijArray<GUM_SCALAR> are read only.")
  }

  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR& MultiDimBijArray< GUM_SCALAR >::get_(const Instantiation& i) const {
    GUM_ERROR(OperationNotAllowed, "MultiDimBijArray<GUM_SCALAR> are read only.")
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimBijArray< GUM_SCALAR >::replace_(const DiscreteVariable* x,
                                                       const DiscreteVariable* y) {
    MultiDimImplementation< GUM_SCALAR >::replace_(x, y);
  }

}   // namespace gum
