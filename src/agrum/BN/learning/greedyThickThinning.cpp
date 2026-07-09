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


/** @file
 * @brief The greedy thick-thinning learning algorithm (for directed graphs)
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Christophe GONZALES(_at_AMU)
 */

#include <agrum/BN/learning/greedyThickThinning.h>

namespace gum {

  namespace learning {

    /// default constructor
    GreedyThickThinning::GreedyThickThinning() {
      setEpsilon(0);
      disableMinEpsilonRate();
      disableMaxIter();
      disableMaxTime();
      GUM_CONSTRUCTOR(GreedyThickThinning);
    }

    /// copy constructor
    GreedyThickThinning::GreedyThickThinning(const GreedyThickThinning& from) :
        ApproximationScheme(from), _allowReversalsInThinPhase_(from._allowReversalsInThinPhase_) {
      GUM_CONS_CPY(GreedyThickThinning);
    }

    /// move constructor
    GreedyThickThinning::GreedyThickThinning(GreedyThickThinning&& from) :
        ApproximationScheme(std::move(from)),
        _allowReversalsInThinPhase_(from._allowReversalsInThinPhase_) {
      GUM_CONS_MOV(GreedyThickThinning);
    }

    /// destructor
    GreedyThickThinning::~GreedyThickThinning() { GUM_DESTRUCTOR(GreedyThickThinning); }

    /// copy operator
    GreedyThickThinning& GreedyThickThinning::operator=(const GreedyThickThinning& from) = default;

    /// move operator
    GreedyThickThinning& GreedyThickThinning::operator=(GreedyThickThinning&& from) {
      ApproximationScheme::operator=(std::move(from));
      _allowReversalsInThinPhase_ = from._allowReversalsInThinPhase_;
      return *this;
    }

    /// returns the approximation policy of the learning algorithm
    ApproximationScheme& GreedyThickThinning::approximationScheme() { return *this; }

    /// enable or disable arc reversals during the thin phase
    void GreedyThickThinning::setAllowReversalsInThinPhase(bool allow) {
      _allowReversalsInThinPhase_ = allow;
    }

    /// returns whether arc reversals are allowed in the thin phase
    bool GreedyThickThinning::allowReversalsInThinPhase() const {
      return _allowReversalsInThinPhase_;
    }

  } /* namespace learning */

} /* namespace gum */
