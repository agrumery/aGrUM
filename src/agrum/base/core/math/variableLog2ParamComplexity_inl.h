/****************************************************************************
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
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/
#pragma once


/** @file
 * @brief the class for computing the log2 of the parametric complexity of
 * an r-ary multinomial variable
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <iostream>
#  include <sstream>

#  include <agrum/base/core/math/gammaLog2.h>
#  include <agrum/base/core/math/variableLog2ParamComplexity.h>

namespace gum {

  /// default constructor
  INLINE VariableLog2ParamComplexity::VariableLog2ParamComplexity() : _cache_(1000) {
    GUM_CONSTRUCTOR(VariableLog2ParamComplexity);
  }

  /// copy constructor
  INLINE VariableLog2ParamComplexity::VariableLog2ParamComplexity(
      const VariableLog2ParamComplexity& from) :
      _use_cache_(from._use_cache_), _cache_(from._cache_) {
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
