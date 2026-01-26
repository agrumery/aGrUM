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
// =====================================================================
#include <cstdlib>
//======================================================================
#include <agrum/base/core/utils_random.h>
//======================================================================
#include <agrum/FMDP/io/dat/fmdpDatReader.h>
#include <agrum/FMDP/simulation/fmdpSimulator.h>

//======================================================================

namespace gum {

  // ===========================================================================
  // Constructors, Destructors.
  // ===========================================================================

  /*
   * Default constructor.
   */
  FMDPSimulator::FMDPSimulator(const FMDP< double >* fmdp) :
      AbstractSimulator(), _fmdp_(const_cast< FMDP< double >* >(fmdp)), _loaded_(false) {
    GUM_CONSTRUCTOR(FMDPSimulator);
  }

  FMDPSimulator::FMDPSimulator(const std::string& ressource) : AbstractSimulator(), _loaded_(true) {
    GUM_CONSTRUCTOR(FMDPSimulator);

    _fmdp_ = new FMDP< double >(true);
    FMDPDatReader< double > reader(_fmdp_, ressource);
    reader.trace(false);
    reader.proceed();
  }

  /*
   * Default destructor.
   */
  FMDPSimulator::~FMDPSimulator() {
    GUM_DESTRUCTOR(FMDPSimulator);
    if (_loaded_) delete _fmdp_;
  }

  // ===========================================================================
  //
  // ===========================================================================

  void FMDPSimulator::perform(Idx actionId) {
    Instantiation newState;
    for (auto varIter = this->beginVariables(); varIter != this->endVariables(); ++varIter) {
      newState.add(**varIter);
      Instantiation transit(currentState_);
      transit.add(*(this->primeVar(*varIter)));

      double proba = randomProba();
      double cdd   = 0.0;
      for (transit.setFirstOut(currentState_); !transit.end(); transit.incOut(currentState_)) {
        cdd += this->transitionProbability_(*varIter, transit, actionId);
        if (proba <= cdd) {
          newState.chgVal(**varIter, transit.val(*(this->primeVar(*varIter))));
          break;
        }
      }
    }
    currentState_ = newState;
  }


}   // End of namespace gum
