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


/**
 * @file
 * @brief Implementation of MultiDimSparse.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

namespace gum {

  // Default constructor: creates an empty null dimensional matrix

  template < typename GUM_SCALAR >
  MultiDimSparse< GUM_SCALAR >::MultiDimSparse(const GUM_SCALAR& default_value) :
      MultiDimWithOffset< GUM_SCALAR >(), default_(default_value) {
    // for debugging purposes
    GUM_CONSTRUCTOR(MultiDimSparse);
  }

  // copy constructor

  template < typename GUM_SCALAR >
  MultiDimSparse< GUM_SCALAR >::MultiDimSparse(const MultiDimSparse< GUM_SCALAR >& from) :
      MultiDimWithOffset< GUM_SCALAR >(from), params_(from.params_), default_(from.default_) {
    // for debugging purposes
    GUM_CONS_CPY(MultiDimSparse);
  }

  // destructor

  template < typename GUM_SCALAR >
  MultiDimSparse< GUM_SCALAR >::~MultiDimSparse() {
    // for debugging purposes
    GUM_DESTRUCTOR(MultiDimSparse);
    // no need to unregister all slaves as it will be done by MultiDimWithOffset
  }

  // data access operator

  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR MultiDimSparse< GUM_SCALAR >::get(const Instantiation& i) const {
    Size key;

    if (i.isMaster(this)) {
      key = this->offsets_[&i];
    } else {
      key = this->getOffs_(i);
    }

    return params_.exists(key) ? params_[key] : default_;
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimSparse< GUM_SCALAR >::set(const Instantiation& i,
                                                const GUM_SCALAR&    value) const {
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

  template < typename GUM_SCALAR >
  INLINE void MultiDimSparse< GUM_SCALAR >::add(const DiscreteVariable& v) {
    MultiDimWithOffset< GUM_SCALAR >::add(v);
    fill(default_);
  }

  // removes a dimension, needed for updating the offsets_ & gaps_

  template < typename GUM_SCALAR >
  INLINE void MultiDimSparse< GUM_SCALAR >::erase(const DiscreteVariable& v) {
    MultiDimWithOffset< GUM_SCALAR >::erase(v);
    fill(default_);
  }

  // synchronise content after MultipleChanges
  template < typename GUM_SCALAR >
  INLINE void MultiDimSparse< GUM_SCALAR >::commitMultipleChanges_() {
    fill(default_);
  }

  // fill the array with the arg
  template < typename GUM_SCALAR >
  INLINE void MultiDimSparse< GUM_SCALAR >::fill(const GUM_SCALAR& d) const {
    params_.clear();
    default_ = d;
  }

  template < typename GUM_SCALAR >
  INLINE Size MultiDimSparse< GUM_SCALAR >::realSize() const {
    return params_.size();
  }

  template < typename GUM_SCALAR >
  INLINE MultiDimContainer< GUM_SCALAR >* MultiDimSparse< GUM_SCALAR >::newFactory() const {
    return new MultiDimSparse< GUM_SCALAR >(default_);
  }

  // returns the name of the implementation
  template < typename GUM_SCALAR >
  INLINE const std::string& MultiDimSparse< GUM_SCALAR >::name() const {
    static const std::string str = "MultiDimSparse";
    return str;
  }

  template < typename GUM_SCALAR >
  INLINE void MultiDimSparse< GUM_SCALAR >::replace_(const DiscreteVariable* x,
                                                     const DiscreteVariable* y) {
    MultiDimImplementation< GUM_SCALAR >::replace_(x, y);
  }

  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR& MultiDimSparse< GUM_SCALAR >::get_(const Instantiation& i) const {
    GUM_ERROR(OperationNotAllowed, "Do not use this with the MultiDimSparse class.")
  }

} /* namespace gum */
