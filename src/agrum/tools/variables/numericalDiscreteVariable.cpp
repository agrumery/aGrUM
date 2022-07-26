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


#include <agrum/tools/variables/numericalDiscreteVariable.h>

#ifdef GUM_NO_INLINE
#  include <agrum/tools/variables/numericalDiscreteVariable_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum {

  /// constructor assigning a domain to the variable
  NumericalDiscreteVariable::NumericalDiscreteVariable(const std::string&           aName,
                                                       const std::string&           aDesc,
                                                       const std::vector< double >& domain) :
      DiscreteVariable(aName, aDesc) {
    // get the values in increasing order
    std::vector< double > dom = domain;
    std::sort(dom.begin(), dom.end());

    // store the sorted values into a sequence
    _domain_.resize(dom.size());
    for (const double val: dom) {
      _domain_ << val;
    }

    // for debugging purposes
    GUM_CONSTRUCTOR(NumericalDiscreteVariable);
  }

  /// constructor assigning a domain to the variable
  NumericalDiscreteVariable::NumericalDiscreteVariable(const std::string& aName,
                                                       const std::string& aDesc,
                                                       double             first,
                                                       double             last,
                                                       Size               nb) :
      DiscreteVariable(aName, aDesc) {
    // store the sorted values into a sequence
    if (nb < 2) GUM_ERROR(ArgumentError, "The size of the domain must be >2 (here :" << nb << ").")
    if (first >= last)
      GUM_ERROR(ArgumentError, "first (here :" << first << " must be <last (here :" << last << ").")

    _domain_.resize(nb);
    _domain_ << first;
    const double step = (last - first) / (double(nb) - 1);

    // now we try to find a significatif number of digits for the value (at least 4 otherwise 2+nbr
    // digit in step)
    const double mask = std::pow(10, std::max(4, int(2 + std::abs(-std::log10(step)))));

    double current = first;
    for (Idx i = 1; i < nb - 1; i++) {
      current += step;
      _domain_ << std::round(current * mask) / mask;
    }
    _domain_ << last;

    // for debugging purposes
    GUM_CONSTRUCTOR(NumericalDiscreteVariable);
  }


  /// equality operator
  bool NumericalDiscreteVariable::operator==(const Variable& var) const {
    try {
      const NumericalDiscreteVariable& xvar = dynamic_cast< const NumericalDiscreteVariable& >(var);
      return                           operator==(xvar);
    } catch (std::bad_cast&) { return false; }
  }


  /// Returns the domain as a string
  std::string NumericalDiscreteVariable::domain() const {
    std::stringstream s;
    const Size        size = domainSize();

    s << "{";
    if (size > 0) {
      s << _domain_[0];
      for (Idx i = 1; i < size; ++i)
        s << '|' << _domain_[i];
    }
    s << "}";
    return s.str();
  }


  /// add a new value to the domain size
  void NumericalDiscreteVariable::addValue(double value) {
    const Size size = _domain_.size();
    if (size == Size(0) || (_domain_[size - 1] < value)) {
      _domain_.insert(value);
    } else {
      // here, the value must not be inserted at the end of the sequence.
      // it is faster to reconstruct the sequence from scratch
      std::vector< double > values;
      values.reserve(_domain_.size() + 1);
      for (const auto val: _domain_)
        values.push_back(val);
      values.push_back(value);
      std::sort(values.begin(), values.end());

      Sequence< double > new_domain(_domain_.size() + 1);
      for (const auto val: values)
        new_domain.insert(val);
      _domain_ = std::move(new_domain);
    }
  }

}   // namespace gum
