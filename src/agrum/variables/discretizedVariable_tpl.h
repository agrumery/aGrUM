
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


#ifndef DOXYGEN_SHOULD_SKIP_THIS
#  include <sstream>

#  include <agrum/variables/discretizedVariable.h>

namespace gum {
  template < typename T_TICKS >
  INLINE void DiscretizedVariable< T_TICKS >::_copy(
     const DiscretizedVariable< T_TICKS >& aDRV) {
    eraseTicks();
    IDiscretizedVariable::_copy(aDRV);

    for (Idx i = 0; i < aDRV.__ticks_size; ++i) {
      addTick((T_TICKS)aDRV.__ticks[i]);
    }
  }

  template < typename T_TICKS >
  Idx DiscretizedVariable< T_TICKS >::_dichotomy(const T_TICKS& target,
                                                 Idx            min,
                                                 Idx            max) const {
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

  template < typename T_TICKS >
  INLINE Idx DiscretizedVariable< T_TICKS >::_pos(const T_TICKS& target) const {
    if (__ticks_size < 2) { GUM_ERROR(OutOfBounds, "not enough ticks"); }

    if (target < __ticks[0]) {
      GUM_ERROR(OutOfLowerBound, "less than first range");
    }

    if (target > __ticks[__ticks_size - 1]) {
      GUM_ERROR(OutOfUpperBound, "more than last range");
    }

    if (target == __ticks[__ticks_size - 1])   // special case for upper limit
      // (which belongs to class __ticks_size-2
      return __ticks_size - 2;

    return _dichotomy(target, 0, __ticks_size - 1);
  }

  template < typename T_TICKS >
  INLINE DiscretizedVariable< T_TICKS >::DiscretizedVariable(
     const std::string& aName, const std::string& aDesc) :
      IDiscretizedVariable(aName, aDesc),
      __ticks_size((Size)0) {
    GUM_CONSTRUCTOR(DiscretizedVariable);
    __ticks.reserve(1);
  }

  template < typename T_TICKS >
  INLINE DiscretizedVariable< T_TICKS >::DiscretizedVariable(
     const std::string&            aName,
     const std::string&            aDesc,
     const std::vector< T_TICKS >& ticks) :
      IDiscretizedVariable(aName, aDesc),
      __ticks_size((Size)0) {
    GUM_CONSTRUCTOR(DiscretizedVariable);
    __ticks.reserve(ticks.size());
    for (const auto t : ticks)
      addTick(t);
  }

  template < typename T_TICKS >
  DiscretizedVariable< T_TICKS >::DiscretizedVariable(
     const DiscretizedVariable< T_TICKS >& aDRV) :
      IDiscretizedVariable(aDRV) {
    GUM_CONS_CPY(DiscretizedVariable);
    __ticks.reserve(1);
    _copy(aDRV);
  }

  template < typename T_TICKS >
  DiscretizedVariable< T_TICKS >::~DiscretizedVariable() {
    GUM_DESTRUCTOR(DiscretizedVariable);
  }

  template < typename T_TICKS >
  DiscretizedVariable< T_TICKS >* DiscretizedVariable< T_TICKS >::clone() const {
    return new DiscretizedVariable< T_TICKS >(*this);
  }

  template < typename T_TICKS >
  INLINE DiscretizedVariable< T_TICKS >& DiscretizedVariable< T_TICKS >::
                                         operator=(const DiscretizedVariable< T_TICKS >& aDRV) {
    _copy(aDRV);
    return *this;
  }

  template < typename T_TICKS >
  INLINE bool DiscretizedVariable< T_TICKS >::isTick(const T_TICKS& aTick) const {
    if (__ticks_size == 0) return false;

    if (__ticks_size == 1) return (__ticks[0] == aTick);

    try {
      Idx zeIdx = _pos(aTick);

      if (zeIdx != __ticks_size - 2)
        return (__ticks[zeIdx] == aTick);
      else   // special case for upper limit
        return ((__ticks[zeIdx] == aTick) || (__ticks[zeIdx + 1] == aTick));
    } catch (OutOfBounds&) { return false; }
  }

  template < typename T_TICKS >
  DiscretizedVariable< T_TICKS >&
     DiscretizedVariable< T_TICKS >::addTick(const T_TICKS& aTick) {
    if (isTick(aTick)) {
      GUM_ERROR(DefaultInLabel,
                "Tick '" << aTick << "' already used for variable " << name());
    }

    if (__ticks_size == __ticks.size()) {   // streching __ticks if necessary
      __ticks.resize(__ticks_size + 1);
    }

    if (__ticks_size == 0) {   // special case for first tick
      __ticks[0] = aTick;
    } else if (__ticks_size == 1) {   // special case for second tick
      if (__ticks[0] < aTick) {
        __ticks[1] = aTick;
      } else {
        __ticks[1] = __ticks[0];
        __ticks[0] = aTick;
      }
    } else {
      try {
        Idx zeIdx =
           _pos(aTick);   // aTick is in [ __ticks[zeIdx],__ticks[zeIdx+1] [

        for (Idx i = __ticks_size - 1; i > zeIdx; --i) {
          __ticks[i + 1] = __ticks[i];
        }

        __ticks[zeIdx + 1] = aTick;
      } catch (OutOfUpperBound&) {   // new upper bound
        __ticks[__ticks_size] = aTick;
      } catch (OutOfLowerBound&) {   // new lower bound
        for (Idx i = __ticks_size; i >= 1; --i) {
          __ticks[i] = __ticks[i - 1];
        }

        __ticks[0] = aTick;
      }
    }

    __ticks_size++;

    return *this;
  }

  template < typename T_TICKS >
  INLINE void DiscretizedVariable< T_TICKS >::eraseTicks() {
    if (__ticks_size != 0) { __ticks_size = 0; }
  }

  template < typename T_TICKS >
  INLINE std::string DiscretizedVariable< T_TICKS >::label(Idx i) const {
    std::stringstream ss;

    if (i >= __ticks_size - 1) {
      GUM_ERROR(OutOfBounds, "inexisting label index");
    }

    ss << "[" << __ticks[i] << ";" << __ticks[i + 1];

    ss << ((i == __ticks_size - 2) ? "]" : "[");

    return ss.str();
  }

  /// get a numerical representation of he indice-the value.
  template < typename T_TICKS >
  INLINE double DiscretizedVariable< T_TICKS >::numerical(Idx indice) const {
    if (indice >= __ticks_size - 1) {
      GUM_ERROR(OutOfBounds, "inexisting label index");
    }

    return double((__ticks[indice + 1] + __ticks[indice]) / 2);
  }

  template < typename T_TICKS >
  INLINE Idx
         DiscretizedVariable< T_TICKS >::index(const std::string& label) const {
    if (empty()) { GUM_ERROR(OutOfBounds, "empty variable : " + toString()); }

    std::istringstream i(label);
    T_TICKS            target;

    if (!(i >> target)) {
      GUM_ERROR(NotFound, "Bad label : " << label << " for " << *this);
    }

    return _pos(target);
  }

  /**
   *
   * @return the size of the random discrete variable domain
   */
  template < typename T_TICKS >
  INLINE Size DiscretizedVariable< T_TICKS >::domainSize() const {
    return (__ticks_size < 2) ? Size(0) : Size(__ticks_size - 1);
  }

  template < typename T_TICKS >
  INLINE VarType DiscretizedVariable< T_TICKS >::varType() const {
    return VarType::Discretized;
  }

  template < typename T_TICKS >
  INLINE const T_TICKS& DiscretizedVariable< T_TICKS >::tick(Idx i) const {
    if (i >= __ticks_size) { GUM_ERROR(OutOfBounds, "There is no such tick"); }

    return __ticks[i];
  }

  template < typename T_TICKS >
  const std::string DiscretizedVariable< T_TICKS >::domain() const {
    std::stringstream s;
    s << "<";

    if (domainSize() > 0) {
      s << label(0);

      for (Idx i = 1; i < domainSize(); ++i) {
        s << ",";
        s << label(i);
      }
    }

    s << ">";

    return s.str();
  }

  template < typename T_TICKS >
  INLINE const std::vector< T_TICKS >&
               DiscretizedVariable< T_TICKS >::ticks() const {
    return this->__ticks;
  }

  template < typename T_TICKS >
  INLINE std::vector< double >
         DiscretizedVariable< T_TICKS >::ticksAsDoubles() const {
    const std::size_t     size = __ticks.size();
    std::vector< double > ticks(size);
    for (std::size_t i = std::size_t(0); i < size; ++i)
      ticks[i] = (double)__ticks[i];
    return ticks;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
