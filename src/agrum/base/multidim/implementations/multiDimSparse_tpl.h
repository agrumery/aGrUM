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
 * @brief Implementation of MultiDimSparse.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#include <agrum/base/multidim/implementations/multiDimSparse.h>   // to ease IDE parser

namespace gum {

  // Default constructor: creates an empty null dimensional matrix

  template < typename GUM_ELEMENT >
  MultiDimSparse< GUM_ELEMENT >::MultiDimSparse(const GUM_ELEMENT& default_value) :
      MultiDimWithOffset< GUM_ELEMENT >(), default_(default_value) {
    // for debugging purposes
    GUM_CONSTRUCTOR(MultiDimSparse);
  }

  // copy constructor

  template < typename GUM_ELEMENT >
  MultiDimSparse< GUM_ELEMENT >::MultiDimSparse(const MultiDimSparse< GUM_ELEMENT >& from) :
      MultiDimWithOffset< GUM_ELEMENT >(from), params_(from.params_), default_(from.default_) {
    // for debugging purposes
    GUM_CONS_CPY(MultiDimSparse);
  }

  // destructor

  template < typename GUM_ELEMENT >
  MultiDimSparse< GUM_ELEMENT >::~MultiDimSparse() {
    // for debugging purposes
    GUM_DESTRUCTOR(MultiDimSparse);
    // no need to unregister all slaves as it will be done by MultiDimWithOffset
  }

  // data access operator

  template < typename GUM_ELEMENT >
  GUM_ELEMENT MultiDimSparse< GUM_ELEMENT >::get(const Instantiation& i) const {
    Size key;

    if (i.isMaster(this)) {
      key = this->offsets_[&i];
    } else {
      key = this->getOffs_(i);
    }

    return params_.exists(key) ? params_[key] : default_;
  }

  template < typename GUM_ELEMENT >
  void MultiDimSparse< GUM_ELEMENT >::set(const Instantiation& i, const GUM_ELEMENT& value) const {
    Size key;

    if (i.isMaster(this)) {
      key = this->offsets_[&i];
    } else {
      key = this->getOffs_(i);
    }

    if (value == default_) {
      params_.reset(key);
    } else {
      params_.set(key, value);
    }
  }

  // add a new dimension, needed for updating the offsets_ & gaps_

  template < typename GUM_ELEMENT >
  void MultiDimSparse< GUM_ELEMENT >::add(const DiscreteVariable& v) {
    MultiDimWithOffset< GUM_ELEMENT >::add(v);
    fill(default_);
  }

  // removes a dimension, needed for updating the offsets_ & gaps_

  template < typename GUM_ELEMENT >
  void MultiDimSparse< GUM_ELEMENT >::erase(const DiscreteVariable& v) {
    MultiDimWithOffset< GUM_ELEMENT >::erase(v);
    fill(default_);
  }

  // synchronise content after MultipleChanges
  template < typename GUM_ELEMENT >
  void MultiDimSparse< GUM_ELEMENT >::commitMultipleChanges_() {
    fill(default_);
  }

  // fill the array with the arg
  template < typename GUM_ELEMENT >
  void MultiDimSparse< GUM_ELEMENT >::fill(const GUM_ELEMENT& d) const {
    params_.clear();
    default_ = d;
  }

  template < typename GUM_ELEMENT >
  Size MultiDimSparse< GUM_ELEMENT >::realSize() const {
    return params_.size();
  }

  template < typename GUM_ELEMENT >
  MultiDimContainer< GUM_ELEMENT >* MultiDimSparse< GUM_ELEMENT >::newFactory() const {
    return new MultiDimSparse< GUM_ELEMENT >(default_);
  }

  // returns the name of the implementation
  template < typename GUM_ELEMENT >
  const std::string& MultiDimSparse< GUM_ELEMENT >::name() const {
    static const std::string str = "MultiDimSparse";
    return str;
  }

  template < typename GUM_ELEMENT >
  void MultiDimSparse< GUM_ELEMENT >::replace_(const DiscreteVariable* x,
                                               const DiscreteVariable* y) {
    MultiDimImplementation< GUM_ELEMENT >::replace_(x, y);
  }

  template < typename GUM_ELEMENT >
  GUM_ELEMENT& MultiDimSparse< GUM_ELEMENT >::get_(const Instantiation& i) const {
    GUM_ERROR(OperationNotAllowed, "Do not use this with the MultiDimSparse class.")
  }

} /* namespace gum */
