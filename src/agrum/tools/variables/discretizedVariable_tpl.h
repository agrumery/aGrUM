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
#  include <sstream>

#  include <agrum/tools/variables/discretizedVariable.h>

namespace gum {
  template < typename T_TICKS >
  INLINE void DiscretizedVariable< T_TICKS >::copy_(const DiscretizedVariable< T_TICKS >& aDRV) {
    eraseTicks();
    IDiscretizedVariable::copy_(aDRV);

    for (Idx i = 0; i < aDRV._ticks_size_; ++i) {
      addTick((T_TICKS)aDRV._ticks_[i]);
    }
  }

  template < typename T_TICKS >
  Idx DiscretizedVariable< T_TICKS >::dichotomy_(const T_TICKS& target, Idx min, Idx max) const {
    if (max - min < 2) return min;
    else {
      const Idx      mid = std::midpoint(min, max);
      const T_TICKS& val = _ticks_[mid];

      if (target < val) return dichotomy_(target, min, mid);
      if (target > val) return dichotomy_(target, mid, max);
      return mid;
    }
  }

  template < typename T_TICKS >
  INLINE Idx DiscretizedVariable< T_TICKS >::pos_(const T_TICKS& target) const {
    if (_ticks_size_ < 2) { GUM_ERROR(OutOfBounds, "not enough ticks") }

    if (target < _ticks_[0]) {
      if (_is_empirical) return 0;
      else GUM_ERROR(OutOfBounds, "less than first range")
    }

    if (target > _ticks_[_ticks_size_ - 1]) {
      if (_is_empirical) return _ticks_size_ - 2;
      else GUM_ERROR(OutOfBounds, "more than last range")
    }

    if (target == _ticks_[_ticks_size_ - 1])   // special case for upper limit
      // (which belongs to class  _ticks_size_-2
      return _ticks_size_ - 2;

    return dichotomy_(target, 0, _ticks_size_ - 1);
  }

  template < typename T_TICKS >
  INLINE DiscretizedVariable< T_TICKS >::DiscretizedVariable(const std::string& aName,
                                                             const std::string& aDesc) :
      IDiscretizedVariable(aName, aDesc),
      _ticks_size_((Size)0) {
    GUM_CONSTRUCTOR(DiscretizedVariable);
    _is_empirical = false;
    _ticks_.reserve(1);
  }

  template < typename T_TICKS >
  INLINE DiscretizedVariable< T_TICKS >::DiscretizedVariable(const std::string&            aName,
                                                             const std::string&            aDesc,
                                                             const std::vector< T_TICKS >& ticks,
                                                             bool is_empirical) :
      IDiscretizedVariable(aName, aDesc),
      _ticks_size_((Size)0) {
    GUM_CONSTRUCTOR(DiscretizedVariable);
    _is_empirical = is_empirical;
    _ticks_.reserve(ticks.size());
    for (const auto t: ticks)
      addTick(t);
  }

