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
 * @brief Base class for Integer discrete random variables
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

// to ease IDE parsers
#include <agrum/tools/variables/integerVariable.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  /// Default constructor
  INLINE IntegerVariable::IntegerVariable(const std::string& aName, const std::string& aDesc) :
      DiscreteVariable(aName, aDesc) {
    // for debugging purposes
    GUM_CONSTRUCTOR(IntegerVariable);
  }

  /// copy constructor
  INLINE IntegerVariable::IntegerVariable(const IntegerVariable& from) :
      DiscreteVariable(from), _domain_(from._domain_) {
    // for debugging purposes
    GUM_CONS_CPY(IntegerVariable);
  }

  /// move constructor
  INLINE IntegerVariable::IntegerVariable(IntegerVariable&& from) noexcept :
      DiscreteVariable(std::move(from)), _domain_(std::move(from._domain_)) {
    from._domain_.clear();
    // for debugging purposes
    GUM_CONS_MOV(IntegerVariable);
  }

  /// virtual copy constructor
  INLINE IntegerVariable* IntegerVariable::clone() const { return new IntegerVariable(*this); }

  /// destructor
  INLINE IntegerVariable::~IntegerVariable() { GUM_DESTRUCTOR(IntegerVariable); }

  /// copy operator
  INLINE IntegerVariable& IntegerVariable::operator=(const IntegerVariable& from) {
    // avoid self assignment
    if (&from != this) {
      DiscreteVariable::operator=(from);
      _domain_ = from._domain_;
    }

    return *this;
  }

  /// move operator
  INLINE IntegerVariable& IntegerVariable::operator=(IntegerVariable&& from) {
    // avoid self assignment
    if (&from != this) {
      DiscreteVariable::operator=(std::move(from));
      _domain_ = std::move(from._domain_);
      from._domain_.clear();
    }

    return *this;
  }

  /// returns the domain size of the discrete random variable
  INLINE Size IntegerVariable::domainSize() const { return _domain_.size(); }

  /// returns the type of variable
  INLINE VarType IntegerVariable::varType() const { return VarType::Integer; }

  /// returns the index of a given label
  INLINE Idx IntegerVariable::index(const std::string& aLabel) const {
    const auto x   = std::stoi(aLabel);
    const Idx  ind = std::lower_bound(_domain_.begin(), _domain_.end(), x) - _domain_.begin();

    if (ind != _domain_.size() && _domain_[ind] == x) {
      return ind;
    } else {
      GUM_ERROR(NotFound, "label '" << aLabel << "' is unknown in " << toString());
    }
  }

  INLINE Idx IntegerVariable::closestIndex(double val) const {
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
  INLINE std::string IntegerVariable::label(Idx i) const {
    // note that if i is outside the domain, Sequence _domain_ will raise
    // an exception
    if (i < 0 || i >= _domain_.size())
      GUM_ERROR(OutOfBounds, "Index out of bounds : " << i << "for variable " << toString() << ".")
    return std::to_string(_domain_[i]);
  }

  /// get a numerical representation of the indice-th value.
  INLINE double IntegerVariable::numerical(Idx i) const {
    if (i < 0 || i >= _domain_.size())
      GUM_ERROR(OutOfBounds, "Index out of bounds : " << i << "for variable " << toString() << ".")
    return double(_domain_[i]);
  }

  /// returns the domain as a sequence of values
  INLINE const std::vector< int >& IntegerVariable::integerDomain() const { return _domain_; }

  INLINE bool IntegerVariable::isValue(int value) const {
    const Idx ind = std::lower_bound(_domain_.begin(), _domain_.end(), value) - _domain_.begin();
    return (ind != _domain_.size() && _domain_[ind] == value);
  }

  /// substitute a value by another one
  INLINE void IntegerVariable::changeValue(int old_value, int new_value) {
    if (!isValue(old_value)) return;
    if (isValue(new_value)) {
      GUM_ERROR(DuplicateElement,
                "Value" << new_value << " already belongs to the domain of the variable");
    }

    eraseValue(old_value);
    addValue(new_value);
  }

  /// erase a value from the domain of the variable
  INLINE void IntegerVariable::eraseValue(int value) {
    const Idx ind = std::lower_bound(_domain_.begin(), _domain_.end(), value) - _domain_.begin();
    if (ind != _domain_.size() && _domain_[ind] == value) {
      _domain_.erase(_domain_.begin() + ind);
    }
  }

  /// clear the domain of the variable
  INLINE void IntegerVariable::eraseValues() { _domain_.clear(); }

  INLINE bool IntegerVariable::_checkSameDomain_(const gum::Variable& aRV) const {
    // we can assume that aRV is a IntegerVariable
    const auto& cv = static_cast< const IntegerVariable& >(aRV);
    if (domainSize() != cv.domainSize()) return false;
    return cv._domain_ == _domain_;
  }

  INLINE void IntegerVariable::addValue(int value) {
    if (isValue(value)) {
      GUM_ERROR(DuplicateElement,
                "Value " << value << " already belongs to the domain of the variable");
    }
    _domain_.push_back(value);
    std::sort(_domain_.begin(), _domain_.end());
  }

  INLINE std::string IntegerVariable::closestLabel(double val) const {
    return label(closestIndex(val));
  }

  INLINE std::string IntegerVariable::toFast() const {
    std::stringstream s;
    s << name() << domain();
    return s.str();
  }
} /* namespace gum */

#endif /* DOXYGEN SHOULD SKIP THIS */
