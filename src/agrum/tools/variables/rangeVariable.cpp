
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
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


/**
 * @file
 * @brief Implementation of gumRangeVariable.
 *
 * @author Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
 */

#include <agrum/tools/variables/rangeVariable.h>

#ifdef GUM_NO_INLINE
#  include <agrum/tools/variables/rangeVariable_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum {

  // ==========================================================================
  // constructors
  // ==========================================================================
  RangeVariable::RangeVariable(const std::string& aName,
                               const std::string& aDesc,
                               long               minVal,
                               long               maxVal) :
      DiscreteVariable(aName, aDesc),
      __minBound(minVal), __maxBound(maxVal) {
    GUM_CONSTRUCTOR(RangeVariable);
  }

  RangeVariable::RangeVariable(const std::string& aName,
                               const std::string& aDesc) :
      DiscreteVariable(aName, aDesc),
      __minBound(0), __maxBound(1) {
    GUM_CONSTRUCTOR(RangeVariable);
  }

  // ==========================================================================
  // Copy Constructor.
  // If aDRV haves any listener, it will not be copied.
  // @param aDRV the variable we copy
  // ==========================================================================
  RangeVariable::RangeVariable(const RangeVariable& aDRV) :
      DiscreteVariable(aDRV), __minBound(aDRV.__minBound),
      __maxBound(aDRV.__maxBound) {
    GUM_CONS_CPY(RangeVariable);
  }

  // ==========================================================================
  // destructor
  // ==========================================================================
  RangeVariable::~RangeVariable() { GUM_DESTRUCTOR(RangeVariable); }

  const std::string RangeVariable::domain() const {
    std::stringstream s;
    s << "[" << minVal() << "," << maxVal() << "]";
    return s.str();
  }
} /* namespace gum */