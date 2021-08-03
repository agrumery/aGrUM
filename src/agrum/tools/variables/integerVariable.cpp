/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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


#include <agrum/tools/variables/integerVariable.h>

#ifdef GUM_NO_INLINE
#  include <agrum/tools/variables/integerVariable_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum {

  /// constructor assigning a domain to the variable
  IntegerVariable::IntegerVariable(const std::string&        aName,
                                   const std::string&        aDesc,
                                   const std::vector< int >& domain) :
      DiscreteVariable(aName, aDesc) {
    // get the values in increasing order
    std::vector< int > dom = domain;
    std::sort(dom.begin(), dom.end());

    // store the sorted values into a sequence
    _domain_.resize(dom.size());
    for (const int val: dom) {
      _domain_ << val;
    }

    // for debugging purposes
    GUM_CONSTRUCTOR(IntegerVariable);
  }


  /// equality operator
  bool IntegerVariable::operator==(const Variable& var) const {
    try {
      const IntegerVariable& xvar = dynamic_cast< const IntegerVariable& >(var);
      return Variable::      operator==(var) && (xvar._domain_ == _domain_);
    } catch (std::bad_cast&) { return false; }
  }


  /// Returns the domain as a string
  const std::string IntegerVariable::domain() const {
    std::stringstream s;
    s << "<";

    const Size size = domainSize();
    if (size > 0) {
      s << _domain_[0];

      for (Idx i = 1; i < size; ++i) {
        s << ',' << _domain_[i];
      }
    }

    s << ">";

    return s.str();
  }


  /// add a new value to the domain size
  IntegerVariable& IntegerVariable::addValue(int value) {
    const Size size = _domain_.size();
    if (size == Size(0) || (_domain_[size - 1] < value)) {
      _domain_.insert(value);
    } else {
      // here, the value must not be inserted at the end of the sequence.
      // it is faster to reconstruct the sequence from scratch
      std::vector< int > values;
      values.reserve(_domain_.size() + 1);
      for (const auto val: _domain_)
        values.push_back(val);
      values.push_back(value);
      std::sort(values.begin(), values.end());

      Sequence< int > new_domain(_domain_.size() + 1);
      for (const auto val: values)
        new_domain.insert(val);
      _domain_ = std::move(new_domain);
    }

    return *this;
  }

}   // namespace gum
