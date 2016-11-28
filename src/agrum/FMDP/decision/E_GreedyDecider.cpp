/***************************************************************************
 *  Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN   *
 *  {prenom.nom}_at_lip6.fr                                                *
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the           *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program; if not, write to the                          *
 *  Free Software Foundation, Inc.,                                        *
 *  59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.               *
 ***************************************************************************/
/**
 * @file
 * @brief Sources of the class.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */

// =========================================================================
#include <cstdlib>
#include <random>
// =========================================================================
#include <agrum/FMDP/decision/E_GreedyDecider.h>
// =========================================================================

namespace gum {

  // ==========================================================================
  // Constructor & destructor.
  // ==========================================================================

  // ###################################################################
  /**
   * Constructor
   */
  // ###################################################################
  E_GreedyDecider::E_GreedyDecider() {

    GUM_CONSTRUCTOR( E_GreedyDecider )

    __sss = 1.0;
  }


  // ###################################################################
  /**
   *
   */
  // ###################################################################
  E_GreedyDecider::~E_GreedyDecider() { GUM_DESTRUCTOR( E_GreedyDecider ) }


  // ==========================================================================
  // Initialization
  // ==========================================================================

  // ###################################################################
  /**
   *
   */
  // ###################################################################
  void E_GreedyDecider::initialize( const FMDP<double>* fmdp ) {
    IDecisionStrategy::initialize( fmdp );
    for ( auto varIter = fmdp->beginVariables(); varIter != fmdp->endVariables();
          ++varIter )
      __sss *= (double)( *varIter )->domainSize();
  }


  // ==========================================================================
  // Incremental methods
  // ==========================================================================

  // ###################################################################
  /*
   * Performs a feedback on the last transition.
   * In extenso, learn from the transition.
   * @param reachedState : the state reached after the transition
   */
  // ###################################################################
  void E_GreedyDecider::checkState( const Instantiation& reachedState,
                                    Idx                  actionId ) {
    if ( __statecpt.nbVisitedStates() == 0 )
      __statecpt.reset( reachedState );
    else if ( !__statecpt.checkState( reachedState ) )
      __statecpt.addState( reachedState );
  }

  // ###################################################################
  /*
   * @param the state in which we currently are
   * @return a set containing every optimal actions on that state
   */
  // ###################################################################
  ActionSet E_GreedyDecider::stateOptimalPolicy( const Instantiation& curState ) {

    double explo = (double)std::rand() / (double)RAND_MAX;
    double temp =
        std::pow( ( __sss - (double)__statecpt.nbVisitedStates() ) / __sss, 3.0 );
    double exploThreshold = temp < 0.1 ? 0.1 : temp;

    //      std::cout << exploThreshold << std::endl;

    ActionSet optimalSet = IDecisionStrategy::stateOptimalPolicy( curState );
    if ( explo > exploThreshold ) {
      //        std::cout << "Exploit : " << optimalSet << std::endl;
      return optimalSet;
    }

    if ( _allActions.size() > optimalSet.size() ) {
      ActionSet ret( _allActions );
      ret -= optimalSet;
      //        std::cout << "Explore : " << ret << std::endl;
      return ret;
    }

    //      std::cout << "Explore : " << _allActions << std::endl;
    return _allActions;
  }

}  // End of namespace gum
