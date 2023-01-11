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


/**
 * @file
 * @brief Inline implementation of gumRangeVariable.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#include <sstream>

// to ease IDE parsing
#include <agrum/tools/variables/rangeVariable.h>

namespace gum {

  // Copy Factory.
  // @return Returns a pointer on a new copy of this.
  INLINE RangeVariable* RangeVariable::clone() const { return new RangeVariable(*this); }

  // returns the size of the random discrete variable domain
  INLINE Size RangeVariable::domainSize() const {
    return (_maxBound_ < _minBound_) ? Size(0) : Size(_maxBound_ + 1 - _minBound_);
  }

  // Get the indice-th label. This method is pure virtual.
  // @param indice the index of the label we wish to return
  // @throw OutOfBound
  INLINE std::string RangeVariable::label(Idx indice) const {
    long target = static_cast< long >(indice) + _minBound_;
    if (belongs(target)) {
      std::stringstream strBuff;
      strBuff << target;
      return strBuff.str();
    } else {
      GUM_ERROR(OutOfBounds, "Indice out of bounds.")
    }
  }

  INLINE
  double RangeVariable::numerical(Idx indice) const {
    return double(_minBound_ + static_cast< long >(indice));
  }


  INLINE Idx RangeVariable::index(const std::string& label) const {
    std::istringstream i(label);
    long               target;

    if (!(i >> target)) { GUM_ERROR(NotFound, "Bad label : " << label << " for " << *this) }

    if (!belongs(target)) { GUM_ERROR(NotFound, "Bad label : " << label << " for " << *this) }

    return static_cast< Idx >(target - _minBound_);
  }

  // Returns the lower bound.
  INLINE long RangeVariable::minVal() const { return _minBound_; }

  // Set a new value for the lower bound.
  INLINE void RangeVariable::setMinVal(long minVal) { _minBound_ = minVal; }

  // Returns the upper bound.
  INLINE long RangeVariable::maxVal() const { return _maxBound_; }

  // Set a new value of the upper bound.
  INLINE void RangeVariable::setMaxVal(long maxVal) { _maxBound_ = maxVal; }

  // Returns true if the param belongs to the variable's interval.
  INLINE bool RangeVariable::belongs(long val) const {
    return ((_minBound_ <= val) && (val <= _maxBound_));
  }

  // Copy operator
  // @param aRV to be copied
  // @return a ref to *this
  INLINE RangeVariable& RangeVariable::operator=(const RangeVariable& aRV) {
    _minBound_ = aRV._minBound_;
    _maxBound_ = aRV._maxBound_;
    return *this;
  }

  INLINE VarType RangeVariable::varType() const { return VarType::Range; }

} /* namespace gum */
