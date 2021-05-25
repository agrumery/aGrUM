/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @author Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
       _minBound_(minVal),  _maxBound_(maxVal) {
    GUM_CONSTRUCTOR(RangeVariable);
  }

  RangeVariable::RangeVariable(const std::string& aName,
                               const std::string& aDesc) :
      DiscreteVariable(aName, aDesc),
       _minBound_(0),  _maxBound_(1) {
    GUM_CONSTRUCTOR(RangeVariable);
  }

  // ==========================================================================
  // Copy Constructor.
  // If aDRV haves any listener, it will not be copied.
  // @param aDRV the variable we copy
  // ==========================================================================
  RangeVariable::RangeVariable(const RangeVariable& aDRV) :
      DiscreteVariable(aDRV),  _minBound_(aDRV. _minBound_),
       _maxBound_(aDRV. _maxBound_) {
    GUM_CONS_CPY(RangeVariable);
  }

  // ==========================================================================
  // destructor
  // ==========================================================================
  RangeVariable::~RangeVariable() {
    GUM_DESTRUCTOR(RangeVariable);
    ;
  }

  const std::string RangeVariable::domain() const {
    std::stringstream s;
    s << "[" << minVal() << "," << maxVal() << "]";
    return s.str();
  }
} /* namespace gum */
