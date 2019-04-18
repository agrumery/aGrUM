
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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


/**
 * @file
 * @brief ContinuousVariable.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#include <sstream>
#include <utility>

#include <agrum/agrum.h>
#include <agrum/variables/continuousVariable.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {


  /// Default constructor
  template < typename GUM_SCALAR >
  INLINE
     ContinuousVariable< GUM_SCALAR >::ContinuousVariable(const std::string& aName,
                                                          const std::string& aDesc,
                                                          GUM_SCALAR lower_bound,
                                                          GUM_SCALAR upper_bound) :
      IContinuousVariable(aName, aDesc),
      __lower_bound(lower_bound), __upper_bound(upper_bound) {
    if (__lower_bound > __upper_bound) { std::swap(__upper_bound, __lower_bound); }
    GUM_CONSTRUCTOR(ContinuousVariable);
  }


  /// Copy Constructor.
  template < typename GUM_SCALAR >
  INLINE ContinuousVariable< GUM_SCALAR >::ContinuousVariable(
     const ContinuousVariable< GUM_SCALAR >& from) :
      IContinuousVariable(from),
      __lower_bound(from.__lower_bound), __upper_bound(from.__upper_bound) {
    GUM_CONS_CPY(ContinuousVariable);
  }


  /// generalized copy constructor
  template < typename GUM_SCALAR >
  template < typename TX_VAL >
  INLINE ContinuousVariable< GUM_SCALAR >::ContinuousVariable(
     const ContinuousVariable< TX_VAL >& from) :
      IContinuousVariable(from),
      __lower_bound(GUM_SCALAR(from.__lower_bound)),
      __upper_bound(GUM_SCALAR(from.__upper_bound)) {
    GUM_CONS_CPY(ContinuousVariable);
  }


  /// move constructor
  template < typename GUM_SCALAR >
  INLINE ContinuousVariable< GUM_SCALAR >::ContinuousVariable(
     ContinuousVariable< GUM_SCALAR >&& from) :
      IContinuousVariable(std::move(from)),
      __lower_bound(from.__lower_bound), __upper_bound(from.__upper_bound) {
    GUM_CONS_MOV(ContinuousVariable);
  }


  /// destructor
  template < typename GUM_SCALAR >
  INLINE ContinuousVariable< GUM_SCALAR >::~ContinuousVariable() {
    GUM_DESTRUCTOR(ContinuousVariable);
  }


  /// Copy Factory.
  template < typename GUM_SCALAR >
  INLINE ContinuousVariable< GUM_SCALAR >*
         ContinuousVariable< GUM_SCALAR >::clone() const {
    return new ContinuousVariable< GUM_SCALAR >(*this);
  }


  /// copy operator
  template < typename GUM_SCALAR >
  INLINE ContinuousVariable< GUM_SCALAR >& ContinuousVariable< GUM_SCALAR >::
                                           operator=(const ContinuousVariable< GUM_SCALAR >& from) {
    IContinuousVariable::operator=(from);
    __lower_bound = from.__lower_bound;
    __upper_bound = from.__upper_bound;
    return *this;
  }


  /// generalized copy operator
  template < typename GUM_SCALAR >
  template < typename TX_VAL >
  INLINE ContinuousVariable< GUM_SCALAR >& ContinuousVariable< GUM_SCALAR >::
                                           operator=(const ContinuousVariable< TX_VAL >& from) {
    IContinuousVariable::operator=(from);
    __lower_bound = GUM_SCALAR(from.__lower_bound);
    __upper_bound = GUM_SCALAR(from.__upper_bound);
    return *this;
  }


  /// move operator
  template < typename GUM_SCALAR >
  INLINE ContinuousVariable< GUM_SCALAR >& ContinuousVariable< GUM_SCALAR >::
                                           operator=(ContinuousVariable< GUM_SCALAR >&& from) {
    IContinuousVariable::operator=(std::move(from));
    __lower_bound = from.__lower_bound;
    __upper_bound = from.__upper_bound;
    return *this;
  }


  /// returns the GUM_SCALAR corresponding to a string (unspecialized version)
  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR ContinuousVariable< GUM_SCALAR >::
                    operator[](const std::string& str) const {
    std::istringstream stream(str);
    GUM_SCALAR         value;
    stream >> value;

    if (belongs(value))
      return value;
    else
      GUM_ERROR(OutOfBounds,
                "the value does not delong to the domain of the variable");
  }


  /// returns the lower bound of the domain of the variable
  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR ContinuousVariable< GUM_SCALAR >::lowerBound() const {
    return __lower_bound;
  }


  /// returns the lower bound of the domain of the variable as a double
  template < typename GUM_SCALAR >
  INLINE double ContinuousVariable< GUM_SCALAR >::lowerBoundAsDouble() const {
    return (double)__lower_bound;
  }


  /// returns the upper bound of the domain of the variable
  template < typename GUM_SCALAR >
  INLINE GUM_SCALAR ContinuousVariable< GUM_SCALAR >::upperBound() const {
    return __upper_bound;
  }


  /// returns the upper bound of the domain of the variable
  template < typename GUM_SCALAR >
  INLINE double ContinuousVariable< GUM_SCALAR >::upperBoundAsDouble() const {
    return (double)__upper_bound;
  }


  /// updates the lower bound of the domain of the variable
  template < typename GUM_SCALAR >
  INLINE void
     ContinuousVariable< GUM_SCALAR >::setLowerBound(const GUM_SCALAR& new_bound) {
    if (new_bound <= __upper_bound)
      __lower_bound = new_bound;
    else
      GUM_ERROR(OutOfBounds,
                "the new lower bound would be higher than the upper bound");
  }


  /// updates the lower bound of the domain of the variable
  template < typename GUM_SCALAR >
  INLINE void ContinuousVariable< GUM_SCALAR >::setLowerBoundFromDouble(
     const double new_bound) {
    setLowerBound((GUM_SCALAR)new_bound);
  }


  /// updates the lower bound of the domain of the variable
  template < typename GUM_SCALAR >
  INLINE void
     ContinuousVariable< GUM_SCALAR >::setUpperBound(const GUM_SCALAR& new_bound) {
    if (new_bound >= __lower_bound)
      __upper_bound = new_bound;
    else
      GUM_ERROR(OutOfBounds,
                "the new upper bound would be lower than the lower bound");
  }


  /// updates the lower bound of the domain of the variable
  template < typename GUM_SCALAR >
  INLINE void ContinuousVariable< GUM_SCALAR >::setUpperBoundFromDouble(
     const double new_bound) {
    setUpperBound((GUM_SCALAR)new_bound);
  }


  /// returns the type of the variable
  template < typename GUM_SCALAR >
  INLINE VarType ContinuousVariable< GUM_SCALAR >::varType() const {
    return VarType::Continuous;
  }


  /// returns a string containing the value of the variable passed in argument
  template < typename GUM_SCALAR >
  INLINE std::string
         ContinuousVariable< GUM_SCALAR >::label(const GUM_SCALAR& value) const {
    if (belongs(value)) return std::to_string(value);
    GUM_ERROR(OutOfBounds,
              "the value does not belong to the domain of the variable");
  }


  /// Returns true if the param belongs to the domain of the variable
  template < typename GUM_SCALAR >
  INLINE bool
     ContinuousVariable< GUM_SCALAR >::belongs(const GUM_SCALAR& value) const {
    return (value >= __lower_bound) && (value <= __upper_bound);
  }


  /// returns the domain of the variable as a string
  template < typename GUM_SCALAR >
  INLINE const std::string ContinuousVariable< GUM_SCALAR >::domain() const {
    std::ostringstream stream;
    stream << '[' << __lower_bound << ';' << __upper_bound << ']';
    return stream.str();
  }


  /// string version of *this
  template < typename GUM_SCALAR >
  INLINE const std::string ContinuousVariable< GUM_SCALAR >::toString() const {
    std::string str(this->name());
    str += domain();
    return str;
  }


  /// string version of *this using description attribute instead of name.
  template < typename GUM_SCALAR >
  INLINE const std::string
               ContinuousVariable< GUM_SCALAR >::toStringWithDescription() const {
    std::string str(this->description());
    str += domain();
    return str;
  }


  /// for friendly displaying the content of the variable
  template < typename GUM_SCALAR >
  std::ostream& operator<<(std::ostream&                           stream,
                           const ContinuousVariable< GUM_SCALAR >& var) {
    return stream << var.toString();
  }


} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
