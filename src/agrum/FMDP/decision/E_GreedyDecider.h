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
 * @brief Headers of the epsilon-greedy decision maker class.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */


// =========================================================================
#ifndef GUM_E_GREEDY_DECIDER_H
#define GUM_E_GREEDY_DECIDER_H
// =========================================================================
#include <agrum/core/types.h>
// =========================================================================
#include <agrum/FMDP/SDyna/Strategies/IDecisionStrategy.h>
#include <agrum/FMDP/SDyna/Strategies/IPlanningStrategy.h>
#include <agrum/FMDP/fmdp.h>
#include <agrum/FMDP/planning/actionSet.h>
#include <agrum/FMDP/simulation/statesChecker.h>
// =========================================================================
#include <agrum/variables/discreteVariable.h>
// =========================================================================

namespace gum {

  /**
   * @class E_GreedyDecider E_GreedyDecider.h
   * <agrum/FMDP/decision/E_GreedyDecider.h>
   * @brief Class to make decision following an epsilon-greedy compromise
   * between exploration and exploitation
   *
   *
   */
  class E_GreedyDecider : public IDecisionStrategy {

    // ###################################################################
    /// @name Constructor & destructor.
    // ###################################################################
    /// @{
    public:
    // ==========================================================================
    /// Constructor
    // ==========================================================================
    E_GreedyDecider();

    // ==========================================================================
    /// Destructor
    // ==========================================================================
    ~E_GreedyDecider();

    /// @}

    // ###################################################################
    /// @name Initialization
    // ###################################################################
    /// @{
    public:
    // ==========================================================================
    /// Initializes the learner
    // ==========================================================================
    void initialize( const FMDP<double>* fmdp );

    /// @}


    // ###################################################################
    /// @name Incremental methods
    // ###################################################################
    /// @{
    public:
    void checkState( const Instantiation& newState, Idx actionId );

    ActionSet stateOptimalPolicy( const Instantiation& curState );

    /// @}

    private:
    StatesChecker __statecpt;
    double        __sss;
  };
}
#endif  // GUM_E_GREEDY_DECIDER_H
