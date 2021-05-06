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


#ifndef DOXYGEN_SHOULD_SKIP_THIS
#  include <sstream>

#  include <agrum/tools/variables/discretizedVariable.h>

namespace gum {
  template < typename T_TICKS >
  INLINE void DiscretizedVariable< T_TICKS >::copy_(
     const DiscretizedVariable< T_TICKS >& aDRV) {
    eraseTicks();
    IDiscretizedVariable::copy_(aDRV);

    for (Idx i = 0; i < aDRV.ticks_size__; ++i) {
      addTick((T_TICKS)aDRV.ticks__[i]);
    }
  }

  template < typename T_TICKS >
  Idx DiscretizedVariable< T_TICKS >::dichotomy_(const T_TICKS& target,
                                                 Idx            min,
                                                 Idx            max) const {
    Idx res;
    Idx mid = 0;

    if (max - min < 2)
      res = min;
    else {
      mid         = (max + min) / 2;
      T_TICKS val = ticks__[mid];

      if (target == val)
        res = mid;
      else if (target < val)
        res = dichotomy_(target, min, mid);
      else if (target > val)
        res = dichotomy_(target, mid, max);
      else
        res = mid;
    }

    return res;
  }

  template < typename T_TICKS >
  INLINE Idx DiscretizedVariable< T_TICKS >::pos_(const T_TICKS& target) const {
    if (ticks_size__ < 2) { GUM_ERROR(OutOfBounds, "not enough ticks"); }

    if (target < ticks__[0]) {
      GUM_ERROR(OutOfLowerBound, "less than first range")
    }

    if (target > ticks__[ticks_size__ - 1]) {
      GUM_ERROR(OutOfUpperBound, "more than last range")
    }

    if (target == ticks__[ticks_size__ - 1])   // special case for upper limit
      // (which belongs to class ticks_size__-2
      return ticks_size__ - 2;

    return dichotomy_(target, 0, ticks_size__ - 1);
  }

  template < typename T_TICKS >
  INLINE DiscretizedVariable< T_TICKS >::DiscretizedVariable(
     const std::string& aName,
     const std::string& aDesc) :
      IDiscretizedVariable(aName, aDesc),
      ticks_size__((Size)0) {
    GUM_CONSTRUCTOR(DiscretizedVariable);
    ticks__.reserve(1);
  }

  template < typename T_TICKS >
  INLINE DiscretizedVariable< T_TICKS >::DiscretizedVariable(
     const std::string&            aName,
     const std::string&            aDesc,
     const std::vector< T_TICKS >& ticks) :
      IDiscretizedVariable(aName, aDesc),
      ticks_size__((Size)0) {
    GUM_CONSTRUCTOR(DiscretizedVariable);
    ticks__.reserve(ticks.size());
    for (const auto t: ticks)
      addTick(t);
  }

