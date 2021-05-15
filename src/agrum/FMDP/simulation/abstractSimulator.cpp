/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief Sources of
 *
 * @author Pierre-Henri WUILLEMIN(@LIP6) and Jean-Christophe MAGNAN and Christophe
 * GONZALES(@AMU)
 *
 */
// =====================================================================
#include <cstdlib>
#include <random>
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
    for (auto varIter = this->beginVariables(); varIter != this->endVariables();
         ++varIter) {
      retState.add(**varIter);
      retState.chgVal(*varIter,
                      (Idx)(((double)std::rand() / (double)RAND_MAX)
                            * (double)(*varIter)->domainSize()));
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
