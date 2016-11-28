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
 * @brief Headers of the Decision Strategy interface.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */


// =========================================================================
#ifndef GUM_SDYNA_DECISION_STRATEGY_H
#define GUM_SDYNA_DECISION_STRATEGY_H
// =========================================================================
#include <agrum/core/types.h>
// =========================================================================
#include <agrum/FMDP/planning/actionSet.h>
// =========================================================================
#include <agrum/variables/discreteVariable.h>
// =========================================================================

namespace gum {

  /**
   * @class IDecisionStrategy IDecisionStrategy.h
   * <agrum/FMDP/SDyna/IDecisionStrategy.h>
   * @brief Interface for manipulating decision center
   * @ingroup fmdp_group
   *
   */
  class IDecisionStrategy {

    // ###################################################################
    /// @name Constructor & destructor.
    // ###################################################################
    /// @{
    public:
    // ==========================================================================
    /// Destructor (virtual and empty since it's an interface)
    // ==========================================================================
    virtual ~IDecisionStrategy() {}

    /// @}

    // ###################################################################
    /// @name Initialization
    // ###################################################################
    /// @{
    public:
    // ==========================================================================
    /// Initializes the learner
    // ==========================================================================
    virtual void initialize( const FMDP<double>* fmdp ) {
      _optPol = nullptr;
      if ( _allActions.size() == 0 )
        for ( auto actionIter = fmdp->beginActions();
              actionIter != fmdp->endActions();
              ++actionIter )
          _allActions += *actionIter;
    }
    /// @}


    // ###################################################################
    /// @name Incremental methods
    // ###################################################################
    /// @{
    public:
    virtual void checkState( const Instantiation& newState, Idx actionId ) = 0;

    void setOptimalStrategy(
        const MultiDimFunctionGraph<ActionSet, SetTerminalNodePolicy>* optPol ) {
      _optPol =
          const_cast<MultiDimFunctionGraph<ActionSet, SetTerminalNodePolicy>*>(
              optPol );
    }

    virtual ActionSet stateOptimalPolicy( const Instantiation& curState ) {
      return ( _optPol && _optPol->realSize() != 0 ) ? _optPol->get( curState )
                                                     : _allActions;
    }

    protected:
    ///
    const MultiDimFunctionGraph<ActionSet, SetTerminalNodePolicy>* _optPol;

    ///
    ActionSet _allActions;

    /// @}
  };
}
#endif  // GUM_SDYNA_DECISION_STRATEGY_H