  template < typename T_TICKS >
  DiscretizedVariable< T_TICKS >::DiscretizedVariable(
     const DiscretizedVariable< T_TICKS >& aDRV) :
      IDiscretizedVariable(aDRV) {
    GUM_CONS_CPY(DiscretizedVariable);
    ticks__.reserve(1);
    copy_(aDRV);
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
  INLINE DiscretizedVariable< T_TICKS >& DiscretizedVariable< T_TICKS >::operator=(
     const DiscretizedVariable< T_TICKS >& aDRV) {
    copy_(aDRV);
    return *this;
  }

  template < typename T_TICKS >
  INLINE bool DiscretizedVariable< T_TICKS >::isTick(const T_TICKS& aTick) const {
    if (ticks_size__ == 0) return false;

    if (ticks_size__ == 1) return (ticks__[0] == aTick);

    try {
      Idx zeIdx = pos_(aTick);

      if (zeIdx != ticks_size__ - 2)
        return (ticks__[zeIdx] == aTick);
      else   // special case for upper limit
        return ((ticks__[zeIdx] == aTick) || (ticks__[zeIdx + 1] == aTick));
    } catch (OutOfBounds&) { return false; }
  }

  template < typename T_TICKS >
  DiscretizedVariable< T_TICKS >&
     DiscretizedVariable< T_TICKS >::addTick(const T_TICKS& aTick) {
    if (isTick(aTick)) {
      GUM_ERROR(DefaultInLabel,
                "Tick '" << aTick << "' already used for variable " << name());
    }

    if (ticks_size__ == ticks__.size()) {   // streching ticks__ if necessary
      ticks__.resize(ticks_size__ + 1);
    }

    if (ticks_size__ == 0) {   // special case for first tick
      ticks__[0] = aTick;
    } else if (ticks_size__ == 1) {   // special case for second tick
      if (ticks__[0] < aTick) {
        ticks__[1] = aTick;
      } else {
        ticks__[1] = ticks__[0];
        ticks__[0] = aTick;
      }
    } else {
      try {
        Idx zeIdx
           = pos_(aTick);   // aTick is in [ ticks__[zeIdx],__ticks[zeIdx+1] [

        for (Idx i = ticks_size__ - 1; i > zeIdx; --i) {
          ticks__[i + 1] = ticks__[i];
        }

        ticks__[zeIdx + 1] = aTick;
      } catch (OutOfUpperBound&) {   // new upper bound
        ticks__[ticks_size__] = aTick;
      } catch (OutOfLowerBound&) {   // new lower bound
        for (Idx i = ticks_size__; i >= 1; --i) {
          ticks__[i] = ticks__[i - 1];
        }

        ticks__[0] = aTick;
      }
    }

    ticks_size__++;

    return *this;
  }

  template < typename T_TICKS >
  INLINE void DiscretizedVariable< T_TICKS >::eraseTicks() {
    if (ticks_size__ != 0) { ticks_size__ = 0; }
  }

  template < typename T_TICKS >
  INLINE std::string DiscretizedVariable< T_TICKS >::label(Idx i) const {
    std::stringstream ss;

    if (i >= ticks_size__ - 1) {
      GUM_ERROR(OutOfBounds, "inexisting label index")
    }

    ss << "[" << ticks__[i] << ";" << ticks__[i + 1];

    ss << ((i == ticks_size__ - 2) ? "]" : "[");

    return ss.str();
  }

  /// get a numerical representation of he indice-the value.
  template < typename T_TICKS >
  INLINE double DiscretizedVariable< T_TICKS >::numerical(Idx indice) const {
    if (indice >= ticks_size__ - 1) {
      GUM_ERROR(OutOfBounds, "inexisting label index")
    }

    return double((ticks__[indice + 1] + ticks__[indice]) / 2);
  }

  template < typename T_TICKS >
  INLINE Idx
     DiscretizedVariable< T_TICKS >::index(const std::string& label) const {
    if (empty()) { GUM_ERROR(OutOfBounds, "empty variable : " + toString()); }

    std::istringstream i(label);
    T_TICKS            target;

    if (!(i >> target)) {
      GUM_ERROR(NotFound, "Bad label : " << label << " for " << *this)
    }

    return pos_(target);
  }

  /**
   *
   * @return the size of the random discrete variable domain
   */
  template < typename T_TICKS >
  INLINE Size DiscretizedVariable< T_TICKS >::domainSize() const {
    return (ticks_size__ < 2) ? Size(0) : Size(ticks_size__ - 1);
  }

  template < typename T_TICKS >
  INLINE VarType DiscretizedVariable< T_TICKS >::varType() const {
    return VarType::Discretized;
  }

  template < typename T_TICKS >
  INLINE const T_TICKS& DiscretizedVariable< T_TICKS >::tick(Idx i) const {
    if (i >= ticks_size__) { GUM_ERROR(OutOfBounds, "There is no such tick"); }

    return ticks__[i];
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
    return this->ticks__;
  }

  template < typename T_TICKS >
  INLINE std::vector< double >
         DiscretizedVariable< T_TICKS >::ticksAsDoubles() const {
    const std::size_t     size = ticks__.size();
    std::vector< double > ticks(size);
    for (std::size_t i = std::size_t(0); i < size; ++i)
      ticks[i] = (double)ticks__[i];
    return ticks;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
