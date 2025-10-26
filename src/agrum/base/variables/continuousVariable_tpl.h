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
 * @brief ContinuousVariable.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <sstream>
#include <utility>

#include <agrum/agrum.h>

#include <agrum/base/variables/continuousVariable.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {


  /// Default constructor
  template < typename GUM_SCALAR >
  INLINE ContinuousVariable< GUM_SCALAR >::ContinuousVariable(const std::string& aName,
                                                              const std::string& aDesc,
                                                              GUM_SCALAR         lower_bound,
                                                              GUM_SCALAR         upper_bound) :
      IContinuousVariable(aName, aDesc), _lower_bound_(lower_bound), _upper_bound_(upper_bound) {
    if (_lower_bound_ > _upper_bound_) { std::swap(_upper_bound_, _lower_bound_); }
    GUM_CONSTRUCTOR(ContinuousVariable);
  }

  /// Copy Constructor.
  template < typename GUM_SCALAR >
  INLINE ContinuousVariable< GUM_SCALAR >::ContinuousVariable(
      const ContinuousVariable< GUM_SCALAR >& from) :
      IContinuousVariable(from), _lower_bound_(from._lower_bound_),
      _upper_bound_(from._upper_bound_) {
    GUM_CONS_CPY(ContinuousVariable);
  }

  /// generalized copy constructor
  template < typename GUM_SCALAR >
  template < typename TX_VAL >
  INLINE ContinuousVariable< GUM_SCALAR >::ContinuousVariable(
      const ContinuousVariable< TX_VAL >& from) :
      IContinuousVariable(from), _lower_bound_(GUM_SCALAR(from._lower_bound_)),
      _upper_bound_(GUM_SCALAR(from._upper_bound_)) {
    GUM_CONS_CPY(ContinuousVariable);
  }

  /// move constructor
  template < typename GUM_SCALAR >
  INLINE ContinuousVariable< GUM_SCALAR >::ContinuousVariable(
      ContinuousVariable< GUM_SCALAR >&& from) :
      IContinuousVariable(std::move(from)), _lower_bound_(from._lower_bound_),
      _upper_bound_(from._upper_bound_) {
    GUM_CONS_MOV(ContinuousVariable);
  }

  /// destructor
  template < typename GUM_SCALAR >
  INLINE ContinuousVariable< GUM_SCALAR >::~ContinuousVariable() {
    GUM_DESTRUCTOR(ContinuousVariable);
  }

  /// Copy Factory.
  template < typename GUM_SCALAR >
  INLINE ContinuousVariable< GUM_SCALAR >* ContinuousVariable< GUM_SCALAR >::clone() const {
    return new ContinuousVariable< GUM_SCALAR >(*this);
  }

  /// copy operator
  template < typename GUM_SCALAR >
  INLINE ContinuousVariable< GUM_SCALAR >&
         ContinuousVariable< GUM_SCALAR >::operator=(const ContinuousVariable< GUM_SCALAR >& from) {
    IContinuousVariable::operator=(from);
    _lower_bound_ = from._lower_bound_;
    _upper_bound_ = from._upper_bound_;
    return *this;
  }

  /// generalized copy operator
  template < typename GUM_SCALAR >
  template < typename TX_VAL >
  INLINE ContinuousVariable< GUM_SCALAR >&
         ContinuousVariable< GUM_SCALAR >::operator=(const ContinuousVariable< TX_VAL >& from) {
    IContinuousVariable::operator=(from);
    _lower_bound_ = GUM_SCALAR(from._lower_bound_);
    _upper_bound_ = GUM_SCALAR(from._upper_bound_);
    return *this;
  }

  /// move operator
  template < typename GUM_SCALAR >
  INLINE ContinuousVariable< GUM_SCALAR >&
         ContinuousVariable< GUM_SCALAR >::operator=(ContinuousVariable< GUM_SCALAR >&& from) {
    IContinuousVariable::operator=(std::move(from));
    _lower_bound_ = from._lower_bound_;
    _upper_bound_ = from._upper_bound_;
    return *this;
  }

  /// returns the GUM_SCALAR corresponding to a string (unspecialized version)
  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR ContinuousVariable< GUM_SCALAR >::operator[](const std::string& str) const {
    std::istringstream stream(str);
    GUM_SCALAR         value;
    stream >> value;

    if (belongs(value)) return value;
    else GUM_ERROR(OutOfBounds, "the value does not delong to the domain of the variable")
  }

  /// returns the lower bound of the domain of the variable
  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR ContinuousVariable< GUM_SCALAR >::lowerBound() const {
    return _lower_bound_;
  }

  /// returns the lower bound of the domain of the variable as a double
  template < typename GUM_SCALAR >
  INLINE double ContinuousVariable< GUM_SCALAR >::lowerBoundAsDouble() const {
    return (double)_lower_bound_;
  }

  /// returns the upper bound of the domain of the variable
  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR ContinuousVariable< GUM_SCALAR >::upperBound() const {
    return _upper_bound_;
  }

  /// returns the upper bound of the domain of the variable
  template < typename GUM_SCALAR >
  INLINE double ContinuousVariable< GUM_SCALAR >::upperBoundAsDouble() const {
    return (double)_upper_bound_;
  }

  /// updates the lower bound of the domain of the variable
  template < typename GUM_SCALAR >
  INLINE void ContinuousVariable< GUM_SCALAR >::setLowerBound(const GUM_SCALAR& new_bound) {
    if (new_bound <= _upper_bound_) _lower_bound_ = new_bound;
    else GUM_ERROR(OutOfBounds, "the new lower bound would be higher than the upper bound")
  }

  /// updates the lower bound of the domain of the variable
  template < typename GUM_SCALAR >
  INLINE void ContinuousVariable< GUM_SCALAR >::setLowerBoundFromDouble(const double new_bound) {
    setLowerBound((GUM_SCALAR)new_bound);
  }

  /// updates the lower bound of the domain of the variable
  template < typename GUM_SCALAR >
  INLINE void ContinuousVariable< GUM_SCALAR >::setUpperBound(const GUM_SCALAR& new_bound) {
    if (new_bound >= _lower_bound_) _upper_bound_ = new_bound;
    else GUM_ERROR(OutOfBounds, "the new upper bound would be lower than the lower bound")
  }

  /// updates the lower bound of the domain of the variable
  template < typename GUM_SCALAR >
  INLINE void ContinuousVariable< GUM_SCALAR >::setUpperBoundFromDouble(const double new_bound) {
    setUpperBound((GUM_SCALAR)new_bound);
  }

  /// returns the type of the variable
  template < typename GUM_SCALAR >
  INLINE VarType ContinuousVariable< GUM_SCALAR >::varType() const {
    return VarType::CONTINUOUS;
  }

  /// returns a string containing the value of the variable passed in argument
  template < typename GUM_SCALAR >
  INLINE std::string ContinuousVariable< GUM_SCALAR >::label(const GUM_SCALAR& value) const {
    if (belongs(value)) return std::to_string(value);
    GUM_ERROR(OutOfBounds, "the value does not belong to the domain of the variable")
  }

  /// Returns true if the param belongs to the domain of the variable
  template < typename GUM_SCALAR >
  INLINE bool ContinuousVariable< GUM_SCALAR >::belongs(const GUM_SCALAR& value) const {
    return (value >= _lower_bound_) && (value <= _upper_bound_);
  }

  /// returns the domain of the variable as a string
  template < typename GUM_SCALAR >
  INLINE std::string ContinuousVariable< GUM_SCALAR >::domain() const {
    std::ostringstream stream;
    stream << '[' << _lower_bound_ << ';' << _upper_bound_ << ']';
    return stream.str();
  }

  /// returns the domain of the variable as a string
  template < typename GUM_SCALAR >
  INLINE bool ContinuousVariable< GUM_SCALAR >::_checkSameDomain_(const gum::Variable& aRV) const {
    // we can assume that aRV is a ContinuousVariable
    const auto& cv = static_cast< const ContinuousVariable< GUM_SCALAR >& >(aRV);
    return cv._lower_bound_ == _lower_bound_ && cv._upper_bound_ == _upper_bound_;
  }

  /// string version of *this
  template < typename GUM_SCALAR >
  INLINE std::string ContinuousVariable< GUM_SCALAR >::toString() const {
    std::string str(this->name());
    str += domain();
    return str;
  }

  /// string version of *this using description attribute instead of name.
  template < typename GUM_SCALAR >
  INLINE std::string ContinuousVariable< GUM_SCALAR >::toStringWithDescription() const {
    std::string str(this->description());
    str += domain();
    return str;
  }

  /// for friendly displaying the content of the variable
  template < typename GUM_SCALAR >
  std::ostream& operator<<(std::ostream& stream, const ContinuousVariable< GUM_SCALAR >& var) {
    return stream << var.toString();
  }


} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
