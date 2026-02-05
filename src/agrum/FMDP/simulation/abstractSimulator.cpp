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


/**
 * @file
 * @brief Sources of
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(_at_AMU)
 *
 */

//======================================================================
#include <agrum/FMDP/simulation/abstractSimulator.h>

//======================================================================

namespace gum {

  // ===========================================================================
  // Constructors, Destructors.
  // ===========================================================================

  /*
   * Default constructor.
   */
  AbstractSimulator::AbstractSimulator() {
    GUM_CONSTRUCTOR(AbstractSimulator);
    ;
  }

  /*
   * Default destructor.
   */
  AbstractSimulator::~AbstractSimulator() {
    GUM_DESTRUCTOR(AbstractSimulator);
    ;
  }

  // ===========================================================================
  //
  // ===========================================================================

  void AbstractSimulator::setInitialStateRandomly() {
    bool hre = true;
    while (hre) {
      currentState_ = randomState_();
      hre           = hasReachEnd();
    }
  }

  Instantiation AbstractSimulator::randomState_() {
    Instantiation retState;
    for (auto varIter = this->beginVariables(); varIter != this->endVariables(); ++varIter) {
      retState.add(**varIter);
      retState.chgVal(**varIter, randomValue((*varIter)->domainSize()));
    }
    return retState;
  }

  ///
  bool AbstractSimulator::hasReachEnd() {
    if (endState_.empty()) return false;

    for (auto varIter = endState_.variablesSequence().beginSafe();
         varIter != endState_.variablesSequence().endSafe();
         ++varIter)
      if (endState_.val(**varIter) != currentState_.val(**varIter)) return false;
    return true;
  }


}   // End of namespace gum
