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


#ifndef DOXYGEN_SHOULD_SKIP_THIS

// to ease parsers in IDE
#  include <agrum/tools/variables/discreteVariable.h>

namespace gum {
  /* =========================================================================*/
  /* =========================================================================*/
  /* ===                         GUM_DISCRETE_VARIABLE === */
  /* =========================================================================*/
  /* =========================================================================*/

  // Default constructor
  INLINE DiscreteVariable::DiscreteVariable(const std::string& aName, const std::string& aDesc) :
      Variable(aName, aDesc) {
    GUM_CONSTRUCTOR(DiscreteVariable);
  }

  // Copy constructor
  INLINE DiscreteVariable::DiscreteVariable(const DiscreteVariable& aDRV) : Variable(aDRV) {
    GUM_CONSTRUCTOR(DiscreteVariable);
  }

  // destructor
  INLINE DiscreteVariable::~DiscreteVariable() { GUM_DESTRUCTOR(DiscreteVariable); }

  // Copy operator
  INLINE DiscreteVariable& DiscreteVariable::operator=(const DiscreteVariable& aRV) {
    if (&aRV != this) { Variable::operator=(aRV); }

    return *this;
  }

  // equality operator
  INLINE bool DiscreteVariable::operator==(const DiscreteVariable& aRV) const {
    return (Variable::operator==(aRV) && (domainSize() == aRV.domainSize()));
  }

  // inequality operator
  INLINE bool DiscreteVariable::operator!=(const DiscreteVariable& aRV) const {
    return (!operator==(aRV));
  }

  // empty() <==> domainSize()<2
  INLINE bool DiscreteVariable::empty() const { return domainSize() < 2; }

  // vector of labels
  INLINE std::vector< std::string > DiscreteVariable::labels() const {
    std::vector< std::string > v;
    for (Idx i = 0; i < domainSize(); i++)
      v.push_back(label(i));
    return v;
  }
} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS
