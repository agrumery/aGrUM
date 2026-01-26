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


/** @file
 * @brief the internal prior for the K2 score = Laplace Prior
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/BN/learning/priors/K2Prior.h>

namespace gum {

  namespace learning {


    /// default constructors
    INLINE K2Prior::K2Prior(const DatabaseTable&                    database,
                            const Bijection< NodeId, std::size_t >& nodeId2columns) :
        SmoothingPrior(database, nodeId2columns) {
      GUM_CONSTRUCTOR(K2Prior);
    }

    /// copy constructor
    INLINE K2Prior::K2Prior(const K2Prior& from) : SmoothingPrior(from) { GUM_CONS_CPY(K2Prior); }

    /// move constructor
    INLINE K2Prior::K2Prior(K2Prior&& from) : SmoothingPrior(std::move(from)) {
      GUM_CONS_MOV(K2Prior);
    }

    /// virtual copy constructor
    INLINE K2Prior* K2Prior::clone() const { return new K2Prior(*this); }

    /// destructor
    INLINE K2Prior::~K2Prior() { GUM_DESTRUCTOR(K2Prior); }

    /// copy operator
    INLINE K2Prior& K2Prior::operator=(const K2Prior& from) {
      SmoothingPrior::operator=(from);
      return *this;
    }

    /// move operator
    INLINE K2Prior& K2Prior::operator=(K2Prior&& from) {
      SmoothingPrior::operator=(std::move(from));
      return *this;
    }

    /// dummy set weight function: in K2, weights are always equal to 1
    INLINE void K2Prior::setWeight(const double weight) {}


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
