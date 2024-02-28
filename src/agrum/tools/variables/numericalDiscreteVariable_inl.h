/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/** @file
 * @brief Base class for NumericalDiscrete discrete random variables
 *
 * @author Christophe GONZALES(_at_AMU) & Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#include <algorithm>
#include <ostream>
#include <sstream>
#include <string>

#include <agrum/agrum.h>

#include <agrum/tools/core/hashTable.h>
#include <agrum/tools/variables/discreteVariable.h>

#include <agrum/tools/core/math/math_utils.h>

// to ease IDE parsers
#include <agrum/tools/variables/numericalDiscreteVariable.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  /// Default constructor
  INLINE NumericalDiscreteVariable::NumericalDiscreteVariable(const std::string& aName,
                                                              const std::string& aDesc) :
      DiscreteVariable(aName, aDesc) {
    // for debugging purposes
    GUM_CONSTRUCTOR(NumericalDiscreteVariable);
  }

  /// copy constructor
  INLINE
  NumericalDiscreteVariable::NumericalDiscreteVariable(const NumericalDiscreteVariable& from) :
      DiscreteVariable(from), _domain_(from._domain_) {
    // for debugging purposes
    GUM_CONS_CPY(NumericalDiscreteVariable);
  }

  /// move constructor
  INLINE NumericalDiscreteVariable::NumericalDiscreteVariable(NumericalDiscreteVariable&& from) :
      DiscreteVariable(std::move(from)), _domain_(std::move(from._domain_)) {
    from._domain_.clear();
    // for debugging purposes
    GUM_CONS_MOV(NumericalDiscreteVariable)
  }

  /// virtual copy constructor
  INLINE NumericalDiscreteVariable* NumericalDiscreteVariable::clone() const {
    return new NumericalDiscreteVariable(*this);
  }

  /// destructor
  INLINE NumericalDiscreteVariable::~NumericalDiscreteVariable() {
    GUM_DESTRUCTOR(NumericalDiscreteVariable);
  }

  /// copy operator
  INLINE NumericalDiscreteVariable&
      NumericalDiscreteVariable::operator=(const NumericalDiscreteVariable& from) {
    // avoid self assignment
    if (&from != this) {
      DiscreteVariable::operator=(from);
      _domain_ = from._domain_;
    }

    return *this;
  }

  /// move operator
  INLINE NumericalDiscreteVariable&
      NumericalDiscreteVariable::operator=(NumericalDiscreteVariable&& from) {
    // avoid self assignment
    if (&from != this) {
      DiscreteVariable::operator=(std::move(from));
      _domain_ = std::move(from._domain_);
      from._domain_.clear();
    }

    return *this;
  }

  /// returns the domain size of the discrete random variable
  INLINE Size NumericalDiscreteVariable::domainSize() const { return _domain_.size(); }

  /// returns the type of variable
  INLINE VarType NumericalDiscreteVariable::varType() const { return VarType::Numerical; }

  /// returns the index of a given label
  INLINE Idx NumericalDiscreteVariable::index(const std::string& aLabel) const {
    const auto x   = std::stod(aLabel);
    const Idx  ind = std::lower_bound(_domain_.begin(), _domain_.end(), x) - _domain_.begin();

    if (ind != _domain_.size() && _domain_[ind] == x) {
      return ind;
    } else {
      GUM_ERROR(NotFound, "label '" << aLabel << "' is unknown in " << toString());
    }
  }

  INLINE Idx NumericalDiscreteVariable::closestIndex(double val) const {
    const Idx ind = std::lower_bound(_domain_.begin(), _domain_.end(), val) - _domain_.begin();

    if (ind == _domain_.size()) return _domain_.size() - 1;
    if (ind == 0) return 0;

    if (_domain_[ind] - val < val - _domain_[ind - 1]) {
      return ind;
    } else {
      return ind - 1;
    }
  }

  /// returns the ith label
  INLINE std::string NumericalDiscreteVariable::label(Idx i) const {
    // note that if i is outside the domain, Sequence _domain_ will raise
    // an exception
    if (i < 0 || i >= _domain_.size())
      GUM_ERROR(OutOfBounds, "Index out of bounds : " << i << "for variable " << toString() << ".")
    return _generateLabel_(_domain_[i]);
  }

  /// get a numerical representation of the indice-th value.
  INLINE double NumericalDiscreteVariable::numerical(Idx i) const {
    if (i < 0 || i >= _domain_.size())
      GUM_ERROR(OutOfBounds, "Index out of bounds : " << i << "for variable " << toString() << ".");
    return _domain_[i];
  }

  /// returns the domain as a sequence of values
  INLINE const std::vector< double >& NumericalDiscreteVariable::numericalDomain() const {
    return _domain_;
  }

  INLINE bool NumericalDiscreteVariable::isValue(double value) const {
    const Idx ind = std::lower_bound(_domain_.begin(), _domain_.end(), value) - _domain_.begin();
    return (ind != _domain_.size() && _domain_[ind] == value);
  }

  /// substitute a value by another one
  INLINE void NumericalDiscreteVariable::changeValue(double old_value, double new_value) {
    if (!gum::isfinite< double >(new_value)) {
      GUM_ERROR(DefaultInLabel,
                "Value '" << new_value << "' is not allowed for variable " << name())
    }
    if (!isValue(old_value)) return;
    if (isValue(new_value)) {
      GUM_ERROR(DuplicateElement,
                "Value" << new_value << " already belongs to the domain of the variable");
    }
    eraseValue(old_value);
    addValue(new_value);
  }

  /// erase a value from the domain of the variable
  INLINE void NumericalDiscreteVariable::eraseValue(double value) {
    const Idx ind = std::lower_bound(_domain_.begin(), _domain_.end(), value) - _domain_.begin();
    if (ind < _domain_.size() && _domain_[ind] == value) { _domain_.erase(_domain_.begin() + ind); }
  }

  /// clear the domain of the variable
  INLINE void NumericalDiscreteVariable::eraseValues() { _domain_.clear(); }

  INLINE bool NumericalDiscreteVariable::_checkSameDomain_(const gum::Variable& aRV) const {
    // we can assume that aRV is a IntegerVariable
    const auto& cv = static_cast< const NumericalDiscreteVariable& >(aRV);
    if (domainSize() != cv.domainSize()) return false;
    return cv._domain_ == _domain_;
  }

  INLINE std::string NumericalDiscreteVariable::closestLabel(double val) const {
    return label(closestIndex(val));
  }

  INLINE void NumericalDiscreteVariable::addValue(double value) {
    if (!gum::isfinite< double >(value)) {
      GUM_ERROR(DefaultInLabel,
                "Value '" << value << "' is not allowed for variable " << toString())
    }
    if (isValue(value)) {
      GUM_ERROR(DuplicateElement,
                "Value " << value << " already belongs to the domain of the variable "
                         << toString());
    }
    _domain_.push_back(value);
    std::sort(_domain_.begin(), _domain_.end());
  }

  INLINE std::string NumericalDiscreteVariable::_generateLabel_(double f) const {
    return compact_tostr(f);
  }

  INLINE std::string NumericalDiscreteVariable::toFast() const {
    std::stringstream s;
    s << name() << domain();
    return s.str();
  }
} /* namespace gum */

#endif /* DOXYGEN SHOULD SKIP THIS */
