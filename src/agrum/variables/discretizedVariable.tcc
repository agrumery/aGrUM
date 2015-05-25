/*************************************************************************
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
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <agrum/variables/discretizedVariable.h>

namespace gum {
#define INC_TICKS_ARRAY 4

#define LESS_THAN_FIRST_RANGE "less than first range"
#define MORE_THAN_LAST_RANGE "more than last range"
#define NOT_ENOUGH_TICKS "not enough ticks"

  template <typename T_TICKS>
  INLINE void
  DiscretizedVariable<T_TICKS>::_copy(const DiscretizedVariable<T_TICKS> &aDRV) {
    eraseTicks();
    DiscreteVariable::_copy(aDRV);

    for (Idx i = 0; i <= aDRV.domainSize(); ++i) {
      addTick((T_TICKS)aDRV.__ticks[i]);
    }
  }

  template <typename T_TICKS>
  Idx DiscretizedVariable<T_TICKS>::_dichotomy(const T_TICKS &target, Idx min,
                                               Idx max) const {
    Idx res;
    Idx mid = 0;

    if (max - min < 2)
      res = min;
    else {
      mid = (max + min) / 2;
      T_TICKS val = __ticks[mid];

      if (target == val)
        res = mid;
      else if (target < val)
        res = _dichotomy(target, min, mid);
      else if (target > val)
        res = _dichotomy(target, mid, max);
      else
        res = mid;
    }

    return res;
  }

  template <typename T_TICKS>
  INLINE Idx DiscretizedVariable<T_TICKS>::_pos(const T_TICKS &target) const {
    if (__ticks_size < 2) {
      GUM_ERROR(OutOfBounds, NOT_ENOUGH_TICKS);
    }

    if (target < __ticks[0]) {
      GUM_ERROR(OutOfLowerBound, LESS_THAN_FIRST_RANGE);
    }

    if (target > __ticks[__ticks_size - 1]) {
      GUM_ERROR(OutOfUpperBound, MORE_THAN_LAST_RANGE);
    }

    if (target == __ticks[__ticks_size - 1]) // special case for upper limit
      // (which belongs to class __ticks_size-2
      return __ticks_size - 2;

    return _dichotomy(target, 0, __ticks_size - 1);
  }

  template <typename T_TICKS>
  INLINE DiscretizedVariable<T_TICKS>::DiscretizedVariable(const std::string &aName,
                                                           const std::string &aDesc)
      : DiscreteVariable(aName, aDesc), __ticks(INC_TICKS_ARRAY),
        __ticks_size((Size)0) {
    GUM_CONSTRUCTOR(DiscretizedVariable);
  }

  template <typename T_TICKS>
  DiscretizedVariable<T_TICKS>::DiscretizedVariable(
      const DiscretizedVariable<T_TICKS> &aDRV)
      : DiscreteVariable(aDRV), __ticks(INC_TICKS_ARRAY) {
    GUM_CONS_CPY(DiscretizedVariable);
    _copy(aDRV);
  }

  template <typename T_TICKS> DiscretizedVariable<T_TICKS>::~DiscretizedVariable() {
    GUM_DESTRUCTOR(DiscretizedVariable);
  }

  template <typename T_TICKS>
  DiscreteVariable *DiscretizedVariable<T_TICKS>::clone() const {
    DiscretizedVariable<T_TICKS> *varPtr = new DiscretizedVariable<T_TICKS>(*this);

    return (DiscreteVariable *)varPtr;
  }

  template <typename T_TICKS>
  INLINE const DiscretizedVariable<T_TICKS> &DiscretizedVariable<T_TICKS>::
  operator=(DiscretizedVariable<T_TICKS> &aDRV) {
    _copy(aDRV);
    return *this;
  }

  template <typename T_TICKS>
  INLINE bool DiscretizedVariable<T_TICKS>::isTick(const T_TICKS &aTick) const {
    if (__ticks_size == 0)
      return false;

    if (__ticks_size == 1)
      return (__ticks[0] == aTick);

    try {
      Idx zeIdx = _pos(aTick);

      if (zeIdx != __ticks_size - 2)
        return (__ticks[zeIdx] == aTick);
      else // special case for upper limit
        return ((__ticks[zeIdx] == aTick) || (__ticks[zeIdx + 1] == aTick));
    } catch (OutOfBounds &) {
      return false;
    }
  }

  template <typename T_TICKS>
  DiscretizedVariable<T_TICKS> &
  DiscretizedVariable<T_TICKS>::addTick(const T_TICKS &aTick) {
    if (isTick(aTick)) {
      GUM_ERROR(DefaultInLabel, "Tick already used for this variable ");
    }

    if (__ticks_size == __ticks.size()) { // streching __ticks if necessary
      __ticks.resize(__ticks_size + INC_MARKS_ARRAY);
    }

    if (__ticks_size == 0) { // special case for first tick
      __ticks[0] = aTick;
    } else if (__ticks_size == 1) { // special case for second tick
      if (__ticks[0] < aTick) {
        __ticks[1] = aTick;
      } else {
        __ticks[1] = __ticks[0];
        __ticks[0] = aTick;
      }
    } else {
      try {
        Idx zeIdx = _pos(aTick); // aTick is in [ __ticks[zeIdx],__ticks[zeIdx+1] [

        for (Idx i = __ticks_size - 1; i > zeIdx; --i) {
          __ticks[i + 1] = __ticks[i];
        }

        __ticks[zeIdx + 1] = aTick;
      } catch (OutOfUpperBound &) { // new upper bound
        __ticks[__ticks_size] = aTick;
      } catch (OutOfLowerBound &) { // new lower bound
        for (Idx i = __ticks_size; i >= 1; --i) {
          __ticks[i] = __ticks[i - 1];
        }

        __ticks[0] = aTick;
      }
    }

    __ticks_size++;

    return *this;
  }

  template <typename T_TICKS>
  INLINE void DiscretizedVariable<T_TICKS>::eraseTicks(void) {
    if (__ticks_size != 0) {
      __ticks_size = 0;
    }
  }

  template <typename T_TICKS>
  INLINE const std::string DiscretizedVariable<T_TICKS>::label(Idx i) const {
    std::stringstream ss;

    if (i >= __ticks_size - 1) {
      GUM_ERROR(OutOfBounds, "inexisting label index");
    }

    ss << "[" << __ticks[i] << ";" << __ticks[i + 1];

    ss << ((i == __ticks_size - 2) ? "]" : "[");

    return ss.str();
  }

  /// get a numerical representation of he indice-the value.
  template <typename T_TICKS>
  INLINE double DiscretizedVariable<T_TICKS>::numerical(Idx indice) const {
    if (indice >= __ticks_size - 1) {
      GUM_ERROR(OutOfBounds, "inexisting label index");
    }

    return double((__ticks[indice + 1] + __ticks[indice]) / 2);
  }

  template <typename T_TICKS>
  INLINE Idx DiscretizedVariable<T_TICKS>::operator[](const T_TICKS &aTarget) const {
    return index(aTarget);
  }
  template <typename T_TICKS>
  INLINE Idx DiscretizedVariable<T_TICKS>::index(const T_TICKS &aTarget) const {
    if (empty()) {
      GUM_ERROR(OutOfBounds, "empty variable : " + toString());
    }

    Idx zeTick = _pos(aTarget);

    return zeTick;
  }

  template <typename T_TICKS>
  INLINE Idx DiscretizedVariable<T_TICKS>::
  operator[](const std::string &aLabel) const {
    return index(aLabel);
  }
  template <typename T_TICKS>
  INLINE Idx DiscretizedVariable<T_TICKS>::index(const std::string &aLabel) const {
    for (Idx i = 0; i < domainSize(); ++i) {
      if (aLabel.compare(label(i)) == 0)
        return i;
    }

    GUM_ERROR(NotFound, "label " + aLabel + " not found in " + toString());
  }

  /**
   *
   * @return the size of the random discrete variable domain
   */
  template <typename T_TICKS>
  INLINE Size DiscretizedVariable<T_TICKS>::domainSize() const {
    return __ticks_size < 2 ? 0 : __ticks_size - 1;
  }

  template <typename T_TICKS>
  INLINE DiscreteVariable::VarType
  DiscretizedVariable<T_TICKS>::varType(void) const {
    return VarType::Discretized;
  }

  template <typename T_TICKS>
  INLINE const T_TICKS &DiscretizedVariable<T_TICKS>::tick(Idx i) const {
    if (i >= __ticks_size) {
      GUM_ERROR(OutOfBounds, "There is no such tick");
    }

    return __ticks[i];
  }
} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
