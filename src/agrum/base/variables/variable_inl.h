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


#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  /// protected copy

  INLINE void Variable::copy_(const Variable& aRV) {
    _name_        = aRV._name_;
    _description_ = aRV._description_;
  }

  /// constructor

  INLINE Variable::Variable(const std::string& aName, const std::string& aDesc) :
      _name_(aName), _description_(aDesc) {   // for debugging purposes
    GUM_CONSTRUCTOR(Variable);
  }

  /// copy constructor

  INLINE Variable::Variable(const Variable& aRV) :
      _name_(aRV._name_), _description_(aRV._description_) {   // for debugging purposes
    GUM_CONS_CPY(Variable);
  }

  /// destructor

  INLINE Variable::~Variable() {   // for debugging purposes
    GUM_DESTRUCTOR(Variable);
  }

  /// Copy operator

  INLINE Variable& Variable::operator=(const Variable& aRV) {
    // avoid self assignment
    if (this != &aRV) copy_(aRV);

    return *this;
  }

  /// sets the  _name_ of the variable

  INLINE void Variable::setName(const std::string& theValue) { _name_ = theValue; }

  /// returns the  _name_ of the variable

  INLINE const std::string& Variable::name() const { return _name_; }

  /// sets the  _description_ of the variable

  INLINE void Variable::setDescription(const std::string& theValue) const {
    _description_ = theValue;
  }

  /// returns the  _description_ of the variable

  INLINE const std::string& Variable::description() const { return _description_; }

  /// equality operator

  INLINE bool Variable::operator==(const Variable& aRV) const {
    if (varType() != aRV.varType()) return false;
    if (name() != aRV.name()) return false;
    return _checkSameDomain_(aRV);
  }

} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS
