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
 * @brief the base class for all a prioris
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum::learning {

  /// default constructor
  INLINE Prior::Prior(const DatabaseTable&                    database,
                      const Bijection< NodeId, std::size_t >& nodeId2columns) :
      database_(&database), nodeId2columns_(nodeId2columns) {
    GUM_CONSTRUCTOR(Prior);
  }

  /// copy constructor
  INLINE Prior::Prior(const Prior& from) :
      weight_(from.weight_), database_(from.database_), nodeId2columns_(from.nodeId2columns_) {
    GUM_CONS_CPY(Prior);
  }

  /// move constructor
  INLINE Prior::Prior(Prior&& from) :
      weight_(from.weight_), database_(from.database_),
      nodeId2columns_(std::move(from.nodeId2columns_)) {
    GUM_CONS_MOV(Prior);
  }

  /// destructor
  INLINE Prior::~Prior() { GUM_DESTRUCTOR(Prior); }

  /// sets the weight of the a prior(kind of effective sample size)
  INLINE void Prior::setWeight(const double weight) {
    if (weight < 0.0) {
      GUM_ERROR(OutOfBounds, "A negative weight (" << weight << ") is forbidden for an prior");
    }
    weight_ = weight;
  }

  /// returns the weight assigned to the prior
  INLINE double Prior::weight() const { return weight_; }


}   // namespace gum::learning

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
