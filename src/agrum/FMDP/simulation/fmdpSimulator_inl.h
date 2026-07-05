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

#include <agrum/FMDP/simulation/fmdpSimulator.h>

namespace gum {


  INLINE double FMDPSimulator::reward() { return _fmdp_->reward()->get(this->currentState_); }

  INLINE const DiscreteVariable* FMDPSimulator::primeVar(const DiscreteVariable* mainVar) {
    return _fmdp_->main2prime(mainVar);
  }

  INLINE SequenceIteratorSafe< const DiscreteVariable* > FMDPSimulator::beginVariables() {
    return _fmdp_->beginVariables();
  }

  INLINE SequenceIteratorSafe< const DiscreteVariable* > FMDPSimulator::endVariables() {
    return _fmdp_->endVariables();
  }

  INLINE const std::string& FMDPSimulator::actionName(Idx actionId) {
    return _fmdp_->actionName(actionId);
  }

  INLINE SequenceIteratorSafe< Idx > FMDPSimulator::beginActions() {
    return _fmdp_->beginActions();
  }

  INLINE SequenceIteratorSafe< Idx > FMDPSimulator::endActions() { return _fmdp_->endActions(); }

  INLINE double FMDPSimulator::transitionProbability_(const DiscreteVariable* var,
                                                      const Instantiation&    transit,
                                                      Idx                     actionId) {
    return reinterpret_cast< const MultiDimFunctionGraph< double >* >(
               _fmdp_->transition(actionId, var))
        ->get(transit);
  }

}   // namespace gum
