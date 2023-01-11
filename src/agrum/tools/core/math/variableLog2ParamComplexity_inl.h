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


/** @file
 * @brief the class for computing the log2 of the parametric complexity of
 * an r-ary multinomial variable
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/tools/core/math/variableLog2ParamComplexity.h>
#  include <agrum/tools/core/math/gammaLog2.h>
#  include <iostream>
#  include <sstream>


namespace gum {

  /// default constructor
  INLINE VariableLog2ParamComplexity::VariableLog2ParamComplexity() : _cache_(1000) {
    GUM_CONSTRUCTOR(VariableLog2ParamComplexity);
  }


  /// copy constructor
  INLINE VariableLog2ParamComplexity::VariableLog2ParamComplexity(
     const VariableLog2ParamComplexity& from) :
      _use_cache_(from._use_cache_),
      _cache_(from._cache_) {
    GUM_CONS_CPY(VariableLog2ParamComplexity);
  }


  /// move constructor
  INLINE
  VariableLog2ParamComplexity::VariableLog2ParamComplexity(VariableLog2ParamComplexity&& from) :
      _use_cache_(from._use_cache_), _cache_(std::move(from._cache_)) {
    GUM_CONS_MOV(VariableLog2ParamComplexity);
  }


  /// virtual copy constructor
  INLINE VariableLog2ParamComplexity* VariableLog2ParamComplexity::clone() const {
    return new VariableLog2ParamComplexity(*this);
  }


  /// destructor
  INLINE VariableLog2ParamComplexity::~VariableLog2ParamComplexity() {
    GUM_DESTRUCTOR(VariableLog2ParamComplexity);
  }


  /// copy operator
  INLINE VariableLog2ParamComplexity&
     VariableLog2ParamComplexity::operator=(const VariableLog2ParamComplexity& from) {
    return *this;
  }


  /// move operator
  INLINE VariableLog2ParamComplexity&
     VariableLog2ParamComplexity::operator=(VariableLog2ParamComplexity&& from) {
    return *this;
  }


  /// indicates whether we wish to use a cache for the Cnr
  INLINE void VariableLog2ParamComplexity::useCache(const bool on_off) { _use_cache_ = on_off; }


  /// clears the current cache
  INLINE void VariableLog2ParamComplexity::clearCache() { _cache_.clear(); }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