  template < typename T_TICKS >
  DiscretizedVariable< T_TICKS >::DiscretizedVariable(const DiscretizedVariable< T_TICKS >& aDRV) :
      IDiscretizedVariable(aDRV) {
    GUM_CONS_CPY(DiscretizedVariable);
    _is_empirical = aDRV._is_empirical;
    _ticks_.reserve(1);
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
  INLINE DiscretizedVariable< T_TICKS >&
     DiscretizedVariable< T_TICKS >::operator=(const DiscretizedVariable< T_TICKS >& aDRV) {
    copy_(aDRV);
    return *this;
  }

  template < typename T_TICKS >
  INLINE bool DiscretizedVariable< T_TICKS >::isTick(const T_TICKS& aTick) const {
    if (_ticks_size_ == 0) return false;

    if (_ticks_size_ == 1) return (_ticks_[0] == aTick);

    try {
      Idx zeIdx = pos_(aTick);

      if (zeIdx != _ticks_size_ - 2) return (_ticks_[zeIdx] == aTick);
      else   // special case for upper limit
        return ((_ticks_[zeIdx] == aTick) || (_ticks_[zeIdx + 1] == aTick));
    } catch (OutOfBounds const&) { return false; }
  }

  template < typename T_TICKS >
  DiscretizedVariable< T_TICKS >& DiscretizedVariable< T_TICKS >::addTick(const T_TICKS& aTick) {
    if (isTick(aTick)) {
      GUM_ERROR(DefaultInLabel, "Tick '" << aTick << "' already used for variable " << name())
    }

    if (_ticks_size_ == _ticks_.size()) {   // streching  _ticks_ if necessary
      _ticks_.resize(_ticks_size_ + 1);
    }

    if (_ticks_size_ == 0) {   // special case for first tick
      _ticks_[0] = aTick;
    } else if (_ticks_size_ == 1) {   // special case for second tick
      if (_ticks_[0] < aTick) {
        _ticks_[1] = aTick;
      } else {
        _ticks_[1] = _ticks_[0];
        _ticks_[0] = aTick;
      }
    } else {
      if (aTick > _ticks_[_ticks_size_ - 1])   // new upper bound
        _ticks_[_ticks_size_] = aTick;
      else if (aTick < _ticks_[0]) {   // new lower bound
        for (Idx i = _ticks_size_; i >= 1; --i) {
          _ticks_[i] = _ticks_[i - 1];
        }

        _ticks_[0] = aTick;
      } else {
        Idx zeIdx = pos_(aTick);   // aTick is in [  _ticks_[zeIdx], __ticks[zeIdx+1] [

        for (Idx i = _ticks_size_ - 1; i > zeIdx; --i) {
          _ticks_[i + 1] = _ticks_[i];
        }

        _ticks_[zeIdx + 1] = aTick;
      }
    }

    _ticks_size_++;

    return *this;
  }

  template < typename T_TICKS >
  INLINE void DiscretizedVariable< T_TICKS >::eraseTicks() {
    if (_ticks_size_ != 0) { _ticks_size_ = 0; }
  }

  template < typename T_TICKS >
  INLINE std::string DiscretizedVariable< T_TICKS >::label(Idx i) const {
    std::stringstream ss;

    if (i >= _ticks_size_ - 1) { GUM_ERROR(OutOfBounds, "inexisting label index") }

    if ((i == 0) && (_is_empirical)) ss << "(";
    else ss << "[";

    ss << _ticks_[i] << ";" << _ticks_[i + 1];

    if (i == _ticks_size_ - 2)
      if (_is_empirical) ss << ")";
      else ss << "]";
    else ss << "[";

    return ss.str();
  }

  /// get a numerical representation of he indice-the value.
  template < typename T_TICKS >
  INLINE double DiscretizedVariable< T_TICKS >::numerical(Idx indice) const {
    if (indice >= _ticks_size_ - 1) { GUM_ERROR(OutOfBounds, "inexisting label index") }

    return double((_ticks_[indice + 1] + _ticks_[indice]) / 2);
  }

  template < typename T_TICKS >
  INLINE Idx DiscretizedVariable< T_TICKS >::index(const std::string& label) const {
    if (empty()) { GUM_ERROR(OutOfBounds, "empty variable : " + toString()) }

    std::istringstream i(label);
    T_TICKS            target;

    if (!(i >> target)) { GUM_ERROR(NotFound, "Bad label : " << label << " for " << *this) }

    return pos_(target);
  }

  /**
   *
   * @return the size of the random discrete variable domain
   */
  template < typename T_TICKS >
  INLINE Size DiscretizedVariable< T_TICKS >::domainSize() const {
    return (_ticks_size_ < 2) ? Size(0) : Size(_ticks_size_ - 1);
  }

  template < typename T_TICKS >
  INLINE VarType DiscretizedVariable< T_TICKS >::varType() const {
    return VarType::Discretized;
  }

  template < typename T_TICKS >
  INLINE const T_TICKS& DiscretizedVariable< T_TICKS >::tick(Idx i) const {
    if (i >= _ticks_size_) { GUM_ERROR(OutOfBounds, "There is no such tick") }

    return _ticks_[i];
  }

  template < typename T_TICKS >
  std::string DiscretizedVariable< T_TICKS >::domain() const {
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
  INLINE const std::vector< T_TICKS >& DiscretizedVariable< T_TICKS >::ticks() const {
    return this->_ticks_;
  }

  template < typename T_TICKS >
  INLINE std::vector< double > DiscretizedVariable< T_TICKS >::ticksAsDoubles() const {
    const std::size_t     size = _ticks_.size();
    std::vector< double > ticks(size);
    for (auto i = std::size_t(0); i < size; ++i)
      ticks[i] = (double)_ticks_[i];
    return ticks;
  }

  template < typename T_TICKS >
  std::string DiscretizedVariable< T_TICKS >::toFast() const {
    std::stringstream s;
    bool              first = true;
    s << name() << "[";
    for (const auto& t: _ticks_) {
      if (!first) s << ",";
      else first = false;
      s << t;
    }
    s << "]";
    return s.str();
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
