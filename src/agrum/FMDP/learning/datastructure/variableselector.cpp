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
 * @brief Template implémentations of the NodeDatabase class.
 *
 * @author Jean-Christophe MAGNAN
 */
// =========================================================================
#include <agrum/FMDP/learning/datastructure/variableselector.h>
// =========================================================================

namespace gum {

  // ==========================================================================
  // Constructor & destructor.
  // ==========================================================================

  // ###################################################################
  // Default constructor
  // ###################################################################
  VariableSelector::VariableSelector( Set<const DiscreteVariable*>& startingSet )
      : __remainingVars( startingSet ) {
    GUM_CONSTRUCTOR( VariableSelector );
    __remainingScores.insert( 0.0, 0.0 );
    __remainingVarsByScore.insert(
        0.0, new Set<const DiscreteVariable*>( __remainingVars ) );

    for ( auto varIter = __remainingVars.cbeginSafe();
          varIter != __remainingVars.cendSafe();
          ++varIter ) {
      __remainingVarsScore.insert( *varIter, 0.0 );
      __remainingVarsOtherScore.insert( *varIter, 0.0 );
    }
  }

  // ###################################################################
  // Default constructor
  // ###################################################################
  VariableSelector::~VariableSelector() { GUM_DESTRUCTOR( VariableSelector ); }


  // ###################################################################
  //
  // ###################################################################
  void VariableSelector::updateScore( const DiscreteVariable* var,
                                      double                  score,
                                      double                  secondaryscore ) {
    __removeVar( var );
    __remainingVarsScore[var] += score;
    __addVar( var );
    __remainingVarsOtherScore[var] += secondaryscore;
  }


  // ###################################################################
  //
  // ###################################################################
  void VariableSelector::downdateScore( const DiscreteVariable* var,
                                        double                  score,
                                        double                  secondaryscore ) {
    __removeVar( var );
    __remainingVarsScore[var] -= score;
    __addVar( var );
    __remainingVarsOtherScore[var] -= secondaryscore;
  }


  // ###################################################################
  // Select the most relevant variable
  // ###################################################################
  const DiscreteVariable* VariableSelector::select() {

    double                        bestScore = __remainingScores.top();
    Set<const DiscreteVariable*>* bestSet = __remainingVarsByScore[bestScore];
    const DiscreteVariable*       bestVar = nullptr;

    for ( auto varIter = bestSet->beginSafe(); varIter != bestSet->endSafe();
          ++varIter ) {
      if ( bestVar == nullptr ||
           __remainingVarsOtherScore[bestVar] <
               __remainingVarsOtherScore[*varIter] ||
           ( __remainingVarsOtherScore[bestVar] ==
                 __remainingVarsOtherScore[*varIter] &&
             bestVar->domainSize() < ( *varIter )->domainSize() ) )
        bestVar = *varIter;
    }
    __removeVar( bestVar );
    __remainingVars >> bestVar;
    return bestVar;
  }


  // ###################################################################
  // Select the most relevant variable
  // ###################################################################
  void VariableSelector::__addVar( const DiscreteVariable* var ) {
    double varScore = __remainingVarsScore[var];

    if ( !__remainingVarsByScore.exists( varScore ) ) {
      __remainingVarsByScore.insert( varScore,
                                     new Set<const DiscreteVariable*>() );
      __remainingScores.insert( varScore, varScore );
    }
    __remainingVarsByScore[varScore]->insert( var );
  }


  // ###################################################################
  // Select the most relevant variable
  // ###################################################################
  void VariableSelector::__removeVar( const DiscreteVariable* var ) {
    double                        varScore = __remainingVarsScore[var];
    Set<const DiscreteVariable*>* varSet = __remainingVarsByScore[varScore];
    *varSet >> var;
    if ( varSet->empty() ) {
      __remainingScores.erase( varScore );
      __remainingVarsByScore.erase( varScore );
      delete varSet;
    }
  }
}
