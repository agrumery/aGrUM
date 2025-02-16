/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


#ifndef DOXYGEN_SHOULD_SKIP_THIS
#  include <limits>
#  include <sstream>

#  include <agrum/base/variables/discretizedVariable.h>

#  include <agrum/base/core/math/math_utils.h>
#  include <agrum/base/core/utils_random.h>

namespace gum {
  template < typename T_TICKS >
  INLINE void DiscretizedVariable< T_TICKS >::copy_(const DiscretizedVariable< T_TICKS >& aDRV) {
    eraseTicks();
    IDiscretizedVariable::copy_(aDRV);
    _is_empirical = aDRV._is_empirical;
    _ticks_       = aDRV._ticks_;
  }

  template < typename T_TICKS >
  INLINE Idx DiscretizedVariable< T_TICKS >::pos_(const T_TICKS& target) const {
    if (target < _ticks_[0]) return Idx(0);
    if (target > _ticks_[_ticks_.size() - 1]) return Idx(_ticks_.size() - 2);
    // now target is in the range [T1,Tn]
    const Idx res = std::lower_bound(_ticks_.begin(), _ticks_.end(), target) - _ticks_.begin();
    if (res + 1 >= _ticks_.size()) return Idx(_ticks_.size() - 2);
    if (_ticks_[res] == target) return res;
    // res>0 because target>=_ticks_[0]
    return res - 1;
  }

  template < typename T_TICKS >
  INLINE Idx DiscretizedVariable< T_TICKS >::index(const T_TICKS target) const {
    const Idx ind = std::lower_bound(_ticks_.begin(), _ticks_.end(), target) - _ticks_.begin();
    if (ind + 1 >= _ticks_.size()) {
      GUM_ERROR(OutOfBounds, target << " is not a tick in " << *this)
    }
    if (_ticks_[ind] == target) return ind;

    GUM_ERROR(OutOfBounds, target << " is not a tick in " << *this)
  }

  template < typename T_TICKS >
  INLINE bool DiscretizedVariable< T_TICKS >::isTick(const T_TICKS& target) const {
    const Size ind = std::lower_bound(_ticks_.begin(), _ticks_.end(), target) - _ticks_.begin();
    if (ind >= _ticks_.size()) { return false; }
    return (_ticks_[ind] == target);
  }

  template < typename T_TICKS >
  INLINE DiscretizedVariable< T_TICKS >::DiscretizedVariable(const std::string& aName,
                                                             const std::string& aDesc) :
      IDiscretizedVariable(aName, aDesc) {
    GUM_CONSTRUCTOR(DiscretizedVariable);
    _is_empirical = false;
    _ticks_.reserve(10);
  }

  template < typename T_TICKS >
  INLINE DiscretizedVariable< T_TICKS >::DiscretizedVariable(const std::string&            aName,
                                                             const std::string&            aDesc,
                                                             const std::vector< T_TICKS >& ticks,
                                                             bool is_empirical) :
      IDiscretizedVariable(aName, aDesc) {
    GUM_CONSTRUCTOR(DiscretizedVariable)
    _is_empirical = is_empirical;
    _ticks_.reserve(ticks.size());
    for (const auto tick: ticks) {
      if (!gum::isfinite< double >(tick)) {
        GUM_ERROR(DefaultInLabel, "Value '" << tick << "' is not allowed for variable " << aName)
      }
      if (!isTick(tick)) { _ticks_.push_back(tick); }
    }
    std::sort(_ticks_.begin(), _ticks_.end());
  }

