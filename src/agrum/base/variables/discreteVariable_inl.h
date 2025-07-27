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


#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <algorithm>

// to ease parsers in IDE
#  include <agrum/base/variables/discreteVariable.h>

namespace gum {
  /* =========================================================================*/
  /* =========================================================================*/
  /* ===                         GUM_DISCRETE_VARIABLE === */
  /* =========================================================================*/
  /* =========================================================================*/

  // Default constructor
  INLINE DiscreteVariable::DiscreteVariable(const std::string& aName, const std::string& aDesc) :
      Variable(aName, aDesc) {
    GUM_CONSTRUCTOR(DiscreteVariable)
  }

  // Copy constructor
  INLINE DiscreteVariable::DiscreteVariable(const DiscreteVariable& aDRV) : Variable(aDRV) {
    GUM_CONSTRUCTOR(DiscreteVariable)
  }

  // destructor
  INLINE DiscreteVariable::~DiscreteVariable() { GUM_DESTRUCTOR(DiscreteVariable) }

  // Copy operator
  INLINE DiscreteVariable& DiscreteVariable::operator=(const DiscreteVariable& aRV) {
    if (&aRV != this) { Variable::operator=(aRV); }

    return *this;
  }

  // empty <==> domainSize()<2
  INLINE bool DiscreteVariable::empty() const { return domainSize() < 2; }

  INLINE std::string DiscreteVariable::closestLabel(double val) const {
    return label(closestIndex(val));
  }

  // vector of labels
  INLINE std::vector< std::string > DiscreteVariable::labels() const {
    std::vector< std::string > v;
    for (Idx i = 0; i < domainSize(); i++)
      v.push_back(label(i));
    return v;
  }
} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS
