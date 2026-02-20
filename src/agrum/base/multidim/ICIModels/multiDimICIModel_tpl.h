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
 * @brief A Interface to all Causal Independence models
 *
 * Causal Independence (CI) is a method of defining a discrete distribution
 * that can dramatically reduce the number of prior probabilities necessary to
 * define a distribution.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#include <agrum/base/core/bijection.h>
#include <agrum/base/multidim/ICIModels/multiDimICIModel.h>
#include <agrum/base/multidim/implementations/multiDimReadOnly.h>

namespace gum {

  // Default constructor
  template < typename GUM_SCALAR >
  INLINE MultiDimICIModel< GUM_SCALAR >::MultiDimICIModel(GUM_SCALAR external_weight,
                                                          GUM_SCALAR default_weight) :
      MultiDimReadOnly< GUM_SCALAR >(), _external_weight_(external_weight),
      _default_weight_(default_weight) {
    GUM_CONSTRUCTOR(MultiDimICIModel);
  }

  // Default constructor
  template < typename GUM_SCALAR >
  INLINE
      MultiDimICIModel< GUM_SCALAR >::MultiDimICIModel(const MultiDimICIModel< GUM_SCALAR >& from) :
      MultiDimReadOnly< GUM_SCALAR >(from) {
    GUM_CONS_CPY(MultiDimICIModel);
    _default_weight_  = from._default_weight_;
    _external_weight_ = from._external_weight_;
    _causal_weights_  = from._causal_weights_;
  }

  // Copy constructor using a bijection to replace variables from source.
  template < typename GUM_SCALAR >
  INLINE MultiDimICIModel< GUM_SCALAR >::MultiDimICIModel(
      const Bijection< const DiscreteVariable*, const DiscreteVariable* >& bij,
      const MultiDimICIModel< GUM_SCALAR >& from) : MultiDimReadOnly< GUM_SCALAR >() {
    GUM_CONSTRUCTOR(MultiDimICIModel);
    _default_weight_  = from._default_weight_;
    _external_weight_ = from._external_weight_;

    for (HashTableConstIteratorSafe< const DiscreteVariable*, GUM_SCALAR > iter
         = from._causal_weights_.beginSafe();
         iter != from._causal_weights_.endSafe();
         ++iter) {
      if (auto pFirst = bij.tryFirst(iter.key())) {
        causalWeight(**pFirst, iter.val());
      } else { causalWeight(*(iter.key()), iter.val()); }
    }
  }

  // destructor
  template < typename GUM_SCALAR >
  INLINE MultiDimICIModel< GUM_SCALAR >::~MultiDimICIModel() {
    GUM_DESTRUCTOR(MultiDimICIModel);
  }

  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR MultiDimICIModel< GUM_SCALAR >::causalWeight(const DiscreteVariable& v) const {
    auto p = _causal_weights_.tryGet(&v);
    return p ? *p : _default_weight_;
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimICIModel< GUM_SCALAR >::causalWeight(const DiscreteVariable& v,
                                                           GUM_SCALAR              w) const {
    if (!this->contains(v)) {
      GUM_ERROR(InvalidArgument, v.name() << " is not a cause for this CI Model")
    }

    if (w == (GUM_SCALAR)0) { GUM_ERROR(gum::OutOfBounds, "causal weight in CI Model>0") }

    _causal_weights_.set(&v, w);
  }

  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR MultiDimICIModel< GUM_SCALAR >::externalWeight() const {
    return _external_weight_;
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimICIModel< GUM_SCALAR >::externalWeight(GUM_SCALAR w) const {
    _external_weight_ = w;
  }

  template < typename GUM_SCALAR >
  std::string MultiDimICIModel< GUM_SCALAR >::toString() const {
    std::stringstream s;
    s << this->variable(0) << "=CIModel([" << externalWeight() << "]";

    for (Idx i = 1; i < this->nbrDim(); i++)
      s << ", " << this->variable(i) << "[" << causalWeight(this->variable(i)) << "]";

    s << ")";
    return s.str();
  }

  template < typename GUM_SCALAR >
  void MultiDimICIModel< GUM_SCALAR >::copyFrom(const MultiDimContainer< GUM_SCALAR >& src) const {
    auto p = dynamic_cast< const MultiDimICIModel< GUM_SCALAR >* >(&src);
    if (p == nullptr) MultiDimReadOnly< GUM_SCALAR >::copyFrom(src);
    else {
      if (src.domainSize() != this->domainSize()) {
        GUM_ERROR(OperationNotAllowed, "Domain sizes do not fit")
      }
      _external_weight_ = p->_external_weight_;
      _default_weight_  = p->_default_weight_;
      for (Idx i = 1; i < this->nbrDim(); i++) {
        _causal_weights_.set(const_cast< const DiscreteVariable* >(&this->variable(i)),
                             p->causalWeight(this->variable(i)));
      }
    }
  }

  // returns the name of the implementation
  template < typename GUM_SCALAR >
  INLINE const std::string& MultiDimICIModel< GUM_SCALAR >::name() const {
    static const std::string str = "MultiDimICIModel";
    return str;
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimICIModel< GUM_SCALAR >::replace_(const DiscreteVariable* x,
                                                       const DiscreteVariable* y) {
    MultiDimReadOnly< GUM_SCALAR >::replace_(x, y);
    _causal_weights_.insert(y, _causal_weights_[x]);
    _causal_weights_.erase(x);
  }

} /* namespace gum */
