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
   * @class IDecisionStrategy IDecisionStrategy.h <agrum/FMDP/SDyna/IDecisionStrategy.h>
   * @brief Interface for manipulating decision center
   * @ingroup fmdp_group
   *
   */
  class IDecisionStrategy {

      // ###################################################################
      /// @name Constructor & destructor.
      // ###################################################################
      /// @{
    public :

        // ==========================================================================
        /// Destructor (virtual and empty since it's an interface)
        // ==========================================================================
        virtual ~IDecisionStrategy(){}

      /// @}

      // ###################################################################
      /// @name Initialization
      // ###################################################################
      /// @{
    public:

        // ==========================================================================
        /// Initializes the learner
        // ==========================================================================
        virtual void initialize( const FMDP<double>* fmdp ) = 0;

      /// @}


      // ###################################################################
      /// @name Incremental methods
      // ###################################################################
      /// @{
    public:
        virtual void checkState( const Instantiation& newState ) = 0;

        virtual ActionSet getStateOptimalPolicy( const Instantiation& curState ) = 0;

      /// @}
  };

}
#endif // GUM_SDYNA_DECISION_STRATEGY_H
