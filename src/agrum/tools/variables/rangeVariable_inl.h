/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief Inline implementation of gumRangeVariable.
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 */
#include <sstream>

// to ease IDE parsing
#include <agrum/tools/variables/rangeVariable.h>

namespace gum {

  // Copy Factory.
  // @return Returns a pointer on a new copy of this.
  INLINE RangeVariable* RangeVariable::clone() const {
    return new RangeVariable(*this);
  }

  // returns the size of the random discrete variable domain
  INLINE Size RangeVariable::domainSize() const {
    return (maxBound__ < minBound__) ? Size(0) : Size(maxBound__ + 1 - minBound__);
  }

  // Get the indice-th label. This method is pure virtual.
  // @param indice the index of the label we wish to return
  // @throw OutOfBound
  INLINE std::string RangeVariable::label(Idx indice) const {
    long target = static_cast< long >(indice) + minBound__;
    if (belongs(target)) {
      std::stringstream strBuff;
      strBuff << target;
      return strBuff.str();
    } else {
      GUM_ERROR(OutOfBounds, "Indice out of bounds.");
    }
  }

  INLINE
  double RangeVariable::numerical(Idx indice) const {
    return double(minBound__ + static_cast< long >(indice));
  }


  INLINE Idx RangeVariable::index(const std::string& label) const {
    std::istringstream i(label);
    long               target;

    if (!(i >> target)) {
      GUM_ERROR(NotFound, "Bad label : " << label << " for " << *this);
    }

    if (!belongs(target)) {
      GUM_ERROR(NotFound, "Bad label : " << label << " for " << *this);
    }

    return static_cast< Idx >(target - minBound__);
  }

  // Returns the lower bound.
  INLINE long RangeVariable::minVal() const { return minBound__; }

  // Set a new value for the lower bound.
  INLINE void RangeVariable::setMinVal(long minVal) { minBound__ = minVal; }

  // Returns the upper bound.
  INLINE long RangeVariable::maxVal() const { return maxBound__; }

  // Set a new value of the upper bound.
  INLINE void RangeVariable::setMaxVal(long maxVal) { maxBound__ = maxVal; }

  // Returns true if the param belongs to the variable's interval.
  INLINE bool RangeVariable::belongs(long val) const {
    return ((minBound__ <= val) && (val <= maxBound__));
  }

  // Copy operator
  // @param aRV to be copied
  // @return a ref to *this
  INLINE RangeVariable& RangeVariable::operator=(const RangeVariable& aRV) {
    minBound__ = aRV.minBound__;
    maxBound__ = aRV.maxBound__;
    return *this;
  }

  INLINE VarType RangeVariable::varType() const { return VarType::Range; }

} /* namespace gum */
