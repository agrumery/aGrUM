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


/**
 * @file
 * @brief Implementation of gumRangeVariable.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#include <agrum/base/variables/rangeVariable.h>

#ifdef GUM_NO_INLINE
#  include <agrum/base/variables/rangeVariable_inl.h>
#endif /* GUM_NO_INLINE */

namespace gum {

  // ==========================================================================
  // constructors
  // ==========================================================================
  RangeVariable::RangeVariable(const std::string& aName,
                               const std::string& aDesc,
                               long               minVal,
                               long               maxVal) :
      DiscreteVariable(aName, aDesc), _minBound_(minVal), _maxBound_(maxVal) {
    GUM_CONSTRUCTOR(RangeVariable);
  }

  RangeVariable::RangeVariable(const std::string& aName, const std::string& aDesc) :
      DiscreteVariable(aName, aDesc), _minBound_(0), _maxBound_(1) {
    GUM_CONSTRUCTOR(RangeVariable);
  }

  // ==========================================================================
  // Copy Constructor.
  // If aDRV haves any listener, it will not be copied.
  // @param aDRV the variable we copy
  // ==========================================================================
  RangeVariable::RangeVariable(const RangeVariable& aDRV) :
      DiscreteVariable(aDRV), _minBound_(aDRV._minBound_), _maxBound_(aDRV._maxBound_) {
    GUM_CONS_CPY(RangeVariable);
  }

  // ==========================================================================
  // destructor
  // ==========================================================================
  RangeVariable::~RangeVariable() {
    GUM_DESTRUCTOR(RangeVariable);
    ;
  }

  std::string RangeVariable::domain() const {
    std::stringstream s;
    s << "[" << minVal() << "," << maxVal() << "]";
    return s.str();
  }

  std::string RangeVariable::toFast() const {
    std::stringstream s;
    s << name();
    if (const auto m = minVal(); m != 0) {
      s << "[" << m << "," << maxVal() << "]";
    } else {
      s << "[" << maxVal() + 1 << "]";
    }
    return s.str();
  }
} /* namespace gum */
