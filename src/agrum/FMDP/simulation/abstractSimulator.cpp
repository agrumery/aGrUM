/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/**
 * @file
 * @brief Sources of
 *
 * @author Jean-Christophe Magnan
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
  AbstractSimulator::AbstractSimulator() { GUM_CONSTRUCTOR( AbstractSimulator ) }

  /*
   * Default destructor.
   */
  AbstractSimulator::~AbstractSimulator() { GUM_DESTRUCTOR( AbstractSimulator ) }


  // ===========================================================================
  //
  // ===========================================================================

  void AbstractSimulator::setInitialStateRandomly() {
    bool hre = true;
    while ( hre ) {
      _currentState = _randomState();
      hre = hasReachEnd();
    }
  }

  Instantiation AbstractSimulator::_randomState() {
    Instantiation retState;
    for ( auto varIter = this->beginVariables(); varIter != this->endVariables();
          ++varIter ) {
      retState.add( **varIter );
      retState.chgVal( *varIter,
                       ( Idx )( ( (double)std::rand() / (double)RAND_MAX ) *
                                (double)( *varIter )->domainSize() ) );
    }
    return retState;
  }

  ///
  bool AbstractSimulator::hasReachEnd() {

    if ( _endState.empty() ) return false;

    for ( auto varIter = _endState.variablesSequence().beginSafe();
          varIter != _endState.variablesSequence().endSafe();
          ++varIter )
      if ( _endState.val( **varIter ) != _currentState.val( **varIter ) )
        return false;
    return true;
  }


}  // End of namespace gum
