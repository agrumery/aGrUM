/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
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

#include <agrum/FMDP/learning/core/testPolicy/ITestPolicy.h>

namespace gum {


  template < typename GUM_ELEMENT >
  ITestPolicy< GUM_ELEMENT >::ITestPolicy() : _isModified_(false), _nbObs_(0) {
    GUM_CONSTRUCTOR(ITestPolicy);
  }

  template < typename GUM_ELEMENT >
  ITestPolicy< GUM_ELEMENT >::~ITestPolicy() {
    GUM_DESTRUCTOR(ITestPolicy);
    ;
  }

  template < typename GUM_ELEMENT >
  void* ITestPolicy< GUM_ELEMENT >::operator new(size_t s) {
    return SmallObjectAllocator::instance().allocate(s);
  }

  template < typename GUM_ELEMENT >
  void ITestPolicy< GUM_ELEMENT >::operator delete(void* p) {
    SmallObjectAllocator::instance().deallocate(p, sizeof(ITestPolicy));
  }

  template < typename GUM_ELEMENT >
  void ITestPolicy< GUM_ELEMENT >::addObservation(Idx attr, GUM_ELEMENT value) {
    _isModified_ = true;
    _nbObs_++;
  }

  template < typename GUM_ELEMENT >
  Idx ITestPolicy< GUM_ELEMENT >::nbObservation() const {
    return _nbObs_;
  }

  template < typename GUM_ELEMENT >
  void ITestPolicy< GUM_ELEMENT >::computeScore() const {
    _isModified_ = false;
  }

  template < typename GUM_ELEMENT >
  void ITestPolicy< GUM_ELEMENT >::add(const ITestPolicy< GUM_ELEMENT >& src) {
    _isModified_ = true;
    _nbObs_ += src.nbObservation();
  }

  template < typename GUM_ELEMENT >
  std::string ITestPolicy< GUM_ELEMENT >::toString() const {
    return std::format("\t\t\tNb Obs : {}\n", _nbObs_);
  }

  template < typename GUM_ELEMENT >
  bool ITestPolicy< GUM_ELEMENT >::isModified_() const {
    return _isModified_;
  }

}   // namespace gum