  template < typename T_TICKS >
  DiscretizedVariable< T_TICKS >::DiscretizedVariable(const DiscretizedVariable< T_TICKS >& aDRV) :
      IDiscretizedVariable(aDRV) {
    GUM_CONS_CPY(DiscretizedVariable);
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
  DiscretizedVariable< T_TICKS >& DiscretizedVariable< T_TICKS >::addTick(const T_TICKS& aTick) {
    // check if aTick is a float or a special value (infinity or not a number)
    if (!gum::isfinite(aTick)) {
      GUM_ERROR(DefaultInLabel, "Tick '" << aTick << "' is not allowed for variable " << name())
    }
    if (isTick(aTick)) {
      GUM_ERROR(DefaultInLabel, "Tick '" << aTick << "' already used for variable " << name())
    }

    _ticks_.push_back(aTick);
    std::sort(_ticks_.begin(), _ticks_.end());

    return *this;
  }

  template < typename T_TICKS >
  INLINE void DiscretizedVariable< T_TICKS >::eraseTicks() {
    _ticks_.clear();
  }

  template < typename T_TICKS >
  INLINE std::string DiscretizedVariable< T_TICKS >::label(Idx i) const {
    std::stringstream ss;

    if (i >= _ticks_.size() - 1) { GUM_ERROR(OutOfBounds, "inexisting label index") }

    if ((i == 0) && (_is_empirical)) ss << "(";
    else ss << "[";

    ss << _ticks_[i] << ";" << _ticks_[i + 1];

    if (i == _ticks_.size() - 2)
      if (_is_empirical) ss << ")";
      else ss << "]";
    else ss << "[";

    return ss.str();
  }

  /**  get a numerical representation of he indice-the value.
   *
   * @param indice the index of the label we wish to return
   * @throw OutOfBound if indice is not compatible
   */
  template < typename T_TICKS >
  INLINE double DiscretizedVariable< T_TICKS >::numerical(Idx indice) const {
    if (indice >= _ticks_.size() - 1) {
      GUM_ERROR(OutOfBounds, "Inexisting label index (" << indice << ") for " << *this << ".")
    }
    const auto& a = double(_ticks_[indice]);
    const auto& b = double(_ticks_[indice + 1]);

    return double((b + a) / 2.0);
  }

  /**  get a numerical representation of he indice-the value.
   *
   * @param indice the index of the label we wish to return
   * @throw OutOfBound if indice is not compatible
   */
  template < typename T_TICKS >
  INLINE double DiscretizedVariable< T_TICKS >::draw(Idx indice) const {
    if (indice >= _ticks_.size() - 1) {
      GUM_ERROR(OutOfBounds, "Inexisting label index (" << indice << ") for " << *this << ".")
    }
    const auto& a = double(_ticks_[indice]);
    const auto& b = double(_ticks_[indice + 1]);

    auto p = gum::randomProba() * (b - a) + a;
    if (indice < _ticks_.size() - 2) {   // p can not be b. We iterate 3 times before returning the
                                         // median (should not be possible)
      if (p == b) p = gum::randomProba() * (b - a) + a;
      if (p == b) p = gum::randomProba() * (b - a) + a;
      if (p == b) p = (b - a) / 2;
    }

    return p;
  }

  template < typename T_TICKS >
  INLINE Idx DiscretizedVariable< T_TICKS >::index(const std::string& label) const {
    if (empty()) { GUM_ERROR(OutOfBounds, "empty variable : " + toString()) }

    // first check if label contains a numeric value
    std::istringstream i(label);
    T_TICKS            target;
    if (i >> target) {
      if (target < _ticks_[0]) {
        if (_ticks_[0] - target < 1e-10) return 0;
        if (_is_empirical) return 0;
        else
          GUM_ERROR(OutOfBounds,
                    "less than first range (< " << _ticks_[0] << ") for " << target << " in "
                                                << *this)
      }

      const auto size = _ticks_.size();
      if (target > _ticks_[size - 1]) {
        if (target - _ticks_[size - 1] < 1e-10) return size - 2;
        if (_is_empirical) return size - 2;
        else
          GUM_ERROR(OutOfBounds,
                    "more than last range (> " << _ticks_[size - 1] << ") for " << target << " in "
                                               << *this << ":" << target - _ticks_[size - 1])
      }

      return pos_(target);
    }

    // second check if label contains an interval '[t1;t2]'
    std::istringstream ii(label);
    T_TICKS            t2;
    char               c1, c2, c3;
    if (!(ii >> c1 >> target >> c2 >> t2 >> c3)) {
      GUM_ERROR(NotFound, "Bad label : " << label << " for " << *this)
    }

    // check if a char is in a string
    const std::string s1{"[]()"};
    const std::string s2{",;"};
    if ((s1.find(c1) == std::string::npos) || (s1.find(c3) == std::string::npos)
        || (s2.find(c2) == std::string::npos)) {
      GUM_ERROR(NotFound, "Bad syntax for interval : " << label << " for " << *this)
    }

    const Idx it1 = pos_(target);

    if ((it1 + 1 >= _ticks_.size()) || (t2 != _ticks_[it1 + 1])) {
      GUM_ERROR(NotFound, "Bad interval : " << label << " for " << *this)
    }

    return it1;
  }

  template < typename T_TICKS >
  INLINE bool DiscretizedVariable< T_TICKS >::_checkSameDomain_(const gum::Variable& aRV) const {
    // we can assume that aRV is a ContinuousVariable
    const auto& cv = static_cast< const DiscretizedVariable< T_TICKS >& >(aRV);
    if (domainSize() != cv.domainSize()) return false;
    return cv._ticks_ == _ticks_ && cv._is_empirical == _is_empirical;
  }

  template < typename T_TICKS >
  INLINE Idx DiscretizedVariable< T_TICKS >::closestIndex(double val) const {
    if (val <= _ticks_[0]) { return 0; }
    if (val >= _ticks_[_ticks_.size() - 1]) { return _ticks_.size() - 2; }
    return pos_((T_TICKS)val);
  }

  /**
   *
   * @return the size of the random discrete variable domain
   */
  template < typename T_TICKS >
  INLINE Size DiscretizedVariable< T_TICKS >::domainSize() const {
    return (_ticks_.size() < 2) ? Size(0) : Size(_ticks_.size() - 1);
  }

  template < typename T_TICKS >
  INLINE VarType DiscretizedVariable< T_TICKS >::varType() const {
    return VarType::DISCRETIZED;
  }

  template < typename T_TICKS >
  INLINE const T_TICKS& DiscretizedVariable< T_TICKS >::tick(Idx i) const {
    if (i >= _ticks_.size()) {
      GUM_ERROR(OutOfBounds, "There is no such tick " << i << " for " << *this << ".")
    }

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
    s << name();
    if (_is_empirical) s << "+";
    s << "[";
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
