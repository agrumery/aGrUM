/**
 *
 *   Copyright (c) 2005-2022 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
#include <ostream>
#include <sstream>
#include <string>
#include <algorithm>

#include <agrum/agrum.h>

#include <agrum/tools/core/hashTable.h>
#include <agrum/tools/variables/discreteVariable.h>

// to ease IDE parsers
#include <agrum/tools/variables/integerVariable.h>

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
    GUM_CONSTRUCTOR(NumericalDiscreteVariable);
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

  /// equality operator
  INLINE bool NumericalDiscreteVariable::operator==(const NumericalDiscreteVariable& var) const {
    return Variable::operator==(var) && (var._domain_ == _domain_);
  }

  /// inequality operator
  INLINE bool NumericalDiscreteVariable::operator!=(const Variable& var) const {
    return !operator==(var);
  }
  INLINE bool NumericalDiscreteVariable::operator!=(const NumericalDiscreteVariable& var) const {
    return !operator==(var);
  }


  /// returns the domain size of the discrete random variable
  INLINE Size NumericalDiscreteVariable::domainSize() const { return _domain_.size(); }


  /// returns the type of variable
  INLINE VarType NumericalDiscreteVariable::varType() const { return VarType::Numerical; }


  /// returns the index of a given label
  INLINE Idx NumericalDiscreteVariable::index(const std::string& aLabel) const {
    try {
      return _domain_.pos(std::stod(aLabel));
    } catch (...) {
      GUM_ERROR(NotFound, "label '" << aLabel << "' is unknown in " << this->toString());
    }
  }


  /// returns the ith label
  INLINE std::string NumericalDiscreteVariable::label(Idx i) const {
    // note that if i is outside the domain, Sequence _domain_ will raise
    // an exception
    return compact_tostr(_domain_[i]);
  }


  /// get a numerical representation of the indice-th value.
  INLINE double NumericalDiscreteVariable::numerical(Idx i) const { return double(_domain_[i]); }


  /// returns the domain as a sequence of values
  INLINE const Sequence< double >& NumericalDiscreteVariable::numericalDomain() const {
    return _domain_;
  }

  INLINE bool NumericalDiscreteVariable::isValue(double value) const {
    return _domain_.exists(value);
  }
  /// substitute a value by another one
  INLINE void NumericalDiscreteVariable::changeValue(double old_value, double new_value) {
    if (!_domain_.exists(old_value)) return;
    if (_domain_.exists(new_value)) {
      GUM_ERROR(DuplicateElement,
                "Value" << new_value << " already belongs to the domain of the variable");
    }

    eraseValue(old_value);
    addValue(new_value);
  }


  /// erase a value from the domain of the variable
  INLINE void NumericalDiscreteVariable::eraseValue(double value) { _domain_.erase(value); }


  /// clear the domain of the variable
  INLINE void NumericalDiscreteVariable::eraseValues() { _domain_.clear(); }


} /* namespace gum */

#endif /* DOXYGEN SHOULD SKIP THIS */
