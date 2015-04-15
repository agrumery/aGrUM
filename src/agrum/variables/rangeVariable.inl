/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/**
 * @file
 * @brief Inline implementation of gumRangeVariable.
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
 */

// to ease IDE parsing
#include <agrum/variables/rangeVariable.h>

namespace gum {

  // Copy Factory.
  // @return Returns a pointer on a new copy of this.
  INLINE DiscreteVariable *RangeVariable::clone() const {
    return new RangeVariable(*this);
  }

  // returns the size of the random discrete variable domain
  INLINE Size RangeVariable::domainSize() const {
    return (__maxBound <= __minBound) ? 0 : (__maxBound - __minBound + 1);
  }

  // Get the indice-th label. This method is pure virtual.
  // @param indice the index of the label we wish to return
  // @throw OutOfBound
  INLINE const std::string RangeVariable::label(Idx indice) const {
    if (belongs(indice + __minBound)) {
      std::stringstream strBuff;
      strBuff << indice + __minBound;
      return strBuff.str();
    } else {
      GUM_ERROR(OutOfBounds, "Indice out of bounds.");
    }
  }

  INLINE
  double RangeVariable::numerical(Idx indice) const {
    return double(__minBound + indice);
  }

  INLINE Idx RangeVariable::operator[](const std::string &aLabel) const {
    return index(aLabel);
  }

  INLINE Idx RangeVariable::index(const std::string &aLabel) const {
    std::istringstream i(aLabel);
    Idx res;

    if (!(i >> res)) {
      GUM_ERROR(NotFound, "Bad label : " << aLabel << " for " << *this);
    }

    if (!belongs(res)) {
      GUM_ERROR(NotFound, "Bad label : " << aLabel << " for " << *this);
    }

    return res - __minBound;
  }

  // Returns the lower bound.
  INLINE Idx RangeVariable::minVal() const { return __minBound; }

  // Set a new value for the lower bound.
  INLINE void RangeVariable::setMinVal(Idx minVal) { __minBound = minVal; }

  // Returns the upper bound.
  INLINE Idx RangeVariable::maxVal() const { return __maxBound; }

  // Set a new value of the upper bound.
  INLINE void RangeVariable::setMaxVal(Idx maxVal) { __maxBound = maxVal; }

  // Returns true if the param belongs to the variable's interval.
  INLINE bool RangeVariable::belongs(Idx indice) const {
    return ((__minBound <= indice) && (indice <= __maxBound));
  }

  // Copy operator
  // @param aRV to be copied
  // @return a ref to *this
  INLINE RangeVariable &RangeVariable::operator=(const RangeVariable &aRV) {
    __minBound = aRV.__minBound;
    __maxBound = aRV.__maxBound;
    return *this;
  }

  INLINE DiscreteVariable::VarType RangeVariable::varType(void) const {
    return VarType::Range;
  }

} /* namespace gum */
