/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
      AbstractSimulator(),  _fmdp_(const_cast< FMDP< double >* >(fmdp)),
       _loaded_(false) {
    GUM_CONSTRUCTOR(FMDPSimulator);
  }

  FMDPSimulator::FMDPSimulator(const std::string& ressource) :
      AbstractSimulator(),  _loaded_(true) {
    GUM_CONSTRUCTOR(FMDPSimulator);

     _fmdp_ = new FMDP< double >(true);
    FMDPDatReader< double > reader( _fmdp_, ressource);
    reader.trace(false);
    reader.proceed();
  }

  /*
   * Default destructor.
   */
  FMDPSimulator::~FMDPSimulator() {
    GUM_DESTRUCTOR(FMDPSimulator);
    if ( _loaded_) delete  _fmdp_;
  }


  // ===========================================================================
  //
  // ===========================================================================

  void FMDPSimulator::perform(Idx actionId) {
    Instantiation newState;
    for (auto varIter = this->beginVariables(); varIter != this->endVariables();
         ++varIter) {
      newState.add(**varIter);
      Instantiation transit(currentState_);
      transit.add(*(this->primeVar(*varIter)));

      double proba = (double)std::rand() / (double)RAND_MAX;
      double cdd   = 0.0;
      for (transit.setFirstOut(currentState_); !transit.end();
           transit.incOut(currentState_)) {
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
